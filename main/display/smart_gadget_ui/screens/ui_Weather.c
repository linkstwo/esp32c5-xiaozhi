// Environment page assembled from tangtang_environment_ui_pack_v3.

#include "../ui.h"

LV_FONT_DECLARE(font_puhui_16_4);
LV_FONT_DECLARE(ui_font_MusicTitle);

lv_obj_t * ui_Weather = NULL;
lv_obj_t * ui_Pary_Cloud = NULL;
lv_obj_t * ui_Celsius = NULL;
lv_obj_t * ui_Weather_Icons = NULL;
lv_obj_t * ui_w1 = NULL;
lv_obj_t * ui_w2 = NULL;
lv_obj_t * ui_w3 = NULL;
lv_obj_t * ui_W1_Num = NULL;
lv_obj_t * ui_W2_Num = NULL;
lv_obj_t * ui_W3_Num = NULL;
lv_obj_t * ui_Weather_Advice = NULL;
lv_obj_t * ui_Scrolldots4 = NULL;

static void weather_pass_gestures(lv_obj_t * obj)
{
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

static lv_obj_t * weather_image(lv_obj_t * parent, const void * source, int x, int y)
{
    lv_obj_t * image = lv_image_create(parent);
    lv_image_set_src(image, source);
    lv_obj_set_pos(image, x, y);
    weather_pass_gestures(image);
    return image;
}

static lv_obj_t * weather_label(lv_obj_t * parent, const char * text,
                                int x, int y, int width, int height,
                                uint32_t color, const lv_font_t * font,
                                lv_text_align_t align)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_obj_set_pos(label, x, y);
    lv_obj_set_size(label, width, height);
    lv_label_set_text(label, text);
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_bg_opa(label, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(label, 0, 0);
    lv_obj_set_style_pad_all(label, 0, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_align(label, align, 0);
    weather_pass_gestures(label);
    return label;
}

static void weather_scale_label(lv_obj_t * label, int width, int height, uint16_t zoom)
{
    lv_obj_set_style_transform_pivot_x(label, width / 2, 0);
    lv_obj_set_style_transform_pivot_y(label, height / 2, 0);
    lv_obj_set_style_transform_zoom(label, zoom, 0);
}

void ui_event_Weather(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_SCREEN_LOADED && ui_Scrolldots4 != NULL) {
        scrolldot_Animation(ui_Scrolldots4, 0);
    }
}

void ui_Weather_screen_init(void)
{
    ui_Weather = lv_obj_create(NULL);
    lv_obj_remove_style_all(ui_Weather);
    lv_obj_set_style_bg_color(ui_Weather, lv_color_hex(0xF8F6E8), 0);
    lv_obj_set_style_bg_opa(ui_Weather, LV_OPA_COVER, 0);
    lv_obj_remove_flag(ui_Weather, LV_OBJ_FLAG_SCROLLABLE);

    weather_image(ui_Weather, &ui_img_environment_v3_topbar, 7, 2);
    weather_image(ui_Weather, &ui_img_environment_v3_speech, 11, 41);
    weather_image(ui_Weather, &ui_img_environment_v3_character, 5, 77);
    weather_image(ui_Weather, &ui_img_environment_v3_hero, 94, 28);
    weather_image(ui_Weather, &ui_img_environment_v3_temp_card, 94, 116);
    weather_image(ui_Weather, &ui_img_environment_v3_humidity_card, 183, 116);
    weather_image(ui_Weather, &ui_img_environment_v3_iaq_card, 94, 165);
    weather_image(ui_Weather, &ui_img_environment_v3_pressure_card, 183, 165);

    ui_Pary_Cloud = weather_label(ui_Weather, "等待传感器",
                                  145, 50, 84, 19,
                                  0x3D812F, &font_puhui_16_4,
                                  LV_TEXT_ALIGN_CENTER);
    weather_scale_label(ui_Pary_Cloud, 84, 19, 300);

    ui_Weather_Advice = weather_label(ui_Weather, "正在连接环境传感器",
                                      102, 83, 190, 17,
                                      0x568B36, &font_puhui_16_4,
                                      LV_TEXT_ALIGN_CENTER);
    weather_scale_label(ui_Weather_Advice, 190, 17, 180);

    ui_Celsius = weather_label(ui_Weather, "--",
                               133, 135, 29, 23,
                               0x293226, &ui_font_MusicTitle,
                               LV_TEXT_ALIGN_RIGHT);
    lv_obj_t * temp_unit = weather_label(ui_Weather, "°C", 158, 141, 24, 18,
                                         0x71806A, &font_puhui_16_4,
                                         LV_TEXT_ALIGN_CENTER);
    weather_scale_label(temp_unit, 24, 18, 210);

    ui_W3_Num = weather_label(ui_Weather, "--",
                              220, 135, 27, 23,
                              0x293226, &ui_font_MusicTitle,
                              LV_TEXT_ALIGN_RIGHT);
    lv_obj_t * humidity_unit = weather_label(ui_Weather, "%", 246, 141, 20, 18,
                                             0x71806A, &font_puhui_16_4,
                                             LV_TEXT_ALIGN_CENTER);
    weather_scale_label(humidity_unit, 20, 18, 210);

    ui_W1_Num = weather_label(ui_Weather, "--",
                              132, 183, 31, 23,
                              0x293226, &ui_font_MusicTitle,
                              LV_TEXT_ALIGN_CENTER);

    ui_W2_Num = weather_label(ui_Weather, "--",
                              216, 183, 42, 23,
                              0x293226, &ui_font_MusicTitle,
                              LV_TEXT_ALIGN_RIGHT);
    lv_obj_t * pressure_unit = weather_label(ui_Weather, "hPa", 252, 189, 28, 18,
                                             0x71806A, &font_puhui_16_4,
                                             LV_TEXT_ALIGN_LEFT);
    weather_scale_label(pressure_unit, 28, 18, 190);

    ui_Weather_Icons = NULL;
    ui_w1 = NULL;
    ui_w2 = NULL;
    ui_w3 = NULL;

    ui_Scrolldots4 = ui_Scrolldots_create(ui_Weather);
    lv_obj_set_align(ui_Scrolldots4, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_y(ui_Scrolldots4, -1);
    ui_update_page_indicator(ui_Scrolldots4, 4, 7, 0x6E9B52, 0xD8CFB2);

    lv_obj_add_event_cb(ui_Weather, ui_event_Weather, LV_EVENT_ALL, NULL);
}

void ui_Weather_screen_destroy(void)
{
    if(ui_Weather != NULL) {
        lv_obj_del(ui_Weather);
    }

    ui_Weather = NULL;
    ui_Pary_Cloud = NULL;
    ui_Celsius = NULL;
    ui_Weather_Icons = NULL;
    ui_w1 = NULL;
    ui_w2 = NULL;
    ui_w3 = NULL;
    ui_W1_Num = NULL;
    ui_W2_Num = NULL;
    ui_W3_Num = NULL;
    ui_Weather_Advice = NULL;
    ui_Scrolldots4 = NULL;
}
