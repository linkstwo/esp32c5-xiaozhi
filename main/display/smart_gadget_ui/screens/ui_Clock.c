// Home clock V3 assembled from the approved 284x240 resource pack.

#include "../ui.h"
#include "ui_home_style.h"

#include <string.h>

LV_FONT_DECLARE(font_puhui_16_4);
LV_FONT_DECLARE(ui_font_HomeTime);

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
lv_obj_t * ui_Clock_NumberShadow = NULL;
lv_obj_t * ui_Clock_Date = NULL;
lv_obj_t * ui_Clock_Status = NULL;
lv_obj_t * ui_Clock_Env_Left = NULL;
lv_obj_t * ui_Clock_Env_Right = NULL;
lv_obj_t * ui_Scrolldots = NULL;

static lv_obj_t * s_mascot;
static lv_obj_t * s_branch;
static lv_obj_t * s_date_chip;
static lv_obj_t * s_date_icon;
static lv_obj_t * s_time_halo;
static lv_obj_t * s_moon;
static lv_obj_t * s_status_leaf;
static lv_obj_t * s_temp_card;
static lv_obj_t * s_hum_card;
static lv_obj_t * s_temp_icon;
static lv_obj_t * s_hum_icon;
static lv_obj_t * s_temp_title;
static lv_obj_t * s_hum_title;
static lv_obj_t * s_temp_plant;
static lv_obj_t * s_hum_plant;
static lv_obj_t * s_sparkle_large;
static lv_obj_t * s_sparkle_small;
static lv_obj_t * s_card_gloss;
static lv_obj_t * s_env_temp_overlay;
static lv_obj_t * s_env_hum_overlay;
static lv_obj_t * s_texture_tiles[6];
static char s_current_time[8];
static char s_pending_time[8];

