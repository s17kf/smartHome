//
// Created by s17kf on 10.12.18.
//

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
    cpy(&key, bytes, sizeof(key), &index);
    cpy(&name_len, &bytes[index], sizeof(name_len), &index);
    uchar name[name_len];
    cpy(&name, &bytes[index], name_len, &index);
    cpy(&pinA, &bytes[index], sizeof(pinA), &index);
    cpy(&pinB, &bytes[index], sizeof(pinB), &index);
    //TODO: check if not bigger than expected size - throw ...
    return new Shade(key, name, name_len, pinA, pinB);
}