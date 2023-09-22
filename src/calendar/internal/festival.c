
#include <stdio.h>
#include <windows.h>

#include "festival.h"
#include "logger.h"


FESTIVAL ChineseTraditionalFestivals[MAX_TRADITIONAL_FESTIVAL_NUM] = {
        {
                .status = 1,
                .index = 0,
                .type = 0,
                .year = 0,
                .month = 1,
                .day = 1,
                .name = "春节",
                .description = "春节，是以农历计算的中国传统新年，亦称新春、正旦、正月朔日，其庆祝活动又俗称过年、度岁等，是汉族四大传统节日之一。农历正月初一为一年之首，故称“岁首”，又称“年节”、“元旦”、“元正”、“元朔”、“元日”、“正旦”、“新正”、“新春”、“履新”、“开正”。庆祝活动叫“过年”。",
        },
        {
                .status = 1,
                .index = 1,
                .type = 0,
                .year = 0,
                .month = 1,
                .day = 15,
                .name = "元宵节",
                .description = "元宵节，亦称为上元节、小正月、新十五、元夕、小年或灯节，又称为元宵、元宵节、上元、正月半，中国传统节日，每年的农历正月十五，为农历新年的第一个月圆，象征著春天的到来。华人传统会吃元宵、赏花灯、猜灯谜，以示祝贺。",
        },
        {
                .status = 1,
                .index = 2,
                .type = 0,
                .year = 0,
                .month = 2,
                .day = 2,
                .name = "龙抬头",
                .description = "龙抬头，又称春耕节、农事节、青龙节、春龙节等，是中国民间传统节日。“龙”指的是二十八宿中的东方苍龙七宿星象，每岁仲春卯月（斗指正东）之初，“龙角星”就从东方地平线上升起，故称“龙抬头”。",
        },
        {
                .status = 1,
                .index = 3,
                .type = 0,
                .year = 0,
                .month = 2,
                .day = 2,
                .name = "上巳节",
                .description = "上巳（sì）节，俗称三月三，是汉民族传统节日，该节日在汉代以前定为三月上旬的巳日，后来固定在夏历三月初三。上巳节是古代举行“祓除畔浴”活动中最重要的节日，人们结伴去水边沐浴，称为“祓禊”，此后又增加了祭祀宴饮、曲水流觞、郊外游春等内容。",
        },
        {
                .status = 1,
                .index = 4,
                .type = 0,
                .year = 0,
                .month = 5,
                .day = 5,
                .name = "端午节",
                .description = "端午节，又称端阳节、龙舟节、重午节、重五节、天中节等，日期在每年农历五月初五，是集拜神祭祖、祈福辟邪、欢庆娱乐和饮食为一体的民俗大节。端午节源于自然天象崇拜，由上古时代祭龙演变而来。",
        },
        {
                .status = 1,
                .index = 5,
                .type = 0,
                .year = 0,
                .month = 7,
                .day = 7,
                .name = "七夕节",
                .description = "七夕节，又称七巧节、七姐节、女儿节、乞巧节、七娘会、七夕祭、牛公牛婆日、巧夕等，是中国民间的传统节日。七夕节由星宿崇拜演化而来，为传统意义上的七姐诞，因拜祭“七姐”活动在七月七晩上举行，故名“七夕”。拜七姐，祈福许愿、乞求巧艺、坐看牵牛织女星、祈祷姻缘、储七夕水等，是七夕的传统习俗。",
        },
        {
                .status = 1,
                .index = 6,
                .type = 0,
                .year = 0,
                .month = 7,
                .day = 15,
                .name = "中元节",
                .description = "中元节，是道教名称，民间世俗称为七月半，佛教则称为盂兰盆节。节日习俗主要有祭祖、放河灯、祀亡魂、焚纸锭、祭祀土地等。",
        },
        {
                .status = 1,
                .index = 7,
                .type = 0,
                .year = 0,
                .month = 8,
                .day = 15,
                .name = "中秋节",
                .description = "中秋节，又称祭月节、月光诞、月夕、秋节、仲秋节、拜月节、月娘节、月亮节、团圆节等，是中国民间传统节日。中秋节源自对天象的崇拜，由上古时代秋夕祭月演变而来。中秋节自古便有祭月、赏月、吃月饼、看花灯、赏桂花、饮桂花酒等民俗，流传至今，经久不息。",
        },
        {
                .status = 1,
                .index = 8,
                .type = 0,
                .year = 0,
                .month = 9,
                .day = 9,
                .name = "重阳节",
                .description = "重阳节，是中国民间传统节日，日期在每年农历九月初九。“九”数在《易经》中为阳数，“九九”两阳数相重，故曰“重阳”；因日与月皆逢九，故又称为“重九”。九九归真，一元肇始，古人认为九九重阳是吉祥的日子。古时民间在重阳节有登高祈福、拜神祭祖及饮宴祈寿等习俗。",
        },
        {
                .status = 1,
                .index = 9,
                .type = 0,
                .year = 0,
                .month = 10,
                .day = 15,
                .name = "下元节",
                .description = "下元节，中国传统节日，为农历十月十五，亦称“下元日”、“下元”。道家有三官：天官、地官、水官，谓上元九炁赐福天官，中元七炁赦罪地官，下元五炁解厄水官。三官的诞生日分别为农历的正月十五天官赐福、七月十五地官赦罪、十月十五水官解厄，这三天被称为“上元节”、“中元节”、“下元节”。下元节，就是水官解厄旸谷帝君解厄之辰，俗谓下元日。",
        },
        {
                .status = 1,
                .index = 10,
                .type = 0,
                .year = 0,
                .month = 12,
                .day = 8,
                .name = "腊八节",
                .description = "腊八节，即每年农历十二月初八，又称为“法宝节”“佛成道节”“成道会”等。本为佛教纪念释迦牟尼佛成道之节日，后逐渐也成为民间节日。腊八节是典型的北方节日。",
        },
        {
                .status = 1,
                .index = 11,
                .type = 0,
                .year = 0,
                .month = 12,
                .day = 30,
                .name = "除夕",
                .description = "除夕，为岁末的最后一天夜晚。岁末的最后一天称为“岁除”，意为旧岁至此而除，另换新岁。“除夕”是岁除之夜的意思，又称大年夜、除夕夜、除夜等，时值年尾的最后一个晚上。除夕是除旧布新、阖家团圆、祭祀祖先的日子。除夕，在国人心中是具有特殊意义的，这个年尾最重要的日子，漂泊再远的游子也是要赶着回家去和家人团聚，在除夕，辞旧岁，迎新春。",
        },
};

