cmake_minimum_required(VERSION 3.24)
project(calendar)

set(CMAKE_C_STANDARD 23)

# 当前目录
set(CUR_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE SrcFiles RELATIVE ${CUR_DIR}
        Calendar1.c
        internal/logger.c
        internal/anniversary.c
)

if (DEFINED OPT_DEBUG)
    foreach(file IN LISTS SrcFiles)
        message(STATUS "[DEBUG] ${file}")
    endforeach(file)
endif()

set(OPT_BUILD_SHARED_LIBS ON)


if (OPT_BUILD_SHARED_LIBS)
    add_library(${PROJECT_NAME} SHARED ${SrcFiles})

    # LIB_SHARED_BUILD 宏定义的访问符为 PUBLIC, 即无论是编译还是安装后作为库文件引用时均有效
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DSHARED_LIB_BUILD)

    # LIB_EXPORTS 宏定义的访问符为 PRIVATE, 即这个宏定义只是在编译时有效
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DLIB_EXPORTS)
else()
    add_library(${PROJECT_NAME} STATIC ${SrcFiles})
endif()
