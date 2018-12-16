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
#include <thread>

#include "exceptions.h"
#include "Const.h"
#include "Answer.h"

class RpiCommunication {
    struct sockaddr_in address;
    int addressLen;
    int socketFd;
    int opt;
    std::mutex answersMtx;
    std::condition_variable answersCv;
    std::queue<Answer *> answersQueue;
    std::map<int, time_t > readfdsSet;
    std::set<int> writefdsSet;
    std::set<std::thread *> activeClientConnections;
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
    void stop();

protected:
    static uint getIpFromSocket(int client_fd);
    static short addNewRpiClient(int client_fd);
    void closeConnection(int client_fd);

private:
    static void clientConnection(int clientSocket);
};



#endif //SERVER_SERVER_H
