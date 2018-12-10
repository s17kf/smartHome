//
// Created by s17kf on 09.12.18.
//

#include "RpiClient.h"
#include "endpoints/Light.h"
#include "endpoints/Shade.h"

RpiClient::RpiClient(ulong ip, short id) :ip(ip), id(id) {}

RpiClient::~RpiClient() {
    for(auto device : devices){
        delete device;
    }
}

/**
 * @param codedDevice [2:devType][encoded endpoint][encoded device part]
 * @param expected_size - size of codedDevice - 2 (dev type id)
 */
void RpiClient::addDevice(uchar *codedDevice, size_t expected_size) {
    short name_len;
    Endpoint *new_device;
    short dev_type_id;
    memcpy(&dev_type_id, codedDevice, sizeof(dev_type_id));
    switch (dev_type_id){
        case DevTypeId::light:
            log(2, "light device received");
            new_device = Light::generateFromBytes(&codedDevice[sizeof(dev_type_id)],
                    expected_size, &name_len);
            break;
        case DevTypeId::shade:
            new_device = Shade::generateFromBytes(&codedDevice[sizeof(dev_type_id)],
                    expected_size, &name_len);
            break;
        default:
            log(0, "unresolved device type received %d", dev_type_id);
    }
    devices.push_back(new_device);
}

void RpiClient::printAllDevices() {
    for(auto device : devices){
        log(4, device->toString().c_str());
//        device->toString();
    }
}