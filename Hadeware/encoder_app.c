#include "encoder_app.h"

// 左右编码器电机
Encoder left_encoder;
Encoder right_encoder;

Encoder left2_encoder;
Encoder right2_encoder;


/**
 * @brief 初始化编码器应用
 */
void Encoder_Init(void)
{
  Encoder_Driver_Init(&left_encoder, &htim2, 1);//左前轮
  Encoder_Driver_Init(&right_encoder, &htim3, 0);//右前轮
	
  Encoder_Driver_Init(&left2_encoder, &htim4, 1);//左后轮
  Encoder_Driver_Init(&right2_encoder, &htim5, 0);//右后轮	
	
	
}

/**
 * @brief 编码器应用运行任务 (应由调度器周期性调用)
 */
void Encoder_Task(void)
{

	
  Encoder_Driver_Update(&left_encoder);
  Encoder_Driver_Update(&right_encoder);
	Encoder_Driver_Update(&left2_encoder);
  Encoder_Driver_Update(&right2_encoder);
	
	//my_printf(&huart1,"Left:%.2f Right:%.2f\r\n",left_encoder.speed_cm_s,right_encoder.speed_cm_s);
	
	
}
