#include "Myiic.h"
#include "Bmp085.h"
#include "delay.h" 
#include "string.h"	
#include "oled.h"	
#include "math.h"
#include "usart.h"
#include "gps.h"
#include "menutal.h"

#define	BMP085_SlaveAddress   0xee	  //定义器件在IIC总线中的从地址                               

#define OSS 0	// Oversampling Setting (note: code is not set up to use other OSS values)
							   
 								//打印缓存器
u8 ge,shi,bai,qian,wan,shiwan;           //显示变量

short ac1;
short ac2; 
short ac3; 
u16 ac4;
u16 ac5;
u16 ac6;
short b1; 
short b2;
short mb;
short mc;
short md;

u8 cy;

int  temperature = 0xffff;
long  pressure = 0xffffffff;;
long Altitude = 0xffff;;											//海拔高度
	

void  Multiple_Read(u8,u8);                                          //连续的读取内部寄存器数据
//------------------------------------
void BMP085_Start(void);
void BMP085_Stop(void);
void BMP085_SendACK(u8 ack);
u8  BMP085_RecvACK(void);
void BMP085_SendByte(u8 txd);
u8 BMP085_RecvByte(void);
void BMP085_ReadPage(void);
void BMP085_WritePage(void);
//-----------------------------------

//*********************************************************
void conversion(long temp_data)  
{  
    
    shiwan=temp_data/100000+0x30 ;
    temp_data=temp_data%100000;   //取余运算 
    wan=temp_data/10000+0x30 ;
    temp_data=temp_data%10000;   //取余运算
	qian=temp_data/1000+0x30 ;
    temp_data=temp_data%1000;    //取余运算
    bai=temp_data/100+0x30   ;
    temp_data=temp_data%100;     //取余运算
    shi=temp_data/10+0x30    ;
    temp_data=temp_data%10;      //取余运算
    ge=temp_data+0x30; 	
}



/**************************************
起始信号
**************************************/
void BMP085_Start()
{
	IIC_Start();
}

/**************************************
停止信号
**************************************/
void BMP085_Stop()
{
	IIC_Stop(); 
}

/**************************************
发送应答信号
入口参数:ack (0:ACK 1:NAK)
**************************************/
void BMP085_SendACK(u8 ack)
{
// if(ack == 0)
//	IIC_Ack();
 //else
 //	IIC_NAck();
	 SDA_OUT();      //SDA设置为输入  
	 IIC_SDA = ack;
     IIC_SCL = 1;
	 delay_us(5);
     IIC_SCL = 0;
	 delay_us(5);
}

/**************************************
接收应答信号
**************************************/
u8 BMP085_RecvACK()
{

#if 0
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
#endif
	SDA_IN();      //SDA设置为输入  
    IIC_SCL = 1;
    delay_us(5);
	cy =IIC_SDA ;
	IIC_SCL = 0;
    delay_us(5);
	return cy;
}

/**************************************
向IIC总线发送一个字节数据
**************************************/
void BMP085_SendByte(u8 txd)
{

#if 0
    u8 i;
	SDA_OUT();      //SDA设置为输入  
    for (i=0; i<8; i++)         //8位计数器
    { 
        txd cy;               //送数据口
        IIC_SCL = 1;                //拉高时钟线
        delay_us(5);             //延时
        IIC_SCL = 0;                //拉低时钟线
        delay_us(5);            //延时
    }
    BMP085_RecvACK(); 
	#endif

	IIC_Send_Byte(txd);
	BMP085_RecvACK();
}

