//
// Created by s17kf on 06.12.18.
//

#include <thread>
#include "RpiCommunication.h"
#include "Packet.h"
#include "RpiClient.h"
#include "common/Logger.h"
#include "common/Atomic.h"
#include "RpiMsg.h"

//struct sockaddr_in RpiCommunication::address;
//int RpiCommunication::addressLen = 0;
//int RpiCommunication::socketFd = 0;

RpiCommunication::RpiCommunication(int port) {
    opt = 1;
    Atomic::set(&stopped, false);
    if ((socketFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        //todo exception service
        throw file_descriptor_error("error creating socket");
    }
    if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        throw std::runtime_error("setsockopt error");
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    addressLen = sizeof(address);
    if (bind(socketFd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        throw std::runtime_error("bind error");
    }
    if (listen(socketFd, 2) < 0) {
        throw std::runtime_error("listen error");
    }
}

int RpiCommunication::getConnection(uint *ip) {
    int client_socket;
    if((client_socket = accept(socketFd, (struct sockaddr *) &address, (socklen_t *) &addressLen)) < 0) {
        throw std::runtime_error("getConnection: accept error");
    }
    *ip = address.sin_addr.s_addr;
    return client_socket;
}

int RpiCommunication::getSocket_fd() const {
    return socketFd;
}

void RpiCommunication::run() {
    log(0, "rpi communication id is %d", std::this_thread::get_id());
    fd_set readfds;
    timeval timeout;
    while(true) {
        if(Atomic::get(&stopped))
            break;
        FD_ZERO(&readfds);
        FD_SET(socketFd, &readfds);
        timeout.tv_sec = Const::answer_timeout;
        timeout.tv_usec = 0;
        int retval = select( socketFd + 1, &readfds, nullptr, nullptr, &timeout);
        if (retval == -1)
            perror("select()");
        else if (retval) {
            log(1, "new connection from raspberry");
            if (FD_ISSET(socketFd, &readfds)) {
                log(2, "ask for new connection available");
                uint new_client_ip;
                int new_client_fd = getConnection(&new_client_ip);
                log(2, "new connection from %s",
                        Const::ipToString(new_client_ip).c_str());
                // TODO: add client to readfds and write fds
                std::thread *t = new std::thread(RpiCommunication::clientConnection,
                        new_client_fd);
                activeClientConnections.insert(t);
                log(0, "communication with client goes in other thread");
                address.sin_addr.s_addr = INADDR_ANY;
            }
            else{
                log(0, "some undefined read possible!!!");
            }

        } else {
            log(0, "no tries to connect within timeout");
        }
    }

}

uint RpiCommunication::getIpFromSocket(int client_fd) {
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int res = getpeername(client_fd, (struct sockaddr *)&addr, &addr_size);
    return addr.sin_addr.s_addr;
}

short RpiCommunication::addNewRpiClient(int client_fd) {
    short new_id = RpiClient::getNextFreeId();
    if(new_id < 0){
        return new_id;
    }
    uint new_ip = getIpFromSocket(client_fd);
    new RpiClient(new_ip, new_id);
    return new_id;
}

void RpiCommunication::stop() {
    Atomic::set(&stopped, true);
    log(5, "stopped set to true");
}

void RpiCommunication::clientConnection(int clientSocket) {
    while(true){
        log(5, "client connection");
        RpiMsg *msg = RpiMsg::receiveMsg(clientSocket);
        if(msg == nullptr){
            log(0, "received msg is nullptr, closing client connectioon");
            close(clientSocket);
            break;
        }
        if(auto dev = dynamic_cast<RDev *>(msg)){
            log(1, "dev received");
            auto rpi_id = dev->getRpiId();
            auto rpiClient = RpiClient::getRpiClient(rpi_id);
            if(rpiClient == nullptr) {
                log(0, "There is no client id %d", rpi_id);
                sendNack(clientSocket, rpi_id);
                // TODO: send end ir something
                continue;
            }
            auto newDevice = rpiClient->addDevice(dev->getCodedDevice(),
                    dev->getCodedDevLen());
            if(newDevice == nullptr){
                log(2, "Failed to add nev device from %d", rpi_id);
                RNack *nack = new RNack(dev->getDevId());
                auto sent = nack->send(clientSocket);
                log(2, "Sent %d bytes: %s", sent, nack->toString().c_str());
                delete nack;
                delete msg;
                continue;
            }
            else{
                log(2, "added new device on %d: %s", rpi_id,
                        newDevice->toString().c_str());
                RAck *ack = new RAck(dev->getDevId());
                auto sent = ack->send(clientSocket);
                log(2, "sent %d bytes: %s", sent, ack->toString().c_str());
                delete ack;
                delete msg;
                continue;
            }
        }else if(auto val = dynamic_cast<RValDouble *>(msg)){
            log(2, "val from %d, %d=%f recveived", val->getRpiId(), val->getDevId(),
            val->getVal());
            sendAck(clientSocket, val->getDevId());
            delete val;
            close(clientSocket);
            break;
        }else if(auto end = dynamic_cast<REnd *>(msg)){
            log(1, "end received, closing socket");
//            ushort client_id = end->getVal();
            auto rpiClient = RpiClient::getRpiClient(end->getVal());
            if(rpiClient == nullptr){
                log(1, "REnd: bad client id %d", end->getVal());
            } else {
                rpiClient->printAllDevices();
            }
            delete msg;
            close(clientSocket);
            break;
        }else if(auto reg = dynamic_cast<RReg *>(msg)){
            log(1, "reg received");
            auto rpi_id = addNewRpiClient(clientSocket);
            auto rpi_ip = getIpFromSocket(clientSocket);
            if(rpi_id < 0){
                log(0, "new rpi id is %d - max rpis reached", rpi_id);
                RNack *nack = new RNack(ushort(0));
                auto sent = nack->send(clientSocket);
                log(2, "sent %d bytes: %s", sent, nack->toString().c_str());
                delete nack;
                delete msg;
                close(clientSocket);
                break;
            } else{
                log(3, "new rpi client %s with id %d",
                        Const::ipToString(rpi_ip).c_str(), rpi_id);
                RAck *ack = new RAck(rpi_id);
                auto sent = ack->send(clientSocket);
                log(2, "sent %d bytes: %s", sent, ack->toString().c_str());
                delete ack;
                delete msg;
                continue;
            }
        }else{
            delete msg;
            log(0, "wrong type msg received");
            close(clientSocket);
            break;
        }
    }
    // TODO: erase client connection thread from connections thread
    log(2, "client connection ended");
}

void RpiCommunication::sendNack(int clientSocket, ushort val) {
    RNack *nack = new RNack(val);
    auto sent = nack->send(clientSocket);
    log(2, "Sent %d bytes: %s", sent, nack->toString().c_str());
    delete nack;
}

void RpiCommunication::sendAck(int clientSocket, ushort val) {
    auto *ack = new RAck(val);
    auto sent = ack->send(clientSocket);
    log(2, "sent %d bytes: %s", sent, ack->toString().c_str());
    delete ack;
}