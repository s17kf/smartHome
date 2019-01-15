//
// Created by s17kf on 09.12.18.
//

#include <cstring>
#include "Endpoint.h"

Endpoint::Endpoint(int key, uchar *name, size_t name_len) {
    this->key = key;
    this->name = new uchar[name_len];
    memcpy(this->name, name, name_len);
    this->name_len = name_len;
}

Endpoint::Endpoint(const Endpoint &org) {
    key = org.key;
    name = new uchar[org.name_len];
    memcpy(name, org.name, org.name_len);
    name_len = org.name_len;
}

Endpoint::~Endpoint() {
    delete[] name;
}

std::string Endpoint::toString() {
    std::string ret = "key=" + std::to_string(key) + ", name=\"";
    for(int i = 0; i < name_len; ++i){
        ret += name[i];
    }
    ret += '\"';
    return ret;
}

void Endpoint::cpyFromBuffer(void *dest, const void *src, size_t num, uint *index) {
    memcpy(dest, src, num);
    // TODO: modify usages to pass to next comment line
//    memcpy(dest, (uchar *)src + *index, num);
    *index += num;
}

void Endpoint::cpyToBuffer(void *dest, const void *src, size_t num, uint *index) {
    memcpy((uchar *)dest + *index, src, num);
    *index += num;
}
short Endpoint::getKey() const {
    return key;
}

unsigned char *Endpoint::getName() const {
    return name;
}

short Endpoint::getNameLen() const {
    return name_len;
}
