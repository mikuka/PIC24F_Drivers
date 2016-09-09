#ifndef SOFT_TIMERS_H
#define	SOFT_TIMERS_H

//******************************************************************************
//  Секция включения заголовочных файлов
//******************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "../drivers/timers.h"

//******************************************************************************
//  Секция определения макросов
//******************************************************************************
/* Макс-ное кол-во доступных программных таймеров */
#define NSOFTTIMERS         5

//******************************************************************************
//  Секция объявления типов
//******************************************************************************



//******************************************************************************
//  Секция описания глобальных переменных
//******************************************************************************

//******************************************************************************
//  Секция прототипов глобальных функций
//******************************************************************************
void SoftTimer_Init(tTimer hw_timer);
int8_t SoftTimer_Register(void (*callback)(void));
bool SoftTimer_SetPeriodMs(uint8_t num, uint32_t msec);
void SoftTimer_Start(uint8_t num);
void SoftTimer_Stop(uint8_t num);

//******************************************************************************
//  Конец файла
//******************************************************************************
#endif	// SOFT_TIMERS_H
