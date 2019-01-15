#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <csignal>

#include "RpiCommunication.h"
#include "Packet.h"
#include "common/Logger.h"
#include "Server.h"

std::condition_variable cv;
std::mutex m;

//RpiCommunication *rpi_communication;
//std::thread rpi_communication_thread, rpi_answering_thread;
Server *server;

void signalHandler(int signum){
    log(0, "Interrupt with signal %d received", signum);
//    rpi_communication->stop();
//    {
//        std::lock_guard<std::mutex> lk(m);
////        ready = true;
//        std::cout << "main() signals data ready for processing\n";
//    }
//    cv.notify_all();
    server->stop();
//    if(rpi_communication_thread.joinable()) {
//        rpi_communication_thread.join();
//        log(0, "one thread joined, waiting for second");
//    }
//    delete rpi_communication;

    delete server;
    log(0, "ending program");
    logClose();
//    cv.notify_all();
    std::cout<<"last line"<<std::endl;
    exit(signum);
    std::cout<<"last line2"<<std::endl;
}

int main(){
    signal(SIGINT, signalHandler);
    std::string log_filepath = "../logs/file.log";
    uint log_stdout_min_lv = 5;
    logInit(log_filepath.c_str(), log_stdout_min_lv);
    log(0, "main thread id is %d", std::this_thread::get_id());
//    rpi_communication = new RpiCommunication(1234);
//    rpi_communication_thread = std::thread(&RpiCommunication::run, rpi_communication);
//    rpi_answering_thread = std::thread(&RpiCommunication::answering, rpi_communication);
    server = new Server(12345);
    server->start();
    log(0, "server work started in other thread");
//    std::unique_lock<std::mutex> lk(m);
//    cv.wait(lk);
//    std::cout<<"hi"<<std::endl;

//    sleep(30);

    while(true)
        sleep(10);
//    log(0, "communication with raspberry is going in other thread");
//    rpi_communication_thread.join();
//    log(0, "one thread joined, waiting for second");
//    rpi_answering_thread.join();
//    log(0, "other thread joined ending program");
//    logClose();
//    delete rpi_communication;
    return 0;
}