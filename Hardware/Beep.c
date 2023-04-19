#include "BEEP.h"
#include "Delay.h"
 
void BEEP_Config(void)				
{
	GPIO_InitTypeDef BEEP_Pin;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	BEEP_Pin.GPIO_Mode = GPIO_Mode_Out_PP;
	BEEP_Pin.GPIO_Pin = GPIO_Pin_12; 
	BEEP_Pin.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&BEEP_Pin);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_12);
}
 
void Passive_BEEP_Run(void)        //运行 无源蜂鸣器
{
    GPIO_ResetBits(GPIOB,GPIO_Pin_12);
    Delay_ms(100);
    GPIO_SetBits(GPIOB,GPIO_Pin_12);
    Delay_ms(100);
}