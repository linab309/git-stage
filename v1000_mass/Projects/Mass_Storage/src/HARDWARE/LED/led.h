#ifndef __LED_H
#define __LED_H	 
#include "stm32l1xx.h"
//Mini STM32������
//LED��������			 
//����ԭ��@ALIENTEK
//2012/2/27

//LED�˿ڶ���
#define LED0 PAout(14)// PA8
#define GPS_POWER PAout(8)// PD2	

void LED_Init(void);//��ʼ��

		 				    
#endif
