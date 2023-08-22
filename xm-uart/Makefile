# all: xm-uart-motors-orig xm-uart-motors-openipc


CC=~/composer/openipc/output/host/bin/arm-openipc-linux-gnueabihf-cc


xm-uart: main.o
	$(CC) -o $@ $^

install:
	install -m 0777 xm-uart /usr/local/bin

clean:
	-rm -f *.o xm-uart 
