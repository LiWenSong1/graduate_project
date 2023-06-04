#include "time.h"
//用于生成pwm控制舵机
void tim8_0ut_init(u32 arr,u32 psc)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO|RCC_APB2Periph_TIM8, ENABLE);
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	
	 TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	//设置重装载值ARR
	TIM_TimeBaseInitStruct.TIM_Period=arr;
	//设置预分频值psc
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc;
	//TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseInitStruct);
	
	
	TIM_OCInitTypeDef  TIM_OCInitStruct;
	
	//TIM_OCInitStruct.TIM_OCIdleState=TIM_OCIdleState_Set;
	/*PWM模式1－ 在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为有效电平，否则为无效电平；
	在向下计数时，一旦TIMx_CNT>TIMx_CCR1时通道1为无效电平(OC1REF=0)，否则为有效电平(OC1REF=1)。 
	PWM模式2－ 在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为无效电平，否则为有效电平；在向下计数时，一旦TIMx_CNT>TIMx_CCR1时通道1为有效电平，否则为无效电平。
	*/
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM2;//定时器模式，TIM脉冲宽度调制模式2
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;
	//TIM_OCInitStruct.TIM_Pulse=500;
//	TIM_OCInitStruct.TIM_OCNIdleState=TIM_OCIdleState_Reset;
//	TIM_OCInitStruct.TIM_OutputNState=TIM_OutputNState_Enable;
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse=1900;
	TIM_OC1Init(TIM8, &TIM_OCInitStruct);
	TIM_ARRPreloadConfig(TIM8, ENABLE);
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);
	//设置占空比CCR
	
	
	TIM_Cmd(TIM8, ENABLE);
	//高级定时器必须使用
	TIM_CtrlPWMOutputs(TIM8, ENABLE);
}
void tim2_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	 TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	//TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	//TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	//设置重装载值ARR
	TIM_TimeBaseInitStruct.TIM_Period=4999;
	//设置预分频值psc
	TIM_TimeBaseInitStruct.TIM_Prescaler=7199;
	//TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
			// 清除计数器中断标志位
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 嵌套向量中断控制器组选择 */
  //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* 配置USART为中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  /* 抢断优先级*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* 子优先级 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* 使能中断 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* 初始化配置NVIC */
  NVIC_Init(&NVIC_InitStructure);
		// 开启计数器中断
   TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
		
		// 使能计数器
		TIM_Cmd(TIM2, ENABLE);
	}	
int update=0;
int num=0;
void TIM2_IRQHandler(void)
{

	if(TIM_GetITStatus(TIM2, TIM_IT_Update)==SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		if(num<60)
		{
		num++;
		}
		else 
		{
		update=1;num=0;
		}
		
	}


}
