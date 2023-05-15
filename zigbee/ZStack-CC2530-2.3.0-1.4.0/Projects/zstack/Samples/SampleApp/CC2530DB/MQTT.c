#include "mqtt.h"         //包含需要的头文件
#include "string.h"       //包含需要的头文件
#include "stdio.h"        //包含需要的头文件
int   Fixed_len;                       			   //固定报头长度
int   Variable_len;                     		   //可变报头长度
int   Payload_len;                       		   //有效负荷长度

char ClientID[128];                                //存放客户端ID的缓冲区
int  ClientID_len;                                 //存放客户端ID的长度

char Username[128];                                //存放用户名的缓冲区
int  Username_len;								   //存放用户名的长度

char Passward[128];                                //存放密码的缓冲区
int  Passward_len;								   //存放密码的长度

char ServerIP[128];                                //存放服务器IP或是域名
int  ServerPort;                                   //存放服务器的端口号

unsigned char  temp_buff[TBUFF_UNIT];			   //临时缓冲区，构建报文用


/*----------------------------------------------------------*/
/*函数名：云初始化参数，得到客户端ID，用户名和密码          */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void IoT_Parameter_Init(void)
{	
	memset(ClientID,0,128);                              //客户端ID的缓冲区全部清零
	sprintf(ClientID,"%s",DEVICEID);                     //构建客户端ID，并存入缓冲区
	ClientID_len = strlen(ClientID);                     //计算客户端ID的长度
	
	memset(Username,0,128);                              //用户名的缓冲区全部清零
	sprintf(Username,"%s",PRODUCTID);                    //构建用户名，并存入缓冲区
	Username_len = strlen(Username);                     //计算用户名的长度
	
	memset(Passward,0,128);                              //用户名的缓冲区全部清零
	sprintf(Passward,"%s",AUTHENTICATION);               //构建密码，并存入缓冲区
	Passward_len = strlen(Passward);                     //计算密码的长度
	
	memset(ServerIP,0,128);  
	sprintf(ServerIP,"%s","183.230.40.39");              //构建服务器域名
	ServerPort = 6002;                                   //服务器端口号6002
	/*
	u1_printf("服 务 器：%s:%d\r\n",ServerIP,ServerPort); //串口输出调试信息
	u1_printf("客户端ID：%s\r\n",ClientID);               //串口输出调试信息
	u1_printf("用 户 名：%s\r\n",Username);               //串口输出调试信息
	u1_printf("密    码：%s\r\n",Passward);               //串口输出调试信息
        */
}

