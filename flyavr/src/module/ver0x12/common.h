#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#define BOOL    char
#define FALSE   0
#define TRUE    (!FALSE)
#if!defined(NULL)
#define NULL    0
#endif

#define FLYAVR_VERSION      (0x12)

#if defined( __AVR_ATtiny2313__ )
#define FLYAVR_HW      		(231)
#endif


#define CMD_GET_EEPROM      (0x01) 
#define CMD_SET_EEPROM      (0x02)
#define CMD_GET_VERSION 	(0x10)
#define CMD_GET_HW 			(0x11)
#define CMD_GET_COUNTER		(0x1C)
#define CMD_GET_REBOOTS     (0x1D)
#define CMD_GET_CRC1 		(0x20)
#define CMD_PIN1_ON 		(0x30)
#define CMD_PIN1_OFF 		(0x31)
#define CMD_PIN2_ON 		(0x32)
#define CMD_PIN2_OFF 		(0x33)
#define CMD_PIN3_ON 		(0x34)
#define CMD_PIN3_OFF 		(0x35)


#define _setL(port,bit) port&=~(1<<bit)
#define _setH(port,bit) port|=(1<<bit)
#define _set(port,bit,val) _set##val(PORT##port,bit)
#define on(x) _set (x)
#define SET _setH

#define _clrL(port,bit) port|=(1<<bit)
#define _clrH(port,bit) port&=~(1<<bit)
#define _clr(port,bit,val) _clr##val(PORT##port,bit)
#define off(x) _clr (x)
#define CLR _clrH

#define _bitL(port,bit) (!(port&(1<<bit)))
#define _bitH(port,bit) (port&(1<<bit))
#define _bit(port,bit,val) _bit##val(PIN##port,bit)
#define _latch(port,bit,val) _bit##val(PORT##port,bit)
#define signal(x) _bit (x)
#define latch(x) _latch (x)
#define BIT _bitH

#define _cpl(port,bit,val) port^=(1<<bit)
#define __cpl(port,bit,val) PORT##port^=(1<<bit)
#define cpl(x) __cpl (x)
#define CPL _cpl

#define _bitnum(port,bit,val) bit
#define BITNUM(x) _bitnum(x)

#define _setO(port,bit) port|=(1<<bit)
#define _setI(port,bit) port&=~(1<<bit)
#define _setPullUp(port,bit) port|=(1<<bit)
#define _setHiZ(port,bit) port&=~(1<<bit)
#define _mode(port,bit,val,mode) _set##mode(DDR##port,bit)
#define _dmode(port,bit,val,dmode) _set##dmode(PORT##port,bit)
#define DIR(port,bit,mode) _set##mode(DDR##port,bit)
#define DRIVER(port,bit,mode) _set##mode(PORT##port,bit)
//mode = O or I
#define direct(x,mode) _mode(x,mode)
//dmode = PullUp or HiZ
#define driver(x,dmode) _dmode(x,dmode)



#if defined( __AVR_ATtiny2313__ )
#define FLY_PIN1  D,2,L
#define FLY_PIN2  D,3,L
#define FLY_PIN3  D,4,L
#define FLY_RESET D,5,L
#endif

typedef unsigned char	u08;
typedef unsigned int	u16;
typedef unsigned long	u32;


// CPU clock speed
//#define F_CPU			(20000000)               		// 20MHz processor
//#define F_CPU			(16000000)               		// 16MHz processor
//#define F_CPU			(14745000)               		// 14.745MHz processor
//#define F_CPU			(14318000)               		// 14.293MHz processor
//#define F_CPU			(14293000)              		// 14.293MHz processor
//#define F_CPU			(12000000)               		// 12MHz processor
#define F_CPU			(8000000)               		// 8MHz processor
//#define F_CPU			(7372800)               		// 7.37MHz processor
//#define F_CPU			(4000000)               		// 4MHz processor
//#define F_CPU			(3686400)               		// 3.69MHz processor



#endif	//	COMMON_H_INCLUDED
