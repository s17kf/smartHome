//
// Created by s17kf on 06.12.18.
//

#include <thread>
#include "RpiCommunication.h"
#include "Packet.h"
#include "RpiClient.h"
#include "common/Logger.h"
#include "common/Atomic.h"
#include "Msg.h"

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

ssize_t RpiCommunication::readMsg(int client_socket, unsigned char *buffer, const size_t length){
    return read( client_socket , buffer, length);
}

ssize_t RpiCommunication::sendMsg(int client_socket, unsigned char *msg, size_t length){
    return send(client_socket, msg , length, 0 );
}

ssize_t RpiCommunication::sendEncodedMsg(int client_socket, unsigned char *msg, size_t length) {
    short encoded_length = length + sizeof(encoded_length);
//    std::cout<<"sending encoded " << length <<"bytes msg"<<std::endl;
    uchar encoded_msg[length + 2];// = new uchar(length + 2);
    memcpy(encoded_msg, &encoded_length, 2);
    memcpy(&encoded_msg[2], msg, length);
    return send(client_socket, encoded_msg , length + 2 , 0 );
}

//void RpiCommunication::closeConnection(int client_socket) {
//    close(client_socket);
//}

int RpiCommunication::getSocket_fd() const {
    return socketFd;
}

void RpiCommunication::run() {
    fd_set readfds;
    timeval timeout;
    readfdsSet.insert(std::pair<int, time_t>(socketFd, time(nullptr)));
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

void RpiCommunication::closeConnection(int client_socket) {
    log(5, "closing socker %d", client_socket);
    readfdsSet.erase(client_socket);
    close(client_socket);
    log(5, "socked %d closed", client_socket);
}

void RpiCommunication::clientConnection(int clientSocket) {
    while(true){
        log(5, "client connection");
        Msg *msg = Msg::receiveMsg(clientSocket);
        if(msg == nullptr){
            log(0, "received msg is nullptr, closing client connectioon");
            close(clientSocket);
            break;
        }
        if(auto dev = dynamic_cast<Dev *>(msg)){
            log(1, "dev received");
            auto rpi_id = dev->getRpiId();
            auto rpiClient = RpiClient::getRpiClient(rpi_id);
            auto newDevice = rpiClient->addDevice(dev->getCodedDevice(),
                    dev->getCodedDevLen());
            if(newDevice == nullptr){
                log(2, "Failed to add nev device from %d", rpi_id);
                Nack *nack = new Nack(dev->getDevId());
                auto sent = nack->send(clientSocket);
                log(2, "Sent %d bytes: %s", sent, nack->toString().c_str());
                delete nack;
                delete msg;
                continue;
            }
            else{
                log(2, "added new device on %d: %s", rpi_id,
                        newDevice->toString().c_str());
                Ack *ack = new Ack(dev->getDevId());
                auto sent = ack->send(clientSocket);
                log(2, "sent %d bytes: %s", sent, ack->toString().c_str());
                delete ack;
                delete msg;
                continue;
            }
        }else if(auto end = dynamic_cast<End *>(msg)){
            log(1, "end received, closing socket");
            delete msg;
            close(clientSocket);
            break;
        }else if(auto reg = dynamic_cast<Reg *>(msg)){
            log(1, "reg received");
            auto rpi_id = addNewRpiClient(clientSocket);
            auto rpi_ip = getIpFromSocket(clientSocket);
            if(rpi_id < 0){
                log(0, "new rpi id is %d - max rpis reached", rpi_id);
                Nack *nack = new Nack(ushort(0));
                auto sent = nack->send(clientSocket);
                log(2, "sent %d bytes: %s", sent, nack->toString().c_str());
                delete nack;
                delete msg;
                close(clientSocket);
                break;
            } else{
                log(3, "new rpi client %s with id %d",
                        Const::ipToString(rpi_ip).c_str(), rpi_id);
                Ack *ack = new Ack(rpi_id);
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
    log(2, "client connection ended");
}