#include <ioCC2530.h>
#include "hal_types.h"
#include "hal_lcd.h"
#include "DHT11.h"
//DS1302引脚定义
#define RST P0_7
#define IO P0_1
#define SCK P0_0
//DS1302地址定义
#define ds1302_sec_add			0x80		//秒数据地址
#define ds1302_min_add			0x82		//分数据地址
#define ds1302_hr_add			0x84		//时数据地址
#define ds1302_date_add			0x86		//日数据地址
#define ds1302_month_add		0x88		//月数据地址
#define ds1302_day_add			0x8a		//星期数据地址
#define ds1302_year_add			0x8c		//年数据地址
#define ds1302_control_add		0x8e		//控制数据地址
#define ds1302_charger_add		0x90 					 
#define ds1302_clkburst_add		0xbe
//初始时间定义
unsigned char time_buf[8] = {0x20,0x23,0x05,0x25,0x16,0x32,0x01,0x04};//初始时间2010年6月1号23点59分55秒 星期二
//unsigned char time_buf[8] ="sdsd";//初始时间2010年6月1号23点59分55秒 星期二
//uint16 count=0;
unsigned char dis_time_buf[16]={0};

void ds1302_init(void);
void ds1302_write_byte(unsigned char addr, unsigned char d);
unsigned char ds1302_read_byte(unsigned char addr);
//void ds1302_write_time(void);

void ds1302_read_time(void);
//void InitT3(void);
//DS1302初始化函数
void ds1302_init(void)
{
   
	ds1302_write_byte(0x8e,0x00);//关闭写保护
	ds1302_write_byte(0x80,0x37);//seconds37秒
	ds1302_write_byte(0x82,0x58);//minutes58分
	ds1302_write_byte(0x84,0x23);//hours23时
	ds1302_write_byte(0x86,0x30);//date30日
	ds1302_write_byte(0x88,0x09);//months9月
	ds1302_write_byte(0x8a,0x07);//days星期日
	ds1302_write_byte(0x8c,0x20);//year2020年
	ds1302_write_byte(0x8e,0x80);//关闭写保护
}

//向DS1302写入一字节数据
void ds1302_write_byte(unsigned char addr, unsigned char d) 
{
      //P0DIR |=0x02;  //P0_1为输出
	unsigned char i;
        SCK=0;
        RST=0;
        Delay_us();
	RST=1;	
        Delay_us();
        Delay_us();//启动DS1302总线	
	//写入目标地址：addr
	addr = addr & 0xFE;   //最低位置零，寄存器0位为0时写，为1时读
	for (i = 0; i < 8; i ++) {
          SCK=0;
		if (addr & 0x01) {
			IO=1;
			}
		else {
			IO=0;
			}
		SCK=1;      //产生时钟
                Delay_us();
                 Delay_us();
		//SCK=0;
		addr = addr >> 1;
		}	
	//写入数据：d
        SCK=0;
	for (i = 0; i < 8; i ++) {
          SCK=0;
		if (d & 0x01) {
			IO=1;
			}
		else {
			IO=0;
			}
		SCK=1;    //产生时钟  
		Delay_us();
                 Delay_us();
		d = d >> 1;
		} 
	RST=0;		//停止DS1302总线
        SCK=0;
         Delay_us();
        Delay_us();
}
//从DS1302读出一字节数据
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
        Delay_us();//启动DS1302总线
	//写入目标地址：addr
	addr = addr | 0x01;    //最低位置高，寄存器0位为0时写，为1时读
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
       
	//输出数据：temp
        P0DIR &=0xfd; //P0_1为输入
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
        P0DIR |=0x02;  //P0_1为输出
        IO=0;//停止DS1302总线
	return temp;
}
/*
//向DS302写入时钟数据
void ds1302_write_time(void) 
{
	ds1302_write_byte(ds1302_control_add,0x00);			//关闭写保护 
	ds1302_write_byte(ds1302_sec_add,0x80);				//暂停时钟 
	//ds1302_write_byte(ds1302_charger_add,0xa9);	    //涓流充电 
	ds1302_write_byte(ds1302_year_add,time_buf[1]);		//年 
	ds1302_write_byte(ds1302_month_add,time_buf[2]);	//月 
	ds1302_write_byte(ds1302_date_add,time_buf[3]);		//日 
	ds1302_write_byte(ds1302_hr_add,time_buf[4]);		//时 
	ds1302_write_byte(ds1302_min_add,time_buf[5]);		//分
	ds1302_write_byte(ds1302_sec_add,time_buf[6]);		//秒
	ds1302_write_byte(ds1302_day_add,time_buf[7]);		//周 
	ds1302_write_byte(ds1302_control_add,0x80);			//打开写保护     
}
*/
//从DS302读出时钟数据
void ds1302_read_time(void)  
{
  time_buf[0]=ds1302_read_byte(0x81);//读秒
time_buf[1]=ds1302_read_byte(0x83);//读分
time_buf[2]=ds1302_read_byte(0x85);//读时
time_buf[3]=ds1302_read_byte(0x87);//读日
time_buf[4]=ds1302_read_byte(0x89);//读月
time_buf[5]=ds1302_read_byte(0x8B);//读星期
time_buf[6]=ds1302_read_byte(0x8D);//读年
  /*
	time_buf[1]=ds1302_read_byte(ds1302_year_add);		//年 
	time_buf[2]=ds1302_read_byte(ds1302_month_add);		//月 
	time_buf[3]=ds1302_read_byte(ds1302_date_add);		//日 
	time_buf[4]=ds1302_read_byte(ds1302_hr_add);		//时 
	time_buf[5]=ds1302_read_byte(ds1302_min_add);		//分 
	time_buf[6]=(ds1302_read_byte(ds1302_sec_add))&0x7f;//秒，屏蔽秒的第7位，避免超出59
	time_buf[7]=ds1302_read_byte(ds1302_day_add);		//周 	
        
        dis_time_buf[0]=(time_buf[0]>>4)+'0'; //年   
         dis_time_buf[1]=(time_buf[0]&0x0f)+'0';
         
         dis_time_buf[2]=(time_buf[1]>>4)+'0';   
         dis_time_buf[3]=(time_buf[1]&0x0f)+'0';
        
        
         
         
         dis_time_buf[4]=(time_buf[2]>>4)+'0'; //月  
         dis_time_buf[5]=(time_buf[2]&0x0f)+'0';
         
        
         dis_time_buf[6]=(time_buf[3]>>4)+'0'; //日   
         dis_time_buf[7]=(time_buf[3]&0x0f)+'0';
         
        // dis_time_buf[14]=(time_buf[7]&0x07)+'0'; //星期
         
         /*
         //第2行显示  
         dis_time_buf[0]=(time_buf[4]>>4)+'0'; //时   
         dis_time_buf[1]=(time_buf[4]&0x0f)+'0';   
        
         dis_time_buf[2]=':';
        
         dis_time_buf[3]=(time_buf[5]>>4)+'0'; //分   
         dis_time_buf[4]=(time_buf[5]&0x0f)+'0';   
         dis_time_buf[5]=' ';
         dis_time_buf[6]=(time_buf[6]>>4)+'0'; //秒   
         dis_time_buf[7]=(time_buf[6]&0x0f)+'0';
         */
}
