//
// Created by s17kf on 10.12.18.
//

#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Light.h"

Light::Light(short key, uchar *name, size_t name_len, short pin, bool zero_trig)
: Endpoint(key, name, name_len) {
    this->pin = pin;
    this->zero_triggered = zero_trig;
}

std::string Light::toString() {
    std::string ret = "Light=[";
    ret += this->Endpoint::toString();
    ret += ", pin=" + std::to_string(pin) + ", 0 triggered=" +
            std::to_string(zero_triggered) + "]";
    return ret;
}

Light* Light::generateFromBytes(uchar *bytes, size_t expected_len, short *ret_name_len) {
    short key;
    ushort name_len;
    short pin;
    bool zero_triggered;
    uint index = 0;
    cpyFromBuffer(&key, bytes, sizeof(key), &index);
    key = ntohs(key);
    cpyFromBuffer(&name_len, &bytes[index], sizeof(name_len), &index);
    name_len = ntohs(name_len);
    uchar name[name_len];
    cpyFromBuffer(&name, &bytes[index], name_len, &index);
    cpyFromBuffer(&pin, &bytes[index], sizeof(pin), &index);
    pin = ntohs(pin);
    cpyFromBuffer(&zero_triggered, &bytes[index], sizeof(zero_triggered), &index);
    //TODO: check if not bigger than expected size - throw ...
    return new Light(key, name, name_len, pin, zero_triggered);
}

std::pair<uchar *, ushort> Light::codeToAMsg() const {
    ushort msgLen = 7 + getNameLen();
    uchar *msg = new uchar[msgLen];
    short key = htons(getKey());
    short nameLen = htons(getNameLen());
    short pin = htons(this->pin);
    uint index = 0;
    cpyToBuffer(msg, &key, sizeof(key), &index);
    cpyToBuffer(msg, &nameLen, sizeof(nameLen), &index);
    cpyToBuffer(msg, getName(), getNameLen(), &index);
    cpyToBuffer(msg, &pin, sizeof(pin), &index);
    cpyToBuffer(msg, &zero_triggered, sizeof(zero_triggered), &index);

    return std::pair<uchar *, ushort>(msg, msgLen);
}
