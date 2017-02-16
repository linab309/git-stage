/**
  ******************************************************************************
  * @file    ADC/ADC1_IDDmeas/main.c 
  * @author  MCD Application Team
  * @version V1.1.1
  * @date    13-April-2012
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include "sys.h"
#include "usart.h"
#include "key.h"
#include "myadc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#if 1//def USE_STM32L152D_EVAL 
  #define  EVAL_RESISTOR_R62      110   /* For EVAL REVA ALPHA1 540 */ 
  #define  IDD_MEASUREMENT_PIN       GPIO_Pin_15
  #define  IDD_MEASUREMENT_GPIO_CLK  RCC_AHBPeriph_GPIOB
  #define  IDD_MEASUREMENT_GPIO      GPIOB
  #define  IDD_MEASUREMENT_ADC_CHANNEL  ADC_Channel_21
  #define  APOSTROPHE_OFF DOUBLEPOINT_OFF

#elif defined USE_STM32L152_EVAL 
  #define  EVAL_RESISTOR_R36      130    /* value of resistor R36 is 1.3 Ohm 
                                           EVAL_RESISTOR_RATIO * 1.3 = 130  */ 
  #define  IDD_MEASUREMENT_PIN       GPIO_Pin_5
  #define  IDD_MEASUREMENT_GPIO_CLK  RCC_AHBPeriph_GPIOA
  #define  IDD_MEASUREMENT_GPIO      GPIOA
  #define  IDD_MEASUREMENT_ADC_CHANNEL  ADC_Channel_5
  #define APOSTROPHE_OFF APOSTROPHE_OFF
#endif 

#define EVAL_RESISTOR_RATIO    100    /* R36 is multiplied by 100 */
#define EVAL_MAX9938_GAIN      50     /* Ampli-op gain = 50 */
#define ADC_CONVERT_RATIO      732    /* (3300mV / 4095)* 1000 */


typedef enum
{
  FALSE = 0, TRUE  = !FALSE
}
bool;



//	ncl = (res == FR_DISK_ERR) ? 0xFFFFFFFF : 1;
static bool IsBatteryPoweroff(uint16_t mv) 
{
    return (mv<=3200)? TRUE:FALSE;
}

static bool IsBatteryLow(uint16_t mv) {return mv>3630 ? TRUE:FALSE;}
/* Check INTERNAL value for shut off battery level  - around 3.00V */
static bool IsBatteryDead(uint16_t mv) {return mv>3200 ? TRUE:FALSE;}
/* battery full is indicated in firmware for this chip*/
static bool IsBatteryMid(uint16_t mv) {return mv>3720 ? TRUE:FALSE;}
static bool IsBatteryHIGH(uint16_t mv) {return mv>3850 ? TRUE:FALSE;}
static bool IsBatteryFull(uint16_t mv) {return mv>=4100 ? TRUE:FALSE;}
	
/**
  * @brief  Display the IDD measured Value On the LCD Glass.
  * @param  IDD measure
  * @retval None
  */

void DisplayIDDrunmV(uint32_t IDDmeas)
{ 
  static int ddrunmv = 0;
  /* x  current value*/
  static u8 ddrunmv_cnt = 0;



    ddrunmv += IDDmeas;
    if(ddrunmv_cnt<10)
    {
        ddrunmv_cnt++;         
        return;
    }
    else
    {
        ddrunmv = ddrunmv/11;
        ddrunmv_cnt = 0;


    }
    //v1000_debug("IDDRUNMV: %d  \r\n",ddrunmv);
    if(IsBatteryPoweroff(ddrunmv))
    {
        //  headsetPowerOff(getApp());system_flag_table->batt_Status
        system_flag_table->batt_Status = 0xFF;
        if(KEY2 == 0)		 
            Choose_power_mode(MCU_STBY);
    }			  

    else
    {

        if (IsBatteryFull(ddrunmv))
        {
            if(system_flag_table->charger_connected == 1)
            {
                if(system_flag_table->batt_change_ok_cnt == 0)
                {
                    system_flag_table->batt_change_ok_cnt = 30*60*100;//ms
                    //system_flag_table->batt_Status  =  BATT_CHARG_OK;
                }
            }
            else
            {
                system_flag_table->batt_Status  = BATT_HIGH;
            }

        }                    

        else if (IsBatteryHIGH(ddrunmv))
        {
            if((system_flag_table->charger_connected == 1)&&(system_flag_table->batt_Status  ==  BATT_CHARG_OK))
                ;
            else	
                system_flag_table->batt_Status  =  BATT_HIGH;

        }       
        else if (IsBatteryMid(ddrunmv))
        {                        
            system_flag_table->batt_Status  =  BATT_MID;
        }
        else  if (IsBatteryLow(ddrunmv))
        {                                	           
            system_flag_table->batt_Status  =  BATT_LOW;
        }
        else if (IsBatteryDead(ddrunmv))
        {                        
            system_flag_table->batt_Status  = BATT_EMPTY;
        }

    }  

    ddrunmv = 0;
	
}





