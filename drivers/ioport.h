#ifndef IOPORT_H
#define	IOPORT_H

//******************************************************************************
//  Секция включения заголовочных файлов
//******************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <p24Fxxxx.h>

//******************************************************************************
//  Секция определения макросов
//******************************************************************************
#define CONC_TRISx(name)		TRIS##name
#define TRISx(name)             CONC_TRISx(name)

#define CONC_PORTx(name)		PORT##name
#define PORTx(name)             CONC_PORTx(name)

#define CONC_LATx(name)         LAT##name
#define LATx(name)              CONC_LATx(name)

#define CONC_ODCx(name)         ODC##name
#define ODCx(name)              CONC_ODCx(name)

#define	MODIFICATION_REGISTER(reg, bit, offset, mask, value)    \
            do{ reg &= ~(mask << (bit * offset)); \
			reg |= (value << (bit * offset)); }while(0)

/* Задание направления работы порта */
#define INP_FUNC_MODE                       1
#define OUT_FUNC_MODE                       0
#define REG_IO_TRIS(port)                   ( TRISx(port) )
#define IOPORT_SET_VALUE_DIR(port, value)   REG_IO_TRIS(port) = value
#define IOPORT_SET_DIR(port, pin, dir) \
            MODIFICATION_REGISTER(REG_IO_TRIS(port), 1, pin, 1, dir)
#define pIOPORT_SET_DIR(name, pin, dir) \
            MODIFICATION_REGISTER((*(name)), 1, pin, 1, dir)

/* Задание значение порта */
#define REG_IO_LAT(port)                    ( LATx(port) )
#define IOPORT_SET_BIT(port, pin)           REG_IO_LAT(port) |= (1 << pin)       
#define IOPORT_CLR_BIT(port, pin)           REG_IO_LAT(port) &= ~(1 << pin)
#define IOPORT_TGL_BIT(port, pin)           REG_IO_LAT(port) ^= (1 << pin)
#define IOPORT_MOD_BIT(port, pin, value) \
            MODIFICATION_REGISTER(REG_IO_LAT(port), 1, pin, 1, value)
#define IOPORT_SET_VALUE(port, value)       REG_IO_LAT(port) = value
#define pIOPORT_MOD_BIT(name, pin, value) \
            MODIFICATION_REGISTER((*(name+0x02)), 1, pin, 1, value)

/* Чтение значения порта */
#define REG_IO_PORT(port)                   ( PORTx(port) )
#define IOPORT_GET_BIT(port, pin)           REG_IO_PORT(port) & (1 << pin)
#define IOPORT_GET_VALUE(port)              REG_IO_PORT(port)
#define pIOPORT_GET_BIT(name, pin)          ((*(name+0x01)) & (1 << pin))

/* Настройка открытого стока порта */
#define IO_OPENDRAIN                        1
#define IO_NORMAL                           0
#define REG_IO_ODC(port)                    ( ODCx(port) )
#define IOPORT_SET_OPENDRAIN(port, pin)     REG_IO_ODC(port) |= (1 << pin)
#define IOPORT_CLR_OPENDRAIN(port, pin)     REG_IO_ODC(port) &= ~(1 << pin)
#define IOPORT_SET_VALUE_OD(port, value)    REG_IO_ODC(port) = value
#define IOPORT_SET_OD(port, pin, dir) \
            MODIFICATION_REGISTER(REG_IO_ODC(port), 1, pin, 1, dir)

/* Настройка Input Change Notification */
#define CN_ENABLE                           1
#define CN_DISABLE                          0
#define IOPORT_CN_ENABLE(num) \
            do {if (num < 16) CNEN1 |= (1 << num); \
            else CNEN2 |= (1 << (num-16));} while(0)
#define IOPORT_CN_DISABLE(num) \
            do {if (num < 16) CNEN1 &= ~(1 << num); \
            else CNEN2 &= ~(1 << (num-16));} while(0)
#define IOPORT_CN_SET_VALUE(num, value) \
            do {if (num < 16) {MODIFICATION_REGISTER(CNEN1, 1, num, 1, value);} \
            else {MODIFICATION_REGISTER(CNEN2, 1, (num-16), 1, value);}} while(0)

