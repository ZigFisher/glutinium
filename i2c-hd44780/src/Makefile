TOOLS := /usr/bin
PREFIX := 
KDIR := /lib/modules/$(shell uname -r)/source
PWD := $(shell pwd)

obj-m :=  lcdi2c.o

all:
	$(MAKE) -C $(KDIR) \
		M=$(PWD) \
		ARCH=arm CROSS_COMPILE=$(TOOLS)/$(PREFIX) \
		modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

