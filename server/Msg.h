//
// Created by s17kf on 30.12.18.
//

#ifndef SERVER_MSG_H
#define SERVER_MSG_H

#include "Const.h"

class Msg {
protected:
    ushort len;
    uchar *buffer;

    Msg(uchar *msg, ushort len);
    explicit Msg(ushort len);

    static ssize_t writeUntillDone(int socket, uchar *buffer, ssize_t len);
    static ssize_t readUntillDone(int socket, uchar *buffer, ssize_t len);

public:
    virtual ~Msg();
    virtual ssize_t send(int socket) const;
    virtual ushort getLen() const{
        return len;
    }
    virtual std::string toString() = 0;
    const uchar* getBytes() const{
        return buffer;
    }

};


#endif //SERVER_MSG_H
