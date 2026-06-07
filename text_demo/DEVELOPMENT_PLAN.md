# 智感桌面伴侣 — 智能环境监测助手

## 项目开发方案

> 基于 ESP32-C5 的多模态交互桌面级环境监测设备，实现 "感知 - 预警 - 服务" 的完整闭环。

---

## 一、项目概述

### 1.1 目标

将现有 `factory_demo`（出厂演示固件）改造为**智感桌面伴侣** —— 一款具备环境监测、AI 语音对话、云端数据管理的桌面级智能设备。

### 1.2 核心功能

| 功能 | 描述 |
|---|---|
| 环境监测 | 温度、湿度、气压、IAQ 空气质量实时采集与可视化 |
| 异常预警 | 环境参数超标时，RGB LED 变色 + 屏幕提示 |
| AI 语音对话 | 基于火山引擎硬件对话智能体，自然语言询问环境问题 |
| 云端数据 | 加密时序存储，趋势分析，可视化数据看板 |
| 触摸交互 | LCD 触摸屏操作，查看各项环境指标 |

### 1.3 硬件平台

- **主控**: ESP32-C5-WROOM-1-N16R8（双核 RISC-V，16MB Flash，8MB PSRAM）
- **显示**: ILI9341 LCD（320×240，SPI）
- **触摸**: CST816S 电容触摸（I2C，地址 0x15）
- **环境传感器**: BME690（温度/湿度/气压/气体，I2C，地址 0x76）
- **IMU**: BMI270（6 轴加速度计+陀螺仪，I2C，地址 0x68）
- **地磁**: BMM350（3 轴磁力计，I2C）
- **通信**: Wi-Fi 6 + BLE 5.0（ESP32-C5 内置）

---

## 二、现有资源盘点

### 2.1 硬件资源（已确认可用）

| 设备 | 类型 | 接口 | 状态 |
|---|---|---|---|
| `display_lcd` | ILI9341 LCD | SPI (MOSI:23, SCLK:24, CS:25, DC:26) | 已配置，已跑通 |
| `lcd_touch` | CST816S 触摸 | I2C (SDA:2, SCL:3, addr:0x15) | 已配置，已跑通 |
| BME690 | 环境传感器 | I2C (addr:0x76) | 驱动完整，App 已验证 |
| BMI270 | 6轴IMU | I2C (addr:0x68) | 驱动完整（managed_components） |
| BMM350 | 3轴磁力计 | I2C | 驱动完整（common_components） |
| Wi-Fi 6 | ESP32-C5 内置 | - | 硬件支持，需软件启用 |
| BLE 5.0 | ESP32-C5 内置 | - | 硬件支持，需软件启用 |

### 2.2 软件模块清单

#### 已集成且可复用

| 模块 | 路径 | 功能 | 复用计划 |
|---|---|---|---|
| **BME690 SensorAPI** | `common_components/brookesia_app_temperature/BME690_SensorAPI/` | Bosch 官方 BME69x 驱动 (bme69x.c/h) | 直接复用 |
| **BSEC IAQ** | `common_components/brookesia_app_temperature/bsec_IAQ/` | Bosch 环境算法库（IAQ/CO2/VOC） | 直接复用 |
| **BMI270 驱动** | `managed_components/espressif__bmi270_sensor/` | BMI270 驱动及动作识别库 | 按需使用 |
| **BMM350 SensorAPI** | `common_components/brookesia_app_compass/BMM350_SensorAPI/` | BMM350 磁力计驱动 | 按需使用 |
| **esp_lvgl_port** | `managed_components/espressif__esp_lvgl_port/` | LVGL 与 ESP-IDF 集成层 | 直接复用 |
| **lvgl** | `managed_components/lvgl__lvgl/` | LVGL 图形库 | 直接复用 |
| **esp_board_manager** | `managed_components/espressif__esp_board_manager/` | 板级硬件抽象层 | 直接复用 |
| **esp-lib-utils** | `common_components/espressif__esp-lib-utils/` | 日志/错误检查/工具宏 | 直接复用 |
| **esp-boost** | `managed_components/espressif__esp-boost/` | C++ Boost 线程库 | 直接复用 |
| **i2c_bus** | `managed_components/espressif__i2c_bus/` | I2C 总线驱动 | 直接复用 |

#### 需要新增

| 模块 | 功能 | 实现方式 |
|---|---|---|
| **Wi-Fi 管理层** | 配网、连接、重连 | ESP-IDF Wi-Fi API + 智能配网 |
| **HTTP/MQTT 客户端** | 云端数据上报与接收 | ESP-IDF esp_http_client / mqtt |
| **火山引擎语音 SDK** | 端侧语音采集 + 云端 AI 对话 | 对接火山引擎硬件对话智能体 API |
| **音频采集** | MEMS 麦克风 I2S 输入 | ESP-IDF I2S PDM 驱动 |
| **音频播放** | 扬声器 I2S 输出 + MP3 解码 | ESP-IDF I2S + esp_adf 或简易解码 |
| **RGB LED 驱动** | 环境状态指示灯 | GPIO 或 RMT (WS2812) |
| **数据管理模块** | 传感器数据缓存、打包、上报 | C++ 类，定时任务 |
| **新 LVGL UI** | 环境仪表盘界面 | 基于 LVGL 自定义开发 |

