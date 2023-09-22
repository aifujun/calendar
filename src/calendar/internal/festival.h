
#ifndef PRETTYTOOLS_FESTIVAL_H
#define PRETTYTOOLS_FESTIVAL_H

#include "calendar_types.h"

/* 添加节日需要同步修改此处数量信息 */
#define MAX_TRADITIONAL_FESTIVAL_NUM 12



int load_data(const char *file);
int load_data_with_index(short index, const char *data_file);


#endif //PRETTYTOOLS_FESTIVAL_H
