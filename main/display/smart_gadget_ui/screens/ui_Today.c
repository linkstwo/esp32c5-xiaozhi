// Cream-green Pomodoro screen for the 284 x 240 ESP32-C5 product display.
// The layout and effects are intentionally implemented with ordinary LVGL objects:
// arc, circles, opacity, small trails and two completion ripple rings.

#include "../ui.h"
#include "focus_ui_tokens.h"

#include <font_awesome.h>
#include <string.h>

LV_FONT_DECLARE(font_puhui_16_4);
LV_FONT_DECLARE(BUILTIN_ICON_FONT);

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

/* Local visual-effect objects. They do not leak into the product logic layer. */
static lv_obj_t * s_focus_card = NULL;
static lv_obj_t * s_focus_halo = NULL;
static lv_obj_t * s_focus_ripple_a = NULL;
static lv_obj_t * s_focus_ripple_b = NULL;
static lv_obj_t * s_focus_orbit_glow = NULL;
static lv_obj_t * s_focus_orbit_dot = NULL;
static lv_obj_t * s_focus_trail[3] = {NULL, NULL, NULL};
static lv_obj_t * s_focus_twinkle[3] = {NULL, NULL, NULL};
static lv_obj_t * s_focus_complete_icon = NULL;
static lv_timer_t * s_focus_fx_timer = NULL;

typedef enum {
    FOCUS_FX_READY = 0,
    FOCUS_FX_RUNNING,
    FOCUS_FX_PAUSED,
    FOCUS_FX_FINISHED,
} focus_fx_state_t;

static focus_fx_state_t s_focus_fx_state = FOCUS_FX_READY;
static uint16_t s_focus_fx_phase = 0;
static uint16_t s_focus_finish_frame = 0;

