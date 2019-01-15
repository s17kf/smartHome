//
// Created by s17kf on 18.12.18.
//

#include "network.h"
#include <sys/socket.h>
#include <netinet/in.h>

uint getIpFromSocket(int client_fd) {
    sockaddr_in addr;
    socklen_t addr_size = sizeof(sockaddr_in);
    int res = getpeername(client_fd, (struct sockaddr *)&addr, &addr_size);
    return addr.sin_addr.s_addr;
}

void ntohDouble(double *val){
    short testVal = 1;
    if(testVal == ntohs(testVal))
        return;
    uint size = sizeof(double);
    auto *tab = reinterpret_cast<uchar *>(val);
    for(int i = 0; i < size / 2; ++i){
        uchar temp = tab[i];
        tab[i] = tab[size - 1 - i];
        tab[size - 1 - i] = temp;
    }
}