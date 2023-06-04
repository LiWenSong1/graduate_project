#include "stm32f10x.h"
#include "USART1.h"
#include "delay.h"
#include "moto_driver.h"
#include "Data_handle.h"
#include "time.h"
#include <stdio.h>
#include <string.h>
#include "ds1302.h"


#define LED0_OPEN GPIO_ResetBits(GPIOA, GPIO_Pin_8)
#define LED0_CLOSE GPIO_SetBits(GPIOA, GPIO_Pin_8)

#define Water_OPEN GPIO_ResetBits(GPIOB, GPIO_Pin_15)
#define Water_CLOSE GPIO_SetBits(GPIOB, GPIO_Pin_15)

#define Fan_OPEN GPIO_ResetBits(GPIOB, GPIO_Pin_14)
#define Fan_CLOSE GPIO_SetBits(GPIOB, GPIO_Pin_14)

#define temp_name "\"temp\":"
#define air_name "\"Air\":"
#define humid_name "\"humid\":"
#define food_name "\"food\":"

#define maxTname "MaxT:"
#define minTname "MinT:"
#define maxAname "MaxA:"
#define minAname "MinA:"
#define hourname "hour:"
#define minutename "minute:"
char buf_recv[64];
extern char *recv,*Max_Minbuf;
extern uint8_t recv_len;
extern uint8_t Data_Flag,Max_MinFlag,addflag;
extern struct tm calendar;
extern struct TIMEData TimeData;//全局变量
uint8_t temp_value,humid_value,air_value,food_value;
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
	systick_init();
	//RTC_Init();
	LED_gpio_init();
	usart1_init();
	//串口2用于接收协调器发送的数据
	usart2_init();
ds1302_gpio_init();delay_ms(5); //初始化ds1302端口
//ds1032_init();delay_ms(5);      //ds1302初始化
	moto_gpio_init();
	TIM4_init();
//	//定时器2用于定时
//	tim2_init();
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
		//Turn_Right();
	//关舵机
	//TIM_SetCompare1(TIM8,1900);
		delay_ms(1000);
	uint8_t maxtemp=40;
	uint8_t mintemp=0;
	uint8_t maxair=120;
	uint8_t minair=60;
	uint16_t hour=23;
	uint8_t minute=0;

	uint8_t food_thresholdmin=30;
	uint8_t food_thresholdmax=60;
	uint8_t time=0;
	//printf("succed\n");
	
	while(1){
		
		ds1032_read_realTime();         //读取实时数据
		printf("year:%d,month:%d,day:%d,hour:%d,minute:%d,sedcon:%d\n",TimeData.year,TimeData.month,TimeData.day,TimeData.hour,TimeData.minute,TimeData.second);
		delay_ms(1000);
		if(addflag==1){
			food_add(food_value,food_thresholdmin,food_thresholdmax);
		
		}	
		if(Max_MinFlag==1){		//是设置阈值数据
					//strcpy(buf_recv,recv);
					//delay_ms(10);
			
						printf(Max_Minbuf);
						maxtemp=get_data(maxTname,Max_Minbuf,recv_len);
						mintemp=get_data(minTname,Max_Minbuf,recv_len);
						maxair=get_data(maxAname,Max_Minbuf,recv_len);
						minair=get_data(minAname,Max_Minbuf,recv_len);
						hour=get_data(hourname,Max_Minbuf,recv_len);
						minute=get_data(minutename,Max_Minbuf,recv_len);				
						printf("maxtemp:%d\n",maxtemp);
						printf("mintemp:%d\n",mintemp);
						printf("maxair:%d\n",maxair);
						printf("minair:%d\n",minair);
						printf("hour:%d\n",hour);
						printf("Minute:%d\n",minute);
						memset(Max_Minbuf,0,64);
					//sprintf("")
					//Usart_SendString("")
			//printf("time:year:%d,month:%d\n",TimeData.year,TimeData.month);
						Max_MinFlag=0;
				}
				
				
				if(Data_Flag==1){     //是传感器数据
					strcpy(buf_recv,recv);					
					printf(buf_recv);
						temp_value=get_data(temp_name,buf_recv,recv_len);
						air_value=get_data(air_name,buf_recv,recv_len);
						humid_value=get_data(humid_name,buf_recv,recv_len);
						food_value=get_data(food_name,buf_recv,recv_len);
						printf("temp:%d\n",temp_value);
						printf("air:%d\n",air_value);
						printf("humid:%d\n",humid_value);	
						printf("food:%d\n",food_value);	
						memset(buf_recv,0,64);
					
						if(temp_value>maxtemp){															//如果温度大于所设上限，启动电机、水泵和风扇
							printf(">>>\n");
							if(curtain_open==0){
								printf("open\n");
									//启动电机
									Turn_Right();											
									delay_ms(1000);
									delay_ms(1000);
									curtain_open=1;
									//关闭电机
									Turn_Stop();
									//启动水泵
									Water_OPEN;
									//启动风扇
									Fan_OPEN;
								delay_ms(1000);
								
								
									
							}
						
						}
						
						if(temp_value<mintemp){																			//如果温度小于所设下限，关闭电机、水泵和风扇
							printf("<<<\n");
							if(curtain_open==1){
								printf("close\n");	
							//	//启动电机
									Turn_left();
									delay_ms(1000);
									delay_ms(1000);
									curtain_open=0;
									//关闭电机
									Turn_Stop();
									//关闭水泵
									Water_CLOSE;
									//关闭风扇
									Fan_CLOSE;
								delay_ms(1000);
								
							
							}
						}
						if(air_value>maxair){																				//如果气体浓度高于所设上限，开启风扇
								
								//启动风扇
									Fan_OPEN;
									delay_ms(1000);
								
						
						}
						if(air_value<minair){																					//如果气体浓度低于所设下限，关闭风扇
							if(curtain_open==0){
								//关闭风扇
									Fan_CLOSE;
								delay_ms(1000);
							
							}								
						
						}
					Data_Flag=0;
					
				}
				if(hour==TimeData.hour && minute==TimeData.minute){
					food_add(food_value,food_thresholdmin,food_thresholdmax);
				
				
				}
				
					
						
						
		
								
		}				
						
		
// 来到这里的时候，系统的时钟已经被配置成72M。
}


