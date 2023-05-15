#include "stm32f10x.h"
#include "USART1.h"
#include "delay.h"
#include "moto_driver.h"
#include "Data_handle.h"
#include "time.h"
#include <stdio.h>
#include <string.h>


#define LED0_OPEN GPIO_ResetBits(GPIOA, GPIO_Pin_8)
#define LED0_CLOSE GPIO_SetBits(GPIOA, GPIO_Pin_8)

#define Water_OPEN GPIO_ResetBits(GPIOB, GPIO_Pin_15)
#define Water_CLOSE GPIO_SetBits(GPIOB, GPIO_Pin_15)

#define Fan_OPEN GPIO_ResetBits(GPIOB, GPIO_Pin_14)
#define Fan_CLOSE GPIO_SetBits(GPIOB, GPIO_Pin_14)

#define temp_name "\"temp\":"
#define air_name "\"Air\":"
#define humid_name "\"humid\":"

extern char *recv;
extern uint8_t recv_len;
extern uint8_t Flag;
uint8_t temp_value,humid_value,air_value;
uint8_t curtain_open=0;
void LED_gpio_init(void){

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

int main(void)
{
	LED_gpio_init();
	usart1_init();
	//串口2用于接收协调器发送的数据
	usart2_init();
	systick_init();
	moto_gpio_init();
	TIM4_init();
	
	water_gpio_init();
		Water_CLOSE;
		Fan_CLOSE;
	//舵机定时器初始化
	tim8_0ut_init(1999,719);
	
	//启动电机
	//Turn_left();
	LED0_OPEN;
	char buf_recv[128];
	//开舵机
	//TIM_SetCompare1(TIM8,1800);
		
	//关舵机
	//TIM_SetCompare1(TIM8,1900);
		delay_ms(1000);
	uint8_t max_temp=24;
	uint8_t min_temp=24;
	uint8_t max_humid=70;
	uint8_t min_humid=65;
	printf("succed\n");
	while(1){
		
	
		if(Flag==1){           //得到一帧数据
			//提取数据
				strcpy(buf_recv,recv);
				Flag=0;
				printf("%s",buf_recv);
			
		}//				temp_value=get_data(temp_name,buf_recv,recv_len);
//				air_value=get_data(air_name,buf_recv,recv_len);
//				humid_value=get_data(humid_name,buf_recv,recv_len);
//				printf("temp:%d\n",temp_value);
//				printf("air:%d\n",air_value);
//				printf("humid:%d\n",humid_value);
//				if(temp_value>max_temp){
//					if(curtain_open==0){
//					//启动电机
//						Turn_Right();
//						curtain_open=1;
//						delay_ms(1000);
//						//delay_ms(1000);
//						
//						//关闭电机
//						Turn_Stop();
//						Water_CLOSE;
//						//Fan_CLOSE;
//						
//						
//					
//					
//					}			
//				
//				}
//				delay_ms(1000);
//						delay_ms(1000);
//						delay_ms(1000);
//				delay_ms(1000);
//						delay_ms(1000);
//				if(temp_value<min_temp){
//					if(curtain_open==1){
//						//启动电机
//						Turn_left();
//						delay_ms(1000);
//						//delay_ms(1000);
//						
//						curtain_open=0;
//						//关闭电机
//						Turn_Stop();
//						Water_OPEN;
//						//Fan_OPEN;
//						delay_ms(1000);
//						delay_ms(1000);
//						delay_ms(1000);
//						delay_ms(1000);
//						delay_ms(1000);
//						
//					}
//						
//					
//				
//				}
//		}

//	
	}
	

	
		

	// 来到这里的时候，系统的时钟已经被配置成72M。
}


