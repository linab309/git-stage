#include "Myiic.h"
#include "Bmp085.h"
#include "delay.h" 
#include "string.h"	
#include "oled.h"	
#include "math.h"
#include "usart.h"
#include "gps.h"
#include "menutal.h"
#include "bmp280.h"

#define	BMP085_SlaveAddress   0xee	  /*定义器件在IIC总线中的从地址*/

#define OSS 0	// Oversampling Setting (note: code is not set up to use other OSS values)
							   

#ifdef NEED_TO_LEDSHOW 								
u8 ge,shi,bai,qian,wan,shiwan;           //显示变量
#endif

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
long Altitude = 0xffff;;											//���θ߶�
	
/*bmp type define */

#define BMP180_TYPE 0
#define BMP280_TYPE 1

u8 bmp_mode_type = BMP180_TYPE;

/**********************
 *  FUCTION CALL
 * 
 * ********************/

void  Multiple_Read(u8,u8);                                          //�����Ķ�ȡ�ڲ��Ĵ�������
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
#ifdef NEED_TO_LEDSHOW    
    shiwan=temp_data/100000+0x30 ;
    temp_data=temp_data%100000;   
    wan=temp_data/10000+0x30 ;
    temp_data=temp_data%10000;   
	qian=temp_data/1000+0x30 ;
    temp_data=temp_data%1000;    
    bai=temp_data/100+0x30   ;
    temp_data=temp_data%100;     
    shi=temp_data/10+0x30    ;
    temp_data=temp_data%10;      
    ge=temp_data+0x30;
#endif 	
}



/**************************************
��ʼ�ź�
**************************************/
void BMP085_Start()
{
	IIC_Start();
}

/**************************************
ֹͣ�ź�
**************************************/
void BMP085_Stop()
{
	IIC_Stop(); 
}

/**************************************
����Ӧ���ź�
��ڲ���:ack (0:ACK 1:NAK)
**************************************/
void BMP085_SendACK(u8 ack)
{
// if(ack == 0)
//	IIC_Ack();
 //else
 //	IIC_NAck();
	 SDA_OUT();      //SDA����Ϊ����  
	 IIC_SDA = ack;
     IIC_SCL = 1;
	 delay_us(15);
     IIC_SCL = 0;
	 delay_us(15);
}

/**************************************
����Ӧ���ź�
**************************************/
u8 BMP085_RecvACK()
{

#if 0
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
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
	IIC_SCL=0;//ʱ�����0 	   
	return 0;  
#endif
	SDA_IN();      //SDA����Ϊ����  
    IIC_SCL = 1;
    delay_us(15);
	cy =IIC_SDA ;
	IIC_SCL = 0;
    delay_us(15);
	return cy;
}

/**************************************
��IIC���߷���һ���ֽ�����
**************************************/
void BMP085_SendByte(u8 txd)
{
    u8 ack = 0;
#if 0
    u8 i;
	SDA_OUT();      //SDA����Ϊ����  
    for (i=0; i<8; i++)         //8λ������
    { 
        txd cy;               //�����ݿ�
        IIC_SCL = 1;                //����ʱ����
        delay_us(5);             //��ʱ
        IIC_SCL = 0;                //����ʱ����
        delay_us(5);            //��ʱ
    }
    BMP085_RecvACK(); 
	#endif

	IIC_Send_Byte(txd);
	ack = BMP085_RecvACK();
	printf("ack = %d \r\n",ack);
}

/**************************************
��IIC���߽���һ���ֽ�����
**************************************/
u8 BMP085_RecvByte(void)
{
 u8 dat;
#if 0
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
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
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
#endif
	dat =IIC_Read_Byte();
    return  dat;
	
}

//���ֽ�д��BMP085�ڲ�����*******************************

void arry_Write(int8_t SlaveAddress,int8_t REG_Address,uint8_t * REG_data,uint16_t REG_LEN)
{
	uint16_t i = 0;

	BMP085_Start();                  //��ʼ�ź�
	BMP085_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�
	BMP085_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ
	for(i = 0;i<REG_LEN;i++)
	{
		BMP085_SendByte(REG_data[i]);       //�ڲ��Ĵ�������
	}

	BMP085_Stop();                   //����ֹͣ�ź�
}


