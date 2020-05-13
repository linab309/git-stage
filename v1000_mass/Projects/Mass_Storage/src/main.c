/*!
*****************************************************************************
** \file        src/main.c
**
** \version     $Id$
**
** \brief       main.c
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. GOKE MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS
**
** (C) Copyright 2012-2013 by GOKE MICROELECTRONICS CO.,LTD
**
*****************************************************************************
*/
/* Author             : lin jian sen
* Version            : V1.0
* Date               : 08/26/2013
* Description        :main
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/

#include "delay.h"
#include "usart.h"
#include "usart2.h"
#include "sys.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "stm32l1xx_rcc.h"
#include "spi.h"
#include "flash.h"
#include "usb_lib.h"
#include "pwm.h"
#include "ff.h"
#include "hw_config.h" 
#include "usb_pwr.h"
#include "led.h"
#include "rtc.h"
#include "timer.h"
#include "myadc.h"
#include "myiic.h"
#include "gps.h"
#include "oled.h"
#include "Bmp085.h"
#include "stm_eeprom.h"
#include "menutal.h"
#include "string.h"
#include "key.h"



#define SYSTEM_CLOCK_CNT 24
#define NO_BMP085
#define MY_GITHUB 0



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/

const u8 *COMPILED_TIME=__TIME__;//��ñ���ʱ��
const u8 *COMPILED_DATE=__DATE__;//��ñ�������

//extern const u8 BMP_logo[];
//
//extern u8 system_power_flag ;
nmea_msg *gpsx; 	

system_flag *system_flag_table;
extern u8 guji_mode;



/*------------------------------------------------------------/
/ Open or create a file in append mode
/------------------------------------------------------------*/

FRESULT open_append (
    FIL* fp,            /* [OUT] File object to create */
    const char* path    /* [IN]  File name to be opened */
)
{
    FRESULT fr;
    /* Opens an existing file. If not exist, creates a new file. */
    fr = f_open(fp, path,FA_READ|FA_WRITE|FA_OPEN_ALWAYS);
    //v1000_debug("open append ok :%d \r\n",fr);
    if (fr == FR_OK) {
        /* Seek to end of the file to append data */
        fr = f_lseek(fp, f_size(fp));
        //v1000_debug("f_lseek :%d \r\n",fr);
        if (fr != FR_OK)
            f_close(fp);
    }
    return fr;
}



FRESULT scan_directory_oldfile (
    char* path,      /* Working buffer filled with start directory */
    char* old_path,      /* Working buffer filled with start directory */
    unsigned long* date_time,
    char mode,
    int *filenumer

)
{
    UINT i, j;
    FRESULT fr;
    DIR dir;
    FILINFO fno;
#if _USE_LFN
    fno.lfname = 0; /* Disable LFN output */
#endif
    fr = f_opendir(&dir, path);
    if (fr == FR_OK) 
    {
        for (i = 0; path[i]; i++) ;
        path[i++] = '/';
        for (;;) 
        {
            fr = f_readdir(&dir, &fno);
            if (fr != FR_OK || !fno.fname[0]) break;
            if (_FS_RPATH && fno.fname[0] == '.') continue;
            j = 0;
            do
                path[i+j] = fno.fname[j];
            while (fno.fname[j++]);
            if (fno.fattrib & AM_DIR) 
            {   
                v1000_debug("fno.name :%s\r\n",fno.fname);
                
                if((strcmp(fno.fname,"POI") != 0)&&(strcmp(fno.fname,"SYSTEM~1") != 0))
                {            
                    fr = scan_directory_oldfile(path,old_path,date_time,mode,filenumer);
                    if (fr != FR_OK) break;
                
                
                    if(mode == 1)
                    {
                        j = 0;
                        do
                           old_path[j] = path[j];
                        while (path[j++]);
                        fr = f_unlink(path);
                        if (fr != FR_OK) break;                      
                    }
                               
                }

            }
            else
            {
                *filenumer +=1;
                if((*date_time > ((fno.fdate <<16 )|fno.ftime))&&(mode == 0))
                {
                    *date_time = ((fno.fdate <<16 )|fno.ftime);
                    j = 0;
                    do
                        old_path[j] = path[j];
                    while (path[j++]);
                   
                    v1000_debug("name :%s, data :%x, timer:%x  j:%d\r\n",path,fno.fdate,fno.ftime,*filenumer);
                }
                
                if(mode == 1)
                {
                    j = 0;
                    do
                        old_path[j] = path[j];
                     while (path[j++]);
                    fr = f_unlink(path);
                    if (fr != FR_OK) break;     
                     
                }                
                
            }

        }
        path[--i] = '\0';
        f_closedir(&dir);
    }
    

    return fr;
}


void change_clk_need_int(u8 mode)
{


#if EN_USART1
    uart_init(115200);
#endif

    if(mode == 0)
      delay_init(16);	     //��ʱ��ʼ��
    else
      delay_init(32);	     //��ʱ��ʼ��

    if(mode == 0)
    	USART2_Init(16,38400);	//��ʼ������2
    else
    	USART2_Init(32,38400);	//��ʼ������2
    // 	v1000_debug("\n\r Flash_Index :%d",Flash_Index);
    //	Timerx_Init(99,3199);//10Khz  ����4999 --- 500ms
    if(mode == 0)
        Timerx_Init(99,1599);//10Khz  ����4999 --- 500ms
    else
        Timerx_Init(99,3199);//10Khz  ����4999 --- 500ms

    SPI_Flash_Init();   //SPI FLASH ��ʼ��
//    PWM_Init();
    ADC_Config();
}


