# TinyPredict-STM32 V0.4B Schematic Connection Table

V0.4B 是完整 STM32F103C8T6 Minimal System Board，不再依赖 Blue Pill 开发板。当前固件引脚保持不变，原理图应按下表连接。

## 固件引脚连接表

| STM32 引脚 | 功能 | 网络名 | 连接目标 |
| --- | --- | --- | --- |
| PB6 | I2C1_SCL | I2C1_SCL | MPU6050 SCL + OLED SCL |
| PB7 | I2C1_SDA | I2C1_SDA | MPU6050 SDA + OLED SDA |
| PA9 | USART1_TX | USART1_TX | USB-TTL RX |
| PA10 | USART1_RX | USART1_RX | USB-TTL TX |
| PA13 | SWDIO | SWDIO | SWD 接口 SWDIO |
| PA14 | SWCLK | SWCLK | SWD 接口 SWCLK |
| PC13 | STATUS_LED | STATUS_LED | 状态 LED，低电平点亮 |

## 一、电源连接

| STM32 / 模块 | 连接 | 说明 |
| --- | --- | --- |
| 所有 VDD | 3.3V | 数字电源 |
| VDDA | 3.3V | 模拟电源 |
| 所有 VSS | GND | 数字地 |
| VSSA | GND | 模拟地 |
| 每个 VDD 附近 | 0.1uF 到 GND | 去耦电容 |
| VDDA 附近 | 0.1uF + 1uF 到 GND | 模拟电源去耦 |
| LDO 输入 | 10uF 到 GND | 输入滤波 |
| LDO 输出 | 10uF 到 GND | 输出滤波 |
| 电源入口 | LED + 1k | 电源指示 |

## 二、时钟连接

| 信号 | 连接 |
| --- | --- |
| OSC_IN | 8MHz 晶振一端 |
| OSC_OUT | 8MHz 晶振另一端 |
| 晶振两端 | 各接 22pF 到 GND |

晶振应靠近 STM32 放置，走线尽量短且对称。

## 三、复位连接

| 信号 | 连接 |
| --- | --- |
| NRST | 10k 上拉到 3.3V |
| NRST | 0.1uF 到 GND |
| NRST | 复位按键到 GND |
| NRST | 引出到 SWD 接口 SWD_NRST |

## 四、BOOT 连接

| 信号 | 连接 |
| --- | --- |
| BOOT0 | 10k 下拉到 GND |
| BOOT0 | 2Pin 跳帽，可选拉到 3.3V |
| BOOT1 / PB2 | 保持默认低电平或不作为启动控制使用 |

正常使用 BOOT0 = 0。

## 五、SWD 连接

| STM32 / 电源 | SWD 接口 |
| --- | --- |
| PA13 | SWDIO |
| PA14 | SWCLK |
| NRST | SWD_NRST |
| 3.3V | SWD_3V3 |
| GND | SWD_GND |

### SWD 5Pin 接口

| Pin | 信号 |
| --- | --- |
| 1 | 3.3V |
| 2 | GND |
| 3 | SWDIO |
| 4 | SWCLK |
| 5 | NRST |

## 六、I2C 连接

| STM32 引脚 | 网络名 | 连接目标 |
| --- | --- | --- |
| PB6 | I2C1_SCL | MPU6050 SCL + OLED SCL |
| PB7 | I2C1_SDA | MPU6050 SDA + OLED SDA |

| 网络名 | 上拉 |
| --- | --- |
| I2C1_SCL | 4.7k 到 3.3V，预留 |
| I2C1_SDA | 4.7k 到 3.3V，预留 |

## 七、USART1 连接

| STM32 引脚 | 网络名 | USB-TTL 接口 |
| --- | --- | --- |
| PA9 | USART1_TX | USB-TTL RX |
| PA10 | USART1_RX | USB-TTL TX |
| GND | GND | USB-TTL GND |

## 八、状态 LED 连接

| 连接顺序 | 说明 |
| --- | --- |
| 3.3V -> 1k 电阻 -> LED 正极 | LED 供电和限流 |
| LED 负极 -> PC13 | PC13 输出低电平时点亮 |

PC13 低电平点亮，符合 Blue Pill 常见板载 LED 逻辑，便于保持当前固件行为。

## 九、外设接口

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

### 蜂鸣器预留接口

| Pin | 信号 |
| --- | --- |
| 1 | 3.3V |
| 2 | BUZZER_CTRL |
| 3 | GND |

蜂鸣器为后续报警扩展预留，不改变当前固件引脚配置。

## 重要提醒

- 风扇电机不要从本 PCB 取电。
- PCB 只负责监测、显示和通信。
- 每画完一个模块就截图检查，不要一次性画完整板。
