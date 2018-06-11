
ifeq ($(PARAM_FILE), )
		PARAM_FILE:=../../Makefile.param
		include $(PARAM_FILE)
endif


obj-m := sample_ist.o
hi_sample_ist-y += sample_ist.o

EXTRA_CFLAGS += -I$(REL_INC)
EXTRA_CFLAGS += $(DRV_CFLAGS)
default:	
	$(CC) -g -Wall -o ist_test ist_test.c	strfunc.c    
	@$(MAKE) -C $(LINUX_ROOT) M=$(PWD) modules
	@rm -f *.o modules.* *.symvers *.mod.c
clean:
	@make -C $(LINUX_ROOT) M=$(PWD) clean 
	@rm -f ist_test *.bak

