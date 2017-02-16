#include "sys.h"
#include "pwm.h"
#include "delay.h"
#include "timer.h"
#include "oled.h"
#include "led.h"
#include "usmart.h"
#include "spi.h"
#include "flash.h"
#include "usart.h"
#include "myadc.h"
#include "menutal.h"
#include "stm_eeprom.h"
#include "key.h"
#include "rtc.h"
#include "hw_config.h"


extern u8 system_power_flag ;
//********************************************************************************
void NVIC_Configuration(void)
{

  /* Set the Vector Table base location at 0x3000 */
//      NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x3000);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

}
//********************************************************************************
//V1.4�޸�˵��
//��NVIC KO��,û��ʹ���κο��ļ�!
//������JTAG_Set����
//////////////////////////////////////////////////////////////////////////////////

//����������ƫ�Ƶ�ַ
//NVIC_VectTab:��ַ
//Offset:ƫ����
//CHECK OK
//091207
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset)
{
  	//�������Ϸ���
	assert_param(IS_NVIC_VECTTAB(NVIC_VectTab));
	assert_param(IS_NVIC_OFFSET(Offset));
	SCB->VTOR = NVIC_VectTab|(Offset & (u32)0x1FFFFF80);//����NVIC��������ƫ�ƼĴ���
	//���ڱ�ʶ����������CODE��������RAM��
}
//����NVIC����
//NVIC_Group:NVIC���� 0~4 �ܹ�5��
//CHECK OK
//091209
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)
{
	u32 temp,temp1;
	temp1=(~NVIC_Group)&0x07;//ȡ����λ
	temp1<<=8;
	temp=SCB->AIRCR;  //��ȡ��ǰ������
	temp&=0X0000F8FF; //�����ǰ����
	temp|=0X05FA0000; //д��Կ��
	temp|=temp1;
	SCB->AIRCR=temp;  //���÷���
}
//����NVIC
//NVIC_PreemptionPriority:��ռ���ȼ�
//NVIC_SubPriority       :��Ӧ���ȼ�
//NVIC_Channel           :�жϱ��
//NVIC_Group             :�жϷ��� 0~4
//ע�����ȼ����ܳ����趨����ķ�Χ!����������벻���Ĵ���
//�黮��:
//��0:0λ��ռ���ȼ�,4λ��Ӧ���ȼ�
//��1:1λ��ռ���ȼ�,3λ��Ӧ���ȼ�
//��2:2λ��ռ���ȼ�,2λ��Ӧ���ȼ�
//��3:3λ��ռ���ȼ�,1λ��Ӧ���ȼ�
//��4:4λ��ռ���ȼ�,0λ��Ӧ���ȼ�
//NVIC_SubPriority��NVIC_PreemptionPriority��ԭ����,��ֵԽС,Խ����
//CHECK OK
//100329
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)
{
	u32 temp;
	u8 IPRADDR=NVIC_Channel/4;  //ÿ��ֻ�ܴ�4��,�õ����ַ
	u8 IPROFFSET=NVIC_Channel%4;//�����ڵ�ƫ��
	IPROFFSET=IPROFFSET*8+4;    //�õ�ƫ�Ƶ�ȷ��λ��
	MY_NVIC_PriorityGroupConfig(NVIC_Group);//���÷���
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;//ȡ����λ

	if(NVIC_Channel<32)NVIC->ISER[0]|=1<<NVIC_Channel;//ʹ���ж�λ(Ҫ����Ļ�,�෴������OK)
	else NVIC->ISER[1]|=1<<(NVIC_Channel-32);
	NVIC->IPR[IPRADDR]|=temp<<IPROFFSET;//������Ӧ���ȼ����������ȼ�
}

