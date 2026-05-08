# TinyPredict-STM32 V0.4B PCB BOM

V0.4B 是完整 STM32F103C8T6 Minimal System Board，不再依赖 Blue Pill 开发板。BOM 以最小系统可启动、可下载、可串口调试、可连接 MPU6050 和 OLED 为目标。

## 第 1 步物料与去耦分配

### 第 1 步使用物料

| 位号 / 用途 | 器件 | 立创编号 | 品牌 | 封装 |
| --- | --- | --- | --- | --- |
| U1 | STM32F103C8T6 | C8734 | ST(意法半导体) | LQFP-48(7x7) |
| C1 / C2 / C3 / C4 / C6 | CC0603KRX7R9BB104，0.1uF | C14663 | YAGEO(国巨) | 0603 |
| C5 | CL10A105KB8NNNC，1uF | C15849 | Samsung Electro-Mechanics(三星电机) | 0603 |

### 第 1 步电源连接

| STM32 电源脚 | 网络 |
| --- | --- |
| VBAT | 3V3 |
| VDD_1 | 3V3 |
| VDD_2 | 3V3 |
| VDD_3 | 3V3 |
| VDDA | 3V3 |
| VSS_1 | GND |
| VSS_2 | GND |
| VSS_3 | GND |
| VSSA | GND |

### 第 1 步去耦电容分配

| 位号 | 容值 | 用途 |
| --- | --- | --- |
| C1 | 0.1uF | VDD_1 decoupling |
| C2 | 0.1uF | VDD_2 decoupling |
| C3 | 0.1uF | VDD_3 decoupling |
| C4 | 0.1uF | VDDA decoupling |
| C5 | 1uF | VDDA filter |
| C6 | 0.1uF | VBAT decoupling |

所有 STM32 电源脚只能接 3.3V，不能接 5V。每完成一个原理图模块后截图检查。
## 核心器件

| 类别 | 器件 | 建议规格 | 用途 | 备注 |
| --- | --- | --- | --- | --- |
| MCU | STM32F103C8T6 | C8734 / ST(意法半导体) / LQFP-48(7x7) | 主控芯片 | 直接焊接在 PCB 上 |
| LDO | AMS1117-3.3 或 XC6206 / ME6211 | 3.3V 输出 | 电源稳压 | 根据电流和发热选择 |
| 晶振 | 8MHz Crystal | 直插或贴片 | HSE 外部时钟 | 靠近 STM32 放置 |
| 晶振电容 | 22pF | C0G/NP0 优先 | 晶振负载电容 | 两颗，分别到 GND |
| 复位电阻 | 10k | 0603 / 0805 | NRST 上拉 | NRST 到 3.3V |
| 复位电容 | 0.1uF | 0603 / 0805 | NRST 滤波 | NRST 到 GND |
| 复位按键 | 轻触按键 | SMD / DIP | 手动复位 | NRST 到 GND |
| BOOT0 电阻 | 10k | 0603 / 0805 | BOOT0 下拉 | 正常启动 BOOT0 = 0 |
| BOOT0 跳帽 | 2Pin | 2.54mm | 可选拉高 BOOT0 | 调试启动模式预留 |

## 电源与去耦

| 类别 | 器件 | 建议规格 | 用途 | 备注 |
| --- | --- | --- | --- | --- |
| 电源输入 | Type-C 6P 或 2Pin 端子 | 5V 输入 | PCB 供电 | Type-C 仅供电，不做 USB 数据 |
| LDO 输入电容 | 10uF | 0805 / 1206 | 输入滤波 | 靠近 LDO 输入 |
| LDO 输出电容 | 10uF | 0805 / 1206 | 输出滤波 | 靠近 LDO 输出 |
| VDD / VDDA / VBAT 去耦 | CC0603KRX7R9BB104，C14663，YAGEO(国巨)，0603，0.1uF | 0603 | MCU 去耦 | C1/C2/C3/C4/C6，靠近对应电源脚 |
| VDDA 滤波 | CL10A105KB8NNNC，C15849，Samsung Electro-Mechanics(三星电机)，0603，1uF | 0603 | 模拟电源滤波 | C5，靠近 VDDA |
| 电源 LED | LED + 1k | 0603 / 0805 | 上电指示 | 接 3.3V |

## 外设接口

| 类别 | 器件 | 建议规格 | 用途 | 备注 |
| --- | --- | --- | --- | --- |
| MPU6050 接口 | 4Pin 接口 | 2.54mm 或 XH2.54 | 传感器连接 | 3.3V / GND / SCL / SDA |
| OLED 接口 | 4Pin 接口 | 2.54mm 或 XH2.54 | 显示屏连接 | 3.3V / GND / SCL / SDA |
| USB-TTL 接口 | 4Pin 接口 | 2.54mm 或 XH2.54 | 串口调试 | 3.3V optional / GND / TX / RX |
| SWD 接口 | 5Pin 接口 | 2.54mm | ST-Link 下载 | 3.3V / GND / SWDIO / SWCLK / NRST |
| I2C 上拉 | 4.7k | 0603 / 0805 | SCL / SDA 上拉预留 | 如模块已有上拉可不焊 |
| 状态 LED | LED + 1k | 0603 / 0805 | PC13 状态指示 | PC13 低电平点亮 |
| 蜂鸣器接口 | 3Pin 接口 | 2.54mm | 报警预留 | 当前固件暂不使用 |

## 可沿用 V0.4A 已选物料

| 器件 | 立创编号 | 品牌 / 厂商 | 说明 |
| --- | --- | --- | --- |
| HS96L03W2C03 | C5248080 | HS(汉昇) | 0.96 寸 I2C OLED 模块 |
| MOD-MPU6050 | C5445394 | Olimex Ltd. | MPU6050 模块 |
| HX-XH2.54-4PZZ | C42391662 | hanxia(韩下) | 4Pin 接口，可用于外设连接 |
| AMS1117-3.3 | C22466222 | TDSEMIC(拓电半导体) | SOT223，3.3V LDO |
| TYPE-C 6P(073) | C668623 | SHOU HAN(首韩) | SMD，5V 电源输入 |
| 0805F106M160NT | C90545 | FH(风华) | 0805，10uF 电容 |
| 0603B104K500NT | 待确认 | FH(风华) | 0603，0.1uF 电容 |
| 0603WAF1001T5E | C21190 | UNI-ROYAL(厚声) | 0603，1k 电阻 |
| HYT-0903 | C7544822 | HYDZ(华宇) | DIP，有源蜂鸣器，可作为预留报警器件 |

## 不纳入 V0.4B 第一版的内容

- 不集成 USB-TTL 芯片，仍使用外部 USB 转 TTL 模块连接 USART1。
- 不直接控制风扇电机电源。
- 不加入无线通信、电机驱动或继电器输出。
- 蜂鸣器只做接口预留，是否焊接和启用可放到后续固件版本。

## 关键提醒

- 风扇电机不要从本 PCB 取电，风扇单独供电。
- PCB 只负责监测、显示和通信。
- V0.4B 首要验证目标：3.3V 正常、NRST 正常、BOOT0 正常、8MHz 晶振正常、SWD 可下载、USART1 可输出、I2C 可读取 MPU6050 和刷新 OLED。

