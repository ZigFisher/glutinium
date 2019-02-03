#
# sensor lib Makefile
#

ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../../../Makefile.param
	include $(PARAM_FILE)
endif

EXT_PATH := $(SDK_PATH)/mpp/extdrv/
ISP_PATH := $(SDK_PATH)/mpp/component/isp
LIBPATH = $(ISP_PATH)/lib
OBJPATH = ./obj

ARFLAGS = rcv
ARFLAGS_SO = -shared -fPIC -o
CFLAGS  = -g -Wall -fPIC -D$(HI_FPGA) -D$(HI_GPIO_I2C)
#CFLAGS  += -O2

ISP_INC := $(ISP_PATH)/include
INC := -I$(REL_INC) -I$(ISP_INC)

ifeq ($(GPIO_I2C), Y)
        INC += -I$(EXT_PATH)/gpio-i2c-ov
	INC += -I$(EXT_PATH)/gpio-i2c
else
        INC += -I$(EXT_PATH)/i2c
endif

COMPILE = $(CC) $(CFLAGS) $(DFLAGS) -lm

$(OBJPATH)/%.o: ./%.c
	@[ -e $(LIBPATH) ] || mkdir $(LIBPATH)
	@[ -e $(OBJPATH) ] || mkdir $(OBJPATH)
	@$(COMPILE) -o $@ -c $< $(INC)

SRCS = $(wildcard ./*.c)
OBJS = $(SRCS:%.c=%.o)
OBJS := $(OBJS:./%=obj/%)

TARGETLIB := $(LIBPATH)/libsns_ov5653_5m.a
TARGETLIB_SO := $(LIBPATH)/libsns_ov5653_5m.so

all:$(TARGETLIB)
$(TARGETLIB):$(OBJS)
	@($(AR) $(ARFLAGS) $(TARGETLIB) $(OBJS))
	@($(CC) $(ARFLAGS_SO) $(TARGETLIB_SO) $(OBJS))

clean:
	@$(RM) -rf $(TARGETLIB) $(OBJS)
	@$(RM) -rf $(LIBPATH) $(OBJPATH)
