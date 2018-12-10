//
// Created by s17kf on 10.12.18.
//

#ifndef SERVER_LIGHT_H
#define SERVER_LIGHT_H

#include "Endpoint.h"

class Light : public Endpoint{
    short pin;
    bool zero_triggered;

public:
    Light(short key, uchar* name, size_t name_len, short pin, bool zero_trig);
    ~Light() = default;

    std::string toString();
    static Light* generateFromBytes(uchar *bytes, size_t expected_len, short *ret_name_len = nullptr);

};


#endif //SERVER_LIGHT_H
