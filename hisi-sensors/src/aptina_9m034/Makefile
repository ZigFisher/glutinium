#
# sensor lib Makefile
#

ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../../../Makefile.param
	include $(PARAM_FILE)
endif

ISP_PATH := $(MPP_PATH)/component/isp2
EXT_PATH := $(MPP_PATH)/$(EXTDRV)
3A_PATH  := $(ISP_PATH)/3a
LIBPATH = $(ISP_PATH)/lib
OBJPATH = ./obj

# GPIO-I2C or I2C
GPIO_I2C?=N

ifeq ($(GPIO_I2C),Y)
HI_GPIO_I2C:=HI_GPIO_I2C
else
HI_GPIO_I2C:=HI_GPIO_XXX
endif

ARFLAGS = rcv
ARFLAGS_SO = -shared -fPIC -o
CFLAGS  = -Wall -fPIC -D$(HI_FPGA) -D$(HI_GPIO_I2C)
CFLAGS  += -O2

ifeq ($(HIGDB),HI_GDB)
CFLAGS += -g
endif
BUS_DIR := $(EXT_PATH)/ssp-sony/

ISP_INC := $(ISP_PATH)/include
3A_INC := $(3A_PATH)/include
INC := -I$(BUS_DIR) -I$(REL_INC) -I$(ISP_INC) -I$(3A_INC)

ifeq ($(GPIO_I2C), Y)
    INC += -I$(EXT_PATH)/gpio-i2c-32bit
	INC += -I$(EXT_PATH)/gpio-i2c
else
    INC += -I$(EXT_PATH)/i2c
    INC += -I$(EXT_PATH)/hi_i2c
endif

COMPILE = $(CC) $(CFLAGS) $(DFLAGS) -lm

$(OBJPATH)/%.o: ./%.c
	@[ -e $(LIBPATH) ] || mkdir $(LIBPATH)
	@[ -e $(OBJPATH) ] || mkdir $(OBJPATH)
	@$(COMPILE) -o $@ -c $< $(INC)

SRCS = $(wildcard ./*.c)
OBJS = $(SRCS:%.c=%.o)
OBJS := $(OBJS:./%=obj/%)

TARGETLIB := $(LIBPATH)/libsns_9m034.a
TARGETLIB_SO := $(LIBPATH)/libsns_9m034.so

all:$(TARGETLIB)
$(TARGETLIB):$(OBJS)
	@($(AR) $(ARFLAGS) $(TARGETLIB) $(OBJS))
	@($(CC) $(ARFLAGS_SO) $(TARGETLIB_SO) $(OBJS))

clean:
	@$(RM) -rf $(TARGETLIB) $(OBJS)
	@$(RM) -rf $(LIBPATH) $(OBJPATH)
