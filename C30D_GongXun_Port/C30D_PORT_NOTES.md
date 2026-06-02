# C30D_GongXun_Port 移植说明

本目录是从 `GongXun2025-main/控制代码` 复制出来的移植工程，原工程不改。

## 当前目标

先验证 C30D 主控板 + HWT101 + 串口张大头闭环步进电机能不能跑出原 GongXun 底盘控制效果。

## 测试按键

- C30D 原厂 KEY：PE0
- 当前底盘测试入口等待 PE0 按键触发
- GongXun 原工程的 PB15 按键配置已不再用于 C30D 测试

## 默认运行模式

`User/main.c` 里默认开启：

```c
#define C30D_CHASSIS_TEST 1
```

上电后只初始化按键、OLED、HWT101 和底盘步进电机。按下 C30D 原厂 KEY（PE0）后依次执行：

1. 向前慢速移动
2. 向右平移
3. 右转 90 度

如果要恢复整套比赛流程，把这个宏改为 `0`。

## 串口分配

- 张大头步进电机驱动器：USART2 TX，PD5
- HWT101：USART3 RX，PD9
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
- 驱动器 ID 建议设为：
  - 1：左前轮
  - 2：左后轮
  - 3：右后轮
  - 4：右前轮

当前代码只用单向发送速度指令。不要把多个驱动器 TX 直接并在一起接回主控 RX，除非确认驱动器支持总线并联回复，或者中间加了合适的总线电路。

## HWT101 接线

- HWT101 TX 接 C30D PD9/USART3_RX
- HWT101 GND 接 C30D GND
- 当前代码按 GongXun 原工程配置为 115200 波特率。HWT101 如果还是出厂 9600，需要先把模块改到 115200，或者把 `Hardware/HWT101.c` 里的 `USART_BaudRate` 改回 9600。
