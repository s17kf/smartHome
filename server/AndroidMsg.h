//
// Created by s17kf on 31.12.18.
//

#ifndef SERVER_ANDROIDMSG_H
#define SERVER_ANDROIDMSG_H


#include "Msg.h"

class AndroidMsg : public Msg{
public:
    enum msg_type{
        nack = 0x00,
        ack = 0x01,
        reg = 0x02,
        dev = 0x03,
        getDevs = 0x04,
        setState = 0x07,
        ssid = 0x0a,
        end = 0x0f,
        exit = 0xff,
    };

protected:
    static const uint msgTypeIndex = 2;

    AndroidMsg(uchar *msg, ushort len) : Msg(msg, len) {}
    explicit AndroidMsg(ushort len) : Msg(len) {}

public:
    static AndroidMsg *receiveMsg(int socket);
};

class AAck : public AndroidMsg{
private:
    static const uint size = 5;
    static const uint valIndex = 3;
public:
    ushort getVal();
    explicit AAck(uchar *buffer) : AndroidMsg(buffer, size) {}
    explicit AAck(ushort val);
    std::string toString() override;
};

class ANack : public AndroidMsg{
private:
    static const uint size = 5;
    static const uint valIndex = 3;
public:
    ushort getVal();
    explicit ANack(uchar *buffer) : AndroidMsg(buffer, size) {}
    explicit ANack(ushort val);
    std::string toString() override;
};

class ASsid : public AndroidMsg{
private:
    static const uint size = 7;
    static const uint ssidIndex = 3;
public:
    explicit ASsid(uchar *buffer) : AndroidMsg(buffer, size) {}
    explicit ASsid(uint ssid);
    uint getSsid();
    std::string toString() override;
};

class AReg : public AndroidMsg{
private:
    static const uint size = 3;
public:
    explicit AReg(uchar *buffer) : AndroidMsg(buffer, size) {}

    std::string toString() override;
};

class AGetDevs : public AndroidMsg{
private:
    static const uint size = 3;
public:
    explicit AGetDevs(uchar *buffer) : AndroidMsg(buffer, size) {}
    std::string toString() override;
};

class ADev : public AndroidMsg{
public:
    ADev(uchar *buffer, ushort len) : AndroidMsg(buffer, len) {}

    std::string toString() override;
};

class AEnd : public AndroidMsg{
private:
    static const uint size = 3;
public:
    AEnd();

    std::string toString() override;
};

class ASetState : public AndroidMsg{
    static const uint size = 9;
    static const uint rpiIdIndex = 3;
    static const uint devIdIndex = 5;
    static const uint stateIndex = 7;
public:
    ASetState(uchar * buffer) : AndroidMsg(buffer, size) {}
    ushort getRpiId() const;
    ushort getDevId() const;
    ushort getState() const;
    std::string toString() override;
};


#endif //SERVER_ANDROIDMSG_H
