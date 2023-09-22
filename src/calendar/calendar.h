
#ifndef CALENDAR_CALENDAR_H
#define CALENDAR_CALENDAR_H

#include "internal/calendar_types.h"
#include "internal/calendar_export.h"

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


/** 日历或日期类型 */
typedef enum {
    LunarCalendar,          /*!< 农历  */
    GregorianCalendar,      /*!< 公历 (格里高利历)  */
} CalendarType;


CALENDAR_API_PUBLIC bool is_leap_year(int year);
CALENDAR_API_PUBLIC int gre_date_to_ordinal(const FC_DATE *date);
CALENDAR_API_PUBLIC int ordinal_to_date(int ordinal_days, short base_year, FC_DATE *date);
CALENDAR_API_PUBLIC int gregorian_to_lunar(const FC_DATE *gre_date, FC_DATE *lunar_date);
CALENDAR_API_PUBLIC double date_to_JDN(const FC_TIME *time);
CALENDAR_API_PUBLIC int JDN_to_date(double julian_days, FC_TIME *time);
CALENDAR_API_PUBLIC short get_day_of_week_deprecated(const FC_DATE *date);
CALENDAR_API_PUBLIC short get_week(const FC_DATE *date);
CALENDAR_API_PUBLIC unsigned short get_weeks(const FC_DATE *date, unsigned short first_week, unsigned short first_weeks);


CALENDAR_API_PUBLIC unsigned char get_ganzhi_year_deprecated(int year);
CALENDAR_API_PUBLIC unsigned char get_ganzhi_year(int year);
CALENDAR_API_PUBLIC unsigned char get_ganzhi_month(int year, unsigned short month);
CALENDAR_API_PUBLIC unsigned char get_ganzhi_day(const FC_DATE *date);
CALENDAR_API_PUBLIC unsigned char get_ganzhi_hour(const FC_DATE *date, unsigned short hour, unsigned char dis_zi);
CALENDAR_API_PUBLIC int lunar_date_to_ordinal(const FC_DATE *date);
CALENDAR_API_PUBLIC int lunar_ordinal_to_date(int ordinal_days, short base_year, FC_DATE *lunar_date);
CALENDAR_API_PUBLIC int lunar_to_gregorian(const FC_DATE *_lunar_date, FC_DATE *_gregorian_date);
CALENDAR_API_PUBLIC unsigned short lunar_get_total_day_of_year(short year);


// 获取日期(可输入农历或公历)的详细信息
CALENDAR_API_PUBLIC int get_date_info(DATE_INFO *date, int year, unsigned short month, unsigned short day, CalendarType calendar_type, bool is_leap);


#ifdef __cplusplus
}
#endif

#endif //CALENDAR_CALENDAR_H
