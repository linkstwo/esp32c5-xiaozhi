// This file follows the SquareLine Studio screen style used by Smart_Gadget.

#include "../ui.h"
#include "music_ui_tokens.h"

#include <font_awesome.h>
#include <stdio.h>

LV_FONT_DECLARE(font_puhui_16_4);
LV_FONT_DECLARE(BUILTIN_ICON_FONT);

lv_obj_t * ui_Music_Player = NULL;
lv_obj_t * ui_Music_Title = NULL;
lv_obj_t * ui_Author = NULL;
lv_obj_t * ui_Play_btn = NULL;
lv_obj_t * ui_Play = NULL;
lv_obj_t * ui_Album = NULL;
lv_obj_t * ui_Backward = NULL;
lv_obj_t * ui_Forward = NULL;
lv_obj_t * ui_Scrolldots3 = NULL;
lv_obj_t * ui_Music_Header = NULL;
lv_obj_t * ui_Music_PageIndex = NULL;
lv_obj_t * ui_Music_Volume = NULL;
lv_obj_t * ui_Music_PlayText = NULL;

static lv_obj_t * ui_Music_CornerLeaf = NULL;
static lv_obj_t * ui_Music_WaveLine = NULL;
static lv_obj_t * ui_Music_WaveGlow = NULL;
static lv_obj_t * ui_Music_WaveBars[11] = {NULL};
static lv_obj_t * ui_Music_BackwardText = NULL;
static lv_obj_t * ui_Music_ForwardText = NULL;
static lv_obj_t * ui_Music_BackwardIcon = NULL;
static lv_obj_t * ui_Music_ForwardIcon = NULL;
static lv_obj_t * ui_Music_VolumeIcon = NULL;
static lv_point_precise_t ui_music_wave_points[] = {
    {0, 42},
    {10, 36},
    {20, 28},
    {30, 24},
    {40, 27},
    {50, 34},
    {60, 43},
    {70, 50},
    {80, 52},
    {90, 49},
    {100, 43},
    {110, 36},
    {120, 33},
};

static const int music_bar_x[] = {0, 11, 22, 33, 44, 55, 66, 77, 88, 99, 110};
static const int music_bar_w[] = {7, 8, 8, 8, 9, 9, 9, 8, 8, 8, 7};
static const int music_idle_heights[] = {16, 24, 34, 28, 44, 54, 46, 36, 29, 22, 15};
static const int music_paused_heights[] = {11, 15, 19, 16, 23, 27, 24, 20, 17, 14, 10};
static const int music_play_min_heights[] = {13, 18, 23, 18, 27, 32, 28, 23, 19, 16, 12};
static const int music_play_max_heights[] = {35, 43, 53, 45, 60, 66, 58, 50, 42, 36, 30};
static const int music_play_duration_ms[] = {170, 160, 180, 165, 175, 170, 180, 160, 175, 165, 170};
static const int music_play_delay_ms[] = {0, 18, 36, 54, 72, 90, 108, 126, 144, 162, 180};

static void music_clear_obj_style(lv_obj_t * obj)
{
    lv_obj_remove_style_all(obj);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

static lv_obj_t * music_circle_create(lv_obj_t * parent, int x, int y, int size, uint32_t color, lv_opa_t opa)
{
    lv_obj_t * obj = lv_obj_create(parent);
    music_clear_obj_style(obj);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, size, size);
    lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(obj, opa, 0);
    return obj;
}

static lv_obj_t * music_oval_create(lv_obj_t * parent, int x, int y, int width, int height,
                                     uint32_t color, lv_opa_t opa)
{
    lv_obj_t * obj = lv_obj_create(parent);
    music_clear_obj_style(obj);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, width, height);
    lv_obj_set_style_radius(obj, height / 2, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(obj, opa, 0);
    return obj;
}

static lv_obj_t * music_label_create(lv_obj_t * parent, const char * text, int x, int y, int width,
                                     uint32_t color, const lv_font_t * font, lv_text_align_t align)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_obj_set_pos(label, x, y);
    lv_obj_set_size(label, width, LV_SIZE_CONTENT);
    lv_label_set_text(label, text);
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_align(label, align, 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return label;
}

