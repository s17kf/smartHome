//
// Created by s17kf on 07.12.18.
//

#ifndef SERVER_CONST_H
#define SERVER_CONST_H

#include <iostream>

class Const {
public:
    static const ssize_t max_rpis = 5;
    static const __time_t answer_timeout = 5;

private:
    Const() = default;
    Const(const Const &org) = default;
    Const&operator = (const Const &org) = default;
};


#endif //SERVER_CONST_H
