// Hand-tuned LVGL focus page for the 284 x 240 XiaoZhi product display.
// The layout intentionally mirrors the approved forest-spirit mockup while
// staying practical for LVGL 9 and the ESP32-C5 memory budget.

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

static lv_obj_t * today_panel_create(lv_obj_t * parent, int x, int y, int width, int height,
                                     int radius, uint32_t color, lv_opa_t opa,
                                     uint32_t border_color, int border_width)
{
    lv_obj_t * obj = lv_obj_create(parent);
    today_clear_obj_style(obj);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, width, height);
    lv_obj_set_style_radius(obj, radius, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(obj, opa, 0);
    lv_obj_set_style_border_color(obj, lv_color_hex(border_color), 0);
    lv_obj_set_style_border_width(obj, border_width, 0);
    return obj;
}

static lv_obj_t * today_circle_create(lv_obj_t * parent, int x, int y, int size,
                                      uint32_t color, lv_opa_t opa)
{
    return today_panel_create(parent, x, y, size, size, LV_RADIUS_CIRCLE,
                              color, opa, color, 0);
}

static lv_obj_t * today_label_create(lv_obj_t * parent, const char * text,
                                      int x, int y, int width,
                                      uint32_t color, const lv_font_t * font,
                                      lv_text_align_t align)
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

static lv_obj_t * today_button_create(lv_obj_t * parent, int x, int y,
                                      int width, int height, int radius,
                                      uint32_t bg_color, uint32_t border_color,
                                      bool primary)
{
    lv_obj_t * btn = today_panel_create(parent, x, y, width, height, radius,
                                        bg_color, LV_OPA_COVER, border_color, 1);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_pad_left(btn, primary ? 13 : 7, 0);
    lv_obj_set_style_pad_right(btn, primary ? 13 : 7, 0);
    lv_obj_set_style_pad_top(btn, 0, 0);
    lv_obj_set_style_pad_bottom(btn, 0, 0);
    lv_obj_set_style_pad_column(btn, primary ? 9 : 5, 0);
    lv_obj_set_layout(btn, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_shadow_color(btn, lv_color_hex(primary ? 0x71875E : 0xC9BFA9), 0);
    lv_obj_set_style_shadow_width(btn, primary ? 11 : 5, 0);
    lv_obj_set_style_shadow_opa(btn, primary ? 58 : 28, 0);
    lv_obj_set_style_shadow_offset_y(btn, primary ? 4 : 2, 0);
    lv_obj_set_style_transform_zoom(btn, 256, 0);
    lv_obj_set_style_transform_zoom(btn, 238, LV_STATE_PRESSED);
    return btn;
}

static lv_obj_t * today_button_icon_label_create(lv_obj_t * parent)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_text(label, "");
    lv_obj_set_style_text_color(label, lv_color_hex(FOCUS_COLOR_TEXT_GREEN), 0);
    lv_obj_set_style_text_font(label, &BUILTIN_ICON_FONT, 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return label;
}

static lv_obj_t * today_button_text_label_create(lv_obj_t * parent)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_text(label, "");
    lv_obj_set_style_text_color(label, lv_color_hex(FOCUS_COLOR_TEXT_GREEN), 0);
    lv_obj_set_style_text_font(label, &font_puhui_16_4, 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return label;
}

void ui_event_Today(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_SCREEN_LOAD_START) {
        if(ui_Today_timer_arc != NULL) {
            upanim_Animation(ui_Today_timer_arc, 40);
        }
        if(ui_Today_status_panel != NULL) {
            upanim_Animation(ui_Today_status_panel, 70);
        }
        if(ui_Today_state_panel != NULL) {
            upanim_Animation(ui_Today_state_panel, 90);
        }
        if(ui_Today_mascot_front != NULL) {
            upanim_Animation(ui_Today_mascot_front, 110);
        }
        if(ui_Today_duration_left != NULL) {
            upanim_Animation(ui_Today_duration_left, 140);
        }
        if(ui_Today_focus_btn != NULL) {
            upanim_Animation(ui_Today_focus_btn, 175);
        }
        if(ui_Today_focus_reset != NULL) {
            upanim_Animation(ui_Today_focus_reset, 210);
        }
    }
    if(event_code == LV_EVENT_GESTURE &&
       lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_RIGHT) {
        lv_indev_wait_release(lv_indev_active());
        _ui_screen_change(&ui_Clock, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0,
                          &ui_Clock_screen_init);
    }
    if(event_code == LV_EVENT_GESTURE &&
       lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_LEFT) {
        lv_indev_wait_release(lv_indev_active());
        _ui_screen_change(&ui_Music_Player, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0,
                          &ui_Music_Player_screen_init);
    }
}

