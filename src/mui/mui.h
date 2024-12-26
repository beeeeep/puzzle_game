#ifndef MUI_H
#define MUI_H

#include <PCA9685.h>
#include <Wire.h>
#include <Arduino.h>

#include "src/abstract_game/puzzle.h"

#ifdef __cplusplus
extern "C" {
#endif

void init_gui_structures(userInterface_t** mui);
void delete_gui_structures(userInterface_t** mui);

#ifdef __cplusplus
}
#endif

#endif // MUI_H