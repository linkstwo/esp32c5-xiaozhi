// This file follows the SquareLine Studio screen style used by Smart_Gadget.

#include "../ui.h"
#include "ui_home_style.h"

LV_FONT_DECLARE(font_puhui_16_4);

lv_obj_t * ui_Clock = NULL;
lv_obj_t * ui_Clock_Panel = NULL;
lv_obj_t * ui_Dot1 = NULL;
lv_obj_t * ui_Dot2 = NULL;
lv_obj_t * ui_Dot3 = NULL;
lv_obj_t * ui_Dot4 = NULL;
lv_obj_t * ui_Dot5 = NULL;
lv_obj_t * ui_Dot6 = NULL;
lv_obj_t * ui_Dot7 = NULL;
lv_obj_t * ui_Dot8 = NULL;
lv_obj_t * ui_Clock_Number1 = NULL;
lv_obj_t * ui_Clock_Number2 = NULL;
lv_obj_t * ui_Clock_Number3 = NULL;
lv_obj_t * ui_Clock_Number4 = NULL;
lv_obj_t * ui_Min = NULL;
lv_obj_t * ui_Hour = NULL;
lv_obj_t * ui_Sec = NULL;
lv_obj_t * ui_Clock_Center = NULL;
lv_obj_t * ui_Clock_Number = NULL;
lv_obj_t * ui_Clock_Date = NULL;
lv_obj_t * ui_Clock_Status = NULL;
lv_obj_t * ui_Clock_Env_Left = NULL;
lv_obj_t * ui_Clock_Env_Right = NULL;
lv_obj_t * ui_Scrolldots = NULL;

static lv_obj_t * ui_Clock_Mascot = NULL;
static lv_obj_t * ui_Clock_CornerLeaf = NULL;
static lv_obj_t * ui_Clock_DateIcon = NULL;
static lv_obj_t * ui_Clock_StatusLeaf = NULL;
static lv_obj_t * ui_Clock_TempIcon = NULL;
static lv_obj_t * ui_Clock_HumidityIcon = NULL;
lv_obj_t * ui_Clock_NumberShadow = NULL;

static void home_clear_default_style(lv_obj_t * obj)
{
    lv_obj_remove_style_all(obj);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

static lv_obj_t * home_circle_create(lv_obj_t * parent, int x, int y, int size, uint32_t color, lv_opa_t opa)
{
    lv_obj_t * obj = lv_obj_create(parent);
    home_clear_default_style(obj);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, size, size);
    lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(obj, opa, 0);
    return obj;
}

static lv_obj_t * home_plus_create(lv_obj_t * parent, int x, int y)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_obj_set_pos(label, x, y);
    lv_label_set_text(label, "+");
    lv_obj_set_style_text_color(label, lv_color_hex(HOME_COLOR_PLUS), 0);
    lv_obj_set_style_text_opa(label, LV_OPA_70, 0);
    lv_obj_set_style_text_font(label, &font_puhui_16_4, 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return label;
}

static lv_obj_t * home_label_create(lv_obj_t * parent, const char * text, int x, int y, int width,
                                    uint32_t color, const lv_font_t * font, lv_text_align_t align)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_obj_set_pos(label, x, y);
    lv_obj_set_size(label, width, LV_SIZE_CONTENT);
    lv_label_set_text(label, text);
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_align(label, align, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return label;
}

static lv_obj_t * home_image_create(lv_obj_t * parent, const lv_image_dsc_t * src, int x, int y)
{
    lv_obj_t * image = lv_image_create(parent);
    lv_image_set_src(image, src);
    lv_obj_set_pos(image, x, y);
    lv_obj_add_flag(image, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return image;
}

static lv_obj_t * home_pill_create(lv_obj_t * parent, int x, uint32_t bg_color, uint32_t border_color)
{
    lv_obj_t * pill = lv_obj_create(parent);
    home_clear_default_style(pill);
    lv_obj_set_pos(pill, x, HOME_PILL_Y);
    lv_obj_set_size(pill, HOME_PILL_W, HOME_PILL_H);
    lv_obj_set_style_radius(pill, HOME_PILL_RADIUS, 0);
    lv_obj_set_style_bg_color(pill, lv_color_hex(bg_color), 0);
    lv_obj_set_style_bg_opa(pill, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(pill, lv_color_hex(border_color), 0);
    lv_obj_set_style_border_width(pill, 1, 0);
    return pill;
}

// event funtions
void ui_event_Clock(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_SCREEN_LOAD_START) {
        upanim_Animation(ui_Clock_Panel, 80);
        upanim_Animation(ui_Clock_Number, 180);
        if(ui_Scrolldots != NULL) {
            scrolldot_Animation(ui_Scrolldots, 0);
        }
    }
    if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_LEFT) {
        lv_indev_wait_release(lv_indev_active());
        _ui_screen_change(&ui_Today, LV_SCR_LOAD_ANIM_FADE_ON, 10, 0, &ui_Today_screen_init);
    }
    if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_RIGHT) {
        lv_indev_wait_release(lv_indev_active());
        _ui_screen_change(&ui_Device, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Device_screen_init);
    }
}