//�ⲿ�ж����ú���
//ֻ���GPIOA~G;������PVD,RTC��USB����������
//����:GPIOx:0~6,����GPIOA~G;BITx:��Ҫʹ�ܵ�λ;TRIM:����ģʽ,1,������;2,�Ͻ���;3�������ƽ����
//�ú���һ��ֻ������1��IO��,���IO��,���ε���
//�ú������Զ�������Ӧ�ж�,�Լ�������
//������...
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM)
{
	u8 EXTADDR;
	u8 EXTOFFSET;
	EXTADDR=BITx/4;//�õ��жϼĴ�����ı��
	EXTOFFSET=(BITx%4)*4;

	RCC->APB2ENR|=0x01;//ʹ��io����ʱ��

	SYSCFG->EXTICR[EXTADDR]&=~(0x000F<<EXTOFFSET);//���ԭ�����ã�����
	SYSCFG->EXTICR[EXTADDR]|=GPIOx<<EXTOFFSET;//EXTI.BITxӳ�䵽GPIOx.BITx

	//�Զ�����
	EXTI->IMR|=1<<BITx;//  ����line BITx�ϵ��ж�
	//EXTI->EMR|=1<<BITx;//������line BITx�ϵ��¼� (������������,��Ӳ�����ǿ��Ե�,��������������ʱ���޷������ж�!)
 	if(TRIM&0x01)EXTI->FTSR|=1<<BITx;//line BITx���¼��½��ش���
	if(TRIM&0x02)EXTI->RTSR|=1<<BITx;//line BITx���¼��������ش���
}


//����������ִ���������踴λ!�����������𴮿ڲ�����.
//������ʱ�ӼĴ�����λ
//CHECK OK
//091209
void MYRCC_DeInit(void)
{
	RCC->APB1RSTR = 0x00000000;//��λ����
	RCC->APB2RSTR = 0x00000000;

  	RCC->AHBENR = 0x00000014;  //˯��ģʽ�����SRAMʱ��ʹ��.�����ر�.
  	RCC->APB2ENR = 0x00000000; //����ʱ�ӹر�.
  	RCC->APB1ENR = 0x00000000;
	RCC->CR |= 0x00000001;     //ʹ���ڲ�����ʱ��HSION
	RCC->CFGR &= 0xF8FF0000;   //��λSW[1:0],HPRE[3:0],PPRE1[2:0],PPRE2[2:0],ADCPRE[1:0],MCO[2:0]
	RCC->CR &= 0xFEF6FFFF;     //��λHSEON,CSSON,PLLON
	RCC->CR &= 0xFFFBFFFF;     //��λHSEBYP
	RCC->CFGR &= 0xFF80FFFF;   //��λPLLSRC, PLLXTPRE, PLLMUL[3:0] and USBPRE
	RCC->CIR = 0x00000000;     //�ر������ж�
	//����������
#ifdef  VECT_TAB_RAM
	MY_NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else
	MY_NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x3000);
#endif
}
//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI
//CHECK OK
//091209
__asm void WFI_SET(void)
{
	WFI;
}
//�������ģʽ
//check ok
//091202
void Sys_Standby(void)
{
	SCB->SCR|=1<<2;//ʹ��SLEEPDEEPλ (SYS->CTRL)
  	RCC->APB1ENR|=1<<28;     //ʹ�ܵ�Դʱ��
 	PWR->CSR|=1<<8;          //����WKUP���ڻ���
	PWR->CR|=1<<2;           //���Wake-up ��־
	PWR->CR|=1<<1;           //PDDS��λ
	WFI_SET();				 //ִ��WFIָ��
}
//�󱸼Ĵ���д�����
//reg:�Ĵ������
//reg:Ҫд�����ֵ
////check ok
////091202
//void BKP_Write(u8 reg,u16 dat)
//{
//  RCC->APB1ENR|=1<<28;     //ʹ�ܵ�Դʱ��
//	RCC->APB1ENR|=1<<27;     //ʹ�ܱ���ʱ��
//	PWR->CR|=1<<8;           //ȡ��������д����
//	switch(reg)
//	{
//		case 1:
//			BKP->DR1=dat;
//			break;
//		case 2:
//			BKP->DR2=dat;
//			break;
//		case 3:
//			BKP->DR3=dat;
//			break;
//		case 4:
//			BKP->DR4=dat;
//			break;
//		case 5:
//			BKP->DR5=dat;
//			break;
//		case 6:
//			BKP->DR6=dat;
//			break;
//		case 7:
//			BKP->DR7=dat;
//			break;
//		case 8:
//			BKP->DR8=dat;
//			break;
//		case 9:
//			BKP->DR9=dat;
//			break;
//		case 10:
//			BKP->DR10=dat;
//			break;
//	}
//}
//ϵͳ��λ
//CHECK OK
//091209
void Sys_Soft_Reset(void)
{
	SCB->AIRCR =0X05FA0000|(u32)0x04;
}

