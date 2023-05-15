#include "moto_driver.h"
#include "delay.h"

void water_gpio_init(void){

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_15;//水泵
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_14;//风扇
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	
}

void moto_gpio_init(void){
	
	//电机逻辑控制端口初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_12|GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_2;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	//电机定时器PWM控制端口初始化
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
}

//用于控制电机
void TIM4_init(void){

	//开启定时器时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	//时基初始化
	 TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	//设置重装载值ARR
	TIM_TimeBaseInitStruct.TIM_Period=999;
	//设置预分频值psc
	TIM_TimeBaseInitStruct.TIM_Prescaler=1439;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);
	
	
	TIM_OCInitTypeDef  TIM_OCInitStruct;	
	/*PWM模式1－ 在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为有效电平，否则为无效电平；
	在向下计数时，一旦TIMx_CNT>TIMx_CCR1时通道1为无效电平(OC1REF=0)，否则为有效电平(OC1REF=1)。 
	PWM模式2－ 在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为无效电平，否则为有效电平；在向下计数时，一旦TIMx_CNT>TIMx_CCR1时通道1为有效电平，否则为无效电平。
	*/
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM2;//定时器模式，TIM脉冲宽度调制模式2
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse=710;
	TIM_OC4Init(TIM4, &TIM_OCInitStruct);
	TIM_OC1Init(TIM4, &TIM_OCInitStruct);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_CtrlPWMOutputs(TIM4, ENABLE);
	//设置占空比CCR
	
	
	TIM_Cmd(TIM4, ENABLE);


}

void Turn_left(void){
	
	Turn_Stop();
	delay_ms(100);
	MOTO_IN1_Set;
	MOTO_IN2_ReSet;
	
	MOTO_IN3_ReSet;
	MOTO_IN4_Set;
	//TIM_SetCompare1(TIM4,800);
	//TIM_SetCompare4(TIM4,800);
	

}

void Turn_Right(void){
	Turn_Stop();
	delay_ms(100);
	MOTO_IN1_ReSet;
	MOTO_IN2_Set;
	
	MOTO_IN3_Set;
	MOTO_IN4_ReSet;
	
	//TIM_SetCompare1(TIM4,800);
	//TIM_SetCompare4(TIM4,800);
	

}
void Turn_Stop(void){
	MOTO_IN1_ReSet;
	MOTO_IN2_ReSet;
	MOTO_IN3_ReSet;
	MOTO_IN4_ReSet;
	

}


