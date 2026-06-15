#include "smart_gadget_display.h"

#include "application.h"
#include "assets/lang_config.h"
#include "board.h"
#include "sensor_service.hpp"
#include "smart_gadget_ui/ui.h"

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_timer.h>

#define TAG "SmartGadgetDisplay"

LV_FONT_DECLARE(BUILTIN_TEXT_FONT);

SmartGadgetDisplay* g_smart_gadget_display = nullptr;

namespace {
constexpr size_t kChatPreviewMaxChars = 50;

std::string TruncateUtf8Text(const std::string& text, size_t max_chars) {
    size_t index = 0;
    size_t count = 0;
    while (index < text.size() && count < max_chars) {
        const unsigned char ch = static_cast<unsigned char>(text[index]);
        size_t char_len = 1;
        if ((ch & 0x80) == 0) {
            char_len = 1;
        } else if ((ch & 0xE0) == 0xC0) {
            char_len = 2;
        } else if ((ch & 0xF0) == 0xE0) {
            char_len = 3;
        } else if ((ch & 0xF8) == 0xF0) {
            char_len = 4;
        }
        if (index + char_len > text.size()) {
            break;
        }
        index += char_len;
        ++count;
    }

    if (index >= text.size()) {
        return text;
    }
    return text.substr(0, index) + "...";
}
}  // namespace

extern "C" void smart_gadget_ui_screen_created(lv_obj_t* screen) {
    if (g_smart_gadget_display != nullptr) {
        g_smart_gadget_display->OnSquareLineScreenCreated(screen);
    }
}

SmartGadgetDisplay::SmartGadgetDisplay(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_handle_t panel,
                                       int width, int height, int offset_x, int offset_y,
                                       bool mirror_x, bool mirror_y, bool swap_xy)
    : SpiLcdDisplay(panel_io, panel, width, height, offset_x, offset_y, mirror_x, mirror_y, swap_xy) {
    g_smart_gadget_display = this;
}

