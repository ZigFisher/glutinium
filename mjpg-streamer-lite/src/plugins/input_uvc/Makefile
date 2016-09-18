###############################################################
#
# Purpose: Makefile for "M-JPEG Streamer"
# Author.: Tom Stoeveken (TST)
# Version: 0.3
# License: GPL
#
###############################################################

CC ?= gcc

OTHER_HEADERS = ../../mjpg_streamer.h ../../utils.h ../output.h ../input.h

CFLAGS += -O1 -DLINUX -D_GNU_SOURCE -Wall -shared -fPIC

#CFLAGS += -g
#CFLAGS += -DDEBUG

ifeq ($(USE_LIBV4L2),true)
LFLAGS += -lv4l2
CFLAGS += -DUSE_LIBV4L2
endif

ifeq ($(USE_JPEGLIB),true)
CFLAGS += -DUSE_JPEGLIB
LFLAGS += -ljpeg
endif

all: input_uvc.so

clean:
	rm -f *.a *.o core *~ *.so *.lo

input_uvc.so: $(OTHER_HEADERS) input_uvc.c v4l2uvc.lo jpeg_utils.lo dynctrl.lo simplified_jpeg_encoder.lo
	$(CC) $(CFLAGS) -o $@ input_uvc.c v4l2uvc.lo jpeg_utils.lo dynctrl.lo simplified_jpeg_encoder.lo $(LFLAGS)

v4l2uvc.lo: huffman.h uvc_compat.h v4l2uvc.c v4l2uvc.h
	$(CC) -c $(CFLAGS) -o $@ v4l2uvc.c

jpeg_utils.lo: jpeg_utils.c jpeg_utils.h
	$(CC) -c $(CFLAGS) -o $@ jpeg_utils.c

dynctrl.lo: dynctrl.c dynctrl.h
	$(CC) -c $(CFLAGS) -o $@ dynctrl.c

simplified_jpeg_encoder.lo: ../../simplified_jpeg_encoder.c ../../simplified_jpeg_encoder.h
	$(CC) -c $(CFLAGS) -o $@ ../../simplified_jpeg_encoder.c
