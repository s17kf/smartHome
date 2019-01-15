//
// Created by s17kf on 05.01.19.
//

#ifndef SERVER_SEMAPHORE_H
#define SERVER_SEMAPHORE_H

#include <mutex>
#include <condition_variable>
//using namespace std;

class semaphore{
    std::mutex mtx;
    std::condition_variable cv;
    int count;

public:

  semaphore(int count_ = 0) : count(count_) {}

  void up(){
    std::unique_lock<std::mutex> lck(mtx);
    ++count;
    cv.notify_one();
  }

  void down(){
    std::unique_lock<std::mutex> lck(mtx);
    while(count == 0){
      cv.wait(lck);
    }
    --count;
  }

};


#endif //SERVER_SEMAPHORE_H