static void today_clear_obj_style(lv_obj_t * obj)
{
    lv_obj_remove_style_all(obj);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

static lv_obj_t * today_box_create(lv_obj_t * parent, int x, int y, int width, int height,
                                   int radius, uint32_t color, lv_opa_t opa)
{
    lv_obj_t * obj = lv_obj_create(parent);
    today_clear_obj_style(obj);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, width, height);
    lv_obj_set_style_radius(obj, radius, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(obj, opa, 0);
    return obj;
}

static lv_obj_t * today_circle_create(lv_obj_t * parent, int x, int y, int size,
                                      uint32_t color, lv_opa_t opa)
{
    return today_box_create(parent, x, y, size, size, LV_RADIUS_CIRCLE, color, opa);
}

static lv_obj_t * today_label_create(lv_obj_t * parent, const char * text, int x, int y, int width,
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

static lv_obj_t * today_button_create(lv_obj_t * parent, int x, int y, int width, int height,
                                      uint32_t bg_color, uint32_t border_color, bool primary)
{
    lv_obj_t * btn = lv_obj_create(parent);
    today_clear_obj_style(btn);
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_size(btn, width, height);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_radius(btn, height / 2, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(bg_color), 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(btn, lv_color_hex(border_color), 0);
    lv_obj_set_style_border_width(btn, primary ? 2 : 1, 0);
    lv_obj_set_style_shadow_color(btn, lv_color_hex(border_color), 0);
    lv_obj_set_style_shadow_opa(btn, primary ? LV_OPA_20 : (lv_opa_t)20, 0);
    lv_obj_set_style_shadow_width(btn, primary ? 9 : 5, 0);
    lv_obj_set_style_shadow_offset_y(btn, 2, 0);
    lv_obj_set_style_pad_left(btn, 5, 0);
    lv_obj_set_style_pad_right(btn, 5, 0);
    lv_obj_set_style_pad_top(btn, 0, 0);
    lv_obj_set_style_pad_bottom(btn, 0, 0);
    lv_obj_set_style_pad_column(btn, 5, 0);
    lv_obj_set_layout(btn, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_transform_zoom(btn, 256, 0);
    lv_obj_set_style_transform_zoom(btn, 239, LV_STATE_PRESSED);
    return btn;
}

static lv_obj_t * today_button_icon_create(lv_obj_t * parent)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_text(label, "");
    lv_obj_set_style_text_color(label, lv_color_hex(FOCUS_COLOR_TEXT_GREEN), 0);
    lv_obj_set_style_text_font(label, &BUILTIN_ICON_FONT, 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return label;
}

static lv_obj_t * today_button_text_create(lv_obj_t * parent)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_text(label, "");
    lv_obj_set_style_text_color(label, lv_color_hex(FOCUS_COLOR_TEXT_GREEN), 0);
    lv_obj_set_style_text_font(label, &font_puhui_16_4, 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return label;
}

static lv_obj_t * today_leaf_create(lv_obj_t * parent, int x, int y, int width, int height, int rotation)
{
    lv_obj_t * leaf = today_box_create(parent, x, y, width, height, LV_RADIUS_CIRCLE,
                                       FOCUS_COLOR_DECOR_GREEN, LV_OPA_60);
    lv_obj_set_style_transform_rotation(leaf, rotation, 0);
    return leaf;
}

static lv_obj_t * today_sparkle_create(lv_obj_t * parent, int x, int y)
{
    lv_obj_t * sparkle = today_label_create(parent, "+", x, y, 10,
                                            0xFFFFFF, &font_puhui_16_4, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_transform_zoom(sparkle, 155, 0);
    lv_obj_set_style_opa(sparkle, (lv_opa_t)90, 0);
    return sparkle;
}

static int32_t focus_triangle_wave(uint16_t phase)
{
    phase &= 0x01FF;
    return phase < 256 ? phase : (511 - phase);
}

static void focus_place_circle_on_orbit(lv_obj_t * obj, int size, int angle_deg, int radius)
{
    int32_t sin_value;
    int32_t cos_value;
    int center_x;
    int center_y;
    int x;
    int y;

    if(obj == NULL) {
        return;
    }

    while(angle_deg < 0) {
        angle_deg += 360;
    }
    while(angle_deg >= 360) {
        angle_deg -= 360;
    }

    sin_value = lv_trigo_sin((int16_t)angle_deg);
    cos_value = lv_trigo_sin((int16_t)(angle_deg + 90));
    center_x = FOCUS_TIMER_X + FOCUS_TIMER_SIZE / 2;
    center_y = FOCUS_TIMER_Y + FOCUS_TIMER_SIZE / 2;
    x = center_x + (radius * cos_value) / 32767 - size / 2;
    y = center_y + (radius * sin_value) / 32767 - size / 2;

    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, size, size);
}

static focus_fx_state_t focus_infer_state(void)
{
    const char * main_text;

    if(ui_Today_focus_btn_label == NULL) {
        return FOCUS_FX_READY;
    }

    main_text = lv_label_get_text(ui_Today_focus_btn_label);
    if(main_text == NULL) {
        return FOCUS_FX_READY;
    }
    if(strcmp(main_text, "继续") == 0) {
        return FOCUS_FX_PAUSED;
    }
    if(strcmp(main_text, "再来一轮") == 0) {
        return FOCUS_FX_FINISHED;
    }
    if(strcmp(main_text, "结束") == 0) {
        return FOCUS_FX_RUNNING;
    }
    return FOCUS_FX_READY;
}

static void focus_set_button_text_colors(void)
{
    /* Product logic refreshes labels every 250 ms; the visual layer keeps the main action white. */
    if(ui_Today_left_btn_icon != NULL) {
        lv_obj_set_style_text_color(ui_Today_left_btn_icon, lv_color_hex(FOCUS_COLOR_TEXT_GREEN), 0);
    }
    if(ui_Today_left_btn_label != NULL) {
        lv_obj_set_style_text_color(ui_Today_left_btn_label, lv_color_hex(FOCUS_COLOR_TEXT_GREEN), 0);
    }
    if(ui_Today_focus_btn_icon != NULL) {
        lv_obj_set_style_text_color(ui_Today_focus_btn_icon, lv_color_hex(0xFFFFFF), 0);
    }
    if(ui_Today_focus_btn_label != NULL) {
        lv_obj_set_style_text_color(ui_Today_focus_btn_label, lv_color_hex(0xFFFFFF), 0);
    }
    if(ui_Today_focus_reset_icon != NULL) {
        lv_obj_set_style_text_color(ui_Today_focus_reset_icon, lv_color_hex(FOCUS_COLOR_TEXT_GREEN), 0);
    }
    if(ui_Today_focus_reset_label != NULL) {
        lv_obj_set_style_text_color(ui_Today_focus_reset_label, lv_color_hex(FOCUS_COLOR_TEXT_GREEN), 0);
    }
}

static void focus_hide_ripples(void)
{
    if(s_focus_ripple_a != NULL) {
        lv_obj_add_flag(s_focus_ripple_a, LV_OBJ_FLAG_HIDDEN);
    }
    if(s_focus_ripple_b != NULL) {
        lv_obj_add_flag(s_focus_ripple_b, LV_OBJ_FLAG_HIDDEN);
    }
}

static void focus_enter_state(focus_fx_state_t state)
{
    s_focus_fx_state = state;
    s_focus_fx_phase = 0;
    s_focus_finish_frame = 0;

    if(ui_Today_state_label != NULL) {
        switch(state) {
        case FOCUS_FX_RUNNING:
            lv_label_set_text(ui_Today_state_label, "专注中");
            break;
        case FOCUS_FX_PAUSED:
            lv_label_set_text(ui_Today_state_label, "已暂停");
            break;
        case FOCUS_FX_FINISHED:
            lv_label_set_text(ui_Today_state_label, "完成啦");
            break;
        case FOCUS_FX_READY:
        default:
            lv_label_set_text(ui_Today_state_label, "准备专注");
            break;
        }
    }

    if(s_focus_complete_icon != NULL) {
        if(state == FOCUS_FX_FINISHED) {
            lv_obj_clear_flag(s_focus_complete_icon, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(s_focus_complete_icon, LV_OBJ_FLAG_HIDDEN);
        }
    }

    if(state != FOCUS_FX_FINISHED) {
        focus_hide_ripples();
    }
}

static void focus_update_orbit(void)
{
    int32_t value = 1000;
    int angle;
    int32_t pulse;
    int i;

    if(ui_Today_timer_arc != NULL) {
        value = lv_arc_get_value(ui_Today_timer_arc);
    }
    if(value < 0) {
        value = 0;
    }
    if(value > 1000) {
        value = 1000;
    }

    angle = FOCUS_ARC_START_DEG + (value * FOCUS_ARC_SPAN_DEG) / 1000;
    pulse = focus_triangle_wave(s_focus_fx_phase);

    if(s_focus_fx_state == FOCUS_FX_FINISHED) {
        if(s_focus_orbit_dot != NULL) {
            lv_obj_add_flag(s_focus_orbit_dot, LV_OBJ_FLAG_HIDDEN);
        }
        if(s_focus_orbit_glow != NULL) {
            lv_obj_add_flag(s_focus_orbit_glow, LV_OBJ_FLAG_HIDDEN);
        }
        for(i = 0; i < 3; ++i) {
            if(s_focus_trail[i] != NULL) {
                lv_obj_add_flag(s_focus_trail[i], LV_OBJ_FLAG_HIDDEN);
            }
        }
        return;
    }

    if(s_focus_orbit_dot != NULL) {
        lv_obj_clear_flag(s_focus_orbit_dot, LV_OBJ_FLAG_HIDDEN);
        focus_place_circle_on_orbit(s_focus_orbit_dot, 8, angle, 68);
        lv_obj_set_style_bg_opa(s_focus_orbit_dot, LV_OPA_COVER, 0);
    }
    if(s_focus_orbit_glow != NULL) {
        lv_obj_clear_flag(s_focus_orbit_glow, LV_OBJ_FLAG_HIDDEN);
        focus_place_circle_on_orbit(s_focus_orbit_glow,
                                    s_focus_fx_state == FOCUS_FX_RUNNING ? 17 : 14,
                                    angle, 68);
        if(s_focus_fx_state == FOCUS_FX_RUNNING) {
            lv_obj_set_style_bg_opa(s_focus_orbit_glow, (lv_opa_t)(35 + pulse / 4), 0);
        } else if(s_focus_fx_state == FOCUS_FX_PAUSED) {
            lv_obj_set_style_bg_opa(s_focus_orbit_glow, LV_OPA_20, 0);
        } else {
            lv_obj_set_style_bg_opa(s_focus_orbit_glow, (lv_opa_t)90, 0);
        }
    }

    for(i = 0; i < 3; ++i) {
        if(s_focus_trail[i] == NULL) {
            continue;
        }
        if(s_focus_fx_state == FOCUS_FX_RUNNING) {
            lv_obj_clear_flag(s_focus_trail[i], LV_OBJ_FLAG_HIDDEN);
            focus_place_circle_on_orbit(s_focus_trail[i], 4 - i, angle + 6 + i * 6, 68);
            lv_obj_set_style_bg_opa(s_focus_trail[i], (lv_opa_t)(120 - i * 35), 0);
        } else {
            lv_obj_add_flag(s_focus_trail[i], LV_OBJ_FLAG_HIDDEN);
        }
    }
}

static void focus_update_breathing(void)
{
    int32_t pulse = focus_triangle_wave(s_focus_fx_phase);
    lv_opa_t halo_opa;

    if(s_focus_halo == NULL) {
        return;
    }

    switch(s_focus_fx_state) {
    case FOCUS_FX_RUNNING:
        halo_opa = (lv_opa_t)(18 + pulse / 7);
        break;
    case FOCUS_FX_PAUSED:
        halo_opa = (lv_opa_t)20;
        break;
    case FOCUS_FX_FINISHED:
        halo_opa = (lv_opa_t)(42 + pulse / 4);
        break;
    case FOCUS_FX_READY:
    default:
        halo_opa = (lv_opa_t)(12 + pulse / 12);
        break;
    }

    lv_obj_set_style_bg_opa(s_focus_halo, halo_opa, 0);

    if(s_focus_card != NULL) {
        if(s_focus_fx_state == FOCUS_FX_PAUSED) {
            lv_obj_set_style_shadow_opa(s_focus_card, (lv_opa_t)12, 0);
        } else if(s_focus_fx_state == FOCUS_FX_FINISHED) {
            lv_obj_set_style_shadow_opa(s_focus_card, (lv_opa_t)64, 0);
        } else {
            lv_obj_set_style_shadow_opa(s_focus_card, (lv_opa_t)30, 0);
        }
    }

    if(s_focus_twinkle[0] != NULL) {
        lv_obj_set_style_opa(s_focus_twinkle[0], (lv_opa_t)(35 + pulse / 3), 0);
    }
    if(s_focus_twinkle[1] != NULL) {
        lv_obj_set_style_opa(s_focus_twinkle[1], (lv_opa_t)(30 + (255 - pulse) / 4), 0);
    }
    if(s_focus_twinkle[2] != NULL) {
        lv_obj_set_style_opa(s_focus_twinkle[2],
                             s_focus_fx_state == FOCUS_FX_RUNNING ? (lv_opa_t)(20 + pulse / 3) : LV_OPA_20, 0);
    }
}

static void focus_update_completion_ripples(void)
{
    int center_x = FOCUS_TIMER_X + FOCUS_TIMER_SIZE / 2;
    int center_y = FOCUS_TIMER_Y + FOCUS_TIMER_SIZE / 2;
    int frame_a;
    int frame_b;
    int size;
    int opa;

    if(s_focus_fx_state != FOCUS_FX_FINISHED) {
        return;
    }

    ++s_focus_finish_frame;
    frame_a = s_focus_finish_frame;
    frame_b = s_focus_finish_frame > 8 ? (s_focus_finish_frame - 8) : -1;

    if(s_focus_ripple_a != NULL) {
        if(frame_a <= 28) {
            size = 146 + frame_a * 2;
            opa = 150 - frame_a * 5;
            if(opa < 0) {
                opa = 0;
            }
            lv_obj_clear_flag(s_focus_ripple_a, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_pos(s_focus_ripple_a, center_x - size / 2, center_y - size / 2);
            lv_obj_set_size(s_focus_ripple_a, size, size);
            lv_obj_set_style_opa(s_focus_ripple_a, (lv_opa_t)opa, 0);
        } else {
            lv_obj_add_flag(s_focus_ripple_a, LV_OBJ_FLAG_HIDDEN);
        }
    }

    if(s_focus_ripple_b != NULL) {
        if(frame_b >= 0 && frame_b <= 26) {
            size = 146 + frame_b * 2;
            opa = 115 - frame_b * 4;
            if(opa < 0) {
                opa = 0;
            }
            lv_obj_clear_flag(s_focus_ripple_b, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_pos(s_focus_ripple_b, center_x - size / 2, center_y - size / 2);
            lv_obj_set_size(s_focus_ripple_b, size, size);
            lv_obj_set_style_opa(s_focus_ripple_b, (lv_opa_t)opa, 0);
        } else {
            lv_obj_add_flag(s_focus_ripple_b, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

static void focus_fx_timer_cb(lv_timer_t * timer)
{
    focus_fx_state_t inferred_state;
    (void)timer;

    if(ui_Today == NULL || lv_screen_active() != ui_Today) {
        return;
    }

    inferred_state = focus_infer_state();
    if(inferred_state != s_focus_fx_state) {
        focus_enter_state(inferred_state);
    }

    s_focus_fx_phase = (uint16_t)((s_focus_fx_phase + 18) & 0x01FF);
    focus_set_button_text_colors();
    if(s_focus_fx_state == FOCUS_FX_FINISHED && ui_Today_focus_time != NULL) {
        lv_label_set_text(ui_Today_focus_time, "完成!");
    }
    focus_update_orbit();
    focus_update_breathing();
    focus_update_completion_ripples();
}

void ui_event_Today(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_SCREEN_LOAD_START) {
        if(s_focus_card != NULL) {
            upanim_Animation(s_focus_card, 30);
        }
        if(ui_Today_timer_arc != NULL) {
            upanim_Animation(ui_Today_timer_arc, 55);
        }
        if(ui_Today_mascot_front != NULL) {
            upanim_Animation(ui_Today_mascot_front, 90);
        }
        if(ui_Today_duration_left != NULL) {
            upanim_Animation(ui_Today_duration_left, 120);
        }
        if(ui_Today_focus_btn != NULL) {
            upanim_Animation(ui_Today_focus_btn, 150);
        }
        if(ui_Today_focus_reset != NULL) {
            upanim_Animation(ui_Today_focus_reset, 180);
        }
    }

    if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_RIGHT) {
        lv_indev_wait_release(lv_indev_active());
        _ui_screen_change(&ui_Clock, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Clock_screen_init);
    }
    if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_LEFT) {
        lv_indev_wait_release(lv_indev_active());
        _ui_screen_change(&ui_Music_Player, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Music_Player_screen_init);
    }
}

void ui_Today_screen_init(void)
{
    lv_obj_t * tomato_leaf;
    lv_obj_t * session_pill;
    lv_obj_t * dot;
    uint32_t i;

    ui_Today = lv_obj_create(NULL);
    lv_obj_remove_flag(ui_Today, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Today, lv_color_hex(FOCUS_COLOR_READY_BG), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_Today, lv_color_hex(0xFFFBEF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_Today, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Today, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Today, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_Today, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* Compact tomato title, made with native LVGL geometry. */
    ui_Today_header = today_circle_create(ui_Today, 8, 4, 18, 0xFFFFFF, LV_OPA_TRANSP);
    lv_obj_set_style_border_color(ui_Today_header, lv_color_hex(FOCUS_COLOR_TEXT_GREEN), 0);
    lv_obj_set_style_border_width(ui_Today_header, 2, 0);
    tomato_leaf = today_leaf_create(ui_Today, 15, 1, 9, 4, 3350);
    lv_obj_set_style_bg_opa(tomato_leaf, LV_OPA_COVER, 0);

    ui_Today_title = today_label_create(ui_Today, "番茄钟", 31, 3, 70,
                                        FOCUS_COLOR_TEXT_MAIN, &font_puhui_16_4, LV_TEXT_ALIGN_LEFT);
    lv_obj_set_style_transform_zoom(ui_Today_title, 238, 0);

    today_leaf_create(ui_Today, 92, 10, 8, 4, 3280);

    session_pill = today_box_create(ui_Today, 192, 3, 62, 21, 11, 0xFFF9E8, LV_OPA_COVER);
    lv_obj_set_style_border_color(session_pill, lv_color_hex(0xD0D89D), 0);
    lv_obj_set_style_border_width(session_pill, 1, 0);
    today_leaf_create(session_pill, 7, 8, 8, 4, 3300);
    ui_Today_date = today_label_create(session_pill, "2 / 4", 16, 2, 42,
                                       FOCUS_COLOR_TEXT_MAIN, &font_puhui_16_4, LV_TEXT_ALIGN_CENTER);

    ui_Today_scroll = today_box_create(ui_Today, 258, 7, 24, 10, 0, 0xFFFFFF, LV_OPA_TRANSP);
    for(i = 0; i < 3; ++i) {
        dot = today_circle_create(ui_Today_scroll, (int)i * 9, 1, 6,
                                  i == 0 ? FOCUS_COLOR_TEXT_GREEN : 0xC8D29A,
                                  i == 0 ? LV_OPA_COVER : LV_OPA_70);
        (void)dot;
    }
    ui_Today_scrolldots = ui_Today_scroll;

    /* Main cream card. */
    s_focus_card = today_box_create(ui_Today, FOCUS_CARD_X, FOCUS_CARD_Y, FOCUS_CARD_W,
                                    FOCUS_CARD_H, FOCUS_CARD_RADIUS,
                                    FOCUS_COLOR_PANEL, LV_OPA_COVER);
    lv_obj_set_style_border_color(s_focus_card, lv_color_hex(FOCUS_COLOR_PANEL_BORDER), 0);
    lv_obj_set_style_border_width(s_focus_card, 1, 0);
    lv_obj_set_style_shadow_color(s_focus_card, lv_color_hex(FOCUS_COLOR_RUNNING_ACCENT), 0);
    lv_obj_set_style_shadow_opa(s_focus_card, (lv_opa_t)30, 0);
    lv_obj_set_style_shadow_width(s_focus_card, 10, 0);
    lv_obj_set_style_shadow_offset_y(s_focus_card, 2, 0);
    ui_Today_status_panel = s_focus_card;

    /* Sparse leaves and sparkles keep the anime tone without filling the tiny screen. */
    ui_Today_left_decor = today_leaf_create(ui_Today, 18, 56, 12, 6, 3310);
    today_leaf_create(ui_Today, 24, 121, 10, 5, 300);
    today_leaf_create(ui_Today, 241, 56, 11, 5, 3240);
    today_leaf_create(ui_Today, 246, 117, 10, 5, 320);
    s_focus_twinkle[0] = today_sparkle_create(ui_Today, 42, 70);
    s_focus_twinkle[1] = today_sparkle_create(ui_Today, 227, 83);
    s_focus_twinkle[2] = today_sparkle_create(ui_Today, 214, 51);

    /* The subtle breathing disc sits underneath the arc and inner timer panel. */
    s_focus_halo = today_circle_create(ui_Today, FOCUS_TIMER_X - 5, FOCUS_TIMER_Y - 5,
                                       FOCUS_TIMER_SIZE + 10, FOCUS_COLOR_RUNNING_ACCENT, (lv_opa_t)38);

    s_focus_ripple_a = today_circle_create(ui_Today, FOCUS_TIMER_X, FOCUS_TIMER_Y,
                                           FOCUS_TIMER_SIZE, 0xFFFFFF, LV_OPA_TRANSP);
    lv_obj_set_style_border_color(s_focus_ripple_a, lv_color_hex(0xE8DB78), 0);
    lv_obj_set_style_border_width(s_focus_ripple_a, 2, 0);
    lv_obj_add_flag(s_focus_ripple_a, LV_OBJ_FLAG_HIDDEN);

    s_focus_ripple_b = today_circle_create(ui_Today, FOCUS_TIMER_X, FOCUS_TIMER_Y,
                                           FOCUS_TIMER_SIZE, 0xFFFFFF, LV_OPA_TRANSP);
    lv_obj_set_style_border_color(s_focus_ripple_b, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_border_width(s_focus_ripple_b, 1, 0);
    lv_obj_add_flag(s_focus_ripple_b, LV_OBJ_FLAG_HIDDEN);

    ui_Today_timer_arc = lv_arc_create(ui_Today);
    lv_obj_set_pos(ui_Today_timer_arc, FOCUS_TIMER_X, FOCUS_TIMER_Y);
    lv_obj_set_size(ui_Today_timer_arc, FOCUS_TIMER_SIZE, FOCUS_TIMER_SIZE);
    lv_arc_set_rotation(ui_Today_timer_arc, FOCUS_ARC_START_DEG);
    lv_arc_set_bg_angles(ui_Today_timer_arc, 0, FOCUS_ARC_SPAN_DEG);
    lv_arc_set_range(ui_Today_timer_arc, 0, 1000);
    lv_arc_set_value(ui_Today_timer_arc, 1000);
    lv_obj_remove_style(ui_Today_timer_arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(ui_Today_timer_arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(ui_Today_timer_arc, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_arc_rounded(ui_Today_timer_arc, true, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_rounded(ui_Today_timer_arc, true, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_Today_timer_arc, FOCUS_ARC_MAIN_WIDTH, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_Today_timer_arc, FOCUS_ARC_INDICATOR_WIDTH,
                               LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui_Today_timer_arc, lv_color_hex(FOCUS_COLOR_TRACK),
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui_Today_timer_arc, lv_color_hex(FOCUS_COLOR_READY_ACCENT),
                               LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_Today_timer_arc, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_Today_timer_arc, LV_OPA_COVER, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    ui_Today_focus_panel = today_circle_create(ui_Today, FOCUS_TIMER_INNER_X, FOCUS_TIMER_INNER_Y,
                                               FOCUS_TIMER_INNER_SIZE, FOCUS_COLOR_PANEL, (lv_opa_t)235);
    lv_obj_set_style_border_color(ui_Today_focus_panel, lv_color_hex(0xE8E6C8), 0);
    lv_obj_set_style_border_width(ui_Today_focus_panel, 1, 0);

    /* Ghost tomato crown behind the time. */
    today_leaf_create(ui_Today_focus_panel, 49, 15, 17, 8, 3360);
    today_leaf_create(ui_Today_focus_panel, 58, 13, 17, 8, 240);
    today_leaf_create(ui_Today_focus_panel, 54, 9, 14, 7, 0);

    ui_Today_focus_hint = today_label_create(ui_Today_focus_panel, "", 0, 18,
                                             FOCUS_TIMER_INNER_SIZE, FOCUS_COLOR_TEXT_GREEN,
                                             &font_puhui_16_4, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_transform_zoom(ui_Today_focus_hint, 185, 0);
    lv_obj_set_style_text_opa(ui_Today_focus_hint, LV_OPA_TRANSP, 0);
    lv_obj_add_flag(ui_Today_focus_hint, LV_OBJ_FLAG_HIDDEN);

    ui_Today_focus_time = today_label_create(ui_Today_focus_panel, "25:00", 0, 48,
                                             FOCUS_TIMER_INNER_SIZE, FOCUS_COLOR_TEXT_MAIN,
                                             &ui_font_Number, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_transform_zoom(ui_Today_focus_time, 148, 0);

    ui_Today_focus_unit = today_label_create(ui_Today_focus_panel, "", 0, 92,
                                             FOCUS_TIMER_INNER_SIZE, FOCUS_COLOR_TEXT_GREEN,
                                             &font_puhui_16_4, LV_TEXT_ALIGN_CENTER);
    lv_obj_add_flag(ui_Today_focus_unit, LV_OBJ_FLAG_HIDDEN);

    ui_Today_state_panel = today_box_create(ui_Today, 99, 143, 86, 21, 11,
                                            0xF4F2C9, LV_OPA_COVER);
    lv_obj_set_style_border_color(ui_Today_state_panel, lv_color_hex(0xD4DB9E), 0);
    lv_obj_set_style_border_width(ui_Today_state_panel, 1, 0);
    ui_Today_state_label = today_label_create(ui_Today_state_panel, "准备专注", 0, 2, 86,
                                              FOCUS_COLOR_TEXT_GREEN, &font_puhui_16_4,
                                              LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_transform_zoom(ui_Today_state_label, 196, 0);

    s_focus_complete_icon = today_label_create(ui_Today, FONT_AWESOME_CIRCLE_CHECK,
                                               116, 51, 52, FOCUS_COLOR_FINISHED_ACCENT,
                                               &BUILTIN_ICON_FONT, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_transform_zoom(s_focus_complete_icon, 240, 0);
    lv_obj_add_flag(s_focus_complete_icon, LV_OBJ_FLAG_HIDDEN);

    /* Orbit bead and a three-dot light trail. */
    s_focus_orbit_glow = today_circle_create(ui_Today, 0, 0, 17, 0xE9EE87, (lv_opa_t)115);
    s_focus_orbit_dot = today_circle_create(ui_Today, 0, 0, 8, 0xFFFFFF, LV_OPA_COVER);
    lv_obj_set_style_border_color(s_focus_orbit_dot, lv_color_hex(0xD9E96E), 0);
    lv_obj_set_style_border_width(s_focus_orbit_dot, 2, 0);
    for(i = 0; i < 3; ++i) {
        s_focus_trail[i] = today_circle_create(ui_Today, 0, 0, 4 - (int)i,
                                               0xFFFFFF, LV_OPA_20);
        lv_obj_add_flag(s_focus_trail[i], LV_OBJ_FLAG_HIDDEN);
    }

    /* Existing four-state mascot assets are reused, so the program and reference art stay aligned. */
    ui_Today_mascot_back = lv_image_create(ui_Today);
    lv_image_set_src(ui_Today_mascot_back, &ui_img_focus_mascot_ready);
    lv_obj_set_pos(ui_Today_mascot_back, FOCUS_MASCOT_RIGHT_X, FOCUS_MASCOT_RIGHT_Y);
    lv_obj_remove_flag(ui_Today_mascot_back, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(ui_Today_mascot_back, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_opa(ui_Today_mascot_back, LV_OPA_TRANSP, 0);
    lv_obj_set_style_transform_zoom(ui_Today_mascot_back, 256, 0);

    ui_Today_mascot_front = lv_image_create(ui_Today);
    lv_image_set_src(ui_Today_mascot_front, &ui_img_focus_mascot_ready);
    lv_obj_set_pos(ui_Today_mascot_front, FOCUS_MASCOT_RIGHT_X, FOCUS_MASCOT_RIGHT_Y);
    lv_obj_remove_flag(ui_Today_mascot_front, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(ui_Today_mascot_front, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_transform_zoom(ui_Today_mascot_front, 256, 0);

    ui_Today_duration_left = today_button_create(ui_Today, FOCUS_LEFT_BTN_X, FOCUS_LEFT_BTN_Y,
                                                 FOCUS_LEFT_BTN_W, FOCUS_LEFT_BTN_H,
                                                 FOCUS_COLOR_READY_LEFT, FOCUS_COLOR_READY_ACCENT, false);
    ui_Today_left_btn_icon = today_button_icon_create(ui_Today_duration_left);
    ui_Today_left_btn_label = today_button_text_create(ui_Today_duration_left);

    ui_Today_focus_btn = today_button_create(ui_Today, FOCUS_MAIN_BTN_X, FOCUS_MAIN_BTN_Y,
                                             FOCUS_MAIN_BTN_W, FOCUS_MAIN_BTN_H,
                                             FOCUS_COLOR_READY_PRIMARY, FOCUS_COLOR_READY_ACCENT, true);
    ui_Today_focus_btn_icon = today_button_icon_create(ui_Today_focus_btn);
    ui_Today_focus_btn_label = today_button_text_create(ui_Today_focus_btn);

    ui_Today_focus_reset = today_button_create(ui_Today, FOCUS_RIGHT_BTN_X, FOCUS_RIGHT_BTN_Y,
                                               FOCUS_RIGHT_BTN_W, FOCUS_RIGHT_BTN_H,
                                               FOCUS_COLOR_READY_SECONDARY, FOCUS_COLOR_READY_ACCENT, false);
    ui_Today_focus_reset_icon = today_button_icon_create(ui_Today_focus_reset);
    ui_Today_focus_reset_label = today_button_text_create(ui_Today_focus_reset);

    ui_Today_primary_icon = ui_Today_focus_btn_icon;
    ui_Today_secondary_icon = ui_Today_focus_reset_icon;

    /* Legacy pointers kept intentionally NULL: product code still checks them safely. */
    ui_Today_status = NULL;
    ui_Today_status_detail = NULL;
    ui_Today_env_panel = NULL;
    ui_Today_env_temp = NULL;
    ui_Today_env_humidity = NULL;
    ui_Today_env_air = NULL;
    ui_Today_countdown_panel = NULL;
    ui_Today_exam = NULL;
    ui_Today_project = NULL;
    ui_Today_duration_right = NULL;
    ui_Today_task_panel = NULL;
    ui_Today_task_label = NULL;
    ui_Today_tip_panel = NULL;
    ui_Today_tip_icon = NULL;
    ui_Today_tip_label = NULL;
    ui_Today_corner_vine = NULL;
    ui_Today_top_back = NULL;
    ui_Today_top_gear = NULL;
    ui_Today_task_edit_icon = NULL;
    ui_Today_center_icon = NULL;

    focus_enter_state(FOCUS_FX_READY);
    focus_update_orbit();
    if(s_focus_fx_timer == NULL) {
        s_focus_fx_timer = lv_timer_create(focus_fx_timer_cb, 80, NULL);
    }

    lv_obj_add_event_cb(ui_Today, ui_event_Today, LV_EVENT_ALL, NULL);
}

void ui_Today_screen_destroy(void)
{
    if(s_focus_fx_timer != NULL) {
        lv_timer_delete(s_focus_fx_timer);
        s_focus_fx_timer = NULL;
    }

    if(ui_Today != NULL) {
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

    s_focus_card = NULL;
    s_focus_halo = NULL;
    s_focus_ripple_a = NULL;
    s_focus_ripple_b = NULL;
    s_focus_orbit_glow = NULL;
    s_focus_orbit_dot = NULL;
    s_focus_trail[0] = NULL;
    s_focus_trail[1] = NULL;
    s_focus_trail[2] = NULL;
    s_focus_twinkle[0] = NULL;
    s_focus_twinkle[1] = NULL;
    s_focus_twinkle[2] = NULL;
    s_focus_complete_icon = NULL;
}
