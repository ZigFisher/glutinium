
VERSION=1.8

UUCP_LOCK_DIR=/var/lock

# CC = gcc
CPPFLAGS=-DVERSION_STR=\"$(VERSION)\" \
         -DUUCP_LOCK_DIR=\"$(UUCP_LOCK_DIR)\" \
         -DHIGH_BAUD
CFLAGS = -Wall -g

# LD = gcc
LDFLAGS = -g
LDLIBS =

picocom: picocom.o term.o

picocom.o : picocom.c term.h
term.o : term.c term.h

strip:
	strip picocom

clean:
	rm -f picocom.o term.o
	rm -f *~
	rm -f \#*\#

distclean: clean
	rm -f picocom
