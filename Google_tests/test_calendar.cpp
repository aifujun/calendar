#include <iostream>
#include <cstdint>
#include "gtest/gtest.h"

#include "../src/calendar/calendar.h"


template<typename T, size_t size>
::testing::AssertionResult ArraysMatch(const T (&expected)[size], const T (&actual)[size]) {
    for (size_t i(0); i < size; ++i) {
        if (expected[i] != actual[i]) {
            return ::testing::AssertionFailure() << "array[" << i
                                                 << "] (" << actual[i] << ") != expected[" << i
                                                 << "] (" << expected[i] << ")";
        }
    }

    return ::testing::AssertionSuccess();
}


/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

/** 测试 is_leap_year */
TEST(TestGregorianCalendar, test_is_leap_year) {
    EXPECT_TRUE(is_leap_year(-1));
    EXPECT_TRUE(is_leap_year(200));
    EXPECT_TRUE(is_leap_year(700));
    EXPECT_TRUE(is_leap_year(2020));
    EXPECT_TRUE(is_leap_year(2000));
    EXPECT_TRUE(is_leap_year(172800));

    EXPECT_FALSE(is_leap_year(-4));
    EXPECT_FALSE(is_leap_year(1900));
    EXPECT_FALSE(is_leap_year(2023));
    EXPECT_FALSE(is_leap_year(6400));
}

/** 测试 gre_date_to_ordinal */
TEST(TestGregorianCalendar, test_gre_date_to_ordinal) {
    FC_DATE date = {0};

    date = {2023, 1, 22, false};
    EXPECT_EQ(gre_date_to_ordinal(&date), 22);

    date = {2023, 3, 1, false};
    EXPECT_EQ(gre_date_to_ordinal(&date), 60);

    date = {2020, 3, 1, true};
    EXPECT_EQ(gre_date_to_ordinal(&date), 61);
}

/** 测试 ordinal_to_date */
TEST(TestGregorianCalendar, test_ordinal_to_date) {
    FC_DATE date = {0};

    ordinal_to_date(22, 2023, &date);
    EXPECT_EQ(2023, date.year);
    EXPECT_EQ(1, date.month);
    EXPECT_EQ(22, date.day);
    EXPECT_FALSE(date.leap_flag);

    ordinal_to_date(60, 2023, &date);
    EXPECT_EQ(2023, date.year);
    EXPECT_EQ(3, date.month);
    EXPECT_EQ(1, date.day);
    EXPECT_FALSE(date.leap_flag);

    ordinal_to_date(61, 2020, &date);
    EXPECT_EQ(2020, date.year);
    EXPECT_EQ(3, date.month);
    EXPECT_EQ(1, date.day);
    EXPECT_TRUE(date.leap_flag);

    ordinal_to_date(-3, 2023, &date);
    EXPECT_EQ(2022, date.year);
    EXPECT_EQ(12, date.month);
    EXPECT_EQ(29, date.day);
    EXPECT_FALSE(date.leap_flag);

    ordinal_to_date(-366, 2023, &date);
    EXPECT_EQ(2021, date.year);
    EXPECT_EQ(12, date.month);
    EXPECT_EQ(31, date.day);
    EXPECT_FALSE(date.leap_flag);
}

/** 测试 gregorian_to_lunar */
TEST(TestGregorianCalendar, test_gregorian_to_lunar) {
    FC_DATE gre_date = {0};
    FC_DATE lunar_date = {0};

    gre_date = {2023, 1, 20, false};
    gregorian_to_lunar(&gre_date, &lunar_date);
    EXPECT_EQ(2022, lunar_date.year);
    EXPECT_EQ(12, lunar_date.month);
    EXPECT_EQ(29, lunar_date.day);
    EXPECT_FALSE(lunar_date.leap_flag);

    gre_date = {2023, 1, 22, false};
    gregorian_to_lunar(&gre_date, &lunar_date);
    EXPECT_EQ(2023, lunar_date.year);
    EXPECT_EQ(1, lunar_date.month);
    EXPECT_EQ(1, lunar_date.day);
    EXPECT_FALSE(lunar_date.leap_flag);

    gre_date = {2023, 1, 24, false};
    gregorian_to_lunar(&gre_date, &lunar_date);
    EXPECT_EQ(2023, lunar_date.year);
    EXPECT_EQ(1, lunar_date.month);
    EXPECT_EQ(3, lunar_date.day);
    EXPECT_FALSE(lunar_date.leap_flag);

    gre_date = {2023, 3, 26, false};
    gregorian_to_lunar(&gre_date, &lunar_date);
    EXPECT_EQ(2023, lunar_date.year);
    EXPECT_EQ(2, lunar_date.month);
    EXPECT_EQ(5, lunar_date.day);
    EXPECT_TRUE(lunar_date.leap_flag);

    gre_date = {2023, 6, 1, false};
    gregorian_to_lunar(&gre_date, &lunar_date);
    EXPECT_EQ(2023, lunar_date.year);
    EXPECT_EQ(4, lunar_date.month);
    EXPECT_EQ(14, lunar_date.day);
    EXPECT_FALSE(lunar_date.leap_flag);
}

