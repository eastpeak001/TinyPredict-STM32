# TinyPredict-STM32 V0.4B PCB Layout Steps

V0.4B 是直接焊接 STM32F103C8T6 的完整最小系统 PCB。画板时应先保证最小系统可靠，再连接 MPU6050、OLED、USART1 和报警接口。

## Datasheet reference pages

Reference document: STM32F103x8/xB datasheet, DS5319 Rev18.

| Pages | Reference content | How to use in V0.4B design |
| --- | --- | --- |
| Page 1 | Chip overview. STM32F103C8 belongs to STM32F103x8, supports up to 72MHz, 2.0-3.6V supply, SWD/JTAG, and packages including LQFP48. | Confirm device family, voltage range, debug support, and package choice. |
| Page 26 | LQFP48 pinout. | Use when drawing STM32F103C8T6 schematic symbol and checking physical pin orientation. |
| Pages 28-33 | Table 5 Medium-density STM32F103xx pin definitions, including pin names, main functions, and alternate functions. | Check PB6, PB7, PA9, PA10, PA13, PA14, PC13, PD0/OSC_IN, PD1/OSC_OUT, NRST, BOOT0, VDD/VSS/VDDA/VSSA/VBAT. |
| Page 36 | Power supply scheme. | Check VDD, VDDA, VSS, VSSA, and VBAT connections. |
| Pages 52-53 | HSE 4-16MHz oscillator characteristics and 8MHz crystal typical application. | Use for the current 8MHz external crystal design. |
| Pages 66-67 | NRST pin characteristics and recommended NRST pin protection. | Use for NRST pull-up, capacitor, reset button, and SWD NRST connection. |
| Pages 68-69 | I2C characteristics. | Use as reference for MPU6050 and OLED sharing I2C1. |
| Pages 101-103 | LQFP48 package information, mechanical data, recommended footprint, and top view example. | Use when checking STM32F103C8T6 LQFP48 footprint and PCB pad orientation. |

Design reminders:

- Each time the schematic symbol is drawn or checked, compare against page 26 and pages 28-33 first.
- Each time the PCB footprint is selected or checked, compare against pages 101-103 for LQFP48 orientation and pad layout.
- Do not connect any STM32 power pin to 5V. VDD and VDDA must use 3.3V within the 2.0-3.6V datasheet range.
- PC13 has limited drive capability. Use a relatively large LED current-limiting resistor and do not use PC13 to drive heavy loads.
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
## 总体布局建议

- PCB 外形为 80 mm x 50 mm，四角预留 M3 定位孔。
- STM32F103C8T6 放在 PCB 中央偏上位置。
- 8MHz 晶振靠近 OSC_IN / OSC_OUT 引脚。
- 3.3V LDO 和电源入口放在板边，方便供电。
- SWD 接口放在板边，便于 ST-Link 下载。
- USB-TTL 接口放在板边，便于串口调试。
- OLED 接口放在前侧，方便显示屏安装和观察。
- MPU6050 接口放在板边，方便传感器线束引出到风扇外壳或支架。
- 状态 LED 和电源 LED 放在容易观察的位置。
- 蜂鸣器预留接口放在边缘或右下角。

## 第 1 步：STM32F103C8T6 芯片 + 电源引脚 + 去耦电容

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

| 位号 | 容值 | 连接 | 用途 |
| --- | --- | --- | --- |
| C1 | 0.1uF | VDD_1 -> GND | VDD_1 decoupling |
| C2 | 0.1uF | VDD_2 -> GND | VDD_2 decoupling |
| C3 | 0.1uF | VDD_3 -> GND | VDD_3 decoupling |
| C4 | 0.1uF | VDDA -> GND | VDDA decoupling |
| C5 | 1uF | VDDA -> GND | VDDA filter |
| C6 | 0.1uF | VBAT -> GND | VBAT decoupling |

### 第 1 步检查要点

- 放置 STM32F103C8T6，LQFP-48(7x7)。
- 对照 DS5319 Rev18 第 26 页和第 28-33 页确认 LQFP48 引脚方向、引脚名称和复用功能。
- 所有 STM32 电源脚只能接 3.3V，不能接 5V。
- C1、C2、C3 分别靠近 VDD_1、VDD_2、VDD_3 放置。
- C4 和 C5 靠近 VDDA 放置。
- C6 靠近 VBAT 放置。
- 完成后截图检查电源脚、地脚和去耦电容，不要继续画下一模块前跳过检查。

## 第 2 步：3.3V LDO 电源模块

- 放置电源输入接口。
- 放置 3.3V LDO。
- LDO 输入端接电源输入。
- LDO 输出端生成 3.3V。
- LDO 输入和输出各放 10uF 电容。
- 电源入口加电源指示 LED 和限流电阻。
- 完成后截图检查 5V、3.3V、GND 和电容极性。

