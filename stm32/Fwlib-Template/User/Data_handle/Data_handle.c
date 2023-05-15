#include "Data_handle.h"
#include <string.h>
#include <stdio.h>

//提取接收字符串中的数据
uint8_t get_data(const char* data_name,char* recv,uint8_t recv_len){
	char *str_head,*str_tail;
	uint8_t data=0;
	uint8_t data_len,data_name_len;
	data_name_len=strlen(data_name);
	if(recv_len<=0){
		return 0;
	}
	else{
		if(recv[0]=='{' && recv[recv_len-3]=='}'){
			str_head=strstr(recv,data_name);			
			str_tail=strstr(str_head,",");
			data_len=strlen(str_head)-strlen(str_tail)-data_name_len;
			switch(data_len){
				case 1:data=str_head[data_name_len]-'0';break;
				case 2:data=(str_head[data_name_len]-'0')*10+str_head[data_name_len+1]-'0';break;
				case 3:data=(str_head[data_name_len]-'0')*100+(str_head[data_name_len+1]-'0')*10+str_head[data_name_len+2];break;
			
			}
		}
		return data;
	}
			

}