static void home_clear_style(lv_obj_t * obj)
{
    lv_obj_remove_style_all(obj);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

static lv_obj_t * home_panel(lv_obj_t * parent, int x, int y, int w, int h,
                             int radius, uint32_t bg, lv_opa_t opa,
                             uint32_t border, int border_width)
{
    lv_obj_t * panel = lv_obj_create(parent);
    home_clear_style(panel);
    lv_obj_set_pos(panel, x, y);
    lv_obj_set_size(panel, w, h);
    lv_obj_set_style_radius(panel, radius, 0);
    lv_obj_set_style_bg_color(panel, lv_color_hex(bg), 0);
    lv_obj_set_style_bg_opa(panel, opa, 0);
    lv_obj_set_style_border_color(panel, lv_color_hex(border), 0);
    lv_obj_set_style_border_width(panel, border_width, 0);
    lv_obj_set_style_clip_corner(panel, true, 0);
    return panel;
}

static lv_obj_t * home_label(lv_obj_t * parent, const char * text,
                             int x, int y, int w, int h,
                             uint32_t color, const lv_font_t * font,
                             lv_text_align_t align)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_obj_set_pos(label, x, y);
    lv_obj_set_size(label, w, h);
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

static lv_obj_t * home_image(lv_obj_t * parent, const void * src, int x, int y)
{
    lv_obj_t * image = lv_image_create(parent);
    lv_image_set_src(image, src);
    lv_obj_set_pos(image, x, y);
    lv_obj_remove_flag(image, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(image, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(image, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return image;
}

static void home_anim_y(void * obj, int32_t value)
{
    lv_obj_set_y((lv_obj_t *)obj, value);
}

static void home_anim_opa(void * obj, int32_t value)
{
    lv_obj_set_style_opa((lv_obj_t *)obj, value, 0);
}

static void home_anim_rotation(void * obj, int32_t value)
{
    lv_image_set_rotation((lv_obj_t *)obj, value);
}

static void home_start_ambient_animations(void)
{
    if (s_mascot != NULL) {
        lv_anim_t mascot;
        lv_anim_init(&mascot);
        lv_anim_set_var(&mascot, s_mascot);
        lv_anim_set_exec_cb(&mascot, home_anim_y);
        lv_anim_set_values(&mascot, HOME_MASCOT_Y,
                           HOME_MASCOT_Y + HOME_MASCOT_FLOAT_PX);
        lv_anim_set_duration(&mascot, 1600);
        lv_anim_set_playback_duration(&mascot, 1600);
        lv_anim_set_repeat_count(&mascot, LV_ANIM_REPEAT_INFINITE);
        lv_anim_set_path_cb(&mascot, lv_anim_path_ease_in_out);
        lv_anim_start(&mascot);
    }

    if (s_time_halo != NULL) {
        lv_anim_t halo;
        lv_anim_init(&halo);
        lv_anim_set_var(&halo, s_time_halo);
        lv_anim_set_exec_cb(&halo, home_anim_opa);
        lv_anim_set_values(&halo, HOME_HALO_OPA_MIN, HOME_HALO_OPA_MAX);
        lv_anim_set_duration(&halo, 1900);
        lv_anim_set_playback_duration(&halo, 1900);
        lv_anim_set_repeat_count(&halo, LV_ANIM_REPEAT_INFINITE);
        lv_anim_set_path_cb(&halo, lv_anim_path_ease_in_out);
        lv_anim_start(&halo);
    }

    if (s_branch != NULL) {
        lv_image_set_pivot(s_branch, 82, 4);
        lv_anim_t branch;
        lv_anim_init(&branch);
        lv_anim_set_var(&branch, s_branch);
        lv_anim_set_exec_cb(&branch, home_anim_rotation);
        lv_anim_set_values(&branch,
                           -HOME_BRANCH_ROTATION_TENTH_DEG,
                           HOME_BRANCH_ROTATION_TENTH_DEG);
        lv_anim_set_duration(&branch, 2200);
        lv_anim_set_playback_duration(&branch, 2200);
        lv_anim_set_repeat_count(&branch, LV_ANIM_REPEAT_INFINITE);
        lv_anim_set_path_cb(&branch, lv_anim_path_ease_in_out);
        lv_anim_start(&branch);
    }
}

static void home_time_slide_in(void)
{
    if (ui_Clock_Number == NULL) {
        return;
    }
    lv_label_set_text(ui_Clock_Number, s_pending_time);
    strncpy(s_current_time, s_pending_time, sizeof(s_current_time) - 1);
    s_current_time[sizeof(s_current_time) - 1] = '\0';
    lv_obj_set_y(ui_Clock_Number, HOME_TIME_Y + HOME_TIME_TRANSITION_Y);
    lv_obj_set_style_opa(ui_Clock_Number, LV_OPA_TRANSP, 0);

    lv_anim_t y;
    lv_anim_init(&y);
    lv_anim_set_var(&y, ui_Clock_Number);
    lv_anim_set_exec_cb(&y, home_anim_y);
    lv_anim_set_values(&y, HOME_TIME_Y + HOME_TIME_TRANSITION_Y, HOME_TIME_Y);
    lv_anim_set_duration(&y, 180);
    lv_anim_set_path_cb(&y, lv_anim_path_ease_out);
    lv_anim_start(&y);

    lv_anim_t opa;
    lv_anim_init(&opa);
    lv_anim_set_var(&opa, ui_Clock_Number);
    lv_anim_set_exec_cb(&opa, home_anim_opa);
    lv_anim_set_values(&opa, LV_OPA_TRANSP, LV_OPA_COVER);
    lv_anim_set_duration(&opa, 180);
    lv_anim_start(&opa);
}

static void home_time_out_ready(lv_anim_t * anim)
{
    (void)anim;
    home_time_slide_in();
}

void ui_Clock_set_time_text(const char * text, bool animate)
{
    if (ui_Clock_Number == NULL || text == NULL) {
        return;
    }
    if (strncmp(s_current_time, text, sizeof(s_current_time)) == 0) {
        return;
    }

    strncpy(s_pending_time, text, sizeof(s_pending_time) - 1);
    s_pending_time[sizeof(s_pending_time) - 1] = '\0';

    if (!animate || s_current_time[0] == '\0') {
        lv_label_set_text(ui_Clock_Number, s_pending_time);
        strncpy(s_current_time, s_pending_time, sizeof(s_current_time) - 1);
        s_current_time[sizeof(s_current_time) - 1] = '\0';
        lv_obj_set_y(ui_Clock_Number, HOME_TIME_Y);
        lv_obj_set_style_opa(ui_Clock_Number, LV_OPA_COVER, 0);
        return;
    }

    lv_anim_delete(ui_Clock_Number, NULL);

    lv_anim_t y;
    lv_anim_init(&y);
    lv_anim_set_var(&y, ui_Clock_Number);
    lv_anim_set_exec_cb(&y, home_anim_y);
    lv_anim_set_values(&y, HOME_TIME_Y,
                       HOME_TIME_Y - HOME_TIME_TRANSITION_Y);
    lv_anim_set_duration(&y, 150);
    lv_anim_set_path_cb(&y, lv_anim_path_ease_in);
    lv_anim_start(&y);

    lv_anim_t opa;
    lv_anim_init(&opa);
    lv_anim_set_var(&opa, ui_Clock_Number);
    lv_anim_set_exec_cb(&opa, home_anim_opa);
    lv_anim_set_values(&opa, LV_OPA_COVER, LV_OPA_TRANSP);
    lv_anim_set_duration(&opa, 150);
    lv_anim_set_ready_cb(&opa, home_time_out_ready);
    lv_anim_start(&opa);
}

void ui_event_Clock(lv_event_t * e)
{
    const lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SCREEN_LOAD_START) {
        home_start_ambient_animations();
    }
}

void ui_Clock_screen_init(void)
{
    ui_Clock = lv_obj_create(NULL);
    home_clear_style(ui_Clock);
    lv_obj_set_style_bg_color(ui_Clock, lv_color_hex(HOME_COLOR_BG), 0);
    lv_obj_set_style_bg_opa(ui_Clock, LV_OPA_COVER, 0);

    ui_Dot1 = home_panel(ui_Clock, -52, 27, 104, 104,
                         LV_RADIUS_CIRCLE, HOME_COLOR_DECOR_GOLD,
                         80, HOME_COLOR_DECOR_GOLD, 0);
    ui_Dot2 = home_panel(ui_Clock, 232, 151, 104, 104,
                         LV_RADIUS_CIRCLE, HOME_COLOR_DECOR_GREEN,
                         96, HOME_COLOR_DECOR_GREEN, 0);

    ui_Clock_Panel = home_panel(ui_Clock, HOME_CARD_X, HOME_CARD_Y,
                                HOME_CARD_W, HOME_CARD_H, HOME_CARD_RADIUS,
                                HOME_COLOR_CARD, LV_OPA_COVER,
                                HOME_COLOR_CARD_BORDER, 1);
    lv_obj_set_style_shadow_color(ui_Clock_Panel,
                                  lv_color_hex(HOME_COLOR_CARD_SHADOW), 0);
    lv_obj_set_style_shadow_opa(ui_Clock_Panel, 64, 0);
    lv_obj_set_style_shadow_width(ui_Clock_Panel, 12, 0);
    lv_obj_set_style_shadow_offset_y(ui_Clock_Panel, 4, 0);

    for (uint32_t i = 0; i < 6; ++i) {
        s_texture_tiles[i] = home_image(
            ui_Clock_Panel, &ui_img_home_paper_texture,
            (int)(i % 3) * 96, (int)(i / 3) * 96);
        lv_obj_set_style_opa(s_texture_tiles[i], 30, 0);
    }
    s_card_gloss = home_image(ui_Clock_Panel,
                              &ui_img_home_card_gloss, 0, -3);
    lv_obj_set_style_opa(s_card_gloss, LV_OPA_80, 0);

    s_mascot = home_image(ui_Clock, &ui_img_home_mascot,
                          HOME_MASCOT_X, HOME_MASCOT_Y);
    s_branch = home_image(ui_Clock, &ui_img_home_branch_corner,
                          HOME_BRANCH_X, HOME_BRANCH_Y);

    s_date_chip = home_panel(ui_Clock, HOME_DATE_X, HOME_DATE_Y,
                             HOME_DATE_W, HOME_DATE_H, 12,
                             HOME_COLOR_CARD, LV_OPA_COVER,
                             HOME_COLOR_CARD_BORDER, 1);
    s_date_icon = home_image(ui_Clock, &ui_img_home_calendar,
                             HOME_DATE_ICON_X, HOME_DATE_ICON_Y);
    ui_Clock_Date = home_label(ui_Clock, "--·-- ---",
                               HOME_DATE_TEXT_X, HOME_DATE_TEXT_Y,
                               HOME_DATE_TEXT_W, 18,
                               HOME_COLOR_DATE, &font_puhui_16_4,
                               LV_TEXT_ALIGN_LEFT);

    s_time_halo = home_image(ui_Clock, &ui_img_home_time_halo,
                             HOME_TIME_HALO_X, HOME_TIME_HALO_Y);
    lv_obj_set_style_opa(s_time_halo, HOME_HALO_OPA_MAX, 0);

    ui_Clock_Number = home_label(ui_Clock, "--:--",
                                 HOME_TIME_X, HOME_TIME_Y,
                                 HOME_TIME_W, HOME_TIME_H,
                                 HOME_COLOR_TIME, &ui_font_HomeTime,
                                 LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_text_letter_space(ui_Clock_Number, 2, 0);
    lv_obj_set_style_translate_y(ui_Clock_Number, 10, 0);
    lv_obj_set_style_transform_pivot_x(ui_Clock_Number, HOME_TIME_W / 2, 0);
    lv_obj_set_style_transform_pivot_y(ui_Clock_Number, HOME_TIME_H / 2, 0);
    lv_obj_set_style_transform_zoom(ui_Clock_Number, 274, 0);
    ui_Clock_NumberShadow = NULL;

    s_moon = home_image(ui_Clock, &ui_img_home_moon,
                        HOME_MOON_X, HOME_MOON_Y);
    s_status_leaf = home_image(ui_Clock, &ui_img_home_center_leaf,
                               HOME_STATUS_LEAF_X, HOME_STATUS_LEAF_Y);
    ui_Clock_Status = home_label(ui_Clock, "等待传感器",
                                 HOME_STATUS_X, HOME_STATUS_Y,
                                 HOME_STATUS_W, HOME_STATUS_H,
                                 HOME_COLOR_GREEN, &font_puhui_16_4,
                                 LV_TEXT_ALIGN_LEFT);

    s_temp_card = home_panel(ui_Clock, HOME_TEMP_CARD_X, HOME_ENV_CARD_Y,
                             HOME_ENV_CARD_W, HOME_ENV_CARD_H, 18,
                             HOME_COLOR_ENV_BG, LV_OPA_COVER,
                             HOME_COLOR_ENV_BORDER, 1);
    s_hum_card = home_panel(ui_Clock, HOME_HUM_CARD_X, HOME_ENV_CARD_Y,
                            HOME_ENV_CARD_W, HOME_ENV_CARD_H, 18,
                            HOME_COLOR_ENV_BG, LV_OPA_COVER,
                            HOME_COLOR_ENV_BORDER, 1);
    s_env_temp_overlay = home_image(ui_Clock,
                                    &ui_img_home_env_overlay,
                                    HOME_TEMP_CARD_X - 5,
                                    HOME_ENV_CARD_Y - 2);
    s_env_hum_overlay = home_image(ui_Clock,
                                   &ui_img_home_env_overlay,
                                   HOME_HUM_CARD_X - 5,
                                   HOME_ENV_CARD_Y - 2);
    s_temp_plant = home_image(ui_Clock, &ui_img_home_temp_plant,
                              HOME_TEMP_PLANT_X, HOME_TEMP_PLANT_Y);
    s_hum_plant = home_image(ui_Clock, &ui_img_home_hum_plant,
                             HOME_HUM_PLANT_X, HOME_HUM_PLANT_Y);
    s_temp_icon = home_image(ui_Clock, &ui_img_home_temp_icon,
                             HOME_TEMP_ICON_X, HOME_ENV_ICON_Y);
    s_hum_icon = home_image(ui_Clock, &ui_img_home_hum_icon,
                            HOME_HUM_ICON_X, HOME_ENV_ICON_Y);

    s_temp_title = home_label(ui_Clock, "温度",
                              HOME_TEMP_TITLE_X, HOME_ENV_TITLE_Y,
                              HOME_ENV_TITLE_W, 16,
                              HOME_COLOR_DATE, &font_puhui_16_4,
                              LV_TEXT_ALIGN_LEFT);
    s_hum_title = home_label(ui_Clock, "湿度",
                             HOME_HUM_TITLE_X, HOME_ENV_TITLE_Y,
                             HOME_ENV_TITLE_W, 16,
                             HOME_COLOR_DATE, &font_puhui_16_4,
                             LV_TEXT_ALIGN_LEFT);
    lv_obj_set_style_transform_zoom(s_temp_title, 200, 0);
    lv_obj_set_style_transform_zoom(s_hum_title, 200, 0);

    ui_Clock_Env_Left = home_label(ui_Clock, "--°C",
                                   HOME_TEMP_VALUE_X, HOME_ENV_VALUE_Y,
                                   HOME_ENV_VALUE_W, 18,
                                   HOME_COLOR_TIME, &font_puhui_16_4,
                                   LV_TEXT_ALIGN_LEFT);
    ui_Clock_Env_Right = home_label(ui_Clock, "--%",
                                    HOME_HUM_VALUE_X, HOME_ENV_VALUE_Y,
                                    HOME_ENV_VALUE_W, 18,
                                    HOME_COLOR_TIME, &font_puhui_16_4,
                                    LV_TEXT_ALIGN_LEFT);
    lv_obj_set_style_transform_zoom(ui_Clock_Env_Left, 245, 0);
    lv_obj_set_style_transform_zoom(ui_Clock_Env_Right, 245, 0);

    s_sparkle_large = home_image(ui_Clock,
                                 &ui_img_home_sparkle_large,
                                 HOME_SPARKLE_LARGE_X,
                                 HOME_SPARKLE_LARGE_Y);
    s_sparkle_small = home_image(ui_Clock,
                                 &ui_img_home_sparkle_small,
                                 HOME_SPARKLE_SMALL_X,
                                 HOME_SPARKLE_SMALL_Y);

    ui_Clock_Number1 = NULL;
    ui_Clock_Number2 = NULL;
    ui_Clock_Number3 = NULL;
    ui_Clock_Number4 = NULL;
    ui_Min = NULL;
    ui_Hour = NULL;
    ui_Sec = NULL;
    ui_Clock_Center = NULL;

    ui_Dot3 = NULL;
    ui_Dot4 = NULL;
    ui_Dot5 = NULL;
    ui_Dot6 = NULL;
    ui_Dot7 = NULL;
    ui_Dot8 = NULL;

    ui_Scrolldots = ui_Scrolldots_create(ui_Clock);
    lv_obj_set_align(ui_Scrolldots, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos(ui_Scrolldots, 111, HOME_PAGE_DOTS_Y - 4);
    ui_update_page_indicator(ui_Scrolldots, 0, 7,
                             HOME_COLOR_PAGE_DOT,
                             HOME_COLOR_PAGE_DOT_IDLE);

    s_current_time[0] = '\0';
    s_pending_time[0] = '\0';
    lv_obj_add_event_cb(ui_Clock, ui_event_Clock, LV_EVENT_ALL, NULL);
}

void ui_Clock_screen_destroy(void)
{
    if (ui_Clock != NULL) {
        lv_obj_del(ui_Clock);
    }

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
    ui_Clock_NumberShadow = NULL;
    ui_Clock_Date = NULL;
    ui_Clock_Status = NULL;
    ui_Clock_Env_Left = NULL;
    ui_Clock_Env_Right = NULL;
    ui_Scrolldots = NULL;

    s_mascot = NULL;
    s_branch = NULL;
    s_date_chip = NULL;
    s_date_icon = NULL;
    s_time_halo = NULL;
    s_moon = NULL;
    s_status_leaf = NULL;
    s_temp_card = NULL;
    s_hum_card = NULL;
    s_temp_icon = NULL;
    s_hum_icon = NULL;
    s_temp_title = NULL;
    s_hum_title = NULL;
    s_temp_plant = NULL;
    s_hum_plant = NULL;
    s_sparkle_large = NULL;
    s_sparkle_small = NULL;
    s_card_gloss = NULL;
    s_env_temp_overlay = NULL;
    s_env_hum_overlay = NULL;
    for (uint32_t i = 0; i < 6; ++i) {
        s_texture_tiles[i] = NULL;
    }
    s_current_time[0] = '\0';
    s_pending_time[0] = '\0';
}
