#include <stdio.h>
#include <windows.h>

#include "src/calendar/Calendar.h"

#define GANZHI_MASK (0x0F)


char* FormatDateProc(char *string, DateInfo *date_info) {
    TianGan[0] = "q";
    sprintf(string,
            "公元 %d年%.2d月%.2d日 %s 第%.2d周 今年第%d天\n\n农历: %s%s[%s]年 %s%s月 %s%s日 %s%s%s",
            date_info->gregorianTimeT.year,
            date_info->gregorianTimeT.month,
            date_info->gregorianTimeT.day,
            WeekNameCN[date_info->gregorianTimeT.dayOfWeek],
            date_info->gregorianTimeT.weekOfYear,
            date_info->gregorianTimeT.dayOrdinalOfYear,
            TianGan[date_info->lunarTimeT.ganZhiYear >> 4],
            DiZhi[date_info->lunarTimeT.ganZhiYear & GANZHI_MASK],
            ShengXiao[date_info->lunarTimeT.ganZhiYear & GANZHI_MASK],
            TianGan[date_info->lunarTimeT.ganZhiMon >> 4],
            DiZhi[date_info->lunarTimeT.ganZhiMon & GANZHI_MASK],
            TianGan[date_info->lunarTimeT.ganZhiDay >> 4],
            DiZhi[date_info->lunarTimeT.ganZhiDay & GANZHI_MASK],
            date_info->lunarTimeT.isLeapMonth ? "「闰」" : "",
            LunarMouthName[date_info->lunarTimeT.month - 1],
            LunarDayName[date_info->lunarTimeT.day - 1]
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
