//******************************************************************************
//  Секция включения заголовочных файлов
//******************************************************************************
#include <p24FJ64GA004.h>
#include "uart.h"
#include "irq.h"
#include "../delay.h"

//******************************************************************************
//  Секция определения макросов
//******************************************************************************
#define UART_MODE_REG(base)			(base + 0x00)
#define UART_STA_REG(base)			(base + 0x01)
#define UART_TXREG_REG(base)		(base + 0x02)
#define UART_RXREG_REG(base)		(base + 0x03)
#define UART_BRG_REG(base)			(base + 0x04)

#define UART_9BIT_NO_PARITY			0x03
#define UART_IRQ_MODE_TX_BUF_EMPTY	0x02	// Прерывание по опустошению буфера
#define UART_IRQ_MODE_TX_OUT_SHIFT	0x01	// Прерывание по сдвигового регистра
#define UART_IRQ_MODE_RX_ANY_CHAR	0x00	// Прерывание по каждому символу

//******************************************************************************
//  Секция объявления типов
//******************************************************************************

/* Структура с ресурсами */
struct tUART_Data
{
	volatile uint16_t *base_addr;
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

static struct tUART_Data UART_Data[NUART] = {
	{
		.base_addr = &U1MODE,
	},
	{
		.base_addr = &U2MODE,
	},
};

//******************************************************************************
//  Секция прототипов функций
//******************************************************************************

//******************************************************************************
//  Секция локальных функций
//******************************************************************************
void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void)
{
	IFS0 &= ~_IFS0_U1RXIF_MASK;
}

void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void)
{
	IFS0 &= ~_IFS0_U1TXIF_MASK;
//	if (U1STA & _U1STA_TRMT_MASK)	// Errata 52. Module: UART (Transmit Interrupt)
//		U1TXREG = 0x33;
}

void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt(void)
{
	IFS1 &= ~_IFS1_U2RXIF_MASK;
}

void __attribute__((interrupt, no_auto_psv)) _U2TXInterrupt(void)
{
	IFS1 &= ~_IFS1_U2TXIF_MASK;
}

static uint16_t CalcBaudrate(uint32_t rate, bool high_speed_mode)
{
	uint16_t value;

	if (high_speed_mode) {
		value = (uint16_t)(FCY / 4 / rate) - 1;
	} else {
		value = FCY / 16 / rate - 1;
	}
	return value;
}

//******************************************************************************
//  Секция глобальных функций
//******************************************************************************
void UART_Init(tUART name, const tUART_Cfg *cfg)
{
	volatile uint16_t *base_reg = UART_Data[name].base_addr;
	volatile uint16_t *UxMODE = UART_MODE_REG(base_reg);
	volatile uint16_t *UxSTA = UART_STA_REG(base_reg);
	volatile uint16_t *UxBRG = UART_BRG_REG(base_reg);
	bool high_speed_mode = false;

	/* Настройка блока, Wake-up, Loopback, Auto-Baud, IRDA 
	   по умолчанию выключены */
	*UxMODE = _U1MODE_USIDL_MASK;	// 1 - останов в спящем режима
	*UxMODE |= (cfg->flow_ctrl << _U1MODE_UEN0_POSITION);	// настройка апп. управления потоком
	/* Длина посылки, 9 бит доступны только без паритета */
	if (cfg->data_bits == 9) {
		*UxMODE |= (UART_9BIT_NO_PARITY << _U1MODE_PDSEL0_POSITION);
	}
	/* 8 бит */
	else {
		*UxMODE |= (cfg->parity << _U1MODE_PDSEL0_POSITION);
	}
	if (cfg->baudrate > FCY / (4 * 65536)) {
		*UxMODE |= _U1MODE_BRGH_MASK;// режим высоких скоростей для меньшего % ошибок
		high_speed_mode = true;
	}
	/* Расчет скорости */
	*UxBRG = CalcBaudrate(cfg->baudrate, high_speed_mode);
	/* Настройка прерываний, по умолчанию Address Detect mode и IRDA
	   выключены */
	*UxSTA = (UART_IRQ_MODE_TX_OUT_SHIFT << _U1STA_UTXISEL0_POSITION);// Прерывание передатчика
	*UxSTA |= (UART_IRQ_MODE_RX_ANY_CHAR << _U1STA_URXISEL0_POSITION);// Прерывание приемника
	*UxMODE |= _U1MODE_UARTEN_MASK;	// Вкл. модуля
	*UxSTA |= _U1STA_UTXEN_MASK;	// Вкл. передатчика
}

char UART_PutChar(tUART name, char c)
{
	volatile uint16_t *base_reg = UART_Data[name].base_addr;
	volatile uint16_t *UxSTA = UART_STA_REG(base_reg);
	volatile uint16_t *UxTX = UART_TXREG_REG(base_reg);
	/* Ожидание опустошения буфера */
	while ((*UxSTA & _U1STA_UTXBF_MASK));
    *UxTX = c;
	return c;
}

//******************************************************************************
//  Конец файла
//******************************************************************************
