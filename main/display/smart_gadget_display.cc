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
#include <font_awesome.h>

#define TAG "SmartGadgetDisplay"

LV_FONT_DECLARE(BUILTIN_TEXT_FONT);
LV_FONT_DECLARE(BUILTIN_ICON_FONT);
LV_FONT_DECLARE(font_puhui_16_4);

SmartGadgetDisplay* g_smart_gadget_display = nullptr;

namespace {
constexpr int kFocusDurationSeconds = 25 * 60;

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

int DaysUntilTarget(const struct tm& today, const CountdownTarget& target) {
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
    if (timeinfo.tm_year >= (2025 - 1900)) {
        ApplyTodayText(timeinfo);
    }
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
        if (ui_Play != nullptr) {
            lv_obj_add_flag(ui_Play, LV_OBJ_FLAG_HIDDEN);
        }
        if (music_pause_icon_ == nullptr) {
            music_pause_icon_ = lv_label_create(ui_Play_btn);
            lv_obj_center(music_pause_icon_);
            lv_obj_set_x(music_pause_icon_, 1);
            lv_label_set_text(music_pause_icon_, FONT_AWESOME_PAUSE);
            lv_obj_set_style_text_font(music_pause_icon_, &BUILTIN_ICON_FONT, 0);
            lv_obj_set_style_text_color(music_pause_icon_, lv_color_hex(0xFFFFFF), 0);
            lv_obj_set_style_text_opa(music_pause_icon_, 255, 0);
        } else {
            lv_obj_clear_flag(music_pause_icon_, LV_OBJ_FLAG_HIDDEN);
        }
    } else {
        if (ui_Play != nullptr) {
            lv_obj_clear_flag(ui_Play, LV_OBJ_FLAG_HIDDEN);
        }
        if (music_pause_icon_ != nullptr) {
            lv_obj_add_flag(music_pause_icon_, LV_OBJ_FLAG_HIDDEN);
        }
    }
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
        lv_label_set_text(ui_Clock_Number, time_text);
    }

    if (ui_Clock_Date != nullptr) {
        char date_text[32];
        snprintf(date_text, sizeof(date_text), "%02d月%02d日 %s",
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
            snprintf(temp_text, sizeof(temp_text), "温 -- C");
            snprintf(humidity_text, sizeof(humidity_text), "湿 --%%");
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
            snprintf(temp_text, sizeof(temp_text), "温 %.0f C", sensor_data.temperature);
            snprintf(humidity_text, sizeof(humidity_text), "湿 %.0f%%", sensor_data.humidity);
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
    if (!focus_running_) {
        return;
    }

    const int64_t remaining_us = focus_deadline_us_ - now_us;
    if (remaining_us <= 0) {
        focus_running_ = false;
        focus_deadline_us_ = 0;
        focus_remaining_seconds_ = 0;
        return;
    }

    focus_remaining_seconds_ = static_cast<int32_t>((remaining_us + 999999) / 1000000);
}

void SmartGadgetDisplay::ApplyTodayText(const struct tm& timeinfo) {
    if (ui_Today == nullptr) {
        return;
    }

    EnsureSensorServiceStarted();
    SensorData sensor_data = {};
    const bool has_sensor_data = SensorService::getInstance().getLatestData(sensor_data);
    UpdateFocusCountdown(esp_timer_get_time());

    char date_text[48];
    char status_text[64];
    char status_detail[96];
    char temp_text[64];
    char humidity_text[64];
    char air_text[64];
    char exam_text[96];
    char project_text[96];
    char focus_time_text[16];
    char focus_hint_text[96];
    char focus_button_text[24];
    const int exam_days = DaysUntilTarget(timeinfo, kCountdownTargets[0]);
    const int project_days = DaysUntilTarget(timeinfo, kCountdownTargets[1]);
    const CountdownTarget* primary_target = &kCountdownTargets[0];
    int primary_days = exam_days;

    snprintf(date_text, sizeof(date_text), "%02d月%02d日 %s",
             timeinfo.tm_mon + 1, timeinfo.tm_mday, GetWeekdayText(timeinfo.tm_wday));

    if ((primary_days < 0 && project_days >= 0) ||
        (project_days >= 0 && (primary_days < 0 || project_days < primary_days))) {
        primary_target = &kCountdownTargets[1];
        primary_days = project_days;
    }

    if (!has_sensor_data) {
        snprintf(status_text, sizeof(status_text), "等待传感器更新");
        snprintf(status_detail, sizeof(status_detail), "先确定一件最重要的事");
        snprintf(temp_text, sizeof(temp_text), "温度：-- C");
        snprintf(humidity_text, sizeof(humidity_text), "湿度：-- %%");
        snprintf(air_text, sizeof(air_text), "空气：等待更新");
        snprintf(project_text, sizeof(project_text), "今天建议：只推进一步");
    } else {
        if (sensor_data.temperature >= 30.0f) {
            snprintf(status_text, sizeof(status_text), "环境偏热");
            snprintf(status_detail, sizeof(status_detail), "先补水，再开始专注");
            snprintf(project_text, sizeof(project_text), "今天建议：降温后开一轮");
        } else if (sensor_data.humidity >= 70.0f || sensor_data.iaq > 150.0f) {
            snprintf(status_text, sizeof(status_text), "建议通风");
            snprintf(status_detail, sizeof(status_detail), "空气有点闷，先改善环境");
            snprintf(project_text, sizeof(project_text), "今天建议：通风后开一轮");
        } else if (sensor_data.temperature >= 22.0f && sensor_data.temperature <= 28.0f &&
                   sensor_data.humidity >= 40.0f && sensor_data.humidity <= 65.0f &&
                   sensor_data.iaq <= 100.0f) {
            snprintf(status_text, sizeof(status_text), "适合专注");
            if (focus_running_) {
                snprintf(status_detail, sizeof(status_detail), "保持当前节奏，别切任务");
                snprintf(project_text, sizeof(project_text), "今天建议：完成当前这一轮");
            } else {
                snprintf(status_detail, sizeof(status_detail), "环境不错，适合推进任务");
                snprintf(project_text, sizeof(project_text), "今天建议：开始 25 分钟");
            }
        } else {
            snprintf(status_text, sizeof(status_text), "可以开始");
            snprintf(status_detail, sizeof(status_detail), "先做一件事，边做边调整");
            snprintf(project_text, sizeof(project_text), "今天建议：先开始，再调整");
        }

        if (sensor_data.iaq <= 100.0f) {
            snprintf(air_text, sizeof(air_text), "空气：良好");
        } else if (sensor_data.iaq <= 150.0f) {
            snprintf(air_text, sizeof(air_text), "空气：一般");
        } else {
            snprintf(air_text, sizeof(air_text), "空气：需要通风");
        }
        snprintf(temp_text, sizeof(temp_text), "温度：%.0f C", sensor_data.temperature);
        snprintf(humidity_text, sizeof(humidity_text), "湿度：%.0f%%", sensor_data.humidity);
    }

    if (primary_days < 0) {
        snprintf(exam_text, sizeof(exam_text), "最近目标：完成一轮专注");
    } else if (primary_days == 0) {
        snprintf(exam_text, sizeof(exam_text), "最近目标：%s今天", primary_target->label);
    } else {
        snprintf(exam_text, sizeof(exam_text), "最近目标：%s D-%d", primary_target->label, primary_days);
    }

    snprintf(focus_time_text, sizeof(focus_time_text), "%02d:%02d",
             static_cast<int>(focus_remaining_seconds_ / 60),
             static_cast<int>(focus_remaining_seconds_ % 60));

    if (focus_running_) {
        snprintf(focus_hint_text, sizeof(focus_hint_text), "专注中，别切换任务");
        snprintf(focus_button_text, sizeof(focus_button_text), "暂停");
    } else if (focus_remaining_seconds_ == 0) {
        snprintf(focus_hint_text, sizeof(focus_hint_text), "这一轮完成了，休息一下");
        snprintf(focus_button_text, sizeof(focus_button_text), "开始");
    } else if (focus_remaining_seconds_ < kFocusDurationSeconds) {
        snprintf(focus_hint_text, sizeof(focus_hint_text), "已暂停，可以继续这一轮");
        snprintf(focus_button_text, sizeof(focus_button_text), "继续");
    } else {
        snprintf(focus_hint_text, sizeof(focus_hint_text), "25 分钟，只做当前这一件事");
        snprintf(focus_button_text, sizeof(focus_button_text), "开始");
    }

    if (ui_Today_title != nullptr) {
        ApplyTodayFont(ui_Today_title);
        lv_label_set_text(ui_Today_title, "今日专注");
    }
    if (ui_Today_date != nullptr) {
        ApplyTodayFont(ui_Today_date);
        lv_label_set_text(ui_Today_date, date_text);
    }
    if (ui_Today_status != nullptr) {
        ApplyTodayFont(ui_Today_status);
        lv_label_set_text(ui_Today_status, status_text);
    }
    if (ui_Today_status_detail != nullptr) {
        ApplyTodayFont(ui_Today_status_detail);
        lv_label_set_text(ui_Today_status_detail, status_detail);
    }
    if (ui_Today_env_temp != nullptr) {
        ApplyTodayFont(ui_Today_env_temp);
        lv_label_set_text(ui_Today_env_temp, temp_text);
    }
    if (ui_Today_env_humidity != nullptr) {
        ApplyTodayFont(ui_Today_env_humidity);
        lv_label_set_text(ui_Today_env_humidity, humidity_text);
    }
    if (ui_Today_env_air != nullptr) {
        ApplyTodayFont(ui_Today_env_air);
        lv_label_set_text(ui_Today_env_air, air_text);
    }
    if (ui_Today_exam != nullptr) {
        ApplyTodayFont(ui_Today_exam);
        lv_label_set_text(ui_Today_exam, exam_text);
    }
    if (ui_Today_project != nullptr) {
        ApplyTodayFont(ui_Today_project);
        lv_label_set_text(ui_Today_project, project_text);
    }
    if (ui_Today_focus_time != nullptr) {
        lv_label_set_text(ui_Today_focus_time, focus_time_text);
    }
    if (ui_Today_focus_hint != nullptr) {
        ApplyTodayFont(ui_Today_focus_hint);
        lv_label_set_text(ui_Today_focus_hint, focus_hint_text);
    }
    if (ui_Today_focus_btn_label != nullptr) {
        ApplyTodayFont(ui_Today_focus_btn_label);
        lv_label_set_text(ui_Today_focus_btn_label, focus_button_text);
    }
    if (ui_Today_focus_reset_label != nullptr) {
        ApplyTodayFont(ui_Today_focus_reset_label);
        lv_label_set_text(ui_Today_focus_reset_label, "重置");
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
        if (timeinfo.tm_year >= (2025 - 1900)) {
            ApplyTodayText(timeinfo);
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

static void smart_gadget_today_focus_event_cb(lv_event_t* e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        if (g_smart_gadget_display != nullptr) {
            g_smart_gadget_display->ToggleFocusTimer();
        }
    }
}

static void smart_gadget_today_reset_event_cb(lv_event_t* e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        if (g_smart_gadget_display != nullptr) {
            g_smart_gadget_display->ResetFocusTimer();
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
    if (today_buttons_bound_ || ui_Today_focus_btn == nullptr || ui_Today_focus_reset == nullptr) {
        return;
    }

    lv_obj_add_flag(ui_Today_focus_btn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(ui_Today_focus_btn, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_event_cb(ui_Today_focus_btn, smart_gadget_today_focus_event_cb, LV_EVENT_CLICKED, nullptr);

    lv_obj_add_flag(ui_Today_focus_reset, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(ui_Today_focus_reset, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_event_cb(ui_Today_focus_reset, smart_gadget_today_reset_event_cb, LV_EVENT_CLICKED, nullptr);

    today_buttons_bound_ = true;
}

void SmartGadgetDisplay::ToggleFocusTimer() {
    if (!setup_ui_called_) {
        return;
    }

    DisplayLockGuard lock(this);
    const int64_t now_us = esp_timer_get_time();
    if (focus_running_) {
        UpdateFocusCountdown(now_us);
        focus_running_ = false;
        focus_deadline_us_ = 0;
    } else {
        if (focus_remaining_seconds_ <= 0 || focus_remaining_seconds_ > kFocusDurationSeconds) {
            focus_remaining_seconds_ = kFocusDurationSeconds;
        }
        focus_deadline_us_ = now_us + static_cast<int64_t>(focus_remaining_seconds_) * 1000000;
        focus_running_ = true;
    }

    time_t now = time(nullptr);
    struct tm timeinfo = {};
    localtime_r(&now, &timeinfo);
    if (timeinfo.tm_year >= (2025 - 1900)) {
        ApplyTodayText(timeinfo);
    }
}

void SmartGadgetDisplay::ResetFocusTimer() {
    if (!setup_ui_called_) {
        return;
    }

    DisplayLockGuard lock(this);
    focus_running_ = false;
    focus_deadline_us_ = 0;
    focus_remaining_seconds_ = kFocusDurationSeconds;

    time_t now = time(nullptr);
    struct tm timeinfo = {};
    localtime_r(&now, &timeinfo);
    if (timeinfo.tm_year >= (2025 - 1900)) {
        ApplyTodayText(timeinfo);
    }
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
        if (timeinfo.tm_year >= (2025 - 1900)) {
            ApplyTodayText(timeinfo);
        }
    } else if (screen == ui_Music_Player) {
        ConfigureScreenLifecycle(screen, kPageMusic);
        music_pause_icon_ = nullptr;
        music_title_label_ = ui_Music_Title;
        music_artist_label_ = ui_Author;
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
    time_t now = time(nullptr);
    struct tm timeinfo = {};
    localtime_r(&now, &timeinfo);
    if (timeinfo.tm_year >= (2025 - 1900)) {
        ApplyTodayText(timeinfo);
    }
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
    if (!setup_ui_called_) {
        return;
    }

    DisplayLockGuard lock(this);
    ApplyMusicButtonState();
}

void SmartGadgetDisplay::SetMusicTrackInfo(const char* title, const char* artist) {
    if (!setup_ui_called_) {
        return;
    }

    DisplayLockGuard lock(this);
    if (ui_Music_Title != nullptr) {
        lv_label_set_text(ui_Music_Title, title != nullptr ? title : "");
    }
    if (ui_Author != nullptr) {
        lv_label_set_text(ui_Author, artist != nullptr ? artist : "");
    }
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
                if (timeinfo.tm_year >= (2025 - 1900)) {
                    ApplyTodayText(timeinfo);
                }
            }
            BindTodayButtons();
            lv_screen_load_anim(ui_Today, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, false);
            break;
        case kPageMusic:
            if (ui_Music_Player == nullptr) {
                ui_Music_Player_screen_init();
                music_pause_icon_ = nullptr;
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
