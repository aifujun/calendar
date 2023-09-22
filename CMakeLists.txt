cmake_minimum_required(VERSION 3.24)
project(cal C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_EXE_LINKER_FLAGS "-static")


option(OPT_DEBUG "Specifies the type of libraries (SHARED or STATIC) to build" ON)
option(OPT_BUILD_SHARED_LIBS "Specifies the type of libraries (SHARED or STATIC) to build" ON)


if (DEFINED OPT_DEBUG)
    message(STATUS "[DEBUG] OPT_DEBUG defined: " ${OPT_DEBUG})
    message(STATUS "[DEBUG] OPT_BUILD_SHARED_LIBS defined: " ${OPT_BUILD_SHARED_LIBS})
else()
    message(STATUS "[DEBUG] OPT_DEBUG undefined: " ${OPT_DEBUG})
    message(STATUS "[DEBUG] OPT_BUILD_SHARED_LIBS undefined: " ${OPT_BUILD_SHARED_LIBS})
endif()


add_subdirectory(src/calendar)

add_subdirectory(Google_tests)



add_executable(${PROJECT_NAME}
        main.c
        src/calendar/calendar.c
        src/calendar/internal/logger.c
        src/calendar/internal/festival.c
        src/calendar/internal/anniversary.c
)

target_link_libraries(${PROJECT_NAME} PUBLIC
        ws2_32
)
