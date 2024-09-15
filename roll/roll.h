#ifndef ROLL
#define ROLL

void roll_init(void);

int roll(const int min, const int max);
int roll_exclusive(const int min, const int max, const int excl_vavues_list[], const int excl_vavues_no);

#endif //ROLL