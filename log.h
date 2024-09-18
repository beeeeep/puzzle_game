#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#ifndef LOG_CHANNGEL
#define LOG_CHANNGEL stderr
#endif

#ifdef ENABLE_LOGGING
  #define LOG_ERROR(fmt, ...) fprintf(LOG_CHANNGEL, "[ERROR]: " fmt "\n", ##__VA_ARGS__)
  #define LOG_WARNING(fmt, ...) fprintf(LOG_CHANNGEL, "[WARNING]: " fmt "\n", ##__VA_ARGS__)
  #define LOG_INFO(fmt, ...) fprintf(LOG_CHANNGEL, "[INFO]: " fmt "\n", ##__VA_ARGS__)
#else
  #define LOG_ERROR(fmt, ...)
  #define LOG_WARNING(fmt, ...)
  #define LOG_INFO(fmt, ...)
#endif

#endif // LOG_H
