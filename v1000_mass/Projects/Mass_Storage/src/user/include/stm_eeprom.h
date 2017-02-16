#ifndef __STM_EEPROM_H
#define __STM_EEPROM_H



void stm_write_eerpom(u16 address,u32 data);
void stm_fast_write_eerpom(u8 address,u32 data);
void stm_read_eerpom(u32 address,u32 *data);
u8 stm_EraseWord_Eeprom(void);




#define CUURENT_FLASH_ADDRER 0x00   /*当前存储的总目*/
#define CUURENT_BUUFER_INDEX_ADDRER 0x01  /*当前BUFFER idex*/

#define HOME_WEIDU_ADDER  0x02
#define HOME_WEIDU_FLAG_ADDER  0x03
#define HOME_JINDU_ADDER  0x04

#define GUJI_INTERST_POS (HOME_JINDU_ADDER+37)

#define GUJI_STRAT_WEIDU (GUJI_INTERST_POS+1)
#define GUJI_STRAT_WEIDU_FLAG (GUJI_STRAT_WEIDU+1)
#define GUJI_STRAT_JINDU (GUJI_STRAT_WEIDU_FLAG+1)

#define GUJI_S_T_TOATL_NUMBER (GUJI_STRAT_JINDU+1)

#define GUJI_S_START_1_ADDERS (GUJI_S_T_TOATL_NUMBER+1)
#define GUJI_S_T_1_NUMBER_ADDERS (GUJI_S_START_1_ADDERS+1)


#define HOME_NUMBER1_INDE (GUJI_S_T_1_NUMBER_ADDERS+1)
#define HOME_NUMBER2_INDE (HOME_NUMBER1_INDE+1)
#define HOME_NUMBER3_INDE (HOME_NUMBER2_INDE+1)
#define HOME_NUMBER4_INDE (HOME_NUMBER3_INDE+1)



#define MENU_SETTING_INDEX_ADDRES (HOME_NUMBER4_INDE+1)


#define MENU_SETTING_RUNMODEL_ADDRES (MENU_SETTING_INDEX_ADDRES+4)
#define MENU_SETTING_GUJI_BY_MODE (MENU_SETTING_RUNMODEL_ADDRES+1)
#define MENU_SETTING_GUJI_BY_DISTANCE (MENU_SETTING_GUJI_BY_MODE+1)
#define MENU_SETTING_GUJI_BY_TIME (MENU_SETTING_GUJI_BY_DISTANCE+1)
#define MENU_SETTING_GUJI_BY_SPEED (MENU_SETTING_GUJI_BY_TIME+1)
#define MENU_SETTING_GUJI_RECOREDMETH (MENU_SETTING_GUJI_BY_SPEED+1)
#define MENU_SETTING_RECODED_AUTOFLAG (MENU_SETTING_GUJI_RECOREDMETH+1)

#define MENU_SETTING_UNIT_TIMER (MENU_SETTING_RECODED_AUTOFLAG+1)
#define MENU_SETTING_UNIT_DATA (MENU_SETTING_UNIT_TIMER+1)
#define MENU_SETTING_UNIT_COORD (MENU_SETTING_UNIT_DATA+1)
#define MENU_SETTING_UNIT_HW (MENU_SETTING_UNIT_COORD+1)
#define MENU_SETTING_UNIT_TMPE (MENU_SETTING_UNIT_HW+1)
#define MENU_SETTING_UNIT_PREE (MENU_SETTING_UNIT_TMPE+1)

#define MENU_SETTING_FALME (MENU_SETTING_UNIT_PREE+1)
#define MENU_SETTING_HEIGHT (MENU_SETTING_FALME+1)
#define MENU_SETTING_WEIGHT (MENU_SETTING_HEIGHT+1)

#define MENU_SETTING_AJUST_ARR (MENU_SETTING_WEIGHT+1)
#define MENU_SETTING_AJUST_FLAG (MENU_SETTING_AJUST_ARR+1)
#define MENU_SETTING_SCREER_TIMER (MENU_SETTING_AJUST_FLAG+1)
#define MENU_SETTING_POWERCTROL (MENU_SETTING_SCREER_TIMER+1)
#define MENU_SETTING_TIMER_ZONE (MENU_SETTING_POWERCTROL+1)
#define MENU_SETTING_WANNER_SOUND (MENU_SETTING_TIMER_ZONE+1)
#define MENU_SETTING_STROMALRRM (MENU_SETTING_WANNER_SOUND)
#define MENU_SETTING_LANG (MENU_SETTING_STROMALRRM+1)




#define CHANGE_TMPE_VAULE_INDEX_ADDRES (MENU_SETTING_LANG+1)
#define CHANGE_SPEED_VAULE_INDEX_ADDRES (CHANGE_TMPE_VAULE_INDEX_ADDRES+1)
#define CHANGE_ALTITUD_VAULE_INDEX_ADDRES (CHANGE_SPEED_VAULE_INDEX_ADDRES+1)

#define  MENU_GUJI_MODE_INDEX_ADDRES (CHANGE_ALTITUD_VAULE_INDEX_ADDRES+1)
#define  MENU_POWERDOWNTIME_INDEX_ADDRES (MENU_GUJI_MODE_INDEX_ADDRES+1)


#define  MENU_FRIST_POWER_INDEX_ADDRES (MENU_POWERDOWNTIME_INDEX_ADDRES+1)
#define  MENU_FRIST_POWERDATE_INDEX_ADDRES (MENU_FRIST_POWER_INDEX_ADDRES+1)
#define  MENU_POWER_STATE_INDEX_ADDRES (MENU_FRIST_POWERDATE_INDEX_ADDRES+1)

#define MENU_SETTING_GUJI_BY_SPEED_1 (MENU_POWER_STATE_INDEX_ADDRES+1)
#define MENU_SETTING_GUJI_BY_SPEED_2 (MENU_SETTING_GUJI_BY_SPEED_1+1)

#define MENU_SETTING_HEIGHT_1 (MENU_SETTING_GUJI_BY_SPEED_2+1)
#define MENU_SETTING_WEIGHT_1 (MENU_SETTING_HEIGHT_1+1)

#define MENU_SETTING_AJUST_ARR_1 (MENU_SETTING_WEIGHT_1+1)

#define MENU_FLASH_FULL_FLAG (MENU_SETTING_AJUST_ARR_1+1)

#define MENU_FILE_NUMBER (MENU_FLASH_FULL_FLAG+1)
#define MENU_SETTING_RECODED_FORMATS (MENU_FILE_NUMBER+1)


#define FLASH_16M 1

#ifdef FLASH_2M
#define MOST_RECOED_MUMBER 65407
#define MOST_FLASH_ADDRES 0x1FF000
#elif FLASH_8M 
#define MOST_RECOED_MUMBER 261628
#define MOST_FLASH_ADDRES 0x7FF000
#elif FLASH_16M 
#define MOST_RECOED_MUMBER 261628
#define MOST_FLASH_ADDRES 0xFFF000
#endif

#define CHANGE_PRE_VAULE_SAVE_ADDRES MOST_FLASH_ADDRES
#define CHANGE_TMPE_VAULE_SAVE_ADDRES (MOST_FLASH_ADDRES+384)
#define CHANGE_SPEED_VAULE_SAVE_ADDRES (CHANGE_TMPE_VAULE_SAVE_ADDRES+192)
#define CHANGE_ALTITUD_VAULE_SAVE_ADDRES (CHANGE_SPEED_VAULE_SAVE_ADDRES+192)




#endif


