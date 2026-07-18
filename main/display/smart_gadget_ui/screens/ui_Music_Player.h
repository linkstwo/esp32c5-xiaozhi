#ifndef UI_MUSIC_PLAYER_H
#define UI_MUSIC_PLAYER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void ui_Music_Player_screen_init(void);
extern void ui_Music_Player_screen_destroy(void);
extern void ui_event_Music_Player(lv_event_t * e);

extern lv_obj_t * ui_Music_Player;
extern lv_obj_t * ui_Music_Title;
extern lv_obj_t * ui_Author;
extern lv_obj_t * ui_Music_Lyrics;
extern lv_obj_t * ui_Play_btn;
extern lv_obj_t * ui_Play;
extern lv_obj_t * ui_Album;
extern lv_obj_t * ui_Backward;
extern lv_obj_t * ui_Forward;
extern lv_obj_t * ui_Music_List_btn;
extern lv_obj_t * ui_Scrolldots3;
extern lv_obj_t * ui_Music_Header;
extern lv_obj_t * ui_Music_PageIndex;
extern lv_obj_t * ui_Music_Volume;
extern lv_obj_t * ui_Music_PlayText;

typedef struct {
    const char * title;
    const char * artist;
    const lv_image_dsc_t * cover;
    uint32_t duration_sec;
} ui_music_track_t;

typedef enum {
    UI_MUSIC_STATE_LOADING = 0,
    UI_MUSIC_STATE_PLAYING,
    UI_MUSIC_STATE_PAUSED,
} ui_music_state_t;

void ui_music_set_track(const ui_music_track_t * track);
void ui_music_set_progress(uint32_t elapsed_sec, uint32_t duration_sec);
void ui_music_set_playback_state(ui_music_state_t state);
void ui_music_set_lyrics(const char * lyrics);

// Compatibility interfaces used by SmartGadgetDisplay.
void ui_music_compact_set_state(uint8_t state);
void ui_music_compact_set_track_index(uint32_t index, uint32_t total);
void ui_music_compact_set_volume(uint32_t percent);
void ui_music_compact_set_track_info(const char * title, const char * artist);
void ui_music_compact_set_progress(uint32_t elapsed_seconds, uint32_t total_seconds);
void ui_music_compact_set_progress_position(uint32_t permille);
void ui_music_compact_set_cover(const lv_image_dsc_t * cover);

#ifdef __cplusplus
}
#endif

#endif
