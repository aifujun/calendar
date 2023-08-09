
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <windows.h>


#define TRACEBACK_ERROR(id, msg) do { \
        error_insert(id, msg, __FILE__, __FUNCTION__, __LINE__); \
    } while(false)


typedef enum {
    //   Hex        | Decimal       | Description
    //Basic error
    ERR_OK = 0x00000000,                                   /*!< 0x00000000 |  0            | No error. */
    ERR_SYSTEM,
    ERR_TIMEOUT,

    //login component
    LOGIN_ERROR_UNKNOWN_ERROR,
    LOGIN_ERROR_UNKNOWN_USER_NAME,

    //data manager component
    DATAMANAGER_ERROR_UNKNOWN_ERROR,
    DATAMANAGER_ERROR_MISSING_CONNECTION,

    // calendar component
    CALENDAR_ERROR_UNKNOWN_ERROR,
    CALENDAR_ERROR_DATE_OUT_OF_RANGE,
    CALENDAR_ERROR_ILLEGAL_DATE,
    CALENDAR_ERROR_ILLEGAL_ORDINAL_DAYS,
    CALENDAR_ERROR_PARAM_ERROR,

}ErrorCode;



/* 错误代码ID */
typedef enum {
    ERROR_ID_ERROR_1 = 0x01,
    ERROR_ID_ERROR_2,
    ERROR_ID_ERROR_3,

}error_id_t;

/* 错误结构体 */
typedef union {
    uint32_t value;

    struct {
        uint16_t id;            /*!<  错误标识 id */
        uint8_t exist;          /*!<  节点存在标志 */
        uint8_t used;           /*!<  节点是否被处理 */
        ErrorCode error_code;   /*!<  错误代码 */
        char *message;          /*!<  代码追踪 */
        uint64_t reserved;      /*!<  预留 */

        struct {
            uint32_t deep;      /*!<  traceback深度 */
            uint32_t line;
            char *file;
            char *function;
        }traceback;
    };
}error_t;


/* 错误链表结构体 */
typedef struct error_node_t {
    error_t error;              /*!<  错误 */
    struct error_node_t *next;  /*!<  指向下一个节点的指针 */
}error_node_t;

typedef struct {
    void *data;
    error_node_t error_node;
} Result;

static error_node_t *error_handle = NULL;

int8_t error_exist(error_node_t *s_error_code);
int8_t error_insert(uint32_t err_code, const char *message, const char *file, const char *func_name, uint32_t line);
int8_t error_delete(error_node_t *s_error_code);
void error_process(error_node_t *target_tr);


/* 查询节点是否存在 */
int8_t error_exist(error_node_t *s_error_code) {
    error_node_t *target = error_handle;

    while (target) {
        if(target == s_error_code) {
            // already exist.
            return -1;
        }
        target = target->next;
    }

    return 0;
}

/* 插入节点 */
//int8_t error_insert(error_node_t *s_error_code) {
//    if (s_error_code->error.exist || error_exist(s_error_code) == -1) {
//        return -1;
//    }
//
//    s_error_code->next = error_handle;
//    error_handle = s_error_code;
//    s_error_code->error.exist = true;
//
//    return 0;
//}

error_node_t *GetNewNode() {
    error_node_t *node = (error_node_t *)malloc(sizeof(error_node_t));
    node->next = NULL;
    return node;
}

error_node_t error_node_error_1 = {
        .error.error_code = (ErrorCode) ERROR_ID_ERROR_1,
        .next = NULL,
};

error_node_t error_node_error_2 = {
        .error.error_code = (ErrorCode) ERROR_ID_ERROR_2,
        .next = NULL,
};

error_node_t error_node_error_3 = {
        .error.error_code = (ErrorCode) ERROR_ID_ERROR_3,
        .next = NULL,
};


/* 插入节点 */
int8_t error_insert(uint32_t err_code, const char *message, const char *file, const char *func_name, uint32_t line) {

    error_node_t *new;
    uint32_t id = error_handle ? error_handle->error.id + 1 : 0;
    uint32_t deep = error_handle ? error_handle->error.traceback.deep + 1 : 0;

    error_t new_error = {
            .id = id,
            .exist = false,
            .used = false,
            .error_code = err_code,
            .message = (char *)message,
            .traceback.deep = deep,
            .traceback.file = (char *)file,
            .traceback.function = (char *)func_name,
            .traceback.line = line,
    };

    new = GetNewNode();

    if (error_exist(new) == -1) {
        return -1;
    }

    new->error = new_error;

    new->next = error_handle;
    error_handle = new;
    new->error.exist = true;

    return 0;
}

error_node_t *FindErrNodeByID(uint16_t id) {
    error_node_t *target = error_handle;

    while (target) {
        if (target->error.id == id) {
            return target;
        }
        target = target->next;
    }

    return NULL;
}

/* 删除节点 */
int8_t error_delete(error_node_t *s_error_code) {
    if(!s_error_code->error.exist || !error_exist(s_error_code)) {
        return -1;
    }

    struct error_node_t** curr;

    for (curr = &error_handle; *curr; ) {
        struct error_node_t *entry = *curr;
        if (entry == s_error_code) {
            *curr = entry->next;
            free(s_error_code);
        }
        else {
            curr = &entry->next;
        }
    }

    return 0;
}

/* 遍历节点 */
void error_process(error_node_t *target_tr) {
    if(!target_tr) {
        target_tr = error_handle;
    }

    while (target_tr) {
        if (!target_tr->error.used) {
            //只执行第一次
            //error_do_first(target_tr->error.var.id);

            printf("Traceback error:\n    File \"%s:%d\", in function: %s\n    [ErrorCode: %d]: %s\n",
                   target_tr->error.traceback.file,
                   target_tr->error.traceback.line,
                   target_tr->error.traceback.function,
                   target_tr->error.error_code,
                   target_tr->error.message);

            target_tr->error.used = true;
        }
        else {
            //不断地处理错误
            //error_do(target_tr->error.id);
        }

        target_tr = target_tr->next;
    }
}




int main(int argc, char **argv) {

    error_t err = {
            .exist = true,
            .used = false,
            .error_code = (ErrorCode) ERROR_ID_ERROR_1,
    };

    while(1) {
        TRACEBACK_ERROR(ERROR_ID_ERROR_1, "test err 1");
        TRACEBACK_ERROR(ERROR_ID_ERROR_2, "test err 2");
        TRACEBACK_ERROR(ERROR_ID_ERROR_3, "test err 3");
        error_process(0);

        error_delete(FindErrNodeByID(0));
        error_delete(FindErrNodeByID(1));
        error_delete(FindErrNodeByID(2));

        error_process(NULL);
        break;
    }

    return 0;
}



