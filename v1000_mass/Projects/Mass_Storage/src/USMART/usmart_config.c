#include "usmart.h"
#include "usmart_str.h" 
////////////////////////////�û�������///////////////////////////////////////////////
//������Ҫ�������õ��ĺ�����������ͷ�ļ�(�û��Լ����) 
#include "delay.h"	
#include "usart.h"		
#include "sys.h"
//#include "lcd.h"
#include "menutal.h"
#include "rtc.h"
#include "flash.h"												 

extern void led_set(u8 sta);
extern void test_fun(void(*ledset)(u8),u8 sta);
//�������б��ʼ��(�û��Լ����)
//�û�ֱ������������Ҫִ�еĺ�����������Ҵ�
struct _m_usmart_nametab usmart_nametab[]=
{
#if USMART_USE_WRFUNS==1 	//���ʹ���˶�д����
	(void*)read_addr,"u32 read_addr(u32 addr)",
	(void*)write_addr,"void write_addr(u32 addr,u32 val)",	 
#endif
//	(void*)delay_ms,"void delay_ms(u16 nms)",
//	(void*)delay_us,"void delay_us(u32 nus)",	 
  //     (void*)menu_keyprocess,"void menu_keyprocess(u8 key)",
	(void*)rtc_set,"void  rtc_set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec,u8 week)",	 
//	(void*)test_read_flash_and_v1000_debug,"test_read_flash_and_v1000_debug(u32 WriteAddr, u32 NumByteToRead)",
	(void*) read_gps,"void read_gps(void)",
	(void*) read_gps_next,"void read_gps_next(void)",
	(void*) stop_read_gps,"void stop_read_gps(void)",
//	(void*) read_last_sector,"void read_last_sector(void)",
//	(void*) Choose_power_mode,"void Choose_power_mode(u8 Mcu_State)",
															  
};						  
///////////////////////////////////END///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//�������ƹ�������ʼ��
//�õ������ܿغ���������
//�õ�����������
struct _m_usmart_dev usmart_dev=
{
	usmart_nametab,
	usmart_init,
	usmart_cmd_rec,
	usmart_exe,
	usmart_scan,
	sizeof(usmart_nametab)/sizeof(struct _m_usmart_nametab),//��������
	0,	  	//��������
	0,	 	//����ID
	1,		//������ʾ����,0,10����;1,16����
	0,		//��������.bitx:,0,����;1,�ַ���	    
	0,	  	//ÿ�������ĳ����ݴ��,��ҪMAX_PARM��0��ʼ��
	0,		//�����Ĳ���,��ҪPARM_LEN��0��ʼ��
};   



















