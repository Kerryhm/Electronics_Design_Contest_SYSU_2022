#ifndef __BEEP_h
#define __BEEP_h
 
 
#include "stm32f10x.h"
#include "delay.h"
 
 
 
 
void BEEP_Config(void);    			//配置 LED
 
void Passive_BEEP_Run(void);		//运行 无源蜂鸣器 
 
 
#endif
