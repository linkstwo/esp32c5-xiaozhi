#include "sensor_service.hpp"

#include "bme69x_defs.h"
#include "bsec_datatypes.h"
#include "bsec_interface.h"
#include "common.h"
#include "driver/gpio.h"
#include "esp_board_manager.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "nvs.h"

static const char *TAG = "SensorService";

#define I2C_MASTER_NUM      I2C_NUM_0
#define I2C_MASTER_FREQ_HZ  100000
#define BME690_SDO_PIN      GPIO_NUM_9
#define BSEC_SAMPLE_RATE    BSEC_SAMPLE_RATE_LP
#define TEMP_OFFSET         5.0f
#define STATE_SAVE_PERIOD   (360 * 60 * 1000000LL)

SensorService &SensorService::getInstance()
{
    static SensorService instance;
    return instance;
}

SensorService::~SensorService()
{
    deinit();
}

esp_err_t SensorService::init()
{
    if (_initialized) {
        ESP_LOGW(TAG, "Already initialized");
        return ESP_OK;
    }

    esp_err_t ret = initHardware();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Hardware init failed");
        return ret;
    }

    ret = initBME690();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BME690 init failed");
        return ret;
    }

    bsec_library_return_t bsecRet = initBSEC();
    if (bsecRet < 0) {
        ESP_LOGE(TAG, "BSEC init failed: %d", bsecRet);
        return ESP_FAIL;
    }
    loadBSECState();

    _dataMutex = xSemaphoreCreateMutex();
    if (_dataMutex == nullptr) {
        ESP_LOGE(TAG, "Failed to create mutex");
        return ESP_ERR_NO_MEM;
    }

    _running = true;
    BaseType_t taskRet = xTaskCreate(&SensorService::sensorTaskFunc, "sensor_task",
                                     16 * 1024, this, 3, &_taskHandle);
    if (taskRet != pdPASS) {
        ESP_LOGE(TAG, "Failed to create sensor task");
        _running = false;
        vSemaphoreDelete(_dataMutex);
        _dataMutex = nullptr;
        return ESP_ERR_NO_MEM;
    }

    _initialized = true;
    ESP_LOGI(TAG, "Sensor service initialized");
    return ESP_OK;
}

void SensorService::deinit()
{
    if (!_initialized) return;

    _running = false;
    if (_taskHandle) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    bme69x_coines_deinit();

    if (_dataMutex) {
        vSemaphoreDelete(_dataMutex);
        _dataMutex = nullptr;
    }

    _initialized = false;
    ESP_LOGI(TAG, "Sensor service deinitialized");
}

