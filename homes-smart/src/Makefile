
#USBLIBS = `libusb-config --libs`
#LIBS	= $(USBLIBS)

all: usbgpio usbnrf usbtemp

usbgpio:
	$(CC) $(LIBS) -lusb -o usbgpio usbgpio.c opendevice.c

usbnrf:
	$(CC) $(LIBS) -lusb -o usbnrf usbnrf.c opendevice.c

usbtemp:
	$(CC) $(LIBS) -lusb -o usbtemp usbtemp.c

strip: $(PROGRAM)
	strip usbgpio
	strip usbnrf
	strip usbtemp

clean:
	rm -f *.o usbgpio usbnrf usbtemp

