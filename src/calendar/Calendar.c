/**
 * @todo (1) 对于函数间 exception 和 错误处理方式 重构
 *       (2) 设置工作周  一周的第一天是星期几？  一年的第一周怎么算？（01月01日， 第一个满4天的周，第一个全周）
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <windows.h>
#include <math.h>
#include <stdint.h>

#include "Calendar.h"
#include "lunarData.h"


/**
 * @brief 公历月份天数索引
 * @note  二维数组一维从1~12表示1-12月, 二维表示是否闰年
 */
static const unsigned short DayOfMonthCounter[13][2] = {0,0,31,31,28,29,31,31,30,30,31,31,30,30,31,31,31,31,30,30,31,31,30,30,31,31};
/**
 * @brief 农历月份天数索引。从0~12表示1-12月(有闰月按顺序索引，无闰月索引12为0)
 * @note 使用前调用 UpdateLunarDayOfMonth 函数更新确保数据准确
 */
static unsigned short LunarDayOfMonth[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


static ErrorCode CheckGregorianDate(const GregorianDate_T *_gregorianDateT);        // 检验公历年、月、日的合法性
static ErrorCode CheckLunarDate(const LunarDate_T *_lunarDateT);                    // 检查农历日期是否合法
static unsigned int GetLunarData(short year);                                           // 获取农数据信息
static unsigned short GetLunarDayOfMonthIndex(const LunarDate_T *_lunarDateT);      // 获取农历月份天数在索引表的索引表
static short GetLunarOrdinalInNewYear(short lunar_year);                            // 获取农历新年在公历的年内序数，参数：农历年，返回值：农历新年的公历年内序数
static ErrorCode UpdateLunarDayOfMonth(short lunarYear);                            // 更新农历月份天数数据
static unsigned short GetGregorianTotalDayOfYear(short year);                                // 获取公历总天数
static void PreprocessDayOrdinalWithYear(short *year, int *ordinalDays, CalendarType type);
                                                                                    // 将序数索引转换成对应年份年内的天数序数
static int32_t RemoveDecimals(double digit);


/**
 * 截取小数的整数部分(精度为 int32_t)
 * @param digit: 带截取的数字
 * @return
 */
int32_t RemoveDecimals(double digit) {
    return (int32_t)digit;
}

/**
 * 检查公历日期是否合法
 * @param [in]_gregorianDateT: 公历日期 GregorianDate_T 结构体
 * @return ErrorCode:
 */
static ErrorCode CheckGregorianDate(const GregorianDate_T *_gregorianDateT) {
    if (_gregorianDateT == NULL) {
        return CALENDAR_ERROR_ILLEGAL_DATE;
    }
    if ((_gregorianDateT->year < START_YEAR) || (_gregorianDateT->year >= END_YEAR)) {
        return CALENDAR_ERROR_DATE_OUT_OF_RANGE;
    }

    if ((_gregorianDateT->day > DayOfMonthCounter[_gregorianDateT->month][IsLeapYear(_gregorianDateT->year)]) ||
        (_gregorianDateT->day < 1) ||
        (_gregorianDateT->month > 12) ||
        (_gregorianDateT->month < 1)) {
        return CALENDAR_ERROR_ILLEGAL_DATE;
    }

    return ERR_OK;
}

/**
 * 简单检查农历日期是否合法
 * @param [in]_lunarDateT: 农历日期 LunarDate_T 结构体指针
 * @return
 */
static ErrorCode CheckLunarDate(const LunarDate_T *_lunarDateT) {
    if (_lunarDateT == NULL) {
        return CALENDAR_ERROR_ILLEGAL_DATE;
    }
    if ((_lunarDateT->year < START_YEAR) || (_lunarDateT->year >= END_YEAR)) {
        return CALENDAR_ERROR_DATE_OUT_OF_RANGE;
    }

    //闰月检查
    if (_lunarDateT->isLeapMonth && GetLunarLeapMonth(_lunarDateT->year) != _lunarDateT->month) {
        return CALENDAR_ERROR_ILLEGAL_DATE;
    }

    if ((_lunarDateT->month < 1) || (_lunarDateT->month > 12) ||
        (_lunarDateT->day < 1) ||
        (_lunarDateT->day > LunarDayOfMonth[GetLunarDayOfMonthIndex(_lunarDateT)])) {
        return CALENDAR_ERROR_ILLEGAL_DATE;
    }

    return ERR_OK;
}

/**
 * 获取某一年的农历数据
 * @param [in]year: 年份
 * @param [out]data: 月份信息数据
 * @return
 */
static unsigned int GetLunarData(short year) {
    unsigned int data = 0;
    if (year < 0) {
        year = (short)(-year);
        data = BCLunarMonthInfo[year];
    }
    else {
        data = CLunarMonthInfo[year];
    }

    return data;
}

/**
 * 根据实际农历月份返回该月份在 LunarDayOfMonth 表中的索引值
 * @param [in]_lunarDateT: 农历日期 LunarDate_T 结构体指针
 * @return
 */
static unsigned short GetLunarDayOfMonthIndex(const LunarDate_T *_lunarDateT) {
    unsigned short real_leap_month = GetLunarLeapMonth(_lunarDateT->year);
    UpdateLunarDayOfMonth(_lunarDateT->year);

    if (_lunarDateT->isLeapMonth || (real_leap_month && _lunarDateT->month > real_leap_month)) {
        return _lunarDateT->month;
    }

    return _lunarDateT->month - 1;
}

/**
 * 获取农历新年在公历的年内序数(农历正月初一到元旦的天数)
 * @param [in]lunar_year: 农历年
 * @return OrdinalDays: 年内序数
 */
static short GetLunarOrdinalInNewYear(short lunar_year) {
    unsigned int source_data = GetLunarData(lunar_year);

    //取出农历新年的年内序数
    return (short)((source_data>>17)&0x3F);
}

/**
 * 根据年份更新农历的天数
 * @param [in]lunarYear: 农历年份
 */
static ErrorCode UpdateLunarDayOfMonth(short lunarYear) {
    unsigned int lunar_data = GetLunarData(lunarYear);

    for (int i = 0; i < 13; ++i) {
        LunarDayOfMonth[i] = lunar_data & 0x1 ? 30 : 29;
        lunar_data >>= 1;
    }

    if (!GetLunarLeapMonth(lunarYear)) {
        LunarDayOfMonth[12] = 0;
    }

    return ERR_OK;
}

/**
 * 获取公历一年的总天数
 * @param [in]year: 年份
 */
static unsigned short GetGregorianTotalDayOfYear(short year) {
    unsigned short total_day = 0;
    bool leap = IsLeapYear(year);

    for (short i = 0; i < 13; ++i) {
        total_day += DayOfMonthCounter[i][leap];
    }

    return total_day;
}


/**
 * 获取农历一年的总天数
 * @param [in]lunarYear: 年份
 */
unsigned short GetLunarTotalDayOfYear(short lunarYear) {
    unsigned short total_day = 0;

    UpdateLunarDayOfMonth(lunarYear);

    for (short i = 0; i < 13; ++i) {
        total_day += LunarDayOfMonth[i];
    }

    return total_day;
}

/**
 * 对基于年份的序数索引进行预处理(将序数索引转换成对应年份年内的天数序数)
 * @param [in,out]year: base年份 -> 所求年份
 * @param [in,out]ordinalDays: base索引 -> 所求年份内的天数序数
 * @param [in]type: 日期 CalendarType(枚举中定义) 类型
 */
static void PreprocessDayOrdinalWithYear(short *year, int *ordinalDays, CalendarType type) {
    unsigned short (*TOTAL_DAY_OF_YEAR)(short);

    switch (type) {
        case GregorianCalendar:
            TOTAL_DAY_OF_YEAR = &GetGregorianTotalDayOfYear;
            break;
        case LunarCalendar:
            TOTAL_DAY_OF_YEAR = &GetLunarTotalDayOfYear;
            break;
        default:
            TOTAL_DAY_OF_YEAR = &GetGregorianTotalDayOfYear;
            break;
    }

    // 对于年份之前的负序数，转换成对应年份的正序数
    while (*ordinalDays <= 0) {
        *year -= 1;
        // 从公元后的年份转到公元前，中间没有公元0年
        if (*year == 0) {
            *year -= 1;
        }
        *ordinalDays += TOTAL_DAY_OF_YEAR(*year);
    }
    // 对于正序数，转换到年内序数的年份方便后续计算
    while (*ordinalDays > TOTAL_DAY_OF_YEAR(*year)) {
        *year += 1;
        // 从公元前的年份转到公元后，中间没有公元0年
        if (*year == 0) {
            *year += 1;
        }
        *ordinalDays -= TOTAL_DAY_OF_YEAR(*year);
    }
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

/**
 * 获取农历某一年的闰月情况
 * @param [in]lunar_year: 待获取农历年
 * @return wLeapMonth: 该年的闰月月份(0表示无闰月)
 */
unsigned short GetLunarLeapMonth(short lunar_year) {
    unsigned int data = GetLunarData(lunar_year);

    return (unsigned short)((data>>13)&0x0F);
}

/**
 * 计算公历日期在年内的序数(距离元旦的天数, 元旦为序数 1)
 * @param [in]_gregorianDate: 公历日期 GregorianDate_T 结构指针
 * @param [out]ordinalDays: 年内序数
 * @return ErrorCode:
 */
ErrorCode GetDayOrdinal(const GregorianDate_T *_gregorianDate, int *ordinalDays) {
    bool is_leap = IsLeapYear(_gregorianDate->year);

    *ordinalDays = 0;
    for (int i = 0; i < _gregorianDate->month; ++i) {
        *ordinalDays += DayOfMonthCounter[i][is_leap];
    }
    *ordinalDays += _gregorianDate->day;

    return ERR_OK;
}

/**
 * 计算农历日期在年内的序数(距离正月初一的天数, 正月初一为序数 1)
 * @param [in]_lunarDateT: 农历日期 LunarDate_T 结构指针
 * @param [out]ordinalDays: 年内序数
 * @return ErrorCode:
 */
ErrorCode GetLunarDayOrdinal(const LunarDate_T *_lunarDateT, int *ordinalDays) {
    ErrorCode result = ERR_SYSTEM;
    unsigned short real_month_index = 0;

    // 获取农历月份中的索引对应值
    real_month_index = GetLunarDayOfMonthIndex(_lunarDateT);

    *ordinalDays = 0;
    for (int i = 0; i < real_month_index; ++i) {
        *ordinalDays += LunarDayOfMonth[i];
    }
    *ordinalDays += _lunarDateT->day;

    return result;
}

/**
 * 根据年内序数计算日期(索引 0 表示上一年最后一天)
 * @param [in]year: 年
 * @param [in]ordinalDays: 年内序数
 * @param [out]_gregorianDate: 公历日期 GregorianDate_T 结构指针
 * @return
 */
ErrorCode GetDateBasedOrdinal(short year, int ordinalDays, GregorianDate_T *_gregorianDate) {
    int tmp_days = 0;
    bool leap_flag = false;

    PreprocessDayOrdinalWithYear(&year, &ordinalDays, GregorianCalendar);
    _gregorianDate->year = year;

    leap_flag = IsLeapYear(_gregorianDate->year);
    for (unsigned short i = 0; i < 13; i++) {
        tmp_days += DayOfMonthCounter[i][leap_flag];
        if (ordinalDays > tmp_days && ordinalDays <= tmp_days + DayOfMonthCounter[i+1][leap_flag]) {
            _gregorianDate->month = i + 1;
            _gregorianDate->day = ordinalDays - tmp_days;
            return ERR_OK;
        }
    }

    return CALENDAR_ERROR_ILLEGAL_ORDINAL_DAYS;
}

/**
 * 根据农历年内序数计算农历日期(特殊的：索引 0 表示上一年最后一天)
 * @param year: 年
 * @param lunarOrdinal: 年内序数
 * @param _lunarDateT: 农历 LunarDate_T 日期结构体指针
 * @return
 */
int GetLunarDateBasedOrdinal(short year, int lunarOrdinal, LunarDate_T *_lunarDateT) {
    int tmp_days = 0;
    unsigned short leap_month = 0;

    PreprocessDayOrdinalWithYear(&year, &lunarOrdinal, LunarCalendar);
    _lunarDateT->year = year;

    leap_month = GetLunarLeapMonth(_lunarDateT->year);
    UpdateLunarDayOfMonth(_lunarDateT->year);

    for (int i = 0; i < 13; i++) {
        tmp_days += LunarDayOfMonth[i];
        if (lunarOrdinal <= tmp_days) {
            _lunarDateT->month = (leap_month && i >= leap_month) ? i : i + 1;
            _lunarDateT->day = lunarOrdinal - tmp_days + LunarDayOfMonth[i];
            _lunarDateT->isLeapMonth = (leap_month && i == leap_month) ? true : false;
            return ERR_OK;
        }
    }

    return CALENDAR_ERROR_ILLEGAL_ORDINAL_DAYS;
}

/**
 * 获取某年干支生肖
 * @param [in]_lunarYear: 所求的年份(公元前用负数表示)
 * @param [out]_gan_zhi: 当年的天干地支
 * @return
 */
int GetGanZhiOfYear(short _lunarYear, unsigned char *_gan_zhi) {
    unsigned char gan = 0;
    unsigned char zhi = 0;
    if (_lunarYear > 0) {
        gan = (_lunarYear - 4) % 10;
        zhi = (_lunarYear - 4) % 12;
    }
    else {
        gan = (_lunarYear + 7) % 10;
        zhi = (_lunarYear + 9) % 12;
    }
    gan = gan < 0 ? gan + 10 : gan;
    zhi = zhi < 0 ? zhi + 12 : zhi;

    *_gan_zhi = gan;
    *_gan_zhi <<= 4;
    *_gan_zhi |= zhi;

    return ERR_OK;
}

/**
 * 获取某年某月干支
 * @note 月干支纪时有两种说法:
 *       (1) 每年的年干从立春开始，月干采用每个地支对应二十四节气自某节气（非中气）至下一个节气，以交节时间决定起始的一个月期间，不是农历某月初一至月底
 *       (2) 直接以农历月区分计算月柱
 * @param [out]_gan_zhi: 所求的年月的天干地支
 * @param [in]_lunarYear: 所求的年月(公元前用负数表示)
 * @param [in]_lunarMonth: 所求的月份
 * @return
 * @todo: 调用时判断日期所属月份
 */
unsigned char GetGanZhiOfMonth(unsigned char *_gan_zhi, short _lunarYear, unsigned short _lunarMonth) {
    // 每年的年干从立春开始，月干采用每个地支对应二十四节气自某节气（非中气）至下一个节气，以交节时间决定起始的一个月期间，不是农历某月初一至月底
    unsigned char gan = 0;
    unsigned char zhi = 0;

    int tmp_offset = _lunarYear > 0 ? -3 : 8;

    gan = (_lunarYear + tmp_offset) % 10;
    gan = gan < 0 ? gan + 10 : gan;
    gan = (gan * 2 + _lunarMonth - 1) % 10;
    zhi = (_lunarMonth + 1) % 12;

    *_gan_zhi = gan;
    *_gan_zhi <<= 4;
    *_gan_zhi |= zhi;

    return *_gan_zhi;
}

/**
 * 获取某天的干支
 * @param [out]_gan_zhi: 所求的日期的天干地支
 * @param [in]_lunarDateT: 农历日期 LunarDate_T 结构体
 * @return
 */
unsigned char GetGanZhiOfDay(unsigned char *_gan_zhi, const LunarDate_T *_lunarDateT) {
    unsigned char gan = 0;
    unsigned char zhi = 0;
    GregorianDate_T gt = {0, 0, 0};
    double julian_days = 0;
    int16_t ganzhi_num = 0;
    unsigned char tmp = 0;

    if (!_gan_zhi) {
        _gan_zhi = &tmp;
    }

    LunarToGregorian(_lunarDateT, &gt);
    GregorianToJulianDays(&gt, &julian_days);
    julian_days += 0.5;

    if (julian_days < 0) {
        ganzhi_num = (int16_t)(((int16_t)(((int64_t)julian_days) % 60) + 60 + JULIAN_START_GANZHI) % 60);
    }
    else {
        ganzhi_num = (int16_t)(((int64_t)julian_days + JULIAN_START_GANZHI) % 60);
    }

    gan = ganzhi_num % 10;
    zhi = ganzhi_num % 12;

    *_gan_zhi = gan;
    *_gan_zhi <<= 4;
    *_gan_zhi |= zhi;

    return *_gan_zhi;
}

/**
 * 获取时干支（24小时纪时）
 * @param [out]_gan_zhi: 所求的日期的天干地支
 * @param [in]_lunarDateT: 农历日期 LunarDate_T 结构体
 * @param [in]hour: 所计算时间
 * @param [in]dis_zi: 是否区分早晚子时 (0-不区分, 1-区分)
 * @note 区分早晚子则夜子(23:00~00:00)应该属于当日的年月日时干支，不区分则夜子(23:00~00:00)属于次日的年月日时干支
 * @return
 */
unsigned char GetGanZhiOfHour(unsigned char *_gan_zhi, const LunarDate_T *_lunarDateT, unsigned short hour, unsigned char dis_zi) {
    unsigned char gan = 0;
    unsigned char zhi = 0;
    unsigned char tmp = 0;
    unsigned char gan_day = 0;
    unsigned short tmp_z = 0;

    if (!_gan_zhi) {
        _gan_zhi = &tmp;
    }
    GetGanZhiOfDay(&gan_day, _lunarDateT);
    gan_day >>= 4;
    gan_day = (!dis_zi && hour >= 23) ? gan_day + 2 : gan_day + 1;
    tmp_z = hour & 0x01 ? 1 : 0;
    zhi = ((hour + tmp_z) / 2 + 1) % 12 - 1;
    gan = (gan_day * 2 + (zhi + 1) - 2) % 10  - 1;

    *_gan_zhi = gan;
    *_gan_zhi <<= 4;
    *_gan_zhi |= zhi;

    return *_gan_zhi;
}


/**
 * 获取公历对应的农历日期
 * @param [in]_gregorianDateT: 公历日期 GregorianDate_T 结构体
 * @param [out]_lunarDateT: 农历日期 LunarDate_T 结构体
 * @return
 */
int GregorianToLunar(const GregorianDate_T *_gregorianDateT, LunarDate_T *_lunarDateT) {
    int gregorian_ordinal = 0;
    int lunar_ordinal = 0;
    short lunar_ordinal_base_new_year = 0;
    GetDayOrdinal(_gregorianDateT, &gregorian_ordinal);
    lunar_ordinal_base_new_year = GetLunarOrdinalInNewYear(_gregorianDateT->year);
    lunar_ordinal = gregorian_ordinal - lunar_ordinal_base_new_year;

    GetLunarDateBasedOrdinal(_gregorianDateT->year, lunar_ordinal, _lunarDateT);

    return ERR_OK;
}

/**
 * 格里高利历转儒略日
 * @param [in]_gregorianDateT: 公历日期 GregorianDate_T 结构体
 * @param [out]julian_days: 儒略日
 * @return
 */
double GregorianToJulianDays(const GregorianDate_T *_gregorianDateT, double *julian_days) {
    int Y = _gregorianDateT->year < 0 ? _gregorianDateT->year + 1 : _gregorianDateT->year;
    int M = _gregorianDateT->month;
    int D = _gregorianDateT->day;
    int B = 0;
    double jd = 0;

    if (!julian_days) {
        julian_days = &jd;
    }
    if ((Y > 1582) || (Y == 1582 && M > 10) || (Y == 1582 && M == 10 && D >= 15)) {
        B = 2 - (int)floor((double)Y / 100) + (int)floor((double)Y / 400);
    }
    if (_gregorianDateT->month <= 2) {
        Y -= 1;
        M += 12;
    }

    // *julian_days = floor(365.25*(Y+4712)) + floor(30.6*(M+1)) + D + B - 63.5;
    *julian_days = floor(365.25*(Y+4716)) + floor(30.6*(M+1)) + D + B - 1524.5;

    return *julian_days;
}


double Round(double x, int y) {
    return floor(x * pow(10, y) + 0.5) / pow(10, y);
}

// 12h起算的日数转时分秒
int day2hms(double JD) {
    double d = JD - floor(JD);
    int h = (int)(d * 24);
    int m = (int)(Round((d * 24 - h) * 60, 4));  // round以防止不确定尾数
    if (m == 60) {
        m = 0;
        h += 1;
    }
    double s = d * 86400 - h * 3600 - m * 60;
    if (fabs(s) < 0.001) {
        s = 0;
    }
    return h;
}

/**
 * 儒略日转格里高利历
 * @param [out]julian_days: 儒略日
 * @param [in]_gregorianDateT: 公历日期 GregorianDate_T 结构体
 * @return
 */
int JulianDaysToGregorian(double julian_days, GregorianDate_T *_gregorianDateT) {
    int A = 0, B = 0, C = 0, D = 0, E = 0, K = 0, Z = 0;
    double day = 0, F = 0;

    // 以BC4713年1月1日0时为历元
    julian_days += 0.5;
    Z = floor(julian_days);

    // 日的小数部分
    F = julian_days - Z;

    // 2299161之前为儒略历, 之后为格里历(将格里历转为儒略历方便计算)
    A = Z < 2299161 ? Z : (int)(Z + 10 + floor((Z - 2305507.25) / 36524.25) - floor(floor((Z - 2305507.25) / 36524.25) /4 ));

    while (true) {
        B = A + 1524;  // 以B.C. 4717年3月1日0时为历元
        C = floor((B - 122.1) / 365.25);  // 积年
        D = floor(365.25 * C);  // 积年的日数
        E = floor((B - D) / 30.6);  // B-D为年内积日，E即月数
        day = B - D - floor(30.6 * E) + F;
        // 当夜24点，实为第二日0点
        if (day2hms(F) == 24) {
            A += 1;
            F = 0;
            continue;
        }
        if (day >= 1) {
            // 否则即在上一月，可前置一日重新计算
            break;
        }
        A -= 1;
        K += 1;
    }

    if ((day += K) == 0) {
        day += 1;
    }
    _gregorianDateT->day = (uint16_t)day;
    _gregorianDateT->month = E < 14 ? E - 1 : E - 13;
    _gregorianDateT->year = (short)(_gregorianDateT->month > 2 ? C - 4716 : C - 4715);
    if (_gregorianDateT->year <= 0) {
        _gregorianDateT->year -= 1;
    }

    return 0;
}

/**
 * 农历日期转公历日期
 * @param [in]_lunarDateT: 农历日期 LunarDate_T 结构体
 * @param [out]_gregorianDateT: 公历日期 GregorianDate_T 结构体
 * @return
 */
int LunarToGregorian(const LunarDate_T *_lunarDateT, GregorianDate_T *_gregorianDateT) {
    int gregorian_ordinal = 0;
    int lunar_ordinal = 0;
    short lunar_ordinal_base_new_year = 0;
    GetLunarDayOrdinal(_lunarDateT, &lunar_ordinal);
    lunar_ordinal_base_new_year = GetLunarOrdinalInNewYear(_lunarDateT->year);
    gregorian_ordinal = lunar_ordinal + lunar_ordinal_base_new_year;

    GetDateBasedOrdinal(_lunarDateT->year, gregorian_ordinal, _gregorianDateT);

    return ERR_OK;
}

/**
 * 得到指定年份的节气信息，首个是小寒
 * @param [in]year: 年份
 * @param [in]month: 月份
 * @param [out]jie_qi: 节气
 * @return
 */
int GetJieQi(short year, unsigned short month, unsigned short jie_qi[2]) {
    int index = 0;                      //对应每公历年首个节气所在字节的索引
    unsigned short wXiaoHanOrder = 0;   //小寒年内序数
    unsigned short wJQData = 0;         //节气数据
    unsigned short wCurJQData = 0;      //当前计算的节气数据
    unsigned short wDays = 0;           //当前节气距离该年小寒的天数
    int i = 0;
    if (year < START_YEAR || year >= END_YEAR || month > 12 || month < 1){
        return CALENDAR_ERROR_ILLEGAL_DATE;
    }
    //对应每公历年首个节气所在字节的索引
    index = (year - START_YEAR) * 6;
    //加上3，转为小寒的年内序数
    wXiaoHanOrder = (unsigned short)(((cSolarTerms[index])>>6)+3);
    wJQData = cSolarTerms[index];
    //当前计算的节气与上一节气的天数差信息
    wCurJQData = (wJQData>>4)&0x03;
    if (month == 1){
        //加1转到日期
        jie_qi[0]= wXiaoHanOrder + 1;
        //大寒：小寒的年内序数加上距离小寒的天数
        jie_qi[1]= wCurJQData + 14 + wXiaoHanOrder + 1;
        return ERR_OK;
    }
    //距离小寒的天数，当前为大寒距离小寒的天数
    wDays = wCurJQData + 14;
    for (i=1; i < month; i++){
        if (i%2 == 0){
            wJQData = cSolarTerms[index+i/2];
        }
        if (i%2 == 1){
            wCurJQData = (wJQData>>2)&0x03;
            wDays += wCurJQData+14;
            wCurJQData = wJQData&0x03;
            wDays += wCurJQData+14;
        }
        else{
            wCurJQData = (wJQData>>6)&0x03;
            wDays += wCurJQData+14;
            wCurJQData = (wJQData>>4)&0x03;
            wDays += wCurJQData+14;
        }
    }
    //wMonth中的第二个节气
//    GetDateBasedOrdinal(year, wDays + wXiaoHanOrder, &month, &jie_qi[1]);
    //第一个节气
//    GetDateBasedOrdinal(year, wDays + wXiaoHanOrder - wCurJQData - 14, &month, &jie_qi[0]);
    return ERR_OK;
}

/**
 * 获取农历月的天数
 * @param [in]lunar_year: 农历年
 * @param [in]lunar_month: 农历月
 * @param [in]is_leap_month: 是否为闰月
 * @return 该农历月的天数，为0代表参数无效
 */
unsigned short GetLunarDaysOfMonth(short lunar_year, unsigned short lunar_month, bool is_leap_month) {

    UpdateLunarDayOfMonth(lunar_year);

    return is_leap_month ? LunarDayOfMonth[lunar_month] : LunarDayOfMonth[lunar_month-1];
}

/**
 * 获取公历日期对应的星期日 (deprecated)
 * @param [in]_gregorianDateT: 公历日期 GregorianDate_T 结构体指针
 * @return week: 对应的星期日
 */
short GetDayOfWeek_deprecated(short year, unsigned short month, unsigned short day) {
    short week = -1;
    short first_week;
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
 * @param [in]_gregorianDateT: 公历日期 GregorianDate_T 结构体指针
 * @return week: 对应的星期日
 */
short GetDayOfWeek(short year, unsigned short month, unsigned short day) {
    short week = -1;
    GregorianDate_T GD = {year, month, day};

    double julian_days = 0;

    GregorianToJulianDays(&GD, &julian_days);
    julian_days += 0.5;

    if (julian_days < 0) {
        // B.C. 4713-01-01 之前的时间
        week = (int16_t)(((int16_t)(((int64_t)julian_days) % 7) + 7 + JULIAN_START_WEEK) % 7);
    }
    else {
        week = (int16_t)(((int64_t)julian_days + JULIAN_START_WEEK) % 7);
    }

    return week;
}

/**
 * 计算公历某个月的天数
 * @param [in]year: 年份
 * @param [in]month: 月份
 * @return
 */
unsigned short GetGreDaysOfMonth(short year, unsigned short month) {
    return DayOfMonthCounter[month][IsLeapYear(year)];
}

/**
 * 计算日期在一年内的星期数(默认一周起始为星期一，以1月4号所在的星期为一年的第一周)
 * @param year: 公历年
 * @param month: 公历月
 * @param day: 公历日
 * @param first_week: 星期的起始(默认周一), 周一为1，周日为0
 * @return
 * @todo: 修改默认第一天
 */
unsigned short GetWeekInYear(const GregorianDate_T *_gregorianDate, unsigned short first_week) {
    unsigned short week_num = 0;

    int days = -1;
    GetDayOrdinal(_gregorianDate, &days);
    short week = GetDayOfWeek(_gregorianDate->year, 1, 1);
    week = (short)(week==0 ? 7 : week);

    if (days == -1) {
        return 0;
    }

    if (week > 4) {
        days = days - (7 - week + 1);
    }
    else {
        days = days + (week - 1);
    }

    int week_num_tmp = days % 7;
    if (week_num_tmp) {
        week_num = days / 7 + 1;
    }else {
        week_num = days / 7;
    }

    return week_num;
}

/**
 * 获取当前时间 保存为 LONGTIME 结构体返回
 * @return: LONGTIME结构体
 */
TimeInfo GetCurTime(TimeInfo *gt){
    struct timeval tv;
    struct timezone tz;

    if (gettimeofday(&tv, &tz) != 0){
        printf("获取时间错误");
    }
    *gt = FormatTimestamp(tv);

    return *gt;
}

/**
 * 将 timeval 结构体的时间转换成 LONGTIME 结构题时间
 * @param origin_time
 * @return
 */
static TimeInfo FormatTimestamp(struct timeval origin_time){
    TimeInfo lt;
    struct tm *t;
    time_t tt = origin_time.tv_sec;
    t = localtime(&tt);
    lt.year = t->tm_year + 1900;
    lt.month = t->tm_mon +1;
    lt.day = t->tm_mday;
    lt.hour = t->tm_hour;
    lt.minute = t->tm_min;
    lt.second = t->tm_sec;
    lt.dayOfWeek = t->tm_wday;
    lt.dayOrdinalOfYear = t->tm_yday;
    lt.isDST = t->tm_isdst;
    lt.millisecond = origin_time.tv_usec/1000;
    lt.microsecond = origin_time.tv_usec - lt.millisecond*1000;

    return lt;
}

/**
 * 显示当前时间
 */
static void ShowNowTime(){
    TimeInfo lt;
    while (true){
        GetCurTime(&lt);
        // printf("%d-%d-%d %2d:%.2d:%.2d %s\r", lt.year, lt.month, lt.day, lt.hour, lt.minute, lt.second, WeekNameCN[lt.dayOfWeek]);

        Sleep(1000);
    }
}

/**
 * 获取控制台光标坐标
 * @param x
 * @param y
 * @return
 */
//static COORD GetConsoleCursorPos(int *x, int *y){
//    COORD pos;
//    CONSOLE_SCREEN_BUFFER_INFO pBuffer;
//    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &pBuffer);
//    *x = pBuffer.dwCursorPosition.X + 1;
//    *y = pBuffer.dwCursorPosition.Y + 1;
//    pos.X = (short)(pBuffer.dwCursorPosition.X + 1);
//    pos.Y = (short)(pBuffer.dwCursorPosition.Y + 1);
//    return pos;
//}

/**
 * 移动光标到指定位置
 * @param x
 * @param y
 */
static bool SetConsoleCursorPos(int x, int y){
    COORD c;
    c.X = (short)(x - 1);
    c.Y = (short)(y - 1);
    return SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),c);
}

/**
 * 设置控制台属性
 * @return
 */
static int SetConsoleAttribute(){
    // Set output mode to handle virtual terminal sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE){
        return (int)GetLastError();
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)){
        return (int)GetLastError();
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode)){
        return (int)GetLastError();
    }

    return (int)GetLastError();
}

