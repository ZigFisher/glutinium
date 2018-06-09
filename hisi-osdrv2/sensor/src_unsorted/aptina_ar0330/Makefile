#
# sensor lib Makefile
#

ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../../../Makefile.param
	include $(PARAM_FILE)
endif

ISP_PATH := $(SDK_PATH)/mpp/component/isp
EXT_PATH := $(SDK_PATH)/mpp/$(EXTDRV)
3A_PATH  := $(ISP_PATH)/3a
LIBPATH = $(ISP_PATH)/lib
OBJPATH = ./obj

ifeq ($(CONFIG_GPIO_I2C),y)
HI_GPIO_I2C:=HI_GPIO_I2C
else
HI_GPIO_I2C:=HI_GPIO_XXX
endif

ARFLAGS = rcv
ARFLAGS_SO = $(LIBS_LD_CFLAGS)
ARFLAGS_SO += -shared -fPIC -o
CFLAGS  = -Wall -fPIC -D$(HI_FPGA) -D$(HI_GPIO_I2C)
CFLAGS  += -O2
CFLAGS  += $(LIBS_CFLAGS)

ifeq ($(ISP_INI_CONFIG), y)
CFLAGS += -D INIFILE_CONFIG_MODE
endif

ifeq ($(HIGDB),HI_GDB)
CFLAGS += -g
endif

ifeq ($(CONFIG_JPEGEDCF), y)
     CFLAGS += -D ENABLE_JPEGEDCF 
endif
BUS_DIR := $(EXT_PATH)/ssp-sony/

ISP_INC := $(ISP_PATH)/include
3A_INC := $(3A_PATH)/include
INC := -I$(BUS_DIR) -I$(REL_INC) -I$(ISP_INC) -I$(3A_INC) -I$(ISP_PATH)/iniparser

ifeq ($(CONFIG_GPIO_I2C), y)
    INC += -I$(EXT_PATH)/gpio-i2c-ex
    INC += -I$(EXT_PATH)/gpio-i2c
else
    INC += -I$(EXT_PATH)/i2c
    INC += -I$(EXT_PATH)/hi_i2c
endif

COMPILE = $(CC) $(CFLAGS) $(DFLAGS) -lm

$(OBJPATH)/%.o: ./%.c
	@[ -e $(LIBPATH) ] || mkdir $(LIBPATH)
	@[ -e $(OBJPATH) ] || mkdir $(OBJPATH)
	@$(COMPILE) -o $@ -c $< $(INC) $(ISP_DEFINE)

SRCS = $(wildcard ./*.c)
OBJS = $(SRCS:%.c=%.o)
OBJS := $(OBJS:./%=obj/%)

TARGETLIB := $(LIBPATH)/libsns_ar0330.a
TARGETLIB_SO := $(LIBPATH)/libsns_ar0330.so

all:$(TARGETLIB)
$(TARGETLIB):$(OBJS)
	@($(AR) $(ARFLAGS) $(TARGETLIB) $(OBJS))
	@($(CC) $(ARFLAGS_SO) $(TARGETLIB_SO) $(OBJS))

clean:
	@$(RM) -rf $(TARGETLIB) $(OBJS)
	@$(RM) -rf $(LIBPATH) $(OBJPATH)
