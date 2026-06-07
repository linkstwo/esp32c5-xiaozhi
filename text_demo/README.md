# 智感桌面伴侣 — 项目总纲

## 项目目标

将 `factory_demo` 出厂演示固件改造为**智感桌面伴侣** —— 具备环境监测、AI 语音对话、云端数据管理的桌面级智能设备，实现 "感知 → 预警 → 服务" 完整闭环。

| 核心功能 | 描述 |
|----------|------|
| 环境监测 | 温度、湿度、气压、IAQ 空气质量实时采集与可视化 |
| 异常预警 | 环境参数超标时，RGB LED 变色 + 屏幕提示 |
| AI 语音对话 | 基于火山引擎硬件对话智能体，自然语言询问环境问题 |
| 云端数据 | 加密时序存储，趋势分析，可视化数据看板 |
| 触摸交互 | LCD 触摸屏操作，查看各项环境指标 |

## 硬件平台

| 项目 | 规格 |
|------|------|
| 主控芯片 | ESP32-C5-WROOM-1-N16R8（双核 RISC-V，16MB Flash，8MB PSRAM） |
| 开发板 | ESP-SensairShuttle v1.3.0 |
| 显示 | ILI9341 LCD，320×240，SPI（MOSI:23, SCLK:24, CS:25, DC:26） |
| 触摸 | CST816S 电容触摸，I2C（SDA:2, SCL:3, 地址 0x15） |
| 环境传感器 | BME690（温度/湿度/气压/气体），I2C 地址 0x76 |
| IMU | BMI270（6轴加速度计+陀螺仪），I2C 地址 0x68 |
| 地磁 | BMM350（3轴磁力计），I2C 地址 0x14 |
| 通信 | Wi-Fi 6 (2.4GHz) + BLE 5.0（ESP32-C5内置） |

## 开发环境

- **构建系统**: ESP-IDF v5.5+ / CMake
- **图形库**: LVGL v9.2.2
- **UI 工具**: SquareLine Studio 1.5.3
- **包管理**: IDF Component Manager (bmgr)
- **语言**: C++17（主应用层）、C（驱动层）
- **构建命令**: `idf.py bmgr -b esp_SensairShuttle -c boards` → `idf.py build`

## 分层架构

```
应用层: main.cpp → Brookesia Phone 框架 → 各 App（Environment/Compass/2048/GestureDetect/FactoryGuide）
        ⬇ 改造后目标
应用层: 环境仪表盘 UI(LVGL) + AI对话UI + 云端数据管理
服务层: 传感器服务(BME690+BSEC) / 语音服务(I2S) / 通信服务(Wi-Fi+MQTT)
驱动层: BME690(bme69x) + BSEC IAQ / BMI270 / BMM350 / ILI9341 + CST816S
硬件抽象层: esp_board_manager（I2C/SPI/GPIO 外设统一管理）
硬件层: ESP32-C5 + 传感器 + LCD + 触摸
```

## 数据流

```
BME690 ──I2C──> BSEC IAQ算法 ──> LVGL UI 显示
                  │
                  ├──> 本地缓存(NVS)
                  │
                  └──> MQTT ──> 云端数据库

用户语音 ──I2S──> 音频采集 ──> 火山引擎 API ──> AI 回复 ──> 扬声器播放
```

## 线程模型

| 线程 | 优先级 | 栈大小 | 职责 |
|------|--------|--------|------|
| LVGL Task | 4 | 20KB | GUI 渲染 |
| Sensor Task | 3 | 16KB | BME690 定时采集 + BSEC 运算 |
| Network Task | 2 | 12KB | Wi-Fi 管理 + MQTT 通信（待开发） |
| Voice Task | 3 | 16KB | 音频采集 + 火山引擎 API（待开发） |
| LED Task | 1 | 4KB | RGB LED 状态刷新（待开发） |

## 软件模块清单

### 已集成可复用

