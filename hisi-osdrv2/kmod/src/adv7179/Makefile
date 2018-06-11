ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../Makefile.param
	include $(PARAM_FILE)
endif
			
ifeq ($(CONFIG_GPIO_I2C),y)
	EXTRA_CFLAGS += -DHI_GPIO_I2C
	EXTRA_CFLAGS+=-I$(PWD)/../gpio-i2c
else
	EXTRA_CFLAGS += -DHI_I2C
	EXTRA_CFLAGS+=-I$(PWD)/../hi_i2c
endif

EXTRA_CFLAGS += -I$(REL_INC)
EXTRA_CFLAGS += $(DRV_CFLAGS)
EXTRA_CFLAGS += -D$(HI_FPGA)

obj-m := adv_7179.o
adv_7179-y += adv7179.o

default:
	echo $(LINUX_ROOT)
	make -C $(LINUX_ROOT) M=$(PWD) modules

clean: 
#	make -C $(LINUX_ROOT) M=$(PWD) clean
	@make -C $(LINUX_ROOT) M=$(PWD) clean

