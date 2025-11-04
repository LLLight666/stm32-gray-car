#include "mpu6050_app.h"


// 全局变量
MPU6050_Attitude_t g_mpu6050_attitude = {0};
bool g_is_climbing = false;     // 是否正在上坡（需要加动力）
bool g_is_on_obstacle = false;  // 是否在障碍物上（不计圈）

// 私有变量
static float pitch_prev = 0.0f;
static float roll_prev = 0.0f;
static uint32_t last_update_time = 0;
static float gyro_y_current = 0.0f;  // 当前角速度
static float roll_filtered = 0.0f;  // 滤波后的Roll，用于检测
static uint32_t obstacle_start_time = 0;  // 进入障碍物的时间

// 互补滤波系数
#define COMPLEMENTARY_FILTER_ALPHA 0.98f

/**
 * @brief MPU6050应用层初始化
 */
int8_t MPU6050_App_Init(void)
{
    if(MPU6050_Init() != 0) {
        return -1;
    }
    
    // 等待MPU6050稳定
    HAL_Delay(100);
    
    g_mpu6050_attitude.pitch = 0.0f;
    g_mpu6050_attitude.roll = 0.0f;
    g_mpu6050_attitude.yaw = 0.0f;
    
    // 确保初始状态为false
    g_is_climbing = false;
    g_is_on_obstacle = false;
    
    last_update_time = HAL_GetTick();
    
    return 0;
}

/**
 * @brief 计算俯仰角2和横滚角
 */
static void MPU6050_App_CalculateAttitude(MPU6050_Data_t *data)
{
    uint32_t current_time = HAL_GetTick();
    float dt = (current_time - last_update_time) / 1000.0f;
    last_update_time = current_time;
    
    if(dt > 0.1f) dt = 0.01f;
    if(dt < 0.001f) dt = 0.01f;
    
    // 加速度计计算角度
    float pitch_accel = atan2f(data->accel_y, sqrtf(data->accel_x * data->accel_x + data->accel_z * data->accel_z)) * 180.0f / 3.14159f;
    float roll_accel = atan2f(-data->accel_x, sqrtf(data->accel_y * data->accel_y + data->accel_z * data->accel_z)) * 180.0f / 3.14159f;
    
    // 陀螺仪积分
    float pitch_gyro = pitch_prev - data->gyro_x * dt;
    float roll_gyro = roll_prev + data->gyro_y * dt;
    
    // 互补滤波
    g_mpu6050_attitude.pitch = COMPLEMENTARY_FILTER_ALPHA * pitch_gyro + (1.0f - COMPLEMENTARY_FILTER_ALPHA) * pitch_accel;
    g_mpu6050_attitude.roll = COMPLEMENTARY_FILTER_ALPHA * roll_gyro + (1.0f - COMPLEMENTARY_FILTER_ALPHA) * roll_accel;
    
    pitch_prev = g_mpu6050_attitude.pitch;
    roll_prev = g_mpu6050_attitude.roll;
    
    // 保存当前角速度供障碍物检测使用
    gyro_y_current = data->gyro_y;
}

/**
 * @brief 障碍物检测（改进版：结合角度和角速度）
 */
static void MPU6050_App_DetectObstacle(void)
{
    float roll = g_mpu6050_attitude.roll;
    float gyro_y = gyro_y_current;  // 当前角速度
    
    // 快速响应滤波（30%旧值 + 70%新值）- 平衡速度和稳定性
		roll_filtered = 0.30f * roll_filtered + 0.70f * roll;
    
    // 上坡检测：简单的滞回判断
    if(roll_filtered > 12.8f) {
        g_is_climbing = true;
    } else if(roll_filtered < 8.0f) {
        g_is_climbing = false;
    }
    // 调试输出（可以打开看看滤波效果）
    // my_printf(&huart1, "Roll: %.2f, Filtered: %.2f, Climb: %d\r\n", 
    //          roll, roll_filtered, g_is_climbing);

    // 障碍物检测：使用滤波后的roll
    bool angle_large = (roll_filtered > 15.0f) || (roll_filtered < 0.0f);  // 角度偏离大（包括下坡负角度）
    bool gyro_large = (fabsf(gyro_y) > 20.0f);          // 角速度大（飞跃特征，降低阈值更敏感）
    
		
//		my_printf(&huart1, "Roll: %.2f,  Gyro_Y: %.2f\r\n", 
//         g_mpu6050_attitude.roll,  gyro_y_current);

    if(angle_large || gyro_large) {
        // 进入障碍物状态
        if(!g_is_on_obstacle) {
            obstacle_start_time = HAL_GetTick();
        }
        g_is_on_obstacle = true;
    } else {
        // 离开障碍物状态，但需要延迟500ms
        // 避免飞跃过程中短暂经过0°时误判为平地
        if(g_is_on_obstacle) {
            if((HAL_GetTick() - obstacle_start_time) > 500) {
                g_is_on_obstacle = false;
            }
        }
    }
}

/**
 * @brief MPU6050应用层任务
 */
void MPU6050_App_Task(void)
{
    static uint32_t init_time = 0;
    MPU6050_Data_t data;
    
    // 初始化后延迟1秒再开始检测，等待MPU6050稳定
    if(init_time == 0) {
        init_time = HAL_GetTick();
    }
    if(HAL_GetTick() - init_time < 1000) {
        g_is_climbing = false;
        g_is_on_obstacle = false;
        return;
    }
    
    if(MPU6050_ReadData(&data) != 0) {
        return;
    }
    
    MPU6050_App_CalculateAttitude(&data);
    MPU6050_App_DetectObstacle();
    
    // 调试输出（建议先打开，确认角度正负号）
//    my_printf(&huart1, "Pitch: %.2f, Roll: %.2f, Climb: %d, Obstacle: %d\r\n", 
//             g_mpu6050_attitude.pitch, g_mpu6050_attitude.roll, g_is_climbing, g_is_on_obstacle);
}

/**
 * @brief 获取俯仰角
 */
float MPU6050_App_GetPitch(void)
{
    return g_mpu6050_attitude.pitch;
}

/**
 * @brief 获取横滚角
 */
float MPU6050_App_GetRoll(void)
{
    return g_mpu6050_attitude.roll;
}
