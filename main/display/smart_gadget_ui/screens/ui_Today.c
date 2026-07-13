// This file follows the SquareLine Studio screen style used by Smart_Gadget.

#include "../ui.h"
#include "focus_ui_tokens.h"

#include <font_awesome.h>

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

static void today_clear_obj_style(lv_obj_t * obj)
{
    lv_obj_remove_style_all(obj);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

static lv_obj_t * today_circle_create(lv_obj_t * parent, int x, int y, int size, uint32_t color, lv_opa_t opa)
{
    lv_obj_t * obj = lv_obj_create(parent);
    today_clear_obj_style(obj);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, size, size);
    lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(obj, opa, 0);
    return obj;
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

static lv_obj_t * today_button_create(lv_obj_t * parent, int x, int y, int width, int height, int radius,
                                      uint32_t bg_color, uint32_t border_color)
{
    lv_obj_t * btn;

    btn = lv_obj_create(parent);
    today_clear_obj_style(btn);
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_size(btn, width, height);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_radius(btn, radius, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(bg_color), 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(btn, lv_color_hex(border_color), 0);
    lv_obj_set_style_border_width(btn, 1, 0);
    lv_obj_set_style_pad_left(btn, 10, 0);
    lv_obj_set_style_pad_right(btn, 10, 0);
    lv_obj_set_style_pad_top(btn, 0, 0);
    lv_obj_set_style_pad_bottom(btn, 0, 0);
    lv_obj_set_style_pad_column(btn, 7, 0);
    lv_obj_set_layout(btn, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_transform_zoom(btn, 256, 0);
    lv_obj_set_style_transform_zoom(btn, 238, LV_STATE_PRESSED);
    return btn;
}

static lv_obj_t * today_button_icon_label_create(lv_obj_t * parent, int y, int width)
{
    lv_obj_t * label;
    (void)y;
    (void)width;
    label = lv_label_create(parent);
    lv_label_set_text(label, "");
    lv_obj_set_style_text_color(label, lv_color_hex(FOCUS_COLOR_TEXT_GREEN), 0);
    lv_obj_set_style_text_font(label, &BUILTIN_ICON_FONT, 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return label;
}

static lv_obj_t * today_button_text_label_create(lv_obj_t * parent, int y, int width)
{
    lv_obj_t * label;
    (void)y;
    (void)width;
    label = lv_label_create(parent);
    lv_label_set_text(label, "");
    lv_obj_set_style_text_color(label, lv_color_hex(FOCUS_COLOR_TEXT_GREEN), 0);
    lv_obj_set_style_text_font(label, &font_puhui_16_4, 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return label;
}

static void today_highlight_page_dot(lv_obj_t * dots, uint32_t index)
{
    ui_update_page_indicator(dots, index, 7, FOCUS_COLOR_DECOR_GREEN, 0xD8CFB2);
}

void ui_event_Today(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_SCREEN_LOAD_START) {
        if(ui_Today_timer_arc != NULL) {
            upanim_Animation(ui_Today_timer_arc, 50);
        }
        if(ui_Today_mascot_front != NULL) {
            upanim_Animation(ui_Today_mascot_front, 90);
        }
        if(ui_Today_duration_left != NULL) {
            upanim_Animation(ui_Today_duration_left, 120);
        }
        if(ui_Today_focus_btn != NULL) {
            upanim_Animation(ui_Today_focus_btn, 160);
        }
        if(ui_Today_focus_reset != NULL) {
            upanim_Animation(ui_Today_focus_reset, 200);
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
    ui_Today = lv_obj_create(NULL);
    lv_obj_remove_flag(ui_Today, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Today, lv_color_hex(FOCUS_COLOR_READY_BG), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Today, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Today, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_Today, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    today_circle_create(ui_Today, -42, 18, 96, 0xF8E7B7, 160);
    today_circle_create(ui_Today, 214, 78, 58, 0xE5F1D6, 150);
    today_circle_create(ui_Today, 28, 22, 6, FOCUS_COLOR_DECOR_BLUE, LV_OPA_COVER);
    today_circle_create(ui_Today, 16, 194, 5, FOCUS_COLOR_DECOR_YELLOW, LV_OPA_COVER);
    today_circle_create(ui_Today, 249, 168, 5, FOCUS_COLOR_DECOR_GREEN, LV_OPA_COVER);
    today_circle_create(ui_Today, 239, 29, 4, FOCUS_COLOR_DECOR_BLUE, LV_OPA_COVER);

    ui_Today_timer_arc = lv_arc_create(ui_Today);
    lv_obj_set_pos(ui_Today_timer_arc, FOCUS_TIMER_X, FOCUS_TIMER_Y);
    lv_obj_set_size(ui_Today_timer_arc, FOCUS_TIMER_SIZE, FOCUS_TIMER_SIZE);
    lv_arc_set_rotation(ui_Today_timer_arc, 270);
    lv_arc_set_bg_angles(ui_Today_timer_arc, 0, 360);
    lv_arc_set_range(ui_Today_timer_arc, 0, 1000);
    lv_arc_set_value(ui_Today_timer_arc, 1000);
    lv_obj_remove_style(ui_Today_timer_arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(ui_Today_timer_arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(ui_Today_timer_arc, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_arc_width(ui_Today_timer_arc, FOCUS_ARC_MAIN_WIDTH, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_Today_timer_arc, FOCUS_ARC_INDICATOR_WIDTH, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui_Today_timer_arc, lv_color_hex(FOCUS_COLOR_TRACK), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui_Today_timer_arc, lv_color_hex(FOCUS_COLOR_READY_ACCENT),
                               LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_Today_timer_arc, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_Today_timer_arc, LV_OPA_COVER, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    ui_Today_focus_panel = today_circle_create(ui_Today, FOCUS_TIMER_INNER_X, FOCUS_TIMER_INNER_Y,
                                               FOCUS_TIMER_INNER_SIZE, FOCUS_COLOR_PANEL, LV_OPA_COVER);
    lv_obj_set_style_border_color(ui_Today_focus_panel, lv_color_hex(FOCUS_COLOR_PANEL_BORDER),
                                  LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Today_focus_panel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Today_focus_hint = today_label_create(ui_Today_focus_panel, "", 0, 20, FOCUS_TIMER_INNER_SIZE,
                                             FOCUS_COLOR_TEXT_GREEN, &font_puhui_16_4, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_transform_zoom(ui_Today_focus_hint, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(ui_Today_focus_hint, LV_OBJ_FLAG_HIDDEN);

    ui_Today_focus_time = today_label_create(ui_Today_focus_panel, "25:00", 0, 28, FOCUS_TIMER_INNER_SIZE,
                                             FOCUS_COLOR_TEXT_MAIN, &ui_font_Number, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_transform_zoom(ui_Today_focus_time, 176, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Today_focus_unit = today_label_create(ui_Today_focus_panel, "分钟", 0, 90, FOCUS_TIMER_INNER_SIZE,
                                             FOCUS_COLOR_TEXT_GREEN, &font_puhui_16_4, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_transform_zoom(ui_Today_focus_unit, 198, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Today_mascot_back = lv_image_create(ui_Today);
    lv_image_set_src(ui_Today_mascot_back, &ui_img_focus_mascot_ready);
    lv_obj_set_pos(ui_Today_mascot_back, FOCUS_MASCOT_RIGHT_X, FOCUS_MASCOT_RIGHT_Y);
    lv_obj_remove_flag(ui_Today_mascot_back, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(ui_Today_mascot_back, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_opa(ui_Today_mascot_back, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_transform_zoom(ui_Today_mascot_back, 268, 0);

    ui_Today_mascot_front = lv_image_create(ui_Today);
    lv_image_set_src(ui_Today_mascot_front, &ui_img_focus_mascot_ready);
    lv_obj_set_pos(ui_Today_mascot_front, FOCUS_MASCOT_RIGHT_X, FOCUS_MASCOT_RIGHT_Y);
    lv_obj_remove_flag(ui_Today_mascot_front, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_transform_zoom(ui_Today_mascot_front, 268, 0);
    lv_obj_add_flag(ui_Today_mascot_front, LV_OBJ_FLAG_GESTURE_BUBBLE);

    ui_Today_duration_left = today_button_create(ui_Today, FOCUS_LEFT_BTN_X, FOCUS_LEFT_BTN_Y,
                                                 FOCUS_LEFT_BTN_W, FOCUS_LEFT_BTN_H, 16,
                                                 FOCUS_COLOR_READY_LEFT, FOCUS_COLOR_READY_ACCENT);
    ui_Today_left_btn_icon = today_button_icon_label_create(ui_Today_duration_left, 9, FOCUS_LEFT_BTN_W);
    ui_Today_left_btn_label = today_button_text_label_create(ui_Today_duration_left, 30, FOCUS_LEFT_BTN_W);

    ui_Today_focus_btn = today_button_create(ui_Today, FOCUS_MAIN_BTN_X, FOCUS_MAIN_BTN_Y,
                                             FOCUS_MAIN_BTN_W, FOCUS_MAIN_BTN_H, 18,
                                             FOCUS_COLOR_READY_PRIMARY, FOCUS_COLOR_READY_ACCENT);
    ui_Today_focus_btn_icon = today_button_icon_label_create(ui_Today_focus_btn, 10, FOCUS_MAIN_BTN_W);
    ui_Today_focus_btn_label = today_button_text_label_create(ui_Today_focus_btn, 35, FOCUS_MAIN_BTN_W);

    ui_Today_focus_reset = today_button_create(ui_Today, FOCUS_RIGHT_BTN_X, FOCUS_RIGHT_BTN_Y,
                                               FOCUS_RIGHT_BTN_W, FOCUS_RIGHT_BTN_H, 16,
                                               FOCUS_COLOR_READY_SECONDARY, FOCUS_COLOR_PAUSED_ACCENT);
    ui_Today_focus_reset_icon = today_button_icon_label_create(ui_Today_focus_reset, 9, FOCUS_RIGHT_BTN_W);
    ui_Today_focus_reset_label = today_button_text_label_create(ui_Today_focus_reset, 30, FOCUS_RIGHT_BTN_W);

    ui_Today_scrolldots = ui_Scrolldots_create(ui_Today);
    today_highlight_page_dot(ui_Today_scrolldots, 1);

    ui_Today_header = NULL;
    ui_Today_title = NULL;
    ui_Today_date = NULL;
    ui_Today_scroll = NULL;
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
    ui_Today_duration_right = NULL;
    ui_Today_task_panel = NULL;
    ui_Today_task_label = NULL;
    ui_Today_tip_panel = NULL;
    ui_Today_tip_icon = NULL;
    ui_Today_tip_label = NULL;
    ui_Today_left_decor = NULL;
    ui_Today_corner_vine = NULL;
    ui_Today_top_back = NULL;
    ui_Today_top_gear = NULL;
    ui_Today_state_panel = NULL;
    ui_Today_state_label = NULL;
    ui_Today_primary_icon = ui_Today_focus_btn_icon;
    ui_Today_secondary_icon = ui_Today_focus_reset_icon;
    ui_Today_task_edit_icon = NULL;
    ui_Today_center_icon = NULL;

    lv_obj_add_event_cb(ui_Today, ui_event_Today, LV_EVENT_ALL, NULL);
}

void ui_Today_screen_destroy(void)
{
    if(ui_Today) {
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
}
