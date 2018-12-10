#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <thread>

#include "RpiCommunication.h"
#include "Packet.h"
#include "logger/Logger.h"

//using namespace std;

void raspberryConnections();

int main(){
    std::string log_filepath = "../logs/file.log";
    uint log_stdout_min_lv = 5;
    logInit(log_filepath.c_str(), log_stdout_min_lv);
    std::thread rpi_communication(raspberryConnections);
//    std::cout<<"communication with raspberry is going in other thread"<<std::endl;
    log(0, "communication with raspberry is going in other thread");
    rpi_communication.join();
//    std::cout<<"other thread joined"<<std::endl;
    log(0, "other thread joined ending program");
    logClose();
    return 0;
}

void raspberryConnections(){
    fd_set rfds;
    timeval tv;
    unsigned char buffer[1024];
    auto *rpi_communication = new RpiCommunication(1234);
    FD_ZERO(&rfds);
    FD_SET(rpi_communication->getSocket_fd(), &rfds);
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    int retval = select(rpi_communication->getSocket_fd()+1, &rfds, nullptr, nullptr, &tv);
    if (retval == -1)
        perror("select()");
    else if (retval) {
        log(1, "new connection from raspberry");
        if (FD_ISSET(rpi_communication->getSocket_fd(), &rfds)) {
            uint client_ip;
            int client_fd = rpi_communication->getConnection(&client_ip);
            char client_ip_str[16];
            const char *ret_val = inet_ntop(AF_INET, &client_ip, client_ip_str, 16);
            if(ret_val != nullptr)
                log(2, "Get connection from %s", client_ip_str);
            else{
                log(1, "Failed to get rpi client ip address");
            }
            ssize_t received = rpi_communication->readMsg(client_fd, buffer, 2);
//            auto *msg_len = reinterpret_cast<short *>(buffer);
            short msg_len;
            memcpy(&msg_len, buffer, sizeof(msg_len));
            received = rpi_communication->readMsg(client_fd, buffer, msg_len - 2);
            if(received == msg_len - 2)
                log(3, "received %d bytes from %s", received, client_ip_str);
            else
                log(0, "received %d bytes when expected %d from %s", received, msg_len - 2, client_ip_str);
            int new_id;
            switch (buffer[0]) {
                case Packet::reg:
                    log(4, "ask for registration received from %s", client_ip_str);
                    new_id = rpi_communication->raspberryRegistrate(client_fd, client_ip);
                    log(3, "%s - new rpi registrate with id %d", client_ip_str, new_id);
                    //TODO: send nack if registration failed
                    break;
                case Packet::exit:
                    log(3, "exit received");
                    break;
                default:
                    log(0, "unresolved packet received from %s", client_ip_str);
                    //TODO: throw exception
            }

            rpi_communication->closeConnection(client_fd);
        }
    }
    else
        printf("No tries to connect within five seconds.\n");

    delete rpi_communication;
}