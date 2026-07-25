// Device page assembled from the device_v3_284x240 UI resource pack.

#include "../ui.h"

#include <stdint.h>

LV_FONT_DECLARE(font_puhui_16_4);

#define DEVICE_BG              0xFFF9EC
#define DEVICE_CARD            0xFFFDF7
#define DEVICE_CARD_ALT        0xFFFBEE
#define DEVICE_BORDER          0xC7D889
#define DEVICE_DIVIDER         0xE8E0C7
#define DEVICE_TEXT            0x24311F
#define DEVICE_SUBTEXT         0x6C695C
#define DEVICE_GREEN_DARK      0x315B24
#define DEVICE_GREEN           0x6E9A3C
#define DEVICE_OK              0x5F9135

enum {
    DEVICE_VIEW_OVERVIEW = 0,
    DEVICE_VIEW_SYSTEM,
    DEVICE_VIEW_CONNECTION,
    DEVICE_VIEW_SENSING,
};

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
lv_obj_t * ui_Device_health_title = NULL;
lv_obj_t * ui_Device_health_subtitle = NULL;
lv_obj_t * ui_Device_temperature = NULL;
lv_obj_t * ui_Device_humidity = NULL;
lv_obj_t * ui_Device_pressure = NULL;
lv_obj_t * ui_Device_co2 = NULL;
lv_obj_t * ui_Device_sensor_accuracy = NULL;
lv_obj_t * ui_Device_overview_wifi = NULL;
lv_obj_t * ui_Device_overview_sensor = NULL;
lv_obj_t * ui_Device_overview_audio = NULL;
lv_obj_t * ui_Device_overview_heap = NULL;
lv_obj_t * ui_Device_overview_uptime = NULL;

static lv_obj_t * s_overview = NULL;
static lv_obj_t * s_detail = NULL;
static lv_obj_t * s_detail_system = NULL;
static lv_obj_t * s_detail_connection = NULL;
static lv_obj_t * s_detail_sensing = NULL;
static lv_obj_t * s_tab_highlight = NULL;
static lv_obj_t * s_active_detail_scroll = NULL;

