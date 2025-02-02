#ifndef RED_SWITCHES_H
#define RED_SWITCHES_H

#define NUM_RED_SWITCHES 3

const struct {int line; int column;} red_switches_indices[NUM_RED_SWITCHES] = {
  {0, 0},
  {1, 1},
  {2, 2}
};
const struct {int line; int column;} red_switch_controlled[NUM_RED_SWITCHES] = {
  {0, 1},
  {1, 2},
  {2, 3}
};

const struct {int line_a; int col_a; int line_b; int col_b;} red_switches_connections[NUM_RED_SWITCHES] = {
  {0, 0, 0, 1},
  {1, 1, 1, 2},
  {2, 2, 2, 3}
};


#endif // RED_SWITCHES_H