/* Настройка подтяжки вверх Input Change Notification */
#define CN_PU_ENABLE                        1
#define CN_PU_DISABLE                       0
#define IOPORT_CN_PULLUP_ENABLE(num) \
            do {if (num < 16) CNPU1 |= (1 << num); \
            else CNPU2 |= (1 << (num-16));} while(0)
#define IOPORT_CN_PULLUP_DISABLE(num) \
            do {if (num < 16) CNPU1 &= ~(1 << num); \
            else CNPU2 &= ~(1 << (num-16));} while(0)
#define IOPORT_CN_PULLUP_SET_VALUE(num, value) \
            do {if (num < 16) {MODIFICATION_REGISTER(CNPU1, 1, num, 1, value);} \
            else {MODIFICATION_REGISTER(CNPU2, 1, (num-16), 1, value);}} while(0)


/* Переназначение имен портов */
#define IOPORTA                             &TRISA
#define IOPORTB                             &TRISB
#define IOPORTC                             &TRISC

/* Ремап портов */
//#define iPPSOutput(pin,fn) pin=fn

/* Разрешить ремап портов */
#define PPS_UNLOCK      __builtin_write_OSCCONL(OSCCONL & ~_OSCCON_IOLOCK_MASK)
/* Запретить ремап портов */
#define PPS_LOCK        __builtin_write_OSCCONL(OSCCONL | _OSCCON_IOLOCK_MASK)

#define RPOR_BASE_ADDR              &RPOR0
#define RPOR_OFFSET(n, reg)         (reg + n / 2)
#define PPS_VALUE_MASK              0x1f
#define PPS_OUTPUT(pps, fn)         do { PPS_UNLOCK; \
                                    MODIFICATION_REGISTER(*RPOR_OFFSET(pps, \
                                        RPOR_BASE_ADDR), 8, (pps % 2), \
                                        PPS_VALUE_MASK, (fn & PPS_VALUE_MASK)); \
                                    PPS_LOCK; } while(0)

#define RPOUT_C1OUT 1
#define RPOUT_C2OUT 2
#define RPOUT_U1TX 3
#define RPOUT_U1RTS 4
#define RPOUT_U2TX 5
#define RPOUT_U2RTS 6
#define RPOUT_SDO1 7
#define RPOUT_SCK1OUT 8
#define RPOUT_SS1OUT 9
#define RPOUT_SDO2 10
#define RPOUT_SCK2OUT 11
#define RPOUT_SS2OUT 12
#define RPOUT_OC1 18
#define RPOUT_OC2 19
#define RPOUT_OC3 20
#define RPOUT_OC4 21
#define RPOUT_OC5 22

#define RPINR_BASE_ADDR             &RPINR0
#define RPINR_OFFSET(n, reg)        (reg + n / 2)
#define PPS_INPUT(fn, pps)          do { PPS_UNLOCK; \
                                    MODIFICATION_REGISTER(*RPINR_OFFSET(pps, \
                                        RPINR_BASE_ADDR), 8, (pps % 2), \
                                        PPS_VALUE_MASK, (fn & PPS_VALUE_MASK)); \
                                    PPS_LOCK; } while(0)

#define RPINP_INT1 1
#define RPINP_INT2 2
#define RPINP_T2CK 6
#define RPINP_T3CK 7
#define RPINP_T4CK 8
#define RPINP_T5CK 9
#define RPINP_IC1 14
#define RPINP_IC2 15
#define RPINP_IC3 16
#define RPINP_IC4 17
#define RPINP_IC5 18
#define RPINP_OCFA 22
#define RPINP_OCFB 23
#define RPINP_U1RX 36
#define RPINP_U1CTS 37
#define RPINP_U2RX 38
#define RPINP_U2CTS 39


//******************************************************************************
//  Секция объявления типов
//******************************************************************************
typedef volatile uint16_t tIOPort;

typedef struct
{
	tIOPort *name;
	uint8_t pin;
    uint8_t func;
}tPortCfg;

//******************************************************************************
//  Секция описания глобальных переменных
//******************************************************************************

//******************************************************************************
//  Секция прототипов глобальных функций
//******************************************************************************

//******************************************************************************
//  Конец файла
//******************************************************************************
#endif	// IOPORT_H
