//
// Created by s17kf on 07.12.18.
//

#ifndef SERVER_PACKET_H
#define SERVER_PACKET_H


#include "RpiCommunication.h"

class Packet {
public:
    static const uchar nack = 0x00;
    static const uchar ack = 0x01;
    static const uchar reg = 0x02;     //registration
    static const uchar dev = 0x03;
    static const uchar all = 0xf0;
    static const uchar end = 0x0f;
    static const uchar exit = 0xff;

private:
    Packet() = default;
    Packet( const Packet &org) = default;
    Packet& operator =(const Packet &org) = default;
};

//const unsigned char Packet::reg = 0x01;
//const unsigned char Packet::exit = 0xff;

#endif //SERVER_PACKET_H
