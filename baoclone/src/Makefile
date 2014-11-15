CC		= gcc -m32

CFLAGS		= -g -O -Wall -Werror
LDFLAGS		=

OBJS		= main.o util.o radio.o uv-5r.o uv-b5.o bf-888s.o ft-60r.o
LIBS            =

all:		baoclone

baoclone:	$(OBJS)
		$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
		strip $@

install:	baoclone
		install -c -s baoclone /usr/local/bin/baoclone

clean:
		rm -f *~ *.o core baoclone

###
bf-888s.o: bf-888s.c radio.h util.h
ft-60r.o: ft-60r.c radio.h util.h
main.o: main.c radio.h util.h
radio.o: radio.c radio.h util.h
util.o: util.c util.h
uv-5r.o: uv-5r.c radio.h util.h
uv-b5.o: uv-b5.c radio.h util.h