bool SensorService::getLatestData(SensorData &out)
{
    if (!_initialized) return false;

    if (xSemaphoreTake(_dataMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        out = _latestData;
        xSemaphoreGive(_dataMutex);
        return true;
    }
    return false;
}

esp_err_t SensorService::initHardware()
{
    gpio_config_t sdoConf = {
        .pin_bit_mask = (1ULL << BME690_SDO_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
        .hys_ctrl_mode = GPIO_HYS_SOFT_DISABLE
    };
    esp_err_t ret = gpio_config(&sdoConf);
    if (ret != ESP_OK) return ret;
    gpio_set_level(BME690_SDO_PIN, 0);

    ret = i2c_master_get_bus_handle(I2C_MASTER_NUM, &_i2cBus);
    if (ret != ESP_OK) return ret;

    if (_i2cBus) {
        ESP_LOGI(TAG, "I2C bus reused: %p", _i2cBus);
        return ESP_OK;
    }

    i2c_master_bus_config_t *i2cCfg = nullptr;
    ret = esp_board_manager_get_periph_config("i2c_master", (void **)&i2cCfg);
    if (ret != ESP_OK || i2cCfg == nullptr) {
        ESP_LOGE(TAG, "Failed to get I2C peripheral config");
        return ESP_FAIL;
    }

    i2c_master_bus_config_t busCfg = {
        .i2c_port = I2C_MASTER_NUM,
        .sda_io_num = i2cCfg->sda_io_num,
        .scl_io_num = i2cCfg->scl_io_num,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
        .trans_queue_depth = 0,
        .flags = {.enable_internal_pullup = true, .allow_pd = false}
    };
    ret = i2c_new_master_bus(&busCfg, &_i2cBus);
    if (ret != ESP_OK) return ret;

    ESP_LOGI(TAG, "I2C bus created: SCL=%d, SDA=%d", i2cCfg->scl_io_num, i2cCfg->sda_io_num);
    return ESP_OK;
}

esp_err_t SensorService::initBME690()
{
    int8_t rslt;

    bme69x_set_i2c_bus_handle(_i2cBus);

    rslt = bme69x_interface_init(&_bme, BME69X_I2C_INTF);
    bme69x_check_rslt("bme69x_interface_init", rslt);
    if (rslt != BME69X_OK) return ESP_FAIL;

    rslt = bme69x_init(&_bme);
    bme69x_check_rslt("bme69x_init", rslt);
    if (rslt != BME69X_OK) return ESP_FAIL;
    ESP_LOGI(TAG, "Chip ID: 0x%02x", _bme.chip_id);

    _conf.filter = BME69X_FILTER_OFF;
    _conf.odr = BME69X_ODR_NONE;
    _conf.os_hum = BME69X_OS_16X;
    _conf.os_pres = BME69X_OS_16X;
    _conf.os_temp = BME69X_OS_16X;
    rslt = bme69x_set_conf(&_conf, &_bme);
    bme69x_check_rslt("bme69x_set_conf", rslt);
    if (rslt != BME69X_OK) return ESP_FAIL;

    _heatrConf.enable = BME69X_ENABLE;
    _heatrConf.heatr_temp = 300;
    _heatrConf.heatr_dur = 100;
    rslt = bme69x_set_heatr_conf(BME69X_FORCED_MODE, &_heatrConf, &_bme);
    bme69x_check_rslt("bme69x_set_heatr_conf", rslt);
    if (rslt != BME69X_OK) return ESP_FAIL;

    return ESP_OK;
}

bsec_library_return_t SensorService::initBSEC()
{
    bsec_library_return_t status = bsec_init();
    if (status != BSEC_OK) {
        ESP_LOGE(TAG, "bsec_init failed: %d", status);
        return status;
    }

    bsec_sensor_configuration_t requestedSensors[8];
    uint8_t nSensors = 8;

    requestedSensors[0] = {BSEC_SAMPLE_RATE, BSEC_OUTPUT_IAQ};
    requestedSensors[1] = {BSEC_SAMPLE_RATE, BSEC_OUTPUT_STATIC_IAQ};
    requestedSensors[2] = {BSEC_SAMPLE_RATE, BSEC_OUTPUT_CO2_EQUIVALENT};
    requestedSensors[3] = {BSEC_SAMPLE_RATE, BSEC_OUTPUT_BREATH_VOC_EQUIVALENT};
    requestedSensors[4] = {BSEC_SAMPLE_RATE, BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE};
    requestedSensors[5] = {BSEC_SAMPLE_RATE, BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY};
    requestedSensors[6] = {BSEC_SAMPLE_RATE, BSEC_OUTPUT_RAW_PRESSURE};
    requestedSensors[7] = {BSEC_SAMPLE_RATE, BSEC_OUTPUT_RAW_GAS};

    bsec_sensor_configuration_t requiredSettings[BSEC_MAX_PHYSICAL_SENSOR];
    uint8_t nRequired = BSEC_MAX_PHYSICAL_SENSOR;

    status = bsec_update_subscription(requestedSensors, nSensors, requiredSettings, &nRequired);

    if (status == BSEC_W_SU_SAMPLERATEMISMATCH) {
        ESP_LOGW(TAG, "BSEC sample rate mismatch (code 14) - BSEC will adapt");
    } else if (status > 0) {
        ESP_LOGW(TAG, "BSEC subscription warning: %d", status);
    } else if (status < 0) {
        ESP_LOGE(TAG, "BSEC subscription error: %d", status);
        return status;
    }

    ESP_LOGI(TAG, "BSEC initialized, %d required settings", nRequired);
    return BSEC_OK;
}

bool SensorService::loadBSECState()
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open("bsec", NVS_READONLY, &handle);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "No NVS BSEC state found, starting fresh calibration");
        return false;
    }

    uint8_t workBuf[BSEC_MAX_PROPERTY_BLOB_SIZE];
    size_t size = sizeof(_bsecState);
    err = nvs_get_blob(handle, "bsec_state", _bsecState, &size);
    if (err != ESP_OK) {
        nvs_close(handle);
        return false;
    }

    bsec_library_return_t status = bsec_set_state(_bsecState, size, workBuf, sizeof(workBuf));
    nvs_close(handle);

    if (status != BSEC_OK) {
        ESP_LOGW(TAG, "Failed to restore BSEC state: %d", status);
        return false;
    }

    ESP_LOGI(TAG, "BSEC state restored from NVS (%d bytes)", (int)size);
    return true;
}

