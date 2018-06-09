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

ARFLAGS = rcv
ARFLAGS_SO = -shared -fPIC -o
CFLAGS  = -Wall
CFLAGS  += -O2

ifeq ($(HIGDB),HI_GDB)
CFLAGS += -g
endif
BUS_DIR := $(EXT_PATH)/ssp-sony/

ISP_INC := $(ISP_PATH)/include
3A_INC := $(3A_PATH)/include
INC := -I$(BUS_DIR) -I$(REL_INC) -I$(ISP_INC) -I$(3A_INC)

COMPILE = $(CC) $(CFLAGS) $(DFLAGS) -lm

$(OBJPATH)/%.o: ./%.c
	@[ -e $(LIBPATH) ] || mkdir $(LIBPATH)
	@[ -e $(OBJPATH) ] || mkdir $(OBJPATH)
	@$(COMPILE) -o $@ -c $< $(INC)

SRCS = $(wildcard ./*.c)
OBJS = $(SRCS:%.c=%.o)
OBJS := $(OBJS:./%=obj/%)

TARGETLIB := $(LIBPATH)/libsns_imx104.a
TARGETLIB_SO := $(LIBPATH)/libsns_imx104.so

all:$(TARGETLIB)
$(TARGETLIB):$(OBJS)
	@($(AR) $(ARFLAGS) $(TARGETLIB) $(OBJS))
	@($(CC) $(ARFLAGS_SO) $(TARGETLIB_SO) $(OBJS))

clean:
	@$(RM) -rf $(TARGETLIB) $(OBJS)
	@$(RM) -rf $(LIBPATH) $(OBJPATH)
