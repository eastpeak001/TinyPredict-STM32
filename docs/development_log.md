# TinyPredict-STM32 开发记录

## 1. 创建 STM32CubeIDE 工程

创建 TinyPredict-STM32 工程，目标芯片为 STM32F103C8T6，使用 HAL 库生成基础工程代码。

## 2. 配置 72MHz 时钟

配置系统时钟到 72MHz，用于满足主控运行、I2C 通信和 USART 串口输出需求。

## 3. 配置 ST-Link SWD

在 STM32CubeIDE 中配置 SYS Debug 为 Serial Wire，使用 PA13 / PA14 作为 SWDIO / SWCLK，实现 ST-Link 下载和调试。

## 4. 加入 App 层结构

新增 App 层代码框架，将业务逻辑从 CubeMX 生成的 main.c 中拆分出来。当前包括：

- app_main.c / app_main.h
- sensor_task.c / sensor_task.h
- vibration_algo.c / vibration_algo.h
- alarm_task.c / alarm_task.h

main.c 中只负责初始化外设，并在主循环中调用 App_MainLoop()。

## 5. 加入 USART1 输出

配置 USART1：

- PA9: TX
- PA10: RX
- 波特率：115200

使用 HAL_UART_Transmit 输出系统状态，不使用 printf 重定向。

## 6. 加入 MPU6050 驱动

新增 mpu6050.c / mpu6050.h，使用 HAL_I2C_Mem_Read 和 HAL_I2C_Mem_Write 访问 MPU6050 寄存器。

已实现内容：

- 读取 WHO_AM_I 并判断 0x68。
- 写 PWR_MGMT_1 退出睡眠模式。
- 读取三轴加速度原始值。
- 将原始加速度转换为 g 单位。

## 7. 解决倾斜误判问题

早期算法直接使用 ax、ay、az 或固定静止基准计算 RMS。测试发现慢慢倾斜模块时也会触发 WARNING / ALARM，说明算法对姿态变化敏感。

为解决该问题，算法改为低通估计重力分量，再用原始加速度减去重力分量，得到动态振动分量。

## 8. 加入高通振动算法

当前算法流程：

1. 启动后进入 CALIBRATING 状态。
2. 第一次有效数据用于初始化 gravity_x / gravity_y / gravity_z。
3. 启动前 2000 ms 使用快速低通学习重力。
4. READY 后使用慢速低通持续估计重力。
5. 计算 vx、vy、vz 动态振动分量。
6. 使用最近 32 个 vib_mag 计算滑动 RMS。
7. 根据 RMS 输出 NORMAL / WARNING / ALARM。

## 9. 完成风扇偏心测试

使用风扇作为测试对象，采集不同状态下的 RMS 数据：

| 工况 | RMS |
| --- | ---: |
| 静止/稳定状态 | 0.012 |
| 正常运行状态 | 0.017 |
| 轻微异常状态 | 0.085 |
| 明显异常状态 | 0.400 |

根据测试结果，当前阈值设置为：

- NORMAL: rms < 0.05
- WARNING: 0.05 <= rms < 0.15
- ALARM: rms >= 0.15

当前版本已经具备基础振动采集、状态判断和串口输出能力。

## 10. V0.1 Prototype 完成

V0.1 Prototype 完成：实现从传感器采集、振动算法、串口输出到风扇测试的完整闭环。

## 11. slight_unbalance 无效采集处理

slight_unbalance 测试过程中发现胶带偏心条件和 MPU6050 连接稳定性会影响数据，因此对异常采集进行了作废处理，并只保留有效数据用于报告。

## 12. V0.2 Python 分段采集与曲线整理

V0.2 完成 Python 串口分段采集流程，完成 `idle`、`normal`、`slight_unbalance`、`severe_unbalance` 四组工况测试，并完成 CSV 数据记录。当前已使用 `plot_rms.py` / `plot_latest_v02.py` 生成 RMS 曲线图，用于测试报告展示。

## 13. V0.2 测试整理完成

V0.2 已完成 Python 依赖安装、CSV 记录、四组工况测试和 RMS 曲线生成。最终报告采用 `idle`、`normal`、`slight_unbalance`、`severe_unbalance` 四组有效 CSV，并引用 `docs/images/rms_test_curve.png` 作为测试曲线。


## 14. V0.3 OLED 本地显示

V0.3 增加 0.96 寸 SSD1306 OLED 本地显示功能。OLED 使用 I2C 接口并与 MPU6050 共用 I2C1，默认地址为 0x3C。App_Init() 中初始化 OLED 并通过串口输出 `OLED init ok` 或 `OLED init failed`；App_MainLoop() 中每 300 ms 刷新一次 OLED，显示项目名、RMS、状态和版本信息。OLED 未连接或初始化失败时，系统仍继续执行 MPU6050 采集、振动算法、报警判断和 USART1 输出。

## 15. V0.3 OLED 实物验证完成
V0.3 OLED 本地显示已完成实物验证。OLED 可以正常点亮并显示 TinyPredict、RMS、状态和 V0.3 OLED 信息；MPU6050、USART1 串口输出和 Python 上位机功能保持正常。

## 16. V0.3 原型照片加入文档
V0.3 OLED prototype photo added. 原型照片记录了 STM32F103C8T6、MPU6050、0.96 寸 OLED、USB 转 TTL 和风扇测试平台，说明 OLED 本地显示已完成实物验证。

## 17. V0.4 PCB 规划启动
V0.4 PCB planning started. 当前推荐先采用 V0.4A 开发板接口版 PCB carrier board，用 Blue Pill / STM32F103C8T6 小板插针替代面包板连接，并预留 MPU6050、OLED、USART1、SWD、电源、状态 LED 和蜂鸣器接口。
## 18. V0.4B PCB 实物 Bring-up 完成

V0.4B STM32F103C8T6 最小系统 PCB 已完成焊接和实物 bring-up。电源测试、ST-Link 下载、PC13 状态 LED、USART1 串口输出、OLED 显示、MPU6050 读取和三脚有源蜂鸣器模块均完成验证。测试过程中发现并解决了 MPU6050 接线稳定性问题，完整 TinyPredict 主程序已在 PCB 上测试成功，NORMAL / WARNING / ALARM 状态判断工作正常，ALARM 状态下蜂鸣器报警成功。

本阶段还为 OLED 增加 PG Logo 开机动画：OLED 初始化成功后先显示 PG Logo 扫描动画，再显示 TinyPredict / V0.4B Ready，随后进入 RMS / Status 主界面。

实物测试经验：MPU6050 需要固定在被测物体上，线束也要固定，避免 I2C 接线晃动导致 OLED 显示异常或传感器读数不稳定。

蜂鸣器接线为 VCC -> 3V3、GND -> GND、I/O -> PB12。该模块为低电平触发，PB12 = 0 时蜂鸣器响，PB12 = 1 时蜂鸣器关闭。


