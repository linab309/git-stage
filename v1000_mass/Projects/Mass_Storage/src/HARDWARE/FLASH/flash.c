#include "flash.h" 
#include "spi.h"
#include "delay.h"
#include "stm_eeprom.h"
#include "sys.h"
#include "usart.h"
#include "menutal.h"
#include "hw_config.h"


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

//4Kbytes为一个Sector
//16个扇区为1个Block
//W25X16
//容量为2M字节,共有32个Block,512个Sector 

//初始化SPI FLASH的IO口
void SPI_Flash_Init(void)
{
#if 0
	RCC->APB2ENR|=1<<2;       //PORTA时钟使能 	    
	//这里 
	GPIOA->CRL&=0XFFF0F0FF; 
	GPIOA->CRL|=0X00030300;//PA2.3.4 推挽 	    
	GPIOA->ODR|=0X5<<2;    //PA2.3.4上拉
#endif
	SPIx_Init();		   //初始化SPI
}  

//读取SPI_FLASH的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
u8 SPI_Flash_ReadSR(void)   
{  
	u8 byte=0;   
	SPI_FLASH_CS=0;                            //使能器件   
	SPIx_ReadWriteByte(W25X_ReadStatusReg);    //发送读取状态寄存器命令    
	byte=SPIx_ReadWriteByte(0Xff);             //读取一个字节  
	SPI_FLASH_CS=1;                            //取消片选     
	return byte;   
} 
//写SPI_FLASH状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
void SPI_FLASH_Write_SR(u8 sr)   
{   
	SPI_FLASH_CS=0;                            //使能器件   
	SPIx_ReadWriteByte(W25X_WriteStatusReg);   //发送写取状态寄存器命令    
	SPIx_ReadWriteByte(sr);               //写入一个字节  
	SPI_FLASH_CS=1;                            //取消片选     	      
}   
//SPI_FLASH写使能	
//将WEL置位   
void SPI_FLASH_Write_Enable(void)   
{
	SPI_FLASH_CS=0;                            //使能器件   
    SPIx_ReadWriteByte(W25X_WriteEnable);      //发送写使能  
	SPI_FLASH_CS=1;                            //取消片选     	      
} 
//SPI_FLASH写禁止	
//将WEL清零  
void SPI_FLASH_Write_Disable(void)   
{  
	SPI_FLASH_CS=0;                            //使能器件   
    SPIx_ReadWriteByte(W25X_WriteDisable);     //发送写禁止指令    
	SPI_FLASH_CS=1;                            //取消片选     	      
} 			    
//读取芯片ID W25X16的ID:0XEF14
u16 SPI_Flash_ReadID(void)
{
	u16 Temp = 0;	  
	SPI_FLASH_CS=0;				    
	SPIx_ReadWriteByte(0x90);//发送读取ID命令	    
	SPIx_ReadWriteByte(0x00); 	    
	SPIx_ReadWriteByte(0x00); 	    
	SPIx_ReadWriteByte(0x00); 	 			   
	Temp|=SPIx_ReadWriteByte(0xFF)<<8;  
	Temp|=SPIx_ReadWriteByte(0xFF);	 
	SPI_FLASH_CS=1;				    
	return Temp;
}   		    
//读取SPI FLASH  
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;    												    
	SPI_FLASH_CS=0;                            //使能器件   
    SPIx_ReadWriteByte(W25X_ReadData);         //发送读取命令   
    SPIx_ReadWriteByte((u8)((ReadAddr)>>16));  //发送24bit地址    
    SPIx_ReadWriteByte((u8)((ReadAddr)>>8));   
    SPIx_ReadWriteByte((u8)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPIx_ReadWriteByte(0XFF);   //循环读数  
    }
	SPI_FLASH_CS=1;                            //取消片选     
}  
//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!	 
void SPI_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
    SPI_FLASH_Write_Enable();                  //SET WEL 
	SPI_FLASH_CS=0;                            //使能器件   
    SPIx_ReadWriteByte(W25X_PageProgram);      //发送写页命令   
    SPIx_ReadWriteByte((u8)((WriteAddr)>>16)); //发送24bit地址    
    SPIx_ReadWriteByte((u8)((WriteAddr)>>8));   
    SPIx_ReadWriteByte((u8)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPIx_ReadWriteByte(pBuffer[i]);//循环写数  
	SPI_FLASH_CS=1;                            //取消片选 
	SPI_Flash_Wait_Busy();					   //等待写入结束
} 
//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		SPI_Flash_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	};	    
} 

#if 1
//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)  		   

