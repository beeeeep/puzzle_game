#include "sound_module.h"


static sound_module_button_press_t play_button;
static sound_module_button_press_t pause_button;
static sound_module_button_press_t next_button;
static sound_module_button_press_t previous_button;
static sound_module_button_press_t volume_up_button;
static sound_module_button_press_t volume_down_button;


void sound_module_init(sound_module_button_press_t play_button_p, sound_module_button_press_t pause_button_p,
    sound_module_button_press_t next_button_p, sound_module_button_press_t previous_button_p,
    sound_module_button_press_t volume_up_button_p, sound_module_button_press_t volume_down_button_p) {
    play_button        = play_button_p;
    pause_button       = pause_button_p;
    next_button        = next_button_p;
    volume_up_button   = volume_up_button_p;
    volume_down_button = volume_down_button_p;

    pause_button();
}


void sound_module_set_volume(unsigned char vol_percent) {
    static int curr_vol = 100;

    int new_vol = (int) (NO_VOL_STATES * (double) (vol_percent / 100));

    if (new_vol > curr_vol) {
        for (int i = curr_vol; i < new_vol; i++) {
            volume_up_button();
        }
    }

    if (new_vol < curr_vol) {
        for (int i = new_vol; i < curr_vol; i++) {
            volume_down_button();
        }
    }
}

void sound_module_play_track(tracklist_t track)
{
    static tracklist_t curr_tracK_no;
    if (track > curr_tracK_no) {
        for (int i = curr_tracK_no; i < track; i++) {
            next_button();
            pause_button();
        }
    }

    if (track < curr_tracK_no) {
        for (int i = track; i < curr_tracK_no; i++) {
        previous_button();
        pause_button();
        }
    }
    play_button();
}
