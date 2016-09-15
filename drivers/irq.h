#ifndef IRQ_H
#define	IRQ_H

//******************************************************************************
//  Секция включения заголовочных файлов
//******************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <p24FJ64GA004.h>

//******************************************************************************
//  Секция определения макросов
//******************************************************************************
/* Регистры статуса прерывания */
#define IRQ_IFS_BASE_ADDR			&IFS0
#define IRQ_IFS_FINAL_ADDR			&IFS4
/* Регистры разрешения прерывания */
#define IRQ_IEC_BASE_ADDR			&IEC0
#define IRQ_IEC_FINAL_ADDR			&IEC4
/* Регистры приоритетов */
#define IRQ_IPC_BASE_ADDR			&IPC0
#define IRQ_IPC_FINAL_ADDR			&IPC18

#define IRQ_REG_ADDR(n, reg)                    reg + n / 16
#define IRQ_BIT_OFFSET(n)                       (1 << n % 16)

#define IRQ_IPC_ADDR(n, reg)                    reg + n / 4
#define IRQ_IPC_BIT_MASK                        0x7
#define IRQ_IPC_BIT_OFFSET(n, pri)      ((pri & IRQ_IPC_BIT_MASK) << n % 4 * 4)

//******************************************************************************
//  Секция объявления типов
//******************************************************************************
typedef enum
{
    irqADC1                         = 13, /* ADC1 Conversion Done */
    irqCOMP_Event                   = 18, /* Comparator Event */
    irqCRC_Generator                = 67, /* CRC Generator */
    irqEXT0                         = 0, /* External Interrupt 0 */
    irqEXT1                         = 1, /* External Interrupt 1 */
    irqEXT2                         = 2, /* External Interrupt 2 */
    irqI2C1_Master                  = 17, /* I2C1 Master Event */
    irqI2C1_Slave                   = 16, /* I2C1 Slave Event */            
    irqI2C2_Master                  = 50, /* I2C2 Master Event */
    irqI2C2_Slave                   = 49, /* I2C2 Slave Event */
    irqINP_Capture1                 = 1, /* Input Capture 1 */
    irqINP_Capture2                 = 5, /* Input Capture 2 */
    irqINP_Capture3                 = 37, /* Input Capture 3 */
    irqINP_Capture4                 = 38, /* Input Capture 4 */
    irqINP_Capture5                 = 39, /* Input Capture 5 */
    irqICN                          = 19, /* Input Change Notification */
    irqOUT_Compare1                 = 2, /* Output Compare 1 */
    irqOUT_Compare2                 = 6, /* Output Compare 2 */
    irqOUT_Compare3                 = 25, /* Output Compare 3 */
    irqOUT_Compare4                 = 26, /* Output Compare 4 */
    irqOUT_Compare5                 = 41, /* Output Compare 5 */
    irqPPORT                        = 45, /* Parallel Master Port */
    irqRTC                          = 62, /* Real-Time Clock/Calendar */
    irqSPI1_Error                   = 9, /* SPI1 Error */
    irqSPI1_Event                   = 10, /* SPI1 Event */
    irqSPI2_Error                   = 32, /* SPI2 Error */
    irqSPI2_Event                   = 33, /* SPI2 Event */
    irqTimer1                       = 3, /* Timer1 */
    irqTimer2                       = 7, /* Timer2 */
    irqTimer3                       = 8, /* Timer3 */
    irqTimer4                       = 27, /* Timer4 */
    irqTimer5                       = 28, /* Timer5 */
    irqUART1_Error                  = 65, /* UART1 Error */ 
    irqUART1_Receiver               = 11, /* UART1 Receiver */
    irqUART1_Transmitter            = 12, /* UART1 Transmitter */
    irqUART2_Error                  = 66, /* UART2 Error */ 
    irqUART2_Receiver               = 30, /* UART2 Receiver */
    irqUART2_Transmitter            = 31, /* UART2 Transmitter */
    irqLVD                          = 72, /* Low-Voltage Detect (LVD) */
}tIRQ_Num;
/*
ADC1 Conversion Done 13 00002Eh 00012Eh IFS0<13> IEC0<13> IPC3<6:4>
Comparator Event 18 000038h 000138h IFS1<2> IEC1<2> IPC4<10:8>
CRC Generator 67 00009Ah 00019Ah IFS4<3> IEC4<3> IPC16<14:12>
External Interrupt 0 0 000014h 000114h IFS0<0> IEC0<0> IPC0<2:0>
External Interrupt 1 20 00003Ch 00013Ch IFS1<4> IEC1<4> IPC5<2:0>
External Interrupt 2 29 00004Eh 00014Eh IFS1<13> IEC1<13> IPC7<6:4>
I2C1 Master Event 17 000036h 000136h IFS1<1> IEC1<1> IPC4<6:4>
I2C1 Slave Event 16 000034h 000034h IFS1<0> IEC1<0> IPC4<2:0>
I2C2 Master Event 50 000078h 000178h IFS3<2> IEC3<2> IPC12<10:8>
I2C2 Slave Event 49 000076h 000176h IFS3<1> IEC3<1> IPC12<6:4>
Input Capture 1 1 000016h 000116h IFS0<1> IEC0<1> IPC0<6:4>
Input Capture 2 5 00001Eh 00011Eh IFS0<5> IEC0<5> IPC1<6:4>
Input Capture 3 37 00005Eh 00015Eh IFS2<5> IEC2<5> IPC9<6:4>
Input Capture 4 38 000060h 000160h IFS2<6> IEC2<6> IPC9<10:8>
Input Capture 5 39 000062h 000162h IFS2<7> IEC2<7> IPC9<14:12>
Input Change Notification 19 00003Ah 00013Ah IFS1<3> IEC1<3> IPC4<14:12>
Output Compare 1 2 000018h 000118h IFS0<2> IEC0<2> IPC0<10:8>
Output Compare 2 6 000020h 000120h IFS0<6> IEC0<6> IPC1<10:8>
Output Compare 3 25 000046h 000146h IFS1<9> IEC1<9> IPC6<6:4>
Output Compare 4 26 000048h 000148h IFS1<10> IEC1<10> IPC6<10:8>
Output Compare 5 41 000066h 000166h IFS2<9> IEC2<9> IPC10<6:4>
Parallel Master Port 45 00006Eh 00016Eh IFS2<13> IEC2<13> IPC11<6:4>
Real-Time Clock/Calendar 62 000090h 000190h IFS3<14> IEC3<14> IPC15<10:8>
SPI1 Error 9 000026h 000126h IFS0<9> IEC0<9> IPC2<6:4>
SPI1 Event 10 000028h 000128h IFS0<10> IEC0<10> IPC2<10:8>
SPI2 Error 32 000054h 000154h IFS2<0> IEC0<0> IPC8<2:0>
SPI2 Event 33 000056h 000156h IFS2<1> IEC2<1> IPC8<6:4>
Timer1 3 00001Ah 00011Ah IFS0<3> IEC0<3> IPC0<14:12>
Timer2 7 000022h 000122h IFS0<7> IEC0<7> IPC1<14:12>
Timer3 8 000024h 000124h IFS0<8> IEC0<8> IPC2<2:0>
Timer4 27 00004Ah 00014Ah IFS1<11> IEC1<11> IPC6<14:12>
Timer5 28 00004Ch 00014Ch IFS1<12> IEC1<12> IPC7<2:0>
UART1 Error 65 000096h 000196h IFS4<1> IEC4<1> IPC16<6:4>
UART1 Receiver 11 00002Ah 00012Ah IFS0<11> IEC0<11> IPC2<14:12>
UART1 Transmitter 12 00002Ch 00012Ch IFS0<12> IEC0<12> IPC3<2:0>
UART2 Error 66 000098h 000198h IFS4<2> IEC4<2> IPC16<10:8>
UART2 Receiver 30 000050h 000150h IFS1<14> IEC1<14> IPC7<10:8>
UART2 Transmitter 31 000052h 000152h IFS1<15> IEC1<15> IPC7<14:12>
Low-Voltage Detect (LVD) 72 0000A4h 000124h IFS4<8> IEC4<8> IPC17<2:0
*/
//******************************************************************************
//  Секция описания глобальных переменных
//******************************************************************************

