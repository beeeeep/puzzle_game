#ifndef RED_SWITCHES_H
#define RED_SWITCHES_H

#define NUM_RED_SWITCHES 5

const struct {int line; int column} red_switches_indices[NUM_RED_SWITCHES] = {
  {0, 0},
  {1, 1},
  {2, 2},
  {3, 3},
  {4, 4},
};

const struct {int line_a; int col_a; int line_b; int col_b} red_switches_connections[NUM_RED_SWITCHES] = {
  {0, 0, 0, 1},
  {1, 1, 1, 2},
  {2, 2 ,2, 3},
  {3, 3, 3, 4},
  {4, 4, 4, 5}
};


#endif // RED_SWITCHES_H