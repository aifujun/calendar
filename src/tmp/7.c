#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

bool is_leap_year(int year) {
    //对于大数值年份，能整除3200且能整除172800为闰年
    //世纪年能被400整除的是闰年
    //普通年能被4整除且不能被100整除的为闰年
    //公元1~1582年的是儒略历, 每隔4年一闰
    //公元前B年的B（正数）除以4余1，那么它也是闰年
    if ((year < 0) && (year % 4 == -1)) {
        return true;
    }

    //1582之前的是儒略历, 每隔4年一闰
    if ((year > 0) && (year <= 1582) && (year % 4 == 0)) {
        return true;
    }

    if (year > 1582 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0 && year % 3200 !=0 ) || (year % 172800 == 0))) {
        return true;
    }

    return false;
}


void test1() {
    printf("%d\n", is_leap_year(-1));
    printf("%d\n", is_leap_year(6400));
    printf("%d\n", is_leap_year(172800));
    printf("%d\n", is_leap_year(-4713));

}


int main(int argc, char **argv) {
    system("@chcp 65001");

    test1();

    return 0;
}
