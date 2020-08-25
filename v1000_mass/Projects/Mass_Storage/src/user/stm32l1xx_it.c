/**
  ******************************************************************************
  * @file    Project/STM32L1xx_StdPeriph_Template/stm32l1xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.3
  * @date    May-2013
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include "stm32l1xx_it.h"
#include "stm32l1xx_exti.h"
#include "stm32l1xx_rtc.h"
//#include "discover_functions.h"
//#include "discover_board.h"
//#include "stm32l_discovery_lcd.h"
//#include "tsl.h"
#include "sys.h"
#include "led.h"


#include "hw_config.h"
#include "usb_lib.h"
#include "usb_istr.h"

extern u8 system_power_flag ;
//extern __IO uint32_t EndOfTransfer;
uint16_t tmpCC1[2] = {0, 0};
extern __IO uint32_t CaptureNumber, PeriodValue;
uint32_t IC1ReadValue1 = 0, IC1ReadValue2 =0;
#if 0
extern volatile bool KeyPressed;
extern uint8_t state_machine;
extern bool self_test;
extern bool UserButton;
extern volatile bool Idd_WakeUP;
extern uint8_t t_bar[2];
#endif

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1);
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1);
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1);
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1);
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{  
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1);
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
 /* Go to infinite loop when Hard Fault exception occurs */
  while (1);
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
  while (1);
}

/**
  * @brief  This function handles SysTick interrupts.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
//    disableGlobalInterrupts();
//   TimingDelay_Decrement();
//    enableGlobalInterrupts();
  
    //TSL_tim_ProcessIT();
    
}

/**
  * @brief  This function handles TIM10 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM10_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM10, TIM_IT_CC1) != RESET)
  {
    /* Clear TIM10 Capture Compare 1 interrupt pending bit */
    TIM_ClearITPendingBit(TIM10, TIM_IT_CC1);
    
    if(CaptureNumber == 0)
    {
      /* Get the Input Capture value */
      IC1ReadValue1 = TIM_GetCapture1(TIM10);
      CaptureNumber = 1;
    }
    else if(CaptureNumber == 1)
    {
       /* Get the Input Capture value */
       IC1ReadValue2 = TIM_GetCapture1(TIM10); 
       TIM_ITConfig(TIM10, TIM_IT_CC1, DISABLE);

       /* Capture computation */
       if (IC1ReadValue2 > IC1ReadValue1)
       {
         PeriodValue = (IC1ReadValue2 - IC1ReadValue1);
       }
       else
       {
         PeriodValue = ((0xFFFF - IC1ReadValue1) + IC1ReadValue2);
       }
       /* capture of two values is done */
       CaptureNumber = 2;
    }
  }
}
/**
  * @brief  This function handles external interrupts generated by UserButton.
  * @param  None
  * @retval None
  */

/**
  * @brief  This function handles External lines 15 to 10 interrupt request.
  * @param  None
  * @retval None
  */



//�ⲿ�ж�15~10�������
void EXTI15_10_IRQHandler(void)
{	
//       LED1 != LED1;
  if(EXTI_GetITStatus(EXTI_Line13) != RESET)
  {
#if 1 
   // if(system_power_flag == 3)	 	
//		Choose_power_mode(0);
#endif	
    EXTI_ClearITPendingBit(EXTI_Line13);
  }

}


//�ⲿ�ж�15~10�������
void EXTI0_IRQHandler(void)
{	
//       LED1 != LED1;
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
//    LED0 = 1;
//    system_flag_table->charger_connected = 1;
    EXTI_ClearITPendingBit(EXTI_Line0);
  }

}

/**
  * @brief  This function handles DMA1 Channel 6 interrupt request.
  * @param  None
  * @retval None
  */
void DMA1_Channel6_IRQHandler(void)
{
  /* Test on DMA1 Channel6 Transfer Complete interrupt */
  if(DMA_GetITStatus(DMA1_IT_TC6))
  {
    /* DMA1 finished the transfer of SrcBuffer */
    //EndOfTransfer = 1;

    /* Clear DMA1 Channel6 Half Transfer, Transfer Complete and Global interrupt pending bits */
    DMA_ClearITPendingBit(DMA1_IT_GL6);
  }
}


#if 0
void RTC_WKUP_IRQHandler (void)
{
  RTC_ClearITPendingBit(RTC_IT_WUT);
  EXTI_ClearITPendingBit(EXTI_Line20);
}
#endif


/*******************************************************************************
* Function Name  : USB_IRQHandler
* Description    : This function handles USB Low Priority interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#if defined(STM32L1XX_MD) || defined(STM32L1XX_HD)|| defined(STM32L1XX_MD_PLUS)|| defined (STM32F37X)
void USB_LP_IRQHandler(void)
#else
void USB_LP_CAN1_RX0_IRQHandler(void)
#endif
{
  USB_Istr();
}


#if 0
/*******************************************************************************
* Function Name  : EVAL_COM1_IRQHandler
* Description    : This function handles EVAL_COM1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EVAL_COM1_IRQHandler(void)
{
  if (USART_GetITStatus(EVAL_COM1, USART_IT_RXNE) != RESET)
  {
    /* Send the received data to the PC Host*/
    USART_To_USB_Send_Data();
  }

  /* If overrun condition occurs, clear the ORE flag and recover communication */
  if (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_ORE) != RESET)
  {
    (void)USART_ReceiveData(EVAL_COM1);
  }
}
#endif

/*******************************************************************************
* Function Name  : USB_FS_WKUP_IRQHandler
* Description    : This function handles USB WakeUp interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

#if defined(STM32L1XX_MD) || defined(STM32L1XX_HD)|| defined(STM32L1XX_MD_PLUS)
void USB_FS_WKUP_IRQHandler(void)
#else
void USBWakeUp_IRQHandler(void)
#endif
{
  EXTI_ClearITPendingBit(EXTI_Line18);
}
/******************************************************************************/
/*                 STM32L1xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l1xx_md.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
