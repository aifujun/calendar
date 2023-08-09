# 📆 万年历

[TOC]

---


## **主要功能**

### 1.  *查询日期(具体某一天)的详细信息*

- API

```c
// 获取日期(可输入农历或公历)的详细信息
CALENDAR_API_PUBLIC DateInfo GetDateInfo(
        DateInfo *dateInfo,
        short year,
        unsigned short month,
        unsigned short day,
        unsigned short calendarType,
        short isLeapMonth
);
```



- <font color="red">输入：</font>`公历日期`或`农历日期`

- [x] <font color="blue">输出：</font>农历的天干地支（`年干支、月干支、日干支）`和`生肖`

- [ ] <font color="blue">输出：</font>当天是否是`节气`：如果是，则显示，不是，则显示距离它最近的节气的间隔日期

- [ ] <font color="blue">输出：</font>`距离今年结束还有多少天`

- [x] <font color="blue">输出：</font>日期是当年第几周

    

### 2. *查询月历*

- <font color="red">输入：</font>(公历或农历)`年月`或`年月日`
  
- [ ] <font color="blue">输出：</font>`对应月份的月历`
### 3. *查询年历*

- <font color="red">输入：</font>(公历或农历)`年份`或`年月日`日期
- [ ] <font color="blue">输出：</font>`当年的年历`

### 4. *查询某天距今天的天数*
- <font color="red">输入：</font>(公历或农历)`年月日`
- [ ] <font color="blue">输出：</font>`距离今天的天数`
### 5. *查询距今天相应天数的日期*
- <font color="red">输入：</font>`距离今天的天数`
- [ ] <font color="blue">输出：</font>距离输入天数的`日期（之前、之后）`
### 6. *查询任意两天之间的天数差*
- <font color="red">输入：</font>`两个具体的日期`
- [ ] <font color="blue">输出：</font>`两者之间间隔的天数`
### 7. *查询二十四节气*
- <font color="red">输入：</font>`查询的年份`
- [ ] <font color="blue">输出：</font>`当年的所有节气，和对应的日期、时间`
### 8. *查询节日*
- <font color="red">输入：</font>`节日名称`
- [ ] <font color="blue">输出：</font>`公历节日`按月份输出；`农历节日`直接显示
### 9. *查询吉日*
- <font color="red">输入：</font>(公历或农历)`年份`或`年月日`日期
- [ ] <font color="blue">输出：</font>`前5个和后25个`
### 10. *查询星座*
- <font color="red">输入：</font>(公历或农历)`年月日`日期
- [ ] <font color="blue">输出：</font>`当日的星座`

- <font color="red">输入：</font>查询星座
- [ ] <font color="blue">输出：</font>`所有星座对应的日期`


## 📑 ***todo***
1. 显示当前时间(2023-2-23 23:15:41 星期四 癸卯[兔]年 二月（小）初八)
2. 记录纪念日。
3. 添加日常记录
4. 添加闹钟，日程提醒功能
5. 计算三伏和三九
6. 桌面倒计时卡片

## 数据压缩说明

| 23      | 22~17           | 16~13             | 12~0                                                        |
|---------|-----------------|-------------------|-------------------------------------------------------------|
| 保留位始终为0 | 农历正月初一距离公历元旦的天数 | 0表示无闰月，1至12表示闰月月份 | 从低位到高位分别对应从正月到（闰）十二月的每个月的大小，“1”表示大月，即该月有30天，“0”表示小月，即该月有29天 |



## 🔗 参考链接

* `贴吧`- [现行农历(紫金历版本)历法概要](https://tieba.baidu.com/p/2237932540#/)
* `知乎`- [每年的农历是怎么算出来的](https://www.zhihu.com/question/437033426/answer/1678577830)

