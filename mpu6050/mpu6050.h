#ifndef __MPU6050_H
#define __MPU6050_H

#include "bsp_sys.h"

// MPU6050 I2C地址
#define MPU6050_ADDR        0xD0    // AD0接GND时的地址

// MPU6050寄存器地址
#define MPU6050_WHO_AM_I    0x75    // 设备ID寄存器，应该返回0x68
#define MPU6050_PWR_MGMT_1  0x6B    // 电源管理1
#define MPU6050_SMPLRT_DIV  0x19    // 采样率分频器
#define MPU6050_CONFIG      0x1A    // 配置寄存器
#define MPU6050_GYRO_CONFIG 0x1B    // 陀螺仪配置
#define MPU6050_ACCEL_CONFIG 0x1C   // 加速度计配置
#define MPU6050_ACCEL_XOUT_H 0x3B   // 加速度X高字节
#define MPU6050_TEMP_OUT_H  0x41    // 温度高字节
#define MPU6050_GYRO_XOUT_H 0x43    // 陀螺仪X高字节

// MPU6050原始数据结构
typedef struct {
    int16_t accel_x;    // 加速度X轴原始值
    int16_t accel_y;    // 加速度Y轴原始值
    int16_t accel_z;    // 加速度Z轴原始值
    int16_t gyro_x;     // 陀螺仪X轴原始值
    int16_t gyro_y;     // 陀螺仪Y轴原始值
    int16_t gyro_z;     // 陀螺仪Z轴原始值
    int16_t temp;       // 温度原始值
} MPU6050_RawData_t;

// MPU6050物理数据结构
typedef struct {
    float accel_x;      // 加速度X轴 (m/s²)
    float accel_y;      // 加速度Y轴 (m/s²)
    float accel_z;      // 加速度Z轴 (m/s²)
    float gyro_x;       // 陀螺仪X轴 (°/s)
    float gyro_y;       // 陀螺仪Y轴 (°/s)
    float gyro_z;       // 陀螺仪Z轴 (°/s)
    float temperature;  // 温度 (°C)
} MPU6050_Data_t;

// 函数声明
int8_t MPU6050_Init(void);
int8_t MPU6050_TestConnection(void);
int8_t MPU6050_ReadRawData(MPU6050_RawData_t *raw_data);
int8_t MPU6050_ReadData(MPU6050_Data_t *data);

#endif
