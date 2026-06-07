idf_component_register(
    SRC_DIRS "." "BME690_SensorAPI"
    INCLUDE_DIRS "." "BME690_SensorAPI"
    REQUIRES driver espressif__i2c_bus
)
