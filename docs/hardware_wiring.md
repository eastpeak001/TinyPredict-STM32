# TinyPredict-STM32 硬件接线说明

本文档记录 TinyPredict-STM32 当前版本的硬件连接方式，包括 MPU6050、USART1 调试串口和 ST-Link SWD 下载接口。

## STM32 与 MPU6050 接线

| MPU6050 | STM32F103C8T6 | 说明 |
| --- | --- | --- |
| VCC | 3.3V | 传感器供电 |
| GND | GND | 共地 |
| SCL | PB6 / I2C1_SCL | I2C 时钟线 |
| SDA | PB7 / I2C1_SDA | I2C 数据线 |

注意：MPU6050 模块需要使用 3.3V 逻辑电平。若使用带稳压和电平转换的模块，也应确认模块规格后再接线。


## STM32 与 OLED 接线

0.96 寸 SSD1306 OLED 使用 I2C 接口，实物验证接线如下。OLED 与 MPU6050 共用 I2C1。

| OLED | STM32F103C8T6 | 说明 |
| --- | --- | --- |
| VCC | 3.3V | OLED 供电 |
| GND | GND | 共地 |
| SCL | PB6 / I2C1_SCL | 与 MPU6050 共用 I2C 时钟线 |
| SDA | PB7 / I2C1_SDA | 与 MPU6050 共用 I2C 数据线 |

默认 OLED I2C 地址为 `0x3C`。如果 OLED 未连接或初始化失败，固件会通过 USART1 输出 `OLED init failed`，但不会影响 MPU6050 采集和串口日志输出。
## STM32 与 USB 转 TTL 接线

| STM32F103C8T6 | USB 转 TTL | 说明 |
| --- | --- | --- |
| PA9 / USART1_TX | RX | STM32 发送串口日志 |
| PA10 / USART1_RX | TX | STM32 接收，当前主要预留 |
| GND | GND | 共地 |

串口参数：

| 参数 | 值 |
| --- | --- |
| 波特率 | 115200 |
| 数据位 | 8 |
| 校验位 | None |
| 停止位 | 1 |

## STM32 与 ST-Link 接线

| ST-Link | STM32F103C8T6 | 说明 |
| --- | --- | --- |
| SWDIO | PA13 | SWD 数据线 |
| SWCLK | PA14 | SWD 时钟线 |
| GND | GND | 共地 |
| 3.3V | 3.3V | 目标板供电或电平参考 |

STM32CubeIDE 中需要配置：

- SYS Debug: Serial Wire
- 下载方式：ST-Link

## 供电注意事项

- STM32、MPU6050、USB 转 TTL、ST-Link 必须共地。
- MPU6050 推荐使用 3.3V 供电。
- 如果 ST-Link 同时给目标板供电，避免再接入另一路不受控 3.3V 电源。
- 若使用 USB 转 TTL，仅用于串口通信时通常只接 TX、RX、GND，不建议同时接 5V 到目标板。
- 风扇或被测设备如果有独立电源，应注意与 STM32 测量系统的安全隔离和地线干扰。

## 调试注意事项

- Reset 后前 2 秒为 CALIBRATING 阶段，应保持传感器和被测结构静止。
- 若串口一直输出 `MPU6050 init failed`，优先检查 I2C 接线、供电和模块地址。
- 若串口无输出，检查 PA9/PA10 是否交叉连接、USB 转 TTL 电平是否为 3.3V、串口参数是否为 115200 8N1。
- 若慢慢倾斜会短暂出现 RMS 变化，保持不动后 RMS 应逐渐回落。
- 若振动数据波动异常大，检查 MPU6050 是否固定牢固，以及线缆是否拉扯传感器模块。


