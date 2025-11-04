#include "pid_app.h"


/* PID 控制器实例 */
PID_T pid_speed_left;  // 左轮速度环
PID_T pid_speed_right; // 右轮速度环

PID_T pid_speed_left2;  // 左轮速度环
PID_T pid_speed_right2; // 右轮速度环

int basic_speed = 180; // 基础速度（直线极速，弯道会大幅降速）

// 圈道识别相关变量
TrackType_t current_track = TRACK_MIDDLE_OUTER; // 当前圈道类型，默认中外圈（快速）
bool is_started = false; // 启动标志：false=等待挥手启动，true=已启动运行

PID_T pid_line;        // 循迹环

extern bool g_is_line_lost;
extern bool g_is_sharp_left_turn;
extern bool g_is_sharp_right_turn;
extern float g_last_position_error;
extern float g_line_position_error; // 原有的误差变量

// 圈数计数相关
extern uint8_t g_lap_count;          // 当前圈数
extern bool g_is_finish_line;        // 是否检测到起止线

// 目标圈数设置（根据赛道类型动态调整）
#define count 8  // 初赛目标计数//决赛为11


// 动态目标圈数
uint8_t g_target_laps = count;  // 默认初赛圈数
bool g_track_type_detected = false;  // 是否已检测赛道类型

//// 增量式PID：P-稳定性，I-响应性，D-准确性

//左前轮
PidParams_t pid_params_left = {
    .kp = 9.0f,       // 减小Kp，抑制超调和震荡
    .ki = 0.5f,       // 减小Ki，避免积分过冲
    .kd = 38.0f,      // 增加Kd，增强阻尼抑制震荡
    .out_min = -999.0f,
    .out_max = 999.0f,
};

//左后轮的pid
PidParams_t pid_params_left2 = {
    .kp = 9.0f,       // 减小Kp，抑制超调和震荡
    .ki = 0.5f,       // 减小Ki，避免积分过冲
    .kd = 38.0f,      // 增加Kd，增强阻尼抑制震荡
    .out_min = -999.0f,
    .out_max = 999.0f,
};

//右前轮的pid
PidParams_t pid_params_right = {
	
    .kp = 9.0f,       // 减小Kp，抑制超调和震荡
    .ki = 0.5f,       // 减小Ki，避免积分过冲
    .kd = 38.0f,      // 增加Kd，增强阻尼抑制震荡
    .out_min = -999.0f,
    .out_max = 999.0f,

};
//右后轮的pid
PidParams_t pid_params_right2 = {
	
    .kp = 9.0f,       // 减小Kp，抑制超调和震荡
    .ki = 0.5f,       // 减小Ki，避免积分过冲
    .kd = 38.0f,      // 增加Kd，增强阻尼抑制震荡
    .out_min = -999.0f,
    .out_max = 999.0f,
};

// 循迹PID参数（针对380高速优化，强力抑制震荡）
PidParams_t pid_params_line = {
    .kp = 6.5f,        // 继续降低Kp，减少震荡（6.5→5.5）
    .ki = 0.0f,         // 不使用积分
    .kd = 65.0f,        // 大幅提高Kd，强力抑制震荡（50→65）
    .out_min = -400.0f, // 增大转向范围，应对急弯
    .out_max = 400.0f,
};


void PID_Init(void)
{
  pid_init(&pid_speed_left,
           pid_params_left.kp, pid_params_left.ki, pid_params_left.kd,
           0.0f, pid_params_left.out_max);
  
  pid_init(&pid_speed_right,
           pid_params_right.kp, pid_params_right.ki, pid_params_right.kd,
           0.0f, pid_params_right.out_max);
	
	pid_init(&pid_speed_left2,
           pid_params_left2.kp, pid_params_left2.ki, pid_params_left2.kd,
           0.0f, pid_params_left2.out_max);
	
	pid_init(&pid_speed_right2,
           pid_params_right2.kp, pid_params_right2.ki, pid_params_right2.kd,
           0.0f, pid_params_right2.out_max);
	pid_init(&pid_line,
           pid_params_line.kp, pid_params_line.ki, pid_params_line.kd,
           0.0f, pid_params_line.out_max);
  
  pid_set_target(&pid_speed_left2, basic_speed);
	pid_set_target(&pid_speed_right2, basic_speed);
	pid_set_target(&pid_speed_right, basic_speed);
	pid_set_target(&pid_speed_left,basic_speed);
	
	pid_set_target(&pid_line, 0);
}


/**
 * @brief 检测挥手启动并设置对应速度
 * @note 请根据实际硬件连接修改 GPIO 引脚
 * 
 * 逻辑：
 * - 传感器1挥手 -> 启动内圈模式，
 * - 传感器2挥手 -> 启动中外圈模式，
 * - 启动后保持选定的速度运行
 */
