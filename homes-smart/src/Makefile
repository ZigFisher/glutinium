
#USBFLAGS = `libusb-config --cflags`
USBLIBS = `libusb-config --libs`

CC		= gcc
#CFLAGS<>= $(CPPFLAGS) $(USBFLAGS) -O -g -Wall
LIBS	= $(USBLIBS)


all: usbtemp usbgpio strip

usbgpio:
	$(CC) $(LIBS) -o usbgpio usbgpio.c opendevice.c

usbtemp:
	$(CC) $(LIBS) -o usbtemp usbtemp.c

strip: $(PROGRAM)
	strip usbgpio
	strip usbtemp

clean:
	rm -f *.o usbtemp usbgpio
