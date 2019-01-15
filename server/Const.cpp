//
// Created by s17kf on 11.12.18.
//

#include "Const.h"
#include <sys/socket.h>
#include <arpa/inet.h>


std::string Const::ipToString(ulong ip) {
    char buffer[16];
    const char *ret_val = inet_ntop(AF_INET, &ip, buffer, 16);
    return std::string(buffer);
}