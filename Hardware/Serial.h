#ifndef __SERIAL_H
#define __SERIAL_H
#define false 0
#define true 1
	
#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
	
//定义数组长度
#define GPS_Buffer_Length 80
#define UTCTime_Length 11
#define latitude_Length 11
#define N_S_Length 2
#define longitude_Length 12
#define E_W_Length 2 

#include <stdio.h>
#include "string.h"

extern char Serial_RxPacket1[];
extern uint8_t Serial_RxFlag1;
extern char Serial_RxPacket2[];
extern uint8_t Serial_RxFlag2;
extern char Serial_RxPacket3;
extern uint8_t Serial_RxFlag3;


void Serial_Init(void);
void Serial_SendByte(USART_TypeDef*u, uint8_t Byte);
void Serial_SendArray(USART_TypeDef* u, uint8_t *Array, uint16_t Length);
void Serial_SendString(USART_TypeDef* u, char *String);
void Serial_SendNumber(USART_TypeDef* u, uint32_t Number, uint8_t Length);
void Serial_Printf(USART_TypeDef* u, char *format, ...);


extern char  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	

typedef struct SaveData 
{
	char GPS_Buffer[GPS_Buffer_Length];
	char isGetData;		//是否获取到GPS数据
	char isParseData;	//是否解析完成
	char UTCTime[UTCTime_Length];		//UTC时间
	char latitude[latitude_Length];		//纬度
	char N_S[N_S_Length];		//N/S
	char longitude[longitude_Length];		//经度
	char E_W[E_W_Length];		//E/W
	char isUsefull;		//定位信息是否有效
} _SaveData;

void uart_init(u32 bound);
extern char rxdatabufer;
extern u16 point1;
extern _SaveData Save_Data;

void CLR_Buf(void);
u8 Hand(char *a);
void clrStruct(void);

#endif
