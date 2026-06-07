# 硬件+代码硬性规约

## 引脚分配（固定，禁止随意修改）

| 外设 | 引脚 | 说明 |
|------|------|------|
| I2C SDA | GPIO 2 | 共享总线，挂载 BME690/BMI270/BMM350/CST816S/BS8112 |
| I2C SCL | GPIO 3 | 共享总线 |
| LCD MOSI | GPIO 23 | SPI 数据 |
| LCD SCLK | GPIO 24 | SPI 时钟 |
| LCD CS | GPIO 25 | SPI 片选 |
| LCD DC | GPIO 26 | SPI 数据/命令 |
| PA Control | GPIO 1 | 功放控制，默认高电平 |
| BME690 SDO | GPIO 9 | 地址选择，接低电平→地址 0x76 |

## I2C 地址（固定）

| 设备 | 地址 |
|------|------|
| BME690 | 0x76（SDO=GND） |
| BMI270 | 0x68 |
| BMM350 | 0x14 |
| CST816S | 0x15 |
| BS8112 | I2C 自定义设备 |

## 通信协议参数

| 参数 | 值 |
|------|------|
| I2C 端口 | I2C_NUM_0 |
| I2C 频率 | 100 kHz |
| SPI 端口 | SPI2_HOST |
| SPI 模式 | Mode 3 |
| SPI 像素时钟 | 20 MHz |
| LVGL 色深 | 16 bit (LV_COLOR_DEPTH=16) |

## BSEC 环境算法配置

| 参数 | 值 |
|------|------|
| 采样率 | BSEC_SAMPLE_RATE_LP（每3秒一次） |
| 温度偏移 | 5.0°C（自热补偿） |
| 状态保存周期 | 6小时 |
| 加热温度 | 300°C（BSEC自动调节） |
| 加热时长 | 100ms（BSEC自动调节） |
| 最小栈空间 | 4096 bytes（thread） |

## 编码规范

1. 应用层使用 C++17，驱动层使用 C
2. 命名风格：类名 PascalCase，函数名 camelCase，变量名 snake_case
3. 全局 UI 变量前缀 `ui_`，由 SquareLine Studio 生成，**禁止手动改名**
4. 日志使用 `ESP_LOGI/E/W/D` + `ESP_UTILS_LOG_TAG` 宏
5. 错误检查使用 `ESP_UTILS_CHECK_*` 系列宏（esp-lib-utils）
6. LVGL 操作必须持锁：非 GUI Task 用 `LvLockGuard`，GUI Task 内不用
7. I2C 总线由 `esp_board_manager` 统一管理，**禁止直接调用 i2c_new_master_bus 创建第二个实例**

## 线程栈空间约束（防止溢出）

| 线程 | 最小栈 | 说明 |
|------|--------|------|
| LVGL Task | 20KB | GUI渲染，含双缓冲区 |
| BSEC Sensor Thread | 4KB (Boost thread) | BSEC循环采集，当前代码配置 |
| BSEC Task（未来重构） | 16KB | 若改为 FreeRTOS Task，BSEC 需要16KB |
| Boost mem_info Thread | 4KB | 内存监控 |

## 已知技术约束

1. **BSEC IAQ 校准**: 首次使用需 5-30 分钟初始校准，24 小时后精度最高
2. **BSEC 状态持久化**: 当前未实现，重启后需重新校准（需 NVS 存取 bsec_state blob）
3. **ESP32-C5 Wi-Fi 6**: 仅支持 2.4GHz 频段，不支持 5GHz
4. **I2C 总线共享**: 所有 I2C 设备挂在同一总线 I2C_NUM_0（100kHz），确保无地址冲突
5. **BME690 SDO 引脚**: 必须拉低（GPIO9=0）才能使用地址 0x76
6. **sdkconfig 更新**: 新增 Kconfig 选项后必须删 sdkconfig 重新生成

## 禁止使用写法

1. `vTaskDelay` 在 LVGL Task 中 — 会造成 GUI 卡死
2. `lv_obj_t*` 全局变量被 SquareLine 重命名 — 必须用生成的名字
3. BSEC 线程栈 < 4KB — 会栈溢出
4. `i2c_new_master_bus` 重复创建 I2C_NUM_0 — 与 board_manager 冲突
5. `printf/cout` — 统一用 ESP_LOG 宏
6. `new/delete` 不加 `std::nothrow` — ESP-IDF 默认禁异常
7. 直接操作 SPI/I2C 底层寄存器 — 必须通过 esp_board_manager 或驱动层 API

## 构建前置条件

1. 首次或板级配置变更后：`idf.py bmgr -b esp_SensairShuttle -c boards`
2. Kconfig 变更后：删除 sdkconfig 重新生成
3. Windows 环境：`PYTHONIOENCODING=utf-8`
4. 新增驱动依赖通过 `idf_component.yml` 或 bmgr 板子 YAML 管理，**不手动复制源文件**
