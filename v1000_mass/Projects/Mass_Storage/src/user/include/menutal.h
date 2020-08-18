#ifndef __MENUTAL_H
#define __MENUTAL_H	 


#define SYSTEM_POWER_STANBY 0
#define SYSTEM_POWER_ON 1
#define SYSTEM_POWER_LOW  2
#define SYSTEM_POWER_LOW_2  3
#define SYSTEM_POWER_ON_USB 4



#define RECORED_IDLE 0
#define RECORED_START 1
#define RECORED_START_DOING 2
#define RECORED_T  3
#define RECORED_STOP 4


#define KEY_UP 0
#define KEY_DOWN 1
#define KEY_ENTER 2
#define KEY_LONG_ENTER 3


/****���ò˵�*****/
#define MENU_SETTING_MAIN_PAGE 0
#define MENU_SETTING_SEC_PAGE 1
#define MENU_SETTING_THR_PAGE 2
#define MENU_SETTING_FOR_PAGE 3

#define PAGE_RUN_MODEL 0
#define PAGE_GUJI_RECORD 1
#define PAGE_GESHI_DANGWEI 2
#define PAGE_GEREN_ZHILIAO 3
#define PAGE_SYSTEM_SET 4
#define PAGE_QUIT 5


#define GUJI_FORMATS_CSV 0
#define GUJI_FORMATS_GPX 1
#define GUJI_FORMATS_GPS 2



typedef enum
{
  SENSOR_MODEL = 0,
  GPS_MODEL,
}RUN_MODEL;

typedef enum
{
  CUSTOM_RECORED,
  RECORED_MODEL,
  AUTO_MODEL,
  RECORED_FORMAT,
  DELECT_ALL,
  CUTOME_RECORED_RETURN,
}CUTOME_RECORED;


typedef enum
{
  UNIT_TIME = 0,
  UNIT_DATE,
  UNIT_COORD,
  UNIT_DISCANCE,
  UNIT_TEMP,
  UNIT_PRES,
  UNIT_REUTNR,

}UNIT_MODEL;

typedef enum
{
    PERSONAL_FALEM = 0,
    PERSONAL_HEIGHT ,
    PERSONAL_WEIGHT,
    PERSONAL_REUTRN,
}PERSONAL_data;

typedef enum
{
    SYSTEM_ADR_ADJUST = 0,
    SCREEN_STANBY,
    POWERDONW_CONTRL,
    TIMER_ZONE,
    WANNER_SOUND,
   // STORM_ALARM,
    LANGUAGE,
    SYSTEM_INFO,
    FACTROY,
    FIRMWARE_UPDATE,
    SYSTEM_RETURN,
}SYSTEM_SETS;



typedef enum
{
    BY_DISTANCE = 0,
    BY_TIMES,
    BY_SPEED,
    RECORED_MODEL_OPTIONS_RETURN,
}RECORED_CUSTOM_OPTIONS;


typedef enum
{
    FORMATS_CSV = 0,
    FORMATS_GPX,
    FORMATS_GPS,
}RECORED_FORMATS_OPTIONS;


typedef enum
{
    AUTO_RECOVRY = 0,
    AUTO_STOP,
    AUTO_MODEL_OPTIONS_RETURN,
}RECORED_MODEL_OPTIONS;

typedef enum
{
    AUTO_ON = 0,
    AUTO_OFF,
}AUTO_MODEL_OPTIONS;



void is_update_to_poweron(void);
void check_time(u8 flag_update);
void menu_keyprocess(u8 key);
void menu_real_speed_display(void);
void menu_main_display(void);
void menu_temp_change_display(void);	
void menu_Pressure_display(void);
void menu_speed_change_display(void);
void menu_3D_display(void);
void read_poi_file(void);
void flash_getfree(void);
void menu_home_display(void);
void save_tsp_s(void);
void save_tsp_tp(void);
void menu_chaoxi_display(void);
void menu_guiji_display(void);
void save_the_interest_place(u32 lat1, char latitude1_flag,u32 lng1, char longitude1_flag,u8 index);
void Recording_guji(void);
void  show_levev_space(void);
void Send_to_goto_lowpower(void);
void write_flash(void);
void restore_pass(void);
void guji_Distance(void);	
void menu_enter_flash(u16 my_timer_flash);
void check_eeprom_init(void);
void menu_3d_seq(void);
void menu_status_dispplay(void);
extern int16_t Max_arry(int16_t* prt,u8 index);
extern int16_t Min_arry(int16_t* prt,u8 index);
extern s32 Max_arry32(s32* prt,u8 index);
extern s32 Min_arry32(s32* prt,u8 index);
extern void di_and_flash_empet(u8 times);
extern void show_baoyu_bmp(void);
extern void change_clk_need_int(u8 mode);
extern void Del_oldfile_nospace(void);
extern void is_recoreding(void);
extern void mkdir_init(unsigned char mode);
void for_test_gps_gpx(void);
extern  void Del_allgujifile(void);
void menu_reflash(void);
void test_angle(void);

#endif

