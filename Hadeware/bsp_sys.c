#include "bsp_sys.h"

uint8_t OLED_buf[20];													//OLED接收数据

int Theta_Err=0;

bool Ei_Key1=0,Ei_Key2=0,Ei_Key3=0;

uint8_t USART1_Res[20]={0};
uint8_t USART2_Tx[2]={0x57,0x57};
uint8_t USART2_RXbuff;
uint16_t Xun_Get[2]={0};

bool Wan_En=0;
int Wan_Time=0;

int F_Left_Encoder,F_Right_Encoder,B_Left_Encoder,B_Right_Encoder;   								//编码值
float F_Left_Speed_cm_s,F_Right_Speed_cm_s,B_Left_Speed_cm_s,B_Right_Speed_cm_s;		//速度
float F_Left_Speed_RPM,F_Right_Speed_RPM,B_Left_Speed_RPM,B_Right_Speed_RPM;				//转速

int Encoder_F_Left,Encoder_F_Right,Encoder_B_Left,Encoder_B_Right; //编码器数据（速度）

