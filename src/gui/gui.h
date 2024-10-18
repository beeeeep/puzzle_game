#ifndef GUI_H
#define GUI_H

#include "../puzzle.h"

#ifdef CMAKE_BUILD

void init_gui_structures(userInterface_t** gui);
void delete_gui_structures(userInterface_t** gui);

#endif

#endif // GUI_H