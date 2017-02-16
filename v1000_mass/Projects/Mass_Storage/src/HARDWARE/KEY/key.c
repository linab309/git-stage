#include "stm32l1xx.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
#include "usart.h"
#include "menutal.h"



//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//按键输入 驱动代码		   
//技术论坛:www.openedv.com
//修改日期:2011/11/28 
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) ALIENTEK  2009-2019
//All rights reserved
//********************************************************************************
 u8 key_press_cnt = 0;
void KEY_Init(void) //IO初始化
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
#if 1

	//init GPIOA.13,GPIOA.15  上拉输入
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC,ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;
//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;
//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);


  /* Enable SYSCFG clock */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC,ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);


      /* Connect PC13 to RTC_AF1 */
        GPIO_PinAFConfig(GPIOC, GPIO_PinSource13, GPIO_AF_WKUP);
	/* Select User Button pin as input source for EXTI Line */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource13);

	/* Configure EXT1 Line 0 in interrupt mode trigged on Rising edge */
	EXTI_InitStructure.EXTI_Line = EXTI_Line13 ;  // Pc13 for User button AND IDD_WakeUP
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI0 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
#endif	


#if 1
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);

      /* Connect PC13 to RTC_AF1 */
      GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_WKUP);
	/* Select User Button pin as input source for EXTI Line */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);

	/* Configure EXT1 Line 0 in interrupt mode trigged on Rising edge */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0 ;  // Pc13 for User button AND IDD_WakeUP
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI0 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
#endif
  
}

extern u8 system_power_flag ;


u8 KEY_Scan(void)
{	 
	static u8 key_up=1;//按键按松开标志

	static u8 key_press_statby_cnt = 0;
	static u8 key_press_vaule = 0xff;
	extern u8 menu_setting_index ;
//       u8 key_reachnumber = 0;
// 	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	JTAG_Set(0);
	if(key_up&&(KEY0==1||KEY1==0||KEY2==1))
	{
	//	delay_ms(10);//去抖动 
		key_up=0;
		key_press_statby_cnt = 0;
		if(KEY0==1)
		{
	 
	//		 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
		//	JTAG_Set(1);
			 key_press_vaule  = 1;	
	//		return 1;
		}
		else if(KEY1==0)
		{
			JTAG_Set(1);	 	 
	   //  /	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
			   key_press_vaule  = 2;	
		//	return 2;
		}
		#if 1
		else if(KEY2==1)
		{
			JTAG_Set(1);	 	 
	   //  /	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
		//	return 1;
		key_press_vaule  = 3;
	    	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
			
		}
		#endif
	}
	else if(KEY0==0&&KEY1==1&&KEY2==0)
		{
		key_up=1; 	    
		key_press_cnt = 0;
		key_press_statby_cnt = 0; 
		if(key_press_vaule == 1)
			{
			key_press_vaule = 0xff;
			JTAG_Set(1);
			return 3;
			}
		else if (key_press_vaule == 2)
			{
			key_press_vaule = 0xff;
			JTAG_Set(1);
			return 2;

			}
		else if (key_press_vaule == 3)
			{
			key_press_vaule = 0xff;
			JTAG_Set(1);
			return 1;

			}

		}
	else if(key_up == 0)
		{
		if((KEY1==0)&&(KEY0 == 1)&&KEY2==1)
			{

			#if 1
			v1000_debug("\n\r key_press_statby_cnt :%d",key_press_statby_cnt);
                   key_press_cnt = 0;
			if(system_power_flag == SYSTEM_POWER_STANBY)
				{
					if(key_press_statby_cnt < 75)
					key_press_statby_cnt++;

					if(key_press_statby_cnt == 75)
					{
					key_press_vaule = 0xff;
					key_press_statby_cnt = 76;
					JTAG_Set(1);
					return 6;
					}
				}
			else
				{
					if(key_press_statby_cnt < 50)
					key_press_statby_cnt++;

					if(key_press_statby_cnt == 50)
					{
					key_press_vaule = 0xff;
					key_press_statby_cnt = 51;
					JTAG_Set(1);
					return 6;
					}

				}
			#endif
			}
		else
			{
			key_press_statby_cnt = 0;
		 if(KEY0 == 1)
		 	{
			if(key_press_cnt < 25)
			 	key_press_cnt++;
			if(key_press_cnt == 25)
				{
				key_press_vaule = 0xff;
				key_press_cnt = 51;
				JTAG_Set(1);
				return 4;
				}
		 	}
		 else if(KEY1 == 0)

		 	{
		 //	if((KeyTab[keyTabIndex].subnumber == 'S'))
			 	key_press_cnt++;
			if(menu_setting_index!=0xff)
				{
				if(key_press_cnt >= 10)
					{
					key_press_vaule = 0xff;
					key_press_cnt = 11;
					JTAG_Set(1);
					return 2;
					}
				}
			else
				{
					if(key_press_cnt == 25)						
						{
						key_press_vaule = 0xff;
						key_press_cnt = 26;
						JTAG_Set(1);
					//	system_flag_table->usb_standy_poweron_flag = 0;
						return 7;
						}
				}
		 	}
		 else if(KEY2 == 1)		 
		 	{

			 key_press_cnt++;
		 	if(menu_setting_index!=0xff)
		 		{
					if(key_press_cnt >= 10)
						{
						key_press_vaule = 0xff;
						key_press_cnt = 11;
						JTAG_Set(1);
						return 1;
						}
		 		}
			else
				{
				#if 0
				if(system_power_flag == SYSTEM_POWER_STANBY)
					key_reachnumber = 50;
				else
					key_reachnumber = 25;
				#endif
					if(key_press_cnt == 25)						
					{
						key_press_vaule = 0xff;
						key_press_cnt = 26;
						JTAG_Set(1);
						system_flag_table->usb_standy_poweron_flag = 0;
						return 5;
					}	
				}
		 	}
			}

		}
		
 		JTAG_Set(1);
 //	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	return 0;// 无按键按下
}

