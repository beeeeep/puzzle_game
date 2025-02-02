#ifndef ROLL
#define ROLL

#ifdef __cplusplus
extern "C" {
#endif


void roll_init(unsigned int seed_value);

int roll(const int min, const int max);
int roll_exclusive(const int min, const int max, const int excl_vavues_list[], const int excl_vavues_no);

#ifdef __cplusplus
}
#endif

#endif //ROLL