//JTAGģʽ����,��������JTAG��ģʽ
//mode:jtag,swdģʽ����;00,ȫʹ��;01,ʹ��SWD;10,ȫ�ر�;
//CHECK OK
//100818
void JTAG_Set(u8 mode)
{
//	u32 temp;
	//temp=mode;
//	temp<<=25;
	//init GPIOA.13,GPIOA.15  ��������

#if 0
	  GPIO_InitTypeDef  GPIO_InitStructure;


	RCC->APB2ENR|=1<<0;     //��������ʱ��
//	SYSCFG->MAPR&=0XF8FFFFFF; //���MAPR��[26:24]
	//SYSCFG->MAPR|=temp;       //����jtagģʽ

	if(mode == 1)
		{
		RCC_APB2PeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource13, GPIO_AF_SWJ);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource14, GPIO_AF_SWJ);
		}
	else
		{
			RCC_APB2PeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
			GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13|GPIO_Pin_14;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
		}
#endif
}





/**
  * @brief  To select MSI as System clock source
  * @caller ADC_Icc_Test
  * @param Frequence, DIV by 2 ot not , With or without RTC
  * @retval None
  */
void SetHSICLKToMSI(u32 freq,u8 div2,u8 With_RTC)
{

  /* RCC system reset */
  RCC_DeInit();

  /* Flash no latency*/
  FLASH_SetLatency(FLASH_Latency_0);

  /* Disable Prefetch Buffer */
  FLASH_PrefetchBufferCmd(DISABLE);

  /* Disable 64-bit access */
  FLASH_ReadAccess64Cmd(DISABLE);

  /* Disable FLASH during SLeep  */
  FLASH_SLEEPPowerDownCmd(ENABLE);

  /* Enable the PWR APB1 Clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Select the Voltage Range 3 (1.2V) */
  PWR_VoltageScalingConfig(PWR_VoltageScaling_Range3);

  /* Wait Until the Voltage Regulator is ready */
  while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET);

  /* Configure the MSI frequency */
  RCC_MSIRangeConfig(freq);

  /* Select MSI as system clock source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_MSI);

  /* Wait until MSI is used as system clock source */
  while (RCC_GetSYSCLKSource() != 0x00);

  if (div2)
  {
    RCC_HCLKConfig(RCC_SYSCLK_Div2);
  }

  RCC_HSICmd(DISABLE);

  /* Disable HSE clock */
  RCC_HSEConfig(RCC_HSE_OFF);

  /* Disable LSE clock */
  if (! With_RTC)
   RCC_LSICmd(DISABLE);

  /* Disable LSI clock */
//  RCC_LSICmd(DISABLE);

}

/**
  * @brief  To select HSI as System clock source
  * @caller ADC_Icc_Test
  * @param None
  * @retval None
  */
void SetHSICLK(void)
{
  /* Enable HSI Clock */
  RCC_HSICmd(ENABLE);

  /*!< Wait till HSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

  /* Enable 64-bit access */
  FLASH_ReadAccess64Cmd(ENABLE);

  /* Enable Prefetch Buffer */
  FLASH_PrefetchBufferCmd(ENABLE);

  /* Flash 1 wait state */
  FLASH_SetLatency(FLASH_Latency_1);

  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);

  while (RCC_GetSYSCLKSource() != 0x04);

  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  /* PCLK2 = HCLK */
  RCC_PCLK2Config(RCC_HCLK_Div1);

  /* PCLK1 = HCLK */
  RCC_PCLK1Config(RCC_HCLK_Div1);

}




//ϵͳʱ�ӳ�ʼ������
//pll:ѡ��ı�Ƶ������2��ʼ�����ֵΪ16
//CHECK OK
//091209
void Stm32_Clock_Init(u8 PLL)
{

#if 0
  /* Enable HSI Clock */
  RCC_HSICmd(ENABLE);

  /*!< Wait till HSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

  /* Set HSI as sys clock*/
 //RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);


  /* Set MSI clock range to ~4.194MHz*/
 // RCC_MSIRangeConfig(RCC_MSIRange_6);


	RCC_PLLConfig(RCC_PLLSource_HSI, RCC_PLLMul_6,RCC_PLLDiv_4);
	RCC_PLLCmd(ENABLE);
    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    ;
    }
  /* Set HSI as sys clock*/
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL)
    {
    ;
    }