void* v1000_memalloc (	/* Returns pointer to the allocated memory block */
	UINT msize		/* Number of bytes to allocate */
)
{
	return malloc(msize);	/* Allocate a new memory block with POSIX API */
}

void mkdir_init(unsigned char mode)
{
    static FATFS *fs = NULL;
    FRESULT fr;

    if(fs == NULL)
    {
        fs = v1000_memalloc(sizeof(FATFS));
    }
    fr = f_mount(fs, "", 0);

    fr = f_mkdir("POI");
    v1000_debug("\r\n mkdir_init %d \r\n",fr);
    

    if((fr == FR_EXIST)&&(mode == 0))
        return;
    else 
    {
        fr = f_mkfs("", 0, 0); 
        fr = f_mount(fs, "", 0);
        fr = f_mkdir("POI");
        if(fr != FR_OK)
        {
            v1000_debug("\r\n mkfs faild");
        }
    }
}

#if 1
void Del_oldfile_nospace(void)
{
    FRESULT fr = FR_OK;
    unsigned long data_time = 0xffffffff;
    char buff[64] = {0};    /* Working buffer */
    char out_buff[64] = {0};    /* Working buffer */
    int filenumber = 0;
	extern u16  Flash_write_buffer_Index ;
    
    strcpy(buff, "/");  /* Directory to be emptied */

    show_levev_space();

    fr = scan_directory_oldfile(buff,out_buff,&data_time,0,&filenumber);
     
    if (fr) 
    {
        v1000_debug("Function failed. (%u) \r\n", fr );
        return ;
    } 
    else 
    {
        v1000_debug("found oldest file :%s.%d\r\n", out_buff,filenumber);
        if((data_time != 0xffffffff)&&(filenumber > 1))
        {
            fr = f_unlink(out_buff);
            if (fr == FR_OK)
                show_levev_space();
            return ;
        }
        else if (filenumber <= 1)
        {
             
			 guji_mode = RECORED_STOP;
			 Flash_write_buffer_Index = 0;
             
        }
    }
}


void Del_allgujifile(void)
{
    FRESULT fr = FR_OK;
    unsigned long data_time = 0xffffffff;
    char buff[64] = {0};    /* Working buffer */
    char out_buff[64] = {0};    /* Working buffer */
    int filenumber = 0;
    
    strcpy(buff, "/");  /* Directory to be emptied */

    

    fr = scan_directory_oldfile(buff,out_buff,&data_time,1,&filenumber);
    show_levev_space();  
    if (fr) 
    {
        v1000_debug("Function failed. (%u)\r\n", fr);
        return ;
    } 
    else 
    {
        v1000_debug("found oldest file :%s. %d\r\n", out_buff,filenumber);

    }
}

#endif



int main (void)
{
    extern u8 rtc_set_flag;

    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x3000);

  
#if 0
    if (PWR_GetFlagStatus(PWR_CSR_WUF) != RESET)
    {
        /* System resumed from STANDBY mode */
        /* Clear StandBy flag */

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
        PWR_ClearFlag(PWR_FLAG_SB);
        /* set StandbyWakeup indicator*/
        //StanbyWakeUp = TRUE;
        //LED0 =0;
    }
    else
    {
        /* Reset StandbyWakeup indicator*/
        //StanbyWakeUp = FALSE;
        //LED0 =1;
    }
#endif
   if(gpsx == NULL)
    {
        gpsx = v1000_memalloc(sizeof(nmea_msg));    
        memset(gpsx,0,sizeof(nmea_msg));
    }
   
    if(system_flag_table == NULL)
    {
        system_flag_table = v1000_memalloc(sizeof(system_flag));    
        memset(system_flag_table,0,sizeof(system_flag));
    }
    
    __enable_irq();
    delay_init(32);      //��ʱ��ʼ�//


#if EN_USART1
    uart_init(115200);
#endif
    //for_test_gps_gpx();

    IIC_Init();
    OLED_Init();			//???OLED
    My_RTC_Init();
    LED_Init();
    //OLED_ShowString12(16,28,"!!!!");

   // v1000_show_logo();
    KEY_Init();				//��ʼ����LED���ӵ�Ӳ���ӿ�
    SPI_Flash_Init();   //SPI FLASH ��ʼ��
    SPI_Flash_WAKEUP();
    SPIx_SetSpeed(SPI_SPEED_2);
    delay_ms(50);
    SPI_Flash_WAKEUP();

    v1000_debug("version :v-1.0.0 \r\n");
    /* Open or create a log file and ready to append */

    mkdir_init(0);
    read_poi_file();
    //Del_oldfile_nospace();
    USART2_Init(32,38400);	//?????2
    Timerx_Init(99,3199);//10Khz  ??4999 --- 500ms
    check_eeprom_init();
    Send_to_goto_lowpower();
    bmp_moudle_preinit();
//    PWM_Init();
    ADC_Config();
    Button_key_perss(0);
#ifndef NO_BMP085
    BMP085_get_process();
#endif
    restore_pass();
    OLED_Clear();
    Set_System();
    Set_USBClock();
    system_flag_table->charger_connected = 0;


    if(USB_CONNECT == 1)
    {
        //system_flag_table->charger_connected = 1;
        USB_Interrupts_Config();
        USB_Init();
    }
  //  else
        
    Choose_power_mode(MCU_RUN);
    change_clk_need_int(0);	
    rtc_set_flag = 1;
    is_update_to_poweron();
//    test_angle();
    while(1)
    {
        Timer_task();
        main_porcess();
    }

 }


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: v1000_debug("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


