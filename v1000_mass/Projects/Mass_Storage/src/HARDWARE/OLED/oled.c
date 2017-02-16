#include "oled.h"
#include "stdlib.h"
#include "font.h"  	 
#include "delay.h"
#include "bigfont.h"
#include "usart.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//SSD1306 OLED 驱动IC驱动代码
//驱动方式:8080并口/4线串口
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  
 
#define Deagel_180

//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 		   
u8 OLED_GRAM[128][8];	 

#define  OLED_X 128
#define  OLED_Y 64

extern u8 system_power_flag;


//更新显存到LCD		 
void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
		for(i=0;i<8;i++)  
		{  
			OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
			OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
			OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
			for(n=0;n<128;n++)
				{
			//	 if((system_power_flag == 2)&&(i<6))
				 	{
			//		 	OLED_WR_Byte(0x00,OLED_DATA); 
				 	}
			//	 else
				 	{
//				 	if((system_power_flag == 2)&&(n>=39)&&(system_flag_table->run_mode == 0))
				 		{
	//			 		OLED_WR_Byte(0x00,OLED_DATA); 
				 		}
		//			else
						OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
				 	}
				}
		}   

}


//更新显存到LCD		 
void OLED_Refresh_Gram_low(void)
{
	u8 i,n;		    
		for(i=2;i<6;i++)  
		{  
			OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
			OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
			OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
			for(n=0;n<128;n++)
				{
						OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
				 	
				}
		}   

}
#if OLED_MODE==1
//向SSD1306写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat,u8 cmd)
{
	DATAOUT(dat);	    
 	OLED_RS=cmd;
	OLED_CS=0;	   
	OLED_WR=0;	 
	OLED_WR=1;
	OLED_CS=1;	  
	OLED_RS=1;	 
} 	    	    
#else
//向SSD1306写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat,u8 cmd)
{	
	u8 i;			  
	OLED_RS=cmd; //写命令 
	OLED_CS=0;		  
	for(i=0;i<8;i++)
	{			  
		OLED_SCLK=0;
		if(dat&0x80)OLED_SDIN=1;
		else OLED_SDIN=0;
		OLED_SCLK=1;
		dat<<=1;   
	}				 
	OLED_CS=1;		  
	OLED_RS=1;   	  
} 
#endif
	  	  
//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();//更新显示
}
//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//超出范围了.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}


/*********************************************************************
*
*       LCD_L0_DrawHLine
*/
void OLED_DrawHLine  (int x0, int y,  int x1) {

    for (; x0 <= x1; x0++) {
      OLED_DrawPoint(x0, y, 1);
    }

}

/*********************************************************************
*
*       LCD_L0_DrawVLine
*/
void OLED_DrawVLine  (int x, int y0,  int y1) {

    for (; y0 <= y1; y0++) {
      OLED_DrawPoint(x, y0, 1);
    }

}


void OLED_DrawVLine_visual(int x, int y0,  int y1) 
	{

    for (; y0 <= y1; y0++) {
      OLED_DrawPoint(x, y0, 1);
	y0++; 
    }

}



//这两个参数定义动画每帧的长宽。
//#define MOVIE_XSIZE	100 
//#define MOVIE_YSIZE 40  
//#define FRAME_SIZE MOVIE_XSIZE*MOVIE_YSIZE/8   
							  
//x:0~127
//y:0~63
//picx:图片编号		 
void Movie_Show_Img(u16 x,u16 y,const u8 *icon)
{      			    
	u8 x1, y1;
	u16 t,t1,FRAME_SIZE;
	u16 x0=x;
	u8 temp;
	x1 = icon[0];
	y1 = icon[1];
	if(x1 == 12)
	     FRAME_SIZE =  x1*y1/6;		    				   
	else
	    FRAME_SIZE =  x1*y1/8;		    				   
   	for(t=0;t<FRAME_SIZE;t++)
      {    
		temp=icon[t+3];  //得到图片的一个字节数据	                          
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,1); 
			else OLED_DrawPoint(x,y,0);		   
			temp<<=1;
			x++;
			if(x==(x1+x0))
			{
				x=x0;
				y++;
				break;
			}
		}  	 
    }          
}

