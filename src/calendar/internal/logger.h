
#ifndef CAL_LOGGER_H
#define CAL_LOGGER_H

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>


#ifdef _WIN32
#   include <windows.h>
#else
#   include <sys/syscall.h>
#endif


/* ---------------- 日志打印宏 ---------------- */

#define LOG_NOTSET      0x00
#define LOG_DEBUG       0x01
#define LOG_INFO        0x02
#define LOG_WARNING     0x04
#define LOG_ERROR       0x08
#define LOG_CRITICAL    0x10

extern int logger_level;

char *get_current_time();
unsigned long get_thread_id();

#define logger(level, format, argv...) do{ \
    if(level & logger_level)               \
        printf("[%s][%s][PID=%d][TID=%lu][%s:%d][%s]" format "\n", \
               get_current_time(),         \
               #level,                     \
               getpid(),                   \
               get_thread_id(),            \
               __FILE__,                   \
               __LINE__,                   \
               __FUNCTION__,               \
               ##argv);                    \
} while(0)



#endif //CAL_LOGGER_H
