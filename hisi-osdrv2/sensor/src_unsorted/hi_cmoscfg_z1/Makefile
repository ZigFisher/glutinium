#
# hi_iniparser lib Makefile
#

ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../../../Makefile.param
	include $(PARAM_FILE)
endif

ISP_PATH := $(SDK_PATH)/mpp/component/isp
LIBPATH = $(ISP_PATH)/lib
OBJPATH = ./obj

INI_INC  := $(ISP_PATH)/iniparser
3A_PATH := $(ISP_PATH)/include

ifeq ($(MPP_BUILD), y)
EXT_PATH := $(SDK_PATH)/mpp/$(EXTDRV)
else
EXT_PATH := $(SDK_PATH)/mpp/extdrv
endif

BUS_DIR := $(EXT_PATH)/pwm

ARFLAGS = rcv
CFLAGS  = -Wall -fPIC

ifeq ($(ISP_INI_CONFIG), y)
CFLAGS += -D INIFILE_CONFIG_MODE
endif

ifeq ($(HIGDB),HI_GDB)
CFLAGS += -g
endif

ifeq ($(CONFIG_JPEGEDCF), y)
     CFLAGS += -D ENABLE_JPEGEDCF 
endif

CFLAGS  += -O2
CFLAGS  += $(LIBS_CFLAGS)
DFLAGS  := -DEXT_REG

ISP_INC := $(ISP_PATH)/include
3A_INC := $(3A_PATH)/include

INC := -I$(REL_INC) -I$(ISP_INC) -I$(BUS_DIR) -I$(INI_INC) -I$(3A_INC)
#-I$(SRC_INC) -I$(VREG_INC) -I$(VREG_INC)/arch/$(HIARCH)

COMPILE = $(CC) $(CFLAGS) $(DFLAGS) -lm

$(OBJPATH)/%.o: ./%.c
	@(echo "compiling $< ...")
	@[ -e $(LIBPATH) ] || mkdir $(LIBPATH)
	@[ -e $(OBJPATH) ] || mkdir $(OBJPATH)
	@($(COMPILE) -o $@ -c $< $(INC))

SRCS = $(wildcard ./*.c)
OBJS = $(SRCS:%.c=%.o)
OBJS := $(OBJS:./%=obj/%)

LIB_A := $(LIBPATH)/lib_cmoscfg.a
LIB_S := $(LIBPATH)/lib_cmoscfg.so

all:$(OBJS)
	@($(AR) $(ARFLAGS) $(LIB_A) $(OBJS))
	@($(CC) $(LIBS_LD_CFLAGS) -shared -fPIC -o $(LIB_S) $(OBJS))

clean:
	@$(RM) -rf $(LIB_A) $(LIB_S) $(OBJS)
	@$(RM) -rf $(LIBPATH) $(OBJPATH)

show:
	@echo "$(QUICK_CONFIG)"	