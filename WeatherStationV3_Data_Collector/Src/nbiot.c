#include "nbiot.h"
#include "usart.h"
#include "hexstring.h"
#include <stdlib.h>
#include <string.h>
char cmdSend[128];
extern int USART_RX_STA;
void osDelay(uint16_t time)
{    
   uint16_t i=0;  
   while(time--)
   {
      i=3450;  //�Լ�����  ����ʱ�䣺20141116
      while(i--) ;    
   }
}
/***************************************************************
* ��������: NB_SendCmd
* ˵    ��: NB��������
* ��    ��: uint8_t *cmd����Ҫ���͵�����
*			uint8_t *result��������õĽ��
*			uint32_t timeOut���ȴ����������ʱ��
*			uint8_t isPrintf���Ƿ��ӡLog
* �� �� ֵ: ret������ֵ
***************************************************************/
int NB_SendCmd(uint8_t *cmd,uint8_t *result,uint32_t timeOut,uint8_t isPrintf)
{
	int ret=0;
	memset(USART1_RX_BUF,0,strlen((const char *)USART1_RX_BUF));//�������
	//if(isPrintf)
		//printf("MCU-->>NB: %s\r\n",cmd);
	USART_RX_STA=0;
	HAL_UART_Transmit(&huart1, cmd, strlen((const char *)cmd),timeOut);
	HAL_Delay(500);
	while(1)
	{
//		if(USART_RX_STA&0x8000)
//		{
			if(strstr((const char *)USART1_RX_BUF,(const char *)result))
			{
				ret = 0;
				USART1_RX_LEN = 0;
				break;
			}
			else
			{
				HAL_UART_Transmit(&huart1, cmd, strlen((const char *)cmd),timeOut);
				USART1_RX_LEN = 0;
			//	USART_RX_STA=0;
				HAL_Delay(timeOut);
			}
			
		//}
	}
//	if(isPrintf)
	//	printf("NB-->>MCU: %s\r\n",USART1_RX_BUF);
	osDelay(timeOut);
	USART1_RX_LEN = 0;
	sleep_counter=0;
	return ret;
}

/***************************************************************
* ��������: NB_SendCmd_Mul
* ˵    ��: NB�������ݣ�����ֵ�ж��
* ��    ��: uint8_t *cmd����Ҫ���͵�����
*						uint8_t *result��������õĽ��1
*						uint8_t *result��������õĽ��2
*						uint32_t timeOut���ȴ����������ʱ��
*						uint8_t isPrintf���Ƿ��ӡLog
* �� �� ֵ: ret������ֵ
***************************************************************/
int NB_SendCmd_Mul(uint8_t *cmd,uint8_t *result1,uint8_t *result2,uint32_t timeOut,uint8_t isPrintf)
{
	int ret=0;
	memset(USART1_RX_BUF,0,strlen((const char *)USART1_RX_BUF));//�������
	//if(isPrintf)
		//printf("MCU-->>NB: %s\r\n",cmd);
	HAL_UART_Transmit(&huart1, cmd, strlen((const char *)cmd),timeOut);
	osDelay(100);
	while(1)
	{
		if(strstr((const char *)USART1_RX_BUF,(const char *)result1)
			||strstr((const char *)USART1_RX_BUF,(const char *)result2))
		{
			ret = 0;
			break;
		}
		else
		{
			HAL_UART_Transmit(&huart1, cmd, strlen((const char *)cmd),timeOut);
			osDelay(timeOut);
		}
	}
	//if(isPrintf)
		//printf("NB-->>MCU: %s\r\n",USART1_RX_BUF);
	osDelay(timeOut);
	USART1_RX_LEN = 0;
	return ret;
}

