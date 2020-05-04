#include "stm32l1xx.h"

void BMP085_get_process(void);
void	Init_BMP085(void);                //��ʼ��BMP085 
void conversion(long temp_data);
void ConvAltitude(void);
int BMP085_Get_Altitude(void);
int bmp_moudle_preinit(void);

