//
// Created by s17kf on 10.12.18.
//

#include <arpa/inet.h>
#include "Shade.h"

Shade::Shade(short key, uchar *name, size_t name_len, short pinA, short pinB)
: Endpoint(key, name, name_len), pinA(pinA), pinB(pinB) {}

std::string Shade::toString() {
    return "Shade=[" + this->Endpoint::toString() + ", pinA=" +
            std::to_string(pinA) + ", pinB=" + std::to_string(pinB) + "]";
}

Shade* Shade::generateFromBytes(uchar *bytes, size_t expected_len, short *ret_name_len) {
    short key, name_len, pinA, pinB;
    uint index = 0;
    cpyFromBuffer(&key, bytes, sizeof(key), &index);
    key = ntohs(key);
    cpyFromBuffer(&name_len, &bytes[index], sizeof(name_len), &index);
    name_len = ntohs(name_len);
    uchar name[name_len];
    cpyFromBuffer(&name, &bytes[index], name_len, &index);
    cpyFromBuffer(&pinA, &bytes[index], sizeof(pinA), &index);
    pinA = ntohs(pinA);
    cpyFromBuffer(&pinB, &bytes[index], sizeof(pinB), &index);
    pinB = ntohs(pinB);
    //TODO: check if not bigger than expected size - throw ...
    return new Shade(key, name, name_len, pinA, pinB);
}

std::pair<uchar *, ushort> Shade::codeToAMsg() const {
    ushort msgLen = 8 + getNameLen();
    uchar *msg = new uchar[msgLen];
    short key = htons(getKey());
    short nameLen = htons(getNameLen());
    short pinA = htons(this->pinA);
    short pinB = htons(this->pinB);
    uint index = 0;
    cpyToBuffer(msg, &key, sizeof(key), &index);
    cpyToBuffer(msg, &nameLen, sizeof(nameLen), &index);
    cpyToBuffer(msg, getName(), getNameLen(), &index);
    cpyToBuffer(msg, &pinA, sizeof(pinA), &index);
    cpyToBuffer(msg, &pinB, sizeof(pinB), &index);

    return std::pair<uchar *, ushort>(msg, msgLen);
}
