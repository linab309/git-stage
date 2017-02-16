#ifndef __BIGFONT_H
#define __BIGFONT_H  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板-扩展实验3
//OLED汉字显示 代码			 
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/8/22
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  


void OLED_ShowChar22(u8 x,u8 y,u8 chr,u8 mode);
void OLED_ShowNum22(u8 x,u8 y,u32 num,u8 len);
void OLED_ShowFont22(u8 x,u8 y,u8 fno,u8 mode);
void OLED_ShowString22(u8 x,u8 y,const u8 *p);


#endif
