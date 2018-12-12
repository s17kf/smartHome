//
// Created by s17kf on 09.12.18.
//

#ifndef SERVER_RPICLIENT_H
#define SERVER_RPICLIENT_H

#include <vector>
#include "RpiCommunication.h"
#include "endpoints/Endpoint.h"
#include "common/Logger.h"

class RpiClient {
    ulong ip;
    short id;
    /**
     * registered_rpis = map< ip_adress, RpiClient *>
     */
    static std::map<short, RpiClient *> registered_rpis;
    static std::set<ulong> ip_adresses;
    static short last_registered_id;
    std::vector<Endpoint *> devices;

public:
    RpiClient(ulong ip, short id);
    ~RpiClient();
    Endpoint *addDevice(uchar *codedDevice, size_t size);
    void printAllDevices();
    static bool isIpInUse(ulong ip);
    static bool isRegisteredId(short id);
    static RpiClient* getRpiClient(short id);
    static short getNextFreeId();

    ulong getIp() const;

    short getId() const;
};


#endif //SERVER_RPICLIENT_H
