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

#define UART_DATA_MASK				0x01ff

//******************************************************************************
//  Секция объявления типов
//******************************************************************************
typedef struct 
{
	uint8_t *buf;
	size_t len;
}tDatab;

/* Структура с буферами данных */
struct tUART_Exchange
{
	tDatab data;
	bool complete;
//	bool busy;
};

struct tUART_Buffer
{
	volatile struct tUART_Exchange rx;
	volatile struct tUART_Exchange tx;	
};

/*struct tUART_Buffer
{
	uint16_t rx;
	uint16_t tx;
};*/

struct tUART_Handler
{
	void (*prx)(uint16_t data);
	uint16_t (*ptx)(void);	
};

/* Структура с ресурсами */
struct tUART_Data
{
	volatile uint16_t *base_addr;
	struct tUART_Buffer buffer;
	struct tUART_Handler handler;
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
#if 0
inline static void UART_RxIrqHandler(tUART bus)
{
	struct tUART_Data *UART = &UART_Data[bus];
	
	UART->buffer.rx = U1RXREG & UART_DATA_MASK;
	if (UART->handler.prx)
		UART->handler.prx(UART->buffer.rx);
}

inline static void UART_TxIrqHandler(tUART bus)
{
	struct tUART_Data *UART = &UART_Data[bus];

	if (U1STA & _U1STA_TRMT_MASK)	// Errata 52. Module: UART (Transmit Interrupt)
		U1TXREG = UART->handler.ptx ? UART->handler.ptx() : UART->buffer.tx;	
}

void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void)
{
	IFS0 &= ~_IFS0_U1RXIF_MASK;
	UART_RxIrqHandler(nUART1);
}

void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void)
{
	IFS0 &= ~_IFS0_U1TXIF_MASK;
	UART_TxIrqHandler(nUART1);
}

void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt(void)
{
	IFS1 &= ~_IFS1_U2RXIF_MASK;
	UART_RxIrqHandler(nUART2);
}

void __attribute__((interrupt, no_auto_psv)) _U2TXInterrupt(void)
{
	IFS1 &= ~_IFS1_U2TXIF_MASK;
	UART_TxIrqHandler(nUART2);
}
#endif

inline static void UART_RxIrqHandler(struct tUART_Data *UART, \
	volatile uint16_t *RXxREG)
{
	static size_t counter = 0;

	if (UART->buffer.rx.complete == false) {
		*UART->buffer.rx.data.buf++ = *RXxREG & UART_DATA_MASK;
		if (++counter < UART->buffer.rx.data.len) {
			UART->buffer.rx.complete = false;
		} else {
			counter = 0;
			UART->buffer.rx.complete = true;
		}
	}
}

inline static void UART_TxIrqHandler(struct tUART_Data *UART, \
	volatile uint16_t *TXxREG)
{
	static size_t counter = 0;
	
//	if ( !(U1STA & _U1STA_TRMT_MASK) )// Errata 52. Module: UART (Transmit Interrupt)
//		return;
	if (UART->buffer.tx.complete == false) {
		if (counter++ < UART->buffer.tx.data.len) {
			*TXxREG = *UART->buffer.tx.data.buf++;
			UART->buffer.tx.complete = false;
		} else {
			counter = 0;
			UART->buffer.tx.complete = true;
		}	
	}
}

void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void)
{
	IFS0 &= ~_IFS0_U1RXIF_MASK;
	UART_RxIrqHandler(&UART_Data[nUART1], &U1RXREG);
}

void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void)
{
	IFS0 &= ~_IFS0_U1TXIF_MASK;
	UART_TxIrqHandler(&UART_Data[nUART1], &U1TXREG);
}

void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt(void)
{
	IFS1 &= ~_IFS1_U2RXIF_MASK;
	UART_RxIrqHandler(&UART_Data[nUART2], &U2RXREG);
}

void __attribute__((interrupt, no_auto_psv)) _U2TXInterrupt(void)
{
	IFS1 &= ~_IFS1_U2TXIF_MASK;
	UART_TxIrqHandler(&UART_Data[nUART2], &U2TXREG);
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

inline static void TransmitEnable(tUART name)
{
	volatile uint16_t *UxSTA = UART_STA_REG(UART_Data[name].base_addr);
	*UxSTA |= _U1STA_UTXEN_MASK;	// Вкл. передатчика
}

inline static void TransmitDisable(tUART name)
{
	volatile uint16_t *UxSTA = UART_STA_REG(UART_Data[name].base_addr);
	*UxSTA &= ~_U1STA_UTXEN_MASK;	// Выкл. передатчика
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
	
	UART_Data[name].buffer.rx.complete = true;
	UART_Data[name].buffer.tx.complete = true;

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
	while (*UxSTA & _U1STA_UTXBF_MASK);
    *UxTX = c;
	return c;
}

char UART_GetChar(tUART name)
{
	volatile uint16_t *base_reg = UART_Data[name].base_addr;
	volatile uint16_t *UxSTA = UART_STA_REG(base_reg);
	volatile uint16_t *UxRX = UART_RXREG_REG(base_reg);
	/* Ожидание появления данных */
	while ( !(*UxSTA & _U1STA_URXDA_MASK));
	return *UxRX;
}

bool UART_GetFlag(tUART name, tUART_Flag flag)
{
	return *UART_STA_REG(UART_Data[name].base_addr) & flag;
}

bool UART_SendByteBlock(tUART name, const uint8_t *buf, size_t len)
{
	struct tUART_Data *UART = &UART_Data[name];

	if (UART->buffer.tx.complete) {
		if (buf) {
			UART->buffer.tx.complete = false;
			UART->buffer.tx.data.buf = (uint8_t *)buf;
			UART->buffer.tx.data.len = len;
			//TransmitEnable(name);
			IRQ_SetFlag(irqUART1_Transmitter);
		}
	}
	return UART->buffer.tx.complete;
}

bool UART_ReceiveByteBlock(tUART name, uint8_t *buf, size_t len)
{
	struct tUART_Data *UART = &UART_Data[name];
	if (UART->buffer.rx.complete) {
		if (buf) {
			UART->buffer.rx.complete = false;
			UART->buffer.rx.data.buf = buf;
			UART->buffer.rx.data.len = len;
		}
	}
	return UART->buffer.rx.complete;
}


bool UART_SendBlockComplete(tUART name)
{
	return UART_Data[name].buffer.tx.complete;	
}

bool UART_ReceiveBlockComplete(tUART name)
{
	return UART_Data[name].buffer.rx.complete;	
}

//******************************************************************************
//  Конец файла
//******************************************************************************
