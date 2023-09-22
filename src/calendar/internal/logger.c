
#include "logger.h"

static char current_time[32] = {0};


typedef struct logging {
    char *time_fmt;

} LOGGING;


int logger_level = LOG_CRITICAL | LOG_ERROR| LOG_WARNING | LOG_INFO | LOG_DEBUG;


LOGGING logging = {
        .time_fmt = "%d-%02d-%02d %02d:%02d:%02d.%d"
};


char *get_current_time() {
    struct timeval tv;
    struct tm time_tm = {0};

    gettimeofday(&tv, NULL);
    time_t now_time = tv.tv_sec;

#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&time_tm, &now_time);
#elif defined(__linux__)
    localtime_r(&now_time, &time_tm);
#endif

    sprintf(current_time,
            logging.time_fmt,
            time_tm.tm_year + 1900,
            time_tm.tm_mon + 1,
            time_tm.tm_mday,
            time_tm.tm_hour,
            time_tm.tm_min,
            time_tm.tm_sec,
            tv.tv_usec);

    return current_time;
}



unsigned long get_thread_id() {
#ifdef WIN32
    return GetCurrentThreadId();
#else
    static __thread int id = 0;
    if (id != 0) return id;
    id = syscall(SYS_gettid);
    return id;
#endif
}
