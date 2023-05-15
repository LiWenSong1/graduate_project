/**************************************************************************************************
  Filename:       SampleApp.c
  Revised:        $Date: 2009-03-18 15:56:27 -0700 (Wed, 18 Mar 2009) $
  Revision:       $Revision: 19453 $

  Description:    Sample Application (no Profile).


  Copyright 2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED �AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/*********************************************************************
  This application isn't intended to do anything useful, it is
  intended to be a simple example of an application's structure.

  This application sends it's messages either as broadcast or
  broadcast filtered group messages.  The other (more normal)
  message addressing is unicast.  Most of the other sample
  applications are written to support the unicast message model.

  Key control:
    SW1:  Sends a flash command to all devices in Group 1.
    SW2:  Adds/Removes (toggles) this device in and out
          of Group 1.  This will enable and disable the
          reception of the flash command.
*********************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "OSAL.h"
#include "ZGlobals.h"
#include "AF.h"
#include "aps_groups.h"
#include "ZDApp.h"


#include "SampleApp.h"
#include "SampleAppHw.h"

#include "OnBoard.h"

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "MT_UART.h"
#include "MT_APP.h"
#include "MT.h"

#include "DHT11.h"
#include <stdio.h>
#include <string.h>

/*********************************************************************
 * MACROS
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr)[0])
uint8 flat = 1;
uint8 con_succ=0;
//��Ҫ�޸ĵ�������4��
#define  devkey   "REWZhCwwBa=6WWaBam6onjn5=HM="   //onenetƽ̨��Ʒapikey
#define  devid    "1067058780"                      //onenetƽ̨�豸id
#define  LYSSID   "Redmi_K40"                      // �޸���·������SSId
#define  LYPASSWD "15876376419"                   // �޸���·����������

#define WIFIRESET       P0_6                            // P0.6�ڿ���reset
//WIFI��������
#define AT_C       "AT\r\n"
#define CWMODE_C   "AT+CWMODE=3\r\n"  //STA+APģʽ
//#define CIPSTART_C "AT+CIPSTART=\"TCP\",\"192.168.1.3\",8234\r\n" //����HTTP������192.168.1.4,8234  8234�Ƿ������Ķ˿ں� ---���ز���
#define CIPSTART_C "AT+CIPSTART=\"TCP\",\"183.230.40.39\",6002\r\n"   //�����Ʒ�����
#define CIPMODE_C  "AT+CIPMODE=1\r\n"   //͸��ģʽ
#define CIPSEND_C  "AT+CIPSEND\r\n"     //�������ݵ�ָ��

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// This list should be filled with Application specific Cluster IDs.
const cId_t SampleApp_ClusterList[SAMPLEAPP_MAX_CLUSTERS] =
{
  SAMPLEAPP_PERIODIC_CLUSTERID,
  SAMPLEAPP_FLASH_CLUSTERID
};

const SimpleDescriptionFormat_t SampleApp_SimpleDesc =
{
  SAMPLEAPP_ENDPOINT,              //  int Endpoint;
  SAMPLEAPP_PROFID,                //  uint16 AppProfId[2];
  SAMPLEAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  SAMPLEAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  SAMPLEAPP_FLAGS,                 //  int   AppFlags:4;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList,  //  uint8 *pAppInClusterList;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList   //  uint8 *pAppInClusterList;
};

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in SampleApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t SampleApp_epDesc;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
uint8 SampleApp_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // SampleApp_Init() is called.
devStates_t SampleApp_NwkState;

uint8 SampleApp_TransID;  // This is the unique message ID (counter)

afAddrType_t SampleApp_Periodic_DstAddr; //�㲥
afAddrType_t SampleApp_Flash_DstAddr;    //�鲥
afAddrType_t SampleApp_P2P_DstAddr;      //�㲥

aps_Group_t SampleApp_Group;
                        
unsigned char concmd[]={0x10,0x21,0x00,0x04,0x4D,0x51,0x54,0x54,0x04,0xC0,0x00,0x78,0x00,0x0A,0x31,0x30,0x37,0x39,0x39,0x36,0x38,0x39,0x34,0x37,0x00,0x06,0x35,0x39,0x30,0x39,0x34,0x35,0x00,0x01,0x32};
  
uint8 SampleAppPeriodicCounter = 0;
uint8 SampleAppFlashCounter = 0;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
void SampleApp_HandleKeys( uint8 shift, uint8 keys );
void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void SampleApp_SendPeriodicMessage( void );
void SampleApp_SendFlashMessage( uint16 flashTime );
void SampleApp_Send_P2P_Message(void);
uint16 ReadGasData(void);
void ReSetWifi(void);
void USART_Receive_messege(mtOSALSerialData_t *MSGpkt);
void publish_msg(afIncomingMSGPacket_t *pkt);
void sub_msg(void);
void send_onenet(afIncomingMSGPacket_t *pkt);
/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SampleApp_Init
 *
 * @brief   Initialization function for the Generic App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void SampleApp_Init( uint8 task_id )
{ 
  unsigned char tmp[10];
  SampleApp_TaskID = task_id;
  SampleApp_NwkState = DEV_INIT;
  SampleApp_TransID = 0;
  
  MT_UartInit();                  //���ڳ�ʼ��
  MT_UartRegisterTaskID(task_id); //ע�ᴮ������
   //HalUARTWrite(0,"success\n", 9);
   //HalUARTWrite(1,"success2\n", 9);
   tmp[0] = HAL_UART_DMA+0x30;
  tmp[1] = HAL_UART_ISR+0x30;
  tmp[2] = HAL_UART_USB+0x30;
    //HalUARTWrite(0, tmp, 6);
   P0SEL &= 0x7f;           //P0_7���ó�ͨ��io ��ʪ��
   ReSetWifi();
   Delay_ms(1000);                //�ȴ�ģ������
  HalUARTWrite(1,"AT\r\n",strlen("AT\r\n")); //����AT���WiFiģ��
  flat = 1;
 
  // Device hardware initialization can be added here or in main() (Zmain.c).
  // If the hardware is application specific - add it here.
  // If the hardware is other parts of the device add it in main().

 #if defined ( BUILD_ALL_DEVICES )
  // The "Demo" target is setup to have BUILD_ALL_DEVICES and HOLD_AUTO_START
  // We are looking at a jumper (defined in SampleAppHw.c) to be jumpered
  // together - if they are - we will start up a coordinator. Otherwise,
  // the device will start as a router.
  if ( readCoordinatorJumper() )
    zgDeviceLogicalType = ZG_DEVICETYPE_COORDINATOR;
  else
    zgDeviceLogicalType = ZG_DEVICETYPE_ROUTER;
#endif // BUILD_ALL_DEVICES

#if defined ( HOLD_AUTO_START )
  // HOLD_AUTO_START is a compile option that will surpress ZDApp
  //  from starting the device and wait for the application to
  //  start the device.
  ZDOInitDevice(0);
#endif

  // Setup for the periodic message's destination address
  // Broadcast to everyone
  SampleApp_Periodic_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
  SampleApp_Periodic_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_Periodic_DstAddr.addr.shortAddr = 0xFFFF;

  // Setup for the flash command's destination address - Group 1
  SampleApp_Flash_DstAddr.addrMode = (afAddrMode_t)afAddrGroup;
  SampleApp_Flash_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_Flash_DstAddr.addr.shortAddr = SAMPLEAPP_FLASH_GROUP;
  
  SampleApp_P2P_DstAddr.addrMode = (afAddrMode_t)Addr16Bit; //�㲥 
  SampleApp_P2P_DstAddr.endPoint = SAMPLEAPP_ENDPOINT; 
  SampleApp_P2P_DstAddr.addr.shortAddr = 0x0000;            //����Э����

  // Fill out the endpoint description.
  SampleApp_epDesc.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_epDesc.task_id = &SampleApp_TaskID;
  SampleApp_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&SampleApp_SimpleDesc;
  SampleApp_epDesc.latencyReq = noLatencyReqs;

  // Register the endpoint description with the AF
  afRegister( &SampleApp_epDesc );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( SampleApp_TaskID );

  // By default, all devices start out in Group 1
  SampleApp_Group.ID = 0x0001;
  osal_memcpy( SampleApp_Group.name, "Group 1", 7 );
  aps_AddGroup( SAMPLEAPP_ENDPOINT, &SampleApp_Group );

#if defined ( LCD_SUPPORTED )
  HalLcdWriteString( "SampleApp", HAL_LCD_LINE_1 );
#endif
}

/*********************************************************************
 * @fn      SampleApp_ProcessEvent
 *
 * @brief   Generic Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
uint16 SampleApp_ProcessEvent( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        // Received when a key is pressed
        case KEY_CHANGE:
          SampleApp_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        // Received when a messages is received (OTA) for this endpoint
        case AF_INCOMING_MSG_CMD:
          SampleApp_MessageMSGCB( MSGpkt );
          break;
        case CMD_SERIAL_MSG:
              USART_Receive_messege((mtOSALSerialData_t *)MSGpkt);
               //HalUARTWrite(1,"ATOK",strlen("ATOK"));
              break;
        

        // Received whenever the device changes state in the network
        case ZDO_STATE_CHANGE:
          SampleApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if ( //(SampleApp_NwkState == DEV_ZB_COORD) ||
                 (SampleApp_NwkState == DEV_ROUTER)
              || (SampleApp_NwkState == DEV_END_DEVICE) )
          {
            // Start sending the periodic message in a regular interval.
            osal_start_timerEx( SampleApp_TaskID,
                              SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
                              SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT );
          }
          else
          {
            // Device is no longer in the network
          }
          break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next - if one is available
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // Send a message out - This event is generated by a timer
  //  (setup in SampleApp_Init()).
  if ( events & SAMPLEAPP_SEND_PERIODIC_MSG_EVT )
  {
    // Send the periodic message
    //SampleApp_SendPeriodicMessage();
    SampleApp_Send_P2P_Message();

    // Setup to send message again in normal period (+ a little jitter)
    osal_start_timerEx( SampleApp_TaskID, SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
        (SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT+(osal_rand() & 0x00FF)));

    // return unprocessed events
    return (events ^ SAMPLEAPP_SEND_PERIODIC_MSG_EVT);
  }

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * Event Generation Functions
 */
