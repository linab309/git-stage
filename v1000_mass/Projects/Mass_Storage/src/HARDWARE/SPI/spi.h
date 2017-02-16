#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//SPI 驱动函数	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/13 
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  


// SPI总线速度设置 
#define SPI_SPEED_2   0
#define SPI_SPEED_8   1
#define SPI_SPEED_16  2
#define SPI_SPEED_256 3


/** @addtogroup STM32L152D_EVAL_LOW_LEVEL_SPI_EE
  * @{
  */
/**
  * @brief  sEE SPI Interface pins   flash....
  */
#define sEE_SPI                          SPI1
#define sEE_SPI_CLK                      RCC_APB2Periph_SPI1
   
#define sEE_SPI_SCK_PIN                  GPIO_Pin_5                  /* PA.05 */
#define sEE_SPI_SCK_GPIO_PORT            GPIOA                       /* GPIOA */
#define sEE_SPI_SCK_GPIO_CLK             RCC_AHBPeriph_GPIOA
#define sEE_SPI_SCK_SOURCE               GPIO_PinSource5
#define sEE_SPI_SCK_AF                   GPIO_AF_SPI1

#define sEE_SPI_MISO_PIN                 GPIO_Pin_6                 /* PE.14 */
#define sEE_SPI_MISO_GPIO_PORT           GPIOA                       /* GPIOE */
#define sEE_SPI_MISO_GPIO_CLK            RCC_AHBPeriph_GPIOA
#define sEE_SPI_MISO_SOURCE              GPIO_PinSource6
#define sEE_SPI_MISO_AF                  GPIO_AF_SPI1

#define sEE_SPI_MOSI_PIN                 GPIO_Pin_7                 /* PE.15 */
#define sEE_SPI_MOSI_GPIO_PORT           GPIOA                       /* GPIOE */
#define sEE_SPI_MOSI_GPIO_CLK            RCC_AHBPeriph_GPIOA
#define sEE_SPI_MOSI_SOURCE              GPIO_PinSource7
#define sEE_SPI_MOSI_AF                  GPIO_AF_SPI1

#define sEE_SPI_CS_PIN                   GPIO_Pin_4                 /* PA.04 */
#define sEE_SPI_CS_GPIO_PORT             GPIOA                       /* GPIOA */
#define sEE_SPI_CS_GPIO_CLK              RCC_AHBPeriph_GPIOA   


						  	    													  
void SPIx_Init(void);			 //初始化SPI口
void SPIx_SetSpeed(u8 SpeedSet); //设置SPI速度   
u8 SPIx_ReadWriteByte(u8 TxData);//SPI总线读写一个字节
		 
#endif

