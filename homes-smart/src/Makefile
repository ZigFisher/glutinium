
USBLIBS = `libusb-config --libs`
LIBS	= $(USBLIBS)

all: nrf24le1 usbgpio usbnrf usbtemp

nrf24le1:
	$(CC) $(LIBS) -lusb -o nrf24le1 nrf24le1_main.c opendevice.c

usbgpio:
	$(CC) $(LIBS) -lusb -o usbgpio usbgpio.c opendevice.c

usbnrf:
	$(CC) $(LIBS) -lusb -o usbnrf usbnrf.c opendevice.c

usbtemp:
	$(CC) $(LIBS) -lusb -o usbtemp usbtemp.c

strip: $(PROGRAM)
	strip nrf24le1
	strip usbgpio
	strip usbnrf
	strip usbtemp

clean:
	rm -f *.o nrf24le1 usbgpio usbnrf usbtemp

