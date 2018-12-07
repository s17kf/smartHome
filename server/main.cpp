#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <thread>

#include "Server.h"
#include "Packet.h"

//using namespace std;

void raspberryConnections();

int main(){
    std::thread rpi_communication(raspberryConnections);
    std::cout<<"communication with raspberry is going in other thread"<<std::endl;
    rpi_communication.join();
    std::cout<<"other thread joined"<<std::endl;

    return 0;
}

void raspberryConnections(){
    fd_set rfds;
    timeval tv;

    unsigned char buffer[1024];
    auto *server = new Server(1234);
    FD_ZERO(&rfds);
    FD_SET(server->getSocket_fd(), &rfds);
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    int retval = select(server->getSocket_fd()+1, &rfds, nullptr, nullptr, &tv);
    if (retval == -1)
        perror("select()");
    else if (retval)
        printf("Data is available now.\n");
    if(FD_ISSET(server->getSocket_fd(), &rfds)){
        std::cout<<"connection on server"<<std::endl;
        uint client_ip;
        int client_fd = server->getConnection(&client_ip);
        ssize_t received = server->readMsg(client_fd, buffer, 2);
        std::cout<<received<< " bytes from client"<<std::endl;
        auto *msg_len = reinterpret_cast<short *>(buffer);
        std::cout<<"msg len: " << *msg_len<<std::endl;
        received = server->readMsg(client_fd, buffer, *msg_len - 2);
        std::cout<<"received " << received << " additional bytes" << std::endl;
        int new_id;
        switch (buffer[0]){
            case Packet::reg:
                std::cout<<"ask for registration received"<<std::endl;
                new_id = server->raspberryRegistrate(client_fd, client_ip);
                std::cout<<"new rpi with id " << new_id<<std::endl;
                //TODO: send nack if registration failed
                break;
            case Packet::exit:
                std::cout<<"exit received"<<std::endl;
                break;
            default:
                std::cout<<"unresolved packet received"<<std::endl;
                //TODO: throw exception
        }

        server->closeConnection(client_fd);
    }
    else
        printf("No tries to connect within five seconds.\n");
    
}