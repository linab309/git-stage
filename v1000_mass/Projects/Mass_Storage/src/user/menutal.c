/*!
*****************************************************************************
** \file        src/user/menutal.c
**
** \version     $Id$
**
** \brief       �����˵�������
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. GOKE MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS
**
** (C) Copyright 2012-2013 by GOKE MICROELECTRONICS CO.,LTD
**
*****************************************************************************
*/
/*                                                                      
* Author           : jianjian
* Version            : V1.0
* Date               : 08/26/2013
* Description       ://C���Բ˵����
*/
#include "sys.h"
//#include "stm32l1xx.h"
#include "oled.h"
#include "gps.h"
#include "menutal.h"
#include "string.h"
//#include "bigfont.h"
#include "oledhz.h"
#include "Bmp085.h"
#include "usart.h"
#include "bigfont.h"
#include "rtc.h"
#include "Gongshi.h"
#include "stm_eeprom.h"
#include "usart2.h"
#include "flash.h"
#include "led.h"
#include "math.h"
#include "pwm.h"
#include "hw_config.h"
#include "delay.h"
#include "ff.h"
#include "ffconf.h"
#include "usb_pwr.h"


#if 0
uc8 menu1[]  ={"1,ʵʱ��Ϣҳ"};
uc8 menu2[]  ={"2,����ҳ"};
uc8 menu3[]  ={"3,���ݱ仯ҳ"};
uc8 menu4[]  ={"4,�����Ϣҳ"};
uc8 menu5[]  ={"5,������Ϣ"};
uc8 menu6[]  ={"6,�����"};
uc8 menu7[]  ={"7.��ϫ��"};
uc8 menu8[]  ={"8.���ò˵�"};
uc8 menu8_a[]  ={"1,.�����ʾ��ʽ"};
uc8 menu8_b[] ={"2,���뵥λ"};
uc8 menu8_c[] ={"3,�¶ȵ�λ"};
uc8 menu8_d[] ={"4,��¼���"};
uc8 menu8_e[] ={"5,�Զ�����ʾҳ��"};
uc8 menu8_f[] ={"6,����ϵͳ����"};
uc8 menu8_g[] ={"7,ʱ������"};
#endif



#define MENU_MAIN 0
#define MENU_COORDINATES     1
#define MENU_REALTIME     2
#define MENU_TRANSFORMATION    3
#define MENU_MILEAGE       4
#define MENU_SATELLITE      5
#define MENU_SETTING_TEMPERATURRE     6
#define MENU_RIDE_TABLE     7
#define MENU_BAROMETER    8
#define MENU_SETTING      10
#define MENU_SETTING_COOR    9
#define MENU_SETTING_DISTANCE     11

#define MENU_SETTING_RECORD_SPACE      12
#define MENU_SETTING_DISPLAYMENU    13
#define MENU_SETTING_LANGUAGE   14
#define MENU_STTING_TIMEZONE   15

#define  MAX_KEYTABSTRUCT_NUM 12




typedef struct
{
	u32 latitude;				//γ�� ������100000��,ʵ��Ҫ����100000
	u8 nshemi;					//��γ/��γ,N:��γ;S:��γ				  
	u32 longitude;			    //���� ������100000��,ʵ��Ҫ����100000
	u8 ewhemi;					//����/����,E:����;W:����
}HomeGpsStruct;

static HomeGpsStruct homex = {0};

static u8 home_flag = 0;
#define CUSTOM_HOME_POST 12


__align(4) u8 dtbuf[50];   								//��ӡ������


u8 rtc_set_flag= 1;
//extern u8 ge,shi,bai,qian,wan,shiwan;           //��ʾ����
extern u32 average_speed;
extern int16_t  temperature;
extern long  pressure;
extern long Altitude;											//���θ߶�
u8 upload = 0;
u8 enter_perss= 0;
float time_zone = 8;
signed char home_menu_item_index = 0,menu_item_index = 0,guji_icon_index = 0;
extern RTC_TimeTypeDef RTC_TimeStructure;
extern RTC_DateTypeDef RTC_DateStructure;


u8 old_div_cnt = 0xff,old_spped_div_cnt = 0xff,old_altiude_div_cnt = 0xff;

u8 menu_id_show_timer = 10;
u8 key_type = 0xff;

static u8 need_to_powerdownflag = 0;


u8   system_power_flag = SYSTEM_POWER_STANBY;

u8   Arriving_sound_flag = 0;

//u8   Flash_full_flag = 0;
u32  Flash_Index = 0;
u16  Flash_write_buffer_Index = 0;
u32  Message_head_number = 0;
u32  tatol_time = 0;
u32  tatol_oled_time = 0;
u8   guji_mode =0;
float guiji_distance = 0;

char guji_latitude1_flag,guji_longitude1_flag;
float guji_tp_long_dest,guji_tp_lati_dest;


extern u16 powerdonw_resume_timer;

FIL sys_fp ;
FRESULT sys_fr ;
tm my_timer;

int16_t temperature_pass[96];
int16_t speed_pass[96];
s32 pressure_pass[96];
long Altitude_pass[96];
long  most_Altitude = 0;
long lest_Altitude = 0;
int16_t most_temperature = 0,lest_temperature = 0;
int16_t most_speed = 0,lest_speed = 0;
s32 most_pressure = 0,lest_pressure = 0;

u8 keyTabIndex = MENU_MAIN;
u8 ParMenuID_loop = MENU_MAIN;
u8 menu_change_index = MENU_TRANSFORMATION;
u8 menu_updown_flag = 0;
//u8 Guijisave_BUF[4096];
/*big font extern define*/


u8 menu_setting_index = 0xff;
u8 menu_setting_page  = PAGE_RUN_MODEL;
u8 menu_setting_item  = SENSOR_MODEL;
long menu_setting_postion  = 0;
long menu_setting_postion_2  = 0;

const u8 arry_bydistance[5]={5,10,20,50,100};
const u16 arry_bydistance_2[5]={20,50,100,200,500};
const u8 arry_time[5]={1,5,10,30,60};
static u32 setting_tp;



/*endf*/
void menu_arruite_change_display(void);
void menu_real_crood_display(void);
void menu_display_setting(void);

extern FRESULT open_append (
    FIL* fp,            /* [OUT] File object to create */
    const char* path    /* [IN]  File name to be opened */
);


typedef union { /* PMU_IRQ_CLR_RTC */
    u8 all;
    struct {
        u8 tag                      : 2;
        u8 ns                       : 1;
        u8 ew                       : 1;
        u8                          : 4;
    } bitc;
} GUJI_TAG;
//0100 0010 0000 0000 
#if 0
typedef union { /* FPC_Reg6964_Dat6 */
    u32 all;
    struct {             
        u32 year                       : 6;
        u32 month                       : 4;
        u32 date                      : 5;
        u32 hour                      : 5;
        u32 min                     : 6;
        u32 sec                      : 6;
    } bitc;
} GUJI_DATE;
#else
//0000 0011 0011 1111 0111
typedef union { /* FPC_Reg6964_Dat6 */
    u32 all;
    struct {             
        u32 sec                      : 6;
        u32 min                      : 6;
        u32 hour                     : 5;
        u32 date                     : 5;      
        u32 month                    : 4;
        u32 year                     : 6;
    } bitc;
} GUJI_DATE;
#endif


typedef struct
{
     u8 MenuID;      //���˵�״̬������
     u8 ParMenuID;    //���˵�״̬������
     u8 *MenuName;  //�˵��ı�����
     void (*CurrentOperate)(void); //���˵��Ĺ��ܺ���
     u8 subnumber;      //���˵�״̬������
}KeyTabStruct;
//�˵�����



const KeyTabStruct KeyTab[MAX_KEYTABSTRUCT_NUM] =
 {
// �˵�ID                ���˵�ID       �˵���        �˵����ܺ���
// MenuID,              ParMenuID,     MenuName,      CurFunction
	{MENU_MAIN, 0, NULL, menu_main_display,0},
	{MENU_COORDINATES, 0, NULL, menu_real_speed_display,0},
	{MENU_REALTIME, 0, NULL, menu_real_crood_display,0},
	{MENU_TRANSFORMATION,    0,   NULL,  menu_temp_change_display,2},
	{MENU_MILEAGE,    MENU_TRANSFORMATION,   NULL,  menu_Pressure_display ,0},
	{MENU_SATELLITE,  MENU_TRANSFORMATION,      NULL,  menu_arruite_change_display,0},
	{MENU_SETTING_TEMPERATURRE,  MENU_TRANSFORMATION,  NULL,menu_speed_change_display  ,0},
	{MENU_RIDE_TABLE,0,  NULL,      menu_3D_display,0},
	{MENU_BAROMETER, 0,  NULL,  menu_home_display,'H'},
	{MENU_SETTING_COOR, 0,  NULL,  menu_guiji_display,'G'},
	{MENU_SETTING, 0,  NULL,  menu_chaoxi_display,'C'},
	{MENU_SETTING_DISTANCE, 0,  NULL,  menu_display_setting,'S'},

 };


const u8 timer_zone_Aarry[][7]=
{
	"-12:00",   // 1
	"-11:00",   // 2
//	"-10:30",   // 3
	"-10:00",   // 4
	"-09:30",   //5
	"-09:00",
//	"-08:30",
	"-08:00",
	"-07:00",
	"-06:00",
	"-05:00",
	"-04:30",
	"-04:00",
	"-03:30",
	"-03:00",
	"-02:30",
	"-02:00",
	"-01:00",
//	"-00:44",
//	"-00:25",
	"+00:00",
//	"+00:20",
//	"+00:30",
	"+01:00",
	"+02:00",
	"+03:00",
	"+03:30",
	"+04:00",
	"+04:30",
//	"+04:51",
	"+05:00",
	"+05:30",
//	"+05:40",
	"+05:45",
	"+06:00",
	"+06:30",
	"+07:00",
//	"+07:20",
//	"+07:30",
	"+08:00",
//	"+08:30",
	"+08:45",
	"+09:00",
	"+09:30",
//	"+09:45",
	"+10:00",
	"+10:30",
	"+11:00",
	"+11:30",
	"+12:00",
	"+12:45",
	"+13:00",
	"+13:45",
	"+14:00",
};

//Monday Tuesday Wednesday Thursday Friday Saturday SunUay
const u8 week_word[7][3]={"SUN","MON","TUE","WED","THU","FRI","SAT"};


#ifdef baoyu
u8 baoyun_flag = 0;
u8 baoyun_flag_cnt = 0;
#endif


void read_poi_file(void)
{

    FIL fil ;
    UINT br = 0;
    u8 buffer[28];
    u8 dx = 0,posx = 0;
    u32 data;
    int index =0;
    u32 flag = 0;
    FRESULT fr = FR_OK;
    
    for(index = 0;index<16;index++)
    {
        sprintf((char *)dtbuf,"POI/%02d.txt",index+1);
       // v1000_debug("dtbuf :%s",dtbuf);
        fr = f_open(&fil,(char *)dtbuf,FA_READ);
        v1000_debug("dtbuf :%s :%d ",dtbuf,fr);
        fr = f_read(&fil,buffer,25, &br);
        v1000_debug("br :%d :%d\r\n",br,fr);
        if((br <= 25) && (br >= 23))
        {
            //v1000_debug("buffer :%s",buffer);
            data  = NMEA_Str2num(buffer,&dx);
            v1000_debug("data :%d \r\n",data);
            v1000_debug("dx :%d \r\n",dx);            
            stm_write_eerpom((HOME_WEIDU_ADDER+(3*index)), data*NMEA_Pow(10,6-dx));
            //stm_read_eerpom((HOME_WEIDU_ADDER+(3*index)), &data);
            //v1000_debug("read data :%d \r\n",data);

            posx=NMEA_Comma_Pos(buffer,1);
            v1000_debug("posx :%d  %c\r\n",posx,*(buffer+posx)); 
            flag = *(buffer+posx) <<8;
            posx=NMEA_Comma_Pos(buffer,2);
            data  = NMEA_Str2num(buffer+posx,&dx);
            v1000_debug("data1 :%d \r\n",data);
            v1000_debug("dx1 :%d \r\n",dx);  
            stm_write_eerpom((HOME_JINDU_ADDER+(3*index)), data*NMEA_Pow(10,6-dx)); 
            posx=NMEA_Comma_Pos(buffer,3);
            v1000_debug("posx :%d  %c\r\n",posx,*(buffer+posx)); 
        	flag |= *(buffer+posx) ;
        	//stm_write_eerpom((HOME_WEIDU_ADDER+(3*index)), lat1);
        	stm_write_eerpom((HOME_WEIDU_FLAG_ADDER+(3*index)), flag);
        	           
        }
        else
        {
             stm_write_eerpom((HOME_WEIDU_ADDER+(3*index)), 0); 
             stm_write_eerpom((HOME_JINDU_ADDER+(3*index)), 0); 
             stm_write_eerpom((HOME_WEIDU_FLAG_ADDER+(3*index)), 0);
        }
        f_close(&fil);
    }

}



u8 what_is_numbe_10(float number_vaule)
{
    if(number_vaule < 0)
    {
    	if(number_vaule <= -1000.0)
    		return 5;
    	if(number_vaule <= -100.0)
    		return 4;
    	if(number_vaule <= -10.0)
    		return 3;
    	if(number_vaule < 0.0)
    		return 2;

    }
    else
    {
    	if(number_vaule >= 100000.0)
    		return 6;
    	if(number_vaule >= 10000.0)
    		return 5;
    	if(number_vaule >= 1000.0)
    		return 4;
    	if(number_vaule >= 100.0)
    		return 3;
    	if(number_vaule >= 10.0)
    		return 2;
    	if(number_vaule > 0.0)
    		return 1;

    }
    return 1;


}


void check_eeprom_init(void)
{
    u32 check_data;
    stm_read_eerpom(MENU_SETTING_INDEX_ADDRES,&check_data);
	

    v1000_debug("check_data:%x \r\n",check_data);
    if(check_data != 0x200167)
    {
        stm_write_eerpom(MENU_SETTING_RUNMODEL_ADDRES,1);
        stm_write_eerpom(MENU_SETTING_GUJI_BY_DISTANCE,0);
        stm_write_eerpom(MENU_SETTING_GUJI_BY_TIME,0);
        stm_write_eerpom(MENU_SETTING_GUJI_BY_SPEED,0);
        stm_write_eerpom(MENU_SETTING_GUJI_BY_SPEED_1,0);
        stm_write_eerpom(MENU_SETTING_GUJI_BY_SPEED_2,0);
        stm_write_eerpom(MENU_SETTING_GUJI_RECOREDMETH,0);
        stm_write_eerpom(MENU_SETTING_RECODED_AUTOFLAG,0);
        stm_write_eerpom(MENU_SETTING_UNIT_TIMER,0);
        stm_write_eerpom(MENU_SETTING_UNIT_DATA,0);
        stm_write_eerpom(MENU_SETTING_UNIT_COORD,0);
        stm_write_eerpom(MENU_SETTING_UNIT_HW,0);
        stm_write_eerpom(MENU_SETTING_UNIT_TMPE,0);
        stm_write_eerpom(MENU_SETTING_UNIT_PREE,0);

        stm_write_eerpom(MENU_SETTING_FALME,0);
        stm_write_eerpom(MENU_SETTING_HEIGHT,70);
        stm_write_eerpom(MENU_SETTING_WEIGHT,40);

        stm_write_eerpom(MENU_SETTING_AJUST_ARR,0);
        stm_write_eerpom(MENU_SETTING_SCREER_TIMER,2);
        stm_write_eerpom(MENU_SETTING_TIMER_ZONE,16);
        stm_write_eerpom(MENU_SETTING_LANG,1);


        stm_write_eerpom(MENU_SETTING_INDEX_ADDRES,0x200167);
        stm_write_eerpom(MENU_SETTING_AJUST_FLAG ,0);
        stm_write_eerpom(CHANGE_TMPE_VAULE_INDEX_ADDRES,0);
        stm_write_eerpom(CHANGE_SPEED_VAULE_INDEX_ADDRES,0);
        stm_write_eerpom(CHANGE_ALTITUD_VAULE_INDEX_ADDRES,0);
        stm_write_eerpom(MENU_GUJI_MODE_INDEX_ADDRES ,0);
        stm_write_eerpom(MENU_SETTING_POWERCTROL ,0);
        //stm_write_eerpom(MENU_POWERDOWNTIME_INDEX_ADDRES ,0);
        stm_write_eerpom(MENU_SETTING_STROMALRRM  ,0);
        stm_write_eerpom(MENU_SETTING_WANNER_SOUND,1);
        stm_write_eerpom(MENU_FILE_NUMBER,0);
        stm_write_eerpom(MENU_SETTING_RECODED_FORMATS ,0);

    }

    stm_read_eerpom(MENU_SETTING_RUNMODEL_ADDRES ,&check_data);
    system_flag_table->run_mode= check_data;
    v1000_debug("system_flag_table->run_mode:%d \r\n",system_flag_table->run_mode);
    if(system_flag_table->run_mode)
    {
		GPS_power_ctrl(1);
		USART_Cmd(USART2, ENABLE);
                    // - 6
    }
    else
    {
		// -3
		GPS_power_ctrl(0);
		USART_Cmd(USART2, DISABLE);
    }
 // stm_read_eerpom(MENU_SETTING_STROMALRRM  ,&check_data);
//  system_flag_table->stormalram = check_data;
    stm_read_eerpom(MENU_SETTING_WANNER_SOUND,&check_data);
    system_flag_table->wanner_sound= check_data;

    stm_read_eerpom(MENU_SETTING_GUJI_BY_MODE ,&check_data);
    system_flag_table->guji_record.by_distance_time_flag= check_data;

    stm_read_eerpom(MENU_SETTING_GUJI_BY_DISTANCE ,&check_data);

    system_flag_table->guji_record.by_distance_vaule_ft= arry_bydistance_2[check_data];
    system_flag_table->guji_record.by_distance_vaule= arry_bydistance[check_data];

    stm_read_eerpom(MENU_SETTING_GUJI_BY_TIME ,&check_data);
    system_flag_table->guji_record.by_time_vaule= arry_time[check_data];

    stm_read_eerpom(MENU_SETTING_GUJI_BY_SPEED ,&check_data);
    system_flag_table->guji_record.by_speed_vaule= check_data;

    stm_read_eerpom(MENU_SETTING_GUJI_BY_SPEED_1 ,&check_data);
    system_flag_table->guji_record.by_speed_vaule_1= check_data;

    stm_read_eerpom(MENU_SETTING_GUJI_BY_SPEED_2 ,&check_data);
    system_flag_table->guji_record.by_speed_vaule_2= check_data;

    stm_read_eerpom(MENU_SETTING_GUJI_RECOREDMETH ,&check_data);
    system_flag_table->guji_record.recoed_meth= check_data;

    stm_read_eerpom(MENU_SETTING_RECODED_AUTOFLAG ,&check_data);
    system_flag_table->guji_record.auto_recoed_flag= check_data;

    stm_read_eerpom(MENU_SETTING_UNIT_TIMER ,&check_data);
    system_flag_table->myunit.TIMER= check_data;

    stm_read_eerpom(MENU_SETTING_UNIT_DATA ,&check_data);
    system_flag_table->myunit.DATA= check_data;

    stm_read_eerpom(MENU_SETTING_UNIT_COORD ,&check_data);
    system_flag_table->myunit.COORD= check_data;

    stm_read_eerpom(MENU_SETTING_UNIT_HW ,&check_data);
    system_flag_table->myunit.H_W= check_data;

    stm_read_eerpom(MENU_SETTING_UNIT_TMPE ,&check_data);
    system_flag_table->myunit.TEMP= check_data;

    stm_read_eerpom(MENU_SETTING_UNIT_PREE ,&check_data);
    system_flag_table->myunit.PREE= check_data;

    stm_read_eerpom(MENU_SETTING_FALME ,&check_data);
    system_flag_table->personal_da.flame= check_data;
    stm_read_eerpom(MENU_SETTING_HEIGHT ,&check_data);
    system_flag_table->personal_da.height= check_data+100;
    stm_read_eerpom(MENU_SETTING_HEIGHT_1 ,&check_data);
    system_flag_table->personal_da.height_ft= (check_data+30)/0.328;
    stm_read_eerpom(MENU_SETTING_WEIGHT ,&check_data);
    system_flag_table->personal_da.weight = check_data+20;
    stm_read_eerpom(MENU_SETTING_WEIGHT_1 ,&check_data);
    system_flag_table->personal_da.weight_ft = ((check_data+40)*0.4536);


    stm_read_eerpom(MENU_SETTING_AJUST_FLAG ,&check_data);
    system_flag_table->ajust_Arr_flag = check_data;
    stm_read_eerpom(MENU_SETTING_AJUST_ARR ,&check_data);
    system_flag_table->ajust_arr = check_data;
    stm_read_eerpom(MENU_SETTING_SCREER_TIMER ,&check_data);
    system_flag_table->screen_timer= check_data;
    stm_read_eerpom(MENU_SETTING_TIMER_ZONE ,&check_data);
    system_flag_table->time_zone = check_data;
    
    time_zone = ((((timer_zone_Aarry[system_flag_table->time_zone][4]-'0')*10)+(timer_zone_Aarry[system_flag_table->time_zone][5]-'0')));
    time_zone =time_zone/60;
    time_zone += ((timer_zone_Aarry[system_flag_table->time_zone][1]-'0')*10)+(timer_zone_Aarry[system_flag_table->time_zone][2]-'0');
    if(timer_zone_Aarry[system_flag_table->time_zone][0] == '+')
    {
        time_zone = time_zone;
    }
    else if(timer_zone_Aarry[system_flag_table->time_zone][0] == '-')
    {
        time_zone = -time_zone;
    }

    v1000_debug("time_zone:%f \r\n",time_zone);
    system_flag_table->index_timerzone = time_zone;
    stm_read_eerpom(MENU_SETTING_LANG ,&check_data);
    system_flag_table->lang= check_data;

    stm_read_eerpom(CHANGE_TMPE_VAULE_INDEX_ADDRES ,&check_data);
    old_div_cnt = check_data;
    stm_read_eerpom(CHANGE_SPEED_VAULE_INDEX_ADDRES ,&check_data);
    old_spped_div_cnt = check_data;
    stm_read_eerpom(CHANGE_ALTITUD_VAULE_INDEX_ADDRES ,&check_data);
    old_altiude_div_cnt = check_data;
    stm_read_eerpom(MENU_GUJI_MODE_INDEX_ADDRES ,&check_data);
    guji_icon_index = check_data;
    stm_read_eerpom(MENU_POWERDOWNTIME_INDEX_ADDRES ,&check_data);


    if(check_data&0x12000000)
    	 system_flag_table->powerdown_timers = check_data&0xffff;
    else
    	system_flag_table->powerdown_timers  = 1;

    stm_read_eerpom(MENU_SETTING_POWERCTROL ,&check_data);
    system_flag_table->usb_powerdown_flash = check_data;
//    stm_read_eerpom(MENU_FLASH_FULL_FLAG ,&check_data);
//    Flash_full_flag = check_data;
    stm_read_eerpom(MENU_SETTING_RECODED_FORMATS ,&check_data);
    system_flag_table->guji_record.recoed_formats = check_data;

    system_flag_table->batt_Status  =  3;
    
}



void Send_to_goto_lowpower(void)
{

    if(system_flag_table->screen_timer == 0)
        system_flag_table->To_lower_timercnt = 16;
    else  if(system_flag_table->screen_timer == 1)
        system_flag_table->To_lower_timercnt = 31;
    else
        system_flag_table->To_lower_timercnt = 0;

    if(system_power_flag == SYSTEM_POWER_STANBY)
        system_flag_table->To_lower_timercnt  = 1;
    v1000_debug("Send_to_goto_lowpower:%d \r\n",system_flag_table->To_lower_timercnt);
}

/*get free size of flash !!!*/
void show_levev_space(void)
{

    FATFS *fs;
    DWORD fre_clust, fre_sect, tot_sect;
    FRESULT res = FR_OK;
    float tp;
	
    /* Get volume information and free clusters of drive 1 */
    res = f_getfree("", &fre_clust, &fs);
    if (res) 
    {
        v1000_debug("f_getfree faild :%d\r\n",res);
        //return ;
    }
    else
    {
        /* Get total sectors and free sectors */
        tot_sect = (fs->n_fatent - 2) * fs->csize;
        fre_sect = fre_clust * fs->csize ;
        
        /* Print the free space (assuming 512 bytes/sector) */
        //v1000_debug("%10lu KiB total drive space.\r\n%10lu KiB available.\r\n",
        //tot_sect / 2, fre_sect / 2);
        tp = fre_sect;
        tp =((tp/tot_sect) *100);
        if(tp < 1)
        {
		    if(tp < 0.1)
                tp = 0.0;
			else
				tp = 1.0;
        }
        system_flag_table->baifenbi = (u8)tp;
//        system_flag_table->fre_clust = fre_clust;
    }

    
    if(tp >= 99.8)
    {
        Movie_Show_Img(101,0,Bmp_100);
    }

    else
    {      
        if(USB_CONNECT == 1)
			sprintf((char *)dtbuf,"--");		    		//�õ��ٶ��ַ���
	    else
            sprintf((char *)dtbuf,"%02d",system_flag_table->baifenbi);		    		//�õ��ٶ��ַ���
        OLED_ShowString(103,0,dtbuf);
        OLED_ShowChar(119, 0, '%', 16, 1);
    }    
}




/*!
*******************************************************************************
** \brief show_baoyu_bmp
*******************************************************************************
*/
void show_baoyu_bmp(void)
{
    di_and_flash_empet(2);
    delay_ms(150);
    di_and_flash_empet(2);
}


u8 get_prev(void)
{
	u8 prevKeyTabIndex  = 0;
    
    key_type = 0xff;
    if(enter_perss == 1)
	{
	    if(keyTabIndex== MENU_TRANSFORMATION)
		{
			prevKeyTabIndex = keyTabIndex+1;
			while(KeyTab[prevKeyTabIndex].ParMenuID ==MENU_TRANSFORMATION)
			{
				prevKeyTabIndex++;
				if(prevKeyTabIndex == MAX_KEYTABSTRUCT_NUM)
					break;
			}

			prevKeyTabIndex --;
		}
        else
        {
		    prevKeyTabIndex =keyTabIndex-1 ;
        }

        if(system_flag_table->run_mode == 0)
        {
            if(keyTabIndex == MENU_TRANSFORMATION)
        	    prevKeyTabIndex = MENU_SATELLITE;

        }
        else
        {
            if(keyTabIndex == MENU_TRANSFORMATION)
        	    prevKeyTabIndex = MENU_SETTING_TEMPERATURRE;
        }

	}
    else
    {

       if(system_flag_table->run_mode == 0)
       {
            if(keyTabIndex == menu_change_index)
                prevKeyTabIndex = MENU_MAIN;
            else if(keyTabIndex == MENU_MAIN)
                prevKeyTabIndex=MENU_SETTING_DISTANCE;
            else if(keyTabIndex == MENU_SETTING_DISTANCE)
                prevKeyTabIndex = menu_change_index;
       	}
    	else
    	{
    		if(keyTabIndex == 0)
    			prevKeyTabIndex = MAX_KEYTABSTRUCT_NUM-1;
    		else
    		{
                if(keyTabIndex == MENU_RIDE_TABLE)
                {
                    prevKeyTabIndex = menu_change_index ;
                }
                else if(keyTabIndex == menu_change_index)
                {
                    prevKeyTabIndex = MENU_REALTIME ;
                }
                else
                    prevKeyTabIndex= keyTabIndex - 1;
    		}

    		while(KeyTab[prevKeyTabIndex].ParMenuID > KeyTab[keyTabIndex].ParMenuID)
    		{
    			prevKeyTabIndex--;
    			if(prevKeyTabIndex == 0)
    				break;
    		}

    		if(prevKeyTabIndex == MENU_TRANSFORMATION)
    			prevKeyTabIndex = menu_change_index;
    	}
    }

	keyTabIndex = prevKeyTabIndex ;

	return 1;

}



u8 get_next(void)
{
    u8 nextKeyTabIndex = 0;

    key_type = 0xff;
    nextKeyTabIndex = keyTabIndex+1;
    if(enter_perss == 1)
    {
        if(KeyTab[nextKeyTabIndex].ParMenuID!= MENU_TRANSFORMATION)
        {

            while(KeyTab[nextKeyTabIndex].MenuID!=MENU_TRANSFORMATION)
            {
                nextKeyTabIndex--;
                if(nextKeyTabIndex == 0)
                break;
            }
        }
        if(system_flag_table->run_mode == 0)
        {
            if(keyTabIndex == MENU_SATELLITE)
                nextKeyTabIndex = MENU_TRANSFORMATION;
        }
        else
        {
            if(keyTabIndex == MENU_SETTING_TEMPERATURRE)
                nextKeyTabIndex = MENU_TRANSFORMATION;
        }

    }
    else
    {

        if(system_flag_table->run_mode == 0)
        {
            if(keyTabIndex == menu_change_index)
                nextKeyTabIndex = MENU_SETTING_DISTANCE;
            else if(keyTabIndex == MENU_SETTING_DISTANCE)
                nextKeyTabIndex = MENU_MAIN;
            else if(keyTabIndex == MENU_MAIN)
                nextKeyTabIndex = menu_change_index;
        }
        else
        {

            if(nextKeyTabIndex>=MAX_KEYTABSTRUCT_NUM)
                nextKeyTabIndex = 0;
            if(keyTabIndex == menu_change_index)
                nextKeyTabIndex = MENU_RIDE_TABLE;

            while((KeyTab[nextKeyTabIndex].ParMenuID > KeyTab[keyTabIndex].ParMenuID))
            {
                nextKeyTabIndex++;
                if(nextKeyTabIndex == MAX_KEYTABSTRUCT_NUM)
                    break;
            }
            if(nextKeyTabIndex == MENU_TRANSFORMATION)
                nextKeyTabIndex = menu_change_index;
        }


    }

    //KeyTab[nextKeyTabIndex].CurrentOperate();
    keyTabIndex = nextKeyTabIndex ;

    return 0;
}




u8 anayes_angle(float angle)
{

    u8 temp_angle = 0;
    if((0<=angle)&&(angle<=22.5))
    {
        if(angle<=11.25)
            temp_angle = 1;	 // zhengbei
        else
            temp_angle = 2;	 // zhengbei
    }
    else if((22.5<angle)&&(angle<=67.5))
    {
        if(angle<=33.75)
            temp_angle = 2;	 // dongbei
        else if(angle<=56.25)
            temp_angle = 3;	 // dongbei
        else
            temp_angle = 4;	 // dongbei
    }
    else if((67.5<angle)&&(angle<=112.5))
    {
        if(angle<=78.75)
            temp_angle = 4;	 // dongbei
        else if(angle<=101.25)
            temp_angle = 5;	 // dongbei
        else
            temp_angle = 6;	 // dongbei
    }
    else if((112.5<angle)&&(angle<=157.5))
    {
        if(angle<=123.75)
            temp_angle = 6;	 // dongbei
        else if(angle<=146.25)
            temp_angle = 7;	 // dongbei
        else
            temp_angle = 8;	 // dongbei
    }
    else if((157.5<angle)&&(angle<=202.5))
    {
        if(angle<=168.75)
            temp_angle = 8;	 // dongbei
        else if(angle<=191.25)
            temp_angle = 9;	 // dongbei
        else
            temp_angle = 10;	 // dongbei

    }

    else if((202.5<angle)&&(angle<=247.5))
    {
        if(angle<=213.75)
            temp_angle = 10;	 // dongbei
        else if(angle<=236.25)
            temp_angle = 11;	 // dongbei
        else
            temp_angle = 12;	 // dongbei

    }

    else if((247.5<angle)&&(angle<=292.5))
    {
        if(angle<=258.75)
            temp_angle = 12;	 // dongbei
        else if(angle<=281.25)
            temp_angle = 13;	 // dongbei
        else
            temp_angle = 14;	 // dongbei
    //		  temp_angle = 5;	 // dongbei
    }
    else if((292.5<angle)&&(angle<=337.5))
    {
        if(angle<=303.75)
            temp_angle = 14;	 // dongbei
        else if(angle<=326.25)
            temp_angle = 15;	 // dongbei
        else
            temp_angle = 16;	 // dongbei
    //		  temp_angle = 2;	 // dongbei
    }

    else if((337.5<angle)&&(angle<=360))
    {
        if(angle<=348.75)
            temp_angle = 16;	 // dongbei
        else
            temp_angle = 1;	 // dongbei
        //  temp_angle = 3;	 // zhengbei
    }
    else
        temp_angle  = 0xff;

return temp_angle;
}


