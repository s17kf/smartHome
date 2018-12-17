//
// Created by s17kf on 16.12.18.
//

#ifndef SERVER_THERMOMETER_H
#define SERVER_THERMOMETER_H


#include "Endpoint.h"

class Thermometer : public Endpoint{
    ushort pin;

public:
    Thermometer(ushort key, uchar* name, ushort name_len, ushort pin);
    ~Thermometer() = default;

    std::string toString();
    static Thermometer *generateFromBytes(uchar *bytes, ushort expected_len, short *ret_name_len = nullptr);

};


#endif //SERVER_THERMOMETER_H
