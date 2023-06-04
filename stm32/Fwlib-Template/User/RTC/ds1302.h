#ifndef __ds1302_H 
#define __ds1302_H  

#include "stm32f10x.h"

#define CE_L GPIO_ResetBits(GPIOC,GPIO_Pin_1)//����ʹ��λ
#define CE_H GPIO_SetBits(GPIOC,GPIO_Pin_1)//����ʹ��λ
#define SCLK_L GPIO_ResetBits(GPIOC,GPIO_Pin_0)//����ʱ����
#define SCLK_H  GPIO_SetBits(GPIOC,GPIO_Pin_0)//����ʱ����
#define DATA_L  GPIO_ResetBits(GPIOA,GPIO_Pin_1)//����������
#define DATA_H  GPIO_SetBits(GPIOA,GPIO_Pin_1)//����������

struct TIMEData
{
	uint16_t year;
	uint8_t  month;
	uint8_t  day;
	uint8_t  hour;
	uint8_t  minute;
	uint8_t  second;
	uint8_t  week;
};//����TIMEData�ṹ�巽��洢ʱ����������

void ds1302_gpio_init();//ds1302�˿ڳ�ʼ��
void ds1302_write_onebyte(uint8_t data);//��ds1302����һ�ֽ�����
void ds1302_wirte_rig(uint8_t address,uint8_t data);//��ָ���Ĵ���дһ�ֽ�����
uint8_t ds1302_read_rig(uint8_t address);//��ָ���Ĵ�����һ�ֽ�����
void ds1032_init();//ds1302��ʼ������
void ds1032_DATAOUT_init();//IO�˿�����Ϊ���
void ds1032_DATAINPUT_init();//IO�˿�����Ϊ����
void ds1032_read_time();//��ds1302��ȡʵʱʱ�䣨BCD�룩
void ds1032_read_realTime();//��BCD��ת��Ϊʮ��������
 
#endif