
#USBLIBS = `libusb-config --libs`
#LIBS	= $(USBLIBS)
#LIBS	= -L/usr/lib/i386-linux-gnu -lusb

all: usbtemp usbgpio

usbgpio:
	$(CC) $(LIBS) -o usbgpio usbgpio.c opendevice.c

usbtemp:
	$(CC) $(LIBS) -o usbtemp usbtemp.c

strip: $(PROGRAM)
	strip usbgpio
	strip usbtemp

clean:
	rm -f *.o usbtemp usbgpio

