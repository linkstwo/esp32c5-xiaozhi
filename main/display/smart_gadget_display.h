#ifndef SMART_GADGET_DISPLAY_H
#define SMART_GADGET_DISPLAY_H

#include "lcd_display.h"

#include <atomic>
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
    void SetEmotion(const char* emotion) override;
    void SetTheme(Theme* theme) override;
    void UpdateStatusBar(bool update_all = false) override;
    void ShowCallPage();
    void ShowChatPage();
    bool IsChatPage() const { return current_page_.load() == kPageChat; }
    lv_display_t* GetLvglDisplay() const { return display_; }
    void OnSquareLineScreenCreated(lv_obj_t* screen);

private:
    enum PageIndex {
        kPageSplash = 0,
        kPageClock,
        kPageCall,
        kPageChat,
        kPageMusic,
        kPageWeather,
        kPageAlarm,
        kPageDevice,
        kPageCount
    };

    std::atomic<int> current_page_{kPageSplash};
    bool ui_ready_ = false;
    bool user_message_visible_ = false;
    bool assistant_message_visible_ = false;
    bool call_buttons_bound_ = false;
    lv_timer_t* live_data_timer_ = nullptr;
    std::string call_status_ = "Preparing...";
    std::string user_message_;
    std::string assistant_message_;

    void ApplyInitialText();
    void ApplyCallText();
    void ApplyChatText();
    void ApplyWeatherText();
    void ApplyDeviceText();
    void ApplyClockText(const struct tm& timeinfo);
    void ApplyReadableFont(lv_obj_t* obj);
    void BindCallButtons();
    void RefreshLiveData();
    static void LiveDataTimerCallback(lv_timer_t* timer);
    void UpdateCallStatus(const char* status);
    void SetObjectHidden(lv_obj_t* obj, bool hidden);
    void LoadPage(PageIndex page);
};

extern SmartGadgetDisplay* g_smart_gadget_display;

#endif // SMART_GADGET_DISPLAY_H
