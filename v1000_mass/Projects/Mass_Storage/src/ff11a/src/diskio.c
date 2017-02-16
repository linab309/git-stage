/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "flash.h" 
#include "usart.h"
//#include "usbdisk.h"	/* Example: Header file of existing USB MSD control module */
//#include "atadrive.h"	/* Example: Header file of existing ATA harddisk control module */
//#include "sdcard.h"		/* Example: Header file of existing MMC/SDC contorl module */

/* Definitions of physical drive number for each drive */
#define ATA		0	/* Example: Map ATA harddisk to physical drive 0 */
#define MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define USB		2	/* Example: Map USB MSD to physical drive 2 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = RES_OK;
    u8 result;

    //v1000_debug("disk_status :%d \r\n",pdrv);

	switch (pdrv) {
	case ATA :
		//result = ATA_disk_status();
		if(system_flag_table->usbdisk_flag == 1)
			return RES_OK;

          result = SPI_Flash_ReadSR();
          //v1000_debug("readsr :%d \r\n",result);
		// translate the reslut code here
              
		return RES_OK;

	case MMC :
		//result = MMC_disk_status();

		// translate the reslut code here

		return stat;

	case USB :
		//result = USB_disk_status();

		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = RES_OK;;
//	int result;

        //v1000_debug("disk_initialize :%d \r\n",pdrv);

	switch (pdrv) {
	case ATA :
		//result = ATA_disk_initialize();

		// translate the reslut code here
        stat = RES_OK;

		return stat;

	case MMC :
		//result = MMC_disk_initialize();

		// translate the reslut code here

		return stat;

	case USB :
		//result = USB_disk_initialize();

		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res = RES_OK;
//	int result;
	switch (pdrv) {
	case ATA :
		// translate the arguments here

		//result = ATA_disk_read(buff, sector, count);
       // v1000_debug("read sector :%d \r\n",sector);
        //v1000_debug(" count :%d \r\n",count);
  		if(system_flag_table->usbdisk_flag == 1)
			return RES_OK;
        SPI_Flash_Read(buff,sector*NAND_PAGE_SIZE,count*NAND_PAGE_SIZE);   
  #if 0              
        for(i = 0;i<512;i++)
          {
                v1000_debug("%x ",buff[i]);
                if(i%32 == 0)
                v1000_debug("\r\n");  
          }
  #endif    
		res = RES_OK;
		// translate the reslut code here

		return res;

	case MMC :
		// translate the arguments here

		//result = MMC_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case USB :
		// translate the arguments here

		//result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res = RES_OK;
//nt result;

	switch (pdrv) {
	case ATA :
		// translate the arguments here

        //v1000_debug("write sector :%d \r\n",sector);
        //v1000_debug(" count :%d \r\n",count);
		if(system_flag_table->usbdisk_flag == 1)
			return RES_OK;

        SPI_Flash_Write((BYTE *)buff,sector*NAND_PAGE_SIZE,count*NAND_PAGE_SIZE);   
        //SPI_Flash_writePage((BYTE *)buff,sector,count);   
        res = RES_OK;
		//result = ATA_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case MMC :
		// translate the arguments here

		//result = MMC_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case USB :
		// translate the arguments here

		//result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}
#endif



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_OK;

	switch (pdrv) {
	case ATA :

		// Process of the command for the ATA drive
        v1000_debug("disk ioctl :%d \r\n",pdrv);   

		if(system_flag_table->usbdisk_flag == 1)
			return RES_OK;
		
        switch(cmd)
        {
             case CTRL_SYNC:
                  res = RES_OK;
               break;
             
             case GET_SECTOR_SIZE:
               
                   *(WORD*)buff = NAND_PAGE_SIZE;
                   res = RES_OK;
               break;
             
             case GET_SECTOR_COUNT:
                   
                   *(DWORD*)buff = NAND_BLOCK_SIZE*NAND_ZONE_SIZE;
                   res = RES_OK;

               break;
             
             case GET_BLOCK_SIZE:
               
                   *(DWORD*)buff = NAND_BLOCK_SIZE;
                   res = RES_OK;

               break;
             
             
             default:
               
               res = RES_PARERR;
               break;
        }
		return res;

	case MMC :

		// Process of the command for the MMC/SD card

		return res;

	case USB :

		// Process of the command the USB drive

		return res;
	}

	return RES_PARERR;
}



#endif
