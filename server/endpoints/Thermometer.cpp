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
    cpy(&key, bytes, sizeof(key), &index);
    key = ntohs(key);
    cpy(&name_len, &bytes[index], sizeof(name_len), &index);
    name_len = ntohs(name_len);
    uchar name[name_len];
    cpy(&name, &bytes[index], name_len, &index);
    cpy(&pin, &bytes[index], sizeof(pin), &index);
    pin = ntohs(pin);
    //TODO: check if not bigger than expected size - throw ...
    return new Thermometer(key, name, name_len, pin);
}