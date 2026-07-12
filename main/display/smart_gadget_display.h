#ifndef SMART_GADGET_DISPLAY_H
#define SMART_GADGET_DISPLAY_H

#include "lcd_display.h"

#include <atomic>
#include <cstdint>
#include <string>

class SmartGadgetDisplay : public SpiLcdDisplay {
public:
    SmartGadgetDisplay(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_handle_t panel,
                       int width, int height, int offset_x, int offset_y,
                       bool mirror_x, bool mirror_y, bool swap_xy);

    void SetupUI() override;
    void SetStatus(const char* status) override;
    void ShowNotification(const char* notification, int duration_ms = 3000) override;
    void SetChatMessage(const char* role, const char* content) override;
    void ClearChatMessages() override;
    void SetMusicTrackInfo(const char* title, const char* artist) override;
    void SetMusicPlaying(bool playing) override;
    void SetEmotion(const char* emotion) override;
    void SetTheme(Theme* theme) override;
    void UpdateStatusBar(bool update_all = false) override;
    void ShowCallPage();
    lv_display_t* GetLvglDisplay() const { return display_; }
    void OnSquareLineScreenCreated(lv_obj_t* screen);
    void ToggleFocusTimer();
    void ResetFocusTimer();

private:
    enum PageIndex {
        kPageSplash = 0,
        kPageClock,
        kPageCall,
        kPageToday,
        kPageMusic,
        kPageWeather,
        kPageAlarm,
        kPageDevice,
        kPageCount
    };

    std::atomic<int> current_page_{kPageSplash};
    bool ui_ready_ = false;
    bool call_buttons_bound_ = false;
    bool music_buttons_bound_ = false;
    bool today_buttons_bound_ = false;
    bool music_playing_ = false;
    bool focus_running_ = false;
    lv_obj_t* music_pause_icon_ = nullptr;
    lv_obj_t* music_title_label_ = nullptr;
    lv_obj_t* music_artist_label_ = nullptr;
    lv_timer_t* live_data_timer_ = nullptr;
    int32_t focus_remaining_seconds_ = 25 * 60;
    int64_t focus_deadline_us_ = 0;
    std::string call_status_ = "Preparing...";
    std::string user_message_;
    std::string assistant_message_;

    void ApplyInitialText();
    void ApplyCallText();
    void ApplyWeatherText();
    void ApplyDeviceText();
    void ApplyTodayText(const struct tm& timeinfo);
    void ApplyMusicButtonState();
    void ApplyClockText(const struct tm& timeinfo);
    void ApplyReadableFont(lv_obj_t* obj);
    void ApplyTodayFont(lv_obj_t* obj);
    void BindCallButtons();
    void BindMusicButtons();
    void BindTodayButtons();
    void RefreshLiveData();
    void UpdateFocusCountdown(int64_t now_us);
    static void LiveDataTimerCallback(lv_timer_t* timer);
    void UpdateCallStatus(const char* status);
    void LoadPage(PageIndex page);
    void EnsureSensorServiceStarted();
    void ConfigureScreenLifecycle(lv_obj_t* screen, PageIndex page);
    void HandleScreenLoaded(PageIndex page);
    static void ScreenLoadedEventCallback(lv_event_t* e);
};

extern SmartGadgetDisplay* g_smart_gadget_display;

#endif // SMART_GADGET_DISPLAY_H