/*----------------------------------------------------------*/
/*函数名：连接服务器报文                                    */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_ConectPack(void)
{	
	int temp,Remaining_len;
	
	Fixed_len = 1;                                                        //连接报文中，固定报头长度暂时先=1
	Variable_len = 10;                                                    //连接报文中，可变报头长度=10
	Payload_len = 2 + ClientID_len + 2 + Username_len + 2 + Passward_len; //连接报文中，负载长度      
	Remaining_len = Variable_len + Payload_len;                           //剩余长度=可变报头长度+负载长度
	
	temp_buff[0]=0x10;                         //固定报头第1个字节 ：固定0x01		
	do{                                        //循环处理固定报头中的剩余长度字节，字节量根据剩余字节的真实长度变化
		temp = Remaining_len%128;              //剩余长度取余128
		Remaining_len = Remaining_len/128;     //剩余长度取整128
		if(Remaining_len>0)               	
			temp |= 0x80;                      //按协议要求位7置位          
		temp_buff[Fixed_len] = temp;           //剩余长度字节记录一个数据
		Fixed_len++;	                       //固定报头总长度+1    
	}while(Remaining_len > 0);                 //如果Remaining_len>0的话，再次进入循环
	
	temp_buff[Fixed_len + 0] = 0x00;     //可变报头第1个字节 ：固定0x00	            
	temp_buff[Fixed_len + 1] = 0x04;     //可变报头第2个字节 ：固定0x04
	temp_buff[Fixed_len + 2] = 0x4D;	 //可变报头第3个字节 ：固定0x4D
	temp_buff[Fixed_len + 3] = 0x51;	 //可变报头第4个字节 ：固定0x51
	temp_buff[Fixed_len + 4] = 0x54;	 //可变报头第5个字节 ：固定0x54
	temp_buff[Fixed_len + 5] = 0x54;     //可变报头第6个字节 ：固定0x54
	temp_buff[Fixed_len + 6] = 0x04;	 //可变报头第7个字节 ：固定0x04
	temp_buff[Fixed_len + 7] = 0xC2;	 //可变报头第8个字节 ：使能用户名和密码校验，不使用遗嘱，不保留会话
	temp_buff[Fixed_len + 8] = 0x00; 	 //可变报头第9个字节 ：保活时间高字节 0x00
	temp_buff[Fixed_len + 9] = 0x64;	 //可变报头第10个字节：保活时间高字节 0x64   100s
	
	/*     CLIENT_ID      */
	temp_buff[Fixed_len+10] = ClientID_len/256;                			  	//客户端ID长度高字节
	temp_buff[Fixed_len+11] = ClientID_len%256;               			  	//客户端ID长度低字节
	memcpy(&temp_buff[Fixed_len+12],ClientID,ClientID_len);                 //复制过来客户端ID字串	
	/*     用户名        */
	temp_buff[Fixed_len+12+ClientID_len] = Username_len/256; 				//用户名长度高字节
	temp_buff[Fixed_len+13+ClientID_len] = Username_len%256; 				//用户名长度低字节
	memcpy(&temp_buff[Fixed_len+14+ClientID_len],Username,Username_len);    //复制过来用户名字串	
	/*      密码        */
	temp_buff[Fixed_len+14+ClientID_len+Username_len] = Passward_len/256;	//密码长度高字节
	temp_buff[Fixed_len+15+ClientID_len+Username_len] = Passward_len%256;	//密码长度低字节
	memcpy(&temp_buff[Fixed_len+16+ClientID_len+Username_len],Passward,Passward_len); //复制过来密码字串

}
/*----------------------------------------------------------*/
/*函数名：SUBSCRIBE订阅topic报文                            */
/*参  数：QoS：订阅等级                                     */
/*参  数：topic_name：订阅topic报文名称                     */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_Subscribe(char *topic_name, int QoS)
{	
	Fixed_len = 2;                              		   //SUBSCRIBE报文中，固定报头长度=2
	Variable_len = 2;                          			   //SUBSCRIBE报文中，可变报头长度=2	
	Payload_len = 2 + strlen(topic_name) + 1;   		   //计算有效负荷长度 = 2字节(topic_name长度)+ topic_name字符串的长度 + 1字节服务等级
	
	temp_buff[0] = 0x82;                                   //第1个字节 ：固定0x82                      
	temp_buff[1] = Variable_len + Payload_len;             //第2个字节 ：可变报头+有效负荷的长度	
	temp_buff[2] = 0x00;                                   //第3个字节 ：报文标识符高字节，固定使用0x00
	temp_buff[3] = 0x01;		                           //第4个字节 ：报文标识符低字节，固定使用0x01
	temp_buff[4] = strlen(topic_name)/256;                 //第5个字节 ：topic_name长度高字节
	temp_buff[5] = strlen(topic_name)%256;		           //第6个字节 ：topic_name长度低字节
	memcpy(&temp_buff[6], topic_name, strlen(topic_name)); //第7个字节开始 ：复制过来topic_name字串		
	temp_buff[6 + strlen(topic_name)] = QoS;               //最后1个字节：订阅等级
	

}

/*----------------------------------------------------------*/
/*函数名：等级0 发布消息报文                                  */
/*参  数：topic_name：topic名称                              */
/*参  数：data：数据                                         */ 
/*参  数：data_len：数据长度                                 */
/*返回值：无                                                 */
/*----------------------------------------------------------*/
void MQTT_PublishQs0(char *topic, char *data, int data_len)
{	
	int temp,Remaining_len;
	
	Fixed_len = 1;                              //固定报头长度暂时先等于：1字节
	Variable_len = 2 + strlen(topic);           //可变报头长度：2字节(topic长度)+ topic字符串的长度
	Payload_len = data_len;                     //有效负荷长度：就是data_len
	Remaining_len = Variable_len + Payload_len; //剩余长度=可变报头长度+负载长度
	
	temp_buff[0] = 0x30;                      	//固定报头第1个字节 ：固定0x30   	
	do{                                         //循环处理固定报头中的剩余长度字节，字节量根据剩余字节的真实长度变化
		temp = Remaining_len%128;           	//剩余长度取余128
		Remaining_len = Remaining_len/128;      //剩余长度取整128
		if(Remaining_len>0)               	
			temp |= 0x80;                    	//按协议要求位7置位          
		temp_buff[Fixed_len] = temp;            //剩余长度字节记录一个数据
		Fixed_len++;	                     	//固定报头总长度+1    
	}while(Remaining_len>0);                    //如果Remaining_len>0的话，再次进入循环
		             
	temp_buff[Fixed_len+0] = strlen(topic)/256;                       //可变报头第1个字节     ：topic长度高字节
	temp_buff[Fixed_len+1] = strlen(topic)%256;		                  //可变报头第2个字节     ：topic长度低字节
	memcpy(&temp_buff[Fixed_len+2], topic,strlen(topic));             //可变报头第3个字节开始 ：拷贝topic字符串	
	memcpy(&temp_buff[Fixed_len + 2 + strlen(topic)], data, data_len);//有效负荷：拷贝data数据
	
	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);//加入发送数据缓冲区	
}