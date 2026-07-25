#include "../ui.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

LV_FONT_DECLARE(font_puhui_14_1);

#define CALL_WIDTH 284
#define CALL_HEIGHT 240
#define CALL_FRAME_COUNT 8
#define CALL_ANIM_PERIOD_MS 80

lv_obj_t * ui_Call = NULL;
lv_obj_t * ui_Call_StateImage = NULL;
lv_obj_t * ui_Call_CaptionCover = NULL;
lv_obj_t * ui_Call_CaptionLabel = NULL;
lv_obj_t * ui_Call_Effect = NULL;
lv_obj_t * ui_Call_Emotion = NULL;
lv_obj_t * ui_Call_LeftControl = NULL;
lv_obj_t * ui_Call_PrimaryControl = NULL;
lv_obj_t * ui_Call_RightControl = NULL;

/* Compatibility aliases used by the desktop preview content hook. */
lv_obj_t * ui_Elena = NULL;
lv_obj_t * ui_Incoming = NULL;
lv_obj_t * ui_Call_Incon1 = NULL;
lv_obj_t * ui_Phone1 = NULL;
lv_obj_t * ui_Call_Incon2 = NULL;
lv_obj_t * ui_Phone2 = NULL;
lv_obj_t * ui_Avatar = NULL;
lv_obj_t * ui_Scrolldots1 = NULL;

static lv_obj_t * s_effect_right = NULL;
static lv_timer_t * s_anim_timer = NULL;
static ui_call_state_t s_state = UI_CALL_IDLE;
static uint8_t s_frame = 0;
static uint8_t s_screen_active = 0;
static uint8_t s_emotion_ticks = 0;
static uint8_t s_input_level = 50;
static uint8_t s_output_level = 50;

static lv_image_dsc_t s_state_images[UI_CALL_STATE_COUNT];
static lv_image_dsc_t s_caption_image;
static lv_image_dsc_t s_emotion_images[7];
static lv_image_dsc_t s_effect_images[4][CALL_FRAME_COUNT];

static const char * const s_state_files[UI_CALL_STATE_COUNT] = {
    "call_idle.r565", "call_connecting.r565", "call_listening.r565",
    "call_thinking.r565", "call_speaking.r565", "call_ended.r565"
};

static const char * const s_emotion_files[7] = {
    "call_em_spark.r5a8", "call_em_heart.r5a8", "call_em_question.r5a8",
    "call_em_exclaim.r5a8", "call_em_tear.r5a8", "call_em_angry.r5a8",
    "call_em_zzz.r5a8"
};

static const char s_effect_codes[4] = {'c', 'i', 't', 'o'};
static const uint16_t s_effect_widths[4] = {24, 64, 72, 64};
static const uint16_t s_effect_heights[4] = {24, 22, 72, 22};

#ifdef _WIN32
static int load_call_asset(const char * name, const uint8_t ** data, size_t * size)
{
    char source_path[512];
    char asset_path[640];
    FILE * file;
    long file_size;
    uint8_t * buffer;
    char * slash;

    snprintf(source_path, sizeof(source_path), "%s", __FILE__);
    slash = strrchr(source_path, '/');
    if(slash == NULL) slash = strrchr(source_path, '\\');
    if(slash == NULL) return 0;
    *slash = '\0';
    snprintf(asset_path, sizeof(asset_path), "%s/../../../assets/call_ui_v3/%s", source_path, name);

    file = fopen(asset_path, "rb");
    if(file == NULL) return 0;
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);
    if(file_size <= 0) {
        fclose(file);
        return 0;
    }
    buffer = (uint8_t *)malloc((size_t)file_size);
    if(buffer == NULL || fread(buffer, 1, (size_t)file_size, file) != (size_t)file_size) {
        free(buffer);
        fclose(file);
        return 0;
    }
    fclose(file);
    *data = buffer;
    *size = (size_t)file_size;
    return 1;
}
#else
extern int smart_gadget_load_call_asset(const char * name, const uint8_t ** data, size_t * size);
static int load_call_asset(const char * name, const uint8_t ** data, size_t * size)
{
    return smart_gadget_load_call_asset(name, data, size);
}
#endif