int GetGregorianDateInfo(const GregorianDate_T *_gregorianDate, GregorianDateInfo *sunDate) {
    int day_ordinal =0;
    GetDayOrdinal(_gregorianDate, &day_ordinal);

    sunDate->year = _gregorianDate->year;
    sunDate->month = _gregorianDate->month;
    sunDate->day = _gregorianDate->day;
    sunDate->dayOfWeek = GetDayOfWeek(_gregorianDate->year, _gregorianDate->month, _gregorianDate->day);
    sunDate->weekOfYear = GetWeekInYear(_gregorianDate, FIRST_DAY_OF_WEEK);
    sunDate->dayOrdinalOfYear = day_ordinal;

    return ERR_OK;
}

/**
 * 获取公历日期信息
 * @param [in]_lunarDate: 农历日期结构
 * @param [out]moonDate: 农历日期信息
 * @return Error code in ErrorCode.h
 */
int GetLunarDateInfo(const LunarDate_T *_lunarDate, LunarDateInfo *moonDate) {
    UpdateLunarDayOfMonth(_lunarDate->year);

    moonDate->year = _lunarDate->year;
    moonDate->month = _lunarDate->month;
    moonDate->day = _lunarDate->day;
    moonDate->isLeapMonth = _lunarDate->isLeapMonth;
    moonDate->monthDay = LunarDayOfMonth[_lunarDate->month];

    GetGanZhiOfYear(_lunarDate->year, &moonDate->ganZhiYear);
    GetGanZhiOfMonth(&moonDate->ganZhiMon, _lunarDate->year, _lunarDate->month);
    GetGanZhiOfDay(&moonDate->ganZhiDay, _lunarDate);

    return ERR_OK;
}

