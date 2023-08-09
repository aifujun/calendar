#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>


/*********************************************/

//#define TRACEBACK_ERROR(res, error_code, msg) do { \
//        error_insert(res, error_code, msg, __FILE__, __FUNCTION__, __LINE__); \
//    } while(false)

//#define TRACEBACK_OK() do { \
//        error_insert(0, ERR_OK, "", __FILE__, __FUNCTION__, __LINE__); \
//    } while(false)

#define TRACEBACK_ERROR(err_trace, error_code, msg) error_insert(err_trace, error_code, msg, __FILE__, __FUNCTION__, __LINE__)

#define TRACEBACK_OK() error_insert(0, ERR_OK, "", __FILE__, __FUNCTION__, __LINE__)

/*********************************************/

/* 错误代码ID */
typedef enum {
    ERR_OK = 0x0,
    ERROR_ID_ERROR_1,
    ERROR_ID_ERROR_2,
    ERROR_ID_ERROR_3,

}ErrorCode;

/* 错误链表结构体 */
typedef struct error_node_t {
    bool is_ok;
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
    struct error_node_t *next;  /*!<  指向下一个节点的指针 */
}error_node_t;


typedef struct {
    int data;
    error_node_t *error;
} Result;


Result Return(int data, error_node_t *err) {
    return (Result){
        .error = err,
        .data = data,
    };
}

error_node_t *GetNewNode() {
    error_node_t *node = (error_node_t *)malloc(sizeof(error_node_t));
    node->next = NULL;
    return node;
}


/* 插入节点 */
error_node_t *error_insert(error_node_t *err_trace, uint32_t err_code, const char *message, const char *file, const char *func_name, uint32_t line) {

    error_node_t *new = GetNewNode();
    uint32_t id = err_trace ? err_trace->id + 1 : 0;
    uint32_t deep = err_trace ? err_trace->traceback.deep + 1 : 0;

    new->id = id;
    new->exist = true;
    new->used = false;
    new->error_code = err_code;
    new->message = (char *)message;
    new->traceback.deep = deep;
    new->traceback.file = (char *)file;
    new->traceback.function = (char *)func_name;
    new->traceback.line = line;

    if (!err_trace) {
        return new;
    }

//    if (error_exist(new) == -1) {
//        return -1;
//    }

    new->next = err_trace;
    err_trace = new;

    return err_trace;
}


/* 遍历节点 */
void error_process(error_node_t *target_tr) {
    while (target_tr) {
        if (!target_tr->used) {
            //只执行第一次
            //error_do_first(target_tr->error.var.id);

            printf("Traceback error:\n    File \"%s:%d\", in function: %s\n    [ErrorCode: %d]: %s\n",
                   target_tr->traceback.file,
                   target_tr->traceback.line,
                   target_tr->traceback.function,
                   target_tr->error_code,
                   target_tr->message);

            target_tr->used = true;
        }
        else {
            //不断地处理错误
            //error_do(target_tr->error.id);
        }

        target_tr = target_tr->next;
    }
}

/*********************************************/

Result testBase(int x) {
    if (x < 0) {
        return Return(0, TRACEBACK_ERROR(0, ERROR_ID_ERROR_1, "testBase err."));
    }
    int a = x * x;
    return Return(a, TRACEBACK_OK());
}

Result test(int x) {
    Result res = testBase(x);
    if (res.error->error_code != ERR_OK) {
        return Return(0, TRACEBACK_ERROR(res.error, ERROR_ID_ERROR_2, "test err."));
    }

    return Return(res.data, TRACEBACK_OK());
}



int main(int argc, char **argv) {

    Result res = test(-25);
    if (res.error->error_code == ERR_OK) {
        printf("---%d\n", res.data);
    }
    else {
        error_process(res.error);
    }

    return 0;
}



