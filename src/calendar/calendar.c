
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "calendar.h"
#include "internal/calendar_types.h"
#include "internal/lunar_data.h"

#ifndef CONST_CHINESE
#define CONST_CHINESE
const char *TianGan[] = {
        "甲", "乙", "丙", "丁", "戊",
        "己", "庚", "辛", "壬", "癸"
};
const char *DiZhi[] = {
        "子", "丑", "寅", "卯",
        "辰", "巳", "午", "未",
        "申", "酉", "戌", "亥"
};
const char *ShengXiao[] = {
        "鼠", "牛", "虎", "兔",
        "龙", "蛇", "马", "羊",
        "猴", "鸡", "狗", "猪"
};
const char *JieQi[] = {
        "冬至", "小寒", "大寒", "立春",
        "雨水", "惊蛰", "春分", "清明",
        "谷雨", "立夏", "小满", "芒种",
        "夏至", "小暑", "大暑", "立秋",
        "处暑", "白露", "秋分", "寒露",
        "霜降", "立冬", "小雪", "大雪"
};
const char *LunarDayName[] = {
        "初一", "初二", "初三", "初四", "初五",
        "初六", "初七", "初八", "初九", "初十",
        "十一", "十二", "十三", "十四", "十五",
        "十六", "十七", "十八", "十九", "二十",
        "廿一", "廿二", "廿三", "廿四", "廿五",
        "廿六", "廿七", "廿八", "廿九", "三十"
};
const char *LunarMouthName[] = {
        "正月", "二月", "三月", "四月", "五月", "六月",
        "七月", "八月", "九月", "十月", "冬月", "腊月",
        "闰正月", "闰二月", "闰三月", "闰四月", "闰五月", "闰六月",
        "闰七月", "闰八月", "闰九月", "闰十月", "闰冬月", "闰腊月"
};
const char *WeekNameEN[] = {
        "Sunday",
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday"
};
const char *WeekNameCN[] = {
        "星期日",
        "星期一",
        "星期二",
        "星期三",
        "星期四",
        "星期五",
        "星期六"
};
#endif