/**
 * 根据输入的日期获取日期的详细信息
 * @param [out]dateInfo: DateInfo 结构体
 * @param [in]year: 日期的年份
 * @param [in]month: 日期月份
 * @param [in]day: 日期
 * @param [in]calendarType: 日期类型(0: 公历, 1: 农历)
 * @param [in]isLeapMonth： 如果为农历, 表示输入的月份是否为闰月
 * @return
 */
DateInfo GetDateInfo(DateInfo *dateInfo,
                     short year,
                     unsigned short month,
                     unsigned short day,
                     unsigned short calendarType,
                     short isLeapMonth){
    DateInfo tmp_data_info;
    if (!dateInfo) {
        dateInfo = &tmp_data_info;
    }

    GregorianDate_T gregorian_date_t = {0};
    LunarDate_T lunar_date_t = {0};

    switch (calendarType) {
        case LunarCalendar: {
            lunar_date_t.year = year;
            lunar_date_t.month = month;
            lunar_date_t.day = day;
            lunar_date_t.isLeapMonth = isLeapMonth;
            LunarToGregorian(&lunar_date_t, &gregorian_date_t);
        }
        case GregorianCalendar: {
            gregorian_date_t.year = year;
            gregorian_date_t.month = month;
            gregorian_date_t.day = day;
            GregorianToLunar(&gregorian_date_t, &lunar_date_t);
        }
        default:
            break;
    }
    GetLunarDateInfo(&lunar_date_t, &dateInfo->lunarTimeT);
    GetGregorianDateInfo(&gregorian_date_t, &dateInfo->gregorianTimeT);

    return *dateInfo;
}

char *FormatterDefault(char *format_string, DateInfo *date_info) {
    sprintf(format_string,
            "公元 %d年%.2d月%.2d日 %s 第%.2d周 今年第%d天\n农历: %s%s[%s]年 %s%s月 %s%s日 %s%s%s",
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

    return format_string;
}

char *FormatDateInfo(char *format_string, DateInfo *date_info, DateFormatter formatter) {
    if (!formatter) {
        return FormatterDefault(format_string, date_info);
    }
    return formatter(format_string, date_info);
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

