# ESP32-C5 XiaoZhi Board

这是一个面向 **ESP32-C5 小智板产品化** 的固件工程。

本仓库基于 XiaoZhi ESP32 开源项目裁剪和定制，目标不是继续保留上游“多开发板通用大仓库”的形态，而是逐步整理成一个清晰、稳定、可维护的 **单板单产品工程**。

## 产品定位

当前产品目标：

- 使用 ESP32-C5 作为主控
- 通过 Wi-Fi 连接小智服务
- 使用 MQTT + UDP 作为实时语音通信链路
- 使用 OPUS 进行语音编解码
- 使用 ADC 麦克风采集语音
- 使用 PDM 喇叭播放语音
- 使用 SPI LCD + 触摸屏作为本地交互界面
- 支持基础状态显示、通话入口、调试信息展示和后续产品功能扩展

## 当前硬件方向

当前主板型：

```text
esp-sensairshuttle
```

当前核心硬件链路：

```text
Chip: ESP32-C5
Display: ILI9341 SPI LCD
Touch: CST816S
Mic: ADC input
Speaker: PDM output
Network: Wi-Fi
Protocol: MQTT + UDP
Audio codec: OPUS
```

## 当前核心运行链路

产品主链路优先级如下：

```text
Boot
-> Wi-Fi
-> OTA / 服务端配置获取
-> MQTT 连接
-> UDP 音频通道
-> 麦克风采集
-> OPUS 编码上传
-> 服务端 ASR / LLM / TTS
-> OPUS 音频下发
-> 本地解码播放
-> 屏幕状态显示
```

现阶段所有代码整理、裁剪和优化，都应该优先保证这条链路稳定。

## 当前保留的主要模块

短期内应优先保留：

- `main/boards/esp-sensairshuttle/`
- Wi-Fi 联网相关代码
- OTA / settings 配置读取
- MQTT + UDP 协议代码
- AudioService / OPUS 编解码链路
- ADC 麦克风 + PDM 喇叭驱动
- LCD + Touch + SmartGadgetDisplay
- 基础状态机与系统信息输出
- 必要的分区表、依赖锁定和 ESP-IDF 工程结构

## 正在裁剪的上游通用内容

本仓库会逐步从上游通用工程中裁剪掉与当前产品无关的内容，例如：

- 其他开发板支持
- 4G / ML307 / NT26 等非当前产品网络路径
- WebSocket 协议路径
- 非当前硬件使用的音频 codec
- 非当前产品使用的显示类型
- 多语言演示配置
- 上游宣传文档、硬件展示图片和无关教程链接
- 未进入当前产品规划的 UI 页面和资源

具体执行清单见：

```text
PRODUCT_CLEANUP.md
```

## 构建与烧录

本工程仍沿用 ESP-IDF 项目结构。

常用命令：

```powershell
idf.py build
idf.py -p COMx flash monitor
```

其中 `COMx` 替换为实际串口号。

建议每次修改后至少执行：

```powershell
idf.py build
```

涉及底层音频、Wi-Fi、显示、分区、配置项修改时，需要重新烧录并抓取完整串口日志。

## 调试重点

当前产品调试重点：

- `Activation done` 后剩余 SRAM
- 进入 `listening` 前剩余 SRAM
- MQTT 是否能稳定收到 server hello
- UDP 音频通道是否能正常创建
- 是否出现 `std::bad_alloc`
- 是否出现 `esp-aes: Failed to allocate memory`
- 是否出现 LVGL 页面切换崩溃
- 麦克风是否有 PCM 输入
- 喇叭是否能稳定播放 TTS 音频

## 仓库整理原则

产品化整理时遵循以下原则：

1. 先改构建入口，再删源码文件。
2. 一组功能一组功能地删，不一次性大删。
3. 每删一组都必须 build。
4. 能不动 `managed_components` 就不要动。
5. 不动 `dependencies.lock`，除非确认依赖变化。
6. 不动分区表，除非明确要重新规划 OTA / assets / storage。
7. 主线目标永远优先于演示功能。

## 上游说明

本工程基于 XiaoZhi ESP32 开源项目进行产品化整理。原项目使用 MIT License。本仓库会保留必要的上游声明和许可证信息。