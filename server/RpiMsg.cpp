//
// Created by s17kf on 15.12.18.
//

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <arpa/inet.h>
#include "RpiMsg.h"

#include "common/Logger.h"
#include "common/network.h"


RpiMsg* RpiMsg::receiveMsg(int socket) {
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
            return new RAck(msg);
        case nack:
            return new RNack(msg);
        case reg:
            return new RReg(msg);
        case dev:
            return new RDev(msg, msgLen);
        case end:
            return new REnd(msg);
        case val:
            switch(msg[RVal::valTypeIndex]){
                case RVal::int_v:
                    return new RValInt(msg);
                case RVal::double_v:
                    return new RValDouble(msg);
                default:
                    log(2, "unresolved val type in val msg received");
                    return nullptr;
            }
        default:
            log(2, "unresolved msg type received");
            return nullptr;
    }
}

RAck::RAck(ushort val) : RpiMsg(5) {
    buffer[2] = ack;
    val = htons(val);
    memcpy(&buffer[3], &val, sizeof(val));
}

ushort RAck::getVal() {
    ushort val;
    memcpy(&val, &buffer[3], sizeof(val));
    return ntohs(val);
}

std::string RAck::toString() {
    return "RAck: " + std::to_string(getVal());
}

RNack::RNack(ushort val) : RpiMsg(5) {
    buffer[2] = nack;
    val = htons(val);
    memcpy(&buffer[3], &val, sizeof(val));
}

ushort RNack::getVal() {
    ushort val;
    memcpy(&val, &buffer[3], sizeof(val));
    return ntohs(val);
}

std::string RNack::toString() {
    return "RNack: " + std::to_string(getVal());
}

RReg::RReg() : RpiMsg(3){
    buffer[2] = reg;
}

std::string RReg::toString() {
    return "RReg";
}

ushort RDev::getRpiId() {
    ushort id;
    memcpy(&id, &buffer[Const::rpi_id_index], sizeof(id));
    return ntohs(id);
}

ushort RDev::getDevId() {
    ushort id;
    memcpy(&id, &buffer[Const::dev_key_index], sizeof(id));
    return ntohs(id);
}

std::string RDev::toString() {
    return "RDev ..."; //std::__cxx11::string();
}

ushort REnd::getVal() {
    ushort val;
    memcpy(&val, &buffer[3], sizeof(val));
    return ntohs(val);
}

std::string REnd::toString() {
    return "REnd" + std::to_string(getVal());
}

ushort RVal::getRpiId() {
    ushort id;
    memcpy(&id, &buffer[Const::rpi_id_index], sizeof(id));
    return ntohs(id);
}

ushort RVal::getDevId() {
    ushort id;
    memcpy(&id, &buffer[devIdIndex], sizeof(id));
    return ntohs(id);
}
;
int RValInt::getVal() {
    int val;
    memcpy(&val, &buffer[valIndex], sizeof(val));
    return ntohl(val);
}

std::string RValInt::toString() {
    return "Int val=" + std::to_string(getVal());
}

double RValDouble::getVal() {
    double val = *((double *)&buffer[valIndex]);
    ntohDouble(&val);
    return val;
}

std::string RValDouble::toString() {
    return "Double val=" + std::to_string(getVal());
}

RSetState::RSetState(ushort devId, ushort state) : RpiMsg(size) {
    buffer[2] = setState;
    devId = htons(devId);
    state = htons(state);
    memcpy(&buffer[devIdIndex], &devId, sizeof(devId));
    memcpy(&buffer[stateIndex], &state, sizeof(state));
}

std::string RSetState::toString() {
    return "RSetState: dev=" + std::to_string(getDevId()) +
    ", state=" + std::to_string(getState());
}

ushort RSetState::getDevId() {
    ushort devId;
    memcpy(&devId, &buffer[devIdIndex], sizeof(devId));
    return ntohs(devId);
}

ushort RSetState::getState() {
    ushort state;
    memcpy(&state, &buffer[stateIndex], sizeof(state));
    return ntohs(state);
}