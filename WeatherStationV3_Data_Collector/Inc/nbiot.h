#ifndef __NBIoT_H_
#define __NBIoT_H_

#include "stm32l0xx_hal.h"
//#include "iwdg.h"
//#include "cmsis_os.h"

/***************************************************************
* ��������: NB_CONNRCT_STATUS
* ˵    ��: ����NB����״̬�Ľṹ�壬�����ò鿴
* ��    ��: ��
* �� �� ֵ: ��
***************************************************************/
typedef struct
{
	uint8_t		NB_ATTRACT_NET;				//NB��������
}NB_CONNRCT_STATUS;

//-----------------------�궨��-------------------------------// 
#define DefaultTimeout	1000				//Ĭ�ϳ�ʱʱ��1s


/**************************************************************/
/**************         NB�ӿ�        ***********************/
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
