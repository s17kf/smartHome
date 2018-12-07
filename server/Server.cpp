//
// Created by s17kf on 06.12.18.
//

#include "Server.h"

Server::Server(int port) {
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

int Server::getConnection(uint *ip) {
    int client_socket;
    if((client_socket = accept(socket_fd, (struct sockaddr *) &address, (socklen_t *) &address_len)) < 0) {
        throw std::runtime_error("getConnection: accept error");
    }
    char client_ip[16];
    const char *ret_val = inet_ntop(AF_INET, &address.sin_addr.s_addr, client_ip, 16);
    if(ret_val == nullptr)
        std::cout<<"Can't get client ip address"<<std::endl;
    else
        std::cout<<"Client ip: "<<ret_val<<std::endl;
    return client_socket;
}

ssize_t Server::readMsg(int client_socket, unsigned char *buffer, size_t length){
    return read( client_socket , buffer, length);
}

ssize_t Server::sendMsg(int client_socket, unsigned char *msg, size_t length){
    return send(client_socket, msg , length, 0 );
}

ssize_t Server::sendEncodedMsg(int client_socket, unsigned char *msg, size_t length) {
    short encoded_length = length + sizeof(encoded_length);
//    std::cout<<"sending encoded " << length <<"bytes msg"<<std::endl;
    uchar encoded_msg[length + 2];// = new uchar(length + 2);
    memcpy(encoded_msg, &encoded_length, 2);
    memcpy(&encoded_msg[2], msg, length);
    return send(client_socket, encoded_msg , length + 2 , 0 );
}

void Server::closeConnection(int client_socket) {
    close(client_socket);
}

int Server::getSocket_fd() const {
    return socket_fd;
}

int Server::raspberryRegistrate(int client_socket, ulong ip) {
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
//    new_id = 123;
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

    return new_id;
}
