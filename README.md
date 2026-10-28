# ESP32-C5 小智语音终端｜联网对话与触摸屏交互

基于 XiaoZhi ESP32 开源项目定制的 **ESP32-C5 语音终端固件**：通过麦克风采集声音，经 Wi-Fi 与小智服务交互，播放服务端返回的语音，并通过 LCD 触摸屏显示状态和提供本地交互。

项目方向是从上游多开发板工程逐步整理成单板产品。当前主要板型为 `esp-sensairshuttle`；仓库仍保留部分上游板型与通用模块，裁剪尚未全部完成。

## 对话是怎么完成的

```text
ADC 麦克风 → 音频采集 → OPUS 编码
                         ↓
                  Wi-Fi / UDP 音频上传
                         ↓
              服务端语音识别 → 大模型 → 语音合成
                         ↓
                UDP 下发 → OPUS 解码 → PDM 播放

MQTT：连接、会话与控制消息
LCD + CST816S 触摸：本地状态显示与交互
```

设备负责采音、传输、播放和界面交互；语音识别、大模型推理与语音合成依赖服务端。主链路为 **Wi-Fi → OTA/服务配置 → MQTT 会话 → UDP 音频通道**，并非在 ESP32-C5 本地运行完整大模型。

## 当前硬件与软件方向

| 部分 | 当前仓库内容 |
| --- | --- |
| 主控与构建 | ESP32-C5，ESP-IDF / CMake |
| 板型 | `main/boards/esp-sensairshuttle/` |
| 音频输入 / 输出 | ADC 麦克风 / PDM 音频输出 |
| 通信 | Wi-Fi、MQTT + UDP、OPUS |
| 本地交互 | SPI LCD、CST816S 触摸、LVGL 界面 |
| 运行组织 | 应用状态机、音频服务、网络协议与板级驱动 |

LCD 实际初始化和引脚以 [板级实现](main/boards/esp-sensairshuttle/esp-sensairshuttle.cc) 与 [config.h](main/boards/esp-sensairshuttle/config.h) 为准；不要仅凭通用开发板名称推断接线。

## 从哪里开始

| 想做什么 | 入口 |
| --- | --- |
| 查看板型与构建选项 | [板型说明](main/boards/esp-sensairshuttle/README.md)、[config.json](main/boards/esp-sensairshuttle/config.json) |
| 查看硬件引脚与音频参数 | [config.h](main/boards/esp-sensairshuttle/config.h) |
| 理解应用状态流转 | [main/application.cc](main/application.cc) |
| 阅读音频采集与播放 | [main/audio/audio_service.cc](main/audio/audio_service.cc) |
| 理解会话与音频传输 | [MQTT/UDP 协议说明](docs/mqtt-udp_zh.md)、[协议实现](main/protocols/mqtt_protocol.cc) |
| 查看 ADC/PDM 驱动 | [adc_pdm_audio_codec.cc](main/boards/esp-sensairshuttle/adc_pdm_audio_codec.cc) |
| 继续修改代码前 | [AGENTS.md](AGENTS.md) |

## 构建与烧录

先准备匹配本工程的 ESP-IDF 环境与组件依赖，并按板型的 `config.json` 配置 `esp32c5` 目标及 `esp-sensairshuttle` 选项。板型说明提供 `scripts/release.py` 构建入口；手动配置后使用：

```powershell
idf.py build
idf.py -p COMx flash monitor
```

其中 `COMx` 替换为实际串口。服务地址、激活/服务配置与网络条件也会影响语音链路，固件编译通过本身不能证明对话成功。

## 调试重点与当前状态

重点观察：Wi-Fi 与 MQTT 连接、server hello、UDP 通道建立、麦克风 PCM 输入、OPUS 上下行、语音播放，以及关键状态下的剩余 SRAM。已有调试关注点还包括 `std::bad_alloc`、AES 内存分配失败与 LVGL 页面切换异常。

本次说明更新核对了仓库结构、板型配置、驱动和 MQTT/UDP 实现，**未重新执行固件构建、烧录或真实语音通话**。涉及音频、显示、网络和内存的修改，仍需在目标板上记录完整日志验证。

## 上游与许可证

基于 XiaoZhi ESP32 开源项目进行定制，保留仓库中的 [MIT License](LICENSE) 与源文件声明。
