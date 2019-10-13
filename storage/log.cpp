#include "log.h"

pthread_mutex_t* mutex_log;
const char LogLevelText[6][10]={"FATAL","ERROR","WARN","INFO","DEBUG","ALL"};
LOGLEVEL logLevelSet = INFO;//设置日志记录级别，高于该级别则输出,0为最高级,可在其他文件重新赋值

//创建共享的mutex, 实现多进程互斥访问log文件
void initMutex(void)
{
    int ret;
    //g_mutex一定要是进程间可以共享的，否则无法达到进程间互斥
    mutex_log=(pthread_mutex_t*)mmap(NULL,sizeof(pthread_mutex_t),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    if( MAP_FAILED==mutex_log )
    {
        perror("mmap");
        exit(1);
    }
    
    //设置attr的属性
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    //一定要设置为PTHREAD_PROCESS_SHARED
    //具体可以参考http://blog.chinaunix.net/u/22935/showart_340408.html
    ret = pthread_mutexattr_setpshared(&attr,PTHREAD_PROCESS_SHARED);
    if( ret!=0 )
    {
        perror("init_mutex pthread_mutexattr_setpshared");
        exit(1);
    }
    pthread_mutex_init(mutex_log, &attr);
}

//设定时间
static char * settime(char * time_s)
{
    time_t timer=time(NULL);
    strftime(time_s, 20, "%Y-%m-%d %H:%M:%S",localtime(&timer));
    return time_s;
}

//打印
static int PrintfLog(LOGLEVEL logLevel, const char *string)
{
    FILE * fd = NULL;
    char headStr[1024];
    char tmp[256];

    //使用追加方式打开文件
    fd = fopen(logFilePath,"a+");
    if(fd == NULL)
    {
        return -1;
    }
    
    memset(headStr, 0, sizeof(headStr));
    memset(tmp, 0,sizeof(tmp));
    
    sprintf(tmp, "%s-[pid=%d][", LogLevelText[logLevel], getpid());
    strcpy(headStr, tmp);
    
    memset(tmp, 0, sizeof(tmp));
    settime(tmp);
    strcat(headStr, tmp);

    fprintf(fd, "%s]:%s\n", headStr, string);
    fclose(fd);
}

/*
  *日志写入
  * */
void LogWrite(LOGLEVEL logLevel, const char *string)
{
    //判断是否需要写LOG
    if (logLevel > logLevelSet)
    {
        return;
    }
    printf("%s\n",string);
    //printf("pthread_mutex_lock begin. \n");
    int ret = pthread_mutex_lock(mutex_log); //lock. //[为支持多线程需要加锁] 
    if( ret!=0 )
    {
        perror("LogFile pthread_mutex_lock");  
    }
    //printf("logwrite begin. \n");
    //打印日志信息
    PrintfLog(logLevel, string);

    ret = pthread_mutex_unlock(mutex_log); //unlock. //[为支持多线程需要加锁] 
    if( ret!=0 )
    {
        perror("LogFile pthread_mutex_unlock");  
    }
}

/*
 * 对外函数接口 log_init log_Error log_Warn log_Info log_Debug
 */
void log_init(void)
{
    initMutex();//初始化多进程信号量，实现log文件互斥访问。为以后多进程做准备
    printf("log_init done.\n");
}

void log_Error(const char *string)
{
    LogWrite(ERROR, string);
}

void log_Warn(const char *string)
{
    LogWrite(WARN, string);
}

void log_Info(const char *string)
{
    LogWrite(INFO, string);
}

void log_Debug(const char *string)
{
    LogWrite(DEBUG, string);
}