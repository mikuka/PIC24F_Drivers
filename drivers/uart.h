#ifndef UART_H
#define	UART_H

//******************************************************************************
//  Секция включения заголовочных файлов
//******************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

//******************************************************************************
//  Секция определения макросов
//******************************************************************************
/* Кол-во аппаратных UART */
#define NUART				2

//******************************************************************************
//  Секция объявления типов
//******************************************************************************
/* Имена таймеров */
typedef enum
{
	nUART1 = 0,
	nUART2,
}tUART;

/* Контроль четности */
typedef enum
{
    pNONE,
    pEVEN,
    pODD,
}tUART_Parity;
/* Кол-во стоп-бит */
typedef enum
{
    sbONE,
    sbTWO,
}tUART_StopBit;
/* Аппаратное управление потоком (не реализовано) */
typedef enum
{
    fcNONE,
    fcRTS,
    fcRTS_CTS,
    fBCLK,
}tUART_FlowControl;

typedef enum
{
    flRX_Available = 0x01,
    flTX_Complete = 0x100,
    flTX_BuffFull = 0x200,            
}tUART_Flag;

typedef struct
{
    uint32_t baudrate;
    uint8_t data_bits;
    tUART_Parity parity;
    tUART_StopBit stop_bit;
    tUART_FlowControl flow_ctrl;
//    bool tx_only;
}tUART_Cfg;

//******************************************************************************
//  Секция описания глобальных переменных
//******************************************************************************

//******************************************************************************
//  Секция прототипов глобальных функций
//******************************************************************************
void UART_Init(tUART name, const tUART_Cfg *cfg);
char UART_PutChar(tUART name, char c);
char UART_GetChar(tUART name);
bool UART_GetFlag(tUART name, tUART_Flag flag);
bool UART_SendByteBlock(tUART name, const uint8_t *buf, size_t len);
bool UART_ReceiveByteBlock(tUART name, uint8_t *buf, size_t len);
bool UART_SendBlockComplete(tUART name);
bool UART_ReceiveBlockComplete(tUART name);

//******************************************************************************
//  Конец файла
//******************************************************************************
#endif	// UART_H
