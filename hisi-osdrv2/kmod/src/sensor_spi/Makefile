ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../Makefile.param
	include $(PARAM_FILE)
endif

obj-m := sensor_spi.o

hi_sensor_spi-y := sensor_spi.o

EXTRA_CFLAGS += -I$(REL_INC)
EXTRA_CFLAGS += $(DRV_CFLAGS)
all:
	make -C $(LINUX_ROOT) M=$(PWD) modules

clean:
	@make -C $(LINUX_ROOT) M=$(PWD) clean
