//
// Created by s17kf on 10.12.18.
//

#include <cstring>
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
    short name_len;
    short pin;
    bool zero_triggered;
    uint index = 0;
    cpy(&key, bytes, sizeof(key), &index);
    cpy(&name_len, &bytes[index], sizeof(name_len), &index);
    uchar name[name_len];
    cpy(&name, &bytes[index], name_len, &index);
    cpy(&pin, &bytes[index], sizeof(pin), &index);
    cpy(&zero_triggered, &bytes[index], sizeof(zero_triggered), &index);
    //TODO: check if not bigger than expected size - throw ...
    return new Light(key, name, name_len, pin, zero_triggered);
}