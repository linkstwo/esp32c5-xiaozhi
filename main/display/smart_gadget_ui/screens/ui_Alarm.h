#ifndef UI_ALARM_H
#define UI_ALARM_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char * time_text;
    const char * label;
    const char * repeat_text;
    bool enabled;
} alarm_ui_item_t;

void ui_Alarm_screen_init(void);
void ui_Alarm_screen_destroy(void);
void ui_event_Alarm(lv_event_t * event);
void ui_alarm_product_set_next(const alarm_ui_item_t * item);
void ui_alarm_product_set_item(unsigned index, const alarm_ui_item_t * item);

extern lv_obj_t * ui_Alarm;
extern lv_obj_t * ui_Alarm_container;
extern lv_obj_t * ui_Set_alarm;
extern lv_obj_t * ui_Alarm_Comp;
extern lv_obj_t * ui_Alarm_Comp1;
extern lv_obj_t * ui_Alarm_Comp2;
extern lv_obj_t * ui_Alarm_Comp3;
extern lv_obj_t * ui_Scrolldots5;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
