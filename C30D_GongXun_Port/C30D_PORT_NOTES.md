# C30D_GongXun_Port 移植说明

作者：陈天远

本目录是从 `GongXun2025-main/控制代码` 复制出来的移植工程，原工程不改。

## 当前目标

先验证 C30D 主控板 + HWT101 + 串口张大头闭环步进电机能不能跑出原 GongXun 底盘控制效果。当前阶段已经完成 USART2 共用方案验证：PD5 用于向张大头驱动器发送控制指令，PD6 用于接收 HWT101 主动上报的角度数据；OLED、PE0 按键、HWT101 陀螺仪、四个张大头底盘电机均已完成单项测试。

## 本版改动内容

作者：陈天远

本版在复制工程 `C30D_GongXun_Port` 中完成以下改动：

1. 将张大头步进电机控制口固定为 `USART2_TX/PD5`，用于向驱动器 RX 单向发送速度、停止和同步运动指令。
2. 将 HWT101 从原先移植测试用的 `USART3_RX/PD9` 改为 `USART2_RX/PD6`，与步进电机共用 USART2，但只使用接收方向。
3. 将 USART2 初始化模式改为 `USART_Mode_Tx | USART_Mode_Rx`，避免步进和 HWT101 分别初始化时互相关闭收发方向。
4. 将 HWT101 接收 DMA 改为 `DMA1_Stream5`，并将空闲中断入口改为 `USART2_IRQHandler`。
5. 保留 C30D 原生 OLED 显示支持，用于显示 HWT101 角度、角速度、接收状态和测试状态。
6. 增加 HWT101 单独测试模式、四轮张大头逐个测试模式、底盘单独测试模式三种入口，方便逐步调试。
7. 根据四轮实测结果修正底盘电机 ID 映射：ID1 为左后轮，ID2 为左前轮，ID3 为右后轮，ID4 为右前轮。
8. 当前主程序切换到底盘标定测试模式，按下 PE0 后执行一次前进测试和一次左移测试，用于测量当前速度时间开环下的实际距离。

## 可实现内容

作者：陈天远

当前代码可以实现：

1. 在 C30D 主控板上通过 `PD5/USART2_TX` 控制张大头闭环步进驱动器。
2. 在 C30D 主控板上通过 `PD6/USART2_RX` 接收 HWT101 的 Yaw 和 Wz 数据。
3. 在 OLED 上实时显示 HWT101 的偏航角、角速度和接收标志位。
4. 通过 C30D 原厂按键 `PE0` 对当前 Yaw 显示值进行清零。
5. 通过四轮测试模式逐个验证 ID1、ID2、ID3、ID4 的轮子编号和正反转方向。
6. 为后续恢复底盘闭环运动测试提供统一的串口和 IO 映射基础。
7. 当前可以进入底盘标定测试，验证 HWT101 姿态保持和四轮张大头驱动配合是否正常。

## IO 口定义总表

作者：陈天远

| 功能模块 | C30D IO | 外设功能 | 连接对象 | 当前用途 |
| --- | --- | --- | --- | --- |
| 张大头步进驱动器 | PD5 | USART2_TX | 驱动器 RX | 主控单向发送控制指令 |
| HWT101 | PD6 | USART2_RX | HWT101 TX | 主控接收角度和角速度数据 |
| C30D 原厂按键 | PE0 | GPIO 输入 | 板载 KEY | 测试触发、Yaw 清零 |
| OLED RST | PD12 | GPIO 输出 | OLED RST | C30D 原生 OLED 复位 |
| OLED DC/RS | PD11 | GPIO 输出 | OLED DC/RS | C30D 原生 OLED 命令/数据选择 |
| OLED SCL | PD14 | GPIO 输出 | OLED SCL | C30D 原生 OLED 时钟 |
| OLED SDA | PD13 | GPIO 输出 | OLED SDA | C30D 原生 OLED 数据 |
| 视觉摄像头 TX | PC12 | UART5_TX | 摄像头 RX | 保持 GongXun 原分配 |
| 视觉摄像头 RX | PD2 | UART5_RX | 摄像头 TX | 保持 GongXun 原分配 |
| 二维码模块 TX | PC10 | UART4_TX | 二维码 RX | 保持 GongXun 原分配 |
| 二维码模块 RX | PC11 | UART4_RX | 二维码 TX | 保持 GongXun 原分配 |

