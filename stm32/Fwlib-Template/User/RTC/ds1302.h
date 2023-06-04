#ifndef __ds1302_H 
#define __ds1302_H  

#include "stm32f10x.h"

#define CE_L GPIO_ResetBits(GPIOC,GPIO_Pin_1)//拉低使能位
#define CE_H GPIO_SetBits(GPIOC,GPIO_Pin_1)//拉高使能位
#define SCLK_L GPIO_ResetBits(GPIOC,GPIO_Pin_0)//拉低时钟线
#define SCLK_H  GPIO_SetBits(GPIOC,GPIO_Pin_0)//拉高时钟线
#define DATA_L  GPIO_ResetBits(GPIOA,GPIO_Pin_1)//拉低数据线
#define DATA_H  GPIO_SetBits(GPIOA,GPIO_Pin_1)//拉高数据线

struct TIMEData
{
	uint16_t year;
	uint8_t  month;
	uint8_t  day;
	uint8_t  hour;
	uint8_t  minute;
	uint8_t  second;
	uint8_t  week;
};//创建TIMEData结构体方便存储时间日期数据

void ds1302_gpio_init();//ds1302端口初始化
void ds1302_write_onebyte(uint8_t data);//向ds1302发送一字节数据
void ds1302_wirte_rig(uint8_t address,uint8_t data);//向指定寄存器写一字节数据
uint8_t ds1302_read_rig(uint8_t address);//从指定寄存器读一字节数据
void ds1032_init();//ds1302初始化函数
void ds1032_DATAOUT_init();//IO端口配置为输出
void ds1032_DATAINPUT_init();//IO端口配置为输入
void ds1032_read_time();//从ds1302读取实时时间（BCD码）
void ds1032_read_realTime();//将BCD码转化为十进制数据
 
#endif