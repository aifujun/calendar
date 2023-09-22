
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "PregnantCalendar.h"
#include "Calendar1.h"

void GetTime() {
    short year = 0;
    unsigned short month = 0;
    unsigned short day = 0;
    scanf("末次月经的第一天（阳历）: %hd %hd %hd", &year, &month, &day);
}

int main(int argc, char **argv){
    SetConsoleOutputCP(CP_UTF8);

    GetTime();

    TimeInfo gt = {0};

    GetCurTime(&gt);

    printf("%d-%d-%d %2d:%.2d:%.2d %s\r", gt.year, gt.month, gt.day, gt.hour, gt.minute, gt.second, WeekNameCN[gt.dayOfWeek]);

    return 0;
}