/*********************************************************************
 * @fn      SampleApp_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
void SampleApp_HandleKeys( uint8 shift, uint8 keys )
{
  (void)shift;  // Intentionally unreferenced parameter
  
  if ( keys & HAL_KEY_SW_1 )
  {
    /* This key sends the Flash Command is sent to Group 1.
     * This device will not receive the Flash Command from this
     * device (even if it belongs to group 1).
     */
    SampleApp_SendFlashMessage( SAMPLEAPP_FLASH_DURATION );
  }

  if ( keys & HAL_KEY_SW_2 )
  {
    /* The Flashr Command is sent to Group 1.
     * This key toggles this device in and out of group 1.
     * If this device doesn't belong to group 1, this application
     * will not receive the Flash command sent to group 1.
     */
    aps_Group_t *grp;
    grp = aps_FindGroup( SAMPLEAPP_ENDPOINT, SAMPLEAPP_FLASH_GROUP );
    if ( grp )
    {
      // Remove from the group
      aps_RemoveGroup( SAMPLEAPP_ENDPOINT, SAMPLEAPP_FLASH_GROUP );
    }
    else
    {
      // Add to the flash group
      aps_AddGroup( SAMPLEAPP_ENDPOINT, &SampleApp_Group );
    }
  }
}
//�ϴ����ݵ�
void send_onenet(afIncomingMSGPacket_t *pkt){
  unsigned char send_buf[64];
   memset(send_buf,0,64);
  //���
    send_buf[0]=0x30;
    send_buf[1]=pkt->cmd.DataLength+6;
    send_buf[2]=0x00;
    send_buf[3]=0x03;
    send_buf[4]=0x24; 
    send_buf[5]=0x64;
    send_buf[6]=0x70;
    send_buf[7]=0x03;
    send_buf[8]=0x00;
    send_buf[9]=pkt->cmd.DataLength-2;
    strcpy(&send_buf[10],pkt->cmd.Data);
    //����
    HalUARTWrite(1, send_buf, pkt->cmd.DataLength+8);

}


