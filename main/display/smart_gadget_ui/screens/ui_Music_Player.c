// Music page rebuilt from music_ui_assets_v3_284x240.

#include "../ui.h"
#include "music_ui_tokens.h"

#include <stdio.h>

LV_FONT_DECLARE(font_puhui_14_1);
LV_FONT_DECLARE(font_puhui_16_4);
LV_FONT_DECLARE(ui_font_MusicTitle);
LV_FONT_DECLARE(ui_font_MusicMeta);

lv_obj_t * ui_Music_Player = NULL;
lv_obj_t * ui_Music_Title = NULL;
lv_obj_t * ui_Author = NULL;
lv_obj_t * ui_Music_Lyrics = NULL;
lv_obj_t * ui_Play_btn = NULL;
lv_obj_t * ui_Play = NULL;
lv_obj_t * ui_Album = NULL;
lv_obj_t * ui_Backward = NULL;
lv_obj_t * ui_Forward = NULL;
lv_obj_t * ui_Music_List_btn = NULL;
lv_obj_t * ui_Scrolldots3 = NULL;
lv_obj_t * ui_Music_Header = NULL;
lv_obj_t * ui_Music_PageIndex = NULL;
lv_obj_t * ui_Music_Volume = NULL;
lv_obj_t * ui_Music_PlayText = NULL;

static lv_obj_t * s_background = NULL;
static lv_obj_t * s_header_meta = NULL;
static lv_obj_t * s_status_bg = NULL;
static lv_obj_t * s_status_icon = NULL;
static lv_obj_t * s_status_text = NULL;
static lv_obj_t * s_album_fade = NULL;
static lv_obj_t * s_album_frame = NULL;
static lv_obj_t * s_equalizer = NULL;
static lv_obj_t * s_progress = NULL;
static lv_obj_t * s_list_icon = NULL;
static lv_obj_t * s_list_text = NULL;
static lv_obj_t * s_prev_icon = NULL;
static lv_obj_t * s_next_icon = NULL;
static lv_obj_t * s_play_icon = NULL;
static lv_obj_t * s_pause_icon = NULL;
static lv_obj_t * s_companion = NULL;
static lv_obj_t * s_music_note = NULL;
static lv_timer_t * s_anim_timer = NULL;

static uint8_t s_music_state = UI_MUSIC_STATE_LOADING;
static uint8_t s_screen_loaded = 0;
static uint8_t s_eq_frame = 0;
static uint8_t s_loading_frame = 0;
static uint16_t s_elapsed_tick_ms = 0;
static uint32_t s_track_index = 0;
static uint32_t s_elapsed_seconds = 0;
static uint32_t s_total_seconds = MUSIC_DEFAULT_TRACK_SECONDS;
static char s_lyrics_text[128] = "我看见云飘过海面\n也听见风在轻轻说再见…";

static const lv_image_dsc_t * const s_eq_frames[] = {
    &ui_img_music_v3_eq_01, &ui_img_music_v3_eq_02,
    &ui_img_music_v3_eq_03, &ui_img_music_v3_eq_04,
    &ui_img_music_v3_eq_05, &ui_img_music_v3_eq_06,
    &ui_img_music_v3_eq_07, &ui_img_music_v3_eq_08,
    &ui_img_music_v3_eq_09, &ui_img_music_v3_eq_10,
    &ui_img_music_v3_eq_11, &ui_img_music_v3_eq_12,
};

static const lv_image_dsc_t * const s_loading_frames[] = {
    &ui_img_music_v3_loading_01, &ui_img_music_v3_loading_02,
    &ui_img_music_v3_loading_03, &ui_img_music_v3_loading_04,
    &ui_img_music_v3_loading_05, &ui_img_music_v3_loading_06,
};