/**************************************
从IIC总线接收一个字节数据
**************************************/
u8 BMP085_RecvByte(void)
{
 u8 dat;
#if 0
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
#endif
	dat =IIC_Read_Byte();
    return  dat;
	
}
/*
//单字节写入BMP085内部数据*******************************

void Single_Write(uchar SlaveAddress,uchar REG_Address,uchar REG_data)
{
    BMP085_Start();                  //起始信号
    BMP085_SendByte(SlaveAddress);   //发送设备地址+写信号
    BMP085_SendByte(REG_Address);    //内部寄存器地址
    BMP085_SendByte(REG_data);       //内部寄存器数据
    BMP085_Stop();                   //发送停止信号
}
*/
/*
//单字节读取BMP085内部数据********************************
uchar Single_Read(uchar REG_Address)
{  uchar REG_data;
    BMP085_Start();                          //起始信号
    BMP085_SendByte(BMP085_SlaveAddress);           //发送设备地址+写信号
    BMP085_SendByte(REG_Address);            //发送存储单元地址	
    BMP085_Start();                          //起始信号
    BMP085_SendByte(BMP085_SlaveAddress+1);         //发送设备地址+读信号
    REG_data=BMP085_RecvByte();              //读出寄存器数据
	BMP085_SendACK(1);   
	BMP085_Stop();                           //停止信号
    return REG_data; 
}
*/
//*********************************************************
//读出BMP085内部数据,连续两个
//*********************************************************
short Multiple_read(u8 ST_Address)
{   
	u8 msb, lsb;
	short _data;
    BMP085_Start();                          //起始信号
    BMP085_SendByte(BMP085_SlaveAddress);    //发送设备地址+写信号
    BMP085_SendByte(ST_Address);             //发送存储单元地址
    BMP085_Start();                          //起始信号
    BMP085_SendByte(BMP085_SlaveAddress+1);         //发送设备地址+读信号

    msb = BMP085_RecvByte();                 //BUF[0]存储
    BMP085_SendACK(0);                       //回应ACK
    lsb = BMP085_RecvByte();     
	BMP085_SendACK(1);                       //最后一个数据需要回NOACK

    BMP085_Stop();                           //停止信号
	delay_ms(5);
    _data = msb << 8;
	_data |= lsb;	
	return _data;
}
//********************************************************************
long bmp085ReadTemp(void)
{
long temp = 0;
    BMP085_Start();                  //起始信号
    BMP085_SendByte(BMP085_SlaveAddress);   //发送设备地址+写信号
    BMP085_SendByte(0xF4);	          // write register address
    BMP085_SendByte(0x2E);       	// write register data for temp
    BMP085_Stop();                   //发送停止信号
	delay_ms(5);  	// max time is 4.5ms
	temp = Multiple_read(0xF6);
//	v1000_debug("\r\n temp : %x \n\n",temp);
	temp &= 0x0000FFFF;
	return temp;
}
//*************************************************************
long bmp085ReadPressure(void)
{
	long pressure = 0;

    BMP085_Start();                   //起始信号
    BMP085_SendByte(BMP085_SlaveAddress);   //发送设备地址+写信号
    BMP085_SendByte(0xF4);	          // write register address
    BMP085_SendByte(0x34);       	  // write register data for pressure
    BMP085_Stop();                    //发送停止信号
	delay_ms(5);    	                  // max time is 4.5ms
	
	pressure = Multiple_read(0xF6);
	pressure &= 0x0000FFFF;
	
	return pressure;	
	//return (long) bmp085ReadShort(0xF6);
}

//**************************************************************

//初始化BMP085，根据需要请参考pdf进行修改**************
void Init_BMP085(void)
{

//	delay_ms(50);
//	v1000_debug("\nInit_BMP085!\n");
	ac1 = Multiple_read(0xAA);
	ac2 = Multiple_read(0xAC);
	ac3 = Multiple_read(0xAE);
	ac4 = Multiple_read(0xB0);
	ac5 = Multiple_read(0xB2);
	ac6 = Multiple_read(0xB4);
	b1 =  Multiple_read(0xB6);
	b2 =  Multiple_read(0xB8);
	mb =  Multiple_read(0xBA);
	mc =  Multiple_read(0xBC);
	md =  Multiple_read(0xBE);

//	v1000_debug("\r\nBMP085   ac1 :%d,ac2 :%d,ac3 :%d,ac4 :%d,ac5 :%d,ac6 :%d,b1 :%d,b2 :%d, mb :%d, mc :%d, md :%d!\n",
//		ac1,ac2,ac3,ac4,ac5,ac6,b1,b2,mb,mc,md);
}




