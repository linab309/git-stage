#include "sys.h"
#include "usart.h"
#include "stm32l1xx_usart.h"
//#include "led.h"
#include "hw_config.h"
//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//����1��ʼ��
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2011/6/14
//�汾��V1.4
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵��
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�v1000_debug��֧��
//�����˴��ڽ��������.
//������v1000_debug��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//////////////////////////////////////////////////////////////////////////////////
//#if EN_USART1_RX   //���ʹ���˽���

#if EN_USART1_RX
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u16 USART_RX_STA=0;       //����״̬���
#endif
//////////////////////////////////////////////////////////////////
//�������´���,֧��v1000_debug����,������Ҫѡ��use MicroLIB
#if EN_USART1
#pragma import(__use_no_semihosting)
//��׼����Ҫ��֧�ֺ���
struct __FILE
{
	int handle;
	/* Whatever you require here. If the only file you are using is */
	/* standard output using v1000_debug() for debugging, no file handling */
	/* is required. */
};
/* FILE is typedef�� d in stdio.h. */
FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
_sys_exit(int x)
{
	x = x;
}
//�ض���fputc����
int fputc(int ch, FILE *f)
{

#if 1
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������
	USART1->DR = (u8) ch;
	return ch;
#else
	USART_To_USB_Send_Data(ch);
#endif
//	return ch;
}

//end
//////////////////////////////////////////////////////////////////

#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���

//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ


u8 re_display_input_char = 0;

void USART1_IRQHandler(void)
{
	u8 res;
	 if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		res=USART1->DR;
		if((USART_RX_STA&0x8000)==0)//����δ���
		{
			if(USART_RX_STA&0x4000)//���յ���0x0d
			{
				if(res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x8000;	//���������m
			}else //��û�յ�0X0D
			{
				if(res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����
				}
			}
		}
		LED0=!LED0;
	}
}
#endif
//��ʼ��IO ����1
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������
//CHECK OK
//091209
#if 0
void uart_init(u32 pclk2,u32 bound)
{
	float temp;
	u16 mantissa;
	u16 fraction;
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������
    mantissa<<=4;
	mantissa+=fraction;
	RCC->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��
	RCC->APB2ENR|=1<<14;  //ʹ�ܴ���ʱ��

	GPIOA->CRH&=0XFFFFF00F;//IO״̬����
	GPIOA->CRH|=0X000008B0;//IO״̬����

	RCC->APB2RSTR|=1<<14;   //��λ����1
	RCC->APB2RSTR&=~(1<<14);//ֹͣ��λ
	//����������
 	USART1->BRR=mantissa; // ����������
	USART1->CR1|=0X200C;  //1λֹͣ,��У��λ.
#if EN_USART1_RX		  //���ʹ���˽���
	//ʹ�ܽ����ж�
	USART1->CR1|=1<<8;    //PE�ж�ʹ��
	USART1->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��
	MY_NVIC_Init(3,3,USART1_IRQn,2);//��2��������ȼ�
#endif
}

#else

/* USART Communication boards Interface */
#define USARTx                           USART1
#define USARTx_CLK                       RCC_APB2Periph_USART1
#define USARTx_APBPERIPHCLOCK            RCC_APB2PeriphClockCmd
#define USARTx_IRQn                      USART1_IRQn
#define USARTx_IRQHandler                USART1_IRQHandler

#define USARTx_TX_PIN                    GPIO_Pin_9
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_TX_GPIO_CLK               RCC_AHBPeriph_GPIOA
#define USARTx_TX_SOURCE                 GPIO_PinSource9
#define USARTx_TX_AF                     GPIO_AF_USART1

#define USARTx_RX_PIN                    GPIO_Pin_10
#define USARTx_RX_GPIO_PORT              GPIOA
#define USARTx_RX_GPIO_CLK               RCC_AHBPeriph_GPIOA
#define USARTx_RX_SOURCE                 GPIO_PinSource10
#define USARTx_RX_AF                     GPIO_AF_USART1

void uart_init(u32 bound){
    //GPIO�˿�����
GPIO_InitTypeDef GPIO_InitStructure;
USART_InitTypeDef USART_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

/* Enable GPIO clock */
RCC_AHBPeriphClockCmd(USARTx_TX_GPIO_CLK | USARTx_RX_GPIO_CLK, ENABLE);



/* Configure USART Tx and Rx as alternate function push-pull */
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
GPIO_InitStructure.GPIO_Pin = USARTx_TX_PIN;
GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure);

GPIO_InitStructure.GPIO_Pin = USARTx_RX_PIN;
//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure);


/* Enable USART clock */
USARTx_APBPERIPHCLOCK(USARTx_CLK, ENABLE);

/* Connect PXx to USARTx_Tx */
GPIO_PinAFConfig(USARTx_TX_GPIO_PORT, USARTx_TX_SOURCE, USARTx_TX_AF);

/* Connect PXx to USARTx_Rx */
GPIO_PinAFConfig(USARTx_RX_GPIO_PORT, USARTx_RX_SOURCE, USARTx_RX_AF);

/* USARTx configuration ----------------------------------------------------*/
/* USARTx configured as follow:
- BaudRate = 230400 baud

- Word Length = 8 Bits
- One Stop Bit
- No parity
- Hardware flow control disabled (RTS and CTS signals)
- Receive and transmit enabled
*/
USART_InitStructure.USART_BaudRate = 115200;
USART_InitStructure.USART_WordLength = USART_WordLength_8b;
USART_InitStructure.USART_StopBits = USART_StopBits_1;
USART_InitStructure.USART_Parity = USART_Parity_No;
USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
#if EN_USART1_RX   //���ʹ���˽���
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
#else
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
#endif
USART_Init(USARTx, &USART_InitStructure);

/* NVIC configuration */
/* Configure the Priority Group to 2 bits */
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

/* Enable the USARTx Interrupt */
NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQn;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStructure);
#if 0//S EN_USART1_RX   //���ʹ���˽���
	USART1->CR1|=1<<8;    //PE�ж�ʹ��
	USART1->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��

	MY_NVIC_Init(1,3,USART1_IRQn,2);//��2��������ȼ�
#endif

/* Enable USART */
USART_Cmd(USARTx, ENABLE);

}


#endif
#endif

