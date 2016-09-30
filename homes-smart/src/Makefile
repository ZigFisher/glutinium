
#USBLIBS = `libusb-config --libs`
#LIBS	= $(USBLIBS)

all: usbtemp usbgpio

usbgpio:
	$(CC) $(LIBS) -lusb -o usbgpio usbgpio.c opendevice.c

usbtemp:
	$(CC) $(LIBS) -lusb -o usbtemp usbtemp.c

strip: $(PROGRAM)
	strip usbgpio
	strip usbtemp

clean:
	rm -f *.o usbtemp usbgpio

