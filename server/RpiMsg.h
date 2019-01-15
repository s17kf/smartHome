//
// Created by s17kf on 15.12.18.
//

#ifndef SERVER_RPIMSG_H
#define SERVER_RPIMSG_H

#include "Msg.h"

class RpiMsg : public Msg {
protected:
    enum msg_type{
        nack = 0x00,
        ack = 0x01,
        reg = 0x02,
        dev = 0x03,
        val = 0x06,
        setState = 0x07,
        end = 0x0f,
        exit = 0xff
    };

    RpiMsg(uchar *msg, ushort len) : Msg(msg, len) {}
    explicit RpiMsg(ushort len) : Msg(len) {}

public:
    static RpiMsg *receiveMsg(int socket);
};

class RAck : public RpiMsg{
public:
    ushort getVal();
    explicit RAck(uchar *buffer) : RpiMsg(buffer, 5) {}
    explicit RAck(ushort val);
    std::string toString() override;
};

class RNack : public RpiMsg{
public:
    ushort getVal();
    explicit RNack(uchar *buffer) : RpiMsg(buffer, 5) {}
    explicit RNack(ushort val);
    std::string toString() override;
};

class RReg : public RpiMsg{
public:
    explicit RReg(uchar *buffer) : RpiMsg(buffer, 3) {}
    RReg();
    std::string toString() override;
};

class RDev : public RpiMsg{
public:
    RDev(uchar *buffer, ushort len): RpiMsg(buffer, len) {}
    ushort getRpiId();
    ushort getDevId();
    uchar *getCodedDevice(){ return &buffer[Const::dev_type_index]; }
    ushort getCodedDevLen(){ return len - Const::dev_type_index; }
    std::string toString() override;
};

class REnd : public  RpiMsg{
public:
    explicit REnd(uchar *buffer): RpiMsg(buffer, 5) {}
    ushort getVal();
    std::string toString() override;
};

class RVal : public RpiMsg{
    friend class RpiMsg;
protected:
    enum val_type{
        int_v = 1,
        double_v = 2
    };
    static const ushort valIndex = 8;
    static const ushort valTypeIndex = 7;
    static const ushort devIdIndex = 5;
    RVal(uchar *buffer, ushort len) : RpiMsg(buffer, len) {}
public:
    ushort getRpiId();
    ushort getDevId();
};

class RValInt : public RVal{
public:
    explicit RValInt(uchar *buffer): RVal(buffer, 12) {}
    int getVal();
    std::string toString() override;
};

class RValDouble : public RVal{
public:
    explicit RValDouble(uchar *buffer) : RVal(buffer, 16) {}
    double getVal();
    std::string toString() override;
};

class RSetState : public RpiMsg{
public:
    static const ushort size = 7;

    RSetState(uchar *buffer) : RpiMsg(buffer, size) {}
    RSetState(ushort devId, ushort state);
    std::string toString() override;

protected:
    static const ushort devIdIndex = 3;
    static const ushort stateIndex = 5;
    ushort getDevId();
    ushort getState();
};

#endif //SERVER_RPIMSG_H
