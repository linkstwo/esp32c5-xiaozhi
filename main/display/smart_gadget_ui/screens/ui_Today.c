// Pomodoro V3 focus page assembled from the approved 284x240 resource pack.

#include "../ui.h"
#include "focus_ui_tokens.h"

#include <font_awesome.h>
#include <string.h>

LV_FONT_DECLARE(font_puhui_16_4);
LV_FONT_DECLARE(BUILTIN_ICON_FONT);
LV_FONT_DECLARE(ui_font_FocusDigits);

lv_obj_t * ui_Today = NULL;
lv_obj_t * ui_Today_header = NULL;
lv_obj_t * ui_Today_title = NULL;
lv_obj_t * ui_Today_date = NULL;
lv_obj_t * ui_Today_scroll = NULL;
lv_obj_t * ui_Today_timer_arc = NULL;
lv_obj_t * ui_Today_status_panel = NULL;
lv_obj_t * ui_Today_status = NULL;
lv_obj_t * ui_Today_status_detail = NULL;
lv_obj_t * ui_Today_env_panel = NULL;
lv_obj_t * ui_Today_env_temp = NULL;
lv_obj_t * ui_Today_env_humidity = NULL;
lv_obj_t * ui_Today_env_air = NULL;
lv_obj_t * ui_Today_countdown_panel = NULL;
lv_obj_t * ui_Today_exam = NULL;
lv_obj_t * ui_Today_project = NULL;
lv_obj_t * ui_Today_focus_panel = NULL;
lv_obj_t * ui_Today_focus_time = NULL;
lv_obj_t * ui_Today_focus_unit = NULL;
lv_obj_t * ui_Today_focus_hint = NULL;
lv_obj_t * ui_Today_duration_left = NULL;
lv_obj_t * ui_Today_left_btn_icon = NULL;
lv_obj_t * ui_Today_left_btn_label = NULL;
lv_obj_t * ui_Today_duration_right = NULL;
lv_obj_t * ui_Today_task_panel = NULL;
lv_obj_t * ui_Today_task_label = NULL;
lv_obj_t * ui_Today_focus_btn = NULL;
lv_obj_t * ui_Today_focus_btn_icon = NULL;
lv_obj_t * ui_Today_focus_btn_label = NULL;
lv_obj_t * ui_Today_focus_reset = NULL;
lv_obj_t * ui_Today_focus_reset_icon = NULL;
lv_obj_t * ui_Today_focus_reset_label = NULL;
lv_obj_t * ui_Today_tip_panel = NULL;
lv_obj_t * ui_Today_tip_icon = NULL;
lv_obj_t * ui_Today_tip_label = NULL;
lv_obj_t * ui_Today_mascot_front = NULL;
lv_obj_t * ui_Today_mascot_back = NULL;
lv_obj_t * ui_Today_left_decor = NULL;
lv_obj_t * ui_Today_corner_vine = NULL;
lv_obj_t * ui_Today_top_back = NULL;
lv_obj_t * ui_Today_top_gear = NULL;
lv_obj_t * ui_Today_scrolldots = NULL;
lv_obj_t * ui_Today_state_panel = NULL;
lv_obj_t * ui_Today_state_label = NULL;
lv_obj_t * ui_Today_primary_icon = NULL;
lv_obj_t * ui_Today_secondary_icon = NULL;
lv_obj_t * ui_Today_task_edit_icon = NULL;
lv_obj_t * ui_Today_center_icon = NULL;

static lv_obj_t * s_title_icon;
static lv_obj_t * s_header_emblem;
static lv_obj_t * s_period_label;
static lv_obj_t * s_ring_overlay;
static lv_obj_t * s_tomato_crown;
static lv_obj_t * s_crown_emblem;
static lv_obj_t * s_corner_vine;
static lv_obj_t * s_sparkle_large;
static lv_obj_t * s_sparkle_small;
static lv_obj_t * s_orbit_glow;
static lv_obj_t * s_orbit_trail;
static lv_obj_t * s_button_left_overlay;
static lv_obj_t * s_button_main_overlay;
static lv_obj_t * s_button_right_overlay;
static lv_obj_t * s_wave_one;
static lv_obj_t * s_wave_two;
static lv_obj_t * s_texture_tiles[6];
static uint8_t s_visual_state = 0xFF;

