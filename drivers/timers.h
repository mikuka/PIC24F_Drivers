#ifndef TIMERS_H
#define	TIMERS_H

//******************************************************************************
//  Секция включения заголовочных файлов
//******************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "irq.h"

//******************************************************************************
//  Секция определения макросов
//******************************************************************************
/* Кол-во аппаратных таймеров */
#define NTIMERS				5

//******************************************************************************
//  Секция объявления типов
//******************************************************************************
/* Имена таймеров */
typedef enum
{
	T1 = 0,
	T2,
	T3,
	T4,
	T5,
}tTimer;

typedef enum
{
    TIM_PRSC1 = 0,
    TIM_PRSC8 = 1,
    TIM_PRSC64 = 2,
    TIM_PRSC256 = 3,            
}tTimPrescaler;

/* Структура с ресурсами таймеров */
struct tTimerData
{
	volatile uint16_t *cnt_reg;
	volatile uint16_t *cfg_reg;
	volatile uint16_t *period_reg;
	volatile uint16_t *compare_reg;
//	volatile uint16_t *iec_reg;
//	uint16_t value_psc;
	uint32_t clock;
        tIRQ_Num irqn;
};

//******************************************************************************
//  Секция описания глобальных переменных
//******************************************************************************

//******************************************************************************
//  Секция прототипов глобальных функций
//******************************************************************************
void Timer16_Init(tTimer timer, tTimPrescaler prescaler);
void Timer16_DisInterrupt(tTimer timer);
void Timer16_EnInterrupt(tTimer timer);
bool Timer16_SetPeriodUs(tTimer timer, uint32_t usec);
bool Timer16_SetPeriodMs(tTimer timer, uint32_t msec);
void Timer16_Start(tTimer timer);
void Timer16_Stop(tTimer timer);
bool Timer16_HandlerRegistration(tTimer timer, void (*handler)(void *), void **arg);
uint32_t Timer16_GetTimeoutUs(tTimer timer);
const uint32_t GetSysTickCounter(void);

//******************************************************************************
//  Конец файла
//******************************************************************************
#endif	// TIMERS_H