void ui_Today_screen_init(void)
{
    ui_Today = lv_obj_create(NULL);
    lv_obj_remove_flag(ui_Today, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Today, lv_color_hex(FOCUS_COLOR_READY_BG),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Today, LV_OPA_COVER,
                            LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Today, 0,
                                  LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_Today, 0,
                            LV_PART_MAIN | LV_STATE_DEFAULT);

    // Restrained background character: one warm glow and two low-contrast hills.
    today_circle_create(ui_Today, -56, 26, 118, 0xF7E8C5, 105);
    today_circle_create(ui_Today, 231, 70, 62, 0xE7F0DA, 105);
    ui_Today_left_decor = today_panel_create(ui_Today, -35, 194, 190, 75, 42,
                                             0xF1EFD9, 185, 0xF1EFD9, 0);
    ui_Today_corner_vine = today_panel_create(ui_Today, 112, 188, 224, 82, 44,
                                              0xE7ECD2, 145, 0xE7ECD2, 0);

    // Soft halo underneath the dial makes the central component feel layered
    // without using a large bitmap or expensive blur effect.
    ui_Today_top_back = today_circle_create(ui_Today,
                                            FOCUS_TIMER_X - 4,
                                            FOCUS_TIMER_Y + 3,
                                            FOCUS_TIMER_SIZE + 8,
                                            0xE9DFC7, 72);

    ui_Today_timer_arc = lv_arc_create(ui_Today);
    lv_obj_set_pos(ui_Today_timer_arc, FOCUS_TIMER_X, FOCUS_TIMER_Y);
    lv_obj_set_size(ui_Today_timer_arc, FOCUS_TIMER_SIZE, FOCUS_TIMER_SIZE);
    lv_arc_set_rotation(ui_Today_timer_arc, 270);
    lv_arc_set_bg_angles(ui_Today_timer_arc, 0, 360);
    lv_arc_set_range(ui_Today_timer_arc, 0, 1000);
    lv_arc_set_value(ui_Today_timer_arc, 1000);
    lv_obj_clear_flag(ui_Today_timer_arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(ui_Today_timer_arc, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_arc_width(ui_Today_timer_arc, FOCUS_ARC_MAIN_WIDTH,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_Today_timer_arc, FOCUS_ARC_INDICATOR_WIDTH,
                               LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui_Today_timer_arc,
                               lv_color_hex(FOCUS_COLOR_TRACK),
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui_Today_timer_arc,
                               lv_color_hex(FOCUS_COLOR_READY_ACCENT),
                               LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_Today_timer_arc, LV_OPA_COVER,
                             LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_Today_timer_arc, LV_OPA_COVER,
                             LV_PART_INDICATOR | LV_STATE_DEFAULT);

    // The arc knob acts as the small leaf-like moving progress marker. An oval
    // is used instead of a separate bitmap so it automatically follows the arc.
    lv_obj_set_style_bg_color(ui_Today_timer_arc,
                              lv_color_hex(FOCUS_COLOR_READY_ACCENT),
                              LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Today_timer_arc, LV_OPA_COVER,
                            LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_width(ui_Today_timer_arc, 17,
                           LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_height(ui_Today_timer_arc, 11,
                            LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_Today_timer_arc, LV_RADIUS_CIRCLE,
                            LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_Today_timer_arc,
                                  lv_color_hex(0xFFFDF8),
                                  LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Today_timer_arc, 2,
                                  LV_PART_KNOB | LV_STATE_DEFAULT);

    ui_Today_focus_panel = today_circle_create(ui_Today,
                                               FOCUS_TIMER_INNER_X,
                                               FOCUS_TIMER_INNER_Y,
                                               FOCUS_TIMER_INNER_SIZE,
                                               FOCUS_COLOR_PANEL,
                                               LV_OPA_COVER);
    lv_obj_set_style_bg_grad_color(ui_Today_focus_panel,
                                   lv_color_hex(0xFFF8EA), 0);
    lv_obj_set_style_bg_grad_dir(ui_Today_focus_panel, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_border_color(ui_Today_focus_panel,
                                  lv_color_hex(FOCUS_COLOR_PANEL_BORDER), 0);
    lv_obj_set_style_border_width(ui_Today_focus_panel, 1, 0);
    lv_obj_set_style_shadow_color(ui_Today_focus_panel,
                                  lv_color_hex(0xBCA987), 0);
    lv_obj_set_style_shadow_width(ui_Today_focus_panel, 10, 0);
    lv_obj_set_style_shadow_opa(ui_Today_focus_panel, 28, 0);
    lv_obj_set_style_shadow_offset_y(ui_Today_focus_panel, 3, 0);

    ui_Today_status_panel = today_panel_create(ui_Today_focus_panel,
                                               34, 14, 72, 24, 12,
                                               0xF3F6E8, LV_OPA_COVER,
                                               0xE0E7D0, 1);
    ui_Today_status = today_label_create(ui_Today_status_panel,
                                         "专注中", 0, 3, 72,
                                         FOCUS_COLOR_TEXT_GREEN,
                                         &font_puhui_16_4,
                                         LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_transform_zoom(ui_Today_status, 205, 0);

    ui_Today_focus_hint = today_label_create(ui_Today_focus_panel,
                                              "", 0, 19,
                                              FOCUS_TIMER_INNER_SIZE,
                                              FOCUS_COLOR_TEXT_GREEN,
                                              &font_puhui_16_4,
                                              LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_transform_zoom(ui_Today_focus_hint, 200, 0);
    lv_obj_add_flag(ui_Today_focus_hint, LV_OBJ_FLAG_HIDDEN);

    ui_Today_focus_time = today_label_create(ui_Today_focus_panel,
                                              "25:00", 0, 48,
                                              FOCUS_TIMER_INNER_SIZE,
                                              FOCUS_COLOR_TEXT_MAIN,
                                              &ui_font_Number,
                                              LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_transform_zoom(ui_Today_focus_time, 160, 0);

    // Kept for the existing runtime state renderer. It remains hidden for the
    // clock-style MM:SS display but is available for future duration modes.
    ui_Today_focus_unit = today_label_create(ui_Today_focus_panel,
                                              "分钟", 0, 90,
                                              FOCUS_TIMER_INNER_SIZE,
                                              FOCUS_COLOR_TEXT_GREEN,
                                              &font_puhui_16_4,
                                              LV_TEXT_ALIGN_CENTER);
    lv_obj_add_flag(ui_Today_focus_unit, LV_OBJ_FLAG_HIDDEN);

    // Tiny vector seedling: three primitive objects, no additional image RAM.
    ui_Today_center_icon = today_panel_create(ui_Today_focus_panel,
                                              54, 101, 32, 17, 0,
                                              0xFFFFFF, LV_OPA_TRANSP,
                                              0xFFFFFF, 0);
    today_panel_create(ui_Today_center_icon, 15, 7, 2, 9, 1,
                       0x789D66, LV_OPA_COVER, 0x789D66, 0);
    today_panel_create(ui_Today_center_icon, 6, 3, 11, 6, LV_RADIUS_CIRCLE,
                       0x89A972, LV_OPA_COVER, 0x89A972, 0);
    today_panel_create(ui_Today_center_icon, 16, 2, 11, 6, LV_RADIUS_CIRCLE,
                       0x6F9560, LV_OPA_COVER, 0x6F9560, 0);
    today_panel_create(ui_Today_center_icon, 7, 14, 19, 2, 1,
                       0xE8D7B2, 150, 0xE8D7B2, 0);

    ui_Today_status_detail = today_label_create(ui_Today_focus_panel,
                                                "今日已专注 50 分钟",
                                                0, 119,
                                                FOCUS_TIMER_INNER_SIZE,
                                                FOCUS_COLOR_TEXT_MUTED,
                                                &font_puhui_16_4,
                                                LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_transform_zoom(ui_Today_status_detail, 190, 0);

    // Compact top-right session badge from the reference design.
    ui_Today_state_panel = today_panel_create(ui_Today,
                                              218, 8, 60, 24, 12,
                                              0xFFF8E9, 245,
                                              0xE8D9BC, 1);
    lv_obj_set_style_shadow_color(ui_Today_state_panel,
                                  lv_color_hex(0xC9B896), 0);
    lv_obj_set_style_shadow_width(ui_Today_state_panel, 5, 0);
    lv_obj_set_style_shadow_opa(ui_Today_state_panel, 24, 0);
    lv_obj_set_style_shadow_offset_y(ui_Today_state_panel, 2, 0);
    today_panel_create(ui_Today_state_panel, 7, 6, 7, 11, LV_RADIUS_CIRCLE,
                       FOCUS_COLOR_DECOR_GREEN, LV_OPA_COVER,
                       FOCUS_COLOR_DECOR_GREEN, 0);
    ui_Today_state_label = today_label_create(ui_Today_state_panel,
                                              "第 2 番", 16, 3, 41,
                                              FOCUS_COLOR_TEXT_MAIN,
                                              &font_puhui_16_4,
                                              LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_transform_zoom(ui_Today_state_label, 202, 0);

    // The mascot is a single existing native-alpha asset. Two layers are kept
    // because SmartGadgetDisplay cross-fades them when the focus state changes.
    ui_Today_mascot_back = lv_image_create(ui_Today);
    lv_image_set_src(ui_Today_mascot_back, &ui_img_focus_mascot_ready);
    lv_obj_set_pos(ui_Today_mascot_back,
                   FOCUS_MASCOT_RIGHT_X, FOCUS_MASCOT_RIGHT_Y);
    lv_obj_remove_flag(ui_Today_mascot_back, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(ui_Today_mascot_back, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_opa(ui_Today_mascot_back, LV_OPA_TRANSP, 0);
    lv_obj_set_style_transform_zoom(ui_Today_mascot_back, 218, 0);

    ui_Today_mascot_front = lv_image_create(ui_Today);
    lv_image_set_src(ui_Today_mascot_front, &ui_img_focus_mascot_ready);
    lv_obj_set_pos(ui_Today_mascot_front,
                   FOCUS_MASCOT_RIGHT_X, FOCUS_MASCOT_RIGHT_Y);
    lv_obj_remove_flag(ui_Today_mascot_front, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(ui_Today_mascot_front, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_transform_zoom(ui_Today_mascot_front, 218, 0);

    // One soft control shelf replaces multiple stacked cards.
    ui_Today_tip_panel = today_panel_create(ui_Today,
                                            3, 173, 278, 54, 25,
                                            0xFFFDF8, 232,
                                            0xEFE4CE, 1);
    lv_obj_set_style_shadow_color(ui_Today_tip_panel,
                                  lv_color_hex(0xBFAE8E), 0);
    lv_obj_set_style_shadow_width(ui_Today_tip_panel, 8, 0);
    lv_obj_set_style_shadow_opa(ui_Today_tip_panel, 25, 0);
    lv_obj_set_style_shadow_offset_y(ui_Today_tip_panel, 3, 0);

    ui_Today_duration_left = today_button_create(ui_Today,
                                                 FOCUS_LEFT_BTN_X,
                                                 FOCUS_LEFT_BTN_Y,
                                                 FOCUS_LEFT_BTN_W,
                                                 FOCUS_LEFT_BTN_H,
                                                 20,
                                                 FOCUS_COLOR_READY_LEFT,
                                                 FOCUS_COLOR_READY_ACCENT,
                                                 false);
    ui_Today_left_btn_icon = today_button_icon_label_create(ui_Today_duration_left);
    ui_Today_left_btn_label = today_button_text_label_create(ui_Today_duration_left);

    ui_Today_focus_btn = today_button_create(ui_Today,
                                             FOCUS_MAIN_BTN_X,
                                             FOCUS_MAIN_BTN_Y,
                                             FOCUS_MAIN_BTN_W,
                                             FOCUS_MAIN_BTN_H,
                                             24,
                                             FOCUS_COLOR_READY_PRIMARY,
                                             FOCUS_COLOR_READY_ACCENT,
                                             true);
    ui_Today_focus_btn_icon = today_button_icon_label_create(ui_Today_focus_btn);
    ui_Today_focus_btn_label = today_button_text_label_create(ui_Today_focus_btn);

    ui_Today_focus_reset = today_button_create(ui_Today,
                                               FOCUS_RIGHT_BTN_X,
                                               FOCUS_RIGHT_BTN_Y,
                                               FOCUS_RIGHT_BTN_W,
                                               FOCUS_RIGHT_BTN_H,
                                               20,
                                               FOCUS_COLOR_READY_SECONDARY,
                                               FOCUS_COLOR_PAUSED_ACCENT,
                                               false);
    ui_Today_focus_reset_icon = today_button_icon_label_create(ui_Today_focus_reset);
    ui_Today_focus_reset_label = today_button_text_label_create(ui_Today_focus_reset);

    // Deliberately omit pagination dots on this dense 284 x 240 page. Horizontal
    // swipe navigation remains active through ui_event_Today().
    ui_Today_scrolldots = NULL;

    ui_Today_header = NULL;
    ui_Today_title = NULL;
    ui_Today_date = NULL;
    ui_Today_scroll = NULL;
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
    ui_Today_tip_icon = NULL;
    ui_Today_tip_label = NULL;
    ui_Today_top_gear = NULL;
    ui_Today_primary_icon = ui_Today_focus_btn_icon;
    ui_Today_secondary_icon = ui_Today_focus_reset_icon;
    ui_Today_task_edit_icon = NULL;

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
