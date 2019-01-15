//
// Created by s17kf on 31.12.18.
//

#include <netinet/in.h>
#include <cstring>
#include "AndroidMsg.h"
#include "common/Logger.h"

AndroidMsg* AndroidMsg::receiveMsg(int socket) {
    ushort msgLen;
    auto readed = readUntillDone(socket, (uchar *)&msgLen, sizeof(msgLen));
    if(readed <= 0)
        return nullptr;
    msgLen = ntohs(msgLen);
    uchar msg[msgLen];
    memcpy(msg, &msgLen, sizeof(msgLen));
    readed = readUntillDone(socket, &msg[2], msgLen - 2);
    if(readed <= 0)
        return nullptr;
    switch(msg[2]){
        case ack:
            return new AAck(msg);
        case reg:
            return new AReg(msg);
        case getDevs:
            return new AGetDevs(msg);
        case setState:
            return new ASetState(msg);
        default:
            log(2, "unresolved msg type received");
            return nullptr;
    }
}

AAck::AAck(ushort val) : AndroidMsg(size){
    buffer[msgTypeIndex] = ack;
    val = htons(val);
    memcpy(&buffer[valIndex], &val, sizeof(val));
}

ushort AAck::getVal() {
    ushort val;
    memcpy(&val, &buffer[valIndex], sizeof(val));
    return ntohs(val);
}

std::string AAck::toString() {
    return "AAck: " + std::to_string(getVal());
}

ANack::ANack(ushort val) : AndroidMsg(size){
    buffer[msgTypeIndex] = nack;
    val = htons(val);
    memcpy(&buffer[valIndex], &val, sizeof(val));
}

ushort ANack::getVal() {
    ushort val;
    memcpy(&val, &buffer[valIndex], sizeof(val));
    return ntohs(val);
}

std::string ANack::toString() {
    return "ANack: " + std::to_string(getVal());
}

ASsid::ASsid(uint ssid) :AndroidMsg(size) {
    buffer[msgTypeIndex] = msg_type::ssid;
    ssid = htonl(ssid);
    memcpy(&buffer[ssidIndex], &ssid, sizeof(ssid));

}

uint ASsid::getSsid() {
    uint ssid;
    memcpy(&ssid, &buffer[ssidIndex], sizeof(ssid));
    return ntohl(ssid);
}

std::string ASsid::toString() {
    return "ASsid: " + std::to_string(getSsid());
}

std::string AReg::toString() {
    return "AReg";
}

std::string AGetDevs::toString() {
    return "AGetDevs";
}

std::string ADev::toString() {
    return "ADev";
}

AEnd::AEnd() : AndroidMsg(size) {
    buffer[msgTypeIndex] = end;
}

std::string AEnd::toString() {
    return "AEnd";
}

ushort ASetState::getRpiId() const {
    ushort rpiId;
    memcpy(&rpiId, &buffer[rpiIdIndex], sizeof(rpiId));
    return ntohs(rpiId);
}

ushort ASetState::getDevId() const {
    ushort devId;
    memcpy(&devId, &buffer[devIdIndex], sizeof(devId));
    return ntohs(devId);
}

ushort ASetState::getState() const {
    ushort state;
    memcpy(&state, &buffer[stateIndex], sizeof(state));
    return ntohs(state);
}
;
std::string ASetState::toString() {
    //TODO: print values
    return "ASetState";
}
