//
// Created by s17kf on 06.12.18.
//

#include "RpiCommunication.h"
#include "Packet.h"
#include "RpiClient.h"
#include "common/Logger.h"
#include "common/Atomic.h"

//struct sockaddr_in RpiCommunication::address;
//int RpiCommunication::address_len = 0;
//int RpiCommunication::socket_fd = 0;

RpiCommunication::RpiCommunication(int port) {
    opt = 1;
    Atomic::set(&stopped, false);
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        //todo exception service
        throw file_descriptor_error("error creating socket");
    }
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        throw std::runtime_error("setsockopt error");
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    address_len = sizeof(address);
    if (bind(socket_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        throw std::runtime_error("bind error");
    }
    if (listen(socket_fd, 2) < 0) {
        throw std::runtime_error("listen error");
    }
}

int RpiCommunication::getConnection(uint *ip) {
    int client_socket;
    if((client_socket = accept(socket_fd, (struct sockaddr *) &address, (socklen_t *) &address_len)) < 0) {
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
    return socket_fd;
}

void RpiCommunication::run() {
    fd_set readfds, writefds;
    timeval timeout;
    readfds_set.insert(0);
    writefds_set.insert(0);
    readfds_set.insert(socket_fd);
    while(true) {
        if(Atomic::get(&stopped))
            break;
        FD_ZERO(&readfds);
        for(auto fd = ++readfds_set.begin(); fd != readfds_set.end(); ++fd){
            FD_SET(*fd, &readfds);
//            log(10, "%d added to readfds");
        }
        timeout.tv_sec = Const::answer_timeout;
        timeout.tv_usec = 0;
        int retval = select( *(readfds_set.rbegin())+ 1,
                &readfds, nullptr, nullptr, &timeout);
        if (retval == -1)
            perror("select()");
        else if (retval) {
            log(1, "new connection from raspberry");
            for (auto fds: readfds_set) {
                if (FD_ISSET(fds, &readfds)) {
                    log(1, "read possible");
                    if (fds == socket_fd) {
                        log(2, "ask for new connection available");
                        uint new_client_ip;
                        int new_client_fd = getConnection(&new_client_ip);
                        log(2, "new connection from %s",
                                Const::ipToString(new_client_ip).c_str());
                        FD_SET(new_client_fd, &readfds);
                        readfds_set.insert(new_client_fd);
                        // TODO: add client to readfds and write fds
                        address.sin_addr.s_addr = INADDR_ANY;
                        break;
                    } else {
                        log(2, "new msg available");
                        unsigned char buffer[128];
                        int received = read(fds, buffer, 128);
                        if(received == 0){
                            log(0, "received 0 bytes so closing socket");
                            closeConnection(fds);
                            continue;
                        }
                        log(1, "received %d bytes", received);
                        // TODO: check if received == length
                        uchar received_packet = buffer[2];
                        short rpi_id, val;
                        uint rpi_ip;
                        RpiClient *rpiClient;
                        Endpoint *new_device;
                        switch(received_packet){
                            case Packet::reg:
                                log(2, "reg received");
                                rpi_id = addNewRpiClient(fds);
                                rpi_ip = getIpFromSocket(fds);
                                if(rpi_id < 0){
                                    answer(fds, Packet::nack);
                                    close(fds);
                                    log(0, "new rpi id is %d - max rpis reached socket closed",
                                            rpi_id);
                                    closeConnection(fds);
                                }
                                else{
                                    log(4, "new rpi client %s with id %d",
                                            Const::ipToString(rpi_ip).c_str(), rpi_id);
                                    answer(fds, Packet::ack, rpi_id);
                                }
                                break;
                            case Packet::dev:
                                log(2, "dev received");
                                memcpy(&rpi_id, &buffer[Const::rpi_id_index], sizeof(rpi_id));
                                rpiClient = RpiClient::getRpiClient(rpi_id);
                                new_device = rpiClient->addDevice(&buffer[Const::dev_type_index],
                                        received - Const::dev_type_index);
                                if(new_device == nullptr){
                                    log(2, "Failed to add nev device from %d", rpi_id);
                                    memcpy(&val, &buffer[Const::dev_key_index], sizeof(val));
                                    answer(fds, Packet::nack, val);
                                }
                                else{
                                    log(2, "Added new device on %d: %s", rpi_id,
                                            new_device->toString().c_str());
                                    answer(fds, Packet::ack, new_device->getKey());
                                }
                                break;
                            case Packet::end:
                                log(2, "end received: closing socket");
                                closeConnection(fds);
                                break;
                            default:
                                log(1, "unresolved packet received %d, so closing socket", received_packet);
                                closeConnection(fds);
                        }
                    }
                }
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

void RpiCommunication::answering() {
    while(true){
        std::unique_lock<std::mutex> lck(answers_mtx);
        while(answers_cv.wait_for(lck, std::chrono::seconds(5))
        == std::cv_status::timeout){
            if(Atomic::get(&stopped))
                break;
            if(!answers_q.empty())
                break;
            log(5, "no answer to send within timeout");
        }
        if(Atomic::get(&stopped))
            break;
        if(answers_q.empty()){
            continue;
        }
        Answer *answer = answers_q.front();
        answers_q.pop();
        lck.unlock();
        int client_fd = answer->getSocket_fd();
        size_t msg_len = answer->getMsg_len();
        uchar msg[msg_len];
        memcpy(msg, answer->getMsg(), msg_len);
        fd_set writefds;
        FD_SET(client_fd, &writefds);
        timeval timeout;
        timeout.tv_sec = Const::answer_timeout;
        timeout.tv_usec = 0;
        // TODO: deleting answer;
        int retval = select(client_fd + 1, nullptr, &writefds, nullptr, &timeout);
        if (retval == -1)
            perror("select()");
        else if (retval){
            if(FD_ISSET(client_fd, &writefds)){
                int sent = write(client_fd, msg, msg_len);
                log(2, "sent %d bytes", sent);
            }else{
                log(0, "possible write to some undefined file descriptor");
            }

        }else {
            log(0, "no nonblocking sockets to write within timeout");
        }
    }
    log(1, "endpoint of answering method received");
}

void RpiCommunication::addAnswer(Answer *answer) {
    log(4, "add answer entered");
    std::unique_lock<std::mutex> lck(answers_mtx);
    answers_q.push(answer);
    answers_cv.notify_one();
}

void RpiCommunication::stop() {
    Atomic::set(&stopped, true);
    log(5, "stopped set to true");
}

void RpiCommunication::answer(int client_fd, uchar packet) {
    short answer_len = 3;
    uchar answer_msg[answer_len];
    memcpy(answer_msg, &answer_len, sizeof(answer_len));
    answer_msg[2] = packet;
    addAnswer(new Answer(client_fd, -1, answer_len, answer_msg));
    log(1, "added %d answer to queue", packet);
}

void RpiCommunication::answer(int client_fd, uchar packet, short val) {
    short answer_len = 5;
    uchar answer_msg[answer_len];
    memcpy(answer_msg, &answer_len, sizeof(answer_len));
    answer_msg[2] = packet;
    memcpy(&answer_msg[3], &val, sizeof(val));
    addAnswer(new Answer(client_fd, val, answer_len, answer_msg));
    log(1, "added %d with val=%d answer to queue", packet, val);
}

void RpiCommunication::closeConnection(int client_socket) {
    readfds_set.erase(client_socket);
    close(client_socket);
}