//
// Created by s17kf on 09.12.18.
//

#include "RpiClient.h"

RpiClient::RpiClient(ulong ip, short id) :ip(ip), id(id) {}

RpiClient::~RpiClient() {
    for(auto device : devices){
        delete device;
    }
}

void RpiClient::addDevice(uchar *codedDevice, size_t expected_size) {
    short name_len;
    Endpoint *new_device = Endpoint::generateFromBytes(codedDevice, expected_size, &name_len);
    devices.push_back(new_device);
}

void RpiClient::printAllDevices() {
    for(auto device : devices){
        device->print();
    }
}