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
#include <mutex>
#include <queue>
#include <condition_variable>

#include "exceptions.h"
#include "Const.h"
#include "Answer.h"

class RpiCommunication {
    struct sockaddr_in address;
    int address_len;
    int socket_fd;
    int opt;
    std::mutex answers_mtx;
    std::condition_variable answers_cv;
    std::queue<Answer *> answers_q;
    std::set<int> readfds_set;
    std::set<int> writefds_set;
    bool stopped;

    // TODO: delete last regirstered , map and set from here, now it is in rpi client class
    
public:
    RpiCommunication(int port);
    int getSocket_fd() const;
    int getConnection(uint *ip);
    ssize_t readMsg(int client_socket, unsigned char *buffer, const size_t length);
    ssize_t sendMsg(int client_socket, unsigned char *msg, size_t length);
    ssize_t sendEncodedMsg(int client_socket, unsigned char *msg, size_t length);
//    void closeConnection(int client_socket);
    void run();
    void answering();
    void stop();

protected:
    uint getIpFromSocket(int client_fd);
    short addNewRpiClient(int client_fd);
    void answer(int client_fd, uchar packet);
    void answer(int client_fd, uchar packet, short val);
    void addAnswer(Answer *answer);
    void closeConnection(int client_fd);

private:

};



#endif //SERVER_SERVER_H
