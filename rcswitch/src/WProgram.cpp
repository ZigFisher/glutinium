#include "WProgram.h"

#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <fstream> // readfile
#include <string> // readfile
#include <iostream>
#include <thread>  
#include <cstring>
//#include <sys/inotify.h>
//#include <unistd.h> // usleep

using namespace std;

bool loopInterrupt = false;
bool isStringPinMode = false;

const char * strPinModeOutput = "OUT";
const char * strPinModeInput = "IN";

char pathGpioData[128] = "/sys/class/gpio_sw/PA%d/data";
char pathGpioPinMode[128] = "/sys/class/gpio_sw/PA%d/cfg";

void setStringPinMode()
{
  isStringPinMode = true;
}

void setPathGpioData(char * path)
{
	strcpy(pathGpioData, path);
}

void setPathGpioPinMode(char * path)
{
	strcpy(pathGpioPinMode, path);
}

void delayMicrosecondsHard (unsigned int howLong)
{
  struct timeval tNow, tLong, tEnd ;

  gettimeofday (&tNow, NULL) ;
  tLong.tv_sec  = howLong / 1000000 ;
  tLong.tv_usec = howLong % 1000000 ;
  timeradd (&tNow, &tLong, &tEnd) ;

  while (timercmp (&tNow, &tEnd, <))
    gettimeofday (&tNow, NULL) ;
}

void delayMicroseconds (unsigned int howLong)
{
  struct timespec sleeper ;

  if (howLong ==   0)
    return ;
  else if (howLong  < 100)
    delayMicrosecondsHard (howLong) ;
  else
  {
    sleeper.tv_sec  = 0 ;
    sleeper.tv_nsec = (long)(howLong * 1000) ;
    nanosleep (&sleeper, NULL) ;
  }
}

char * getPathGpioPinMode(int pin)
{
	char * fName = new char[128];
	sprintf(fName, pathGpioPinMode, pin);
	
	return fName;
}

void pinMode(int pin, int mode)
{
	ofstream myfile;
	myfile.open(getPathGpioPinMode(pin));
    if (isStringPinMode) {
      myfile << (mode == INPUT ? strPinModeInput : strPinModeOutput);
    }
    else {
      myfile << mode;
    }
	
	myfile.close();
}

char * getPathGpioData(int pin)
{
	char * fName = new char[128];
	sprintf(fName, pathGpioData, pin);
	
	return fName;
}

void digitalWrite(int pin, int value)
{
	ofstream myfile;
	myfile.open(getPathGpioData(pin));
	myfile << value;
	myfile.close();
}

int digitalRead(int pin)
{
  string line;
  
  ifstream myfile(getPathGpioData(pin));
  if (myfile.is_open())
  {
    getline (myfile,line);
    myfile.close();
  }
  
  return atoi(line.c_str());
}

void tAttachInterrupt(voidFuncPtr handler, int pin)
{
  int val2 = 0;
  int val = digitalRead(pin);
  //cout << "start loop" << endl;
  loopInterrupt = true;
  while(loopInterrupt) {
    val2 = digitalRead(pin);
    if (val != val2) {
      //cout << "change" << endl;
      val = val2;
      handler();
    }
    //usleep(1);
  }
}

/*
void tAttachInterrupt(voidFuncPtr handler)
{
	int wd;
	int fd = inotify_init();
	loopInterrupt = true;
	while(loopInterrupt) {
		//cout << "wait" << endl;
		wd = inotify_add_watch( fd, "/sys/class/gpio_sw/PA1/data", IN_MODIFY);
		//cout << "change" << endl;
		handler();
	}
	inotify_rm_watch( fd, wd );
	//cout << "end watch" << endl;
}*/

// to trigger the interrupt whenever the pin changes value
void attachInterrupt(int pin, voidFuncPtr handler, int mode)
{
  pinMode(pin, INPUT);
  thread tAttInt (tAttachInterrupt, handler, pin);
  tAttInt.detach();
  //cout << "end attachInt" << endl;
}

// if implement attach Interrupt with thread, stop attachInterrupt
void detachInterrupt(int pin)
{
  loopInterrupt = false;
}

unsigned long micros(void)
{
    struct timeval t;
    long micros;
    gettimeofday(&t, NULL);
    micros = t.tv_sec * 1000000L + t.tv_usec;
    
    return micros; // should be the time in microsecond
}
