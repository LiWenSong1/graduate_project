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
uint8_t first=1;
uint8_t First=1;
char *recv;
char *foodrecv;
uint8_t j=0;
uint8_t k=0;
uint8_t recv_len=0;
uint8_t Data_Flag=0;
uint8_t Max_MinFlag=0;
uint8_t addflag=0;
char *Max_Minbuf;

//串口GPIO引脚初始化，PA9,PA10
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
//串口GPIO引脚初始化，PA2,PA3
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


//串口1初始化,使用接收中断
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
//串口2初始化,使用接收中断
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
	Max_Minbuf=(char *)malloc(64);
	foodrecv=(char *)malloc(32);
	
}
//重定向fputc，实现printf功能
int fputc(int ch,FILE *f)
{
	
		USART_SendData(USART1, (uint8_t)ch);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
		USART_ClearFlag(USART1, USART_FLAG_TC);
		return ch;

	
}

/*****************  接受字符串 **********************/
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
  
  /* 等待发送完成 */
  delay_ms(10);
	return str;


}
///重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
		/* 等待串口输入数据 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
}


/*****************  发送字符串 **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      USART_SendData( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* 等待发送完成 */
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
		//如果接收到消息
		if(Max_MinFlag==0 || Data_Flag==0 ||addflag==0){
			if(Max_MinFlag==1 && Data_Flag==1){
				*(foodrecv + j)=USART_ReceiveData(USART2);
				if(*(foodrecv + j)=='\0'){	
						j=0;
					if(foodrecv[0]=='{' && foodrecv[1]=='A'  && foodrecv[2]=='D' && foodrecv[3]=='D'){
							printf("{ADD}\n");
							addflag=1;
					}
				}else{
				j++;}
			
			}
			else{
				*(recv + k)=USART_ReceiveData(USART2);
				//USART_SendData(USART1,*(recv + k));
				if(*(recv + k)=='\0'){			
					recv_len=k;
					k=0;
			
					if(recv[0]=='{' && recv[1]=='M'  && recv[2]=='a' && recv[3]=='x'){
						Max_MinFlag=1;
						strcpy(Max_Minbuf,recv);
						
					
					}
					
					if(recv[0]=='{' && recv[1]=='A'  && recv[2]=='D' && recv[3]=='D'){
						printf("{ADD}\n");
						addflag=1;
					
					}
				
					else if(recv[0]=='{' && recv[recv_len-3]=='}')
					{
						Data_Flag=1;
					}
					
				}
				
				else{
						k++;
						
				}
			
			}
		}
		
		
		
		//USART_SendData(USART1,recv)
		
		//recv=Usart_RecvString(USART2);
		//recv_len=strlen(recv);
		
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}
void food_add(int food_value,int food_thresholdmin,int food_thresholdmax)
{
	if(food_value>=0 && food_value<food_thresholdmax*2){
		if(food_value<food_thresholdmin && first==0){
			//开舵机,投料
			TIM_SetCompare1(TIM8,1850);
			delay_ms(500);
			
		}
		else if(food_value<food_thresholdmin && first==1)
		{
			first=0;
			
		}
		else if(food_value>(food_thresholdmin+food_thresholdmax)/2 && First==0){
			//关舵机
			TIM_SetCompare1(TIM8,1900);
			delay_ms(500);
			addflag=0;
		}
		else if(food_value>(food_thresholdmin+food_thresholdmax)/2 && First==1){
			First=0;
		
		}
	
	
	}
		
}