// build funtions

void ui_Clock_screen_init(void)
{
    lv_obj_t * temp_pill;
    lv_obj_t * humidity_pill;

    ui_Clock = lv_obj_create(NULL);
    lv_obj_remove_flag(ui_Clock, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Clock, lv_color_hex(HOME_COLOR_BG), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Clock, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Clock, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_Clock, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Dot1 = home_circle_create(ui_Clock, -38, 48, 100, HOME_COLOR_DECOR_YELLOW, 150);
    ui_Dot2 = home_circle_create(ui_Clock, 229, 166, 88, HOME_COLOR_DECOR_GREEN, 145);
    ui_Dot3 = home_circle_create(ui_Clock, 13, 214, 9, HOME_COLOR_DECOR_DOT_1, 220);
    ui_Dot4 = home_circle_create(ui_Clock, 250, 47, 9, HOME_COLOR_DECOR_DOT_2, 190);
    ui_Dot5 = home_plus_create(ui_Clock, 262, 94);
    ui_Dot6 = home_plus_create(ui_Clock, 240, 176);
    ui_Dot7 = home_circle_create(ui_Clock, 48, 229, 4, HOME_COLOR_DECOR_DOT_3, 210);
    ui_Dot8 = home_circle_create(ui_Clock, 125, 231, 5, HOME_COLOR_DECOR_DOT_4, 220);

    ui_Clock_Panel = lv_obj_create(ui_Clock);
    home_clear_default_style(ui_Clock_Panel);
    lv_obj_set_pos(ui_Clock_Panel, HOME_CARD_X, HOME_CARD_Y);
    lv_obj_set_size(ui_Clock_Panel, HOME_CARD_W, HOME_CARD_H);
    lv_obj_set_style_radius(ui_Clock_Panel, HOME_CARD_RADIUS, 0);
    lv_obj_set_style_bg_color(ui_Clock_Panel, lv_color_hex(HOME_COLOR_CARD), 0);
    lv_obj_set_style_bg_opa(ui_Clock_Panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(ui_Clock_Panel, lv_color_hex(HOME_COLOR_CARD_BORDER), 0);
    lv_obj_set_style_border_width(ui_Clock_Panel, 1, 0);
    lv_obj_set_style_shadow_color(ui_Clock_Panel, lv_color_hex(HOME_COLOR_CARD_SHADOW), 0);
    lv_obj_set_style_shadow_opa(ui_Clock_Panel, LV_OPA_20, 0);
    lv_obj_set_style_shadow_width(ui_Clock_Panel, 14, 0);
    lv_obj_set_style_shadow_offset_y(ui_Clock_Panel, 4, 0);
    lv_obj_set_style_pad_all(ui_Clock_Panel, 0, 0);

    ui_Clock_Mascot = home_image_create(ui_Clock, &ui_img_home_mascot, HOME_MASCOT_X, HOME_MASCOT_Y);
    lv_obj_set_style_transform_zoom(ui_Clock_Mascot, 224, 0);
    lv_obj_remove_flag(ui_Clock_Mascot, LV_OBJ_FLAG_CLICKABLE);
    ui_Clock_CornerLeaf = home_image_create(ui_Clock, &ui_img_home_corner_leaf, HOME_CORNER_LEAF_X, HOME_CORNER_LEAF_Y);
    ui_Clock_DateIcon = home_image_create(ui_Clock_Panel, &ui_img_home_calendar, HOME_DATE_ICON_X, HOME_DATE_ICON_Y);

    ui_Clock_Date = home_label_create(ui_Clock_Panel, "-- -- ---",
                                      HOME_DATE_X, HOME_DATE_Y, HOME_DATE_W,
                                      HOME_COLOR_DATE, &font_puhui_16_4, LV_TEXT_ALIGN_LEFT);

    ui_Clock_NumberShadow = home_label_create(ui_Clock_Panel, "--:--",
                                              HOME_TIME_X + 1, HOME_TIME_Y + 1, HOME_TIME_W,
                                              0xD8C6B5, &ui_font_Number, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_text_opa(ui_Clock_NumberShadow, LV_OPA_90, 0);

    ui_Clock_Number = home_label_create(ui_Clock_Panel, "--:--",
                                        HOME_TIME_X, HOME_TIME_Y, HOME_TIME_W,
                                        HOME_COLOR_TIME, &ui_font_Number, LV_TEXT_ALIGN_CENTER);

    ui_Clock_StatusLeaf = home_image_create(ui_Clock_Panel, &ui_img_home_center_leaf,
                                            HOME_STATUS_LEAF_X, HOME_STATUS_LEAF_Y);

    ui_Clock_Status = home_label_create(ui_Clock_Panel, "Sensor warming up",
                                        HOME_STATUS_X, HOME_STATUS_Y, HOME_STATUS_W,
                                        HOME_COLOR_GREEN, &font_puhui_16_4, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_transform_zoom(ui_Clock_Status, 224, 0);

    temp_pill = home_pill_create(ui_Clock_Panel, HOME_TEMP_PILL_X, HOME_COLOR_TEMP_BG, HOME_COLOR_TEMP_BORDER);
    ui_Clock_TempIcon = home_image_create(temp_pill, &ui_img_home_temperature, HOME_PILL_ICON_X, HOME_PILL_ICON_Y);
    ui_Clock_Env_Left = home_label_create(temp_pill, "Temp -- C",
                                          HOME_PILL_LABEL_X, HOME_PILL_LABEL_Y, HOME_PILL_LABEL_W,
                                          HOME_COLOR_TEMP_TEXT, &font_puhui_16_4, LV_TEXT_ALIGN_LEFT);
    lv_obj_set_style_transform_zoom(ui_Clock_Env_Left, 214, 0);

    humidity_pill = home_pill_create(ui_Clock_Panel, HOME_HUM_PILL_X, HOME_COLOR_HUM_BG, HOME_COLOR_HUM_BORDER);
    ui_Clock_HumidityIcon = home_image_create(humidity_pill, &ui_img_home_humidity, HOME_PILL_ICON_X, HOME_PILL_ICON_Y);
    ui_Clock_Env_Right = home_label_create(humidity_pill, "Hum --%",
                                           HOME_PILL_LABEL_X, HOME_PILL_LABEL_Y, HOME_PILL_LABEL_W,
                                           HOME_COLOR_HUM_TEXT, &font_puhui_16_4, LV_TEXT_ALIGN_LEFT);
    lv_obj_set_style_transform_zoom(ui_Clock_Env_Right, 214, 0);

    ui_Clock_Number1 = NULL;
    ui_Clock_Number2 = NULL;
    ui_Clock_Number3 = NULL;
    ui_Clock_Number4 = NULL;
    ui_Min = NULL;
    ui_Hour = NULL;
    ui_Sec = NULL;
    ui_Clock_Center = NULL;

    ui_Scrolldots = ui_Scrolldots_create(ui_Clock);
    ui_update_page_indicator(ui_Scrolldots, 0, 7, HOME_COLOR_GREEN, 0xD8CFB2);

    lv_obj_add_event_cb(ui_Clock, ui_event_Clock, LV_EVENT_ALL, NULL);
}

void ui_Clock_screen_destroy(void)
{
    if(ui_Clock) {
        lv_obj_del(ui_Clock);
    }

    // NULL screen variables
    ui_Clock = NULL;
    ui_Clock_Panel = NULL;
    ui_Dot1 = NULL;
    ui_Dot2 = NULL;
    ui_Dot3 = NULL;
    ui_Dot4 = NULL;
    ui_Dot5 = NULL;
    ui_Dot6 = NULL;
    ui_Dot7 = NULL;
    ui_Dot8 = NULL;
    ui_Clock_Number1 = NULL;
    ui_Clock_Number2 = NULL;
    ui_Clock_Number3 = NULL;
    ui_Clock_Number4 = NULL;
    ui_Min = NULL;
    ui_Hour = NULL;
    ui_Sec = NULL;
    ui_Clock_Center = NULL;
    ui_Clock_Number = NULL;
    ui_Clock_Date = NULL;
    ui_Clock_Status = NULL;
    ui_Clock_Env_Left = NULL;
    ui_Clock_Env_Right = NULL;
    ui_Scrolldots = NULL;

    ui_Clock_Mascot = NULL;
    ui_Clock_CornerLeaf = NULL;
    ui_Clock_DateIcon = NULL;
    ui_Clock_StatusLeaf = NULL;
    ui_Clock_TempIcon = NULL;
    ui_Clock_HumidityIcon = NULL;
    ui_Clock_NumberShadow = NULL;
}
