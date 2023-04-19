#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "Serial.h"

char Serial_RxPacket1[200];				//"@MSG#@"
char Serial_RxPacket2[200];	//"@MSG#@"
char Serial_RxPacket3;	
uint8_t Serial_RxFlag1;
uint8_t Serial_RxFlag2;
uint8_t Serial_RxFlag3;
_SaveData Save_Data;
int ti = 0;

char USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记

char rxdatabufer;
u16 point1 = 0;

void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	USART_Init(USART2, &USART_InitStructure);
	USART_Init(USART3, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
	USART_Cmd(USART2, ENABLE);
	USART_Cmd(USART3, ENABLE);
}

void Serial_SendByte(USART_TypeDef*u, uint8_t Byte)
{
	USART_SendData(u, Byte);
	while (USART_GetFlagStatus(u, USART_FLAG_TXE) == RESET);
}

void Serial_SendArray(USART_TypeDef* u, uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(u, Array[i]);
	}
}

void Serial_SendString(USART_TypeDef* u, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(u,String[i]);
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void Serial_SendNumber(USART_TypeDef* u, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(u,Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

int fputc(int ch, FILE *f)
{
	Serial_SendByte(USART3, ch);
	return ch;
}

void Serial_Printf(USART_TypeDef* u, char *format, ...)
{
	char String[200];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(u, String);
}


void USART1_IRQHandler(void)
{
	
	static uint8_t RxState = 0;
	static uint8_t pRxPacket = 0;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		uint8_t RxData = USART_ReceiveData(USART1);
		
		if (RxState == 0)
		{
			if (RxData == '@' && Serial_RxFlag1 == 0)
			{
				RxState = 1;
				pRxPacket = 0;
			}
		}
		else if (RxState == 1)
		{
			if (RxData == '#')
			{
				RxState = 2;
			}
			else
			{
				Serial_RxPacket1[pRxPacket] = RxData;
				pRxPacket ++;
			}
		}
		else if (RxState == 2)
		{
			if (RxData == '@')
			{
				RxState = 0;
				Serial_RxPacket1[pRxPacket] = '\0';
				Serial_RxFlag1 = 1;
			}
		}
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}



void USART2_IRQHandler(void)                	// GPS
{
		//Serial_SendString(USART1, USART_RX_BUF);
	
		u8 Res;
		if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 
		{
			Res =USART_ReceiveData(USART2);//(USART1->DR);	//读取接收到的数据
			// Serial_SendByte(USART1, Res);
			if(Res == '$' && !Save_Data.isGetData)
			{
				
				point1 = 0;	
			}
			USART_RX_BUF[point1++] = Res;

			if(USART_RX_BUF[0] == '$' && USART_RX_BUF[4] == 'M' && USART_RX_BUF[5] == 'C')			//确定是否收到"GPRMC/GNRMC"这一帧数据
			{
				if(Res == '\n')									   
				{
					// Serial_SendByte(USART1,'3');
					memset(Save_Data.GPS_Buffer, 0, GPS_Buffer_Length);      //清空
					memcpy(Save_Data.GPS_Buffer, USART_RX_BUF, point1); 	//保存数据
					Save_Data.isGetData = true;
					point1 = 0;
					memset(USART_RX_BUF, 0, USART_REC_LEN);      //清空
					// Serial_SendString(USART1, Save_Data.GPS_Buffer);
				}	
						
			}
		
			if(point1 >= USART_REC_LEN)
			{
				point1 = USART_REC_LEN;
			}	
		}
	
}


//////////////////////////////////////////
///////////   Bluetooth /////////////////
////////////////////////////////////////

/*
void USART2_IRQHandler(void)
{
	// Serial_SendByte(USART1,USART_ReceiveData(USART2));
	
	static uint8_t RxState = 0;
	static uint8_t pRxPacket = 0;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
		uint8_t RxData = USART_ReceiveData(USART2);
		
		if (RxState == 0)
		{
			if (RxData == '@' && Serial_RxFlag2 == 0)
			{
				RxState = 1;
				pRxPacket = 0;
			}
		}
		else if (RxState == 1)
		{
			if (RxData == '#')
			{
				RxState = 2;
			}
			else
			{
				Serial_RxPacket2[pRxPacket] = RxData;
				pRxPacket ++;
			}
		}
		else if (RxState == 2)
		{
			if (RxData == '@')
			{
				RxState = 0;
				Serial_RxPacket2[pRxPacket] = '\0';
				Serial_RxFlag2 = 1;
			}
		}
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		
	}
}
*/

void USART3_IRQHandler(void) // BC26
{
		Serial_SendByte(USART1,USART_ReceiveData(USART3));
}