//***********************************************************************


void bmp085Convert(void)
{
	long ut;
	long up;
	long x1, x2, b5, b6, x3, b3, p;
	unsigned long b4, b7;
	long pressure_new;

//	u8 dtbuf_jian[50];  
	
	ut = bmp085ReadTemp();
	//ut = bmp085ReadTemp();	   // 读取温度
	up = bmp085ReadPressure();
//	up = bmp085ReadPressure();  // 读取压强
	
	x1 = ((long)ut - ac6) * ac5 >> 15;
	x2 = ((long) mc << 11) / (x1 + md);
	b5 = x1 + x2;

//	 if(temperature == 0xffffffff)	 	
    temperature = (b5 + 8) >> 4;


    if(system_flag_table->run_mode == 1)
    {
    	temperature = temperature-(temperature*0.22);
    }
    else
    {
    	temperature = temperature-(temperature*0.1);
    }

//	 else
	 	{
	//	temperature = (temperature +((b5 + 8) >> 4))/2;
		// temperature = (temperature+temperature_new)/2;
	 	}
#if 0
	 //*************

	 conversion(temperature);
	v1000_debug("\r\ntemperature : %ld \n\n",temperature);
	 sprintf((char *)dtbuf_jian,"T:  %c%c.%cC",bai,shi,ge);	//得到经度字符串
	 OLED_ShowString(0,16,dtbuf_jian);	 	
     //*************
 #endif
	
	b6 = b5 - 4000;
	x1 = (b2 * (b6 * b6 >> 12)) >> 11;
	x2 = ac2 * b6 >> 11;
	x3 = x1 + x2;
	b3 = (((long)ac1 * 4 + x3) + 2)/4;
	x1 = ac3 * b6 >> 13;
	x2 = (b1 * (b6 * b6 >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (ac4 * (unsigned long) (x3 + 32768)) >> 15;
	b7 = ((unsigned long) up - b3) * (50000 >> OSS);
	if( b7 < 0x80000000)
	     p = (b7 * 2) / b4 ;
           else  
		    p = (b7 / b4) * 2;
	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	 if(pressure == 0xffffffff)	 
	 	pressure = p + ((x1 + x2 + 3791) >> 4);
	 else
	 	{
	 	pressure_new = (pressure +(p + ((x1 + x2 + 3791) >> 4)))/2;	 	
		 pressure = (pressure+pressure_new)/2;
	 	}
//	 conversion(pressure);

//	  v1000_debug("\r\n 2: %ld!\n\n",pressure);
//	 sprintf((char *)dtbuf_jian,"P: %c%c%c%c.%c%chpa",shiwan,wan,qian,bai,shi,ge);	//得到经度字符串
//	 OLED_ShowString(0,48,dtbuf_jian);	 	
	
}

/****************************************************************
函数名称：BMP085_Get_Altitude			    
函数功能：获取海拔高度值
入口参数：无
出口参数：altitude //int型  2字节，当前海拔高度值
备 注：	 返回的高度值单位为厘米，调用时再换算成带小数的以米为单位的高度值
*****************************************************************/
int BMP085_Get_Altitude(void)
{
	float pressure_tmep,pp;
	float altitude;
//	u8 dtbuf_jian[50]; 
	//v1000_debug("\r\n pressure: %ld!\n\n",pressure);
	pressure_tmep = (float)pressure;
	pp =(pressure_tmep/101325);
//	v1000_debug("\r\n pressure_tmep: %f m \n\n",pressure_tmep);
//	v1000_debug("\r\n pp: %f m \n\n",pp);
	pressure_tmep = (1/5.255);	
//	v1000_debug("\r\n 1/5.255: %f m \n\n",pressure_tmep);
	pressure_tmep = pow(pp,pressure_tmep);				//获取气压值
//	v1000_debug("\r\n pressure_tmep: %f m \n\n",pressure_tmep);
	altitude=44330*(1-pressure_tmep);  	//根据芯片手册提供的公式计算海拔高度
	altitude*=10;	 		//转换成厘米单位的高度值，调用时再换算成带小数的高度值，提高精度
//	if(altitude<0)
//		altitude	
//	conversion(altitude);
	
//	sprintf((char *)dtbuf_jian,"H: +%c%c%c.%c%cm",wan,qian,bai,shi,ge);	//得到经度字符串
//		
//	OLED_ShowString(0,32,dtbuf_jian);	 
	//v1000_debug("\r\n H: %f m \n\n",altitude);
	return (int)altitude;
}


/****************************************************************
　　　函数名称：ConvAltitude()			    
　　　函数功能：转换海拔高度
　　　入口参数：无
　　　出口参数：无   
　　　备 注：将得到数据转化成lcd的数据，便于显示	 
*****************************************************************/
void ConvAltitude(void)
{

    extern u8 guji_mode;

    if((guji_mode > RECORED_IDLE)&&(gpsx->fixmode != 3))
        return;    

	Altitude = BMP085_Get_Altitude();

    if(system_flag_table->ajust_Arr_flag == 1)
    {
        Altitude = Altitude - (system_flag_table->ajust_arr*10);
    }
    else
    {
     	  if(system_flag_table->run_mode == 1)
    	  {
    	      if(gpsx->fixmode == 3)  
    	      {
  		          Altitude = gpsx->altitude;
    	      }
    	  }
  
    }
	  		  	
#if 0
	if(gpsx->fixmode == 3)  
		{
		if((gpsx->speed/1000>400)||((gpsx->altitude/10)>8000))	  
			Altitude = (gpsx->altitude/10)*100;
		}
#endif	
	//v1000_debug("system_flag_table->ajust_arr: %d m \n\n",system_flag_table->ajust_arr);
	//Altitude=(Altitude+BMP085_Get_Altitude())/2;
/**************取10次测量值的平均值作为测量給峁怂惴ù嬖诩且湫Ч�*************/

#if 0
		conversion(Altitude);
		v1000_debug("\r\n H: %ld m \n\n",Altitude);
		if(flag ==0)
			sprintf((char *)dtbuf_jian,"H: +%c%c%c%c.%cm",wan,qian,bai,shi,ge);	//得到经度字符串		
		else	
			sprintf((char *)dtbuf_jian,"H: -%c%c%c%c.%cm",wan,qian,bai,shi,ge);	//得到经度字符串		
		OLED_ShowString(0,32,dtbuf_jian);

	AltitudeTempFlag++;									//标志自加
	if(AltitudeTempFlag>=10)
		{
		AltitudeTempFlag=0;		
		Tempnum/=10;//计算出平均值
		conversion(Tempnum);	
		v1000_debug("\r\n H: %ld m \n\n",Tempnum);
		Tempnum = 0;
		if(flag ==0)
			sprintf((char *)dtbuf_jian,"H: +%c%c%c.%c%cm",wan,qian,bai,shi,ge);	//得到经度字符串		
		else	
			sprintf((char *)dtbuf_jian,"H: -%c%c%c.%c%cm",wan,qian,bai,shi,ge);	//得到经度字符串		
		}
	
		Tempnum+=Altitude;			//根据标志保存数据到数组

#endif



}
//*********************************************************
//******主程序********
//*********************************************************
void BMP085_get_process(void)
{ 
	bmp085Convert();
	ConvAltitude();
} 

