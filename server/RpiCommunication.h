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
#include "RpiClient.h"

class RpiCommunication {
    struct sockaddr_in address;
    int addressLen;
    int socketFd;
    int opt;
    std::set<std::thread *> activeClientConnections;
    bool stopped;

    // TODO: delete last regirstered , map and set from here, now it is in rpi client class
    
public:
    RpiCommunication(int port);
    int getSocket_fd() const;
    void run();
    void stop();


protected:
    static uint getIpFromSocket(int client_fd);
    static short addNewRpiClient(int client_fd);
    int getConnection(uint *ip);

private:
    static void clientConnection(int clientSocket);
    static void sendNack(int clientSocket, ushort val);
    static void sendAck(int clientSocket, ushort val);
};



#endif //SERVER_SERVER_H