void Check_Start_And_Set_Speed(void)
{
    // 如果还没启动，检测挥手信号
    if (!is_started) {
        // TODO: 根据你的红外传感器引脚读取状态
        // 示例代码（请修改为实际引脚）：
        
        uint8_t inner_start_sensor = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_9);   // 内圈启动传感器
        uint8_t outer_start_sensor = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_11);   // 中外圈启动传感器
        
        // 检测挥手信号
        if (inner_start_sensor == GPIO_PIN_RESET)
				{  // 传感器1检测到挥手
             current_track = TRACK_INNER;
             basic_speed = SPEED_INNER;  
             is_started = true;
             
         } 
         else if (outer_start_sensor == GPIO_PIN_RESET) {  // 传感器2检测到挥手
             current_track = TRACK_MIDDLE_OUTER;
             basic_speed = SPEED_MIDDLE_OUTER;  
             is_started = true;

         }
        

    }
    else {
        // 已启动，根据当前圈道保持速度
        if (current_track == TRACK_INNER) {
            basic_speed = SPEED_INNER;
        } else {
            basic_speed = SPEED_MIDDLE_OUTER;
        }
    }
}

void Line_PID_control(void) // 循迹环控制
{
	 int left_speed;
	 int right_speed;
	 
	 // 直角弯处理（仅内圈有直角弯）
	 if (current_track == TRACK_INNER && g_is_sharp_left_turn)
   {
        // --- 内圈左直角弯：快速转向 ---
        left_speed = SHARP_TURN_INNER_SPEED_INNER;  // 内侧轮强力反转
        right_speed = SHARP_TURN_SPEED_INNER;       // 外侧轮快速
   }
	 else if (current_track == TRACK_INNER && g_is_sharp_right_turn)
   {
        // --- 内圈右直角弯：快速转向 ---
        left_speed = SHARP_TURN_SPEED_INNER;        // 外侧轮快速
        right_speed = SHARP_TURN_INNER_SPEED_INNER; // 内侧轮强力反转
   }


	 else if (g_is_line_lost)
   {
        // --- 状态2: 丢线 ---
        // 根据最后一次有效误差来判断如何找回线
        // 如果丢线前是在向左转，就继续向左转来找线
      if (g_last_position_error < -2.0f) // -2.0f 是阈值，需要调试
      {
            left_speed = -60; // 继续左转找线
            right_speed = 80;
      }
        // 如果丢线前是在向右转，就继续向右转
      else if (g_last_position_error > 2.0f)
      {
            left_speed = 80; // 继续右转找线
            right_speed = -60;
      }
      else // 如果在直线上丢线，那就是虚线
      {
            left_speed = basic_speed;
            right_speed = basic_speed;
      }
		}
			
	 else
   {
        // --- 状态3: 正常循迹 
        int line_pid_output = 0;
        float abs_error = fabsf(g_line_position_error);
        int current_speed = basic_speed;

        // 根据误差动态调速（针对350高速优化，弯道适度减速保证不出线）
        if(abs_error > 4.5f) {
            current_speed = basic_speed * 0.35f;  // 急弯必须减速（350→122）
        } else if(abs_error > 3.5f) {
            current_speed = basic_speed * 0.50f;  // 大弯减速（350→175）
        } else if(abs_error > 2.5f) {
            current_speed = basic_speed * 0.65f;  // 中弯减速（350→227）
        } else if(abs_error > 2.0f) {
            current_speed = basic_speed * 0.78f;  // 小弯轻微减速（350→273）
        } else if(abs_error > 1.2f) {
            current_speed = basic_speed * 0.90f;  // 微小偏差（350→315）
        } 
				else {
             current_speed = basic_speed;  // 误差<1.2保持满速350
        }
        
        
        // PID计算（增大死区，直线时减少修正，避免震荡）
        if(abs_error < 0.8f) {  // 增大死区到1.3，直线时不修正
            line_pid_output = 0;
        } else {
            line_pid_output = pid_calculate_positional(&pid_line, -g_line_position_error);
            line_pid_output = pid_constrain(line_pid_output, pid_params_line.out_min, pid_params_line.out_max);
        }

        left_speed = current_speed + line_pid_output;
        right_speed = current_speed - line_pid_output;

        // 最低速度限制（高速时保持足够的内轮速度）
        int min_speed = current_speed * 0.20f;  // 20%最低速度
        if(min_speed < 25) min_speed = 25;      // 最低25
        if(min_speed > 60) min_speed = 60;      // 最高70
        if(left_speed < min_speed && line_pid_output > 0) left_speed = min_speed;
        if(right_speed < min_speed && line_pid_output < 0) right_speed = min_speed;
    }
				
				// 上坡动力补偿：直接设置高速，不依赖当前速度
				if(g_is_climbing == true)
				{
					
						
							// 直接设置固定高速，确保有足够动力
							left_speed = 500;   // 固定高速
							right_speed = 500;
						
				}
		
				
				
				// 将差值作用在速度环的目标量上
				// 误差为负（左侧传感器亮）→ PID输出为负 → 左轮慢，右轮快 → 左转
				// 误差为正（右侧传感器亮）→ PID输出为正 → 左轮快，右轮慢 → 右转
				pid_set_target(&pid_speed_left, left_speed);
				pid_set_target(&pid_speed_right, right_speed);
				
				pid_set_target(&pid_speed_left2, left_speed);
				pid_set_target(&pid_speed_right2, right_speed);
	
	
}


