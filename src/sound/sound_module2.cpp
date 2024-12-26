#include "sound_module2.h"

void GPD2846::longPress(const uint8_t pinNum) {
    pinMode(pinNum, OUTPUT); // pin goes low
    delay(1000);
    _highZ();
    delay(100);
}

void GPD2846::shortPress(const uint8_t pinNum) {
    pinMode(pinNum, OUTPUT); // pin goes low
    delay(100);
    _highZ();
    delay(100);
}

GPD2846::GPD2846(int pinS1, int pinS2, int pinS3, int pinS6, int numberOfTracks) {
    pinMode(pinS1, INPUT);
    _pinS1 = pinS1;
    pinMode(pinS2, INPUT);
    _pinS2 = pinS2;
    pinMode(pinS3, INPUT);
    _pinS3 = pinS3;
    if (pinS6 != -1) {
        pinMode(pinS6, INPUT);
        _pinS6 = pinS6;
        toggleRepeat(); // start always repeating the same track
    }
    else {
        _pinS6 = -1;
    }
    _numberOfTracks = numberOfTracks;
    _currentTrack = 0;
}

void goToTrack(int track) {
    if (track < 0 || track >= _numberOfTracks) {
        return;
    }
    if (track > _currentTrack) {
        for (int i = _currentTrack; i < track; i++) {
            next();
        }
    }
    if (track < _currentTrack) {
        for (int i = track; i < _currentTrack; i++) {
            previous();
        }
    }
    _currentTrack = track;
}

void GPD2846::volumeUp() {
    longPress(_pinS3);
}

void GPD2846::volumeDown() {
    longPress(_pinS1);
}

void GPD2846::next() {
    shortPress(_pinS3);
}

void GPD2846::previous() {
    shortPress(_pinS1);
}

void GPD2846::togglePause() {
    shortPress(_pinS2);
}

void GPD2846::toggleRepeat() {
    shortPress(_pinS6);
}

void GPD2846::_highZ() {
    pinMode(_pinS1, INPUT); // pin goes to tristate
    pinMode(_pinS2, INPUT); // pin goes to tristate
    pinMode(_pinS3, INPUT); // pin goes to tristate
    if (_pinS6 != -1) {
        pinMode(_pinS6, INPUT); // pin goes to tristate
    }
}