void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    
       u8 SPI_FLASH_BUF[4096];

	
	secpos=WriteAddr/4096;//扇区地址 0~511 for w25x16
	secoff=WriteAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   

	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
	while(1) 
	{	
		SPI_Flash_Read(SPI_FLASH_BUF,secpos*4096,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			SPI_Flash_Erase_Sector(secpos);//擦除这个扇区
			for(i=0;i<secremain;i++)	   //复制
			{
				SPI_FLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);//写入整个扇区  

		}else SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumByteToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	pBuffer+=secremain;  //指针偏移
			WriteAddr+=secremain;//写地址偏移	   
		   	NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>4096)secremain=4096;	//下一个扇区还是写不完
			else secremain=NumByteToWrite;			//下一个扇区可以写完了
		}	 
	};	 	 
}
#else
//写SPI FLASH  
//写入指定的page 带探险
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)  		   

void SPI_Flash_writePage(u8* pBuffer,u32 secpos,u16 NumByteToWrite)   
{ 

    u16 NumByte = 0;
    NumByte = NumByteToWrite;

    while(NumByte >0)
    {
         SPI_Flash_Erase_Sector(secpos);//擦除这个扇区
         SPI_Flash_Write_Page(pBuffer,secpos*4096,4096);//写入整个扇区  
         NumByte --;
    }
	 
}
#endif

//擦除整个芯片
//整片擦除时间:
//W25X16:25s 
//W25X32:40s 
//W25X64:40s 
//等待时间超长...
void SPI_Flash_Erase_Chip(void)   
{                                             
    SPI_FLASH_Write_Enable();                  //SET WEL 
    SPI_Flash_Wait_Busy();   
  	SPI_FLASH_CS=0;                            //使能器件   
    SPIx_ReadWriteByte(W25X_ChipErase);        //发送片擦除命令  
	SPI_FLASH_CS=1;                            //取消片选     	      
	SPI_Flash_Wait_Busy();   				   //等待芯片擦除结束
}   
//擦除一个扇区
//Dst_Addr:扇区地址 0~511 for w25x16
//擦除一个山区的最少时间:150ms
void SPI_Flash_Erase_Sector(u32 Dst_Addr)   
{   
	Dst_Addr*=4096;
    SPI_FLASH_Write_Enable();                  //SET WEL 	 
    SPI_Flash_Wait_Busy();   
  	SPI_FLASH_CS=0;                            //使能器件   
    SPIx_ReadWriteByte(W25X_SectorErase);      //发送扇区擦除指令 
    SPIx_ReadWriteByte((u8)((Dst_Addr)>>16));  //发送24bit地址    
    SPIx_ReadWriteByte((u8)((Dst_Addr)>>8));   
    SPIx_ReadWriteByte((u8)Dst_Addr);  
	SPI_FLASH_CS=1;                            //取消片选     	      
    SPI_Flash_Wait_Busy();   				   //等待擦除完成
}  
//等待空闲
void SPI_Flash_Wait_Busy(void)   
{   
	while ((SPI_Flash_ReadSR()&0x01)==0x01);   // 等待BUSY位清空
}  
//进入掉电模式
void SPI_Flash_PowerDown(void)   
{ 
  	SPI_FLASH_CS=0;                            //使能器件   
    SPIx_ReadWriteByte(W25X_PowerDown);        //发送掉电命令  
	SPI_FLASH_CS=1;                            //取消片选     	      
    delay_us(3);                               //等待TPD  
}   
//唤醒
void SPI_Flash_WAKEUP(void)   
{  
  	SPI_FLASH_CS=0;                            //使能器件   
    SPIx_ReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
	SPI_FLASH_CS=1;                            //取消片选     	      
    delay_us(3);                               //等待TRES1


}


#ifdef NEED_COM_TO_PC
u8 gps_v1000_debug_status = 0;
u8 send_status = 0;
u8 send_status_flag = 0；

extern uint32_t USART_Rx_ptr_in ;
extern uint32_t USART_Rx_ptr_out ;

static u16 a_number;
static u16 t_number;


int Mycrc16(u8 *buffer,int buffer_len)
{
int cksum = 0,i,stat = 0;
cksum = 0; //初始化为0
for (stat=buffer_len ; stat>0; stat--) //len是所要计算的长度
	{
		cksum = cksum^(int)(*buffer++) << 8; //这句我没搞明白。
		for (i=8; i!=0; i--)
		{
			if (cksum & 0x8000)
				cksum = cksum << 1 ^ 0x1021;
			else
				cksum = cksum << 1;
		}
	}
return cksum;
}




