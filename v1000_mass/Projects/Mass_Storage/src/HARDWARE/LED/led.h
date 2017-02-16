#ifndef __LED_H
#define __LED_H	 
#include "stm32l1xx.h"
//Mini STM32开发板
//LED驱动代码			 
//正点原子@ALIENTEK
//2012/2/27

//LED端口定义
#define LED0 PAout(14)// PA8
#define GPS_POWER PAout(8)// PD2	

void LED_Init(void);//初始化

		 				    
#endif
