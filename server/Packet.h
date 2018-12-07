//
// Created by s17kf on 07.12.18.
//

#ifndef SERVER_PACKET_H
#define SERVER_PACKET_H


class Packet {
public:
    static const unsigned char nack = 0x00;
    static const unsigned char ack = 0x01;
    static const unsigned char reg = 0x02;     //registration
    static const unsigned char exit = 0xff;

private:
    Packet() = default;
    Packet( const Packet &org) = default;
    Packet& operator =(const Packet &org) = default;
};

//const unsigned char Packet::reg = 0x01;
//const unsigned char Packet::exit = 0xff;

#endif //SERVER_PACKET_H
