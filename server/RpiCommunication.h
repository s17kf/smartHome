//
// Created by s17kf on 06.12.18.
//

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <map>
#include <set>

#include "exceptions.h"
#include "Const.h"

class RpiCommunication {
    struct sockaddr_in address;
    int address_len;
    int socket_fd;
    int opt;
    short last_registered_id;
    std::map<ulong, short> registered_rpis; //key - rpi's ip, value - rpi identifier
    std::set<short> ids_in_use;
    
public:
    RpiCommunication(int port);
    int getSocket_fd() const;
    int getConnection(uint *ip);
    ssize_t readMsg(int client_socket, unsigned char *buffer, const size_t length);
    ssize_t sendMsg(int client_socket, unsigned char *msg, size_t length);
    ssize_t sendEncodedMsg(int client_socket, unsigned char *msg, size_t length);
    void closeConnection(int client_socket);
    int raspberryRegistrate(int client_socket, ulong ip);

private:
    int addDevices(int client_fd);

};



#endif //SERVER_SERVER_H