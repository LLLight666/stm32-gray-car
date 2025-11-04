#include "mpu6050.h"


/**
 * @brief MPU6050写寄存器
 */
static int8_t MPU6050_WriteReg(uint8_t reg_addr, uint8_t data)
{
    SOFT_I2C_Start();
    SOFT_I2C_SendByte(MPU6050_ADDR);
    if(SOFT_I2C_ReceiveAck() != 0) {
        SOFT_I2C_Stop();
        return -1;
    }
    
    SOFT_I2C_SendByte(reg_addr);
    if(SOFT_I2C_ReceiveAck() != 0) {
        SOFT_I2C_Stop();
        return -1;
    }
    
    SOFT_I2C_SendByte(data);
    if(SOFT_I2C_ReceiveAck() != 0) {
        SOFT_I2C_Stop();
        return -1;
    }
    
    SOFT_I2C_Stop();
    return 0;
}

/**
 * @brief MPU6050读寄存器
 */
static int8_t MPU6050_ReadReg(uint8_t reg_addr, uint8_t *data)
{
    SOFT_I2C_Start();
    SOFT_I2C_SendByte(MPU6050_ADDR);
    if(SOFT_I2C_ReceiveAck() != 0) {
        SOFT_I2C_Stop();
        return -1;
    }
    
    SOFT_I2C_SendByte(reg_addr);
    if(SOFT_I2C_ReceiveAck() != 0) {
        SOFT_I2C_Stop();
        return -1;
    }
    
    SOFT_I2C_Start();  // 重新起始
    SOFT_I2C_SendByte(MPU6050_ADDR | 0x01);  // 读模式
    if(SOFT_I2C_ReceiveAck() != 0) {
        SOFT_I2C_Stop();
        return -1;
    }
    
    *data = SOFT_I2C_ReceiveByte();
    SOFT_I2C_SendAck(1);  // NACK
    SOFT_I2C_Stop();
    
    return 0;
}

/**
 * @brief MPU6050连续读取多个寄存器
 */
static int8_t MPU6050_ReadRegs(uint8_t reg_addr, uint8_t *data, uint8_t len)
{
    SOFT_I2C_Start();
    SOFT_I2C_SendByte(MPU6050_ADDR);
    if(SOFT_I2C_ReceiveAck() != 0) {
        SOFT_I2C_Stop();
        return -1;
    }
    
    SOFT_I2C_SendByte(reg_addr);
    if(SOFT_I2C_ReceiveAck() != 0) {
        SOFT_I2C_Stop();
        return -1;
    }
    
    SOFT_I2C_Start();  // 重新起始
    SOFT_I2C_SendByte(MPU6050_ADDR | 0x01);  // 读模式
    if(SOFT_I2C_ReceiveAck() != 0) {
        SOFT_I2C_Stop();
        return -1;
    }
    
    for(uint8_t i = 0; i < len; i++)
    {
        data[i] = SOFT_I2C_ReceiveByte();
        
        if(i < len - 1)
            SOFT_I2C_SendAck(0);  // ACK，继续读取
        else
            SOFT_I2C_SendAck(1);  // NACK，最后一个字节
    }
    
    SOFT_I2C_Stop();
    return 0;
}

/**
 * @brief MPU6050初始化
 * @return 0=成功, -1=失败
 */
int8_t MPU6050_Init(void)
{
    // 检查连接
    if(MPU6050_TestConnection() != 0) {
        return -1;
    }
    
    // 唤醒MPU6050（PWR_MGMT_1寄存器写0）
    if(MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x00) != 0) {
        return -1;
    }
    HAL_Delay(10);
    
    // 配置采样率分频器（1kHz / (1+9) = 100Hz）
    if(MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09) != 0) {
        return -1;
    }
    
    // 配置低通滤波器
    if(MPU6050_WriteReg(MPU6050_CONFIG, 0x06) != 0) {
        return -1;
    }
    
    // 配置陀螺仪量程（±250°/s）
    if(MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x00) != 0) {
        return -1;
    }
    
    // 配置加速度计量程（±2g）
    if(MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x00) != 0) {
        return -1;
    }
    
    return 0;
}

/**
 * @brief 测试MPU6050连接
 * @return 0=连接正常, -1=连接失败
 */
int8_t MPU6050_TestConnection(void)
{
    uint8_t who_am_i;
    
    if(MPU6050_ReadReg(MPU6050_WHO_AM_I, &who_am_i) != 0) {
        return -1;
    }
    
    if(who_am_i != 0x68) {
        return -1;
    }
    
    return 0;
}

/**
 * @brief 读取MPU6050原始数据
 * @param raw_data 存储原始数据的结构体指针
 * @return 0=成功, -1=失败
 */
int8_t MPU6050_ReadRawData(MPU6050_RawData_t *raw_data)
{
    uint8_t data[14];
    
    // 从0x3B开始连续读取14字节
    if(MPU6050_ReadRegs(MPU6050_ACCEL_XOUT_H, data, 14) != 0) {
        return -1;
    }
    
    // 解析数据（大端格式）
    raw_data->accel_x = (int16_t)((data[0] << 8) | data[1]);
    raw_data->accel_y = (int16_t)((data[2] << 8) | data[3]);
    raw_data->accel_z = (int16_t)((data[4] << 8) | data[5]);
    raw_data->temp = (int16_t)((data[6] << 8) | data[7]);
    raw_data->gyro_x = (int16_t)((data[8] << 8) | data[9]);
    raw_data->gyro_y = (int16_t)((data[10] << 8) | data[11]);
    raw_data->gyro_z = (int16_t)((data[12] << 8) | data[13]);
    
    return 0;
}

/**
 * @brief 读取MPU6050物理数据
 * @param data 存储物理数据的结构体指针
 * @return 0=成功, -1=失败
 */
int8_t MPU6050_ReadData(MPU6050_Data_t *data)
{
    MPU6050_RawData_t raw_data;
    
    if(MPU6050_ReadRawData(&raw_data) != 0) {
        return -1;
    }
    
    // 转换为物理单位
    // 加速度：±2g量程，灵敏度16384 LSB/g
    data->accel_x = (float)raw_data.accel_x / 16384.0f * 9.8f;  // m/s²
    data->accel_y = (float)raw_data.accel_y / 16384.0f * 9.8f;
    data->accel_z = (float)raw_data.accel_z / 16384.0f * 9.8f;
    
    // 陀螺仪：±250°/s量程，灵敏度131 LSB/(°/s)
    data->gyro_x = (float)raw_data.gyro_x / 131.0f;  // °/s
    data->gyro_y = (float)raw_data.gyro_y / 131.0f;
    data->gyro_z = (float)raw_data.gyro_z / 131.0f;
    
    // 温度：Temperature = (TEMP_OUT / 340) + 36.53
    data->temperature = (float)raw_data.temp / 340.0f + 36.53f;  // °C
    
    return 0;
}
