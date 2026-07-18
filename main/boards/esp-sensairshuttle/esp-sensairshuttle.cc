#include "wifi_board.h"
#include "adc_pdm_audio_codec.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "mcp_server.h"
#include <wifi_station.h>
#include <esp_log.h>
#include <driver/i2c_master.h>
#include <driver/spi_common.h>
#include <esp_wifi.h>
#include <esp_event.h>

#include "display/smart_gadget_display.h"
#include <esp_lcd_touch_cst816s.h>
#include <esp_lvgl_port.h>
#include <esp_lcd_panel_vendor.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include "esp_lcd_ili9341.h"

#include "display/emote_display.h"

#include "assets/lang_config.h"
#include "anim_player.h"
#include "led_strip.h"
#include "driver/rmt_tx.h"

#include "sdkconfig.h"

constexpr char TAG[] = "ESP_SensairShuttle";

static const ili9341_lcd_init_cmd_t vendor_specific_init[] = {
    // {cmd, { data }, data_size, delay_ms}
    {0x11, NULL, 0, 120},                                          // Sleep Out
    {0x36, (uint8_t []){0x00}, 1, 0},                              // Memory Data Access Control
    {0x3A, (uint8_t []){0x05}, 1, 0},                              // Interface Pixel Format (16-bit)
    {0xB2, (uint8_t []){0x0C, 0x0C, 0x00, 0x33, 0x33}, 5, 0},      // Porch Setting
    {0xB7, (uint8_t []){0x05}, 1, 0},                              // Gate Control
    {0xBB, (uint8_t []){0x21}, 1, 0},                              // VCOM Setting
    {0xC0, (uint8_t []){0x2C}, 1, 0},                              // LCM Control
    {0xC2, (uint8_t []){0x01}, 1, 0},                              // VDV and VRH Command Enable
    {0xC3, (uint8_t []){0x15}, 1, 0},                              // VRH Set
    {0xC6, (uint8_t []){0x0F}, 1, 0},                              // Frame Rate Control
    {0xD0, (uint8_t []){0xA7}, 1, 0},                              // Power Control 1
    {0xD0, (uint8_t []){0xA4, 0xA1}, 2, 0},                        // Power Control 1
    {0xD6, (uint8_t []){0xA1}, 1, 0},                              // Gate output GND in sleep mode
    {
        0xE0, (uint8_t [])
        {
            0xF0, 0x05, 0x0E, 0x08, 0x0A, 0x17, 0x39, 0x54,
            0x4E, 0x37, 0x12, 0x12, 0x31, 0x37
        }, 14, 0
    },                                                             // Positive Gamma Control
    {
        0xE1, (uint8_t [])
        {
            0xF0, 0x10, 0x14, 0x0D, 0x0B, 0x05, 0x39, 0x44,
            0x4D, 0x38, 0x14, 0x14, 0x2E, 0x35
        }, 14, 0
    },                                                             // Negative Gamma Control
    {0xE4, (uint8_t []){0x23, 0x00, 0x00}, 3, 0},                  // Gate position control
    {0x21, NULL, 0, 0},                                            // Display Inversion On
    {0x29, NULL, 0, 0},                                            // Display On
    {0x2C, NULL, 0, 0},                                            // Memory Write
};

class EspSensairShuttle : public WifiBoard {
private:
    i2c_master_bus_handle_t i2c_bus_;
    esp_lcd_touch_handle_t touch_handle_ = nullptr;
    lv_indev_t* touch_indev_ = nullptr;
    Display* display_ = nullptr;
    Button boot_button_;
    lv_point_t touch_start_point_ = {};
    bool touch_tracking_ = false;

    static void TouchInputEventCallback(lv_event_t* event)
    {
        auto* board = static_cast<EspSensairShuttle*>(lv_event_get_user_data(event));
        if (board == nullptr || board->touch_indev_ == nullptr) {
            return;
        }

        lv_point_t point;
        lv_indev_get_point(board->touch_indev_, &point);
        const lv_event_code_t code = lv_event_get_code(event);
        if (code == LV_EVENT_PRESSED) {
            board->touch_start_point_ = point;
            board->touch_tracking_ = true;
        } else if (code == LV_EVENT_RELEASED && board->touch_tracking_) {
            board->touch_tracking_ = false;
            auto* smart_display = dynamic_cast<SmartGadgetDisplay*>(board->display_);
            if (smart_display != nullptr) {
                smart_display->HandleTouchSwipe(
                    point.x - board->touch_start_point_.x,
                    point.y - board->touch_start_point_.y);
            }
        }
    }