## 第 3 步：8MHz 晶振模块

- 放置 8MHz 晶振。
- 晶振一端接 OSC_IN，另一端接 OSC_OUT。
- 晶振两端各接 22pF 到 GND。
- PCB 布局时晶振必须靠近 STM32。
- 完成后截图检查 OSC_IN / OSC_OUT 和 22pF 电容。

## 第 4 步：NRST 复位模块

- NRST 通过 10k 上拉到 3.3V。
- NRST 通过 0.1uF 到 GND。
- NRST 连接复位按键到 GND。
- NRST 引出到 SWD 接口。
- 完成后截图检查 NRST 网络是否只有正确连接。

## 第 5 步：BOOT0 启动配置模块

- BOOT0 通过 10k 下拉到 GND。
- BOOT0 预留 2Pin 跳帽，可选拉到 3.3V。
- 文档和丝印注明正常使用 BOOT0 = 0。
- BOOT1 / PB2 保持默认低电平或不作为启动控制使用。
- 完成后截图检查 BOOT0 下拉和跳帽方向。

## 第 6 步：SWD 下载接口

- PA13 连接 SWDIO。
- PA14 连接 SWCLK。
- NRST 连接 SWD_NRST。
- 3.3V 连接 SWD_3V3。
- GND 连接 SWD_GND。
- 使用 5Pin 接口：3.3V、GND、SWDIO、SWCLK、NRST。
- 完成后截图检查接口引脚顺序和丝印。

## 第 7 步：I2C OLED / MPU6050 接口

- PB6 / I2C1_SCL 连接 MPU6050 SCL 和 OLED SCL。
- PB7 / I2C1_SDA 连接 MPU6050 SDA 和 OLED SDA。
- SCL 和 SDA 各预留 4.7k 上拉到 3.3V。
- MPU6050 4Pin：3.3V、GND、SCL、SDA。
- OLED 4Pin：3.3V、GND、SCL、SDA。
- 完成后截图检查 I2C 共线和接口引脚顺序。

## 第 8 步：USART1 USB-TTL 接口

- PA9 / USART1_TX 连接 USB-TTL RX。
- PA10 / USART1_RX 连接 USB-TTL TX。
- USB-TTL 接口引出 3.3V optional、GND、USART1_TX、USART1_RX。
- 完成后截图检查 TX/RX 是否按交叉连接说明标注。

## 第 9 步：状态 LED 和蜂鸣器预留

- 状态 LED：3.3V -> 1k 电阻 -> LED 正极，LED 负极 -> PC13。
- 文档和丝印注明 PC13 低电平点亮。
- PC13 只驱动状态 LED，使用较大限流电阻，不驱动大负载。
- 蜂鸣器预留接口：3.3V、BUZZER_CTRL、GND。
- 蜂鸣器不影响当前固件，后续版本再定义控制 GPIO。
- 完成后截图检查 LED 极性和蜂鸣器接口丝印。

## 第 10 步：ERC 检查

- 检查所有电源脚是否有电源网络。
- 检查 VSS / VSSA 是否接 GND。
- 检查 NRST、BOOT0、OSC_IN、OSC_OUT 是否连接完整。
- 检查 SWD、USART1、I2C 网络名是否正确。
- 检查是否有未连接的关键引脚。
- 修复 ERC 警告后再转 PCB。

## 第 11 步：转 PCB、布局、布线、DRC

- 先绘制 80 mm x 50 mm 板框，并放置 H1-H4 四个 M3 定位孔。
- H1=(4,4)、H2=(76,4)、H3=(4,46)、H4=(76,46)，孔径 3.2 mm，建议禁布 / 外径 6.0 mm。
- 先布局 STM32、晶振、去耦电容和 LDO。
- 再布局 SWD、USB-TTL、OLED、MPU6050、LED 和蜂鸣器接口。
- 先布电源线和 GND，再布晶振、SWD、I2C、USART1。
- 3.3V 主电源线适当加宽。
- 使用 GND 铺铜，保证地回流路径连续。
- 晶振走线短且远离高噪声线。
- I2C 线尽量短，避免靠近电机电源线。
- 运行 DRC，检查短路、未连接、间距、丝印压焊盘和封装方向。
- 对照 DS5319 Rev18 第 101-103 页确认 LQFP48 封装、焊盘方向和 top view 方向。
- 打样前人工复查 SWD、BOOT0、NRST、电源、晶振和 USART1 TX/RX。

## 强制提醒

每画完一个模块就截图检查，不要一次性画完整板。V0.4B 的第一目标是最小系统能上电、SWD 能下载、USART1 能输出、I2C 能读取 MPU6050 并刷新 OLED。

风扇电机不要从本 PCB 取电，PCB 只负责监测、显示和通信。



