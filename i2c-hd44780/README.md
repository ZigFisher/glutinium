Linux kernel module for alphanumeric LCDs on HD44780 and I2C expander attached. 
Module supports LCDs with PCF8574 based expanders attached.

Default configuration and makefile adapted for OpenWRT/LEDE, tested on Onion Omega2 (Mediatek MT7688).

The module requires kernel version 3.x or higher, access to I2C bus on destination machine 
(module was tested on RaspberryPI 2 with kernel 4.1 and i2c_bcm2708 already loaded). 
It is currently able to drive only one LCD at once, however you have a choice which LCD
you would like to drive by this module, using proper module option.

More info
---------
* https://github.com/rvbglas/lcdi2c
* https://github.com/lucidm/lcdi2c

requirements
------------
* Running Linux Kernel and its source code. Version 3.x or higher is supported.
* Prepared Kernel source for modules compilation.
* Already loaded kernel modules for i2c bus (for example i2c_bcm2708 module loaded if 
  destination host machine is Raspberry Pi) and i2c-dev if you want to test LCD before module
  compilation.

compilation
-----------
* Before making module, make sure you have properly installed Linux Kernel source in your
  system. Symlinks in */lib/modules/$(uname -r)/source* and */lib/modules/$(uname -r)/build*
  should point to proper kernel source directory tree.

* If you didn't prepare compilation of Kernel previously, go to */lib/modules/$(uname -r)/build*
  directory and run:
  *make modules_prepare*. 