const u8 input_jinweidu_post[18]={24,32,40,48,64,72,88,96,112,24,32,40,48,64,72,88,96,112};

void convert_show_en(float tp,u8 flag,u8 y,u8 mode)
{

    u8 du,fen,index;
    float miao;


    if(system_flag_table->myunit.COORD == 0)
  	{
      	miao = tp/1000000;
	    if(mode == 1 )
		{
    		if(miao<100)
    			sprintf((char *)dtbuf,"%1c  %.6f",flag,miao);	//�õ������ַ���
    		else
    			sprintf((char *)dtbuf,"%1c %.6f",flag,miao);	//�õ������ַ���
		}
	    else
		{
    		if(miao<100)
    			sprintf((char *)dtbuf," %1c %.6f",flag,miao);	//�õ������ַ���
    		else
    			sprintf((char *)dtbuf," %1c%.6f",flag,miao);	//�õ������ַ���

		}

		if(miao == 0)
			index = 104;
		else
			index = 112;
	    OLED_ShowString(16,y,dtbuf);
	    Movie_Show_Img(index,y,bmpdu);
  	}
    else if(system_flag_table->myunit.COORD == 1)
    {
        du = (u8)(tp/1000000);
        miao = (tp/1000000 - du)*60 ;
        if(mode == 1 )
        {
            if(du >100)
            {
                sprintf((char *)dtbuf,"%1c %03d",flag,du);	//�õ������ַ���
            }
            else
            {
                sprintf((char *)dtbuf,"%1c  %02d",flag,du);	//�õ������ַ���

            }
         }
         else
         {
             if(du >100)
             {
                sprintf((char *)dtbuf," %1c%03d",flag,du);	//�õ������ַ���
             }
             else
             {
                sprintf((char *)dtbuf," %1c %02d",flag,du);	//�õ������ַ���
     
             }

         }
         index = 56;
         OLED_ShowString(16,y,dtbuf);
         index +=0;
         Movie_Show_Img(index,y,bmpdu);
     
         if(miao<10)
            sprintf((char *)dtbuf,"0%.3f",miao);	//�õ������ַ���
         else
            sprintf((char *)dtbuf,"%.3f",miao);	//�õ������ַ���
         index += 8;
         OLED_ShowString(index,y,dtbuf);
         index =112;
         Movie_Show_Img(index,y,bmpfen);

    }

	else if(system_flag_table->myunit.COORD == 2)
    {
        du = (u8)(tp/1000000);
        fen = (u8)((tp/1000000 - du)*60);
        miao = ((tp/1000000 - du)*60 -fen)*60;

        if(miao >59.0)
        {
            miao = 0.0;
            fen++;
            if(fen>59)
                du++;
        }

        if(mode == 1 )
        {
            if(du >100)
            {
            sprintf((char *)dtbuf,"%1c %03d",flag,du);	//�õ������ַ���
            }
            else
            {
            sprintf((char *)dtbuf,"%1c  %02d",flag,du);	//�õ������ַ���

            }
        }
        else
        {
            if(du >100)
            {
                sprintf((char *)dtbuf," %1c%03d",flag,du);	//�õ������ַ���
            }
            else
            {
                sprintf((char *)dtbuf," %1c %02d",flag,du);	//�õ������ַ���

            }

        }

        index = 56;
        //v1000_debug("\r\n du: %d  %d %d \n\n",du,fen,miao);
        OLED_ShowString(16,y,dtbuf);
        index +=0;
        Movie_Show_Img(index,y,bmpdu);
        index += 8;
        sprintf((char *)dtbuf,"%02d",fen);	//�õ������ַ���
        OLED_ShowString(index,y,dtbuf);
        index +=16;
        Movie_Show_Img(index,y,bmpfen);
        index += 8;
        if(miao<10.0)
            sprintf((char *)dtbuf,"0%.1f",miao);	//�õ������ַ���
        else
            sprintf((char *)dtbuf,"%.1f",miao);	//�õ������ַ���
        OLED_ShowString(index,y,dtbuf);
        index =120;
        Movie_Show_Img(index,y,bmpmaio);
    }
}

void convert_show_en_input(u8 *prt_c ,u8 y)
{

	u8 index;

	sprintf((char *)dtbuf,"%1c%c%c%c",prt_c[0],prt_c[1],prt_c[2],prt_c[3]);	//�õ������ַ���
	index = 56;

	//v1000_debug("\r\n du: %d  %d %d \n\n",du,fen,miao);
 	OLED_ShowString(24,y,dtbuf);
	index +=0;
	Movie_Show_Img(index,y,bmpdu);
	index += 8;
	sprintf((char *)dtbuf,"%c%c",prt_c[4],prt_c[5]);	//�õ������ַ���
	OLED_ShowString(index,y,dtbuf);
	index +=16;
	Movie_Show_Img(index,y,bmpfen);
	index += 8;
	sprintf((char *)dtbuf,"%c%c.%c",prt_c[6],prt_c[7],prt_c[8]);	//�õ������ַ���
	OLED_ShowString(index,y,dtbuf);
	index =120;
	Movie_Show_Img(index,y,bmpmaio);
}
void menu_real_crood_display(void)
{

	float tp;
	extern u8 NS_LG;
    
	OLED_Fill(0, 0, 127, 63, 0);

#ifdef NEED_TEST_NSEW
	if(key_type == KEY_UP)
	{
        if(NS_LG  == 4)
            NS_LG = 0;
        else
            NS_LG++;
	}
	else if (key_type == KEY_DOWN)
	{
        if(NS_LG  == 0)
            NS_LG = 4;
        else
            NS_LG--;

	}
#endif
    menu_status_dispplay();
    if(system_flag_table->lang == 1)
        OLED_ShowString(0,48,"Accuracy:");
    else
    {
        Movie_Show_Img(0,48,zhunquedu);
    }
    tp =gpsx->pdop;
    tp =tp/100;
    sprintf((char *)dtbuf,"%0.1f",tp);	//��ʾUTC����
    OLED_ShowString(72, 48, dtbuf);

    if(gpsx->fixmode == 3)
        tp = gpsx->longitude;

    convert_show_en(tp,gpsx->ewhemi,32,1);
    if(gpsx->fixmode == 3)
        tp = gpsx->latitude;

    convert_show_en(tp,gpsx->nshemi,16,1);
    OLED_Refresh_Gram();

}

void check_time(void)
{
    u16 gpsx_utc;
    u16 hour_timer,min_timer;
    //u16 hour_timer_rtc,min_timer_rtc;
    s16 diff_tone = 0.0;
    u32 setting_tp = 0;


	v1000_debug("check_time\r\n");

    if((system_flag_table->run_mode == 1)&&(gpsx->svnum >= 2))
    {
        if((gpsx->utc.year < 2014)||(gpsx->utc.year == 2080))
        {
            rtc_set_flag = 1;
            return;
        }
        my_timer.w_year = gpsx->utc.year -2000;
        my_timer.w_month =gpsx->utc.month;
        my_timer.w_date = gpsx->utc.date;
        my_timer.week = RTC_Get_Week(gpsx->utc.year ,my_timer.w_month,my_timer.w_date);
        gpsx_utc = (gpsx->utc.hour*60+gpsx->utc.min);
        my_timer.sec = gpsx->utc.sec;

	    if(timer_zone_Aarry[system_flag_table->time_zone][0] == '+')
		{
    		hour_timer = ((timer_zone_Aarry[system_flag_table->time_zone][1]-'0')*10)+(timer_zone_Aarry[system_flag_table->time_zone][2]-'0');
    		min_timer = ((timer_zone_Aarry[system_flag_table->time_zone][4]-'0')*10)+(timer_zone_Aarry[system_flag_table->time_zone][5]-'0');
    		v1000_debug("\n\r hour_timer : %d   min_timer :%d  ",hour_timer,min_timer);
    		if((gpsx_utc +(hour_timer*60+min_timer))>=(24*60))
    	    {
                RTC_Get(1,&my_timer);
                gpsx_utc  = (gpsx_utc + (hour_timer*60 + min_timer) - (24*60));
    
                hour_timer = gpsx_utc/60;
                min_timer  = gpsx_utc - hour_timer*60;
    		}
    		else
    		{
                gpsx_utc  = (gpsx_utc + (hour_timer*60 + min_timer));
    
                hour_timer = gpsx_utc /60;
                min_timer  = gpsx_utc - hour_timer*60;
    
    		}
    	//	v1000_debug("\n\r hour_timer : %d   min_timer :%d  ",hour_timer,min_timer);
		}
	    else if(timer_zone_Aarry[system_flag_table->time_zone][0] == '-')
		{
    		hour_timer = ((timer_zone_Aarry[system_flag_table->time_zone][1]-'0')*10)+(timer_zone_Aarry[system_flag_table->time_zone][2]-'0');
    		min_timer = ((timer_zone_Aarry[system_flag_table->time_zone][4]-'0')*10)+(timer_zone_Aarry[system_flag_table->time_zone][5]-'0');
			v1000_debug("\n\r hour_timer : %d   min_timer :%d  ",hour_timer,min_timer);
    		if(gpsx_utc > (hour_timer*60+min_timer))
    		{
                gpsx_utc  = (gpsx_utc - (hour_timer*60+min_timer));
            
                hour_timer = gpsx_utc/60;
                min_timer  = gpsx_utc - hour_timer*60;
    		}
    		else
    		{
                gpsx_utc  = (gpsx_utc+(24*60) - (hour_timer*60+min_timer));
    
                hour_timer = gpsx_utc/60;
                min_timer  = gpsx_utc - hour_timer*60;
                RTC_Get(0,&my_timer);
    		}
		 }

	 }
	 else
     {
         my_timer.w_year = RTC_DateStructure.RTC_Year;
         my_timer.w_month =RTC_DateStructure.RTC_Month;
         my_timer.w_date = RTC_DateStructure.RTC_Date;
         my_timer.week = RTC_DateStructure.RTC_WeekDay;
         gpsx_utc = (RTC_TimeStructure.RTC_Hours *60+RTC_TimeStructure.RTC_Minutes);
         my_timer.sec = RTC_TimeStructure.RTC_Seconds ;
         diff_tone = gpsx_utc;
		 v1000_debug("time_zone:%.1f \r\n",time_zone);
		 v1000_debug("index_timerzone:%.1f \r\n",system_flag_table->index_timerzone);
         diff_tone +=(s16)((time_zone - system_flag_table->index_timerzone)*60);
 
         if(diff_tone >= (24*60))
         {
             RTC_Get(1,&my_timer);
             diff_tone = diff_tone - (24*60);
         }
         if(diff_tone < 0)
         {
             RTC_Get(0,&my_timer);
             diff_tone = diff_tone + (24*60);
         }
         hour_timer = diff_tone/60;
         min_timer  = diff_tone - hour_timer*60;
     }

	rtc_set(my_timer.w_year,my_timer.w_month,my_timer.w_date,hour_timer,min_timer,my_timer.sec,my_timer.week);

	stm_read_eerpom(MENU_FRIST_POWER_INDEX_ADDRES ,&setting_tp);
    if((system_flag_table->run_mode == 1)&&(gpsx->svnum >= 2))
    {
        if(setting_tp !=0x12345678)
        {
            stm_write_eerpom(MENU_FRIST_POWER_INDEX_ADDRES ,0x12345678);
            setting_tp = (my_timer.w_year<<16)|(my_timer.w_month<<8)|my_timer.w_date;
            stm_write_eerpom(MENU_FRIST_POWERDATE_INDEX_ADDRES ,setting_tp);
        }
    }

	system_flag_table->index_timerzone = time_zone;
}

void menu_show_gujIcon_Status(u8 icon)
{
    switch(icon)
	{
	    case 0:
		    Movie_Show_Img(85,0,Bmp_gujiiconsetp);
		    break;
	    case 1:
		    Movie_Show_Img(85,0,Bmp_gujiiconrun);
		    break;
	    case 2:
		    Movie_Show_Img(85,0,Bmp_gujiiconB);
		    break;
	    case 3:
	        Movie_Show_Img(85,0,Bmp_gujiiconcar);
		    break;
	    case 4:
		    Movie_Show_Img(85,0,Bmp_gujiiconfly);
		    break;
	    case 5:
		    Movie_Show_Img(85,0,Bmp_gujiiconCustom);
		    break;

	}
}



void menu_status_dispplay(void)
{

    extern u8 usb_delay_timer_poweron;

    float tp;
    u16 gpsx_utc;
    extern   int ddrunmv;

    gpsx_utc = (RTC_TimeStructure.RTC_Hours*60+RTC_TimeStructure.RTC_Minutes);


    if((system_power_flag == SYSTEM_POWER_LOW)&&(system_flag_table->run_mode == 0))
	{
    	OLED_Fill(0, 19, 22, 44, 0);
	    if(system_flag_table->myunit.TIMER == 0)
 		{
 		    if(gpsx_utc>=12*60)
 			{
			    if(gpsx_utc>=13*60)
		 		    gpsx_utc= gpsx_utc - 12*60;

			    if(gpsx_utc/60>=10)
			 	{
				    Movie_Show_Img(1,19,Bmp_bigpm_1);
			 	    sprintf((char *)dtbuf,"%d:%02d:%02d",gpsx_utc/60-10,gpsx_utc%60,RTC_TimeStructure.RTC_Seconds);	//��ʾUTC����
			 	}
			    else
			 	{

			 	    Movie_Show_Img(1,19,Bmp_bigpm);
		 		    sprintf((char *)dtbuf,"%d:%02d:%02d",gpsx_utc/60,gpsx_utc%60,RTC_TimeStructure.RTC_Seconds);	//��ʾUTC����

			 	}
			    OLED_ShowString_24(16,19,dtbuf);
		 	}
		    else
            {
                if(gpsx_utc/60>=10)
                {
                    sprintf((char *)dtbuf,"%d:%02d:%02d",gpsx_utc/60-10,gpsx_utc%60,RTC_TimeStructure.RTC_Seconds);	//��ʾUTC����
                    Movie_Show_Img(1,19,Bmp_bigam_1);
                }
                else if(gpsx_utc < 60)
                {
                    sprintf((char *)dtbuf,"%d:%02d:%02d",2,gpsx_utc%60,RTC_TimeStructure.RTC_Seconds);	//��ʾUTC����
                    Movie_Show_Img(1,19,Bmp_bigam_1);
                }
                else
                {
                    sprintf((char *)dtbuf,"%d:%02d:%02d",gpsx_utc/60,gpsx_utc%60,RTC_TimeStructure.RTC_Seconds);	//��ʾUTC����
                    Movie_Show_Img(1,19,Bmp_bigam);
                }
                OLED_ShowString_24(16,19,dtbuf);
            }
	 	}
		else
		{
            sprintf((char *)dtbuf,"%02d:%02d:%02d",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds);	//��ʾUTC����
            OLED_ShowString_24(0,19,dtbuf);			
		}

	}
    else if((system_power_flag == SYSTEM_POWER_STANBY)||(system_power_flag == SYSTEM_POWER_LOW_2))
	{
	    if((USB_CONNECT == 1)&&(usb_delay_timer_poweron == 0))
	 	{
			if(system_flag_table->charger_connected == 1)
			{
				Movie_Show_Img(43,1,Bmp_chageing);
			    if(system_flag_table->batt_Status == 5)
				    Movie_Show_Img(43,1,Bmp_chage_ok);
			}
			else
			{
			    if(system_flag_table->batt_Status == 1)
				    Movie_Show_Img(43,1,Bmp_batt_1);
			    else if(system_flag_table->batt_Status == 2)
				    Movie_Show_Img(43,1,Bmp_batt_2);
			    else if(system_flag_table->batt_Status == 0)
				    Movie_Show_Img(43,1,Bmp_batt_0);
			    else if(system_flag_table->batt_Status == 3)
				    Movie_Show_Img(43,1,Bmp_batt_3);
		    	else
				    Movie_Show_Img(43,1,Bmp_batt_3);
			}
	 	}
	    else
	 	{
			OLED_Fill(0, 0, 127, 16, 0);

	 	}
#ifdef baoyu

	    if(baoyun_flag == 1)
		{
			 Movie_Show_Img(61,0,Bmp_wanner);
		}
#endif        
	}
    else
	{
	    OLED_Fill(0, 0, 127, 16, 0);
		{
		    if(system_flag_table->myunit.TIMER == 0)
	 		{
	 		    if(gpsx_utc>=12*60)
	 			{
	 			    Movie_Show_Img(34,0,Bm_pm);
				    if(gpsx_utc>=13*60)
			 	    	gpsx_utc= gpsx_utc - 12*60;
		 		    sprintf((char *)dtbuf,"%02d:%02d",gpsx_utc/60,gpsx_utc%60);	//��ʾUTC����
				     OLED_ShowString_head(0,0,dtbuf);

			 	}
			    else
			 	{
				    Movie_Show_Img(34,0,Bmp_am);
			        if(gpsx_utc>60)
			 	    	sprintf((char *)dtbuf,"%02d:%02d",gpsx_utc/60,gpsx_utc%60);	//��ʾUTC����
			 	    else
					    sprintf((char *)dtbuf,"%02d:%02d",12,gpsx_utc%60);	//��ʾUTC����
				     OLED_ShowString_head(0,0,dtbuf);

			 	}
		 	}
		    else
			{
			    sprintf((char *)dtbuf,"%02d:%02d",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes);	//��ʾUTC����
			    OLED_ShowString_head(0,0,dtbuf);

			}
		}

		if(system_flag_table->charger_connected == 1)
		{
			Movie_Show_Img(43,1,Bmp_chageing);
		    if(system_flag_table->batt_Status == 5)
			    Movie_Show_Img(43,1,Bmp_chage_ok);
		}
		else
		{
		   if(system_flag_table->batt_Status == 1)	//3// 3
		       Movie_Show_Img(43,1,Bmp_batt_1);
		   else if(system_flag_table->batt_Status == 2)	 // 4 -5
			   Movie_Show_Img(43,1,Bmp_batt_2);
		   else if(system_flag_table->batt_Status == 0)
			   Movie_Show_Img(43,1,Bmp_batt_0);
		   else if(system_flag_table->batt_Status == 3)	//
			   Movie_Show_Img(43,1,Bmp_batt_3);
		   else
			   Movie_Show_Img(43,1,Bmp_batt_3);
		}

    	tp =gpsx->posslnum;

#ifdef baoyu

		if(baoyun_flag == 1)
		{
			 Movie_Show_Img(61,0,Bmp_wanner);
		}
		else
#endif           
		{
            if(system_flag_table->run_mode == 1)
            {
                if(gpsx->fixmode >= 2)
                {
                    if(tp >=7)
                    {
                        Movie_Show_Img(61,0,Bmp_singe_3);
                    }
                    else if(tp >=4)
                        Movie_Show_Img(61,0,Bmp_singe_2);
                    else if(tp >=3)
                        Movie_Show_Img(61,0,Bmp_singe_1);
                }
                else
                    Movie_Show_Img(61,0,Bmp_singe_no);
            }
		}


	   if(guji_mode == RECORED_START_DOING)
	   	{
		    menu_show_gujIcon_Status(guji_icon_index);
	   	}

	   show_levev_space();

	}

}

void menu_main_display(void)
{

    s16 gpsx_utc;
    s32 feet = 0;
    float temperature_F;
    u16 year,month,data ,week,hour,min,sec;

    OLED_Fill(0, 0, 127, 63, 0);
	menu_status_dispplay();
    
    year = RTC_DateStructure.RTC_Year;
    month = RTC_DateStructure.RTC_Month;
    data = RTC_DateStructure.RTC_Date;
    week = RTC_DateStructure.RTC_WeekDay;
    hour = RTC_TimeStructure.RTC_Hours;
    min =  RTC_TimeStructure.RTC_Minutes;
    sec =  RTC_TimeStructure.RTC_Seconds;

	//  v1000_debug("\r\n%d-%d-%d  %d  %d-%d-%d\r\n",year,month,data,week,hour,min,sec);

	switch(system_flag_table->myunit.DATA)
    {
		case 0:
		 	sprintf((char *)dtbuf,"%02d-%02d-%02d",year,month,data);	//��ʾUTC����
		 	break;
		case 1:
		 	sprintf((char *)dtbuf,"%02d-%02d-%02d",month,data,year);	//��ʾUTC����
		 	break;
		case 2:
		 	sprintf((char *)dtbuf,"%02d-%02d-%02d",data,month,year);	//��ʾUTC����
		 	break;

	}
	//v1000_debug("year2:%d\r\n",gpsx->utc.year);
	OLED_ShowString(0,48,dtbuf);

	gpsx_utc = (hour*60+min);
	if(system_flag_table->myunit.TIMER == 1)
	{
		sprintf((char *)dtbuf,"%02d:%02d:%02d",hour,min,sec);	//��ʾUTCʱ��
	  	OLED_ShowString(0,32,dtbuf);
	}
	else
	{
 	    if(gpsx_utc>=12*60)
 		{
 		    if(gpsx_utc>=13*60)
	 		    gpsx_utc= gpsx_utc - 12*60;
 	 	    sprintf((char *)dtbuf,"%02d:%02d:%02d PM  ",gpsx_utc/60,gpsx_utc%60,sec);
		    OLED_ShowString(0,32,dtbuf);

	 	}
	    else
	 	{
	 	    if(gpsx_utc<60)
			    sprintf((char *)dtbuf,"%02d:%02d:%02d AM  ",12,gpsx_utc%60,sec);
		    else
			    sprintf((char *)dtbuf,"%02d:%02d:%02d AM  ",gpsx_utc/60,gpsx_utc%60,sec);
		    OLED_ShowString(0,32,dtbuf);
	 	}

	}


//	rtc_timer = (RTC_TimeStructure.RTC_Hours*60+RTC_TimeStructure.RTC_Minutes);
#if 0
	if(gpsx_utc>rtc_timer)
		{
		comp_time = (gpsx_utc-rtc_timer)/60;
//		time_zone = comp_time;
		sprintf((char *)dtbuf,"+%02d:%02d",comp_time,(gpsx_utc-rtc_timer-(comp_time*60)));	//��ʾUTCʱ��
		}
	else

		{
		comp_time = (rtc_timer-gpsx_utc)/60;
//		time_zone = comp_time;
		sprintf((char *)dtbuf,"+%02d:%02d",comp_time,(rtc_timer-gpsx_utc-(comp_time*60)));	//��ʾUTCʱ��
		}
#endif

    if(week >= 7)
    {
        week = RTC_Get_Week(year ,month,data);
    }
    
    sprintf((char *)dtbuf,"%c%c%c",week_word[week][0],week_word[week][1],week_word[week][2]);
	OLED_ShowString(88,48,dtbuf);
	temperature_F = temperature;

	if(system_flag_table->myunit.TEMP == 0)
	{

        sprintf((char *)dtbuf,"%.1f C",temperature_F/10);	//�õ������ַ���

	}
	else if(system_flag_table->myunit.TEMP == 1)
	{
        temperature_F = 320 + (1.8*temperature);
        sprintf((char *)dtbuf,"%.1f F",temperature_F/10);	//�õ������ַ���

	}

	OLED_ShowString((80 - what_is_numbe_10(temperature_F/10)*8),16,dtbuf);
	Movie_Show_Img(96,16,bmpdu);

    if(system_flag_table->myunit.H_W == 0)
    {
	    sprintf((char *)&dtbuf,"%dm",Altitude/10);	//�õ������ַ���
    }
    else  if(system_flag_table->myunit.H_W >= 1)
    {

		feet = (floor)(Altitude/10/0.3048);

		sprintf((char *)&dtbuf,"%dft",feet);	//�õ������ַ���
    }

	OLED_ShowString(0,16,dtbuf);


}


void menu_real_speed_display(void)
{

    float tp;


    s32 tmp_Att = 0;

	OLED_Fill(0, 0, 127, 63, 0);
    menu_status_dispplay();

    // v1000_debug("\r fixmode = %d speed = %d ",gpsx->fixmode ,gpsx->speed);
	if((gpsx->fixmode >=2)&&(gpsx->speed > 1000))
		tp=gpsx->speed;
	else
		tp = 0;

	if(system_flag_table->myunit.H_W == 0)//||(system_flag_table->myunit.H_W == 1))
	 	sprintf((char *)dtbuf,"%.1fkm/h",tp/1000);		    		//�õ��ٶ��ַ���

#if 1
	else if(system_flag_table->myunit.H_W == 1)
	 	sprintf((char *)dtbuf,"%.1fmi/h",tp/1609);		    		//�õ��ٶ��ַ���
#endif
	else if(system_flag_table->myunit.H_W == 2)
	 	sprintf((char *)dtbuf,"%.1fkt/h",tp/1852);		    		//�õ��ٶ��ַ���
 	OLED_ShowString(48,16,dtbuf);

    if(system_flag_table->myunit.H_W == 0)
	{
	    sprintf((char *)&dtbuf,"%dm",Altitude/10);	//�õ������ַ���
	}
    else  if(system_flag_table->myunit.H_W >= 1)
    {
        tmp_Att= (floor)(Altitude/10/0.3048);
        sprintf((char *)&dtbuf,"%dft",tmp_Att);	//�õ������ַ���
    }

	OLED_ShowString(48,32,dtbuf);
	if(gpsx->fixmode >=2)
		tp=gpsx->angle;
	else
		tp =0;
	tp /=1000;

	if(tp <10)
		sprintf((char *)dtbuf,"   %d",(u16)tp);		    		//�õ��ٶ��ַ���
	else if(tp <100)
		sprintf((char *)dtbuf,"  %d",(u16)tp);		    		//�õ��ٶ��ַ���
	else
		sprintf((char *)dtbuf," %d",(u16)tp);		    		//�õ��ٶ��ַ���

 	OLED_ShowString(88,48,dtbuf);
	Movie_Show_Img(120,48,bmpdu);

	switch(anayes_angle(tp))
    {
		case 1:  //zhengbei
		//	Movie_Show_Img(0,16,fangxianbmp);
		if(system_flag_table->lang == 1)
			OLED_ShowString(48,48,"N");
		else
			Movie_Show_Img(48,48,bmp_wordBei);
			break;
		case 2: //zhengdong
		//	Movie_Show_Img(0,16,fangxianbmp22);
		if(system_flag_table->lang == 1)
			OLED_ShowString(48,48,"NNE");
		else
			{
			Movie_Show_Img(48,48,bmp_wordBei);
			Movie_Show_Img(64,48,bmp_wordBei);
			Movie_Show_Img(80,48,bmp_worddong);
			}
			break;
		case 3: //zhengnan
		//	Movie_Show_Img(0,16,fangxianbmp45);
		if(system_flag_table->lang == 1)
			OLED_ShowString(48,48,"NE");
		else
			{

			Movie_Show_Img(48,48,bmp_worddong);
			Movie_Show_Img(64,48,bmp_wordBei);
			}

			break;
		case 4:
		//	Movie_Show_Img(0,16,fangxianbmp67);
		if(system_flag_table->lang == 1)
			OLED_ShowString(48,48,"ENE");
		else
			{

			Movie_Show_Img(48,48,bmp_worddong);
			Movie_Show_Img(64,48,bmp_wordBei);
			Movie_Show_Img(80,48,bmp_worddong);
			}
			break;
		case 5:  //zhengbei
		//	Movie_Show_Img(0,16,bmpfangxian90);
		if(system_flag_table->lang == 1)
			OLED_ShowString(48,48,"E");
		else
			{
			Movie_Show_Img(48,48,bmp_worddong);
			}
			break;
		case 6: //zhengdong
		//	Movie_Show_Img(0,16,bmpfangxian112);
		if(system_flag_table->lang == 1)
			OLED_ShowString(48,48,"ESE");
		else
			{

			Movie_Show_Img(48,48,bmp_worddong);
			Movie_Show_Img(64,48,bmp_wordnan);
			Movie_Show_Img(80,48,bmp_worddong);
			}
			break;
		case 7: //zhengnan
		//	Movie_Show_Img(0,16,bmpfangxian135);
		if(system_flag_table->lang == 1)
			OLED_ShowString(48,48,"SE");
		else
			{
			Movie_Show_Img(48,48,bmp_worddong);
			Movie_Show_Img(64,48,bmp_wordnan);
			}

			break;
		case 8:
		//	Movie_Show_Img(0,16,bmpfangxian157);
		if(system_flag_table->lang == 1)
			OLED_ShowString(48,48,"SSE");
		else
			{

			Movie_Show_Img(48,48,bmp_wordnan);
			Movie_Show_Img(64,48,bmp_wordnan);
			Movie_Show_Img(80,48,bmp_worddong);
			}
			break;
		case 9:  //zhengbei
		//	Movie_Show_Img(0,16,bmpfangxian180);
		if(system_flag_table->lang == 1)
			OLED_ShowString(48,48,"S");
		else
			{
			Movie_Show_Img(48,48,bmp_wordnan);
			}
			break;
		case 10: //zhengdong
		//	Movie_Show_Img(0,16,bmpfangxian202);
		if(system_flag_table->lang == 1)
			OLED_ShowString(48,48,"SSW");
		else
			{

			Movie_Show_Img(48,48,bmp_wordnan);
			Movie_Show_Img(64,48,bmp_wordnan);
			Movie_Show_Img(80,48,bmp_wordXi);
			}
			break;
		case 11: //zhengnan
		//	Movie_Show_Img(0,16,bmpfangxian225);
		if(system_flag_table->lang == 1)
			OLED_ShowString(48,48,"SW");
		else
			{
			Movie_Show_Img(48,48,bmp_wordXi);
			Movie_Show_Img(64,48,bmp_wordnan);
			}

			break;
		case 12:
		//	Movie_Show_Img(0,16,bmpfangxian247);
		if(system_flag_table->lang == 1)
			OLED_ShowString(48,48,"WSW");
		else
			{
			Movie_Show_Img(48,48,bmp_wordXi);
			Movie_Show_Img(64,48,bmp_wordnan);
			Movie_Show_Img(80,48,bmp_wordXi);
			}
			break;
		case 13:  //zhengbei
		//	Movie_Show_Img(0,16,bmpfangxian270);
		if(system_flag_table->lang == 1)
			OLED_ShowString(48,48,"W");
		else
			{
			Movie_Show_Img(48,48,bmp_wordXi);
			}
			break;
		case 14: //zhengdong
		//	Movie_Show_Img(0,16,bmpfangxian292);
		if(system_flag_table->lang == 1)
			OLED_ShowString(48,48,"WNW");
		else
			{
			Movie_Show_Img(48,48,bmp_wordXi);
			Movie_Show_Img(64,48,bmp_wordBei);
			Movie_Show_Img(80,48,bmp_wordXi);
			}
			break;
		case 15: //zhengnan
		//	Movie_Show_Img(0,16,bmpfangxian314);
		if(system_flag_table->lang == 1)
			OLED_ShowString(48,48,"NW");
		else
			{

			Movie_Show_Img(48,48,bmp_wordXi);
			Movie_Show_Img(64,48,bmp_wordBei);
			}
			break;
		case 16:
		//	Movie_Show_Img(0,16,bmpfangxian336);
		if(system_flag_table->lang == 1)
			OLED_ShowString(48,48,"NNW");
		else
			{

			Movie_Show_Img(48,48,bmp_wordBei);
			Movie_Show_Img(64,48,bmp_wordBei);
			Movie_Show_Img(80,48,bmp_wordXi);
			}
			break;
		}
	tp = 360 -tp;
	switch(anayes_angle(tp))
		{
		case 1:  //zhengbei
			Movie_Show_Img(0,16,fangxianbmp);
	//		Movie_Show_Img(48,48,bmp_wordBei);
			break;
		case 2: //zhengdong
			Movie_Show_Img(0,16,fangxianbmp22);
			//Movie_Show_Img(48,48,bmp_wordBei);
			//Movie_Show_Img(64,48,bmp_wordBei);
			//Movie_Show_Img(80,48,bmp_worddong);
			break;
		case 3: //zhengnan
			Movie_Show_Img(0,16,fangxianbmp45);
			//Movie_Show_Img(48,48,bmp_worddong);
			//Movie_Show_Img(64,48,bmp_wordBei);
			break;
		case 4:
			Movie_Show_Img(0,16,fangxianbmp67);
			//Movie_Show_Img(48,48,bmp_worddong);
			//Movie_Show_Img(64,48,bmp_wordBei);
			//
			//Movie_Show_Img(80,48,bmp_worddong);
			break;
		case 5:  //zhengbei
			Movie_Show_Img(0,16,bmpfangxian90);
			//Movie_Show_Img(48,48,bmp_worddong);
			break;
		case 6: //zhengdong
			Movie_Show_Img(0,16,bmpfangxian112);
			//Movie_Show_Img(48,48,bmp_worddong);
			//Movie_Show_Img(64,48,bmp_wordnan);
			//Movie_Show_Img(80,48,bmp_worddong);
			break;
		case 7: //zhengnan
			Movie_Show_Img(0,16,bmpfangxian135);
			//Movie_Show_Img(48,48,bmp_worddong);
			//Movie_Show_Img(64,48,bmp_wordnan);
			break;
		case 8:
			Movie_Show_Img(0,16,bmpfangxian157);
			//Movie_Show_Img(48,48,bmp_wordnan);
			//Movie_Show_Img(64,48,bmp_wordnan);
			//Movie_Show_Img(80,48,bmp_worddong);
			break;
		case 9:  //zhengbei
			Movie_Show_Img(0,16,bmpfangxian180);
			//Movie_Show_Img(48,48,bmp_wordnan);
			break;
		case 10: //zhengdong
			Movie_Show_Img(0,16,bmpfangxian202);
			//Movie_Show_Img(48,48,bmp_wordnan);
			//Movie_Show_Img(64,48,bmp_wordnan);
			//Movie_Show_Img(80,48,bmp_wordXi);
			break;
		case 11: //zhengnan
			Movie_Show_Img(0,16,bmpfangxian225);
			//Movie_Show_Img(48,48,bmp_wordXi);
			//Movie_Show_Img(64,48,bmp_wordnan);
			break;
		case 12:
			Movie_Show_Img(0,16,bmpfangxian247);
			//Movie_Show_Img(48,48,bmp_wordXi);
			//Movie_Show_Img(64,48,bmp_wordnan);
			//Movie_Show_Img(80,48,bmp_wordXi);
			break;
		case 13:  //zhengbei
			Movie_Show_Img(0,16,bmpfangxian270);
			//Movie_Show_Img(48,48,bmp_wordXi);
			break;
		case 14: //zhengdong
			Movie_Show_Img(0,16,bmpfangxian292);
			//Movie_Show_Img(48,48,bmp_wordXi);
			//Movie_Show_Img(64,48,bmp_wordBei);
			//Movie_Show_Img(80,48,bmp_wordXi);
			break;
		case 15: //zhengnan
			Movie_Show_Img(0,16,bmpfangxian314);
			//Movie_Show_Img(48,48,bmp_wordXi);
			//Movie_Show_Img(64,48,bmp_wordBei);
			break;
		case 16:
			Movie_Show_Img(0,16,bmpfangxian336);
			//Movie_Show_Img(48,48,bmp_wordBei);
			//Movie_Show_Img(64,48,bmp_wordBei);
			//Movie_Show_Img(80,48,bmp_wordXi);
			break;
	}

//	OLED_Refresh_Gram();

}






