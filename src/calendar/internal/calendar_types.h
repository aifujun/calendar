
#ifndef CAL_CALENDAR_TYPES_H
#define CAL_CALENDAR_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>


#define JULIAN_START_YEAR           (-4713)         /*!< 当前儒略周期(7980年)起始年份(儒略历), 下一个儒略周期将开始于 公元3268 年 */
#define JULIAN_TO_GREGORIAN         2299161         /*!< 2299161(儒略日数, 1582年10月4日 --> 1582年10月15日)之前为儒略历, 之后为格里历 */
#define JULIAN_START_WEEK           1               /*!< 当前儒略周期起始日(儒略历: B.C. 4713-01-01)的星期(星期一)  */
#define JULIAN_START_YEAR_GANZHI    24              /*!< 当前儒略周期起始年(儒略历: B.C. 4713-01-01)干支 戊子 (<甲子-->癸亥>干支数排序为<0~59>方便计算) */
#define JULIAN_START_GANZHI         49              /*!< 当前儒略周期起始日(儒略历: B.C. 4713-01-01)干支 癸丑 (<甲子-->癸亥>干支数排序为<0~59>方便计算) */
#define FIRST_DAY_OF_WEEK           (1)
#define FIRST_WEEK_OF_YEAR          (0)
#define MAX_MONTH                   16
#define GANZHI_MASK                 (0xF)           /*!> 计算地支时取值 */

#ifndef ZeroMemory
    #define ZeroMemory(_dest, _size) memset((_dest), 0, (_size))
#endif

/*
typedef enum bool {
    false =  0,
    true = 1,
} bool;
*/


/**
 * 节日类型:
 *     0: 中国传统节日
 */
typedef struct festival {
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


/** 阴阳历日期结构体 */
typedef struct data_info {
    struct {
        int year;                       /*!< 农历年份 */
        int month;                      /*!< 农历月份 - 取值范围 [1, 12] */
        int day;                        /*!< 农历日期 - 取值范围 [1, 30] */
        int monthDay;                   /*!< 月天数 - 取值范围 [1, 30] */
        unsigned char ganZhiYear;       /*!< 年份干支, 天干和地支分别占4位, 获取天干向右移4, 获取地支和GANZHI_MASK作 & 运算 */
        unsigned char ganZhiMon;        /*!< 月份干支, 天干和地支分别占4位, 获取天干向右移4, 获取地支和GANZHI_MASK作 & 运算 */
        unsigned char ganZhiDay;        /*!< 日干支, 天干和地支分别占4位, 获取天干向右移4, 获取地支和GANZHI_MASK作 & 运算 */
        bool isLeapMonth;               /*!< 闰月标志 */
    } lunar;
    struct {
        int year;                   /*!< 年份 */
        int month;                  /*!< 月份 - 取值区间为[0,11] */
        int day;                    /*!< 日期 - 取值区间为[1,31] */
        int dayOfWeek;              /*!< 星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 */
        int weekOfYear;             /*!< 自然周期 – 取值区间为[1,53]，默认一周起始为星期一，1月4号所在的星期为一年的第一周 */
        int dayOrdinalOfYear;       /*!< 从每年的1月1日开始的天数 – 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推 */
        int firstDayOfWeek;         /*!< 一周起始日, 默认设置为周一 */
        int firstWeekOfYear;        /*!< 一年起始周, 默认设置为第一个满4天的周 */
    } gregorian;
} DATE_INFO;


/** 自定义时间结构体 */
typedef struct fc_time {
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
} FC_TIME;

typedef struct fc_date {
    int year;
    unsigned short month;
    unsigned short day;
    bool leap_flag;
    char reserved;
} FC_DATE;


typedef struct lunar_data {
    short year;
    unsigned int raw_data;
    short ordinal;
    short leap_month;
    short month_info[MAX_MONTH];
    short month_days[MAX_MONTH];
} LUNAR_DATA;


#endif //CAL_CALENDAR_TYPES_H
