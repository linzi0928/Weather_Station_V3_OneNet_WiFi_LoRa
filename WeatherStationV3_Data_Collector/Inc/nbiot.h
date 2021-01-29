#ifndef __NBIoT_H_
#define __NBIoT_H_

#include "stm32l0xx_hal.h"
//#include "iwdg.h"
//#include "cmsis_os.h"

/***************************************************************
* 函数名称: NB_CONNRCT_STATUS
* 说    明: 定义NB连接状态的结构体，供调用查看
* 参    数: 无
* 返 回 值: 无
***************************************************************/
typedef struct
{
	uint8_t		NB_ATTRACT_NET;				//NB附着网络
}NB_CONNRCT_STATUS;

//-----------------------宏定义-------------------------------// 
#define DefaultTimeout	1000				//默认超时时间1s


/**************************************************************/
/**************         NB接口        ***********************/
/**************************************************************/
int NB_SendCmd(uint8_t *cmd,uint8_t *result,uint32_t timeOut,uint8_t isPrintf);
int NB_SendCmd_Mul(uint8_t *cmd,uint8_t *result1,uint8_t *result2,uint32_t timeOut,uint8_t isPrintf);
void NB_InitConnect(void);
void NB_SendMsgToServer(uint8_t *msg);
void NB_SetCDPServer(uint8_t *ncdpIP,uint8_t *port);
uint8_t NB_ReceiveMsg(uint8_t *msgReceive);
extern uint8_t sleep_counter;
extern void Enter_STOP_mode(void);
#endif
