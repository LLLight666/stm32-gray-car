#ifndef __BSP_SYS_H
#define __BSP_SYS_H	

/*头文件引用区域*/
#include "main.h"


#include "tim.h"
#include "usart.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "stdarg.h"



#include "scheduler.h"

#include "uart_app.h"

#include "led_app.h"

//电机驱动
#include "motor_driver.h"

#include "motor_app.h"
//编码器
#include "encoder_driver.h"
#include "encoder_app.h"

//pid
#include "pid.h"
#include "pid_app.h"
//gray
#include "gray_app.h"

//mpu6050

#include "soft_i2c.h"
#include "mpu6050.h"
#include "mpu6050_app.h"


/* Standard Includes */






void Stm32_Clock_Init(uint32_t PLL);				
extern float g_line_position_error;
extern bool is_started;//判断是否启动

/*Variable declaration table*/  



#endif
