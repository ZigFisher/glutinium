#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer0.h"

#define	TIMER0_FREQ			(15000)
//#define	TIMER0_PRESCALER	(1)
#define	TIMER0_PRESCALER	(8)
//#define	TIMER0_PRESCALER	(64)
//#define	TIMER0_PRESCALER	(256)
//#define	TIMER0_PRESCALER	(1024)

#define	TIMER0_RELOAD_LO	((0xFFFF-(F_CPU / TIMER0_PRESCALER / TIMER0_FREQ)+1) & 0xFF)
#define	TIMER0_RELOAD_HI	((0xFFFF-(F_CPU / TIMER0_PRESCALER / TIMER0_FREQ)+1) >> 8)

#if (TIMER0_RELOAD_HI!=0xFF)
#error TIMER0_RELOAD_HI!=0xFF!
#endif


TIMER_CALLBACK_FUNC CallbackFunc[TIMER0_NUM_CALLBACKS];





// Value definition:
// 0      The timer has expired
// 1-254  The timer is counting down
// 255    Free timer

u08 CountDownTimers[TIMER0_NUM_COUNTDOWNTIMERS];


/*****************************************************************************
*
*   Function name : Timer0_Init
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Initialize Timer/Counter 0
*
*****************************************************************************/
void Timer0_Init(void)
{
    u08 i;

#if (TIMER0_PRESCALER==1)
	TCCR0B |= _BV(CS10);	// clock source = clkio / 1
#elif (TIMER0_PRESCALER==8)
	TCCR0B |= _BV(CS11);
#elif (TIMER0_PRESCALER==64)
	TCCR0B |= _BV(CS11) | _BV(CS10);
#elif (TIMER0_PRESCALER==256)
	TCCR0B |= _BV(CS12);
#elif (TIMER0_PRESCALER==1024)
	TCCR0B |= _BV(CS12) | _BV(CS10);
#else
	#error TIMER0_PRESCALER not set properly!
#endif
	TCNT0 = TIMER0_RELOAD_LO;
	TIMSK |= _BV(TOIE0);




    // Initialize array of callback functions
    for (i=0; i<TIMER0_NUM_CALLBACKS; i++)
        CallbackFunc[i] = NULL;

    // Initialize countdown timers
    for (i=0; i<TIMER0_NUM_COUNTDOWNTIMERS; i++)
        CountDownTimers[i] = 255;

}


volatile u08 ms_ticks_byte;
void delay_ms(u08 ms)
{
	ms_ticks_byte = ms;
	while (ms_ticks_byte);
}


extern volatile unsigned int timer_val;
extern volatile unsigned int state;

SIGNAL(SIG_TIMER0_OVF)
{

	TCNT0 = TIMER0_RELOAD_LO;

    u08 i;
    for (i=0; i<TIMER0_NUM_CALLBACKS; i++)
        if (CallbackFunc[i] != NULL)
            CallbackFunc[i]();

#if 0
    // Count down timers
    for (i=0; i<TIMER0_NUM_COUNTDOWNTIMERS; i++)
        if (CountDownTimers[i] != 255 && CountDownTimers[i] != 0)
            CountDownTimers[i]--;
#endif


#if 0
	if ((timer_val/1000)%2 == 0)
		on(PinBlink);
	else
		off(PinBlink);


	timer_val++;

	if (state > 0) {
		on(PinState);
		state--;
	} else
		off(PinState);
#endif

	if (ms_ticks_byte) ms_ticks_byte--;
}




/*****************************************************************************
*
*   Function name : Timer0_RegisterCallbackFunction
*
*   Returns :       None
*
*   Parameters :    pFunc
*
*   Purpose :       Set up functions to be called from the
*                   TIMER0_COMP_interrupt
*
*****************************************************************************/
BOOL Timer0_RegisterCallbackFunction(TIMER_CALLBACK_FUNC pFunc)
{
    u08 i;

    for (i=0; i<TIMER0_NUM_CALLBACKS; i++)
    {
        if (CallbackFunc[i] == pFunc)
            return TRUE;
    }

    for (i=0; i<TIMER0_NUM_CALLBACKS; i++)
    {
        if (CallbackFunc[i] == NULL)
        {
            CallbackFunc[i] = pFunc;
            return TRUE;
        }
    }

    return FALSE;
}


/*****************************************************************************
*
*   Function name : Timer0_RemoveCallbackFunction
*
*   Returns :       None
*
*   Parameters :    pFunc
*
*   Purpose :       Remove functions from the list which is called int the
*                   TIMER0_COMP_interrupt
*
*****************************************************************************/
BOOL Timer0_RemoveCallbackFunction(TIMER_CALLBACK_FUNC pFunc)
{
    u08 i;

    for (i=0; i<TIMER0_NUM_CALLBACKS; i++)
    {
        if (CallbackFunc[i] == pFunc)
        {
            CallbackFunc[i] = NULL;
            return TRUE;
        }
    }

    return FALSE;
}


char Timer0_AllocateCountdownTimer()
{
    u08 i;
    for (i=0; i<TIMER0_NUM_COUNTDOWNTIMERS; i++)
        if (CountDownTimers[i] == 255)
        {
            CountDownTimers[i] = 0;
            return i+1;
        }

    return 0;
}

void Timer0_SetCountdownTimer(char timer, char value)
{
    cli();
    CountDownTimers[timer-1] = value;
    sei();
}

char Timer0_GetCountdownTimer(char timer)
{
    u08 t;
    cli();
    t = CountDownTimers[timer-1];
    sei();

    return t;
}

void Timer0_ReleaseCountdownTimer(char timer)
{
    cli();
    CountDownTimers[timer-1] = 255;
    sei();
}



