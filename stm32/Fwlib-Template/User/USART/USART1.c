#include "USART1.h"
#include <stdio.h>
#include "delay.h"
#include "moto_driver.h"
#include <stdlib.h>
#include <string.h>
extern int g_USART_FLAG;
int forWard=0;
int backWard=0;
int Stop=0;
char *recv;
uint8_t k=0;
uint8_t recv_len=0;
uint8_t Flag=0;
//����GPIO���ų�ʼ����PA9,PA10
void GPIO_Usart1_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	

}
//����GPIO���ų�ʼ����PA2,PA3
void GPIO_Usart2_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	

}


//����1��ʼ��,ʹ�ý����ж�
void usart1_init(void)
{
	GPIO_Usart1_init();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate=115200;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStruct.USART_Parity=USART_Parity_No;
	USART_InitStruct.USART_StopBits=USART_StopBits_1;
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStruct);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef  NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStruct);

	USART_Cmd(USART1, ENABLE);
	
}
//����2��ʼ��,ʹ�ý����ж�
void usart2_init(void)
{
	GPIO_Usart2_init();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate=9600;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStruct.USART_Parity=USART_Parity_No;
	USART_InitStruct.USART_StopBits=USART_StopBits_1;
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStruct);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef  NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_Cmd(USART2, ENABLE);
	recv=(char *)malloc(128);
	
}
//�ض���fputc��ʵ��printf����
int fputc(int ch,FILE *f)
{
	
		USART_SendData(USART1, (uint8_t)ch);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
		USART_ClearFlag(USART1, USART_FLAG_TC);
		return ch;

	
}

/*****************  �����ַ��� **********************/
char *Usart_RecvString( USART_TypeDef * pUSARTx)
{
	char *str;
	str=(char *)malloc(128);
	unsigned int k=0;
	*(str + k)=USART_ReceiveData( pUSARTx);
  do 
  {
			k++;
      *(str + k)=USART_ReceiveData( pUSARTx);
  } while(*(str + k)!='\0');
  
  /* �ȴ�������� */
  delay_ms(10);
	return str;


}
///�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
		/* �ȴ������������� */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
}


/*****************  �����ַ��� **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      USART_SendData( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* �ȴ�������� */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}
void USART1_IRQHandler(void)
{
	char recv;
	if(USART_GetITStatus(USART1, USART_IT_RXNE)!=RESET){	
		recv=USART_ReceiveData(USART1);
		switch(recv){
			case 'o':Turn_left();break;				
			case 'c':Turn_Right();break;
			case 's':Turn_Stop();break;
		
		}
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);

	}
}
void USART2_IRQHandler(void)
{
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE)!=RESET)
	{				
		
		//
		if(Flag==0){
			*(recv + k)=USART_ReceiveData(USART2);
			//USART_SendData(USART1,*(recv + k));
			if(*(recv + k)=='\0'){
				Flag=1;
				recv_len=k;
				k=0;
				
			}else{
					k++;
					
			}
		
		}
		
		
		
		//USART_SendData(USART1,recv)
		
		//recv=Usart_RecvString(USART2);
		//recv_len=strlen(recv);
		
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}