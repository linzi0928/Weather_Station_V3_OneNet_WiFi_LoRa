#ifndef __HEXSTRING_H
#define __HEXSTRING_H

#include "stm32l0xx.h"


/**************************************************************/
/**************         hexstring½Ó¿Ú        ******************/
/**************************************************************/
unsigned char *StringToByte(char *pString,unsigned char *pByte);
char *ByteToString(unsigned char *pByte,char *pString,unsigned int len);
char *DecToString(unsigned int Dec, char *pString);
#endif
