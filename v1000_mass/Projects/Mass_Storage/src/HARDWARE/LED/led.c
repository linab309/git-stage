#include "stm32l1xx.h"
#include "led.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/5/27
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved											  
////////////////////////////////////////////////////////////////////////////////// 	   

//��ʼ��PA8��PD2Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;

 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);	 //ʹ��PA�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;				 //LED0-->PA.8 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 		 //�������
 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 GPIO_Init(GPIOA, &GPIO_InitStructure);
 GPIO_ResetBits(GPIOA,GPIO_Pin_14);						 //PA.8 �����

#if 0
 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);   //ʹ��PD�˿�ʱ��

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	    		 //LED1-->PD.2 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 		 //�������
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 GPIO_Init(GPIOD, &GPIO_InitStructure);	
 GPIO_SetBits(GPIOD,GPIO_Pin_2); 						 //PD.2 ����� 
 #endif
}
 