//��������
void publish_msg(afIncomingMSGPacket_t *pkt){
  unsigned char pub_buf[64];
   memset(pub_buf,0,64);
  //���
    pub_buf[0]=0x30;
    pub_buf[1]=pkt->cmd.DataLength+5;
    pub_buf[2]=0x00;
    pub_buf[3]=0x03;
    pub_buf[4]=0x6D;   //'m'
    pub_buf[5]=0x73;  //'s'
    pub_buf[6]=0x67;   //'g'
    strcpy(&pub_buf[7],pkt->cmd.Data);
    HalUARTWrite(1, pub_buf, pkt->cmd.DataLength+7);
}

//��������
void sub_msg(void){
  unsigned char sub_buf[64];
   memset(sub_buf,0,64);
  //���
    sub_buf[0]=0x82;
    sub_buf[1]=0x08;  //����ʣ�೤��
    sub_buf[2]=0x00;
    sub_buf[3]=0x02;
    sub_buf[4]=0x00;   
    sub_buf[5]=0x03;  
    sub_buf[6]=0x6D;        //'m'
    sub_buf[7]=0x73;     //'s'
    sub_buf[8]=0x67;    //'g'
    sub_buf[9]=0x00;
    HalUARTWrite(1, sub_buf, 10);
}


/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      SampleApp_MessageMSGCB
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *
 * @param   none
 *
 * @return  none
 */