static void music_decor_icon_create(lv_obj_t * parent, const char * icon, int x, int y,
                                    uint32_t color, int zoom)
{
    lv_obj_t * label = music_label_create(parent, icon, x, y, 18, color,
                                          &BUILTIN_ICON_FONT, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_transform_zoom(label, zoom, 0);
    lv_obj_set_style_opa(label, LV_OPA_70, 0);
}

static lv_obj_t * music_button_create(lv_obj_t * parent, int x, int y, int width, int height,
                                      uint32_t bg_color, uint32_t border_color)
{
    lv_obj_t * btn = lv_obj_create(parent);
    music_clear_obj_style(btn);
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_size(btn, width, height);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_radius(btn, height / 2, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(bg_color), 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(btn, lv_color_hex(border_color), 0);
    lv_obj_set_style_border_width(btn, 2, 0);
    lv_obj_set_style_shadow_color(btn, lv_color_hex(border_color), 0);
    lv_obj_set_style_shadow_opa(btn, LV_OPA_10, 0);
    lv_obj_set_style_shadow_width(btn, 8, 0);
    lv_obj_set_style_shadow_offset_y(btn, 2, 0);
    lv_obj_set_style_pad_left(btn, 4, 0);
    lv_obj_set_style_pad_right(btn, 4, 0);
    lv_obj_set_style_pad_top(btn, 0, 0);
    lv_obj_set_style_pad_bottom(btn, 0, 0);
    lv_obj_set_style_transform_zoom(btn, 256, 0);
    lv_obj_set_style_transform_zoom(btn, 240, LV_STATE_PRESSED);
    return btn;
}

static uint32_t music_wave_bar_color(uint32_t index)
{
    return (index == 2 || index == 5 || index == 8) ? MUSIC_COLOR_GOLD_SOFT : MUSIC_COLOR_ACCENT;
}

static void music_highlight_page_dot(lv_obj_t * dots, uint32_t index)
{
    ui_update_page_indicator(dots, index, 7, MUSIC_COLOR_ACCENT, MUSIC_COLOR_DOT_IDLE);
}

static void music_wave_bar_exec(void * var, int32_t value)
{
    lv_obj_t * bar = (lv_obj_t *)var;

    if(bar == NULL) {
        return;
    }

    lv_obj_set_height(bar, value);
    lv_obj_set_y(bar, MUSIC_WAVE_BASELINE_Y - value);
}

static void music_wave_stop_all(void)
{
    uint32_t i;

    for(i = 0; i < 11; ++i) {
        if(ui_Music_WaveBars[i] != NULL) {
            lv_anim_delete(ui_Music_WaveBars[i], music_wave_bar_exec);
        }
    }
}

static void music_wave_apply_static(const int * heights)
{
    uint32_t i;

    music_wave_stop_all();

    for(i = 0; i < 11; ++i) {
        if(ui_Music_WaveBars[i] == NULL) {
            continue;
        }
        lv_obj_set_style_bg_color(ui_Music_WaveBars[i], lv_color_hex(music_wave_bar_color(i)), 0);
        music_wave_bar_exec(ui_Music_WaveBars[i], heights[i]);
    }
}

static void music_wave_start_playing(void)
{
    uint32_t i;

    music_wave_stop_all();
    for(i = 0; i < 11; ++i) {
        lv_anim_t anim;

        if(ui_Music_WaveBars[i] == NULL) {
            continue;
        }

        lv_obj_set_style_bg_color(ui_Music_WaveBars[i], lv_color_hex(music_wave_bar_color(i)), 0);
        lv_anim_init(&anim);
        lv_anim_set_var(&anim, ui_Music_WaveBars[i]);
        lv_anim_set_exec_cb(&anim, music_wave_bar_exec);
        lv_anim_set_values(&anim, music_play_min_heights[i], music_play_max_heights[i]);
        lv_anim_set_time(&anim, music_play_duration_ms[i]);
        lv_anim_set_playback_time(&anim, music_play_duration_ms[i]);
        lv_anim_set_repeat_count(&anim, LV_ANIM_REPEAT_INFINITE);
        lv_anim_set_repeat_delay(&anim, 0);
        lv_anim_set_delay(&anim, music_play_delay_ms[i]);
        lv_anim_set_path_cb(&anim, lv_anim_path_ease_in_out);
        lv_anim_start(&anim);
    }
}

void ui_music_compact_set_state(uint8_t state)
{
    if(ui_Play == NULL || ui_Music_PlayText == NULL || ui_Play_btn == NULL) {
        return;
    }

    switch(state) {
    case 1:
        lv_label_set_text(ui_Play, FONT_AWESOME_PAUSE);
        lv_label_set_text(ui_Music_PlayText, "暂停");
        lv_obj_set_style_bg_color(ui_Play_btn, lv_color_hex(MUSIC_COLOR_ACCENT), 0);
        lv_obj_set_style_border_color(ui_Play_btn, lv_color_hex(MUSIC_COLOR_ACCENT_DARK), 0);
        if(ui_Album != NULL) {
            lv_image_set_src(ui_Album, &ui_img_focus_mascot_running);
        }
        music_wave_start_playing();
        break;
    case 2:
        lv_label_set_text(ui_Play, FONT_AWESOME_PLAY);
        lv_label_set_text(ui_Music_PlayText, "继续");
        lv_obj_set_style_bg_color(ui_Play_btn, lv_color_hex(MUSIC_COLOR_ACCENT_DARK), 0);
        lv_obj_set_style_border_color(ui_Play_btn, lv_color_hex(MUSIC_COLOR_ACCENT_DARK), 0);
        if(ui_Album != NULL) {
            lv_image_set_src(ui_Album, &ui_img_focus_mascot_paused);
        }
        music_wave_apply_static(music_paused_heights);
        break;
    case 0:
    default:
        lv_label_set_text(ui_Play, FONT_AWESOME_PLAY);
        lv_label_set_text(ui_Music_PlayText, "播放");
        lv_obj_set_style_bg_color(ui_Play_btn, lv_color_hex(MUSIC_COLOR_ACCENT), 0);
        lv_obj_set_style_border_color(ui_Play_btn, lv_color_hex(MUSIC_COLOR_ACCENT_DARK), 0);
        if(ui_Album != NULL) {
            lv_image_set_src(ui_Album, &ui_img_focus_mascot_running);
        }
        music_wave_apply_static(music_idle_heights);
        break;
    }
}

void ui_music_compact_set_track_index(uint32_t index, uint32_t total)
{
    (void)index;
    (void)total;
}

void ui_music_compact_set_volume(uint32_t percent)
{
    char text[24];

    if(ui_Music_Volume == NULL) {
        return;
    }

    snprintf(text, sizeof(text), "音量 %u%%", (unsigned)percent);
    lv_label_set_text(ui_Music_Volume, text);
}

void ui_event_Music_Player(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_SCREEN_UNLOAD_START) {
        music_wave_stop_all();
    }
    if(event_code == LV_EVENT_SCREEN_LOAD_START) {
        if(ui_Music_WaveLine != NULL) {
            upanim_Animation(ui_Music_WaveLine, 40);
        }
        if(ui_Album != NULL) {
            upanim_Animation(ui_Album, 80);
        }
        if(ui_Backward != NULL) {
            upanim_Animation(ui_Backward, 120);
        }
        if(ui_Play_btn != NULL) {
            upanim_Animation(ui_Play_btn, 160);
        }
        if(ui_Forward != NULL) {
            upanim_Animation(ui_Forward, 200);
        }
        if(ui_Scrolldots3 != NULL) {
            scrolldot_Animation(ui_Scrolldots3, 0);
        }
    }
    if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_RIGHT) {
        lv_indev_wait_release(lv_indev_active());
        _ui_screen_change(&ui_Today, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Today_screen_init);
    }
    if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_LEFT) {
        lv_indev_wait_release(lv_indev_active());
        _ui_screen_change(&ui_Call, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Call_screen_init);
    }
}