* To make the module, go to the directory of the module and run commands:
  *make -C /lib/modules/$(uname -r)/build M=$PWD*
  *make -C /lib/modules/$(uname -r)/build M=$PWD modules_install*

  Sometimes, especially if you made the kernel using cross compilation, you get an error statement like:
  "the KERNEL_SOURCE/scripts/recordmcount: Syntax error: "(" unexpected".
  To fix this, got to */lib/modules/$(uname -r)/build* on destination host (not the host you cross 
  compiled kernel) and make:
  make scripts
  Then retry to make the module.

* After successful compilation module will be installed in */lib/modules/$(uname -r)/extra* and
  you be able to modprobe lcdi2c

* Go to examples and run 
  *python lcddev.py*

module arguments
----------------
* Module expect arguments to be set before loading. If none of them is given, module will be loaded
  with default values, which may, or may not be suitable for your particular LCD back board. If you want to
  read more about module parameters run command "modinfo lcdi2c" which will display short description
  about module it self and arguments you can set.
  
* busno  - bus number, same as in proof application.

* address - I2C expander address, default set to 0x27

* pinout - array of number of pins configuration. Not all expanders are configured the same, so you
           have to be prepared for different pin configurations, this parameter will help you to make this out. 
           This parameter is a list of numbers representing pshysical pin number of the expander chip, 
           connected to your LCD control lines in order given order: RS,RW,E,BL,D4,D5,D6,D7.
           So for example, if your expander pins are pshysically connected in following configuration:
              D4 = PIN.0, D5 = PIN.1, D6 = PIN.2, D7 = PIN.3, RS = PIN.4, RW = PIN.5, E = PIN.6, BL = PIN.7,
           you should set this argument to: 4,5,6,7,0,1,2,3. 
           Pin BL is used for switching backlight of an LCD. Default value is: 0,1,2,3,4,5,6,7 some popular cheap
           expanders (usually with black solder mask on PCB) use this pin configuration.
           
* cursor - set to 1 will show cursor at start, 0 - will prevent from displaying the cursor. Default set to 1

* blink  - 1 will blink current character position, 0 - blinking character will be disabled. Default set to 1

* major  - driver will register new device in /dev/i2clcd, you can force major number of the device by using this 
           configuration option or leave it for kernel to decide. 
           Preffered is not to give this parameter and let the kernel decide. 
           You can later read the number form /sys filesystem.
           
* topo   - LCD topology, same as described in "testing" section. Default set to 4 (16x2).

/sys device interface
----------------
* Module has two sets of interfaces you can interact with your LCD. It registers /dev/lcdi2c character device, which you
  can open and manipulate using standard open/close/read/write/ioctl quintet, or through /sys interface. However you
  are able to use /dev/lcdi2c device, /sys interface is preffered. 
  Let's work out /sys interface first. Once module is loaded, driver will register new class devices called
  "/sys/class/alphalcd" in which subdirectory "lcdi2c" will be created.
  
* "/sys/class/alphalcd/lcdi2c" is the directory containing all interace files required to interact with LCD, here is
  alphabetically ordered list of files with short descritpiion:
  
  - brighness - write "0" to this file to switch backlight off, any value different from 0 up to 255 will switch 
                backlight on.
                
  - blink     - write "0" to switch blinking character off, "1" to switch it on. Reading this file will tell you 
                about current status of blinking.
                
  - clear     - write only, writing "1" to this file will clear LCD.
  
  - cursor    - write "0" for switch display cursor off, "1" for switch cursor on. Reading this file will tell you about
                current status of the cursor.
                
  - customchar- HD44780 and its clones usually provide way to define 8 custom characters, this file will let you to
                define those characters. This file is binary, each character consists of 9 bytes, first byte informs
                about character number (characters have numbers from 0 to 7), next eight bytes is bitmap definition of
                the character itself. Next 9 bytes repeats this pattern for another character. Total length of this file is
                72 bytes. Writing to this file will make you able to define your own set of new 8 characters. If you want
                to define new bitmat for a character, write at least 9 bytes, first byte is character number you'd like
                to change, next 8 bytes defines bitmap of the character. You are allowed to define more than one character per
                write, but keep in mind, you're always writing 9 * n bytes, where n is number of characters you'd like to define.
                
  - data      - driver uses internal buffer which is 1:1 internal LCD RAM mirror, everything you write to LCD through
                driver interface will be represented in this file. You can also write to this file, and all that you wrote
                would be visible on the display. Keep in mind, size of RAM of LCD is limited only to 104 bytes.
                Internal RAM organization depends on "topo" parameter, however reading this file will represent what is actually
                on the screen, and writing to it will have 1:1 representation on the LCD, configuration of this buffer depends on
                current LCD display topology (for some displays, not all bytes in RAM are used to display data and it's true for
                this buffer file too).
                If you want to know more about this kind of displays RAM organization, please read link below
                http://web.alfredstate.edu/weimandn/lcd/lcd_addressing/lcd_addressing_index.html
                which will greatly explain how RAM organization differs in different LCD types.
                
  - dev       - description of major:minor device number associated with /dev/lcdi2c device file.
  
  - home      - writing "1" will cause LCD to move cursor to first column and row of LCD.
  
  - meta      - description of currently used LCD. You can only read this file to get current LCD configurartion.
  
  - position  - this file will help you to set or read current cursor position. This file contains two bytes,
	        value of first byte informs about current cursor position at column, second byte contain information
	        about current cursor position at row. Writing two bytes to this file, will set cursor at position. 
	        
  - reset     - write only file, "1" write to this file will reset LCD to state after module was loaded.
  
  - scrollhz  - scroll horizontally, write "1" to this file to scroll content of LCD horizontally by 1 character to the right,
                scrolling to the left is made by writing "0" to this file. This scrolling technique will not change contents of internal RAM of
                your display, so "data" file will also keep its content intact. Opposite character which leave display during 
                the scroll will appear on the on the other outermost position, so this scroll always keeps information on the screen.
                This is the internal HD44780 mechanism.

/dev/lcdi2c device interace
---------------------------
* Module has alternative interface to drive connected LCD. It registers /dev/lcdi2c device file, which you're able to write to or read from.
  Typical method for accessing such devices is to use open() function, complementary close() function, read() and write() functions. To be able
  to use rest of features of HD44780 some ioctls commands are provided. List of all suprted IOCTLS commands with codes is available through 
  /sys/class/alphalcd/lcdi2c/meta file under IOCTLS: section. Command codes repeat functionality of /sys interface. However some are unavailable, like 
  "meta" for example. Reading and writing to the device is also different, you should write to it using write() function and complementary read()
  function to read data from device. Below is a list of supported IOCTL commands:
  
  - CLEAR - writing "1" as argument of this ioctl, will clear the display
  
  - HOME  - writing "1" as argument of this ioctl, will move cursor to first column and row of the display
  
  - RESET - writing "1" will reset LCD to default state
  
  - GETCHAR - this ioctl will return ASCII value of current character (character cursor is hovering at)
  
  - SETCHAR - this ioctl will set given ASCII character at position pointed by current cursor setting
  
  - GETPOSITION - will return current cursor position as two bytes, value of first byte represents current column, second one - current row
  
  - SETPOSITION - writing two bytes to this ioctl will set current cursor position on the display
  
  - GETBACKLIGHT - will return "0" if backlight is currently switched off, "1" otherwise
  
  - SETBACKLIGHT - "1" written to this ioctl will switch backlight on or if "0" is written, will switch it off
  
  - GETCURSOR - returns current cursor visibility status, "0" - invisible, "1" - visible
  
  - SETCURSOR - sets cursor visibility, "0" - invisible, "1" - visible
  
  - GETBLINK - returns blinking cursor status, "0" - cursors is not blinking, "1" - cursor is blinking
  
  - SETBLINK - sets or resets cursor blink, "0" - cursor will blink, "1" - cursor will not blink
  
  - SCROLLHZ - wrtting "0" to this ioctl will scroll screen to the left by one column, "1" - will scroll to the right
  
  - SETCUSTOMCHAR - allows to define new character map for given character number. This ioctl expects 9 bytes of data exactly, first byte is character number
                  eight subsequent bytes defines actual bitmap of font. This control differs from "customchar" in a way, that you cannot send more than
                  one character definition at once. If you want to define more than one character, just call this ioctl multiple times for each character
                  you would like to define.
                  
media
-----
  - https://youtu.be/CNj7ykGRBHw Module working with 8x2 LCD
  - https://youtu.be/3B-uGth-hZk Module working with 16x2 LCD
  - https://youtu.be/qbU7RORUYO8 Module working with 40x2 LCD
  