/** 测试 gregorian_to_lunar */
TEST(TestGregorianCalendar, test_gregorian_to_lunar_special) {
    FC_DATE gre_date = {0};
    FC_DATE lunar_date = {0};

    gre_date = {699, 11, 28, false};
    gregorian_to_lunar(&gre_date, &lunar_date);
    EXPECT_EQ(700, lunar_date.year);
    EXPECT_EQ(1, lunar_date.month);
    EXPECT_EQ(2, lunar_date.day);
    EXPECT_FALSE(lunar_date.leap_flag);

    gre_date = {700, 1, 1, false};
    gregorian_to_lunar(&gre_date, &lunar_date);
    EXPECT_EQ(700, lunar_date.year);
    EXPECT_EQ(12, lunar_date.month);
    EXPECT_EQ(6, lunar_date.day);
    EXPECT_FALSE(lunar_date.leap_flag);

    gre_date = {701, 1, 26, false};
    gregorian_to_lunar(&gre_date, &lunar_date);
    EXPECT_EQ(700, lunar_date.year);
    EXPECT_EQ(12, lunar_date.month);
    EXPECT_EQ(13, lunar_date.day);
    EXPECT_TRUE(lunar_date.leap_flag);

    gre_date = {700, 8, 20, true};
    gregorian_to_lunar(&gre_date, &lunar_date);
    EXPECT_EQ(700, lunar_date.year);
    EXPECT_EQ(7, lunar_date.month);
    EXPECT_EQ(2, lunar_date.day);
    EXPECT_TRUE(lunar_date.leap_flag);
}

/** 测试 date_to_JDN */
TEST(TestGregorianCalendar, test_date_to_JDN) {
    FC_TIME ctime = {0};

    ctime = {.year=1987, .month=1, .day=27};
    EXPECT_EQ(date_to_JDN(&ctime), 2446822.5);

    ctime = {.year=-1001, .month=2, .day=29};
    EXPECT_EQ(date_to_JDN(&ctime), 1355866.5);

    ctime = {.year=-4713, .month=1, .day=2};
    EXPECT_EQ(date_to_JDN(&ctime), 0.5);

    ctime = {.year=1900, .month=1, .day=1};
    EXPECT_EQ(date_to_JDN(&ctime), 2415020.5);

    ctime = {.year=2023, .month=9, .day=22};
    EXPECT_EQ(date_to_JDN(&ctime), 2460209.5);

    ctime = {.year=2023, .month=9, .day=22, .hour=12};
    EXPECT_EQ(date_to_JDN(&ctime), 2460210);

    ctime = {.year=2023, .month=9, .day=22, .hour=12, .minute=30, .second=15, .millisecond=300, .microsecond=500};
    EXPECT_EQ(date_to_JDN(&ctime), 2460210.0210104226);

    ctime = {.year=1582, .month=10, .day=4, .hour=12};
    EXPECT_EQ(date_to_JDN(&ctime), 2299160);

    ctime = {.year=1582, .month=10, .day=15, .hour=12};
    EXPECT_EQ(date_to_JDN(&ctime), 2299161);

    ctime = {.year=1583, .month=1, .day=1, .hour=12};
    EXPECT_EQ(date_to_JDN(&ctime), 2299239);
}