#endif
  //MYRCC_DeInit();		  //��λ�����������

  /* Enable the GPIOs clocks */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC| RCC_AHBPeriph_GPIOD, ENABLE);


#if 1  /* ��Ӱ��������RTC���� ��������¸�λ��*/
  /* Enable comparator, LCD and PWR mngt clocks */
 RCC_APB1PeriphClockCmd( RCC_APB1Periph_PWR,ENABLE);

  /* Enable ADC & SYSCFG clocks */
 RCC_APB2PeriphClockCmd( RCC_APB2Periph_SYSCFG , ENABLE);

  /* Allow access to the RTC */
 // PWR_RTCAccessCmd(ENABLE);

  /* Reset RTC Backup Domain */
  //RCC_RTCResetCmd(ENABLE);
  //RCC_RTCResetCmd(DISABLE);

  /* LSE Enable */
  //RCC_LSEConfig(RCC_LSE_ON);

  /* Wait until LSE is ready */
 // while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

   /* RTC Clock Source Selection */
 // RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable the RTC */
  //RCC_RTCCLKCmd(ENABLE);



  /*Disable HSE*/
// RCC_HSEConfig(RCC_HSE_OFF);
//if(RCC_GetFlagStatus(RCC_FLAG_HSERDY) != RESET )
  {
    /* Stay in infinite loop if HSE is not disabled*/
 //   while(1);
  }
  #endif



  /* Set internal voltage regulator to 1.8V */
  PWR_VoltageScalingConfig(PWR_VoltageScaling_Range1);

  /* Wait Until the Voltage Regulator is ready */
  while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET) ;



  #ifdef  VECT_TAB_RAM
    // Set the Vector Table base location at 0x20000000
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
    // Set the Vector Table base location at 0x08000000
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x3000);
#endif

}

/**
  * @brief converts a 32bit unsined int into ASCII
  * @caller several callers for display values
  * @param Number digit to displays
  *  p_tab values in array in ASCII
  * @retval None
  */
void convert_into_char(uint32_t number, uint16_t *p_tab)
{
   uint16_t units=0, tens=0, hundreds=0, thousands=0, misc=0;
 
   units = (((number%10000)%1000)%100)%10;
   tens = ((((number-units)/10)%1000)%100)%10;
   hundreds = (((number-tens-units)/100))%100%10;
   thousands = ((number-hundreds-tens-units)/1000)%10;
   misc = ((number-thousands-hundreds-tens-units)/10000);
 
   *(p_tab+4) = units + 0x30;
   *(p_tab+3) = tens + 0x30;
   *(p_tab+2) = hundreds + 0x30;
   *(p_tab+1) = thousands + 0x30;
   *(p_tab) = misc + 0x30;

}





/**
  * @brief  Executed when a sensor is in Error state
  * @param  None
  * @retval None
  */
void MyLinRots_ErrorStateProcess(void)
{
  // Add here your own processing when a sensor is in Error state
 // TSL_linrot_SetStateOff();
}


/**
  * @brief  Executed when a sensor is in Off state
  * @param  None
  * @retval None
  */
void MyLinRots_OffStateProcess(void)
{
  // Add here your own processing when a sensor is in Off state
}





void SoftReset(void)
{
    __set_FAULTMASK(1);      // �ر������ж�
     NVIC_SystemReset();// ��λ

}





#if 1

/* Variables used for save GPIO configuration */
u32 GPIOA_MODER, GPIOB_MODER, GPIOC_MODER,GPIOD_MODER;
//,GPIOE_MODER ,GPIOE_MODER,GPIOH_MODER;
u32 GPIOA_PUPDR, GPIOB_PUPDR , GPIOC_PUPDR, GPIOD_PUPDR;
//,GPIOE_PUPDR,GPIOH_PUPDR;

/**
  * @brief  Function used to Configure the GPIO in low consumption
  * @caller ADC_Icc_Test
  * @param None
  * @retval None
  */