#### 计划移除

| 模块 | 原因 |
|---|---|
| `brookesia_app_game_2048` | 2048 游戏，与新项目无关 |
| `brookesia_app_factory_guide` | 工厂指南二维码，与新项目无关 |
| `brookesia_app_gesture_detect` | 手势识别，不做拍打唤醒 |
| `brookesia_system_phone` | Phone 桌面框架，替换为独立 UI |
| `brookesia_system_core` | 随 Phone 框架移除 |

---

## 三、技术架构

### 3.1 分层架构

```
┌─────────────────────────────────────────────────────┐
│                    应用层 (Application)              │
│  ┌───────────┐ ┌───────────┐ ┌───────────────────┐  │
│  │ 环境仪表盘 │ │ AI 对话   │ │ 云端数据管理      │  │
│  │ (LVGL UI) │ │ (火山引擎) │ │ (MQTT/HTTP)      │  │
│  └───────────┘ └───────────┘ └───────────────────┘  │
├─────────────────────────────────────────────────────┤
│                   服务层 (Service)                   │
│  ┌───────────┐ ┌───────────┐ ┌───────────────────┐  │
│  │ 传感器服务 │ │ 语音服务  │ │ 通信服务           │  │
│  │ (BME690)  │ │ (I2S音频) │ │ (Wi-Fi/MQTT)      │  │
│  └───────────┘ └───────────┘ └───────────────────┘  │
├─────────────────────────────────────────────────────┤
│                   驱动层 (Driver)                     │
│  ┌───────────┐ ┌───────────┐ ┌───────────────────┐  │
│  │ BME690    │ │ BMI270    │ │ LCD + Touch + LED │  │
│  │ bme69x    │ │ bmi2      │ │ ili9341 + cst816s │  │
│  └───────────┘ └───────────┘ └───────────────────┘  │
├─────────────────────────────────────────────────────┤
│              硬件抽象层 (HAL)                         │
│  esp_board_manager (I2C/SPI/GPIO 外设管理)          │
├─────────────────────────────────────────────────────┤
│              硬件层 (Hardware)                        │
│  ESP32-C5 + BME690 + ILI9341 + CST816S + ...        │
└─────────────────────────────────────────────────────┘
```

### 3.2 数据流

```
BME690 ──I2C──> 传感器服务 ──> 数据管理模块 ──> LVGL UI 显示
                              │
                              ├──> 本地缓存
                              │
                              └──> MQTT ──> 云端数据库
                                              │
用户语音 ──I2S──> 音频采集 ──> 火山引擎 API ──> AI 回复 ──> 扬声器播放
                                              │
                                      云端数据查询 ──> 语音播报
```

### 3.3 线程模型

| 线程 | 优先级 | 栈大小 | 职责 |
|---|---|---|---|
| LVGL Task | 4 | 20KB | GUI 渲染 |
| Sensor Task | 3 | 16KB | BME690 定时采集 + BSEC 运算 |
| Network Task | 2 | 12KB | Wi-Fi 管理 + MQTT 通信 |
| Voice Task | 3 | 16KB | 音频采集 + 火山引擎 API |
| LED Task | 1 | 4KB | RGB LED 状态刷新 |

---

## 四、开发阶段

### 第一阶段：核心环境监测（预计 3-5 天）

- [ ] 移除不必要的 Brookesia 组件（2048、Factory Guide、Gesture Detect、Phone）
- [ ] 简化 main.cpp，去除 Phone 框架，保留 LVGL 初始化
- [ ] 提取 BME690 传感器服务为独立模块
- [ ] 开发环境仪表盘 LVGL UI
  - 温度/湿度显示
  - 气压 + 天气推测
  - IAQ 空气质量显示
- [ ] RGB LED 状态指示灯驱动

### 第二阶段：网络通信（预计 3-5 天）

- [ ] Wi-Fi 配网与管理模块
- [ ] MQTT 客户端集成
- [ ] 环境数据云端上报（定时 + 变化触发）
- [ ] 远程配置下发

### 第三阶段：AI 语音交互（预计 5-7 天）

- [ ] I2S MEMS 麦克风驱动
- [ ] 音频采集与降噪
- [ ] 火山引擎硬件对话智能体对接
- [ ] 语音唤醒（可选）
- [ ] TTS 语音合成播放

### 第四阶段：完善与优化（预计 2-3 天）

- [ ] UI 交互优化
- [ ] 低功耗策略
- [ ] BLE 近场配网
- [ ] 云端数据看板（Web 端）

---

## 五、关键接口说明

### 5.1 BME690 传感器接口

