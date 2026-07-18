#include "smart_gadget_display.h"

#include "application.h"
#include "assets/lang_config.h"
#include "board.h"
#include "sensor_service.hpp"
#include "smart_gadget_ui/screens/focus_ui_tokens.h"
#include "smart_gadget_ui/ui.h"

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_timer.h>
#include <font_awesome.h>

#define TAG "SmartGadgetDisplay"

LV_FONT_DECLARE(BUILTIN_TEXT_FONT);
LV_FONT_DECLARE(BUILTIN_ICON_FONT);
LV_FONT_DECLARE(font_puhui_16_4);
LV_FONT_DECLARE(ui_font_FocusDigits);

SmartGadgetDisplay* g_smart_gadget_display = nullptr;

namespace {
constexpr int kFocusDurationSeconds = 25 * 60;
constexpr uint32_t kMusicTrackTotalUi = 2;
constexpr uint32_t kMusicVolumePercentUi = 80;

struct CountdownTarget {
    const char* label;
    int year;
    int month;
    int day;
};

constexpr CountdownTarget kCountdownTargets[] = {
    {"考试", 2026, 7, 1},
    {"项目", 2026, 7, 15},
};

const char* kWeekdaysZh[] = {
    "周日", "周一", "周二", "周三", "周四", "周五", "周六",
};

const char* GetWeekdayText(int wday) {
    if (wday < 0 || wday > 6) {
        return "--";
    }
    return kWeekdaysZh[wday];
}

[[maybe_unused]] int DaysUntilTarget(const struct tm& today, const CountdownTarget& target) {
    struct tm today_midnight = today;
    today_midnight.tm_hour = 0;
    today_midnight.tm_min = 0;
    today_midnight.tm_sec = 0;
    today_midnight.tm_isdst = -1;

    struct tm target_tm = {};
    target_tm.tm_year = target.year - 1900;
    target_tm.tm_mon = target.month - 1;
    target_tm.tm_mday = target.day;
    target_tm.tm_isdst = -1;

    const time_t today_ts = mktime(&today_midnight);
    const time_t target_ts = mktime(&target_tm);
    if (today_ts == static_cast<time_t>(-1) || target_ts == static_cast<time_t>(-1)) {
        return -1;
    }
    return static_cast<int>((target_ts - today_ts) / (24 * 60 * 60));
}

constexpr int kFocusTimerPeriodMs = 250;
constexpr int kFocusArcMaxValue = FOCUS_ARC_PROGRESS_MAX_VALUE;

struct FocusPalette {
    uint32_t bg;
    uint32_t accent;
    uint32_t left_bg;
    uint32_t left_text;
    uint32_t left_border;
    uint32_t main_bg;
    uint32_t main_text;
    uint32_t main_border;
    uint32_t right_bg;
    uint32_t right_text;
    uint32_t right_border;
};

bool FocusStateUsesCountdown(SmartGadgetDisplay::FocusUiState state) {
    return state == SmartGadgetDisplay::FocusUiState::Running;
}

int InferMusicTrackIndex(const char* title) {
    if (title == nullptr || title[0] == '\0') {
        return -1;
    }
    if (std::strstr(title, "Boss Koto") != nullptr) {
        return 0;
    }
    if (std::strstr(title, "Waiting 40 Lofi") != nullptr) {
        return 1;
    }
    return -1;
}

FocusPalette GetFocusPalette(SmartGadgetDisplay::FocusUiState state) {
    switch (state) {
    case SmartGadgetDisplay::FocusUiState::Running:
        return {FOCUS_COLOR_RUNNING_BG, FOCUS_COLOR_RUNNING_ACCENT,
                FOCUS_COLOR_RUNNING_LEFT, FOCUS_COLOR_DEEP_GREEN, FOCUS_COLOR_LEAF_GREEN,
                FOCUS_COLOR_RUNNING_PRIMARY, FOCUS_COLOR_BROWN_TEXT, FOCUS_COLOR_CORAL_TOMATO,
                FOCUS_COLOR_RUNNING_SECONDARY, FOCUS_COLOR_DEEP_GREEN, FOCUS_COLOR_LEAF_GREEN};
    case SmartGadgetDisplay::FocusUiState::Paused:
        return {FOCUS_COLOR_PAUSED_BG, FOCUS_COLOR_PAUSED_ACCENT,
                FOCUS_COLOR_PAUSED_LEFT, FOCUS_COLOR_DEEP_GREEN, FOCUS_COLOR_LEAF_GREEN,
                FOCUS_COLOR_PAUSED_PRIMARY, FOCUS_COLOR_BROWN_TEXT, FOCUS_COLOR_CORAL_TOMATO,
                FOCUS_COLOR_PAUSED_SECONDARY, FOCUS_COLOR_DEEP_GREEN, FOCUS_COLOR_LEAF_GREEN};
    case SmartGadgetDisplay::FocusUiState::Finished:
        return {FOCUS_COLOR_FINISHED_BG, FOCUS_COLOR_FINISHED_ACCENT,
                FOCUS_COLOR_FINISHED_LEFT, FOCUS_COLOR_DEEP_GREEN, FOCUS_COLOR_LEAF_GREEN,
                FOCUS_COLOR_FINISHED_PRIMARY, FOCUS_COLOR_BROWN_TEXT, FOCUS_COLOR_CORAL_TOMATO,
                FOCUS_COLOR_FINISHED_SECONDARY, FOCUS_COLOR_DEEP_GREEN, FOCUS_COLOR_LEAF_GREEN};
    case SmartGadgetDisplay::FocusUiState::Ready:
    default:
        return {FOCUS_COLOR_READY_BG, FOCUS_COLOR_READY_ACCENT,
                FOCUS_COLOR_READY_LEFT, FOCUS_COLOR_DEEP_GREEN, FOCUS_COLOR_LEAF_GREEN,
                FOCUS_COLOR_READY_PRIMARY, FOCUS_COLOR_BROWN_TEXT, FOCUS_COLOR_CORAL_TOMATO,
                FOCUS_COLOR_READY_SECONDARY, FOCUS_COLOR_DEEP_GREEN, FOCUS_COLOR_LEAF_GREEN};
    }
}

struct FocusButtonContent {
    const char* icon;
    const char* text;
};

struct FocusButtonSet {
    FocusButtonContent left;
    FocusButtonContent main;
    FocusButtonContent right;
};

FocusButtonSet GetFocusButtons(SmartGadgetDisplay::FocusUiState state) {
    switch (state) {
    case SmartGadgetDisplay::FocusUiState::Running:
        return {{FONT_AWESOME_PAUSE, "暂停"}, {FONT_AWESOME_STOP, "结束"}, {"+", "+5分"}};
    case SmartGadgetDisplay::FocusUiState::Paused:
        return {{FONT_AWESOME_ARROWS_ROTATE, "重置"}, {FONT_AWESOME_PLAY, "继续"}, {FONT_AWESOME_STOP, "结束"}};
    case SmartGadgetDisplay::FocusUiState::Finished:
        return {{FONT_AWESOME_CIRCLE_CHECK, "休息"}, {nullptr, "再来一轮"}, {FONT_AWESOME_STOP, "结束"}};
    case SmartGadgetDisplay::FocusUiState::Ready:
    default:
        return {{nullptr, "专注"}, {FONT_AWESOME_PLAY, "开始"}, {FONT_AWESOME_ARROWS_ROTATE, "重置"}};
    }
}

const lv_image_dsc_t* GetFocusMascot(SmartGadgetDisplay::FocusUiState state) {
    switch (state) {
    case SmartGadgetDisplay::FocusUiState::Running:
        return &ui_img_focus_mascot_running;
    case SmartGadgetDisplay::FocusUiState::Paused:
        return &ui_img_focus_mascot_paused;
    case SmartGadgetDisplay::FocusUiState::Finished:
        return &ui_img_focus_mascot_finished;
    case SmartGadgetDisplay::FocusUiState::Ready:
    default:
        return &ui_img_focus_mascot_ready;
    }
}

lv_point_t GetFocusMascotPosition(SmartGadgetDisplay::FocusUiState state) {
    if (state == SmartGadgetDisplay::FocusUiState::Paused) {
        return {FOCUS_MASCOT_LEFT_X, FOCUS_MASCOT_LEFT_Y};
    }
    return {FOCUS_MASCOT_RIGHT_X, FOCUS_MASCOT_RIGHT_Y};
}

void AnimateObjectOpacity(lv_obj_t* obj, int32_t from, int32_t to, uint32_t duration) {
    if (obj == nullptr) {
        return;
    }

    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, obj);
    lv_anim_set_values(&anim, from, to);
    lv_anim_set_duration(&anim, duration);
    lv_anim_set_exec_cb(&anim, [](void* var, int32_t value) {
        lv_obj_set_style_opa(static_cast<lv_obj_t*>(var), value, LV_PART_MAIN | LV_STATE_DEFAULT);
    });
    lv_anim_start(&anim);
}