void menu_temp_change_display(void)
{
	//extern const u8 Temp_change[];
	u8 i;
	signed char y;
	static signed char old_y1;
	float tp,tp_most,tp_lest,temperature_F;
	int temp = 0;
	OLED_Fill(0, 0, 127, 63, 0);

	if(system_flag_table->lang == 1)
		OLED_ShowString(30,0,"TEMP");
	else
	{
		Movie_Show_Img(30, 0,Bmp_temp_word);
	}

#ifdef baoyu    
    if(baoyun_flag)
		Movie_Show_Img(0,29,Bmp_wanner);
#endif    
    Movie_Show_Img(30, 17,Bmp_change_upline);
	Movie_Show_Img(24, 56,Bmp_24hour_downline);

	if(enter_perss == 1)
    {
		Movie_Show_Img(0,0,Bmp_updwon);
	}
	else       
	{
	    if(keyTabIndex == menu_change_index)
			Movie_Show_Img(0,1,Bmp_ponit);
	}


    if(old_div_cnt == 0)
    {
        most_temperature = temperature;
        lest_temperature = temperature;
    }
    
    temperature_F  = temperature;

	/*
	
	������ģʽ����ʾ�¶�=����������¶� - |����¶�*0.1|
	GPSģʽ����ʾ�¶�=����¶� - |����¶�*0.22|
	*/
	if(system_flag_table->myunit.TEMP == 0)
	{


		sprintf((char *)dtbuf,"%.1f C",temperature_F/10);	//�õ������ַ���
		OLED_ShowString((65),0,dtbuf);
		Movie_Show_Img((80+what_is_numbe_10(temperature_F/10)*8),0,bmpdu);

		sprintf((char *)dtbuf,"%d",most_temperature/10);	//�õ������ַ���
		
		OLED_ShowString12(0,16,dtbuf);

		sprintf((char *)dtbuf,"%d",lest_temperature/10);	//�õ������ַ���
		OLED_ShowString12(0,46,dtbuf);

	}
	else
	{

		temperature_F = 320+(1.8*temperature);

		sprintf((char *)dtbuf,"%.1f F",temperature_F/10);	//�õ������ַ���
		OLED_ShowString((65),0,dtbuf);
		Movie_Show_Img((80+what_is_numbe_10(temperature_F/10)*8),0,bmpdu);

		temp = (int)(320+(1.8*most_temperature));
		sprintf((char *)dtbuf,"%d",temp/10);	//�õ������ַ���
		OLED_ShowString12(0,16,dtbuf);
		temp = (int)(320+(1.8*lest_temperature));
		sprintf((char *)dtbuf,"%d",temp/10);	//�õ������ַ���
		OLED_ShowString12(0,46,dtbuf);
	}


    i = 95;


    while(i != (95-old_div_cnt)  && (old_div_cnt != 0xff))
	{

        tp_most = most_temperature;
        tp_lest =  lest_temperature;
        tp =temperature_pass[i] ;

        tp = (tp- ((tp_most+tp_lest)/2))*40/(tp_most-tp_lest);

        y = (signed char)(tp);
        if(y==20)
        y =19;


        if(35-y >56)
        y = -21;
        if(35-y <17)
        y = 18;

        OLED_DrawPoint(i+30,35-y,1);
#if 1
        if((i!=95)&&(old_div_cnt>0))
        {
            if(old_y1<=y)
                OLED_DrawVLine(i+31,35-y,35-old_y1);
            else if(old_y1>y)
                OLED_DrawVLine(i+31,35-old_y1,35-y);

        }
#endif
   	    old_y1 = y;
                   i--;
	}


    if(old_div_cnt>=95)
    {
        for(i=0;i<95;i++)
        {
            if(temperature_pass[i] == most_temperature)
                break;
        }
        
        OLED_DrawVLine_visual(i+31,17,56);
    }


//	OLED_Refresh_Gram();
}


void menu_Pressure_display(void)
{
	
	u8 i;
	signed char y;
	static signed char old_y1;
	float tp,tp_most,tp_lest,pressure_unit;

	OLED_Fill(0, 0, 127, 63, 0);

#ifdef baoyu
    if(baoyun_flag)
		Movie_Show_Img(0,29,Bmp_wanner);
#endif
	if(system_flag_table->lang == 1)
		OLED_ShowString(22,0,"PRES");
	else
	{
		Movie_Show_Img(22, 0,Bmp_pree_word);
	}
	Movie_Show_Img(30, 17,Bmp_change_upline);
	Movie_Show_Img(24, 56,Bmp_24hour_downline);
	if(enter_perss == 1)
	{
		Movie_Show_Img(0,0,Bmp_updwon);
	}
	else
	{
		if(keyTabIndex == menu_change_index)
			Movie_Show_Img(0,1,Bmp_ponit);
	}


	conversion(pressure);


    if(old_div_cnt == 0)
    {
        most_pressure = pressure;
        lest_pressure = pressure;
    }
    
	if(system_flag_table->myunit.PREE == 0)
    {

		pressure_unit = pressure;
		sprintf((char *)dtbuf,"%.1fhpa",pressure_unit/100);	//�õ������ַ���
		OLED_ShowString(57,0,dtbuf);
	
		sprintf((char *)dtbuf,"%d",(int)(most_pressure/100));	//�õ������ַ���
		OLED_ShowString12(0,16,dtbuf);
	
		sprintf((char *)dtbuf,"%d",(int)(lest_pressure/100));	//�õ������ַ���
		OLED_ShowString12(0,46,dtbuf);
	}
	else if(system_flag_table->myunit.PREE == 1)
	{

	    pressure_unit = pressure;
		sprintf((char *)dtbuf,"%.1fmba",pressure_unit/100);	//�õ������ַ���
		OLED_ShowString(57,0,dtbuf);
	
		sprintf((char *)dtbuf,"%d",(int)(most_pressure/100));	//�õ������ַ����
		OLED_ShowString12(0,16,dtbuf);

		sprintf((char *)dtbuf,"%d",(int)(lest_pressure/100));	//�õ������ַ���
		OLED_ShowString12(0,46,dtbuf);
	}
	else if(system_flag_table->myunit.PREE == 2)
	{
		pressure_unit = pressure/1.333;
		
		sprintf((char *)dtbuf,"%.1fmmhg",pressure_unit/100);	//�õ������ַ���
		OLED_ShowString(57,0,dtbuf);
		pressure_unit = most_pressure/1.333;
		
		sprintf((char *)dtbuf,"%d",(int)(pressure_unit/100));	//�õ������ַ���
		OLED_ShowString12(0,16,dtbuf);
		pressure_unit = lest_pressure/1.333;
		conversion(pressure_unit);
		sprintf((char *)dtbuf,"%d",(int)(pressure_unit/100));	//�õ������ַ���
		OLED_ShowString12(0,46,dtbuf);
	}
	else if(system_flag_table->myunit.PREE == 3)
	{
		pressure_unit = pressure/33.864;
		sprintf((char *)dtbuf,"%.2finhg",pressure_unit/100);	//�õ������ַ���
		OLED_ShowString(57,0,dtbuf);
		pressure_unit = most_pressure/33.864;
		sprintf((char *)dtbuf,"%d",(int)(pressure_unit/100));	//�õ������ַ���
		OLED_ShowString12(0,16,dtbuf);
		pressure_unit = lest_pressure/33.864;
		sprintf((char *)dtbuf,"%d",(int)(pressure_unit/100));	//�õ������ַ���
		OLED_ShowString12(0,46,dtbuf);
	}

	if(old_div_cnt == 0xff)
		return;

    i = 95;

	while(i != (95-old_div_cnt))
	{
        tp_most = most_pressure;
        tp_lest =  lest_pressure;
        tp = pressure_pass[i];
        tp = (tp - ((tp_most+tp_lest)/2))*40/(tp_most-tp_lest);

        y = (signed char)(tp);
        // 	y = (signed char)((pressure_pass[i] -(most_pressure + lest_pressure)/2)*15/(most_pressure-lest_pressure));
        if(y==20)
            y =19;

        //	v1000_debug("\r pressure_pass[%d]= %d i= %d y = %d %.1f",i,pressure_pass[i],i,y,tp);
        if(35-y >56)
            y = -21;
        
        if(35-y <17)
            y = 18;
        
		OLED_DrawPoint(i+30,35-y,1);

		if((i!=95)&&(old_div_cnt>0))
		{
			if(old_y1<=y)
				OLED_DrawVLine(i+31,35-y,35-old_y1);
			else if(old_y1>y)
				OLED_DrawVLine(i+31,35-old_y1,35-y);

		}


		old_y1 = y;
        i--;
	}

    if(old_div_cnt>=95)
    {
        for(i=0;i<95;i++)
        {
            if(pressure_pass[i] == most_pressure)
            break;
        }
        OLED_DrawVLine_visual(i+31,17,56);
    }
//	OLED_Refresh_Gram();
}

void menu_arruite_change_display(void)
{
//	const unsigned char Bmp_pree_word[];
	u8 i=0;
	float tp,tp_most,tp_lest;
	signed char y;
	static signed char old_y1;
	OLED_Fill(0, 0, 127, 63, 0);

	if(system_flag_table->lang == 1)
		OLED_ShowString(30,0,"ALTI");
	else
	{
		Movie_Show_Img(30, 0,Bmp_attide_word);
	}

#ifdef baoyu
	if(baoyun_flag)
		Movie_Show_Img(0,29,Bmp_wanner);
#endif

	Movie_Show_Img(30, 17,Bmp_change_upline);
	Movie_Show_Img(27, 56,Bmp_4hour_downline);
	if(enter_perss == 1)
	{
			Movie_Show_Img(0,0,Bmp_updwon);
	}
	else
	{
		if(keyTabIndex == menu_change_index)
			Movie_Show_Img(0,1,Bmp_ponit);
	}


	if(old_altiude_div_cnt == 0)
	{
		most_Altitude = Altitude;
		lest_Altitude = Altitude;
	}

    if(system_flag_table->myunit.H_W == 0)
    {
		sprintf((char *)&dtbuf,"%ldm",Altitude/10);	//�õ������ַ���
		OLED_ShowString(65,0,dtbuf);

		sprintf((char *)dtbuf,"%ld",most_Altitude/10);		    		//�õ��ٶ��ַ���
		OLED_ShowString12(0,16,dtbuf);

		sprintf((char *)dtbuf,"%ld",lest_Altitude/10);		    		//�õ��ٶ��ַ���
		OLED_ShowString12(0,46,dtbuf);
	//	v1000_debug("\r\nlest_Altitude= %ld",lest_Altitude);
     }
     else  if(system_flag_table->myunit.H_W >= 1)
     {
		sprintf((char *)&dtbuf,"%.0fft",(Altitude/3.048));	//�õ������ַ���
		OLED_ShowString(65,0,dtbuf);

		sprintf((char *)dtbuf,"%.0f",(most_Altitude/3.048));		    		//�õ��ٶ��ַ���
		OLED_ShowString12(0,16,dtbuf);

		sprintf((char *)dtbuf,"%.0f",lest_Altitude/3.048);		    		//�õ��ٶ��ַ���
		OLED_ShowString12(0,46,dtbuf);

     }

     i = 95;

	while((i != (95-old_altiude_div_cnt))  && (old_altiude_div_cnt != 0xff))
	{

        tp_most = most_Altitude;
        //tp_most /= 100;
        tp_lest =  lest_Altitude;
        //tp_lest /=100;

        tp = (Altitude_pass[i] - (tp_most+tp_lest)/2)*40/(tp_most-tp_lest);

        y = (signed char)(tp);

        if(y==20)
        y =19;

        //	v1000_debug("\r\n speed_pass[i]= %d i= %d , y = %d",speed_pass[i],i,y);
        if(37-y >56)
        y = -19;
        if(37-y <17)
        y = 20;

		OLED_DrawPoint(i+30,37-y,1);
#if 1
		if((i!=95)&&(old_altiude_div_cnt>0))
		{
			if(old_y1<=y)
				OLED_DrawVLine(i+31,37-y,37-old_y1);
			else if(old_y1>y)
				OLED_DrawVLine(i+31,37-old_y1,37-y);
		}
		old_y1 = y;
#endif

		i --;
	}

    if(old_altiude_div_cnt>=95)
    {
        for(i=0;i<95;i++)
        {
          if(Altitude_pass[i] == most_Altitude)
              break;
        }
        OLED_DrawVLine_visual(i+31,17,56);
    }


}




void menu_speed_change_display(void)
{
	float tp,tp_most,tp_lest;

	u8 i;
	signed char y;

	OLED_Fill(0, 0, 127, 63, 0);

	if(system_flag_table->lang == 1)
		OLED_ShowString(30,0,"SPD");
	else
	{
		Movie_Show_Img(30, 0,Bmp_speed_word);
       
	}
	Movie_Show_Img(30, 17,Bmp_change_upline);
	Movie_Show_Img(27, 56,Bmp_4hour_downline);
	tp = average_speed;

#ifdef baoyu

    if(baoyun_flag)
		Movie_Show_Img(0,29,Bmp_wanner);
#endif

	if(enter_perss == 1)
	{
			Movie_Show_Img(0,0,Bmp_updwon);
	}
	else
	{
		if(keyTabIndex == menu_change_index)
			Movie_Show_Img(0,1,Bmp_ponit);
	}

	if(old_spped_div_cnt == 0)
	{
		most_speed = tp/1000;
		lest_speed = tp/1000;
	}


	if(system_flag_table->myunit.H_W == 0)
	{
		sprintf((char *)dtbuf,"%dkm/h",(u32)tp/1000);		    		//�õ��ٶ��ַ���
		OLED_ShowString(70,0,dtbuf);
		tp=most_speed;
		sprintf((char *)dtbuf,"%.1f",tp/1.000);		    		//�õ��ٶ��ַ���
		OLED_ShowString12(0,16,dtbuf);
		tp=lest_speed;
		sprintf((char *)dtbuf,"%.1f",tp/1.000);		    		//�õ��ٶ��ַ���
		OLED_ShowString12(0,46,dtbuf);
	}
	else if(system_flag_table->myunit.H_W == 1)
	{

		sprintf((char *)dtbuf,"%dmi/h",(u32)tp/1609);		    		//�õ��ٶ��ַ���
		OLED_ShowString(70,0,dtbuf);
		tp=most_speed;
		sprintf((char *)dtbuf,"%.1f",tp/1.609);		    		//�õ��ٶ��ַ���
		OLED_ShowString12(0,16,dtbuf);
		tp=lest_speed;
		sprintf((char *)dtbuf,"%.1f",tp/1.609);		    		//�õ��ٶ��ַ���
		OLED_ShowString12(0,46,dtbuf);

	}
	else if(system_flag_table->myunit.H_W == 2)
	{

		sprintf((char *)dtbuf,"%dkt/h",(u32)tp/1852);		    		//�õ��ٶ��ַ���
		OLED_ShowString(70,0,dtbuf);
		tp=most_speed;
		sprintf((char *)dtbuf,"%.1f",tp/1.852);		    		//�õ��ٶ��ַ���
		OLED_ShowString12(0,16,dtbuf);
		tp=lest_speed;
		sprintf((char *)dtbuf,"%.1f",tp/1.852);		    		//�õ��ٶ��ַ���
		OLED_ShowString12(0,46,dtbuf);

	}

    i = 95;

	while(i != (95-old_spped_div_cnt)  && (old_spped_div_cnt != 0xff))
    {

        tp_lest =  lest_speed;
        tp_most =  most_speed;
        tp = (speed_pass[i] - (tp_most+tp_lest)/2)*40/(tp_most-tp_lest);

        y = (signed char)(tp);

        if(y==20)
           y =19;

        if(35-y >56)
           y = -21;
        if(35-y <17)
           y = 18;

        OLED_DrawPoint(i+30,35-y,1);

        i--;
    }
        
    if(old_spped_div_cnt>=95)
    {
        for(i=0;i<95;i++)
        {
            if(speed_pass[i] == most_speed)
                break;
        }
        OLED_DrawVLine_visual(i+31,17,56);
    }



}



void menu_show_sl_number(u8 number,u8 index)
{

    const u8 *sl_number[]={BMp_0,BMp_1,BMp_2,BMp_3,BMp_4,BMp_5,BMp_6,BMp_7,BMp_8,BMp_9};


    if(number<=9)
	{
    	Movie_Show_Img_3(index*10,55,BMp_0);
    	Movie_Show_Img_3(index*10+4,55,sl_number[number]);
	}
    else
	{

	    if(number>99)
		{
    		Movie_Show_Img_3(index*10,55,sl_number[9]);
    		Movie_Show_Img_3(index*10+4,55,sl_number[9]);
		}
	    else
		{
    		Movie_Show_Img_3(index*10,55,sl_number[number/10]);
    		Movie_Show_Img_3((index*10+4),55,sl_number[number%10]);
		}

	}


}

void menu_3d_seq(void)
{
	nmea_slmsg seq_buff;
	u8 i,j;
    if(gpsx->svnum >13)
       gpsx->svnum = 13;
    
    for (i = 0; i < gpsx->svnum; i++) 
    {
        for (j = gpsx->svnum - 1; j > i; j--) 
        {
            if(gpsx->slmsg[j].num<gpsx->slmsg[j-1].num) 
            {
                seq_buff = gpsx->slmsg[j-1];
                gpsx->slmsg[j-1] =  gpsx->slmsg[j];
                gpsx->slmsg[j] = seq_buff;
            }
        }
    }
}

void menu_3D_display(void)
{
//	float tp;

	//extern const u8 BMPweixing[];
	u8 i,j,din_wei_flag;



//	Movie_Show_Img(39,0,Bmp_test_status);
//	show_levev_space();
	OLED_Fill(0, 0, 127, 63, 0);
	menu_status_dispplay();
//	sprintf((char *)dtbuf,"%02d:%02d",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes);	//��ʾUTC����
//	OLED_ShowString_head(0,0,dtbuf);

//	OLED_Fill(0, 16, 127, 63, 0);
	if(gpsx->fixmode == 3)
	{
		if(system_flag_table->lang == 1)
			OLED_ShowString(0,16,"3D Fix");
		else
			Movie_Show_Img(0, 16,BMP3Ddinwei);
	}
	else if(gpsx->fixmode == 2)
	{
		if(system_flag_table->lang == 1)
			OLED_ShowString(0,16,"2D Fix");
		else
			Movie_Show_Img(0, 16,BMP2Ddinwei);
	}
	else
	{
		if(system_flag_table->lang == 1)
			OLED_ShowString(0,16,"Locating.");
		else
			Movie_Show_Img(0, 16,Bmp_waitsinge);
	}


    if(gpsx->svnum>13)
		gpsx->svnum =13;
#if 1
	for(i=0;i<gpsx->svnum;i++)
	{

        menu_show_sl_number(gpsx->slmsg[i].num,i);
        din_wei_flag = 0;
        for(j=0;j<gpsx->posslnum;j++)
        {
            if(gpsx->slmsg[i].num== gpsx->possl[j])
            {
                if(gpsx->fixmode  >=2)
                    din_wei_flag = 1;
                break;
            }
        }
        if(((gpsx->slmsg[i].sn*23)/46)>0)
        {
            if(((gpsx->slmsg[i].sn*23)/46)>22)
                OLED_Fill((i*10), 34, (i*10+6), 56, 1);
            else
                OLED_Fill((i*10), (56-((gpsx->slmsg[i].sn*23)/46)), (i*10+6), 56, 1);
        }
        
        if(din_wei_flag)
        {
        }
        else
        {
            if(((gpsx->slmsg[i].sn*23)/46)>2)
            {
                if(((gpsx->slmsg[i].sn*23)/46)>22)
                    OLED_Fill((i*10), 35, (i*10+6), 55, 1);
                else
                    OLED_Fill((i*10+1), (57-((gpsx->slmsg[i].sn*23)/46)), (i*10+5), 55, 0);
            }
        }


    }
#endif


}



void show_the_fanxiang(float angle)
{
    u8 temp_angle = 0;
    
    temp_angle = anayes_angle(angle);
    OLED_Fill(0, 16, 15, 63, 0);
    OLED_Fill(16, 16, 47, 31, 0);
    OLED_Fill(32, 32, 47,63, 0);
    OLED_Fill(16, 48, 31,63, 0);

    switch(temp_angle)
	{
	    case 1:
		    Movie_Show_Img(16, 16,BMPzhengbei);
		//v1000_debug("\r\n BMPdongbei ");
		break;
	    case 2:
    //	case 16:
		    Movie_Show_Img(16, 16,BMPzhengbei);
		    Movie_Show_Img(32 ,16,BMPdongbei);
		break;

        case 3:
    //	case 15:
	  	    Movie_Show_Img(32 ,16,BMPdongbei);
	  	break;
	    case 4:
    //	case 14:
		    Movie_Show_Img(32 ,16,BMPdongbei);
		    Movie_Show_Img(32, 32,Bmpzhengdong);
		break;
	    case 5:
    //	case 13:
		    Movie_Show_Img(32, 32,Bmpzhengdong);
	    //	Movie_Show_Img(32, 48,Bmpdongnan);
		break;
	    case 6:
	    //case 12:
		    Movie_Show_Img(32, 32,Bmpzhengdong);
		    Movie_Show_Img(32, 48,Bmpdongnan);
		break;
	    case 7:
        
	    //case 11:
	    	Movie_Show_Img(32, 48,Bmpdongnan);
		break;
	    case 8:
	    //case 10:
	    	Movie_Show_Img(32, 48,Bmpdongnan);
	    	Movie_Show_Img(16, 48,Bmpzhengnan);
		break;
	    case 9:
	    	Movie_Show_Img(16, 48,Bmpzhengnan);
	    break;
	    case 10:
	    //case 8:
	    	Movie_Show_Img(16, 48,Bmpzhengnan);
	    	Movie_Show_Img(0, 48,BMPxinan);
	    break;
	    case 11:
	    //case 7:
	    	Movie_Show_Img(0, 48,BMPxinan);
		//v1000_debug("\r\n BMPdongbei ");
		break;
	    case 12:
	    //case 6:
	    	Movie_Show_Img(0, 48,BMPxinan);
	    	Movie_Show_Img(0, 32,Bmpzhengxi);

		break;

	    case 13:
	    //se 5:
	    	Movie_Show_Img(0, 32,Bmpzhengxi);
		break;
	    case 14:
	    //se 4:
	    	Movie_Show_Img(0, 32,Bmpzhengxi);
	    	Movie_Show_Img(0, 16,BMPxibei);
		break;
    	case 15:
    	//se 3:
	    	Movie_Show_Img(0, 16,BMPxibei);
		break;
	    case 16:
	    //case 2:
	    	Movie_Show_Img(0, 16,BMPxibei);
	    	Movie_Show_Img(16, 16,BMPzhengbei);
		break;
	}

//	v1000_debug("\r\n angle  %.3f temp_angle :%d,",angle,temp_angle);
}

//26.073861,119.286032

void save_the_interest_place(u32 lat1, char latitude1_flag,u32 lng1, char longitude1_flag,u8 index)
{
    u32 flag;
    char pio_file[13] ={0};
    //char pio_buffer[48] ={0};
    float temp1 = 0.0 ;
    float temp2 = 0.0 ;
    FRESULT fr;
    FIL home_fp ;

    
    flag = (latitude1_flag<<8)|longitude1_flag;
    stm_write_eerpom((HOME_WEIDU_ADDER+(3*index)), lat1);
    stm_write_eerpom((HOME_WEIDU_FLAG_ADDER+(3*index)), flag);
    stm_write_eerpom((HOME_JINDU_ADDER+(3*index)), lng1);
    temp1 = lat1;
    temp2 = lng1; 

    fr = f_mkdir("POI");
    
    sprintf(pio_file,"POI/%02d.TXT",index+1);
    v1000_debug("pio_file :%s\r\n",pio_file);
    //f_mount(&fs, "", 0);     
    //sys_fr = open_append(&sys_fp, pio_file);
    fr = f_open(&home_fp, pio_file,FA_READ|FA_WRITE|FA_OPEN_ALWAYS);

    if((FR_OK  == fr)||(FR_EXIST == fr))
    {
        sprintf((char *)dtbuf,"%.6f,%c,%.6f,%c",temp1/1000000,latitude1_flag,temp2/1000000,longitude1_flag);
        v1000_debug("pio_buffer :%s\r\n",dtbuf);
        f_printf(&home_fp,"%s\n",(char *)dtbuf);
        f_close(&home_fp);
    }

}


void read_the_interest_place(u32 *lat1, char *latitude1_flag,u32 *lng1, char *longitude1_flag,u8 index)
{
    u32 flag;
    stm_read_eerpom((HOME_WEIDU_ADDER+(3*index)), lat1);
    stm_read_eerpom((HOME_WEIDU_FLAG_ADDER+(3*index)), &flag);
    stm_read_eerpom((HOME_JINDU_ADDER+(3*index)), lng1);
//	stm_read_eerpom(HOME_JINDU_FLAG_ADDER+(4*index), longitude1_flag);

	*latitude1_flag = (char)((flag>>8)&0xff);
	*longitude1_flag = (char)(flag&0xff);
}


const u8 number_input[10]={'0','1','2','3','4','5','6','7','8','9'};
const u8 number_input_DU[10]={' ','1','2','3','4','5','6','7','8','9'};
const u8 char_weidu_input[2]={'N','S'};
const u8 char_jidu_input[2]={'E','W'};
static u8 input_memery[18];
static u8 input_index;