static int init_descriptor(lv_image_dsc_t * descriptor, const char * name,
                           uint16_t width, uint16_t height, lv_color_format_t format)
{
    const uint8_t * data = NULL;
    size_t size = 0;
    size_t expected = (size_t)width * height * (format == LV_COLOR_FORMAT_RGB565 ? 2U : 3U);
    if(!load_call_asset(name, &data, &size) || size != expected) return 0;

    memset(descriptor, 0, sizeof(*descriptor));
    descriptor->header.magic = LV_IMAGE_HEADER_MAGIC;
    descriptor->header.cf = format;
    descriptor->header.w = width;
    descriptor->header.h = height;
    descriptor->header.stride = width * 2U;
    descriptor->data_size = (uint32_t)size;
    descriptor->data = data;
    return 1;
}

static void load_descriptors(void)
{
    unsigned state;
    unsigned emotion;
    unsigned group;
    unsigned frame;
    char name[32];

    for(state = 0; state < UI_CALL_STATE_COUNT; ++state) {
        init_descriptor(&s_state_images[state], s_state_files[state], CALL_WIDTH, CALL_HEIGHT,
                        LV_COLOR_FORMAT_RGB565);
    }
    init_descriptor(&s_caption_image, "call_caption.r5a8", 149, 30, LV_COLOR_FORMAT_RGB565A8);
    for(emotion = 0; emotion < 7; ++emotion) {
        init_descriptor(&s_emotion_images[emotion], s_emotion_files[emotion], 36, 36,
                        LV_COLOR_FORMAT_RGB565A8);
    }
    for(group = 0; group < 4; ++group) {
        for(frame = 0; frame < CALL_FRAME_COUNT; ++frame) {
            snprintf(name, sizeof(name), "call_fx_%c_%02u.r5a8", s_effect_codes[group], frame + 1);
            init_descriptor(&s_effect_images[group][frame], name,
                            s_effect_widths[group], s_effect_heights[group],
                            LV_COLOR_FORMAT_RGB565A8);
        }
    }
}

