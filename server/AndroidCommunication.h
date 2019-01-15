//
// Created by s17kf on 18.12.18.
//

#ifndef SERVER_ANDROIDCOMMUNICATION_H
#define SERVER_ANDROIDCOMMUNICATION_H


#include <netinet/in.h>
#include <thread>
#include <set>

#include "common/InterThreadQueue.h"

#include "AndroidMsg.h"

class AndroidCommunication {
    sockaddr_in address;
    int addressLen;
    int socketFd;
    int opt;
    std::set<std::thread *> activeConnections;
    bool stopped;
    InterThreadQueue<ASetState *> *toRpiMsgs;


public:
    explicit AndroidCommunication(int port);
    void run();
    void stop();
    InterThreadQueue<ASetState *> *getToRpiMsgs(){ return toRpiMsgs; }

protected:
    int getConnection();

private:
    void androindConnection(int clientSocket);
};


#endif //SERVER_ANDROIDCOMMUNICATION_H
