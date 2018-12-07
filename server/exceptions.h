//
// Created by s17kf on 06.12.18.
//

#ifndef SERVER_EXCEPTIONS_H
#define SERVER_EXCEPTIONS_H

#include <stdexcept>

class file_descriptor_error: public std::runtime_error
{
public:
    file_descriptor_error(std::string const& msg):
            std::runtime_error(msg)
    {}
};


#endif //SERVER_EXCEPTIONS_H
