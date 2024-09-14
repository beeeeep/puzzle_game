#ifndef ROLL
#define ROLL
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void roll_init(void);

int roll(int min, int max);
int roll_exclusive(int min, int max, int *excl_vavues_list, int excl_vavues_no);

#endif //ROLL