void SetTodayButtonLabel(lv_obj_t* icon_obj, lv_obj_t* text_obj, const char* icon_text,
                         const char* label_text, uint32_t color) {
    if (icon_obj != nullptr) {
        lv_obj_set_style_text_color(icon_obj, lv_color_hex(color), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(icon_obj, icon_text != nullptr ? icon_text : "");
        if (icon_text != nullptr && icon_text[0] != '\0') {
            lv_obj_clear_flag(icon_obj, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(icon_obj, LV_OBJ_FLAG_HIDDEN);
        }
    }
    if (text_obj != nullptr) {
        lv_obj_set_style_text_color(text_obj, lv_color_hex(color), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(text_obj, label_text != nullptr ? label_text : "");
    }
}

void AnimateArcValue(lv_obj_t* arc, int32_t value) {
    if (arc == nullptr) {
        return;
    }

    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, arc);
    lv_anim_set_values(&anim, lv_arc_get_value(arc), value);
    lv_anim_set_duration(&anim, 260);
    lv_anim_set_exec_cb(&anim, [](void* var, int32_t v) {
        lv_arc_set_value(static_cast<lv_obj_t*>(var), v);
        ui_Today_set_focus_orbit_value(v);
    });
    lv_anim_start(&anim);
}

}

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

void SmartGadgetDisplay::ApplyReadableFont(lv_obj_t* obj) {
    if (obj != nullptr) {
        lv_obj_set_style_text_font(obj, &BUILTIN_TEXT_FONT, 0);
    }
}

void SmartGadgetDisplay::ApplyTodayFont(lv_obj_t* obj) {
    if (obj != nullptr) {
        lv_obj_set_style_text_font(obj, &font_puhui_16_4, 0);
    }
}

void SmartGadgetDisplay::ApplyIconFont(lv_obj_t* obj) {
    if (obj != nullptr) {
        lv_obj_set_style_text_font(obj, &BUILTIN_ICON_FONT, 0);
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
    ApplyWeatherText();
    ApplyDeviceText();
    time_t now = time(nullptr);
    struct tm timeinfo = {};
    localtime_r(&now, &timeinfo);
    ApplyTodayText(timeinfo);
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

void SmartGadgetDisplay::ApplyMusicButtonState() {
    if (ui_Play_btn == nullptr) {
        return;
    }

    if (music_playing_) {
        music_ui_state_ = MusicUiState::Playing;
        music_session_started_ = true;
    } else {
        music_ui_state_ = music_session_started_ ? MusicUiState::Paused : MusicUiState::Idle;
    }

    ui_music_set_playback_state(static_cast<ui_music_state_t>(music_ui_state_));
    ui_music_compact_set_track_index(music_track_index_, music_track_total_);
    ui_music_compact_set_volume(music_volume_percent_);
    ui_music_compact_set_track_info(music_title_.c_str(), music_artist_.c_str());
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
        snprintf(temp_text, sizeof(temp_text), "%.0fC", sensor_data.temperature);
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
        ui_Clock_set_time_text(time_text, true);
        if (ui_Clock_NumberShadow != nullptr) {
            lv_label_set_text(ui_Clock_NumberShadow, time_text);
        }
    }

    if (ui_Clock_Date != nullptr) {
        char date_text[32];
        snprintf(date_text, sizeof(date_text), "%02d·%02d %s",
                 timeinfo.tm_mon + 1, timeinfo.tm_mday, GetWeekdayText(timeinfo.tm_wday));
        ApplyTodayFont(ui_Clock_Date);
        lv_label_set_text(ui_Clock_Date, date_text);
    }

    if (ui_Clock_Status != nullptr || ui_Clock_Env_Left != nullptr || ui_Clock_Env_Right != nullptr) {
        EnsureSensorServiceStarted();
        SensorData sensor_data = {};
        const bool has_sensor_data = SensorService::getInstance().getLatestData(sensor_data);

        char status_text[32];
        char temp_text[24];
        char humidity_text[24];

        if (!has_sensor_data) {
            snprintf(status_text, sizeof(status_text), "等待传感器");
            snprintf(temp_text, sizeof(temp_text), "--°C");
            snprintf(humidity_text, sizeof(humidity_text), "--%%");
        } else {
            if (sensor_data.temperature >= 30.0f) {
                snprintf(status_text, sizeof(status_text), "环境偏热");
            } else if (sensor_data.humidity >= 70.0f || sensor_data.iaq > 150.0f) {
                snprintf(status_text, sizeof(status_text), "建议通风");
            } else if (sensor_data.temperature >= 22.0f && sensor_data.temperature <= 28.0f &&
                       sensor_data.humidity >= 40.0f && sensor_data.humidity <= 65.0f &&
                       sensor_data.iaq <= 100.0f) {
                snprintf(status_text, sizeof(status_text), "适合专注");
            } else {
                snprintf(status_text, sizeof(status_text), "状态稳定");
            }
            snprintf(temp_text, sizeof(temp_text), "%.0f°C", sensor_data.temperature);
            snprintf(humidity_text, sizeof(humidity_text), "%.0f%%", sensor_data.humidity);
        }

        if (ui_Clock_Status != nullptr) {
            ApplyTodayFont(ui_Clock_Status);
            lv_label_set_text(ui_Clock_Status, status_text);
        }
        if (ui_Clock_Env_Left != nullptr) {
            lv_label_set_text(ui_Clock_Env_Left, temp_text);
        }
        if (ui_Clock_Env_Right != nullptr) {
            lv_label_set_text(ui_Clock_Env_Right, humidity_text);
        }
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

void SmartGadgetDisplay::UpdateFocusCountdown(int64_t now_us) {
    if (!FocusStateUsesCountdown(focus_state_)) {
        return;
    }

    const int64_t remaining_us = focus_deadline_us_ - now_us;
    if (remaining_us > 0) {
        focus_remaining_seconds_ = static_cast<int32_t>((remaining_us + 999999) / 1000000);
        return;
    }

    focus_deadline_us_ = 0;
    focus_remaining_seconds_ = 0;
    if (!focus_finished_counted_) {
        ++focus_completed_sessions_today_;
        focus_finished_counted_ = true;
    }
    TransitionFocusState(FocusUiState::Finished);
}

void SmartGadgetDisplay::SetFocusDuration(int32_t duration_seconds) {
    if (focus_state_ != FocusUiState::Ready) {
        return;
    }

    (void)duration_seconds;
    focus_selected_duration_seconds_ = kFocusDurationSeconds;
    focus_active_duration_seconds_ = kFocusDurationSeconds;
    focus_remaining_seconds_ = kFocusDurationSeconds;
    focus_finished_counted_ = false;
    RenderTodayFocusState(true);
}

void SmartGadgetDisplay::StartFocusCountdown(int32_t duration_seconds, FocusUiState running_state) {
    focus_remaining_seconds_ = duration_seconds;
    focus_deadline_us_ = esp_timer_get_time() + static_cast<int64_t>(duration_seconds) * 1000000;
    focus_state_ = running_state;
    if (focus_ui_timer_ != nullptr) {
        lv_timer_set_period(focus_ui_timer_, kFocusTimerPeriodMs);
        lv_timer_resume(focus_ui_timer_);
    }
}

void SmartGadgetDisplay::StopFocusCountdown() {
    focus_deadline_us_ = 0;
    if (focus_ui_timer_ != nullptr) {
        lv_timer_pause(focus_ui_timer_);
    }
}

void SmartGadgetDisplay::ExtendFocusTime(int32_t extra_seconds) {
    if (focus_state_ != FocusUiState::Running || extra_seconds <= 0) {
        return;
    }

    constexpr int32_t kFocusMaxDurationSeconds = 60 * 60;
    const int32_t available = kFocusMaxDurationSeconds - focus_active_duration_seconds_;
    if (available <= 0) {
        return;
    }

    if (extra_seconds > available) {
        extra_seconds = available;
    }

    focus_active_duration_seconds_ += extra_seconds;
    focus_remaining_seconds_ += extra_seconds;
    focus_deadline_us_ += static_cast<int64_t>(extra_seconds) * 1000000;
}

void SmartGadgetDisplay::TransitionFocusState(FocusUiState next_state) {
    focus_state_ = next_state;
    if (!FocusStateUsesCountdown(next_state)) {
        StopFocusCountdown();
    } else if (focus_ui_timer_ != nullptr) {
        lv_timer_set_period(focus_ui_timer_, kFocusTimerPeriodMs);
        lv_timer_resume(focus_ui_timer_);
    }

    RenderTodayFocusState(true);
}

void SmartGadgetDisplay::UpdateFocusActionButtons(FocusUiState state) {
    const FocusButtonSet buttons = GetFocusButtons(state);
    const FocusPalette palette = GetFocusPalette(state);

    ApplyIconFont(ui_Today_left_btn_icon);
    if (ui_Today_left_btn_label != nullptr) {
        ApplyTodayFont(ui_Today_left_btn_label);
    }
    ApplyIconFont(ui_Today_focus_btn_icon);
    if (ui_Today_focus_btn_label != nullptr) {
        ApplyTodayFont(ui_Today_focus_btn_label);
    }
    ApplyIconFont(ui_Today_focus_reset_icon);
    if (ui_Today_focus_reset_label != nullptr) {
        ApplyTodayFont(ui_Today_focus_reset_label);
    }

    SetTodayButtonLabel(ui_Today_left_btn_icon, ui_Today_left_btn_label,
                        buttons.left.icon, buttons.left.text, palette.left_text);
    SetTodayButtonLabel(ui_Today_focus_btn_icon, ui_Today_focus_btn_label,
                        buttons.main.icon, buttons.main.text, palette.main_text);
    SetTodayButtonLabel(ui_Today_focus_reset_icon, ui_Today_focus_reset_label,
                        buttons.right.icon, buttons.right.text, palette.right_text);
}

void SmartGadgetDisplay::UpdateFocusPalette(FocusUiState state) {
    const FocusPalette palette = GetFocusPalette(state);

    if (ui_Today != nullptr) {
        lv_obj_set_style_bg_color(ui_Today, lv_color_hex(palette.bg), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    if (ui_Today_focus_time != nullptr) {
        lv_obj_set_style_text_color(ui_Today_focus_time, lv_color_hex(FOCUS_COLOR_TEXT_MAIN), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    if (ui_Today_focus_unit != nullptr) {
        lv_obj_set_style_text_color(ui_Today_focus_unit, lv_color_hex(palette.accent), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    if (ui_Today_status != nullptr) {
        lv_obj_set_style_text_color(ui_Today_status, lv_color_hex(FOCUS_COLOR_DEEP_GREEN), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    if (ui_Today_status_detail != nullptr) {
        lv_obj_set_style_text_color(ui_Today_status_detail, lv_color_hex(FOCUS_COLOR_BROWN_TEXT), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    if (ui_Today_focus_hint != nullptr) {
        lv_obj_set_style_text_color(ui_Today_focus_hint, lv_color_hex(FOCUS_COLOR_TEXT_GREEN), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    if (ui_Today_timer_arc != nullptr) {
        lv_obj_set_style_arc_color(ui_Today_timer_arc, lv_color_hex(FOCUS_COLOR_TRACK), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_arc_color(ui_Today_timer_arc, lv_color_hex(FOCUS_COLOR_CORAL_TOMATO), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    }
    if (ui_Today_focus_panel != nullptr) {
        lv_obj_set_style_border_color(ui_Today_focus_panel, lv_color_hex(FOCUS_COLOR_CREAM_TRACK), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    if (ui_Today_duration_left != nullptr) {
        lv_obj_set_style_bg_color(ui_Today_duration_left, lv_color_hex(palette.left_bg), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(ui_Today_duration_left, lv_color_hex(palette.left_border), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    if (ui_Today_focus_btn != nullptr) {
        lv_obj_set_style_bg_color(ui_Today_focus_btn, lv_color_hex(palette.main_bg), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(ui_Today_focus_btn, lv_color_hex(palette.main_border), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    if (ui_Today_focus_reset != nullptr) {
        lv_obj_set_style_bg_color(ui_Today_focus_reset, lv_color_hex(palette.right_bg), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(ui_Today_focus_reset, lv_color_hex(palette.right_border), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}

void SmartGadgetDisplay::UpdateFocusMascot(FocusUiState state, bool animate) {
    const lv_image_dsc_t* mascot = GetFocusMascot(state);
    const lv_point_t position = GetFocusMascotPosition(state);

    if (ui_Today_mascot_front == nullptr) {
        return;
    }

    if (!animate || focus_rendered_state_ == state || ui_Today_mascot_back == nullptr) {
        lv_image_set_src(ui_Today_mascot_front, mascot);
        lv_obj_set_pos(ui_Today_mascot_front, position.x, position.y);
        lv_obj_set_style_opa(ui_Today_mascot_front, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
        if (ui_Today_mascot_back != nullptr) {
            lv_obj_set_style_opa(ui_Today_mascot_back, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        return;
    }

    const void* previous_src = lv_image_get_src(ui_Today_mascot_front);
    lv_image_set_src(ui_Today_mascot_back, previous_src != nullptr ? previous_src : mascot);
    lv_obj_set_pos(ui_Today_mascot_back, lv_obj_get_x(ui_Today_mascot_front), lv_obj_get_y(ui_Today_mascot_front));
    lv_obj_set_style_opa(ui_Today_mascot_back, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_image_set_src(ui_Today_mascot_front, mascot);
    lv_obj_set_pos(ui_Today_mascot_front, position.x, position.y);
    lv_obj_set_style_opa(ui_Today_mascot_front, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    AnimateObjectOpacity(ui_Today_mascot_back, LV_OPA_COVER, LV_OPA_TRANSP, 220);
    AnimateObjectOpacity(ui_Today_mascot_front, LV_OPA_TRANSP, LV_OPA_COVER, 220);
}

void SmartGadgetDisplay::UpdateFocusArc(int32_t arc_value, bool animate) {
    if (ui_Today_timer_arc == nullptr) {
        return;
    }

    if (arc_value < 0) {
        arc_value = 0;
    }
    if (arc_value > kFocusArcMaxValue) {
        arc_value = kFocusArcMaxValue;
    }

    if (animate && focus_last_arc_value_ >= 0) {
        AnimateArcValue(ui_Today_timer_arc, arc_value);
    } else {
        lv_arc_set_value(ui_Today_timer_arc, arc_value);
    }
    ui_Today_set_focus_orbit_value(arc_value);
    focus_last_arc_value_ = arc_value;
}

void SmartGadgetDisplay::RenderTodayFocusState(bool animate) {
    if (ui_Today == nullptr) {
        return;
    }

    char focus_time_text[24];
    char focus_session_text[16];
    const int32_t total_seconds = focus_active_duration_seconds_ > 0 ? focus_active_duration_seconds_ : kFocusDurationSeconds;
    int32_t arc_value = 0;
    int32_t time_zoom = FOCUS_TIMER_ZOOM;
    int32_t time_y = FOCUS_TIMER_Y;
    int32_t unit_y = 83;
    int32_t hint_y = 19;
    bool show_unit = false;
    bool show_hint = false;
    const char* unit_text = "专注时光";
    const char* focus_status_text = "准备中";
    const lv_font_t* time_font = &ui_font_FocusDigits;

    UpdateFocusPalette(focus_state_);
    UpdateFocusActionButtons(focus_state_);
    ui_Today_set_focus_visual_state(static_cast<uint8_t>(focus_state_));
    ui_Today_refresh_focus_button_layout();
    UpdateFocusMascot(focus_state_, animate);

    switch (focus_state_) {
    case FocusUiState::Running:
        snprintf(focus_time_text, sizeof(focus_time_text), "%02d:%02d",
                 static_cast<int>(focus_remaining_seconds_ / 60),
                 static_cast<int>(focus_remaining_seconds_ % 60));
        focus_status_text = "专注中";
        arc_value = total_seconds > 0
            ? (focus_remaining_seconds_ * kFocusArcMaxValue) / total_seconds
            : 0;
        unit_text = "专注中";
        break;
    case FocusUiState::Paused:
        snprintf(focus_time_text, sizeof(focus_time_text), "%02d:%02d",
                 static_cast<int>(focus_remaining_seconds_ / 60),
                 static_cast<int>(focus_remaining_seconds_ % 60));
        focus_status_text = "已暂停";
        arc_value = total_seconds > 0
            ? (focus_remaining_seconds_ * kFocusArcMaxValue) / total_seconds
            : 0;
        unit_text = "已暂停";
        break;
    case FocusUiState::Finished:
        snprintf(focus_time_text, sizeof(focus_time_text), "%s", "完成");
        focus_status_text = "已完成";
        arc_value = kFocusArcMaxValue;
        show_unit = false;
        show_hint = true;
        time_font = &ui_font_FocusDigits;
        time_zoom = FOCUS_TIMER_ZOOM;
        time_y = FOCUS_TIMER_Y;
        break;
    case FocusUiState::Ready:
    default:
        snprintf(focus_time_text, sizeof(focus_time_text), "%02ld:%02d",
                 static_cast<long>(focus_selected_duration_seconds_ / 60), 0);
        focus_status_text = "准备中";
        arc_value = kFocusArcMaxValue;
        break;
    }

    const int32_t completed_sessions = focus_completed_sessions_today_ > 4
        ? 4 : focus_completed_sessions_today_;
    snprintf(focus_session_text, sizeof(focus_session_text), "%ld/4",
             static_cast<long>(completed_sessions));
    if (ui_Today_status != nullptr) {
        lv_label_set_text(ui_Today_status, focus_status_text);
    }
    if (ui_Today_status_detail != nullptr) {
        lv_label_set_text(ui_Today_status_detail, focus_session_text);
    }

    if (ui_Today_focus_time != nullptr) {
        lv_obj_set_style_text_font(ui_Today_focus_time, time_font, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_y(ui_Today_focus_time, time_y);
        lv_obj_set_style_transform_zoom(ui_Today_focus_time, time_zoom, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(ui_Today_focus_time, focus_time_text);
    }
    if (ui_Today_focus_unit != nullptr) {
        ApplyTodayFont(ui_Today_focus_unit);
        lv_obj_set_y(ui_Today_focus_unit, unit_y);
        lv_label_set_text(ui_Today_focus_unit, unit_text);
        if (show_unit) {
            lv_obj_clear_flag(ui_Today_focus_unit, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(ui_Today_focus_unit, LV_OBJ_FLAG_HIDDEN);
        }
    }
    if (ui_Today_focus_hint != nullptr) {
        if (show_hint) {
            ApplyTodayFont(ui_Today_focus_hint);
            lv_obj_set_y(ui_Today_focus_hint, hint_y);
            lv_label_set_text(ui_Today_focus_hint, "休息一下");
            lv_obj_clear_flag(ui_Today_focus_hint, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(ui_Today_focus_hint, LV_OBJ_FLAG_HIDDEN);
        }
    }

    UpdateFocusArc(arc_value, animate);
    focus_rendered_state_ = focus_state_;
}

void SmartGadgetDisplay::ApplyTodayText(const struct tm& timeinfo) {
    (void)timeinfo;
    UpdateFocusCountdown(esp_timer_get_time());
    RenderTodayFocusState(false);
}
void SmartGadgetDisplay::UpdateStatusBar(bool update_all) {
    (void)update_all;
    if (!ui_ready_) {
        return;
    }

    time_t now = time(nullptr);
    struct tm timeinfo = {};
    localtime_r(&now, &timeinfo);
    DisplayLockGuard lock(this);
    if (timeinfo.tm_year >= (2025 - 1900)) {
        ApplyClockText(timeinfo);
    }

    if (lv_screen_active() == ui_Today) {
        ApplyTodayText(timeinfo);
    } else if (lv_screen_active() == ui_Weather) {
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
    } else if (active_screen == ui_Today) {
        time_t now = time(nullptr);
        struct tm timeinfo = {};
        localtime_r(&now, &timeinfo);
        ApplyTodayText(timeinfo);
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

void SmartGadgetDisplay::FocusUiTimerCallback(lv_timer_t* timer) {
    auto* display = static_cast<SmartGadgetDisplay*>(lv_timer_get_user_data(timer));
    if (display == nullptr || !display->ui_ready_) {
        return;
    }

    DisplayLockGuard lock(display);
    display->UpdateFocusCountdown(esp_timer_get_time());
    if (lv_screen_active() == ui_Today) {
        display->RenderTodayFocusState(false);
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

static void smart_gadget_music_play_event_cb(lv_event_t* e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        Application::GetInstance().ToggleMusicPlayback();
    }
}

static void smart_gadget_music_prev_event_cb(lv_event_t* e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        Application::GetInstance().PreviousMusicTrack();
    }
}

static void smart_gadget_music_next_event_cb(lv_event_t* e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        Application::GetInstance().NextMusicTrack();
    }
}

static void smart_gadget_today_left_event_cb(lv_event_t* e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        if (g_smart_gadget_display != nullptr) {
            g_smart_gadget_display->HandleTodayLeftAction();
        }
    }
}

static void smart_gadget_today_main_event_cb(lv_event_t* e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        if (g_smart_gadget_display != nullptr) {
            g_smart_gadget_display->HandleTodayMainAction();
        }
    }
}

static void smart_gadget_today_right_event_cb(lv_event_t* e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        if (g_smart_gadget_display != nullptr) {
            g_smart_gadget_display->HandleTodayRightAction();
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

void SmartGadgetDisplay::BindMusicButtons() {
    if (music_buttons_bound_ || ui_Play_btn == nullptr) {
        return;
    }

    lv_obj_add_flag(ui_Play_btn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(ui_Play_btn, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_event_cb(ui_Play_btn, smart_gadget_music_play_event_cb, LV_EVENT_CLICKED, nullptr);

    if (ui_Play != nullptr) {
        lv_obj_add_flag(ui_Play, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_flag(ui_Play, LV_OBJ_FLAG_GESTURE_BUBBLE);
        lv_obj_add_event_cb(ui_Play, smart_gadget_music_play_event_cb, LV_EVENT_CLICKED, nullptr);
    }

    if (ui_Backward != nullptr) {
        lv_obj_add_flag(ui_Backward, LV_OBJ_FLAG_GESTURE_BUBBLE);
        lv_obj_add_event_cb(ui_Backward, smart_gadget_music_prev_event_cb, LV_EVENT_CLICKED, nullptr);
    }

    if (ui_Forward != nullptr) {
        lv_obj_add_flag(ui_Forward, LV_OBJ_FLAG_GESTURE_BUBBLE);
        lv_obj_add_event_cb(ui_Forward, smart_gadget_music_next_event_cb, LV_EVENT_CLICKED, nullptr);
    }

    music_buttons_bound_ = true;
    ApplyMusicButtonState();
}

void SmartGadgetDisplay::BindTodayButtons() {
    if (today_buttons_bound_ || ui_Today_duration_left == nullptr ||
        ui_Today_focus_btn == nullptr || ui_Today_focus_reset == nullptr) {
        return;
    }

    lv_obj_add_flag(ui_Today_duration_left, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(ui_Today_duration_left, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_event_cb(ui_Today_duration_left, smart_gadget_today_left_event_cb, LV_EVENT_CLICKED, nullptr);

    lv_obj_add_flag(ui_Today_focus_btn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(ui_Today_focus_btn, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_event_cb(ui_Today_focus_btn, smart_gadget_today_main_event_cb, LV_EVENT_CLICKED, nullptr);

    lv_obj_add_flag(ui_Today_focus_reset, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(ui_Today_focus_reset, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_event_cb(ui_Today_focus_reset, smart_gadget_today_right_event_cb, LV_EVENT_CLICKED, nullptr);

    today_buttons_bound_ = true;
}

void SmartGadgetDisplay::HandleTodayLeftAction() {
    if (!setup_ui_called_) {
        return;
    }

    DisplayLockGuard lock(this);
    const int64_t now_us = esp_timer_get_time();
    switch (focus_state_) {
    case FocusUiState::Running:
        UpdateFocusCountdown(now_us);
        if (focus_state_ != FocusUiState::Running) {
            break;
        }
        StopFocusCountdown();
        focus_state_ = FocusUiState::Paused;
        RenderTodayFocusState(true);
        break;
    case FocusUiState::Paused:
        focus_remaining_seconds_ = focus_selected_duration_seconds_;
        focus_active_duration_seconds_ = focus_selected_duration_seconds_;
        focus_finished_counted_ = false;
        TransitionFocusState(FocusUiState::Ready);
        break;
    case FocusUiState::Finished:
        focus_remaining_seconds_ = focus_selected_duration_seconds_;
        focus_active_duration_seconds_ = focus_selected_duration_seconds_;
        focus_finished_counted_ = false;
        TransitionFocusState(FocusUiState::Ready);
        break;
    case FocusUiState::Ready:
    default:
        RenderTodayFocusState(true);
        break;
    }
}

void SmartGadgetDisplay::HandleTodayMainAction() {
    if (!setup_ui_called_) {
        return;
    }

    DisplayLockGuard lock(this);
    switch (focus_state_) {
    case FocusUiState::Ready:
        focus_active_duration_seconds_ = focus_selected_duration_seconds_;
        focus_remaining_seconds_ = focus_selected_duration_seconds_;
        focus_finished_counted_ = false;
        StartFocusCountdown(focus_selected_duration_seconds_, FocusUiState::Running);
        RenderTodayFocusState(true);
        break;
    case FocusUiState::Running:
        StopFocusCountdown();
        focus_remaining_seconds_ = focus_selected_duration_seconds_;
        focus_active_duration_seconds_ = focus_selected_duration_seconds_;
        focus_finished_counted_ = false;
        TransitionFocusState(FocusUiState::Ready);
        break;
    case FocusUiState::Paused:
        StartFocusCountdown(focus_remaining_seconds_ > 0 ? focus_remaining_seconds_ : focus_selected_duration_seconds_,
                            FocusUiState::Running);
        RenderTodayFocusState(true);
        break;
    case FocusUiState::Finished:
        focus_active_duration_seconds_ = focus_selected_duration_seconds_;
        focus_remaining_seconds_ = focus_selected_duration_seconds_;
        focus_finished_counted_ = false;
        StartFocusCountdown(focus_selected_duration_seconds_, FocusUiState::Running);
        RenderTodayFocusState(true);
        break;
    }
}

void SmartGadgetDisplay::HandleTodayRightAction() {
    if (!setup_ui_called_) {
        return;
    }

    DisplayLockGuard lock(this);
    switch (focus_state_) {
    case FocusUiState::Running:
        ExtendFocusTime(5 * 60);
        RenderTodayFocusState(true);
        break;
    case FocusUiState::Paused:
    case FocusUiState::Finished:
        focus_remaining_seconds_ = focus_selected_duration_seconds_;
        focus_active_duration_seconds_ = focus_selected_duration_seconds_;
        focus_finished_counted_ = false;
        TransitionFocusState(FocusUiState::Ready);
        break;
    case FocusUiState::Ready:
    default:
        SetFocusDuration(kFocusDurationSeconds);
        break;
    }
}

void SmartGadgetDisplay::ToggleFocusTimer() {
    HandleTodayMainAction();
}

void SmartGadgetDisplay::ResetFocusTimer() {
    HandleTodayRightAction();
}

void SmartGadgetDisplay::OnSquareLineScreenCreated(lv_obj_t* screen) {
    if (screen == ui_Clock) {
        ConfigureScreenLifecycle(screen, kPageClock);
    } else if (screen == ui_Call) {
        ConfigureScreenLifecycle(screen, kPageCall);
        ApplyCallText();
    } else if (screen == ui_Today) {
        ConfigureScreenLifecycle(screen, kPageToday);
        BindTodayButtons();
        time_t now = time(nullptr);
        struct tm timeinfo = {};
        localtime_r(&now, &timeinfo);
        ApplyTodayText(timeinfo);
    } else if (screen == ui_Music_Player) {
        ConfigureScreenLifecycle(screen, kPageMusic);
        BindMusicButtons();
        ApplyMusicButtonState();
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
    focus_task_text_ = "关键一步";
    ui_init();
    ApplyInitialText();
    ui_ready_ = true;
    current_page_ = kPageSplash;
    if (live_data_timer_ == nullptr) {
        live_data_timer_ = lv_timer_create(SmartGadgetDisplay::LiveDataTimerCallback, 3000, this);
    }
    if (focus_ui_timer_ == nullptr) {
        focus_ui_timer_ = lv_timer_create(SmartGadgetDisplay::FocusUiTimerCallback, kFocusTimerPeriodMs, this);
        lv_timer_pause(focus_ui_timer_);
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
    time_t now = time(nullptr);
    struct tm timeinfo = {};
    localtime_r(&now, &timeinfo);
    ApplyTodayText(timeinfo);
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
    if (page == kPageWeather || page == kPageDevice || page == kPageToday) {
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

    if (ui_ready_ && current_page_.load() == kPageSplash && (strcmp(text, Lang::Strings::STANDBY) == 0)) {
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
        ApplyDeviceText();
        return;
    }

    if (strcmp(safe_role, "assistant") == 0) {
        assistant_message_ = safe_content;
        ApplyDeviceText();
        return;
    }

    if (safe_content[0] != '\0') {
        UpdateCallStatus(safe_content);
    }
}

void SmartGadgetDisplay::ClearChatMessages() {
    DisplayLockGuard lock(this);
    user_message_.clear();
    assistant_message_.clear();
    ApplyDeviceText();
}

void SmartGadgetDisplay::SetMusicPlaying(bool playing) {
    music_playing_ = playing;
    if (playing) {
        music_session_started_ = true;
        music_ui_state_ = MusicUiState::Playing;
    } else {
        music_ui_state_ = music_session_started_ ? MusicUiState::Paused : MusicUiState::Idle;
    }
    if (!setup_ui_called_) {
        return;
    }

    DisplayLockGuard lock(this);
    ApplyMusicButtonState();
}

void SmartGadgetDisplay::SetMusicTrackInfo(const char* title, const char* artist) {
    music_title_ = title != nullptr && title[0] != '\0' ? title : "正在加载";
    music_artist_ = artist != nullptr && artist[0] != '\0' ? artist : "本地音乐";

    const int inferred_index = InferMusicTrackIndex(title);
    if (inferred_index >= 0) {
        music_track_index_ = static_cast<uint32_t>(inferred_index);
    }
    if (!setup_ui_called_) {
        return;
    }

    DisplayLockGuard lock(this);
    ui_music_compact_set_track_info(music_title_.c_str(), music_artist_.c_str());
    ui_music_compact_set_track_index(music_track_index_, music_track_total_);
}

void SmartGadgetDisplay::SetMusicTrackIndex(uint32_t index, uint32_t total) {
    music_track_index_ = index;
    music_track_total_ = total == 0 ? 1 : total;
    if (!setup_ui_called_) {
        return;
    }

    DisplayLockGuard lock(this);
    ui_music_compact_set_track_index(music_track_index_, music_track_total_);
}

void SmartGadgetDisplay::SetEmotion(const char* emotion) {
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

void SmartGadgetDisplay::HandleTouchSwipe(int32_t delta_x, int32_t delta_y) {
    const int32_t abs_x = delta_x < 0 ? -delta_x : delta_x;
    const int32_t abs_y = delta_y < 0 ? -delta_y : delta_y;
    constexpr int32_t kSwipeThreshold = 30;

    /* Page navigation is intentionally horizontal.  Ignore vertical drags
     * instead of letting them accidentally activate a page or a button. */
    if (abs_x < kSwipeThreshold || abs_x <= abs_y) {
        return;
    }

    const int page = current_page_.load();
    if (page < kPageClock || page >= kPageCount) {
        return;
    }

    int next_page = page + (delta_x < 0 ? 1 : -1);
    if (next_page < kPageClock) {
        next_page = kPageDevice;
    } else if (next_page > kPageDevice) {
        next_page = kPageClock;
    }
    LoadPage(static_cast<PageIndex>(next_page));
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
        case kPageToday:
            if (ui_Today == nullptr) {
                ui_Today_screen_init();
            }
            {
                time_t now = time(nullptr);
                struct tm timeinfo = {};
                localtime_r(&now, &timeinfo);
                ApplyTodayText(timeinfo);
            }
            BindTodayButtons();
            lv_screen_load_anim(ui_Today, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, false);
            break;
        case kPageMusic:
            if (ui_Music_Player == nullptr) {
                ui_Music_Player_screen_init();
            }
            BindMusicButtons();
            ApplyMusicButtonState();
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
