#include "roll.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void roll_init()
{
   srand(time(NULL));
}

/**
 * @brief returns a random integer number between (and including) min and max
 * 
 */
int roll(const int min, const int max)
{
   return min + rand() % (max - min + 1);
}
/**
 * @brief returns a random integer number between (and including) min and max excluding the values in the excl_values_list
 * 
 */
int roll_exclusive(const int min, const int max, const int excl_values_list[], const int number_excl_values)
{
   int random_val = 0;
   int redo_flag = 0;
   do
   {
      redo_flag = 0;
      random_val = roll(min, max);

      for (int i = 0; i < number_excl_values; i++)
      {
         const int excl_value = excl_values_list[i];

         if (random_val == excl_value)
         {
            redo_flag = 1;
         }
      }
   } while (redo_flag == 1);

   return random_val;
}
