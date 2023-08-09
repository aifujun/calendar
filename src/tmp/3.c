#include <stdio.h>
#include <stdbool.h>
#include <math.h>


// 使用结构体来表示 monad，然后定义 bind 和 return 操作的函数
typedef struct {
    int has_value;
    int value;
} Maybe;

typedef Maybe (*MaybeFunction)(int);


Maybe bind(Maybe m, MaybeFunction f) {
    if (m.has_value) {
        return f(m.value);
    } else {
        Maybe result;
        result.has_value = 0;
        return result;
    }
}

Maybe Return(int x) {
    return (Maybe){
        .has_value = true,
        .value = x,
    };
}

Maybe safe_sqrt(int x) {
    if (x < 0) {
        Maybe result;
        result.has_value = 0;
        return result;
    } else {
        return Return((int)sqrt(x));
    }
}

Maybe square(int x) {
    return (Maybe){ .has_value = 1, .value = x * x };
}

//函数指针写法
Maybe sqrt_sqr(int x) {
    return bind(safe_sqrt(x), square);
}



int main(int argc, char **argv) {

    Maybe res = sqrt_sqr(-9);
    printf("%d\n", res.value);

    res = sqrt_sqr(9);
    printf("%d\n", res.value);

    return 0;
}



