#ifndef __USART1__H
#define __USART1__H

#endif
#include "stm32f10x.h"

//串口GPIO引脚初始化，PA9,PA10
void usart1_init(void);
void usart2_init(void);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
void food_add(int food_value,int food_thresholdmin,int food_thresholdmax);