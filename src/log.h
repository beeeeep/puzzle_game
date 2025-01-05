#ifndef LOG_H
#define LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#ifndef LOG_CHANNGEL
#define LOG_CHANNGEL stderr
#endif

#define VOID_FUNCTION(...) { }

#ifndef PRINT_FUNCTION
#define PRINT_FUNCTION(...) fprintf(LOG_CHANNGEL, ##__VA_ARGS__)
#endif

#ifdef ENABLE_LOGGING
  #define LOG_ERROR(fmt, ...) PRINT_FUNCTION("[ERROR]: " fmt "\n", ##__VA_ARGS__)
  #define LOG_WARNING(fmt, ...) PRINT_FUNCTION("[WARNING]: " fmt "\n", ##__VA_ARGS__)
  #define LOG_INFO(fmt, ...) PRINT_FUNCTION("[INFO]: " fmt "\n", ##__VA_ARGS__)
#else
  #define LOG_ERROR(fmt, ...)
  #define LOG_WARNING(fmt, ...)
  #define LOG_INFO(fmt, ...)
#endif

#ifdef __cplusplus
}
#endif

#endif // LOG_H
