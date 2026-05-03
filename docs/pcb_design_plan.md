# TinyPredict-STM32 V0.4B PCB Design Plan

## 项目目标

TinyPredict-STM32 V0.4B 是完整 STM32F103C8T6 Minimal System Board，不再依赖 Blue Pill / STM32F103C8T6 开发板。PCB 直接焊接 STM32F103C8T6 LQFP-48 芯片，并集成电源、时钟、复位、BOOT、SWD、USART1、MPU6050、OLED、状态 LED 和蜂鸣器预留接口。

本版本只改变硬件 PCB 形态，当前 STM32 固件引脚保持不变。

## V0.4B 设计目标

- 直接在 PCB 上焊接 STM32F103C8T6，LQFP-48。
- 集成 3.3V LDO 电源模块。
- 集成 8MHz 外部晶振。
- 集成 NRST 复位电路。
- 集成 BOOT0 下拉和跳帽配置。
- 提供 SWD 下载调试接口。
- 提供 USART1 USB-TTL 调试接口。
- 提供 MPU6050 4Pin 接口。
- 提供 OLED 4Pin 接口。
- 预留 I2C 上拉电阻。
- 提供状态 LED。
- 预留蜂鸣器接口。
- 提供电源输入接口。

## PCB 外形与定位孔

V0.4B STM32F103C8T6 最小系统板建议使用 80 mm x 50 mm PCB 外形。该尺寸用于预留 Type-C、电源、SWD、USART1、OLED、MPU6050、BOOT0、复位、状态 LED 和蜂鸣器接口布局空间。

### PCB 外形

| 项目 | 数值 |
| --- | --- |
| PCB 宽度 | 80 mm |
| PCB 高度 | 50 mm |
| 坐标原点建议 | 左下角或左上角，画板时保持全项目一致 |

### M3 定位孔

| 孔位 | X | Y | 孔径 | 建议禁布 / 外径 |
| --- | ---: | ---: | ---: | ---: |
| H1 | 4 mm | 4 mm | 3.2 mm | 6.0 mm |
| H2 | 76 mm | 4 mm | 3.2 mm | 6.0 mm |
| H3 | 4 mm | 46 mm | 3.2 mm | 6.0 mm |
| H4 | 76 mm | 46 mm | 3.2 mm | 6.0 mm |

布局提醒：

- 定位孔中心距板边 4.0 mm。
- M3 孔周围建议保留 6.0 mm 禁布或外径区域。
- 定位孔附近不要放置 Type-C、SWD、USART1、OLED、MPU6050、BOOT0、复位、状态 LED 和蜂鸣器接口。
- 先确定板框和孔位，再放置 STM32、LDO、晶振和外设接口。
## 固件引脚保持不变

| STM32 引脚 | 固件功能 | PCB 用途 |
| --- | --- | --- |
| PB6 | I2C1_SCL | MPU6050 SCL + OLED SCL |
| PB7 | I2C1_SDA | MPU6050 SDA + OLED SDA |
| PA9 | USART1_TX | USB-TTL RX |
| PA10 | USART1_RX | USB-TTL TX |
| PA13 | SWDIO | SWDIO |
| PA14 | SWCLK | SWCLK |
| PC13 | STATUS_LED | 状态 LED，低电平点亮 |

## 最小系统模块

### 电源

- 所有 VDD 接 3.3V。
- VDDA 接 3.3V。
- 所有 VSS 接 GND。
- VSSA 接 GND。
- 每个 VDD 附近放 0.1uF 去耦电容。
- VDDA 附近放 0.1uF + 1uF。
- LDO 输入输出各放 10uF。
- 电源入口加电源指示 LED。

### 时钟

- 8MHz 晶振接 OSC_IN / OSC_OUT。
- 晶振两端各接 22pF 到 GND。
- 晶振靠近 STM32 放置，走线短且对称。

### 复位

- NRST 通过 10k 上拉到 3.3V。
- NRST 通过 0.1uF 到 GND。
- NRST 接复位按键到 GND。
- NRST 建议引出到 SWD 接口。

### BOOT

- BOOT0 通过 10k 下拉到 GND。
- BOOT0 预留 2Pin 跳帽，可选拉到 3.3V。
- BOOT1 / PB2 保持默认低电平或不作为启动控制使用。
- 正常运行和 SWD 下载时使用 BOOT0 = 0。

## 外设接口

### SWD 5Pin

| Pin | 信号 |
| --- | --- |
| 1 | 3.3V |
| 2 | GND |
| 3 | SWDIO |
| 4 | SWCLK |
| 5 | NRST |

### MPU6050 4Pin

| Pin | 信号 |
| --- | --- |
| 1 | 3.3V |
| 2 | GND |
| 3 | SCL |
| 4 | SDA |

### OLED 4Pin

| Pin | 信号 |
| --- | --- |
| 1 | 3.3V |
| 2 | GND |
| 3 | SCL |
| 4 | SDA |

### USB-TTL 4Pin

| Pin | 信号 |
| --- | --- |
| 1 | 3.3V optional |
| 2 | GND |
| 3 | USART1_TX |
| 4 | USART1_RX |

## V0.4B 推荐开发方式

V0.4B 是第一次完整最小系统 PCB，建议严格按模块逐步绘制：

1. 每画完一个模块就截图检查。
2. 不要一次性画完整板。
3. 每个模块完成后先检查网络名、电源符号、地符号和器件封装。
4. 原理图完成后先做 ERC，再转 PCB。
5. PCB 布局完成后先检查最小系统，再连接外设。
6. 布线完成后运行 DRC，并人工检查 SWD、BOOT0、NRST、电源和晶振。

## 风险提示

- V0.4B 不再使用 Blue Pill，最小系统错误会导致整板无法下载或启动。
- 第一版 PCB 重点是让 STM32 正常上电、SWD 可下载、串口可输出、I2C 外设可通信。
- 风扇电机不要从本 PCB 取电，风扇应单独供电。
- PCB 只负责监测、显示和通信。
- 电机电源线和传感器线应分开，避免电机噪声影响 STM32 和 MPU6050。

