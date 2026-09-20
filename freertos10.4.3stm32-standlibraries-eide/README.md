# STM32F103C8 FreeRTOS 示例工程

这是一个面向 FreeRTOS 学习的 STM32F103C8 示例工程。项目基于 STM32F10x
标准外设库开发，通过两个 FreeRTOS 任务分别控制 PA0 和 PA1，以不同周期输出
电平变化，演示任务创建、抢占式调度以及阻塞延时的基本用法。

## 技术栈

| 类别 | 技术或配置 |
| --- | --- |
| 微控制器 | STM32F103C8 |
| CPU 内核 | ARM Cortex-M3 |
| 编程语言 | C99、ARM 汇编 |
| 实时操作系统 | FreeRTOS V10.4.3 |
| 外设驱动 | STM32F10x Standard Peripheral Library |
| 内核接口 | CMSIS / CMSIS-Core |
| 编译器 | ARM Compiler 5（ARMCC/AC5） |
| 开发环境 | VS Code + EIDE，或 Keil MDK |
| 下载与调试 | ST-Link / SWD；EIDE 也预置 J-Link 和 OpenOCD 配置 |
| FreeRTOS 内存管理 | `heap_4`，动态堆大小 8 KB |

> 本工程使用 STM32 标准外设库，不是 STM32 HAL 库。

## 目录结构

```text
.
├── EIDE/                 # VS Code EIDE 工程配置及构建目录
├── HardWare/             # 板级硬件驱动，目前包含 LED/GPIO 驱动
├── Library/              # STM32F10x 标准外设库
├── Start/                # 启动文件、CMSIS 和系统时钟配置
├── System/
│   ├── include/          # FreeRTOS 头文件
│   ├── source/           # FreeRTOS 内核源码
│   └── portable/         # Cortex-M3 移植层和 heap_4
├── User/                 # 应用入口、中断处理和 FreeRTOS 配置
├── Project.uvprojx       # Keil MDK 工程文件
└── README.md
```

## 程序运行逻辑

程序入口位于 `User/main.c`：

1. `Hardware_Init()` 初始化 GPIOA 的 PA0、PA1 为 50 MHz 推挽输出。
2. 使用 `xTaskCreate()` 动态创建 `Task1` 和 `Task2`。
3. 调用 `vTaskStartScheduler()` 启动 FreeRTOS 调度器。
4. 两个任务以相同优先级运行，并通过 `vTaskDelay()` 主动进入阻塞状态。

| 任务 | 引脚 | 高/低电平保持时间 | 完整周期 | 优先级 | 栈深度 |
| --- | --- | ---: | ---: | ---: | ---: |
| `Task1` | PA0 | 500 ms | 1000 ms | 2 | 128 words |
| `Task2` | PA1 | 500 ms | 1000 ms | 2 | 128 words |

当前 PA0 和 PA1 均保持 500 ms 高电平、500 ms 低电平，LED 完整闪烁周期为
1000 ms。此次调整将 `Task1` 的单次延时由 300 ms 增加到 500 ms，使两个 LED
以相同频率闪烁。

如果任一任务创建失败，程序会在错误循环中持续翻转 PA0。

## FreeRTOS 关键配置

配置文件为 `User/FreeRTOSConfig.h`。

| 配置项 | 当前值 | 说明 |
| --- | ---: | --- |
| `configUSE_PREEMPTION` | 1 | 启用抢占式调度 |
| `configTICK_RATE_HZ` | 1000 | 系统节拍为 1 ms |
| `configMAX_PRIORITIES` | 56 | 最大任务优先级数量 |
| `configSUPPORT_DYNAMIC_ALLOCATION` | 1 | 支持动态创建内核对象 |
| `configSUPPORT_STATIC_ALLOCATION` | 0 | 未启用静态分配 |
| `configTOTAL_HEAP_SIZE` | 8192 | FreeRTOS 堆为 8 KB |
| `configUSE_TIMERS` | 1 | 启用软件定时器 |

工程使用 `System/portable/heap_4.c` 管理 FreeRTOS 动态内存。`heap_4` 支持相邻
空闲块合并，相比简单的只分配方案更适合会反复创建和删除对象的应用。

## 硬件连接

- 目标芯片：STM32F103C8
- 调试接口：SWD
- 示例输出：PA0、PA1

可以将 LED 串联限流电阻后连接至 PA0 和 PA1，也可以使用逻辑分析仪观察输出。
LED 的有效电平取决于实际开发板接法。

## 使用 EIDE 构建

### 环境要求

- Visual Studio Code
- EIDE 扩展
- Keil ARM Compiler 5 工具链
- ST-Link 驱动及下载工具

### 操作步骤

1. 使用 VS Code 打开 `EIDE/Project.code-workspace`。
2. 在 EIDE 中选择 `Target 1`。
3. 确认工具链为 ARM Compiler 5。
4. 执行构建。
5. 连接 ST-Link，通过 SWD 下载并运行固件。

EIDE 当前构建配置包括：

- CPU：Cortex-M3
- 优化等级：`-O0`
- 调试信息：启用
- C 标准：C99
- 运行库：MicroLIB
- Flash：起始地址 `0x08000000`，大小 64 KB
- RAM：起始地址 `0x20000000`，大小 20 KB

## 使用 Keil MDK 构建

1. 使用 Keil MDK 打开根目录下的 `Project.uvprojx`。
2. 选择工程目标并执行 Build。
3. 在调试器设置中选择 ST-Link 和 SWD 接口。
4. 下载程序至目标板并运行。

## 核心代码示例

```c
xTaskCreate(vTask1, "Task1", 128, NULL, 2, NULL);
xTaskCreate(vTask2, "Task2", 128, NULL, 2, NULL);
vTaskStartScheduler();
```

任务通过阻塞延时让出 CPU：

```c
GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);
vTaskDelay(pdMS_TO_TICKS(500));
GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET);
vTaskDelay(pdMS_TO_TICKS(500));
```

在 FreeRTOS 任务中，应优先使用 `vTaskDelay()` 等阻塞 API，避免使用空循环延时
长期占用 CPU。

## 注意事项

- `FreeRTOSConfig.h` 文件头仍标注 V10.3.1，但 FreeRTOS 内核源码及
  `System/include/task.h` 标明的实际版本为 V10.4.3。
- 两个示例任务优先级相同，由 FreeRTOS 进行时间片调度。
- 当前任务句柄未保存；如果后续需要暂停、恢复或删除指定任务，应保存
  `xTaskCreate()` 返回的任务句柄。
- 添加会调用 FreeRTOS API 的中断时，应遵守
  `configMAX_SYSCALL_INTERRUPT_PRIORITY` 的中断优先级限制。

## 后续学习建议

可以在此工程基础上继续添加：

- 队列：在任务之间传递数据
- 二值信号量：实现中断与任务同步
- 互斥量：保护串口等共享资源
- 软件定时器：执行周期性非阻塞操作
- 任务通知：实现轻量级任务间通信
- 栈溢出检测和运行时统计

## License

许可证信息请参见根目录下的 `LICENSE` 文件。
