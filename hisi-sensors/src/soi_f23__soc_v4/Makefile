#
# sensor lib Makefile
#
ifeq ($(PARAM_FILE), )
    PARAM_FILE:=../../../../../../Makefile.param
    include $(PARAM_FILE)
endif

ifeq ($(MPP_BUILD), y)
ifeq ($(CBB_PARAM_FILE), )
    CBB_PARAM_FILE:=../../../../../Makefile.param
    include $(CBB_PARAM_FILE)
endif
endif

ISP_PATH := $(ISP_ROOT)
EXT_PATH := $(ISP_PATH)/ext_drv/$(HIARCH)
3A_PATH  := $(ISP_PATH)/user/3a
LIBPATH = ../../../lib
OBJPATH = ./obj

ARFLAGS = rcv
ARFLAGS_SO = $(LIBS_LD_CFLAGS)
ARFLAGS_SO += -shared -fPIC -o
CFLAGS  += -Wall -fPIC -D$(HI_FPGA)
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

ISP_INC := $(ISP_PATH)/include
CUR_INC := ./
3A_INC := $(3A_PATH)/include
INC := -I$(REL_INC) -I$(ISP_INC) -I$(CUR_INC) -I$(3A_INC) $(COMM_INC)
DEPEND_FILE := $(foreach file,$(subst -I, ,$(CUR_INC)), $(wildcard $(file)/*.h))

COMPILE = $(CC) $(CFLAGS) $(DFLAGS) -lm

$(OBJPATH)/%.o: ./%.c $(DEPEND_FILE)
	@[ -e $(LIBPATH) ] || mkdir -p $(LIBPATH)
	@[ -e $(OBJPATH) ] || mkdir -p $(OBJPATH)
	@$(COMPILE) -o $@ -c $< $(INC)

SRCS = $(wildcard ./*.c)
OBJS = $(SRCS:%.c=%.o)
OBJS := $(OBJS:./%=obj/%)

TARGETLIB := $(LIBPATH)/libsns_f23.a
TARGETLIB_SO := $(LIBPATH)/libsns_f23.so

all:$(TARGETLIB)
$(TARGETLIB):$(OBJS)
	@($(AR) $(ARFLAGS) $(TARGETLIB) $(OBJS))
	@($(CC) $(ARFLAGS_SO) $(TARGETLIB_SO) $(OBJS))
	@mkdir -p $(REL_LIB)
	@cp $(TARGETLIB_SO) $(REL_LIB)
	@cp $(TARGETLIB) $(REL_LIB)
clean:
	@$(RM) -rf $(TARGETLIB) $(OBJS)
	@$(RM) -rf $(LIBPATH) $(OBJPATH)
aa:
	echo "$(INC)"