void ui_Music_Player_screen_init(void)
{
    uint32_t i;

    ui_Music_Player = lv_obj_create(NULL);
    lv_obj_remove_flag(ui_Music_Player, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Music_Player, lv_color_hex(MUSIC_COLOR_BG_TOP), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_Music_Player, lv_color_hex(MUSIC_COLOR_BG_BOTTOM), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_Music_Player, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Music_Player, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Music_Player, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_Music_Player, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    music_circle_create(ui_Music_Player, -42, 12, 105, MUSIC_COLOR_CIRCLE_SOFT, LV_OPA_20);
    music_oval_create(ui_Music_Player, 157, 39, 116, 116, MUSIC_COLOR_CIRCLE_MINT, LV_OPA_50);
    music_oval_create(ui_Music_Player, 176, 128, 92, 28, MUSIC_COLOR_CIRCLE_SOFT, LV_OPA_30);
    music_circle_create(ui_Music_Player, 14, 145, 5, MUSIC_COLOR_GOLD, LV_OPA_60);
    music_circle_create(ui_Music_Player, 144, 57, 4, MUSIC_COLOR_GOLD, LV_OPA_50);
    music_circle_create(ui_Music_Player, 151, 147, 5, MUSIC_COLOR_CIRCLE_MINT, LV_OPA_70);
    music_circle_create(ui_Music_Player, 274, 91, 7, MUSIC_COLOR_GOLD_SOFT, LV_OPA_40);

    ui_Music_CornerLeaf = lv_image_create(ui_Music_Player);
    lv_image_set_src(ui_Music_CornerLeaf, &ui_img_home_corner_leaf);
    lv_obj_set_pos(ui_Music_CornerLeaf, 0, 0);
    lv_obj_set_style_transform_zoom(ui_Music_CornerLeaf, 205, 0);
    lv_obj_set_style_opa(ui_Music_CornerLeaf, LV_OPA_80, 0);
    lv_obj_remove_flag(ui_Music_CornerLeaf, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(ui_Music_CornerLeaf, LV_OBJ_FLAG_GESTURE_BUBBLE);

    music_decor_icon_create(ui_Music_Player, FONT_AWESOME_MUSIC, 57, 45, MUSIC_COLOR_GOLD, 170);
    music_decor_icon_create(ui_Music_Player, FONT_AWESOME_MUSIC, 135, 69, MUSIC_COLOR_ACCENT, 150);
    music_decor_icon_create(ui_Music_Player, FONT_AWESOME_STAR, 111, 50, MUSIC_COLOR_GOLD, 115);
    music_decor_icon_create(ui_Music_Player, FONT_AWESOME_STAR, 150, 119, MUSIC_COLOR_GOLD, 100);

    ui_Music_Header = music_label_create(ui_Music_Player, "音乐", 0, MUSIC_HEADER_Y, MUSIC_SCREEN_W,
                                         MUSIC_COLOR_TEXT, &font_puhui_16_4, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_transform_zoom(ui_Music_Header, 245, 0);

    for(i = 0; i < 11; ++i) {
        ui_Music_WaveBars[i] = lv_obj_create(ui_Music_Player);
        music_clear_obj_style(ui_Music_WaveBars[i]);
        lv_obj_set_pos(ui_Music_WaveBars[i], MUSIC_WAVE_X + music_bar_x[i], MUSIC_WAVE_BASELINE_Y - music_idle_heights[i]);
        lv_obj_set_size(ui_Music_WaveBars[i], music_bar_w[i], music_idle_heights[i]);
        lv_obj_set_style_radius(ui_Music_WaveBars[i], music_bar_w[i], 0);
        lv_obj_set_style_bg_color(ui_Music_WaveBars[i], lv_color_hex(music_wave_bar_color(i)), 0);
        lv_obj_set_style_bg_opa(ui_Music_WaveBars[i], LV_OPA_70, 0);
        lv_obj_set_style_border_width(ui_Music_WaveBars[i], 0, 0);
    }

    ui_Music_WaveGlow = lv_line_create(ui_Music_Player);
    lv_line_set_points(ui_Music_WaveGlow, ui_music_wave_points,
                       (uint16_t)(sizeof(ui_music_wave_points) / sizeof(ui_music_wave_points[0])));
    lv_obj_set_pos(ui_Music_WaveGlow, MUSIC_WAVE_X, MUSIC_WAVE_Y + 18);
    lv_obj_set_size(ui_Music_WaveGlow, MUSIC_WAVE_W, 64);
    lv_obj_set_style_line_color(ui_Music_WaveGlow, lv_color_hex(0xFFF9E9), 0);
    lv_obj_set_style_line_width(ui_Music_WaveGlow, 7, 0);
    lv_obj_set_style_line_rounded(ui_Music_WaveGlow, true, 0);
    lv_obj_add_flag(ui_Music_WaveGlow, LV_OBJ_FLAG_GESTURE_BUBBLE);

    ui_Music_WaveLine = lv_line_create(ui_Music_Player);
    lv_line_set_points(ui_Music_WaveLine, ui_music_wave_points,
                       (uint16_t)(sizeof(ui_music_wave_points) / sizeof(ui_music_wave_points[0])));
    lv_obj_set_pos(ui_Music_WaveLine, MUSIC_WAVE_X, MUSIC_WAVE_Y + 18);
    lv_obj_set_size(ui_Music_WaveLine, MUSIC_WAVE_W, 64);
    lv_obj_set_style_line_color(ui_Music_WaveLine, lv_color_hex(MUSIC_COLOR_ACCENT_DARK), 0);
    lv_obj_set_style_line_width(ui_Music_WaveLine, 3, 0);
    lv_obj_set_style_line_rounded(ui_Music_WaveLine, true, 0);
    lv_obj_add_flag(ui_Music_WaveLine, LV_OBJ_FLAG_GESTURE_BUBBLE);

    ui_Album = lv_image_create(ui_Music_Player);
    lv_image_set_src(ui_Album, &ui_img_focus_mascot_running);
    lv_obj_set_pos(ui_Album, MUSIC_MASCOT_X, MUSIC_MASCOT_Y);
    lv_obj_set_style_transform_zoom(ui_Album, 272, 0);
    lv_obj_remove_flag(ui_Album, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(ui_Album, LV_OBJ_FLAG_GESTURE_BUBBLE);

    ui_Backward = music_button_create(ui_Music_Player, MUSIC_LEFT_BTN_X, MUSIC_LEFT_BTN_Y,
                                      MUSIC_LEFT_BTN_W, MUSIC_LEFT_BTN_H,
                                      MUSIC_COLOR_SIDE_BUTTON, MUSIC_COLOR_BORDER);
    lv_obj_set_layout(ui_Backward, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(ui_Backward, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_Backward, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(ui_Backward, 4, 0);
    ui_Music_BackwardIcon = music_label_create(ui_Backward, FONT_AWESOME_BACKWARD_STEP, 0, 0, 12,
                                               MUSIC_COLOR_TEXT, &BUILTIN_ICON_FONT, LV_TEXT_ALIGN_LEFT);
    ui_Music_BackwardText = music_label_create(ui_Backward, "上一首", 0, 0, 44,
                                               MUSIC_COLOR_TEXT, &font_puhui_16_4, LV_TEXT_ALIGN_LEFT);
    lv_obj_set_style_text_letter_space(ui_Music_BackwardText, -2, 0);
    lv_obj_set_style_transform_zoom(ui_Music_BackwardText, 220, 0);

    ui_Play_btn = music_button_create(ui_Music_Player, MUSIC_MAIN_BTN_X, MUSIC_MAIN_BTN_Y,
                                      MUSIC_MAIN_BTN_W, MUSIC_MAIN_BTN_H,
                                      MUSIC_COLOR_ACCENT, MUSIC_COLOR_ACCENT_DARK);
    lv_obj_set_layout(ui_Play_btn, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(ui_Play_btn, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_Play_btn, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(ui_Play_btn, 7, 0);
    ui_Play = music_label_create(ui_Play_btn, FONT_AWESOME_PLAY, 0, 0, 18,
                                 0xFFFFFF, &BUILTIN_ICON_FONT, LV_TEXT_ALIGN_LEFT);
    ui_Music_PlayText = music_label_create(ui_Play_btn, "播放", 0, 0, 48,
                                           0xFFFFFF, &font_puhui_16_4, LV_TEXT_ALIGN_LEFT);

    ui_Forward = music_button_create(ui_Music_Player, MUSIC_RIGHT_BTN_X, MUSIC_RIGHT_BTN_Y,
                                     MUSIC_RIGHT_BTN_W, MUSIC_RIGHT_BTN_H,
                                     MUSIC_COLOR_SIDE_BUTTON, MUSIC_COLOR_BORDER);
    lv_obj_set_layout(ui_Forward, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(ui_Forward, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_Forward, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(ui_Forward, 4, 0);
    ui_Music_ForwardText = music_label_create(ui_Forward, "下一首", 0, 0, 44,
                                              MUSIC_COLOR_TEXT, &font_puhui_16_4, LV_TEXT_ALIGN_LEFT);
    lv_obj_set_style_text_letter_space(ui_Music_ForwardText, -2, 0);
    lv_obj_set_style_transform_zoom(ui_Music_ForwardText, 220, 0);
    ui_Music_ForwardIcon = music_label_create(ui_Forward, FONT_AWESOME_FORWARD_STEP, 0, 0, 12,
                                              MUSIC_COLOR_TEXT, &BUILTIN_ICON_FONT, LV_TEXT_ALIGN_RIGHT);

    ui_Music_VolumeIcon = music_label_create(ui_Music_Player, FONT_AWESOME_VOLUME_HIGH,
                                             MUSIC_VOLUME_X, MUSIC_VOLUME_Y, 14,
                                             MUSIC_COLOR_TEXT, &BUILTIN_ICON_FONT, LV_TEXT_ALIGN_LEFT);
    ui_Music_Volume = music_label_create(ui_Music_Player, "音量 80%", MUSIC_VOLUME_X + 18, MUSIC_VOLUME_Y, 70,
                                         MUSIC_COLOR_TEXT, &font_puhui_16_4, LV_TEXT_ALIGN_LEFT);
    lv_obj_set_style_transform_zoom(ui_Music_Volume, 190, 0);

    ui_Scrolldots3 = ui_Scrolldots_create(ui_Music_Player);
    lv_obj_set_y(ui_Scrolldots3, MUSIC_DOTS_Y - (MUSIC_SCREEN_H - 8));
    music_highlight_page_dot(ui_Scrolldots3, 2);

    ui_music_compact_set_volume(80);
    ui_music_compact_set_state(0);

    lv_obj_add_event_cb(ui_Music_Player, ui_event_Music_Player, LV_EVENT_ALL, NULL);
}

void ui_Music_Player_screen_destroy(void)
{
    if(ui_Music_Player) {
        music_wave_stop_all();
        lv_obj_del(ui_Music_Player);
    }

    ui_Music_Player = NULL;
    ui_Music_Title = NULL;
    ui_Author = NULL;
    ui_Play_btn = NULL;
    ui_Play = NULL;
    ui_Album = NULL;
    ui_Backward = NULL;
    ui_Forward = NULL;
    ui_Scrolldots3 = NULL;
    ui_Music_Header = NULL;
    ui_Music_PageIndex = NULL;
    ui_Music_Volume = NULL;
    ui_Music_PlayText = NULL;
    ui_Music_CornerLeaf = NULL;
    ui_Music_WaveLine = NULL;
    ui_Music_WaveGlow = NULL;
    ui_Music_BackwardText = NULL;
    ui_Music_ForwardText = NULL;
    ui_Music_BackwardIcon = NULL;
    ui_Music_ForwardIcon = NULL;
    ui_Music_VolumeIcon = NULL;
    ui_Music_WaveBars[0] = NULL;
    ui_Music_WaveBars[1] = NULL;
    ui_Music_WaveBars[2] = NULL;
    ui_Music_WaveBars[3] = NULL;
    ui_Music_WaveBars[4] = NULL;
    ui_Music_WaveBars[5] = NULL;
    ui_Music_WaveBars[6] = NULL;
    ui_Music_WaveBars[7] = NULL;
    ui_Music_WaveBars[8] = NULL;
    ui_Music_WaveBars[9] = NULL;
    ui_Music_WaveBars[10] = NULL;
}
