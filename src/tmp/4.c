
typedef struct {
    short year;             /*!< 农历年份 */
    unsigned short month;   /*!< 农历月份 */
    unsigned short day;     /*!< 农历年、月、日 */
    bool isLeapMonth;       /*!< 闰月标志 */
} LunarDate_T;


unsigned char GetGanZhiOfHour(unsigned char *_gan_zhi, const LunarDate_T *_lunarDateT, unsigned short hour) {
    unsigned char gan = 0;
    unsigned char zhi = 0;
    unsigned char tmp = 0;
    unsigned char gan_day = 0;
    unsigned short tmp_h = 0;

    if (!_gan_zhi) {
        _gan_zhi = &tmp;
    }

    tmp_h = hour & 0x01 ? 1 : 0;
    zhi = (hour + tmp_h) / 2 + 1;

    gan_day >>= 4;
    gan = ((gan_day + 1) * 2 - zhi - 1) % 10;

    *_gan_zhi = gan;
    *_gan_zhi <<= 4;
    *_gan_zhi |= zhi;

    return *_gan_zhi;
}

int main(int argc, char **argv) {
    unsigned char gan_zhi = 0;
    LunarDate_T ld = {2023, 6, 22, 0};

    GetGanZhiOfHour(&gan_zhi, &ld, 11);

    printf()

    return 0;
}

