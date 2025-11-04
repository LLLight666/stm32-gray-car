/**
 ******************************************************************************
 * @file    motor_driver.c
 * @brief   电机驱动库源文件
 * @author  Light
 * @date    2025-10-12
 ******************************************************************************
 * @attention
 *

 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "motor_driver.h"



void Set_PWM(int PWM_L,int PWM_R,int PWM_L_2,int PWM_R_2)
{
	// 先限幅，再设置 PWM
	Limit(&PWM_L,&PWM_R);
	Limit(&PWM_L_2,&PWM_R_2);
	
	// 左前轮
	if(PWM_L>0)
	{
		 __HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,PWM_L);
		 __HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_2,0);
	}
	else
	{
		 __HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,0);
		 __HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_2,-PWM_L);
	}
	
	// 右前轮
	if(PWM_R>0)
	{
		 __HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_3,0);
		 __HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_4,PWM_R);
	}
	else
	{
		 __HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_3,-PWM_R);
		 __HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_4,0);
	}
	
	// 左后轮
	if(PWM_L_2>0)
	{
		 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,PWM_L_2);
		 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,0);
	}
	else
	{
		 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,0);
		 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,-PWM_L_2);
	}
	
	// 右后轮
	if(PWM_R_2>0)
	{
		 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_3,0);
		 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_4,PWM_R_2);
	}
	else
	{
		 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_3,-PWM_R_2);
		 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_4,0);
	}
}

/**********************
PWM限幅
**********************/
void Limit(int* PWMA,int* PWMB)
{
	if(* PWMA>PWM_MAX)  *PWMA=PWM_MAX;
	if(* PWMA<PWM_MIN)  *PWMA=PWM_MIN;
	if(* PWMB>PWM_MAX)  *PWMB=PWM_MAX;
	if(* PWMB<PWM_MIN)  *PWMB=PWM_MIN;
}

/**
  * @brief  电机使能函数
  * @param  无
	*	@note 	无 
  * @retval 无
  */

uint8_t   is_motor_en  = 0;   //电机使能标志位
uint8_t   is_motor2_en = 0;   //电机1使能标志位

void set_motor_enable(void)
{
	
}

/**
  * @brief  电机失能函数
  * @param  无
	*	@note 	无 
  * @retval 无
  */
void set_motor_disable(void)
{
	__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,0);
	__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_2,0);
	__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_3,0);
	__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_4,0);
	
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,0);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,0);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_3,0);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_4,0);
}





