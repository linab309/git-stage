#ifndef __FLASH_H
#define __FLASH_H			    
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//W25X16 ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/6/13 
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
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

#define	SPI_FLASH_CS PAout(4)  //ѡ��FLASH					 
////////////////////////////////////////////////////////////////////////////
//W25X16��д
//#define FLASH_ID 0XC216
//#define FLASH_ID 0XEF14
#define FLASH_ID 0XC217

//ָ���
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
u16  SPI_Flash_ReadID(void);  	    //��ȡFLASH ID
u8	 SPI_Flash_ReadSR(void);        //��ȡ״̬�Ĵ��� 
void SPI_FLASH_Write_SR(u8 sr);  	//д״̬�Ĵ���
void SPI_FLASH_Write_Enable(void);  //дʹ�� 
void SPI_FLASH_Write_Disable(void);	//д����
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   //��ȡflash
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//д��flash
void SPI_Flash_Erase_Chip(void);    	  //��Ƭ����
void SPI_Flash_Erase_Sector(u32 Dst_Addr);//��������
void SPI_Flash_Wait_Busy(void);           //�ȴ�����
void SPI_Flash_PowerDown(void);           //�������ģʽ
void SPI_Flash_WAKEUP(void);			  //����
void test_read_flash_and_v1000_debug(void);
void read_gps(void);
void read_last_sector(void);
void stop_read_gps(void);
void read_gps_next(void);
void SPI_Flash_writePage(u8* pBuffer,u32 secpos,u16 NumByteToWrite)   ;

#endif
















