#include <iostream>
#include <cstdint>
#include "gtest/gtest.h"

#include "../src/calendar/Calendar.h"
#include "../src/calendar/Calendar.c"


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


/** 测试 GetLunarDayOrdinal 获取闰月前的日期序数(2023年闰二月)*/
TEST(TestCalendar, test_GetLunarDayOrdinal_before_leap_month) {
    LunarDate_T lunarDateT = {2023, 2, 22, false};
    int ordinalDays = 0;
    GetLunarDayOrdinal(&lunarDateT, &ordinalDays);
    EXPECT_EQ(ordinalDays, 51);
}

/** 测试 GetLunarDayOrdinal 获取闰月的日期序数(2023年闰二月) */
TEST(TestCalendar, test_GetLunarDayOrdinal_leap_month) {
    LunarDate_T lunarDateT = {2023, 2, 22, true};
    int ordinalDays = 0;
    GetLunarDayOrdinal(&lunarDateT, &ordinalDays);
    EXPECT_EQ(ordinalDays, 81);
}

/** 测试 GetLunarDayOrdinal 获取闰月后的日期序数(2023年闰二月) */
TEST(TestCalendar, test_GetLunarDayOrdinal_after_leap_month) {
    LunarDate_T lunarDateT = {2023, 3, 22, false};
    int ordinalDays = 0;
    GetLunarDayOrdinal(&lunarDateT, &ordinalDays);
    EXPECT_EQ(ordinalDays, 110);
}

TEST(TestCalendar, test_UpdateLunarDayOfMonth_2023) {
    uint16_t LunarMonthDay2023[13] = {29, 30, 29, 29, 30, 30, 29, 30, 30, 29, 30, 29, 30};
    short lunar_year = 2023;
    UpdateLunarDayOfMonth(lunar_year);
    EXPECT_TRUE(ArraysMatch(LunarDayOfMonth, LunarMonthDay2023));
}

TEST(TestCalendar, test_GetDateBasedOrdinal_future) {
    GregorianDate_T gregorianDateT = {0};
    GetDateBasedOrdinal(2023, 852, &gregorianDateT);
    EXPECT_EQ(2025, gregorianDateT.year);
    EXPECT_EQ(5, gregorianDateT.month);
    EXPECT_EQ(1, gregorianDateT.day);
}

TEST(TestCalendar, test_GetDateBasedOrdinal_past) {
    GregorianDate_T gregorianDateT = {0};
    GetDateBasedOrdinal(2023, -730, &gregorianDateT);
    EXPECT_EQ(2020, gregorianDateT.year);
    EXPECT_EQ(12, gregorianDateT.month);
    EXPECT_EQ(31, gregorianDateT.day);
}

TEST(TestCalendar, test_GetLunarDateBasedOrdinal_future) {
    LunarDate_T lunarDateT = {0};
    GetLunarDateBasedOrdinal(2023, 384, &lunarDateT);
    EXPECT_EQ(2023, lunarDateT.year);
    EXPECT_EQ(12, lunarDateT.month);
    EXPECT_EQ(30, lunarDateT.day);
    EXPECT_FALSE(lunarDateT.isLeapMonth);
}

TEST(TestCalendar, test_GetLunarDateBasedOrdinal_future_leap) {
    LunarDate_T lunarDateT = {0};
    GetLunarDateBasedOrdinal(2023, 62, &lunarDateT);
    EXPECT_EQ(2023, lunarDateT.year);
    EXPECT_EQ(2, lunarDateT.month);
    EXPECT_EQ(3, lunarDateT.day);
    EXPECT_TRUE(lunarDateT.isLeapMonth);
}

TEST(TestCalendar, test_GetLunarDateBasedOrdinal_past) {
    LunarDate_T lunarDateT = {0};
    GetLunarDateBasedOrdinal(2023, -1093, &lunarDateT);
    EXPECT_EQ(2019, lunarDateT.year);
    EXPECT_EQ(12, lunarDateT.month);
    EXPECT_EQ(30, lunarDateT.day);
    EXPECT_FALSE(lunarDateT.isLeapMonth);
}

TEST(TestCalendar, test_GetLunarDateBasedOrdinal_past_leap) {
    LunarDate_T lunarDateT = {0};
    GetLunarDateBasedOrdinal(2023, -965, &lunarDateT);
    EXPECT_EQ(2020, lunarDateT.year);
    EXPECT_EQ(4, lunarDateT.month);
    EXPECT_EQ(9, lunarDateT.day);
    EXPECT_TRUE(lunarDateT.isLeapMonth);
}

TEST(TestCalendar, test_GregorianToLunar) {
    LunarDate_T lunarDateT = {0};
    GregorianDate_T gregorianDateT = {2023, 4, 14};
    GregorianToLunar(&gregorianDateT, &lunarDateT);
    EXPECT_EQ(2023, lunarDateT.year);
    EXPECT_EQ(2, lunarDateT.month);
    EXPECT_EQ(24, lunarDateT.day);
    EXPECT_TRUE(lunarDateT.isLeapMonth);
}

TEST(TestCalendar, test_LunarToGregorian) {
    LunarDate_T lunarDateT = {2023, 2, 24, true};
    GregorianDate_T gregorianDateT = {0};
    LunarToGregorian(&lunarDateT, &gregorianDateT);
    EXPECT_EQ(2023, gregorianDateT.year);
    EXPECT_EQ(4, gregorianDateT.month);
    EXPECT_EQ(14, gregorianDateT.day);
}

TEST(TestCalendar, test_GetGanZhiOfDay) {
    LunarDate_T lt = {0, 0, 0, false};

    lt = {2023, 2, 24, true};
    EXPECT_EQ(0x82, GetGanZhiOfDay(nullptr, &lt));

    lt = {2023, 6, 19, true};
    EXPECT_EQ(0x17, GetGanZhiOfDay(nullptr, &lt));

    lt = {1582, 9, 18, false};
    EXPECT_EQ(0x99, GetGanZhiOfDay(nullptr, &lt));
}


TEST(TestCalendar, test_GetGanZhiOfHour) {
    LunarDate_T lt = {0, 0, 0, false};

    lt = {2023, 6, 22, false};
    EXPECT_EQ(0x46, GetGanZhiOfHour(nullptr, &lt, 12, 0));
    EXPECT_EQ(0x46, GetGanZhiOfHour(nullptr, &lt, 12, 1));

    lt = {2023, 2, 6, true};
    EXPECT_EQ(0x20, GetGanZhiOfHour(nullptr, &lt, 23, 0));
    EXPECT_EQ(0x00, GetGanZhiOfHour(nullptr, &lt, 23, 1));

    lt = {2023, 6, 22, false};
    EXPECT_EQ(0x80, GetGanZhiOfHour(nullptr, &lt, 0, 0));
    EXPECT_EQ(0x80, GetGanZhiOfHour(nullptr, &lt, 0, 1));

    lt = {2023, 6, 22, false};
    EXPECT_EQ(0x00, GetGanZhiOfHour(nullptr, &lt, 23, 0));
    EXPECT_EQ(0x80, GetGanZhiOfHour(nullptr, &lt, 23, 1));
}