// 低通滤波器系数 (Low-pass filter coefficient 'alpha')
// alpha 越小, 滤波效果越强, 但延迟越大。建议从 0.1 到 0.5 之间开始尝试。
#define SPEED_FILTER_ALPHA_LEFT  0.15f 
#define SPEED_FILTER_ALPHA_RIGHT 0.15f 

#define SPEED_FILTER_ALPHA_LEFT2  0.15f 
#define SPEED_FILTER_ALPHA_RIGHT2 0.15f 


// 用于存储滤波后速度的变量
static float filtered_speed_left = 0.0f;
static float filtered_speed_right = 0.0f;

static float filtered_speed_left2 = 0.0f;
static float filtered_speed_right2 = 0.0f;

bool pid_running = true; // PID 控制使能开关

// 外部变量：上坡检测
extern bool g_is_climbing;

void PID_Task(void)
{
	
	
    if(pid_running == false) return;
		
		// 检测挥手启动并设置速度
		Check_Start_And_Set_Speed();
		
		// 如果还没启动，电机不运行
		if (!is_started) {
				Set_PWM(0, 0, 0, 0);  // 所有电机停止
				return;
		}
		

	
		// 达到目标圈数后停止
		if (g_lap_count >= g_target_laps)
		{
				Set_PWM(0,0,0,0);
				return;
		}
		
		
		
		Line_PID_control();
	
		
    float output_left, output_right;
    float output_left2, output_right2;
    // filtered = alpha * raw + (1 - alpha) * previous_filtered
    filtered_speed_left = SPEED_FILTER_ALPHA_LEFT * left_encoder.speed_cm_s + (1.0f - SPEED_FILTER_ALPHA_LEFT) * filtered_speed_left;
    filtered_speed_right = SPEED_FILTER_ALPHA_RIGHT * right_encoder.speed_cm_s + (1.0f - SPEED_FILTER_ALPHA_RIGHT) * filtered_speed_right;
		
	
		filtered_speed_left2 = SPEED_FILTER_ALPHA_LEFT2 * left2_encoder.speed_cm_s + (1.0f - SPEED_FILTER_ALPHA_LEFT2) * filtered_speed_left2;
		filtered_speed_right2 = SPEED_FILTER_ALPHA_RIGHT2 * right2_encoder.speed_cm_s + (1.0f - SPEED_FILTER_ALPHA_RIGHT2) * filtered_speed_right2;
	

    output_left = pid_calculate_positional(&pid_speed_left, filtered_speed_left);
    output_right = pid_calculate_positional(&pid_speed_right, filtered_speed_right);
		
		output_left2 = pid_calculate_positional(&pid_speed_left2, filtered_speed_left2);
    output_right2 = pid_calculate_positional(&pid_speed_right2, filtered_speed_right2);
	

	
    output_left = pid_constrain(output_left, pid_params_left.out_min, pid_params_left.out_max);
    output_right = pid_constrain(output_right, pid_params_right.out_min, pid_params_right.out_max);
		
    output_left2 = pid_constrain(output_left2, pid_params_left2.out_min, pid_params_left2.out_max);
    output_right2 = pid_constrain(output_right2, pid_params_right2.out_min, pid_params_right2.out_max);		
    // 设置电机速度
 
		
    Set_PWM(output_left,output_right,output_left2,output_right2);//左前 右前 左后 右后
	
    //my_printf(&huart1,"{right_filtered}%.2f,%.2f\r\n", pid_speed_left2.target, filtered_speed_left2);
		//my_printf(&huart1,"{left_filtered}%.2f,%.2f\r\n", pid_speed_right2.target, filtered_speed_right2);
		
		
}



