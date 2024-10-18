#ifndef MISC_H
#define MISC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    SUCCESS = 0,
    FAILURE = 1
} function_status_t;

#define does_fail(x) (x == FAILURE)
#define does_succeed(x) (x == SUCCESS)

#define LOOP_FOREVER() for(;;)
#define LOOP_FOREVER_MAX_GUARD(sentinel, maxn) int sentinel=0; while(sentinel < maxn)
#define increase_sentinel(sentinel) sentinel++
#define reset_sentinel(sentinel) sentinel=0
#define sentinel_check_block(expr, sentinel) if (does_fail(expr))\
      {\
        increase_sentinel(sentinel);\
        continue;\
      }\
      else\
      {\
        reset_sentinel(sentinel);\
      }

#define exit_on_fail(code) if (does_fail(code))\
      {\
        LOG_ERROR( "failure when running " #code );\
        return 1;\
      }

long long millis_timestamp();

#ifdef __cplusplus
}
#endif

#endif // MISC_H