#include "roll.h"

void roll_init(void)
{
srand(time(NULL));
}


int roll(int min, int max)
{
   return min + rand() % (max - min + 1);
}

int roll_exclusive(int min, int max, int *excl_values_list, int excl_values_no)
{
   int random_val = 0;
   int redo_flag = 0;
   do
   {
      redo_flag = 0;
      random_val = min + rand() % (max - min + 1);

      for (int i = 0; i < excl_values_no; i++)
      {
         int excl_value;

         if (excl_values_no == 1)
         {
            excl_value = *excl_values_list;
         }
         else
         {
            excl_value = excl_values_list[i];
         }

         if (random_val == excl_value)
         {
            redo_flag = 1;
         }
      }
   } while (redo_flag == 1);

   return random_val;
}
