# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is an STM32F103ZET6-based four-wheel drive (4WD) off-road vehicle control system (越野车). The project uses STM32CubeMX configuration and Keil MDK-ARM for development.

## Build and Development Commands

### Building the Project
- **Primary IDE**: Keil MDK-ARM V5.32
- **Project File**: `MDK-ARM/ZET6_Dianji.uvprojx`
- **Build Output**: `MDK-ARM/ZET6_Dianji/ZET6_Dianji.hex` and `ZET6_Dianji.axf`

### Clean Build Artifacts
- Run `keilkill.bat` to clean temporary build files (*.o, *.d, *.crf, etc.)
- This batch file removes compilation artifacts and temporary files

### STM32CubeMX Configuration
- Configuration file: `ZET6_Dianji.ioc`
- Re-generate code using STM32CubeMX when modifying peripheral configurations

## Project Architecture

### Hardware Configuration
- **MCU**: STM32F103ZET6 (Cortex-M3, 512KB Flash, 64KB RAM)
- **System Clock**: 72MHz
- **External Crystal**: 8MHz HSE

### Peripheral Configuration
- **Timers**:
  - TIM1: PWM Generation for motors (4 channels)
  - TIM8: PWM Generation for motors (4 channels)
  - TIM2, TIM3, TIM4, TIM5: Encoder interfaces (quadrature encoder mode)
- **USART1**: UART communication (PA9-TX, PA10-RX)
- **GPIO**: Multiple digital inputs for sensors (XIN1-XIN16) and control signals

### Directory Structure
```
├── Core/                    # STM32 HAL generated code
│   ├── Inc/                 # Header files
│   └── Src/                 # Source files (main.c, hal_msp.c, etc.)
├── Hadeware/               # Hardware abstraction layer
│   ├── motor_driver.c/h    # Motor PWM control (4 motors)
│   ├── motor_app.c/h       # Motor application logic
│   ├── encoder_driver.c/h   # Encoder reading and speed calculation
│   ├── encoder_app.c/h     # Encoder application logic
│   ├── pid.c/h            # PID control algorithms
│   ├── pid_app.c/h        # PID application implementation
│   ├── scheduler.c/h      # Task scheduler
│   ├── uart_app.c/h      # UART communication
│   ├── gray_app.c/h      # Gray code sensor processing
│   └── led_app.c/h       # LED control
├── mpu6050/               # IMU sensor implementation
│   ├── mpu6050.c/h        # MPU6050 driver
│   ├── soft_i2c.c/h       # Software I2C implementation
│   └── mpu6050_app.c/h   # MPU6050 application logic
├── MDK-ARM/               # Keil project files
└── ZET6_Dianji.ioc       # STM32CubeMX configuration
```

### Key Components

#### Motor Control System
- **4 Motors**: Configured as 2 front motors + 2 rear motors (left/right pairs)
- **PWM Control**: 1000-step resolution (0-999) with direction control
- **Function**: `Set_PWM(PWM_L, PWM_R, PWM_L_2, PWM_R_2)` sets motor power

#### Encoder System
- **4 Encoders**: One per motor for closed-loop speed control
- **Specification**: 13-line encoder with 20:1 gearbox, 4x quadrature detection
- **PPR**: 1040 pulses per revolution (13 × 20 × 4)
- **Speed Calculation**: Updates every 10ms via scheduler

#### PID Controllers
- **Positional PID**: For precise position control
- **Incremental PID**: For smooth speed control
- **Structure**: Includes Kp, Ki, Kd parameters with output limiting

#### Task Scheduler
- **Period**: 10ms main loop cycle
- **Initialization**: `Scheduler_Init()` sets up all peripherals
- **Runtime**: `Scheduler_Run()` executes all periodic tasks

#### MPU6050 IMU
- **I2C Interface**: Software bit-banging implementation
- **Data**: 3-axis accelerometer + 3-axis gyroscope + temperature
- **Usage**: Vehicle orientation and motion sensing

### Pin Configuration (from STM32CubeMX)

#### Motor PWM Outputs
- **Front Motors**: TIM1 (PE9-CH1, PE11-CH2, PE13-CH3, PE14-CH4)
- **Rear Motors**: TIM8 (PC6-CH1, PC7-CH2, PC8-CH3, PC9-CH4)

#### Encoder Inputs
- **TIM2**: PA15-CH1, PB3-CH2 (Front motors)
- **TIM3**: PA6-CH1, PA7-CH2 (Left motors)
- **TIM4**: PD12-CH1, PD13-CH2 (Right motors)
- **TIM5**: PA0-CH1, PA1-CH2 (Rear motors)

#### Digital Inputs
- **XIN1-XIN16**: 16 digital input pins for sensors
- **E1-E3**: Edge-triggered interrupt inputs (PF9, PF11, PF12)
- **LED**: Status LED output (PF10)

#### Communication
- **UART1**: PA9 (TX), PA10 (RX)
- **I2C**: PB4 (SCL), PB5 (SDA) for MPU6050

## Development Guidelines

### Code Organization
- Hardware drivers go in `Hadeware/` directory
- Sensor-specific implementations in dedicated folders (e.g., `mpu6050/`)
- Keep application logic separate from hardware abstraction

### Programming Conventions
- Use STM32 HAL library functions
- Maintain 10ms scheduler timing for consistent control loops
- All motor commands should be limited to PWM_MAX (999) and PWM_MIN (-999)

### Adding New Features
1. Configure peripherals in STM32CubeMX (edit `.ioc` file)
2. Add driver files to appropriate directory
3. Include new files in Keil project groups
4. Update scheduler if new periodic tasks are needed

### Debugging
- Use USART1 for debugging output
- Check pin assignments in `.ioc` file against hardware connections
- Verify timer configurations match motor and encoder specifications