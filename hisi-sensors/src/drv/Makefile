ifeq ($(PARAM_FILE), ) 
    PARAM_FILE:=../../../../Makefile.param
    include $(PARAM_FILE)
endif

#*******************************************************************************
#used to linux kernel kbuild EXTRA_CFLAGS or HuaweiLite build flags.
DRV_CFLAGS += -Wall -D$(HIDBG) -D$(HI_FPGA)
DRV_CFLAGS += $(MPP_CFLAGS)

SRCS := sensor.c
SRCS += init/$(OSTYPE)/sensor_init.c
SRCS += i2c_drv.c
SRCS += ssp_drv.c

INC := -I$(REL_INC)
INC += -I$(PWD)
INC += -I$(PWD)/../../include
INC += -I$(PWD)/../../firmware/drv
INC += -I$(OSAL_ROOT)/include

DRV_CFLAGS += $(INC)	
export DRV_CFLAGS
#*************************************************************************
TARGET := $(HIARCH)_sensor

#*************************************************************************
# compile linux or HuaweiLite
include $(PWD)/Make.$(OSTYPE)