//���ֽڶ�ȡBMP085�ڲ�����********************************
int8_t arry_Read(int8_t Slave_Address,int8_t REG_Address,uint8_t * REG_data,uint16_t REG_LEN)
{  
	uint16_t i = 0;

	BMP085_Start();                          //��ʼ�ź�
	BMP085_SendByte(Slave_Address);           //�����豸��ַ+д�ź�
	BMP085_SendByte(REG_Address);            //���ʹ洢��Ԫ��ַ	
	BMP085_Start();                          //��ʼ�ź�
	BMP085_SendByte(Slave_Address+1);         //�����豸��ַ+���ź�
	for(i = 0;i<REG_LEN; i++)
	{
		REG_data[i]=BMP085_RecvByte();              //�����Ĵ�������
		BMP085_SendACK(0);   
	}
	BMP085_SendACK(1);   
	BMP085_Stop();                           //ֹͣ�ź�
	return 0; 
}

//*********************************************************
//����BMP085�ڲ�����,��������
//*********************************************************
short Multiple_read(u8 REG_Address)
{   
	u8 msb, lsb;
	short _data;
    BMP085_Start();                          //��ʼ�ź�
    BMP085_SendByte(BMP085_SlaveAddress);    //�����豸��ַ+д�ź�
    BMP085_SendByte(REG_Address);             //���ʹ洢��Ԫ��ַ
    BMP085_Start();                          //��ʼ�ź�
    BMP085_SendByte(BMP085_SlaveAddress+1);         //�����豸��ַ+���ź�

    msb = BMP085_RecvByte();                 //BUF[0]�洢
    BMP085_SendACK(0);                       //��ӦACK
    lsb = BMP085_RecvByte();     
	BMP085_SendACK(1);                       //���һ��������Ҫ��NOACK

    BMP085_Stop();                           //ֹͣ�ź�
	delay_ms(5);
    _data = msb << 8;
	_data |= lsb;	
	return _data;
}
//********************************************************************
long bmp085ReadTemp(void)
{
long temp = 0;
    BMP085_Start();                  //��ʼ�ź�
    BMP085_SendByte(BMP085_SlaveAddress);   //�����豸��ַ+д�ź�
    BMP085_SendByte(0xF4);	          // write register address
    BMP085_SendByte(0x2E);       	// write register data for temp
    BMP085_Stop();                   //����ֹͣ�ź�
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

    BMP085_Start();                   //��ʼ�ź�
    BMP085_SendByte(BMP085_SlaveAddress);   //�����豸��ַ+д�ź�
    BMP085_SendByte(0xF4);	          // write register address
    BMP085_SendByte(0x34);       	  // write register data for pressure
    BMP085_Stop();                    //����ֹͣ�ź�
	delay_ms(5);    	                  // max time is 4.5ms
	
	pressure = Multiple_read(0xF6);
	pressure &= 0x0000FFFF;
	
	return pressure;	
	//return (long) bmp085ReadShort(0xF6);
}

//**************************************************************

//��ʼ��BMP085��������Ҫ��ο�pdf�����޸�**************
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
	//ut = bmp085ReadTemp();	   // ��ȡ�¶�
	up = bmp085ReadPressure();
//	up = bmp085ReadPressure();  // ��ȡѹǿ
	
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
	 sprintf((char *)dtbuf_jian,"T:  %c%c.%cC",bai,shi,ge);	//�õ������ַ���
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
//	 sprintf((char *)dtbuf_jian,"P: %c%c%c%c.%c%chpa",shiwan,wan,qian,bai,shi,ge);	//�õ������ַ���
//	 OLED_ShowString(0,48,dtbuf_jian);	 	
	
}

/****************************************************************
�������ƣ�BMP085_Get_Altitude			    
�������ܣ���ȡ���θ߶�ֵ
��ڲ�������
���ڲ�����altitude //int��  2�ֽڣ���ǰ���θ߶�ֵ
�� ע��	 ���صĸ߶�ֵ��λΪ���ף�����ʱ�ٻ���ɴ�С��������Ϊ��λ�ĸ߶�ֵ
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
	pressure_tmep = pow(pp,pressure_tmep);				//��ȡ��ѹֵ
//	v1000_debug("\r\n pressure_tmep: %f m \n\n",pressure_tmep);
	altitude=44330*(1-pressure_tmep);  	//����оƬ�ֲ��ṩ�Ĺ�ʽ���㺣�θ߶�
	altitude*=10;	 		//ת�������׵�λ�ĸ߶�ֵ������ʱ�ٻ���ɴ�С���ĸ߶�ֵ����߾���
//	if(altitude<0)
//		altitude	
//	conversion(altitude);
	
//	sprintf((char *)dtbuf_jian,"H: +%c%c%c.%c%cm",wan,qian,bai,shi,ge);	//�õ������ַ���
//		
//	OLED_ShowString(0,32,dtbuf_jian);	 
	//v1000_debug("\r\n H: %f m \n\n",altitude);
	return (int)altitude;
}


/****************************************************************
�������������ƣ�ConvAltitude()			    
�������������ܣ�ת�����θ߶�
��������ڲ�������
���������ڲ�������   
�������� ע�����õ�����ת����lcd�����ݣ�������ʾ	 
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
/**************ȡ10�β���ֵ��ƽ��ֵ��Ϊ�����o�������㷨���ڼ���Ч��*************/

