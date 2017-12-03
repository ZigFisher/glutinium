Directory contains example of module usage. Was written in Python, as the default access for the device
registered by the module is granted for root, you should run python example using sudo.

"linux-module-config" directory contains example configuration for modprobe with predependency of
i2c-dev Kernel module. It may be helpful although it's not necesserely required for module to work.

Example of system module configuration with default parameters set to 16x2 LCD topology
and required module dependency. You should copy this tree to your /etc directory. Every
modprobe lcdi2c will insert the module with parameters defined in /etc/modprobe.d/lcdi2c.conf.



Notes:

    lcd.SETBACKLIGHT = '0'
    lcd.RESET = '1'
    lcd.CLEAR = '1'
    lcd.HOME = '1'
    lcd.SETBACKLIGHT = '1'
    lcd.SETCURSOR = '0'
    lcd.SETBLINK = '0'

  '''
    Will try to configure itself based on /sys/class/alphalcd/lcdi2c/meta file.
    All ioctls will be accessible through class attributes
  '''

#lcdi2c module depends on i2c_dev module.
#so, i2c_dev should be prior loaded
#

softdep lcdi2c pre: i2c_dev
options lcdi2c busno=1 address=0x27 topo=4 cursor=0 blink=0

--------------------------------


