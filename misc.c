#include "misc.h"

#include <sys/time.h>
#include <time.h>

long long millis_timestamp()
{
   struct timeval te;
   gettimeofday(&te, NULL);                                         // get current time
   long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000; // calculate milliseconds
   // printw("milliseconds: %lld\n", milliseconds);
   return milliseconds;
}
