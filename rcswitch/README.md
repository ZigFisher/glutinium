# RCswitch
Port of RCswitch for any kind of GPIO module (without WiringPi).

* https://github.com/apiel/rcswitch

## Test

To compile the test example run:
```
g++ -o  test test.cpp RCSwitch.cpp WProgram.cpp -std=c++0x -pthread
```

You can change the path of the gpio with setPathGpioData and setPathGpioPinMode.

## RC app

RC is a little app to receive and transmit a RF code.

To receive, run `./rc`

It will output json data like: `{"code":234123,"bit":24"protocol":1"delay":180}`

The to reproduce this code run `./rc -x tx -c 234123 -b 24 -p 1 -d 180`

To compile:
```
g++ -o  rc rc.cpp RCSwitch.cpp WProgram.cpp -std=c++0x -pthread
```

Commands:
```
./rc [-x rx|tx] [-c code] [-b bit] [-p protocol] [-r repetition] [-d delay] [-i gpio]

rx: receive mode (default)
tx: transmit mode. This mode can get extra parameters [-c code] [-b bit] [-p protocol] [-r repetition] [-d delay].
code: code to transmit eg.: 12345
bit: length of the code to transmit or 0 for binary code, default: 24
protocol: protocol used to transmit (1, 2 or 3), default: 1
delay: pulse length or 0 for using the default pulse length protocol.
repetition: how many time the code is transmit, default: 20
gpio: the pin connected to your transmiter or receiver, default: 1 (for orangePi, 1 is the pin 11, GPIO 17)
```
