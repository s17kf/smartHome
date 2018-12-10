//
// Created by s17kf on 09.12.18.
//

#ifndef SERVER_RPICLIENT_H
#define SERVER_RPICLIENT_H

#include <vector>
#include "RpiCommunication.h"
#include "endpoints/Endpoint.h"

class RpiClient {
    ulong ip;
    short id;
    std::vector<Endpoint *> devices;

public:
    RpiClient(ulong ip, short id);
    ~RpiClient();
    void addDevice(uchar *codedDevice, size_t size);
    void printAllDevices();
};


#endif //SERVER_RPICLIENT_H
