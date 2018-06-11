
ifeq ($(PARAM_FILE), )
		PARAM_FILE:=../../Makefile.param
		include $(PARAM_FILE)
endif

obj-m := pwm.o
hi_pwm-y += pwm.o

EXTRA_CFLAGS += -I$(REL_INC)
EXTRA_CFLAGS += $(DRV_CFLAGS)
default:	
	$(CC) -g -Wall -o pwm_write pwm_write.c	strfunc.c
	$(CC) -g -Wall -o pwm_test pwm_test.c	strfunc.c
	@$(MAKE) -C $(LINUX_ROOT) M=$(PWD) modules 
	@rm -f *.o modules.* *.symvers *.mod.c
clean:
	@make -C $(LINUX_ROOT) M=$(PWD) clean 
	@rm -f pwm_write pwm_test *.bak