static void device_pass_gestures(lv_obj_t * object)
{
    lv_obj_remove_flag(object, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(object, LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_EVENT_BUBBLE);
}

static lv_obj_t * device_image(lv_obj_t * parent, const lv_image_dsc_t * source, int x, int y)
{
    lv_obj_t * image = lv_image_create(parent);
    lv_image_set_src(image, source);
    lv_obj_set_pos(image, x, y);
    device_pass_gestures(image);
    return image;
}

static lv_obj_t * device_label(lv_obj_t * parent, const char * text,
                               int x, int y, int width, int height,
                               uint32_t color, uint16_t zoom, lv_text_align_t align)
{
    (void)zoom;
    lv_obj_t * label = lv_label_create(parent);
    lv_obj_set_pos(label, x, y);
    lv_obj_set_size(label, width, height);
    lv_label_set_text(label, text);
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_bg_opa(label, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(label, 0, 0);
    lv_obj_set_style_pad_all(label, 0, 0);
    lv_obj_set_style_text_font(label, &font_puhui_16_4, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    lv_obj_set_style_text_align(label, align, 0);
    device_pass_gestures(label);
    return label;
}

static lv_obj_t * device_panel(lv_obj_t * parent, int x, int y, int width, int height,
                               uint32_t background, int radius)
{
    lv_obj_t * panel = lv_obj_create(parent);
    lv_obj_set_pos(panel, x, y);
    lv_obj_set_size(panel, width, height);
    device_pass_gestures(panel);
    lv_obj_set_style_bg_color(panel, lv_color_hex(background), 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(panel, lv_color_hex(DEVICE_BORDER), 0);
    lv_obj_set_style_border_width(panel, 1, 0);
    lv_obj_set_style_radius(panel, radius, 0);
    lv_obj_set_style_pad_all(panel, 0, 0);
    lv_obj_set_style_shadow_width(panel, 0, 0);
    return panel;
}

static lv_obj_t * device_detail_scroll(lv_obj_t * parent)
{
    lv_obj_t * scroll = lv_obj_create(parent);
    lv_obj_set_pos(scroll, 0, 60);
    lv_obj_set_size(scroll, 284, 180);
    /* The page-level handler below owns scrolling so every child drags consistently. */
    lv_obj_remove_flag(scroll, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(scroll, LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_scroll_dir(scroll, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(scroll, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(scroll, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(scroll, 0, 0);
    lv_obj_set_style_pad_all(scroll, 0, 0);
    return scroll;
}

static lv_obj_t * device_button(lv_obj_t * parent, int x, int y, int width, int height,
                                uint8_t action)
{
    lv_obj_t * button = lv_obj_create(parent);
    lv_obj_set_pos(button, x, y);
    lv_obj_set_size(button, width, height);
    lv_obj_remove_flag(button, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_style_bg_opa(button, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_set_style_radius(button, 0, 0);
    lv_obj_set_style_opa(button, LV_OPA_70, LV_STATE_PRESSED);
    lv_obj_add_event_cb(button, ui_event_Device, LV_EVENT_CLICKED, (void *)(uintptr_t)action);
    return button;
}

static void device_show_view(uint8_t view)
{
    if(view == DEVICE_VIEW_OVERVIEW) {
        s_active_detail_scroll = NULL;
        ui_Device_scroll = s_overview;
        lv_obj_clear_flag(s_overview, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(s_detail, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    lv_obj_add_flag(s_overview, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(s_detail, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_x(s_tab_highlight, (int)(view - DEVICE_VIEW_SYSTEM) * 89);
    lv_obj_add_flag(s_detail_system, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s_detail_connection, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s_detail_sensing, LV_OBJ_FLAG_HIDDEN);
    if(view == DEVICE_VIEW_SYSTEM) {
        s_active_detail_scroll = s_detail_system;
        ui_Device_scroll = s_active_detail_scroll;
        lv_obj_clear_flag(s_detail_system, LV_OBJ_FLAG_HIDDEN);
    }
    else if(view == DEVICE_VIEW_CONNECTION) {
        s_active_detail_scroll = s_detail_connection;
        ui_Device_scroll = s_active_detail_scroll;
        lv_obj_clear_flag(s_detail_connection, LV_OBJ_FLAG_HIDDEN);
    }
    else {
        s_active_detail_scroll = s_detail_sensing;
        ui_Device_scroll = s_active_detail_scroll;
        lv_obj_clear_flag(s_detail_sensing, LV_OBJ_FLAG_HIDDEN);
    }
}

static void device_detail_drag(lv_event_t * event)
{
    (void)event;
    lv_indev_t * indev = lv_indev_get_act();
    if(s_active_detail_scroll == NULL || indev == NULL ||
       lv_indev_get_type(indev) != LV_INDEV_TYPE_POINTER) return;

    lv_point_t vector;
    lv_indev_get_vect(indev, &vector);
    if(vector.y != 0) lv_obj_scroll_by_bounded(s_active_detail_scroll, 0, vector.y, LV_ANIM_OFF);
}

static void device_consume_detail_gesture(lv_event_t * event)
{
    lv_event_stop_bubbling(event);
}

static void device_section_title(lv_obj_t * parent, const lv_image_dsc_t * icon,
                                 const char * title, int width)
{
    device_image(parent, icon, 7, 5);
    device_label(parent, title, 34, 7, width - 42, 14, DEVICE_GREEN_DARK, 178, LV_TEXT_ALIGN_LEFT);
}

static lv_obj_t * device_detail_row(lv_obj_t * parent, int y,
                                    const char * label, const char * value)
{
    device_label(parent, label, 14, y, 100, 20, DEVICE_SUBTEXT, 256, LV_TEXT_ALIGN_LEFT);
    return device_label(parent, value, 120, y, 134, 20, DEVICE_GREEN_DARK, 256, LV_TEXT_ALIGN_LEFT);
}

static void device_overview_card(lv_obj_t * parent, int x, const lv_image_dsc_t * icon,
                                 const char * title, const char * value, const char * summary,
                                 uint8_t action, lv_obj_t ** value_label)
{
    (void)summary;
    lv_obj_t * card = device_panel(parent, x, 100, 82, 80, DEVICE_CARD, 10);
    device_image(card, icon, 7, 7);
    device_label(card, title, 5, 35, 72, 17, DEVICE_TEXT, 0, LV_TEXT_ALIGN_CENTER);
    *value_label = device_label(card, value, 5, 55, 72, 17, DEVICE_OK, 0, LV_TEXT_ALIGN_CENTER);
    device_button(card, 0, 0, 82, 80, action);
}

static void device_build_overview(void)
{
    s_overview = lv_obj_create(ui_Device);
    lv_obj_set_pos(s_overview, 0, 0);
    lv_obj_set_size(s_overview, 284, 240);
    lv_obj_add_flag(s_overview, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_dir(s_overview, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(s_overview, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(s_overview, LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_style_bg_opa(s_overview, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(s_overview, 0, 0);
    lv_obj_set_style_pad_all(s_overview, 0, 0);

    device_image(s_overview, &ui_img_device_v3_header_left, 7, 2);
    ui_Device_header = s_overview;
    ui_Device_title = device_label(s_overview, "设备状态", 8, 5, 100, 23,
                                   DEVICE_TEXT, 0, LV_TEXT_ALIGN_LEFT);
    ui_Device_subtitle = device_label(s_overview, "详情 ›", 226, 8, 48, 17,
                                      DEVICE_GREEN_DARK, 0, LV_TEXT_ALIGN_RIGHT);
    device_button(s_overview, 220, 0, 60, 28, DEVICE_VIEW_SYSTEM);

    lv_obj_t * health = device_panel(s_overview, 8, 30, 268, 64, DEVICE_CARD, 12);
    device_image(health, &ui_img_device_v3_header_right, 170, 2);
    device_image(health, &ui_img_device_v3_icon_shield, 13, 15);
    ui_Device_health_title = device_label(health, "运行良好", 55, 13, 104, 23,
                                          DEVICE_GREEN_DARK, 0, LV_TEXT_ALIGN_LEFT);
    ui_Device_health_subtitle = device_label(health, "小智在线，核心功能正常", 55, 39, 174, 18,
                                             DEVICE_SUBTEXT, 0, LV_TEXT_ALIGN_LEFT);
    ui_Device_status = NULL;
    ui_Device_status_dot = NULL;

    ui_Device_connection_panel = s_overview;
    device_overview_card(s_overview, 8, &ui_img_device_v3_icon_cloud,
                         "小智连接", "在线", "协议服务正常", DEVICE_VIEW_CONNECTION, &ui_Device_overview_wifi);
    ui_Device_sensor_panel = s_overview;
    device_overview_card(s_overview, 97, &ui_img_device_v3_icon_environment,
                         "环境传感", "正常", "传感器预热中", DEVICE_VIEW_SENSING, &ui_Device_overview_sensor);
    ui_Device_audio_panel = s_overview;
    device_overview_card(s_overview, 186, &ui_img_device_v3_icon_speaker,
                         "语音系统", "正常", "麦克风与扬声器可用", DEVICE_VIEW_SENSING, &ui_Device_overview_audio);

    lv_obj_t * footer = device_panel(s_overview, 8, 186, 268, 50, DEVICE_CARD, 10);
    device_image(footer, &ui_img_device_v3_corner_flower, 4, 2);
    device_label(footer, "已运行", 34, 8, 50, 17, DEVICE_SUBTEXT, 0, LV_TEXT_ALIGN_LEFT);
    ui_Device_overview_uptime = device_label(footer, "--", 34, 27, 58, 17, DEVICE_TEXT, 0, LV_TEXT_ALIGN_LEFT);
    lv_obj_t * divider_one = lv_obj_create(footer);
    lv_obj_set_pos(divider_one, 95, 8);
    lv_obj_set_size(divider_one, 1, 34);
    lv_obj_set_style_bg_color(divider_one, lv_color_hex(DEVICE_DIVIDER), 0);
    lv_obj_set_style_bg_opa(divider_one, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(divider_one, 0, 0);
    device_label(footer, "可用内存", 108, 8, 62, 17, DEVICE_SUBTEXT, 0, LV_TEXT_ALIGN_LEFT);
    ui_Device_overview_heap = device_label(footer, "--", 108, 27, 62, 17, DEVICE_TEXT, 0, LV_TEXT_ALIGN_LEFT);
    device_label(footer, "设备详情 ›", 182, 17, 78, 18, DEVICE_GREEN_DARK, 0, LV_TEXT_ALIGN_CENTER);
    device_button(footer, 172, 0, 96, 50, DEVICE_VIEW_SYSTEM);

    lv_obj_t * details = device_panel(s_overview, 8, 246, 268, 58, DEVICE_CARD_ALT, 10);
    device_label(details, "更多设备信息", 16, 10, 126, 18, DEVICE_TEXT, 0, LV_TEXT_ALIGN_LEFT);
    device_label(details, "系统、连接与感知", 16, 31, 150, 17, DEVICE_SUBTEXT, 0, LV_TEXT_ALIGN_LEFT);
    device_label(details, "查看 ›", 204, 20, 48, 18, DEVICE_GREEN_DARK, 0, LV_TEXT_ALIGN_RIGHT);
    device_button(details, 0, 0, 268, 58, DEVICE_VIEW_SYSTEM);

    ui_Device_server = NULL;
    ui_Device_protocol = NULL;
    ui_Device_conversation_panel = NULL;
    ui_Device_chat_state = NULL;
    ui_Device_user = NULL;
    ui_Device_ai = NULL;
    ui_Device_speaker = NULL;
    ui_Device_decoder = NULL;
    ui_Device_system_panel = NULL;
    ui_Device_board = NULL;
    ui_Device_page = NULL;
}

static void device_build_system_page(void)
{
    s_detail_system = device_detail_scroll(s_detail);

    lv_obj_t * info = device_panel(s_detail_system, 8, 0, 268, 92, DEVICE_CARD, 10);
    device_image(info, &ui_img_device_v3_card_wave, 150, 1);
    device_section_title(info, &ui_img_device_v3_icon_chip, "设备信息", 268);
    device_detail_row(info, 31, "主控", "ESP32-C5");
    ui_Device_board = device_detail_row(info, 58, "板型", "ESP-SensAirShuttle");

    lv_obj_t * runtime = device_panel(s_detail_system, 8, 98, 268, 92, DEVICE_CARD, 10);
    device_section_title(runtime, &ui_img_device_v3_icon_memory, "运行资源", 268);
    ui_Device_heap = device_detail_row(runtime, 31, "可用内存", "--");
    ui_Device_uptime = device_detail_row(runtime, 58, "运行时间", "--");

    lv_obj_t * software = device_panel(s_detail_system, 8, 196, 268, 92, DEVICE_CARD, 10);
    device_section_title(software, &ui_img_device_v3_icon_firmware, "软件状态", 268);
    device_detail_row(software, 31, "OTA状态", "已是最新版本");
    device_detail_row(software, 58, "UI状态", "正常");
    return;

#if 0
    s_detail_system = device_detail_scroll(s_detail);

    lv_obj_t * info = device_panel(s_detail_system, 8, 0, 268, 84, DEVICE_CARD, 10);
    device_image(info, &ui_img_device_v3_card_wave, 150, 1);
    device_section_title(info, &ui_img_device_v3_icon_chip, "设备信息", 268);
    device_pair(info, 8, 26, 120, "主控", "ESP32-C5", &ui_img_device_v3_icon_chip);
    ui_Device_board = device_label(info, "ESP-SensAirShuttle", 157, 40, 100, 14,
                                  DEVICE_GREEN_DARK, 135, LV_TEXT_ALIGN_LEFT);
    device_label(info, "板型", 157, 28, 70, 10, DEVICE_SUBTEXT, 120, LV_TEXT_ALIGN_LEFT);

    lv_obj_t * runtime = device_panel(s_detail_system, 8, 90, 268, 76, DEVICE_CARD, 10);
    device_section_title(runtime, &ui_img_device_v3_icon_memory, "运行资源", 268);
    ui_Device_heap = device_label(runtime, "--", 33, 39, 70, 13, DEVICE_GREEN_DARK, 155, LV_TEXT_ALIGN_LEFT);
    device_label(runtime, "可用内存", 33, 28, 70, 10, DEVICE_SUBTEXT, 120, LV_TEXT_ALIGN_LEFT);
    ui_Device_uptime = device_label(runtime, "--", 156, 39, 76, 13, DEVICE_GREEN_DARK, 155, LV_TEXT_ALIGN_LEFT);
    device_label(runtime, "运行时间", 156, 28, 70, 10, DEVICE_SUBTEXT, 120, LV_TEXT_ALIGN_LEFT);
    device_image(runtime, &ui_img_device_v3_icon_memory, 8, 29);
    device_image(runtime, &ui_img_device_v3_icon_clock, 132, 29);

    lv_obj_t * software = device_panel(s_detail_system, 8, 172, 268, 62, DEVICE_CARD, 10);
    device_section_title(software, &ui_img_device_v3_icon_firmware, "软件状态", 268);
    device_pair(software, 8, 17, 120, "OTA状态", "已是最新版", &ui_img_device_v3_icon_ota);
    device_pair(software, 150, 17, 100, "UI状态", "正常", &ui_img_device_v3_icon_firmware);
#endif
}

static void device_build_connection_page(void)
{
    s_detail_connection = device_detail_scroll(s_detail);

    lv_obj_t * wifi = device_panel(s_detail_connection, 8, 0, 268, 142, DEVICE_CARD, 10);
    device_section_title(wifi, &ui_img_device_v3_icon_wifi, "Wi-Fi", 268);
    ui_Device_wifi = device_detail_row(wifi, 31, "状态", "已连接");
    device_detail_row(wifi, 57, "网络名称", "--");
    device_detail_row(wifi, 83, "信号强度", "-- dBm");
    device_detail_row(wifi, 109, "IP地址", "--");

    lv_obj_t * cloud = device_panel(s_detail_connection, 8, 148, 268, 142, DEVICE_CARD, 10);
    device_image(cloud, &ui_img_device_v3_card_wave, 150, 1);
    device_section_title(cloud, &ui_img_device_v3_icon_cloud, "云端服务", 268);
    ui_Device_server = device_detail_row(cloud, 31, "会话服务", "在线");
    ui_Device_protocol = device_detail_row(cloud, 57, "MQTT", "已连接");
    device_detail_row(cloud, 83, "UDP音频", "已建立");
    device_detail_row(cloud, 109, "最近响应", "-- ms");
    device_image(s_detail_connection, &ui_img_device_v3_bottom_banner, 8, 296);
    device_label(s_detail_connection, "链路稳定，可正常进行语音对话与工具调用", 30, 304, 228, 20,
                 DEVICE_GREEN_DARK, 256, LV_TEXT_ALIGN_CENTER);
    return;

#if 0
    s_detail_connection = device_detail_scroll(s_detail);

    lv_obj_t * wifi = device_panel(s_detail_connection, 8, 0, 268, 116, DEVICE_CARD, 10);
    device_section_title(wifi, &ui_img_device_v3_icon_wifi, "Wi-Fi", 268);
    ui_Device_wifi = device_label(wifi, "已连接", 12, 43, 48, 14, DEVICE_OK, 155, LV_TEXT_ALIGN_LEFT);
    device_label(wifi, "状态", 12, 31, 42, 10, DEVICE_SUBTEXT, 120, LV_TEXT_ALIGN_LEFT);
    device_pair(wifi, 76, 28, 82, "网络名称", "--", &ui_img_device_v3_icon_router);
    device_pair(wifi, 166, 28, 88, "信号强度", "-- dBm", &ui_img_device_v3_icon_signal);
    device_pair(wifi, 76, 70, 120, "IP地址", "--", &ui_img_device_v3_icon_ip);

    lv_obj_t * cloud = device_panel(s_detail_connection, 8, 122, 268, 116, DEVICE_CARD, 10);
    device_image(cloud, &ui_img_device_v3_card_wave, 150, 1);
    device_section_title(cloud, &ui_img_device_v3_icon_cloud, "云端服务", 268);
    ui_Device_server = device_label(cloud, "在线", 12, 43, 48, 14, DEVICE_OK, 155, LV_TEXT_ALIGN_LEFT);
    device_label(cloud, "会话服务", 12, 31, 48, 10, DEVICE_SUBTEXT, 120, LV_TEXT_ALIGN_LEFT);
    ui_Device_protocol = device_label(cloud, "已连接", 76, 43, 55, 14, DEVICE_OK, 155, LV_TEXT_ALIGN_LEFT);
    device_label(cloud, "MQTT", 76, 31, 48, 10, DEVICE_SUBTEXT, 120, LV_TEXT_ALIGN_LEFT);
    device_pair(cloud, 144, 28, 110, "UDP音频", "已建立", &ui_img_device_v3_icon_udp);
    device_pair(cloud, 76, 70, 120, "最近响应", "-- ms", &ui_img_device_v3_icon_mqtt);
    device_image(s_detail_connection, &ui_img_device_v3_bottom_banner, 8, 244);
    device_label(s_detail_connection, "链路稳定，可正常进行语音对话与工具调用", 30, 252, 228, 13,
                 DEVICE_GREEN_DARK, 123, LV_TEXT_ALIGN_CENTER);
#endif
}

static void device_build_sensing_page(void)
{
    s_detail_sensing = device_detail_scroll(s_detail);

    lv_obj_t * sensor = device_panel(s_detail_sensing, 8, 0, 268, 240, DEVICE_CARD, 10);
    device_section_title(sensor, &ui_img_device_v3_icon_bme, "BME690环境传感器", 268);
    ui_Device_sensor_state = device_detail_row(sensor, 31, "状态", "预热中");
    ui_Device_temperature = device_detail_row(sensor, 57, "温度", "--");
    ui_Device_humidity = device_detail_row(sensor, 83, "湿度", "--");
    ui_Device_pressure = device_detail_row(sensor, 109, "气压", "--");
    ui_Device_sensor_iaq = device_detail_row(sensor, 135, "IAQ", "--");
    ui_Device_co2 = device_detail_row(sensor, 161, "等效CO2", "--");
    ui_Device_sensor_accuracy = device_detail_row(sensor, 187, "IAQ精度", "--");
    ui_Device_sensor_env = device_detail_row(sensor, 213, "最近更新", "--");

    lv_obj_t * audio = device_panel(s_detail_sensing, 8, 246, 268, 124, DEVICE_CARD, 10);
    device_section_title(audio, &ui_img_device_v3_icon_speaker, "语音系统", 268);
    ui_Device_mic = device_detail_row(audio, 31, "麦克风", "正常");
    ui_Device_speaker = device_detail_row(audio, 57, "扬声器", "正常");
    ui_Device_decoder = device_detail_row(audio, 83, "音频解码", "正常");
    device_detail_row(audio, 109, "当前音量", "80%");
    device_image(s_detail_sensing, &ui_img_device_v3_bottom_banner, 8, 376);
    return;

#if 0
    s_detail_sensing = device_detail_scroll(s_detail);

    lv_obj_t * sensor = device_panel(s_detail_sensing, 8, 0, 268, 180, DEVICE_CARD, 10);
    device_section_title(sensor, &ui_img_device_v3_icon_bme, "BME690环境传感器", 268);
    ui_Device_sensor_state = device_label(sensor, "预热中", 12, 43, 96, 14, DEVICE_OK, 153, LV_TEXT_ALIGN_LEFT);
    device_label(sensor, "状态", 12, 31, 86, 10, DEVICE_SUBTEXT, 120, LV_TEXT_ALIGN_LEFT);
    ui_Device_temperature = device_label(sensor, "--", 144, 43, 104, 14, DEVICE_GREEN_DARK, 153, LV_TEXT_ALIGN_LEFT);
    device_label(sensor, "温度", 144, 31, 94, 10, DEVICE_SUBTEXT, 120, LV_TEXT_ALIGN_LEFT);
    ui_Device_humidity = device_label(sensor, "--", 12, 80, 96, 14, DEVICE_GREEN_DARK, 153, LV_TEXT_ALIGN_LEFT);
    device_label(sensor, "湿度", 12, 68, 86, 10, DEVICE_SUBTEXT, 120, LV_TEXT_ALIGN_LEFT);
    ui_Device_pressure = device_label(sensor, "--", 144, 80, 104, 14, DEVICE_GREEN_DARK, 153, LV_TEXT_ALIGN_LEFT);
    device_label(sensor, "气压", 144, 68, 94, 10, DEVICE_SUBTEXT, 120, LV_TEXT_ALIGN_LEFT);
    ui_Device_sensor_iaq = device_label(sensor, "--", 12, 117, 96, 14, DEVICE_GREEN_DARK, 153, LV_TEXT_ALIGN_LEFT);
    device_label(sensor, "IAQ", 12, 105, 86, 10, DEVICE_SUBTEXT, 120, LV_TEXT_ALIGN_LEFT);
    ui_Device_co2 = device_label(sensor, "--", 144, 117, 104, 14, DEVICE_GREEN_DARK, 153, LV_TEXT_ALIGN_LEFT);
    device_label(sensor, "等效CO2", 144, 105, 94, 10, DEVICE_SUBTEXT, 120, LV_TEXT_ALIGN_LEFT);
    ui_Device_sensor_accuracy = device_label(sensor, "--", 12, 154, 96, 14, DEVICE_GREEN_DARK, 153, LV_TEXT_ALIGN_LEFT);
    device_label(sensor, "IAQ精度", 12, 142, 86, 10, DEVICE_SUBTEXT, 120, LV_TEXT_ALIGN_LEFT);
    ui_Device_sensor_env = device_label(sensor, "--", 144, 154, 104, 14, DEVICE_GREEN_DARK, 153, LV_TEXT_ALIGN_LEFT);
    device_label(sensor, "最近更新", 144, 142, 94, 10, DEVICE_SUBTEXT, 120, LV_TEXT_ALIGN_LEFT);

    lv_obj_t * audio = device_panel(s_detail_sensing, 8, 188, 268, 64, DEVICE_CARD, 10);
    device_section_title(audio, &ui_img_device_v3_icon_microphone, "语音硬件", 268);
    ui_Device_mic = device_label(audio, "正常", 12, 34, 43, 13, DEVICE_OK, 145, LV_TEXT_ALIGN_LEFT);
    device_label(audio, "麦克风", 12, 23, 43, 10, DEVICE_SUBTEXT, 115, LV_TEXT_ALIGN_LEFT);
    ui_Device_speaker = device_label(audio, "正常", 78, 34, 43, 13, DEVICE_OK, 145, LV_TEXT_ALIGN_LEFT);
    device_label(audio, "扬声器", 78, 23, 43, 10, DEVICE_SUBTEXT, 115, LV_TEXT_ALIGN_LEFT);
    ui_Device_decoder = device_label(audio, "正常", 144, 34, 43, 13, DEVICE_OK, 145, LV_TEXT_ALIGN_LEFT);
    device_label(audio, "音频解码", 144, 23, 47, 10, DEVICE_SUBTEXT, 115, LV_TEXT_ALIGN_LEFT);
    device_label(audio, "80%", 210, 34, 45, 13, DEVICE_GREEN_DARK, 145, LV_TEXT_ALIGN_LEFT);
    device_label(audio, "当前音量", 210, 23, 45, 10, DEVICE_SUBTEXT, 115, LV_TEXT_ALIGN_LEFT);
    device_image(s_detail_sensing, &ui_img_device_v3_bottom_banner, 8, 258);
#endif
}

static void device_build_detail(void)
{
    s_detail = lv_obj_create(ui_Device);
    lv_obj_set_pos(s_detail, 0, 0);
    lv_obj_set_size(s_detail, 284, 240);
    device_pass_gestures(s_detail);
    lv_obj_set_style_bg_opa(s_detail, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(s_detail, 0, 0);
    lv_obj_set_style_pad_all(s_detail, 0, 0);
    lv_obj_add_event_cb(s_detail, device_detail_drag, LV_EVENT_PRESSING, NULL);
    lv_obj_add_event_cb(s_detail, device_consume_detail_gesture, LV_EVENT_GESTURE, NULL);
    device_image(s_detail, &ui_img_device_v3_detail_bg, 0, 0);
    device_image(s_detail, &ui_img_device_v3_icon_back, 6, 3);
    device_button(s_detail, 0, 0, 30, 28, DEVICE_VIEW_OVERVIEW);
    device_label(s_detail, "设备详情", 96, 5, 92, 19, DEVICE_TEXT, 190, LV_TEXT_ALIGN_CENTER);
    device_label(s_detail, "刷新", 238, 8, 30, 13, DEVICE_GREEN_DARK, 130, LV_TEXT_ALIGN_RIGHT);
    device_image(s_detail, &ui_img_device_v3_header_right, 184, 0);

    lv_obj_t * tabs = device_panel(s_detail, 8, 30, 268, 24, DEVICE_CARD, 12);
    lv_obj_set_style_border_color(tabs, lv_color_hex(DEVICE_DIVIDER), 0);
    s_tab_highlight = device_image(tabs, &ui_img_device_v3_tab_active, 0, 0);
    device_label(tabs, "系统", 0, 5, 89, 15, DEVICE_GREEN_DARK, 145, LV_TEXT_ALIGN_CENTER);
    device_label(tabs, "连接", 89, 5, 89, 15, DEVICE_TEXT, 145, LV_TEXT_ALIGN_CENTER);
    device_label(tabs, "感知", 178, 5, 90, 15, DEVICE_TEXT, 145, LV_TEXT_ALIGN_CENTER);
    device_button(tabs, 0, 0, 89, 24, DEVICE_VIEW_SYSTEM);
    device_button(tabs, 89, 0, 89, 24, DEVICE_VIEW_CONNECTION);
    device_button(tabs, 178, 0, 90, 24, DEVICE_VIEW_SENSING);

    device_build_system_page();
    device_build_connection_page();
    device_build_sensing_page();
    lv_obj_add_flag(s_detail, LV_OBJ_FLAG_HIDDEN);
}

void ui_event_Device(lv_event_t * event)
{
    if(lv_event_get_code(event) == LV_EVENT_CLICKED) {
        const uint8_t view = (uint8_t)(uintptr_t)lv_event_get_user_data(event);
        device_show_view(view);
    }
}

int ui_Device_is_detail_visible(void)
{
    return s_detail != NULL && !lv_obj_has_flag(s_detail, LV_OBJ_FLAG_HIDDEN);
}

void ui_Device_return_to_overview(void)
{
    if(ui_Device_is_detail_visible()) {
        device_show_view(DEVICE_VIEW_OVERVIEW);
    }
}

void ui_Device_screen_init(void)
{
    ui_Device = lv_obj_create(NULL);
    lv_obj_remove_style_all(ui_Device);
    lv_obj_set_style_bg_color(ui_Device, lv_color_hex(DEVICE_BG), 0);
    lv_obj_set_style_bg_opa(ui_Device, LV_OPA_COVER, 0);
    lv_obj_remove_flag(ui_Device, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(ui_Device, LV_OBJ_FLAG_CLICKABLE);

    device_build_overview();
    device_build_detail();
    device_show_view(DEVICE_VIEW_OVERVIEW);
}

void ui_Device_screen_destroy(void)
{
    if(ui_Device != NULL) lv_obj_del(ui_Device);

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
    ui_Device_health_title = NULL;
    ui_Device_health_subtitle = NULL;
    ui_Device_temperature = NULL;
    ui_Device_humidity = NULL;
    ui_Device_pressure = NULL;
    ui_Device_co2 = NULL;
    ui_Device_sensor_accuracy = NULL;
    ui_Device_overview_wifi = NULL;
    ui_Device_overview_sensor = NULL;
    ui_Device_overview_audio = NULL;
    ui_Device_overview_heap = NULL;
    ui_Device_overview_uptime = NULL;
    s_overview = NULL;
    s_detail = NULL;
    s_detail_system = NULL;
    s_detail_connection = NULL;
    s_detail_sensing = NULL;
    s_tab_highlight = NULL;
    s_active_detail_scroll = NULL;
}
