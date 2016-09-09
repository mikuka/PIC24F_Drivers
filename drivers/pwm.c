//******************************************************************************
//  Секция включения заголовочных файлов
//******************************************************************************
#include <p24FJ64GA004.h>
#include "pwm.h"
#include "../delay.h"

#include <xc.h>
#include <libpic30.h>
//#include <PPS.h>

//******************************************************************************
//  Секция определения макросов
//******************************************************************************
#define OFFSET		( sizeof(CompareBase) / sizeof(CompareBase.comparesec_reg) )

//******************************************************************************
//  Секция объявления типов
//******************************************************************************
/* Структура с ресурсами таймеров */
struct tTimerCompare
{
	volatile uint16_t *comparesec_reg;
	volatile uint16_t *compare_reg;
	volatile uint16_t *config_reg;
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
struct tTimerCompare CompareBase = {
	.comparesec_reg =	&OC1RS,
	.compare_reg =		&OC1R,
	.config_reg =		&OC1CON,
};
	

//******************************************************************************
//  Секция прототипов функций
//******************************************************************************

extern uint32_t Timer16_GetValueClock(tTimer timer);
extern struct tTimerData *Timer16_GetResource(tTimer timer);

//******************************************************************************
//  Секция локальных функций
//******************************************************************************

//******************************************************************************
//  Секция глобальных функций
//******************************************************************************

bool Timer16_PWM_Setup(tTimer timer, tTimerChx channel, uint32_t freqHz)
{
	/*PWM Period = (PR2 + 1) • TCY • (Timer2 Prescale Value)
	PWM Period = PR2 • TCY • Timer2 Prescale Value + TCY • Timer2 Prescale Value
	PR2 • TCY • Timer2 Prescale Value = PWM Period - TCY • Timer2 Prescale Value
	PR2 = (PWM Period - TCY • Timer2 Prescale Value) / TCY • Timer2 Prescale Value
	PR2 = PWM Period / (TCY • Timer2 Prescale Value) - 1
	PR2 = CY / PWM Freq • Timer2 Prescale Value - 1*/
	uint32_t pwm_period = Timer16_GetValueClock(timer) / freqHz - 1;
	if (pwm_period > UINT16_MAX)
		return false;

	volatile uint16_t *cfg_reg = CompareBase.config_reg;
	cfg_reg += OFFSET * channel;
	*cfg_reg = (0x06 << _OC1CON_OCM_POSITION);	// включение режима ШИМ
	if (timer != T2) {
		if (timer == T3)
			*cfg_reg |= (0x01 << _OC1CON_OCTSEL_POSITION); // источник - таймер 3
		else
			return false;
	}
	*Timer16_GetResource(timer)->period_reg = pwm_period;
	return true;
}

void Timer16_PWM_SetDutyCycle(tTimer timer, tTimerChx channel, float duty_cycle)
{
	*(CompareBase.compare_reg + OFFSET * channel) = 1;
//	duty_cycle = 100.0 - duty_cycle;	// Инверсия скважности
	/* Умножение на 10 для сохранения разрядности без потери при делении */
	uint32_t value = (*Timer16_GetResource(timer)->period_reg + 1) * 10 / 100;
	*(CompareBase.comparesec_reg + OFFSET * channel) = (value * duty_cycle) / 10;
}

//******************************************************************************
//  Конец файла
//******************************************************************************
