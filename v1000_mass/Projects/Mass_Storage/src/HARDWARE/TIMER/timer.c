#include "sys.h"
#include "timer.h"
#include "led.h"
#include "gps.h"
#include "string.h"
#include "usart.h"
#include "usart2.h"
#include "menutal.h"
#include "bmp085.h"
#include "dma.h"
#include "key.h"
#include "rtc.h"
#include "oled.h"
#include "pwm.h"
#include "myadc.h"
#include "hw_config.h"
#include "myadc.h"
#include "delay.h"
#include "usb_lib.h"

///////////////////
///////////////////////////////////////////////////////////////
//All rights reservedadc
//////////////////////////////////////////////////////////////////////////////////

u8 timer_flag =0;
#ifdef baoyu
    u8 flash_baoyu_flag = 0;
#endif
//u8 USART1_TX_BUF[USART2_MAX_RECV_LEN]; 					//����1,���ͻ�����

#ifdef baoyu
extern u8 baoyun_flag;
extern const unsigned char Bmp_wanner[];
#endif

extern long  temperature;
extern u8 Arriving_sound_flag ;
extern u8 system_power_flag ;
//extern u16 system_flag_table->To_lower_timercnt ;
extern u8 enter_perss;

static u16 timer_1s_cnt = 0,save_tsp_s_timer = 0;
u16 powerdonw_resume_timer;
u16 flash_led_low_power_timer = 0;
u8 button_100_time = 0;
u8 guji_1s_time = 0;
u8 flash_led_flag = 0;
u32 save_tsp_tp_timer = 0;
static u16 wait_timer_cnt;
u32 average_speed  = 0;
extern u32 tatol_time ;
extern u8 guji_mode ;
extern u8 rtc_set_flag;

u16 usb_delay_timer_poweron = 0;
u16 usb_delay_timer_poweroff = 0;


extern const unsigned char Bmp_stanby_1[];

extern void change_clk_need_int(u8 mode);

/*
*
      API
*
*/

void timer_lower2_type(void)
{
    if((system_power_flag == SYSTEM_POWER_LOW_2)&&(system_flag_table->run_mode == 0))
    {
        flash_led_low_power_timer ++;
        if(flash_led_flag == 0)
        {
            if(flash_led_low_power_timer == 350)
            {
                flash_led_low_power_timer = 0;
                //LED0=1;
                Movie_Show_Img(58,24,Bmp_stanby_1);
                OLED_Refresh_Gram();//������ʾ
                flash_led_flag =1;
            }
        }
        else if(flash_led_flag == 1)
        {
            if(flash_led_low_power_timer == 10)
            {
                flash_led_low_power_timer = 0;
                OLED_Clear();
                OLED_Fill(58, 24, 70, 36, 0);
                OLED_Refresh_Gram();//������ʾ
                flash_led_flag = 2;
            }
        }
        else if(flash_led_flag == 2)
        {
            if(flash_led_low_power_timer == 20)
            {
                flash_led_low_power_timer = 0;
                OLED_Clear();
                Movie_Show_Img(58,24,Bmp_stanby_1);
                OLED_Refresh_Gram();//������ʾ
                flash_led_flag = 3;
            }
        }
        else if(flash_led_flag == 3)
        {
            if(flash_led_low_power_timer == 10)
            {
                flash_led_low_power_timer = 0;
                OLED_Clear();
                OLED_Fill(58, 24, 70, 36, 0);
                OLED_Refresh_Gram();//������ʾ
                flash_led_flag = 0;
            }
         }
     }
     else
     {
         flash_led_flag = 0;
         flash_led_low_power_timer = 0;
     }


}


void timer_guji_type(void)
{
    if(guji_mode >= RECORED_START_DOING)
    {
       guji_1s_time++;
       if(guji_1s_time == 100)
       {
           guji_1s_time  = 0;
           tatol_time += 1;
       }
    
    }
    else
    {
          guji_1s_time  = 0;
    }
       

}


void timer_powermanage_type(void)
{
    if(powerdonw_resume_timer)
    {
        powerdonw_resume_timer--;
        if(powerdonw_resume_timer == 0)
            menu_keyprocess(1);
    }

}



