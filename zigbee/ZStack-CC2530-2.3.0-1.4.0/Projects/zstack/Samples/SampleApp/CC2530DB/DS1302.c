#include <ioCC2530.h>
#include "hal_types.h"
#include "hal_lcd.h"
#include "DHT11.h"
//DS1302���Ŷ���
#define RST P0_7
#define IO P0_1
#define SCK P0_0
//DS1302��ַ����
#define ds1302_sec_add			0x80		//�����ݵ�ַ
#define ds1302_min_add			0x82		//�����ݵ�ַ
#define ds1302_hr_add			0x84		//ʱ���ݵ�ַ
#define ds1302_date_add			0x86		//�����ݵ�ַ
#define ds1302_month_add		0x88		//�����ݵ�ַ
#define ds1302_day_add			0x8a		//�������ݵ�ַ
#define ds1302_year_add			0x8c		//�����ݵ�ַ
#define ds1302_control_add		0x8e		//�������ݵ�ַ
#define ds1302_charger_add		0x90 					 
#define ds1302_clkburst_add		0xbe
//��ʼʱ�䶨��
unsigned char time_buf[8] = {0x20,0x23,0x05,0x25,0x16,0x32,0x01,0x04};//��ʼʱ��2010��6��1��23��59��55�� ���ڶ�
//unsigned char time_buf[8] ="sdsd";//��ʼʱ��2010��6��1��23��59��55�� ���ڶ�
//uint16 count=0;
unsigned char dis_time_buf[16]={0};

void ds1302_init(void);
void ds1302_write_byte(unsigned char addr, unsigned char d);
unsigned char ds1302_read_byte(unsigned char addr);
//void ds1302_write_time(void);

void ds1302_read_time(void);
//void InitT3(void);
//DS1302��ʼ������
void ds1302_init(void)
{
   
	ds1302_write_byte(0x8e,0x00);//�ر�д����
	ds1302_write_byte(0x80,0x37);//seconds37��
	ds1302_write_byte(0x82,0x58);//minutes58��
	ds1302_write_byte(0x84,0x23);//hours23ʱ
	ds1302_write_byte(0x86,0x30);//date30��
	ds1302_write_byte(0x88,0x09);//months9��
	ds1302_write_byte(0x8a,0x07);//days������
	ds1302_write_byte(0x8c,0x20);//year2020��
	ds1302_write_byte(0x8e,0x80);//�ر�д����
}