static lv_obj_t * music_image(lv_obj_t * parent, const lv_image_dsc_t * source, int x, int y)
{
    lv_obj_t * image = lv_image_create(parent);
    lv_image_set_src(image, source);
    lv_obj_set_pos(image, x, y);
    lv_obj_remove_flag(image, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(image, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return image;
}

static lv_obj_t * music_image_button(lv_obj_t * parent, const lv_image_dsc_t * source, int x, int y)
{
    lv_obj_t * button = music_image(parent, source, x, y);
    lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_opa(button, LV_OPA_80, LV_STATE_PRESSED);
    return button;
}

static lv_obj_t * music_label(lv_obj_t * parent, const char * text,
                              int x, int y, int width, int height,
                              uint32_t color, lv_text_align_t align)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_obj_set_pos(label, x, y);
    lv_obj_set_size(label, width, height);
    lv_label_set_text(label, text);
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_font(label, &font_puhui_14_1, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    lv_obj_set_style_text_align(label, align, 0);
    lv_obj_set_style_pad_all(label, 0, 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return label;
}

static lv_obj_t * music_centered_icon(lv_obj_t * parent, const lv_image_dsc_t * source)
{
    lv_obj_t * icon = music_image(parent, source, 0, 0);
    lv_obj_center(icon);
    return icon;
}

static void music_format_time(char * buffer, size_t size, uint32_t seconds)
{
    snprintf(buffer, size, "%02u:%02u",
             (unsigned)(seconds / 60U), (unsigned)(seconds % 60U));
}

void ui_music_compact_set_progress(uint32_t elapsed_seconds, uint32_t total_seconds)
{
    uint32_t value = 0;

    s_total_seconds = total_seconds;
    s_elapsed_seconds = total_seconds == 0U ? 0U :
                        (elapsed_seconds > total_seconds ? total_seconds : elapsed_seconds);
    if(total_seconds != 0U) {
        value = (s_elapsed_seconds * 1000U) / total_seconds;
    }
    if(s_progress != NULL) {
        lv_slider_set_value(s_progress, (int32_t)value, LV_ANIM_OFF);
    }
}

void ui_music_compact_set_progress_position(uint32_t permille)
{
    if(s_progress != NULL) {
        lv_slider_set_value(s_progress,
                            (int32_t)(permille > 1000U ? 1000U : permille),
                            LV_ANIM_OFF);
    }
}

static void music_update_frame(void)
{
    if(s_equalizer == NULL) {
        return;
    }

    if(s_music_state == UI_MUSIC_STATE_PLAYING) {
        lv_image_set_src(s_equalizer, s_eq_frames[s_eq_frame]);
        lv_obj_set_style_opa(s_equalizer, LV_OPA_COVER, 0);
    }
    else if(s_music_state == UI_MUSIC_STATE_LOADING) {
        lv_image_set_src(s_equalizer, s_loading_frames[s_loading_frame]);
        lv_obj_set_style_opa(s_equalizer, LV_OPA_COVER, 0);
    }
    else {
        lv_image_set_src(s_equalizer, &ui_img_music_v3_eq_01);
        lv_obj_set_style_opa(s_equalizer, 64, 0);
    }
}

static void music_anim_timer_cb(lv_timer_t * timer)
{
    (void)timer;
    if(s_screen_loaded == 0U) {
        return;
    }

    if(s_music_state == UI_MUSIC_STATE_PLAYING) {
        s_eq_frame = (uint8_t)((s_eq_frame + 1U) % 12U);
        music_update_frame();

        s_elapsed_tick_ms = (uint16_t)(s_elapsed_tick_ms + MUSIC_ANIM_TIMER_MS);
        if(s_elapsed_tick_ms >= 1000U) {
            s_elapsed_tick_ms = (uint16_t)(s_elapsed_tick_ms - 1000U);
            if(s_total_seconds != 0U) {
                s_elapsed_seconds = s_elapsed_seconds >= s_total_seconds ? 0U : s_elapsed_seconds + 1U;
                ui_music_compact_set_progress(s_elapsed_seconds, s_total_seconds);
            }
        }
    }
    else if(s_music_state == UI_MUSIC_STATE_LOADING) {
        s_loading_frame = (uint8_t)((s_loading_frame + 1U) % 6U);
        music_update_frame();
    }
}

static void music_timer_start(void)
{
    if(s_anim_timer != NULL) {
        lv_timer_resume(s_anim_timer);
        lv_timer_ready(s_anim_timer);
    }
}

static void music_timer_stop(void)
{
    if(s_anim_timer != NULL) {
        lv_timer_pause(s_anim_timer);
    }
}

void ui_music_compact_set_cover(const lv_image_dsc_t * cover)
{
    if(ui_Album != NULL) {
        lv_image_set_src(ui_Album, cover != NULL ? cover : &ui_img_music_v3_album_cover);
    }
}

void ui_music_compact_set_track_info(const char * title, const char * artist)
{
    if(ui_Music_Title != NULL) {
        lv_label_set_text(ui_Music_Title,
                          title != NULL && title[0] != '\0' ? title : "正在加载");
    }
    if(ui_Author != NULL) {
        lv_label_set_text(ui_Author,
                          artist != NULL && artist[0] != '\0' ? artist : "本地音乐");
    }
}

void ui_music_set_lyrics(const char * lyrics)
{
    const char * text = lyrics != NULL && lyrics[0] != '\0' ? lyrics : "歌词加载中…";
    snprintf(s_lyrics_text, sizeof(s_lyrics_text), "%s", text);
    if(ui_Music_Lyrics != NULL && s_music_state != UI_MUSIC_STATE_LOADING) {
        lv_label_set_text(ui_Music_Lyrics, s_lyrics_text);
    }
}

void ui_music_compact_set_state(uint8_t state)
{
    if(state > UI_MUSIC_STATE_PAUSED) {
        state = UI_MUSIC_STATE_LOADING;
    }
    s_music_state = state;

    if(state == UI_MUSIC_STATE_PLAYING) {
        if(s_status_icon != NULL) lv_image_set_src(s_status_icon, &ui_img_music_v3_status_playing);
        if(s_status_text != NULL) lv_label_set_text(s_status_text, "播放中");
        if(s_companion != NULL) lv_image_set_src(s_companion, &ui_img_music_v3_companion_playing);
        if(s_play_icon != NULL) lv_obj_add_flag(s_play_icon, LV_OBJ_FLAG_HIDDEN);
        if(s_pause_icon != NULL) lv_obj_clear_flag(s_pause_icon, LV_OBJ_FLAG_HIDDEN);
        if(s_music_note != NULL) lv_obj_clear_flag(s_music_note, LV_OBJ_FLAG_HIDDEN);
        if(ui_Music_Lyrics != NULL) lv_label_set_text(ui_Music_Lyrics, s_lyrics_text);
        if(s_screen_loaded != 0U) music_timer_start();
    }
    else if(state == UI_MUSIC_STATE_PAUSED) {
        if(s_status_icon != NULL) lv_image_set_src(s_status_icon, &ui_img_music_v3_status_paused);
        if(s_status_text != NULL) lv_label_set_text(s_status_text, "已暂停");
        if(s_companion != NULL) lv_image_set_src(s_companion, &ui_img_music_v3_companion_paused);
        if(s_play_icon != NULL) lv_obj_clear_flag(s_play_icon, LV_OBJ_FLAG_HIDDEN);
        if(s_pause_icon != NULL) lv_obj_add_flag(s_pause_icon, LV_OBJ_FLAG_HIDDEN);
        if(s_music_note != NULL) lv_obj_add_flag(s_music_note, LV_OBJ_FLAG_HIDDEN);
        if(ui_Music_Lyrics != NULL) lv_label_set_text(ui_Music_Lyrics, s_lyrics_text);
        music_timer_stop();
    }
    else {
        if(s_status_icon != NULL) lv_image_set_src(s_status_icon, &ui_img_music_v3_status_loading);
        if(s_status_text != NULL) lv_label_set_text(s_status_text, "加载中");
        if(s_companion != NULL) lv_image_set_src(s_companion, &ui_img_music_v3_companion_loading);
        if(s_play_icon != NULL) lv_obj_clear_flag(s_play_icon, LV_OBJ_FLAG_HIDDEN);
        if(s_pause_icon != NULL) lv_obj_add_flag(s_pause_icon, LV_OBJ_FLAG_HIDDEN);
        if(s_music_note != NULL) lv_obj_add_flag(s_music_note, LV_OBJ_FLAG_HIDDEN);
        if(ui_Music_Lyrics != NULL) lv_label_set_text(ui_Music_Lyrics, "正在轻轻载入这首歌…");
        if(s_screen_loaded != 0U) music_timer_start();
    }

    music_update_frame();
}

void ui_music_compact_set_track_index(uint32_t index, uint32_t total)
{
    (void)total;
    if(index != s_track_index) {
        s_track_index = index;
        s_elapsed_tick_ms = 0;
        ui_music_compact_set_progress(0, MUSIC_DEFAULT_TRACK_SECONDS);
    }
}

void ui_music_compact_set_volume(uint32_t percent)
{
    (void)percent;
}

void ui_music_set_track(const ui_music_track_t * track)
{
    if(track == NULL) {
        ui_music_compact_set_track_info("正在加载", "本地音乐");
        ui_music_compact_set_cover(NULL);
        ui_music_compact_set_progress(0, 0);
    }
    else {
        ui_music_compact_set_track_info(track->title, track->artist);
        ui_music_compact_set_cover(track->cover);
        ui_music_compact_set_progress(0, track->duration_sec);
    }
    ui_music_compact_set_state(s_music_state);
}

void ui_music_set_progress(uint32_t elapsed_sec, uint32_t duration_sec)
{
    ui_music_compact_set_progress(elapsed_sec, duration_sec);
}

void ui_music_set_playback_state(ui_music_state_t state)
{
    ui_music_compact_set_state((uint8_t)state);
}

void ui_event_Music_Player(lv_event_t * e)
{
    const lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_SCREEN_UNLOAD_START) {
        s_screen_loaded = 0;
        music_timer_stop();
    }
    else if(event_code == LV_EVENT_SCREEN_LOAD_START) {
        s_screen_loaded = 1;
        if(s_music_state != UI_MUSIC_STATE_PAUSED) {
            music_timer_start();
        }
    }

}

void ui_Music_Player_screen_init(void)
{
    s_screen_loaded = 0;
    if(s_anim_timer == NULL) {
        s_anim_timer = lv_timer_create(music_anim_timer_cb, MUSIC_ANIM_TIMER_MS, NULL);
        lv_timer_pause(s_anim_timer);
    }

    ui_Music_Player = lv_obj_create(NULL);
    /* Keep the screen itself as the gesture target.  Most music-page
     * children are images/buttons, so relying on bubbling alone can make
     * swipes disappear on the hardware touch driver. */
    lv_obj_add_flag(ui_Music_Player, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(ui_Music_Player, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Music_Player, lv_color_hex(0xF9F6EB), 0);
    lv_obj_set_style_bg_opa(ui_Music_Player, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(ui_Music_Player, 0, 0);
    lv_obj_set_style_pad_all(ui_Music_Player, 0, 0);

    s_background = music_image(ui_Music_Player, &ui_img_music_v3_bg, 0, 0);
    music_image(ui_Music_Player, &ui_img_music_v3_icon_leaf,
                MUSIC_HEADER_LEAF_X, MUSIC_HEADER_LEAF_Y);
    ui_Music_Header = music_label(ui_Music_Player, "音乐",
                                  MUSIC_HEADER_TITLE_X, MUSIC_HEADER_TITLE_Y,
                                  52, 27, MUSIC_COLOR_TEXT, LV_TEXT_ALIGN_LEFT);
    lv_obj_set_style_text_font(ui_Music_Header, &ui_font_MusicTitle, 0);
    s_header_meta = music_label(ui_Music_Player, "MUSIC",
                                MUSIC_HEADER_META_X, MUSIC_HEADER_META_Y,
                                34, 12, MUSIC_COLOR_ACCENT, LV_TEXT_ALIGN_LEFT);
    lv_obj_set_style_text_font(s_header_meta, &ui_font_MusicMeta, 0);

    s_status_bg = music_image(ui_Music_Player, &ui_img_music_v3_status_bg,
                              MUSIC_STATUS_X, MUSIC_STATUS_Y);
    s_status_icon = music_image(ui_Music_Player, &ui_img_music_v3_status_loading,
                                MUSIC_STATUS_ICON_X, MUSIC_STATUS_ICON_Y);
    s_status_text = music_label(ui_Music_Player, "加载中",
                                MUSIC_STATUS_TEXT_X, MUSIC_STATUS_TEXT_Y,
                                50, 16, MUSIC_COLOR_ACCENT, LV_TEXT_ALIGN_LEFT);

    ui_Album = music_image(ui_Music_Player, &ui_img_music_v3_album_cover,
                           MUSIC_ALBUM_X, MUSIC_ALBUM_Y);
    s_album_fade = music_image(ui_Music_Player, &ui_img_music_v3_album_fade,
                               MUSIC_ALBUM_FADE_X, MUSIC_ALBUM_Y);
    s_album_frame = music_image(ui_Music_Player, &ui_img_music_v3_album_frame,
                                MUSIC_ALBUM_X, MUSIC_ALBUM_Y);

    ui_Music_Title = music_label(ui_Music_Player, "正在加载",
                                 MUSIC_TITLE_X, MUSIC_TITLE_Y,
                                 MUSIC_TITLE_W, MUSIC_TITLE_H,
                                 MUSIC_COLOR_TEXT, LV_TEXT_ALIGN_LEFT);
    lv_obj_set_style_text_font(ui_Music_Title, &ui_font_MusicTitle, 0);
    lv_label_set_long_mode(ui_Music_Title, LV_LABEL_LONG_SCROLL_CIRCULAR);

    ui_Author = music_label(ui_Music_Player, "本地音乐",
                            MUSIC_ARTIST_X, MUSIC_ARTIST_Y,
                            MUSIC_ARTIST_W, MUSIC_ARTIST_H,
                            MUSIC_COLOR_TEXT_SOFT, LV_TEXT_ALIGN_LEFT);
    lv_label_set_long_mode(ui_Author, LV_LABEL_LONG_DOT);

    ui_Music_Lyrics = music_label(ui_Music_Player, s_lyrics_text,
                                  MUSIC_LYRICS_X, MUSIC_LYRICS_Y,
                                  MUSIC_LYRICS_W, MUSIC_LYRICS_H,
                                  MUSIC_COLOR_ACCENT, LV_TEXT_ALIGN_LEFT);
    lv_label_set_long_mode(ui_Music_Lyrics, LV_LABEL_LONG_DOT);

    s_equalizer = music_image(ui_Music_Player, &ui_img_music_v3_loading_01,
                              MUSIC_EQ_X, MUSIC_EQ_Y);

    s_progress = lv_slider_create(ui_Music_Player);
    lv_obj_set_pos(s_progress, MUSIC_PROGRESS_X, MUSIC_PROGRESS_Y);
    lv_obj_set_size(s_progress, MUSIC_PROGRESS_W, MUSIC_PROGRESS_H);
    lv_slider_set_range(s_progress, 0, 1000);
    lv_slider_set_value(s_progress, 0, LV_ANIM_OFF);
    lv_obj_remove_flag(s_progress, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(s_progress, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_bg_color(s_progress, lv_color_hex(MUSIC_COLOR_TRACK), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(s_progress, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(s_progress, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_pad_top(s_progress, 3, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(s_progress, 3, LV_PART_MAIN);
    lv_obj_set_style_bg_color(s_progress, lv_color_hex(MUSIC_COLOR_ACCENT), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(s_progress, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_radius(s_progress, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
    lv_obj_set_style_width(s_progress, 12, LV_PART_KNOB);
    lv_obj_set_style_height(s_progress, 12, LV_PART_KNOB);
    lv_obj_set_style_bg_color(s_progress, lv_color_hex(MUSIC_COLOR_WHITE), LV_PART_KNOB);
    lv_obj_set_style_bg_opa(s_progress, LV_OPA_COVER, LV_PART_KNOB);
    lv_obj_set_style_radius(s_progress, LV_RADIUS_CIRCLE, LV_PART_KNOB);
    lv_obj_set_style_border_color(s_progress, lv_color_hex(MUSIC_COLOR_ACCENT), LV_PART_KNOB);
    lv_obj_set_style_border_width(s_progress, 1, LV_PART_KNOB);

    ui_Music_List_btn = music_image_button(ui_Music_Player, &ui_img_music_v3_btn_list_bg,
                                           MUSIC_LIST_X, MUSIC_LIST_Y);
    s_list_icon = music_image(ui_Music_List_btn, &ui_img_music_v3_icon_list, 7, 5);
    s_list_text = music_label(ui_Music_List_btn, "播放列表", 22, 4, 47, 18,
                              MUSIC_COLOR_TEXT_SOFT, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_transform_zoom(s_list_text, 200, 0);

    ui_Backward = music_image_button(ui_Music_Player, &ui_img_music_v3_btn_small_bg,
                                     MUSIC_PREV_X, MUSIC_PREV_Y);
    s_prev_icon = music_centered_icon(ui_Backward, &ui_img_music_v3_icon_prev);

    ui_Play_btn = music_image_button(ui_Music_Player, &ui_img_music_v3_btn_play_bg,
                                     MUSIC_PLAY_X, MUSIC_PLAY_Y);
    s_play_icon = music_centered_icon(ui_Play_btn, &ui_img_music_v3_icon_play);
    s_pause_icon = music_centered_icon(ui_Play_btn, &ui_img_music_v3_icon_pause);

    ui_Forward = music_image_button(ui_Music_Player, &ui_img_music_v3_btn_small_bg,
                                    MUSIC_NEXT_X, MUSIC_NEXT_Y);
    s_next_icon = music_centered_icon(ui_Forward, &ui_img_music_v3_icon_next);

    s_music_note = music_image(ui_Music_Player, &ui_img_music_v3_icon_note,
                               MUSIC_NOTE_X, MUSIC_NOTE_Y);
    s_companion = music_image(ui_Music_Player, &ui_img_music_v3_companion_loading,
                              MUSIC_COMPANION_X, MUSIC_COMPANION_Y);

    ui_Play = NULL;
    ui_Music_PlayText = NULL;
    ui_Music_PageIndex = NULL;
    ui_Music_Volume = NULL;
    ui_Scrolldots3 = NULL;

    ui_music_compact_set_progress(0, MUSIC_DEFAULT_TRACK_SECONDS);
    ui_music_compact_set_state(s_music_state);
    lv_obj_add_event_cb(ui_Music_Player, ui_event_Music_Player, LV_EVENT_ALL, NULL);
}

void ui_Music_Player_screen_destroy(void)
{
    s_screen_loaded = 0;
    music_timer_stop();
    if(s_anim_timer != NULL) {
        lv_timer_delete(s_anim_timer);
        s_anim_timer = NULL;
    }
    if(ui_Music_Player != NULL) {
        lv_obj_del(ui_Music_Player);
    }

    ui_Music_Player = NULL;
    ui_Music_Title = NULL;
    ui_Author = NULL;
    ui_Music_Lyrics = NULL;
    ui_Play_btn = NULL;
    ui_Play = NULL;
    ui_Album = NULL;
    ui_Backward = NULL;
    ui_Forward = NULL;
    ui_Music_List_btn = NULL;
    ui_Scrolldots3 = NULL;
    ui_Music_Header = NULL;
    ui_Music_PageIndex = NULL;
    ui_Music_Volume = NULL;
    ui_Music_PlayText = NULL;
    s_background = NULL;
    s_header_meta = NULL;
    s_status_bg = NULL;
    s_status_icon = NULL;
    s_status_text = NULL;
    s_album_fade = NULL;
    s_album_frame = NULL;
    s_equalizer = NULL;
    s_progress = NULL;
    s_list_icon = NULL;
    s_list_text = NULL;
    s_prev_icon = NULL;
    s_next_icon = NULL;
    s_play_icon = NULL;
    s_pause_icon = NULL;
    s_companion = NULL;
    s_music_note = NULL;
}
