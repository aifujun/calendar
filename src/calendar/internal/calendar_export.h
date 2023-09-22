
#ifndef CALENDAR_CALENDAR_EXPORT_H
#define CALENDAR_CALENDAR_EXPORT_H


// 创建跨平台动态库定义
#ifdef SHARED_LIB_BUILD
#   ifdef _WIN32
#       ifdef LIB_EXPORTS
#           define CALENDAR_API_PUBLIC __declspec(dllexport)
#           define CALENDAR_API_LOCAL static
#       else
#           define CALENDAR_API_PUBLIC __declspec(dllimport)
#           define CALENDAR_API_LOCAL static
#       endif  // LIB_EXPORTS
#   elif __linux__ && __GNUC__ >= 4
#       define CALENDAR_API_PUBLIC __attribute__((visibility("default")))
#       define CALENDAR_API_LOCAL __attribute__((visibility("hidden")))
#   else
#       define CALENDAR_API_PUBLIC
#       define CALENDAR_API_LOCAL static
#   endif  // _WIN32
#else
#   define CALENDAR_API_PUBLIC
#   define CALENDAR_API_LOCAL static
#endif  // SHARED_LIB_BUILD


#ifndef CALENDAR_DEPRECATED
#   define CALENDAR_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef CALENDAR_DEPRECATED_API_PUBLIC
#   define CALENDAR_DEPRECATED_API_PUBLIC CALENDAR_API_PUBLIC CALENDAR_DEPRECATED
#endif

#ifndef CALENDAR_DEPRECATED_API_LOCAL
#   define CALENDAR_DEPRECATED_API_LOCAL CALENDAR_API_LOCAL CALENDAR_DEPRECATED
#endif


#endif //CALENDAR_CALENDAR_EXPORT_H
