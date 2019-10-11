#ifndef LOGGING_H
#define LOGGING_H

// #include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>

// log文件路径
#define logFilePath "./ps_com_log.log"

typedef enum{
    FATAL=0,
    ERROR=1,
    WARN=2,
    INFO=3,
    DEBUG=4,
    ALL=5
}LOGLEVEL;

const static char LogLevelText[6][10]={"FATAL","ERROR","WARN","INFO","DEBUG","ALL"};

pthread_mutex_t* g_mutex;
static LOGLEVEL logLevelSet = INFO;//设置日志记录级别，高于该级别则输出,0为最高级

//设定时间
static char * settime(char * time_s){
    time_t timer=time(NULL);
    strftime(time_s, 20, "%Y-%m-%d %H:%M:%S",localtime(&timer));
    return time_s;
}

//创建共享的mutex, 实现多进程互斥访问log文件
void init_mutex(void)
{
    int ret;
    //g_mutex一定要是进程间可以共享的，否则无法达到进程间互斥
    g_mutex=(pthread_mutex_t*)mmap(NULL, sizeof(pthread_mutex_t), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    if( MAP_FAILED==g_mutex )
    {
        perror("mmap");
        exit(1);
    }
    
    //设置attr的属性
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    //一定要设置为PTHREAD_PROCESS_SHARED
    //具体可以参考http://blog.chinaunix.net/u/22935/showart_340408.html
    ret=pthread_mutexattr_setpshared(&attr,PTHREAD_PROCESS_SHARED);
    if( ret!=0 )
    {
        perror("init_mutex pthread_mutexattr_setpshared");
        exit(1);
    }
    pthread_mutex_init(g_mutex, &attr);
}

/*
 *打印
 * */
static int PrintfLog(LOGLEVEL logLevel, char *string){
    FILE * fd = NULL;
    char headStr[1024];
    char tmp[256];

    //使用追加方式打开文件
    fd = fopen(logFilePath,"a+");
    if(fd == NULL){
        return -1;
    }
    
    memset(headStr, 0, sizeof(headStr));
    memset(tmp, 0,sizeof(tmp));
    
    sprintf(tmp, "***[pid=%d]:[", getpid());
    strcpy(headStr, tmp);
    
    memset(tmp, 0, sizeof(tmp));
    settime(tmp);
    strcat(headStr, tmp);

    fprintf(fd, "]%s-%s:%s\n", headStr, LogLevelText[logLevel], string);
    fclose(fd);
}

/*
  *日志写入
  * */
void LogWrite(LOGLEVEL logLevel, char *string)
{
    //判断是否需要写LOG
    if (logLevel > logLevelSet)
    {
        return;
    }
    ret = pthread_mutex_lock(&mutex_log); //lock. //[为支持多线程需要加锁] 
    if( ret!=0 )
    {
        perror("LogFile pthread_mutex_lock");  
    }
    //打印日志信息
    PrintfLog(logText, string);

    ret = pthread_mutex_unlock(&mutex_log); //unlock. //[为支持多线程需要加锁] 
    if( ret!=0 )
    {
        perror("LogFile pthread_mutex_unlock");  
    }
}

void logError(char *string)
{
    LogWrite(ERROR, str)
}

void logWarn(char *string)
{
    LogWrite(WARN, str)
}

void logInfo(char *string)
{
    LogWrite(INFO, str)
}

void logDebug(char *string)
{
    LogWrite(DEBUG, str)
}

#endif