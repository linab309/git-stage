#ifndef __FLASH_H
#define __FLASH_H			    
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//W25X16 驱动函数	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/13 
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

#if 0
/* FSMC NAND memory parameters */
#define NAND_PAGE_SIZE             ((uint16_t)0x1000) /* 4096 bytes per page w/o Spare Area */
#define NAND_BLOCK_SIZE            ((uint16_t)0x00200) /* 512*4096 bytes pages per block */
#define NAND_ZONE_SIZE             ((uint16_t)0x0020) /* 32 Block per zone */
#define NAND_SPARE_AREA_SIZE       ((uint16_t)0x0010) /* last 16 bytes as spare area */
#define NAND_MAX_ZONE              ((uint16_t)0x0001) /* 1 zones of 32 block */
#else

#define NAND_PAGE_SIZE             ((uint16_t)0x200) /* 512 bytes per page w/o Spare Area */
#define NAND_BLOCK_SIZE            ((uint16_t)0x008) /* 8*512 bytes pages per block */
//#define NAND_ZONE_SIZE             ((uint16_t)0x0200) /* 512 Block per zone */
#define NAND_ZONE_SIZE             ((uint16_t)0xFFF) /* 512 Block per zone */
#define NAND_SPARE_AREA_SIZE       ((uint16_t)0x0010) /* last 16 bytes as spare area */
#define NAND_MAX_ZONE              ((uint16_t)0x0001) /* 1 zones of 32 block */
#endif

#define	SPI_FLASH_CS PAout(4)  //选中FLASH					 
////////////////////////////////////////////////////////////////////////////
//W25X16读写
//#define FLASH_ID 0XC216
//#define FLASH_ID 0XEF14
#define FLASH_ID 0XC217

//指令表
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 

void SPI_Flash_Init(void);
u16  SPI_Flash_ReadID(void);  	    //读取FLASH ID
u8	 SPI_Flash_ReadSR(void);        //读取状态寄存器 
void SPI_FLASH_Write_SR(u8 sr);  	//写状态寄存器
void SPI_FLASH_Write_Enable(void);  //写使能 
void SPI_FLASH_Write_Disable(void);	//写保护
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   //读取flash
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//写入flash
void SPI_Flash_Erase_Chip(void);    	  //整片擦除
void SPI_Flash_Erase_Sector(u32 Dst_Addr);//扇区擦除
void SPI_Flash_Wait_Busy(void);           //等待空闲
void SPI_Flash_PowerDown(void);           //进入掉电模式
void SPI_Flash_WAKEUP(void);			  //唤醒
void test_read_flash_and_v1000_debug(void);
void read_gps(void);
void read_last_sector(void);
void stop_read_gps(void);
void read_gps_next(void);
void SPI_Flash_writePage(u8* pBuffer,u32 secpos,u16 NumByteToWrite)   ;

#endif
















