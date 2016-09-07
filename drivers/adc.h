#ifndef ADC_H
#define	ADC_H

//******************************************************************************
//  Секция включения заголовочных файлов
//******************************************************************************
#include <stdint.h>
#include <stdbool.h>

//******************************************************************************
//  Секция определения макросов
//******************************************************************************
/* Значение опорного в мВ */
#define ADC_VREF            3000

//******************************************************************************
//  Секция объявления типов
//******************************************************************************
typedef union
{
    struct
    {
        uint16_t ch0        : 1;
        uint16_t ch1        : 1;
        uint16_t ch2        : 1;
        uint16_t ch3        : 1;
        uint16_t ch4        : 1;
        uint16_t ch5        : 1;
        uint16_t ch6        : 1;
        uint16_t ch7        : 1;
        uint16_t ch8        : 1;
        uint16_t ch9        : 1;
        uint16_t ch10       : 1;
        uint16_t ch11       : 1;
        uint16_t ch12       : 1;
        uint16_t            : 3;
    }bits;
    uint16_t mask;
}tADC_Ch;

typedef enum
{
    vrINTERNAL_1_2V = 0,
    vrEXTERNAL,
}tADC_VRef;

typedef struct
{
    tADC_VRef vref_source;
    uint16_t vref_mv;
    uint32_t sample_rate;
}tADC_Config;

//******************************************************************************
//  Секция описания глобальных переменных
//******************************************************************************

//******************************************************************************
//  Секция прототипов глобальных функций
//******************************************************************************
void ADC_Init(tADC_Ch ch, const tADC_Config *cfg);
void ADC_Start(void);
void ADC_Stop(void);
uint16_t ADC_GetCode(uint8_t ch);
uint16_t ADC_GetVRef_mV(void);
uint16_t ADC_GetValue(uint8_t ch);
bool ADC_ConvIsComplete(void);
bool ADC_HandlerRegistration(uint8_t ch, void (*handler)(uint16_t *code));

//******************************************************************************
//  Конец файла
//******************************************************************************
#endif	// ADC_H