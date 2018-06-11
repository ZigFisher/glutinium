
ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../Makefile.param
	include $(PARAM_FILE)
endif

obj-m := ssp_ota5182.o
ssp-y += hi_ssp.o

EXTRA_CFLAGS += -I$(REL_INC)
EXTRA_CFLAGS += $(DRV_CFLAGS)

default:	
	
	
	make -C $(LINUX_ROOT) M=$(PWD) modules
	



clean:
	make -C $(LINUX_ROOT) M=$(PWD) clean
	rm -f  lcd_write_cmd lcd_write_cmd16 lcd_write_dat