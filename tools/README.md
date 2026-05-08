# TinyPredict-STM32 Tools

## serial_logger.py

`serial_logger.py` 是 TinyPredict-STM32 V0.2 的 Python 串口数据记录工具。它从 USART1 串口读取 STM32 输出的数据，解析 `ax`、`ay`、`az`、`vx`、`vy`、`vz`、`rms`、`status`，并保存为 CSV 文件。

适用环境：Windows + Python 3。

## 查看可用串口

先列出当前电脑识别到的串口：

```bash
py tools/serial_logger.py --list-ports
```

当前测试环境中，USB 转 TTL 被识别为：

```text
USB-Enhanced-SERIAL CH343 (COM5)
```

推荐运行：

```bash
py tools/serial_logger.py --port COM5 --baud 115200
```

如果设备管理器显示的 COM 号不是 COM5，需要把命令里的 COM5 改成实际 COM 号。

## 分段测试采集

当前风扇偏心测试不是连续实时切换。由于制造轻微偏心和明显偏心需要停机贴胶带，推荐按下面四段分别采集：

| label | 测试状态 |
| --- | --- |
| idle | 静止/稳定状态 |
| normal | 正常转动状态 |
| slight_unbalance | 轻微偏心状态 |
| severe_unbalance | 明显偏心状态 |

分别运行以下命令：

```bash
py tools/serial_logger.py --port COM5 --baud 115200 --label idle
py tools/serial_logger.py --port COM5 --baud 115200 --label normal
py tools/serial_logger.py --port COM5 --baud 115200 --label slight_unbalance
py tools/serial_logger.py --port COM5 --baud 115200 --label severe_unbalance
```

生成的 CSV 文件名会自动包含 label，例如：

```text
tinypredict_idle_20260502_193000.csv
tinypredict_normal_20260502_193100.csv
tinypredict_slight_unbalance_20260502_193200.csv
tinypredict_severe_unbalance_20260502_193300.csv
```

CSV 字段为：

```text
timestamp, label, ax, ay, az, vx, vy, vz, rms, status
```

终端会实时输出：

```text
label=normal, rms=0.025, status=NORMAL
```

如果需要指定 CSV 文件：

```bash
py tools/serial_logger.py --port COM5 --baud 115200 --label normal --output vibration_normal.csv
```

## 四组测试完成后的处理流程

四组分段测试完成后，建议按以下流程整理 V0.2 数据：

1. 确认最终有效 CSV 文件分别来自 `idle`、`normal`、`slight_unbalance`、`severe_unbalance` 四组。
2. 对于 `slight_unbalance`，前几次因胶带贴少或 MPU6050 连接不稳定产生的无效 CSV 不参与绘图和报告分析。
3. 只选择重新采集后连接稳定、偏心条件正确的 `slight_unbalance` CSV 作为最终结果。
4. 使用 `plot_rms.py` 按顺序拼接四组 CSV，生成 RMS 曲线图。
5. 将输出图保存到 `docs/images/rms_test_curve.png`，供测试报告引用。

示例命令：

```bash
py tools/plot_rms.py --csv tinypredict_idle_20260502_203200.csv tinypredict_normal_20260502_203243.csv tinypredict_slight_unbalance_20260502_204730.csv tinypredict_severe_unbalance_20260502_205423.csv
```

默认输出：

```text
docs/images/rms_test_curve.png
```
## RMS 分段绘图
### 最简单的一键绘图命令

四组 CSV 都采集完成后，可以直接运行：

```bash
py tools/plot_latest_v02.py
```

该脚本会自动在项目根目录查找最新的四组 CSV：

1. `idle`
2. `normal`
3. `slight_unbalance`
4. `severe_unbalance`

并按 `idle -> normal -> slight_unbalance -> severe_unbalance` 的顺序生成：

```text
docs/images/rms_test_curve.png
```

运行时会打印实际使用的 CSV 文件名，例如：

```text
Using idle: tinypredict_idle_20260502_203200.csv
Using normal: tinypredict_normal_20260502_203243.csv
Using slight_unbalance: tinypredict_slight_unbalance_20260502_204730.csv
Using severe_unbalance: tinypredict_severe_unbalance_20260502_205423.csv
```