void GPIO_LowPower_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* store GPIO configuration before lowpower switch */
  GPIOA_MODER = GPIOA->MODER;
  GPIOB_MODER = GPIOB->MODER;
  GPIOC_MODER = GPIOC->MODER;
  GPIOD_MODER = GPIOD->MODER;
  //GPIOE_MODER = GPIOE->MODER;
  //GPIOH_MODER = GPIOH->MODER;
  GPIOA_PUPDR = GPIOA->PUPDR;
  GPIOB_PUPDR = GPIOB->PUPDR;
  GPIOC_PUPDR = GPIOC->PUPDR;
  GPIOD_PUPDR = GPIOD->PUPDR;
  //GPIOE_PUPDR = GPIOE->PUPDR;
  //GPIOH_PUPDR = GPIOH->PUPDR;

  /* Configure all GPIO port pins in Analog input mode (trigger OFF) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;


  GPIOD->MODER   = 0xFFFFFFFF;
 // GPIOE->MODER   = 0xFFFFFFFF;
  //GPIOH->MODER   = 0xFFFFFFFF;

  /* all GPIOA */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_6| GPIO_Pin_7 \
    | GPIO_Pin_13 | GPIO_Pin_14|GPIO_Pin_5 | GPIO_Pin_8  |GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |  GPIO_Pin_12 |GPIO_Pin_15 ;

   GPIO_Init(GPIOA, &GPIO_InitStructure);
   
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

   /* All GPIOC except PC13 which is used for mesurement */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4|  GPIO_Pin_5 |GPIO_Pin_6| GPIO_Pin_7| GPIO_Pin_8 \
                                 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |  GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15 ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

   /* all GPIOB except PB6 and PB7 used for LED*/
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4|  GPIO_Pin_5 |GPIO_Pin_6 |GPIO_Pin_7 | GPIO_Pin_8 \
                                 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |  GPIO_Pin_12 |GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 ;
  GPIO_Init(GPIOB, &GPIO_InitStructure);


 // GPIO_LOW(GPIOB,GPIO_Pin_6);
 // GPIO_LOW(GPIOB,GPIO_Pin_7);
}

/**
  * @brief  To restore register values for GPIO.
  * @caller ADC_Icc_Test
  * @param None
  * @retval None
  */
void Restore_GPIO_Config(void)
{
  GPIOA->MODER = GPIOA_MODER;
  GPIOB->MODER = GPIOB_MODER;
  GPIOC->MODER = GPIOC_MODER;
  GPIOD->MODER = GPIOD_MODER;
 // GPIOE->MODER = GPIOE_MODER;
  //GPIOH->MODER = GPIOH_MODER;

  GPIOA->PUPDR = GPIOA_PUPDR;
  GPIOB->PUPDR = GPIOB_PUPDR;
  GPIOC->PUPDR = GPIOC_PUPDR;
  GPIOD->PUPDR = GPIOD_PUPDR;
  //GPIOE->PUPDR = GPIOE_PUPDR;
  //GPIOH->PUPDR = GPIOH_PUPDR;
}




#endif


/**
  * @brief To confgure RCC for current measurmeent
  * @caller  ADC_Icc_Test
  * @param Structure address for save the RCC configuration
  * @retval None
  */
void Config_RCC(RCC_TypeDef *sav_RCC)
{
  /* Save the RCC configuration registers */
  sav_RCC->AHBENR   = RCC->AHBENR;
  sav_RCC->APB1ENR  = RCC->APB1ENR;
  sav_RCC->APB2ENR  = RCC->APB2ENR;
  sav_RCC->AHBLPENR   = RCC->AHBLPENR;
  sav_RCC->APB1LPENR  = RCC->APB1LPENR;
  sav_RCC->APB2LPENR  = RCC->APB2LPENR;

  /* Set low power configuration */
  RCC->AHBENR = 0x04; // Ports A and C enable
  RCC->AHBLPENR = 0x04;
  RCC->APB1ENR =  RCC_APB1ENR_PWREN;     // PWR management enable
  RCC->APB2ENR = 0;

}



void alarm_everytimer(void)
{

   RTC_TimeTypeDef MY_RTC_TimeStructure;
   RTC_AlarmTypeDef  RTC_AlarmStructure;

    /* Disable the Alarm A */
    RTC_AlarmCmd(RTC_Alarm_A, DISABLE);

    RTC_GetTime(RTC_Format_BCD, &MY_RTC_TimeStructure);

    /* Set the alarm X+5s */
    RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = MY_RTC_TimeStructure.RTC_H12;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = MY_RTC_TimeStructure.RTC_Hours;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = MY_RTC_TimeStructure.RTC_Minutes;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = (MY_RTC_TimeStructure.RTC_Seconds + 0x5) % 60;
    RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
    RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
    RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
    RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);

    /* Enable RTC Alarm A Interrupt */
    RTC_ITConfig(RTC_IT_ALRA, ENABLE);

    /* Enable the Alarm A */
    RTC_AlarmCmd(RTC_Alarm_A, ENABLE);

    /* Clear RTC Alarm A flag */
    RTC_ClearFlag(RTC_FLAG_ALRAF);

    /* Clear WakeUp flag */
    PWR_ClearFlag(PWR_FLAG_WU);
}

