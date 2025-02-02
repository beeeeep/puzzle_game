#ifndef SOUND_MODULE2
#define SOUND_MODULE2

#include "Arduino.h"

class GPD2846
{
  public:
    GPD2846(int pinS1, int pinS2, int pinS3, int pinS6, int numberOfTracks);
    void goToTrack(int track);
    void volumeUp();
    void volumeDown();
    void togglePause();
    void toggleRepeat();
    void test();
  private:
    void next();
    void previous();
    void longPress(const uint8_t pinNum);
    void shortPress(const uint8_t pinNum);
    void _highZ();
  private:
    int _numberOfTracks;
    int _currentTrack;
    uint8_t _pinS1;
    uint8_t _pinS2;
    uint8_t _pinS3;
    uint8_t _pinS6 = -1;
};


#endif // SOUND_MODULE2