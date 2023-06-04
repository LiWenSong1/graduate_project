#include "time.h"
//��������pwm���ƶ��
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
	//������װ��ֵARR
	TIM_TimeBaseInitStruct.TIM_Period=arr;
	//����Ԥ��Ƶֵpsc
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc;
	//TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseInitStruct);
	
	
	TIM_OCInitTypeDef  TIM_OCInitStruct;
	
	//TIM_OCInitStruct.TIM_OCIdleState=TIM_OCIdleState_Set;
	/*PWMģʽ1�� �����ϼ���ʱ��һ��TIMx_CNT<TIMx_CCR1ʱͨ��1Ϊ��Ч��ƽ������Ϊ��Ч��ƽ��
	�����¼���ʱ��һ��TIMx_CNT>TIMx_CCR1ʱͨ��1Ϊ��Ч��ƽ(OC1REF=0)������Ϊ��Ч��ƽ(OC1REF=1)�� 
	PWMģʽ2�� �����ϼ���ʱ��һ��TIMx_CNT<TIMx_CCR1ʱͨ��1Ϊ��Ч��ƽ������Ϊ��Ч��ƽ�������¼���ʱ��һ��TIMx_CNT>TIMx_CCR1ʱͨ��1Ϊ��Ч��ƽ������Ϊ��Ч��ƽ��
	*/
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM2;//��ʱ��ģʽ��TIM�����ȵ���ģʽ2
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;
	//TIM_OCInitStruct.TIM_Pulse=500;
//	TIM_OCInitStruct.TIM_OCNIdleState=TIM_OCIdleState_Reset;
//	TIM_OCInitStruct.TIM_OutputNState=TIM_OutputNState_Enable;
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse=1900;
	TIM_OC1Init(TIM8, &TIM_OCInitStruct);
	TIM_ARRPreloadConfig(TIM8, ENABLE);
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);
	//����ռ�ձ�CCR
	
	
	TIM_Cmd(TIM8, ENABLE);
	//�߼���ʱ������ʹ��
	TIM_CtrlPWMOutputs(TIM8, ENABLE);
}
void tim2_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	 TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	//TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	//TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	//������װ��ֵARR
	TIM_TimeBaseInitStruct.TIM_Period=4999;
	//����Ԥ��Ƶֵpsc
	TIM_TimeBaseInitStruct.TIM_Prescaler=7199;
	//TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
			// ����������жϱ�־λ
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Ƕ�������жϿ�������ѡ�� */
  //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* ����USARTΪ�ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  /* �������ȼ�*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* �����ȼ� */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* ʹ���ж� */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* ��ʼ������NVIC */
  NVIC_Init(&NVIC_InitStructure);
		// �����������ж�
   TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
		
		// ʹ�ܼ�����
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
