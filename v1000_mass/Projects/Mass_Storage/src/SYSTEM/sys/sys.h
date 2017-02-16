#ifndef __SYS_H
#define __SYS_H	
#include "stm32l1xx.h"

#include "misc.h"
//#include "stm32l1xx_map.h"
//#include "stm32l1xx_nvic.h" 

//////////////////////////////////////////////////////////////////////////////////	 

//******************************************************************************** 
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO�ڵ�ַӳ��
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40011E08 
 
//IO�ڲ���,ֻ�Ե�һ��IO��!
//ȷ��n��ֵС��16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //��� 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //���� 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //��� 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //���� 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //��� 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //���� 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //��� 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //����

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //��� 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //����

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //��� 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //����



//JTAGģʽ���ö���
#define JTAG_SWD_DISABLE   0X02
#define SWD_ENABLE         0X01
#define JTAG_SWD_ENABLE    0X00	

void SoftReset(void) ;
void NVIC_Configuration(void);
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)	 ;
void JTAG_Set(u8 mode);
void Stm32_Clock_Init(u8 PLL);  //ʱ�ӳ�ʼ��  
void Sys_Soft_Reset(void);      //ϵͳ��λ
void Sys_Standby(void);         //����ģʽ 	
void convert_into_char(uint32_t number, uint16_t *p_tab);
void MyLinRots_ErrorStateProcess(void);
void MyLinRots_OffStateProcess(void);


typedef struct  
{	
	u8 flame;
	u16 height;
	u16 weight;
	u16 height_ft;
	u16 weight_ft;
}personal_data; 

typedef struct  
{		
	u8 by_distance_time_flag;
	u16 by_distance_vaule;
	u16 by_distance_vaule_ft;
	u8 by_time_vaule;
	u16 by_speed_vaule;
	u16 by_speed_vaule_1;
	u16 by_speed_vaule_2;
	u8 recoed_meth;  /*�Զ����ǻ��Ǽ���ֹͣ*/
	u8 auto_recoed_flag;
    u8 recoed_formats;
}GUJI_RECOCE_TABLE; 

typedef struct  
{										    
	u8 TIMER;
	u8 DATA;
	u8 COORD;
	u8 H_W;  /*�Զ����ǻ��Ǽ���ֹͣ*/
	u8 TEMP;
	u8 PREE;
}UNIT_format; 

typedef struct  
{										    
	u8 run_mode;
	GUJI_RECOCE_TABLE guji_record;
	UNIT_format myunit;
	personal_data personal_da;	
	s32 ajust_arr;
//	s32 ajust_arr_1;
	u8 ajust_Arr_flag;
	u8 screen_timer;
	u8 time_zone;
	u8 lang;
	u8 batt_Status;
	u8 charger_connected;
	u16 powerdown_timers;
	u8 usb_powerdown_flash;
	u8 updatefile;
	u8 wanner_sound;
	u8 usb_standy_poweron_flag;
	float index_timerzone;
    u8 usbdisk_flag;
    u8 recored_run_flag;
    u8 gujiFormats;
    u16  To_lower_timercnt;
    u8 baifenbi;
    u32 batt_change_ok_cnt;

}system_flag; 

extern system_flag *system_flag_table;




#define GPIO_HIGH(a,b) 		a->BSRRL = b
#define GPIO_LOW(a,b)		a->BSRRH = b
#define GPIO_TOGGLE(a,b) 	a->ODR ^= b 

#define NoRTC FALSE
#define WITHRTC !NoRTC
#define NoDIV2 FALSE
#define DIV2 !NoDIV2


#define MCU_RUN		0
#define MCU_SLEEP       1
#define MCU_LP_RUN      2
#define MCU_LP_SLEEP    3
#define MCU_STOP_RTC	4
#define MCU_STOP_NoRTC	5
#define MCU_STBY	6
#define MCU_RUN_USB 7

void GPS_power_ctrl(u8 ctrl);
void board_io_init(void);
void SetHSICLKToMSI(u32 freq,u8 div2,u8 With_RTC);
void Choose_power_mode(u8 Mcu_State);
//#define NO_BMP085

#endif
