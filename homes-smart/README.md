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


Software
========

* usb-gpio =>
* usb-nrf =>
* usb-temp =>


Flashing with bootloader
========================
	
	avrdude -c usbasp -p m8 -U flash:w:usbasp_m8_bootloader.hex -U hfuse:w:0xc0:m -U lfuse:w:0x9f:m -U lock:w:0x2f:m


Flashing firmware
=================
	
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


Usage
=====
	
	usbtemp sensors
	usbtemp temp <address>
	usbtemp ledon
	usbtemp ledoff
	
	usbnrf ALL
	
	usbgpio on <num gpio>
	usbgpio off <num gpio>
	usbgpio status
	usbgpio statusin
	usbgpio mode
	usbgpio rcsend <key>
	usbgpio dhtread
	usbgpio dhtsetup
	usbgpio pwm3 <level>
	usbgpio pwm4 <level>


Links
=====

* https://habrahabr.ru/post/208470/
* http://homes-smart.ru/index.php/oborudovanie/prochee/i2c-usb-perekhodnik
* http://homes-smart.ru/index.php/oborudovanie/uroki-i-primery/ustanovka-usbasploader-na-platu-usbasp
* http://homes-smart.ru/index.php/oborudovanie/uroki-i-primery/41-usb-kontroller-umnogo-doma
* http://homes-smart.ru/index.php/oborudovanie/uroki-i-primery/podklyuchaem-datchik-ds18b20-k-kompyuteru
* https://ruessel.in-chemnitz.de/usbtemp/
* https://tosiek.pl/usbasp-v2-0-warning-cannot-set-sck-period/
* -
* http://homes-smart.ru/index.php/oborudovanie/prochee/radiopult
* http://homes-smart.ru/index.php/oborudovanie/bez-provodov-433-315mgts/opisanie-radiomodulej-433mgts-i-315mgts
* http://homes-smart.ru/index.php/oborudovanie/bez-provodov-433-315mgts/algoritm-peredachi-dannykh-v-vide-tsifrovogo-koda-na-radiomodulyakh
* http://homes-smart.ru/index.php/oborudovanie/bez-provodov-433-315mgts/58-besprovodnoj-datchik-temperatury-vlazhnosti-na-attiny13-integratsiya-v-sistemu-umnogo-doma
* http://homes-smart.ru/index.php/oborudovanie/bez-provodov-433-315mgts/besprovodnoj-datchik-temperatury-i-vlazhnosti-na-baze-radiomodulej-433-315
* -
* http://homes-smart.ru/index.php/oborudovanie/bez-provodov-2-4-ggts/opisanie-radiomodulya-nrf24l01
* http://homes-smart.ru/index.php/oborudovanie/bez-provodov-2-4-ggts/nrf24l01-usb-iz-usbasp
* http://homes-smart.ru/index.php/oborudovanie/bez-provodov-2-4-ggts/klient-dlya-usb-nrf24l01-besprovodnoj-datchik-vlazhnosti-arduino
* http://homes-smart.ru/index.php/oborudovanie/bez-provodov-2-4-ggts/klient-dlya-usb-nrf24l01-besprovodnoj-datchik-vlazhnosti-si
* http://homes-smart.ru/index.php/oborudovanie/bez-provodov-2-4-ggts/shlyuz-ethernet-nrf24l01-w5100-na-baze-arduino-dlya-obmena-dannymi-s-nrf24l01-i-nrf24le1
* http://homes-smart.ru/index.php/oborudovanie/bez-provodov-2-4-ggts/60-deshevyj-usb-priemnik-dlya-nrf24l01-i-nrf24le1-fz0769
* http://homes-smart.ru/index.php/oborudovanie/bez-provodov-2-4-ggts/63-shlyuz-wi-fi-nrf24l01-na-baze-esp8266-dlya-obmena-dannymi-s-nrf24l01-i-nrf24le1
* http://homes-smart.ru/index.php/oborudovanie/bez-provodov-2-4-ggts/nrf24le1-nrf24l01-mikrokontroller
* http://homes-smart.ru/index.php/oborudovanie/bez-provodov-2-4-ggts/54-programmirovanie-nrf24le1-cherez-raspberry-pi
* http://homes-smart.ru/index.php/oborudovanie/bez-provodov-2-4-ggts/55-programmirovanie-nrf24le1-cherez-usbasp
* http://homes-smart.ru/index.php/oborudovanie/bez-provodov-2-4-ggts/konstruktor-besprovodnogo-klienta-na-nrf24le1
* http://homes-smart.ru/fusecalc/?prog=avrstudio&part=ATmega8A

