SRCS = gpio-watch.c gpio.c fileutil.c
OBJS = $(SRCS:.c=.o)

all: gpio-watch

gpio-watch: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS)
	rm -f gpio-watch

