EXEC = serialoverip

all: $(EXEC)

serialoverip: serialoverip.o
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	-rm -f $(EXEC) *.o

