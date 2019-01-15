//
// Created by s17kf on 09.12.18.
//

#include "Logger.h"
#include <cstdarg>
#include <ctime>
#include <thread>

FILE *log_file;
uint std_min_lv;

void logInit(const char *filepath, uint &stdout_min_level){
    std_min_lv = stdout_min_level;
    log_file = fopen(filepath, "w");
    if(log_file == nullptr){
        log(0, "failed to open log file %s", filepath);
    }
    else{
        log(5, "log file %s successfully opened", filepath);
    }
}

void log(int lv, const char *format, ...) {
    time_t rawtime;
    struct tm *timeinfo;
    rawtime = time(nullptr);
    timeinfo = localtime(&rawtime);
    uint time_str_len = 30;
    char time_str[time_str_len];
    strftime(time_str, time_str_len, "%a %F %T", timeinfo);
    va_list args, file_args;
    va_start(args, format);
    va_copy(file_args, args);
    char indent[lv+1];
    for(int i = 0; i < lv; ++i)
        indent[i] = '-';
    std::thread::id threadId = std::this_thread::get_id();
    if(log_file != nullptr){
        indent[lv] = '\0';
        fprintf(log_file, "%s[%d-%d] %s: ", indent, lv, threadId, time_str);
        vfprintf(log_file, format, file_args);
        fprintf(log_file, "\n");
        fflush(log_file);
    }
    if(lv <= std_min_lv){
        printf("%s[%d-%d] %s: ", indent, lv, threadId, time_str);
        vprintf(format, args);
        printf("\n");
        fflush(stdout);
    }
    va_end(args);
    va_end(file_args);
}

void logClose(){
    fclose(log_file);
}
