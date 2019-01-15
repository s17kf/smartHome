//
// Created by s17kf on 18.12.18.
//

#ifndef SERVER_NETWORK_H
#define SERVER_NETWORK_H

#include "../Const.h"

uint getIpFromSocket(int client_fd);

void ntohDouble(double *val);

#endif //SERVER_NETWORK_H
