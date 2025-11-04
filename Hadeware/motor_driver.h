/**
 ******************************************************************************
 * @file    motor_driver.h
 * @brief    电机驱动库头文件
 * @author  Light
 * @date    2025-10-12
 ******************************************************************************
 * @attention
 * 
 
 ******************************************************************************
 */

#ifndef __MOTOR_DRIVER_H__
#define __MOTOR_DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"

/* Exported types ------------------------------------------------------------*/
#define PWM_MAX 999
#define PWM_MIN -999


/**
 * @brief 电机状态枚举
 */
void Set_PWM(int PWM_L,int PWM_R,int PWM_L_2,int PWM_R_2);
void Limit(int* PWMA,int* PWMB);
void set_motor_disable(void);

#ifdef __cplusplus
}
#endif

#endif /* __MOTOR_DRIVER_H__ */

