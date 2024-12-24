#ifndef SOUND_MODULE
#define SOUND_MODULE


#define NO_OF_TRACKS 5
#define NO_VOL_STATES 100


typedef enum { button_click, rotary_click, Winning_sound, Lamp_sound, Winning_song, Intro_song } tracklist_t;

typedef void (*sound_module_button_press_t)(void);

void sound_module_init(sound_module_button_press_t play_button_p, sound_module_button_press_t pause_button,
    sound_module_button_press_t next_button_p, sound_module_button_press_t previous_button_p,
    sound_module_button_press_t volume_up_button, sound_module_button_press_t volume_down_button_p);


void sound_module_set_volume(unsigned char vol_percent);

void sound_module_play_track(tracklist_t track);


#endif // SOUND_MODULE