void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  uint16 flashTime;
  char HttpData[170];
  char send_data[70];
  unsigned char data[64];
  unsigned char heart[2]={0xc0,0x00};
  switch ( pkt->clusterId )
  {
    case SAMPLEAPP_P2P_CLUSTERID:

      //HalUARTWrite(0, "ZIGBEE-WIFI OK1\r\n\0", 18);
      //sprintf(send_data,"%s\0",pkt->cmd.Data);
      //strcat(pkt->cmd.Data,'\0');
      //HalUARTWrite(0,send_data,pkt->cmd.DataLength+1);
      strcpy(send_data,pkt->cmd.Data);
      send_data[pkt->cmd.DataLength-3]=',';
      send_data[pkt->cmd.DataLength-2]='}';
      send_data[pkt->cmd.DataLength-1]='\r';
      send_data[pkt->cmd.DataLength]='\n';
      HalUARTWrite(0,send_data,pkt->cmd.DataLength+1);
      HalUARTWrite(0, "\0", 1);
      if(flat==7)
      {   
        //��������
        HalUARTWrite(1,heart,2);
        send_onenet(pkt);
        publish_msg(pkt);
       
      }
       else
      {
        //SystemReset();
      }       // �س�����
      
      break;    
    case SAMPLEAPP_PERIODIC_CLUSTERID:
     
      break;

    case SAMPLEAPP_FLASH_CLUSTERID:
      flashTime = BUILD_UINT16(pkt->cmd.Data[1], pkt->cmd.Data[2] );
      HalLedBlink( HAL_LED_4, 4, 50, (flashTime / 4) );
      break;
  }
}

/*********************************************************************
 * @fn      SampleApp_SendPeriodicMessage
 *
 * @brief   Send the periodic message.
 *
 * @param   none
 *
 * @return  none
 */
