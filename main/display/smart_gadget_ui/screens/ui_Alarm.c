#include "../ui.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

LV_FONT_DECLARE(font_puhui_14_1);
LV_FONT_DECLARE(ui_font_FocusDigits);

#define ALARM_UI_WIDTH 284
#define ALARM_UI_HEIGHT 240
#define ALARM_ITEM_COUNT 3
#define ALARM_COLOR_DEEP_GREEN 0x285B2A
#define ALARM_COLOR_LEAF_GREEN 0x6BA34B
#define ALARM_COLOR_SUBTEXT 0x64A044

lv_obj_t * ui_Alarm = NULL;
lv_obj_t * ui_Alarm_container = NULL;
lv_obj_t * ui_Set_alarm = NULL;
lv_obj_t * ui_Alarm_Comp = NULL;
lv_obj_t * ui_Alarm_Comp1 = NULL;
lv_obj_t * ui_Alarm_Comp2 = NULL;
lv_obj_t * ui_Alarm_Comp3 = NULL;
lv_obj_t * ui_Scrolldots5 = NULL;

typedef struct {
    char time_text[8];
    char label[32];
    char repeat_text[16];
    bool enabled;
} alarm_item_state_t;

static alarm_item_state_t s_items[ALARM_ITEM_COUNT] = {
    {"7:00", "Wake Up", "每天", true},
    {"8:00", "Breakfast", "工作日", false},
    {"9:30", "Focus", "每天", true},
};

static const int16_t s_row_y[ALARM_ITEM_COUNT] = {81, 134, 184};
static const int16_t s_card_y[ALARM_ITEM_COUNT] = {71, 125, 177};
static const int16_t s_card_h[ALARM_ITEM_COUNT] = {50, 48, 45};

static lv_obj_t * s_shell = NULL;
static lv_obj_t * s_next_text = NULL;
static lv_obj_t * s_time[ALARM_ITEM_COUNT];
static lv_obj_t * s_label[ALARM_ITEM_COUNT];
static lv_obj_t * s_repeat[ALARM_ITEM_COUNT];
static lv_obj_t * s_toggle[ALARM_ITEM_COUNT];
static lv_obj_t * s_card_zone[ALARM_ITEM_COUNT];
static lv_image_dsc_t s_shell_image;
static lv_image_dsc_t s_toggle_on_image;
static lv_image_dsc_t s_toggle_off_image;
static bool s_assets_loaded = false;

#ifdef _WIN32
static int load_alarm_asset(const char * name, const uint8_t ** data, size_t * size)
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
static int load_alarm_asset(const char * name, const uint8_t ** data, size_t * size)
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
    if(!load_alarm_asset(name, &data, &size) || size != expected) return 0;

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
    if(s_assets_loaded) return;
    s_assets_loaded = init_descriptor(&s_shell_image, "alarm_shell.r565", 284, 240,
                                      LV_COLOR_FORMAT_RGB565) &&
                      init_descriptor(&s_toggle_on_image, "alarm_on.r5a8", 50, 30,
                                      LV_COLOR_FORMAT_RGB565A8) &&
                      init_descriptor(&s_toggle_off_image, "alarm_off.r5a8", 50, 30,
                                      LV_COLOR_FORMAT_RGB565A8);
}

static lv_obj_t * make_label(lv_obj_t * parent, int x, int y, int width,
                             const char * text, const lv_font_t * font, uint32_t color)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_obj_set_pos(label, x, y);
    lv_obj_set_width(label, width);
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return label;
}

static void render_next_enabled(void)
{
    unsigned i;
    if(s_next_text == NULL) return;
    for(i = 0; i < ALARM_ITEM_COUNT; ++i) {
        if(s_items[i].enabled) {
            lv_label_set_text_fmt(s_next_text, "%s · %s", s_items[i].time_text, s_items[i].label);
            return;
        }
    }
    lv_label_set_text(s_next_text, "--:-- · 暂无提醒");
}

static void render_item(unsigned index)
{
    if(index >= ALARM_ITEM_COUNT || s_time[index] == NULL) return;
    lv_label_set_text(s_time[index], s_items[index].time_text);
    lv_label_set_text(s_label[index], s_items[index].label);
    lv_label_set_text(s_repeat[index], s_items[index].repeat_text);
    lv_image_set_src(s_toggle[index], s_items[index].enabled ?
                     &s_toggle_on_image : &s_toggle_off_image);
}

static void set_image_scale(void * object, int32_t value)
{
    lv_image_set_scale((lv_obj_t *)object, (uint32_t)value);
}

static void toggle_clicked(lv_event_t * event)
{
    unsigned index = (unsigned)(uintptr_t)lv_event_get_user_data(event);
    lv_anim_t animation;
    if(index >= ALARM_ITEM_COUNT) return;

    s_items[index].enabled = !s_items[index].enabled;
    render_item(index);
    render_next_enabled();

    lv_anim_init(&animation);
    lv_anim_set_var(&animation, s_toggle[index]);
    lv_anim_set_values(&animation, 232, 256);
    lv_anim_set_duration(&animation, 150);
    lv_anim_set_path_cb(&animation, lv_anim_path_ease_out);
    lv_anim_set_exec_cb(&animation, set_image_scale);
    lv_anim_start(&animation);
}

static void fade_next_text(void * object, int32_t value)
{
    lv_obj_set_style_opa((lv_obj_t *)object, (lv_opa_t)value, 0);
}

