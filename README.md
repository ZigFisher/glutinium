Glutinium
=========

Glutinium - OpenWRT custom packages for extends functionality


License
=======

This sources is free software; you can redistribute it and/or modify it under the terms of
the GNU Lesser General Public License as published by the Free Software Foundation;
either version 2.1 of the License, or (at your option) any later version.

You should have received a copy of the GNU Lesser General Public License along with this
script; if not, please visit http://www.gnu.org/copyleft/gpl.html for more information.


Software
========

* accel-ppp =>
* baoclone =>
* barcode => Utility to get data from event devices
* changemac =>
* fixsum => Fix atheros ath9k chipset EEPROM checksum
* flyavr => I2C protect device system
* flygps =>
* flypeek =>
* kdb =>
* remserial => Bridge between a TCP/IP network and serial ports
* telemetry => AM2321, BH1750, BMP085, DS1621, INA219, LM75, PCF8591, PCF8574
* vtun-lite => VPN tunneling daemon (without lzo, zlib, ssl)
* webadmin => FlyRouter WEB administration interface


Usage
=====
	
	cd OpenWRT
	mkdir glutinium
	echo "src-git glutinium https://github.com/ZigFisher/Glutinium.git" >./feeds.conf
	./scripts/feeds update glutinium       # or ./scripts/feeds update -a
	./scripts/feeds install i2c-telemetry  # or ./scripts/feeds install -a
	
	make menuconfig                        # select i2c-telemetry in utils dir
	make package/i2c-telemetry/compile     # {clean,compile,install}
	make package/i2c-telemetry/install     # {clean,compile,install}
	
	make package/index V=99                # Create index of packages