void timer_save_map_type(void)
{
    if(save_tsp_tp_timer == 90000)
    {
    }
    else
        save_tsp_tp_timer++;
    
    if(save_tsp_s_timer == 15000)
    {
    }
    else
        save_tsp_s_timer++;

    
}

void timer_when_user_press_key_type(void)
{
    if(enter_perss == 3)
    {
        wait_timer_cnt++;
        if(wait_timer_cnt == 300)
        {
            wait_timer_cnt = 0;
            enter_perss =0;
            OLED_Clear();
        }
    }
    else if (enter_perss == 4)
    {
        if(wait_timer_cnt == 200)
        {
            enter_perss =0;
            menu_enter_flash(wait_timer_cnt);
            wait_timer_cnt = 0;
        }
        else
            menu_enter_flash(wait_timer_cnt);
    
        wait_timer_cnt++;
    }

}
//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)
{
    static u8 static_rtc_cnt = 0;

	if(TIM3->SR&0X0001)//����ж�
	{

        timer_when_user_press_key_type();
        timer_lower2_type();
        timer_guji_type();
        timer_powermanage_type();
        timer_save_map_type();

       	if(timer_1s_cnt == 100)
        {

       	}
		else
			timer_1s_cnt++;

        if(system_flag_table->batt_change_ok_cnt)
        {
            if(system_flag_table->charger_connected == 1)
            {
                system_flag_table->batt_change_ok_cnt--;
                if(system_flag_table->batt_change_ok_cnt == 0)
                {
                     system_flag_table->batt_Status  =  BATT_CHARG_OK ;
                }
            }
            else
            {
                system_flag_table->batt_change_ok_cnt = 0;    
            }
        }
    
        if(button_100_time)
        {
            button_100_time --;
            if(button_100_time == 0)
                Button_key_perss(0);
        }
        
        if(static_rtc_cnt == 10)
        {
            static_rtc_cnt = 0;
            RTC_DateShow();
            RTC_TimeShow();
           
        }
        else
            static_rtc_cnt++;

     

         timer_flag = 1;

	}

	TIM3->SR&=~(1<<0);//����жϱ�־λ
}
//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!

void Timerx_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;//TIM3ʱ��ʹ��
 	TIM3->ARR=arr;  //�趨�������Զ���װֵ//�պ�1ms
	TIM3->PSC=psc;  //Ԥ��Ƶ��7200,�õ�10Khz�ļ���ʱ��
	//����������Ҫͬʱ���òſ���ʹ���ж�
	TIM3->DIER|=1<<0;   //���������ж�
//	TIM3->DIER|=1<<6;   //���������ж�
	TIM3->CR1|=0x01;    //ʹ�ܶ�ʱ��3
  	MY_NVIC_Init(1,3,TIM3_IRQn,2);//��ռ1�������ȼ�3����2
}




