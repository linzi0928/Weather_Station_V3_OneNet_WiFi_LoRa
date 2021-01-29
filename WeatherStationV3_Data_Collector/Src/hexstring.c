#include "hexstring.h"
#include <string.h>
#include "math.h"
#include <stdlib.h>
/***************************************************************
* 函数名称: *StringToByte
* 说    明：字节转字符
* 参    数: char *pString，需要转换的字符串
*			unsigned char *pByte，转换之后的字节串
* 返 回 值: 转换之后的字节串
***************************************************************/
unsigned char *StringToByte(char *pString, unsigned char *pByte)
{
	unsigned int i, len = strlen(pString) / 2;
	char Hex[3] = { 0 };

	for (i = 0; i < len; i++)
	{
		memcpy(Hex, pString + 2 * i, 2);
		pByte[i] = strtol(Hex, NULL, 16);
	}
	return pByte;
}

/***************************************************************
* 函数名称: *ByteToString
* 说    明：字节转字符串
* 参    数: unsigned char *pByte，需要转换的字节串
*			char *pString，转换之后的字符串
*			unsigned int len，len是转换之前string的长度
* 返 回 值: 转换之后字符串
***************************************************************/
char *ByteToString(unsigned char *pByte, char *pString, unsigned int len)
{
	unsigned int i;
	char a, b;
	for (i = 0; i<len; i++)
	{
		a = *(pByte + i) / 16;
		b = *(pByte + i) % 16;
		if (a <= 9 && b <= 9)
		{
			*(pString + i * 2) = a + '0';
			*(pString + i * 2 + 1) = b + '0';
		}
		else if (a <= 9 && b>9)
		{
			*(pString + i * 2) = a + '0';
			*(pString + i * 2 + 1) = b - 10 + 'A';
		}
		else if (a>9 && b <= 9)
		{
			*(pString + i * 2) = a - 10 + 'A';
			*(pString + i * 2 + 1) = b + '0';
		}
		else
		{
			*(pString + i * 2) = a - 10 + 'A';
			*(pString + i * 2 + 1) = b - 10 + 'A';
		}
	}
	*(pString + i * 2) = '\0';
	return pString;
}

/***************************************************************
* 函数名称: *DecToString
* 说    明：十进制数转字符串形式
* 参    数: unsigned int Dec，需要转换的十进制数据
*			char *pString，转换之后的字符串
* 返 回 值: 转换之后字符串
***************************************************************/
char *DecToString(unsigned int Dec, char *pString)
{
	unsigned char i = 0, j = 0;
	unsigned int Num;
	Num = Dec;
	while (Num >= 10)
	{
		Num /= 10;
		i++;
	}
	i++;
	while (i)
	{
		*(pString + j) = Dec / pow(10, i - 1) + '0';
		Dec %= (uint16_t)pow(10, i - 1);
		i--;
		j++;
	}
	*(pString + j) = '\0';
	return pString;
}
