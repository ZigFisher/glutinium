Homes-smarT
===========

Homes-smarT - some utils (author MaksMS) for simple experimental SmartHome project based on USBasp device.


License
=======

This sources is free software; you can redistribute it and/or modify it under the terms of
the GNU Lesser General Public License as published by the Free Software Foundation;
either version 2.1 of the License, or (at your option) any later version.

You should have received a copy of the GNU Lesser General Public License along with this
script; if not, please visit http://www.gnu.org/copyleft/gpl.html for more information.


Hardware
========

Hardware help here: https://github.com/ZigFisher/Glutinium/blob/master/homes-smart/hardware/README.md


Flashing bootloader
===================
	
	avrdude -c usbasp -p m8 -U flash:w:usbasp_m8_bootloader.hex -U hfuse:w:0xc0:m -U lfuse:w:0x9f:m -U lock:w:0x2f:m


Flashing firmware (need bootloader)
===================================
	
	avrdude -c usbasp -p m8 -U flash:w:usbasp_m8_i2c-tiny-usb
	or
	avrdude -c usbasp -p m8 -U flash:w:usbasp_m8_usb-asp.hex
	or
	avrdude -c usbasp -p m8 -U flash:w:usbasp_m8_usb-gpio.hex
	or
	avrdude -c usbasp -p m8 -U flash:w:usbasp_m8_usb-nrf.hex
	or
	avrdude -c usbasp -p m8 -U flash:w:usbasp_m8_usb-spi.hex


Flashing direct (without bootloader)
====================================
	
	avrdude -c usbasp -p m8 -U flash:w:usbasp_m8_usb-temp.hex -U hfuse:w:0xc9:m -U lfuse:w:0xef:m


Links
=====

* [Homepage of Homes-smarT (Russian)](http://homes-smart.ru)

