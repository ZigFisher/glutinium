modbus-cli
==========

A simple application to send Modbus commands from the CLI


License
=======

This sources is free software; you can redistribute it and/or modify it under the terms of
the GNU Lesser General Public License as published by the Free Software Foundation;
either version 2.1 of the License, or (at your option) any later version.

You should have received a copy of the GNU Lesser General Public License along with this
script; if not, please visit http://www.gnu.org/copyleft/gpl.html for more information.


Usage
=====

This package provides a command line interface to send Modbus requests. It supports both Modbus RTU and Modbus TCP modes. It depends on the excellent library [libmodbus](http://libmodbus.org/) for C.

Once installed, it is possible to check its options and usage examples by executing `modbus-cli -h`:

```
Usage: modbus-cli [options...]
Examples:
	 modbus-cli -r -d /dev/ttyS1 -b 9600 -f 4 -s 1 -a 0 -n 20
	 modbus-cli -t -i 192.168.1.1 -p 502 -f 4 -s 1 -a 0 -n 20
Options: (R) means Modbus RTU, (T) means Modbus TCP
 -r,	 Establish a Modbus RTU connection
 -t,	 Establish a Modbus TCP connection
 -d,	 Define the device, default is /dev/ttyS1 (R)
 -b,	 Define the baudrate, default is 9600 (R)
 -i,	 Define the IP address (T)
 -p,	 Define the port (T)
 -f,	 Define the function code.
		 3=Read Holding Registers
		 4=Read Input Registers
		 5=Write Single Coil
		 6=Write Single Register
 -s,	 Define the slave ID to send the Modbus request to
 -a,	 Define the start address
 -n,	 Define the number of registers (for -f = 3 or 4)
 -v,	 Define the value to write TRUE or FALSE for (for -f = 5 or 6)
```

More info for use modbus-cli on OpenWRT routers you can see [here (Russian)](http://zftlab.org)


Links
=====

* [Original idea and sources from Maestro Wireless Solutions](https://dev.maestro-wireless.eu/kb/modbus-cli-package/)


