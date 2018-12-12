# hp_ipc Makefile
#
# Put directory hp-ipc into sdk.../mpp/sample/
# Run command: cd hp-ipc && make
#

include ../Makefile.param

INC  := -I./include
SRC  := $(wildcard *.c) $(wildcard ./src/*.c) $(wildcard ./src/sensor/*.c)
OBJ  := $(SRC:%.c=%.o)

CFLAGS += $(INC)

TARGET := hp_ipc
.PHONY : clean all

all: $(TARGET)


$(TARGET):$(OBJ)
	$(CC) $(CFLAGS) -lpthread -lm -o $@ $^ $(MPI_LIBS) $(AUDIO_LIBA) $(SENSOR_LIBS)

clean:
	@rm -f $(TARGET)
	@rm -f $(OBJ)
	@rm -f $(COMM_OBJ)
info:
	echo $(OBJ)
cleanstream:
	@rm -f *.h264
	@rm -f *.jpg
	@rm -f *.mjp
	@rm -f *.mp4
