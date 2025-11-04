#include "scheduler.h"

//extern Motor_t right_motor;
//extern Motor_t left_motor; 

// 任务结构体
typedef struct {
  void (*task_func)(void);  // 任务函数指针
  uint32_t rate_ms;         // 执行周期（毫秒）
  uint32_t last_run;        // 上次执行时间（初始化为 0，每次运行时刷新）
} scheduler_task_t;

// 全局变量，用于存储任务数量
uint8_t task_num;

/**
 * @brief 用户初始化函数
 * 在HAL和硬件初始化后调用
 */
void System_Init()
{
	// 初始化电机
	Motor_Init();	
	// 初始化编码器
	Encoder_Init();
	
  led_init();
 
  
  // 初始化MPU6050
  MPU6050_App_Init();
  
  PID_Init();
}

// 静态任务数组，每个任务包含：任务函数、执行周期（毫秒）、上次运行时间（毫秒）
static scheduler_task_t scheduler_task[] =
{
  
  {Encoder_Task, 10, 0},
  {MPU6050_App_Task, 10, 0},  // MPU6050任务，10ms周期		
	{gray_task, 10, 0},
  {PID_Task, 10, 0},
  {led_task, 1, 0},
	
};


/**
 * @brief 调度器初始化函数
 * 计算任务数组元素个数，并将结果存储在 task_num 中
 */
void Scheduler_Init(void)
{
  System_Init();
  // 计算任务数组元素个数，并将结果存储在 task_num 中
  task_num = sizeof(scheduler_task) / sizeof(scheduler_task_t); // 数组大小 / 数组元素大小 = 数组元素个数
}

/**
 * @brief 调度器运行函数
 * 遍历任务数组，检查是否有任务需要执行。如果当前时间已经达到或超过执行周期，则执行该任务并更新上次运行时间
 */
void Scheduler_Run(void)
{
  // 遍历任务数组中的所有任务
  for (uint8_t i = 0; i < task_num; i++)
  {
    // 获取当前的系统时间（毫秒）
    uint32_t now_time = HAL_GetTick();

    // 检查当前时间是否达到任务执行时间
    if (now_time >= scheduler_task[i].rate_ms + scheduler_task[i].last_run)
    {
      // 更新任务上次运行时间为当前时间
      scheduler_task[i].last_run = now_time;

      // 执行任务函数
      scheduler_task[i].task_func();
    }
  }
}
