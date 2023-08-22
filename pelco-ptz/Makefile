#---------------------------------------------------------------
# May 1, 2002 Joe Briggs
#--------------------------------------------------------------

PELCO = pelco.o
DEFS = pelco.h

CC=~/composer/openipc/output/host/bin/arm-openipc-linux-gnueabihf-cc

#ifeq (,$(findstring Debian,$(shell cat /etc/issue)))
#	CFLAGS := ${CFLAGS} -DREDHAT
#endif



pelco: ${PELCO} ${LIBS} ${DEFS}
	${CC} -o $@ ${PELCO} ${LIBS}

install:
	install -m 0777 pelco /usr/local/bin

clean:
	-rm -f *.o pelco pelco.log


