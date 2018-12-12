//
// Created by s17kf on 07.12.18.
//

#ifndef SERVER_CONST_H
#define SERVER_CONST_H

#include <iostream>

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned long ulong;

class Const {
public:
    static const ssize_t max_rpis = 1;
    static const time_t answer_timeout = 5;
    static const uint rpi_id_index = 3;
    static const uint dev_key_index = 7;
    static const uint dev_type_index = 5;

    static std::string ipToString(ulong ip);

private:
    Const() = default;
    Const(const Const &org) = default;
    Const&operator = (const Const &org) = default;
};

enum DevTypeId{
    light = 1,
    shade = 2
};

#endif //SERVER_CONST_H
