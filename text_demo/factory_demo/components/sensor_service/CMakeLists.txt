idf_component_register(
    SRC_DIRS "."
    INCLUDE_DIRS "."
    REQUIRES
        driver
        esp_timer
        freertos
        nvs_flash
        espressif__esp_board_manager
        espressif__i2c_bus
        bme690_sensor
    bsec_iaq
)
