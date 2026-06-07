#pragma once

#include "bme69x.h"
#include "bsec_datatypes.h"
#include "bsec_interface.h"
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif

struct SensorData {
    float temperature;
    float humidity;
    float pressure;
    float iaq;
    uint8_t iaq_accuracy;
    float co2_equivalent;
    float breath_voc_equivalent;
    float static_iaq;
    float gas_resistance;
};

#ifdef __cplusplus
}

class SensorService {
public:
    static SensorService &getInstance();

    esp_err_t init();
    void deinit();
    bool isInitialized() const { return _initialized; }
    bool getLatestData(SensorData &out);

private:
    SensorService() = default;
    ~SensorService();
    SensorService(const SensorService &) = delete;
    SensorService &operator=(const SensorService &) = delete;

    static void sensorTaskFunc(void *arg);

    esp_err_t initHardware();
    esp_err_t initBME690();
    bsec_library_return_t initBSEC();
    bool loadBSECState();
    bool saveBSECState();

    bool _initialized = false;
    bool _running = false;

    i2c_master_bus_handle_t _i2cBus = nullptr;
    struct bme69x_dev _bme;
    struct bme69x_conf _conf;
    struct bme69x_heatr_conf _heatrConf;

    uint8_t _bsecState[BSEC_MAX_STATE_BLOB_SIZE];
    int64_t _lastStateSave = 0;

    SensorData _latestData;
    SemaphoreHandle_t _dataMutex = nullptr;
    TaskHandle_t _taskHandle = nullptr;

    // Humidity sliding average
    static constexpr int HUMIDITY_AVG_COUNT = 5;
    float _humidityHistory[HUMIDITY_AVG_COUNT] = {0.0f};
    int _humidityIndex = 0;
    int _humidityCount = 0;

    float _previousPressure = 0.0f;
};

#endif
