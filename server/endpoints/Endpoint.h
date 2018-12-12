//
// Created by s17kf on 09.12.18.
//

#ifndef SERVER_ENDPOINT_H
#define SERVER_ENDPOINT_H

#include "../Const.h"

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

    short getName_len() const;

    virtual std::string toString();
//    virtual Endpoint* generateFromBytes(uchar *bytes, size_t expected_len, short *name_len = nullptr) = 0;
    // TODO: endpoint virtual = 0

protected:
    static void cpy(void *dest, const void *src, size_t num, uint *index);
};


#endif //SERVER_ENDPOINT_H
