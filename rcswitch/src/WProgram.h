#define NULL 0
#define OUTPUT 1
#define INPUT 0
#define HIGH 1
#define LOW 0
#define CHANGE 1
typedef bool boolean;
typedef int byte;

typedef void (*voidFuncPtr)(void);

void delayMicroseconds(unsigned int howLong);
void digitalWrite(int pin, int value);
void pinMode(int pin, int mode);
void setStringPinMode();
void attachInterrupt(int pin, voidFuncPtr handler, int mode);
void detachInterrupt(int pin);
unsigned long micros(void);
void setPathGpioData(char * path);
void setPathGpioPinMode(char * path);
