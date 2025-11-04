#ifndef __UART_APP_H__
#define __UART_APP_H__

#include "bsp_sys.h"

int my_printf(UART_HandleTypeDef *huart, const char *format, ...);

#endif
