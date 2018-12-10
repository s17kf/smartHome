//
// Created by s17kf on 10.12.18.
//

#ifndef SERVER_SHADE_H
#define SERVER_SHADE_H

#include "Endpoint.h"

class Shade : public Endpoint{
    short pinA;
    short pinB;

public:
    Shade(short key, uchar *name, size_t name_len, short pinA, short pinB);
    ~Shade() = default;

    std::string toString();
    static Shade* generateFromBytes(uchar *bytes, size_t expected_len, short *ret_name_len = nullptr);

};


#endif //SERVER_SHADE_H
