# 开发进度

## 已完成功能

- [x] ESP32-C5 板级配置（bmgr + board YAML）
- [x] ILI9341 LCD 驱动 + LVGL 端口适配（SPI, 320x240）
- [x] CST816S 触摸驱动（I2C, 地址 0x15, 坐标交换+镜像）
- [x] ~~Brookesia Phone 桌面框架集成~~ — 已移除
- [x] ~~5个App（Environment/Compass/GestureDetect/2048/FactoryGuide）~~ — 已移除
- [x] **第一阶段：核心架构重构**
  - [x] 移除 Brookesia Phone 框架（brookesia_system_phone + brookesia_system_core）
  - [x] 移除无关 App（2048/工厂指南/手势识别/指南针/温度）
  - [x] 提取 BME690 驱动为独立组件 `components/bme690_sensor/`
  - [x] 提取 BSEC IAQ 库为独立组件 `components/bsec_iaq/`（只选一个配置，修复 GLOB_RECURSE 问题）
  - [x] 创建 `components/sensor_service/`：FreeRTOS 传感器采集任务 + BSEC NVS 持久化 + 湿度滑动平均
  - [x] 创建 `components/ui/`：双屏 LVGL 界面
    - 主屏：实时时钟 + 日期 + 可爱角色动画（眨眼/呼吸） + 语音按钮（Toast 占位）
    - 传感器屏：温湿度大字 + 气压趋势 + IAQ 弧形指示器（颜色等级）
    - 左右滑动手势切换 + 页面圆点指示器 + 滑动动画
  - [x] 重写 main.cpp 为独立 LVGL 入口，精简依赖树

## 当前调试问题

- [ ] **构建未通过**：idf.py 不在当前 shell PATH 中，需在 ESP-IDF CMD/PowerShell 中执行构建
- [ ] BSEC IAQ 校准时间长（首次 5-30 分钟），期间 UI 显示不准确
- [ ] BSEC 状态 NVS 持久化已实现，待实机验证
- [ ] 湿度滑动平均窗已实现，待实机验证

## 现存 BUG

- (暂无记录)

## 待开发任务

### 第一阶段：核心环境监测（继续调试）

- [ ] **构建验证**：在 ESP-IDF 环境中执行 `idf.py build`，修复编译错误
- [ ] **实机验证**：烧录测试双屏切换、传感器数据刷新、动画效果
- [ ] **BSEC 持久化**：验证重启后 BSEC 状态从 NVS 正确恢复
- [ ] RGB LED 状态指示灯驱动（GPIO 或 RMT WS2812）

### 第二阶段：网络通信（预计 3-5 天，暂未启动）

- [ ] Wi-Fi 配网与管理模块
- [ ] MQTT 客户端集成
- [ ] 环境数据云端上报（定时 + 变化触发）
- [ ] 远程配置下发

### 第三阶段：AI 语音交互（预计 5-7 天，暂未启动）

- [ ] I2S MEMS 麦克风驱动
- [ ] 音频采集与降噪
- [ ] 火山引擎硬件对话智能体对接
- [ ] TTS 语音合成播放

### 第四阶段：完善与优化（预计 2-3 天，暂未启动）

- [ ] UI 交互优化
- [ ] 低功耗策略
- [ ] BLE 近场配网
- [ ] 云端数据看板（Web 端）

## 临时搁置

- 第二~四阶段全部任务 — 待第一阶段构建验证通过后依次启动
