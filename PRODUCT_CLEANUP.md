# ESP32-C5 XiaoZhi Product Cleanup Roadmap

这个文件用于指导本仓库从“上游通用小智 ESP32 工程”整理成“ESP32-C5 小智板产品工程”。

目标不是一次性大删，而是按阶段裁剪，确保每一步都能 build，必要时能快速回滚。

## 0. 当前产品边界

当前产品只面向：

```text
Board: esp-sensairshuttle
Chip: ESP32-C5
Network: Wi-Fi
Protocol: MQTT + UDP
Audio input: ADC mic
Audio output: PDM speaker
Display: ILI9341 SPI LCD
Touch: CST816S
UI: SmartGadgetDisplay
```

当前核心链路：

```text
Boot -> Wi-Fi -> OTA -> MQTT -> UDP audio -> mic upload -> TTS playback -> screen status
```

所有清理动作必须优先保证这条链路。

## 1. 必须保留

短期内不要删除这些模块：

```text
main/application.cc
main/application.h
main/main.cc
main/device_state_machine.cc
main/system_info.cc
main/settings.cc
main/ota.cc
main/assets.cc
main/audio/audio_service.cc
main/audio/audio_service.h
main/audio/audio_codec.cc
main/audio/audio_codec.h
main/audio/demuxer/ogg_demuxer.cc
main/audio/demuxer/ogg_demuxer.h
main/protocols/protocol.cc
main/protocols/protocol.h
main/protocols/mqtt_protocol.cc
main/protocols/mqtt_protocol.h
main/boards/esp-sensairshuttle/
main/boards/common/board.cc
main/boards/common/board.h
main/boards/common/wifi_board.cc
main/boards/common/wifi_board.h
main/boards/common/button.cc
main/boards/common/button.h
main/display/smart_gadget_display.cc
main/display/smart_gadget_display.h
main/display/smart_gadget_ui/
managed_components/
dependencies.lock
partitions/
```

## 2. 第一阶段：文档整理

目标：先把仓库外观从“上游通用项目”改成“本产品项目”。

已执行：

- 重写 `README.md`
- 新增 `PRODUCT_CLEANUP.md`

后续可以继续处理：

```text
README_zh.md
README_ja.md
docs/v1/ 中其他硬件图片
docs/ 中与本产品无关的教程、协议、硬件说明
```

注意：如果某些文档仍然用于理解上游协议，先移动到 `docs/upstream/`，不要直接删除。

## 3. 第二阶段：构建系统裁剪

目标：先让 CMake / Kconfig 只关心本产品，不再默认暴露几十种开发板。

建议顺序：

1. Kconfig 默认板型改为 `BOARD_TYPE_ESP_SENSAIRSHUTTLE`。
2. Kconfig 隐藏或删除其他 board type 选项。
3. CMake 中只保留 `esp-sensairshuttle` 的 board 分支。
4. CMake 中移除 WebSocket 构建入口，只保留 MQTT + UDP。
5. CMake 中移除 ML307 / NT26 / 4G 相关构建入口。
6. CMake 中移除不用的 audio codec 构建入口。
7. CMake 中移除不用的 display 类型构建入口。
8. 每完成一组都运行 `idf.py build`。

不要一边改 CMake 一边直接删除源码。先确保“不再引用”，再删文件。

## 4. 第三阶段：源码目录裁剪

在第二阶段 build 成功后，再开始删除不再被引用的源码。

候选删除对象：

```text
其他 board 目录
ML307 / NT26 / 4G 相关代码
WebSocket 协议代码
非 ADC/PDM 当前硬件需要的 audio codec
OLED 显示代码
EmoteDisplay 相关代码
未使用的 GIF / JPG 解码路径
未进入产品规划的 UI 页面和图片资源
多语言演示资源
```

每一类删除建议单独 commit，例如：

```text
Remove unused board implementations
Remove unused audio codecs
Remove websocket backend
Remove unused display backends
Remove upstream demo documents
```

## 5. UI 裁剪建议

当前 SmartGadget UI 编译了多个页面。产品化时需要重新确认每个页面是否属于产品功能。

建议保留：

```text
Splash
Clock / Home
Call
Chat
Device / Settings
```

建议评估后再决定：

```text
Weather
Alarm
Music_Player
```

如果页面没有进入产品功能闭环，先从入口禁用，再从 CMake 移除，再删除资源。

## 6. 配置项清理建议

产品版建议固定或默认关闭：

```text
CONFIG_XIAOZHI_ENABLE_WEBSOCKET_PROTOCOL=n
CONFIG_USE_EMOTE_MESSAGE_STYLE=n
CONFIG_FLASH_CUSTOM_ASSETS=n
CONFIG_USE_ESP_WAKE_WORD=n  # 如果产品阶段暂不做离线唤醒
```

产品版建议固定：

```text
CONFIG_BOARD_TYPE_ESP_SENSAIRSHUTTLE=y
CONFIG_IDF_TARGET="esp32c5"
```

注意：`config.json` 只是构建预设，最终必须确认实际 `sdkconfig` 生效。

## 7. 不要轻易删除

以下内容先不要删：

```text
managed_components/
dependencies.lock
partitions/
main/ota.cc
main/settings.cc
main/assets.cc
main/audio/audio_service.*
main/protocols/mqtt_protocol.*
main/boards/common/wifi_board.*
```

这些模块虽然看起来不是产品 UI，但可能是联网、配置、音频、资源、OTA 的基础链路。

## 8. 每轮清理验收标准

每一轮清理后至少完成：

```powershell
idf.py build
```

涉及底层运行链路时，还要完成：

```powershell
idf.py -p COMx flash monitor
```

串口日志至少确认：

```text
Boot OK
Wi-Fi connected
OTA check OK
MQTT connected
Activation done
进入 Call / Listening 不崩溃
无明显 bad_alloc
无明显 esp-aes memory error
```

## 9. 当前优先级

当前最推荐的下一步：

```text
1. 先整理 CMake，只编译 esp-sensairshuttle 当前产品真正需要的源码。
2. 再整理 Kconfig，只暴露当前产品相关配置。
3. build 成功后，再分组删除其他板子和无关源码。
```

不要让 Codex 一次性执行“大清仓”。产品工程清理必须小步提交、小步验证。