/** 测试 JDN_to_date */
TEST(TestGregorianCalendar, test_JDN_to_date) {
    FC_TIME ctime = {0};

    JDN_to_date(2460209.5, &ctime);
    EXPECT_EQ(ctime.year, 2023);
    EXPECT_EQ(ctime.month, 9);
    EXPECT_EQ(ctime.day, 22);

    JDN_to_date(2460210, &ctime);
    EXPECT_EQ(ctime.year, 2023);
    EXPECT_EQ(ctime.month, 9);
    EXPECT_EQ(ctime.day, 22);
    EXPECT_EQ(ctime.hour, 12);

    JDN_to_date(2460210.0210104226, &ctime);
    EXPECT_EQ(ctime.year, 2023);
    EXPECT_EQ(ctime.month, 9);
    EXPECT_EQ(ctime.day, 22);
    EXPECT_EQ(ctime.hour, 12);
    EXPECT_EQ(ctime.minute, 30);
    EXPECT_EQ(ctime.second, 15);
    EXPECT_EQ(ctime.millisecond, 300);

    JDN_to_date(2299160, &ctime);
    EXPECT_EQ(ctime.year, 1582);
    EXPECT_EQ(ctime.month, 10);
    EXPECT_EQ(ctime.day, 4);
    EXPECT_EQ(ctime.hour, 12);

    JDN_to_date(2299161, &ctime);
    EXPECT_EQ(ctime.year, 1582);
    EXPECT_EQ(ctime.month, 10);
    EXPECT_EQ(ctime.day, 15);
    EXPECT_EQ(ctime.hour, 12);
}

/** 测试 get_week */
TEST(TestGregorianCalendar, test_get_week) {
    FC_DATE gre_date = {0};

    gre_date = {2023, 9, 18, false};
    EXPECT_EQ(get_week(&gre_date), 1);

    gre_date = {2023, 9, 19, false};
    EXPECT_EQ(get_week(&gre_date), 2);

    gre_date = {2023, 9, 20, false};
    EXPECT_EQ(get_week(&gre_date), 3);

    gre_date = {2023, 9, 21, false};
    EXPECT_EQ(get_week(&gre_date), 4);

    gre_date = {2023, 9, 22, false};
    EXPECT_EQ(get_week(&gre_date), 5);

    gre_date = {2023, 9, 23, false};
    EXPECT_EQ(get_week(&gre_date), 6);

    gre_date = {2023, 9, 24, false};
    EXPECT_EQ(get_week(&gre_date), 0);

    gre_date = {1989, 12, 31, false};
    EXPECT_EQ(get_week(&gre_date), 0);

    gre_date = {1900, 1, 1, false};
    EXPECT_EQ(get_week(&gre_date), 1);

    gre_date = {1582, 10, 4, false};
    EXPECT_EQ(get_week(&gre_date), 4);

    gre_date = {1582, 10, 15, false};
    EXPECT_EQ(get_week(&gre_date), 5);

    gre_date = {-100, 1, 15, false};
    EXPECT_EQ(get_week(&gre_date), 0);

    gre_date = {-4713, 1, 1, true};
    EXPECT_EQ(get_week(&gre_date), 1);

    gre_date = {-4714, 12, 21, true};
    EXPECT_EQ(get_week(&gre_date), 4);
}