//******************************************************************************
//  Секция прототипов глобальных функций
//******************************************************************************
inline static void IRQ_Enable(tIRQ_Num irq)
{
	volatile uint16_t *piec = IRQ_REG_ADDR(irq, IRQ_IEC_BASE_ADDR);
	/* Защита от выхода за границы регистров IECx */
	if (piec <= IRQ_IEC_FINAL_ADDR)
		*piec |= IRQ_BIT_OFFSET(irq);
}

inline static void IRQ_Disable(tIRQ_Num irq)
{
	volatile uint16_t *piec = IRQ_REG_ADDR(irq, IRQ_IEC_BASE_ADDR);
	/* Защита от выхода за границы регистров IECx */
	if (piec <= IRQ_IEC_FINAL_ADDR)
		*piec &= ~IRQ_BIT_OFFSET(irq);
}

inline static void IRQ_SetPriority(tIRQ_Num irq, int8_t pri)
{
    volatile uint16_t *pipc = IRQ_IPC_ADDR(irq, IRQ_IPC_BASE_ADDR);
    /* Защита от выхода за границы регистров IECx */
    if (pipc <= IRQ_IPC_FINAL_ADDR) {
        *pipc &= ~IRQ_IPC_BIT_OFFSET(irq, IRQ_IPC_BIT_MASK);    // Очистка
        *pipc |= IRQ_IPC_BIT_OFFSET(irq, pri);  // Записьs
    }
}

inline static void IRQ_ClearFlag(tIRQ_Num irq)
{
	volatile uint16_t *pifs = IRQ_REG_ADDR(irq, IRQ_IFS_BASE_ADDR);
	/* Защита от выхода за границы регистров IECx */
//	if (pifs <= IRQ_IFS_FINAL_ADDR)
	*pifs &= ~IRQ_BIT_OFFSET(irq);
}

inline static void IRQ_SetFlag(tIRQ_Num irq)
{
	volatile uint16_t *pifs = IRQ_REG_ADDR(irq, IRQ_IFS_BASE_ADDR);
	/* Защита от выхода за границы регистров IECx */
//	if (pifs <= IRQ_IFS_FINAL_ADDR)
	*pifs |= IRQ_BIT_OFFSET(irq);
}

inline static bool IRQ_GetStatus(tIRQ_Num irq)
{
	volatile uint16_t *pifs = IRQ_REG_ADDR(irq, IRQ_IFS_BASE_ADDR);
	return *pifs & IRQ_BIT_OFFSET(irq);
}

//******************************************************************************
//  Конец файла
//******************************************************************************
#endif	// IRQ_H