/***************************************************************
* ��������: NB_InitConnect
* ˵    ��: NB��ʼ����������
* ��    ��: ��
* �� �� ֵ: ��
***************************************************************/
void NB_InitConnect(void)
{
	NB_SendCmd((uint8_t *)"AT+CFUN?\r\n",(uint8_t *)"OK",DefaultTimeout,1);
HAL_IWDG_Refresh(&hiwdg);
	NB_SendCmd((uint8_t *)"AT+CGSN=1\r\n",(uint8_t *)"OK",DefaultTimeout,1);
HAL_IWDG_Refresh(&hiwdg);
	NB_SendCmd((uint8_t *)"AT+CIMI\r\n",(uint8_t *)"OK",DefaultTimeout,1);
HAL_IWDG_Refresh(&hiwdg);
	NB_SendCmd((uint8_t *)"AT+CGATT=1\r\n",(uint8_t *)"OK",DefaultTimeout,1);
HAL_IWDG_Refresh(&hiwdg);
	NB_SendCmd((uint8_t *)"AT+CGATT?\r\n",(uint8_t *)"+CGATT:1",DefaultTimeout,1);
HAL_IWDG_Refresh(&hiwdg);
	NB_SendCmd((uint8_t *)"AT+CSQ\r\n",(uint8_t *)"OK",DefaultTimeout,1);
HAL_IWDG_Refresh(&hiwdg);
	NB_SendCmd((uint8_t *)"AT+CEREG=1\r\n",(uint8_t *)"OK",DefaultTimeout,1);
HAL_IWDG_Refresh(&hiwdg);
	NB_SendCmd((uint8_t *)"AT+CEREG?\r\n",(uint8_t *)"+CEREG:1,1",DefaultTimeout,1);
HAL_IWDG_Refresh(&hiwdg);
	NB_SendCmd((uint8_t *)"AT+NNMI=1\r\n",(uint8_t *)"OK",DefaultTimeout,1);
HAL_IWDG_Refresh(&hiwdg);
	
//	NB_SendCmd((uint8_t *)"AT+CFUN?\r\n",(uint8_t *)"OK",DefaultTimeout,1);
//	NB_SendCmd((uint8_t *)"AT+CGSN=1\r\n",(uint8_t *)"OK",DefaultTimeout,1);
//	NB_SendCmd((uint8_t *)"AT+CIMI\r\n",(uint8_t *)"ERROR",DefaultTimeout,1);
//	NB_SendCmd((uint8_t *)"AT+CGATT=1\r\n",(uint8_t *)"ERROR",DefaultTimeout,1);
//	NB_SendCmd((uint8_t *)"AT+CGATT?\r\n",(uint8_t *)"+CGATT:0",DefaultTimeout,1);
//	NB_SendCmd((uint8_t *)"AT+CSQ\r\n",(uint8_t *)"OK",DefaultTimeout,1);
//	NB_SendCmd((uint8_t *)"AT+CEREG=1\r\n",(uint8_t *)"ERROR",DefaultTimeout,1);
//	NB_SendCmd((uint8_t *)"AT+CEREG?\r\n",(uint8_t *)"+CEREG:0,0",DefaultTimeout,1);
//	NB_SendCmd((uint8_t *)"AT+NNMI=1\r\n",(uint8_t *)"ERROR",DefaultTimeout,1);
}

/***************************************************************
* ��������: NB_SendMsgToServer
* ˵    ��: NB������Ϣ��������
* ��    ��: *msg��Ҫ���͵����ݣ�String��ʽ��
* �� �� ֵ: ��
***************************************************************/
void NB_SendMsgToServer(uint8_t *msg)
{
	//����AT+NMGS=len,msg
	memset(cmdSend,0,sizeof(cmdSend));
	uint8_t len = 0;
	char msgLen[4]={0};
	len = strlen((const char *)msg)/2;
	DecToString(len, msgLen);
	strcat(cmdSend,"AT+NMGS=");
	strcat(cmdSend,msgLen);
	strcat(cmdSend,",");
	strcat(cmdSend,(const char *)msg);
	strcat(cmdSend,"\r\n");
	NB_SendCmd((uint8_t *)cmdSend,(uint8_t *)"OK",DefaultTimeout,1);	
}
/***************************************************************
* ��������: NB_SetCDPServer
* ˵    ��: NB����CDP������
* ��    ��: *ncdpIP��Ҫ���͵ķ�������ƽ̨����IP��ַ
*			*port��Ҫ���͵ķ�������ƽ̨���Ķ˿ں�
* �� �� ֵ: ��
***************************************************************/
void NB_SetCDPServer(uint8_t *ncdpIP,uint8_t *port)
{
	//����AT+NCDP=ncdpIP,port
	memset(cmdSend,0,sizeof(cmdSend));
	strcat(cmdSend,"AT+NCDP=");	
	strcat(cmdSend,(const char *)ncdpIP);
	strcat(cmdSend,",");
	strcat(cmdSend,(const char *)port);
	strcat(cmdSend,"\r\n");
	NB_SendCmd((uint8_t *)cmdSend,(uint8_t *)"OK",DefaultTimeout,1);
}

