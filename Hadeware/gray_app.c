#include "gray_app.h"



bool g_is_line_lost = false;         // 标志位：是否丢线
bool g_is_sharp_left_turn = false;   // 标志位：是否检测到左直角弯
bool g_is_sharp_right_turn = false;  // 标志位：是否检测到右直角弯
float g_last_position_error = 0.0f;  // 记录上一次有效的循迹误差
float g_line_position_error;

// 圈数计数相关变量
uint8_t g_lap_count = 0;             // 当前圈数

bool g_finish_line_passed = false;   // 防止重复计数标志

/**
 * @brief 检测起止线
 * @return true=检测到起止线，false=未检测到
 */
bool Check_Finish_Line_Sensor(void)
{
    // 读取起止线传感器（根据你的实际引脚修改）
    uint8_t sensor = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_12);   // 左侧传感器

    
    // 传感器都检测到黑线（低电平）才判定为起止线
    if (sensor == GPIO_PIN_RESET)
    {
        return true;
    }
    
    return false;
}

//16路数字量的数据
uint8_t digtal(uint8_t channel)
{
    uint8_t value = 0;
	
    switch(channel)
    {
        case 1:  // ͨ��1��
            value = (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_0) == GPIO_PIN_SET) ? 1 : 0;
            break;
        case 2:  // ͨ��2��
            value = (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_1) == GPIO_PIN_SET) ? 1 : 0;
            break;
        case 3:  // ͨ��3��
            value = (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2) == GPIO_PIN_SET) ? 1 : 0;
            break;
        case 4:  // ͨ��4��
            value = (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3) == GPIO_PIN_SET) ? 1 : 0;
            break;
        case 5:  // ͨ��5��
            value = (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4) == GPIO_PIN_SET) ? 1 : 0;
            break;
        case 6:  // ͨ��6��
            value = (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5) == GPIO_PIN_SET) ? 1 : 0;
            break;
        case 7:  // ͨ��7��
            value = (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6) == GPIO_PIN_SET) ? 1 : 0;
            break;
        case 8:  // ͨ��8��
            value = (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_0) == GPIO_PIN_SET) ? 1 : 0;
            break;
        case 9:  // ͨ��9��
            value = (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_1) == GPIO_PIN_SET) ? 1 : 0;
            break;
        case 10: // ͨ��10��
            value = (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_2) == GPIO_PIN_SET) ? 1 : 0;
            break;
        case 11: // ͨ��11��
            value = (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_3) == GPIO_PIN_SET) ? 1 : 0;
            break;
        case 12: // ͨ��12��
            value = (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_4) == GPIO_PIN_SET) ? 1 : 0;
            break;
        case 13: // ͨ��13��
            value = (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_5) == GPIO_PIN_SET) ? 1 : 0;
            break;
        case 14: // ͨ��14��
            value = (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6) == GPIO_PIN_SET) ? 1 : 0;
            break;
        case 15: // ͨ��15��
            value = (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_7) == GPIO_PIN_SET) ? 1 : 0;
            break;
        case 16: // ͨ��16��
            value = (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_8) == GPIO_PIN_SET) ? 1 : 0;
            break;
        default:
            value = 0; 
            break;
    }
    return value;
}









float gray_weights[16] = {
    -7.5f, -6.5f, -5.5f, -4.5f,  // ������1-4���������ݶ�
    -3.5f, -2.5f, -1.5f, -0.5f,  // ������5-8  
     0.5f,  1.5f,  2.5f,  3.5f,  // ������9-12
     4.5f,  5.5f,  6.5f,  7.5f   // ������13-16���������ݶ�
};




//void gray_init(void)
//{
//	
//}




void gray_task(void)
{
	
	  // 调试：打印所有传感器状态（需要时取消注释）
	  //my_printf(&huart1, "Digtal %d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d\r\n",digtal(1),digtal(2),digtal(3),digtal(4),digtal(5),digtal(6),digtal(7),digtal(8),digtal(9),digtal(10),digtal(11),digtal(12),digtal(13),digtal(14),digtal(15),digtal(16));
		
		float weighted_sum = 0;
    uint8_t white_line_count = 0;
		uint8_t left_white_count = 0;  // 左侧传感器计数
    uint8_t right_white_count = 0; // 右侧传感器计数
    
    for(uint8_t i = 1; i < 17; i++)
    {
      if((digtal(i) & 0x01) == 0 )  // 检测到白线（低电平）
      {
        weighted_sum += gray_weights[i-1];
        white_line_count++;
        
        if (i <= 8)
        {
            left_white_count++;
        } 
        else
        {
            right_white_count++;
        }
      }
    }
    
    // 2. 检测起止线（简单直接：扫到就计）
    if (Check_Finish_Line_Sensor())
    {
        // 检测到起止线，且未通过标志（防止在线上重复计数）
        if (!g_finish_line_passed && !g_is_on_obstacle)
        {
            g_lap_count++;
            g_finish_line_passed = true;
        }
    }
    else
    {
        // 离开起止线区域，重置标志
        g_finish_line_passed = false;
    }
    
    // 3. 根据传感器状态更新全局标志位
    if(white_line_count == 0)
    {
        // 所有传感器都没检测到线，判断为丢线
        g_is_line_lost = true;
        // g_line_position_error 保持不变，使用 g_last_position_error 来恢复
    }
    else
    {
        // 只要有传感器检测到线，就不是丢线状态
        g_is_line_lost = false;
        g_line_position_error = weighted_sum / (float)white_line_count;
        g_last_position_error = g_line_position_error; // 记录最后一次有效误差

        // 判断是否为直角弯（降低阈值，更早检测）
        // 条件：一侧传感器大部分被触发，另一侧几乎没有。
        #define SHARP_TURN_THRESHOLD 5  // 降低到4，更早触发直角弯
        if (left_white_count >= SHARP_TURN_THRESHOLD && right_white_count <= 2)
        {
            g_is_sharp_left_turn = true;
        }
        else
        {
            g_is_sharp_left_turn = false;
        }

        if (right_white_count >= SHARP_TURN_THRESHOLD && left_white_count <= 2)
        {
            g_is_sharp_right_turn = true;
        }
        else
        {
            g_is_sharp_right_turn = false;
        }
    }
		
		

}

