#ifndef UI_TODAY_H
#define UI_TODAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"

extern lv_obj_t * ui_Today;
extern lv_obj_t * ui_Today_header;
extern lv_obj_t * ui_Today_title;
extern lv_obj_t * ui_Today_date;
extern lv_obj_t * ui_Today_scroll;
extern lv_obj_t * ui_Today_timer_arc;
extern lv_obj_t * ui_Today_status_panel;
extern lv_obj_t * ui_Today_status;
extern lv_obj_t * ui_Today_status_detail;
extern lv_obj_t * ui_Today_env_panel;
extern lv_obj_t * ui_Today_env_temp;
extern lv_obj_t * ui_Today_env_humidity;
extern lv_obj_t * ui_Today_env_air;
extern lv_obj_t * ui_Today_countdown_panel;
extern lv_obj_t * ui_Today_exam;
extern lv_obj_t * ui_Today_project;
extern lv_obj_t * ui_Today_focus_panel;
extern lv_obj_t * ui_Today_focus_time;
extern lv_obj_t * ui_Today_focus_unit;
extern lv_obj_t * ui_Today_focus_hint;
extern lv_obj_t * ui_Today_duration_left;
extern lv_obj_t * ui_Today_left_btn_icon;
extern lv_obj_t * ui_Today_left_btn_label;
extern lv_obj_t * ui_Today_duration_right;
extern lv_obj_t * ui_Today_task_panel;
extern lv_obj_t * ui_Today_task_label;
extern lv_obj_t * ui_Today_focus_btn;
extern lv_obj_t * ui_Today_focus_btn_icon;
extern lv_obj_t * ui_Today_focus_btn_label;
extern lv_obj_t * ui_Today_focus_reset;
extern lv_obj_t * ui_Today_focus_reset_icon;
extern lv_obj_t * ui_Today_focus_reset_label;
extern lv_obj_t * ui_Today_tip_panel;
extern lv_obj_t * ui_Today_tip_icon;
extern lv_obj_t * ui_Today_tip_label;
extern lv_obj_t * ui_Today_mascot_front;
extern lv_obj_t * ui_Today_mascot_back;
extern lv_obj_t * ui_Today_left_decor;
extern lv_obj_t * ui_Today_corner_vine;
extern lv_obj_t * ui_Today_top_back;
extern lv_obj_t * ui_Today_top_gear;
extern lv_obj_t * ui_Today_scrolldots;
extern lv_obj_t * ui_Today_state_panel;
extern lv_obj_t * ui_Today_state_label;
extern lv_obj_t * ui_Today_primary_icon;
extern lv_obj_t * ui_Today_secondary_icon;
extern lv_obj_t * ui_Today_task_edit_icon;
extern lv_obj_t * ui_Today_center_icon;

void ui_Today_screen_init(void);
void ui_Today_screen_destroy(void);
void ui_event_Today(lv_event_t * e);
void ui_Today_set_focus_visual_state(uint8_t state);
void ui_Today_set_focus_orbit_value(int32_t value);
void ui_Today_refresh_focus_button_layout(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