/***************************************************************
* ��������: NB_ReceiveMsg
* ˵    ��: NB�ӷ�������������
* ��    ��: msgReceive,NB���յ�������,�ò�������ʱ���ȷ�����ڴ�ռ�
* �� �� ֵ: SUCCESS�����ճɹ�
*			ERROR������ʧ�ܻ��޽�������
***************************************************************/
uint8_t NB_ReceiveMsg(uint8_t *msgReceive)
{
	//����+NNMI:2,4F4E
	char *pos1,*pos3;
	pos1 = strstr((char *)USART1_RX_BUF,"+NNMI:");
	if(pos1)
	{
		osDelay(500);
		char *pos2;
		uint8_t msgReceiveLen;
		msgReceiveLen = atoi(pos1+6);
		//printf("NET-->>NB: %s \r\n",USART1_RX_BUF);
		pos2 = strstr((char*)USART1_RX_BUF,",");
		memcpy(msgReceive,pos2+1,msgReceiveLen*2);	//pos2ָ����ǡ�,��  pos2+1ָ���������
		memset(USART1_RX_BUF,0,USART1_RX_LEN);
		USART1_RX_LEN = 0;
		return SUCCESS;
	}
	pos3 = strstr((char *)USART1_RX_BUF,"+NSONMI:0");
	if(pos3)
	{
		osDelay(500);
		char *recvBuf;
		char msg[20] = {0};
		//printf("NET-->>NB: %s \r\n",USART1_RX_BUF);
		strcat(msg,"AT+NSORF=0,");
		USART1_RX_LEN = atoi(pos3+10);				//��ȡҪ�������ݵĳ���
		//printf("USART1_RX_BUF_LEN: %d \r\n",USART1_RX_LEN);
		recvBuf =(char *)malloc(sizeof(uint8_t)*USART1_RX_LEN*2);
		memset(recvBuf,0,USART1_RX_LEN*2);
		DecToString(USART1_RX_LEN,&msg[strlen(msg)]);//ATָ��ƴ��
		strcat(msg,"\r\n");
		memset(USART1_RX_BUF,0,strlen((const char *)USART1_RX_BUF));
		USART1_RX_LEN = 0;
		HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen((const char *)msg),DefaultTimeout);//����ȡ����ָ��
		while(NULL!=strstr((const char *)USART1_RX_BUF,",5683,"));//���յ�����
		osDelay(200);								//Ϊ���ܹ���ȫ���յ���������
		//printf("NB DATA RECEIVE: %s \r\n",USART1_RX_BUF);
		recvBuf = strtok((char *)USART1_RX_BUF,",");//AT+NSORF=0
		recvBuf = strtok(NULL,",");//�е��̺߳�
//		printf("�̺߳�: %s \r\n",recvBuf);			//��ӡ���̺߳�����ǰ����AT+NSORF=0����ĳ��ȼ��س�����	
		recvBuf = strtok(NULL,",");//�е�IP��ַ
//		printf("IP��ַ: %s \r\n",recvBuf);
		recvBuf = strtok(NULL,",");//�е��˿ں�
//		printf("�˿ں�: %s \r\n",recvBuf);
		recvBuf = strtok(NULL,",");//�е�message����
//		printf("message����: %s \r\n",recvBuf);
		recvBuf = strtok(NULL,",");//����message
		//printf("NB Msg RECEIVE: %s \r\n",recvBuf);
		memcpy(msgReceive,recvBuf,USART1_RX_LEN*2);
		free(recvBuf);
		recvBuf = NULL;
		memset(USART1_RX_BUF,0,USART1_RX_LEN);
		USART1_RX_LEN = 0;
		return SUCCESS;
	}
	return ERROR;
}
