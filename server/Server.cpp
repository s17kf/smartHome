//
// Created by s17kf on 09.12.18.
//

#include "Server.h"
#include "RpiClient.h"
#include "common/Atomic.h"
#include <arpa/inet.h>

Server::Server(uint port) : port(port){
    rpiCommunication = new RpiCommunication(port);
    androidCommunication = new AndroidCommunication(port + 2);
    Atomic::set(&stopped, false);
}

Server::~Server() {
    delete rpiCommunication;
//    delete androidCommunication;
}

void Server::start() {
    rpiServerThread = std::thread(&RpiCommunication::run, rpiCommunication);
    androidServerThread = std::thread(&AndroidCommunication::run,
            androidCommunication);
    mainThread = std::thread(&Server::main, this);
    log(0, "rpi server started");
}

void Server::stop() {
//    12346

    Atomic::set(&stopped, true);
    rpiCommunication->stop();
    androidCommunication->stop();
    rpiServerThread.join();
    log(0, "pi server thread joined");
    androidServerThread.join();
    log(0, "android thread joined");
    mainThread.join();
    log(0, "main server thread joined");
}

void Server::main() {
    log(1, "main server thread started id is %d", std::this_thread::get_id());
//    srand(time(nullptr));
    while(!isStopped()){
        //if need to change device state
        ASetState *aSetState = androidCommunication->getToRpiMsgs()->get();
        if(!RpiClient::isRegisteredId(aSetState->getRpiId())){
            log(1, "set state to unregistered client id %d", aSetState->getRpiId());
            delete aSetState;
            continue;
        }
        RpiClient * rpiClient = RpiClient::getRpiClient(aSetState->getRpiId());
        RSetState *rSetState = new RSetState(aSetState->getDevId(),
                aSetState->getState());

//        ulong rpiIp = rpiClient->getIp();
        int sock;
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            log(1, "failed to create socket: connecting to pi server");
            continue;
        }
        sockaddr_in pi_server_address;
        pi_server_address.sin_family = AF_INET;
        pi_server_address.sin_port = htons(port + 1);
        pi_server_address.sin_addr.s_addr = rpiClient->getIp();

        if (connect(sock, (sockaddr *)&pi_server_address,
                sizeof(pi_server_address)) < 0){
            std::string ipStr = Const::ipToString(rpiClient->getIp());
            log(1, "failed to connect to pi server ip: %s, port: %d: %d: %s",
                    ipStr.c_str(), port + 1, errno, strerror(errno));
            continue;
        }
        rSetState->send(sock);
        close(sock);

//        sleep(2);
//        ushort rpiId = 1;
//        RpiClient *rpiClient;
//        if(RpiClient::isRegisteredId(rpiId))
//            rpiClient = RpiClient::getRpiClient(rpiId);
//        else {
//            log(1, "no client id %d", rpiId);
//            continue;
//        }
//        ulong rpiIp = rpiClient->getIp();
//        int sock;
//        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
//            log(1, "failed to create socket: connecting to pi server");
//            continue;
//        }
//        sockaddr_in pi_server_address;
//        pi_server_address.sin_family = AF_INET;
//        pi_server_address.sin_port = htons(port + 1);
//        pi_server_address.sin_addr.s_addr = rpiIp;
//
//        if (connect(sock, (sockaddr *)&pi_server_address,
//                sizeof(pi_server_address)) < 0){
//            std::string ipStr = Const::ipToString(rpiIp);
//            log(1, "failed to connect to pi server ip: %s, port: %d: %d: %s",
//                    ipStr.c_str(), port + 1, errno, strerror(errno));
//            continue;
//        }
//        log(3, "connected to pi server");
//        ushort devId = rand() % 3;
//        log(3, "dev to set state: %s",
//                rpiClient->getDevice(devId)->toString().c_str());
//        // TODO: get divice throws exception (out of range)
//        ushort state = rand() % 2;
//        RSetState *setState = new RSetState(devId, state);
//        log(3, "msg to send is %s", setState->toString().c_str());
//        setState->send(sock);
//        close(sock);
    }

}

bool Server::isStopped() {
    return Atomic::get(&stopped);
}