void SampleApp_SendPeriodicMessage( void )
{
 
  if ( AF_DataRequest( &SampleApp_Periodic_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_PERIODIC_CLUSTERID,
                       1,
                       (uint8*)&SampleAppPeriodicCounter,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
  }
  else
  {
    // Error occurred in request to send.
  }
}

/*********************************************************************
 * @fn      SampleApp_SendFlashMessage
 *
 * @brief   Send the flash message to group 1.
 *
 * @param   flashTime - in milliseconds
 *
 * @return  none
 */
void SampleApp_SendFlashMessage( uint16 flashTime )
{
  uint8 buffer[3];
  buffer[0] = (uint8)(SampleAppFlashCounter++);
  buffer[1] = LO_UINT16( flashTime );
  buffer[2] = HI_UINT16( flashTime );

  if ( AF_DataRequest( &SampleApp_Flash_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_FLASH_CLUSTERID,
                       3,
                       buffer,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
  }
  else
  {
    // Error occurred in request to send.
  }
}

//��ȡMQ2����������
unsigned char TxBuf[30];
uint16 GasData;
uint16 ReadGasData(void)
{
  uint16 reading = 0;
  
  /* Enable channel */
  ADCCFG |= 0x40;
  
  /* writing to this register starts the extra conversion */
  ADCCON3 = 0x86;// AVDD5 ����  00�� 64 ��ȡ��(7 λENOB)  0110�� AIN6
  
  /* Wait for the conversion to be done */
  while (!(ADCCON1 & 0x80));
  
  /* Disable channel after done conversion */
  ADCCFG &= (0x40 ^ 0xFF); //��λ�����1010^1111=0101�������ƣ�
  
  /* Read the result */
  reading = ADCL;
  reading |= (int16) (ADCH << 8); 
  
  reading >>= 8;
  
  return (reading);
}
//��λWiFiģ�麯��
void ReSetWifi(void)
{
  P0DIR |= 0x40;                  //P0.6����Ϊ���
  WIFIRESET = 0;                  //�͵�ƽ��λ---------------------
  Delay_ms(500);
  WIFIRESET = 1;                  //�ߵ�ƽ����------------
   Delay_ms(500);
}

//���ڽ������ݴ�������
void USART_Receive_messege(mtOSALSerialData_t *MSGpkt)
{
  
   //HalUARTWrite(0, "ZIGBEE-WIFI OK8\r\n", 17);
    char nn[100];  //mqtt���ӻ���
   // unsigned char recv_buf[64];
    mtOSALSerialData_t *p =MSGpkt;
   // HalUARTWrite(1,(unsigned char*)(&p->msg[1]), p->msg[0]);
    if(flat==7) //�յ�WIFI��ƽ̨��������

      if(con_succ==0){
        if(p->msg[1]==0x20 && p->msg[2]==0x02) 
        {
            con_succ=1;
            sub_msg();
        }
        else 
          HalUARTWrite(1,concmd,35);
      
      }
      else{
        if(p->msg[0]>7){
          if(p->msg[1]==0x30 && p->msg[5]=='m' && p->msg[6]=='s' && p->msg[7]=='g')
            {
              
               HalUARTWrite(0,(unsigned char*)(&p->msg[8]), p->msg[0]-7);
                HalUARTWrite(0,"\0",1);
            }
        
        
        }
          
      
      }
      
      
      //HalUARTWrite(0,(uint8 *)(&p->msg[1]), p->msg[0]);
    if(flat==1) //��AT����
    {
      // HalUARTWrite(1,(unsigned char*)(&p->msg[1]), p->msg[0]);
       //HalUARTWrite(0,(uint8 *)(&p->msg[1]), p->msg[0]);
       if(strstr((char const *)(&p->msg[1]),"OK"))
       {
          HalUARTWrite(1,CWMODE_C,strlen(CWMODE_C));
          
          flat=2;
          Delay_ms(500);
       }
        else if(strstr((char const *)(&p->msg[1]),"ERROR"))  //ʧ�ܾ�����
       {
          ReSetWifi();//����WiFi
          Delay_ms(1000);
          HalUARTWrite(1,"AT\r\n",4);
          flat=1;    
       }
    }
     else if(flat==2) //����WiFi
    {
       if(strstr((char const *)(&p->msg[1]),"OK"))
       {
          memset(nn,0,100);
          sprintf(nn,"AT+CWJAP=\"%s\",\"%s\"\r\n",LYSSID,LYPASSWD);
          HalUARTWrite(1,(unsigned char*)nn, strlen(nn));//���ӱ���WiFi
         
          flat=3;
          Delay_ms(1000);Delay_ms(1000);
       }
    }
     else if(flat==3) //���ӷ�����
    {
       if(strstr((char const *)(&p->msg[1]),"OK"))
       {             
         HalUARTWrite(1,CIPSTART_C, strlen(CIPSTART_C));//���ӷ�����
         flat=4;
        
         Delay_ms(1000);Delay_ms(1000);Delay_ms(1000);
       }
    }
    else if(flat==4)  //����͸��ģʽ
    {
       if(strstr((char const *)(&p->msg[1]),"OK"))
       {
          HalUARTWrite(1,CIPMODE_C, strlen(CIPMODE_C));//����͸��
          flat=5;
         
          Delay_ms(500);
       }
       else 
       {
          ReSetWifi();//����WiFi
          Delay_ms(1000);
          HalUARTWrite(1,"AT\r\n",4);
          flat=1;    
         // HalUARTWrite(0, "ZIGBEE-WIFI OK6\r\n", 17);
       }
    }
     else if(flat==5) //�������ݵ�����
    {
       if(strstr((char const *)(&p->msg[1]),"OK"))
       {
          HalUARTWrite(1,CIPSEND_C, strlen(CIPSEND_C));//����
          flat=6;
          Delay_ms(500);
         
       }
       else 
       {
          ReSetWifi();//����WiFi
          Delay_ms(1000);
          HalUARTWrite(1,"AT\r\n",4);
          flat=1;    
       }
    }
    else if(flat==6)  //׼���������ݵ�������
    {
       if(strstr((char const *)(&p->msg[1]),">"))
       {           
        
          
            flat=7;
            HalUARTWrite(1,concmd,35);
         
            
            //HalUARTWrite(0,concmd,strlen(concmd));
       }
       else 
       {
          ReSetWifi();//����WiFi
          Delay_ms(1000);
          HalUARTWrite(1,"AT\r\n",4);
          flat=1;    
       }
    }




}


/*********************************************************************
 * @fn      SampleApp_Send_P2P_Message
 *
 * @brief   point to point.
 *
 * @param   none
 *
 * @return  none
 */
void SampleApp_Send_P2P_Message( void )
{
  GasData = ReadGasData();  //��ȡ���������������ϵ�adת��ֵ����û�л�����ܱ�ʾ����Ũ�ȵ�ֵ
    //��ʾ���ʹ��2530оƬ��AD���ܣ��������������и���
    
    //��ȡ������ֵת�����ַ����������ں������
  /*
    TxBuf[0] = GasData / 100 + '0';
    TxBuf[1] = GasData / 10%10 + '0';
    TxBuf[2] = GasData % 10 + '0';
    TxBuf[3] = '\n';
    TxBuf[4] = 0;
    */
  DHT11();
  sprintf((char *)TxBuf,"{\"Air\":%d,\"temp\":%d,\"humid\":%d}\r\n",GasData,ucharT_data_H,ucharRH_data_H);
  //sprintf((char *)TxBuf,",;Air,%d,temp,%d,humid,%d\r\n",GasData,ucharT_data_H,ucharRH_data_H);
     //HalUARTWrite(0,TxBuf, strlen(TxBuf));                 //������ʾ

     
     
  if ( AF_DataRequest( &SampleApp_P2P_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_P2P_CLUSTERID,
                      strlen(TxBuf),
                      TxBuf,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
  }
  else
  {
     HalUARTWrite(0,"error", 5); 
    // Error occurred in request to send.
  }
}
/*********************************************************************
*********************************************************************/