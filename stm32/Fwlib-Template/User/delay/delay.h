#ifndef __DELAY_H
#define __DELAY_H

#endif
#include "stm32f10x.h"
static u8  fac_us=0;							//us延时倍乘数			   
static u16 fac_ms=0;							//ms延时倍乘数,在ucos下,代表每个节拍的ms数
void systick_init(void);
void Delay_us(u32 nus);
void delay_ms(u16 nms);