void ui_event_Alarm(lv_event_t * event)
{
    if(lv_event_get_code(event) == LV_EVENT_SCREEN_LOADED && s_next_text != NULL) {
        lv_anim_t animation;
        lv_anim_init(&animation);
        lv_anim_set_var(&animation, s_next_text);
        lv_anim_set_values(&animation, LV_OPA_TRANSP, LV_OPA_COVER);
        lv_anim_set_duration(&animation, 160);
        lv_anim_set_exec_cb(&animation, fade_next_text);
        lv_anim_start(&animation);
    }
}

void ui_Alarm_screen_init(void)
{
    unsigned i;
    load_descriptors();

    ui_Alarm = lv_obj_create(NULL);
    lv_obj_remove_style_all(ui_Alarm);
    lv_obj_set_size(ui_Alarm, ALARM_UI_WIDTH, ALARM_UI_HEIGHT);
    lv_obj_set_style_bg_color(ui_Alarm, lv_color_hex(0xF7F5DF), 0);
    lv_obj_set_style_bg_opa(ui_Alarm, LV_OPA_COVER, 0);
    lv_obj_remove_flag(ui_Alarm, LV_OBJ_FLAG_SCROLLABLE);

    ui_Alarm_container = ui_Alarm;
    s_shell = lv_image_create(ui_Alarm);
    if(s_assets_loaded) lv_image_set_src(s_shell, &s_shell_image);
    lv_obj_set_pos(s_shell, 0, 0);
    lv_obj_remove_flag(s_shell, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(s_shell, LV_OBJ_FLAG_GESTURE_BUBBLE);

    for(i = 0; i < ALARM_ITEM_COUNT; ++i) {
        s_card_zone[i] = lv_obj_create(ui_Alarm);
        lv_obj_set_pos(s_card_zone[i], 10, s_card_y[i]);
        lv_obj_set_size(s_card_zone[i], 263, s_card_h[i]);
        lv_obj_set_style_bg_opa(s_card_zone[i], LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(s_card_zone[i], 0, 0);
        lv_obj_set_style_pad_all(s_card_zone[i], 0, 0);
        lv_obj_remove_flag(s_card_zone[i], LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(s_card_zone[i], LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_GESTURE_BUBBLE);

        s_time[i] = make_label(ui_Alarm, 31, s_row_y[i] - 3, 94, "--:--",
                               &ui_font_FocusDigits, ALARM_COLOR_LEAF_GREEN);
        s_label[i] = make_label(ui_Alarm, 132, s_row_y[i], 75, "",
                                &font_puhui_14_1, ALARM_COLOR_DEEP_GREEN);
        s_repeat[i] = make_label(ui_Alarm, 143, s_row_y[i] + 24, 62, "",
                                 &font_puhui_14_1, ALARM_COLOR_SUBTEXT);

        s_toggle[i] = lv_image_create(ui_Alarm);
        lv_obj_set_pos(s_toggle[i], 211, s_row_y[i] + 5);
        lv_obj_add_flag(s_toggle[i], LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_GESTURE_BUBBLE);
        lv_obj_remove_flag(s_toggle[i], LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_event_cb(s_toggle[i], toggle_clicked, LV_EVENT_SHORT_CLICKED,
                            (void *)(uintptr_t)i);
    }

    s_next_text = make_label(ui_Alarm, 103, 43, 160, "", &font_puhui_14_1,
                             ALARM_COLOR_DEEP_GREEN);
    for(i = 0; i < ALARM_ITEM_COUNT; ++i) render_item(i);
    render_next_enabled();

    ui_Set_alarm = s_next_text;
    ui_Alarm_Comp = s_card_zone[0];
    ui_Alarm_Comp1 = s_card_zone[1];
    ui_Alarm_Comp2 = s_card_zone[2];
    ui_Alarm_Comp3 = NULL;
    ui_Scrolldots5 = NULL;

    lv_obj_add_event_cb(ui_Alarm, ui_event_Alarm, LV_EVENT_ALL, NULL);
    smart_gadget_ui_screen_created(ui_Alarm);
}

void ui_alarm_product_set_next(const alarm_ui_item_t * item)
{
    if(item == NULL || s_next_text == NULL) return;
    lv_label_set_text_fmt(s_next_text, "%s · %s",
                          item->time_text != NULL ? item->time_text : "--:--",
                          item->label != NULL ? item->label : "");
}

void ui_alarm_product_set_item(unsigned index, const alarm_ui_item_t * item)
{
    if(index >= ALARM_ITEM_COUNT || item == NULL) return;
    lv_snprintf(s_items[index].time_text, sizeof(s_items[index].time_text), "%s",
                item->time_text != NULL ? item->time_text : "--:--");
    lv_snprintf(s_items[index].label, sizeof(s_items[index].label), "%s",
                item->label != NULL ? item->label : "");
    lv_snprintf(s_items[index].repeat_text, sizeof(s_items[index].repeat_text), "%s",
                item->repeat_text != NULL ? item->repeat_text : "");
    s_items[index].enabled = item->enabled;
    render_item(index);
    render_next_enabled();
}

void ui_Alarm_screen_destroy(void)
{
    if(ui_Alarm != NULL) lv_obj_delete(ui_Alarm);
    ui_Alarm = NULL;
    ui_Alarm_container = NULL;
    ui_Set_alarm = NULL;
    ui_Alarm_Comp = NULL;
    ui_Alarm_Comp1 = NULL;
    ui_Alarm_Comp2 = NULL;
    ui_Alarm_Comp3 = NULL;
    ui_Scrolldots5 = NULL;
    s_shell = NULL;
    s_next_text = NULL;
    memset(s_time, 0, sizeof(s_time));
    memset(s_label, 0, sizeof(s_label));
    memset(s_repeat, 0, sizeof(s_repeat));
    memset(s_toggle, 0, sizeof(s_toggle));
    memset(s_card_zone, 0, sizeof(s_card_zone));
}
