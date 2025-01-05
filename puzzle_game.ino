#define PRINT_FUNCTION Serial.print
#include "src/abstract_game/puzzle.h"
#include "src/mui/mui.h"
#include "src/ota/credentials.h"

#include <WebOTA.h>


game_state_t game_state;
bool initializationCheck = true;

userInterface_t* my_ui; // TODO: remove

void setup() {
    Serial.begin(115200);
    Serial.print("Started setting it up\n");
    init_mui_structures(&my_ui);
 //   init_wifi(ssid, password, host);
    initializationCheck = puzzleGameInit(&game_state, init_mui_structures) == SUCCESS;
    Serial.print("Finished setting it up\n");
}

void loop() {
  // put your main code here, to run repeatedly:
   if (initializationCheck)
   {
     puzzleGameMainIteration(&game_state);
  }
 // webota.handle();
}

// void terminate() {
//   puzzleGameTermination(delete_mui_structures);
// }