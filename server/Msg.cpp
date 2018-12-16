//
// Created by s17kf on 15.12.18.
//

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include "Msg.h"
#include "common/Logger.h"

ssize_t writeUntillDone(int socket, uchar *buffer, ssize_t len){
    ssize_t written = 0;

    while(written < len){
        auto i = write(socket, &buffer[written], len - written);
        if(i < 0){
            log(0, "Failed writing to socket: [Error %d] %s",
                    errno, strerror(errno));
            return i;
        } else if(i == 0){
            log(0, "client socket is closed");
            return 0;
        }
        written += len;
    }
    return written;
}

ssize_t readUntillDone(int socket, uchar *buffer, ssize_t len){
    ssize_t readed = 0;

    while(readed < len){
        auto i = read(socket, &buffer[readed], len - readed);
        if(i < 0){
            log(0, "Failed reading from socket: [Errno %d] %s",
                    errno, strerror(errno));
            return i;
        } else if(i == 0){
            log(0, "client socket is closed");
            return 0;
        }
        readed += len;
    }
    return readed;
}


Msg::Msg(ushort len) : len(len){
    buffer = new uchar[len];
    memcpy(buffer, &len, sizeof(len));
}

Msg::Msg(uchar *msg, ushort len) : len(len) {
    buffer = new uchar[len];
    memcpy(buffer, msg, len);
}

Msg::~Msg() {
    delete[] buffer;
}

Msg* Msg::receiveMsg(int socket) {
    ushort msgLen;
    auto readed = readUntillDone(socket, (uchar *)&msgLen, sizeof(msgLen));
    if(readed <= 0)
        return nullptr;
    uchar msg[msgLen];
    memcpy(msg, &msgLen, sizeof(msgLen));
    readed = readUntillDone(socket, &msg[2], msgLen - 2);
    if(readed <= 0)
        return nullptr;
    switch(msg[2]){
        case ack:
            return new Ack(msg);
        case nack:
            return new Nack(msg);
        case reg:
            return new Reg(msg);
        case dev:
            return new Dev(msg, msgLen);
        case end:
            return new End(msg);
        default:
            log(2, "unresolved msg type received");
            return nullptr;
    }
}

ssize_t Msg::send(int socket) const {
    return writeUntillDone(socket, buffer, ssize_t(len));
}

Ack::Ack(ushort val) : Msg(5) {
    buffer[2] = ack;
    memcpy(&buffer[3], &val, sizeof(val));
}

ushort Ack::getVal() {
    ushort val;
    memcpy(&val, &buffer[3], sizeof(val));
    return val;
}

std::string Ack::toString() {
    return "Ack: " + std::to_string(getVal());
}

Nack::Nack(ushort val) : Msg(5) {
    buffer[2] = nack;
    memcpy(&buffer[3], &val, sizeof(val));
}

ushort Nack::getVal() {
    ushort val;
    memcpy(&val, &buffer[3], sizeof(val));
    return val;
}

std::string Nack::toString() {
    return "Nack: " + std::to_string(getVal());
}

Reg::Reg() : Msg(3){
    buffer[2] = reg;
}

std::string Reg::toString() {
    return "Reg";
}

ushort Dev::getRpiId() {
    ushort id;
    memcpy(&id, &buffer[Const::rpi_id_index], sizeof(id));
    return id;
}

ushort Dev::getDevId() {
    ushort id;
    memcpy(&id, &buffer[Const::dev_key_index], sizeof(id));
    return id;
}

std::string Dev::toString() {
    return std::__cxx11::string();
}

ushort End::getVal() {
    ushort val;
    memcpy(&val, &buffer[3], sizeof(val));
    return val;
}

std::string End::toString() {
    return "End" + std::to_string(getVal());
}