void change_latlon_to_index(void)
{
    u32 coxtern1,coxtern2,coxtern3;
    u8 i,temp,const_index = 0;

    stm_read_eerpom(HOME_NUMBER1_INDE, &coxtern1);
    stm_read_eerpom(HOME_NUMBER2_INDE, &coxtern2);
    stm_read_eerpom(HOME_NUMBER3_INDE, &coxtern3);

    for(i=0;i<18;i++)
    {



        if(i == 0)
        {
            const_index =  (u8)((coxtern1>>24)&0xff);
            if(const_index == 'N')
            const_index = 0;
            else
            const_index = 1;


            temp = char_weidu_input[i];

        }
        else if(i == 9)
        {
            const_index =   (u8)((coxtern1>>16)&0xff);
    
            if(const_index == 'E')
                const_index = 0;
            else
                const_index = 1;
    
            temp = char_jidu_input[const_index];

        }
        else if((i == 1)||(i == 10))
        {
            if(i == 1)
                const_index= (u8)((coxtern1>>8)&0xff);

            if(i == 10)
                const_index= (u8)((coxtern1)&0xff);

            if(const_index == ' ')
                const_index = 0;
            else
                const_index = const_index-'0';


            temp = number_input_DU[const_index];

        }
        else
        {
            if(i<9)
                const_index = (u8)((coxtern2>>(28-(4*(i-2))))&0xf);
            else
                const_index = (u8)((coxtern3>>(28-(4*(i-11))))&0xf);
            v1000_debug("\r\n coxtern1  %x coxtern2 :%x  coxtern3 :%x ",coxtern1,coxtern2,coxtern3);
            v1000_debug("\r\n const_index  %d  ",const_index);
            temp = number_input[const_index];
        }


        input_memery[i] = temp;

    }
    input_memery[1] = ' ';

	convert_show_en_input(input_memery,32);
	convert_show_en_input(&input_memery[9],48);


}
void menu_input_process(void)
{
	u8 temp,const_index = 0,flag_60;
	u32 coxtern1,coxtern2,coxtern3;

    //	read_the_interest_place(&lat1,&latitude1_flag,&lng1,&longitude1_flag,save_pos);
    stm_read_eerpom(HOME_NUMBER1_INDE, &coxtern1);
    stm_read_eerpom(HOME_NUMBER2_INDE, &coxtern2);
    stm_read_eerpom(HOME_NUMBER3_INDE, &coxtern3);

    //	v1000_debug("\r\n coxtern1  %d coxtern2 :%d  coxtern3 :%d ",coxtern1,coxtern2,coxtern3);
    v1000_debug("\r\n coxtern1  %x coxtern2 :%x  coxtern3 :%x ",coxtern1,coxtern2,coxtern3);
    v1000_debug("\r\n const_index  %d  ",const_index);

	if(input_index>=18)
		input_index = 0;

        if(input_index == 0)
       	{
       	const_index =  (u8)((coxtern1>>24)&0xff);
		if(const_index == 'N')
			const_index = 0;
		else
			const_index = 1;
		if((key_type == KEY_UP)||(key_type == KEY_DOWN))
			{
			if(const_index == 1)
				const_index = 0;
			else
				const_index = 1;
			}

	   	temp = char_weidu_input[const_index];
		coxtern1 &=0x00ffffff;
		coxtern1 |= temp<<24;
		stm_write_eerpom(HOME_NUMBER1_INDE, coxtern1);
       	}
	   else if(input_index == 9)
	   	{
	   	const_index =   (u8)((coxtern1>>16)&0xff);

		if(const_index == 'E')
			const_index = 0;
		else
			const_index = 1;
		if((key_type == KEY_UP)||(key_type == KEY_DOWN))
			{
			if(const_index == 1)
				const_index = 0;
			else
				const_index = 1;
			}
	   	temp = char_jidu_input[const_index];
		coxtern1 &=0xff00ffff;
		coxtern1 |= temp<<16;
		stm_write_eerpom(HOME_NUMBER1_INDE, coxtern1);
	   	}
	 else if((input_index == 1)||(input_index == 10))
	 	{
	 	if(input_index == 1)
	 		const_index= (u8)((coxtern1>>8)&0xff);

	      if(input_index == 10)
		 	const_index= (u8)((coxtern1)&0xff);

		if(const_index == ' ')
			const_index = 0;
		else
			const_index = const_index-'0';

		if(key_type == KEY_UP)
			{
			if(const_index == 0)
				const_index = 1;
			else
				const_index --;
			}
		else if(key_type == KEY_DOWN)
			{
			if(const_index == 1)
				const_index = 0;
			else
				const_index ++;
			}
	   	temp = number_input_DU[const_index];
		if(input_index == 1)
			{
			coxtern1 &=0xffff00ff;
			coxtern1 |= temp<<8;
			}
		else
			{
			coxtern1 &=0xffffff00;
			coxtern1 |= temp;

			}
		stm_write_eerpom(HOME_NUMBER1_INDE, coxtern1);
	 	}
	  else
	  	{
	  	if(input_index<9)
	  		{
		   	const_index = (u8)((coxtern2>>(28-(4*(input_index-2))))&0xf);

			if((input_index==4)||(input_index == 6))
				flag_60 =1;
			else
				flag_60 =0;

	  		}
		else
			{
			if((input_index==13)||(input_index == 15))
				flag_60 =1;
			else
				flag_60 =0;

			const_index = (u8)((coxtern3>>(28-(4*(input_index-11))))&0xf);
			}

		if(key_type == KEY_UP)
			{
			if(const_index == 0)
				{
				if(flag_60 == 1)
					const_index = 5;
				else
					{
					       if((input_index==2))
						   	const_index = 8;
						 else if((input_index==11) &&(((coxtern1)&0xff) == '1'))
						 {
						 	const_index = 7;
						 }
						 else
							const_index = 9;
					}
				}
			else
				const_index --;
			}
		else if(key_type == KEY_DOWN)
			{
			if(flag_60 == 0)
				{
				 if((input_index==2)&&(const_index == 8))
				   	{
				   		const_index = 0;
				 	}
				else if((input_index==11)&(const_index == 7)&&(((coxtern1)&0xff) == '1'))
					{
						const_index = 0;
					}

				else if(const_index == 9)
					{
						const_index = 0;
					}
				else
						const_index ++;
				}
			else
				{

					if(const_index == 5)
						const_index = 0;
					else
						const_index ++;

				}
			}

		if(input_index<9)
	  		{
            coxtern2  &= ~(((uint32_t)0x0000000f) << (28-(4*(input_index-2))));

            //GPIOx->MODER |= (((uint32_t)GPIO_InitStruct->GPIO_Mode) << (pinpos * 2));
            //coxtern2 &=(0xf<<(28-(4*(input_index-2))));
            //coxtern2 = ~coxtern2;
            coxtern2 |= const_index<<(28-(4*(input_index-2)));
            stm_write_eerpom(HOME_NUMBER2_INDE, coxtern2);
	  		}
		else
			{

			//coxtern3 &=(0xf<<(28-(4*(input_index-11))));
			//coxtern3 = ~coxtern3;
			coxtern3  &= ~(((uint32_t)0x0000000f) << (28-(4*(input_index-11))));
			coxtern3 |= const_index<<(28-(4*(input_index-11)));
			stm_write_eerpom(HOME_NUMBER3_INDE, coxtern3);
			}
	   	temp = number_input[const_index];
	  	}


	   	input_memery[input_index] = temp;


	convert_show_en_input(input_memery,32);
	convert_show_en_input(&input_memery[9],48);



}

void convert_u32_to_index(float lat1, char latitude1_flag,float lng1, char longitude1_flag)
{
    u8 du,fen,temp_vaule,temp_vaule1;
    float miao;
    u32 coxtern1 =0 ,coxtern2 = 0,coxtern3 = 0;

	coxtern1 |= (latitude1_flag<<24)|(longitude1_flag<<16);
	du = (u8)(lat1/1000000);
	fen = (u8)((lat1/1000000 - du)*60);
	miao = ((lat1/1000000 - du)*60 -fen)*600;

    v1000_debug("\r\n convert_u32_to_index  %.1f  ",miao);
    if(miao >599.0)
    {
    	miao = 0.0;
    	fen++;
    	if(fen>59)
    		du++;
    }
	if(du<100)
	{
        coxtern1 |= ' '<<8;
        temp_vaule = du/10;
        coxtern2 |= (temp_vaule<<28);
        //	  v1000_debug("\r\n temp_vaule  %d  ",temp_vaule);
        temp_vaule = du -(temp_vaule*10);
        //	 v1000_debug("\r\n temp_vaule  %d  ",temp_vaule);
        coxtern2 |= (temp_vaule<<24);
	}
	else
	{
        temp_vaule = du/100;
        coxtern1 |= ((temp_vaule+'0')<<8);
        temp_vaule1 = ((du-temp_vaule*100)/10);
        coxtern2 |= (temp_vaule1<<28);
        temp_vaule = (du -(temp_vaule*100)-(temp_vaule1*10));
        coxtern2 |= (temp_vaule<<24);
	}

    temp_vaule = fen/10;
    coxtern2 |= (temp_vaule<<20);
    temp_vaule = (fen -temp_vaule*10);
    coxtern2 |= (temp_vaule<<16);
    
    temp_vaule = (u8)(miao/100);
	//  v1000_debug("\r\n temp_vaule  %d  ",temp_vaule);
	coxtern2 |= (temp_vaule<<12);
	temp_vaule1 = (u8)(miao -temp_vaule*100)/10;
//	  v1000_debug("\r\n temp_vaule1  %d  ",temp_vaule1);
	coxtern2 |= (temp_vaule1<<8);
	temp_vaule1 = (u8)(miao -(temp_vaule*100) - (temp_vaule1*10));
//	  v1000_debug("\r\n temp_vaule1  %d  ",temp_vaule1);
	coxtern2 |= (temp_vaule1<<4);



	du = (u8)(lng1/1000000);
	fen = (u8)((lng1/1000000 - du)*60);
	miao = ((lng1/1000000 - du)*60 -fen)*600;

	v1000_debug("\r\n convert_u32_to_index  %d %d %.1f  ",du,fen,miao);
    if(miao >599.0)
    {
    	miao = 0.0;
   	fen++;
   	if(fen>59)
   		du++;
    }
	if(du<100)
	{
        coxtern1 |= ' ';
        temp_vaule = (du/10);
        coxtern3 |= (temp_vaule<<28);
        temp_vaule = (du -(temp_vaule*10));
        coxtern3 |= (temp_vaule<<24);
	}
	else
	{
		temp_vaule = du/100;
		coxtern1 |= (temp_vaule+'0');
	//	v1000_debug("\r\n temp_vaule  %d  ",temp_vaule);
		temp_vaule1 = ((du-temp_vaule*100)/10);
	//	v1000_debug("\r\n temp_vaule1  %d  ",temp_vaule1);
		coxtern3 |= (temp_vaule1<<28);
		temp_vaule = (du -(temp_vaule*100)-(temp_vaule1*10));
		coxtern3 |= (temp_vaule<<24);
	//	v1000_debug("\r\n temp_vaule  %d  ",temp_vaule);
	}

    temp_vaule = fen/10;
    coxtern3 |= (temp_vaule<<20);
    temp_vaule = fen -temp_vaule*10;
    coxtern3 |= (temp_vaule<<16);

    temp_vaule = (u8)(miao/100);
	//	  v1000_debug("\r\n temp_vaule  %d  ",temp_vaule);
    coxtern3 |= temp_vaule<<12;
    temp_vaule1 = (u8)(miao -temp_vaule*100)/10;
	//	  v1000_debug("\r\n temp_vaule1  %d  ",temp_vaule1);
    coxtern3 |= (temp_vaule1<<8);
    temp_vaule1 = (u8)(miao -temp_vaule*100 - temp_vaule1*10);
//		  v1000_debug("\r\n temp_vaule1  %d  ",temp_vaule1);
	 coxtern3 |= (temp_vaule1<<4);

	stm_write_eerpom(HOME_NUMBER1_INDE, coxtern1);
	stm_write_eerpom(HOME_NUMBER2_INDE, coxtern2);
	stm_write_eerpom(HOME_NUMBER3_INDE, coxtern3);
}


//#define TEST_TIME
#if 0
void test_angle(void)
{
    float angle = 0.0;
    float tp_long,tp_lati,tp_long_dest,tp_lati_dest;
    
    gpsx->nshemi = 'N';
    gpsx->ewhemi = 'E';
    tp_lati_dest= 26.097866;
    tp_long_dest= 119.263535;
//    tp_lati= 28.222201;
//    tp_long= 113.117470;   
     tp_lati= 26.097914;
     tp_long= 119.265923;   

    angle = get_angle(tp_lati, gpsx->nshemi, tp_long, gpsx->ewhemi, tp_lati_dest,  gpsx->nshemi, tp_long_dest,   gpsx->ewhemi);

	angle = angle - 82;
	if(angle<0)
	{
		angle = angle+360;
    }    
    v1000_debug("\r\n angle : %.1f\r\n",angle);

}
#endif
void menu_home_display(void)
{
	float tp;


    float tp_long,tp_lati,tp_long_dest,tp_lati_dest;
    float distance  = 0.0,angle = 0.0;
    u32 lat1, lng1;
    char latitude1_flag,longitude1_flag;
    u16 timer_need;
//    FIL test_fp ;
 //   FRESULT test_fr ;


	tp_long = gpsx->longitude;
	tp_long/= 1000000;
	tp_lati = gpsx->latitude;
	tp_lati/= 1000000;


	OLED_Fill(0, 0, 127, 63, 0);
	menu_status_dispplay();
	if(home_menu_item_index+menu_item_index >=16)
	{
		home_menu_item_index= 0;
		menu_item_index = 0;
	}
	if(home_menu_item_index+menu_item_index <0)
	{
		home_menu_item_index = 15;
		menu_item_index = 0;
	}

	read_the_interest_place(&lat1,&latitude1_flag,&lng1,&longitude1_flag,home_menu_item_index+menu_item_index);
	if(enter_perss == 0)
	{

		Movie_Show_Img_force1616(16,32,home_menu_item_index+menu_item_index);
        if(gpsx->fixmode > 1)
        {
        	if((latitude1_flag == 'N')||(latitude1_flag == 'S'))
        	{
        		tp_long_dest = lng1;
        		tp_long_dest/=1000000;
        		tp_lati_dest = lat1;
        		tp_lati_dest/=1000000;
            	//distance = getDistanceVer1( tp_lati, tp_long, tp_lati_dest,  tp_long_dest);
            	distance = getDistanceVer2( tp_lati, gpsx->nshemi, tp_long, gpsx->ewhemi, tp_lati_dest,  latitude1_flag, tp_long_dest,  longitude1_flag);
            	//angle = get_angle( tp_lati_dest,  latitude1_flag, tp_long_dest,  longitude1_flag,tp_lati, gpsx->nshemi, tp_long, gpsx->ewhemi);
            	angle = get_angle(tp_lati, gpsx->nshemi, tp_long, gpsx->ewhemi, tp_lati_dest,  latitude1_flag, tp_long_dest,  longitude1_flag);
            	////angle = GetAngle( tp_lati_dest,  latitude1_flag, tp_long_dest,  longitude1_flag,tp_lati, gpsx->nshemi, tp_long, gpsx->ewhemi);
            	tp = gpsx->angle;
                //sprintf((char *)dtbuf,"%.1f:%.1f",angle,tp/1000);
                //OLED_ShowString(48,16,dtbuf);
                //v1000_debug("angle :%d:%d",(int)angle,(int)tp/1000);
#if 0                
                if((angle >360.0) || (angle < -360.0))
                {
                    test_fr = open_append(&test_fp, "log.txt");
                    if((FR_OK  == test_fr)||(FR_EXIST == test_fr))
                    {
                        v1000_debug("create file \r\n");
                        sprintf((char *)dtbuf,"src:%.6f,%c,%.6f,%c",tp_lati,gpsx->nshemi,tp_long, gpsx->ewhemi);                
                        f_printf(&test_fp,"%s\n",(char *)dtbuf);  
                        sprintf((char *)dtbuf,"dest:%.6f,%c,%.6f,%c",tp_lati_dest, latitude1_flag, tp_long_dest,longitude1_flag);                
                        f_printf(&test_fp,"%s\n",(char *)dtbuf);                      

                        sprintf((char *)dtbuf,"angle:%.1f-tp:%.1f",angle,tp/1000);                
                        f_printf(&test_fp,"%s \n",(char *)dtbuf);
                        f_close(&test_fp);
                    }
                    else
                        v1000_debug("create file faild \r\n");
                }
#endif				
            	tp /= 1000;
            	angle = angle - tp;
            	if(angle<0)
            	{
            		angle = angle+360;
        	}

	    }
	    else
        {   
            lat1 = 0;
		    return;
        }
        }
    
        #ifndef TEST_TIME
    	    if(gpsx->fixmode <= 1)
    		{
    			if(system_flag_table->lang == 1)
    				OLED_ShowString(32,16,"Locating.");
    			else
    				Movie_Show_Img(32,16,Bmp_waitsinge);
    
    		    return;
    		}
       #endif
    	//v1000_debug("\r\n distance  %.1fkm",distance);
    	if((distance <= 0.01)&&(distance != 0))
    	{  
    	#if 1
    		if(system_flag_table->lang == 1)
    			OLED_ShowString(48,16,"Arriving.");
    		else
    			Movie_Show_Img(48,16,bmp_near_place);
        #endif
            if(Arriving_sound_flag == 0)
            {
        	    Arriving_sound_flag =1;
            }
    	}
    	else
    	{
    		Arriving_sound_flag = 0;
    		if(system_flag_table->myunit.H_W == 0)
    		{
    			if(distance>=10 && distance<100)
    				sprintf((char *)dtbuf,"%.2fkm",distance);		    		//�õ��ٶ��ַ���
    			else if (distance<10)
    				sprintf((char *)dtbuf,"%.3fkm",distance);		    		//�õ��ٶ��ַ���
    			else
    				sprintf((char *)dtbuf,"%ldkm",(u32)(distance));		    		//�õ��ٶ��ַ���
    		}
    		else if(system_flag_table->myunit.H_W == 1)
    		{
    			if((distance/1.609)>=10 && (distance/1.609)<100)
    				sprintf((char *)dtbuf,"%.2fmi",distance/1.609);		    		//�õ��ٶ��ַ���
    			else if ((distance/1.609)<10)
    				sprintf((char *)dtbuf,"%.3fmi",distance/1.609);		    		//�õ��ٶ��ַ���
    			else
    				sprintf((char *)dtbuf,"%ldmi",(u32)(distance/1.609));		    		//�õ��ٶ��ַ���
               
    		}

    		else if(system_flag_table->myunit.H_W == 2)
    		{
    			if((distance/1.852)>=10 && (distance/1.852)<100)
    				sprintf((char *)dtbuf,"%.2fkt",distance/1.852);		    		//�õ��ٶ��ַ���
    			else if ((distance/1.852)<10)
    				sprintf((char *)dtbuf,"%.3fkt",distance/1.852);		    		//�õ��ٶ��ַ���
    			else
    				sprintf((char *)dtbuf,"%ldkt",(u32)(distance/1.852));		    		//�õ��ٶ��ַ���
    
    		}

    		OLED_ShowString(48,16,dtbuf);
    		show_the_fanxiang(angle);
    	}
    
        tp=gpsx->speed;
        if(tp >1500000.0)
            tp = 999900.0;
        else if(tp < 1000)
            tp = 0;
    
    	if(Arriving_sound_flag == 0)
    	{
    	#if 1
    		if(tp >= 3000)
    			Movie_Show_Img(110,16,Bmp_lock);
    		else
    			OLED_Fill(110, 16, 127, 31, 0);
        #endif    
    	}
    
    	if(system_flag_table->myunit.H_W == 0)//||(system_flag_table->myunit.H_W == 1))
    	{
    		if(tp/1000>=100.0)
    			sprintf((char *)dtbuf,"%.0fkm/h",tp/=1000);		    		//�õ��ٶ��ַ���
    		else
    		 	sprintf((char *)dtbuf,"%.1fkm/h",tp/=1000);		    		//�õ��ٶ��ַ���
    	}

    	else if(system_flag_table->myunit.H_W == 1)
    	{
    		if(tp/1609>=100.0)
    			sprintf((char *)dtbuf,"%.0fmi/h",tp/=1609);		    		//�õ��ٶ��ַ���
    		else
    		 	sprintf((char *)dtbuf,"%.1fmi/h",tp/=1609);		    		//�õ��ٶ��ַ���
    	}

    	else if(system_flag_table->myunit.H_W == 2)
    	{
    		if(tp/1852>=100.0)
    			sprintf((char *)dtbuf,"%.0fkt/h",tp/=1852);		    		//�õ��ٶ��ַ���
    		else
    		 	sprintf((char *)dtbuf,"%.1fkt/h",tp/=1852);		    		//�õ��ٶ��ַ���
    	}
    
    	OLED_ShowString(48,32,dtbuf);
    
    #ifdef TEST_TIME
    	distance = 3.45;
    	tp = 13112.1;
    #endif
    	timer_need = (u16)((distance/tp)*60);
    
    	if((tp == 0.0)||(timer_need>=6000))
    	{
    			
    	    timer_need  = 5999;

    	}

        if((distance < 0.01)&&(lat1 >0))
        {
            timer_need = 0;
        }        
        sprintf((char *)dtbuf,"%02d:%02d",timer_need/60,timer_need%60);		    		//�õ��ٶ��ַ���
    //	v1000_debug("\r\n timer_need1  %d %d  ",timer_need/60,timer_need%60);
    
    	OLED_ShowString(80,48,dtbuf);
    	if(system_flag_table->lang == 1)
    		OLED_ShowString(48,48,"ETE.");
    	else
    		Movie_Show_Img(48, 48,bmpyuji);
    }
	else if(enter_perss >0)
	{
			
		if((latitude1_flag == 'N')||(latitude1_flag == 'S'))
		{
			convert_show_en(lng1,longitude1_flag,48,0);
			convert_show_en(lat1,latitude1_flag,32,0);
            if(gpsx->fixmode > 1)
            {
				tp_long_dest = lng1;
				tp_long_dest/=1000000;
				tp_lati_dest = lat1;
				tp_lati_dest/=1000000;
				distance = getDistanceVer2( tp_lati, gpsx->nshemi, tp_long, gpsx->ewhemi, tp_lati_dest,  latitude1_flag, tp_long_dest,  longitude1_flag);
            }
		}
		else if(home_flag == 0)
		{
		
		    Movie_Show_Img(1,24,bmp_destination_updown);
			sprintf((char *)dtbuf,"%02d",home_menu_item_index+menu_item_index+1);				
			Movie_Show_Img_force1616(0,32,home_menu_item_index+menu_item_index);
			OLED_ShowString(16,16,dtbuf);
			//return ;
		}

        
		if(home_flag != 3)
	    {
			sprintf((char *)dtbuf,"%02d",home_menu_item_index+menu_item_index+1);				
			Movie_Show_Img(1,24,bmp_destination_updown);
			OLED_ShowString(16,16,dtbuf);;
	    }
      	switch(home_flag )
	    {
	      	case 0:
			   

                if(gpsx->fixmode <= 1)
                {
                    if(system_flag_table->lang == 1)
                        OLED_ShowString(32,16,"Locating.");
                    else
                        Movie_Show_Img(32,16,Bmp_waitsinge);
                    v1000_debug("\n\r index :%x,%x ",home_menu_item_index +menu_item_index,enter_perss);
                    if(((home_menu_item_index +menu_item_index)>= CUSTOM_HOME_POST) && (enter_perss == 2))
                    {
                        home_flag = 1;
                    }

                
                }   
                else
                {
                    OLED_Fill(48, 16, 127, 31, 0);
    			    if((distance <0.001)&&(lat1 >0))
    			    {                    
                          
                        if(system_flag_table->lang == 1)
        					OLED_ShowString(21,16,"Arriving.");
        				else
        					Movie_Show_Img(48,16,bmp_near_place);
                       
                       
    			    }
    			    else if(lat1 >0)
    				{
    				    if(system_flag_table->myunit.H_W == 0)
    					{
        					if(distance>=10 && distance<100)
        						sprintf((char *)dtbuf,"%.2fkm",distance);		    		//�õ��ٶ��ַ���
        					else if (distance<10)
        						sprintf((char *)dtbuf,"%.3fkm",distance);		    		//�õ��ٶ��ַ���
        					else
        						sprintf((char *)dtbuf,"%ldkm",(u32)(distance));		    		//�õ��ٶ��ַ���
    					}
    				    else if(system_flag_table->myunit.H_W == 1)
    					{
        					if((distance/1.609)>=10 && (distance/1.609)<100)
        						sprintf((char *)dtbuf,"%.2fmi",distance/1.609);		    		//�õ��ٶ��ַ���
        					else if ((distance/1.609)<10)
        						sprintf((char *)dtbuf,"%.3fmi",distance/1.609);		    		//�õ��ٶ��ַ���
        					else
        						sprintf((char *)dtbuf,"%ldmi",(u32)(distance/1.609));		    		//�õ��ٶ��ַ���
    					}
    			
    			    	else if(system_flag_table->myunit.H_W == 2)
    					{
        					if((distance/1.852)>=10 && (distance/1.852)<100)
        						sprintf((char *)dtbuf,"%.2fkt",distance/1.852);		    		//�õ��ٶ��ַ���
        					else if ((distance/1.852)<10)
        						sprintf((char *)dtbuf,"%.3fkt",distance/1.852);		    		//�õ��ٶ��ַ���
        					else
        						sprintf((char *)dtbuf,"%ldkt",(u32)(distance/1.852));		    		//�õ��ٶ��ַ���
    
    					}
    
    
                        if(gpsx->fixmode >1)  // GPS��λ�������ʾ����
    					    OLED_ShowString(48,16,dtbuf);
    				}
                }
			    break;
	      	case 1:

                if(system_flag_table->lang == 1)
            		OLED_ShowString(32,16,"Choose Icon");
            	else
            		 Movie_Show_Img(32,16,bmp_choose_post);
                if((gpsx->fixmode <= 1) && ((home_menu_item_index +menu_item_index) <CUSTOM_HOME_POST))
                {
                    home_flag = 0;
                }

			    break;
	        case 2:
		    	OLED_Fill(0, 16, 127, 63, 0);
		      	if((home_menu_item_index+menu_item_index >= CUSTOM_HOME_POST )&&((key_type == KEY_ENTER)||(key_type == KEY_LONG_ENTER)))
		      	{
		      		if((latitude1_flag != 'N')&&(latitude1_flag != 'S'))
		      		{
		      			latitude1_flag = 'N';
		      			longitude1_flag = 'E';
		      		}
	
    				convert_u32_to_index(lat1,latitude1_flag,lng1,longitude1_flag);
                    if(system_flag_table->lang == 1)
    				    OLED_ShowString(16,16,"Enter Location");
    			    else
    				    Movie_Show_Img(22,16,Bmp_input_jiweidu);
	
    				sprintf((char *)dtbuf,"_");
    				OLED_ShowString(24,32,dtbuf);
    				change_latlon_to_index();
    				input_index = 0;
    				home_flag = 3;
	      		}
			    else
				{

    				convert_show_en(homex.longitude,homex.ewhemi,48,0);
    				convert_show_en(homex.latitude,homex.nshemi,32,0);
    				home_menu_item_index = home_menu_item_index+menu_item_index;
    				menu_item_index = 0;
    				save_the_interest_place(homex.latitude, homex.nshemi, homex.longitude, homex.ewhemi,home_menu_item_index);
    				
    				if(system_flag_table->lang == 1)
    					OLED_ShowString(21,16,"POI Marked");
    				else
    					Movie_Show_Img(22,16,bmp_choosed);
                    memset(&homex,0x00,sizeof(HomeGpsStruct));
    				home_flag = 0;
    				enter_perss =3;
				}
			    break;

		    case 3:
				//OLED_Fill(48, 16, 127, 32, 0);
			    if(system_flag_table->lang == 1)
					OLED_ShowString(21,16,"Enter Location");
				else
					Movie_Show_Img(22,16,Bmp_input_jiweidu);
				if(key_type == KEY_LONG_ENTER)
				{
					OLED_Fill(0, 16, 127, 63, 0);
					if(input_memery[10] ==' ')
						lng1 = (1000000*((input_memery[11]-'0')*10+(input_memery[12]-'0')));
					else
						lng1 = (1000000*((input_memery[10]-'0')*100+(input_memery[11]-'0')*10+(input_memery[12]-'0')));


                                tp = 10*((input_memery[13]-'0')*10+(input_memery[14]-'0'));
					tp /=6;
					//v1000_debug("\r\n feng   %.5fd",tp);
					lng1 += tp*10000;
					tp = 1000*((input_memery[15]-'0')*100+(input_memery[16]-'0')*10+(input_memery[17]-'0'));
					//v1000_debug("\r\n maio number  %.1f",tp);
					tp /=36;
					//v1000_debug("\r\n maio   %.1fd",tp);
					lng1 += tp;
					//v1000_debug("\r\n lng1 number  %d",lng1);


					longitude1_flag= input_memery[9];
				//	v1000_debug("\r\n llongitude1_flag  %c",longitude1_flag);

					if(input_memery[1] ==' ')
						lat1 = (1000000*((input_memery[2]-'0')*10+(input_memery[3]-'0')));
					else
						lat1 = (1000000*((input_memery[1]-'0')*100+(input_memery[2]-'0')*10+(input_memery[3]-'0')));


                                tp = 10*((input_memery[4]-'0')*10+(input_memery[5]-'0'));
					tp /=6;
					//v1000_debug("\r\n fen %.5fd",tp);
					lat1 += tp*10000;
					tp = 1000*((input_memery[6]-'0')*100+(input_memery[7]-'0')*10+(input_memery[8]-'0'));
					tp /=36;
					//v1000_debug("\r\n maio  %.1fd",tp);
					lat1 += tp;
					//v1000_debug("\r\n lat1 number  %d",lat1);
					latitude1_flag = input_memery[0];
					//v1000_debug("\r\n latitude1_flag  %c",latitude1_flag);

					convert_show_en(lng1,longitude1_flag,48,0);
					convert_show_en(lat1,latitude1_flag,32,0);
					home_menu_item_index =home_menu_item_index+menu_item_index;
					menu_item_index = 0;
					save_the_interest_place(lat1,latitude1_flag,lng1,longitude1_flag,home_menu_item_index);
					//v1000_debug("\r\n home_menu_item_index  %d",home_menu_item_index);
               		if(system_flag_table->lang == 1)
						OLED_ShowString(21,16,"POI Marked");
					else
						Movie_Show_Img(22,16,bmp_choosed);
					home_flag = 0;
					enter_perss =3;
					return;
				}
				if(key_type == KEY_ENTER)
				{
    				sprintf((char *)dtbuf,"%c",input_memery[input_index]);
    				if(input_index<9)
    					OLED_ShowString(input_jinweidu_post[input_index],32,dtbuf);
    				else
    					OLED_ShowString(input_jinweidu_post[input_index],48,dtbuf);
    				input_index++;
    				if(input_index == 1)
    					input_index  = 2;
    				sprintf((char *)dtbuf,"_");
    				if(input_index<9)
    					OLED_ShowString(input_jinweidu_post[input_index],32,dtbuf);
    				else
    					OLED_ShowString(input_jinweidu_post[input_index],48,dtbuf);
				}
				menu_input_process();
			    break;

	    }


		Movie_Show_Img_force1616(0,32,home_menu_item_index+menu_item_index);


	}

}

tm my_timer;

int test_sunriset(u16 year,u8 month,u8 day,float lon,float lat)
{

	float sunrise = 0.0;
    float sunset = 0.0 ;
	float altit = -35.0/60.0,upper_limb = 1.0;

	float zone = time_zone;

	int hour,minute;

    
	sunriset( year, month, day, lon, lat,
		altit, upper_limb, &sunrise, &sunset );//�����㷨��������ճ�����ʱ�䱣����sunrise��sunset��

	hour = sunrise + zone;
	minute = (sunrise - hour + zone)*60;

    if(gpsx->fixmode!=3)
        sprintf((char *)dtbuf,"--:--");	//��ʾUTC����
    else
        sprintf((char *)dtbuf,"%02d:%02d",hour,minute);	//��ʾUTC����
	OLED_ShowString(22,16,dtbuf);


	hour = sunset + zone;
	minute = (sunset - hour+zone)*60;
 
    if(gpsx->fixmode!=3)
   		sprintf((char *)dtbuf,"--:--");	//��ʾUTC����
    else
       	sprintf((char *)dtbuf,"%02d:%02d",hour,minute);	//��ʾUTC����
	OLED_ShowString(83,16,dtbuf);

	return 0;
}



void menu_chaoxi_display(void)
{
	float tp_long = 0.0 ;
    float tp_lati = 0.0 ;


    tp_long = gpsx->longitude;
	tp_long/=1000000;

    if(gpsx->ewhemi == 'W')
    {

        tp_long = -tp_long;
    }
    else
    {
    
    }
    

    tp_lati = gpsx->latitude;  
    tp_lati/=1000000;
    if(gpsx->nshemi == 'S')
    {
        tp_lati = -tp_lati;
    }
    else
    {
      
    }

	OLED_Fill(0, 0, 127, 63, 0);
	menu_status_dispplay();
	Movie_Show_Img(0, 16,bmpchaoxi);


	if(enter_perss == 0)
	{
		my_timer.w_year = RTC_DateStructure.RTC_Year+2000;
		my_timer.w_month =RTC_DateStructure.RTC_Month;
		my_timer.w_date = RTC_DateStructure.RTC_Date;
		my_timer.week = RTC_DateStructure.RTC_WeekDay;
		Movie_Show_Img(112, 49,Bmp_ponit);
	}
	else
	{
		Movie_Show_Img(112, 48,Bmp_updwon);
		my_timer.week = RTC_Get_Week(my_timer.w_year ,my_timer.w_month,my_timer.w_date);
	}

	sprintf((char *)dtbuf,"%02d-%02d-%02d",RTC_DateStructure.RTC_Year ,my_timer.w_month,my_timer.w_date);	//��ʾUTC����
	OLED_ShowString(0,48,dtbuf);
	sprintf((char *)dtbuf,"  ");	//��ʾUTC����
   	OLED_ShowString(64,48,dtbuf);
	test_sunriset(my_timer.w_year ,my_timer.w_month,my_timer.w_date,tp_long,tp_lati);
	moon_test(tp_long,tp_lati,my_timer.w_year ,my_timer.w_month,my_timer.w_date,time_zone);
    sprintf((char *)dtbuf,"%c%c%c",week_word[my_timer.week][0],week_word[my_timer.week][1],week_word[my_timer.week][2]);
	OLED_ShowString(86,48,dtbuf);

}



