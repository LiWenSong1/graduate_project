#ifndef __MOTO_DRIVER_H
#define __MOTO_DRIVER_H

#include "stm32f10x.h"
#define MOTO_IN1_ReSet GPIO_ResetBits(GPIOB, GPIO_Pin_10)
#define MOTO_IN1_Set GPIO_SetBits(GPIOB, GPIO_Pin_10);

#define MOTO_IN2_ReSet GPIO_ResetBits(GPIOC, GPIO_Pin_2)
#define MOTO_IN2_Set GPIO_SetBits(GPIOC, GPIO_Pin_2);

#define MOTO_IN3_ReSet GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define MOTO_IN3_Set GPIO_SetBits(GPIOB, GPIO_Pin_12);

#define MOTO_IN4_ReSet GPIO_ResetBits(GPIOB, GPIO_Pin_13)
#define MOTO_IN4_Set GPIO_SetBits(GPIOB, GPIO_Pin_13);
void water_gpio_init(void);
void moto_gpio_init(void);
void TIM4_init(void);

void Turn_Stop(void);
void Turn_Right(void);
void Turn_left(void);

#endif
