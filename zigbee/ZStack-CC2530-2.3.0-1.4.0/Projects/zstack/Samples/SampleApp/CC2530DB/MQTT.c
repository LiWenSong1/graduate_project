#include "mqtt.h"         //������Ҫ��ͷ�ļ�
#include "string.h"       //������Ҫ��ͷ�ļ�
#include "stdio.h"        //������Ҫ��ͷ�ļ�
int   Fixed_len;                       			   //�̶���ͷ����
int   Variable_len;                     		   //�ɱ䱨ͷ����
int   Payload_len;                       		   //��Ч���ɳ���

char ClientID[128];                                //��ſͻ���ID�Ļ�����
int  ClientID_len;                                 //��ſͻ���ID�ĳ���

char Username[128];                                //����û����Ļ�����
int  Username_len;								   //����û����ĳ���

char Passward[128];                                //�������Ļ�����
int  Passward_len;								   //�������ĳ���

char ServerIP[128];                                //��ŷ�����IP��������
int  ServerPort;                                   //��ŷ������Ķ˿ں�

unsigned char  temp_buff[TBUFF_UNIT];			   //��ʱ������������������


/*----------------------------------------------------------*/
/*���������Ƴ�ʼ���������õ��ͻ���ID���û���������          */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void IoT_Parameter_Init(void)
{	
	memset(ClientID,0,128);                              //�ͻ���ID�Ļ�����ȫ������
	sprintf(ClientID,"%s",DEVICEID);                     //�����ͻ���ID�������뻺����
	ClientID_len = strlen(ClientID);                     //����ͻ���ID�ĳ���
	
	memset(Username,0,128);                              //�û����Ļ�����ȫ������
	sprintf(Username,"%s",PRODUCTID);                    //�����û����������뻺����
	Username_len = strlen(Username);                     //�����û����ĳ���
	
	memset(Passward,0,128);                              //�û����Ļ�����ȫ������
	sprintf(Passward,"%s",AUTHENTICATION);               //�������룬�����뻺����
	Passward_len = strlen(Passward);                     //��������ĳ���
	
	memset(ServerIP,0,128);  
	sprintf(ServerIP,"%s","183.230.40.39");              //��������������
	ServerPort = 6002;                                   //�������˿ں�6002
	/*
	u1_printf("�� �� ����%s:%d\r\n",ServerIP,ServerPort); //�������������Ϣ
	u1_printf("�ͻ���ID��%s\r\n",ClientID);               //�������������Ϣ
	u1_printf("�� �� ����%s\r\n",Username);               //�������������Ϣ
	u1_printf("��    �룺%s\r\n",Passward);               //�������������Ϣ
        */
}

