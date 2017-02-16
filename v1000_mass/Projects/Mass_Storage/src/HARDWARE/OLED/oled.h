#ifndef __OLED_H
#define __OLED_H			  	 
#include "sys.h"
#include "stdlib.h"	    
//////////////////////////////////////////////////////////////////////////////////	 

////////////////////////////////////////////////////////////////////////////////// 	  


//OLED模式设置
//0:4线串行模式
//1:并行8080模式
#define OLED_MODE 1
		    						  
//-----------------OLED端口定义----------------  					   
#define OLED_RST PBout(10)
#define OLED_CS PBout(11)
//#define OLED_RST  PBout(14)//在MINISTM32上直接接到了STM32的复位脚！	
#define OLED_RS PBout(12)
#define OLED_WR PBout(13)		  
#define OLED_RD PBout(14)

//PB0~7,作为数据线
#define DATAOUT(x) GPIOB->ODR=(GPIOB->ODR&0xff00)|(x&0x00FF); //输出

//#define DATAOUT(DataValue) {GPIO_Write(GPIOB,(GPIO_ReadOutputData(GPIOB)&0xff00)|(DataValue&0x00FF));}  

//使用4线串行接口时使用 
#define OLED_SCLK PBout(0)
#define OLED_SDIN PBout(1)
		     
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
//OLED控制用函数
void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);		   
							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y,const u8 *p);	 