## 底盘电机 ID 分配

作者：陈天远

| 张大头驱动器 ID | 底盘位置 | 当前说明 |
| --- | --- | --- |
| ID1 | 左后轮 | 已按四轮单独测试结果确认 |
| ID2 | 左前轮 | 已按四轮单独测试结果确认 |
| ID3 | 右后轮 | 已按四轮单独测试结果确认 |
| ID4 | 右前轮 | 已按四轮单独测试结果确认 |

## 测试按键

- C30D 原厂 KEY：PE0
- 当前底盘测试入口等待 PE0 按键触发
- GongXun 原工程的 PB15 按键配置已不再用于 C30D 测试

## 默认运行模式

`User/main.c` 里当前默认开启底盘标定测试：

```c
#define C30D_HWT101_TEST 0
#define C30D_WHEEL_STEPPER_TEST 0
#define C30D_CHASSIS_TEST 1
```

上电后屏幕显示：

```text
C30D Test
Press Key
```

按 C30D 原厂 KEY（PE0）后，程序会先执行一次前进测试，再执行一次左移测试，用于检查当前底盘 ID、方向、HWT101 姿态保持和实际位移误差。

如果要切回 HWT101 陀螺仪单独测试，改为：

```c
#define C30D_HWT101_TEST 1
#define C30D_WHEEL_STEPPER_TEST 0
#define C30D_CHASSIS_TEST 1
```

HWT101 单测模式下，屏幕显示：

```text
HWT101 Test
Yaw:+0000.0
Wz :+0000.0
A:0 W:0 KeyZero
```

如果要恢复四轮步进电机逐个测试，改为：

```c
#define C30D_HWT101_TEST 0
#define C30D_WHEEL_STEPPER_TEST 1
#define C30D_STEPPER_TEST_RPM 60
```

四轮测试模式下，上电后屏幕显示 `Wheel Test`。每按一次 C30D 原厂 KEY（PE0），测试下一个张大头驱动器 ID，顺序为 1 -> 2 -> 3 -> 4 -> 1 循环。每个 ID 的测试流程为：

1. 正转 60 RPM，2 秒
2. 停止，0.8 秒
3. 反转 60 RPM，2 秒
4. 停止并显示 `IDx Done`

如果要恢复底盘测试，把四轮步进测试宏改为 `0`，并保留：

```c
#define C30D_CHASSIS_TEST 1
```

底盘测试模式下，上电后只初始化按键、OLED、HWT101 和底盘步进电机。按下 C30D 原厂 KEY（PE0）后依次执行：

1. 向前慢速移动
2. 向左平移

如果要恢复整套比赛流程，把这个宏改为 `0`。

## 串口分配

- 张大头步进电机驱动器：USART2 TX，PD5
- HWT101：USART2 RX，PD6
- 视觉摄像头：UART5，PC12/PD2，保持原 GongXun 分配
- 二维码模块：UART4，PC10/PC11，保持原 GongXun 分配

## OLED 接线

已换成 C30D 原生 4 线 OLED 驱动，保留 GongXun 工程原来的 `OLED_Init`、`OLED_ShowString`、`OLED_Clean` 等调用接口。

- RST：PD12
- DC/RS：PD11
- SCL：PD14
- SDA：PD13

## 张大头驱动器接线

- C30D PD5/USART2_TX 接 4 个张大头驱动器 RX
- C30D GND 和所有驱动器 GND 共地
- 驱动器 ID 当前设为：
  - 1：左后轮
  - 2：左前轮
  - 3：右后轮
  - 4：右前轮

当前代码只用单向发送速度指令。不要把多个驱动器 TX 直接并在一起接回主控 RX，除非确认驱动器支持总线并联回复，或者中间加了合适的总线电路。

## HWT101 接线

- HWT101 TX 接 C30D PD6/USART2_RX
- HWT101 GND 接 C30D GND
- HWT101 当前只接收模块主动上报数据，不给模块发送配置命令；所以不用接 HWT101 RX
- 当前代码按 GongXun 原工程配置为 115200 波特率。HWT101 如果还是出厂 9600，需要先把模块改到 115200，或者把 `Hardware/HWT101.c` 里的 `USART_BaudRate` 改回 9600。