/** 测试 get_weeks */
TEST(TestGregorianCalendar, test_get_weeks) {
    FC_DATE gre_date = {0};

    gre_date = {2023, 9, 22, false};
    EXPECT_EQ(get_weeks(&gre_date, 0, 0), 38);
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

/** 测试 get_ganzhi_year */
TEST(TestLunarCalendar, test_get_ganzhi_year) {
    EXPECT_EQ(get_ganzhi_year(2023), 0x93);
    EXPECT_EQ(get_ganzhi_year(1582), 0x86);
    EXPECT_EQ(get_ganzhi_year(700), 0x60);
    EXPECT_EQ(get_ganzhi_year(1), 0x79);
    EXPECT_EQ(get_ganzhi_year(-1), 0x68);
    EXPECT_EQ(get_ganzhi_year(-1000), 0x75);
    EXPECT_EQ(get_ganzhi_year(-4713), 0x40);
}

/** 测试 get_ganzhi_month */
TEST(TestLunarCalendar, test_get_ganzhi_month) {
    EXPECT_EQ(get_ganzhi_month(2023, 8), 0x79);
    EXPECT_EQ(get_ganzhi_month(1582, 8), 0x59);
    EXPECT_EQ(get_ganzhi_month(700, 8), 0x19);
    EXPECT_EQ(get_ganzhi_month(1, 8), 0x39);
    EXPECT_EQ(get_ganzhi_month(-1, 8), 0x19);
    EXPECT_EQ(get_ganzhi_month(-1000, 8), 0x39);
    EXPECT_EQ(get_ganzhi_month(-4713, 8), 0x79);
}

/** 测试 get_ganzhi_day */
TEST(TestLunarCalendar, test_get_ganzhi_day) {
    FC_DATE lunar_date = {0};

    lunar_date = {2023, 8, 8, false};
    EXPECT_EQ(get_ganzhi_day(&lunar_date), 0x97);

    lunar_date = {1582, 9, 18, false};
    EXPECT_EQ(get_ganzhi_day(&lunar_date), 0x99);

    lunar_date = {1582, 9, 19, false};
    EXPECT_EQ(get_ganzhi_day(&lunar_date), 0x0A);

    lunar_date = {700, 1, 2, false};
    EXPECT_EQ(get_ganzhi_day(&lunar_date), 0x80);

    lunar_date = {700, 1, 2, true};
    EXPECT_EQ(get_ganzhi_day(&lunar_date), 0x80);

    lunar_date = {1, 1, 2, false};
    EXPECT_EQ(get_ganzhi_day(&lunar_date), 0x68);

    lunar_date = {-1, 1, 2, false};
    EXPECT_EQ(get_ganzhi_day(&lunar_date), 0x22);

    lunar_date = {-1000, 1, 2, false};
    EXPECT_EQ(get_ganzhi_day(&lunar_date), 0x26);

    lunar_date = {-4713, 1, 2, false};
    EXPECT_EQ(get_ganzhi_day(&lunar_date), 0x8A);
}

/** 测试 get_ganzhi_hour */
TEST(TestLunarCalendar, test_get_ganzhi_hour) {
    FC_DATE lunar_date = {0};

    lunar_date = {2023, 8, 8, false};
    EXPECT_EQ(get_ganzhi_hour(&lunar_date, 12, 0), 0x46);

    lunar_date = {2023, 2, 6, true};
    EXPECT_EQ(get_ganzhi_hour(&lunar_date, 23, 0), 0x20);
    EXPECT_EQ(get_ganzhi_hour(&lunar_date, 23, 1), 0x00);

    lunar_date = {2023, 6, 22, false};
    EXPECT_EQ(get_ganzhi_hour(&lunar_date, 0, 0), 0x80);
    EXPECT_EQ(get_ganzhi_hour(&lunar_date, 0, 1), 0x80);

    lunar_date = {2023, 6, 22, false};
    EXPECT_EQ(get_ganzhi_hour(&lunar_date, 23, 0), 0x00);
    EXPECT_EQ(get_ganzhi_hour(&lunar_date, 23, 1), 0x80);
}

/** 测试 lunar_date_to_ordinal */
TEST(TestLunarCalendar, test_lunar_date_to_ordinal) {
    FC_DATE lunar_date = {0};

    // 获取闰月前的日期序数(2023年闰二月)
    lunar_date = {2023, 2, 22, false};
    EXPECT_EQ(lunar_date_to_ordinal(&lunar_date), 51);

    // 获取闰月的日期序数(2023年闰二月)
    lunar_date = {2023, 2, 22, true};
    EXPECT_EQ(lunar_date_to_ordinal(&lunar_date), 81);

    // 获取闰月后的日期序数(2023年闰二月)
    lunar_date = {2023, 3, 22, false};
    EXPECT_EQ(lunar_date_to_ordinal(&lunar_date), 110);
}

/** 测试 lunar_date_to_ordinal 特殊日期的序数获取 */
TEST(TestLunarCalendar, test_lunar_date_to_ordinal_special) {
    FC_DATE lunar_date = {0};

    lunar_date = {700, 12, 22, false};
    EXPECT_EQ(lunar_date_to_ordinal(&lunar_date), 52);

    lunar_date = {700, 12, 22, true};
    EXPECT_EQ(lunar_date_to_ordinal(&lunar_date), 436);

    lunar_date = {700, 3, 22, false};
    EXPECT_EQ(lunar_date_to_ordinal(&lunar_date), 141);
}

/** 测试 lunar_ordinal_to_date */
TEST(TestLunarCalendar, test_lunar_ordinal_to_date) {
    FC_DATE lunar_date = {0};

    lunar_ordinal_to_date(-5, 2023, &lunar_date);
    EXPECT_EQ(2022, lunar_date.year);
    EXPECT_EQ(12, lunar_date.month);
    EXPECT_EQ(26, lunar_date.day);
    EXPECT_FALSE(lunar_date.leap_flag);

    lunar_ordinal_to_date(51, 2023, &lunar_date);
    EXPECT_EQ(2023, lunar_date.year);
    EXPECT_EQ(2, lunar_date.month);
    EXPECT_EQ(22, lunar_date.day);
    EXPECT_FALSE(lunar_date.leap_flag);

    lunar_ordinal_to_date(81, 2023, &lunar_date);
    EXPECT_EQ(2023, lunar_date.year);
    EXPECT_EQ(2, lunar_date.month);
    EXPECT_EQ(22, lunar_date.day);
    EXPECT_TRUE(lunar_date.leap_flag);

    lunar_ordinal_to_date(110, 2023, &lunar_date);
    EXPECT_EQ(2023, lunar_date.year);
    EXPECT_EQ(3, lunar_date.month);
    EXPECT_EQ(22, lunar_date.day);
    EXPECT_FALSE(lunar_date.leap_flag);
}

/** 测试 lunar_ordinal_to_date 特殊日期的序数获取 */
TEST(TestLunarCalendar, test_lunar_ordinal_to_date_special) {
    FC_DATE lunar_date = {0};

    lunar_ordinal_to_date(52, 700, &lunar_date);
    EXPECT_EQ(700, lunar_date.year);
    EXPECT_EQ(12, lunar_date.month);
    EXPECT_EQ(22, lunar_date.day);
    EXPECT_FALSE(lunar_date.leap_flag);

    lunar_ordinal_to_date(436, 700, &lunar_date);
    EXPECT_EQ(700, lunar_date.year);
    EXPECT_EQ(12, lunar_date.month);
    EXPECT_EQ(22, lunar_date.day);
    EXPECT_TRUE(lunar_date.leap_flag);

    lunar_ordinal_to_date(141, 700, &lunar_date);
    EXPECT_EQ(700, lunar_date.year);
    EXPECT_EQ(3, lunar_date.month);
    EXPECT_EQ(22, lunar_date.day);
    EXPECT_FALSE(lunar_date.leap_flag);
}

/** 测试 lunar_to_gregorian */
TEST(TestLunarCalendar, test_lunar_to_gregorian) {
    FC_DATE gre_date = {0};
    FC_DATE lunar_date = {0};

    lunar_date = {2023, 1, 2, false};
    lunar_to_gregorian(&lunar_date, &gre_date);
    EXPECT_EQ(2023, gre_date.year);
    EXPECT_EQ(1, gre_date.month);
    EXPECT_EQ(23, gre_date.day);
    EXPECT_FALSE(gre_date.leap_flag);

    lunar_date = {2023, 2, 26, false};
    lunar_to_gregorian(&lunar_date, &gre_date);
    EXPECT_EQ(2023, gre_date.year);
    EXPECT_EQ(3, gre_date.month);
    EXPECT_EQ(17, gre_date.day);
    EXPECT_FALSE(gre_date.leap_flag);

    lunar_date = {2023, 2, 26, true};
    lunar_to_gregorian(&lunar_date, &gre_date);
    EXPECT_EQ(2023, gre_date.year);
    EXPECT_EQ(4, gre_date.month);
    EXPECT_EQ(16, gre_date.day);
    EXPECT_FALSE(gre_date.leap_flag);

    lunar_date = {-4713, 1, 7, false};
    lunar_to_gregorian(&lunar_date, &gre_date);
    EXPECT_EQ(-4713, gre_date.year);
    EXPECT_EQ(3, gre_date.month);
    EXPECT_EQ(15, gre_date.day);
    EXPECT_TRUE(gre_date.leap_flag);
}

/** 测试 lunar_to_gregorian, 特殊日期的转换 */
TEST(TestLunarCalendar, test_lunar_to_gregorian_special) {
    FC_DATE gre_date = {0};
    FC_DATE lunar_date = {0};

    lunar_date = {700, 12, 2, false};
    lunar_to_gregorian(&lunar_date, &gre_date);
    EXPECT_EQ(699, gre_date.year);
    EXPECT_EQ(12, gre_date.month);
    EXPECT_EQ(28, gre_date.day);
    EXPECT_FALSE(gre_date.leap_flag);

    lunar_date = {700, 3, 3, false};
    lunar_to_gregorian(&lunar_date, &gre_date);
    EXPECT_EQ(700, gre_date.year);
    EXPECT_EQ(3, gre_date.month);
    EXPECT_EQ(27, gre_date.day);
    EXPECT_TRUE(gre_date.leap_flag);

    lunar_date = {700, 12, 2, true};
    lunar_to_gregorian(&lunar_date, &gre_date);
    EXPECT_EQ(701, gre_date.year);
    EXPECT_EQ(1, gre_date.month);
    EXPECT_EQ(15, gre_date.day);
    EXPECT_FALSE(gre_date.leap_flag);
}

/** 测试 lunar_get_total_day_of_year */
TEST(TestLunarCalendar, test_lunar_get_total_day_of_year) {
    EXPECT_EQ(lunar_get_total_day_of_year(2023), 384);
    EXPECT_EQ(lunar_get_total_day_of_year(700), 444);
    EXPECT_EQ(lunar_get_total_day_of_year(1), 355);
    EXPECT_EQ(lunar_get_total_day_of_year(-1), 354);
    EXPECT_EQ(lunar_get_total_day_of_year(-4713), 383);
}

/** 测试 get_date_info */
TEST(TestLunarCalendar, test_get_date_info) {
    DATE_INFO date = {0};

    get_date_info(&date, 2023, 9, 22, GregorianCalendar, false);
    EXPECT_EQ(date.gregorian.year, 2023);
    EXPECT_EQ(date.gregorian.month, 9);
    EXPECT_EQ(date.gregorian.day, 22);
    EXPECT_EQ(date.gregorian.dayOfWeek, 5);
    EXPECT_EQ(date.gregorian.weekOfYear, 38);
    EXPECT_EQ(date.gregorian.dayOrdinalOfYear, 265);
    EXPECT_EQ(date.gregorian.firstDayOfWeek, 1);
    EXPECT_EQ(date.gregorian.firstWeekOfYear, 0);

    EXPECT_EQ(date.lunar.year, 2023);
    EXPECT_EQ(date.lunar.month, 8);
    EXPECT_EQ(date.lunar.day, 8);
    EXPECT_EQ(date.lunar.monthDay, 30);
    EXPECT_EQ(date.lunar.ganZhiYear, 0x93);
    EXPECT_EQ(date.lunar.ganZhiMon, 0x79);
    EXPECT_EQ(date.lunar.ganZhiDay, 0x97);
    EXPECT_FALSE(date.lunar.isLeapMonth);


    get_date_info(&date, 2023, 8, 8, LunarCalendar, false);
    EXPECT_EQ(date.gregorian.year, 2023);
    EXPECT_EQ(date.gregorian.month, 9);
    EXPECT_EQ(date.gregorian.day, 22);
    EXPECT_EQ(date.gregorian.dayOfWeek, 5);
    EXPECT_EQ(date.gregorian.weekOfYear, 38);
    EXPECT_EQ(date.gregorian.dayOrdinalOfYear, 265);
    EXPECT_EQ(date.gregorian.firstDayOfWeek, 1);
    EXPECT_EQ(date.gregorian.firstWeekOfYear, 0);

    EXPECT_EQ(date.lunar.year, 2023);
    EXPECT_EQ(date.lunar.month, 8);
    EXPECT_EQ(date.lunar.day, 8);
    EXPECT_EQ(date.lunar.monthDay, 30);
    EXPECT_EQ(date.lunar.ganZhiYear, 0x93);
    EXPECT_EQ(date.lunar.ganZhiMon, 0x79);
    EXPECT_EQ(date.lunar.ganZhiDay, 0x97);
    EXPECT_FALSE(date.lunar.isLeapMonth);
}


