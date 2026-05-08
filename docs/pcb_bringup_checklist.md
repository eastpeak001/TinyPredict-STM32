# TinyPredict-STM32 PCB Bring-up Checklist

## 1. Purpose

本文档用于 TinyPredict-STM32 PCB 焊接完成后的首次上电、固件烧录和功能验证。目标是在分阶段测试中尽早发现短路、反接、焊接错误和接口方向问题，避免一次性接入所有模块进行全功能测试带来的风险。

## 2. Before Power-On Inspection

- 检查 PCB 是否有明显划伤、断线、连锡。
- 检查 STM32 芯片方向是否正确。
- 检查模块插座方向是否正确。
- 检查 OLED 插入方向是否正确。
- 检查 3.3V 与 GND 是否短路。
- 检查 5V 与 GND 是否短路。
- 检查 SWDIO、SWCLK、GND 是否连通到对应接口。
- 检查 I2C1 SCL/SDA 是否没有短路。
- 检查 USART1 TX/RX 是否没有短路。

## 3. Power Rail Test

- 不插 OLED。
- 不插 MPU6050。
- 不插其他外部模块。
- 只接电源。
- 测量 3.3V 是否正常。
- 测量 5V 是否正常。
- 用手轻触确认没有元件异常发热。
- 如果电压异常，立即断电并检查电源、LDO、焊接和短路情况。

## 4. SWD Download Test

- 接入 ST-LINK。
- SWDIO -> PA13。
- SWCLK -> PA14。
- GND 共地。
- 尝试连接 STM32。
- 尝试下载当前 commit 固件。
- 如果无法连接，检查 BOOT0、NRST、供电和 SWD 接线。

## 5. Basic Firmware Run Test

- 烧录后复位。
- 观察 PC13 LED 或串口输出。
- 确认程序没有卡死在 `Error_Handler`。

## 6. USART1 CSV Test

- USB-TTL RX 接 PA9。
- USB-TTL TX 接 PA10。
- GND 共地。
- 打开串口：115200, 8N1。
- 预期看到 CSV 表头：

```text
time_ms,ax,ay,az,rms,status
```

## 7. OLED I2C Test

- 插入 OLED 前确认 VCC/GND/SCL/SDA 方向。
- OLED 地址：0x3C。
- I2C1: PB6 SCL, PB7 SDA。
- 预期 OLED 显示 TinyPredict / RMS / Status。
- 如果不亮，检查供电、地址、SCL/SDA、焊接方向。

## 8. MPU6050 Test

- 确认 MPU6050 VCC/GND/SCL/SDA 接线方向。
- MPU6050 与 OLED 共用 I2C1。
- 静止时 RMS 应较低。
- 轻微晃动时 RMS 应上升。
- 明显晃动时应进入 WARNING / ALARM。

## 9. Python Logger and Analysis Test

```powershell
python tools/serial_logger.py --list-ports
python tools/serial_logger.py --port COM5 --baud 115200 --out docs/data/pcb_bringup_test.csv
python tools/analyze_log.py --file docs/data/pcb_bringup_test.csv
```

如果设备管理器显示的 COM 号不是 COM5，需要把命令中的 COM5 改为实际串口号。

## 10. System Status Test

- WARNING enter: rms >= 0.05, continuous 3 samples.
- WARNING exit: rms < 0.04, continuous 5 samples.
- ALARM enter: rms >= 0.15, continuous 3 samples.
- ALARM exit: rms < 0.12, continuous 5 samples.

## 11. Failure Checklist

- 无法上电。
- 3.3V 不正常。
- ST-LINK 无法连接。
- 串口无输出。
- OLED 不亮。
- MPU6050 init failed。
- 状态一直 ALARM。
- 状态一直 NORMAL。
- PC13 LED 逻辑反了。

## 12. Acceptance Checklist

- [ ] PCB 外观检查通过
- [ ] 3.3V 与 GND 无短路
- [ ] 5V 与 GND 无短路
- [ ] 3.3V 电源正常
- [ ] ST-LINK 可连接
- [ ] 固件可烧录
- [ ] USART1 CSV 输出正常
- [ ] OLED 显示正常
- [ ] MPU6050 数据正常
- [ ] RMS 随振动变化
- [ ] NORMAL / WARNING / ALARM 状态正常
- [ ] Python 记录 CSV 成功
- [ ] Python 生成图表和报告成功