/*----------------------------------------------------------*/
/*�����������ӷ���������                                    */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_ConectPack(void)
{	
	int temp,Remaining_len;
	
	Fixed_len = 1;                                                        //���ӱ����У��̶���ͷ������ʱ��=1
	Variable_len = 10;                                                    //���ӱ����У��ɱ䱨ͷ����=10
	Payload_len = 2 + ClientID_len + 2 + Username_len + 2 + Passward_len; //���ӱ����У����س���      
	Remaining_len = Variable_len + Payload_len;                           //ʣ�೤��=�ɱ䱨ͷ����+���س���
	
	temp_buff[0]=0x10;                         //�̶���ͷ��1���ֽ� ���̶�0x01		
	do{                                        //ѭ������̶���ͷ�е�ʣ�೤���ֽڣ��ֽ�������ʣ���ֽڵ���ʵ���ȱ仯
		temp = Remaining_len%128;              //ʣ�೤��ȡ��128
		Remaining_len = Remaining_len/128;     //ʣ�೤��ȡ��128
		if(Remaining_len>0)               	
			temp |= 0x80;                      //��Э��Ҫ��λ7��λ          
		temp_buff[Fixed_len] = temp;           //ʣ�೤���ֽڼ�¼һ������
		Fixed_len++;	                       //�̶���ͷ�ܳ���+1    
	}while(Remaining_len > 0);                 //���Remaining_len>0�Ļ����ٴν���ѭ��
	
	temp_buff[Fixed_len + 0] = 0x00;     //�ɱ䱨ͷ��1���ֽ� ���̶�0x00	            
	temp_buff[Fixed_len + 1] = 0x04;     //�ɱ䱨ͷ��2���ֽ� ���̶�0x04
	temp_buff[Fixed_len + 2] = 0x4D;	 //�ɱ䱨ͷ��3���ֽ� ���̶�0x4D
	temp_buff[Fixed_len + 3] = 0x51;	 //�ɱ䱨ͷ��4���ֽ� ���̶�0x51
	temp_buff[Fixed_len + 4] = 0x54;	 //�ɱ䱨ͷ��5���ֽ� ���̶�0x54
	temp_buff[Fixed_len + 5] = 0x54;     //�ɱ䱨ͷ��6���ֽ� ���̶�0x54
	temp_buff[Fixed_len + 6] = 0x04;	 //�ɱ䱨ͷ��7���ֽ� ���̶�0x04
	temp_buff[Fixed_len + 7] = 0xC2;	 //�ɱ䱨ͷ��8���ֽ� ��ʹ���û���������У�飬��ʹ���������������Ự
	temp_buff[Fixed_len + 8] = 0x00; 	 //�ɱ䱨ͷ��9���ֽ� ������ʱ����ֽ� 0x00
	temp_buff[Fixed_len + 9] = 0x64;	 //�ɱ䱨ͷ��10���ֽڣ�����ʱ����ֽ� 0x64   100s
	
	/*     CLIENT_ID      */
	temp_buff[Fixed_len+10] = ClientID_len/256;                			  	//�ͻ���ID���ȸ��ֽ�
	temp_buff[Fixed_len+11] = ClientID_len%256;               			  	//�ͻ���ID���ȵ��ֽ�
	memcpy(&temp_buff[Fixed_len+12],ClientID,ClientID_len);                 //���ƹ����ͻ���ID�ִ�	
	/*     �û���        */
	temp_buff[Fixed_len+12+ClientID_len] = Username_len/256; 				//�û������ȸ��ֽ�
	temp_buff[Fixed_len+13+ClientID_len] = Username_len%256; 				//�û������ȵ��ֽ�
	memcpy(&temp_buff[Fixed_len+14+ClientID_len],Username,Username_len);    //���ƹ����û����ִ�	
	/*      ����        */
	temp_buff[Fixed_len+14+ClientID_len+Username_len] = Passward_len/256;	//���볤�ȸ��ֽ�
	temp_buff[Fixed_len+15+ClientID_len+Username_len] = Passward_len%256;	//���볤�ȵ��ֽ�
	memcpy(&temp_buff[Fixed_len+16+ClientID_len+Username_len],Passward,Passward_len); //���ƹ��������ִ�

}
/*----------------------------------------------------------*/
/*��������SUBSCRIBE����topic����                            */
/*��  ����QoS�����ĵȼ�                                     */
/*��  ����topic_name������topic��������                     */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_Subscribe(char *topic_name, int QoS)
{	
	Fixed_len = 2;                              		   //SUBSCRIBE�����У��̶���ͷ����=2
	Variable_len = 2;                          			   //SUBSCRIBE�����У��ɱ䱨ͷ����=2	
	Payload_len = 2 + strlen(topic_name) + 1;   		   //������Ч���ɳ��� = 2�ֽ�(topic_name����)+ topic_name�ַ����ĳ��� + 1�ֽڷ���ȼ�
	
	temp_buff[0] = 0x82;                                   //��1���ֽ� ���̶�0x82                      
	temp_buff[1] = Variable_len + Payload_len;             //��2���ֽ� ���ɱ䱨ͷ+��Ч���ɵĳ���	
	temp_buff[2] = 0x00;                                   //��3���ֽ� �����ı�ʶ�����ֽڣ��̶�ʹ��0x00
	temp_buff[3] = 0x01;		                           //��4���ֽ� �����ı�ʶ�����ֽڣ��̶�ʹ��0x01
	temp_buff[4] = strlen(topic_name)/256;                 //��5���ֽ� ��topic_name���ȸ��ֽ�
	temp_buff[5] = strlen(topic_name)%256;		           //��6���ֽ� ��topic_name���ȵ��ֽ�
	memcpy(&temp_buff[6], topic_name, strlen(topic_name)); //��7���ֽڿ�ʼ �����ƹ���topic_name�ִ�		
	temp_buff[6 + strlen(topic_name)] = QoS;               //���1���ֽڣ����ĵȼ�
	

}

/*----------------------------------------------------------*/
/*���������ȼ�0 ������Ϣ����                                  */
/*��  ����topic_name��topic����                              */
/*��  ����data������                                         */ 
/*��  ����data_len�����ݳ���                                 */
/*����ֵ����                                                 */
/*----------------------------------------------------------*/
void MQTT_PublishQs0(char *topic, char *data, int data_len)
{	
	int temp,Remaining_len;
	
	Fixed_len = 1;                              //�̶���ͷ������ʱ�ȵ��ڣ�1�ֽ�
	Variable_len = 2 + strlen(topic);           //�ɱ䱨ͷ���ȣ�2�ֽ�(topic����)+ topic�ַ����ĳ���
	Payload_len = data_len;                     //��Ч���ɳ��ȣ�����data_len
	Remaining_len = Variable_len + Payload_len; //ʣ�೤��=�ɱ䱨ͷ����+���س���
	
	temp_buff[0] = 0x30;                      	//�̶���ͷ��1���ֽ� ���̶�0x30   	
	do{                                         //ѭ������̶���ͷ�е�ʣ�೤���ֽڣ��ֽ�������ʣ���ֽڵ���ʵ���ȱ仯
		temp = Remaining_len%128;           	//ʣ�೤��ȡ��128
		Remaining_len = Remaining_len/128;      //ʣ�೤��ȡ��128
		if(Remaining_len>0)               	
			temp |= 0x80;                    	//��Э��Ҫ��λ7��λ          
		temp_buff[Fixed_len] = temp;            //ʣ�೤���ֽڼ�¼һ������
		Fixed_len++;	                     	//�̶���ͷ�ܳ���+1    
	}while(Remaining_len>0);                    //���Remaining_len>0�Ļ����ٴν���ѭ��
		             
	temp_buff[Fixed_len+0] = strlen(topic)/256;                       //�ɱ䱨ͷ��1���ֽ�     ��topic���ȸ��ֽ�
	temp_buff[Fixed_len+1] = strlen(topic)%256;		                  //�ɱ䱨ͷ��2���ֽ�     ��topic���ȵ��ֽ�
	memcpy(&temp_buff[Fixed_len+2], topic,strlen(topic));             //�ɱ䱨ͷ��3���ֽڿ�ʼ ������topic�ַ���	
	memcpy(&temp_buff[Fixed_len + 2 + strlen(topic)], data, data_len);//��Ч���ɣ�����data����
	
	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);//���뷢�����ݻ�����	
}