void SmartGadgetDisplay::SetObjectHidden(lv_obj_t* obj, bool hidden) {
    if (obj == nullptr) {
        return;
    }
    if (hidden) {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_remove_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

void SmartGadgetDisplay::ApplyReadableFont(lv_obj_t* obj) {
    if (obj != nullptr) {
        lv_obj_set_style_text_font(obj, &BUILTIN_TEXT_FONT, 0);
    }
}

void SmartGadgetDisplay::ApplyInitialText() {
    if (ui_Smart_Gadget != nullptr) {
        lv_label_set_text(ui_Smart_Gadget, "XiaoZhi AI");
    }
    if (ui_Demo != nullptr) {
        lv_label_set_text(ui_Demo, call_status_.c_str());
    }

    ApplyCallText();
    ApplyChatText();
    ApplyWeatherText();
    ApplyDeviceText();
}

void SmartGadgetDisplay::ApplyCallText() {
    if (ui_Elena != nullptr) {
        ApplyReadableFont(ui_Elena);
        lv_label_set_text(ui_Elena, "XiaoZhi AI");
    }
    if (ui_Incoming != nullptr) {
        ApplyReadableFont(ui_Incoming);
        lv_label_set_text(ui_Incoming, call_status_.c_str());
    }
    BindCallButtons();
}

void SmartGadgetDisplay::ApplyChatText() {
    const bool has_assistant_message = !assistant_message_.empty();
    const std::string assistant_text = has_assistant_message ?
        TruncateUtf8Text(assistant_message_, kChatPreviewMaxChars) :
        "Hold the button and ask";

    if (ui_Chat_date != nullptr) {
        ApplyReadableFont(ui_Chat_date);
        lv_label_set_text(ui_Chat_date, "Ask XiaoZhi");
    }
    if (ui_Chat1 != nullptr) {
        ApplyReadableFont(ui_Chat1);
        lv_label_set_long_mode(ui_Chat1, LV_LABEL_LONG_WRAP);
        lv_label_set_text(ui_Chat1, assistant_text.c_str());
        lv_obj_set_width(ui_Chat1, 184);
    }
    if (ui_Chat2 != nullptr) {
        ApplyReadableFont(ui_Chat2);
        lv_label_set_long_mode(ui_Chat2, LV_LABEL_LONG_CLIP);
        lv_label_set_text(ui_Chat2, "Hold to speak");
        lv_obj_set_width(ui_Chat2, 128);
        lv_obj_set_style_text_align(ui_Chat2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    }
    if (ui_Chat3 != nullptr) {
        ApplyReadableFont(ui_Chat3);
        lv_label_set_text(ui_Chat3, "");
    }
    if (ui_Delifered != nullptr) {
        ApplyReadableFont(ui_Delifered);
        lv_label_set_text(ui_Delifered, "");
    }

    user_message_visible_ = false;
    assistant_message_visible_ = true;
    SetObjectHidden(ui_C1, false);
    SetObjectHidden(ui_C2, false);
    SetObjectHidden(ui_C3, true);
    SetObjectHidden(ui_Chat_Icon1, true);
    SetObjectHidden(ui_Chat_Icon2, true);
    SetObjectHidden(ui_Chat_Icon3, true);

    if (ui_Chat_container != nullptr) {
        lv_obj_add_flag(ui_Chat_container, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_scroll_dir(ui_Chat_container, LV_DIR_VER);
        lv_obj_set_scrollbar_mode(ui_Chat_container, LV_SCROLLBAR_MODE_AUTO);
        lv_obj_add_flag(ui_Chat_container, LV_OBJ_FLAG_GESTURE_BUBBLE);
        lv_obj_set_layout(ui_Chat_container, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(ui_Chat_container, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(ui_Chat_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
        lv_obj_set_style_pad_bottom(ui_Chat_container, 28, LV_PART_MAIN);
    }
    if (ui_C1 != nullptr && ui_Chat_Panel1 != nullptr) {
        lv_obj_set_height(ui_C1, LV_SIZE_CONTENT);
        lv_obj_set_width(ui_Chat_Panel1, 204);
        lv_obj_set_height(ui_Chat_Panel1, LV_SIZE_CONTENT);
        lv_obj_set_align(ui_Chat_Panel1, LV_ALIGN_TOP_MID);
    }
    if (ui_C2 != nullptr && ui_Chat_Panel2 != nullptr) {
        lv_obj_set_height(ui_C2, LV_SIZE_CONTENT);
        lv_obj_set_width(ui_Chat_Panel2, 152);
        lv_obj_set_height(ui_Chat_Panel2, 44);
        lv_obj_set_align(ui_Chat_Panel2, LV_ALIGN_TOP_MID);
        lv_obj_set_style_radius(ui_Chat_Panel2, 22, LV_PART_MAIN);
    }
    BindChatButton();
}

void SmartGadgetDisplay::ApplyWeatherText() {
    if (ui_Weather == nullptr) {
        return;
    }

    EnsureSensorServiceStarted();
    SensorData sensor_data = {};
    const bool has_sensor_data = SensorService::getInstance().getLatestData(sensor_data);
    char temp_text[24];
    char iaq_text[24];
    char pressure_text[24];
    char humidity_text[24];
    if (has_sensor_data) {
        snprintf(temp_text, sizeof(temp_text), "%.0f°", sensor_data.temperature);
        snprintf(iaq_text, sizeof(iaq_text), "IAQ %.0f", sensor_data.iaq);
        snprintf(pressure_text, sizeof(pressure_text), "%.0fhPa", sensor_data.pressure / 100.0f);
        snprintf(humidity_text, sizeof(humidity_text), "%.0f%%", sensor_data.humidity);
    } else {
        snprintf(temp_text, sizeof(temp_text), "--");
        snprintf(iaq_text, sizeof(iaq_text), "IAQ --");
        snprintf(pressure_text, sizeof(pressure_text), "P -- hPa");
        snprintf(humidity_text, sizeof(humidity_text), "H -- %%");
    }

    if (ui_Pary_Cloud != nullptr) {
        ApplyReadableFont(ui_Pary_Cloud);
        if (!has_sensor_data) {
            lv_label_set_text(ui_Pary_Cloud, "Sensor wait");
        } else if (sensor_data.iaq <= 50) {
            lv_label_set_text(ui_Pary_Cloud, "Excellent air");
        } else if (sensor_data.iaq <= 100) {
            lv_label_set_text(ui_Pary_Cloud, "Good air");
        } else if (sensor_data.iaq <= 150) {
            lv_label_set_text(ui_Pary_Cloud, "Moderate air");
        } else if (sensor_data.iaq <= 200) {
            lv_label_set_text(ui_Pary_Cloud, "Polluted air");
        } else {
            lv_label_set_text(ui_Pary_Cloud, "Bad air");
        }
    }
    if (ui_Celsius != nullptr) {
        lv_label_set_text(ui_Celsius, temp_text);
    }
    if (ui_W1_Num != nullptr) {
        ApplyReadableFont(ui_W1_Num);
        lv_label_set_text(ui_W1_Num, iaq_text);
    }
    if (ui_W2_Num != nullptr) {
        ApplyReadableFont(ui_W2_Num);
        lv_label_set_text(ui_W2_Num, pressure_text);
    }
    if (ui_W3_Num != nullptr) {
        ApplyReadableFont(ui_W3_Num);
        lv_label_set_text(ui_W3_Num, humidity_text);
    }
}

void SmartGadgetDisplay::ApplyClockText(const struct tm& timeinfo) {
    if (ui_Clock_Number != nullptr) {
        char time_text[16];
        strftime(time_text, sizeof(time_text), "%H:%M", &timeinfo);
        lv_label_set_text(ui_Clock_Number, time_text);
    }

    if (ui_Hour != nullptr) {
        const uint16_t hour_rotation = static_cast<uint16_t>(
            (((timeinfo.tm_hour % 12) * 3600) + (timeinfo.tm_min * 60) + timeinfo.tm_sec) / 12);
        lv_image_set_rotation(ui_Hour, hour_rotation);
    }
    if (ui_Min != nullptr) {
        const uint16_t minute_rotation = static_cast<uint16_t>((timeinfo.tm_min * 60) + timeinfo.tm_sec);
        lv_image_set_rotation(ui_Min, minute_rotation);
    }
    if (ui_Sec != nullptr) {
        const uint16_t second_rotation = static_cast<uint16_t>(timeinfo.tm_sec * 60);
        lv_image_set_rotation(ui_Sec, second_rotation);
    }
}

void SmartGadgetDisplay::ApplyDeviceText() {
    if (ui_Device == nullptr) {
        return;
    }

    EnsureSensorServiceStarted();
    const bool has_user_message = !user_message_.empty();
    const bool has_assistant_message = !assistant_message_.empty();
    SensorData sensor_data = {};
    const bool has_sensor_data = SensorService::getInstance().getLatestData(sensor_data);
    const char* status = call_status_.empty() ? "Ready" : call_status_.c_str();
    char heap_text[32];
    char uptime_text[32];
    char sensor_state[40];
    char sensor_iaq[40];
    char sensor_env[64];
    snprintf(heap_text, sizeof(heap_text), "Heap %u KB", static_cast<unsigned>(esp_get_free_heap_size() / 1024));
    snprintf(uptime_text, sizeof(uptime_text), "Up %lld s", static_cast<long long>(esp_timer_get_time() / 1000000));
    if (has_sensor_data) {
        snprintf(sensor_state, sizeof(sensor_state), "BME690 OK acc %u", sensor_data.iaq_accuracy);
        snprintf(sensor_iaq, sizeof(sensor_iaq), "IAQ %.0f CO2 %.0f", sensor_data.iaq, sensor_data.co2_equivalent);
        snprintf(sensor_env, sizeof(sensor_env), "T %.1fC H %.0f%% P %.0fhPa",
                 sensor_data.temperature, sensor_data.humidity, sensor_data.pressure / 100.0f);
    } else {
        snprintf(sensor_state, sizeof(sensor_state), "BME690 waiting");
        snprintf(sensor_iaq, sizeof(sensor_iaq), "IAQ -- CO2 --");
        snprintf(sensor_env, sizeof(sensor_env), "T --C H --%% P --hPa");
    }

    if (ui_Device_title != nullptr) {
        ApplyReadableFont(ui_Device_title);
    }
    if (ui_Device_subtitle != nullptr) {
        ApplyReadableFont(ui_Device_subtitle);
    }
    if (ui_Device_status != nullptr) {
        ApplyReadableFont(ui_Device_status);
        lv_label_set_text(ui_Device_status, status);
    }
    if (ui_Device_wifi != nullptr) {
        ApplyReadableFont(ui_Device_wifi);
        lv_label_set_text(ui_Device_wifi, "Wi-Fi linked");
    }
    if (ui_Device_server != nullptr) {
        ApplyReadableFont(ui_Device_server);
        lv_label_set_text(ui_Device_server, status);
    }
    if (ui_Device_protocol != nullptr) {
        ApplyReadableFont(ui_Device_protocol);
        lv_label_set_text(ui_Device_protocol, "Protocol realtime");
    }
    if (ui_Device_chat_state != nullptr) {
        ApplyReadableFont(ui_Device_chat_state);
        lv_label_set_text(ui_Device_chat_state, (has_user_message || has_assistant_message) ? "State chatting" : "State waiting");
    }
    if (ui_Device_user != nullptr) {
        ApplyReadableFont(ui_Device_user);
        lv_label_set_text(ui_Device_user, has_user_message ? "You message ready" : "You no message");
    }
    if (ui_Device_ai != nullptr) {
        ApplyReadableFont(ui_Device_ai);
        lv_label_set_text(ui_Device_ai, has_assistant_message ? "AI reply ready" : "AI no reply");
    }
    if (ui_Device_sensor_state != nullptr) {
        ApplyReadableFont(ui_Device_sensor_state);
        lv_label_set_text(ui_Device_sensor_state, sensor_state);
    }
    if (ui_Device_sensor_iaq != nullptr) {
        ApplyReadableFont(ui_Device_sensor_iaq);
        lv_label_set_text(ui_Device_sensor_iaq, sensor_iaq);
    }
    if (ui_Device_sensor_env != nullptr) {
        ApplyReadableFont(ui_Device_sensor_env);
        lv_label_set_text(ui_Device_sensor_env, sensor_env);
    }
    if (ui_Device_mic != nullptr) {
        ApplyReadableFont(ui_Device_mic);
    }
    if (ui_Device_speaker != nullptr) {
        ApplyReadableFont(ui_Device_speaker);
    }
    if (ui_Device_decoder != nullptr) {
        ApplyReadableFont(ui_Device_decoder);
    }
    if (ui_Device_board != nullptr) {
        ApplyReadableFont(ui_Device_board);
    }
    if (ui_Device_heap != nullptr) {
        ApplyReadableFont(ui_Device_heap);
        lv_label_set_text(ui_Device_heap, heap_text);
    }
    if (ui_Device_uptime != nullptr) {
        ApplyReadableFont(ui_Device_uptime);
        lv_label_set_text(ui_Device_uptime, uptime_text);
    }
    if (ui_Device_page != nullptr) {
        ApplyReadableFont(ui_Device_page);
    }
}

void SmartGadgetDisplay::UpdateStatusBar(bool update_all) {
    (void)update_all;
    if (!ui_ready_) {
        return;
    }

    time_t now = time(nullptr);
    struct tm timeinfo = {};
    localtime_r(&now, &timeinfo);
    if (timeinfo.tm_year < (2025 - 1900)) {
        return;
    }

    DisplayLockGuard lock(this);
    ApplyClockText(timeinfo);

    if (lv_screen_active() == ui_Weather) {
        ApplyWeatherText();
    } else if (lv_screen_active() == ui_Device) {
        ApplyDeviceText();
    }
}

void SmartGadgetDisplay::RefreshLiveData() {
    if (!ui_ready_) {
        return;
    }

    lv_obj_t* active_screen = lv_screen_active();
    if (active_screen == ui_Clock) {
        time_t now = time(nullptr);
        struct tm timeinfo = {};
        localtime_r(&now, &timeinfo);
        if (timeinfo.tm_year >= (2025 - 1900)) {
            ApplyClockText(timeinfo);
        }
    } else if (active_screen == ui_Weather) {
        ApplyWeatherText();
    } else if (active_screen == ui_Device) {
        ApplyDeviceText();
    }
}

void SmartGadgetDisplay::LiveDataTimerCallback(lv_timer_t* timer) {
    auto* display = static_cast<SmartGadgetDisplay*>(lv_timer_get_user_data(timer));
    if (display != nullptr) {
        display->RefreshLiveData();
    }
}

static void smart_gadget_hangup_event_cb(lv_event_t* e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        Application::GetInstance().HangupChat();
    }
}

static void smart_gadget_answer_event_cb(lv_event_t* e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        Application::GetInstance().StartListening();
    }
}

static void smart_gadget_chat_speak_event_cb(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED) {
        if (ui_Chat2 != nullptr) {
            lv_label_set_text(ui_Chat2, "Listening...");
        }
        auto& app = Application::GetInstance();
        if (app.GetDeviceState() == kDeviceStateIdle) {
            app.ToggleChatState();
        }
    } else if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        if (ui_Chat2 != nullptr) {
            lv_label_set_text(ui_Chat2, "Hold to speak");
        }
        auto& app = Application::GetInstance();
        auto state = app.GetDeviceState();
        if (state == kDeviceStateListening) {
            app.StopListening();
        } else if (state == kDeviceStateConnecting) {
            app.HangupChat();
        }
    }
}

void SmartGadgetDisplay::BindCallButtons() {
    if (call_buttons_bound_ || ui_Call_Incon1 == nullptr || ui_Call_Incon2 == nullptr) {
        return;
    }

    lv_obj_add_flag(ui_Call_Incon1, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(ui_Call_Incon1, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_event_cb(ui_Call_Incon1, smart_gadget_hangup_event_cb, LV_EVENT_CLICKED, nullptr);

    lv_obj_add_flag(ui_Call_Incon2, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(ui_Call_Incon2, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_event_cb(ui_Call_Incon2, smart_gadget_answer_event_cb, LV_EVENT_CLICKED, nullptr);

    call_buttons_bound_ = true;
}

void SmartGadgetDisplay::BindChatButton() {
    if (chat_button_bound_ || ui_Chat_Panel2 == nullptr) {
        return;
    }

    lv_obj_add_flag(ui_Chat_Panel2, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(ui_Chat_Panel2, smart_gadget_chat_speak_event_cb, LV_EVENT_PRESSED, nullptr);
    lv_obj_add_event_cb(ui_Chat_Panel2, smart_gadget_chat_speak_event_cb, LV_EVENT_RELEASED, nullptr);
    lv_obj_add_event_cb(ui_Chat_Panel2, smart_gadget_chat_speak_event_cb, LV_EVENT_PRESS_LOST, nullptr);

    chat_button_bound_ = true;
}

void SmartGadgetDisplay::OnSquareLineScreenCreated(lv_obj_t* screen) {
    if (screen == ui_Clock) {
        ConfigureScreenLifecycle(screen, kPageClock);
    } else if (screen == ui_Call) {
        ConfigureScreenLifecycle(screen, kPageCall);
        ApplyCallText();
    } else if (screen == ui_Chat) {
        ConfigureScreenLifecycle(screen, kPageChat);
        ApplyChatText();
    } else if (screen == ui_Music_Player) {
        ConfigureScreenLifecycle(screen, kPageMusic);
    } else if (screen == ui_Weather) {
        ConfigureScreenLifecycle(screen, kPageWeather);
        ApplyWeatherText();
    } else if (screen == ui_Alarm) {
        ConfigureScreenLifecycle(screen, kPageAlarm);
    } else if (screen == ui_Device) {
        ConfigureScreenLifecycle(screen, kPageDevice);
        ApplyDeviceText();
    } else if (screen == ui_Splash) {
        ConfigureScreenLifecycle(screen, kPageSplash);
        ApplyInitialText();
    }
}

void SmartGadgetDisplay::SetupUI() {
    if (setup_ui_called_) {
        ESP_LOGW(TAG, "SetupUI() called multiple times, skipping duplicate call");
        return;
    }

    Display::SetupUI();
    DisplayLockGuard lock(this);
    ui_init();
    ApplyInitialText();
    ui_ready_ = true;
    current_page_ = kPageSplash;
    if (live_data_timer_ == nullptr) {
        live_data_timer_ = lv_timer_create(SmartGadgetDisplay::LiveDataTimerCallback, 3000, this);
    }
}

void SmartGadgetDisplay::UpdateCallStatus(const char* status) {
    const char* text = status != nullptr ? status : "";
    call_status_ = text;
    if (ui_Incoming != nullptr) {
        ApplyReadableFont(ui_Incoming);
        lv_label_set_text(ui_Incoming, text);
    }
    if (ui_Demo != nullptr) {
        lv_label_set_text(ui_Demo, text);
    }
    ApplyWeatherText();
    ApplyDeviceText();
}

void SmartGadgetDisplay::EnsureSensorServiceStarted() {
#if CONFIG_BOARD_TYPE_ESP_SENSAIRSHUTTLE
    auto& sensor_service = SensorService::getInstance();
    if (sensor_service.isInitialized()) {
        return;
    }

    esp_err_t sensor_ret = sensor_service.init();
    if (sensor_ret != ESP_OK) {
        ESP_LOGW(TAG, "BME690/BSEC sensor service lazy init failed: %s", esp_err_to_name(sensor_ret));
    }
#endif
}

void SmartGadgetDisplay::ConfigureScreenLifecycle(lv_obj_t* screen, PageIndex page) {
    if (screen == nullptr) {
        return;
    }

    lv_obj_add_event_cb(screen, ScreenLoadedEventCallback, LV_EVENT_SCREEN_LOADED,
                        reinterpret_cast<void*>(static_cast<intptr_t>(page)));
}

void SmartGadgetDisplay::HandleScreenLoaded(PageIndex page) {
    current_page_ = page;
    if (page == kPageWeather || page == kPageDevice) {
        EnsureSensorServiceStarted();
    }
}

void SmartGadgetDisplay::ScreenLoadedEventCallback(lv_event_t* e) {
    if (g_smart_gadget_display == nullptr) {
        return;
    }

    auto page = static_cast<PageIndex>(reinterpret_cast<intptr_t>(lv_event_get_user_data(e)));
    g_smart_gadget_display->HandleScreenLoaded(page);
}

void SmartGadgetDisplay::SetStatus(const char* status) {
    if (!setup_ui_called_) {
        ESP_LOGW(TAG, "SetStatus('%s') called before SetupUI() - message will be lost!", status);
        return;
    }

    DisplayLockGuard lock(this);
    const char* text = status != nullptr ? status : "";
    UpdateCallStatus(text);

    if (ui_ready_ && (strcmp(text, Lang::Strings::STANDBY) == 0)) {
        LoadPage(kPageClock);
    }
}

void SmartGadgetDisplay::ShowNotification(const char* notification, int duration_ms) {
    (void)duration_ms;
    if (!setup_ui_called_) {
        ESP_LOGW(TAG, "ShowNotification('%s') called before SetupUI() - message will be lost!", notification);
        return;
    }

    DisplayLockGuard lock(this);
    UpdateCallStatus(notification != nullptr ? notification : "");
}

void SmartGadgetDisplay::SetChatMessage(const char* role, const char* content) {
    if (!setup_ui_called_) {
        ESP_LOGW(TAG, "SetChatMessage('%s', '%s') called before SetupUI() - message will be lost!", role, content);
        return;
    }

    DisplayLockGuard lock(this);
    const char* safe_role = role != nullptr ? role : "system";
    const char* safe_content = content != nullptr ? content : "";

    if (strcmp(safe_role, "user") == 0) {
        user_message_ = safe_content;
        ApplyChatText();
        ApplyDeviceText();
        return;
    }

    if (strcmp(safe_role, "assistant") == 0) {
        assistant_message_ = safe_content;
        ApplyChatText();
        ApplyDeviceText();
        return;
    }

    if (safe_content[0] != '\0') {
        UpdateCallStatus(safe_content);
    }
}

void SmartGadgetDisplay::ClearChatMessages() {
    DisplayLockGuard lock(this);
    user_message_visible_ = false;
    assistant_message_visible_ = true;
    user_message_.clear();
    assistant_message_.clear();
    ApplyChatText();
    ApplyDeviceText();
}

void SmartGadgetDisplay::SetEmotion(const char* emotion) {
    // Smart_Gadget UI keeps its own visual language; emotion updates are not shown on Call/Chat pages.
    (void)emotion;
}

void SmartGadgetDisplay::SetTheme(Theme* theme) {
    current_theme_ = theme;
}

void SmartGadgetDisplay::ShowCallPage() {
    if (!setup_ui_called_) {
        return;
    }
    DisplayLockGuard lock(this);
    LoadPage(kPageCall);
}

void SmartGadgetDisplay::ShowChatPage() {
    if (!setup_ui_called_) {
        return;
    }
    DisplayLockGuard lock(this);
    LoadPage(kPageChat);
}

void SmartGadgetDisplay::LoadPage(PageIndex page) {
    switch (page) {
        case kPageClock:
            if (ui_Clock == nullptr) {
                ui_Clock_screen_init();
            }
            {
                time_t now = time(nullptr);
                struct tm timeinfo = {};
                localtime_r(&now, &timeinfo);
                if (timeinfo.tm_year >= (2025 - 1900)) {
                    ApplyClockText(timeinfo);
                }
            }
            lv_screen_load_anim(ui_Clock, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, false);
            break;
        case kPageCall:
            if (ui_Call == nullptr) {
                ui_Call_screen_init();
            }
            ApplyCallText();
            lv_screen_load_anim(ui_Call, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, false);
            break;
        case kPageChat:
            if (ui_Chat == nullptr) {
                ui_Chat_screen_init();
            }
            ApplyChatText();
            lv_screen_load_anim(ui_Chat, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, false);
            break;
        case kPageMusic:
            if (ui_Music_Player == nullptr) {
                ui_Music_Player_screen_init();
            }
            lv_screen_load_anim(ui_Music_Player, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, false);
            break;
        case kPageWeather:
            if (ui_Weather == nullptr) {
                ui_Weather_screen_init();
            }
            EnsureSensorServiceStarted();
            ApplyWeatherText();
            lv_screen_load_anim(ui_Weather, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, false);
            break;
        case kPageAlarm:
            if (ui_Alarm == nullptr) {
                ui_Alarm_screen_init();
            }
            lv_screen_load_anim(ui_Alarm, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, false);
            break;
        case kPageDevice:
            if (ui_Device == nullptr) {
                ui_Device_screen_init();
            }
            EnsureSensorServiceStarted();
            ApplyDeviceText();
            lv_screen_load_anim(ui_Device, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, false);
            break;
        case kPageSplash:
        default:
            if (ui_Splash == nullptr) {
                ui_Splash_screen_init();
            }
            lv_screen_load_anim(ui_Splash, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, false);
            page = kPageSplash;
            break;
    }
    current_page_ = page;
}
