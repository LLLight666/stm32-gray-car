#ifndef __MPU6050_APP_H
#define __MPU6050_APP_H

#include "bsp_sys.h"

// 姿态角度结构
typedef struct {
    float pitch;        // 俯仰角 (°)
    float roll;         // 横滚角 (°)
    float yaw;          // 偏航角 (°) - 可选
} MPU6050_Attitude_t;

// 悬崖检测配置
typedef struct {
    float pitch_threshold;      // 俯仰角阈值 (°)
    float pitch_normal_range;   // 正常范围 (°)
    uint32_t confirm_time_ms;   // 确认时间 (ms)
} CliffDetection_Config_t;

// 全局变量
extern MPU6050_Attitude_t g_mpu6050_attitude;
extern bool g_is_climbing;      // 是否正在上坡（需要加动力）
extern bool g_is_on_obstacle;   // 是否在障碍物上（不计圈）

// 函数声明
int8_t MPU6050_App_Init(void);
void MPU6050_App_Task(void);
float MPU6050_App_GetPitch(void);
float MPU6050_App_GetRoll(void);

#endif