/**
  * @brief To confgure RCC for current measurmeent
  * @caller  ADC_Icc_Test
  * @param Structure address for save the RCC configuration
  * @retval None
  */


void Choose_power_mode(u8 Mcu_State)
{
//  GPIO_InitTypeDef GPIO_InitStructure;
//  u32 i;
    RCC_TypeDef SavRCC;
    extern u8 key_press_cnt ;
    v1000_debug("Mcu_State: %d \r\n ",Mcu_State);
    switch(Mcu_State)
	{
	    case MCU_RUN_USB:

    		PWR_EnterLowPowerRunMode(DISABLE);
    		while(PWR_GetFlagStatus(PWR_FLAG_REGLP) != RESET) ;
    		PWR_VoltageScalingConfig(PWR_VoltageScaling_Range1);
    		while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET) ;

    		PWR_EnterLowPowerRunMode(DISABLE);

    		/* Disable Ultra low power mode */
    		PWR_UltraLowPowerCmd(DISABLE);

    		/* Disable FLASH during SLeep LP */
    		FLASH_SLEEPPowerDownCmd(DISABLE);
    		/* Clear Wake Up flag */
    		PWR_ClearFlag(PWR_FLAG_WU);
    		/* Enable PVD */
    		PWR_PVDCmd(ENABLE);
    		SystemInit();
    //        Leave_LowPowerMode();
            //PowerOn();
    		RTC_Config();
    		SystemCoreClock =32000000 ;
    		Button_key_perss(0);
		break;

    /* Run mode : Measurement Measurement performed with MSI 4 MHz without RTC*/
      case MCU_RUN:


    		PWR_EnterLowPowerRunMode(DISABLE);
    		while(PWR_GetFlagStatus(PWR_FLAG_REGLP) != RESET) ;

    		PWR_VoltageScalingConfig(PWR_VoltageScaling_Range1);
    		while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET) ;

    		SetHSICLK();
    		PWR_EnterLowPowerRunMode(DISABLE);

    		/* Disable Ultra low power mode */
    		PWR_UltraLowPowerCmd(DISABLE);

    		/* Disable FLASH during SLeep LP */
    		FLASH_SLEEPPowerDownCmd(DISABLE);
    		/* Clear Wake Up flag */
    		PWR_ClearFlag(PWR_FLAG_WU);
    		/* Enable PVD */
    		PWR_PVDCmd(ENABLE);
    //           Enter_LowPowerMode();
    		delay_init(16);	     //��ʱ��ʼ�//
    		Timerx_Init(99,1599);//10Khz  ����4999 --- 500ms
#if EN_USART1
    		uart_init(115200);
#endif
            
         USART2_Init(16,38400);  //��ʼ������2
            
    		RTC_Config();
    		SPI_Flash_Init();   //SPI FLASH ��ʼ��

    		//SPIx_SetSpeed(SPI_SPEED_2);
#if EN_USART1_RX
    		usmart_dev.init(16); 	//��ʼ��USMART
#endif


            if(system_power_flag == 3)
            {
               system_power_flag = 1;
               OLED_Display_On();
            }
            SystemCoreClock =16000000 ;
            Button_key_perss(0);
            ADC_Config();
    	    break;
	case MCU_LP_RUN:
            /* switch on MSI clock */
            /* Disable PVD */
    		Enter_LowPowerMode();
    		PWR_PVDCmd(DISABLE);

    		/* Enable The ultra Low Power Mode */
    		PWR_UltraLowPowerCmd(ENABLE);

    		SetHSICLKToMSI(RCC_MSIRange_6,0,1) ;
    		Timerx_Init(99,419);//10Khz  ����4999 --- 500ms
    		/* shitch on MSI clock */
    		//    Config_RCC(&SavRCC);
    		delay_init(4);
#if EN_USART1
    		uart_init(115200);
