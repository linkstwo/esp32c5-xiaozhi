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
    void SetMusicTrackIndex(uint32_t index, uint32_t total) override;
    void SetMusicPlaying(bool playing) override;
    void SetEmotion(const char* emotion) override;
    void SetTheme(Theme* theme) override;
    void UpdateStatusBar(bool update_all = false) override;
    void ShowCallPage();
    lv_display_t* GetLvglDisplay() const { return display_; }
    void OnSquareLineScreenCreated(lv_obj_t* screen);
    void HandleTodayLeftAction();
    void HandleTodayMainAction();
    void HandleTodayRightAction();
    void ToggleFocusTimer();
    void ResetFocusTimer();
    void SetFocusDuration(int32_t duration_seconds);

public:
    enum class MusicUiState : uint8_t {
        Idle = 0,
        Playing,
        Paused,
    };

    enum class FocusUiState : uint8_t {
        Ready = 0,
        Running,
        Paused,
        Finished,
    };

private:
    enum PageIndex {
        kPageSplash = 0,
        kPageClock,
        kPageToday,
        kPageMusic,
        kPageCall,
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
    bool music_session_started_ = false;
    MusicUiState music_ui_state_ = MusicUiState::Idle;
    uint32_t music_track_index_ = 0;
    uint32_t music_track_total_ = 2;
    uint32_t music_volume_percent_ = 80;
    lv_timer_t* live_data_timer_ = nullptr;
    lv_timer_t* focus_ui_timer_ = nullptr;
    FocusUiState focus_state_ = FocusUiState::Ready;
    FocusUiState focus_rendered_state_ = FocusUiState::Ready;
    int32_t focus_selected_duration_seconds_ = 25 * 60;
    int32_t focus_active_duration_seconds_ = 25 * 60;
    int32_t focus_remaining_seconds_ = 25 * 60;
    int64_t focus_deadline_us_ = 0;
    int32_t focus_completed_sessions_today_ = 0;
    int32_t focus_last_arc_value_ = -1;
    bool focus_finished_counted_ = false;
    std::string call_status_ = "Preparing...";
    std::string user_message_;
    std::string assistant_message_;
    std::string focus_task_text_ = "完成最重要的一小步";

    void ApplyInitialText();
    void ApplyCallText();
    void ApplyWeatherText();
    void ApplyDeviceText();
    void ApplyTodayText(const struct tm& timeinfo);
    void ApplyMusicButtonState();
    void ApplyClockText(const struct tm& timeinfo);
    void ApplyReadableFont(lv_obj_t* obj);
    void ApplyTodayFont(lv_obj_t* obj);
    void ApplyIconFont(lv_obj_t* obj);
    void BindCallButtons();
    void BindMusicButtons();
    void BindTodayButtons();
    void RefreshLiveData();
    void UpdateFocusCountdown(int64_t now_us);
    void StartFocusCountdown(int32_t duration_seconds, FocusUiState running_state);
    void StopFocusCountdown();
    void ExtendFocusTime(int32_t extra_seconds);
    void TransitionFocusState(FocusUiState next_state);
    void UpdateFocusActionButtons(FocusUiState state);
    void UpdateFocusPalette(FocusUiState state);
    void UpdateFocusMascot(FocusUiState state, bool animate);
    void UpdateFocusArc(int32_t arc_value, bool animate);
    void RenderTodayFocusState(bool animate);
    static void LiveDataTimerCallback(lv_timer_t* timer);
    static void FocusUiTimerCallback(lv_timer_t* timer);
    void UpdateCallStatus(const char* status);
    void LoadPage(PageIndex page);
    void EnsureSensorServiceStarted();
    void ConfigureScreenLifecycle(lv_obj_t* screen, PageIndex page);
    void HandleScreenLoaded(PageIndex page);
    static void ScreenLoadedEventCallback(lv_event_t* e);
};

extern SmartGadgetDisplay* g_smart_gadget_display;

#endif // SMART_GADGET_DISPLAY_H