void guji_Distance(void)
{
    float tp_long,tp_lati;
    float tp_distance = 0;
    float tp =0.0;
    static float tp_distacn_temp = 0;


    if((gpsx->fixmode ==3)&&(gpsx->latitude >0)&&(gpsx->longitude>0))
    {
    	tp_long = gpsx->longitude;
    	tp_long/= 1000000;
    	tp_lati = gpsx->latitude;
    	tp_lati/= 1000000;

    	if(guji_mode >= RECORED_START_DOING)
    	{
            if(Message_head_number >0)
            {
                if((guji_tp_lati_dest >0)&&(guji_latitude1_flag>0))
                    tp_distance = getDistanceVer2( tp_lati, gpsx->nshemi, tp_long, gpsx->ewhemi, guji_tp_lati_dest,  guji_latitude1_flag, guji_tp_long_dest,  guji_longitude1_flag);
                else
                {
                    guji_tp_long_dest = tp_long;
                    guji_tp_lati_dest = tp_lati;
                    guji_latitude1_flag= gpsx->nshemi;
                    guji_longitude1_flag = gpsx->ewhemi;
                }

    	    }
	        if(tp_distance > 4)
            {
            	guji_tp_long_dest = tp_long;
            	guji_tp_lati_dest = tp_lati;
            	guji_latitude1_flag= gpsx->nshemi;
            	guji_longitude1_flag = gpsx->ewhemi;
            	return;
            }


     	   switch(guji_icon_index)
           {
           	   	case 0:
           			tp = 0.005;
           			break;
           	   	case 1:
           			tp = 0.010;
           			break;
           	   	case 2:
           			tp = 0.020;
           			break;
           	   	case 3:
           			tp = 0.050;
           			break;
           	   	case 4:
           			tp = 0.500;
           			break;
           	   	case 5:
          			if(system_flag_table->guji_record.by_distance_time_flag == 0)
          			{
                          if(system_flag_table->myunit.H_W >= 1)
                          {
                              tp = system_flag_table->guji_record.by_distance_vaule_ft;
                              tp /= 1000;
                              tp *= 0.3048;
                          }
                          else
                          {
                              tp = system_flag_table->guji_record.by_distance_vaule;
                              tp /= 1000;
                          }
          			}
          			else
                    {
                        if((tatol_time -tatol_oled_time) >= system_flag_table->guji_record.by_time_vaule)
                        {
                            if(tp_distance > 0.005)
                            {
                                guiji_distance += tp_distance;
                                guji_tp_long_dest = tp_long;
                                guji_tp_lati_dest = tp_lati;
                                guji_latitude1_flag = gpsx->nshemi;
                                guji_longitude1_flag = gpsx->ewhemi;
        
                            }
        
        
        
                            if(system_flag_table->myunit.H_W == 0)
                            {
                                if(gpsx->speed/1000<system_flag_table->guji_record.by_speed_vaule)
                                return;
                            }
                            else if(system_flag_table->myunit.H_W == 1)
                            {
                                if(gpsx->speed<(system_flag_table->guji_record.by_speed_vaule_1*1609))
                                return;
                            }
                            else if(system_flag_table->myunit.H_W == 2)
                            {
                                if(gpsx->speed<(system_flag_table->guji_record.by_speed_vaule_2*1852))
                                return;
                            }
        
                            tatol_oled_time = tatol_time;
                            system_flag_table->recored_run_flag = 1;
                        }
                        return;
                    }
          		    break;
          
         }

        if(tp_distance>0.005)
     	{
    		guiji_distance += tp_distance;
    	
    		guji_tp_long_dest = tp_long;
    		guji_tp_lati_dest = tp_lati;
    		guji_latitude1_flag = gpsx->nshemi;
    		guji_longitude1_flag = gpsx->ewhemi;
            
    		tp_distacn_temp += tp_distance;
            v1000_debug("flag :%d , distance :%f,%d\r\n",system_flag_table->guji_record.by_distance_time_flag,tp,system_flag_table->guji_record.by_distance_vaule);
    		if((tp_distacn_temp > tp))
			{
			    if(guji_icon_index == 5)
				{
				    if(system_flag_table->myunit.H_W == 0)
					{
					    if(gpsx->speed/1000 < system_flag_table->guji_record.by_speed_vaule)
					         return;
					}
				    else if(system_flag_table->myunit.H_W == 1)
					{
					    if(gpsx->speed < (system_flag_table->guji_record.by_speed_vaule_1*1609))
					         return;
					}
				    else if(system_flag_table->myunit.H_W == 2)
					{
					    if(gpsx->speed<(system_flag_table->guji_record.by_speed_vaule_2*1852))
					         return;
					}

				 }
			     system_flag_table->recored_run_flag = 1;
			     tp_distacn_temp = 0;


			   }
     	    }
        }
    }
}



void save_guiji_message(u8 guji_record_type)
{
    u8 one_shot_buffer[28];
    u16 pressure16 = pressure/10;
    u8 index = 0;
    GUJI_TAG flag ;
    GUJI_DATE guji_data ;
    u8 error = 0;

//	one_shot_buffer[index++] = (u8)(Message_head_number>>24)&0xff;  // 1mb
	one_shot_buffer[index++] = (u8)((Message_head_number +1)>>16)&0xff;  // 1mb
	one_shot_buffer[index++] = (u8)((Message_head_number +1)>>8)&0xff;  // 2mb
	one_shot_buffer[index++] = (u8)((Message_head_number +1))&0xff;  // 3 mb

    flag.all = 0;
    guji_data.all = 0;

    if(guji_record_type == 'T')
    {
       flag.bitc.tag = 0;
    }
    else if(guji_record_type == 'C')
    {
       flag.bitc.tag = 1;  
    }

    if(gpsx->nshemi == 'N')
    {
       flag.bitc.ns = 0;    
    }
    else if(gpsx->nshemi == 'S')
    {
       flag.bitc.ns = 1;     
    }
    else
    {
       error = 1;
    }


    if(gpsx->ewhemi== 'E')
    {
       flag.bitc.ew = 0;    
    }
    else if(gpsx->ewhemi== 'W')
    {
       flag.bitc.ew = 1;    
    }
    else
    {
       error = 1;
    }


    if(error == 1)
        return ;
    
    one_shot_buffer[index++]  = flag.all;  // 4mb

	if(system_flag_table->gujiFormats == GUJI_FORMATS_GPX)
	{
		guji_data.bitc.year  = gpsx->utc.year-2016;
		guji_data.bitc.month = gpsx->utc.month;
		guji_data.bitc.date  = gpsx->utc.date;
		guji_data.bitc.hour  = gpsx->utc.hour;
		guji_data.bitc.min =  gpsx->utc.min;
		guji_data.bitc.sec =  gpsx->utc.sec;
	}
	else
    {
        if(gpsx->utc.sec != RTC_TimeStructure.RTC_Seconds)
        {
            check_time();
        }
        
		guji_data.bitc.year  = (RTC_DateStructure.RTC_Year-16);
		guji_data.bitc.month = RTC_DateStructure.RTC_Month;
		guji_data.bitc.date  = RTC_DateStructure.RTC_Date;
		guji_data.bitc.hour  = RTC_TimeStructure.RTC_Hours;
		guji_data.bitc.min =   RTC_TimeStructure.RTC_Minutes;
		guji_data.bitc.sec =   RTC_TimeStructure.RTC_Seconds;
	}
   // v1000_debug("date :%x \r\n",guji_data.all);

  //  memcpy(one_shot_buffer+index,&guji_data.all,4);
 //   index = index+4;
	one_shot_buffer[index++]  = (u8)((guji_data.all>>24)&0xff);  // 11mb
	one_shot_buffer[index++]  = (u8)((guji_data.all>>16)&0xff);  // 12mb
	one_shot_buffer[index++]  = (u8)((guji_data.all>>8)&0xff);  // 13mb
	one_shot_buffer[index++]  = (u8)((guji_data.all)&0xff);  // 14mb
	
	one_shot_buffer[index++]  = (u8)((gpsx->latitude>>24)&0xff);  // 11mb
	one_shot_buffer[index++]  = (u8)((gpsx->latitude>>16)&0xff);  // 12mb
	one_shot_buffer[index++]  = (u8)((gpsx->latitude>>8)&0xff);  // 13mb
	one_shot_buffer[index++]  = (u8)((gpsx->latitude)&0xff);  // 14mb

	one_shot_buffer[index++]  = (u8)((gpsx->longitude>>24)&0xff);  // 15mb
	one_shot_buffer[index++]  = (u8)((gpsx->longitude>>16)&0xff);  // 16mb
	one_shot_buffer[index++]  = (u8)((gpsx->longitude>>8)&0xff);  // 17mb
	one_shot_buffer[index++]  = (u8)((gpsx->longitude)&0xff);  // 18mb

	one_shot_buffer[index++]  = (u8)((Altitude>>24)&0xff);  // 15mb
	one_shot_buffer[index++]  = (u8)((Altitude>>16)&0xff);  // 16mb
	one_shot_buffer[index++]  = (u8)((Altitude>>8)&0xff);  // 21mb
	one_shot_buffer[index++]  = (u8)((Altitude)&0xff);  // 22mb

	one_shot_buffer[index++]  = (u8)(((gpsx->speed/100)>>8)&0xff);  // 23mb
	one_shot_buffer[index++]  = (u8)(((gpsx->speed/100))&0xff);  // 24mb

	one_shot_buffer[index++]  = (u8)(((gpsx->angle/1000)>>8)&0xff);  // 25mb
	one_shot_buffer[index++]  = (u8)((gpsx->angle/1000)&0xff);  // 26mb


	one_shot_buffer[index++]  = (u8)(pressure16>>8)&0xff;  // 29mb
	one_shot_buffer[index++]  = (u8)(pressure16)&0xff;  // 30mb


	one_shot_buffer[index++]  = (u8)(temperature>>8)&0xff;  // 27mb
	one_shot_buffer[index++]  = (u8)(temperature)&0xff;  // 28mb

	

	memcpy(&USART2_TX_BUF[Flash_write_buffer_Index],one_shot_buffer,28);

    Flash_write_buffer_Index  += 28;
    Message_head_number++;

}


void buffer_Analysis(u8 *buffer,u16 munber)
{
    u16 index = 0,angle = 0,i = 0;
    u8 lat_flag,lon_flag,record_type;
    u32 lat,log,message_number_index;
    s32 attiautl;
    s16 temp;
    float tp_lat =0.0,tp_lon =0.0,per=0.0,speed =0.0;
    GUJI_TAG flag ;
    GUJI_DATE guji_data ;

    do
    {
        message_number_index = (buffer[2 + index]|(buffer[1 + index]<<8)|(buffer[0 + index]<<16));
        flag.all = buffer[3 + index];
       // memcpy(&guji_data.all,buffer+index+4,4);
        guji_data.all = (buffer[7 + index]|(buffer[6 + index]<<8)|(buffer[5 + index]<<16)|(buffer[4 + index]<<24));
        lat= (buffer[11 + index]|(buffer[10 + index]<<8)|(buffer[9 + index]<<16)|(buffer[8 + index]<<24));	    	

        log = (buffer[15 + index]|(buffer[14 + index]<<8)|(buffer[13 + index]<<16)|(buffer[12 + index]<<24));	    	

        attiautl = (buffer[19 + index]|(buffer[18 + index]<<8)|(buffer[17+index]<<16)|(buffer[16+index]<<24));

        speed = (buffer[21 + index]|(buffer[20 + index]<<8));

        angle=  (buffer[23 + index]|(buffer[22 + index]<<8));

        per =  (buffer[25 + index]|(buffer[24 + index]<<8));		

        temp =   (buffer[27 + index]|(buffer[26+ index]<<8));

        index = index+28;
        tp_lat = lat;
        tp_lon = log;

        record_type = flag.bitc.tag ? 'C':'T';
        lat_flag = flag.bitc.ns ? 'S':'N';
        lon_flag = flag.bitc.ew ? 'W':'E';

        if(angle >360)
             angle = 0;
        
        if(system_flag_table->gujiFormats == GUJI_FORMATS_GPX)
        {

            /*
            <trkpt lat="26.098903" lon="119.269088"><ele>149</ele>
            <time>2014-07-22T01:49:28Z</time></trkpt>
            */  
            //f_printf(&sys_fp,"<trkseg>\n");
			if(lat_flag == 'S')
		    {
				sprintf((char *)dtbuf,"<trkpt lat=\"-%.6f\"",tp_lat/1000000);	

		    
		    }
			else 
		    {
				sprintf((char *)dtbuf,"<trkpt lat=\"%.6f\"",tp_lat/1000000);	

		    }


            f_printf(&sys_fp,"%s",(char *)dtbuf);
			if(lon_flag == 'W')
		    {
				sprintf((char *)dtbuf," lon=\"-%.6f\">",tp_lon/1000000);	
		    
		    }
			else 
		    {
		        sprintf((char *)dtbuf," lon=\"%.6f\">",tp_lon/1000000);	
		    }
		    f_printf(&sys_fp,"%s",(char *)dtbuf);
            sprintf((char *)dtbuf,"<ele>%d</ele>",attiautl/10);	
            f_printf(&sys_fp,"%s\n",(char *)dtbuf);
            sprintf((char *)dtbuf,"<time>%04d-%02d-%02dT%02d:%02d:%02dZ</time></trkpt>",
               guji_data.bitc.year+2016,guji_data.bitc.month,guji_data.bitc.date,
               guji_data.bitc.hour,guji_data.bitc.min,guji_data.bitc.sec);   
            f_printf(&sys_fp,"%s\n",(char *)dtbuf);
        }
        else
        {
            if(message_number_index == 1)
            {
                if(FR_OK  == sys_fr)
                    f_printf(&sys_fp,"INDEX,TAG,DATE,TIME,LATITUDE N/S,LONGITUDE E/W,HEIGHT,SPEED,HEADING,PRES,TEMP\n");
            }
            
            if(FR_OK  == sys_fr)
            {
                sprintf((char *)dtbuf,"%d,%c,%02d%02d%02d,",message_number_index,record_type,
                    guji_data.bitc.year+16,guji_data.bitc.month,guji_data.bitc.date);
                
                f_printf(&sys_fp,"%s",(char *)dtbuf);
                sprintf((char *)dtbuf,"%02d%02d%02d,%.6f%c,",guji_data.bitc.hour,
                guji_data.bitc.min,guji_data.bitc.sec,tp_lat/1000000,lat_flag);
        
                f_printf(&sys_fp,"%s",(char *)dtbuf);
                sprintf((char *)dtbuf,"%.6f%c,%d,%.1f,%d,%.1f,%d",tp_lon/1000000,lon_flag,attiautl/10,(speed/10),angle,per/10,(int)(temp/10));	
                f_printf(&sys_fp,"%s \n",(char *)dtbuf);
    
            }
        }
        //v1000_debug("\r\n index :%d  munber= %d\r\n ",index,munber);
    }while (index <munber);	
}