#if 0
		conversion(Altitude);
		v1000_debug("\r\n H: %ld m \n\n",Altitude);
		if(flag ==0)
			sprintf((char *)dtbuf_jian,"H: +%c%c%c%c.%cm",wan,qian,bai,shi,ge);	//�õ������ַ���		
		else	
			sprintf((char *)dtbuf_jian,"H: -%c%c%c%c.%cm",wan,qian,bai,shi,ge);	//�õ������ַ���		
		OLED_ShowString(0,32,dtbuf_jian);

	AltitudeTempFlag++;									//��־�Լ�
	if(AltitudeTempFlag>=10)
		{
		AltitudeTempFlag=0;		
		Tempnum/=10;//�����ƽ��ֵ
		conversion(Tempnum);	
		v1000_debug("\r\n H: %ld m \n\n",Tempnum);
		Tempnum = 0;
		if(flag ==0)
			sprintf((char *)dtbuf_jian,"H: +%c%c%c.%c%cm",wan,qian,bai,shi,ge);	//�õ������ַ���		
		else	
			sprintf((char *)dtbuf_jian,"H: -%c%c%c.%c%cm",wan,qian,bai,shi,ge);	//�õ������ַ���		
		}
	
		Tempnum+=Altitude;			//���ݱ�־�������ݵ�����

#endif



}



/*!
 *  @brief Function for writing the sensor's registers through I2C bus.
 *
 *  @param[in] i2c_addr : sensor I2C address.
 *  @param[in] reg_addr : Register address.
 *  @param[in] reg_data : Pointer to the data buffer whose value is to be written.
 *  @param[in] length   : No of bytes to write.
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval >0 -> Failure Info
 *
 */
int8_t i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length)
{
	int i = 0;
    /* Implement the I2C write routine according to the target machine. */\
	arry_Write(i2c_addr,reg_addr,reg_data,length);
	printf("i2c write :  i2c_addr=%x  ,reg_addr=%x length =%d \r\n", i2c_addr,reg_addr,length);
	for(i = 0;i<length;i++)
	{
		printf("%x ",reg_data[i]);
	}
	printf("\r\n");
    return 0;
}

/*!
 *  @brief Function for reading the sensor's registers through I2C bus.
 *
 *  @param[in] i2c_addr : Sensor I2C address.
 *  @param[in] reg_addr : Register address.
 *  @param[out] reg_data    : Pointer to the data buffer to store the read data.
 *  @param[in] length   : No of bytes to read.
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval >0 -> Failure Info
 *
 */
int8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length)
{
	int i = 0;
	arry_Read(i2c_addr,reg_addr,reg_data,length);
	printf("i2c read :  i2c_addr=%x  ,reg_addr=%x length =%d \r\n", i2c_addr,reg_addr,length);
	for(i = 0;i<length;i++)
	{
		printf("%x ",reg_data[i]);
	}
	printf("\r\n");
	/* Implement the I2C read routine according to the target machine. */
	return 0;
}


/*!
 *  @brief Prints the execution status of the APIs.
 *
 *  @param[in] api_name : name of the API whose execution status has to be printed.
 *  @param[in] rslt     : error code returned by the API whose execution status has to be printed.
 *
 *  @return void.
 */
void print_rslt(const char api_name[], int8_t rslt)
{
    if (rslt != BMP280_OK)
    {
        printf("%s\t", api_name);
        if (rslt == BMP280_E_NULL_PTR)
        {
            printf("Error [%d] : Null pointer error\r\n", rslt);
        }
        else if (rslt == BMP280_E_COMM_FAIL)
        {
            printf("Error [%d] : Bus communication failed\r\n", rslt);
        }
        else if (rslt == BMP280_E_IMPLAUS_TEMP)
        {
            printf("Error [%d] : Invalid Temperature\r\n", rslt);
        }
        else if (rslt == BMP280_E_DEV_NOT_FOUND)
        {
            printf("Error [%d] : Device not found\r\n", rslt);
        }
        else
        {
            /* For more error codes refer "*_defs.h" */
            printf("Error [%d] : Unknown error code\r\n", rslt);
        }
    }
}

