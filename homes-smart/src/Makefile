
USBLIBS = `libusb-config --libs`

CC		= gcc
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
