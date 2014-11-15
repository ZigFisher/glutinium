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
* fixsum =>
* flygps =>
* flypeek =>
* i2c-telemetry =>


Usage
=====
	
	cd openwrt
	mkdir glutinium
	echo "src-link glutinium /var/www/builder/data/trunk/build_ng/openwrt/glutinium" >>./feeds.conf
	./scripts/feeds update -a && ./scripts/feeds install -a
	
	make package/index V=99                     # Create index of packages
	make package/bwm/{clean,compile,install}