```c
// 现有接口 (bme69x.h)
int8_t bme69x_init(struct bme69x_dev *dev);
int8_t bme69x_get_conf(struct bme69x_conf *conf, struct bme69x_dev *dev);
int8_t bme69x_set_conf(struct bme69x_conf *conf, struct bme69x_dev *dev);
int8_t bme69x_set_heatr_conf(uint8_t op_mode, struct bme69x_heatr_conf *conf, struct bme69x_dev *dev);
int8_t bme69x_get_data(uint8_t op_mode, struct bme69x_data *data, uint8_t *n_data, struct bme69x_dev *dev);

// 数据输出结构
struct bme69x_data {
    float temperature;    // 温度 (°C)
    float pressure;       // 气压 (hPa)
    float humidity;       // 湿度 (%RH)
    float gas_resistance; // 气体电阻 (Ω)
    // ...
};
```

### 5.2 BSEC 环境算法接口

```c
// BSEC 输出 (bsec_datatypes.h)
typedef struct {
    int64_t timestamp;
    float temperature;
    float humidity;
    float pressure;
    float iaq;              // IAQ 指数 (0-500)
    uint8_t iaq_accuracy;   // 精度等级 (0-3)
    float static_iaq;
    float co2_equivalent;   // CO2 等效浓度 (ppm)
    float breath_voc_equivalent; // 呼吸 VOC 等效浓度 (ppm)
    float gas_percentage;
    // ...
} bsec_output_t;
```

### 5.3 I2C 总线配置

```
I2C_NUM_0, 100kHz
SDA: GPIO 2
SCL: GPIO 3
设备:
  - BME690:   0x76 (或 0x77)
  - BMI270:   0x68 (或 0x69)
  - BMM350:   0x14 (或 0x15)
  - CST816S:  0x15
```

---

## 六、文件结构规划

```
factory_demo/
├── main/                          # 主程序
│   ├── main.cpp                   # 入口，初始化各模块
│   ├── display.cpp/hpp            # 显示初始化（保留，简化）
│   ├── idf_component.yml          # 依赖配置
│   └── CMakeLists.txt             # 构建配置
├── components/                    # 项目自定义组件
│   ├── sensor_service/            # [新增] 传感器采集服务
│   │   ├── sensor_service.cpp/hpp
│   │   └── CMakeLists.txt
│   ├── network_service/           # [新增] Wi-Fi/MQTT 通信服务
│   │   ├── network_service.cpp/hpp
│   │   └── CMakeLists.txt
│   ├── voice_service/             # [新增] 语音交互服务
│   │   ├── voice_service.cpp/hpp
│   │   └── CMakeLists.txt
│   ├── led_indicator/             # [新增] RGB LED 驱动
│   │   ├── led_indicator.cpp/hpp
│   │   └── CMakeLists.txt
│   ├── ui/                        # [新增] 环境仪表盘 UI
│   │   ├── dashboard_ui.cpp/hpp
│   │   ├── screens/
│   │   └── CMakeLists.txt
│   └── gen_bmgr_codes/            # [bmgr 自动生成] 板级配置
├── common_components/             # 保留的公共组件
│   ├── brookesia_app_compass/     # 指南针（可选保留）
│   ├── brookesia_app_temperature/ # BME690 驱动 + BSEC（提取驱动层）
│   └── espressif__esp-lib-utils/  # 工具库
├── boards/                        # 板子配置
│   └── esp_SensairShuttle/
└── managed_components/            # IDF 组件管理器
```

---

## 七、注意事项

### 7.1 构建规范

1. **必须先运行 bmgr**：`idf.py bmgr -b esp_SensairShuttle -c boards`
2. **Windows 编码问题**：如果直接运行 Python 脚本，设置 `PYTHONIOENCODING=utf-8`
3. **sdkconfig 更新**：添加新 Kconfig 选项后，删除 sdkconfig 重新生成
4. **依赖管理**：新增驱动依赖通过 `idf_component.yml` 或 bmgr 板子 YAML 管理

### 7.2 已知技术约束

1. **BSEC IAQ 校准**：首次使用需 5-30 分钟初始校准，24 小时后精度最高
2. **BSEC 状态持久化**：当前未实现，重启后需重新校准（需开发状态保存到 NVS）
3. **栈空间**：BSEC 线程需要至少 16KB 栈空间
4. **ESP32-C5 限制**：Wi-Fi 6 仅支持 2.4GHz，不支持 5GHz

---

## 八、参考资料

- [ESP-IDF 编程指南](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32c5/)
- [BME690 数据手册](https://www.bosch-sensortec.com/products/environmental-sensors/gas-sensors/bme690/)
- [BSEC 集成指南](https://www.bosch-sensortec.com/software-tools/software/bme688-software/)
- [火山引擎硬件对话智能体](https://www.volcengine.com/product/hardware-conversational-agent)
- [LVGL 文档](https://docs.lvgl.io/)
- [ESP Board Manager 文档](managed_components/espressif__esp_board_manager/README.md)
