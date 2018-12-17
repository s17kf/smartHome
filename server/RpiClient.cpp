//
// Created by s17kf on 09.12.18.
//

#include "RpiClient.h"
#include "endpoints/Light.h"
#include "endpoints/Shade.h"
#include "endpoints/Thermometer.h"

std::map<short, RpiClient *> RpiClient::registeredRpis;
std::set<ulong> RpiClient::ipAddresses;
short RpiClient::lastRegisteredId = 0;

RpiClient::RpiClient(ulong ip, short id) :ip(ip), id(id) {
    // TODO: throw if id already in use
    registeredRpis.insert(std::pair<short, RpiClient *>(id, this));
    ipAddresses.insert(ip);
    lastRegisteredId = id;
    lastActivityTime = time(nullptr);
//    log(10, "new rpi client added (RpiClient constructor)");
}

RpiClient::~RpiClient() {
    for(auto device : devices){
        delete device.second;
    }
    auto this_in_registered = registeredRpis.find(id);
    if(this_in_registered != registeredRpis.end()){
        registeredRpis.erase(this_in_registered);
    }
    else{
        log(0, "failed to find client id %d when deleting it", id);
    }
    auto ip_in_set = ipAddresses.find(ip);
    if(ip_in_set != ipAddresses.end()){
        ipAddresses.erase(ip_in_set);
    }
    else{
        char client_ip_str[16];
        inet_ntop(AF_INET, &ip, client_ip_str, 16);
        log(0, "there is no rpi client in registeredRpis when deleting client %s",
                client_ip_str);
//        log(0, "there is no client id (%d) in ipAddresses set", id);
    }
}

/**
 * @param codedDevice [2:devType][encoded endpoint][encoded device part]
 * @param expected_size - size of codedDevice
 */
Endpoint* RpiClient::addDevice(uchar *codedDevice, size_t expected_size) {
    short nameLen;
    Endpoint *newDevice;
    ushort devTypeId;
    memcpy(&devTypeId, codedDevice, sizeof(devTypeId));
    devTypeId = ntohs(devTypeId);
    switch (devTypeId){
        case DevTypeId::light:
            log(2, "light device received");
            newDevice = Light::generateFromBytes(&codedDevice[sizeof(devTypeId)],
                    expected_size - sizeof(devTypeId), &nameLen);
            break;
        case DevTypeId::shade:
            newDevice = Shade::generateFromBytes(&codedDevice[sizeof(devTypeId)],
                    expected_size - sizeof(devTypeId), &nameLen);
            break;
        case DevTypeId::thermometer:
            newDevice = Thermometer::generateFromBytes(&codedDevice[sizeof(devTypeId)],
                    expected_size - sizeof(devTypeId), &nameLen);
            break;
        default:
            log(0, "unresolved device type received %d", devTypeId);
            return nullptr;
    }

//    log(3, "added new device: %s", newDevice->toString().c_str());
    devices.insert(std::pair<int, Endpoint*>(newDevice->getKey() ,newDevice));
    return newDevice;
}

void RpiClient::printAllDevices() {
    for(auto device : devices){
        log(4, device.second->toString().c_str());
    }
}

bool RpiClient::isIpInUse(ulong ip) {
    return(ipAddresses.find(ip) != ipAddresses.end());
}

bool RpiClient::isRegisteredId(short id) {
    return(registeredRpis.find(id) != registeredRpis.end());
}

RpiClient* RpiClient::getRpiClient(short id) {
    return registeredRpis.find(id)->second;
}

short RpiClient::getNextFreeId() {
    short new_id = (lastRegisteredId + 1) % Const::max_rpis;
    while(true){
        if(!isRegisteredId(new_id))
            break;
        if(new_id == lastRegisteredId)
            return -1;
        new_id = (new_id + 1) % Const::max_rpis;
    }
    return new_id;
}

