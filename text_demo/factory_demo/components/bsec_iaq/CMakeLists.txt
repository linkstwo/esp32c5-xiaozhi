idf_component_register(
    SRC_DIRS "config/bme680/bme680_iaq_33v_300s_28d"
    INCLUDE_DIRS "inc" "config/bme680/bme680_iaq_33v_300s_28d"
    WHOLE_ARCHIVE
)

if(CONFIG_IDF_TARGET_ARCH_RISCV)
    target_link_libraries(${COMPONENT_LIB} PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/bin/esp/esp32_risc_v/libalgobsec.a"
    )
endif()

target_link_options(${COMPONENT_LIB} PUBLIC
    "-Wl,--allow-multiple-definition"
)