void Timer_task(void)
{
    static u8 timer_cnt = 0;
    static u16 timer_bmp085_cnt = 0;


#ifdef NEED_COM_TO_PC    
    extern u8 upload;
    extern u8 send_status ;
    extern u8 send_status_flag;
#endif    
    //extern u32 Flash_Index;
    u8 user_key =0;

    if(timer_flag)
    {
        timer_flag = 0;
    
        if(timer_cnt == 5)/**/
        {
            user_key = KEY_Scan();
            menu_keyprocess(user_key);
            user_key = 0;
            timer_cnt = 0;
            write_flash();


            if(USB_CONNECT == 1)
            {

                if(system_power_flag == SYSTEM_POWER_STANBY)
                    GPS_power_ctrl(0);

                if(usb_delay_timer_poweron)
                {
                    v1000_debug("\r\n usb_delay_timer_poweron:%d",usb_delay_timer_poweron);
                    usb_delay_timer_poweron--;
                    if(usb_delay_timer_poweron == 0)
                    {
                        system_flag_table->usb_standy_poweron_flag = 1;
                        menu_keyprocess(5);
#if 0                        
                        if(system_flag_table->guji_record.auto_recoed_flag == 1)
                        {
                            change_to_runmode_recoder(5);
                        }
#endif
                    }
                }
                usb_delay_timer_poweroff =0;
            }
            else
            {
                usb_delay_timer_poweron = 0;
                if(usb_delay_timer_poweroff)
                {
           
                    usb_delay_timer_poweroff--;
                    if(usb_delay_timer_poweroff == 0)
                    {
                        system_flag_table->usb_standy_poweron_flag = 0;
                        Choose_power_mode(MCU_STBY);
                        system_power_flag = SYSTEM_POWER_STANBY;
                    }
                }
            }

           if(USB_CONNECT == 1)
            {
                if(system_flag_table->charger_connected == 0)
                {    
                    if((system_flag_table->usb_powerdown_flash == 1)&&(system_power_flag == SYSTEM_POWER_STANBY))
                    {                
                        usb_delay_timer_poweron = 30;
                        Choose_power_mode(MCU_RUN_USB);
                        change_clk_need_int(1);
                       // USB_Init();
                       // USB_Interrupts_Config();
                        v1000_debug("\r\n usb_delay_timer_poweron :%x",usb_delay_timer_poweron);
                    }
                    else
                    {
                        Choose_power_mode(MCU_RUN_USB);
                        change_clk_need_int(1);
                        USB_Init();
                        USB_Interrupts_Config();
                        menu_keyprocess(0xff);
                        //OLED_Clear();
                        OLED_Display_On();                            		
                        //GPS_power_ctrl(0);
                    }

      
                    v1000_debug("\r\n change to 32 mhz");
                    
                    if(system_power_flag != SYSTEM_POWER_STANBY)
                        menu_reflash();

                }

                system_flag_table->charger_connected = 1;

            }
            else
            {

                if(system_flag_table->charger_connected == 1)
                {
                    v1000_debug("\r\n change to 16 mhz  :%d \r\n",system_flag_table->usb_standy_poweron_flag );
                    USB_Interrupts_Config_disable();
                    system_flag_table->charger_connected = 0;
                    system_flag_table->usbdisk_flag = 0;
                    if(system_flag_table->usb_standy_poweron_flag == 1)
                    {
                        //system_flag_table->usb_standy_poweron_flag = 0;
                        usb_delay_timer_poweroff = 30;
                    }
            
                    if((system_power_flag == 2)&&(system_flag_table->run_mode == 0))
                    {
                        Choose_power_mode(MCU_LP_RUN);
                       //menu_keyprocess(0xff);
                    }
                    else if(system_power_flag == 1)
                    {
                        Choose_power_mode(MCU_RUN);
                        change_clk_need_int(0);
            
                    }
                    if(system_power_flag != SYSTEM_POWER_STANBY)
                        menu_reflash();

                    menu_keyprocess(0xff);
                    read_poi_file();
                }


             }
        }
        else
        {
            timer_cnt ++;
        }
    
        if(timer_bmp085_cnt == 50)
        {
            timer_bmp085_cnt = 0;
            //v1000_debug("\r\n timer_bmp085_cnt\n");

            BMP085_get_process();

        }
        else
             timer_bmp085_cnt ++;
    

    }
}


#ifdef baoyu

void baoyu_1s_timer_proess(void)
{
       
    if(baoyun_flag==1)
    {

        if(flash_baoyu_flag <20)
        {
            flash_baoyu_flag ++;
            if(flash_baoyu_flag%2 != 0)
            {
                LED0 =1;
                show_baoyu_bmp();
    
            }
            else
            {
                LED0 =0;
            }
        }

     }


}

#endif

void arriving_sound_proess(void)
{
    if(Arriving_sound_flag != 0 && Arriving_sound_flag != 0xff)
    {
        Arriving_sound_flag++;
        if(Arriving_sound_flag == 2|| Arriving_sound_flag == 4 || Arriving_sound_flag == 6)
        {
            LED0 = 1;
            di_and_flash_empet(2);
            LED0 = 0;/*��Ҫ������Ӧ̫��ʱ��*/
            delay_ms(150);
            di_and_flash_empet(2);
            if(Arriving_sound_flag == 6)
            {
               LED0 = 0;
               Arriving_sound_flag = 0xff;
            }
    
         }
         else
            LED0 = 0;
    
      }

}
void powerlow_proess(void)
{
    if(system_flag_table->To_lower_timercnt)
    {
        system_flag_table->To_lower_timercnt--;
        if(system_flag_table->To_lower_timercnt == 0)
        {
            OLED_Fill(0, 0, 127, 63, 0);
            if((system_flag_table->run_mode == 0)&&(usb_delay_timer_poweron == 0))
            {
                if(system_power_flag == SYSTEM_POWER_STANBY)
                {
                    if(USB_CONNECT  == 0)
                    {

                        Choose_power_mode(MCU_STBY);
                    }
                }
                else
                {
                    system_power_flag = 3;
                    if(USB_CONNECT  == 0)
                    {
                         Choose_power_mode(MCU_LP_RUN);
                    }
                }
            }
            else
                system_power_flag = 2;
            OLED_Refresh_Gram();
        }
    }
}