void Movie_Show_Img_3(u16 x,u16 y,const u8 *icon)
{      			    
	u8 x1, y1;
	u16 t,t1,FRAME_SIZE;
	u16 x0=x;
	u8 temp;
	x1 = icon[0];
	y1 = icon[1];
	FRAME_SIZE =  x1*y1/3   	;		    				   
   	for(t=0;t<FRAME_SIZE;t++)
      {    
		temp=icon[t+3];  //得到图片的一个字节数据	                          
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,1); 
			else OLED_DrawPoint(x,y,0);		   
			temp<<=1;
			x++;
			if(x==(x1+x0))
			{
				x=x0;
				y++;
				break;
			}
		}  	 
    }          
}
void Movie_Show_Img_force1616(u16 x,u16 y,u8 index)
{      			    
	u8 x1, y1;
//	extern const u8 bmp_destination_icon[][];
	u16 t,t1,FRAME_SIZE;
	u16 y0=y;
	u8 temp;
	x1 =16;
	y1 =16;
	FRAME_SIZE =  x1*y1/8   	;		    				   
   	for(t=0;t<FRAME_SIZE;t++)
      {    
		temp=bmp_destination_icon[index][t];  //得到图片的一个字节数据	                          
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,1); 
			else OLED_DrawPoint(x,y,0);		   
			temp<<=1;
			y++;
			if(y==(y1+y0))
			{
				y=y0;
				x++;
				break;
			}
		}  	 


    }          
}

//#define MAX_CHAR_POSX 122
//#define MAX_CHAR_POSY 58        

void OLED_ShowString_24(u8 x,u8 y,const u8 *p)
{

#define MAX_CHAR_POSX 112
#define MAX_CHAR_POSY 50          
    while(*p!='\0')
    {       
        if(x>MAX_CHAR_POSX){x=0;y+=24;}
        if(y>MAX_CHAR_POSY){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,42,1);	 
        x+=16;
        p++;
    }  
	
}

void OLED_ShowString_head(u8 x,u8 y,const u8 *p)
{
     
    while(*p!='\0')
    {       
        if(x>122){x=0;y+=16;}
        if(y>58){y=x=0;OLED_Clear();}
	 if(0)//x == 0)
	 	{
	 	OLED_ShowChar_head(x,y,*p,16,1);
		 x+=7;
	 	}
	 else
	 	{
		 if(*p == ':')
			  x-=2;	
		    OLED_ShowChar(x,y,*p,16,1);
		 if(*p == ':')
		 	{
			  x-=6;	
			  p--;
			 OLED_ShowChar(x,y,*p,16,1);	
			 x+=6;
			 p++;
		 	}
	 	
		 if(*p == ':')	
	    	    x+=6;
		 else
		 	  x+=8;
	 	}
        p++;
    }  
}
//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
//	OLED_Refresh_Gram();//更新显示
}

//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	  
void OLED_Fill_icon(u8 x1,u8 y1,u8 *icon)  
{  
	u8 x,y,i = 0;  
	for(x=x1;x<OLED_X;x++)
	{
		for(y=y1;y<=OLED_Y;y++)
			{
			if(icon[i] == '\0')
				break;
			OLED_DrawPoint(x,y,icon[i]);
				i++;
			}
	}													    
	OLED_Refresh_Gram();//更新显示
}

#if 0
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 x0=x;
	chr=chr-' ';//得到偏移后的值				   
    for(t=0;t<28;t++)
    {   
		if(size==12)temp=asc2_1206[chr][t];  //调用1206字体
		else temp=asc2_1314[chr][t];		 //调用1608字体 	                          
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			x++;
			if((x-x0)==size)
			{
				x=x0;
				y++;
				break;
			}
		}  	 
    }          
}
#endif

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp = 0,t,t1;
	u8 y0=y;
	if(size == 42)
		chr=chr-'0';//得到偏移后的值				   		
	else
		chr=chr-' ';//得到偏移后的值				   
    for(t=0;t<size;t++)
    {   
		if(size==12)temp=asc2_1206[chr][t];  //调用1206字体
		else if(size == 16) temp=asc2_1608[chr][t];		 //调用1608字体 	                          
		else if(size == 42)
			{
			temp=asc2_1242[chr][t];		 //调用1608字体 	                          

			}

        //v1000_debug("chr :%x temp :%x \r\n",chr,temp);
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if(size == 42)
				{
				if((y-y0)==24)
					{
					y=y0;
					x++;
					break;
					}
				}  	 
			else
				{
				if((y-y0)==size)
					{
					y=y0;
					x++;
					break;
					}
				}
        	}
    }          
}


//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar_head(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	chr=chr-' ';//得到偏移后的值				   
    for(t=2;t<size;t++)
    {   
		if(size==12)temp=asc2_1206[chr][t];  //调用1206字体
		else temp=asc2_1608[chr][t];		 //调用1608字体 	                          

        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}

//显示字符串
//x,y:起点坐标  
//*p:字符串起始地址
//用12字体
void OLED_ShowString12(u8 x,u8 y,const u8 *p)
{
//#define MAX_CHAR_POSX 124
//#define MAX_CHAR_POSY 62          
    while(*p!='\0')
    {       
        if(x>124){x=0;y+=12;}
        if(y>62){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,12,1);	 
        x+=6;
        p++;
    }  
}	


