//
// Created by s17kf on 09.12.18.
//

#ifndef SERVER_RPICLIENT_H
#define SERVER_RPICLIENT_H

#include <vector>
#include "RpiCommunication.h"
#include "endpoints/Endpoint.h"
#include "common/Logger.h"
#include <mutex>

class RpiClient {
    ulong ip;
    short id;
    time_t lastActivityTime;
    std::mutex last_activity_time_mtx;
    std::vector<Endpoint *> devices;

    /**
     * registered_rpis = map< ip_adress, RpiClient *>
     */
    static std::map<short, RpiClient *> registeredRpis;
    static std::set<ulong> ipAddresses;
    static short lastRegisteredId;

public:
    RpiClient(ulong ip, short id);
    ~RpiClient();
    Endpoint *addDevice(uchar *codedDevice, size_t size);
    void printAllDevices();
    static bool isIpInUse(ulong ip);
    static bool isRegisteredId(short id);
    static RpiClient* getRpiClient(short id);
    static short getNextFreeId();

    ulong getIp() const {
        return ip;
    }

    short getId() const {
        return id;
    }

    void setLastActivityTime(time_t last_activity_time = time(nullptr)) {
        std::unique_lock<std::mutex> lck(last_activity_time_mtx);
        RpiClient::lastActivityTime = last_activity_time;
    }

    time_t getLastActivityTime() {
        std::unique_lock<std::mutex> lck(last_activity_time_mtx);
        return lastActivityTime;
    }
};


#endif //SERVER_RPICLIENT_H
