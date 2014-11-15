#ifndef TIMER0_H_INCLUDED
#define TIMER0_H_INCLUDED
#include "common.h"

extern volatile u08 ms_ticks_byte;

extern void delay_ms(u08 ms);


// Typedef for Timer callback function
typedef void (*TIMER_CALLBACK_FUNC) (void);

#define TIMER0_NUM_CALLBACKS        1
#define TIMER0_NUM_COUNTDOWNTIMERS  1

void Timer0_Init(void);

BOOL Timer0_RegisterCallbackFunction(TIMER_CALLBACK_FUNC pFunc);
BOOL Timer0_RemoveCallbackFunction(TIMER_CALLBACK_FUNC pFunc);


char Timer0_GetCountdownTimer(char timer);
char Timer0_AllocateCountdownTimer(void);
void Timer0_SetCountdownTimer(char timer, char value);
void Timer0_ReleaseCountdownTimer(char timer);

#endif // TIMER0_H_INCLUDED