//m^n函数
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 

#if 0
//显示字符串
//x,y:起点坐标  
//*p:字符串起始地址
//用16字体  13*14
void OLED_ShowString(u8 x,u8 y,const u8 *p)
{
#define MAX_CHAR_POSX 115
#define MAX_CHAR_POSY 50          
    while(*p!='\0')
    {       
        if(x>MAX_CHAR_POSX){x=0;y+=14;}
        if(y>MAX_CHAR_POSY){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,13,1);	 
        x+=13;
        p++;
    }  
}	   
#endif

//显示字符串
//x,y:起点坐标  
//*p:字符串起始地址
//用16字体
void OLED_ShowString(u8 x,u8 y,const u8 *p)
{
//#define MAX_CHAR_POSX 122
//#define MAX_CHAR_POSY 58          
    while(*p!='\0')
    {       
        if(x>122){x=0;y+=16;}
        if(y>58){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,16,1);	 
        x+=8;
        p++;
    }  
}	
//初始化SSD1306					    
void OLED_Init(void)
{ 	

 	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC->APB2ENR|=1<<3;    //使能PORTB时钟 
//	RCC->APB2ENR|=1<<4;    //使能PORTC时钟 	  
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC,ENABLE);
#if OLED_MODE==1					 
 	JTAG_Set(SWD_ENABLE);
#if 0
	GPIOB->CRL=0X33333333;
	GPIOB->ODR|=0XFFFF;								    	 
 
 	GPIOC->CRH&=0XFFFFFF00;
	GPIOC->CRL&=0X00FFFFFF;
 	GPIOC->CRH|=0X00000033;
	GPIOC->CRL|=0X33000000;
	GPIOC->ODR|=0X03C0;
#endif


	//init GPIOA.13,GPIOA.15  上拉输入
 //	RCC_APB2PeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	GPIOB->ODR|=0X7cFF;		
	//GPIOB->ODR|=0X03C0;
//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

 //	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	OLED_RST = 1;
	delay_ms(50);
	OLED_RST = 0;
	delay_ms(50);
	OLED_RST = 1;
#else
	GPIOB->CRL&=0XFFFFFF00;
	GPIOB->CRL|=0XF0000033;
	GPIOB->ODR|=0X03;

 	GPIOC->CRH&=0XFFFFFF00;	   
 	GPIOC->CRH|=0X00000033;	 
	GPIOC->ODR|=3<<8;
#endif
								  
	//OLED_RST=0;
	//delay_ms(100);
	//OLED_RST=1; 
					  
	OLED_WR_Byte(0xAE,OLED_CMD); //关闭显示
	OLED_WR_Byte(0xD5,OLED_CMD); //设置时钟分频因子,震荡频率
	OLED_WR_Byte(0x80,OLED_CMD);   //[3:0],分频因子;[7:4],震荡频率
	OLED_WR_Byte(0xA8,OLED_CMD); //设置驱动路数
	OLED_WR_Byte(0X3F,OLED_CMD); //默认0X3F(1/64) 
	OLED_WR_Byte(0xD3,OLED_CMD); //设置显示偏移
	OLED_WR_Byte(0X00,OLED_CMD); //默认为0

	OLED_WR_Byte(0x40,OLED_CMD); //设置显示开始行 [5:0],行数.
													    
	OLED_WR_Byte(0x8D,OLED_CMD); //电荷泵设置
	OLED_WR_Byte(0x14,OLED_CMD); //bit2，开启/关闭
	OLED_WR_Byte(0x20,OLED_CMD); //设置内存地址模式
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
#ifdef Deagel_180
	OLED_WR_Byte(0xA0,OLED_CMD); //?????¨ò?éè??,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC8,OLED_CMD); //éè??COMé¨?è·??ò;bit3:0,??í¨?￡ê?;1,???¨ò??￡ê? COM[N-1]->COM0;N:?y?ˉ?·êy
#else	
	OLED_WR_Byte(0xA1,OLED_CMD); //段重定义设置,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC0,OLED_CMD); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
#endif

	OLED_WR_Byte(0xDA,OLED_CMD); //设置COM硬件引脚配置
	OLED_WR_Byte(0x12,OLED_CMD); //[5:4]配置
		 
	OLED_WR_Byte(0x81,OLED_CMD); //对比度设置
	OLED_WR_Byte(0xfF,OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)
	OLED_WR_Byte(0xD9,OLED_CMD); //设置预充电周期
	OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD); //设置VCOMH 电压倍率
	OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	OLED_WR_Byte(0xA6,OLED_CMD); //设置显示方式;bit0:1,反相显示;0,正常显示	    						   
	OLED_WR_Byte(0xAF,OLED_CMD); //开启显示	 
	OLED_Clear();
}  

























