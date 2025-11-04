#ifndef __PID_APP_H__
#define __PID_APP_H__

#include "bsp_sys.h"

// PID参数结构体
typedef struct
{
    float kp;          // 比例系数
    float ki;          // 积分系数
    float kd;          // 微分系数
    float out_min;     // 输出最小值
    float out_max;     // 输出最大值
} PidParams_t;

// 圈道类型枚举
typedef enum {
    TRACK_INNER = 0,        // 内圈（慢速）
    TRACK_MIDDLE_OUTER = 1  // 中圈+外圈（快速）
} TrackType_t;

// 不同圈道的速度配置
#define SPEED_INNER        150   // 内圈速度（保持不变）
#define SPEED_MIDDLE_OUTER 320   // 中圈+外圈速度（降低到320，避免震荡）

// 内圈直角弯速度配置（仅内圈有直角弯，快速转向但不出线）
#define SHARP_TURN_SPEED_INNER        65    // 内圈直角弯外侧轮速度（降低速度防止出线）
#define SHARP_TURN_INNER_SPEED_INNER  -90   // 内圈直角弯内侧轮速度（适度反转）



void PID_Init(void);
void PID_Task(void);
void Check_Start_And_Set_Speed(void);  // 检测挥手启动并设置速度


void pid_target(uint8_t left,uint8_t left2,uint8_t right,uint8_t right2);
#endif
