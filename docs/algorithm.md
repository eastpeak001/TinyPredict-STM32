# TinyPredict-STM32 振动检测算法说明

## 为什么不能直接用 ax/ay/az 判断振动

MPU6050 输出的 ax、ay、az 是三轴加速度，里面同时包含两类信息：

- 重力分量：由传感器姿态决定，静止时也存在。
- 动态分量：由设备振动、冲击或快速运动产生。

如果直接用 ax、ay、az 的大小或与固定基准的差值判断振动，慢慢倾斜模块时重力方向会改变，算法会把姿态变化误判为振动异常。这不适合工业设备振动监测，因为目标通常是检测快速抖动、偏心、松动等动态变化，而不是检测模块姿态。

## 低通估计重力分量

为了抑制姿态变化带来的误判，算法使用低通滤波估计慢变化的重力分量。

启动学习阶段使用较快的低通更新：

```c
gravity_x = gravity_x * 0.90f + ax * 0.10f;
gravity_y = gravity_y * 0.90f + ay * 0.10f;
gravity_z = gravity_z * 0.90f + az * 0.10f;
```

正常运行阶段使用较慢的低通更新：

```c
gravity_x = gravity_x * 0.95f + ax * 0.05f;
gravity_y = gravity_y * 0.95f + ay * 0.05f;
gravity_z = gravity_z * 0.95f + az * 0.05f;
```

这样，慢速倾斜会逐渐被认为是新的重力方向，而不会长期触发报警。

## 高通提取动态振动分量

动态振动分量通过原始加速度减去慢变化重力分量得到：

```c
vx = ax - gravity_x;
vy = ay - gravity_y;
vz = az - gravity_z;
```

其中 vx、vy、vz 表示高通后的动态分量，主要反映快速变化的振动。

## 滑动 RMS 计算

算法先计算当前振动幅值：

```c
vib_mag = sqrtf(vx * vx + vy * vy + vz * vz);
```

然后对最近 32 个 vib_mag 做滑动 RMS：

```c
rms = sqrtf(mean(vib_mag * vib_mag));
```

RMS 能够平滑瞬时噪声，同时保留持续振动的强度变化。当前窗口长度为 32 个采样点。

校准阶段不会把数据写入 RMS 窗口，且 rms 强制为 0，避免启动阶段低通尚未收敛时误报警。

## 状态判断阈值

基于当前风扇测试数据，运行阶段 NORMAL / WARNING / ALARM 的判断集中在 `system_status` 模块。OLED、串口 CSV 和 LED 报警共享同一状态来源。

核心进入阈值保持原项目含义不变：

| 状态切换 | 条件 | 连续次数 |
| --- | --- | ---: |
| NORMAL -> WARNING | rms >= 0.05 | 3 |
| WARNING -> ALARM | rms >= 0.15 | 3 |
| NORMAL -> ALARM | rms >= 0.15 | 3 |

恢复阈值加入迟滞：

| 状态切换 | 条件 | 连续次数 |
| --- | --- | ---: |
| WARNING -> NORMAL | rms < 0.04 | 5 |
| ALARM -> WARNING / NORMAL | rms < 0.12 后根据 RMS 回落 | 5 |

状态切换逻辑：

- NORMAL -> WARNING：RMS 连续 3 次大于等于 0.05 后进入 WARNING。
- WARNING -> ALARM：RMS 连续 3 次大于等于 0.15 后进入 ALARM。
- NORMAL -> ALARM：RMS 连续 3 次大于等于 0.15 时可以直接进入 ALARM。
- WARNING -> NORMAL：RMS 连续 5 次小于 0.04 后恢复 NORMAL。
- ALARM -> WARNING / NORMAL：RMS 连续 5 次小于 0.12 后退出 ALARM；如果当前 RMS 仍高于 WARNING 恢复区间则回到 WARNING，否则回到 NORMAL。

迟滞的作用是让进入阈值和恢复阈值错开，避免 RMS 在 0.05 或 0.15 附近轻微波动时频繁跳变。连续计数防抖要求条件持续满足多次才切换状态，可降低偶发噪声对 OLED、串口状态和 PC13 LED 报警的影响。

说明：启动学习阶段由 `vibration_algo` 输出 CALIBRATING，不属于运行阶段报警状态；`alarm_task` 在 CALIBRATING 阶段保持 PC13 LED 关闭。

实测参考：

| 工况 | RMS |
| --- | ---: |
| 静止/稳定状态 | 0.012 |
| 正常运行状态 | 0.017 |
| 轻微异常状态 | 0.085 |
| 明显异常状态 | 0.400 |

## 算法局限性

- 当前阈值来自单次风扇测试，不一定适用于所有设备。
- MPU6050 的安装方式会显著影响 RMS 数值。
- 当前算法主要用于快速振动检测，不包含频谱分析和特征频率识别。
- 当前未区分不同轴向的故障来源。
- 当前未加入温度补偿、长期漂移补偿和传感器自检。
- 当前未保存历史数据，无法进行趋势分析。

后续可以增加 FFT、包络分析、阈值自学习和数据记录功能，以提高工业场景下的诊断能力。