void gpx_filehead_write(char* filename)
{
 /*
    <?xml version="1.0" encoding="UTF-8"?>
    <gpx
    version="1.1"
    creator="Columbus GPS - http://cbgps.com/"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns="http://www.topografix.com/GPX/1/1"
    xsi:schemaLocation="http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd">
    <trk>
    <name>21174928.gpx</name>
    <trkseg>
 
 */
    f_printf(&sys_fp,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    f_printf(&sys_fp,"<gpx\n");
    f_printf(&sys_fp,"version=\"1.1\"\n");
    f_printf(&sys_fp,"creator=\"Columbus GPS - http://cbgps.com/\"\n");
    f_printf(&sys_fp,"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n");
    f_printf(&sys_fp,"xmlns=\"http://www.topografix.com/GPX/1/1\"\n");
    f_printf(&sys_fp,"xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd\">\n");
    f_printf(&sys_fp,"<trk>\n");
    f_printf(&sys_fp,"<name>%s</name>\n",filename);
    f_printf(&sys_fp,"<trkseg>\n");

}

void write_flash(void)  /*write to  the file by true*/
{
//    int i = 0;
    int wb = 0;
    if(Flash_write_buffer_Index)
    {
      //  if((Flash_Index >= (MOST_FLASH_ADDRES-Flash_write_buffer_Index)))
        show_levev_space();
        if(system_flag_table->baifenbi < 1)
        {
            if(system_flag_table->guji_record.recoed_meth == 0)
            {
                Flash_Index = 0;
                //Flash_full_flag = 1;
//                stm_write_eerpom(MENU_FLASH_FULL_FLAG ,Flash_full_flag);
                Del_oldfile_nospace();
                
            }
            else
            {
                guji_mode = RECORED_STOP;
				Flash_write_buffer_Index = 0;
            }
            //Flash_full_flag = 1;
            return;
        }

#ifdef NO_FILESYSTEM     
        SPI_Flash_Write(USART2_TX_BUF,Flash_Index, Flash_write_buffer_Index);      
#else
        // v1000_debug("\r\n buffer_Analysis \r\n ");
        if(system_flag_table->gujiFormats == GUJI_FORMATS_CSV)
        {
            buffer_Analysis(USART2_TX_BUF,Flash_write_buffer_Index);
        }
        else if(system_flag_table->gujiFormats == GUJI_FORMATS_GPS)
        {

            f_write(&sys_fp,USART2_TX_BUF,Flash_write_buffer_Index,&wb);

        }
        else if(system_flag_table->gujiFormats == GUJI_FORMATS_GPX)
        {
            //todo:add gpx file conote
            buffer_Analysis(USART2_TX_BUF,Flash_write_buffer_Index);
        }

#endif        
       
        Flash_Index += Flash_write_buffer_Index;
        stm_write_eerpom(CUURENT_FLASH_ADDRER ,Flash_Index);
        Flash_write_buffer_Index = 0;
    }
}

	


void Recording_guji(void)
{
	u32 interst_pos_number;
	DIR* dp = NULL;

    char   track_file[26] ={0};
    FRESULT fr;
    UINT wb;
    //v1000_debug("recored type :%d \r\n",guji_mode);

	switch(guji_mode )
    {
		case RECORED_IDLE:

			break;
		case RECORED_START:
			Message_head_number = 0;
            // v1000_debug("fixmode :%d ��latitude %d,longitude %d\r\n",gpsx->fixmode,gpsx->latitude,gpsx->longitude);
			if((gpsx->fixmode >= 2)&&(gpsx->latitude >0)&&(gpsx->longitude>0))
			{

				guji_tp_long_dest = gpsx->longitude;
				guji_tp_lati_dest = gpsx->latitude;
				guji_tp_long_dest /= 1000000;
				guji_tp_lati_dest /= 1000000;
				guji_latitude1_flag = 	gpsx->nshemi;
				guji_longitude1_flag = gpsx->ewhemi;
				Flash_write_buffer_Index = 0;
                check_time();  
                //v1000_debug("w_year :%d \r\n",my_timer.w_year);
                if(system_flag_table->baifenbi == 0)
                {

				    if(1)//system_flag_table->guji_record.recoed_meth == 1)
                    {
					    guji_mode = RECORED_IDLE;
					    return ;
				    }
					else
				    {
				        Del_oldfile_nospace();			
						if(guji_mode == RECORED_STOP)
					    {
						    guji_mode = RECORED_IDLE;
						    return ;
					    }
				    }					 
                }
	
				
                if(my_timer.w_year >= 15)
                {
                    sprintf(track_file,"%04d-%02d",RTC_DateStructure.RTC_Year + 2000,RTC_DateStructure.RTC_Month); 
                    v1000_debug("track_file :%s \r\n",track_file);
                    fr= f_opendir(dp,track_file);
                    if((FR_OK  == fr) && (FR_EXIST == fr)) 
                    {
                        v1000_debug("track file dir exist %d\r\n",fr);
                        //return;
                    }
					else  
				    {
				        fr = f_mkdir(track_file);
				    }
                    
                    v1000_debug("track file dir fr %d\r\n",fr);
                    
                    if(system_flag_table->guji_record.recoed_formats == GUJI_FORMATS_CSV)
                    {
                        system_flag_table->gujiFormats = GUJI_FORMATS_CSV;
                        sprintf(track_file,"%04d-%02d/%02d%02d%02d%02d.CSV",RTC_DateStructure.RTC_Year+2000,RTC_DateStructure.RTC_Month,RTC_DateStructure.RTC_Date,
                        RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds);
                        
                    }
                    else if(system_flag_table->guji_record.recoed_formats == GUJI_FORMATS_GPS)  
                    {
                        system_flag_table->gujiFormats = GUJI_FORMATS_GPS;
                        sprintf(track_file,"%04d-%02d/%02d%02d%02d%02d.GPS",RTC_DateStructure.RTC_Year+2000,RTC_DateStructure.RTC_Month,RTC_DateStructure.RTC_Date,
                        RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds);
                    }
                    else if(system_flag_table->guji_record.recoed_formats == GUJI_FORMATS_GPX)  
                    {
                        system_flag_table->gujiFormats = GUJI_FORMATS_GPX;
                        sprintf(track_file,"%04d-%02d/%02d%02d%02d%02d.GPX",RTC_DateStructure.RTC_Year+2000,RTC_DateStructure.RTC_Month,RTC_DateStructure.RTC_Date,
                        RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds);
                    }                    
                    v1000_debug("\r\n track_file :%s \r\n ",track_file);
                    sys_fr = open_append(&sys_fp, track_file);
                    
                    if(FR_OK  != sys_fr)
                    {
                        v1000_debug("open append faild \n");
                        return; 
                    }
                    if(system_flag_table->guji_record.recoed_formats == GUJI_FORMATS_GPS) 
                    {
                        track_file[0] = 0x07;
                        track_file[1] = 0x07;
                        f_write(&sys_fp,track_file,2,&wb);
                    }
                    else if(system_flag_table->guji_record.recoed_formats == GUJI_FORMATS_GPX)
                    {
                        //todo: add gpx file head.
                        gpx_filehead_write(track_file);
                    }
                    
        			save_guiji_message('T');
        			tatol_oled_time = tatol_time;
        			guji_mode = RECORED_START_DOING;
        			interst_pos_number = 0;
        			//	sprintf((char *)dtbuf,"%d",interst_pos_number);		    		//�õ��ٶ��ַ���
        			//	OLED_ShowString(17,32,dtbuf);
        			stm_write_eerpom(GUJI_INTERST_POS,interst_pos_number);
               }
               else
                   system_flag_table->recored_run_flag = 1;
			}
			else
			{
			    system_flag_table->recored_run_flag = 1;
			}

			break;
		case RECORED_START_DOING:
			if((gpsx->fixmode >= 2)&&(gpsx->latitude >0)&&(gpsx->longitude>0))
			{
				save_guiji_message('T');
			}
			else
			{
			}
			break;
		case RECORED_T:
			save_guiji_message('C');
			stm_read_eerpom(GUJI_INTERST_POS ,&interst_pos_number);
			//if(interst_pos_number>100)
			//	interst_pos_number = 0;
			interst_pos_number++;
    		//sprintf((char *)dtbuf,"%02d",interst_pos_number);		    		//�õ��ٶ��ַ���
    		//OLED_ShowString(18,32,dtbuf);
			stm_write_eerpom(GUJI_INTERST_POS,interst_pos_number);
			guji_mode = RECORED_START_DOING;
			break;
		case RECORED_STOP:

			if(system_flag_table->baifenbi > 0)
		    {
              	if((gpsx->fixmode >= 2)&&(gpsx->latitude >0)&&(gpsx->longitude>0))
              	save_guiji_message('T');              
              	write_flash();              
              	stm_write_eerpom(CUURENT_FLASH_ADDRER ,Flash_Index);
                v1000_debug("\r\n close file \r\n ");
			}
			
			guji_mode = RECORED_IDLE;
            if(FR_OK == sys_fr)
            {
                if(system_flag_table->gujiFormats == GUJI_FORMATS_GPX)
                {
                    f_printf(&sys_fp,"</trkseg>\n");
                    f_printf(&sys_fp,"</trk>\n");
                    f_printf(&sys_fp,"</gpx>\n");

                }
                f_close(&sys_fp);
            }

			break;

	}
}

void menu_show_gujIcon(u8 icon)
{

    switch(icon)
	{
    	case 0:
    		Movie_Show_Img(0,16,Bmp_gujiiconsetp);
    		break;
    	case 1:
    		Movie_Show_Img(0,16,Bmp_gujiiconrun);
    		break;
    	case 2:
    		Movie_Show_Img(0,16,Bmp_gujiiconB);
    		break;
    	case 3:
    	    Movie_Show_Img(0,16,Bmp_gujiiconcar);
    		break;
    	case 4:
    		Movie_Show_Img(0,16,Bmp_gujiiconfly);
    		break;
    	case 5:
    		Movie_Show_Img(0,16,Bmp_gujiiconCustom);
		    break;

	}

//MENU_GUJI_MODE_INDEX_ADDRES
}


void menu_show_gujiword(u8 icon)
{
    switch(icon)
	{
	case 0:
        if(system_flag_table->lang == 1)
			 OLED_ShowString(32,16,"Walking");
		else
			Movie_Show_Img(32,16,Bmp_wordbuxing);
		break;
	case 1:
       if(system_flag_table->lang == 1)
			 OLED_ShowString(32,16,"Running");
		else
			Movie_Show_Img(32,16,Bmp_wordrun);
		break;
	case 2:
        if(system_flag_table->lang == 1)
			 OLED_ShowString(32,16,"Cycling");
		else
			Movie_Show_Img(32,16,Bmp_wordB);
		break;
	case 3:
        if(system_flag_table->lang == 1)
			 OLED_ShowString(32,16,"By car");
		else
		      Movie_Show_Img(32,16,Bmp_wordCar);
		break;
	case 4:
        if(system_flag_table->lang == 1)
			 OLED_ShowString(32,16,"By plane");
		else
			Movie_Show_Img(32,16,Bmp_feixing);
		break;
	case 5:
        if(system_flag_table->lang == 1)
			 OLED_ShowString(32,16,"User Defined");
		else
			Movie_Show_Img(32,16,Bmp_wordcustom);
		break;

	}
}

#define MAN_PARPAM 1.02
#define WOMAN_PARPAM 0.99
void menu_guiji_display(void)
{
	u8 d,h,m,s;
//	u32 distance;
	u32 tp = 0	;
	u16 diff_weght = 0;
    //u16 diff_hight = 0;

	OLED_Fill(0, 0, 127, 63, 0);
	menu_status_dispplay();

    if((enter_perss != 1)||(guji_mode != RECORED_IDLE))
    {
        if(system_flag_table->lang == 1)
            OLED_ShowString(0,32,"TOT");
        else
            Movie_Show_Img(0,32,Bmp_leiji);
        //distance	 = (floor)(guiji_distance);

        {
            //	sprintf((char *)dtbuf,"%dkm",distance);		    		//�õ��ٶ��ַ���
            if(system_flag_table->myunit.H_W == 0)
            {
                if(guiji_distance>1.0)
                    sprintf((char *)dtbuf,"%.2fkm",guiji_distance);		    		//�õ��ٶ��ַ���
                else
                    sprintf((char *)dtbuf,"%dm",((u32)(guiji_distance*1000)));
            }
            else if(system_flag_table->myunit.H_W == 1)
            {
                if((guiji_distance/1.609)>1.0)
                    sprintf((char *)dtbuf,"%.2fmi",guiji_distance/1.609);
                else
                    sprintf((char *)dtbuf,"%dft",((u32)(guiji_distance*1000/0.3048)));		    		//�õ��ٶ��ַ���
            //�õ��ٶ��ַ���
            }
        
            else if(system_flag_table->myunit.H_W == 2)
            {
                if((guiji_distance/1.852)>1.0)
                    sprintf((char *)dtbuf,"%.2fkt",guiji_distance/1.852);		    		//�õ��ٶ��ַ���
                else
                    sprintf((char *)dtbuf,"%dft",((u32)(guiji_distance*1000/0.3048)));		    		//�õ��ٶ��ַ���
            }
            OLED_ShowString(0,48,dtbuf);
        }

        d = tatol_time/(3600*24);
        h = (tatol_time - (d*24*3600))/3600;
        m = (tatol_time - (d*24*3600) - h*3600)/60;
        if(d>0)
            sprintf((char *)dtbuf,"%02dd%02dh",d,h);
        else if(d == 0)
        {
            if(h > 0)
                sprintf((char *)dtbuf,"%02dh%02dm",h,m);
            else if ( h == 0)
            {
                s = (tatol_time - (d*24*3600) - h*3600) -(60*m); 
                sprintf((char *)dtbuf,"%02dm%02ds",m,s);
            }
        }


                   //�õ��ٶ��ַ���
        OLED_ShowString(80,48,dtbuf);
        if(guji_icon_index == 0)
        {
            if(system_flag_table->myunit.H_W == 0)
                tp =(u32)( (guiji_distance*100000)/(system_flag_table->personal_da.height*0.41));
            else
                tp =(u32)( (guiji_distance*100000)/(system_flag_table->personal_da.height_ft*0.41));
            sprintf((char *)dtbuf,"%d",tp);		    		//�õ��ٶ��ַ���
            OLED_ShowString(80-((what_is_numbe_10(tp)*8)),32,dtbuf);
            if(system_flag_table->lang == 1)
                OLED_ShowString(80,32,"Steps");
            else
                Movie_Show_Img(80,32,Bmp_sept);
        }
        else if((guji_icon_index == 1)||(guji_icon_index == 2))
        {
            if(system_flag_table->lang == 1)
                OLED_ShowString(0,32,"PACE");
            else
                Movie_Show_Img(0,32,Bmp_peishu);
            
            if(system_flag_table->myunit.H_W == 0)
                tp = 3600000/average_speed;
            else
                tp = 5792400/average_speed;
            
            h = tp/60;
            m = tp - h*60;
            if((h > 99)||(average_speed <=1000))
            {
                h = 99;
                m = 99;
            }
            sprintf((char *)dtbuf,"%02d:%02d",h,m);		    		//�õ��ٶ��ַ���
            OLED_ShowString(40,32,dtbuf);
        }
        else
        {
            sprintf((char *)dtbuf,"%d",Message_head_number);		    		//�õ��ٶ��ַ���
            OLED_ShowString(80-((what_is_numbe_10(Message_head_number)*8)),32,dtbuf);
            if(system_flag_table->lang == 1)
                OLED_ShowString(80,32,"Points");
            else
                Movie_Show_Img(80,32,Bmp_dian);
        }
        
        if(guji_icon_index <= 2)
        {
            if(((guji_mode >= RECORED_START)&&(gpsx->fixmode == 3))||(guji_mode == RECORED_IDLE))
            {

                //tp = guiji_distance;
                if(system_flag_table->myunit.H_W == 0)
                {
                    diff_weght = system_flag_table->personal_da.weight;
                }
                else
                {
                    diff_weght = system_flag_table->personal_da.weight_ft;
                }
                switch(guji_icon_index)
                {
                    case 0:
                        if(system_flag_table->personal_da.flame == 0)
                            tp = diff_weght*guiji_distance*MAN_PARPAM*0.9;
                        else
                            tp = diff_weght*guiji_distance*WOMAN_PARPAM*0.9;
                        break;
                    case 1:
                        if(system_flag_table->personal_da.flame == 0)
                            tp = diff_weght*guiji_distance*MAN_PARPAM;
                        else
                            tp = diff_weght*guiji_distance*WOMAN_PARPAM;
                    
                        break;
                    case 2:
                        if(system_flag_table->personal_da.flame == 0)
                            tp = diff_weght*guiji_distance*MAN_PARPAM*0.5;
                        else
                            tp = diff_weght*guiji_distance*WOMAN_PARPAM*0.5;
                    
                        break;
                   
                }

                sprintf((char *)dtbuf,"%d",tp);		    		//�õ��ٶ��ַ���
                OLED_ShowString(80-((what_is_numbe_10(tp)*8)),16,dtbuf);
                if(system_flag_table->lang == 1)
                    OLED_ShowString(80,16,"Kcal");
                else
                    Movie_Show_Img(80,16,Bmp_daka);
            }
         
        }
        else
        {
            //menu_show_gujiword(guji_icon_index);       
        }

    }
    

  	if(guji_mode == RECORED_IDLE)
    {
        if(enter_perss == 1)
        {
            if(key_type == KEY_UP)
            {
                if(guji_icon_index > 0)
                    guji_icon_index --;
                else
                    guji_icon_index = 5;
                stm_write_eerpom(MENU_GUJI_MODE_INDEX_ADDRES ,guji_icon_index);

            }
            else if(key_type == KEY_DOWN)
         	{

             	if(guji_icon_index < 5)
             	 	guji_icon_index ++;
            	else
            		guji_icon_index = 0;
            	stm_write_eerpom(MENU_GUJI_MODE_INDEX_ADDRES ,guji_icon_index);
         	}
            Movie_Show_Img(16,16,Bmp_updwon);
            menu_show_gujiword(guji_icon_index);

        }
        else if(enter_perss == 2)
        {
            guiji_distance = 0 ;
            tatol_time = 0;          
            OLED_Fill(0, 16, 127, 32, 0);
            if(system_flag_table->lang == 1)
            	OLED_ShowString(32,16,"Start");
            else
            	Movie_Show_Img(32,16,Bmp_sart);
            menu_show_gujIcon(guji_icon_index);
            OLED_Refresh_Gram();
            delay_ms(1000);
            guji_mode = RECORED_START;
            system_flag_table->recored_run_flag = 1;
            enter_perss = 0;
            v1000_debug("RECORED_START :%d \r\n",system_flag_table->recored_run_flag);
        }
        else
            Movie_Show_Img(16,17,Bmp_ponit);
    }
	else if(guji_mode == RECORED_START_DOING)
    {
	     if(enter_perss == 2)
	 	 {
			guji_mode = RECORED_STOP;
			Recording_guji();
			OLED_Fill(0, 16, 127, 32, 0);
	        if(system_flag_table->lang == 1)
				OLED_ShowString(32,16,"Track Saved");
			else
				Movie_Show_Img(32,16,Bmp_savedguji);
			enter_perss = 4;		
			guiji_distance = 0;
			tatol_time = 0;
			Message_head_number = 0;
            system_flag_table->usb_standy_poweron_flag  = 0;
		 }
	 	 else if(enter_perss == 1)
	     {
	        if((gpsx->fixmode >= 2)&&(gpsx->latitude >0)&&(gpsx->longitude>0))
	        {
                guji_mode = RECORED_T;
                OLED_Fill(0, 16, 127, 32, 0);
                Movie_Show_Img(16,16,Bmp_gujiBI);
                Movie_Show_Img(0,16,Bmp_xinqudian);
                if(system_flag_table->lang == 1)
                    OLED_ShowString(32,16,"POI Marked");
                else
                    Movie_Show_Img(32,16,Bmp_xingqudiansaved);
   	 	    
                Recording_guji();
                enter_perss = 4;
	        }
            else 
            {
                enter_perss = 0;
            }
	     }
		 else if(enter_perss == 0)
		 {
			if((gpsx->fixmode >= 2 )&&(gpsx->latitude >0)&&(gpsx->longitude>0))
		    {
				Movie_Show_Img(16,16,Bmp_gujiBI);
			}            		
		 }

   }
   else if(guji_mode == RECORED_START)
   {
  	  if(enter_perss == 2)
      {
           guji_mode = RECORED_IDLE;
           menu_show_gujIcon(guji_icon_index);                        
           enter_perss = 0;
           v1000_debug("\n\r stop to idle");
           system_flag_table->usb_standy_poweron_flag  = 0;
  	   }
   }

   if((gpsx->fixmode < 2)&&(guji_mode != RECORED_IDLE))   	
   {
       OLED_Fill(32, 16, 127, 32, 0);
       if(system_flag_table->lang == 1)
       	   OLED_ShowString(32,16,"Locating.");
       else
           Movie_Show_Img(32,16,Bmp_waitsinge);
   }
    
   if(enter_perss != 4)
   {
       menu_show_gujIcon(guji_icon_index);
   }
   
   if(guji_mode >= RECORED_START_DOING)
       menu_show_gujIcon_Status(guji_icon_index);
 //  v1000_debug("\n\r menu_guiji_display  guji_mode:%d  Flash_Index %d ",guji_mode,Flash_Index);
}

int16_t Max_arry(int16_t* prt,u8 index)
{
    u8 cnt;
    int16_t Max = 0;
    if(index>0)
    	Max=prt[95];
    for(cnt=96-index;cnt<96;cnt++)
    {
         if(prt[cnt]>=Max) Max=prt[cnt];
    }
     return Max;
}

int16_t Min_arry(int16_t* prt,u8 index)
{
    u8 cnt;
    int16_t min = 0;
    
    if(index>0)
    	min=prt[95];
    for(cnt=96-index;cnt<96;cnt++)
    {
         if(prt[cnt]<=min) min=prt[cnt];
    }
     return min;
}

s32 Max_arry32(s32* prt,u8 index)
{
    u8 cnt;
    s32 Max = 0;
    
    if(index>0)
    	Max=prt[95];
    for(cnt=96-index;cnt<96;cnt++)
    {
         if(prt[cnt]>=Max) Max=prt[cnt];
    }
     return Max;
}

s32 Min_arry32(s32* prt,u8 index)
{
    u8 cnt;
    s32 min = 0;
    
    if(index>0)
    	min=prt[95];
    for(cnt=96-index;cnt<96;cnt++)
    {
         if(prt[cnt]<=min) min=prt[cnt];
    }
     return min;
    }

#if 0
void analytical_press_baoyu(void)
{
	switch(baoyun_flag_cnt)
		{
		case 15:
			#if 0
			    if((pressure_pass[15]>=(pressure_pass[0]-400))||(pressure_pass[15]>=(pressure_pass[1]-400))\
					||(pressure_pass[15]>=(pressure_pass[2]-400))||(pressure_pass[15]>=(pressure_pass[3]-400)))
					baoyun_flag = 1;

			break;
			#endif
		case 31:
		case 47:
		case 63:
		case 79:
		case 95:
			    if((pressure_pass[95]>=(pressure_pass[80]+400))||(pressure_pass[95]>=(pressure_pass[81]+400))\
					||(pressure_pass[95]>=(pressure_pass[82]+400))||(pressure_pass[95]>=(pressure_pass[83]+400)))
			    		{
					if(system_flag_table->stormalram==1)
						baoyun_flag = 1;
			    		}

			break;

		}
}
#endif

void save_tsp_tp(void)
{

 int i;

#ifdef NO_BMP085
	pressure= 0;
	temperature = 0;
#endif

        v1000_debug("temp:%d \r\n",temperature);

        if(old_div_cnt <95)
        old_div_cnt ++;
        else if(old_div_cnt  == 0xff)
        old_div_cnt  = 0;




        for(i= 0;i<95;i++)
        {
             temperature_pass[i] = temperature_pass[i+1];
            // v1000_debug("\n\r temperature_pass[%d] :%d ",i,temperature_pass[i] );
        }
        
        temperature_pass[95] = temperature;
        most_temperature = Max_arry(temperature_pass,old_div_cnt);
        lest_temperature = Min_arry(temperature_pass,old_div_cnt);

        for(i= 0;i<95;i++)
        {
            pressure_pass[i] = pressure_pass[i+1];
        }
        
        pressure_pass[95] = pressure;
        most_pressure= Max_arry32(pressure_pass,old_div_cnt);
        lest_pressure= Min_arry32(pressure_pass,old_div_cnt);
        
        v1000_debug("\n\r old_div_cnt:%d ",old_div_cnt);

        if(KeyTab[keyTabIndex].MenuID ==MENU_TRANSFORMATION ||  KeyTab[keyTabIndex].MenuID ==MENU_MILEAGE)
        {
            OLED_Fill(32, 17, 125, 54, 0);
        }
        SPI_Flash_Write((u8*)&temperature_pass, CHANGE_TMPE_VAULE_SAVE_ADDRES, 192);
        SPI_Flash_Write((u8*)&pressure_pass, CHANGE_PRE_VAULE_SAVE_ADDRES, 384);
        stm_write_eerpom(CHANGE_TMPE_VAULE_INDEX_ADDRES,old_div_cnt);

        //analytical_press_baoyu();

#ifdef baoyu

        if(baoyun_flag_cnt == 95)
        {
            baoyun_flag_cnt = 79;
        }
#endif
}


void save_tsp_s(void)
{

    u8 i;

	if(system_flag_table->run_mode == 1)
	{
		if(old_spped_div_cnt < 95)
			old_spped_div_cnt ++;
		if(old_spped_div_cnt  == 0xff)
			old_spped_div_cnt  = 0;
	}

	if(old_altiude_div_cnt < 95)
		old_altiude_div_cnt ++;
	if(old_altiude_div_cnt  == 0xff)
		old_altiude_div_cnt  = 0;

	for(i= 0;i<95;i++)
	{
		if(system_flag_table->run_mode == 1)
			speed_pass[i] = speed_pass[i+1];
		Altitude_pass[i] = Altitude_pass[i+1];
	}

	if(system_flag_table->run_mode == 1)
	{
		speed_pass[95] = (u16)(average_speed /1000);
		average_speed = 0xffff;
		most_speed = Max_arry(speed_pass,old_spped_div_cnt);
		lest_speed = Min_arry(speed_pass,old_spped_div_cnt);
	}

	Altitude_pass[95] = Altitude;
	//Altitude = 0xffffffff;
	most_Altitude = Max_arry32(Altitude_pass,old_altiude_div_cnt);
	lest_Altitude = Min_arry32(Altitude_pass,old_altiude_div_cnt);

  	v1000_debug("\n\r Altitude %d  speed_pass[95] :%d ",Altitude,speed_pass[95] );
  	v1000_debug("\n\r old_altiude_div_cnt %d  old_spped_div_cnt:%d ",old_altiude_div_cnt,old_spped_div_cnt);
	if(system_flag_table->run_mode == 1)
  	{
	 	SPI_Flash_Write((u8*)&speed_pass, CHANGE_SPEED_VAULE_SAVE_ADDRES, 192);
	 	stm_write_eerpom(CHANGE_SPEED_VAULE_INDEX_ADDRES,old_spped_div_cnt);
  	}
//	 memcpy(tsp_S_buffer,(u8 *)&most_Altitude,384);
	SPI_Flash_Write((u8*)&Altitude_pass, CHANGE_ALTITUD_VAULE_SAVE_ADDRES, 384);
//		 SPI_Flash_Write(&old_altiude_div_cnt, CHANGE_ALTITUD_VAULE_INDEX_ADDRES, 1);
	stm_write_eerpom(CHANGE_ALTITUD_VAULE_INDEX_ADDRES,old_altiude_div_cnt);



}



void is_update_to_poweron(void)
{
    u32 check_data = 0;
    stm_read_eerpom(CUURENT_BUUFER_INDEX_ADDRER ,&check_data);
    if(check_data == 0x11111111)
         menu_keyprocess(5);
    else
         stm_write_eerpom(CUURENT_BUUFER_INDEX_ADDRER ,0x22222222);
}

void menu_display_settingitem_recode(void)
{

    //u32 setting_tp;
    u8 i = 0;
//    int K = 0;

    Movie_Show_Img(1,0,BMP_RECORD_HEAD);
	if(system_flag_table->lang == 1)
	{
		OLED_ShowString(16,0,"Track Log    ");
	}
    if(menu_setting_index == MENU_SETTING_SEC_PAGE)
	{
	    if(menu_setting_item > (DELECT_ALL+1))
		    menu_setting_item = 0;
        if(menu_setting_item <= RECORED_FORMAT)
    	{

	        if(system_flag_table->lang == 1)
			{
		   	  OLED_ShowString12(16,16,"User Defined");
		   	  OLED_ShowString12(16,28,"Auto Overwrite");
		   	  OLED_ShowString12(16,40,"Auto Start");
		   	  OLED_ShowString12(16,52,"Formats");

			}
	    	else
			{
			Movie_Show_Img(16,16,bmp_CustomRecored);
			Movie_Show_Img(16,29,bmp_jilufashi);
			Movie_Show_Img(16,40,bmp_autoreocrd);
            Movie_Show_Img(16,52,Bmp_fontguji);
            Movie_Show_Img(40,52,Bmp_fontformats);

			}
	    	Movie_Show_Img(4,(menu_setting_item*12+16),BMP_CHOOSE1);
  	    }
        else
    	{
  		    if(system_flag_table->lang == 1)
			{
			  OLED_ShowString12(16,16,"Delete All Tracks");
		   	  OLED_ShowString12(16,28,"Return");
			}
		    else
			{
			    Movie_Show_Img(16,17,bmp_delect_record_word);
			    Movie_Show_Img(16,28,BMP_SETTING_RETURN);
			}
			Movie_Show_Img(4,((menu_setting_item -DELECT_ALL)*12+16),BMP_CHOOSE1);


    	}
    }



	switch(menu_setting_item)
	{
	    case CUSTOM_RECORED:

            if(menu_setting_index == MENU_SETTING_THR_PAGE)
            {
                if(system_flag_table->lang == 1)
                {
                    OLED_ShowString12(16,16,"User Defined");
                }
                else
                    Movie_Show_Img(16,16,bmp_CustomRecored);
                if(menu_setting_postion>RECORED_MODEL_OPTIONS_RETURN)
                   menu_setting_postion =0;
 
 			    if(menu_setting_postion<=BY_SPEED)
 			    {
		  		    if(system_flag_table->lang == 1)
					{
    					OLED_ShowString12(16,28,"By Distance");
    					OLED_ShowString12(16,40,"By Time");
    					OLED_ShowString12(16,52,"Speed Mask");
					}
			    	else
					{
    					Movie_Show_Img(16,28,BMP_BYDICTANCE);
    					Movie_Show_Img(16,40,BMP_BYtime);
    					Movie_Show_Img(16,52,BMP_BYspeed);
					}
					Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE1);
				}
			    else if(menu_setting_postion<=RECORED_MODEL_OPTIONS_RETURN)
				{
					Movie_Show_Img(4,28,BMP_CHOOSE1);
					if(system_flag_table->lang == 1)
				    {
						OLED_ShowString12(16,28,"Return");
					}
					else
						Movie_Show_Img(16,28,BMP_SETTING_RETURN);


				}

			    Movie_Show_Img(2,16,BMP_CHOOSE3);

            }

		    if(menu_setting_index == MENU_SETTING_FOR_PAGE)
		    {
    			Movie_Show_Img(2,16,BMP_CHOOSE3);
    			stm_read_eerpom((MENU_SETTING_GUJI_BY_DISTANCE+menu_setting_postion) ,&setting_tp);
    			if(setting_tp>1000)
    				setting_tp =0 ;

			    switch(menu_setting_postion)
				{
				    case BY_DISTANCE:
       			        if(system_flag_table->lang == 1)
						{
    						OLED_ShowString12(16,16,"By Distance");
  
						}
					    else
					    	Movie_Show_Img(16,16,BMP_BYDICTANCE);

                        if(menu_setting_postion_2 >4)
						    menu_setting_postion_2 = 0;

					    if(system_flag_table->myunit.H_W == 0)
						{
						    if(menu_setting_postion_2<=2)
							{
						    	for(i =0;i<3;i++)
								{
    							//	system_flag_table->guji_record.by_distance_vaule= arry_bydistance[i];
    								sprintf((char *)dtbuf,"%dm",arry_bydistance[i]);		    		//�õ��ٶ��ַ���
    								OLED_ShowString12(16,(29+(i*12)),dtbuf);
								}
							}
						    else
							{
						    	for(i =3;i<5;i++)
								{
    								//system_flag_table->guji_record.by_distance_vaule= arry_bydistance[i];
    								sprintf((char *)dtbuf,"%dm",arry_bydistance[i]);		    		//�õ��ٶ��ַ���
    								OLED_ShowString12(16,(29+((i-3)*12)),dtbuf);
								}
							}
						}
					    else
						{
						    if(menu_setting_postion_2<=2)
							{
						    	for(i =0;i<3;i++)
								{
    								//system_flag_table->guji_record.by_distance_vaule = arry_bydistance_2[i];
    								sprintf((char *)dtbuf,"%dft",arry_bydistance_2[i]);		    		//�õ��ٶ��ַ���
    								OLED_ShowString12(16,(29+(i*12)),dtbuf);
								}
							}
					    	else
							{
							    for(i =3;i<5;i++)
								{
    
    								sprintf((char *)dtbuf,"%dft",arry_bydistance_2[i]);		    		//�õ��ٶ��ַ���
    								OLED_ShowString12(16,(29+((i-3)*12)),dtbuf);
								}
							}
						}

					    if(key_type == KEY_ENTER)
						{
						    setting_tp = menu_setting_postion_2;
    						if(system_flag_table->myunit.H_W >= 1)
    							system_flag_table->guji_record.by_distance_vaule_ft= arry_bydistance_2[setting_tp];
    						else
    							system_flag_table->guji_record.by_distance_vaule = arry_bydistance[setting_tp];
    
    						stm_write_eerpom(MENU_SETTING_GUJI_BY_DISTANCE ,setting_tp);
    						stm_write_eerpom(MENU_SETTING_GUJI_BY_MODE ,0);
    						system_flag_table->guji_record.by_distance_time_flag= 0;
    						enter_perss = 4;
						}

					    break;
				    case BY_TIMES:
		                if(menu_setting_postion_2 > 4)
    					   menu_setting_postion_2 = 0;
//						stm_read_eerpom(MENU_SETTING_GUJI_BY_TIME ,&setting_tp);
  					    if(system_flag_table->lang == 1)
						{
							OLED_ShowString12(16,16,"By Time");
						}
						else
							Movie_Show_Img(16,16,BMP_BYtime);
						if(menu_setting_postion_2<=2)
						{
							for(i =0;i<3;i++)
							{
							//	system_flag_table->guji_record.by_time_vaule= arry_time[i];
								sprintf((char *)dtbuf,"%ds",arry_time[i]);		    		//�õ��ٶ��ַ���
								OLED_ShowString12(16,(29+(i*12)),dtbuf);
							}
						}
						else
						{
							for(i =3;i<5;i++)
							{
							//	system_flag_table->guji_record.by_time_vaule = arry_time[i];
								sprintf((char *)dtbuf,"%ds",arry_time[i]);		    		//�õ��ٶ��ַ���
								OLED_ShowString12(16,(29+((i-3)*12)),dtbuf);
							}
						}

						if(key_type == KEY_ENTER)
						{
							setting_tp = menu_setting_postion_2;
							system_flag_table->guji_record.by_time_vaule= arry_time[setting_tp];
							stm_write_eerpom(MENU_SETTING_GUJI_BY_TIME,setting_tp);
							enter_perss = 4;
							stm_write_eerpom(MENU_SETTING_GUJI_BY_MODE ,1);
							system_flag_table->guji_record.by_distance_time_flag= 1;

						}
					    break;
				    case BY_SPEED:
					    if(system_flag_table->lang == 1)
					    {
							OLED_ShowString12(16,16,"Speed Mask");
					    }
					    else
							 Movie_Show_Img(16,16,BMP_BYspeed);
						Movie_Show_Img(0,26,Bmp_updwon);
						menu_updown_flag = 1;
						//setting_tp = menu_setting_postion_2;
						#if 1
						if(system_flag_table->myunit.H_W == 0)
						{
							sprintf((char *)dtbuf,"%dkm",menu_setting_postion_2);		    		//�õ��ٶ��ַ���
						}
						else if(system_flag_table->myunit.H_W == 1)
						{
							sprintf((char *)dtbuf,"%dmi",menu_setting_postion_2);		    		//�õ��ٶ��ַ���
						}
						else
						{
							sprintf((char *)dtbuf,"%dkt",menu_setting_postion_2);		    		//�õ��ٶ��ַ���
						}
						#endif
			
						OLED_ShowString12(16,29,dtbuf);
						if(key_type == KEY_ENTER)
						{
							 if(system_flag_table->myunit.H_W == 0)
							 {
								system_flag_table->guji_record.by_speed_vaule = menu_setting_postion_2;
								stm_write_eerpom(MENU_SETTING_GUJI_BY_SPEED,menu_setting_postion_2);
							 }
							 else if(system_flag_table->myunit.H_W == 1)
							 {
								system_flag_table->guji_record.by_speed_vaule_1 = menu_setting_postion_2;
								stm_write_eerpom(MENU_SETTING_GUJI_BY_SPEED_1,menu_setting_postion_2);

							 }
							 else
							 {
								system_flag_table->guji_record.by_speed_vaule_2 = menu_setting_postion_2;
								stm_write_eerpom(MENU_SETTING_GUJI_BY_SPEED_2,menu_setting_postion_2);

							 }

							 enter_perss = 4;
						}
					    break;
				}
			    if(menu_setting_postion != BY_SPEED)
		        {
                    if(menu_setting_postion_2<3)
					    Movie_Show_Img(4,(menu_setting_postion_2*12+28),BMP_CHOOSE2);
				    else
				    	Movie_Show_Img(4,((menu_setting_postion_2-3)*12+28),BMP_CHOOSE2);

			        if(setting_tp<3)
     				{
         				if(menu_setting_postion_2<3)
         					Movie_Show_Img(4,(setting_tp*12+28),BMP_CHOOSE1);
     
     				}
			        else
    				{
    					if(menu_setting_postion_2>=3)
    						Movie_Show_Img(4,((setting_tp-3)*12+28),BMP_CHOOSE1);
    				}


				}
		    }
			break;
		case RECORED_MODEL:
			if(menu_setting_index == MENU_SETTING_THR_PAGE)
		 	{
                if(system_flag_table->lang == 1)
                {
                    OLED_ShowString12(16,16,"Auto Overwrite");
                    OLED_ShowString12(16,28,"On");
                    OLED_ShowString12(16,40,"Off");
                }
                else
                {
                    Movie_Show_Img(16,17,bmp_jilufashi);
                    Movie_Show_Img(16,29,bmp_jilufashi_1);
                }
		 	    stm_read_eerpom(MENU_SETTING_GUJI_RECOREDMETH ,&setting_tp);

				Movie_Show_Img(2,16,BMP_CHOOSE3);
				switch(menu_setting_postion)
				{
				    case AUTO_RECOVRY:
					
					    break;
				    case AUTO_STOP:
				
					    break;
				    default:
					    menu_setting_postion = 0;
				
					break;
				}
			    if(key_type == KEY_ENTER)
				{
					setting_tp = menu_setting_postion;
					system_flag_table->guji_record.recoed_meth= menu_setting_postion;
					stm_write_eerpom(MENU_SETTING_GUJI_RECOREDMETH ,setting_tp);
					enter_perss = 4;
				}

				Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);
				Movie_Show_Img(4,(setting_tp*12+28),BMP_CHOOSE1);
			}
			break;

		case AUTO_MODEL:
			if(menu_setting_index == MENU_SETTING_THR_PAGE)
			{
			 	Movie_Show_Img(2,16,BMP_CHOOSE3);

  			    if(system_flag_table->lang == 1)
				{
    				OLED_ShowString12(16,16,"Auto Start");
    				OLED_ShowString12(16,28,"Off");
    				OLED_ShowString12(16,40,"On");
				}
        		else
    		 	{
        		 	Movie_Show_Img(16,16,bmp_autoreocrd);
        		 	Movie_Show_Img(16,29,bmp_autoreocrd_1);
    		 	}
			 	stm_read_eerpom(MENU_SETTING_RECODED_AUTOFLAG ,&setting_tp);


    			switch(menu_setting_postion)
    			{
    				case AUTO_ON:
    					//setting_tp = 0;
    					break;
    				case AUTO_OFF:
    				//	setting_tp = 1;
    					break;
    				default:
    					menu_setting_postion = 0;
    					//setting_tp = 0;
    					break;
    			}
                
				if(key_type == KEY_ENTER)
				{
					setting_tp = menu_setting_postion;
					system_flag_table->guji_record.auto_recoed_flag = menu_setting_postion;
					stm_write_eerpom(MENU_SETTING_RECODED_AUTOFLAG ,setting_tp);
					enter_perss = 4;
				}


				Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);
				Movie_Show_Img(4,(setting_tp*12+28),BMP_CHOOSE1);
			}

			break;
        case RECORED_FORMAT:
            if(menu_setting_index == MENU_SETTING_THR_PAGE)
            {
                if(menu_setting_postion > 2)
                {
                    menu_setting_postion = 0;
                }

                Movie_Show_Img(2,16,BMP_CHOOSE3);
        
                if(system_flag_table->lang == 1)
                {
                    OLED_ShowString12(16,16,"Formats");
                    OLED_ShowString12(16,28,"CSV");
                    OLED_ShowString12(16,40,"GPX");
                    OLED_ShowString12(16,52,"GPS");
                }
                else
                {
                    Movie_Show_Img(16,16,Bmp_fontguji);
                    Movie_Show_Img(40,16,Bmp_fontformats);
                    OLED_ShowString12(16,28,"CSV");
                    OLED_ShowString12(16,40,"GPX");
                    OLED_ShowString12(16,52,"GPS");

                }
                stm_read_eerpom(MENU_SETTING_RECODED_FORMATS ,&setting_tp);
        
        

                
                if(key_type == KEY_ENTER)
                {
                    setting_tp = menu_setting_postion;
                    system_flag_table->guji_record.recoed_formats = menu_setting_postion;
                    stm_write_eerpom(MENU_SETTING_RECODED_FORMATS ,setting_tp);
                    enter_perss = 4;
                }
        
        
                Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);
                Movie_Show_Img(4,(setting_tp*12+28),BMP_CHOOSE1);
            }
        
            break;

		case DELECT_ALL:
			if(menu_setting_index == MENU_SETTING_THR_PAGE)
			{
                if(system_flag_table->lang == 1)
                {
                	OLED_ShowString12(16,16,"Delete All Tracks");
                	OLED_ShowString12(16,28,"No");
                    if(USB_CONNECT == 0)
                	    OLED_ShowString12(16,40,"Yes");
                    else
                        OLED_ShowString12(16,40,"Unplug the Cable");
                }
                else
                {
                    Movie_Show_Img(0,0,bmp_guji_remove_recode);
                    if(USB_CONNECT == 1)
                        Movie_Show_Img(16,40,unplug_usb);
                                        
                }
					                 
				switch(menu_setting_postion)
				{
    				case AUTO_ON:
    					//setting_tp = 0;
    					break;
    				case AUTO_OFF:
    				//	setting_tp = 1;
    					break;
    				default:
    					menu_setting_postion = 0;
    					//setting_tp = 0;
    					break;
				}

				if(key_type == KEY_ENTER)
				{
					if(menu_setting_postion  == 1)
					{
                        Flash_Index = 0;
//                        Flash_full_flag = 0;
                        stm_write_eerpom(CUURENT_FLASH_ADDRER ,Flash_Index);
//                        stm_write_eerpom(MENU_FLASH_FULL_FLAG ,Flash_full_flag);
                        OLED_Fill(0, 28, 127, 63, 0);
                        if(system_flag_table->lang == 0)
                        {
                            Movie_Show_Img(16,29,Bmp_deleting_guji);

                        }
                        else
                        {
                             OLED_ShowString12(16,28,"Deleting");
                             menu_setting_postion = 0;
                        }

                        OLED_Refresh_Gram();

                        Del_allgujifile();

                        
				    }

				    enter_perss = 4;
				}
				Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);
				Movie_Show_Img(4,(setting_tp*12+28),BMP_CHOOSE1);
		    }
			break;
		}

}



void menu_enter_flash(u16 my_timer_flash)
{
    if(KeyTab[keyTabIndex].subnumber == 'S')
	{
	    if((my_timer_flash == 0)||(my_timer_flash == 200)||(my_timer_flash == 100))
		{
    		OLED_Fill(0, 28, 15, 63, 0);
    		if(menu_updown_flag)
    		{
    			if(menu_updown_flag == 2)
    				Movie_Show_Img(4,40,BMP_CHOOSE1);
    			else
    				Movie_Show_Img(4,28,BMP_CHOOSE1);
    		}
    		else
    		{
    
    			if(menu_setting_index == MENU_SETTING_THR_PAGE)
    			{
    				if(menu_setting_postion > 2)
    					Movie_Show_Img(4,((menu_setting_postion-3)*12+28),BMP_CHOOSE1);
    				else
    					Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE1);
    			}
    			else if (menu_setting_index == MENU_SETTING_FOR_PAGE)
    			{
    				if(menu_setting_postion_2 > 2)
    					Movie_Show_Img(4,((menu_setting_postion_2-3)*12+28),BMP_CHOOSE1);
    				else
    					Movie_Show_Img(4,(menu_setting_postion_2*12+28),BMP_CHOOSE1);
    			}
    			else
                {
                    if(menu_setting_item == FIRMWARE_UPDATE)
                        Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE1);
                    else
    				    Movie_Show_Img(4,(menu_setting_item*12+16),BMP_CHOOSE1);
                }   
    		}

		    OLED_Refresh_Gram();
	    }
	    else if((my_timer_flash == 50)||(my_timer_flash == 150))
	    {
            if(menu_setting_index == MENU_SETTING_SEC_PAGE)
              OLED_Fill(0, 16, 15, 63, 0);
            else
              OLED_Fill(0, 28, 15, 63, 0);
            OLED_Refresh_Gram();
	    }
    
    	if(my_timer_flash == 200)
        {
            if(need_to_powerdownflag == 2)
            {
                enter_perss = 0;
                menu_setting_index = 0xff;
                key_type = 0xff;
                keyTabIndex = MENU_MAIN;
                need_to_powerdownflag = 0;
            }
            else
            {
                menu_setting_index--;
                enter_perss = 1;
            }

            KeyTab[keyTabIndex].CurrentOperate();
        }
	}
}


/***ϵͳ���ø�����*******************/
void menu_setting_mainpage_control(void)
{

    //u32 setting_tp;

    menu_setting_item = 0;
    menu_updown_flag = 0;
    if(key_type == KEY_UP)
    {
        if(menu_setting_page >0)
            menu_setting_page--;
        else
            menu_setting_page = PAGE_QUIT;

    }
    else if(key_type == KEY_DOWN)
    {
        if(menu_setting_page <PAGE_QUIT)
            menu_setting_page++;
        else
            menu_setting_page = 0;
    }
    else if(key_type == KEY_ENTER)
    {
        if(menu_setting_page != PAGE_QUIT)
        {
            menu_setting_index = MENU_SETTING_SEC_PAGE;
            if(menu_setting_page == PAGE_RUN_MODEL)
            {        
                stm_read_eerpom(MENU_SETTING_RUNMODEL_ADDRES ,&setting_tp);
                menu_setting_item= setting_tp;
            }
        }
        else
        {
            menu_setting_page = PAGE_RUN_MODEL;
            menu_setting_index = 0xff;
            enter_perss = 0;
            OLED_Fill(0, 28, 16, 63, 0);
        }
        key_type = 0xff;
    }


}


