//
// Created by s17kf on 09.12.18.
//

#include "RpiClient.h"
#include "endpoints/Light.h"
#include "endpoints/Shade.h"

std::map<short, RpiClient *> RpiClient::registered_rpis;
std::set<ulong> RpiClient::ip_adresses;
short RpiClient::last_registered_id = 0;

RpiClient::RpiClient(ulong ip, short id) :ip(ip), id(id) {
    // TODO: throw if id already in use
    registered_rpis.insert(std::pair<short, RpiClient *>(id, this));
    ip_adresses.insert(ip);
    last_registered_id = id;
//    log(10, "new rpi client added (RpiClient constructor)");
}

RpiClient::~RpiClient() {
    for(auto device : devices){
        delete device;
    }
    auto this_in_registered = registered_rpis.find(id);
    if(this_in_registered != registered_rpis.end()){
        registered_rpis.erase(this_in_registered);
    }
    else{
        log(0, "failed to find client id %d when deleting it", id);
    }
    auto ip_in_set = ip_adresses.find(ip);
    if(ip_in_set != ip_adresses.end()){
        ip_adresses.erase(ip_in_set);
    }
    else{
        char client_ip_str[16];
        inet_ntop(AF_INET, &ip, client_ip_str, 16);
        log(0, "there is no rpi client in registered_rpis when deleting client %s",
                client_ip_str);
//        log(0, "there is no client id (%d) in ip_adresses set", id);
    }
}

/**
 * @param codedDevice [2:devType][encoded endpoint][encoded device part]
 * @param expected_size - size of codedDevice
 */
Endpoint* RpiClient::addDevice(uchar *codedDevice, size_t expected_size) {
    short name_len;
    Endpoint *new_device;
    short dev_type_id;
    memcpy(&dev_type_id, codedDevice, sizeof(dev_type_id));
    switch (dev_type_id){
        case DevTypeId::light:
            log(2, "light device received");
            new_device = Light::generateFromBytes(&codedDevice[sizeof(dev_type_id)],
                    expected_size - 2, &name_len);
            break;
        case DevTypeId::shade:
            new_device = Shade::generateFromBytes(&codedDevice[sizeof(dev_type_id)],
                    expected_size - 2, &name_len);
            break;
        default:
            log(0, "unresolved device type received %d", dev_type_id);
            return nullptr;
    }
//    log(3, "added new device: %s", new_device->toString().c_str());
    devices.push_back(new_device);
    return new_device;
}

void RpiClient::printAllDevices() {
    for(auto device : devices){
        log(4, device->toString().c_str());
//        device->toString();
    }
}

bool RpiClient::isIpInUse(ulong ip) {
    return(ip_adresses.find(ip) != ip_adresses.end());
}

bool RpiClient::isRegisteredId(short id) {
    return(registered_rpis.find(id) != registered_rpis.end());
}

RpiClient* RpiClient::getRpiClient(short id) {
    return registered_rpis.find(id)->second;
}

short RpiClient::getNextFreeId() {
    short new_id = (last_registered_id + 1) % Const::max_rpis;
    while(true){
        if(!isRegisteredId(new_id))
            break;
        if(new_id == last_registered_id)
            return -1;
        ++new_id;
    }
    return new_id;
}

ulong RpiClient::getIp() const {
    return ip;
}

short RpiClient::getId() const {
    return id;
}
