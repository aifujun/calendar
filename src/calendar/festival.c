/** todo: 增加节日 节气 */

#include "festival.h"

//得到指定年份的节气信息，首个是小寒
int GetJieQi(short year, unsigned short month, unsigned short jie_qi[2]);

/**
 * 得到指定年份的节气信息，首个是小寒
 * @param [in]year: 年份
 * @param [in]month: 月份
 * @param [out]jie_qi: 节气
 * @return
 */
int GetJieQi(short year, unsigned short month, unsigned short jie_qi[2]){
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
    GetDateFromOrdinal(year, wDays + wXiaoHanOrder, &month, &jie_qi[1]);
    //第一个节气
    GetDateFromOrdinal(year, wDays + wXiaoHanOrder - wCurJQData - 14, &month, &jie_qi[0]);
    return ERR_OK;
}


int main(int argc, char **argv){
    GetJieQi(2023, 3, 22);

    return 0;
}