void send_packge_to_usb(u8 comd ,u8 *buffer, u32 len)
{
 	u32 i;    
	int cksum = 0;
       u32 len_tmp;
	len_tmp = len;
	if(USB_CONNECT == 1)
	{

	
	USART_To_USB_Send_Data(0x55);		
	USART_To_USB_Send_Data(comd);
	len_tmp= len_tmp+2;
	USART_To_USB_Send_Data((u8)(len_tmp>>24&0xff));		
	USART_To_USB_Send_Data((u8)(len_tmp>>16&0xff));		
	USART_To_USB_Send_Data((u8)(len_tmp>>8&0xff));		
	USART_To_USB_Send_Data((u8)(len_tmp&0xff));		

      if(comd != 0)
      	{
            for(i=0;i<len_tmp-2;i++)
                USART_To_USB_Send_Data(buffer[i]);		
            cksum = Mycrc16(buffer, len);
      	}
	USART_To_USB_Send_Data((u8)((cksum>>8)&0xff));		
	USART_To_USB_Send_Data((u8)(cksum&0xff));	
	}
}

//#define 0x00


void test_read_flash_and_v1000_debug(void)
{

	static u32 WriteAddr = 0;
	static u32 NumByteToRead;
#if 0
	extern u32 Flash_Index;
#endif
	static u32 secpos;
	u16 secoff;
	static u16 secremain;	   
    extern u8 Flash_full_flag;
	
	u8 temp[4] ={0};
	
	switch(send_status)
		{
		case 0:
                if(Flash_full_flag>0)
                    NumByteToRead = MOST_FLASH_ADDRES;
                else
				    NumByteToRead = Flash_Index; 
				WriteAddr = 0;
				secpos=WriteAddr/4096;//扇区地址 0~511 for w25x16
				secoff=WriteAddr%4096;//在扇区内的偏移
				secremain=4096-secoff;//扇区剩余空间大小   
				if(NumByteToRead<=secremain) 
					secremain=NumByteToRead;//不大于4096个字节

			//	 v1000_debug("\r\n INDEX,TAG,  DATE,  TIME  , LATITUDE N/S, LONGITUDE E/W, HEIGHT,SPEED,ANGLE,TEMP,PER");	   

				//temp[0] = (u8)(NumByteToRead>>24&0xff);
				//temp[1] = (u8)(NumByteToRead>>16&0xff);
				//temp[2] = (u8)(NumByteToRead>>8&0xff);
				//temp[3] = (u8)(NumByteToRead&0xff);
				
				send_packge_to_usb(0x00,temp,NumByteToRead);//第一条语句
				send_status = 1;
		break;
		
		case 1:
			{
			if(USART_Rx_ptr_in != USART_Rx_ptr_out)
				return;
			   SPI_Flash_Read(SPI_FLASH_BUF,secpos*4096,secremain);//读出整个扇区的内容

			//	   v1000_debug("");
			//   buffer_Analysis(SPI_FLASH_BUF,secremain);

			//v1000_debug("\r\n test_read_flash_and_v1000_debug \r\n");
#if 0
			USART_To_USB_Send_Data(0x55);		
			USART_To_USB_Send_Data(0x01);		
			USART_To_USB_Send_Data((u8)((secremain>>8)&0xff));		
			USART_To_USB_Send_Data((u8)(secremain&0xff));		
			    for(i =0;i<secremain;i++)			
			    	{
				    USART_To_USB_Send_Data(SPI_FLASH_BUF[i]);
						
			    	}
			cksum = Mycrc16(SPI_FLASH_BUF, secremain);
			USART_To_USB_Send_Data((u8)((cksum>>8)&0xff));		
			USART_To_USB_Send_Data((u8)(cksum&0xff));		
#endif		
			v1000_debug("\r\n NumByteToRead:%d,secremain:%d \r\n ",NumByteToRead,secremain);
			   if(NumByteToRead==secremain)
			   	{
			   //	  if(USART_Rx_ptr_in != ((USART_Rx_ptr_out+1)%2048))
			//	    if(USART_Rx_ptr_in == USART_Rx_ptr_out);	   
			   	  	{
						send_packge_to_usb(0x02,SPI_FLASH_BUF,secremain);//第一条语句
						send_status = 0;
						send_status_flag = 0;
						v1000_debug("\r  共有%d条轨迹记录，其中有%d组，兴趣点有%d个",NumByteToRead/32,t_number,a_number);
			   	  	}
					 break;//写入结束了
			   	}
			   else//写入未结束
				{
			//		    if(USART_Rx_ptr_in == USART_Rx_ptr_out);	   
					send_packge_to_usb(0x01,SPI_FLASH_BUF,secremain);//第一条语句
					secpos++;//扇区地址增1
					secoff=0;//偏移位置为0 	 
					WriteAddr+=secremain;//写地址偏移	   
				   	NumByteToRead-=secremain;				//字节数递减
					if(NumByteToRead>4096)secremain=4096;	//下一个扇区还是写不完
					else secremain=NumByteToRead;			//下一个扇区可以写完了
					

				}	
			}
         break;
		}
}





