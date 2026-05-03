# TinyPredict-STM32 V0.4B Schematic Checklist

本清单用于检查 STM32F103C8T6 Minimal System Board 原理图。每画完一个模块就截图检查，不要一次性画完整板。

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
## 1. STM32F103C8T6 芯片

- [ ] 选用 STM32F103C8T6，LQFP-48。
- [ ] 所有 VDD 已连接到 3.3V。
- [ ] VDDA 已连接到 3.3V。
- [ ] 任何 STM32 电源脚都没有接到 5V。
- [ ] 所有 VSS 已连接到 GND。
- [ ] VSSA 已连接到 GND。
- [ ] PB6、PB7、PA9、PA10、PA13、PA14、PC13 网络名与固件一致。
- [ ] BOOT0、NRST、OSC_IN、OSC_OUT 已连接到对应模块。

## 2. 电源模块

- [ ] 电源输入接口已连接到 LDO 输入。
- [ ] LDO 输出为 3.3V。
- [ ] LDO 输入侧放置 10uF 电容。
- [ ] LDO 输出侧放置 10uF 电容。
- [ ] 电源入口有电源指示 LED。
- [ ] 电源 LED 串联限流电阻。
- [ ] 3.3V 和 GND 网络名统一。

## 3. 去耦电容

- [ ] 每个 VDD 附近放置 0.1uF 去耦电容。
- [ ] VDDA 附近放置 0.1uF + 1uF。
- [ ] 去耦电容另一端接 GND。
- [ ] PCB 布局时去耦电容靠近 STM32 电源脚。

## 4. 8MHz 晶振模块

- [ ] 8MHz 晶振连接 OSC_IN / OSC_OUT。
- [ ] 晶振两端各接 22pF 到 GND。
- [ ] 晶振靠近 STM32 放置。
- [ ] 晶振走线短、对称，避免穿越高噪声区域。

## 5. NRST 复位模块

- [ ] NRST 通过 10k 上拉到 3.3V。
- [ ] NRST 通过 0.1uF 到 GND。
- [ ] NRST 通过复位按键到 GND。
- [ ] NRST 引出到 SWD 接口。

## 6. BOOT 配置模块

- [ ] BOOT0 通过 10k 下拉到 GND。
- [ ] BOOT0 预留 2Pin 跳帽，可选拉到 3.3V。
- [ ] 文档和丝印注明正常使用 BOOT0 = 0。
- [ ] BOOT1 / PB2 保持默认低电平或不作为启动控制使用。

## 7. SWD 接口

- [ ] PA13 连接 SWDIO。
- [ ] PA14 连接 SWCLK。
- [ ] NRST 连接 SWD_NRST。
- [ ] 3.3V 连接 SWD_3V3。
- [ ] GND 连接 SWD_GND。
- [ ] SWD 接口为 5Pin：3.3V、GND、SWDIO、SWCLK、NRST。

## 8. I2C 接口

- [ ] PB6 连接 I2C1_SCL。
- [ ] PB7 连接 I2C1_SDA。
- [ ] I2C1_SCL 同时连接 MPU6050 SCL 和 OLED SCL。
- [ ] I2C1_SDA 同时连接 MPU6050 SDA 和 OLED SDA。
- [ ] SCL 预留 4.7k 上拉到 3.3V。
- [ ] SDA 预留 4.7k 上拉到 3.3V。

## 9. USART1 接口

- [ ] PA9 / USART1_TX 连接 USB-TTL RX。
- [ ] PA10 / USART1_RX 连接 USB-TTL TX。
- [ ] USB-TTL 接口引出 GND。
- [ ] USB-TTL 接口 3.3V 只作为 optional，不用于大电流供电。

## 10. 状态 LED

- [ ] 3.3V 连接 1k 电阻。
- [ ] 1k 电阻连接 LED 正极。
- [ ] LED 负极连接 PC13。
- [ ] 文档注明 PC13 低电平点亮。
- [ ] PC13 使用较大限流电阻驱动 LED，没有连接蜂鸣器、电机或其他大负载。

## 11. 蜂鸣器预留

- [ ] 预留蜂鸣器接口。
- [ ] 蜂鸣器控制信号暂不影响当前固件。
- [ ] 接口丝印标注 3.3V、GND 和控制信号。

## 12. ERC 前检查

- [ ] 所有电源输入脚都有明确电源网络。
- [ ] 所有 GND 已连接。
- [ ] 未使用引脚没有误接到错误网络。
- [ ] SWD、USART1、I2C、NRST、BOOT0 网络名清晰。
- [ ] 风扇电机不从本 PCB 取电。

