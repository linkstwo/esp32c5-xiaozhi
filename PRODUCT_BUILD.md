# Product Build Notes

This repository is being converted from the upstream multi-board XiaoZhi ESP32 project into a single-product ESP32-C5 firmware project.

The product build drafts are:

```text
main/CMakeLists.product.txt
main/Kconfig.product.projbuild
```

They are intended to replace the upstream generic build entry files after local validation.

## Product build target

```text
Chip: ESP32-C5
Board: esp-sensairshuttle
Network: Wi-Fi
Protocol: MQTT + UDP
Display: ILI9341 SPI LCD
Touch: CST816S
Mic: ADC
Speaker: PDM
Language: zh-CN
```

## Keep during productization

```text
main/application.*
main/main.cc
main/device_state_machine.*
main/system_info.*
main/settings.*
main/ota.*
main/assets.*
main/audio/audio_service.*
main/audio/audio_codec.*
main/audio/demuxer/ogg_demuxer.*
main/protocols/protocol.*
main/protocols/mqtt_protocol.*
main/boards/esp-sensairshuttle/
main/boards/common/board.*
main/boards/common/wifi_board.*
main/boards/common/button.*
main/display/smart_gadget_display.*
main/display/smart_gadget_ui/
managed_components/
dependencies.lock
partitions/
```

## Remove only after build entry is clean

Do not delete source directories first. First remove references from the product CMake, then build, then delete unused files.

Candidates for later deletion:

```text
other board directories
WebSocket backend
ML307 / NT26 / 4G board paths
unused audio codecs
OLED display backend
EmoteDisplay backend
unused docs and demo assets
unused languages
unused UI pages after product design is finalized
```

## Validation checklist

After each build-system cleanup step, run a full local build.

After runtime-affecting cleanup, flash the board and monitor logs.

Confirm:

```text
Boot OK
Wi-Fi connected
OTA check OK
MQTT connected
Activation done
Call / Listening can start
No std::bad_alloc
No esp-aes memory error
No LVGL crash on normal product pages
```
