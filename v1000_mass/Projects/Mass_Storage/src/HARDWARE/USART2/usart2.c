#include "delay.h"
#include "usart2.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	   
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//±¾³ÌÐòÖ»¹©Ñ§Ï°Ê¹ÓÃ£¬Î´¾­×÷ÕßÐí¿É£¬²»µÃÓÃÓÚÆäËüÈÎºÎÓÃÍ¾
//ALIENTEKÕ½½¢STM32¿ª·¢°å
//´®¿Ú2Çý¶¯´úÂë	   
//ÕýµãÔ­×Ó@ALIENTEK
//¼¼ÊõÂÛÌ³:www.openedv.com
//ÐÞ¸ÄÈÕÆÚ:2013/2/22
//°æ±¾£ºV1.0
//°æÈ¨ËùÓÐ£¬µÁ°æ±Ø¾¿¡£
//Copyright(C) ¹ãÖÝÊÐÐÇÒíµç×Ó¿Æ¼¼ÓÐÏÞ¹«Ë¾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//´®¿Ú·¢ËÍ»º´æÇø 	
__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//·¢ËÍ»º³å,×î´óUSART2_MAX_SEND_LEN×Ö½Ú
#ifdef USART2_RX_EN   								//Èç¹ûÊ¹ÄÜÁË½ÓÊÕ   	  
//´®¿Ú½ÓÊÕ»º´æÇø 	
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//½ÓÊÕ»º³å,×î´óUSART2_MAX_RECV_LEN¸ö×Ö½Ú.