void read_gps(void)
{
	gps_v1000_debug_status = 1;
	send_status = 0;
	USART_Rx_ptr_out = USART_Rx_ptr_in;
	send_status_flag = 1;

}

void read_gps_next(void)
{

	send_status_flag = 1;
}

void stop_read_gps(void)
{
	gps_v1000_debug_status = 0;
	send_status = 0;
	send_status_flag = 0;

}

//read_last_sector()
void read_last_sector(void)
{
    u32 writeaddr = 0;
//	extern u32 Flash_Index;
	SPI_Flash_Read(SPI_FLASH_BUF,0x1FF000,1024);//读出整个扇区的内容
//	v1000_debug("\r  共有%d条轨迹记录，其中有%d组，兴趣点有%d个",Flash_Index/32,t_number,a_number);
	v1000_debug("\r\n read change vaule \r\n");
    for(writeaddr = 0;writeaddr<1024;writeaddr++)
    {
        if(writeaddr%32 == 0)
        v1000_debug("\r\n");
        v1000_debug(" %x",SPI_FLASH_BUF[writeaddr]);
    }
}

#endif

#if 1


void restore_pass(void)
{
    u16 i ;
    extern int16_t temperature_pass[];
    extern int16_t speed_pass[];
    extern s32 pressure_pass[];
    extern s32 Altitude_pass[];
    extern u8 old_div_cnt;
    extern u8 old_spped_div_cnt ;
    extern u8 old_altiude_div_cnt ;


    extern s32  most_Altitude ;
    extern s32 lest_Altitude ;
    extern int16_t most_temperature ;
    extern int16_t lest_temperature ;
    extern int16_t most_speed ;
    extern int16_t lest_speed ;
    extern s32 most_pressure ;
    extern s32 lest_pressure ;
    u8 SPI_FLASH_BUF[1154];

    SPI_Flash_Read(SPI_FLASH_BUF,CHANGE_PRE_VAULE_SAVE_ADDRES,1152);//读出整个扇区的内容

    if(old_div_cnt<=96)
    {
        for(i = 0;i<96;i++)
        {
            temperature_pass[i] = ((SPI_FLASH_BUF[384+(i*2)])|(SPI_FLASH_BUF[385+(i*2)]<<8));
   
        }

   
        for(i = 0;i<96;i++)
        {
            pressure_pass[i] = ((SPI_FLASH_BUF[0+(i*4)])|(SPI_FLASH_BUF[1+(i*4)]<<8)|(SPI_FLASH_BUF[2+(i*4)]<<16)|(SPI_FLASH_BUF[3+(i*4)]<<24));
        }
    }

    if(old_spped_div_cnt <= 96 )
    {

        for(i = 0;i<96;i++)
        {
            speed_pass[i] = (SPI_FLASH_BUF[576+(i*2)])|(SPI_FLASH_BUF[577+(i*2)]<<8);

        }
    }
    
    if(old_altiude_div_cnt <= 96)
    {
        for(i = 0;i<96;i++)
        {
            //Altitude_pass[i] = (SPI_FLASH_BUF[768+(i*2)])|(SPI_FLASH_BUF[769+(i*2)]<<8);
           Altitude_pass[i] = ((SPI_FLASH_BUF[768+(i*4)])|(SPI_FLASH_BUF[769+(i*4)]<<8)|(SPI_FLASH_BUF[770+(i*4)]<<16)|(SPI_FLASH_BUF[771+(i*4)]<<24));
        }


    }


    most_temperature = Max_arry(temperature_pass,old_div_cnt);
    lest_temperature = Min_arry(temperature_pass,old_div_cnt);

    most_pressure= Max_arry32(pressure_pass,old_div_cnt);
    lest_pressure= Min_arry32(pressure_pass,old_div_cnt);	

    most_Altitude= Max_arry32(Altitude_pass,old_altiude_div_cnt);
    lest_Altitude = Min_arry32(Altitude_pass,old_altiude_div_cnt);

    most_speed= Max_arry(speed_pass,old_spped_div_cnt);
    lest_speed = Min_arry(speed_pass,old_spped_div_cnt);

}



#endif


