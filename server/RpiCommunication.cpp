//
// Created by s17kf on 06.12.18.
//

#include "RpiCommunication.h"
#include "Packet.h"
#include "RpiClient.h"
#include "logger/Logger.h"

RpiCommunication::RpiCommunication(int port) {
    opt = 1;
    last_registered_id = 0;
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
    if (listen(socket_fd, 1) < 0) {
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

void RpiCommunication::closeConnection(int client_socket) {
    close(client_socket);
}

int RpiCommunication::getSocket_fd() const {
    return socket_fd;
}

int RpiCommunication::raspberryRegistrate(int client_socket, ulong ip) {
    if(registered_rpis.find(ip) == registered_rpis.end()){
        //TODO: delete old registered rpi
    }
    short new_id = (last_registered_id + 1)%Const::max_rpis;
    while(true){
        if(ids_in_use.find(new_id) == ids_in_use.end())
            break;
        if(new_id == last_registered_id)
            throw std::out_of_range("max registered raspberries reached");
        ++new_id;
    }
//    new_id = 1111;
    last_registered_id = new_id;
    registered_rpis.insert(std::pair<short, ulong>(ip, new_id));
    ids_in_use.insert(new_id);
    auto answer_len = sizeof(new_id) + sizeof(Packet::ack);
//    auto *answer = new uchar(answer_len);
    uchar answer[answer_len];
    answer[0] = Packet::ack;
    memcpy(&answer[1], &new_id, sizeof(new_id));
    auto *msg_len = reinterpret_cast<short *>(&answer[1]);
//    std::cout<<"wait before answer"<<std::endl;
//    sleep(1);
    sendEncodedMsg(client_socket, answer, answer_len);
    auto new_devices = addDevices(client_socket);
    log(3, "registered %d new devices", new_devices);
    //TODO: log rpi id

    return new_id;
}

int RpiCommunication::addDevices(int client_fd) {
    fd_set rfds;
    timeval timeout;
    timeout.tv_sec = Const::answer_timeout;
    timeout.tv_usec = 0;
    FD_ZERO(&rfds);
    FD_SET(client_fd, &rfds);
    int ret = 0;
    RpiClient *rpiClient = new RpiClient(15, 12);
    while(true) {
        int retval = select(client_fd + 1, &rfds, nullptr, nullptr, &timeout);
        if (retval == -1) {
            perror("select error");
            return -1;
        } else if (retval) {
            if (FD_ISSET(client_fd, &rfds)) {
                auto *msg = new uchar[3];
                auto received = readMsg(client_fd, msg, 2);
                auto *msg_len = new short;
                memcpy(msg_len, msg, 2);
                delete[] msg;
                msg = new uchar[*msg_len - 2];
                received = readMsg(client_fd, msg, *msg_len - 2);
//                std::cout<<"received "<< received <<"bytes"<<std::endl;
//                for (auto i = 0; i < received; ++i) {
//                    std::cout << msg[i] << " ";
//                }
//                std::cout << std::endl;
                delete msg_len;
                if(msg[2] == Packet::end){
                    delete[] msg;
                    break;
                }
                rpiClient->addDevice(&msg[Const::name_len_index], received);
                delete[] msg;
                uchar answer_ack[1];
                answer_ack[0] = Packet::ack;
                sendEncodedMsg(client_fd, answer_ack, 1);
                ++ret;
            } else {
                std::cout << "something received not on client socket" << std::endl;
            }
        }
    }
    //TODO: add devices implement light and shade
    //TODO: endpoint should be virtual ...
    log(3, "registered %d new devices", ret);
    rpiClient->printAllDevices();
    delete rpiClient;
    return ret;
}