#ifndef __DHT11_H__
#define __DHT11_H__

#define uchar unsigned char
extern void Delay_ms(unsigned int xms);	//��ʱ����
extern void COM(void);                  // ��ʪд��
extern void DHT11(void);                //��ʪ��������
extern void Delay_us(void); //1 us��ʱ
extern void Delay_10us(void); //10 us��ʱ
extern uchar temp[2]; 
extern uchar temp1[5];
extern uchar humidity[2];
extern uchar humidity1[9];
extern uchar shidu_shi,shidu_ge,wendu_shi,wendu_ge;
extern uchar ucharT_data_H,ucharRH_data_H;

#endif