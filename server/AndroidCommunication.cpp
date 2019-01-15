//
// Created by s17kf on 18.12.18.
//

#include <unistd.h>
#include "AndroidCommunication.h"
#include "common/Atomic.h"
#include "exceptions.h"
#include "common/Logger.h"
#include "common/network.h"
#include "AndroidMsg.h"
#include "RpiClient.h"

AndroidCommunication::AndroidCommunication(int port) {
    opt = 1;
    toRpiMsgs = new InterThreadQueue<ASetState *>();
    Atomic::set(&stopped, false);
    if((socketFd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        throw file_descriptor_error("error creating socket android server");
    }
    if(setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        throw std::runtime_error("setsockopt error");
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    addressLen = sizeof(address);
    if(bind(socketFd, (sockaddr *)&address, sizeof(address)) < 0){
        throw std::runtime_error("bind error");
    }
    if(listen(socketFd, 2) < 0){
        throw std::runtime_error("listen error");
    }
}

int AndroidCommunication::getConnection() {
    int client_socket;
    if((client_socket = accept(socketFd, (struct sockaddr *) &address, (socklen_t *) &addressLen)) < 0) {
        throw std::runtime_error("android getConnection: accept error");
    }
    return client_socket;
}

void AndroidCommunication::stop() {
    Atomic::set(&stopped, true);
    log(5, "stopped set to true");
}

void AndroidCommunication::run() {
    log(0, "android communication thread id is %d", std::this_thread::get_id());
    fd_set readfds;
    timeval timeout;
    while(true){
        if(Atomic::get(&stopped)){
            break;
        }
        FD_ZERO(&readfds);
        FD_SET(socketFd, &readfds);
        timeout.tv_sec = Const::answer_timeout;
        timeout.tv_usec = 0;
        int retval = select(socketFd + 1, &readfds, nullptr, nullptr, &timeout);
        if(retval == -1){
            perror("select()");
        } else if(retval){
            log(1, "new connection from android");
            if(FD_ISSET(socketFd, &readfds)){
                log(2, "ask for new connection from android available");
                int newAndroidFd = getConnection();
                log(2, "new connection from %s",
                        Const::ipToString(getIpFromSocket(newAndroidFd)).c_str());
                std::thread *t = new std::thread(&AndroidCommunication::androindConnection,
                        this, newAndroidFd);
                activeConnections.insert(t);
                log(0, "communication with android goes in other thread: %d",
                        t->get_id());
                address.sin_addr.s_addr = INADDR_ANY;
            }
            else{
                log(0, "some undefined read possible!!!");
            }
        } else{
            log(0, "no tries to connect within timeout");
        }
    }
}

void AndroidCommunication::androindConnection(int clientSocket) {
    log(1, "android connection on socket %d", clientSocket);
//    sleep(10);
    while(true){
        AndroidMsg *msg = AndroidMsg::receiveMsg(clientSocket);
        if(msg == nullptr){
            log(0, "received ,sg is nullptr, closing client connection");
            close(clientSocket);
            break;
        }
        if(auto getDevs = dynamic_cast<AGetDevs *>(msg)){
            log(1, "android GetDevs received");
            for(auto dev: RpiClient::getACodedDevices()){
                dev->send(clientSocket);
                log(2, "dev %s sent", dev->toString().c_str());
                delete dev;
            }
            AEnd *aEnd = new AEnd();
            aEnd->send(clientSocket);
            delete aEnd;
            delete msg;
            break;
        }else if(auto setState = dynamic_cast<ASetState *>(msg)){
            log(1, "android SetState received");
            toRpiMsgs->put(setState);
        }else if(auto reg = dynamic_cast<AReg *>(msg)){
            log(1, "android reg received");
            // TODO: creating client object ...
            ASsid *ssid = new ASsid(2);
            ssid->send(clientSocket);
            delete msg;
            delete ssid;
            break;
        }else{
            delete msg;
            log(0, "wrong type msg recieved");
            close(clientSocket);
            break;
        }
    }
    // TODO: erase android client connection thread from connections
    log(2, "android client connection ended");
}