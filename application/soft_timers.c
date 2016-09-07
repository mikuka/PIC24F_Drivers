//******************************************************************************
//  Секция включения заголовочных файлов
//******************************************************************************
#include "soft_timers.h"
#include "../delay.h"

//******************************************************************************
//  Секция определения макросов
//******************************************************************************


//******************************************************************************
//  Секция объявления типов
//******************************************************************************
struct tSoftTimer
{
	tTimer hw_timer;
	int8_t max_number;
	void (*handler[NSOFTTIMERS])(void);
	bool enable[NSOFTTIMERS];
	uint32_t counter[NSOFTTIMERS];
	uint32_t period[NSOFTTIMERS];
};

//******************************************************************************
//  Секция определения констант
//******************************************************************************

//******************************************************************************
//  Секция описания внешних переменных
//******************************************************************************

//******************************************************************************
//  Секция описания внешних переменных
//******************************************************************************

//******************************************************************************
//  Секция объявления локальных переменных
//******************************************************************************
struct tSoftTimer SoftTimers;



//******************************************************************************
//  Секция прототипов функций
//******************************************************************************

//******************************************************************************
//  Секция локальных функций
//******************************************************************************

static void Handler(void *arg)
{
	uint8_t index;
	for (index = 0; index < SoftTimers.max_number; index++) {
		if (SoftTimers.enable[index] != true)
			continue;
		if (!SoftTimers.handler[index])
			continue;
		if (++SoftTimers.counter[index] == SoftTimers.period[index]) {
			SoftTimers.counter[index] = 0;
			SoftTimers.handler[index]();
		}
	}
}

//******************************************************************************
//  Секция глобальных функций
//******************************************************************************

void SoftTimer_Init(tTimer hw_timer)
{
	SoftTimers.hw_timer = hw_timer;
	Timer16_HandlerRegistration(hw_timer, Handler, 0);
}

int8_t SoftTimer_Register(void (*callback)(void))
{
	int8_t number = SoftTimers.max_number;

	if (++SoftTimers.max_number > NSOFTTIMERS)
		return -1;
	SoftTimers.handler[number] = callback;

	return number;
}

bool SoftTimer_SetPeriodMs(uint8_t num, uint32_t msec)
{
	if (num < 0 || num >= SoftTimers.max_number)
		return false;
	SoftTimers.period[num] = msec / (Timer16_GetTimeoutUs(SoftTimers.hw_timer) / 1000);
	return true;
}

void SoftTimer_Start(uint8_t num)
{
	if (num >= 0 || num < SoftTimers.max_number) {
		SoftTimers.counter[num] = 0;
		SoftTimers.enable[num] = true;
	}
}

void SoftTimer_Stop(uint8_t num)
{
	if (num >= 0 || num < SoftTimers.max_number)
		SoftTimers.enable[num] = false;
}

//******************************************************************************
//  Конец файла
//******************************************************************************