//Í¨¹ýÅÐ¶Ï½ÓÊÕÁ¬Ðø2¸ö×Ö·ûÖ®¼äµÄÊ±¼ä²î²»´óÓÚ10msÀ´¾ö¶¨ÊÇ²»ÊÇÒ»´ÎÁ¬ÐøµÄÊý¾Ý.
//Èç¹û2¸ö×Ö·û½ÓÊÕ¼ä¸ô³¬¹ý10ms,ÔòÈÏÎª²»ÊÇ1´ÎÁ¬ÐøÊý¾Ý.Ò²¾ÍÊÇ³¬¹ý10msÃ»ÓÐ½ÓÊÕµ½
//ÈÎºÎÊý¾Ý,Ôò±íÊ¾´Ë´Î½ÓÊÕÍê±Ï.
//½ÓÊÕµ½µÄÊý¾Ý×´Ì¬
//[15]:0,Ã»ÓÐ½ÓÊÕµ½Êý¾Ý;1,½ÓÊÕµ½ÁËÒ»ÅúÊý¾Ý.
//[14:0]:½ÓÊÕµ½µÄÊý¾Ý³¤¶È
u16 USART2_RX_STA=0;   	 
void USART2_IRQHandler(void)
{
	u8 res;	    
	//if(USART2->SR&(1<<5))//½ÓÊÕµ½Êý¾Ý
	 if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)	
	{	 
		res=USART2->DR; 			 
		if(USART2_RX_STA<USART2_MAX_RECV_LEN)		//»¹¿ÉÒÔ½ÓÊÕÊý¾Ý
		{
			TIM4->CNT=0;         					//¼ÆÊýÆ÷Çå¿Õ
			if(USART2_RX_STA==0)TIM4_Set(1);	 	//Ê¹ÄÜ¶¨Ê±Æ÷4µÄÖÐ¶Ï 
			USART2_RX_BUF[USART2_RX_STA++]=res;		//¼ÇÂ¼½ÓÊÕµ½µÄÖµ	 
		}else 
		{
			USART2_RX_STA|=1<<15;					//Ç¿ÖÆ±ê¼Ç½ÓÊÕÍê³É
		} 
	}  											 
}   
//³õÊ¼»¯IO ´®¿Ú2
//pclk1:PCLK1Ê±ÖÓÆµÂÊ(Mhz)
//bound:²¨ÌØÂÊ	  
void USART2_Init(u32 pclk1,u32 bound)
{  	 
   GPIO_InitTypeDef  GPIO_InitStructure;
   USART_InitTypeDef USART_InitStructure;
//NVIC_InitTypeDef NVIC_InitStructure;


/* Enable GPIO clock */
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOAEN , ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* Connect rx to GPIO_AF_USART2 */
   //    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	//GPIO_Init(GPIOA, &GPIO_InitStructure);

/* Enable USART clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); 

//	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); 

	   


	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx ;
	USART_Init(USART2, &USART_InitStructure);



	USART2->CR1|=1<<8;    	//PEÖÐ¶ÏÊ¹ÄÜ
	USART2->CR1|=1<<5;    	//½ÓÊÕ»º³åÇø·Ç¿ÕÖÐ¶ÏÊ¹ÄÜ	    	
	MY_NVIC_Init(2,3,USART2_IRQn,2);//×é2£¬×îµÍÓÅÏÈ¼¶ 

//      USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	USART_Cmd(USART2, ENABLE);
	//UART_DMA_Config(DMA1_Channel7,(u32)&USART2->DR,(u32)USART2_TX_BUF);//DMA1Í¨µÀ7,ÍâÉèÎª´®¿Ú2,´æ´¢Æ÷ÎªUSART2_TX_BUF 

      if(pclk1 == 16)
	  TIM4_Init(99,1599);		//10msÖÐ¶Ï
      else
	  TIM4_Init(99,3199);		//10msÖÐ¶
	  
	USART2_RX_STA=0;		//ÇåÁã
	TIM4_Set(0);			//¹Ø±Õ¶¨Ê±Æ÷4
//	UART_DMA_Config(DMA1_Channel7,(u32)&USART2->DR,(u32)USART2_TX_BUF);//DMA1Í¨µÀ7,ÍâÉèÎª´®¿Ú2,´æ´¢Æ÷ÎªUSART2_TX_BUF 
#if 0  
   	   
	//²¨ÌØÂÊÉèÖÃ
 	USART2->BRR=(pclk1*1000000)/(bound);// ²¨ÌØÂÊÉèÖÃ	 
	USART2->CR1|=0X200C;  	//1Î»Í£Ö¹,ÎÞÐ£ÑéÎ».
	USART2->CR3=1<<7;   	//Ê¹ÄÜ´®¿Ú2µÄDMA·¢ËÍ
//	UART_DMA_Config(DMA1_Channel7,(u32)&USART2->DR,(u32)USART2_TX_BUF);//DMA1Í¨µÀ7,ÍâÉèÎª´®¿Ú2,´æ´¢Æ÷ÎªUSART2_TX_BUF 
#ifdef USART2_RX_EN		  	//Èç¹ûÊ¹ÄÜÁË½ÓÊÕ
	//Ê¹ÄÜ½ÓÊÕÖÐ¶Ï
	USART2->CR1|=1<<8;    	//PEÖÐ¶ÏÊ¹ÄÜ
	USART2->CR1|=1<<5;    	//½ÓÊÕ»º³åÇø·Ç¿ÕÖÐ¶ÏÊ¹ÄÜ	    	
	MY_NVIC_Init(2,3,USART2_IRQn,2);//×é2£¬×îµÍÓÅÏÈ¼¶ 
	TIM4_Init(99,1599);		//10msÖÐ¶Ï
	USART2_RX_STA=0;		//ÇåÁã
	TIM4_Set(0);			//¹Ø±Õ¶¨Ê±Æ÷4
#endif		
#endif
}

#if 0
//´®¿Ú2,v1000_debug º¯Êý
//È·±£Ò»´Î·¢ËÍÊý¾Ý²»³¬¹ýUSART2_MAX_SEND_LEN×Ö½Ú
void u2_v1000_debug(char* fmt,...)  
{  
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	while(DMA1_Channel7->CNDTR!=0);	//µÈ´ýÍ¨µÀ7´«ÊäÍê³É   
//	UART_DMA_Enable(DMA1_Channel7,strlen((const char*)USART2_TX_BUF)); 	//Í¨¹ýdma·¢ËÍ³öÈ¥
}
#endif

//¶¨Ê±Æ÷4ÖÐ¶Ï·þÎñ³ÌÐò		    
void TIM4_IRQHandler(void)
{ 	
	if(TIM4->SR&0X01)//ÊÇ¸üÐÂÖÐ¶Ï
	{	 			   
		USART2_RX_STA|=1<<15;	//±ê¼Ç½ÓÊÕÍê³É
		TIM4->SR&=~(1<<0);		//Çå³ýÖÐ¶Ï±êÖ¾Î»		   
		TIM4_Set(0);			//¹Ø±ÕTIM4  
	}	    
}
//ÉèÖÃTIM4µÄ¿ª¹Ø
//sta:0£¬¹Ø±Õ;1,¿ªÆô;
void TIM4_Set(u8 sta)
{
	if(sta)
	{
    	TIM4->CNT=0;         //¼ÆÊýÆ÷Çå¿Õ
		TIM4->CR1|=1<<0;     //Ê¹ÄÜ¶¨Ê±Æ÷4
	}else TIM4->CR1&=~(1<<0);//¹Ø±Õ¶¨Ê±Æ÷4	   
}
//Í¨ÓÃ¶¨Ê±Æ÷ÖÐ¶Ï³õÊ¼»¯
//ÕâÀïÊ¼ÖÕÑ¡ÔñÎªAPB1µÄ2±¶£¬¶øAPB1Îª36M
//arr£º×Ô¶¯ÖØ×°Öµ¡£
//psc£ºÊ±ÖÓÔ¤·ÖÆµÊý		 
void TIM4_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<2;	//TIM4Ê±ÖÓÊ¹ÄÜ    
 	TIM4->ARR=arr;  	//Éè¶¨¼ÆÊýÆ÷×Ô¶¯ÖØ×°Öµ   
	TIM4->PSC=psc;  	//Ô¤·ÖÆµÆ÷
 	TIM4->DIER|=1<<0;   //ÔÊÐí¸üÐÂÖÐ¶Ï				
 	TIM4->CR1|=0x01;  	//Ê¹ÄÜ¶¨Ê±Æ÷4	  	   
   	MY_NVIC_Init(1,3,TIM4_IRQn,2);//ÇÀÕ¼2£¬×ÓÓÅÏÈ¼¶3£¬×é2	ÔÚ2ÖÐÓÅÏÈ¼¶×îµÍ								 
}
#endif	

#if 0
///////////////////////////////////////USART2 DMA·¢ËÍÅäÖÃ²¿·Ö//////////////////////////////////	   		    
//DMA1µÄ¸÷Í¨µÀÅäÖÃ
//ÕâÀïµÄ´«ÊäÐÎÊ½ÊÇ¹Ì¶¨µÄ,ÕâµãÒª¸ù¾Ý²»Í¬µÄÇé¿öÀ´ÐÞ¸Ä
//´Ó´æ´¢Æ÷->ÍâÉèÄ£Ê½/8Î»Êý¾Ý¿í¶È/´æ´¢Æ÷ÔöÁ¿Ä£Ê½
//DMA_CHx:DMAÍ¨µÀCHx
//cpar:ÍâÉèµØÖ·
//cmar:´æ´¢Æ÷µØÖ·    
void UART_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar)
{
 	RCC->AHBENR|=1<<0;			//¿ªÆôDMA1Ê±ÖÓ
	delay_us(5);
	DMA_CHx->CPAR=cpar; 		//DMA1 ÍâÉèµØÖ· 
	DMA_CHx->CMAR=cmar; 		//DMA1,´æ´¢Æ÷µØÖ·	 
	DMA_CHx->CCR=0X00000000;	//¸´Î»
	DMA_CHx->CCR|=1<<4;  		//´Ó´æ´¢Æ÷¶Á
	DMA_CHx->CCR|=0<<5;  		//ÆÕÍ¨Ä£Ê½
	DMA_CHx->CCR|=0<<6;  		//ÍâÉèµØÖ··ÇÔöÁ¿Ä£Ê½
	DMA_CHx->CCR|=1<<7;  		//´æ´¢Æ÷ÔöÁ¿Ä£Ê½
	DMA_CHx->CCR|=0<<8;  		//ÍâÉèÊý¾Ý¿í¶ÈÎª8Î»
	DMA_CHx->CCR|=0<<10; 		//´æ´¢Æ÷Êý¾Ý¿í¶È8Î»
	DMA_CHx->CCR|=1<<12; 		//ÖÐµÈÓÅÏÈ¼¶
	DMA_CHx->CCR|=0<<14; 		//·Ç´æ´¢Æ÷µ½´æ´¢Æ÷Ä£Ê½		  	
} 
//¿ªÆôÒ»´ÎDMA´«Êä
void UART_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,u8 len)
{
	DMA_CHx->CCR&=~(1<<0);       //¹Ø±ÕDMA´«Êä 
	DMA_CHx->CNDTR=len;          //DMA1,´«ÊäÊý¾ÝÁ¿ 
	DMA_CHx->CCR|=1<<0;          //¿ªÆôDMA´«Êä
}	   
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 									 

#endif



















