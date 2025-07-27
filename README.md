# 【交接专用】四轮循迹模块测试平台超详细说明文档

## 1. 【重要】项目背景与现状

**请务必先阅读本节！**

各位接手的同事请注意，本项目的当前状态与代码仓库的名称 **严重不符**。

-   **项目名称的历史遗留问题**：本项目最初是为一个 **两轮自平衡小车** 而创建的，因此仓库和工程名是 `balance car`。
-   **当前实际功能**：由于原项目被砍，现在它已经被改造为一个多功能测试平台，具有多种用途。
-   **车体结构**：
    *   **四轮结构**：前二轮为无动力的万向轮，后二轮为驱动轮。
    *   **后轮驱动**：由两个电机提供动力。
    *   **前进方式**：采用一个牛眼轮推动前进的方式，不依靠舵机转向。
-   **核心任务**：当前项目的主要目标是作为 **硬件模块的测试平台** 和 **未来功能扩展的基础**。

---

## 2. 环境搭建与运行

本项目基于"立创·地猛星MSPM0G3507开发板"，所有环境搭建、编译、烧录的详细步骤，请 **严格参考** 以下官方中文文档站，其中包含了最权威、最详尽的图文教程：

➡️ **[https://wiki.lckfb.com/zh-hans/](https://wiki.lckfb.com/zh-hans/)**

请在该网站中查找与 **MSPM0G3507** 相关的教程。

---

## 3. 【必读】平台特性与巨坑警告

在开始看代码前，请务必了解这块单片机（MCU）的一些"个性"和局限性，能帮你省下大量调试时间。

### 警告1：SysTick只有毫秒(ms)精度，微秒(us)延时是"假"的！
-   **问题**：这颗MCU内置的滴答定时器（SysTick）中断，只能实现`ms`级别的非阻塞延时。
-   **`delay_us`的真相**：代码中虽然提供了`delay_us`函数，但它并非由定时器实现，而是通过`delay_cycles`函数，让CPU执行一个根据时钟频率计算出的、精确的空循环。
-   **结论**：`delay_ms`是较为高效的（触发中断后CPU可干别的事），而`delay_us`是 **纯粹的阻塞式延时**，在使用微秒延时期间，CPU将被完全占用，无法处理任何其他任务。请在使用时特别注意这一点。

### 特性2：编码器硬件资源匮乏，纯靠中断模拟
-   **问题**：这颗MCU片上没有足够的硬件正交编码器接口（QEI）来同时处理两个电机的编码器。
-   **解决方案**：两个后轮电机的编码器，**全部** 都是通过配置GPIO的 **双边沿外部中断** 来模拟QEI功能的。
-   **实现方式**：在中断服务函数中，通过判断当前哪个引脚（A相或B相）触发了中断，并结合另一相引脚的电平状态，用软件逻辑来判断方向和累加/减计数，从而计算出速度。
-   **你需要知道的**：`Hardware/Encoder/` 里的代码实现了这个复杂的逻辑。这是一个因硬件资源不足而采取的纯软件补偿方案。

### 特性3：没有标准数据类型，已手动定义
-   **问题**：这个新的MCU平台（或其编译器工具链）默认不包含 `stdint.h` 头文件，因此无法直接使用像 `uint8_t`, `int16_t` 这样的标准C语言数据类型。
-   **解决方案**：为了代码的可移植性和可读性，我们已经在 `system/sys/sys.h` 文件中手动 `typedef` 了所有常用的标准数据类型。
-   **你需要知道的**：在编写代码时，正常包含`sys.h`即可使用这些标准类型。如果发现类型未定义，请检查是否包含了 `system/sys/sys.h`。

---

## 4. 文件结构一览
```
.
├── empty.c                 # MCU的主程序入口
├── Hardware/               # 【核心】硬件驱动代码
│   ├── APP/                # 应用层通信协议
│   ├── Control/            # 循迹车控制逻辑
│   ├── Encoder/            # 电机编码器驱动 (纯中断模拟)
│   ├── HTS221/             # HTS221总线舵机驱动
│   ├── key/                # 按键驱动
│   ├── MPU6050/            # MPU6050姿态传感器驱动
│   ├── OLED/               # OLED显示屏驱动
│   ├── PID/                # PID算法实现
│   ├── SMS_STS/            # SMS_STS舵机云台驱动
│   ├── Serial/             # 串口底层驱动
│   ├── Servo/              # 普通舵机驱动 (需重构，当前版本有问题)
│   ├── Tracking/           # 红外循迹传感器驱动 (替代原line_follower)
│   └── WIT/                # 维特智能IMU驱动
├── README.md               # 本文档
└── system/                 # MCU底层系统配置
    ├── delay/              # 延时函数
    ├── sys/                # 系统核心配置
    └── topic/              # 消息主题管理系统
```
---

## 5. 模块详解

### **主干逻辑与核心模块**

#### `empty.c` - 项目的大脑
-   **作用**: 程序主入口`main`函数所在地，负责初始化系统并进入主循环，协调所有模块工作。
-   **当前功能**: 当前主要用于驱动和测试SMS_STS总线舵机，包括设置角度、读取位置信息，并在OLED上显示。
-   **工作流程**:
    1.  `SYSCFG_DL_init()`: TI工具生成的底层硬件初始化。
    2.  `OLED_Init()`, `APP_Init()`, `WIT_Init()`: 调用各个模块的初始化函数。
    3.  `NVIC_EnableIRQ()`: 开启所需的中断。
    4.  `while(1)`: 主循环，控制舵机并在OLED上显示信息。

##### 关键代码示例
下面是 `empty.c` 中在主循环里对 SMS_STS 舵机操作的简化示例：
```c
// 初始化和中断使能省略
while (1) {
    float angle = 0.0f;
    SMS_STS_Set_Angle(1, angle, 0, 3090);
    delay_ms(10);
    SMS_STS_Set_Angle(2, angle, 0, 3090);
    delay_ms(200);
    SMS_STS_Read(1);
    delay_ms(50);
    SMS_STS_Read(2);
    delay_ms(50);
    OLED_ShowNum(1, 10, (uint16_t)angle, 3);
    angle += 30.0f;
}
```7:24:empty.c

#### `Control/` - 循迹车控制核心
-   **作用**: 实现循迹车的核心逻辑，包括速度和方向控制。它接收来自`Tracking`模块的数据，通过PID算法计算后，控制后轮驱动电机。
-   **关键函数**:
    -   `Control_forward()` / `Control_back()` / `Control_stop()`: 控制电机定性运动。
    -   `Control_speed(int left, int right)`: 分别设置左右轮的速度和方向（正负代表方向）。

##### 关键代码示例
下面展示了 `Control_speed` 用于设置左右电机 PWM 和方向的核心逻辑：
```c
void Control_speed(int left_speed, int right_speed) {
    if (left_speed > 0) {
        Control_AIN1_ON();
        Control_AIN2_OFF();
        DL_TimerA_setCaptureCompareValue(DL_TimerA_INST, left_speed, DL_TimerA_CC1_INST);
    } else if (left_speed < 0) {
        Control_AIN1_OFF();
        Control_AIN2_ON();
        DL_TimerA_setCaptureCompareValue(DL_TimerA_INST, -left_speed, DL_TimerA_CC1_INST);
    }
    // ...省略右轮控制...
}
```49:80:Hardware/Control/Control.c

#### `Encoder/` - 编码器驱动 (纯中断模拟)
-   **作用**: 测量两个后轮电机的转速和方向，是速度PID控制的基础。
-   **实现方式**: 如"巨坑警告"所述，两个编码器**都**是使用GPIO外部中断模拟的。

#### `Tracking/` - 红外循迹传感器
-   **作用**: 取代了原来的line_follower模块（原文档已丢失），检测地面黑线，为`Control`模块提供方向决策依据。
-   **工作原理**: 通过读取12个红外传感器的状态，计算出当前车身与黑线的偏差值，用于PID控制转向。
-   **核心函数**: `xunji()` 返回包含偏差值和是否检测到线的结构体。

##### 关键代码示例
`xunji` 函数读取 12 路传感器并根据状态位计算偏差：
```c
TrackingStatus xunji(void) {
    uint16_t pin_value = Tracking_get_pin_value();
    switch (pin_value) {
        case 0b100000000000: last_deviation = 55; break;
        // ...更多case...
        case 0b000000000000: status.line_detected = 0; break;
    }
    status.deviation = last_deviation;
    return status;
}
```18:47:Hardware/Tracking/Tracking.c

#### `SMS_STS/` - 舵机云台
-   **作用**: 用于驱动SMS_STS系列总线舵机，主要用作舵机云台，注意其扭矩不足以用于车辆转向控制。
-   **核心功能**:
    -   `SMS_STS_Set_Angle()`: 通过角度控制舵机转动
    -   `SMS_STS_Read()`: 读取舵机当前位置
    -   `SMS_STS_Set_Mode()`: 修改舵机工作模式
    -   `SMS_STS_Set_Speed()`: 设置恒速模式下的转速

##### 关键代码示例
下面展示 `SMS_STS_Run` 组装并发送写位置指令帧的过程：
```c
void SMS_STS_Run(uint8_t ID, uint16_t Position, uint16_t RunTime, uint16_t Speed) {
    sms_tx_buf[0] = SMS_STS_FRAME_HEADER;
    sms_tx_buf[1] = SMS_STS_FRAME_HEADER;
    sms_tx_buf[2] = ID;
    sms_tx_buf[5] = SMS_STS_REG_GOAL_POSITION;
    // …填充Position、RunTime、Speed字节…
    Serial_SendArray(sms_tx_buf, 13);
}
```22:50:Hardware/SMS_STS/SMS_STS.c

#### `PID/` - PID控制器
-   **作用**: 通用的PID算法实现，被`Control`模块调用来计算电机和舵机的控制量。
-   **关键函数**:
    -   `Turn_Pid()`: 位置式PID，用于转向控制，根据目标角度和当前角度计算修正值。
    -   `FeedbackControl()`: 增量式PI，用于速度控制，根据目标速度和当前速度计算输出。

#### `OLED/` - OLED显示屏
-   **作用**: 调试"功臣"，用于在屏幕上实时显示各种关键数据。

### **通信与底层模块**

#### `APP/` - 上位机通信与调试模块
-   **核心作用**：一个专为**调试**设计的应用层模块，用于将MCU数据打包后通过串口发送给PC上位机（如FireWater）。
-   **精髓设计：函数指针**：通过`APP_Init(app_printf_t p_printf)`函数，将底层的串口发送函数（如`uart0_printf`）注册进来，实现了解耦。

#### `Serial/` - 串口驱动
-   **作用**: 封装了UART的底层收发功能，为`APP`模块和其他需要串口通信的模块提供基础。
-   **主要功能**: 提供字符、字符串、数组发送和格式化打印函数。

#### `system/` - 系统底层
-   **`sys/`**: 负责系统初始化、时钟配置，并**定义了平台缺失的标准数据类型**。
-   **`delay/`**: 提供`delay_ms`（基于SysTick）和`delay_us`（阻塞式）延时函数。
-   **`topic/`**: 新增模块，用于系统内部消息主题管理。

### **当前可用的功能模块**

#### `WIT/` 和 `MPU6050/` - IMU姿态传感器
-   **功能**: 提供车辆姿态信息，可用于姿态控制或状态监测。
-   **`WIT` (推荐)**: 配置为DMA模式，应用层可直接读取姿态数据，无需关心刷新过程。
-   **`MPU6050`**: 作为备选方案，提供另一种IMU数据读取方式。

#### `key/` - 按键
-   **功能**: 实现用户交互。可以定义按键功能为启动/停止、模式切换等。
-   **工作原理**: 内部实现了一个状态机，能够自动处理按键的抖动，并准确区分单击、双击和长按三种事件。

#### `Servo/` - 普通PWM舵机
-   **状态**: 当前版本存在问题，需要重构，暂不建议使用。
-   **功能**: 理论上提供对标准PWM控制舵机的支持，用于简单的角度控制。

### **已移除的模块**
- **`HCSR04/`**: 超声波传感器驱动已被移除，因为项目不需要且存在一些问题。
- **`line_follower/`**: 因交接问题及文档丢失，已被`Tracking`模块替代。

### **系统运行流程**
当前系统主要用于测试和验证SMS_STS总线舵机功能，主循环中执行以下操作：
1. 设置两个舵机按指定角度转动
2. 读取舵机的当前位置信息
3. 在OLED上显示舵机控制报文和位置信息
4. 更新目标角度，实现循环转动效果


