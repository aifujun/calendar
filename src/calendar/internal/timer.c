
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdbool.h>

#include "timer.h"

typedef struct {
    char status;                /*!< 当前数据状态, 0-未使用, 1-正常, */
    char type;                  /*!< 节日类型 */
    short index;                /*!< 节日索引 */
    short tmp;                  /*!<  */
    short year;                 /*!< 节日年 */
    unsigned short month;       /*!< 节日月 */
    unsigned short day;         /*!< 节日日 */
    int reserved;               /*!< 保留 */
    time_t start;               /*!< 节日起始时间(时间戳) */
    char name[32];              /*!< 节日名称 */
    char description[512];      /*!< 节日简介 */
} FESTIVAL;


/** 自定义时间结构体 */
typedef struct time_info {
    int year;               /*!< 年份 */
    int month;              /*!< 月份 - 取值区间为[0,11] */
    int day;                /*!< 日期 - 取值区间为[1,31] */
    int hour;               /*!< 时 - 取值区间为[0,23] */
    int minute;             /*!< 分 - 取值区间为[0,59] */
    int second;             /*!< 秒 – 取值区间为[0,59] */
    int millisecond;        /*!< 毫秒 - 取值区间为[0,999] */
    int microsecond;        /*!< 微秒 - 取值区间为[0,999] */
    int dayOfWeek;          /*!< 星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 */
    int dayOrdinalOfYear;   /*!< 从每年的1月1日开始的天数 – 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推 */
    int isDST;              /*!< 夏令时标识符，实行夏令时的时候，为正值；不实行的进候为0；不生效时为负 */
    int UTC;                /*!< 指定了日期变更线东面时区中UTC东部时区或UTC西部时区 */
    const char *timeZone;   /*!< 当前时区的名字(与环境变量TZ有关) */
} V_TIME;


typedef struct {
    int id;                 /*!< id */
    time_t start;           /*!< 年份 */
    char timing;            /*!< 0-倒计时, 1-正计时 */
    int period;             /*!< 周期 */
    int frequency;          /*!< 周期数, 频次 */
} TIMER;


V_TIME _get_current_time() {
    V_TIME v_time;
    struct timeval tv;
    struct timezone tz;
    struct tm time_tm = {0};

    if (gettimeofday(&tv, &tz) != 0){
        printf("获取时间错误");
    }
    time_t now_time = tv.tv_sec;

#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&time_tm, &now_time);
#elif defined(__linux__)
    localtime_r(&now_time, &time_tm);
#endif

    v_time.year = time_tm.tm_year + 1900;
    v_time.month = time_tm.tm_mon +1;
    v_time.day = time_tm.tm_mday;
    v_time.hour = time_tm.tm_hour;
    v_time.minute = time_tm.tm_min;
    v_time.second = time_tm.tm_sec;
    v_time.dayOfWeek = time_tm.tm_wday;
    v_time.dayOrdinalOfYear = time_tm.tm_yday;
    v_time.isDST = time_tm.tm_isdst;
    v_time.millisecond = (int)(tv.tv_usec / 1000);
    v_time.microsecond = tv.tv_usec - v_time.millisecond * 1000;

    return v_time;
}


int update_timer(TIMER timer) {


    return 0;
}

int positive_timing() {

}

/**
 * 判断公历年是否是闰年
 *   [基准]回归年为365d5h48'46''
 * @param [in]year: 待判断的年份
 * @return [bool]bool: 如果是闰年就返回true 平年就返回false
 */
bool IsLeapYear(short year) {
    //对于大数值年份，能整除3200且能整除172800为闰年
    if ((year%3200 == 0) && (year%172800 == 0)) {
        return true;
    }
    //世纪年能被400整除的是闰年
    if (year%400 == 0) {
        return true;
    }
    //普通年能被4整除且不能被100整除的为闰年
    if ((year%4 == 0) && (year%100 != 0)) {
        return true;
    }

    return false;
}


V_TIME format_time(time_t start, time_t interval) {
    V_TIME v_time = {0};
    struct tm time_tm = {0};
    int interval_days = 0;
    int remain_seconds = 0;
    int mon_days = 0;

#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&time_tm, &start);
#elif defined(__linux__)
    localtime_r(&start, &time_tm);
#endif
    time_tm.tm_year += 1900;
    time_tm.tm_mon += 1;

    interval_days = (int)interval / 86400;
    remain_seconds = (int)interval % 86400;
    const unsigned short DayOfMonthCounter[2][13] = {
        {0,31,28,31,30,31,30,31,31,30,31,30,31},
        {0,31,29,31,30,31,30,31,31,30,31,30,31},
    };

    while ((mon_days = DayOfMonthCounter[IsLeapYear((short)time_tm.tm_year)][time_tm.tm_mon]) < interval_days) {
        time_tm.tm_mon += 1;
        if (time_tm.tm_mon > 12) {
            time_tm.tm_mon = 1;
            time_tm.tm_year += 1;
        }
        v_time.month += 1;
        if (v_time.month >= 12) {
            v_time.month = 0;
            v_time.year += 1;
        }
        interval_days -= mon_days;
    }
    v_time.day = interval_days;

    v_time.hour = remain_seconds / 3600;
    remain_seconds = remain_seconds % 3600;
    v_time.minute = remain_seconds / 60;
    v_time.second = remain_seconds % 60;
    return v_time;
}

/**
 * 创建一个定时器
 * @param timer: 定时器
 * @return
 */
int create_timer(TIMER timer) {
    int remain_seconds = 0;
    time_t interval;
    time_t tmp_time = timer.start;
    V_TIME f_time = {0};

    while ((time(NULL) - tmp_time) < timer.period) {
        tmp_time += timer.period;
        timer.frequency++;
    }

    interval = timer.timing == 0 ? (tmp_time + timer.period - time(NULL)) : (time(NULL) -  timer.start);
    f_time = format_time(timer.start, interval);

    while (1) {
        if (timer.period == 86400) {
            interval += 1;
            f_time.second += 1;
            if (f_time.second > 59) {
                f_time.second = 0;
                f_time.minute += 1;
                if (f_time.minute > 59) {
                    f_time.minute = 0;
                    f_time.hour += 1;
                    if (f_time.hour > 23) {
                        f_time.hour = 0;
                        f_time.day += 1;
                        if (f_time.day > 30) {

                        }
                    }
                }
            }

            printf("已经过去: %d天 %2d:%.2d:%.2d\n", days, hours, minutes, remain_seconds);
        }
        Sleep(1000);
    }
//    printf("%d-%d-%d %2d:%.2d:%.2d\n", vt.year, vt.month, vt.day, vt.hour, vt.minute, vt.second);
}


void *run_timer(void *args) {

}


int run()  {
    pthread_t t1;
    pthread_mutex_t lock;

    pthread_mutex_init(&lock, NULL);
    pthread_create(&t1, NULL, run_timer, NULL);

    pthread_join(t1, NULL);
}


int main(int argc, char **argv) {
    SetConsoleOutputCP(CP_UTF8);

    TIMER timer = {
            .id = 0,
            .start = 1525104000,  // 2018-05-01 00:00:00
            .timing = 0,
            .period = 86400,
    };

    create_timer(timer);

    return 0;
}
