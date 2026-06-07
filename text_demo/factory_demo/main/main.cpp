#include "esp_board_manager.h"
#include "esp_lvgl_port.h"
#include "display.hpp"
#include "sensor_service.hpp"
#include "ui.h"
#include "freertos/FreeRTOS.h"
#include <ctime>

#ifdef ESP_UTILS_LOG_TAG
#undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "Main"
#include "esp_lib_utils.h"

// ---- UI update helpers (called from LVGL timer) ----

static void update_clock_time()
{
    time_t now;
    time(&now);
    struct tm *t = localtime(&now);

    if (ui_Clock_Number) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%02d:%02d", t->tm_hour, t->tm_min);
        lv_label_set_text(ui_Clock_Number, buf);
    }

    // Analog clock hands (0.1 degree units)
    if (ui_Hour) lv_image_set_rotation(ui_Hour, ((t->tm_hour % 12) * 300 + t->tm_min * 5));
    if (ui_Min)  lv_image_set_rotation(ui_Min, t->tm_min * 60);
    if (ui_Sec)  lv_image_set_rotation(ui_Sec, t->tm_sec * 60);
}

static void update_weather_sensor()
{
    SensorService &s = SensorService::getInstance();
    if (!s.isInitialized()) return;

    SensorData data;
    if (!s.getLatestData(data)) return;

    char buf[32];

    if (ui_Celsius) {
        snprintf(buf, sizeof(buf), "%.0f°", data.temperature);
        lv_label_set_text(ui_Celsius, buf);
    }

    if (ui_W1_Num) {
        snprintf(buf, sizeof(buf), "IAQ %.0f", data.iaq);
        lv_label_set_text(ui_W1_Num, buf);
    }

    if (ui_W2_Num) {
        snprintf(buf, sizeof(buf), "%.0fhPa", data.pressure / 100.0f);
        lv_label_set_text(ui_W2_Num, buf);
    }

    if (ui_W3_Num) {
        snprintf(buf, sizeof(buf), "%.0f%%", data.humidity);
        lv_label_set_text(ui_W3_Num, buf);
    }

    if (ui_Pary_Cloud) {
        if (data.iaq <= 50)
            lv_label_set_text(ui_Pary_Cloud, "Excellent air");
        else if (data.iaq <= 100)
            lv_label_set_text(ui_Pary_Cloud, "Good air");
        else if (data.iaq <= 150)
            lv_label_set_text(ui_Pary_Cloud, "Moderate air");
        else if (data.iaq <= 200)
            lv_label_set_text(ui_Pary_Cloud, "Polluted air");
        else
            lv_label_set_text(ui_Pary_Cloud, "Bad air");
    }

}

// ---- Main ----

extern "C" void app_main(void)
{
    ESP_UTILS_LOGI("Smart Desktop Companion - SquareLine UI");

    // 1. Board manager
    esp_board_manager_print_board_info();
    int ret = esp_board_manager_init();
    assert(ret == 0 && "Board manager init failed");

    // 2. Display (LCD + LVGL port + touch)
    display_init();

    // 3. Sensor service
    SensorService &sensor = SensorService::getInstance();
    if (sensor.init() != ESP_OK) {
        ESP_LOGW(ESP_UTILS_LOG_TAG, "Sensor init failed, continuing without sensor");
    }

    // 4. SquareLine UI init (must hold LVGL lock)
    {
        if (lvgl_port_lock(1000)) {
            ui_init();

            lv_timer_create([](lv_timer_t *t) {
                update_clock_time();
                update_weather_sensor();
            }, 1000, nullptr);

            lvgl_port_unlock();
        }
    }

    ESP_UTILS_LOGI("Main init complete. LVGL task running.");
}