static void today_clear_style(lv_obj_t * obj)
{
    lv_obj_remove_style_all(obj);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

static lv_obj_t * today_panel(lv_obj_t * parent, int x, int y, int w, int h,
                              int radius, uint32_t bg, lv_opa_t opa,
                              uint32_t border, int border_width)
{
    lv_obj_t * obj = lv_obj_create(parent);
    today_clear_style(obj);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, h);
    lv_obj_set_style_radius(obj, radius, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(bg), 0);
    lv_obj_set_style_bg_opa(obj, opa, 0);
    lv_obj_set_style_border_color(obj, lv_color_hex(border), 0);
    lv_obj_set_style_border_width(obj, border_width, 0);
    lv_obj_set_style_clip_corner(obj, true, 0);
    return obj;
}

static lv_obj_t * today_label(lv_obj_t * parent, const char * text, int x, int y,
                              int w, uint32_t color, const lv_font_t * font,
                              lv_text_align_t align)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_obj_set_pos(label, x, y);
    lv_obj_set_width(label, w);
    lv_label_set_text(label, text);
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_bg_opa(label, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(label, 0, 0);
    lv_obj_set_style_pad_all(label, 0, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_align(label, align, 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return label;
}

static lv_obj_t * today_image(lv_obj_t * parent, const void * src, int x, int y)
{
    lv_obj_t * image = lv_image_create(parent);
    lv_image_set_src(image, src);
    lv_obj_set_pos(image, x, y);
    lv_obj_remove_flag(image, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(image, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return image;
}

static lv_obj_t * today_button(lv_obj_t * parent, int x, int y, int w, int h,
                               uint32_t bg, uint32_t border, bool primary,
                               lv_obj_t ** overlay_out, lv_obj_t ** icon_out,
                               lv_obj_t ** label_out)
{
    lv_obj_t * button = today_panel(parent, x, y, w, h, h / 2, bg,
                                    LV_OPA_COVER, border, 1);
    lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_transform_pivot_x(button, w / 2, LV_STATE_PRESSED);
    lv_obj_set_style_transform_pivot_y(button, h / 2, LV_STATE_PRESSED);
    lv_obj_set_style_transform_zoom(button, FOCUS_BUTTON_PRESSED_ZOOM, LV_STATE_PRESSED);
    lv_obj_set_style_shadow_color(button, lv_color_hex(FOCUS_COLOR_BROWN_TEXT), 0);
    lv_obj_set_style_shadow_width(button, primary ? 5 : 2, 0);
    lv_obj_set_style_shadow_opa(button, primary ? 24 : 14, 0);
    lv_obj_set_style_shadow_offset_y(button, 1, 0);

    lv_obj_t * overlay = today_image(
        button,
        primary ? (const void *)&ui_img_focus_button_primary_overlay
                : (const void *)&ui_img_focus_button_secondary_overlay,
        0, 0);
    lv_obj_t * icon = today_label(button, "", primary ? 17 : 7,
                                  (h - 16) / 2, 20,
                                  primary ? FOCUS_COLOR_CORAL_TOMATO
                                          : FOCUS_COLOR_DEEP_GREEN,
                                  &BUILTIN_ICON_FONT, LV_TEXT_ALIGN_CENTER);
    lv_obj_t * label = today_label(button, "", 0, (h - 16) / 2, w,
                                   primary ? FOCUS_COLOR_BROWN_TEXT
                                           : FOCUS_COLOR_DEEP_GREEN,
                                   &font_puhui_16_4, LV_TEXT_ALIGN_CENTER);
    lv_obj_move_foreground(icon);
    lv_obj_move_foreground(label);

    *overlay_out = overlay;
    *icon_out = icon;
    *label_out = label;
    return button;
}

static void today_anim_zoom(void * var, int32_t value)
{
    lv_obj_set_style_transform_zoom((lv_obj_t *)var, value, 0);
}

static void today_anim_y(void * var, int32_t value)
{
    lv_obj_set_y((lv_obj_t *)var, value);
}

static void today_anim_opa(void * var, int32_t value)
{
    lv_obj_set_style_opa((lv_obj_t *)var, value, 0);
}

static void today_start_ambient_animations(void)
{
    if (s_orbit_glow != NULL) {
        lv_anim_t glow;
        lv_anim_init(&glow);
        lv_anim_set_var(&glow, s_orbit_glow);
        lv_anim_set_values(&glow, 256, FOCUS_ORBIT_GLOW_MAX_ZOOM);
        lv_anim_set_time(&glow, 700);
        lv_anim_set_playback_time(&glow, 700);
        lv_anim_set_repeat_count(&glow, LV_ANIM_REPEAT_INFINITE);
        lv_anim_set_exec_cb(&glow, today_anim_zoom);
        lv_anim_start(&glow);
    }

    if (ui_Today_mascot_front != NULL) {
        lv_anim_t mascot;
        lv_anim_init(&mascot);
        lv_anim_set_var(&mascot, ui_Today_mascot_front);
        lv_anim_set_values(&mascot, FOCUS_MASCOT_Y,
                           FOCUS_MASCOT_Y - FOCUS_MASCOT_FLOAT_PX);
        lv_anim_set_time(&mascot, 1200);
        lv_anim_set_playback_time(&mascot, 1200);
        lv_anim_set_repeat_count(&mascot, LV_ANIM_REPEAT_INFINITE);
        lv_anim_set_exec_cb(&mascot, today_anim_y);
        lv_anim_start(&mascot);
    }
}

static void today_start_completion_waves(void)
{
    lv_obj_t * waves[] = {s_wave_one, s_wave_two};
    const uint32_t delays[] = {0, 220};

    for (uint32_t i = 0; i < 2; ++i) {
        if (waves[i] == NULL) {
            continue;
        }
        lv_obj_set_style_opa(waves[i], LV_OPA_70, 0);
        lv_obj_set_style_transform_zoom(waves[i], 256, 0);

        lv_anim_t zoom;
        lv_anim_init(&zoom);
        lv_anim_set_var(&zoom, waves[i]);
        lv_anim_set_values(&zoom, 256, 340);
        lv_anim_set_time(&zoom, 720);
        lv_anim_set_delay(&zoom, delays[i]);
        lv_anim_set_exec_cb(&zoom, today_anim_zoom);
        lv_anim_start(&zoom);

        lv_anim_t opa;
        lv_anim_init(&opa);
        lv_anim_set_var(&opa, waves[i]);
        lv_anim_set_values(&opa, LV_OPA_70, LV_OPA_TRANSP);
        lv_anim_set_time(&opa, 720);
        lv_anim_set_delay(&opa, delays[i]);
        lv_anim_set_exec_cb(&opa, today_anim_opa);
        lv_anim_start(&opa);
    }
}

void ui_Today_set_focus_orbit_value(int32_t value)
{
    if (s_orbit_glow == NULL || s_orbit_trail == NULL) {
        return;
    }
    if (value < 0) value = 0;
    if (value > 1000) value = 1000;

    const int32_t span = FOCUS_ARC_END_DEG - FOCUS_ARC_START_DEG;
    const int32_t angle = FOCUS_ARC_START_DEG + (span * value) / 1000;
    const int32_t px = FOCUS_ARC_CENTER_X +
        ((lv_trigo_sin(angle + 90) * FOCUS_ARC_RADIUS) >> LV_TRIGO_SHIFT);
    const int32_t py = FOCUS_ARC_CENTER_Y +
        ((lv_trigo_sin(angle) * FOCUS_ARC_RADIUS) >> LV_TRIGO_SHIFT);

    lv_obj_set_pos(s_orbit_glow, px - 12, py - 12);
    lv_obj_set_pos(s_orbit_trail, px - 46, py - 9);
    lv_image_set_pivot(s_orbit_trail, 46, 9);
    lv_image_set_rotation(s_orbit_trail, (angle + 90) * 10);
}

void ui_Today_set_focus_visual_state(uint8_t state)
{
    if (state > 3) {
        state = 0;
    }
    if (s_period_label == NULL) {
        return;
    }

    s_visual_state = state;
    const char * period = "准备时段";
    if (state == 1) {
        period = "专注时段";
    } else if (state == 2) {
        period = "暂停片刻";
    } else if (state == 3) {
        period = "专注完成";
    }
    lv_label_set_text(s_period_label, period);

    if (s_sparkle_large != NULL) {
        lv_obj_set_style_opa(s_sparkle_large,
                             state == 3 ? LV_OPA_COVER : LV_OPA_60, 0);
    }
    if (s_sparkle_small != NULL) {
        lv_obj_set_style_opa(s_sparkle_small,
                             state == 3 ? LV_OPA_COVER : LV_OPA_50, 0);
    }
    if (state == 3) {
        today_start_completion_waves();
    } else {
        if (s_wave_one != NULL) lv_obj_set_style_opa(s_wave_one, LV_OPA_TRANSP, 0);
        if (s_wave_two != NULL) lv_obj_set_style_opa(s_wave_two, LV_OPA_TRANSP, 0);
    }

    if (ui_Today_focus_reset_icon != NULL) {
        if (state == 1) {
            lv_obj_set_style_text_font(ui_Today_focus_reset_icon,
                                       &font_puhui_16_4, 0);
            lv_obj_set_style_border_color(ui_Today_focus_reset_icon,
                                          lv_color_hex(FOCUS_COLOR_LEAF_GREEN), 0);
            lv_obj_set_style_border_width(ui_Today_focus_reset_icon, 1, 0);
            lv_obj_set_style_radius(ui_Today_focus_reset_icon,
                                    LV_RADIUS_CIRCLE, 0);
            lv_obj_set_size(ui_Today_focus_reset_icon, 16, 16);
            lv_obj_set_style_text_align(ui_Today_focus_reset_icon,
                                        LV_TEXT_ALIGN_CENTER, 0);
        } else {
            lv_obj_set_style_text_font(ui_Today_focus_reset_icon,
                                       &BUILTIN_ICON_FONT, 0);
            lv_obj_set_style_border_width(ui_Today_focus_reset_icon, 0, 0);
            lv_obj_set_size(ui_Today_focus_reset_icon, 20, 16);
        }
    }
}

static void today_layout_button_content(lv_obj_t * button, lv_obj_t * icon,
                                        lv_obj_t * label)
{
    if (button == NULL || icon == NULL || label == NULL) {
        return;
    }
    const char * icon_text = lv_label_get_text(icon);
    const bool has_icon = icon_text != NULL && icon_text[0] != '\0' &&
                          !lv_obj_has_flag(icon, LV_OBJ_FLAG_HIDDEN);
    const int32_t width = lv_obj_get_width(button);

    if (!has_icon) {
        lv_obj_set_x(label, 0);
        lv_obj_set_width(label, width);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_transform_zoom(label, 256, 0);
        return;
    }

    const bool long_text = strlen(lv_label_get_text(label)) > 12;
    const int32_t icon_x = width >= 100 ? (long_text ? 9 : 18) : 8;
    const int32_t text_x = width >= 100 ? (long_text ? 30 : 43) : 29;
    lv_obj_set_x(icon, icon_x);
    lv_obj_set_x(label, text_x);
    lv_obj_set_width(label, width - text_x - 4);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_transform_zoom(label, long_text ? 232 : 256, 0);
}

void ui_Today_refresh_focus_button_layout(void)
{
    today_layout_button_content(ui_Today_duration_left,
                                ui_Today_left_btn_icon,
                                ui_Today_left_btn_label);
    today_layout_button_content(ui_Today_focus_btn,
                                ui_Today_focus_btn_icon,
                                ui_Today_focus_btn_label);
    today_layout_button_content(ui_Today_focus_reset,
                                ui_Today_focus_reset_icon,
                                ui_Today_focus_reset_label);
}

void ui_event_Today(lv_event_t * e)
{
    const lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SCREEN_LOAD_START) {
        today_start_ambient_animations();
    }
}

void ui_Today_screen_init(void)
{
    ui_Today = lv_obj_create(NULL);
    today_clear_style(ui_Today);
    lv_obj_set_style_bg_color(ui_Today, lv_color_hex(FOCUS_COLOR_BACKGROUND), 0);
    lv_obj_set_style_bg_opa(ui_Today, LV_OPA_COVER, 0);

    s_title_icon = today_image(ui_Today, &ui_img_focus_title_tomato,
                               FOCUS_HEADER_ICON_X, FOCUS_HEADER_ICON_Y);
    ui_Today_title = today_label(ui_Today, "番茄钟", FOCUS_TITLE_X, FOCUS_TITLE_Y,
                                 FOCUS_TITLE_W, FOCUS_COLOR_BROWN_TEXT,
                                 &font_puhui_16_4, LV_TEXT_ALIGN_LEFT);
    lv_obj_set_style_transform_zoom(ui_Today_title, 280, 0);

    ui_Today_status_panel = today_panel(
        ui_Today, FOCUS_HEADER_STATE_X, FOCUS_HEADER_STATE_Y,
        FOCUS_HEADER_STATE_W, FOCUS_HEADER_STATE_H, 13,
        FOCUS_COLOR_PANEL, LV_OPA_COVER, FOCUS_COLOR_LEAF_GREEN, 1);
    s_header_emblem = today_panel(ui_Today_status_panel, 7, 8, 9, 9,
                                  LV_RADIUS_CIRCLE, FOCUS_COLOR_LEAF_GREEN,
                                  LV_OPA_COVER, FOCUS_COLOR_DEEP_GREEN, 0);
    ui_Today_status = today_label(ui_Today_status_panel, "准备中", 22, 3, 52,
                                  FOCUS_COLOR_DEEP_GREEN, &font_puhui_16_4,
                                  LV_TEXT_ALIGN_CENTER);

    ui_Today_countdown_panel = today_panel(
        ui_Today, FOCUS_SESSION_X, FOCUS_SESSION_Y,
        FOCUS_SESSION_W, FOCUS_SESSION_H, 9,
        FOCUS_COLOR_PRIMARY_CREAM, LV_OPA_COVER,
        FOCUS_COLOR_CREAM_TRACK, 1);
    ui_Today_status_detail = today_label(ui_Today_countdown_panel, "0/4", 0, 5,
                                         FOCUS_SESSION_W,
                                         FOCUS_COLOR_BROWN_TEXT,
                                         &font_puhui_16_4,
                                         LV_TEXT_ALIGN_CENTER);

    ui_Today_focus_panel = today_panel(
        ui_Today, FOCUS_MAIN_PANEL_X, FOCUS_MAIN_PANEL_Y,
        FOCUS_MAIN_PANEL_W, FOCUS_MAIN_PANEL_H,
        FOCUS_MAIN_PANEL_RADIUS, FOCUS_COLOR_PANEL, LV_OPA_COVER,
        FOCUS_COLOR_CREAM_TRACK, 1);

    for (uint32_t i = 0; i < 6; ++i) {
        const int x = (int)(i % 3) * 96;
        const int y = (int)(i / 3) * 96;
        s_texture_tiles[i] = today_image(ui_Today_focus_panel,
                                         &ui_img_focus_paper_texture, x, y);
        lv_obj_set_style_opa(s_texture_tiles[i], FOCUS_PAPER_TEXTURE_OPA, 0);
    }

    s_corner_vine = today_image(ui_Today, &ui_img_focus_corner_vine_right,
                                FOCUS_CORNER_VINE_X, FOCUS_CORNER_VINE_Y);
    s_wave_one = today_panel(ui_Today, 52, 51, 128, 128, LV_RADIUS_CIRCLE,
                             FOCUS_COLOR_PANEL, LV_OPA_TRANSP,
                             FOCUS_COLOR_CORAL_TOMATO, 2);
    s_wave_two = today_panel(ui_Today, 52, 51, 128, 128, LV_RADIUS_CIRCLE,
                             FOCUS_COLOR_PANEL, LV_OPA_TRANSP,
                             FOCUS_COLOR_CREAM_TRACK, 2);

    ui_Today_timer_arc = lv_arc_create(ui_Today);
    lv_obj_set_pos(ui_Today_timer_arc, FOCUS_ARC_X, FOCUS_ARC_Y);
    lv_obj_set_size(ui_Today_timer_arc, FOCUS_ARC_SIZE, FOCUS_ARC_SIZE);
    lv_arc_set_rotation(ui_Today_timer_arc, FOCUS_ARC_START_DEG);
    lv_arc_set_bg_angles(ui_Today_timer_arc, 0,
                         FOCUS_ARC_END_DEG - FOCUS_ARC_START_DEG);
    lv_arc_set_range(ui_Today_timer_arc, 0, 1000);
    lv_arc_set_value(ui_Today_timer_arc, 0);
    lv_obj_remove_style(ui_Today_timer_arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(ui_Today_timer_arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(ui_Today_timer_arc, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_arc_width(ui_Today_timer_arc,
                               FOCUS_ARC_LINE_WIDTH, LV_PART_MAIN);
    lv_obj_set_style_arc_width(ui_Today_timer_arc,
                               FOCUS_ARC_LINE_WIDTH, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(ui_Today_timer_arc,
                               lv_color_hex(FOCUS_COLOR_CREAM_TRACK),
                               LV_PART_MAIN);
    lv_obj_set_style_arc_color(ui_Today_timer_arc,
                               lv_color_hex(FOCUS_COLOR_CORAL_TOMATO),
                               LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(ui_Today_timer_arc, true, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(ui_Today_timer_arc, true, LV_PART_INDICATOR);

    s_ring_overlay = today_image(ui_Today, &ui_img_focus_ring_overlay,
                                 FOCUS_RING_OVERLAY_X, FOCUS_RING_OVERLAY_Y);
    s_tomato_crown = today_image(ui_Today, &ui_img_focus_tomato_crown,
                                 FOCUS_CROWN_X, FOCUS_CROWN_Y);
    s_crown_emblem = today_image(ui_Today, &ui_img_focus_tomato_emblem,
                                 101, 40);
    lv_obj_set_style_transform_zoom(s_crown_emblem, 120, 0);

    ui_Today_focus_time = today_label(
        ui_Today, "25:00", FOCUS_TIMER_X, FOCUS_TIMER_Y, FOCUS_TIMER_W,
        FOCUS_COLOR_DEEP_GREEN, &ui_font_FocusDigits, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_text_letter_space(ui_Today_focus_time, 1, 0);
    lv_obj_set_style_transform_pivot_x(ui_Today_focus_time,
                                       FOCUS_TIMER_W / 2, 0);
    lv_obj_set_style_transform_pivot_y(ui_Today_focus_time, 20, 0);
    lv_obj_set_style_transform_zoom(ui_Today_focus_time,
                                    FOCUS_TIMER_ZOOM, 0);

    s_period_label = today_label(ui_Today, "准备时段",
                                 FOCUS_PERIOD_X, FOCUS_PERIOD_Y,
                                 FOCUS_PERIOD_W, FOCUS_COLOR_LEAF_GREEN,
                                 &font_puhui_16_4, LV_TEXT_ALIGN_CENTER);

    s_sparkle_large = today_image(ui_Today, &ui_img_focus_sparkle_large,
                                  FOCUS_SPARKLE_LARGE_X,
                                  FOCUS_SPARKLE_LARGE_Y);
    s_sparkle_small = today_image(ui_Today, &ui_img_focus_sparkle_small,
                                  FOCUS_SPARKLE_SMALL_X,
                                  FOCUS_SPARKLE_SMALL_Y);
    s_orbit_trail = today_image(ui_Today, &ui_img_focus_orbit_trail, 0, 0);
    s_orbit_glow = today_image(ui_Today, &ui_img_focus_orbit_glow, 0, 0);
    lv_obj_set_style_transform_pivot_x(s_orbit_glow, 12, 0);
    lv_obj_set_style_transform_pivot_y(s_orbit_glow, 12, 0);
    ui_Today_set_focus_orbit_value(720);

    ui_Today_mascot_back = today_image(ui_Today,
                                       &ui_img_focus_mascot_ready,
                                       FOCUS_MASCOT_X, FOCUS_MASCOT_Y);
    lv_obj_set_style_opa(ui_Today_mascot_back, LV_OPA_TRANSP, 0);
    ui_Today_mascot_front = today_image(ui_Today,
                                        &ui_img_focus_mascot_ready,
                                        FOCUS_MASCOT_X, FOCUS_MASCOT_Y);

    ui_Today_duration_left = today_button(
        ui_Today, FOCUS_LEFT_BTN_X, FOCUS_LEFT_BTN_Y,
        FOCUS_LEFT_BTN_W, FOCUS_LEFT_BTN_H,
        FOCUS_COLOR_BUTTON_GREEN, FOCUS_COLOR_LEAF_GREEN, false,
        &s_button_left_overlay, &ui_Today_left_btn_icon,
        &ui_Today_left_btn_label);
    ui_Today_focus_btn = today_button(
        ui_Today, FOCUS_MAIN_BTN_X, FOCUS_MAIN_BTN_Y,
        FOCUS_MAIN_BTN_W, FOCUS_MAIN_BTN_H,
        FOCUS_COLOR_PRIMARY_CREAM, FOCUS_COLOR_CORAL_TOMATO, true,
        &s_button_main_overlay, &ui_Today_focus_btn_icon,
        &ui_Today_focus_btn_label);
    ui_Today_focus_reset = today_button(
        ui_Today, FOCUS_RIGHT_BTN_X, FOCUS_RIGHT_BTN_Y,
        FOCUS_RIGHT_BTN_W, FOCUS_RIGHT_BTN_H,
        FOCUS_COLOR_BUTTON_GREEN, FOCUS_COLOR_LEAF_GREEN, false,
        &s_button_right_overlay, &ui_Today_focus_reset_icon,
        &ui_Today_focus_reset_label);

    ui_Today_focus_unit = today_label(ui_Today, "", 0, 0, 1,
                                      FOCUS_COLOR_LEAF_GREEN,
                                      &font_puhui_16_4,
                                      LV_TEXT_ALIGN_CENTER);
    ui_Today_focus_hint = today_label(ui_Today, "", 0, 0, 1,
                                      FOCUS_COLOR_LEAF_GREEN,
                                      &font_puhui_16_4,
                                      LV_TEXT_ALIGN_CENTER);
    lv_obj_add_flag(ui_Today_focus_unit, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_Today_focus_hint, LV_OBJ_FLAG_HIDDEN);

    ui_Today_header = s_title_icon;
    ui_Today_corner_vine = s_corner_vine;
    ui_Today_left_decor = s_sparkle_large;
    ui_Today_top_back = s_texture_tiles[0];
    ui_Today_state_panel = ui_Today_status_panel;
    ui_Today_state_label = ui_Today_status;
    ui_Today_primary_icon = ui_Today_focus_btn_icon;
    ui_Today_secondary_icon = ui_Today_focus_reset_icon;

    ui_Today_date = NULL;
    ui_Today_scroll = NULL;
    ui_Today_env_panel = NULL;
    ui_Today_env_temp = NULL;
    ui_Today_env_humidity = NULL;
    ui_Today_env_air = NULL;
    ui_Today_exam = NULL;
    ui_Today_project = NULL;
    ui_Today_duration_right = NULL;
    ui_Today_task_panel = NULL;
    ui_Today_task_label = NULL;
    ui_Today_tip_panel = NULL;
    ui_Today_tip_icon = NULL;
    ui_Today_tip_label = NULL;
    ui_Today_top_gear = NULL;
    ui_Today_scrolldots = NULL;
    ui_Today_task_edit_icon = NULL;
    ui_Today_center_icon = NULL;

    s_visual_state = 0xFF;
    ui_Today_set_focus_visual_state(0);
    lv_obj_add_event_cb(ui_Today, ui_event_Today, LV_EVENT_ALL, NULL);
}

void ui_Today_screen_destroy(void)
{
    if (ui_Today != NULL) {
        lv_obj_del(ui_Today);
    }

    ui_Today = NULL;
    ui_Today_header = NULL;
    ui_Today_title = NULL;
    ui_Today_date = NULL;
    ui_Today_scroll = NULL;
    ui_Today_timer_arc = NULL;
    ui_Today_status_panel = NULL;
    ui_Today_status = NULL;
    ui_Today_status_detail = NULL;
    ui_Today_env_panel = NULL;
    ui_Today_env_temp = NULL;
    ui_Today_env_humidity = NULL;
    ui_Today_env_air = NULL;
    ui_Today_countdown_panel = NULL;
    ui_Today_exam = NULL;
    ui_Today_project = NULL;
    ui_Today_focus_panel = NULL;
    ui_Today_focus_time = NULL;
    ui_Today_focus_unit = NULL;
    ui_Today_focus_hint = NULL;
    ui_Today_duration_left = NULL;
    ui_Today_left_btn_icon = NULL;
    ui_Today_left_btn_label = NULL;
    ui_Today_duration_right = NULL;
    ui_Today_task_panel = NULL;
    ui_Today_task_label = NULL;
    ui_Today_focus_btn = NULL;
    ui_Today_focus_btn_icon = NULL;
    ui_Today_focus_btn_label = NULL;
    ui_Today_focus_reset = NULL;
    ui_Today_focus_reset_icon = NULL;
    ui_Today_focus_reset_label = NULL;
    ui_Today_tip_panel = NULL;
    ui_Today_tip_icon = NULL;
    ui_Today_tip_label = NULL;
    ui_Today_mascot_front = NULL;
    ui_Today_mascot_back = NULL;
    ui_Today_left_decor = NULL;
    ui_Today_corner_vine = NULL;
    ui_Today_top_back = NULL;
    ui_Today_top_gear = NULL;
    ui_Today_scrolldots = NULL;
    ui_Today_state_panel = NULL;
    ui_Today_state_label = NULL;
    ui_Today_primary_icon = NULL;
    ui_Today_secondary_icon = NULL;
    ui_Today_task_edit_icon = NULL;
    ui_Today_center_icon = NULL;

    s_title_icon = NULL;
    s_header_emblem = NULL;
    s_period_label = NULL;
    s_ring_overlay = NULL;
    s_tomato_crown = NULL;
    s_crown_emblem = NULL;
    s_corner_vine = NULL;
    s_sparkle_large = NULL;
    s_sparkle_small = NULL;
    s_orbit_glow = NULL;
    s_orbit_trail = NULL;
    s_button_left_overlay = NULL;
    s_button_main_overlay = NULL;
    s_button_right_overlay = NULL;
    s_wave_one = NULL;
    s_wave_two = NULL;
    for (uint32_t i = 0; i < 6; ++i) {
        s_texture_tiles[i] = NULL;
    }
    s_visual_state = 0xFF;
}
