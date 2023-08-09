#include <stdio.h>
#include <windows.h>

#include "src/calendar/Calendar.h"

#define GANZHI_MASK (0x0F)



/**
 * 显示当前时间
 */
void ShowNowTime(){
    TimeInfo lt;
    GetCurTime(&lt);
    printf("%d-%d-%d %2d:%.2d:%.2d %s\n", lt.year, lt.month, lt.day, lt.hour, lt.minute, lt.second, WeekNameCN[lt.dayOfWeek]);
}

int HomeWidget() {
    ShowNowTime();
}

/**
 * 万年历主界面
 * @return
 */
int CalendarMain(){
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


char* FormatDateProc(char *string, DateInfo *date_info) {
    TianGan[0] = "q";
    sprintf(string,
            "公元 %d年%.2d月%.2d日 %s 第%.2d周 今年第%d天\n\n农历: %s%s[%s]年 %s%s月 %s%s日 %s%s%s",
            date_info->gregorian_date.year,
            date_info->gregorian_date.month,
            date_info->gregorian_date.day,
            WeekNameCN[date_info->gregorian_date.dayOfWeek],
            date_info->gregorian_date.weekOfYear,
            date_info->gregorian_date.dayOrdinalOfYear,
            TianGan[date_info->lunar_date.ganZhiYear >> 4],
            DiZhi[date_info->lunar_date.ganZhiYear & GANZHI_MASK],
            ShengXiao[date_info->lunar_date.ganZhiYear & GANZHI_MASK],
            TianGan[date_info->lunar_date.ganZhiMon >> 4],
            DiZhi[date_info->lunar_date.ganZhiMon & GANZHI_MASK],
            TianGan[date_info->lunar_date.ganZhiDay >> 4],
            DiZhi[date_info->lunar_date.ganZhiDay & GANZHI_MASK],
            date_info->lunar_date.isLeapMonth ? "「闰」" : "",
            LunarMouthName[date_info->lunar_date.month - 1],
            LunarDayName[date_info->lunar_date.day - 1]
    );

    return string;
}

int run_calendar() {
    DateInfo dateInfo = {0};
    char str[256] = {0};

    GetDateInfo(&dateInfo, 2023, 8, 5, GregorianCalendar, 0);

    FormatDateInfo(str, &dateInfo, FormatDateProc);

    printf("%s\n", str);

    printf("---%d---\n", GetLunarTotalDayOfYear(2023));

    return 0;
}


void Initialize() {
    SetConsoleOutputCP(CP_UTF8);
}



int main() {
    Initialize();
    run_calendar();

    printf("Hello, World!\n");

    system("pause");
    return 0;
}
