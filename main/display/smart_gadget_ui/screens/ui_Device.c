// This file follows the SquareLine Studio screen style used by Smart_Gadget.

#include "../ui.h"

lv_obj_t * ui_Device = NULL;
lv_obj_t * ui_Device_header = NULL;
lv_obj_t * ui_Device_title = NULL;
lv_obj_t * ui_Device_subtitle = NULL;
lv_obj_t * ui_Device_status_dot = NULL;
lv_obj_t * ui_Device_status = NULL;
lv_obj_t * ui_Device_scroll = NULL;
lv_obj_t * ui_Device_connection_panel = NULL;
lv_obj_t * ui_Device_wifi = NULL;
lv_obj_t * ui_Device_server = NULL;
lv_obj_t * ui_Device_protocol = NULL;
lv_obj_t * ui_Device_conversation_panel = NULL;
lv_obj_t * ui_Device_chat_state = NULL;
lv_obj_t * ui_Device_user = NULL;
lv_obj_t * ui_Device_ai = NULL;
lv_obj_t * ui_Device_sensor_panel = NULL;
lv_obj_t * ui_Device_sensor_state = NULL;
lv_obj_t * ui_Device_sensor_iaq = NULL;
lv_obj_t * ui_Device_sensor_env = NULL;
lv_obj_t * ui_Device_audio_panel = NULL;
lv_obj_t * ui_Device_mic = NULL;
lv_obj_t * ui_Device_speaker = NULL;
lv_obj_t * ui_Device_decoder = NULL;
lv_obj_t * ui_Device_system_panel = NULL;
lv_obj_t * ui_Device_board = NULL;
lv_obj_t * ui_Device_heap = NULL;
lv_obj_t * ui_Device_uptime = NULL;
lv_obj_t * ui_Device_page = NULL;

