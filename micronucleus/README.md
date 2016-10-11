micronucleus
============

Micronucleus - bootloader for Digispark tiny85 USB modul


License
=======

This sources is free software; you can redistribute it and/or modify it under the terms of
the GNU Lesser General Public License as published by the Free Software Foundation;
either version 2.1 of the License, or (at your option) any later version.

You should have received a copy of the GNU Lesser General Public License along with this
script; if not, please visit http://www.gnu.org/copyleft/gpl.html for more information.


Standart Flashing
=================

	avrdude -c usbasp -p t85 -U flash:w:digispark_t85_micronucleus-v2.03.hex -U lfuse:w:0xe1:m -U hfuse:w:0x5d:m -U efuse:w:0xfe:m


NoLock Flashing
===============

So, this flashing option no lock Reset pin (experimental, tnx mr.Umka)

	avrdude -c usbasp -p t85 -U flash:w:digispark_t85_micronucleus-v2.03.hex -U lfuse:w:0xe1:m -U hfuse:w:0xdd:m -U efuse:w:0xfe:m


Usage
=====

You can run this command on router (Bricket, etc.):

	micronucleus --run --type intel-hex /usr/share/little-wire/digispark_t85_littlewire_v13.hex


Links
=====

* https://github.com/micronucleus/micronucleus
