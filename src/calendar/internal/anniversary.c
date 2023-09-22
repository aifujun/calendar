
#include <stdio.h>
#include <windows.h>

#include "anniversary.h"
#include "calendar_types.h"
#include "logger.h"


int save_data(FESTIVAL fest, const char *save_file) {
    FILE *fp = NULL;

    if (fopen_s(&fp, save_file, "rb+") != 0) {
        logger(LOG_ERROR, "Can not open the file: <%s>\n", save_file);
        return -1;
    }

    //因为 FESTIVAL 结构体占556个字节
    //所以定位按行输出只能定位到每行的行首，即距文件开端556的倍数字节处
    fseek(fp, (long)(fest.index * sizeof(FESTIVAL)),SEEK_CUR);

    // fwrite执行成功的返回值是fwrite函数的第三个参数
    if (fwrite(&fest, sizeof(FESTIVAL), 1,  fp) != 1) {
        logger(LOG_ERROR, "Save fest: %s(%d) failed, write error\n", fest.name, fest.index);
        if (fp) {
            fclose(fp);
        }
        return -2;
    }

    logger(LOG_INFO, "Success to save fest: %s(%d) to file: <%s>\n", fest.name, fest.index, save_file);
    fclose(fp);
    return 0;
}

