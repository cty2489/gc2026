# Mac 开发说明

作者：CTY

本工程已经补了 GNU ARM 工具链支持，可以在 macOS 下不用 Keil 编译。

## 编译

在本目录执行：

```sh
make
```

成功后会生成：

- `build/C30D_GongXun_Port.elf`
- `build/C30D_GongXun_Port.hex`
- `build/C30D_GongXun_Port.bin`
- `build/C30D_GongXun_Port.map`

清理构建产物：

```sh
make clean
```

## 烧录

默认使用 CMSIS-DAP / DAPLink，通过 `pyocd` 烧录。

如果还没安装 `pyocd`，先执行：

```sh
python3 -m pip install pyocd
```

如果是新电脑第一次烧 STM32F407ZGTx，再安装一次芯片包：

```sh
python3 -m pyocd pack install STM32F407ZGTx
```

接好 CMSIS-DAP 后先查看探针：

```sh
make list-probes
```

烧录：

```sh
make flash
```

默认目标是 `stm32f407zgtx`，连接方式是 `under-reset`，频率是 `100000Hz`。如果不接 NRST，可以临时改用热插：

```sh
make flash PYOCD_CONNECT=attach
```

如果要临时使用 ST-LINK：

```sh
make flash-stlink
```

## VS Code

可以直接用 VS Code 打开本目录：

```sh
code .
```

已经加入：

- `.vscode/tasks.json`：构建、清理、烧录任务
- `.vscode/c_cpp_properties.json`：STM32F407ZG 的头文件和宏配置

快捷键 `Cmd + Shift + B` 默认执行 `make`。

## 文件说明

- `Makefile`：Mac/GCC 构建入口
- `STM32F407ZG_FLASH.ld`：STM32F407ZG 1MB Flash 链接脚本
- `Start/startup_stm32f40_41xxx_gcc.S`：GNU 汇编启动文件
- `System/syscalls.c`：newlib 占位系统调用，避免链接警告

## 注意

作者：CTY

当前 `User/main.c` 默认是 C30D 底盘位置模式小距离测试：

```c
#define C30D_HWT101_TEST 0
#define C30D_WHEEL_STEPPER_TEST 0
#define C30D_POSITION_WHEEL_TEST 0
#define C30D_POSITION_TEST_STEP 1600
#define C30D_CHASSIS_TEST 1
#define C30D_CHASSIS_TEST_MM 300
#define C30D_CHASSIS_TEST_RPM 60
#define C30D_CHASSIS_TEST_ACC 100
```

当前 IO 分配为：`PD5/USART2_TX` 发送张大头步进电机控制指令，`PD6/USART2_RX` 接收 HWT101 主动上报数据，二者共用 USART2 的不同方向。当前底盘电机 ID 分配为 ID1 左后轮、ID2 左前轮、ID3 右后轮、ID4 右前轮。当前测试按下 PE0 后执行前进 300mm、左移 300mm、后退 300mm、右移 300mm 的位置模式小方形路径，当前速度 60 RPM、Acc=100；若要切换到 HWT101 单测、速度模式四轮逐个测试或单轮位置同步测试，按 `C30D_PORT_NOTES.md` 中的运行模式说明修改宏定义。
