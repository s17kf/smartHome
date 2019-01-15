//
// Created by s17kf on 09.12.18.
//

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include "RpiCommunication.h"
#include "AndroidCommunication.h"
#include <thread>

class Server {
    uint port;
    RpiCommunication *rpiCommunication;
    AndroidCommunication *androidCommunication;
    std::thread rpiServerThread;
    std::thread mainThread;
    std::thread androidServerThread;
    bool stopped;
public:
    explicit Server(uint port);
    ~Server();

    void start();
    void stop();

private:
    void main();
    bool isStopped();
};


#endif //SERVER_SERVER_H