static lv_obj_t * device_label_create(lv_obj_t * parent, const char * text, int x, int y, int width,
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
    lv_obj_add_flag(label, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return label;
}

static lv_obj_t * device_panel_create(lv_obj_t * parent, int y, const char * title, uint32_t accent_color)
{
    lv_obj_t * panel = lv_obj_create(parent);
    lv_obj_set_width(panel, 206);
    lv_obj_set_height(panel, LV_SIZE_CONTENT);
    (void)y;
    lv_obj_remove_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(panel, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_layout(panel, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(panel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_radius(panel, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(panel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(panel, 235, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(panel, lv_color_hex(0xE4E6F8), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(panel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(panel, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(panel, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(panel, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(panel, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(panel, 7, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * accent = lv_obj_create(panel);
    lv_obj_set_width(accent, 34);
    lv_obj_set_height(accent, 3);
    lv_obj_set_x(accent, 0);
    lv_obj_set_y(accent, 0);
    lv_obj_remove_flag(accent, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(accent, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_radius(accent, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(accent, lv_color_hex(accent_color), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(accent, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(accent, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    device_label_create(panel, title, 0, 0, 164, 0x293062, &lv_font_montserrat_14);
    return panel;
}

static lv_obj_t * device_meter_create(lv_obj_t * parent, int x, int y, int width, uint32_t color)
{
    lv_obj_t * meter_bg = lv_obj_create(parent);
    lv_obj_set_width(meter_bg, width);
    lv_obj_set_height(meter_bg, 5);
    lv_obj_set_x(meter_bg, x);
    lv_obj_set_y(meter_bg, y);
    lv_obj_remove_flag(meter_bg, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(meter_bg, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_radius(meter_bg, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(meter_bg, lv_color_hex(0xE4E6F8), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(meter_bg, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(meter_bg, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * meter_fg = lv_obj_create(meter_bg);
    lv_obj_set_width(meter_fg, width * 2 / 3);
    lv_obj_set_height(meter_fg, 5);
    lv_obj_set_x(meter_fg, 0);
    lv_obj_set_y(meter_fg, 0);
    lv_obj_remove_flag(meter_fg, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(meter_fg, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_radius(meter_fg, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(meter_fg, lv_color_hex(color), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(meter_fg, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(meter_fg, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    return meter_bg;
}

// event funtions
void ui_event_Device(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_SCREEN_LOADED) {
        upanim_Animation(ui_Device_header, 100);
        upanim_Animation(ui_Device_scroll, 200);
    }
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_RIGHT) {
        lv_indev_wait_release(lv_indev_active());
        _ui_screen_change(&ui_Alarm, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Alarm_screen_init);
    }
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_LEFT) {
        lv_indev_wait_release(lv_indev_active());
        _ui_screen_change(&ui_Clock, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Clock_screen_init);
    }
}

// build funtions

void ui_Device_screen_init(void)
{
    ui_Device = lv_obj_create(NULL);
    lv_obj_remove_flag(ui_Device, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Device, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Device, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_src(ui_Device, &ui_img_pattern_png, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_tiled(ui_Device, true, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Device_header = lv_obj_create(ui_Device);
    lv_obj_set_width(ui_Device_header, lv_pct(100));
    lv_obj_set_height(ui_Device_header, 50);
    lv_obj_set_align(ui_Device_header, LV_ALIGN_TOP_MID);
    lv_obj_remove_flag(ui_Device_header, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Device_header, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Device_header, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Device_header, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui_Device_header, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Device_title = lv_label_create(ui_Device_header);
    lv_obj_set_width(ui_Device_title, 110);
    lv_obj_set_height(ui_Device_title, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_Device_title, 18);
    lv_obj_set_y(ui_Device_title, 9);
    lv_label_set_text(ui_Device_title, "Device");
    lv_obj_set_style_text_color(ui_Device_title, lv_color_hex(0x293062), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Device_title, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_Device_title, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Device_subtitle = device_label_create(ui_Device_header, "XiaoZhi Board", 18, 30, 130, 0x9C9CD9,
                                             &lv_font_montserrat_14);

    ui_Device_status_dot = lv_obj_create(ui_Device_header);
    lv_obj_set_width(ui_Device_status_dot, 8);
    lv_obj_set_height(ui_Device_status_dot, 8);
    lv_obj_set_x(ui_Device_status_dot, -72);
    lv_obj_set_y(ui_Device_status_dot, 17);
    lv_obj_set_align(ui_Device_status_dot, LV_ALIGN_TOP_RIGHT);
    lv_obj_remove_flag(ui_Device_status_dot, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_Device_status_dot, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Device_status_dot, lv_color_hex(0x10D262), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Device_status_dot, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Device_status_dot, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Device_status = device_label_create(ui_Device_header, "Ready", -18, 12, 50, 0x293062, &lv_font_montserrat_14);
    lv_obj_set_align(ui_Device_status, LV_ALIGN_TOP_RIGHT);

    ui_Device_scroll = lv_obj_create(ui_Device);
    lv_obj_set_width(ui_Device_scroll, lv_pct(92));
    lv_obj_set_height(ui_Device_scroll, lv_pct(76));
    lv_obj_set_x(ui_Device_scroll, 0);
    lv_obj_set_y(ui_Device_scroll, 48);
    lv_obj_set_align(ui_Device_scroll, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_Device_scroll, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(ui_Device_scroll, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_scroll_dir(ui_Device_scroll, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(ui_Device_scroll, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_layout(ui_Device_scroll, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(ui_Device_scroll, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_Device_scroll, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_style_bg_color(ui_Device_scroll, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Device_scroll, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Device_scroll, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_Device_scroll, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_Device_scroll, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_Device_scroll, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_Device_scroll, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_Device_scroll, 12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Device_connection_panel = device_panel_create(ui_Device_scroll, 0, "Connection", 0x10D262);
    ui_Device_wifi = device_label_create(ui_Device_connection_panel, "Wi-Fi linked", 0, 0, 164, 0x293062,
                                         &lv_font_montserrat_14);
    ui_Device_server = device_label_create(ui_Device_connection_panel, "Server ready", 0, 0, 164, 0x293062,
                                           &lv_font_montserrat_14);
    ui_Device_protocol = device_label_create(ui_Device_connection_panel, "Protocol realtime", 0, 0, 164, 0x9C9CD9,
                                             &lv_font_montserrat_14);

    ui_Device_conversation_panel = device_panel_create(ui_Device_scroll, 122, "Conversation", 0x9C9CD9);
    ui_Device_chat_state = device_label_create(ui_Device_conversation_panel, "State waiting", 0, 0, 164, 0x293062,
                                               &lv_font_montserrat_14);
    ui_Device_user = device_label_create(ui_Device_conversation_panel, "You no message", 0, 0, 164, 0x293062,
                                         &lv_font_montserrat_14);
    ui_Device_ai = device_label_create(ui_Device_conversation_panel, "AI no reply", 0, 0, 164, 0x9C9CD9,
                                       &lv_font_montserrat_14);

    ui_Device_sensor_panel = device_panel_create(ui_Device_scroll, 244, "Sensors", 0x10D262);
    ui_Device_sensor_state = device_label_create(ui_Device_sensor_panel, "BME690 waiting", 0, 0, 164, 0x293062,
                                                 &lv_font_montserrat_14);
    ui_Device_sensor_iaq = device_label_create(ui_Device_sensor_panel, "IAQ --", 0, 0, 164, 0x293062,
                                               &lv_font_montserrat_14);
    ui_Device_sensor_env = device_label_create(ui_Device_sensor_panel, "T --C H --% P --hPa", 0, 0, 164, 0x9C9CD9,
                                               &lv_font_montserrat_14);

    ui_Device_audio_panel = device_panel_create(ui_Device_scroll, 244, "Audio", 0x293062);
    ui_Device_mic = device_label_create(ui_Device_audio_panel, "Mic realtime input", 0, 0, 164, 0x293062,
                                        &lv_font_montserrat_14);
    ui_Device_speaker = device_label_create(ui_Device_audio_panel, "Speaker output", 0, 0, 164, 0x293062,
                                            &lv_font_montserrat_14);
    ui_Device_decoder = device_label_create(ui_Device_audio_panel, "Opus decoder ready", 0, 0, 164, 0x9C9CD9,
                                            &lv_font_montserrat_14);

    ui_Device_system_panel = device_panel_create(ui_Device_scroll, 366, "System", 0xF6A44D);
    ui_Device_board = device_label_create(ui_Device_system_panel, "Board SensAir Shuttle", 0, 0, 164, 0x293062,
                                          &lv_font_montserrat_14);
    ui_Device_heap = device_label_create(ui_Device_system_panel, "Heap -- KB", 0, 0, 164, 0x293062,
                                         &lv_font_montserrat_14);
    ui_Device_uptime = device_label_create(ui_Device_system_panel, "Build smart gadget UI", 0, 0, 164, 0x9C9CD9,
                                           &lv_font_montserrat_14);
    device_meter_create(ui_Device_system_panel, 0, 0, 160, 0xF6A44D);

    ui_Device_page = device_label_create(ui_Device, "7 / 7", -18, -11, 46, 0x9C9CD9, &lv_font_montserrat_14);
    lv_obj_set_align(ui_Device_page, LV_ALIGN_BOTTOM_RIGHT);

    lv_obj_add_event_cb(ui_Device, ui_event_Device, LV_EVENT_ALL, NULL);
}

void ui_Device_screen_destroy(void)
{
    if(ui_Device) lv_obj_del(ui_Device);

    // NULL screen variables
    ui_Device = NULL;
    ui_Device_header = NULL;
    ui_Device_title = NULL;
    ui_Device_subtitle = NULL;
    ui_Device_status_dot = NULL;
    ui_Device_status = NULL;
    ui_Device_scroll = NULL;
    ui_Device_connection_panel = NULL;
    ui_Device_wifi = NULL;
    ui_Device_server = NULL;
    ui_Device_protocol = NULL;
    ui_Device_conversation_panel = NULL;
    ui_Device_chat_state = NULL;
    ui_Device_user = NULL;
    ui_Device_ai = NULL;
    ui_Device_sensor_panel = NULL;
    ui_Device_sensor_state = NULL;
    ui_Device_sensor_iaq = NULL;
    ui_Device_sensor_env = NULL;
    ui_Device_audio_panel = NULL;
    ui_Device_mic = NULL;
    ui_Device_speaker = NULL;
    ui_Device_decoder = NULL;
    ui_Device_system_panel = NULL;
    ui_Device_board = NULL;
    ui_Device_heap = NULL;
    ui_Device_uptime = NULL;
    ui_Device_page = NULL;

}
