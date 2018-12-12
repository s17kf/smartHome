//
// Created by s17kf on 09.12.18.
//

#ifndef SERVER_LOGGER_H
#define SERVER_LOGGER_H

#include <iostream>
#include "../Const.h"


    void log(int lv, const char *format, ...);
    void logInit(const char *filepath, uint &stdout_min_level);
    void logClose();




#endif //SERVER_LOGGER_H
