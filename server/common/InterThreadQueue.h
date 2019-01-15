//
// Created by s17kf on 05.01.19.
//

#ifndef SERVER_INTERTHREADQUEUE_H
#define SERVER_INTERTHREADQUEUE_H


#include <mutex>
#include <queue>
#include "semaphore.h"

template <class T>
class InterThreadQueue {
    semaphore fillCount;
    semaphore emptyCount;
    std::queue<T> queue;
    int capacity;

public:
    InterThreadQueue(int capacity = 100) : capacity(capacity), fillCount(0),
        emptyCount(capacity){
//        fillCount = semaphore(0);
//        emptyCount = semaphore(capacity);
    }

    void put(T item){
        emptyCount.down();
        queue.push(item);
        fillCount.up();
    }

    T get(){
        fillCount.down();
        T item = queue.front();
        queue.pop();
        emptyCount.up();
        return item;
    }


};


#endif //SERVER_INTERTHREADQUEUE_H