void menu_setting_secpage_control(void)
{

   //u32 setting_tp;

   menu_updown_flag = 0;
   
   if(key_type == KEY_UP)
   {
       if(menu_setting_item >0)
           menu_setting_item--;
       else
       {
           switch(menu_setting_page)
           {
               case PAGE_RUN_MODEL:
                   menu_setting_item = 1;
                   break;
               case PAGE_GUJI_RECORD:
                   menu_setting_item = 5;
                   break;
               case PAGE_GESHI_DANGWEI:
                   menu_setting_item = 6;
                   break;
               case PAGE_GEREN_ZHILIAO:
                   menu_setting_item = 3;
                   break;
               case PAGE_SYSTEM_SET:
                   menu_setting_item = 9;
                   break;
           }
       }
   }
   else if(key_type == KEY_DOWN)
       menu_setting_item++;
   else if(key_type == KEY_ENTER)
   {
       setting_tp = 0;
       menu_setting_index = MENU_SETTING_THR_PAGE;
       switch(menu_setting_page)
       {
           case PAGE_RUN_MODEL:
               setting_tp = menu_setting_item;
               stm_write_eerpom(MENU_SETTING_RUNMODEL_ADDRES ,setting_tp);
               if(system_flag_table->run_mode != menu_setting_item)
               {
                   if(system_flag_table->run_mode == 1)
                   {
                       memset(gpsx,0,sizeof(nmea_msg));
                       GPS_power_ctrl(0);
                       USART_Cmd(USART2, DISABLE);
                       if(guji_mode  != RECORED_IDLE)
                       {
                           guji_mode = RECORED_STOP;
                           Recording_guji();
                       }    

                   }
                   else
                   {
                       GPS_power_ctrl(1);
                       USART_Cmd(USART2, ENABLE);
                       rtc_set_flag = 1;
                   }
               }
   
               system_flag_table->run_mode = menu_setting_item;
       
               menu_setting_index--;
               enter_perss = 4;
               break;
           case PAGE_GUJI_RECORD:
                if(menu_setting_item ==CUTOME_RECORED_RETURN )
                    menu_setting_index = MENU_SETTING_MAIN_PAGE;
                else
                {
                   menu_setting_postion = 0;
                   if(menu_setting_item ==RECORED_MODEL )
                   {
                       stm_read_eerpom(MENU_SETTING_GUJI_RECOREDMETH,&setting_tp);
                       menu_setting_postion = setting_tp;
                   }
                   else if(menu_setting_item ==AUTO_MODEL )
                   {
                       stm_read_eerpom(MENU_SETTING_RECODED_AUTOFLAG,&setting_tp);
                       menu_setting_postion = setting_tp;
                   }
                   else if(menu_setting_item ==RECORED_FORMAT )
                   {
                       stm_read_eerpom(MENU_SETTING_RECODED_FORMATS,&setting_tp);
                       menu_setting_postion = setting_tp;
                   }
                   else if(menu_setting_item ==DELECT_ALL)
                       menu_setting_postion = 0;
                   else
                       menu_setting_postion = system_flag_table->guji_record.by_distance_time_flag;                
               }
               break;
          case PAGE_GESHI_DANGWEI:
              if(menu_setting_item ==UNIT_REUTNR )
                  menu_setting_index = MENU_SETTING_MAIN_PAGE;
              else
              {
                  stm_read_eerpom(MENU_SETTING_UNIT_TIMER+ menu_setting_item,&setting_tp);
                  menu_setting_postion = setting_tp;
              }
      
              break;
          case PAGE_GEREN_ZHILIAO:
              if(menu_setting_item ==PERSONAL_REUTRN )
                  menu_setting_index = MENU_SETTING_MAIN_PAGE;
              else
              {
                  if((menu_setting_item == PERSONAL_HEIGHT)&&(system_flag_table->myunit.H_W >=1))
                  {
                     stm_read_eerpom(MENU_SETTING_HEIGHT_1,&setting_tp);
                  }
                  else if((menu_setting_item == PERSONAL_WEIGHT)&&(system_flag_table->myunit.H_W >=1))
                  {
                     stm_read_eerpom(MENU_SETTING_WEIGHT_1,&setting_tp);
                  }
                  else
                     stm_read_eerpom(MENU_SETTING_FALME+ menu_setting_item,&setting_tp);
                  menu_setting_postion = setting_tp;
              }
  
              break;
          case PAGE_SYSTEM_SET:
              if(menu_setting_item ==SYSTEM_RETURN )
                  menu_setting_index = MENU_SETTING_MAIN_PAGE;
              else
              {
                 if(menu_setting_item <SYSTEM_INFO)
                 {
                      stm_read_eerpom(MENU_SETTING_AJUST_FLAG+ menu_setting_item,&setting_tp);
                     menu_setting_postion = setting_tp;
      
                 }
                  else
                 {
                     menu_setting_postion = 0;					
                     setting_tp = 0;
					 if(menu_setting_item == FIRMWARE_UPDATE)
 				 	 {
                         menu_setting_postion = 1;					
                         setting_tp = 1;
					 }
       
                 }
              }
      
              break;
            }        
        key_type = 0xff;
    }
    

}

void menu_up_recode_limit(u8 item )
{

    switch(item)
    {
        case CUSTOM_RECORED:
            menu_setting_postion = RECORED_MODEL_OPTIONS_RETURN;
            break;
        case RECORED_MODEL:
        case AUTO_MODEL:
        case DELECT_ALL:
            menu_setting_postion = 1;
            break;
        case RECORED_FORMAT:
            menu_setting_postion = FORMATS_GPS;
            break;

    }
}


void menu_up_format_limit(u8 item )
{

    switch(item)
    {
        case UNIT_TIME:
        case UNIT_TEMP:
            menu_setting_postion = 1;
            break;
        case UNIT_DATE:
        case UNIT_COORD:
        case UNIT_DISCANCE:
            menu_setting_postion = 2;
            break;
        case UNIT_PRES:
            menu_setting_postion = 3;
            break;

    }
}


void menu_up_geren_limit(u8 item )
{

    switch(item)
    {
        case PERSONAL_FALEM:
            menu_setting_postion = 1;
            break;


    }
}

void menu_up_system_limit(u8 item )
{

    switch(item)
    {
        case SCREEN_STANBY:
            menu_setting_postion = 2;
            break;
        case POWERDONW_CONTRL:
        case SYSTEM_ADR_ADJUST:    
        case WANNER_SOUND:
        case LANGUAGE:
        case FACTROY:
        case FIRMWARE_UPDATE:
            menu_setting_postion = 1;
            break;


    }
}


void menu_setting_control()
{
    //u32 setting_tp;

    switch(menu_setting_index)
    {
        case 0xff:
            menu_setting_page = 0;
            if(key_type == KEY_ENTER)
            {
                menu_setting_index = MENU_SETTING_MAIN_PAGE;
                key_type = 0xff;
            }
            menu_updown_flag = 0;
            break;
        case MENU_SETTING_MAIN_PAGE:
            menu_setting_mainpage_control();

            break;
        case MENU_SETTING_SEC_PAGE:
            menu_setting_secpage_control();
            break;
        case MENU_SETTING_THR_PAGE:
            if(key_type == KEY_UP)
            {
                if((menu_setting_page == PAGE_GEREN_ZHILIAO)&&(menu_setting_item ==  PERSONAL_HEIGHT||menu_setting_item ==  PERSONAL_WEIGHT))
                {
                    menu_setting_postion++;
                }
                else
                {
                    if(menu_setting_postion >0)
                        menu_setting_postion--;
                    else
                    {
                        switch(menu_setting_page)
                        {

                            case PAGE_GUJI_RECORD:
                                menu_up_recode_limit(menu_setting_item);
                                break;
                            case PAGE_GESHI_DANGWEI:
                                menu_up_format_limit(menu_setting_item);
                                break;
                            case PAGE_GEREN_ZHILIAO:
                                menu_up_geren_limit(menu_setting_item);
                                break;
                            case PAGE_SYSTEM_SET:
                                menu_up_system_limit(menu_setting_item);
                                break;
                        }
                    }
                    if(((menu_setting_page == PAGE_SYSTEM_SET)&&(menu_setting_item ==  SYSTEM_INFO))||
                      ((menu_setting_page == PAGE_SYSTEM_SET)&&(menu_setting_item ==  FIRMWARE_UPDATE)&&(system_flag_table->updatefile == 0)))
                    {
                        menu_setting_postion = 0;
                        menu_setting_index = MENU_SETTING_SEC_PAGE;
                    }
                }

            
            }
            else if(key_type == KEY_DOWN)
            {
                if((menu_setting_page == PAGE_GEREN_ZHILIAO)&&(menu_setting_item ==  PERSONAL_HEIGHT
                    ||menu_setting_item ==  PERSONAL_WEIGHT))
                {
                    if(menu_setting_postion >0)
                        menu_setting_postion--;
                }
                else
                {
                    if(((menu_setting_page == PAGE_GUJI_RECORD)&&(menu_setting_item ==  DELECT_ALL)&&(USB_CONNECT == 1))
                        ||((menu_setting_page == PAGE_SYSTEM_SET)&&(menu_setting_item ==  FACTROY)&&(USB_CONNECT == 1)))
                        menu_setting_postion = 0;
                    else
                        menu_setting_postion++;
                    if(((menu_setting_page == PAGE_SYSTEM_SET)&&(menu_setting_item ==  SYSTEM_INFO))||
                      ((menu_setting_page == PAGE_SYSTEM_SET)&&(menu_setting_item ==  FIRMWARE_UPDATE)&&(system_flag_table->updatefile == 0)))
                    {
                    	menu_setting_postion = 0;
                    	menu_setting_index = MENU_SETTING_SEC_PAGE;
                    }
                }
            }
            else if(key_type == KEY_ENTER)
            {
                if((menu_setting_page == PAGE_GUJI_RECORD)&&(menu_setting_item == CUSTOM_RECORED))
                {
                    if(menu_setting_postion == RECORED_MODEL_OPTIONS_RETURN)
                    {
                        menu_setting_postion = 0;
                        menu_setting_index = MENU_SETTING_SEC_PAGE;
                    }
                    else
                    {
                        menu_setting_index = MENU_SETTING_FOR_PAGE;
                        if((menu_setting_page == PAGE_GUJI_RECORD)&&(menu_setting_item ==  CUSTOM_RECORED)
                         &&(menu_setting_postion == BY_SPEED))
                        {
                          	if(system_flag_table->myunit.H_W == 0)
                        	{
                         		stm_read_eerpom(MENU_SETTING_GUJI_BY_SPEED,&setting_tp);
                         		menu_setting_postion_2 = setting_tp;
                        	}
                          	else if(system_flag_table->myunit.H_W == 1)
                        	{
                         		stm_read_eerpom(MENU_SETTING_GUJI_BY_SPEED_1,&setting_tp);
                         		menu_setting_postion_2 = setting_tp;
                 
                        	}
                         	else
                           	{
                         		stm_read_eerpom(MENU_SETTING_GUJI_BY_SPEED_2,&setting_tp);
                         		menu_setting_postion_2 = setting_tp;
                
                        	}
                        }
                    }
                    key_type = 0xff;
                }
    
                if((menu_setting_page == PAGE_SYSTEM_SET)&&(menu_setting_item ==  SYSTEM_ADR_ADJUST)&&(menu_setting_postion == 1))
                {
                    menu_setting_index = MENU_SETTING_FOR_PAGE;
                    system_flag_table->ajust_Arr_flag  = 0;
                    ConvAltitude();
                    if(gpsx->fixmode >1)
                    {
                    	if(system_flag_table->myunit.H_W == 0)
                    		menu_setting_postion_2 = gpsx->altitude/10;
                    	else
                    		menu_setting_postion_2 = gpsx->altitude/10/0.3048;
                    }
                    else
                    {
                    	if(system_flag_table->myunit.H_W == 0)
                    		menu_setting_postion_2 = Altitude/10;
                    	else
                    		menu_setting_postion_2 = Altitude/10/0.3048;
            
                    }
                    key_type = 0xff;
                }
    
                if((menu_setting_page == PAGE_SYSTEM_SET)&&(menu_setting_item ==  SYSTEM_INFO))
                {
                    menu_setting_postion = 0;
                    menu_setting_index = MENU_SETTING_SEC_PAGE;
                }
            }
            break;
        case MENU_SETTING_FOR_PAGE:
   
            if((menu_setting_page == PAGE_GUJI_RECORD)&&
               (menu_setting_item ==  CUSTOM_RECORED)&&menu_setting_postion ==BY_SPEED )
            {
                 if(key_type == KEY_DOWN)
                 {
                     if(menu_setting_postion_2 >=5)
                         menu_setting_postion_2-=5;
                     else
                         menu_setting_postion_2 = 200;
         
                 }
                 else if(key_type == KEY_UP)
                 {
                     if(menu_setting_postion_2 <=195)
                         menu_setting_postion_2+=5;
                     else
                         menu_setting_postion_2 = 0;
         
                 }
             }
       
             else if((menu_setting_page == PAGE_SYSTEM_SET)&&(menu_setting_item ==  SYSTEM_ADR_ADJUST)&&menu_setting_index ==MENU_SETTING_FOR_PAGE )
             {
                 if(key_type == KEY_DOWN)
                 {
                     if(((menu_setting_page == PAGE_SYSTEM_SET)&&(menu_setting_item ==  SYSTEM_ADR_ADJUST)&&menu_setting_index ==MENU_SETTING_FOR_PAGE ))
                     {
                     //	if(menu_setting_postion_2)
                         menu_setting_postion_2--;
                     }
                     else
                     {
                         if(menu_setting_postion_2 >0)
                         menu_setting_postion_2--;
                     }
                 }
                 else if(key_type == KEY_UP)
                     menu_setting_postion_2++;
             }
             else
             {
                 if(key_type == KEY_UP)
                 {
                     if(menu_setting_postion_2 >0)
                         menu_setting_postion_2--;
                 }
                 else if(key_type == KEY_DOWN)
                      menu_setting_postion_2++;
             }
             break;
    }

}

void setting_index_org(void)
{
    Movie_Show_Img(0,0,BMP_SETTING_HEAD);
    
    if(system_flag_table->lang == 1)
    {
        OLED_ShowString(16,0,"SETTINGS");
    }

}

void setting_index_mainpage(void)
{

    Movie_Show_Img(0,0,BMP_SETTING_HEAD);
    
    if(system_flag_table->lang == 1)
    {
        OLED_ShowString(16,0,"SETTINGS");
    }
    

    if(menu_setting_page < PAGE_SYSTEM_SET)
    {
        Movie_Show_Img(4,(menu_setting_page*12+16),BMP_CHOOSE1);


        if(system_flag_table->lang == 1)
        {
            OLED_ShowString12(16,16,"Operating Modes");
            OLED_ShowString12(16,28,"Track Log");
            OLED_ShowString12(16,40,"Format/Units");
            OLED_ShowString12(16,52,"Profiles");
        }
        else
        {
            Movie_Show_Img(16,16,BMP_SETTING_HEAD_1);

        }
    }
    else
    {
        Movie_Show_Img(4,((menu_setting_page-PAGE_SYSTEM_SET)*12+16),BMP_CHOOSE1);


        if(system_flag_table->lang == 1)
        {
            OLED_ShowString12(16,16,"Setup");
            OLED_ShowString12(16,28,"Exit");
        }
        else
        {
            Movie_Show_Img(16,16,BMP_SETTING_HEAD_2);
    
        }

    }

}

void firmware_update_process(void)
{
    FIL fil;
    FRESULT fr;

    if(menu_setting_postion > 1)
        menu_setting_postion = 0;
    
    if(system_flag_table->lang == 1)
    {
        fr = f_open(&fil, "V1000.BIN",FA_READ);              
        if(fr == FR_OK)
        {
            system_flag_table->updatefile = 1;
            OLED_ShowString12(16,16,"Firmware Update");
            OLED_ShowString12(16,28,"Confirm");
            OLED_ShowString12(16,40,"Return");
        }
        else
        {
            system_flag_table->updatefile = 0;
            OLED_ShowString12(16,16,"Firmware Update");
            OLED_ShowString12(16,28,"No Firmware Found!");
            v1000_debug("Can't found v1000.bin:%d \r\n",fr);

        }
        
        f_close(&fil);
    }
    else
    {
        fr = f_open(&fil, "V1000.BIN",FA_READ);
        if(fr == FR_OK)
        {
            system_flag_table->updatefile = 1;
            Movie_Show_Img(16,17,firmwareupdate);
            Movie_Show_Img(16,29,confirm);
            Movie_Show_Img(16,40,BMP_SETTING_RETURN);

        }
        else
        {
            system_flag_table->updatefile = 0;
            Movie_Show_Img(16,17,firmwareupdate);
            Movie_Show_Img(16,28,nofirmware);        
            v1000_debug("Can't found v1000.bin:%d \r\n",fr);
        }
        
        f_close(&fil);
    }    

}

void system_setting_item(void)
{
 
    u16 year,month,data;
    float setting_tpf;
 
    Movie_Show_Img(0,0,Bmp_system_head);
    
    if(system_flag_table->lang == 1)
    {
        OLED_ShowString(16,0,"Setup    ");
    }
    
    if(menu_setting_index == MENU_SETTING_SEC_PAGE)
    {
        if(menu_setting_item > 9)
            menu_setting_item = 0;
 
        if(menu_setting_item <= 3)
        {
            if(system_flag_table->lang == 1)
            {
                OLED_ShowString12(16,16,"Altimeter Cal.");
                OLED_ShowString12(16,28,"Screen Off");
                OLED_ShowString12(16,40,"Auto On/Off");
 
                OLED_ShowString12(16,52,"Time Zone");
      
            }
            else
            {
                Movie_Show_Img(16,16,Bmp_arr_ajust);
                Movie_Show_Img(16,28,Bmp_sreen_stanby);
                Movie_Show_Img(16,40,bmp_powerdownword);
                Movie_Show_Img(16,53,Bmp_timer_zone);
 
            }
            Movie_Show_Img(4,(menu_setting_item*12+16),BMP_CHOOSE1);
 
        }
        else if(menu_setting_item <= 7)
        {
            if(system_flag_table->lang == 1)
            {
                OLED_ShowString12(16,16,"Tones");
                //OLED_ShowString12(16,28,"Storm Alarm");
                OLED_ShowString12(16,28,"Language");
                OLED_ShowString12(16,40,"System Info");
                OLED_ShowString12(16,52,"Factory Reset");
            }
            else
            {
                Movie_Show_Img(16,17,Bmp_wannersonud);
                //Movie_Show_Img(16,28,Bmp_baofeng);
                Movie_Show_Img(16,28,Bmp_lan);
                Movie_Show_Img(16,41,bmp_system_word);
                Movie_Show_Img(16,52,Bmp_factroy);
   
                //  Movie_Show_Img(16,52,BMP_SETTING_RETURN);
            }
   
            Movie_Show_Img(4,((menu_setting_item-4)*12+16),BMP_CHOOSE1);
   
        }
        else
        {   
            if(system_flag_table->lang == 1)
            {
                OLED_ShowString12(16,16,"Firmware Update");
                OLED_ShowString12(16,28,"Return");
            }
            else
            {
                Movie_Show_Img(16,17,firmwareupdate);
                Movie_Show_Img(16,28,BMP_SETTING_RETURN);
            }
   
            Movie_Show_Img(4,((menu_setting_item-8)*12+16),BMP_CHOOSE1);   
        }
 
    }
    else if((menu_setting_index == MENU_SETTING_THR_PAGE)
         ||(menu_setting_index == MENU_SETTING_FOR_PAGE))
    {
   
        Movie_Show_Img(2,16,BMP_CHOOSE3)    ;
        switch(menu_setting_item)
        {
   
        #if 0
            case STORM_ALARM:
                if(menu_setting_postion>1)
                    menu_setting_postion = 0;
                if(system_flag_table->lang == 1)
                {
                      OLED_ShowString12(16,16,"Storm Alarm");
                      OLED_ShowString12(16,28,"Off");
                      OLED_ShowString12(16,40,"On");
                }
                else
                {
                    Movie_Show_Img(16,16,Bmp_baofeng);
                    Movie_Show_Img(16,29,bmp_autoreocrd_1);
                }
                Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);
                Movie_Show_Img(4,(setting_tp*12+28),BMP_CHOOSE1);
                if(key_type == KEY_ENTER)
                {
                    system_flag_table->stormalram = menu_setting_postion;
                    stm_write_eerpom(MENU_SETTING_STROMALRRM ,menu_setting_postion);
                }
                break;
   #endif                
            case SYSTEM_ADR_ADJUST:
                if(menu_setting_index == MENU_SETTING_THR_PAGE)
                {
                    if(menu_setting_postion>1)
                        menu_setting_postion = 0;
                    if(system_flag_table->lang == 1)
                    {
                        OLED_ShowString12(16,16,"Altimeter Cal.");
                        OLED_ShowString12(16,28,"Off");
                        OLED_ShowString12(16,40,"On");
                    }
                    else
                    {
                        Movie_Show_Img(16,16,Bmp_arr_ajust);
                        Movie_Show_Img(16,29,bmp_autoreocrd_1);
                    }
                    Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);
                    Movie_Show_Img(4,(setting_tp*12+28),BMP_CHOOSE1);
                }
                else
                {
                    if(system_flag_table->myunit.H_W == 0)
                    {
                        if(menu_setting_postion_2 > 7999)
                            menu_setting_postion_2 = 7999; 
                        
                        if(menu_setting_postion_2 < -7999)
                            menu_setting_postion_2 = -7999;       
                    }
                    else       
                    {       
                        if(menu_setting_postion_2 > 26246)
                            menu_setting_postion_2 = 26246;
                        
                        if(menu_setting_postion_2 < -26246)
                            menu_setting_postion_2 = -26246;
                    }
        
                    if(system_flag_table->lang == 1)
                    {
                        OLED_ShowString12(16,16,"Altimeter Cal.");
                        OLED_ShowString12(16,28,"Current Elevation?");
                    }
                    else
                    {
                        Movie_Show_Img(16,16,Bmp_arr_ajust);
                        Movie_Show_Img(16,29,Bmp_arr_ajust_1)   ;
                    }
                    
                    Movie_Show_Img(0,39,Bmp_updwon);
                    menu_updown_flag = 2;
                    if(system_flag_table->myunit.H_W == 0)
                        sprintf((char *)dtbuf,"%d m",(menu_setting_postion_2));
                    else
                        sprintf((char *)dtbuf,"%d ft",(menu_setting_postion_2));
        
                    OLED_ShowString12(16,41,dtbuf);
                    if(key_type == KEY_ENTER)
                    {
                        Altitude=BMP085_Get_Altitude();
                        if(system_flag_table->myunit.H_W == 0)
                        {       
                            if( Altitude/10>menu_setting_postion_2)
                                system_flag_table->ajust_arr = Altitude/10 - menu_setting_postion_2;
                            else
                            {
                                system_flag_table->ajust_arr = menu_setting_postion_2 - Altitude/10;
                                system_flag_table->ajust_arr = -system_flag_table->ajust_arr;
                            }
                            stm_write_eerpom(MENU_SETTING_AJUST_FLAG ,menu_setting_postion);
        
                            stm_write_eerpom(MENU_SETTING_AJUST_ARR ,system_flag_table->ajust_arr);
                            v1000_debug("\r\n%d-%d-%d\r\n",system_flag_table->ajust_arr,menu_setting_postion_2,Altitude/10);
                        }
                        else
                        {
                            setting_tpf = menu_setting_postion_2*0.3048;
                            if( Altitude/10 >setting_tpf)
                                system_flag_table->ajust_arr = Altitude/10-setting_tpf;
                            else
                            {
                                system_flag_table->ajust_arr = setting_tpf - Altitude/10;
                                system_flag_table->ajust_arr = -system_flag_table->ajust_arr;
                            }
                            stm_write_eerpom(MENU_SETTING_AJUST_FLAG ,menu_setting_postion);
                            stm_write_eerpom(MENU_SETTING_AJUST_ARR ,system_flag_table->ajust_arr);
                            v1000_debug("\r\n%d-%d-%d\r\n",system_flag_table->ajust_arr,menu_setting_postion_2,Altitude/10);
                        }
        
                        system_flag_table->ajust_Arr_flag = 1;
                        key_type = 0xff;
                        menu_setting_index--;
                        enter_perss = 4;
                    }
                }
    
                break;
    
           case SCREEN_STANBY:
                if(menu_setting_postion>2)
                    menu_setting_postion = 0;
                if(system_flag_table->lang == 1)
                {
                    OLED_ShowString12(16,16,"Screen Off");
                    OLED_ShowString12(16,28,"15 sec");
                    OLED_ShowString12(16,40,"30 sec");
                    OLED_ShowString12(16,52,"Always On");
                }
                else
                {
                    Movie_Show_Img(16,16,Bmp_sreen_stanby)  ;
                    Movie_Show_Img(16,29,Bmp_sreen_stanby_1)        ;
                }
                if(key_type == KEY_ENTER)
                {
                    system_flag_table->screen_timer = menu_setting_postion;
                }
                break;
           case TIMER_ZONE:
                if(menu_setting_postion>41)
                    menu_setting_postion = 0;
                if(system_flag_table->lang == 1)
                {
                    OLED_ShowString12(16,16,"Time Zone");
                }
                else
                {
                    Movie_Show_Img(16,17,Bmp_timer_zone)    ;
                }
                Movie_Show_Img(0,28,Bmp_updwon);
                menu_updown_flag = 1;
                sprintf((char *)dtbuf,"%s",timer_zone_Aarry[menu_setting_postion]);
                OLED_ShowString12(16,29,dtbuf);         
                break;
           case WANNER_SOUND:
                if(menu_setting_postion>1)
                    menu_setting_postion = 0;
                if(system_flag_table->lang == 1)
                {
                    OLED_ShowString12(16,16,"Tones");
                    OLED_ShowString12(16,28,"Off");
                    OLED_ShowString12(16,40,"On");
                }
                else
                {
                    Movie_Show_Img(16,17,Bmp_wannersonud);
                    Movie_Show_Img(16,29,bmp_autoreocrd_1);
                }
                Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);
                Movie_Show_Img(4,(setting_tp*12+28),BMP_CHOOSE1);
                if(key_type == KEY_ENTER)
                {
                    system_flag_table->wanner_sound = menu_setting_postion;
                    stm_write_eerpom(MENU_SETTING_WANNER_SOUND ,menu_setting_postion);
                }
           
                break;
           case LANGUAGE:
                if(menu_setting_postion>1)
                    menu_setting_postion = 0;
            
                Movie_Show_Img(16,16,Bmp_lan)   ;
                Movie_Show_Img(16,29,Bmp_lan_1);
            
                break;
            case POWERDONW_CONTRL:
                if(menu_setting_postion>1)
                    menu_setting_postion = 0;
                if(system_flag_table->lang == 1)
                {
                      OLED_ShowString12(16,16,"Auto On/Off");
                      OLED_ShowString12(16,28,"Off");
                      OLED_ShowString12(16,40,"On");
                }
                else
                {
                //  Movie_Show_Img(0,0,bmp_system_power_contrl);
                    Movie_Show_Img(16,16,bmp_powerdownword);
                    Movie_Show_Img(16,29,bmp_autoreocrd_1);
                }
                OLED_Fill(0, 28, 15, 63, 0);
                if(key_type == KEY_ENTER)
                {
                    system_flag_table->usb_powerdown_flash = menu_setting_postion;
                }
                break;
            case SYSTEM_INFO:
                menu_setting_postion = 0;
                if(system_flag_table->lang == 1)
                {
                    OLED_ShowString12(16,16,"Sytem Info");
                    OLED_ShowString12(16,28,"FW:");
                    OLED_ShowString12(16,40,"Power On:");
                    OLED_ShowString12(16,52,"First Use:");
                }
                else
                {
                    Movie_Show_Img(0,0,bmp_system_status);
                }
                OLED_ShowString12(72,28,"V1.0.7");
                sprintf((char *)dtbuf,"%d",system_flag_table->powerdown_timers); //most
                OLED_ShowString12(72,40,dtbuf);
                stm_read_eerpom(MENU_FRIST_POWER_INDEX_ADDRES ,&setting_tp);
                if(setting_tp == 0x12345678)
                {
                    stm_read_eerpom(MENU_FRIST_POWERDATE_INDEX_ADDRES ,&setting_tp);
                    year = (setting_tp&0x00ff0000)>>16;
                    month = (setting_tp&0x0000ff00)>>8;
                    data = (setting_tp&0x000000ff);
                    sprintf((char *)dtbuf,"%02d-%02d-%02d",year,month,data);    //��ʾUTC����
                    OLED_ShowString12(72,52,dtbuf);
                }
                //OLED_ShowString12(80,52,COMPILED_DATE);
                break;
            case FACTROY:
                if(menu_setting_postion>1)
                    menu_setting_postion = 0;
                if(system_flag_table->lang == 1)
                {
                    OLED_ShowString12(16,16,"Factory Reset");
                    OLED_ShowString12(16,28,"No");                     
                    if(USB_CONNECT == 0)
                	    OLED_ShowString12(16,40,"Yes");
                    else
                        OLED_ShowString12(16,40,"Unplug the Cable");
                }
                else
                {
                    Movie_Show_Img(16,16,Bmp_factroy)   ;

                    
                    Movie_Show_Img(16,29,Bmp_yes_or_no);
                    if(USB_CONNECT == 1)
                        Movie_Show_Img(16,40,unplug_usb);
                    
                }       
                break;
            case FIRMWARE_UPDATE:
                firmware_update_process();
                break;
   
        }
   
        if((menu_setting_item!= SYSTEM_INFO)&&(menu_setting_item!= SYSTEM_ADR_ADJUST)&&(menu_setting_item!=TIMER_ZONE)&&(menu_setting_item != FIRMWARE_UPDATE))
        {
        
            Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);
            Movie_Show_Img(4,(setting_tp*12+28),BMP_CHOOSE1);
        
        }
       
        if((menu_setting_item == FIRMWARE_UPDATE)&&(system_flag_table->updatefile == 1))
        {
        
            Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);
            Movie_Show_Img(4,(setting_tp*12+28),BMP_CHOOSE1);
        
        }
   
        if(key_type == KEY_ENTER)
        { 
            if((menu_setting_item != FACTROY)&&(menu_setting_item != SYSTEM_INFO)&&(menu_setting_item != FIRMWARE_UPDATE))
            {
                setting_tp = menu_setting_postion;
                stm_write_eerpom(MENU_SETTING_AJUST_FLAG+ menu_setting_item ,setting_tp);
                if(menu_setting_item == SYSTEM_ADR_ADJUST)
                {
                     system_flag_table->ajust_Arr_flag = 0;
                     system_flag_table->ajust_arr  = 0;
                }
            }
            else
            {
                if((menu_setting_postion == 1)&&(menu_setting_item == FACTROY))
                {
                    stm_read_eerpom(MENU_FRIST_POWERDATE_INDEX_ADDRES ,&setting_tp);
                    stm_write_eerpom(MENU_SETTING_INDEX_ADDRES,0x20077);
                    stm_EraseWord_Eeprom();
                    stm_write_eerpom(MENU_FRIST_POWER_INDEX_ADDRES ,0x12345678);
                    stm_write_eerpom(MENU_FRIST_POWERDATE_INDEX_ADDRES ,setting_tp);
                    stm_write_eerpom(MENU_POWERDOWNTIME_INDEX_ADDRES ,system_flag_table->powerdown_timers|0x12000000);
                    OLED_Refresh_Gram();
                    check_eeprom_init();
                    mkdir_init(1);
                    old_spped_div_cnt = 0;
                    old_div_cnt = 0;
                    old_altiude_div_cnt = 0;
                }
       
                if((menu_setting_postion == 0)&&(menu_setting_item == FIRMWARE_UPDATE))
                {
                    if(system_flag_table->updatefile == 0)
                    {
                        menu_setting_postion = 0;
                        menu_setting_index = MENU_SETTING_SEC_PAGE;
                    }
                    else
                    {
                        stm_write_eerpom(CUURENT_BUUFER_INDEX_ADDRER ,0x12345678);
                        SoftReset();
                    }
                }
                else
                {
                }
       
            }
        
            if(menu_setting_item == TIMER_ZONE)
            {
                rtc_set_flag = 1;
                system_flag_table->time_zone = menu_setting_postion;
       
                time_zone = ((((timer_zone_Aarry[system_flag_table->time_zone][4]-'0')*10)+(timer_zone_Aarry[system_flag_table->time_zone][5]-'0')));
                time_zone =time_zone/60;
                time_zone += ((timer_zone_Aarry[system_flag_table->time_zone][1]-'0')*10)+(timer_zone_Aarry[system_flag_table->time_zone][2]-'0');
                v1000_debug("\r\ntime_zone-%f\r\n",time_zone);
				if(timer_zone_Aarry[system_flag_table->time_zone][0] == '-')
					time_zone = -time_zone;
                check_time();
            }
        
            if(menu_setting_item == LANGUAGE)
            {
                system_flag_table->lang = menu_setting_postion;
            }
            enter_perss = 4;
        }
   }
 
 
}

