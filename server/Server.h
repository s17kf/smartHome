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
#include "Packet.h"

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned long ulong;

class Server {
    struct sockaddr_in address;
    int address_len;
    int socket_fd;
    int opt;
    short last_registered_id;
    std::map<ulong, short> registered_rpis; //key - rpi's ip, value - rpi identifier
    std::set<short> ids_in_use;
    
public:
    Server(int port);
    int getSocket_fd() const;
    int getConnection(uint *ip);
    ssize_t readMsg(int client_socket, unsigned char *buffer, size_t length);
    ssize_t sendMsg(int client_socket, unsigned char *msg, size_t length);
    ssize_t sendEncodedMsg(int client_socket, unsigned char *msg, size_t length);
    void closeConnection(int client_socket);
    int raspberryRegistrate(int client_socket, ulong ip);

};



#endif //SERVER_SERVER_H
