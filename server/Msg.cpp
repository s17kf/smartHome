//
// Created by s17kf on 30.12.18.
//

#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include "Msg.h"

#include "common/Logger.h"

ssize_t Msg::writeUntillDone(int socket, uchar *buffer, ssize_t len){
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

ssize_t Msg::readUntillDone(int socket, uchar *buffer, ssize_t len){
    ssize_t readed = 0;

    while(readed < len){
        auto i = read(socket, &buffer[readed], len - readed);
        if(i < 0){
            log(5, "Failed reading from socket: [Errno %d] %s",
                errno, strerror(errno));
            return i;
        } else if(i == 0){
            log(5, "client socket is closed");
            return 0;
        }
        readed += len;
    }
    return readed;
}

Msg::Msg(ushort len) : len(len){
    buffer = new uchar[len];
    len = htons(len);
    memcpy(buffer, &len, sizeof(len));
}

Msg::Msg(uchar *msg, ushort len) : len(len){
    buffer = new uchar[len];
    memcpy(buffer, msg, len);
}

Msg::~Msg() {
    delete buffer;
}

ssize_t Msg::send(int socket) const {
    return writeUntillDone(socket, buffer, ssize_t(len));
}