//
// Created by s17kf on 09.12.18.
//

#ifndef SERVER_ENDPOINT_H
#define SERVER_ENDPOINT_H

#include "../Const.h"
#include "../AndroidMsg.h"

class Endpoint {
    short key;
    unsigned char *name;
    short name_len;

public:
    Endpoint(int key, uchar *name, size_t name_len);
    Endpoint(const Endpoint &org);
    ~Endpoint();

    short getKey() const;

    unsigned char *getName() const;

    short getNameLen() const;

    virtual std::string toString();
//    virtual Endpoint* generateFromBytes(uchar *bytes, size_t expected_len, short *name_len = nullptr) = 0;
    // TODO: endpoint virtual = 0

    virtual std::pair<uchar *, ushort> codeToAMsg() const = 0;
    virtual ushort getDevTypeId() = 0;
public:
    static void cpyFromBuffer(void *dest, const void *src, size_t num, uint *index);
    // TODO: index using inside function (pass just buffer pointer not element pointer
    //  as in cpyToBuffer)
    static void cpyToBuffer(void *dest, const void *src, size_t num, uint *index);

};


#endif //SERVER_ENDPOINT_H
