#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <thread>
#include <csignal>

#include "RpiCommunication.h"
#include "Packet.h"
#include "common/Logger.h"

//using namespace std;

RpiCommunication *rpi_communication;
std::thread rpi_communication_thread, rpi_answering_thread;

void signalHandler(int signum){
    log(0, "Interupted with signal %d received", signum);
    rpi_communication->stop();
    if(rpi_communication_thread.joinable()) {
        rpi_communication_thread.join();
        log(0, "one thread joined, waiting for second");
    }
    if(rpi_answering_thread.joinable()) {
        rpi_answering_thread.join();
        log(0, "other thread joined ending program");
    }
    logClose();
    delete rpi_communication;

    exit(signum);
}

int main(){
    signal(SIGINT, signalHandler);
    std::string log_filepath = "../logs/file.log";
    uint log_stdout_min_lv = 5;
    logInit(log_filepath.c_str(), log_stdout_min_lv);

    rpi_communication = new RpiCommunication(1234);
    rpi_communication_thread = std::thread(&RpiCommunication::run, rpi_communication);
    rpi_answering_thread = std::thread(&RpiCommunication::answering, rpi_communication);

    log(0, "communication with raspberry is going in other thread");
    rpi_communication_thread.join();
    log(0, "one thread joined, waiting for second");
    rpi_answering_thread.join();
    log(0, "other thread joined ending program");
    logClose();
    delete rpi_communication;
    return 0;
}