//��DS1302д��һ�ֽ�����
void ds1302_write_byte(unsigned char addr, unsigned char d) 
{
      //P0DIR |=0x02;  //P0_1Ϊ���
	unsigned char i;
        SCK=0;
        RST=0;
        Delay_us();
	RST=1;	
        Delay_us();
        Delay_us();//����DS1302����	
	//д��Ŀ���ַ��addr
	addr = addr & 0xFE;   //���λ���㣬�Ĵ���0λΪ0ʱд��Ϊ1ʱ��
	for (i = 0; i < 8; i ++) {
          SCK=0;
		if (addr & 0x01) {
			IO=1;
			}
		else {
			IO=0;
			}
		SCK=1;      //����ʱ��
                Delay_us();
                 Delay_us();
		//SCK=0;
		addr = addr >> 1;
		}	
	//д�����ݣ�d
        SCK=0;
	for (i = 0; i < 8; i ++) {
          SCK=0;
		if (d & 0x01) {
			IO=1;
			}
		else {
			IO=0;
			}
		SCK=1;    //����ʱ��  
		Delay_us();
                 Delay_us();
		d = d >> 1;
		} 
	RST=0;		//ֹͣDS1302����
        SCK=0;
         Delay_us();
        Delay_us();
}
//��DS1302����һ�ֽ�����
unsigned char ds1302_read_byte(unsigned char addr) {

	unsigned char i,temp;	
        RST=0;	
        SCK=0;
        Delay_us();
        Delay_us();
        Delay_us();
	RST=1;	
        Delay_us();
        Delay_us();
        Delay_us();//����DS1302����
	//д��Ŀ���ַ��addr
	addr = addr | 0x01;    //���λ�øߣ��Ĵ���0λΪ0ʱд��Ϊ1ʱ��
	for (i = 0; i < 8; i ++) {
           SCK=0;
		if (addr & 0x01) {
			IO=1;
			}
		else {
			IO=0;
			}
		SCK=1;
		//SCK=0;
		addr = addr >> 1;
		}	
       
	//������ݣ�temp
        P0DIR &=0xfd; //P0_1Ϊ����
          Delay_us();
        Delay_us();
	for (i = 0; i < 8; i ++) {
                Delay_us();
                Delay_us();
                
		temp = temp >> 1;
                SCK=1;
                Delay_us();
                Delay_us();
                Delay_us();
                Delay_us();
                SCK=0;
                Delay_10us();
                 Delay_us();
                Delay_us();
                Delay_us();
                Delay_us();
		if (IO) {
			temp |= 0x80;
		
                }
                
		else {
			temp &= 0x7F;
			}
                
               
		}
         Delay_us();
         Delay_us();
	RST=0;
        P0DIR |=0x02;  //P0_1Ϊ���
        IO=0;//ֹͣDS1302����
	return temp;
}
/*
//��DS302д��ʱ������
void ds1302_write_time(void) 
{
	ds1302_write_byte(ds1302_control_add,0x00);			//�ر�д���� 
	ds1302_write_byte(ds1302_sec_add,0x80);				//��ͣʱ�� 
	//ds1302_write_byte(ds1302_charger_add,0xa9);	    //������ 
	ds1302_write_byte(ds1302_year_add,time_buf[1]);		//�� 
	ds1302_write_byte(ds1302_month_add,time_buf[2]);	//�� 
	ds1302_write_byte(ds1302_date_add,time_buf[3]);		//�� 
	ds1302_write_byte(ds1302_hr_add,time_buf[4]);		//ʱ 
	ds1302_write_byte(ds1302_min_add,time_buf[5]);		//��
	ds1302_write_byte(ds1302_sec_add,time_buf[6]);		//��
	ds1302_write_byte(ds1302_day_add,time_buf[7]);		//�� 
	ds1302_write_byte(ds1302_control_add,0x80);			//��д����     
}
*/
//��DS302����ʱ������
void ds1302_read_time(void)  
{
  time_buf[0]=ds1302_read_byte(0x81);//����
time_buf[1]=ds1302_read_byte(0x83);//����
time_buf[2]=ds1302_read_byte(0x85);//��ʱ
time_buf[3]=ds1302_read_byte(0x87);//����
time_buf[4]=ds1302_read_byte(0x89);//����
time_buf[5]=ds1302_read_byte(0x8B);//������
time_buf[6]=ds1302_read_byte(0x8D);//����
  /*
	time_buf[1]=ds1302_read_byte(ds1302_year_add);		//�� 
	time_buf[2]=ds1302_read_byte(ds1302_month_add);		//�� 
	time_buf[3]=ds1302_read_byte(ds1302_date_add);		//�� 
	time_buf[4]=ds1302_read_byte(ds1302_hr_add);		//ʱ 
	time_buf[5]=ds1302_read_byte(ds1302_min_add);		//�� 
	time_buf[6]=(ds1302_read_byte(ds1302_sec_add))&0x7f;//�룬������ĵ�7λ�����ⳬ��59
	time_buf[7]=ds1302_read_byte(ds1302_day_add);		//�� 	
        
        dis_time_buf[0]=(time_buf[0]>>4)+'0'; //��   
         dis_time_buf[1]=(time_buf[0]&0x0f)+'0';
         
         dis_time_buf[2]=(time_buf[1]>>4)+'0';   
         dis_time_buf[3]=(time_buf[1]&0x0f)+'0';
        
        
         
         
         dis_time_buf[4]=(time_buf[2]>>4)+'0'; //��  
         dis_time_buf[5]=(time_buf[2]&0x0f)+'0';
         
        
         dis_time_buf[6]=(time_buf[3]>>4)+'0'; //��   
         dis_time_buf[7]=(time_buf[3]&0x0f)+'0';
         
        // dis_time_buf[14]=(time_buf[7]&0x07)+'0'; //����
         
         /*
         //��2����ʾ  
         dis_time_buf[0]=(time_buf[4]>>4)+'0'; //ʱ   
         dis_time_buf[1]=(time_buf[4]&0x0f)+'0';   
        
         dis_time_buf[2]=':';
        
         dis_time_buf[3]=(time_buf[5]>>4)+'0'; //��   
         dis_time_buf[4]=(time_buf[5]&0x0f)+'0';   
         dis_time_buf[5]=' ';
         dis_time_buf[6]=(time_buf[6]>>4)+'0'; //��   
         dis_time_buf[7]=(time_buf[6]&0x0f)+'0';
         */
}
