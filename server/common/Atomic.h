//
// Created by s17kf on 11.12.18.
//

#ifndef SERVER_ATOMIC_H
#define SERVER_ATOMIC_H


#include <mutex>

class Atomic {
    static std::mutex bool_mtx;
public:
    static void set(bool *to_set, const bool val);
    static const bool get(const bool *to_get);

private:
    Atomic() = default;
    Atomic(const Atomic &other) = default;
    Atomic &operator = (const Atomic &other) = default;

};


#endif //SERVER_ATOMIC_H