u8 handler_timer_1s(void)
{
    
    if(timer_1s_cnt == 100)
    {
        timer_1s_cnt = 0;

        vddmv_adc_proess();

        if(system_power_flag == SYSTEM_POWER_STANBY)
        {
            //v1000_debug("vddmv_adc_proess \r\n");
            if((USB_CONNECT == 0)&&(usb_delay_timer_poweron == 0)&&(KEY2 == 0))
            {
                system_power_flag = SYSTEM_POWER_STANBY;
                Choose_power_mode(MCU_STBY);
            }
            return 1;
        }
        else
        {
            arriving_sound_proess();
            powerlow_proess();
            #ifdef baoyu
               baoyu_1s_timer_proess(); 
            #endif
        }
    }
   return 0;    
}

void gps_data_receiver(void)
{
    u16 rxlen = 0;

    if(USART2_RX_STA&0X8000)        //���յ�һ��������
    {
        //��ʾ��Ϣ
        rxlen = USART2_RX_STA&0X7FFF;   //�õ����ݳ���
        
#ifdef NEED_COM_TO_PC   

        for(i=0;i<rxlen;i++)
        {
            USART1_TX_BUF[i]=USART2_RX_BUF[i];
         
            if(gps_v1000_debug_status == 0)
            {
                USART_To_USB_Send_Data(USART1_TX_BUF[i]);

            }

        }
#endif           
    
        USART2_RX_BUF[rxlen] = 0;           //�Զ����ӽ�����
        //v1000_debug("%s",USART2_RX_BUF);        
        //memset(gpsx,0,sizeof(nmea_msg));
        GPS_Analysis(gpsx,(u8*)USART2_RX_BUF);//�����ַ���
        USART2_RX_STA=0;           //������һ�ν���
        
        if(average_speed == 0xffff)
            average_speed = gpsx->speed;
        else
            average_speed =(average_speed + gpsx->speed)/2;

#if 0
        {
            v1000_debug("\nDMA DATA:");
            USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);           //ʹ�ܴ���1��DMA����
            MYDMA_Enable(DMA1_Channel4);//��ʼһ��DMA���䣡
        }
#endif
        guji_Distance();
        menu_3d_seq();

        if(rtc_set_flag == 1)
        {
            if(gpsx->svnum >= 3)
            {
                rtc_set_flag = 0;            
                 check_time(1);
            }
        }
        else
        {
            if(gpsx->svnum < 2)
            {
                rtc_set_flag = 1;
            }

        }
    }

}

void main_porcess(void)
{
    u8 ret =0;
#ifdef NEED_COM_TO_PC    
    extern u8 gps_v1000_debug_status ;
#endif

    gps_data_receiver();

    ret = handler_timer_1s();

    if(system_flag_table->run_mode == 1)
    {
        if(system_flag_table->recored_run_flag == 1)
        {
            system_flag_table->recored_run_flag = 0;
            Recording_guji();
    
        }

    }
    
    if(ret == 1)
    {
        return;
    }    
    if(save_tsp_tp_timer == 90000)
  	{
  	    save_tsp_tp();
	    save_tsp_tp_timer = 0;
  	}

    if(save_tsp_s_timer == 15000)
  	{
        save_tsp_s();
	    save_tsp_s_timer = 0;
  	}
    
#ifdef NEED_COM_TO_PC
    if(send_status_flag == 1)
    {
        test_read_flash_and_v1000_debug();
        send_status_flag = 0;
    }
#endif
}