| 模块 | 路径 | 功能 |
|------|------|------|
| BME690 SensorAPI | `common_components/brookesia_app_temperature/BME690_SensorAPI/` | Bosch 官方 bme69x 驱动 |
| BSEC IAQ | `common_components/brookesia_app_temperature/bsec_IAQ/` | Bosch 环境算法库(闭源.a) |
| BMI270 驱动 | `managed_components/espressif__bmi270_sensor/` | 6轴IMU驱动 |
| BMM350 SensorAPI | `common_components/brookesia_app_compass/BMM350_SensorAPI/` | 磁力计驱动 |
| esp_lvgl_port | `managed_components/espressif__esp_lvgl_port/` | LVGL集成层 |
| lvgl | `managed_components/lvgl__lvgl/` | LVGL图形库 |
| esp_board_manager | `managed_components/espressif__esp_board_manager/` | 板级HAL |
| esp-lib-utils | `common_components/espressif__esp-lib-utils/` | 日志/错误检查宏 |
| esp-boost | `managed_components/espressif__esp-boost/` | C++ Boost线程 |
| i2c_bus | `managed_components/espressif__i2c_bus/` | I2C总线驱动 |

### 计划新增（第一阶段）

| 模块 | 功能 |
|------|------|
| sensor_service | BME690 + BSEC 独立传感器服务类 |
| led_indicator | RGB LED 状态指示灯驱动 |
| dashboard_ui | 环境仪表盘独立 LVGL UI |

### 计划新增（后续阶段）

| 模块 | 功能 |
|------|------|
| network_service | Wi-Fi配网 + MQTT客户端 |
| voice_service | I2S音频采集 + 火山引擎API |
| data_manager | 传感器数据缓存/打包/上报 |

### 计划移除

| 模块 | 原因 |
|------|------|
| brookesia_app_game_2048 | 与新项目无关 |
| brookesia_app_factory_guide | 与新项目无关 |
| brookesia_app_gesture_detect | 手势识别不做 |
| brookesia_system_phone | Phone桌面框架替换为独立UI |
| brookesia_system_core | 随Phone框架移除 |

## 目标文件结构

```
factory_demo/
├── main/
│   ├── main.cpp                   # 入口，独立LVGL初始化
│   ├── display.cpp/hpp            # 显示初始化（保留简化）
│   ├── idf_component.yml
│   └── CMakeLists.txt
├── components/                    # 项目自定义组件
│   ├── sensor_service/            # [新增] 传感器采集服务
│   ├── network_service/           # [新增] Wi-Fi/MQTT
│   ├── voice_service/             # [新增] 语音交互
│   ├── led_indicator/             # [新增] RGB LED
│   └── ui/                        # [新增] 环境仪表盘UI
├── common_components/             # 保留的公共组件
│   ├── brookesia_app_temperature/ # BME690驱动+BSEC（提取驱动层）
│   └── espressif__esp-lib-utils/  # 工具库
├── boards/esp_SensairShuttle/     # 板级配置
└── managed_components/            # IDF组件管理器
```

## 不可改动核心区域

1. `boards/esp_SensairShuttle/` — 板级硬件配置，由 bmgr 自动管理
2. `common_components/*/BME690_SensorAPI/` — Bosch 官方驱动，禁止修改
3. `common_components/*/BMM350_SensorAPI/` — Bosch 官方驱动，禁止修改
4. `common_components/*/bsec_IAQ/` — Bosch 闭源算法库(.a)，禁止修改
5. `managed_components/` — IDF 组件管理器托管，禁止手动修改
6. `setup_device.c` — LCD/Touch 工厂入口，仅板级适配时修改

## 参考资料

- [ESP-IDF 编程指南](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32c5/)
- [BME690 数据手册](https://www.bosch-sensortec.com/products/environmental-sensors/gas-sensors/bme690/)
- [BSEC 集成指南](https://www.bosch-sensortec.com/software-tools/software/bme688-software/)
- [火山引擎硬件对话智能体](https://www.volcengine.com/product/hardware-conversational-agent)
- [LVGL 文档](https://docs.lvgl.io/)
