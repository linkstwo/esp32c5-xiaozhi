// This file follows the SquareLine Studio screen style used by Smart_Gadget.

#ifndef UI_DEVICE_H
#define UI_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

// SCREEN: ui_Device
extern void ui_Device_screen_init(void);
extern void ui_Device_screen_destroy(void);
extern void ui_event_Device(lv_event_t * e);
extern int ui_Device_is_detail_visible(void);
extern void ui_Device_return_to_overview(void);
extern lv_obj_t * ui_Device;
extern lv_obj_t * ui_Device_header;
extern lv_obj_t * ui_Device_title;
extern lv_obj_t * ui_Device_subtitle;
extern lv_obj_t * ui_Device_status_dot;
extern lv_obj_t * ui_Device_status;
extern lv_obj_t * ui_Device_scroll;
extern lv_obj_t * ui_Device_connection_panel;
extern lv_obj_t * ui_Device_wifi;
extern lv_obj_t * ui_Device_server;
extern lv_obj_t * ui_Device_protocol;
extern lv_obj_t * ui_Device_conversation_panel;
extern lv_obj_t * ui_Device_chat_state;
extern lv_obj_t * ui_Device_user;
extern lv_obj_t * ui_Device_ai;
extern lv_obj_t * ui_Device_sensor_panel;
extern lv_obj_t * ui_Device_sensor_state;
extern lv_obj_t * ui_Device_sensor_iaq;
extern lv_obj_t * ui_Device_sensor_env;
extern lv_obj_t * ui_Device_audio_panel;
extern lv_obj_t * ui_Device_mic;
extern lv_obj_t * ui_Device_speaker;
extern lv_obj_t * ui_Device_decoder;
extern lv_obj_t * ui_Device_system_panel;
extern lv_obj_t * ui_Device_board;
extern lv_obj_t * ui_Device_heap;
extern lv_obj_t * ui_Device_uptime;
extern lv_obj_t * ui_Device_page;
extern lv_obj_t * ui_Device_health_title;
extern lv_obj_t * ui_Device_health_subtitle;
extern lv_obj_t * ui_Device_temperature;
extern lv_obj_t * ui_Device_humidity;
extern lv_obj_t * ui_Device_pressure;
extern lv_obj_t * ui_Device_co2;
extern lv_obj_t * ui_Device_sensor_accuracy;
extern lv_obj_t * ui_Device_overview_wifi;
extern lv_obj_t * ui_Device_overview_sensor;
extern lv_obj_t * ui_Device_overview_audio;
extern lv_obj_t * ui_Device_overview_heap;
extern lv_obj_t * ui_Device_overview_uptime;

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
