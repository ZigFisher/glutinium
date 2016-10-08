#include "RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h> // usleep
#include <string.h> // strcmp

using namespace std;

void rx(int pin) {
  RCSwitch mySwitch = RCSwitch();
  pinMode(pin, INPUT);
  mySwitch.enableReceive(pin);

  do{
    usleep(100000); // 0.1sec
  } while (!mySwitch.available());
  
  int value = mySwitch.getReceivedValue();
  if (value == 0) {
    cout << "Unknown encoding" << endl;
  } else {
    cout << "{\"code\":";
    cout << mySwitch.getReceivedValue();
    cout << ",\"bit\":";
    cout << mySwitch.getReceivedBitlength();
    cout << "\"protocol\":";
    cout << mySwitch.getReceivedProtocol();
    cout << "\"delay\":";
    cout << mySwitch.getReceivedDelay();
    cout << "}" << endl;
  }
  mySwitch.resetAvailable();
}

RCSwitch initTx(int pin, int protocol, int repetition, int delay)
{
    pinMode(pin, OUTPUT);
	RCSwitch mySwitch = RCSwitch();
	mySwitch.setRepeatTransmit(repetition);
	mySwitch.enableTransmit(pin);
    mySwitch.setProtocol(protocol);
	if (delay > 0) {
		mySwitch.setPulseLength(delay);
	}
	return mySwitch;
}

void tx(RCSwitch mySwitch, int code, int bit) {
	mySwitch.send(code, bit);
    cout << "Send code " << code << endl;
}

void tx(RCSwitch mySwitch, char* sCodeWord) {
	mySwitch.send(sCodeWord);
    cout << "Send code " << sCodeWord << endl;
}

int main(int argc, char *argv[]) {
  int c;
  int x = 1;
  int protocol = 1;
  int code = 12345;
  int bit = 24;
  int pin = 1;
  int delay = 0;
  int repetition = 20;
  bool run = true;
  char sCodeWord[128] = "111111111";
  opterr = 0;
  
  while ((c = getopt(argc, argv, "hsx:p:c:b:i:r:d:m:a:")) != -1) {
    switch (c) {
      case 'h':
	    run = false;
        cout << argv[0] << " [-x rx|tx] [-c code] [-b bit] [-p protocol] [-r repetition] [-d delay] [-i gpio] [-m pathPinMode] [-a pathPinData] [-s]" << endl << endl;
        cout << "rx: receive mode (default)" << endl;
        cout << "tx: transmit mode. This mode can get extra parameters [-c code] [-b bit] [-p protocol] [-r repetition] [-d delay]." << endl;
        cout << "code: code to transmit eg.: " << code << endl;
        cout << "bit: length of the code to transmit or 0 for binary code, default: " << bit << endl;
        cout << "protocol: protocol used to transmit (1, 2 or 3), default: " << protocol << endl;
		cout << "delay: pulse length or 0 for using the default pulse length protocol." << endl;
		cout << "repetition: how many time the code is transmit, default: " << repetition << endl;
        cout << "gpio: the pin connected to your transmiter or receiver, default: " << pin << " (for orangePi, 1 is the pin 11, GPIO 17)" << endl;
        cout << "pathPinMode: default '/sys/class/gpio_sw/PA%d/cfg', can be as well '/sys/class/gpio/gpio%d/direction'" << endl;
        cout << "pathPinData: default '/sys/class/gpio_sw/PA%d/data', can be as well '/sys/class/gpio/gpio%d/value'" << endl;
        cout << "-s to set the pint mode with a string OUT/IN instead of 1/0." << endl;
        cout << endl;
        
        break;
      case 's': // stringPinMode
        setStringPinMode();
        break;
      case 'x': // rx or tx
        x = strcmp (optarg, "tx");
        break;
      case 'p': // protocol
        protocol = atoi(optarg);
        break;
      case 'c': // code
		strcpy(sCodeWord, optarg);
        code = atoi(optarg);
        break;
      case 'b': // bit
        bit = atoi(optarg);
        break;
	  case 'd': // bit
        delay = atoi(optarg);
        break;
      case 'r': // bit
        repetition = atoi(optarg);
        break;
      case 'i':
        pin = atoi(optarg);
        break;
      case 'm':
        setPathGpioPinMode(optarg);
        break;
      case 'a':
        setPathGpioData(optarg);
        break;
      case '?':
        if (optopt == 'x')
          fprintf(stderr, "Option -%c requires an argument: rx to receive or tx to transmit.\n", optopt);
        else if (optopt == 'p' || optopt == 'c' || optopt == 'b' || optopt == 'i' || optopt == 'r' || optopt == 'd' || optopt == 'm' || optopt == 'a')
          fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint(optopt))
          fprintf(stderr, "Unknown option -%c.\n", optopt);
        else
          fprintf(stderr, "Unknown option character \\x%x.\n", optopt);
        return 1;
      default:
        abort();
    }
  }
  if (run) {
	  if (x == 0) {
		RCSwitch mySwitch = initTx(pin, protocol, repetition, delay);
		if (bit == 0) {
			tx(mySwitch, sCodeWord);
		}
		else {
			tx(mySwitch, code, bit);
		}
	  }
	  else {
		rx(pin);
	  }  
  }
  return 0;
}