#endif
    		SPI_Flash_Init();   //SPI FLASH ��ʼ��
    		OLED_Init();
    		 RTC_Config();
    		SystemCoreClock =4194000 ;
    //		PWM_Init();
    		Button_key_perss(0);
    		//SPIx_SetSpeed(SPI_SPEED_2);
#if EN_USART1_RX
    		usmart_dev.init(4); 	//��ʼ��USMART
#endif
            ADC_Config();
            PWR_EnterLowPowerRunMode(ENABLE);
            while(PWR_GetFlagStatus(PWR_FLAG_REGLP) == RESET) ;
         
        break;

  case MCU_STBY:
        key_press_cnt = 0;
        Button_key_perss(0);

        if(system_power_flag == SYSTEM_POWER_STANBY)
        ;
        else
        {
            system_power_flag = SYSTEM_POWER_STANBY;
            di_and_flash_empet(3);
        }
        
        is_recoreding();/*�Ƿ��ڼ�¼�켣*/
        GPS_power_ctrl(0);
        OLED_Display_Off();
        /* Disable PVD */

        SPI_Flash_PowerDown();
        GPIO_LowPower_Config();
        PWR_PVDCmd(DISABLE);
        /* Enable Ultra low power mode */
        PWR_UltraLowPowerCmd(ENABLE);
        Config_RCC(&SavRCC);
        SysTick->CTRL = 0;
        /* Swith in MSI 32KHz */
        SetHSICLKToMSI(RCC_MSIRange_0,1,1) ;
        KEY_Init();
        PWR_WakeUpPinCmd(PWR_WakeUpPin_2,ENABLE);/*wake up pin 2--- pin13*/
        PWR_WakeUpPinCmd(PWR_WakeUpPin_1,ENABLE);/*wake up pin 2--- pina0*/
        PWR_EnterSTANDBYMode();
        SetHSICLK();

        PWR_VoltageScalingConfig(PWR_VoltageScaling_Range1);
        /* Wait Until the Voltage Regulator is ready */
        while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET) ;
        Leave_LowPowerMode();

        RCC->AHBENR = SavRCC.AHBENR;
        /* To restore RCC registers */
        RCC->APB1ENR = SavRCC.APB1ENR;
        RCC->APB2ENR = SavRCC.APB2ENR;
        RCC->AHBLPENR = SavRCC.AHBLPENR;
        RCC->APB1LPENR = SavRCC.APB1LPENR;
        RCC->APB2LPENR = SavRCC.APB2LPENR;

        /* Need to reinit RCC for LCD*/

        Stm32_Clock_Init(9);	//ϵͳʱ������
        delay_init(16);
        PWR_EnterLowPowerRunMode(DISABLE);
        /* Disable Ultra low power mode */
        PWR_UltraLowPowerCmd(DISABLE);
        /* Disable FLASH during SLeep LP */
        FLASH_SLEEPPowerDownCmd(DISABLE);

        //  Restore_GPIO_Config();
        /* Clear Wake Up flag */
        PWR_ClearFlag(PWR_FLAG_WU);
        /* Enable PVD */
        PWR_PVDCmd(ENABLE);
        Restore_GPIO_Config();
        LED_Init();
        Timerx_Init(99,1599);//10Khz  ����4999 --- 500ms

#if EN_USART1
        uart_init(115200);
#endif
        //   system_power_flag = 1;
        SPI_Flash_WAKEUP();
        RTC_Config();
        break;
	}
}


void board_io_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//init GPIOA.13,GPIOA.15  ��������
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	v1000_debug("\n\r SYSCLK_Frequency222 :1d");

	#if 0
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	v1000_debug("\n\r SYSCLK_Frequency111 :1d");

	v1000_debug("\n\r SYSCLK_Frequency222 :1d");
	GPS_power_ctrl(0);
	v1000_debug("\n\r SYSCLK_Frequency1 :1d");
#endif

}
#if 1
void GPS_power_ctrl(u8 ctrl)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);

	if(ctrl == 1)  /* new board  for GPS power ctrl */
		{
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_HIGH(GPIOA, GPIO_Pin_8);
		}
	else
		{

		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;//|GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		}
    //v1000_debug("gps power %d \r\n",ctrl);
}

#else
void GPS_power_ctrl(u8 ctrl)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);

	if(ctrl == 1)
		{
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_LOW(GPIOA, GPIO_Pin_8);
		}
	else
		{

		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;//|GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		}

}
#endif

