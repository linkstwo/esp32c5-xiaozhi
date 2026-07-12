// This file follows the SquareLine Studio screen style used by Smart_Gadget.

#include "../ui.h"

LV_FONT_DECLARE(BUILTIN_TEXT_FONT);
LV_FONT_DECLARE(font_puhui_16_4);

lv_obj_t * ui_Today = NULL;
lv_obj_t * ui_Today_header = NULL;
lv_obj_t * ui_Today_title = NULL;
lv_obj_t * ui_Today_date = NULL;
lv_obj_t * ui_Today_scroll = NULL;
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
lv_obj_t * ui_Today_focus_hint = NULL;
lv_obj_t * ui_Today_focus_btn = NULL;
lv_obj_t * ui_Today_focus_btn_label = NULL;
lv_obj_t * ui_Today_focus_reset = NULL;
lv_obj_t * ui_Today_focus_reset_label = NULL;

static lv_obj_t * today_label_create(lv_obj_t * parent, const char * text, int x, int y, int width,
                                     uint32_t color, const lv_font_t * font)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_obj_set_width(label, width);
    lv_obj_set_height(label, LV_SIZE_CONTENT);
    lv_obj_set_x(label, x);
    lv_obj_set_y(label, y);
    lv_label_set_text(label, text);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_color(label, lv_color_hex(color), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label, font, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(label, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(label, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return label;
}

static lv_obj_t * today_block_create(lv_obj_t * parent, int width, uint32_t bg_color, lv_opa_t bg_opa)
{
    lv_obj_t * block = lv_obj_create(parent);
    lv_obj_set_width(block, width);
    lv_obj_set_height(block, LV_SIZE_CONTENT);
    lv_obj_remove_flag(block, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(block, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_layout(block, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(block, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(block, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_radius(block, 18, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(block, lv_color_hex(bg_color), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(block, bg_opa, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(block, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(block, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(block, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(block, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(block, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(block, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    return block;
}

static lv_obj_t * today_panel_create(lv_obj_t * parent, const char * title, uint32_t accent_color)
{
    lv_obj_t * panel = today_block_create(parent, 216, 0xFFFFFF, 242);
    lv_obj_set_style_radius(panel, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(panel, lv_color_hex(0xDCE8D7), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(panel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * accent = lv_obj_create(panel);
    lv_obj_set_width(accent, 34);
    lv_obj_set_height(accent, 3);
    lv_obj_remove_flag(accent, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(accent, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_radius(accent, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(accent, lv_color_hex(accent_color), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(accent, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(accent, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    today_label_create(panel, title, 0, 0, 192, 0x253530, &font_puhui_16_4);
    return panel;
}

static lv_obj_t * today_value_create(lv_obj_t * parent, const char * text, uint32_t bg_color, uint32_t text_color)
{
    lv_obj_t * row = lv_obj_create(parent);
    lv_obj_set_width(row, 192);
    lv_obj_set_height(row, LV_SIZE_CONTENT);
    lv_obj_remove_flag(row, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(row, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_radius(row, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(row, lv_color_hex(bg_color), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(row, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(row, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(row, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(row, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(row, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(row, 6, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * label = lv_label_create(row);
    lv_obj_set_width(label, 172);
    lv_obj_set_height(label, LV_SIZE_CONTENT);
    lv_label_set_text(label, text);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_color(label, lv_color_hex(text_color), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label, &font_puhui_16_4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(label, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(label, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return label;
}

void ui_event_Today(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_SCREEN_LOADED) {
        upanim_Animation(ui_Today_scroll, 100);
    }
    if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_RIGHT) {
        lv_indev_wait_release(lv_indev_active());
        _ui_screen_change(&ui_Call, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Call_screen_init);
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
    lv_obj_set_style_bg_color(ui_Today, lv_color_hex(0xEDF5EE), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Today, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Today_scroll = lv_obj_create(ui_Today);
    lv_obj_set_width(ui_Today_scroll, 232);
    lv_obj_set_height(ui_Today_scroll, 228);
    lv_obj_set_x(ui_Today_scroll, 0);
    lv_obj_set_y(ui_Today_scroll, 2);
    lv_obj_set_align(ui_Today_scroll, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_Today_scroll, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(ui_Today_scroll, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_scroll_dir(ui_Today_scroll, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(ui_Today_scroll, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_layout(ui_Today_scroll, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(ui_Today_scroll, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_Today_scroll, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_style_bg_color(ui_Today_scroll, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Today_scroll, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Today_scroll, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_Today_scroll, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_Today_scroll, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_Today_scroll, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_Today_scroll, 28, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_Today_scroll, 9, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Today_focus_panel = today_block_create(ui_Today_scroll, 216, 0x143B33, 255);
    ui_Today_header = ui_Today_focus_panel;
    lv_obj_set_style_pad_left(ui_Today_focus_panel, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_Today_focus_panel, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_Today_focus_panel, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_Today_focus_panel, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_Today_focus_panel, 7, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Today_title = today_label_create(ui_Today_focus_panel, "今日专注", 0, 0, 188, 0xFFFFFF,
                                        &font_puhui_16_4);
    ui_Today_date = today_label_create(ui_Today_focus_panel, "06月17日 周三", 0, 0, 188, 0xA9D6C6,
                                       &font_puhui_16_4);

    ui_Today_focus_time = lv_label_create(ui_Today_focus_panel);
    lv_obj_set_width(ui_Today_focus_time, 188);
    lv_obj_set_height(ui_Today_focus_time, LV_SIZE_CONTENT);
    lv_label_set_text(ui_Today_focus_time, "25:00");
    lv_label_set_long_mode(ui_Today_focus_time, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_align(ui_Today_focus_time, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_Today_focus_time, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Today_focus_time, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_Today_focus_time, &ui_font_Number, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(ui_Today_focus_time, LV_OBJ_FLAG_GESTURE_BUBBLE);

    ui_Today_focus_hint = today_label_create(ui_Today_focus_panel, "25 分钟，只做当前这一件事", 0, 0, 188,
                                             0xD8F2E9, &font_puhui_16_4);
    lv_obj_set_style_text_align(ui_Today_focus_hint, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * ui_Today_focus_actions = lv_obj_create(ui_Today_focus_panel);
    lv_obj_set_width(ui_Today_focus_actions, 188);
    lv_obj_set_height(ui_Today_focus_actions, LV_SIZE_CONTENT);
    lv_obj_remove_flag(ui_Today_focus_actions, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(ui_Today_focus_actions, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_bg_opa(ui_Today_focus_actions, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Today_focus_actions, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui_Today_focus_actions, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_Today_focus_actions, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_layout(ui_Today_focus_actions, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(ui_Today_focus_actions, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_Today_focus_actions, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    ui_Today_focus_btn = lv_obj_create(ui_Today_focus_actions);
    lv_obj_set_width(ui_Today_focus_btn, 112);
    lv_obj_set_height(ui_Today_focus_btn, 36);
    lv_obj_remove_flag(ui_Today_focus_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(ui_Today_focus_btn, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_radius(ui_Today_focus_btn, 24, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Today_focus_btn, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Today_focus_btn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Today_focus_btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui_Today_focus_btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Today_focus_btn, lv_color_hex(0xD8F2E9), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_Today_focus_btn, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_Today_focus_btn_label = lv_label_create(ui_Today_focus_btn);
    lv_obj_center(ui_Today_focus_btn_label);
    lv_label_set_text(ui_Today_focus_btn_label, "开始");
    lv_obj_set_style_text_color(ui_Today_focus_btn_label, lv_color_hex(0x173B32), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Today_focus_btn_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_Today_focus_btn_label, &font_puhui_16_4, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Today_focus_reset = lv_obj_create(ui_Today_focus_actions);
    lv_obj_set_width(ui_Today_focus_reset, 62);
    lv_obj_set_height(ui_Today_focus_reset, 36);
    lv_obj_remove_flag(ui_Today_focus_reset, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(ui_Today_focus_reset, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_radius(ui_Today_focus_reset, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Today_focus_reset, lv_color_hex(0x173B32), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Today_focus_reset, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_Today_focus_reset, lv_color_hex(0xD8F2E9), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_Today_focus_reset, 220, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Today_focus_reset, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui_Today_focus_reset, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Today_focus_reset_label = lv_label_create(ui_Today_focus_reset);
    lv_obj_center(ui_Today_focus_reset_label);
    lv_label_set_text(ui_Today_focus_reset_label, "重置");
    lv_obj_set_style_text_color(ui_Today_focus_reset_label, lv_color_hex(0xD8F2E9), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Today_focus_reset_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_Today_focus_reset_label, &font_puhui_16_4, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Today_status_panel = today_panel_create(ui_Today_scroll, "环境建议", 0x28B86B);
    ui_Today_status = today_label_create(ui_Today_status_panel, "等待传感器更新", 0, 0, 192, 0x173B32,
                                         &font_puhui_16_4);
    ui_Today_status_detail = today_label_create(ui_Today_status_panel, "先确定一件最重要的事", 0, 0, 192,
                                                0x66716A, &font_puhui_16_4);

    ui_Today_env_panel = ui_Today_status_panel;
    ui_Today_env_temp = today_value_create(ui_Today_status_panel, "温度：-- C", 0xE7FFF2, 0x173B32);
    ui_Today_env_humidity = today_value_create(ui_Today_status_panel, "湿度：-- %", 0xFFF0C8, 0x5A4310);
    ui_Today_env_air = today_value_create(ui_Today_status_panel, "空气：等待更新", 0xE9EEFF, 0x2F3B7A);

    ui_Today_countdown_panel = today_panel_create(ui_Today_scroll, "今日目标", 0xF2A922);
    ui_Today_exam = today_label_create(ui_Today_countdown_panel, "最近目标：等待时间同步", 0, 0, 192, 0x253530,
                                       &font_puhui_16_4);
    ui_Today_project = today_label_create(ui_Today_countdown_panel, "今天建议：只推进一步", 0, 0, 192, 0x66716A,
                                          &font_puhui_16_4);

    lv_obj_add_event_cb(ui_Today, ui_event_Today, LV_EVENT_ALL, NULL);
}

void ui_Today_screen_destroy(void)
{
    if(ui_Today) lv_obj_del(ui_Today);

    ui_Today = NULL;
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
    ui_Today_focus_panel = NULL;
    ui_Today_focus_time = NULL;
    ui_Today_focus_hint = NULL;
    ui_Today_focus_btn = NULL;
    ui_Today_focus_btn_label = NULL;
    ui_Today_focus_reset = NULL;
    ui_Today_focus_reset_label = NULL;
}