bool SensorService::saveBSECState()
{
    uint8_t workBuf[BSEC_MAX_PROPERTY_BLOB_SIZE];
    uint32_t nSerialized = 0;

    bsec_library_return_t status = bsec_get_state(0, _bsecState, sizeof(_bsecState),
                                                   workBuf, sizeof(workBuf), &nSerialized);
    if (status != BSEC_OK) {
        ESP_LOGW(TAG, "bsec_get_state failed: %d", status);
        return false;
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open("bsec", NVS_READWRITE, &handle);
    if (err != ESP_OK) return false;

    err = nvs_set_blob(handle, "bsec_state", _bsecState, nSerialized);
    if (err == ESP_OK) nvs_commit(handle);
    nvs_close(handle);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "BSEC state saved to NVS (%lu bytes)", nSerialized);
        return true;
    }
    return false;
}

static int64_t getTimestampUs()
{
    return esp_timer_get_time();
}

void SensorService::sensorTaskFunc(void *arg)
{
    SensorService *self = static_cast<SensorService *>(arg);
    ESP_LOGI(TAG, "Sensor task started");

    bsec_bme_settings_t sensorSettings;
    bsec_input_t inputs[BSEC_MAX_PHYSICAL_SENSOR];
    bsec_output_t outputs[BSEC_NUMBER_OUTPUTS];
    struct bme69x_data bmeData;
    self->_lastStateSave = getTimestampUs();

    while (self->_running) {
        int64_t nowNs = getTimestampUs() * 1000;

        bsec_library_return_t bsecStatus = bsec_sensor_control(nowNs, &sensorSettings);
        if (bsecStatus != BSEC_OK) {
            ESP_LOGW(TAG, "bsec_sensor_control failed: %d", bsecStatus);
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        if (sensorSettings.trigger_measurement) {
            struct bme69x_conf conf;
            conf.filter = BME69X_FILTER_OFF;
            conf.odr = BME69X_ODR_NONE;
            conf.os_hum = sensorSettings.humidity_oversampling;
            conf.os_pres = sensorSettings.pressure_oversampling;
            conf.os_temp = sensorSettings.temperature_oversampling;

            int8_t rslt = bme69x_set_conf(&conf, &self->_bme);
            if (rslt != BME69X_OK) { vTaskDelay(pdMS_TO_TICKS(100)); continue; }

            struct bme69x_heatr_conf heatrConf;
            heatrConf.enable = BME69X_ENABLE;
            heatrConf.heatr_temp = sensorSettings.heater_temperature;
            heatrConf.heatr_dur = sensorSettings.heater_duration;

            rslt = bme69x_set_heatr_conf(BME69X_FORCED_MODE, &heatrConf, &self->_bme);
            if (rslt != BME69X_OK) { vTaskDelay(pdMS_TO_TICKS(100)); continue; }

            rslt = bme69x_set_op_mode(BME69X_FORCED_MODE, &self->_bme);
            if (rslt != BME69X_OK) { vTaskDelay(pdMS_TO_TICKS(100)); continue; }

            uint32_t delPeriod = bme69x_get_meas_dur(BME69X_FORCED_MODE, &conf, &self->_bme)
                                 + (heatrConf.heatr_dur * 1000);
            self->_bme.delay_us(delPeriod, self->_bme.intf_ptr);

            uint8_t nData = 0;
            rslt = bme69x_get_data(BME69X_FORCED_MODE, &bmeData, &nData, &self->_bme);
            if (rslt != BME69X_OK || nData == 0) { vTaskDelay(pdMS_TO_TICKS(100)); continue; }

            if (!(bmeData.status & BME69X_GASM_VALID_MSK)) {
                vTaskDelay(pdMS_TO_TICKS(100));
                continue;
            }

            uint8_t nInputs = 0;
            int64_t tsNs = getTimestampUs() * 1000;

            if (sensorSettings.process_data & BSEC_PROCESS_TEMPERATURE) {
                inputs[nInputs].sensor_id = BSEC_INPUT_TEMPERATURE;
                inputs[nInputs].signal = bmeData.temperature;
                inputs[nInputs].time_stamp = tsNs;
                nInputs++;

                inputs[nInputs].sensor_id = BSEC_INPUT_HEATSOURCE;
                inputs[nInputs].signal = TEMP_OFFSET;
                inputs[nInputs].time_stamp = tsNs;
                nInputs++;
            }
            if (sensorSettings.process_data & BSEC_PROCESS_HUMIDITY) {
                inputs[nInputs].sensor_id = BSEC_INPUT_HUMIDITY;
                inputs[nInputs].signal = bmeData.humidity;
                inputs[nInputs].time_stamp = tsNs;
                nInputs++;
            }
            if (sensorSettings.process_data & BSEC_PROCESS_PRESSURE) {
                inputs[nInputs].sensor_id = BSEC_INPUT_PRESSURE;
                inputs[nInputs].signal = bmeData.pressure;
                inputs[nInputs].time_stamp = tsNs;
                nInputs++;
            }
            if (sensorSettings.process_data & BSEC_PROCESS_GAS) {
                inputs[nInputs].sensor_id = BSEC_INPUT_GASRESISTOR;
                inputs[nInputs].signal = bmeData.gas_resistance;
                inputs[nInputs].time_stamp = tsNs;
                nInputs++;
            }

            uint8_t nOutputs = BSEC_NUMBER_OUTPUTS;
            bsecStatus = bsec_do_steps(inputs, nInputs, outputs, &nOutputs);

            if (bsecStatus == BSEC_OK && nOutputs > 0) {
                SensorData newData = {};
                for (uint8_t i = 0; i < nOutputs; i++) {
                    switch (outputs[i].sensor_id) {
                    case BSEC_OUTPUT_IAQ:
                        newData.iaq = outputs[i].signal;
                        newData.iaq_accuracy = outputs[i].accuracy;
                        break;
                    case BSEC_OUTPUT_STATIC_IAQ:
                        newData.static_iaq = outputs[i].signal;
                        break;
                    case BSEC_OUTPUT_CO2_EQUIVALENT:
                        newData.co2_equivalent = outputs[i].signal;
                        break;
                    case BSEC_OUTPUT_BREATH_VOC_EQUIVALENT:
                        newData.breath_voc_equivalent = outputs[i].signal;
                        break;
                    case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE:
                        newData.temperature = outputs[i].signal;
                        break;
                    case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY:
                        newData.humidity = outputs[i].signal;
                        break;
                    case BSEC_OUTPUT_RAW_PRESSURE:
                        newData.pressure = outputs[i].signal;
                        break;
                    case BSEC_OUTPUT_RAW_GAS:
                        newData.gas_resistance = outputs[i].signal;
                        break;
                    }
                }

                // Humidity sliding average
                self->_humidityHistory[self->_humidityIndex] = newData.humidity;
                self->_humidityIndex = (self->_humidityIndex + 1) % HUMIDITY_AVG_COUNT;
                if (self->_humidityCount < HUMIDITY_AVG_COUNT) self->_humidityCount++;

                float avgHumidity = 0;
                for (int i = 0; i < self->_humidityCount; i++) {
                    avgHumidity += self->_humidityHistory[i];
                }
                avgHumidity /= self->_humidityCount;
                newData.humidity = avgHumidity;

                if (xSemaphoreTake(self->_dataMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
                    self->_latestData = newData;
                    xSemaphoreGive(self->_dataMutex);
                }
            }

            // Save BSEC state periodically
            int64_t nowUs = getTimestampUs();
            if ((nowUs - self->_lastStateSave) >= STATE_SAVE_PERIOD) {
                self->saveBSECState();
                self->_lastStateSave = nowUs;
            }
        }

        int64_t nextCallNs = sensorSettings.next_call;
        int64_t endNs = getTimestampUs() * 1000;
        int64_t sleepMs = (nextCallNs - endNs) / 1000000;
        if (sleepMs > 0 && sleepMs < 10000) {
            vTaskDelay(pdMS_TO_TICKS(sleepMs));
        }
    }

    self->saveBSECState();
    ESP_LOGI(TAG, "Sensor task stopped");
    vTaskDelete(nullptr);
}