`plot_rms.py` 可以一次读取多个 CSV，并按输入顺序拼接到一张图里。每段开始位置会标注 label，段与段之间会留出小间隔，表示这是分段测试，不是连续实时切换。

示例：

```bash
py tools/plot_rms.py --csv tinypredict_idle_xxx.csv tinypredict_normal_xxx.csv tinypredict_slight_unbalance_xxx.csv tinypredict_severe_unbalance_xxx.csv
```

默认输出：

```text
docs/images/rms_test_curve.png
```

如需指定输出文件：

```bash
py tools/plot_rms.py --csv tinypredict_idle_xxx.csv tinypredict_normal_xxx.csv --output fan_rms_plot.png
```

## Windows 使用步骤

### 第一种方式：Git Bash / CMD 手动运行

在项目根目录按顺序执行：

```bash
py tools/serial_logger.py --list-ports
py tools/serial_logger.py --port COM5 --baud 115200 --label idle
```

如果你使用的是 `python` 命令，也可以运行：

```bash
python tools/check_python_env.py
python -m pip install -r tools/requirements.txt
python -c "import serial; print(serial.__version__)"
python tools/serial_logger.py --list-ports
python tools/serial_logger.py --port COM5 --baud 115200 --label idle
```

### 第二种方式：双击运行

双击运行：

```text
tools/run_logger_windows.bat
```

该脚本会自动进入项目根目录，检查 Python 环境，安装依赖，列出可用串口，然后提示你输入真实 COM 号和测试 label。

## Available serial ports: none 排查

如果运行：

```bash
py tools/serial_logger.py --list-ports
```

输出：

```text
No serial ports detected.
```

这通常不是 STM32 固件问题，也不是 Python pyserial 问题，而是 Windows 当前没有识别到 USB 转 TTL 串口设备。

请按下面顺序检查：

1. 确认 USB 转 TTL 模块已经插好。
2. 确认 USB 线是数据线，不是只能充电的线。
3. 打开 Windows 设备管理器，查看 `端口 COM 和 LPT`。
4. 检查是否出现 `USB-Enhanced-SERIAL CH343`、`USB-SERIAL CH340`、`CP210x` 或 `USB Serial Port`。
5. 如果看不到 COM 口，需要安装对应 USB 转 TTL 驱动。
6. CH340 / CH341 对应 WCH CH341SER 驱动。
7. CP2102 对应 Silicon Labs CP210x VCP 驱动。
8. 尝试更换 USB 口、USB 数据线，确认 USB 转 TTL 模块指示灯是否亮。

## COM 口排查

请打开 Windows 设备管理器查看真实串口号：

```text
设备管理器 -> 端口 COM 和 LPT
```

找到类似下面的设备：

```text
USB-Enhanced-SERIAL CH343 (COM5)
USB Serial Port (COM7)
```

然后使用真实 COM 号运行：

```bash
py tools/serial_logger.py --port COM5 --baud 115200 --label normal
```

如果设备管理器显示的 COM 号不是 COM5，需要把命令里的 COM5 改成实际 COM 号。如果 COM 口存在但打不开，可能是串口助手已经占用了该端口。请关闭串口助手后再运行本工具。

## requirements.txt

依赖文件为：

```text
pyserial
matplotlib
```

注意：Python 包名是 `pyserial`，代码导入名是 `serial`，这是正常的。

## CSV 示例

```csv
timestamp,label,ax,ay,az,vx,vy,vz,rms,status
2026-05-02T18:30:00.123,normal,0.098,-0.860,0.504,-0.001,-0.218,-0.108,0.349,ALARM
```

## 注意事项

- 串口助手和本工具不能同时打开同一个 COM 口。
- 确认 USB 转 TTL 的 RX/TX 已与 STM32 PA9/PA10 交叉连接，并且 GND 共地。
- 如果串口断开或某一行数据解析失败，程序会打印错误并继续运行，不会因为单行异常直接退出。