struct bmp280_dev bmp;

void bmp280_read_pressure_tempreature(struct bmp280_dev _bmp)
{
	int32_t temp32;
	uint32_t pre32;
	int8_t rslt;
	uint8_t power_mode = 0;
	struct bmp280_config conf;
	
	struct bmp280_uncomp_data ucomp_data;

	/* Reading the raw data from sensor */
	rslt = bmp280_get_uncomp_data(&ucomp_data, &_bmp);

	/* Getting the compensated pressure using 32 bit precision */
	rslt = bmp280_get_comp_pres_32bit(&pre32, ucomp_data.uncomp_press, &_bmp);
	/* Getting the 32 bit compensated temperature */
	rslt = bmp280_get_comp_temp_32bit(&temp32, ucomp_data.uncomp_temp, &_bmp);

	temperature =(int)(temp32&0xffff);
	pressure = (long)pre32;
	printf("UT: %d, UP: %d,T32: %d, P32: %d, \r\n", ucomp_data.uncomp_temp,ucomp_data.uncomp_press, temp32, pre32);   

	
	rslt = bmp280_set_power_mode(BMP280_FORCED_MODE, &_bmp);
	print_rslt(" bmp280_set_power_mode status", rslt);	 
}

int bmp_moudle_preinit(void)
{
    int8_t rslt;
    struct bmp280_config conf;
	
    /* Map the delay function pointer with the function responsible for implementing the delay */
    bmp.delay_ms = delay_ms;

    /* Assign device I2C address based on the status of SDO pin (GND for PRIMARY(0x76) & VDD for SECONDARY(0x77)) */
    bmp.dev_id = BMP280_I2C_ADDR_V1000;

    /* Select the interface mode as I2C */
    bmp.intf = BMP280_I2C_INTF;

    /* Map the I2C read & write function pointer with the functions responsible for I2C bus transfer */
    bmp.read = i2c_reg_read;
    bmp.write = i2c_reg_write;

    /* To enable SPI interface: comment the above 4 lines and uncomment the below 4 lines */

    /*
     * bmp.dev_id = 0;
     * bmp.read = spi_reg_read;
     * bmp.write = spi_reg_write;
     * bmp.intf = BMP280_SPI_INTF;
     */
    rslt = bmp280_init(&bmp);
    if (rslt != BMP280_OK)
	{
		print_rslt(" bmp280_init status", rslt);
		bmp_mode_type = BMP180_TYPE;
		Init_BMP085();
	}        
    else
    {       
		rslt = bmp280_get_config(&conf, &bmp);
		printf(" bmp280 init OK");

		/* configuring the temperature oversampling, filter coefficient and output data rate */
		/* Overwrite the desired settings */
		conf.filter = BMP280_FILTER_COEFF_4;

		/* Pressure oversampling set at 4x */
		conf.os_pres = BMP280_OS_2X;
		conf.os_temp = BMP280_OS_2X;
		/* Setting the output data rate as 1HZ(1000ms) */
		conf.odr = BMP280_ODR_1000_MS;
		rslt = bmp280_set_config(&conf, &bmp);
		print_rslt(" bmp280_set_config status", rslt);
    	rslt = bmp280_set_power_mode(BMP280_NORMAL_MODE, &bmp);
		print_rslt(" bmp280_set_power_mode status", rslt);	 
		rslt = bmp280_get_config(&conf, &bmp);
		/* Always set the power mode after setting the configuration */
		bmp_mode_type = BMP280_TYPE;	
    }
    

    return 0;
}

//*********************************************************
//**************
//*********************************************************
void BMP085_get_process(void)
{ 
	struct bmp280_status status = {0};

	if(bmp_mode_type == BMP180_TYPE)
	{
		bmp085Convert();
		ConvAltitude();
	}
	else
	{

		bmp280_get_status(&status,&bmp);
		printf("status.measuring = %d \r\n",status.measuring);
		printf("status.im_update = %d \r\n",status.im_update);
		if((status.measuring == 0) &&(status.im_update == 0))
		{
			bmp280_read_pressure_tempreature(bmp);
		}
	}	
} 