void OLED_Fill_icon(u8 x1,u8 y1,u8 *icon)  ;
void Movie_Show_Img(u16 x,u16 y,const u8 *icon);
void OLED_ShowString_head(u8 x,u8 y,const u8 *p);
void OLED_ShowString12(u8 x,u8 y,const u8 *p);
void OLED_DrawVLine  (int x, int y0,  int y1) ;
void Movie_Show_Img_force1616(u16 x,u16 y,u8 index);
void Movie_Show_Img_3(u16 x,u16 y,const u8 *icon);
void OLED_ShowChar_head(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowString_24(u8 x,u8 y,const u8 *p);
void OLED_Refresh_Gram_low(void);
void OLED_DrawVLine_visual(int x, int y0,  int y1) ;
/*font extern */



extern const u8 BMPdongbei[];
extern const u8 BMPxibei[];
extern const u8 BMPzhengbei[];
extern const u8 BMPxinan[];
extern const u8 Bmpzhengnan[];
extern const u8 Bmpzhengxi[];
extern const u8 Bmpzhengdong[];
extern const u8 Bmpdongnan[];


extern const u8 fangxianbmp[];
extern const u8 bmpfangxian90[];
extern const u8 bmpfangxian180[];
extern const u8 bmpfangxian270[];
extern const u8 fangxianbmp22[];
extern const u8 fangxianbmp45[];
extern const u8 fangxianbmp67[];
extern const u8 bmpfangxian112[];
extern const u8 bmpfangxian135[];
extern const u8 bmpfangxian157[];
extern const u8 bmpfangxian202[];
extern const u8 bmpfangxian225[];
extern const u8 bmpfangxian247[];
extern const u8 bmpfangxian270[];
extern const u8 bmpfangxian292[];
extern const u8 bmpfangxian314[];
extern const u8 bmpfangxian336[];
//extern const u8 donnanword[];

extern const u8 bmp_worddong[];
extern const u8 bmp_wordBei[];
extern const u8 bmp_wordXi[];
extern const u8 bmp_wordnan[];

extern const unsigned char Bmp_updwon[];
extern const unsigned char Bmp_ponit[];
extern const u8 Bmp_system_head[];
extern const u8 Bmp_arr_ajust[];
extern const u8 Bmp_arr_ajust_1[];
extern const u8 Bmp_sreen_stanby[];
extern const u8 Bmp_sreen_stanby_1[];
extern const u8 Bmp_timer_zone[];
extern const u8 Bmp_lan[];
extern const u8 Bmp_lan_1[];
extern const u8 Bmp_yes_or_no[];
extern const u8 Bmp_factroy[];
extern const u8 unplug_usb[];

extern const u8 bmp_delect_record_word[];
extern const u8 bmp_powerdownword[];
extern const u8 bmp_system_word[];
extern const u8 Bmp_wannersonud[];

extern const u8 BMP_SETTING_HEAD[];
extern const u8 BMP_SETTING_HEAD_1[];
extern const u8 BMP_SETTING_HEAD_2[];
extern const u8 BMP_SETTING_RUNMODE[];
extern const u8 bmp_unit_head[];
extern const u8 bmp_timer[];
extern const u8 bmp_data[];
extern const u8 bmp_crood[];
extern const u8 bmp_distance_hwe[];
extern const u8 bmp_temp[];
extern const u8 bmp_pres[];

extern const u8 Bmp_time1[];
extern const u8 Bmp_data1[];
extern const u8 Bmp_coord1[];
extern const u8 Bmp_temp1[];
extern const u8 Bmp_temp2[];
extern const u8 Bmp_pres_1[];
extern const u8 Bmp_pres_2[];

extern const unsigned char Bmp_personal_head[];
extern const unsigned char Bmp_weigt[];
extern const unsigned char Bmp_heigt[];
extern const u8 Bmp_xinbie[];
extern const u8 Bmp_manfalem[];
extern const unsigned char firmwareupdate[];
extern const unsigned char nofirmware[];
extern const unsigned char confirm[];

extern const unsigned char Bmp_24hour_downline[];
extern const unsigned char Bmp_speed_word[];
extern const unsigned char Bmp_temp_word[];
extern const unsigned char Bmp_attide_word[];
extern const unsigned char Bmp_pree_word[];
extern const unsigned char Bmp_4hour_downline[];
extern const unsigned char Bmp_change_upline[];

extern const u8 bmp_system_status[];
extern const u8 bmpdu[];
extern const u8 BMP_logo[];
extern const u8 bmpmaio[];
extern const u8 bmpfen[];

extern const u8 zhunquedu[];
extern const u8 bmp_jinwei[];


extern const u8 BMp_0[];
extern const u8 BMp_1[];
extern const u8 BMp_2[];
extern const u8 BMp_3[];
extern const u8 BMp_4[];
extern const u8 BMp_5[];
extern const u8 BMp_6[];
extern const u8 BMp_7[];
extern const u8 BMp_8[];
extern const u8 BMp_9[];


extern const u8 BMPWaitdinwei[];
extern const u8 BMP3Ddinwei[];
extern const u8 BMP2Ddinwei[];



extern const u8 bmpyuji[];
extern const u8 bmp_near_place[];
extern const u8 bmp_choose_post[];
extern const u8 bmp_choosed[];
extern const u8 bmp_destination_updown[];
extern const u8 Bmp_input_jiweidu[];
extern const u8 Bmp_lock[];


extern const unsigned char Bmp_leiji[];
extern const unsigned char Bmp_sept[];
extern const unsigned char Bmp_sart[];
extern const unsigned char Bmp_dian[];
extern const unsigned char Bmp_daka[];


extern const unsigned char Bmp_savedguji[];
extern const unsigned char Bmp_gujiBI[];

extern const unsigned char Bmp_xinqudian[];
extern const unsigned char Bmp_xingqudiansaved[];
extern const u8 Bmp_peishu[];
extern const u8 bmp_flash_full[];


extern const u8 BMP_CHOOSE1[];
extern const u8 BMP_CHOOSE2[];
extern const u8 BMP_CHOOSE3[];
extern const u8 BMP_SETTING_RETURN[];

extern const u8 bmp_autoreocrd_1[];


extern const u8 BMP_RECORD_HEAD[];
extern const u8 BMP_BYDICTANCE[];
extern const u8 BMP_BYtime[];
extern const u8 BMP_BYspeed[];
extern const u8 bmp_jilufashi[];
extern const u8 bmp_jilufashi_1[];
extern const u8 bmp_CustomRecored[];
extern const u8 bmp_autoreocrd[];
extern const u8 bmp_delect_record_word[];
extern const u8 bmp_guji_remove_recode[];
extern const u8 Bmp_deleting_guji[];

extern const u8 bmp_consure_powerdown[];
extern const u8 bmp_consure_powerdown_en[];


extern const unsigned char Bmp_chageing[];
extern const unsigned char Bmp_chage_ok[];
extern const unsigned char Bmp_batt_0[];
extern const unsigned char Bmp_batt_1[];
extern const unsigned char Bmp_batt_2[];
extern const unsigned char Bmp_batt_3[];

extern const unsigned char Bmp_singe_no[];
extern const unsigned char Bmp_singe_1[];
extern const unsigned char Bmp_singe_2[];
extern const unsigned char Bmp_singe_3[];

extern const unsigned char Bmp_bigam[];
extern const unsigned char Bmp_bigpm[];
extern const unsigned char Bmp_bigam_1[];
extern const unsigned char Bmp_bigpm_1[];

extern const unsigned char Bmp_waitsinge[];

extern const unsigned char Bmp_am[];
extern const unsigned char Bm_pm[];

extern const unsigned char Bmp_wanner[];
extern const unsigned char Bmp_100[];

extern const unsigned char Bmp_gujiiconsetp[];
extern const unsigned char Bmp_gujiiconrun[];
extern const unsigned char Bmp_gujiiconB[];
extern const unsigned char Bmp_gujiiconcar[];
extern const unsigned char Bmp_gujiiconfly[];
extern const unsigned char Bmp_gujiiconCustom[];

extern const unsigned char bmpchaoxi[];

extern const unsigned char Bmp_gujiiconsetp[];
extern const unsigned char Bmp_gujiiconrun[];
extern const unsigned char Bmp_gujiiconB[];
extern const unsigned char Bmp_gujiiconcar[];
extern const unsigned char Bmp_gujiiconfly[];
extern const unsigned char Bmp_gujiiconCustom[];



extern const unsigned char Bmp_wordbuxing[];
extern const unsigned char Bmp_wordrun[];
extern const unsigned char Bmp_wordB[];
extern const unsigned char Bmp_wordCar[];
extern const unsigned char Bmp_feixing[];
extern const unsigned char Bmp_wordcustom[];

extern const unsigned char Bmp_batt_0[];
extern const unsigned char Bmp_fontformats[];
extern const unsigned char Bmp_fontguji[];

#endif  
	 



