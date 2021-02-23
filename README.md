Glutinium
=========

Glutinium - OpenWRT custom packages for extends functionality


Supporting
==========

If you like our work, please consider supporting us on <a href="https://www.paypal.com/donate/?hosted_button_id=C6F7UJLA58MBS">PayPal</a>. Thanks a lot !!!

<p align="center">
<a href="https://www.paypal.com/donate/?hosted_button_id=C6F7UJLA58MBS"><img src="https://www.paypalobjects.com/en_US/IT/i/btn/btn_donateCC_LG.gif" alt="PayPal donate button" /> </a>
</p>


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
* c328  =>
* changemac =>
* empty =>
* etherdump  =>
* fixsum => Fix atheros ath9k chipset EEPROM checksum
* flyavr => I2C protect device system
* flygps =>
* flypeek =>
* flyscripts =>
* gpio-littlewire =>
* gpio-rcswitch =>
* homes-smart =>
* httping =>
* i2c-telemetry => AM2321, BH1750, BMP085, DS1621, INA219, LM75, PCF8591, PCF8574
* i2c-tools =>
* kdb =>
* littlewire =>
* littlewire-extra =>
* micronucleus =>
* mini_snmpd =>
* mjpg-streamer-lite =>
* ncftp =>
* pzem004t => A simple util to read/write data PZEM-004T
* rcswitch =>
* remserial => Bridge between a TCP/IP network and serial ports
* sshpass =>
* suart =>
* uvc-ctrl =>
* uvc-streamer =>
* uvc2http =>
* vlhttp =>
* vtun-lite => VPN tunneling daemon (without lzo, zlib, ssl)
* webadmin => FlyRouter WEB administration interface


Usage
=====
	
	cd OpenWRT
	mkdir glutinium
	echo "src-git glutinium https://github.com/ZigFisher/Glutinium.git" >./feeds.conf  # if not have it in feeds.conf
	./scripts/feeds update glutinium       # or ./scripts/feeds update -a
	./scripts/feeds install i2c-telemetry  # or ./scripts/feeds install -a
	
	make menuconfig                        # select i2c-telemetry in utils dir
	make package/i2c-telemetry/compile     # {clean,compile,install}
	make package/i2c-telemetry/install     # {clean,compile,install}
	
	make package/index V=99                # Create index of packages


