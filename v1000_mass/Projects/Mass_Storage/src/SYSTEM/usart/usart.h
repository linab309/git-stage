#ifndef __USART_H
#define __USART_H
#include "stdio.h"
//////////////////////////////////////////////////////////////////////////////////	 

//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�v1000_debug��֧��
//�����˴��ڽ��������.
//������v1000_debug��һ���ַ���ʧ��bug
////////////////////////////////////////////////////////////////////////////////// 

#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			0		//ʹ�ܣ�1��/��ֹ��0������1����
#define EN_USART1 0

#if EN_USART1_RX

    extern u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���?63���ֽ�.ĩ�ֽ�Ϊ���з� 
    extern u16 USART_RX_STA;         //����״̬���?	
#endif

#if EN_USART1

//�����?���жϽ��գ��벻Ҫע�����º궨��
    void uart_init(u32 bound);
    #define v1000_debug(fmt, arg...) printf(fmt,##arg)

#else

    #define v1000_debug(fmt, arg...) 
 	

#endif


#endif
