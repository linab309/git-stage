#ifndef __BIGFONT_H
#define __BIGFONT_H  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������-��չʵ��3
//OLED������ʾ ����			 
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/8/22
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  


void OLED_ShowChar22(u8 x,u8 y,u8 chr,u8 mode);
void OLED_ShowNum22(u8 x,u8 y,u32 num,u8 len);
void OLED_ShowFont22(u8 x,u8 y,u8 fno,u8 mode);
void OLED_ShowString22(u8 x,u8 y,const u8 *p);


#endif