void setting_index_23page(void)
{
    //u32 setting_tp;
    float setting_tpf;

    switch(menu_setting_page)
    {
       case PAGE_RUN_MODEL:
           stm_read_eerpom(MENU_SETTING_RUNMODEL_ADDRES ,&setting_tp);
           Movie_Show_Img(0,0,BMP_SETTING_RUNMODE);
           if(system_flag_table->lang == 1)
           {
             OLED_ShowString(16,0,"Operating Mode");
             OLED_ShowString12(16,16,"Sensor Mode");
             OLED_ShowString12(16,28,"GPS Mode");
           }
             
           if(menu_setting_item >1)       
               menu_setting_item = 0;
           Movie_Show_Img(4,(menu_setting_item*12+16),BMP_CHOOSE2);
           Movie_Show_Img(4,(setting_tp*12+16),BMP_CHOOSE1);
       
           switch(menu_setting_item)
           {
               case SENSOR_MODEL:
                   setting_tp = 0;
                   break;
               case GPS_MODEL:
                   setting_tp = 1;
                   break;
               default:       
                   break;      
           }
       
           break;
       case PAGE_GUJI_RECORD:
           menu_display_settingitem_recode();
           break;
       case PAGE_GESHI_DANGWEI:
           Movie_Show_Img(0,0,bmp_unit_head);
           if(system_flag_table->lang == 1)
           {
               OLED_ShowString(16,0,"Format/Unit    ");
           }

           if(menu_setting_index == MENU_SETTING_THR_PAGE)
           {
               Movie_Show_Img(2,16,BMP_CHOOSE3);   
               switch(menu_setting_item)
               {
                   case UNIT_TIME:
    
                       if(system_flag_table->lang == 1)
                       {
                             OLED_ShowString12(16,16,"Time");
                             OLED_ShowString12(16,28,"12-Hour");
                             OLED_ShowString12(16,40,"24-Hour");
                       }
                       else
                       {
                           Movie_Show_Img(16,16,bmp_timer);
           
                           Movie_Show_Img(16,28,Bmp_time1);
           
                       }
                       if(menu_setting_postion>1)
                           menu_setting_postion = 0;
                       system_flag_table->myunit.TIMER = menu_setting_postion;
                       Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);
                       break;
                   case UNIT_DATE:
                       if(system_flag_table->lang == 1)
                       {
                           OLED_ShowString12(16,16,"Date");
                           OLED_ShowString12(16,28,"YY-MM-DD");
                           OLED_ShowString12(16,40,"MM-DD-YY");
                           OLED_ShowString12(16,52,"DD-MM-YY");
                       }
                       else
                       {
                           Movie_Show_Img(16,16,bmp_data);
       
                           Movie_Show_Img(16,28,Bmp_data1);
       
                       }
                       if(menu_setting_postion>2)
                           menu_setting_postion = 0;
       
                       system_flag_table->myunit.DATA= menu_setting_postion;
                       Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);
                       break;
                   case UNIT_DISCANCE:
       
                       if(menu_setting_postion>2)
                           menu_setting_postion = 0;
                       if(system_flag_table->lang == 1)
                           {
                             OLED_ShowString12(16,16,"Distance/Weight");
                           }
                       else
                           Movie_Show_Img(16,16,bmp_distance_hwe);
           
                       sprintf((char *)dtbuf,"m/km/kg");
                       OLED_ShowString12(16,28,dtbuf);
                       sprintf((char *)dtbuf,"ft/mi/pound");
                       OLED_ShowString12(16,40,dtbuf);
                       sprintf((char *)dtbuf,"ft/kt/pound");
                       OLED_ShowString12(16,52,dtbuf);
           
                       system_flag_table->myunit.H_W= menu_setting_postion;
                       Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);
                       break;
                   case UNIT_COORD:
                       if(menu_setting_postion>2)
                           menu_setting_postion = 0;
                       if(system_flag_table->lang == 1)
                       {
                             OLED_ShowString12(16,16,"Position");
                             OLED_ShowString12(16,28,"ddd.dddd#");
                             OLED_ShowString12(16,40,"ddd.mm.mmm$");
                             OLED_ShowString12(16,52,"ddd.mm$ss.s&");
                       }
                       else
                       {
                           Movie_Show_Img(16,16,bmp_crood);
           
                           Movie_Show_Img(16,29,Bmp_coord1);
           
                       }
                       system_flag_table->myunit.COORD= menu_setting_postion;
                       Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);
                       break;
      
                   case UNIT_TEMP:
                       if(menu_setting_postion>1)
                           menu_setting_postion = 0;
                       if(system_flag_table->lang == 1)
                       {
                           OLED_ShowString12(16,16,"Temperature");
                           OLED_ShowString12(16,28,"Celsius");
                           OLED_ShowString12(16,40,"Fahrenheit");
                       }
                       else
                       {
                           Movie_Show_Img(16,16,bmp_temp);
           
                           Movie_Show_Img(16,28,Bmp_temp1);
           
                           Movie_Show_Img(16,40,Bmp_temp2);
                       }
                       system_flag_table->myunit.TEMP= menu_setting_postion;
                       Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);
                       break;
                   case UNIT_PRES:
       
                       if(menu_setting_postion>3)
                           menu_setting_postion = 0;
                       if(system_flag_table->lang == 1)
                       {
                           OLED_ShowString12(16,16,"Pressure");
                       }
                       else
                           Movie_Show_Img(16,16,bmp_pres);
       
       
                       if(menu_setting_postion<3)
                       {
                           if(system_flag_table->lang == 1)
                           {
                               OLED_ShowString12(16,16,"Pressure");
                               OLED_ShowString12(16,28,"Hectopascals");
                               OLED_ShowString12(16,40,"Millibars");
                               OLED_ShowString12(16,52,"mm Hg");
                           }
       
                           else
                           {
       
                               Movie_Show_Img(16,28,Bmp_pres_1);
                           }
                       }
                       else
       
                       {
                           if(system_flag_table->lang == 1)
                           {
                               OLED_ShowString12(16,28,"Inch Hg");
                           }
                           else
                               Movie_Show_Img(16,28,Bmp_pres_2);
       
                       }
       
                       system_flag_table->myunit.PREE= menu_setting_postion;
       
                       if(menu_setting_postion<3)
                           Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);
                       else
                           Movie_Show_Img(4,((menu_setting_postion-3)*12+28),BMP_CHOOSE2); 
       
                       break;
                   case UNIT_REUTNR:
       //                     system_flag_table->unit.DATA= menu_setting_postion;
                       break;
       
               }
   
               if(setting_tp < 3)
               {
                   if(menu_setting_postion < 3)
                       Movie_Show_Img(4,(setting_tp*12+28),BMP_CHOOSE1);
      
               }
               else
               {
                   if(menu_setting_postion>=3)
                      Movie_Show_Img(4,((setting_tp-3)*12+28),BMP_CHOOSE1);
               }
   
               if(key_type == KEY_ENTER)
               {
                   setting_tp = menu_setting_postion;
                   stm_write_eerpom(MENU_SETTING_UNIT_TIMER+ menu_setting_item ,setting_tp);
                   enter_perss = 4;
               }
   
   
   
          }
          else if(menu_setting_index == MENU_SETTING_SEC_PAGE)
          {
              if(menu_setting_item>UNIT_REUTNR)
              {
                  menu_setting_item = 0;
              }
              if(menu_setting_item<=UNIT_DISCANCE)
              {
                  if(system_flag_table->lang == 1)
                  {
                      OLED_ShowString12(16,16,"Time");
                      OLED_ShowString12(16,28,"Date");
                      OLED_ShowString12(16,40,"Position");
                      OLED_ShowString12(16,52,"Distance/Weight");
                  }
                  else
                  {
                      Movie_Show_Img(16,16,bmp_timer);
                      Movie_Show_Img(16,28,bmp_data);
                      Movie_Show_Img(16,40,bmp_crood);
                      Movie_Show_Img(16,53,bmp_distance_hwe);
                  }
                  Movie_Show_Img(4,(menu_setting_item*12+16),BMP_CHOOSE1);
              }
              else
              {
                  if(system_flag_table->lang == 1)
                  {
                      OLED_ShowString12(16,16,"Temperature");
                      OLED_ShowString12(16,28,"Pressure");
                      OLED_ShowString12(16,40,"Return");
                  }
                  else
                  {
                      Movie_Show_Img(16,17,bmp_temp);
                      Movie_Show_Img(16,28,bmp_pres);
                      Movie_Show_Img(16,40,BMP_SETTING_RETURN);
                  }
                  Movie_Show_Img(4,((menu_setting_item-UNIT_TEMP)*12+16),BMP_CHOOSE1);
      
              }
      
          }   
          break;
      case PAGE_GEREN_ZHILIAO:
        
          Movie_Show_Img(0,0,Bmp_personal_head);
          if(system_flag_table->lang == 1)
          {
              OLED_ShowString(16,0,"Profiles   ");
          }
          
          if(menu_setting_index == MENU_SETTING_SEC_PAGE)
          {
   
              if(menu_setting_item>3)
              {
                  menu_setting_item = 0;
              }
       
              if(system_flag_table->lang == 1)
              {
                  OLED_ShowString12(16,16,"Gender");
                  OLED_ShowString12(16,28,"Height");
                  OLED_ShowString12(16,40,"Weight");
                  OLED_ShowString12(16,52,"Return");
              }
              else
              {
                  Movie_Show_Img(16,17,Bmp_xinbie);
                  Movie_Show_Img(16,29,Bmp_heigt);
                  Movie_Show_Img(16,41,Bmp_weigt);
                  Movie_Show_Img(16,52,BMP_SETTING_RETURN);
              }
              Movie_Show_Img(4,(menu_setting_item*12+16),BMP_CHOOSE1);
   
          }
          else if(menu_setting_index == MENU_SETTING_THR_PAGE)
          {   
              Movie_Show_Img(2,16,BMP_CHOOSE3);
              switch(menu_setting_item)
              {
                  case PERSONAL_FALEM:
                      if(menu_setting_postion>1)
                      {
                          menu_setting_postion = 0;
                      }
                      if(system_flag_table->lang == 1)
                      {
                            OLED_ShowString12(16,16,"Gender");
                            OLED_ShowString12(16,28,"Male");
                            OLED_ShowString12(16,40,"Female");
                      }
                      else
                      {
                          Movie_Show_Img(16,17,Bmp_xinbie);
          
                          Movie_Show_Img(16,29,Bmp_manfalem);
          
                      }
                      Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);
          
                      Movie_Show_Img(4,(setting_tp*12+28),BMP_CHOOSE1);
          
                      break;
                  case PERSONAL_HEIGHT:
                      menu_updown_flag = 1;
                      if(system_flag_table->lang == 1)
                      {
                          OLED_ShowString12(16,16,"Height");
                      }
                      else
                      {
                          Movie_Show_Img(16,17,Bmp_heigt);
                      }
                      Movie_Show_Img(0,27,Bmp_updwon);
                      if(system_flag_table->myunit.H_W == 0)
                      {
                          if(menu_setting_postion>150)
                          {
                              menu_setting_postion = 0;
                          }       
                          sprintf((char *)dtbuf,"%d cm",(100+menu_setting_postion));
   
                      }
                      else
                      {
                          if(menu_setting_postion>50)
                          {
                              menu_setting_postion = 0;
                          }
      
                          setting_tpf = (30+menu_setting_postion);
      
                          sprintf((char *)dtbuf,"%.1f ft",setting_tpf/10);
      
                      }
                      OLED_ShowString12(16,29,dtbuf);
                      break;
                  case PERSONAL_WEIGHT:
                      menu_updown_flag = 1;
                      if(system_flag_table->lang == 1)
                      {
                          OLED_ShowString12(16,16,"Weight");
                      }
                      else
                      {
                          Movie_Show_Img(16,17,Bmp_weigt);
                      }
                      Movie_Show_Img(0,27,Bmp_updwon);
                      if(system_flag_table->myunit.H_W == 0)
                      {
                          if(menu_setting_postion>180)
                          {
                               menu_setting_postion = 0;
                          }
      
                          sprintf((char *)dtbuf,"%d kg",(20+menu_setting_postion));
   
                      }
                      else
                      {
                          if(menu_setting_postion>460)
                          {
                               menu_setting_postion = 0;
                          }
      
                           sprintf((char *)dtbuf,"%d lbs",(40+menu_setting_postion));
    
                      }
                      OLED_ShowString12(16,29,dtbuf);
                      break;
      
                  }
   
                  if(key_type == KEY_ENTER)
                  {
                       if(menu_setting_item == PERSONAL_WEIGHT)
                       {
                          if(system_flag_table->myunit.H_W == 0)
                          {
                              system_flag_table->personal_da.weight = menu_setting_postion+20;
                              stm_write_eerpom(MENU_SETTING_FALME+ menu_setting_item ,menu_setting_postion);
                          }
                          else
                          {
                              system_flag_table->personal_da.weight_ft= ((menu_setting_postion+40)*0.4536);
                              stm_write_eerpom(MENU_SETTING_WEIGHT_1 ,menu_setting_postion);
                          }
              
                      }
                      else if(menu_setting_item == PERSONAL_HEIGHT)
                      {
                          if(system_flag_table->myunit.H_W == 0)
                          {
                              system_flag_table->personal_da.height= menu_setting_postion+100;
                              stm_write_eerpom(MENU_SETTING_FALME+ menu_setting_item ,menu_setting_postion);
                          }
                          else
                          {
                              system_flag_table->personal_da.height_ft=(menu_setting_postion+30)/0.328;
                              stm_write_eerpom(MENU_SETTING_HEIGHT_1 ,menu_setting_postion);
                          }
              
                      }
                      else
                          stm_write_eerpom(MENU_SETTING_FALME ,menu_setting_postion);
                      /*�Ա𲻻ᱣ��*/
                  //  setting_tp = menu_setting_postion;
              
                      enter_perss = 4;
                  }
             }
             break;
         case PAGE_SYSTEM_SET:
             system_setting_item();
             break;
         case PAGE_QUIT:
             enter_perss =  0;
             break;
   
   }
}

void menu_setting_index_show(void)
{
    if(menu_setting_index == 0xff)
    {
        setting_index_org();
    }
    else if(menu_setting_index == MENU_SETTING_MAIN_PAGE)
    {
        setting_index_mainpage();
    }
    else if((menu_setting_index == MENU_SETTING_SEC_PAGE)||(menu_setting_index == MENU_SETTING_THR_PAGE)||(menu_setting_index == MENU_SETTING_FOR_PAGE))
    {
        setting_index_23page();
    }

}

/*ϵͳ����*/
void menu_display_setting(void)
{
  
    OLED_Fill(0, 0, 127, 63, 0);

    if(key_type == KEY_LONG_ENTER)
	{
    	enter_perss = 0;
    	menu_setting_index = 0xff;
		need_to_powerdownflag = 0;
    	key_type = 0xff;
    	keyTabIndex = MENU_MAIN;
    	return;
	}
		
    menu_setting_control();
    menu_setting_index_show();


}


/***���ò˵�END******/




void system_power_down_process()
{

    // u32 setting_tp;
    Movie_Show_Img(0,14,bmp_consure_powerdown);

    if(system_flag_table->lang == 1)
    {
    //OLED_ShowString12(56,14,"Return    ");
    //OLED_ShowString12(56,26,"Power off ");
    //OLED_ShowString12(56,38,"Mode Switching");
        Movie_Show_Img(0,14,bmp_consure_powerdown_en);
    }
    else
        Movie_Show_Img(0,14,bmp_consure_powerdown);

    if(key_type == KEY_ENTER)
    {
        KeyTab[keyTabIndex].CurrentOperate();
        system_flag_table->powerdown_timers++;
        stm_write_eerpom(MENU_POWERDOWNTIME_INDEX_ADDRES ,system_flag_table->powerdown_timers|0x12000000);
        need_to_powerdownflag = 0;

        if(USB_CONNECT == 0)
             Choose_power_mode(MCU_STBY);
        else
        {
            system_power_flag = SYSTEM_POWER_STANBY;
            OLED_Clear();
            di_and_flash_empet(3);
        }
        powerdonw_resume_timer  = 0;
    }
    else if(key_type == KEY_UP)
    {
        KeyTab[keyTabIndex].CurrentOperate();
        need_to_powerdownflag = 0;
        powerdonw_resume_timer  = 0;
    }
    else if (key_type == KEY_DOWN)
    {
        keyTabIndex = MENU_SETTING_DISTANCE;
        menu_setting_index = MENU_SETTING_MAIN_PAGE;
        menu_setting_page = PAGE_RUN_MODEL;
        key_type = KEY_ENTER ;
        enter_perss =1;
        menu_display_setting();
        need_to_powerdownflag = 2;
        powerdonw_resume_timer  = 0;
    }
    OLED_Refresh_Gram();
}



void di_and_flash_empet(u8 times)
{
    u8 i;

    if(system_power_flag == SYSTEM_POWER_ON)
	{
	    LED0 = 1;
    	for(i=0;i<times;i++)
     	{
    	    if(system_flag_table->batt_Status == 0xff)
     	 	{
     	 	}

            Button_key_perss(1);
            delay_ms(50);

            Button_key_perss(0);
            delay_ms(50);

    	    if(system_flag_table->batt_Status == 0xff)
     	 	{
     	 	}

 	    }
 	    LED0 = 0;
	}
    else if(system_power_flag == SYSTEM_POWER_STANBY)
	{
        OLED_Clear();
        for(i=0;i<times;i++)
        {
            if(system_flag_table->batt_Status == 0xff)
       	 	{
          		Movie_Show_Img(43,0,Bmp_batt_0);
          		OLED_Refresh_Gram();
          		delay_ms(500);
          		OLED_Fill(0, 0, 127, 16, 0);
          		OLED_Refresh_Gram();
          		delay_ms(500);
       	 	}
       
       
       	}
		Button_key_perss(1);
		delay_ms(600);
		Button_key_perss(0);
	}

}

void is_recoreding(void)
{
    if(guji_mode  != RECORED_IDLE)
    {
        guji_mode = RECORED_STOP;
        Recording_guji();
    } 

}
void change_to_runmode_recoder(signed char index)
{
    keyTabIndex = MENU_SETTING_COOR;
    if(index != 0xff)
        guji_icon_index = index;
    enter_perss = 2;
    guji_mode = RECORED_IDLE;
    if(system_flag_table->run_mode == 0 )
    {
        system_flag_table->run_mode  = 1;
        stm_write_eerpom(MENU_SETTING_RUNMODEL_ADDRES ,1);
        GPS_power_ctrl(1);
        USART_Cmd(USART2, ENABLE);
    }

    KeyTab[keyTabIndex].CurrentOperate();
}

void menu_reflash(void)
{
    menu_setting_postion = 0;

    KeyTab[keyTabIndex].CurrentOperate();
}

void menu_keyprocess(u8 key)
{

    static u8 menu_info_update;
    extern u8 button_100_time;
#ifdef baoyu
    extern u8 flash_baoyu_flag ;
#endif

#if 1
    if(key != 0)
  	{
    	Send_to_goto_lowpower();

  #ifdef baoyu
    	baoyun_flag = 0;
    	flash_baoyu_flag = 0;
  #endif      
    	if(system_power_flag == SYSTEM_POWER_LOW)
  		{
    		system_power_flag  =  SYSTEM_POWER_ON;
    		if((system_flag_table->run_mode == 0)&&(USB_CONNECT == 0))
    			Choose_power_mode(MCU_RUN);
    		KeyTab[keyTabIndex].CurrentOperate();
    		OLED_Refresh_Gram();
    		Button_key_perss(1);
    		button_100_time = 5;
    		return ;
  		}
	    else if(system_power_flag == SYSTEM_POWER_LOW_2)
		{
		    system_power_flag = SYSTEM_POWER_LOW;
    
    		Button_key_perss(1);
    		button_100_time = 5;
    		system_flag_table->To_lower_timercnt = 6;
    		OLED_Fill(0, 0, 127, 63, 0);
    		OLED_Refresh_Gram();
    		return;
		}
	    else
		{
			if(system_power_flag != SYSTEM_POWER_STANBY)
			{
			    if((need_to_powerdownflag != 1)||(key != 3))
				{
    				Button_key_perss(1);
    				button_100_time = 5;
				}
			}
		}
  	}
    else
	{
 	    if(system_power_flag == SYSTEM_POWER_LOW)
 		{
 		    if(menu_info_update == 1)
 			{
     			menu_info_update = 0;
    
    			menu_status_dispplay();
    			if(system_flag_table->run_mode == 0)
    				OLED_Refresh_Gram_low();
    			else
    				OLED_Refresh_Gram();
 			}
		    else
		    	menu_info_update ++;
 		    return;

 		}
	    else if(system_power_flag == 0)
		{

			menu_status_dispplay();
			OLED_Refresh_Gram();
		}
	    else if(system_power_flag == 3)
		{
	    	if(system_flag_table->run_mode == 0)
			{
    			menu_status_dispplay();
    			OLED_Refresh_Gram();
			}
	    	return;
		}


	}


    if(need_to_powerdownflag == 1)
	{
        system_flag_table->To_lower_timercnt = 0;
        if(key == 1)
            key_type = KEY_UP;
        else if(key == 2)
           	key_type = KEY_DOWN;
        else if(key == 3)
            key_type = KEY_ENTER;
        system_power_down_process();
        return;
	}
	


#endif
    if(enter_perss == 3 || enter_perss == 4)
    {
        return ;
    }

    if((system_power_flag == SYSTEM_POWER_STANBY)&&((key !=5)&&(key !=6)))
    {
        return;
    }

    switch(key)
	{
	    case 1:// up
		    key_type = KEY_UP;
		    if((home_flag!=3)&&(KeyTab[keyTabIndex].subnumber != 'S'))
			    OLED_Fill(0, 0, 127, 63, 0);
		    if(enter_perss >0)
			{
			    if(KeyTab[keyTabIndex].subnumber == 'H')
				{
				    if(home_flag<=1)
                    {
                        
					    menu_item_index--;
                        KeyTab[keyTabIndex].CurrentOperate();
                    }
				}
		    	else if(KeyTab[keyTabIndex].subnumber == 'C')
			    	RTC_Get(0,&my_timer);
			    else if((KeyTab[keyTabIndex].subnumber != 'G')&&(KeyTab[keyTabIndex].subnumber != 'S'))//&&(KeyTab[keyTabIndex].MenuID != MENU_REALTIME))
			    	get_prev();
			}
		    else
			{
			    get_prev();

			    v1000_debug("\r\n %s",KeyTab[keyTabIndex].MenuName);
			}
			KeyTab[keyTabIndex].CurrentOperate();
		    break;


	    case 2://down
		    key_type = KEY_DOWN;
		    if((home_flag!=3)&&(KeyTab[keyTabIndex].subnumber != 'S'))
		    	OLED_Fill(0, 0, 127, 63, 0);
		    if(enter_perss>0)
			{
			    if(KeyTab[keyTabIndex].subnumber == 'H')
				{
				    if(home_flag<=1)
                    {
                        
					    menu_item_index++;
                        KeyTab[keyTabIndex].CurrentOperate();
                    }
				}
			    else if(KeyTab[keyTabIndex].subnumber == 'C')
				    RTC_Get(1,&my_timer);
			    else if((KeyTab[keyTabIndex].subnumber != 'G')&&(KeyTab[keyTabIndex].subnumber != 'S'))//&&(KeyTab[keyTabIndex].MenuID != MENU_REALTIME))
				    get_next();
			}
		    else
		    {
		        get_next();
		    }

		    KeyTab[keyTabIndex].CurrentOperate();
		    break;

	    case 3://OK
            key_type = KEY_ENTER;
            if(keyTabIndex == MENU_MAIN)
                return;
    
            if((home_flag!=3)&&(KeyTab[keyTabIndex].subnumber != 'G'))
                OLED_Fill(0, 0, 127, 63, 0);
            //	   OLED_Clear();
            if(KeyTab[keyTabIndex].subnumber ||(menu_change_index == KeyTab[keyTabIndex].MenuID)|| (enter_perss >0))
            {
                if(enter_perss)
                {
                    if((KeyTab[keyTabIndex].subnumber == 2)||(KeyTab[keyTabIndex].ParMenuID == MENU_TRANSFORMATION))
                    {
                    //keyTabIndex  = ParMenuID_loop;
                        menu_change_index = KeyTab[keyTabIndex].MenuID;
                    }
                    else if(KeyTab[keyTabIndex].subnumber == 'H')
                    {
                        home_menu_item_index = home_menu_item_index+menu_item_index;
                        menu_item_index = 0;
            
                        if(home_flag == 1)
                        {

                            home_flag  = 2;
                        }
            
                    }               
                }
                else
                {
                     if(KeyTab[keyTabIndex].subnumber == 2)
                         ParMenuID_loop = KeyTab[keyTabIndex].MenuID;
                }
        
        
                if(menu_setting_index == 0xff)
                {
                    if((enter_perss)&&(home_flag==0))
                        enter_perss =0;
                    else
                        enter_perss  = 1;
                }
        
            }
    
    		KeyTab[keyTabIndex].CurrentOperate();
    		v1000_debug("\r\nkey 3  enter_perss  :%d",enter_perss);
            break;

	    case 4:

            key_type = KEY_LONG_ENTER;
            if(keyTabIndex == MENU_MAIN)
                return;
            enter_perss = 2;
            if((KeyTab[keyTabIndex].subnumber == 'H')||(KeyTab[keyTabIndex].subnumber == 'G')||(KeyTab[keyTabIndex].subnumber == 'S'))
            {
                if((KeyTab[keyTabIndex].subnumber == 'H'))
                {
                    if(home_flag == 0)
                    {
                        
                        if(gpsx->fixmode > 1)
                        {
                            homex.longitude = gpsx->longitude;
                            homex.latitude = gpsx->latitude;
                            homex.nshemi = gpsx->nshemi;
                            homex.ewhemi = gpsx->ewhemi;
                            home_flag = 1;
                        }
                        else if((home_menu_item_index +menu_item_index)>= CUSTOM_HOME_POST)
                        {
                            home_flag = 1;
                        }
                    }
                    else if(home_flag == 1)
                    {
                        if((gpsx->fixmode > 1)||((home_menu_item_index +menu_item_index)>= CUSTOM_HOME_POST))
                             home_flag  = 2;
                    }
    
                }
                KeyTab[keyTabIndex].CurrentOperate();
                //enter_perss = 0;
    
            }
            v1000_debug("\r\nkey 4");

		    break;

       case 5:
    		v1000_debug("\n\r MCU_STBY ");
    		if(need_to_powerdownflag == 0)
    	    {
            	if(system_power_flag == SYSTEM_POWER_STANBY)
     	 		{
      //            USB_Interrupts_Config_disable();
                    system_flag_table->charger_connected = 0;
                    LED0 = 0;
                    Choose_power_mode(MCU_RUN);
                    change_clk_need_int(0);
                    system_power_flag = SYSTEM_POWER_ON;
                    old_div_cnt = 0;
                    old_spped_div_cnt = 0;
                    old_altiude_div_cnt = 0;
                    stm_write_eerpom(MENU_POWER_STATE_INDEX_ADDRES ,0);
                    OLED_Clear();
                    OLED_Display_On();
                    Movie_Show_Img(0, 22,BMP_logo);
					//OLED_ShowString(60,30,"1.00");
                    OLED_Refresh_Gram();
                    bmp_moudle_preinit();
                    di_and_flash_empet(2);
                    //delay_ms(1200);
                    delay_ms(1200);
                    keyTabIndex = MENU_MAIN;
                    KeyTab[keyTabIndex].CurrentOperate();
                    Send_to_goto_lowpower();			
                    keyTabIndex = MENU_MAIN;
                    if(system_flag_table->run_mode == 1)
                        GPS_power_ctrl(1);
     
                    if(system_flag_table->guji_record.auto_recoed_flag == 1)
                    {
                        change_to_runmode_recoder(5);
                    }
     		    }
    			else
    			{
    			//if(USB_CONNECT == 0)
    				{
                        OLED_Clear();
                        need_to_powerdownflag =1;
                        powerdonw_resume_timer = 1000;
    				}
    			}
    
    			return ;
    		}
    
    		break;

	    case 6:
            if(system_power_flag == SYSTEM_POWER_STANBY)
            {
                menu_keyprocess(5);
                stm_write_eerpom(MENU_FRIST_POWER_INDEX_ADDRES ,0x0);
                system_flag_table->powerdown_timers = 0;
                stm_write_eerpom(MENU_POWERDOWNTIME_INDEX_ADDRES ,system_flag_table->powerdown_timers|0x12000000);
                stm_write_eerpom(MENU_FRIST_POWERDATE_INDEX_ADDRES ,0);
    
                return;
            }
            else if(system_power_flag == SYSTEM_POWER_ON)
            {
                SoftReset();
            }
		    break;
        case 7:
   
            if(guji_mode == RECORED_IDLE)
            {
                change_to_runmode_recoder(1);
   
            }
            else 
            {
                keyTabIndex = MENU_SETTING_COOR;
                guji_icon_index = 1;
                key_type = KEY_LONG_ENTER;
                enter_perss = 2;
                KeyTab[keyTabIndex].CurrentOperate();
               //menu_keyprocess(4);
            }
	   	    break;

	}

    key_type = 0xff;

    if(home_flag == 3)
    {
        if(menu_info_update == 20)
        {
            sprintf((char *)dtbuf,"_");
            if(input_index<9)
                OLED_ShowString(input_jinweidu_post[input_index],32,dtbuf);
            else
                OLED_ShowString(input_jinweidu_post[input_index],48,dtbuf);
            //	LED0=!LED0;
            menu_info_update = 0;
        }
        else if(menu_info_update == 10)
        {
            sprintf((char *)dtbuf,"%c",input_memery[input_index]);
            if(input_index<9)
                OLED_ShowString(input_jinweidu_post[input_index],32,dtbuf);
            else
                OLED_ShowString(input_jinweidu_post[input_index],48,dtbuf);
    
            menu_info_update++;
        }
        else
            menu_info_update++;
    }


    if((KeyTab[keyTabIndex].subnumber != 'C')&&(enter_perss == 0)&&(KeyTab[keyTabIndex].subnumber != 'S'))
    {
        KeyTab[keyTabIndex].CurrentOperate();
    }
    else if((KeyTab[keyTabIndex].MenuID == MENU_TRANSFORMATION)||(KeyTab[keyTabIndex].ParMenuID == MENU_TRANSFORMATION))
        KeyTab[keyTabIndex].CurrentOperate();

    if(KeyTab[keyTabIndex].subnumber == 'C')
    {
        menu_status_dispplay(); 
    }
    

    if(KeyTab[keyTabIndex].MenuID == MENU_REALTIME)
        KeyTab[keyTabIndex].CurrentOperate();
    OLED_Refresh_Gram();


}

#if 0
void for_test_gps_gpx()
{
    Message_head_number = 0;
        
    gpsx->nshemi = 'N';
    gpsx->ewhemi = 'E';
    RTC_DateStructure.RTC_Year = 16;
    RTC_DateStructure.RTC_Month = 1;
    RTC_DateStructure.RTC_Date = 1;
    RTC_TimeStructure.RTC_Hours = 0;
    RTC_TimeStructure.RTC_Minutes = 0;
    RTC_TimeStructure.RTC_Seconds = 0;
    Altitude = -10000;
    gpsx->speed = 100;
    gpsx->angle = 1000;
    gpsx->latitude = 1;
    gpsx->longitude = 1;        
 
    pressure = 99990;
    temperature = -101;
    Flash_write_buffer_Index = 28;
    save_guiji_message('T');
    Message_head_number = 1;
    gpsx->nshemi = 'S';
    gpsx->ewhemi = 'W';
    RTC_DateStructure.RTC_Year = 16;
    RTC_DateStructure.RTC_Month = 12;
    RTC_DateStructure.RTC_Date = 31;
    RTC_TimeStructure.RTC_Hours = 23;
    RTC_TimeStructure.RTC_Minutes = 59;
    RTC_TimeStructure.RTC_Seconds = 59;
    Altitude = 300000;
    gpsx->speed = 2000000;
    gpsx->angle = 359000;
    gpsx->latitude = 899999999;
    gpsx->longitude = 179999999;   

    pressure = 100010;
    temperature = 3550;
       Flash_write_buffer_Index = 28;
    save_guiji_message('C');

}

#endif


