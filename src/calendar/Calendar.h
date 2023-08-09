
#ifndef CTOOLS_CALENDAR_H
#define CTOOLS_CALENDAR_H

#include <stdbool.h>
#include "ErrorCode.h"
#include "ctools_export.h"


#ifdef __cplusplus
extern "C" {
#endif


/*! {"甲", "乙", "丙", "丁", "戊", "己", "庚", "辛", "壬", "癸"} */
extern const char *TianGan[];

/*! {"子", "丑", "寅", "卯", "辰", "巳", "午", "未", "申", "酉", "戌", "亥"} */
extern const char *DiZhi[];

/*! {"鼠", "牛", "虎", "兔", "龙", "蛇", "马", "羊", "猴", "鸡", "狗", "猪"} */
extern const char *ShengXiao[];

/*! {
    "立春", "雨水", "惊蛰", "春分", "清明", "谷雨",
    "立夏", "小满", "芒种", "夏至", "小暑", "大暑",
    "立秋", "处暑", "白露", "秋分", "寒露", "霜降",
    "立冬", "小雪", "大雪", "冬至", "小寒", "大寒"
}*/
extern const char *JieQi[];

/*! {
    "初一", "初二", "初三", "初四", "初五", "初六", "初七", "初八", "初九", "初十",
    "十一", "十二", "十三", "十四", "十五", "十六", "十七", "十八", "十九", "二十",
    "廿一", "廿二", "廿三", "廿四", "廿五", "廿六", "廿七", "廿八", "廿九", "三十"
}*/
extern const char *LunarDayName[];

/*! {
    "正月", "二月", "三月", "四月", "五月", "六月",
    "七月", "八月", "九月", "十月", "冬月", "腊月",
    "闰正月", "闰二月", "闰三月", "闰四月", "闰五月", "闰六月",
    "闰七月", "闰八月", "闰九月", "闰十月", "闰冬月", "闰腊月"
}*/
extern const char *LunarMouthName[];

/*! {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"} */
extern const char *WeekNameEN[];

/*! {"星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"} */
extern const char *WeekNameCN[];

typedef struct {
    short year;             /*!< 农历年份 */
    unsigned short month;   /*!< 农历月份 */
    unsigned short day;     /*!< 农历年、月、日 */
    bool isLeapMonth;       /*!< 闰月标志 */
} LunarDate_T;

typedef struct {
    short year;             /*!< 年份 */
    unsigned short month;   /*!< 月份 - 取值区间为[0,11] */
    unsigned short day;     /*!< 日期 - 取值区间为[1,31] */
} GregorianDate_T;


/** 自定义时间结构体 */
typedef struct TimeInfo {
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
} TimeInfo;

/** 农历日期结构体 */
typedef struct LunarDateInfo {
    int year;                       /*!< 农历年份 */
    int month;                      /*!< 农历月份 - 取值范围 [1, 12] */
    int day;                        /*!< 农历日期 - 取值范围 [1, 30] */
    int monthDay;                   /*!< 月天数 - 取值范围 [1, 30] */
    unsigned char ganZhiYear;       /*!< 年份干支, 天干和地支分别占4位, 获取天干向右移4, 获取地支和GANZHI_MASK作 & 运算 */
    unsigned char ganZhiMon;        /*!< 月份干支, 天干和地支分别占4位, 获取天干向右移4, 获取地支和GANZHI_MASK作 & 运算 */
    unsigned char ganZhiDay;        /*!< 日干支, 天干和地支分别占4位, 获取天干向右移4, 获取地支和GANZHI_MASK作 & 运算 */
    bool isLeapMonth;               /*!< 闰月标志 */
} LunarDateInfo;

/** 公历日期结构体 */
typedef struct GregorianDateInfo {
    int year;                   /*!< 年份 */
    int month;                  /*!< 月份 - 取值区间为[0,11] */
    int day;                    /*!< 日期 - 取值区间为[1,31] */
    int dayOfWeek;              /*!< 星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 */
    int weekOfYear;             /*!< 自然周期 – 取值区间为[1,53]，默认一周起始为星期一，1月4号所在的星期为一年的第一周 */
    int dayOrdinalOfYear;       /*!< 从每年的1月1日开始的天数 – 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推 */
    int firstDayOfWeek;         /*!< 一周起始日, 默认设置为周一 */
    int firstWeekOfYear;        /*!< 一年起始周, 默认设置为第一个满4天的周 */
} GregorianDateInfo;

/** 阴阳历日期结构体 */
typedef struct DateInfo {
    GregorianDateInfo gregorian_date;
    LunarDateInfo lunar_date;
} DateInfo;

/** 日历或日期类型 */
typedef enum {
    LunarCalendar,          /*!< 农历  */
    GregorianCalendar,      /*!< 公历 (格里高利历)  */
} CalendarType;

typedef char *(*DateFormatter)(char *string, DateInfo *date_info);


// 获取日期(可输入农历或公历)的详细信息
CALENDAR_API_PUBLIC DateInfo GetDateInfo(
        DateInfo *dateInfo,
        short year,
        unsigned short month,
        unsigned short day,
        unsigned short calendarType,
        short isLeapMonth
);

// 获取当前时间并保存到 TimeInfo
CALENDAR_API_PUBLIC TimeInfo GetCurTime(TimeInfo *gt);

// 判断公历年是否闰年
CALENDAR_API_PUBLIC bool IsLeapYear(short year);

// 获取农历总天数
CALENDAR_API_PUBLIC unsigned short GetLunarTotalDayOfYear(short lunar_year);

// 获取农历日期信息
CALENDAR_API_PUBLIC int GetLunarDateInfo(const LunarDate_T *_lunarDate, LunarDateInfo *lunar_date);

// 格式化日期信息
CALENDAR_API_PUBLIC char *FormatDateInfo(char *format_string, DateInfo *date_info, DateFormatter formatter);

// 计算公历日期在年内的序数(距离元旦的天数, 元旦为序数 1)
CALENDAR_API_PUBLIC ErrorCode GetDayOrdinal(const GregorianDate_T *_gregorianDate, int *ordinalDays);

// 计算农历日期在年内的序数(距离正月初一的天数, 正月初一为序数 1)
CALENDAR_API_PUBLIC ErrorCode GetLunarDayOrdinal(const LunarDate_T *_lunarDateT, int *ordinalDays);

// 从年内序数计算月、日，参数：年，年内序数，月，日，返回值：0-失败，1-成功
CALENDAR_API_PUBLIC ErrorCode GetDateBasedOrdinal(short year, int ordinalDays, GregorianDate_T *_gregorianDate);

// 获取年份对应的干支
CALENDAR_API_PUBLIC int GetGanZhiOfYear(short _lunarYear, unsigned char *_gan_zhi);

// 获取月份对应的干支
CALENDAR_API_PUBLIC unsigned char GetGanZhiOfMonth(unsigned char *_gan_zhi, short _lunarYear, unsigned short _lunarMonth);

// 获取日期对应的干支
CALENDAR_API_PUBLIC unsigned char GetGanZhiOfDay(unsigned char *_gan_zhi, const LunarDate_T *_lunarDateT);

// 获取时干支
CALENDAR_API_PUBLIC unsigned char GetGanZhiOfHour(unsigned char *_gan_zhi, const LunarDate_T *_lunarDateT, unsigned short hour, unsigned char dis_zi);

// 公历转农历，参数：公历年、月、日，农历日期结构体，是否为闰月，返回值：0-失败，1-成功
CALENDAR_API_PUBLIC int GregorianToLunar(const GregorianDate_T *_gregorianDateT, LunarDate_T *_lunarDateT);

// 格里高利历转儒略日, 参数：公历日期结构体
CALENDAR_API_PUBLIC double GregorianToJulianDays(const GregorianDate_T *_gregorianDateT, double *julian_days);

// 农历转公历，参数：农历年、月、日，是否为闰月，公历年、月、日，返回值：0-失败，1-成功
CALENDAR_API_PUBLIC int LunarToGregorian(const LunarDate_T *_lunarDateT, GregorianDate_T *_gregorianDateT);

// 得到指定年份的节气信息，首个是小寒
CALENDAR_API_PUBLIC int GetJieQi(short year, unsigned short month, unsigned short jie_qi[2]);

// 获取农历某一年的闰月情况，参数：农历年，返回值，该年的闰月月份，0表示无闰月
CALENDAR_API_PUBLIC unsigned short GetLunarLeapMonth(short lunar_year);

// 获取农历月的天数，参数：农历年，农历月，是否为闰月，返回值：该农历月的天数，为0代表参数无效
CALENDAR_API_PUBLIC unsigned short GetLunarDaysOfMonth(short lunar_year, unsigned short lunar_month, bool is_leap_month);

// 计算星期，返回-1表示输入的年月日不正确或者超出年份范围
CALENDAR_API_PUBLIC short GetDayOfWeek(short year, unsigned short month, unsigned short day);

// 计算公历某个月的天数，返回天数，如果返回0表示年或月有误
CALENDAR_API_PUBLIC unsigned short GetGreDaysOfMonth(short year, unsigned short month);

// 计算日期在一年内的星期数(默认一周起始为星期一，以1月4号所在的星期为一年的第一周), 参数: 年、月、日、星期的起始星期，返回星期数
CALENDAR_API_PUBLIC unsigned short GetWeekInYear(const GregorianDate_T *_gregorianDate, unsigned short first_week);

// 计算两个日期之间的间隔天数
CALENDAR_API_PUBLIC double CalculateIntervalDays(const GregorianDate_T *from, const GregorianDate_T *to);

#ifdef __cplusplus
}
#endif

#endif //CTOOLS_CALENDAR_H