    void InitializeI2c()
    {
        i2c_master_bus_config_t i2c_bus_cfg = {
            .i2c_port = I2C_NUM_0,
            .sda_io_num = LCD_TP_SDA,
            .scl_io_num = LCD_TP_SCL,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .intr_priority = 0,
            .trans_queue_depth = 0,
            .flags = {
                .enable_internal_pullup = 1,
            },
        };
        ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_cfg, &i2c_bus_));
    }

    static void ProcessTouchCoordinates(esp_lcd_touch_handle_t tp, uint16_t *x, uint16_t *y, uint16_t *strength,
                                        uint8_t *point_num, uint8_t max_point_num)
    {
        (void)tp;
        (void)strength;

        if (x == nullptr || y == nullptr || point_num == nullptr) {
            return;
        }

        for (uint8_t i = 0; i < *point_num && i < max_point_num; ++i) {
            const uint16_t raw_x = x[i];
            const uint16_t raw_y = y[i];

            if (raw_x >= 0x0FF0 || raw_y >= 0x0FF0) {
                *point_num = 0;
                return;
            }

            uint16_t display_x = raw_y;
            uint16_t display_y = raw_x;

            if (display_x >= DISPLAY_WIDTH) {
                display_x = raw_x;
            }
            if (display_y >= DISPLAY_HEIGHT) {
                display_y = raw_y;
            }
            if (display_x >= DISPLAY_WIDTH || display_y >= DISPLAY_HEIGHT) {
                *point_num = 0;
                return;
            }

            x[i] = display_x;
            y[i] = display_y;
        }
    }

    void InitializeTouch()
    {
        auto smart_display = dynamic_cast<SmartGadgetDisplay*>(display_);
        if (smart_display == nullptr || smart_display->GetLvglDisplay() == nullptr) {
            ESP_LOGW(TAG, "Skip touch init: LVGL display is not ready");
            return;
        }

        esp_lcd_panel_io_handle_t touch_io = nullptr;
        esp_lcd_panel_io_i2c_config_t touch_io_config = {
            .dev_addr = ESP_LCD_TOUCH_IO_I2C_CST816S_ADDRESS,
            .on_color_trans_done = nullptr,
            .user_ctx = nullptr,
            .control_phase_bytes = 1,
            .dc_bit_offset = 0,
            .lcd_cmd_bits = 8,
            .lcd_param_bits = 0,
            .flags = {
                .dc_low_on_data = 0,
                .disable_control_phase = 1,
            },
            .scl_speed_hz = 100000,
        };
        esp_err_t ret = esp_lcd_new_panel_io_i2c(i2c_bus_, &touch_io_config, &touch_io);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create CST816S I2C IO: %s", esp_err_to_name(ret));
            return;
        }

        esp_lcd_touch_config_t touch_config = {
            .x_max = DISPLAY_WIDTH - 1,
            .y_max = DISPLAY_HEIGHT - 1,
            .rst_gpio_num = GPIO_NUM_NC,
            .int_gpio_num = GPIO_NUM_NC,
            .levels = {
                .reset = 0,
                .interrupt = 0,
            },
            .flags = {
                .swap_xy = 0,
                .mirror_x = 0,
                .mirror_y = 0,
            },
            .process_coordinates = ProcessTouchCoordinates,
        };

        ret = esp_lcd_touch_new_i2c_cst816s(touch_io, &touch_config, &touch_handle_);
        if (ret != ESP_OK || touch_handle_ == nullptr) {
            ESP_LOGE(TAG, "Failed to initialize CST816S touch: %s", esp_err_to_name(ret));
            return;
        }

        const lvgl_port_touch_cfg_t touch_cfg = {
            .disp = smart_display->GetLvglDisplay(),
            .handle = touch_handle_,
        };
        touch_indev_ = lvgl_port_add_touch(&touch_cfg);
        if (touch_indev_ == nullptr) {
            ESP_LOGE(TAG, "Failed to register CST816S touch with LVGL");
            return;
        }

        lv_indev_add_event_cb(touch_indev_, TouchInputEventCallback,
                              LV_EVENT_PRESSED, this);
        lv_indev_add_event_cb(touch_indev_, TouchInputEventCallback,
                              LV_EVENT_RELEASED, this);

        ESP_LOGI(TAG, "CST816S touch registered as LVGL input device");
    }

    void InitializeButtons()
    {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting) {
                ESP_LOGI(TAG, "Boot button pressed, enter WiFi configuration mode");
                EnterWifiConfigMode();
                return;
            }
            app.ToggleChatState();
        });
    }

    void InitializeSpi()
    {
        spi_bus_config_t buscfg = {};
        buscfg.mosi_io_num = DISPLAY_MOSI_PIN;
        buscfg.miso_io_num = GPIO_NUM_NC;
        buscfg.sclk_io_num = DISPLAY_CLK_PIN;
        buscfg.quadwp_io_num = GPIO_NUM_NC;
        buscfg.quadhd_io_num = GPIO_NUM_NC;
        buscfg.max_transfer_sz = DISPLAY_WIDTH * 10 * sizeof(uint16_t);
        ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    }

    void InitializeLcdDisplay()
    {
        esp_lcd_panel_io_handle_t panel_io = nullptr;
        esp_lcd_panel_handle_t panel = nullptr;

        ESP_LOGD(TAG, "Install panel IO");
        esp_lcd_panel_io_spi_config_t io_config = {};
        io_config.cs_gpio_num = DISPLAY_CS_PIN;
        io_config.dc_gpio_num = DISPLAY_DC_PIN;
        io_config.spi_mode = DISPLAY_SPI_MODE;
        io_config.pclk_hz = 40 * 1000 * 1000;
        io_config.trans_queue_depth = 10;
        io_config.lcd_cmd_bits = 8;
        io_config.lcd_param_bits = 8;
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(SPI2_HOST, &io_config, &panel_io));

        ESP_LOGD(TAG, "Install LCD driver");
        const ili9341_vendor_config_t vendor_config = {
            .init_cmds = &vendor_specific_init[0],
            .init_cmds_size = sizeof(vendor_specific_init) / sizeof(ili9341_lcd_init_cmd_t),
        };

        esp_lcd_panel_dev_config_t panel_config = {};
        panel_config.reset_gpio_num = DISPLAY_RST_PIN;
        panel_config.rgb_ele_order = DISPLAY_RGB_ORDER;
        panel_config.bits_per_pixel = 16;
        panel_config.vendor_config = (void *) &vendor_config;
        ESP_ERROR_CHECK(esp_lcd_new_panel_ili9341(panel_io, &panel_config, &panel));

        esp_lcd_panel_reset(panel);
        esp_lcd_panel_init(panel);
        esp_lcd_panel_invert_color(panel, DISPLAY_INVERT_COLOR);
        esp_lcd_panel_set_gap(panel, DISPLAY_OFFSET_X, DISPLAY_OFFSET_Y);
        esp_lcd_panel_mirror(panel, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y);
        esp_lcd_panel_swap_xy(panel, DISPLAY_SWAP_XY);
        ESP_LOGI(TAG, "LCD panel create success, %p", panel);

#ifdef CONFIG_USE_EMOTE_MESSAGE_STYLE
        display_ = new emote::EmoteDisplay(panel, panel_io, DISPLAY_WIDTH, DISPLAY_HEIGHT);
#else
        display_ = new SmartGadgetDisplay(panel_io, panel,
            DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, 0, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y, DISPLAY_SWAP_XY);
#endif

    }

public:
    EspSensairShuttle() : boot_button_(BOOT_BUTTON_GPIO) {
        InitializeI2c();
        InitializeButtons();
        InitializeSpi();
        InitializeLcdDisplay();
        InitializeTouch();
    }

    virtual AudioCodec* GetAudioCodec() override
    {
        static AdcPdmAudioCodec audio_codec(
            AUDIO_INPUT_SAMPLE_RATE,
            AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_ADC_MIC_CHANNEL,
            AUDIO_PDM_SPEAK_P_GPIO,
            AUDIO_PDM_SPEAK_N_GPIO,
            AUDIO_PA_CTL_GPIO);
        return &audio_codec;
    }

    virtual Display* GetDisplay() override
    {
        return display_;
    }

};

DECLARE_BOARD(EspSensairShuttle);
