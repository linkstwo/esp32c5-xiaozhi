// This file follows the SquareLine Studio screen style used by Smart_Gadget.

#include "../ui.h"

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

static lv_obj_t * clock_label_create(lv_obj_t * parent, const char * text, int y, int width,
                                     uint32_t color, const lv_font_t * font, lv_text_align_t align)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_obj_set_width(label, width);
    lv_obj_set_height(label, LV_SIZE_CONTENT);
    lv_obj_set_y(label, y);
    lv_obj_set_align(label, LV_ALIGN_TOP_MID);
    lv_label_set_text(label, text);
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_align(label, align, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label, lv_color_hex(color), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label, font, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(label, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return label;
}

static lv_obj_t * clock_pill_create(lv_obj_t * parent, int x, int y, int width, uint32_t color)
{
    lv_obj_t * pill = lv_obj_create(parent);
    lv_obj_set_width(pill, width);
    lv_obj_set_height(pill, 31);
    lv_obj_set_x(pill, x);
    lv_obj_set_y(pill, y);
    lv_obj_set_align(pill, LV_ALIGN_TOP_MID);
    lv_obj_remove_flag(pill, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(pill, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_radius(pill, 18, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(pill, lv_color_hex(color), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(pill, 235, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(pill, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(pill, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    return pill;
}

static void clock_dot_style(lv_obj_t * dot, int x, int y, int size, uint32_t color, lv_opa_t opa)
{
    lv_obj_set_width(dot, size);
    lv_obj_set_height(dot, size);
    lv_obj_set_x(dot, x);
    lv_obj_set_y(dot, y);
    lv_obj_set_align(dot, LV_ALIGN_CENTER);
    lv_obj_remove_flag(dot, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(dot, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_radius(dot, size, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(dot, lv_color_hex(color), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(dot, opa, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(dot, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
}

// event funtions
void ui_event_Clock(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_SCREEN_LOAD_START) {
        upanim_Animation(ui_Clock_Panel, 80);
        upanim_Animation(ui_Clock_Number, 180);
        scrolldot_Animation(ui_Scrolldots, 0);
    }
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_LEFT) {
        lv_indev_wait_release(lv_indev_active());
        _ui_screen_change(&ui_Device, LV_SCR_LOAD_ANIM_FADE_ON, 10, 0, &ui_Device_screen_init);
    }
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_RIGHT) {
        lv_indev_wait_release(lv_indev_active());
        _ui_screen_change(&ui_Alarm, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Alarm_screen_init);
    }
}

// build funtions

void ui_Clock_screen_init(void)
{
    ui_Clock = lv_obj_create(NULL);
    lv_obj_remove_flag(ui_Clock, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Clock, lv_color_hex(0xF7F2E8), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Clock, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Dot1 = lv_obj_create(ui_Clock);
    clock_dot_style(ui_Dot1, -111, -75, 78, 0xFFE0A3, 185);

    ui_Dot2 = lv_obj_create(ui_Clock);
    clock_dot_style(ui_Dot2, 114, 68, 92, 0xBFEBD8, 175);

    ui_Dot3 = lv_obj_create(ui_Clock);
    clock_dot_style(ui_Dot3, 105, -84, 11, 0x6CA7F8, 210);

    ui_Dot4 = lv_obj_create(ui_Clock);
    clock_dot_style(ui_Dot4, -117, 78, 8, 0xFFB85C, 210);

    ui_Dot5 = lv_obj_create(ui_Clock);
    clock_dot_style(ui_Dot5, -84, -101, 5, 0x7CC7A6, 180);

    ui_Dot6 = lv_obj_create(ui_Clock);
    clock_dot_style(ui_Dot6, 72, 101, 5, 0xF19B5A, 170);

    ui_Dot7 = lv_obj_create(ui_Clock);
    clock_dot_style(ui_Dot7, -32, 88, 4, 0x6CA7F8, 170);

    ui_Dot8 = lv_obj_create(ui_Clock);
    clock_dot_style(ui_Dot8, 34, -92, 4, 0x7CC7A6, 170);

    ui_Clock_Panel = lv_obj_create(ui_Clock);
    lv_obj_set_width(ui_Clock_Panel, 238);
    lv_obj_set_height(ui_Clock_Panel, 180);
    lv_obj_set_x(ui_Clock_Panel, 0);
    lv_obj_set_y(ui_Clock_Panel, -2);
    lv_obj_set_align(ui_Clock_Panel, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_Clock_Panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(ui_Clock_Panel, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_radius(ui_Clock_Panel, 30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Clock_Panel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Clock_Panel, 246, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_Clock_Panel, lv_color_hex(0xF0D9B5), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_Clock_Panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Clock_Panel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui_Clock_Panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Clock_Date = clock_label_create(ui_Clock_Panel, "06月18日 周四", 17, 198, 0x8A6A43,
                                       &font_puhui_16_4, LV_TEXT_ALIGN_CENTER);

    ui_Clock_Number = clock_label_create(ui_Clock_Panel, "09:25", 47, 222, 0x26324A,
                                         &ui_font_Number, LV_TEXT_ALIGN_CENTER);

    ui_Clock_Status = clock_label_create(ui_Clock_Panel, "适合专注", 116, 196, 0x2F8F63,
                                         &font_puhui_16_4, LV_TEXT_ALIGN_CENTER);

    lv_obj_t * left_pill = clock_pill_create(ui_Clock_Panel, -55, 140, 98, 0xEAF4FF);
    ui_Clock_Env_Left = clock_label_create(left_pill, "温 -- C", 7, 88, 0x355170,
                                           &font_puhui_16_4, LV_TEXT_ALIGN_CENTER);

    lv_obj_t * right_pill = clock_pill_create(ui_Clock_Panel, 55, 140, 98, 0xEAF8EF);
    ui_Clock_Env_Right = clock_label_create(right_pill, "湿 --%", 7, 88, 0x3B674C,
                                            &font_puhui_16_4, LV_TEXT_ALIGN_CENTER);

    ui_Clock_Number1 = NULL;
    ui_Clock_Number2 = NULL;
    ui_Clock_Number3 = NULL;
    ui_Clock_Number4 = NULL;
    ui_Min = NULL;
    ui_Hour = NULL;
    ui_Sec = NULL;
    ui_Clock_Center = NULL;

    ui_Scrolldots = ui_Scrolldots_create(ui_Clock);
    lv_obj_set_x(ui_Scrolldots, 0);
    lv_obj_set_y(ui_Scrolldots, -8);

    lv_obj_set_style_bg_color(ui_comp_get_child(ui_Scrolldots, UI_COMP_SCROLLDOTS_D1), lv_color_hex(0x2F8F63),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_comp_get_child(ui_Scrolldots, UI_COMP_SCROLLDOTS_D1), 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_Clock, ui_event_Clock, LV_EVENT_ALL, NULL);

}

void ui_Clock_screen_destroy(void)
{
    if(ui_Clock) lv_obj_del(ui_Clock);

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

}
