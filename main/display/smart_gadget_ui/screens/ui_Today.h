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
extern lv_obj_t * ui_Today_focus_hint;
extern lv_obj_t * ui_Today_focus_btn;
extern lv_obj_t * ui_Today_focus_btn_label;
extern lv_obj_t * ui_Today_focus_reset;
extern lv_obj_t * ui_Today_focus_reset_label;

void ui_Today_screen_init(void);
void ui_Today_screen_destroy(void);
void ui_event_Today(lv_event_t * e);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
