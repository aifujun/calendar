
cmake_minimum_required(VERSION 3.24)

# 'RunTests' is the project name
project(RunTests)


set(GTest_INCLUDE_DIR include)
set(GTest_LIBRARY_DIR lib)

if (DEFINED OPT_DEBUG)
    message("CMAKE_CURRENT_SOURCE_DIR:" ${CMAKE_CURRENT_SOURCE_DIR})
endif()

link_directories(${GTest_LIBRARY_DIR})

#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)


# for window static debug
#set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
# for window dynamic debug
# set(CMAKE_CXX_FLAGS_DEBUG "/MDd")
# for window dynamic release
# set(CMAKE_CXX_FLAGS_DEBUG "/MD")
# for window static release
# set(CMAKE_CXX_FLAGS_DEBUG "/MT")

# Link runTests with what we want to test and the GTest and pthread library
add_executable(${PROJECT_NAME}
        main.cpp
        test_Calendar.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${GTest_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
        libgtest.a
        libgtest_main.a
        libgmock.a
        libgmock_main.a
)

