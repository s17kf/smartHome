//
// Created by s17kf on 15.12.18.
//

#ifndef SERVER_MSG_H
#define SERVER_MSG_H

#include "Const.h"

class Msg {
protected:
    enum msg_type{
        nack = 0x00,
        ack = 0x01,
        reg = 0x02,
        dev = 0x03,
        val = 0x06,
        end = 0x0f
    };
    uchar *buffer;
    ushort len;

    Msg(uchar *msg, ushort len);
    Msg(ushort len);

public:
    static Msg *receiveMsg(int socket);
    virtual ~Msg();
    virtual ssize_t send(int socket) const;
    virtual ushort getLen() const{
        return len;
    }
    virtual std::string toString() = 0;
};

class Ack : public Msg{
public:
    ushort getVal();
    Ack(uchar *buffer) : Msg(buffer, 5) {}
    Ack(ushort val);
    std::string toString() override;
};

class Nack : public Msg{
public:
    ushort getVal();
    Nack(uchar *buffer) : Msg(buffer, 5) {}
    Nack(ushort val);
    std::string toString() override;
};

class Reg : public Msg{
public:
    Reg(uchar *buffer) : Msg(buffer, 3) {}
    Reg();
    std::string toString() override;
};

class Dev : public Msg{
public:
    Dev(uchar *buffer, ushort len): Msg(buffer, len) {}
    ushort getRpiId();
    ushort getDevId();
    uchar *getCodedDevice(){ return &buffer[Const::dev_type_index]; }
    ushort getCodedDevLen(){ return len - Const::dev_type_index; }
    std::string toString() override;
};

class End : public  Msg{
public:
    End(uchar *buffer): Msg(buffer, 5) {}
    ushort getVal();
    std::string toString() override;
};

class Val : public Msg{
    friend class Msg;
protected:
    enum val_type{
        int_v = 1,
        double_v = 2
    };
    static const ushort valIndex = 8;
    static const ushort valTypeIndex = 7;
    static const ushort devIdIndex = 5;
    Val(uchar *buffer, ushort len) : Msg(buffer, len) {}
public:
    ushort getRpiId();
    ushort getDevId();
};

class ValInt : public Val{
public:
    ValInt(uchar *buffer): Val(buffer, 12) {}
    int getVal();
    std::string toString() override;
};

class ValDouble : public Val{
public:
    ValDouble(uchar *buffer) : Val(buffer, 16) {}
    double getVal();
    std::string toString() override;
};


#endif //SERVER_MSG_H