static const unsigned short SOLAR_DAY_OF_MONTH[4][13] = {
        {365, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
        {366, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
        {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
        {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366},
};

static int32_t remove_decimals(double number);
static double fc_round(double number, int digits);
static int check_gregorian_date(const FC_DATE *date);
static int JDN_to_hms(double jdn, FC_TIME *time);

static int get_month_info(LUNAR_DATA *lunar_data);
static LUNAR_DATA get_lunar_data(int year);
static int check_lunar_date(const FC_DATE *date);
static bool is_leap_month(int month_info_index, const LUNAR_DATA *lunar_data);
static short get_lunar_days_of_month(int year, unsigned short month, int is_leap);

static int get_gregorian_date_info(const FC_DATE *_gregorian_date, DATE_INFO *date);
static int get_lunar_date_info(const FC_DATE *_lunar_date, DATE_INFO *date);


/**
 * 截取小数的整数部分(向0取整, 精度为 int32_t)
 * @param [in]number: 待截取的数字
 * @return
 */
int32_t remove_decimals(double number) {
    return (int32_t)number;
}

/**
 * 按照指定的小数位数进行四舍五入运算的结果
 * @param [in]number: 需要进行四舍五入的数字
 * @param [in]digits: 保留的小数位数，按此位数后一位进行四舍五入
 * @return
 */
double fc_round(double number, int digits) {
    return floor(number * pow(10, digits) + 0.5) / pow(10, digits);
}


/**
 * 检查公历日期是否合法
 * @param [in]date: 公历日期
 * @return
 */
int check_gregorian_date(const FC_DATE *date) {
    if ((date->year < START_YEAR) || (date->year >= END_YEAR)) {
        return -1;
    }
    if ((date->month > 12) || (date->month < 1)) {
        return -1;
    }

    if ((date->day > SOLAR_DAY_OF_MONTH[is_leap_year(date->year)][date->month]) || (date->day < 1)) {
        return -1;
    }

    return 0;
}

/**
 * 12h起算的日数转时分秒
 * @param [in]jdn: 12h起算的日数转
 * @param [out]time: 转换后的时间
 * @return
 */
int JDN_to_hms(double jdn, FC_TIME *time) {
    double d = jdn - floor(jdn);
    time->hour = remove_decimals(d * 24);
    time->minute = remove_decimals(fc_round((d * 24 - time->hour) * 60, 4));
    if (time->minute == 60) {
        time->minute = 0;
        time->hour += 1;
    }
    double s = d * 86400 - time->hour * 3600 - time->minute * 60;
    if (fabs(s) < 0.001) {
        s = 0;
    }
    time->second = remove_decimals(s);
    time->millisecond = remove_decimals((s - time->second) * 1000);
    return time->hour;
}


/**
 * 判断公历年是否是闰年(公元前1年用-1表示)
 *   [基准]回归年为 365d5h48'46''
 * @note:
 *      1)公元前B年的B（正数）除以4余1，那么它是闰年
 *      2)公元1~1582年的是儒略历, 每隔4年一闰
 *      3)1582之前的是儒略历, 每隔4年一闰
 *      4)1582年之后: 年份为4的倍数但非100的倍数为366天闰年, 年份为400的倍数但非3200的倍数为366天闰年, 年份为172800的倍数为366天闰年
 * @param [in]year: 待判断的年份
 * @return [bool]bool: 如果是闰年就返回true 平年就返回false
 */
bool is_leap_year(int year) {
    if ((year < 0) && (year % 4 == -1)) {
        return true;
    }

    if ((year > 0) && (year <= 1582) && (year % 4 == 0)) {
        return true;
    }

    if (year > 1582 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0 && year % 3200 !=0) || (year % 172800 == 0))) {
        return true;
    }

    return false;
}

/**
 * 计算公历日期在年内的序数(距离元旦的天数, 元旦为序数 1)
 * @param [in]date: 公历日期
 * @return ordinals: 日期在年内的序数
 */
int gre_date_to_ordinal(const FC_DATE *date) {
    return SOLAR_DAY_OF_MONTH[date->leap_flag + 2][date->month - 1] + date->day;
}

/**
 * 根据年内序数计算日期(索引 -1表示上一年最后一天, 1表示1月1号元旦)
 * @param [in]ordinal_days: 序数
 * @param [in]base_year: 序数对应的年份
 * @param [out]date: 公历日期
 * @return
 */
int ordinal_to_date(int ordinal_days, short base_year, FC_DATE *date) {
    if (ordinal_days < 0) {
        ordinal_days += 1;
    }
    // 对于年份之前的负序数，转换成对应年份的正序数
    while (ordinal_days <= 0) {
        base_year -= 1;
        // 从公元后的年份转到公元前，中间没有公元0年
        if (base_year == 0) {
            base_year -= 1;
        }
        ordinal_days += SOLAR_DAY_OF_MONTH[is_leap_year(base_year)][0];
    }
    // 对于正序数，转换到年内序数的年份方便后续计算
    while (ordinal_days > SOLAR_DAY_OF_MONTH[is_leap_year(base_year)][0]) {
        base_year += 1;
        // 从公元前的年份转到公元后，中间没有公元0年
        if (base_year == 0) {
            base_year += 1;
        }
        ordinal_days -= SOLAR_DAY_OF_MONTH[is_leap_year(base_year-1)][0];
    }

    date->year = base_year;
    date->month = 1;
    date->leap_flag = is_leap_year(base_year);
    while (ordinal_days > SOLAR_DAY_OF_MONTH[date->leap_flag][date->month]) {
        ordinal_days -= SOLAR_DAY_OF_MONTH[date->leap_flag][date->month];
        date->month += 1;
    }
    date->day = ordinal_days;

    return check_gregorian_date(date);
}


/**
 * 获取公历对应的农历日期
 * @param [in]gre_date: 公历日期
 * @param [out]lunar_date: 农历日期
 * @return
 */
int gregorian_to_lunar(const FC_DATE *gre_date, FC_DATE *lunar_date) {
    int gregorian_ordinal = gre_date_to_ordinal(gre_date);
    LUNAR_DATA lunar_data = get_lunar_data(gre_date->year);
    int lunar_ordinal = gregorian_ordinal - lunar_data.ordinal;

    // 春节在元旦后面的, 春节被重复计算(多减一次), 需加上
    lunar_ordinal = (lunar_data.ordinal > 0) ? lunar_ordinal + 1 : lunar_ordinal;
    //
    lunar_ordinal = (lunar_ordinal <= 0) ? lunar_ordinal - 1 : lunar_ordinal;

    lunar_ordinal_to_date(lunar_ordinal, (short)gre_date->year, lunar_date);

    return 0;
}


/**
 * 格里高利历转儒略日数
 * @param [in]time: 公历日期
 * @return
 */
double date_to_JDN(const FC_TIME *time) {
    int Y = time->year < 0 ? time->year + 1 : time->year;
    int M = time->month;
    int D = time->day;
    int B = 0;
    double julian_num = 0;

    if (M <= 2) {
        Y -= 1;
        M += 12;
    }

    if ((Y > 1582) || (Y == 1582 && M > 10) || (Y == 1582 && M == 10 && D >= 15)) {
        B = 2 - (int)floor((double)Y / 100) + (int)floor((double)Y / 400);
    }

    // julian_days = floor(365.25*(Y+4712)) + floor(30.6*(M+1)) + D + B - 63.5;
    julian_num = floor(365.25 * (Y + 4716)) + floor(30.6 * (M + 1)) + D + B - 1524.5;

    julian_num += (double)(time->hour * 3600 + time->minute * 60 + time->second + (double)time->millisecond / 1000 + (double)time->microsecond / 1000000) / 86400;

    return julian_num;
}


/**
 * 儒略日转格里高利历
 * @param [in]julian_days: 儒略日数
 * @param [out]time: 转换后的公历时间
 * @return
 */
int JDN_to_date(double julian_days, FC_TIME *time) {
    int A = 0, B = 0, C = 0, D = 0, E = 0, K = 0, Z = 0;
    double tmp_day = 0, F = 0;

    // 转换成以 BC. 4713年1月1日0时为历元
    julian_days += 0.5;
    Z = floor(julian_days);

    // 日的小数部分
    F = julian_days - Z;
    JDN_to_hms(F, time);

    // 2299161之前为儒略历, 之后为格里历(将格里历转为儒略历方便计算)
    A = Z < JULIAN_TO_GREGORIAN ? Z : (int)(Z + 10 + floor((Z - 2305507.25) / 36524.25) - floor(floor((Z - 2305507.25) / 36524.25) / 4));

    while (tmp_day < 1) {
        B = A + 1524;  // 以B.C. 4717年3月1日0时为历元
        C = floor((B - 122.1) / 365.25);  // 积年
        D = floor(365.25 * C);  // 积年的日数
        E = floor((B - D) / 30.6);  // B-D为年内积日，E即月数
        tmp_day = B - D - floor(30.6 * E) + F;
        // 当夜24点，实为第二日0点
        if (time->hour == 24) {
            A += 1;
            F = 0;
            continue;
        }
        if (tmp_day >= 1) {
            break;
        }
        // 当月1日在前一日，前置一日重新计算
        A -= 1;
        K += 1;
    }

    if ((tmp_day += K) == 0) {
        tmp_day += 1;
    }
    time->day = (uint16_t)tmp_day;
    time->month = E < 14 ? E - 1 : E - 13;
    time->year = (short)(time->month > 2 ? C - 4716 : C - 4715);
    if (time->year <= 0) {
        time->year -= 1;
    }

    return 0;
}

/**
 * 获取公历日期对应的星期日 (deprecated, 使用get_week)
 * @param [in]date: 公历日期
 * @return week: 对应的星期日
 */
short get_day_of_week_deprecated(const FC_DATE *date) {
    short week = -1;
    short first_week;
    int year = date->year;
    int month = date->month;
    int day = date->day;

    // 一二月单独换算
    if (month == 1 || month == 2) {
        month += 12;
        year--;
    }

    // 格利高里历（Gregorian calendar）从1582年10月5日到1582年10月14日的10天并不存在
    if ((year < 1582) || (year == 1582 && month < 10) || (year == 1582 && month == 10 && day <= 4)) {
        first_week = 6;
    }
    else {
        first_week = 1;
    }
    // 基姆拉尔森公式
    week = (short)((day + 2*month + 3*(month+1)/5 + year + year/4 - year/100 + year/400 + first_week) % 7);
    // 其中0~6表示周日到周六
    return week;
}

/**
 * 获取公历日期对应的星期日
 * @param [in]date: 公历日期
 * @return week: 对应的星期日
 */
short get_week(const FC_DATE *date) {
    FC_TIME time = {0};
    time.year = date->year;
    time.month = date->month;
    time.day = date->day;
    time.hour = 12;
    double julian_days = date_to_JDN(&time);

    return (int16_t)(((((int64_t)julian_days) % 7) + 7 + JULIAN_START_WEEK) % 7);
}


/**
 * 计算日期在一年内的星期数(默认一周起始为星期一，以1月4号所在的星期为一年的第一周)
 * @param [in]date: 公历日期
 * @param [in]first_week: 星期的起始(默认周一), 周一为1，周日为0
 * @param [in]first_weeks: 第一周的起始日期算法
 * @return
 * @todo: 修改默认第一天
 */
unsigned short get_weeks(const FC_DATE *date, unsigned short first_week, unsigned short first_weeks) {
    int ordinal_days = SOLAR_DAY_OF_MONTH[is_leap_year(date->year) + 2][date->month - 1] + date->day;
    FC_DATE new_years_day = {.year = date->year, .month = 1, .day = 1, .leap_flag = is_leap_year(date->year)};
    short week = get_week(&new_years_day);

    week = (short)((week == 0) ? 7 : week);

    if (week > 4) {
        ordinal_days = ordinal_days - (7 - week + 1);
    }
    else {
        ordinal_days = ordinal_days + (week - 1);
    }

    return ordinal_days % 7 ? ordinal_days / 7 + 1 : ordinal_days / 7;
}


/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

/**
 * 获取农历数据月份信息
 * @param [in,out]lunar_data: 农历数据自定义结构体指针
 * @return
 */
int get_month_info(LUNAR_DATA *lunar_data) {
    int rows_num = sizeof(SPECIAL_MONTH)/sizeof(SPECIAL_MONTH[0]);
    int columns_num = sizeof(SPECIAL_MONTH[0])/sizeof(SPECIAL_MONTH[0][0]);
    int index = -1;
    unsigned int month_days = 0;

    for (int i = 0; i < rows_num; i++) {
        if (SPECIAL_MONTH[i][0] == lunar_data->year) {
            index = i;
            break;
        }
    }

    if (index != -1) {
        month_days = SPECIAL_MONTH[index][columns_num-1];
        for (int i = 1; i < columns_num - 1; i++) {
            lunar_data->month_info[i] = SPECIAL_MONTH[index][i];
        }
    }
    else {
        month_days = lunar_data->raw_data & MON_INFO_MASK;
        for (short i = 0, j = 0; i < 14; i++, j++) {
            lunar_data->month_info[i] = j;
            if (lunar_data->leap_month && lunar_data->leap_month == i){
                lunar_data->month_info[++i] = j;
            }
        }
        if (lunar_data->leap_month == 0) {
            lunar_data->month_info[13] = 0;
        }
    }
    for (int i = 1; i < MAX_MONTH; i++) {
        lunar_data->month_days[i] = (month_days & 0x1) ? 30 : 29;
        if (lunar_data->month_info[i] == 0) {
            lunar_data->month_days[i] = 0;
        }
        lunar_data->month_days[0] = (short)(lunar_data->month_days[i] + lunar_data->month_days[0]);
        month_days >>= 1;
    }

    return 0;
}

/**
 * 获取某一年的农历数据
 * @param [in]year: 年份
 * @return lunar_data: 对应年份的农历数据
 */
LUNAR_DATA get_lunar_data(int year) {
    LUNAR_DATA lunar_data;
    unsigned int ori_data;

    ZeroMemory(&lunar_data, sizeof(LUNAR_DATA));
    ori_data = year < 0 ? LUNAR_DATA_BC[-year] : LUNAR_DATA_AD[year];

    lunar_data.year = (short)year;
    lunar_data.raw_data = ori_data;
    lunar_data.ordinal = (short)((ori_data & MINUS_MASK) ? -((ori_data & ORDINAL_MASK) >> 17) : (ori_data & ORDINAL_MASK) >> 17);
    lunar_data.leap_month = (short)((ori_data & LEAP_MON_MASK) >> 13);
    get_month_info(&lunar_data);
    return lunar_data;
}

/**
 * 简单检查农历日期是否合法
 * @param [in]_lunarDateT: 农历日期 LunarDate_T 结构体指针
 * @return
 * @todo: 需要重写
 */
int check_lunar_date(const FC_DATE *date) {
    LUNAR_DATA lunar_data = get_lunar_data(date->year);
    if (date == NULL) {
        return -1;
    }
    if ((date->year < START_YEAR) || (date->year >= END_YEAR)) {
        return -2;
    }

    if ((date->month < 1) || (date->month > 12) ||
        (date->day < 1) || (date->day > lunar_data.month_days[date->month])) {
        return -1;
    }

    return 0;
}

/**
 * 判断月份信息索引对应的月份是否为闰月
 * @param [in]month_info_index: 月份信息索引
 * @param [in]lunar_data: 对应年份的农历数据
 * @return
 */
bool is_leap_month(int month_info_index, const LUNAR_DATA *lunar_data) {
    int month_flag = 0;
    for (int i = 1; i < month_info_index; ++i) {
        if (lunar_data->month_info[i] == lunar_data->month_info[month_info_index]) {
            month_flag++;
        }
    }
    return month_flag;
}

/**
 * 获取农历月的天数
 * @param year: 农历年
 * @param month: 农历月
 * @param is_leap: 是否为闰月
 * @return
 */
short get_lunar_days_of_month(int year, unsigned short month, int is_leap) {
    LUNAR_DATA lunar_data = get_lunar_data(year);

    for (int i = 1; i < MAX_MONTH; i++) {
        if (lunar_data.month_info[i] == month && --is_leap < 0) {
            return lunar_data.month_days[i];
        }
    }
    return -1;
}

/**
 * 获取某年干支生肖
 * @param [in]year: 所求的年份(公元前用负数表示)
 * @return
 */
unsigned char get_ganzhi_year_deprecated(int year) {
    unsigned char gan = 0;
    unsigned char zhi = 0;
    unsigned char ganzhi = 0;
    if (year > 0) {
        gan = (year - 4) % 10;
        zhi = (year - 4) % 12;
    }
    else {
        gan = (year + 7) % 10;
        zhi = (year + 9) % 12;
    }
    gan = gan < 0 ? gan + 10 : gan;
    zhi = zhi < 0 ? zhi + 12 : zhi;

    ganzhi = gan;
    ganzhi <<= 4;
    ganzhi |= zhi;

    return ganzhi;
}

/**
 * 获取某年干支生肖
 * @param [in]year: 所求的年份(公元前用负数表示)
 * @return
 */
unsigned char get_ganzhi_year(int year) {
    unsigned char gan = 0;
    unsigned char zhi = 0;
    unsigned char ganzhi = 0;

    year = year > 0 ? year - 1 : year;
    gan = (((year + 4713 + JULIAN_START_YEAR_GANZHI) % 60 + 60) % 60) % 10;
    zhi = (((year + 4713 + JULIAN_START_YEAR_GANZHI) % 60 + 60) % 60) % 12;

    ganzhi = gan;
    ganzhi <<= 4;
    ganzhi |= zhi;
    return ganzhi;
}

/**
 * 获取某年某月干支 (调用时需要判断日期所属月份)
 * @note 月干支纪时有两种说法:
 *       (1) 每年的年干从立春开始，月干采用每个地支对应二十四节气自某节气（非中气）至下一个节气，以交节时间决定起始的一个月期间，不是农历某月初一至月底
 *       (2) 直接以农历月区分计算月柱
 * @param [in]year: 所求的年份(公元前用负数表示)
 * @param [in]month: 所求的月份
 * @return
 */
unsigned char get_ganzhi_month(int year, unsigned short month) {
    // 每年的年干从立春开始，月干采用每个地支对应二十四节气自某节气（非中气）至下一个节气，以交节时间决定起始的一个月期间，不是农历某月初一至月底
    unsigned char gan = 0;
    unsigned char zhi = 0;
    unsigned char ganzhi;

    // 月干 = 年干 × 2 + 月数
    year = year > 0 ? year - 1 : year;
    gan = (((((year + 4713 + JULIAN_START_YEAR_GANZHI) % 60 + 60) % 60) % 10 + 1) * 2 + month) % 10 - 1;

    zhi = (month + 1) % 12;

    ganzhi = gan;
    ganzhi <<= 4;
    ganzhi |= zhi;

    return ganzhi;
}

/**
 * 获取某天的干支
 * @param [in]date: 所求的农历日期
 * @return
 */
unsigned char get_ganzhi_day(const FC_DATE *date) {
    unsigned char gan = 0;
    unsigned char zhi = 0;
    unsigned char ganzhi = 0;
    FC_DATE gre_date = {0};

    double julian_days = 0;
    int16_t ganzhi_num = 0;

    lunar_to_gregorian(date, &gre_date);
    FC_TIME gt = {.year=gre_date.year, .month=gre_date.month, .day=gre_date.day};

    julian_days = date_to_JDN(&gt);
    julian_days += 0.5;

    if (julian_days < 0) {
        ganzhi_num = (int16_t)(((int16_t)(((int64_t)julian_days) % 60) + 60 + JULIAN_START_GANZHI) % 60);
    }
    else {
        ganzhi_num = (int16_t)(((int64_t)julian_days + JULIAN_START_GANZHI) % 60);
    }

    gan = ganzhi_num % 10;
    zhi = ganzhi_num % 12;

    ganzhi = gan;
    ganzhi <<= 4;
    ganzhi |= zhi;

    return ganzhi;
}

/**
 * 获取时干支（24小时纪时）
 * @param [in]date: 农历日期
 * @param [in]hour: 所计算时间(24小时制)
 * @param [in]dis_zi: 是否区分早晚子时 (0-不区分, 1-区分)
 * @note 区分早晚子则夜子(23:00~00:00)应该属于当日的年月日时干支，不区分则夜子(23:00~00:00)属于次日的年月日时干支
 * @return
 */
unsigned char get_ganzhi_hour(const FC_DATE *date, unsigned short hour, unsigned char dis_zi) {
    unsigned char gan = 0;
    unsigned char zhi = 0;
    unsigned char gan_zhi = 0;
    unsigned char gan_day = 0;
    unsigned short tmp_z = 0;

    gan_day = get_ganzhi_day(date);
    gan_day >>= 4;

    gan_day = (!dis_zi && hour >= 23) ? gan_day + 2 : gan_day + 1;
    tmp_z = hour & 0x01 ? 1 : 0;
    zhi = ((hour + tmp_z) / 2 + 1) % 12 - 1;
    gan = (gan_day * 2 + (zhi + 1) - 2) % 10  - 1;

    gan_zhi = gan;
    gan_zhi <<= 4;
    gan_zhi |= zhi;

    return gan_zhi;
}

/**
 * 计算农历日期在年内的序数(正月初一为序数 1, 除夕序数为 -1)
 * @param [in]date: 农历日期
 * @return ordinal_days: 农历日期对应的年内序数
 */
int lunar_date_to_ordinal(const FC_DATE *date) {
    int month_flag = date->leap_flag;
    int ordinal_days = 0;
    LUNAR_DATA lunar_data = get_lunar_data(date->year);

    for (int i = 1; i < MAX_MONTH; ++i) {
        if (lunar_data.month_info[i] == date->month && (--month_flag < 0)) {
            break;
        }
        ordinal_days += lunar_data.month_days[i];
    }
    ordinal_days += date->day;

    return ordinal_days;
}


/**
 * 根据农历年内序数计算农历日期(-1 表示上一年最后一天,  1 表示正月初一)
 * @param [in]ordinal_days: 序数
 * @param [in]base_year: 序数对应的年份
 * @param [out]lunar_date: 农历日期
 * @return
 */
int lunar_ordinal_to_date(int ordinal_days, short base_year, FC_DATE *lunar_date) {
    LUNAR_DATA data = get_lunar_data(base_year);

    if (ordinal_days < 0) {
        ordinal_days += 1;
    }
    // 对于年份之前的负序数，转换成对应年份的正序数
    while (ordinal_days < 0) {
        base_year -= 1;
        // 从公元后的年份转到公元前，中间没有公元0年
        if (base_year == 0) {
            base_year -= 1;
        }
        data = get_lunar_data(base_year);
        ordinal_days += data.month_days[0];
    }
    // 对于正序数，转换到对应年份的年内序数方便后续计算
    while (ordinal_days > data.month_days[0]) {
        base_year += 1;
        // 从公元前的年份转到公元后，中间没有公元0年
        if (base_year == 0) {
            base_year += 1;
        }
        ordinal_days -= data.month_days[0];
        data = get_lunar_data(base_year);
    }

    lunar_date->year = base_year;
    lunar_date->month = 1;

    for (int i = 1; i < MAX_MONTH; i++) {
        if (ordinal_days <= data.month_days[i]) {
            lunar_date->month = data.month_info[i];
            lunar_date->day = ordinal_days;
            lunar_date->leap_flag = is_leap_month(i, &data);
            return 0;
        }
        ordinal_days -= data.month_days[i];
    }

    return check_lunar_date(lunar_date);
}

/**
 * 农历日期转公历日期
 * @param [in]_lunar_date: 农历日期
 * @param [out]_gregorian_date: 公历日期
 * @return
 */
int lunar_to_gregorian(const FC_DATE *_lunar_date, FC_DATE *_gregorian_date) {
    int gregorian_ordinal = 0;
    int lunar_ordinal = 0;

    LUNAR_DATA lunar_data = get_lunar_data((short)_lunar_date->year);
    lunar_ordinal = lunar_date_to_ordinal(_lunar_date);
    gregorian_ordinal = lunar_ordinal + lunar_data.ordinal;

    // (春节在元旦后面的) 或 (春节在元旦前面并且农历日期在元旦前面时), 农历正月初一被重复计算, 需要减一;
    gregorian_ordinal = (gregorian_ordinal <= 0 || lunar_data.ordinal > 0) ? gregorian_ordinal - 1 : gregorian_ordinal;

    return ordinal_to_date(gregorian_ordinal, _lunar_date->year, _gregorian_date);
}


/**
 * 获取农历一年的总天数
 * @param [in]year: 年份
 */
unsigned short lunar_get_total_day_of_year(short year) {
    LUNAR_DATA lunar_data = get_lunar_data(year);

    return lunar_data.month_days[0];
}



/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/
/**
 * 获取公历日期信息
 * @param [in]_gregorian_date: 公历日期
 * @param [out]date: 日期信息
 * @return
 */
int get_gregorian_date_info(const FC_DATE *_gregorian_date, DATE_INFO *date) {
    int day_ordinal = gre_date_to_ordinal(_gregorian_date);

    date->gregorian.year = _gregorian_date->year;
    date->gregorian.month = _gregorian_date->month;
    date->gregorian.day = _gregorian_date->day;
    date->gregorian.dayOfWeek = get_week(_gregorian_date);
    date->gregorian.firstDayOfWeek = FIRST_DAY_OF_WEEK;
    date->gregorian.firstWeekOfYear = FIRST_WEEK_OF_YEAR;
    date->gregorian.weekOfYear = get_weeks(_gregorian_date, FIRST_DAY_OF_WEEK, FIRST_WEEK_OF_YEAR);
    date->gregorian.dayOrdinalOfYear = day_ordinal;

    return 0;
}

/**
 * 获取农历日期信息
 * @param [in]_lunar_date: 农历日期
 * @param [out]date: 日期信息
 * @return
 */
int get_lunar_date_info(const FC_DATE *_lunar_date, DATE_INFO *date) {
    LUNAR_DATA lunar_data = get_lunar_data(_lunar_date->year);

    date->lunar.year = _lunar_date->year;
    date->lunar.month = _lunar_date->month;
    date->lunar.day = _lunar_date->day;
    date->lunar.isLeapMonth = _lunar_date->leap_flag;

    date->lunar.monthDay = get_lunar_days_of_month(_lunar_date->year, _lunar_date->month, _lunar_date->leap_flag);

    date->lunar.ganZhiYear = get_ganzhi_year(_lunar_date->year);
    date->lunar.ganZhiMon = get_ganzhi_month(_lunar_date->year, _lunar_date->month);
    date->lunar.ganZhiDay = get_ganzhi_day(_lunar_date);

    return 0;
}


/**
 * 根据输入的日期获取日期的详细信息
 * @param date: 输入日期的详细信息
 * @param year: 日期的年份
 * @param month: 日期月份
 * @param day: 日期
 * @param calendar_type: 日期类型(0: 农历, 1: 公历)
 * @param is_leap: 如果为农历, 表示输入的月份是否为闰月
 * @return
 */
int get_date_info(DATE_INFO *date, int year, unsigned short month, unsigned short day, CalendarType calendar_type, bool is_leap) {
    FC_DATE lunar_date = {0};
    FC_DATE gre_date = {0};

    switch (calendar_type) {
        case LunarCalendar: {
            lunar_date.year = year;
            lunar_date.month = month;
            lunar_date.day = day;
            lunar_date.leap_flag = is_leap;
            lunar_to_gregorian(&lunar_date, &gre_date);
            break;
        }
        case GregorianCalendar: {
            gre_date.year = year;
            gre_date.month = month;
            gre_date.day = day;
            gregorian_to_lunar(&gre_date, &lunar_date);
            break;
        }
        default:
            break;
    }

    get_lunar_date_info(&lunar_date, date);
    get_gregorian_date_info(&gre_date, date);

    return 0;
}




/*--------------------------------------------------------------------------------------------------------------------*/

int test(DATE_INFO *date_info) {
    printf("公元 %d年%.2d月%.2d日 %s 第%.2d周 今年第%d天\n农历: %s%s[%s]年 %s%s月 %s%s日 %s%s%s",
            date_info->gregorian.year,
            date_info->gregorian.month,
            date_info->gregorian.day,
            WeekNameCN[date_info->gregorian.dayOfWeek],
            date_info->gregorian.weekOfYear,
            date_info->gregorian.dayOrdinalOfYear,
            TianGan[date_info->lunar.ganZhiYear >> 4],
            DiZhi[date_info->lunar.ganZhiYear & GANZHI_MASK],
            ShengXiao[date_info->lunar.ganZhiYear & GANZHI_MASK],
            TianGan[date_info->lunar.ganZhiMon >> 4],
            DiZhi[date_info->lunar.ganZhiMon & GANZHI_MASK],
            TianGan[date_info->lunar.ganZhiDay >> 4],
            DiZhi[date_info->lunar.ganZhiDay & GANZHI_MASK],
            date_info->lunar.isLeapMonth ? "「闰」" : "",
            LunarMouthName[date_info->lunar.month - 1],
            LunarDayName[date_info->lunar.day - 1]
    );

    return 0;
}


/*
int main(int argc, char **argv) {
    system("@chcp 65001");

    DATE_INFO date = {0};
    get_date_info(&date, 2023, 9, 21, GregorianCalendar, 0);

    test(&date);

    return 0;
}
*/
