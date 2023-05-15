#include "moto_driver.h"
#include "delay.h"

void water_gpio_init(void){

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_15;//ˮ��
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_14;//����
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	
}

void moto_gpio_init(void){
	
	//����߼����ƶ˿ڳ�ʼ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_12|GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_2;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	//�����ʱ��PWM���ƶ˿ڳ�ʼ��
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
}

//���ڿ��Ƶ��
void TIM4_init(void){

	//������ʱ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	//ʱ����ʼ��
	 TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	//������װ��ֵARR
	TIM_TimeBaseInitStruct.TIM_Period=999;
	//����Ԥ��Ƶֵpsc
	TIM_TimeBaseInitStruct.TIM_Prescaler=1439;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);
	
	
	TIM_OCInitTypeDef  TIM_OCInitStruct;	
	/*PWMģʽ1�� �����ϼ���ʱ��һ��TIMx_CNT<TIMx_CCR1ʱͨ��1Ϊ��Ч��ƽ������Ϊ��Ч��ƽ��
	�����¼���ʱ��һ��TIMx_CNT>TIMx_CCR1ʱͨ��1Ϊ��Ч��ƽ(OC1REF=0)������Ϊ��Ч��ƽ(OC1REF=1)�� 
	PWMģʽ2�� �����ϼ���ʱ��һ��TIMx_CNT<TIMx_CCR1ʱͨ��1Ϊ��Ч��ƽ������Ϊ��Ч��ƽ�������¼���ʱ��һ��TIMx_CNT>TIMx_CCR1ʱͨ��1Ϊ��Ч��ƽ������Ϊ��Ч��ƽ��
	*/
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM2;//��ʱ��ģʽ��TIM�����ȵ���ģʽ2
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse=710;
	TIM_OC4Init(TIM4, &TIM_OCInitStruct);
	TIM_OC1Init(TIM4, &TIM_OCInitStruct);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_CtrlPWMOutputs(TIM4, ENABLE);
	//����ռ�ձ�CCR
	
	
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


