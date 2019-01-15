//
// Created by s17kf on 16.12.18.
//

#include <arpa/inet.h>
#include "Thermometer.h"

Thermometer::Thermometer(ushort key, uchar *name, ushort name_len, ushort pin)
: Endpoint(key, name, name_len), pin(pin) {}

std::string Thermometer::toString() {
    return "Thermometer=[" + Endpoint::toString() + ", pin=" +
    std::to_string(pin) + "]";
}

Thermometer* Thermometer::generateFromBytes(uchar *bytes, ushort expected_len, short *ret_name_len) {
    short key;
    ushort name_len;
    short pin;
    uint index = 0;
    cpyFromBuffer(&key, bytes, sizeof(key), &index);
    key = ntohs(key);
    cpyFromBuffer(&name_len, &bytes[index], sizeof(name_len), &index);
    name_len = ntohs(name_len);
    uchar name[name_len];
    cpyFromBuffer(&name, &bytes[index], name_len, &index);
    cpyFromBuffer(&pin, &bytes[index], sizeof(pin), &index);
    pin = ntohs(pin);
    //TODO: check if not bigger than expected size - throw ...
    return new Thermometer(key, name, name_len, pin);
}

std::pair<uchar *, ushort> Thermometer::codeToAMsg() const {
    ushort msgLen = 6 + getNameLen();
    uchar *msg = new uchar[msgLen];
    ushort key = htons(getKey());
    ushort nameLen = htons(getNameLen());
    ushort pin = htons(this->pin);
    uint index = 0;
    cpyToBuffer(msg, &key, sizeof(key), &index);
    cpyToBuffer(msg, &nameLen, sizeof(nameLen), &index);
    cpyToBuffer(msg, getName(), getNameLen(), &index);
    cpyToBuffer(msg, &pin, sizeof(pin), &index);

    return std::pair<uchar *, ushort>(msg, msgLen);
}
