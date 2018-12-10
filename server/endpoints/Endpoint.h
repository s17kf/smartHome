//
// Created by s17kf on 09.12.18.
//

#ifndef SERVER_ENDPOINT_H
#define SERVER_ENDPOINT_H

#include "../Const.h"

class Endpoint {
    int key;
    unsigned char *name;
    int name_len;

public:
    Endpoint(int key, uchar *name, size_t name_len);
    Endpoint(const Endpoint &org);
    ~Endpoint();

    virtual std::string toString();
    static Endpoint* generateFromBytes(uchar *bytes, size_t expected_len, short *name_len = nullptr);
    // TODO: endpoint virtual = 0

protected:
    static void cpy(void *dest, const void *src, size_t num, uint *index);
};


#endif //SERVER_ENDPOINT_H
