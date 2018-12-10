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

void Endpoint::print() {
    std::cout<<"key: "<<key<<"name: ";
    for(int i = 0; i < name_len; ++i){
        std::cout<<name[i];
    }
    std::cout<<std::endl;
}

Endpoint* Endpoint::generateFromBytes(uchar *bytes, size_t expected_len, short *ret_name_len) {
    short key;
    short name_len;
    memcpy(&key, bytes, sizeof(key));
    memcpy(&name_len, &bytes[sizeof(key)], sizeof(name_len));
    //TODO: if more than expected_size -  throw exception (broken msg)
    if(ret_name_len != nullptr){
        *ret_name_len = name_len;
    }
    uchar name[name_len];
    memcpy(name, &bytes[sizeof(key) + sizeof(name_len)], name_len);
    Endpoint *ret = new Endpoint(key, name, name_len);
    return ret;
}