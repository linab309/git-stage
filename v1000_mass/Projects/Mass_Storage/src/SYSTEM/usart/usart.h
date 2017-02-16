#ifndef __USART_H
#define __USART_H
#include "stdio.h"
//////////////////////////////////////////////////////////////////////////////////	 

//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对v1000_debug的支持
//增加了串口接收命令功能.
//修正了v1000_debug第一个字符丢失的bug
////////////////////////////////////////////////////////////////////////////////// 

#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			0		//使能（1）/禁止（0）串口1接收
#define EN_USART1 0

#if EN_USART1_RX

    extern u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大63个字节.末字节为换行符 
    extern u16 USART_RX_STA;         //接收状态标记	
#endif

#if EN_USART1

//如果想串口中断接收，请不要注释以下宏定义
    void uart_init(u32 bound);
    #define v1000_debug(fmt, arg...) printf(fmt,##arg)

#else

    #define v1000_debug(fmt, arg...) 
 	

#endif


#endif
