//******************************************************************************
//  Секция включения заголовочных файлов
//******************************************************************************
#include "drivers/ioport.h"
#include "drivers/mcu.h"
#include "drivers/irq.h"
#include "drivers/uart.h"
#include "drivers/timers.h"
#include "drivers/pwm.h"
#include "../pc_packet.h"

//******************************************************************************
//  Секция определения макросов
//******************************************************************************

// Configuration bits: selected in the GUI

// CONFIG2
#pragma config POSCMOD = NONE           // Primary Oscillator Select (Primary oscillator disabled)
#pragma config I2C1SEL = PRI            // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF            // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = ON            // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as port I/O (RC15))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching is enabled, Fail-Safe Clock Monitor is enabled)
#pragma config FNOSC = FRCPLL           // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))
#pragma config SOSCSEL = SOSC           // Sec Oscillator Select (Default Secondary Oscillator (SOSC))
#pragma config WUTSEL = LEG             // Wake-up timer Select (Legacy Wake-up Timer)
#pragma config IESO = OFF               // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) disabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = ON              // Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config ICS = PGx1               // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config COE = OFF				// Set Clip On Emulation Mode->Reset Into Operational Mode
#pragma config BKBUG = OFF				// Background Debug->Device resets into Operational mode
#pragma config GWRP = OFF				// General Code Segment Write Protect->Writes to program memory are allowed
#pragma config GCP = OFF				// General Code Segment Code Protect->Code protection is disabled
#pragma config JTAGEN = OFF				// JTAG Port Enable->JTAG port is disabled

//******************************************************************************
//  Секция объявления типов
//******************************************************************************

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

static tPC_RequestPacket PC_RequestPacket;
static tPC_ResponsePacket PC_ResponsePacket;

//******************************************************************************
//  Секция прототипов функций
//******************************************************************************
extern int debug(const char *format, ...);

//******************************************************************************
//  Секция локальных функций
//******************************************************************************

//******************************************************************************
//  Секция глобальных функций
//******************************************************************************
int main(void)
{
	MCU_InitClock();

//	IOPORT_SET_DIR(B, 10, OUT_FUNC_MODE);
//	IOPORT_SET_BIT(B, 10);

	IOPORT_SET_DIR(B, 13, OUT_FUNC_MODE);
	IOPORT_TGL_BIT(B, 13);

	Timer16_Init(T1, TIM_PRSC8);
	Timer16_SetPeriodMs(T1, 1000);
	IRQ_SetPriority(irqTimer1, 2);
	IRQ_ClearFlag(irqTimer1);
	IRQ_Enable(irqTimer1);
	Timer16_Start(T1);

	/*Timer16_Init(T2, TIM_PRSC256);
	Timer16_SetPeriodMs(T2, 1000);
	IOPORT_SET_DIR(B, 10, OUT_FUNC_MODE);
	PPS_OUTPUT(10, _RPOUT_OC1);
	Timer16_PWM_Setup(T2, tCH1, 1000);
	Timer16_PWM_SetDutyCycle(T2, tCH1, 50);
	Timer16_Start(T2);*/
	
	IOPORT_SET_DIR(C, 8, OUT_FUNC_MODE);
	IOPORT_SET_DIR(C, 9, INP_FUNC_MODE);
	PPS_OUTPUT(24, RPOUT_U1TX);
	PPS_INPUT(25, RPINP_U1RX);
	
	tUART_Cfg uart_cfg;
	uart_cfg.baudrate = 38400;
	uart_cfg.data_bits = 8;
	uart_cfg.flow_ctrl = fcNONE;
	uart_cfg.stop_bit = sbONE;
	uart_cfg.parity = pNONE;
	UART_Init(nUART1, &uart_cfg);

	IRQ_SetPriority(irqUART1_Receiver, 4);
	IRQ_ClearFlag(irqUART1_Receiver);
	IRQ_Enable(irqUART1_Receiver);
	IRQ_SetPriority(irqUART1_Transmitter, 5);
	IRQ_ClearFlag(irqUART1_Transmitter);
	IRQ_Enable(irqUART1_Transmitter);

	static int cnt;
	char ch;

	volatile int i;
	for (i = 0; i < 1000; i++);

	uint8_t rx[8];
	static uint8_t tx[8] = {"Hello!\r\0"};
	for ( ;; ) {
 		cnt = GetSysTickCounter();
		/*while (!UART_GetFlag(nUART1, flRX_Available));
		ch = UART_GetChar(nUART1);
		while (!UART_GetFlag(nUART1, flTX_Complete));
		UART_PutChar(nUART1, ch);*/
		/*UART_ReceiveByteBlock(nUART1, rx, sizeof(rx));
		while(!UART_ReceiveBlockIsComplete(nUART1));
		UART_SendByteBlock(nUART1, rx, sizeof(rx));
		while(!UART_SendBlockIsComplete(nUART1));*/
//		debug("Hello! %d, %d\r\n", sizeof(PC_ResponsePacket), sizeof(PC_ResponsePacket.state));
		if (PC_Request(&PC_RequestPacket)) {
			PC_ResponsePacket.func = PC_RequestPacket.func; 
			PC_ResponsePacket.state = stACK;
			PC_ResponsePacket.number = 1;
			PC_ResponsePacket.data_len = 0;
			PC_Response(&PC_ResponsePacket);	
		}
	}
	return 0;
}

//******************************************************************************
//  Конец файла
//******************************************************************************
