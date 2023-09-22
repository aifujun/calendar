#include <stdio.h>
#include <windows.h>
#include <sys/time.h>

#include "src/calendar/calendar.h"
#include "src/calendar/internal/festival.h"
#include "src/calendar/internal/anniversary.h"


/**
 * 显示当前时间
 */
void GetCurTime() {
    FC_TIME now_time = {0};
    struct timeval tv = {0};
    struct timezone tz = {0};
    struct tm time_tm = {0};

    if (gettimeofday(&tv, &tz) != 0) {
        printf("获取时间错误");
    }
    time_t now_t = tv.tv_sec;

#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&time_tm, &now_t);
#elif defined(__linux__)
    localtime_r(&now_t, &time_tm);
#endif

    now_time.year = time_tm.tm_year + 1900;
    now_time.month = time_tm.tm_mon +1;
    now_time.day = time_tm.tm_mday;
    now_time.hour = time_tm.tm_hour;
    now_time.minute = time_tm.tm_min;
    now_time.second = time_tm.tm_sec;
    now_time.dayOfWeek = time_tm.tm_wday;
    now_time.dayOrdinalOfYear = time_tm.tm_yday;
    now_time.isDST = time_tm.tm_isdst;
    now_time.millisecond = (int)(tv.tv_usec / 1000);
    now_time.microsecond = tv.tv_usec - now_time.millisecond * 1000;

    printf("%d-%d-%d %2d:%.2d:%.2d %s\n", now_time.year, now_time.month, now_time.day, now_time.hour, now_time.minute, now_time.second, WeekNameCN[now_time.dayOfWeek]);
}

/**
 * 万年历主界面
 * @return
 */
int CalendarMainWidget(){
    int mode = 0;
    int error_times = 0;

    system("color f0");             //设置为白底

    while (error_times < 5){
        printf("超级万年历(1840～2100)\n\n");
        //ShowNowTime();
        printf("\t\t您的输入：");
        scanf("%d",&mode);
        error_times += 1;
        system("cls");
    }

    return 0;
}


int run_calendar() {
    DATE_INFO date_info = {0};
    get_date_info(&date_info, 2023, 9, 22, GregorianCalendar, 0);

    printf("公元 %d年%.2d月%.2d日 %s 第%.2d周 今年第%d天\n农历: %s%s[%s]年 %s%s%s %s%s月 %s%s日\n",
           date_info.gregorian.year,
           date_info.gregorian.month,
           date_info.gregorian.day,
           WeekNameCN[date_info.gregorian.dayOfWeek],
           date_info.gregorian.weekOfYear,
           date_info.gregorian.dayOrdinalOfYear,
           TianGan[date_info.lunar.ganZhiYear >> 4],
           DiZhi[date_info.lunar.ganZhiYear & GANZHI_MASK],
           ShengXiao[date_info.lunar.ganZhiYear & GANZHI_MASK],
           date_info.lunar.isLeapMonth ? "「闰」" : "",
           LunarMouthName[date_info.lunar.month - 1],
           LunarDayName[date_info.lunar.day - 1],
           TianGan[date_info.lunar.ganZhiMon >> 4],
           DiZhi[date_info.lunar.ganZhiMon & GANZHI_MASK],
           TianGan[date_info.lunar.ganZhiDay >> 4],
           DiZhi[date_info.lunar.ganZhiDay & GANZHI_MASK]
    );

    return 0;
}


void Initialize() {
    // SetConsoleOutputCP(CP_UTF8);
    system("@echo off&chcp 65001>nul");
}


int test_fest() {
    char *file = "C:\\Users\\fujun\\Desktop\\develop\\Calendar\\data\\public\\festival.dat";

    FESTIVAL fest[4] = {
            {
                    .index = 0,
                    .type = 0,
                    .year = 0,
                    .month = 1,
                    .day = 1,
                    .name = "春节",
                    .description = "春节，是以农历计算的中国传统新年，亦称新春、正旦、正月朔日，其庆祝活动又俗称过年、度岁等，是汉族四大传统节日之一。农历正月初一为一年之首，故称“岁首”，又称“年节”、“元旦”、“元正”、“元朔”、“元日”、“正旦”、“新正”、“新春”、“履新”、“开正”。庆祝活动叫“过年”。",
            },
            {
                    .index = 1,
                    .type = 0,
                    .year = 0,
                    .month = 1,
                    .day = 15,
                    .name = "元宵节",
                    .description = "元宵节，亦称为上元节、小正月、新十五、元夕、小年或灯节，又称为元宵、元宵节、上元、正月半，中国传统节日，每年的农历正月十五，为农历新年的第一个月圆，象征著春天的到来。华人传统会吃元宵、赏花灯、猜灯谜，以示祝贺。",
            },
            {
                    .index = 2,
                    .type = 0,
                    .year = 0,
                    .month = 1,
                    .day = 1,
                    .name = "春节1",
                    .description = "春节，是以农历计算的中国传统新年，亦称新春、正旦、正月朔日，其庆祝活动又俗称过年、度岁等，是汉族四大传统节日之一。农历正月初一为一年之首，故称“岁首”，又称“年节”、“元旦”、“元正”、“元朔”、“元日”、“正旦”、“新正”、“新春”、“履新”、“开正”。庆祝活动叫“过年”。",
            },
            {
                    .index = 4,
                    .type = 0,
                    .year = 0,
                    .month = 1,
                    .day = 15,
                    .name = "元宵节",
                    .description = "元宵节，亦称为上元节、小正月、新十五、元夕、小年或灯节，又称为元宵、元宵节、上元、正月半，中国传统节日，每年的农历正月十五，为农历新年的第一个月圆，象征著春天的到来。华人传统会吃元宵、赏花灯、猜灯谜，以示祝贺。",
            },
    };

    save_data(fest[2], file);


    load_data(file);
    printf("--------------------------------\n");
    load_data_with_index(2, file);

    return 0;
}



int main(int argc, char **argv) {
    Initialize();

    GetCurTime();

    run_calendar();

//    system("pause");
    return 0;
}