static lv_obj_t * create_image(lv_obj_t * parent)
{
    lv_obj_t * image = lv_image_create(parent);
    lv_obj_remove_flag(image, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(image, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return image;
}

static lv_obj_t * create_touch_zone(lv_obj_t * parent, int x, int y, int width, int height)
{
    lv_obj_t * zone = lv_obj_create(parent);
    lv_obj_set_pos(zone, x, y);
    lv_obj_set_size(zone, width, height);
    lv_obj_set_style_bg_opa(zone, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(zone, 0, 0);
    lv_obj_set_style_pad_all(zone, 0, 0);
    lv_obj_remove_flag(zone, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(zone, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return zone;
}

static int effect_group_for_state(ui_call_state_t state)
{
    if(state == UI_CALL_CONNECTING) return 0;
    if(state == UI_CALL_LISTENING) return 1;
    if(state == UI_CALL_THINKING) return 2;
    if(state == UI_CALL_SPEAKING) return 3;
    return -1;
}

static void position_effects(int group)
{
    if(group == 0) {
        lv_obj_set_pos(ui_Call_Effect, 205, 7);
        lv_obj_add_flag(s_effect_right, LV_OBJ_FLAG_HIDDEN);
    } else if(group == 2) {
        lv_obj_set_pos(ui_Call_Effect, 106, 70);
        lv_obj_add_flag(s_effect_right, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_set_pos(ui_Call_Effect, 10, 101);
        lv_obj_set_pos(s_effect_right, 210, 101);
        lv_obj_clear_flag(s_effect_right, LV_OBJ_FLAG_HIDDEN);
    }
}

static void render_effect_frame(void)
{
    int group = effect_group_for_state(s_state);
    uint8_t frame = s_frame;
    if(group < 0 || ui_Call_Effect == NULL) {
        if(ui_Call_Effect != NULL) lv_obj_add_flag(ui_Call_Effect, LV_OBJ_FLAG_HIDDEN);
        if(s_effect_right != NULL) lv_obj_add_flag(s_effect_right, LV_OBJ_FLAG_HIDDEN);
        return;
    }
    if(group == 1) frame = (uint8_t)((frame + s_input_level / 25U) % CALL_FRAME_COUNT);
    if(group == 3) frame = (uint8_t)((frame + s_output_level / 25U) % CALL_FRAME_COUNT);
    position_effects(group);
    lv_image_set_src(ui_Call_Effect, &s_effect_images[group][frame]);
    lv_obj_clear_flag(ui_Call_Effect, LV_OBJ_FLAG_HIDDEN);
    if(group == 1 || group == 3) lv_image_set_src(s_effect_right, &s_effect_images[group][frame]);
}

static void call_anim_timer_cb(lv_timer_t * timer)
{
    (void)timer;
    if(!s_screen_active) return;
    s_frame = (uint8_t)((s_frame + 1U) % CALL_FRAME_COUNT);
    render_effect_frame();
    if(s_emotion_ticks > 0 && --s_emotion_ticks == 0 && ui_Call_Emotion != NULL) {
        lv_obj_add_flag(ui_Call_Emotion, LV_OBJ_FLAG_HIDDEN);
    }
}

void ui_call_set_state(ui_call_state_t state)
{
    if(state < UI_CALL_IDLE || state >= UI_CALL_STATE_COUNT) state = UI_CALL_IDLE;
    s_state = state;
    s_frame = 0;
    if(ui_Call_StateImage != NULL && s_state_images[state].data != NULL) {
        lv_image_set_src(ui_Call_StateImage, &s_state_images[state]);
    }
    if(ui_Call_RightControl != NULL) {
        if(state == UI_CALL_SPEAKING) lv_obj_add_flag(ui_Call_RightControl, LV_OBJ_FLAG_CLICKABLE);
        else lv_obj_remove_flag(ui_Call_RightControl, LV_OBJ_FLAG_CLICKABLE);
    }
    render_effect_frame();
}

ui_call_state_t ui_call_get_state(void)
{
    return s_state;
}

void ui_call_set_caption(ui_call_caption_role_t role, const char * utf8_text)
{
    uint32_t color = role == UI_CALL_CAPTION_USER ? 0x425C27 : 0x36551E;
    if(ui_Call_CaptionCover == NULL || ui_Call_CaptionLabel == NULL) return;
    if(utf8_text == NULL || utf8_text[0] == '\0') {
        lv_obj_add_flag(ui_Call_CaptionCover, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_Call_CaptionLabel, LV_OBJ_FLAG_HIDDEN);
        return;
    }
    lv_label_set_text(ui_Call_CaptionLabel, utf8_text);
    lv_obj_set_style_text_color(ui_Call_CaptionLabel, lv_color_hex(color), 0);
    lv_obj_clear_flag(ui_Call_CaptionCover, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_Call_CaptionLabel, LV_OBJ_FLAG_HIDDEN);
}

static int emotion_index(const char * emotion)
{
    if(emotion == NULL || strcmp(emotion, "neutral") == 0) return -1;
    if(strcmp(emotion, "loving") == 0 || strcmp(emotion, "kissy") == 0 ||
       strcmp(emotion, "delicious") == 0) return 1;
    if(strcmp(emotion, "thinking") == 0 || strcmp(emotion, "confused") == 0) return 2;
    if(strcmp(emotion, "surprised") == 0 || strcmp(emotion, "shocked") == 0 ||
       strcmp(emotion, "embarrassed") == 0) return 3;
    if(strcmp(emotion, "sad") == 0 || strcmp(emotion, "crying") == 0) return 4;
    if(strcmp(emotion, "angry") == 0) return 5;
    if(strcmp(emotion, "sleepy") == 0 || strcmp(emotion, "relaxed") == 0) return 6;
    return 0;
}

void ui_call_set_emotion(const char * emotion)
{
    int index = emotion_index(emotion);
    if(ui_Call_Emotion == NULL) return;
    if(index < 0 || s_emotion_images[index].data == NULL) {
        s_emotion_ticks = 0;
        lv_obj_add_flag(ui_Call_Emotion, LV_OBJ_FLAG_HIDDEN);
        return;
    }
    lv_image_set_src(ui_Call_Emotion, &s_emotion_images[index]);
    lv_obj_clear_flag(ui_Call_Emotion, LV_OBJ_FLAG_HIDDEN);
    s_emotion_ticks = 10;
}

void ui_call_set_input_level(uint8_t level) { s_input_level = level > 100 ? 100 : level; }
void ui_call_set_output_level(uint8_t level) { s_output_level = level > 100 ? 100 : level; }

void ui_call_set_screen_active(uint8_t active)
{
    s_screen_active = active != 0;
    if(s_anim_timer == NULL) return;
    if(s_screen_active) {
        lv_timer_resume(s_anim_timer);
        render_effect_frame();
    } else {
        lv_timer_pause(s_anim_timer);
    }
}

void ui_event_Call(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SCREEN_LOADED) ui_call_set_screen_active(1);
    else if(code == LV_EVENT_SCREEN_UNLOADED) ui_call_set_screen_active(0);
}

void ui_Call_screen_init(void)
{
    load_descriptors();
    ui_Call = lv_obj_create(NULL);
    lv_obj_remove_flag(ui_Call, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Call, lv_color_hex(0xFFFDF2), 0);
    lv_obj_set_style_bg_opa(ui_Call, LV_OPA_COVER, 0);

    ui_Call_StateImage = create_image(ui_Call);
    lv_obj_set_pos(ui_Call_StateImage, 0, 0);

    ui_Call_Effect = create_image(ui_Call);
    s_effect_right = create_image(ui_Call);

    ui_Call_CaptionCover = create_image(ui_Call);
    lv_image_set_src(ui_Call_CaptionCover, &s_caption_image);
    lv_obj_set_pos(ui_Call_CaptionCover, 68, 164);
    lv_obj_add_flag(ui_Call_CaptionCover, LV_OBJ_FLAG_HIDDEN);

    ui_Call_CaptionLabel = lv_label_create(ui_Call);
    lv_obj_set_pos(ui_Call_CaptionLabel, 87, 171);
    lv_obj_set_size(ui_Call_CaptionLabel, 120, 16);
    lv_label_set_long_mode(ui_Call_CaptionLabel, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_font(ui_Call_CaptionLabel, &font_puhui_14_1, 0);
    lv_obj_set_style_text_align(ui_Call_CaptionLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_all(ui_Call_CaptionLabel, 0, 0);
    lv_obj_add_flag(ui_Call_CaptionLabel, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_GESTURE_BUBBLE);

    ui_Call_Emotion = create_image(ui_Call);
    lv_obj_set_pos(ui_Call_Emotion, 205, 48);
    lv_obj_add_flag(ui_Call_Emotion, LV_OBJ_FLAG_HIDDEN);

    ui_Call_LeftControl = create_touch_zone(ui_Call, 55, 193, 52, 43);
    ui_Call_PrimaryControl = create_touch_zone(ui_Call, 99, 192, 91, 45);
    ui_Call_RightControl = create_touch_zone(ui_Call, 193, 193, 52, 43);
    lv_obj_remove_flag(ui_Call_LeftControl, LV_OBJ_FLAG_CLICKABLE);

    ui_Call_Incon1 = ui_Call_PrimaryControl;
    ui_Call_Incon2 = ui_Call_RightControl;
    lv_obj_add_event_cb(ui_Call, ui_event_Call, LV_EVENT_ALL, NULL);

    if(s_anim_timer == NULL) {
        s_anim_timer = lv_timer_create(call_anim_timer_cb, CALL_ANIM_PERIOD_MS, NULL);
        lv_timer_pause(s_anim_timer);
    }
    ui_call_set_state(s_state);
    smart_gadget_ui_screen_created(ui_Call);
}

void ui_Call_screen_destroy(void)
{
    if(s_anim_timer != NULL) {
        lv_timer_delete(s_anim_timer);
        s_anim_timer = NULL;
    }
    if(ui_Call != NULL) lv_obj_del(ui_Call);
    ui_Call = NULL;
    ui_Call_StateImage = NULL;
    ui_Call_CaptionCover = NULL;
    ui_Call_CaptionLabel = NULL;
    ui_Call_Effect = NULL;
    ui_Call_Emotion = NULL;
    ui_Call_LeftControl = NULL;
    ui_Call_PrimaryControl = NULL;
    ui_Call_RightControl = NULL;
    ui_Elena = NULL;
    ui_Incoming = NULL;
    ui_Call_Incon1 = NULL;
    ui_Phone1 = NULL;
    ui_Call_Incon2 = NULL;
    ui_Phone2 = NULL;
    ui_Avatar = NULL;
    ui_Scrolldots1 = NULL;
    s_effect_right = NULL;
}
