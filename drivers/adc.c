//******************************************************************************
//  Секция включения заголовочных файлов
//******************************************************************************
#include <p24FJ64GA004.h>
#include "adc.h"


//******************************************************************************
//  Секция определения макросов
//******************************************************************************

#define ADC_MAX_NUMBER_CH		13
#define ADC_BAND_GAP_REF		1200
#define ADC_RESOLUTION			1024
#define ADC_CODE_MASK			0x3ff

//******************************************************************************
//  Секция объявления типов
//******************************************************************************

struct tADCConfig
{
	uint8_t number_channels;	// кол-во разрешенных каналов
	uint8_t first_channel;		// номер первого разрешенного канала
	uint16_t channel_mask;		// маска каналов
	volatile uint16_t *padc_buffer;		// адрес аппаратного буфера АЦП
	uint16_t code[ADC_MAX_NUMBER_CH];	// значение кодов АЦП
	uint16_t vref_mv;			// значение ИОН в мВ.
};

struct tADCHandler
{
	void (*phandler)(uint16_t *code);
	uint16_t ch_mask;
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


static struct tADCConfig adc_config;
static struct tADCHandler adc_handler;



//******************************************************************************
//  Секция прототипов функций
//******************************************************************************

//******************************************************************************
//  Секция локальных функций
//******************************************************************************

#include "uart_.h"

void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void)
{
	uint8_t ch;
	uint8_t index = 0;
	IFS0 &= ~_IFS0_AD1IF_MASK;	// сброс флага прерывания
	/* Т.к. номер ячейки аппаратного буфера не соответствует номеру канала,
	 * а заполняется последовательно согласно кол-ву разрешенных каналов,
	 * начиная с 0, то реализован поиск индекса соответвующей ячейки */
	/* Поиск с первого разрешенного канала для лучшего быстродействия */
	for (ch = adc_config.first_channel; ch < ADC_MAX_NUMBER_CH; ch++) {
		if ( (adc_config.channel_mask & (1 << ch)) == 0)
			continue;
		adc_config.code[ch] = adc_config.padc_buffer[index] & ADC_CODE_MASK;
		++index;
		/* вызов обработчика только в случае соответствия номеров канала */
		if (adc_handler.ch_mask == ch)
			if (adc_handler.phandler)	// проверка на 0
				adc_handler.phandler(&adc_config.code[ch]);
	}
}

//******************************************************************************
//  Секция глобальных функций
//******************************************************************************


void ADC_Init(tADC_Ch ch, const tADC_Config *cfg)
{
	uint8_t i;
	bool flag = false;
	for (i = 0; i < ADC_MAX_NUMBER_CH; i++) {
		if (ch.mask & (1 << i)) {
			++adc_config.number_channels;
			if (flag == false) {
				adc_config.first_channel = i;	// сохранения номера первого разрешенного канала
				flag = true;
			}
		}
	}
	adc_config.channel_mask = ch.mask;
	adc_config.padc_buffer = &ADC1BUF0;	// указание на буфер АЦП
	if (cfg->vref_source == vrEXTERNAL)
		adc_config.vref_mv = cfg->vref_mv;
	else
		adc_config.vref_mv = ADC_BAND_GAP_REF;

//	    AD1PCFG = 0x1EFF; // выбор пинов как цифровых линий
//    AD1CSSL = 0x0100; // выбор пинов для сканирования 1 - сканируем 0 -не сканируем
	AD1CON1 = 0x00;
    AD1CON2 = 0x00;
    AD1CON3 = 0x00;

	AD1CON1bits.FORM = 0x00;	// формат целого числа
	AD1CON1bits.SSRC = 0x07;	// последовательное авто преобр-ние каналов
	AD1CON1bits.ASAM = 0x01;	// автозапуск преобразования

	AD1CON2bits.VCFG = 0x00;	// ИОН - AVdd/AVss
	AD1CON2bits.CSCNA = 0x01;	// сканирование портов
	AD1CON2bits.SMPI = adc_config.number_channels - 1; // прерывание по окончании преобразования всех выбранных входов
	AD1CON2bits.BUFM = 0x00;	// буфер - 16 слов

	AD1CON3bits.ADRC = 0x00;	// 0 - тактирование от системной частоты, 1 - от встроенного RC
	// TODO - добавить расчет частоты сэмплирования
	AD1CON3bits.SAMC = 31;		// время преобразования, в единицвх Tad
	AD1CON3bits.ADCS = 64 - 1;	// частота семплирования, в единицах+1 Tcy,  Tcy = 2 * Tosc

	AD1CHS = 0x00;
//	AD1CHSbits.CH0NB = 0x00;	// MUX B Channel 0 negative input is VR-
	AD1CHSbits.CH0NA = 0x00;	// MUX A Channel 0 negative input is VR-
	AD1CHSbits.CH0SA = 0x00;	// Channel 0 positive input is AN0

	/* Выбор функции портов */
	AD1PCFG = (cfg->vref_source << _AD1PCFG_PCFG15_POSITION) |	// 0 - использование встроенного ИОН 1.2 В,  Band gap
				( (~ch.mask << _AD1PCFG_PCFG0_POSITION) & 0x1fff);	// 0 - порт вход АЦП, 1 / порт ввода-вывода
	/* Выбор каналов */
	AD1CSSL = (0 << _AD1CSSL_CSSL15_POSITION);	// 0 - вход VRef не опрашивается
	AD1CSSL |= ch.mask & 0xfff;

}

void ADC_Start(void)
{
	_AD1IP = 5;	// TODO - перенести в драйвер задания приоритетов IRQ
	AD1CON1bits.ADON = 0x01;	// включение модуля
    IEC0bits.AD1IE = 0x01;	// разрешение прерывания
    IFS0bits.AD1IF = 0x00;	// сброс флага
}

void ADC_Stop(void)
{
	AD1CON1bits.ADON = 0x00;	// выключение модуля
    IEC0bits.AD1IE = 0x00;	// запрещение прерывания	
}

uint16_t ADC_GetCode(uint8_t ch)
{
	return adc_config.code[ch] & ADC_CODE_MASK;
}

uint16_t ADC_GetVRef_mV(void)
{
	return adc_config.vref_mv;
}

	
uint16_t ADC_GetValue(uint8_t ch)
{
	/* умнажение на 1000 для сохранения остатка при делении */
	const uint32_t quantum = adc_config.vref_mv * 1000 / ADC_RESOLUTION;
	uint32_t value = quantum * (adc_config.code[ch] & ADC_CODE_MASK);
	return value / 1000;
}

bool ADC_ConvIsComplete(void)
{
	return AD1CON1bits.DONE ? true : false;
}

bool ADC_HandlerRegistration(uint8_t ch, void (*handler)(uint16_t *code))
{
	if (handler) {
		adc_handler.phandler = handler;
		adc_handler.ch_mask = ch;
		return true;
	}

	return false;
}


//******************************************************************************
//  Конец файла
//******************************************************************************
