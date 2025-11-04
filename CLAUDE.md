# CLAUDE.md

此文件为Claude Code (claude.ai/code)在处理此仓库代码时提供指导。

## 项目概述

这是一个基于STM32F103ZET6的四轮驱动（4WD）越野车控制系统。项目使用STM32CubeMX配置和Keil MDK-ARM进行开发。

## 构建和开发命令

### 项目构建
- **主要IDE**: Keil MDK-ARM V5.32
- **项目文件**: `MDK-ARM/ZET6_Dianji.uvprojx`
- **构建输出**: `MDK-ARM/ZET6_Dianji/ZET6_Dianji.hex` 和 `ZET6_Dianji.axf`

### 清理构建文件
- 运行 `keilkill.bat` 清理临时构建文件（*.o, *.d, *.crf等）
- 这个批处理文件会移除编译产物和临时文件

### STM32CubeMX配置
- 配置文件: `ZET6_Dianji.ioc`
- 修改外设配置时使用STM32CubeMX重新生成代码

## 项目架构

### 硬件配置
- **MCU**: STM32F103ZET6 (Cortex-M3, 512KB Flash, 64KB RAM)
- **系统时钟**: 72MHz
- **外部晶振**: 8MHz HSE

### 外设配置
- **定时器**:
  - TIM1: 电机PWM生成（4通道）
  - TIM8: 电机PWM生成（4通道）
  - TIM2, TIM3, TIM4, TIM5: 编码器接口（正交编码器模式）
- **USART1**: UART通信（PA9-TX, PA10-RX）
- **GPIO**: 多个传感器数字输入引脚（XIN1-XIN16）和控制信号

### 目录结构
```
├── Core/                    # STM32 HAL生成代码
│   ├── Inc/                 # 头文件
│   └── Src/                 # 源文件（main.c, hal_msp.c等）
├── Hadeware/               # 硬件抽象层
│   ├── motor_driver.c/h    # 电机PWM控制（4个电机）
│   ├── motor_app.c/h       # 电机应用逻辑
│   ├── encoder_driver.c/h  # 编码器读取和速度计算
│   ├── encoder_app.c/h     # 编码器应用逻辑
│   ├── pid.c/h            # PID控制算法
│   ├── pid_app.c/h        # PID应用实现
│   ├── scheduler.c/h      # 任务调度器
│   ├── uart_app.c/h       # UART通信
│   ├── gray_app.c/h       # 灰度传感器处理
│   └── led_app.c/h        # LED控制
├── mpu6050/               # IMU传感器实现
│   ├── mpu6050.c/h        # MPU6050驱动
│   ├── soft_i2c.c/h       # 软件I2C实现
│   └── mpu6050_app.c/h    # MPU6050应用逻辑
├── MDK-ARM/               # Keil项目文件
└── ZET6_Dianji.ioc        # STM32CubeMX配置
```

### 核心组件

#### 电机控制系统
- **4个电机**: 配置为2个前电机 + 2个后电机（左右对）
- **PWM控制**: 1000步分辨率（0-999）带方向控制
- **功能**: `Set_PWM(PWM_L, PWM_R, PWM_L_2, PWM_R_2)` 设置电机功率

#### 编码器系统
- **4个编码器**: 每个电机一个用于闭环速度控制
- **规格**: 13线编码器配20:1减速箱，4倍频正交检测
- **PPR**: 每转1040个脉冲（13 × 20 × 4）
- **速度计算**: 通过调度器每10ms更新一次

#### PID控制器
- **位置式PID**: 用于精确位置控制
- **增量式PID**: 用于平滑速度控制
- **结构**: 包含Kp、Ki、Kd参数和输出限幅

#### 任务调度器
- **周期**: 10ms主循环周期
- **初始化**: `Scheduler_Init()` 设置所有外设
- **运行时**: `Scheduler_Run()` 执行所有周期性任务

#### MPU6050 IMU
- **I2C接口**: 软件位操作实现
- **数据**: 3轴加速度计 + 3轴陀螺仪 + 温度
- **用途**: 车辆方向和运动感知

### 引脚配置（来自STM32CubeMX）

#### 电机PWM输出
- **前电机**: TIM1 (PE9-CH1, PE11-CH2, PE13-CH3, PE14-CH4)
- **后电机**: TIM8 (PC6-CH1, PC7-CH2, PC8-CH3, PC9-CH4)

#### 编码器输入
- **TIM2**: PA15-CH1, PB3-CH2 (前电机)
- **TIM3**: PA6-CH1, PA7-CH2 (左电机)
- **TIM4**: PD12-CH1, PD13-CH2 (右电机)
- **TIM5**: PA0-CH1, PA1-CH2 (后电机)

#### 数字输入
- **XIN1-XIN16**: 16个传感器数字输入引脚
- **E1-E3**: 边沿触发中断输入（PF9, PF11, PF12）
- **LED**: 状态LED输出（PF10）

#### 通信
- **UART1**: PA9 (TX), PA10 (RX)
- **I2C**: PB4 (SCL), PB5 (SDA) 用于MPU6050

## 开发指南

### 代码组织
- 硬件驱动放在 `Hadeware/` 目录
- 传感器特定实现放在专用文件夹（如 `mpu6050/`）
- 保持应用逻辑与硬件抽象分离

### 编程约定
- 使用STM32 HAL库函数
- 保持10ms调度器时序以获得一致的控制循环
- 所有电机命令应限制在PWM_MAX（999）和PWM_MIN（-999）之间

### 添加新功能
1. 在STM32CubeMX中配置外设（编辑`.ioc`文件）
2. 将驱动文件添加到相应目录
3. 将新文件包含在Keil项目组中
4. 如需新的周期性任务则更新调度器

### 调试
- 使用USART1进行调试输出
- 检查`.ioc`文件中的引脚分配与硬件连接的对应关系
- 验证定时器配置与电机和编码器规格匹配