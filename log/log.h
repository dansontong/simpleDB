#ifndef LOG_H_INCLUDE
#define LOG_H_INCLUDE

// #include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

// log文件路径
static char logFilePath[50] = "../data/DBrun.log";

typedef enum
{
    FATAL=0,
    ERROR=1,
    WARN=2,
    INFO=3,
    DEBUG=4,
    ALL=5
}LOGLEVEL;

/*
 * 对外函数接口 log_init log_Error log_Warn log_Info log_Debug Log
 */
void log_init(void);

void log_init(const char *newLogFilePath);

void log_Error(const char *string);

void log_Warn(const char *string);

void log_Info(const char *string);

void log_Debug(const char *string);

void Log(LOGLEVEL logLevel, const char *fmt, ...);

#endif