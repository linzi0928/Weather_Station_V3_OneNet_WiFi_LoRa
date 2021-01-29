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
      i=3450;  //自己定义  调试时间：20141116
      while(i--) ;    
   }
}
/***************************************************************
* 函数名称: NB_SendCmd
* 说    明: NB发送数据
* 参    数: uint8_t *cmd，需要发送的命令
*			uint8_t *result，期望获得的结果
*			uint32_t timeOut，等待期望结果的时间
*			uint8_t isPrintf，是否打印Log
* 返 回 值: ret，返回值
***************************************************************/
int NB_SendCmd(uint8_t *cmd,uint8_t *result,uint32_t timeOut,uint8_t isPrintf)
{
	int ret=0;
	memset(USART1_RX_BUF,0,strlen((const char *)USART1_RX_BUF));//清除缓存
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
* 函数名称: NB_SendCmd_Mul
* 说    明: NB发送数据，返回值有多个
* 参    数: uint8_t *cmd，需要发送的命令
*						uint8_t *result，期望获得的结果1
*						uint8_t *result，期望获得的结果2
*						uint32_t timeOut，等待期望结果的时间
*						uint8_t isPrintf，是否打印Log
* 返 回 值: ret，返回值
***************************************************************/
int NB_SendCmd_Mul(uint8_t *cmd,uint8_t *result1,uint8_t *result2,uint32_t timeOut,uint8_t isPrintf)
{
	int ret=0;
	memset(USART1_RX_BUF,0,strlen((const char *)USART1_RX_BUF));//清除缓存
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
* 函数名称: NB_InitConnect
* 说    明: NB初始化入网过程
* 参    数: 无
* 返 回 值: 无
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
* 函数名称: NB_SendMsgToServer
* 说    明: NB发送消息到服务器
* 参    数: *msg，要发送的数据（String形式）
* 返 回 值: 无
***************************************************************/
void NB_SendMsgToServer(uint8_t *msg)
{
	//例：AT+NMGS=len,msg
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
* 函数名称: NB_SetCDPServer
* 说    明: NB设置CDP服务器
* 参    数: *ncdpIP，要发送的服务器（平台）的IP地址
*			*port，要发送的服务器（平台）的端口号
* 返 回 值: 无
***************************************************************/
void NB_SetCDPServer(uint8_t *ncdpIP,uint8_t *port)
{
	//例：AT+NCDP=ncdpIP,port
	memset(cmdSend,0,sizeof(cmdSend));
	strcat(cmdSend,"AT+NCDP=");	
	strcat(cmdSend,(const char *)ncdpIP);
	strcat(cmdSend,",");
	strcat(cmdSend,(const char *)port);
	strcat(cmdSend,"\r\n");
	NB_SendCmd((uint8_t *)cmdSend,(uint8_t *)"OK",DefaultTimeout,1);
}

/***************************************************************
* 函数名称: NB_ReceiveMsg
* 说    明: NB从服务器接收数据
* 参    数: msgReceive,NB接收到的数据,该参数传入时请先分配好内存空间
* 返 回 值: SUCCESS，接收成功
*			ERROR，接收失败或无接收数据
***************************************************************/
uint8_t NB_ReceiveMsg(uint8_t *msgReceive)
{
	//例：+NNMI:2,4F4E
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
		memcpy(msgReceive,pos2+1,msgReceiveLen*2);	//pos2指向的是“,”  pos2+1指向的是数据
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
		USART1_RX_LEN = atoi(pos3+10);				//获取要接收数据的长度
		//printf("USART1_RX_BUF_LEN: %d \r\n",USART1_RX_LEN);
		recvBuf =(char *)malloc(sizeof(uint8_t)*USART1_RX_LEN*2);
		memset(recvBuf,0,USART1_RX_LEN*2);
		DecToString(USART1_RX_LEN,&msg[strlen(msg)]);//AT指令拼接
		strcat(msg,"\r\n");
		memset(USART1_RX_BUF,0,strlen((const char *)USART1_RX_BUF));
		USART1_RX_LEN = 0;
		HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen((const char *)msg),DefaultTimeout);//发送取数据指令
		while(NULL!=strstr((const char *)USART1_RX_BUF,",5683,"));//接收到数据
		osDelay(200);								//为了能够完全接收到所有数据
		//printf("NB DATA RECEIVE: %s \r\n",USART1_RX_BUF);
		recvBuf = strtok((char *)USART1_RX_BUF,",");//AT+NSORF=0
		recvBuf = strtok(NULL,",");//切掉线程号
//		printf("线程号: %s \r\n",recvBuf);			//打印的线程号数据前面是AT+NSORF=0后面的长度及回车换行	
		recvBuf = strtok(NULL,",");//切掉IP地址
//		printf("IP地址: %s \r\n",recvBuf);
		recvBuf = strtok(NULL,",");//切掉端口号
//		printf("端口号: %s \r\n",recvBuf);
		recvBuf = strtok(NULL,",");//切掉message长度
//		printf("message长度: %s \r\n",recvBuf);
		recvBuf = strtok(NULL,",");//返回message
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