char *solar_term[24] = {
        "VERNAL_EQUINOX",
        ""
};

/*---------------------------------------
 * 立春 the Beginning of Spring
 * 雨水 Rain Water
 * 惊蛰 the Waking of Insects
 * 春分 the Spring Equinox
 * 清明 Pure Brightness
 * 谷雨 Grain Rain
 * 立夏 the Beginning of Summer
 * 小满 Lesser Fullness of Grain
 * 芒种 Grain in Beard
 * 夏至 the Summer Solstice
 * 小暑 Lesser Heat
 * 大暑 Greater Heat
 * 立秋 the Beginning of Autumn
 * 处暑 the End of Heat
 * 白露 White Dew
 * 秋分 the Autumn Equinox
 * 寒露 Cold Dew
 * 霜降 Frost's Descent
 * 立冬 the Beginning of Winter
 * 小雪 Lesser Snow
 * 大雪 Greater Snow
 * 冬至 the Winter Solstice
 * 小寒 Lesser Cold
 * 大寒 Greater Cold
 ---------------------------------------*/

const int VERNAL_EQUINOX      = 0;    // 春分
const int CLEAR_AND_BRIGHT    = 1;    // 清明
const int GRAIN_RAIN          = 2;    // 谷雨
const int SUMMER_BEGINS       = 3;    // 立夏
const int GRAIN_BUDS          = 4;    // 小满
const int GRAIN_IN_EAR        = 5;    // 芒种
const int SUMMER_SOLSTICE     = 6;    // 夏至
const int LESSER_HEAT         = 7;    // 小暑
const int GREATER_HEAT        = 8;    // 大暑
const int AUTUMN_BEGINS       = 9;    // 立秋
const int STOPPING_THE_HEAT   = 10;   // 处暑
const int WHITE_DEWS          = 11;   // 白露
const int AUTUMN_EQUINOX      = 12;   // 秋分
const int COLD_DEWS           = 13;   // 寒露
const int HOAR_FROST_FALLS    = 14;   // 霜降
const int WINTER_BEGINS       = 15;   // 立冬
const int LESSER_SNOW         = 16;   // 小雪
const int GREATER_SNOW        = 17;   // 大雪
const int WINTER_SOLSTICE     = 18;   // 冬至
const int LESSER_COLD         = 19;   // 小寒
const int GREATER_COLD        = 20;   // 大寒
const int SPRING_BEGINS       = 21;   // 立春
const int RAIN_WATER          = 22;   // 雨水
const int INSECTS_AWAKING     = 23;   // 惊蛰


int load_data(const char *file) {
    FILE *fp = NULL;
    FESTIVAL fest;

    if ((fopen_s(&fp, file,"rb")) != 0) {
        logger(LOG_ERROR, "Can not open the file: <%s>\n", file);
        return -1;
    }

    // fread函数如果读成功返回的值是fread函数的第三个参数，此时为1
    // 如果读到数据，就显示；否则退出
    while (fread(&fest, sizeof(FESTIVAL), 1, fp) == 1) {
        printf("节日: %s\n", fest.name);
        printf("索引: %d\n", fest.index);
        printf("日期: %d-%d-%d\n", fest.year, fest.month, fest.day);
        printf("简介: %s\n\n", fest.description);
    }

    fclose(fp);
    return 0;
}


int load_data_with_index(short index, const char *data_file) {
    FILE *fp = NULL;
    FESTIVAL fest;

    if ((fopen_s(&fp, data_file,"rb")) != 0) {
        logger(LOG_ERROR, "Can not open the file: <%s>\n", data_file);
        return -1;
    }

    //因为 FESTIVAL 结构体占556个字节
    //所以定位按行输出只能定位到每行的行首，即距文件开端556的倍数字节处
    fseek(fp, (long)(index * sizeof(FESTIVAL)),0);

    if (fread(&fest, sizeof(FESTIVAL), 1, fp) == 1) {
        printf("节日: %s\n", fest.name);
        printf("索引: %d\n", fest.index);
        printf("日期: %d-%d-%d\n", fest.year, fest.month, fest.day);
        printf("简介: %s\n\n", fest.description);
    }

    fclose(fp);
    return 0;
}

