
i2c-telemetry
=============

Simple I2C telemetry system


Supported devices
=================

* AM2321  =>  НomeMade module (humidity + temperature)
* BH1750  =>  GY-302 module (brightness)
* BMP085  =>  GY-65  module (pressure + temperature)
* INA219  =>  HomeMade module (voltage + current)
* LM75    =>  Single chip
* PCF8591 =>  YL-40  module (8-bit A/D and D/A converter)
* PCF8574 =>  Single chip

* GPS     =>  USB or Serial
* WebCam  =>  Serial


Usage
=====
	
	boss@pistacho:~$ (bmp085; bh1750)
	Temperature (C)    26.2
	Pressure (mmHg)    734.97
	Brightness (Lux)   140.0
