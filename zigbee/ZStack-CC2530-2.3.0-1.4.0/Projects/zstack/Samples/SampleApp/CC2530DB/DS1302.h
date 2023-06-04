#ifndef __DS1302_H__
#define __DS1302_H__

extern void ds1302_init(void);
//extern void ds1302_write_time(void);

extern void ds1302_read_time(void);
extern unsigned char dis_time_buf[16];
extern unsigned char time_buf[8];
//extern void InitT3(void);
#endif
