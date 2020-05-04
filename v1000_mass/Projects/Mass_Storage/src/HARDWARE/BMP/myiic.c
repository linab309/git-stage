#include "myiic.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//Mini STM32������
//IIC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/6/10 
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

//��ʼ��IIC
void IIC_Init(void)
{					     
	GPIO_InitTypeDef  GPIO_InitStructure;

	//RCC->APB2ENR|=1<<4;//��ʹ������IO PORTCʱ�� 							 
	//	GPIOC->CRH&=0XFFF00FFF;//PC11/12 �������
	//	GPIOC->CRH|=0X00033000;	   
	//	GPIOC->ODR|=3<<11;     //PC11,12 �����
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //SDA-->PC.4 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 		 //�������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_8);						 //PA.8 �����

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	    		 //LED1-->PD.2 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 		 //�������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	GPIO_SetBits(GPIOB,GPIO_Pin_9); 						 //PD.2 ����� 

//	GPIOC->CRL&=0XFF0FFF0F;//PC4/5 �������
//	GPIOC->CRL|=0X00300030;	   
//	GPIOC->ODR|=1<<5;     //PC4,4 �����
//	GPIOC->ODR|=1<<1;     //PC4,4 �����
}
//����IIC��ʼ�ź�


//IO��������
void SDA_IN(void) 
{
	//GPIOC->MODE&=0XFFFFFF0F;GPIOC->CRL|=2<<4;
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //SDA-->PC.4 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 		 //�������
	// GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void SDA_OUT(void) 
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //SDA-->PC.4 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 		 //�������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(5);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(5);
	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	//IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	//delay_us(5);
	IIC_SCL=1; 
	delay_us(5);
	IIC_SDA=1;//����I2C���߽����ź�
	delay_us(5);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	IIC_SDA=1;delay_us(5);	   
	IIC_SCL=1;delay_us(5);	 
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
} 
//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(5);
	IIC_SCL=1;
	delay_us(5);
	IIC_SCL=0;
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL=1;
		delay_us(5); 
		IIC_SCL=0;	
		delay_us(5);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(void)
{
	unsigned char i,receive=0;
	SDA_OUT();//SDA����Ϊ����
	IIC_SDA = 1;
    for(i=0;i<8;i++ )
	{
        receive<<=1;
		IIC_SCL=1;
        delay_us(5);
	    SDA_IN();//SDA����Ϊ����
        if(READ_SDA)receive++;   
        IIC_SCL=0; 
        delay_us(5);
    }
	#if 0
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
	#endif
    return receive;
}



























