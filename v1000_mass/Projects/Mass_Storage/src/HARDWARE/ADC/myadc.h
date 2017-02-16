#ifndef MY_ADC_H
#define MY_ADC_H
/* Private function prototypes -----------------------------------------------*/
void ADC_Config(void);

void DisplayIDDrunmA(uint32_t IDDmeas);

void vddmv_adc_proess(void);


#define BATT_CHARG_OK 0x05
#define BATT_EMPTY 0x00
#define BATT_HIGH 0x03
#define BATT_LOW 0x01
#define BATT_MID 0x02

#define BATT_CHARGING 0x04


/* Private functions ---------------------------------------------------------*/

#endif
