#include "led_app.h"



uint8_t ucLed = 0;

void led_control(uint8_t ucLed)
{
	uint8_t temp = 0x00;
	static uint8_t temp_old = 0xff;
	

	if(ucLed) temp |= (1<<0);
	
	
	if(temp_old != temp)
	{
		
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, (temp & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET); // LED 0
		
		
			temp_old = temp;

	}
}




void led_init(void)
{
		ucLed = 0;
	
		led_control(ucLed);
}

void led_task(void)
{
	
		if(is_started == true)
		{
				static uint32_t last_time = 0;
				uint32_t current_time = HAL_GetTick();
			
				if(current_time - last_time >= 500)
				{
					 ucLed ^= 1;
					
					last_time = current_time;
				}
				
				
					
				led_control(ucLed);
		}
}
