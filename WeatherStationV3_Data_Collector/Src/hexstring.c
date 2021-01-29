#include "hexstring.h"
#include <string.h>
#include "math.h"
#include <stdlib.h>
/***************************************************************
* ��������: *StringToByte
* ˵    �����ֽ�ת�ַ�
* ��    ��: char *pString����Ҫת�����ַ���
*			unsigned char *pByte��ת��֮����ֽڴ�
* �� �� ֵ: ת��֮����ֽڴ�
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
* ��������: *ByteToString
* ˵    �����ֽ�ת�ַ���
* ��    ��: unsigned char *pByte����Ҫת�����ֽڴ�
*			char *pString��ת��֮����ַ���
*			unsigned int len��len��ת��֮ǰstring�ĳ���
* �� �� ֵ: ת��֮���ַ���
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
* ��������: *DecToString
* ˵    ����ʮ������ת�ַ�����ʽ
* ��    ��: unsigned int Dec����Ҫת����ʮ��������
*			char *pString��ת��֮����ַ���
* �� �� ֵ: ת��֮���ַ���
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
