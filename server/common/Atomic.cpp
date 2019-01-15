//
// Created by s17kf on 11.12.18.
//

#include "Atomic.h"

std::mutex Atomic::bool_mtx;

void Atomic::set(bool *to_set, const bool val){
    std::unique_lock<std::mutex> lck(bool_mtx);
    *to_set = val;
}

const bool Atomic::get(const bool *to_get){
    std::unique_lock<std::mutex> lck(bool_mtx);
    return *to_get;
}