/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
void vddmv_adc_proess(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32l1xx_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32l1xx.c file
     */
    /* Read ADC conversion result */
__IO uint16_t ADCdata = 0;
__IO uint32_t VDDmV = 0;


    ADCdata = ADC_GetConversionValue(ADC1);
//  v1000_debug("ADCdata: %d  \r\n",ADCdata);
    /* Calculate voltage value*/
    VDDmV = (uint32_t)((uint32_t)ADCdata *6000/4095);
    
#ifdef USE_STM32L152_EVAL 
    /* Calculate the current consumption */
//    IDDrunuA = (uint32_t) ((uint32_t)(((VDDmV * EVAL_RESISTOR_RATIO) / EVAL_MAX9938_GAIN))\
 //                          / (uint32_t)(EVAL_RESISTOR_R36));
#elif defined USE_STM32L152D_EVAL
    /* Calculate the current consumption */
    IDDrunuA = (uint32_t) ((uint32_t)(((VDDmV * EVAL_RESISTOR_RATIO) / EVAL_MAX9938_GAIN))\
                           / (uint32_t)(EVAL_RESISTOR_R62));

#endif    

    /* Display the IDD measured Value On the LCD Glass (mA) */
    DisplayIDDrunmV(VDDmV);

}

/**
  * @brief  Configures the ADC1 channel5.
  * @param  None
  * @retval None
  */
void ADC_Config(void)
{

ADC_InitTypeDef ADC_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;


  /* Enable The HSI (16Mhz) */
  RCC_HSICmd(ENABLE);
  
  /* Enable the GPIOF or GPIOA Clock */
  RCC_AHBPeriphClockCmd(IDD_MEASUREMENT_GPIO_CLK, ENABLE);
  /* Configure PF.11 (ADC Channel11) or PA.05 (ADC Channe5) in analog mode */
  GPIO_InitStructure.GPIO_Pin =  IDD_MEASUREMENT_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(IDD_MEASUREMENT_GPIO, &GPIO_InitStructure);

  /* ADC1 Configuration ------------------------------------------------------*/
  
  /* Enable ADC1 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
#if 1//def USE_STM32L152D_EVAL
  /* Select ADC Bank channel */
  //ADC_BankSelection(ADC1, ADC_Bank_B);
#endif
  
  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel5 or channel1 configuration */
  ADC_RegularChannelConfig(ADC1, IDD_MEASUREMENT_ADC_CHANNEL, 1, ADC_SampleTime_96Cycles);

  /* Define delay between ADC1 conversions */
  ADC_DelaySelectionConfig(ADC1, ADC_DelayLength_Freeze);
  
  /* Enable ADC1 Power Down during Delay */
  ADC_PowerDownCmd(ADC1, ADC_PowerDown_Idle_Delay, ENABLE);
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
	v1000_debug("\r\n ADC_Cmd(ADC1, ENABLE)");
  /* Wait until ADC1 ON status */
  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET)
  {
  }
	v1000_debug("\r\n ADC_FLAG_ADONS");
  /* Start ADC1 Software Conversion */
  ADC_SoftwareStartConv(ADC1);
	v1000_debug("\r\n ADC_SoftwareStartConv");
  /* Wait until ADC Channel 5 or 1 end of conversion */
  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
  {
  }
  	v1000_debug("\r\n ADC_GetFlagStatus");
}






