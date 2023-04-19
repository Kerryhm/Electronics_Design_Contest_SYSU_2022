#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "LED.h"
#include "string.h"
#include "Beep.h"

void parseGpsBuffer(void);
int GPSempty = 1;
int Sound = 0;
int d = 1;



int main(void)
{
	BEEP_Config();
	Serial_Init();
	Delay_s(10);
	Serial_SendString(USART3, "AT+CGATT?"); Serial_SendString(USART3, "\r\n");
	Delay_ms(500);
	Serial_SendString(USART3, "AT+QMTCFG=\"ALIAUTH\",0,\"hveoaDatr9X\",\"test\",\"d722cf9f08ebd6da05915e74f13a17a2\"");Serial_SendString(USART3, "\r\n");
	Delay_ms(1000);
	Serial_SendString(USART3, "AT+QMTOPEN=0,\"iot-06z00ecgie1cp6b.mqtt.iothub.aliyuncs.com\",1883");Serial_SendString(USART3, "\r\n");
	Delay_ms(3000);
	Serial_SendString(USART3, "AT+QMTCONN=0,\"test\"");Serial_SendString(USART3, "\r\n");
	Delay_ms(3000);
	while(1)
	{
		if(Save_Data.isGetData)
		{
			// Serial_SendByte(USART1,'a');
			parseGpsBuffer();
			if(!GPSempty)
			{				
				printf("AT+QMTPUB=0,1,1,0,\"/sys/hveoaDatr9X/test/thing/event/property/post\",\"{\"params\":{\"GeoLocation\":{\"Latitude\":%s,\"Longitude\":%s}}}\"", Save_Data.latitude, Save_Data.longitude);
				Serial_SendString(USART3, "\r\n");
				Delay_s(10);
				GPSempty = 1;
			}
		}
		while(Sound)
		{
			Passive_BEEP_Run();
			if(Serial_RxFlag1){break;}
		}
		if(Serial_RxFlag1)
		{
			if(Serial_RxPacket1[0]=='S'){Sound=1;}
			else if(Serial_RxPacket1[0]=='N'){Sound=0;}
			//else if(Serial_RxPacket1[0]=='O'){setttl();}
			//else if(Serial_RxPacket1[0]=='C'){resetttl();}
			//Serial_SendString(USART1, Serial_RxPacket1);
			Serial_RxFlag1=0;
		}
		//printf("AT+QMTPUB=0,1,1,0,\"/sys/hveoaDatr9X/test/thing/event/property/post\","{params:{Location:1}}"
	}
	
}
	

void parseGpsBuffer(void)
{
	// Serial_SendByte(USART1,'5');
	char *subString;
	char *subStringNext;
	char i = 0;
	if (Save_Data.isGetData)
	{
		// Serial_SendByte(USART1,'!');
		Save_Data.isGetData = false;
		for (i = 0 ; i <= 6 ; i++)
		{
			if (i == 0)
			{
				if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL){break;}
					//errorLog(1);	//解析错误
			}
			else
			{
				subString++;
				if ((subStringNext = strstr(subString, ",")) != NULL)
				{
					char usefullBuffer[2]; 
					switch(i)
					{
						case 1:memcpy(Save_Data.UTCTime, subString, subStringNext - subString);break;	//获取UTC时间
						case 2:memcpy(usefullBuffer, subString, subStringNext - subString);break;	//获取UTC时间
						case 3:memcpy(Save_Data.latitude, subString, subStringNext - subString);break;	//获取纬度信息
						case 4:memcpy(Save_Data.N_S, subString, subStringNext - subString);break;	//获取N/S
						case 5:memcpy(Save_Data.longitude, subString, subStringNext - subString);break;	//获取经度信息
						case 6:memcpy(Save_Data.E_W, subString, subStringNext - subString);break;	//获取E/W
						default:break;
					}

					subString = subStringNext;
					//Save_Data.isParseData = true;
					//if(usefullBuffer[0] == 'A')
						//Save_Data.isUsefull = true;
					//else if(usefullBuffer[0] == 'V')
						//Save_Data.isUsefull = false;
					if(strlen(Save_Data.latitude)==0)
					{
						GPSempty=1;
					}
					else
					{
						GPSempty=0;
					}
					

				}
				else
				{
					//errorLog(2);	//解析错误
				}
			}
		}
	}
}
