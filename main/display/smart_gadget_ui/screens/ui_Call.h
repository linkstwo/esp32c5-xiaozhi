#ifndef UI_CALL_H
#define UI_CALL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UI_CALL_IDLE = 0,
    UI_CALL_CONNECTING,
    UI_CALL_LISTENING,
    UI_CALL_THINKING,
    UI_CALL_SPEAKING,
    UI_CALL_ENDED,
    UI_CALL_STATE_COUNT
} ui_call_state_t;

typedef enum {
    UI_CALL_CAPTION_SYSTEM = 0,
    UI_CALL_CAPTION_USER,
    UI_CALL_CAPTION_ASSISTANT
} ui_call_caption_role_t;

void ui_Call_screen_init(void);
void ui_Call_screen_destroy(void);
void ui_event_Call(lv_event_t * e);

void ui_call_set_state(ui_call_state_t state);
ui_call_state_t ui_call_get_state(void);
void ui_call_set_caption(ui_call_caption_role_t role, const char * utf8_text);
void ui_call_set_emotion(const char * emotion);
void ui_call_set_input_level(uint8_t level);
void ui_call_set_output_level(uint8_t level);
void ui_call_set_screen_active(uint8_t active);

extern lv_obj_t * ui_Call;
extern lv_obj_t * ui_Call_StateImage;
extern lv_obj_t * ui_Call_CaptionCover;
extern lv_obj_t * ui_Call_CaptionLabel;
extern lv_obj_t * ui_Call_Effect;
extern lv_obj_t * ui_Call_Emotion;
extern lv_obj_t * ui_Call_LeftControl;
extern lv_obj_t * ui_Call_PrimaryControl;
extern lv_obj_t * ui_Call_RightControl;
extern lv_obj_t * ui_Elena;
extern lv_obj_t * ui_Incoming;

#ifdef __cplusplus
}
#endif

#endif
