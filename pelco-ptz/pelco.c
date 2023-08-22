/*-------------------------------- pelco.c -------------------------------
*	Copyright 2002 Briggs Media Systems
*	may 24 02	 modified for pelco camera	J Briggs
*----------------------------------------------------------------------*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "pelco.h"

unsigned char pelco[7];
unsigned char resp[4];
int mdlns;
int addr = 1;

struct baud_rates {
        unsigned int rate;
        unsigned int cflag;
};

static const struct baud_rates baud_rates[] = {
        { 921600, B921600 },
        { 460800, B460800 },
        { 230400, B230400 },
        { 115200, B115200 },
        {  57600, B57600  },
        {  38400, B38400  },
        {  19200, B19200  },
        {   9600, B9600   },
        {   4800, B4800   },
        {   2400, B2400   },
        {   1200, B1200   },
        {      0, B38400  }
};

struct termios oldtio,newtio;
static int verb = 0;	/* reporting verbosity */
int got_ready = 0;
int got_connect = 0;

error_code pelcoInit( u_int32 comport, u_int32 baud_rate,int *fd);
static int mlog( char *s,... );

int fdOut = 0;
int fdIn = 0;


void pelcoDelete(int fd)
{
	tcsetattr(fd,TCSANOW,&oldtio);
	close(fd);
}
error_code pelcoInit( u_int32 comport, u_int32 baud_rate, int *fd )
{
	u_int32 ticks = 0;
	char buf[80];
	char *f = "pelcoInit";

	sprintf(buf,"/dev/ttyS%u",comport);

	if( (*fd = open(buf,O_RDWR|O_NOCTTY)) < 0 )
	{
		printf("ERROR:%s:\n",f);
		perror(buf);
		return -1;
	}
	mlog("%s: port %u dev %s baud %u fd[%u]\n",f,comport,buf,baud_rate,*fd);
	tcgetattr(*fd,&oldtio);
	bzero(&newtio,sizeof(newtio));
	newtio.c_cflag = baud_rate | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;
	newtio.c_cc[VMIN]=1;
	newtio.c_cc[VTIME]=0;
	tcflush(*fd,TCIFLUSH);
	tcsetattr(*fd,TCSANOW,&newtio);
	if( ioctl(*fd,TIOCMGET,&mdlns) )
	{
		perror("pelcoInit:TIOCMGET");
		return -1;
	}
	mdlns |= TIOCM_DTR;
	if( ioctl(*fd,TIOCMSET,&mdlns) )
	{
		perror("pelcoInit:TIOCMSET");
		return -1;
	}
	return 0;
}
/*
*	pelcoput - place a single char into the transmit buffer
*/
error_code pelcoPut( unsigned char c )
{
	if(fdOut)
	{
		if( write(fdOut,&c,1) < 0)
		{
			mlog("error:pelcoput:write\n");
			return -1;
		}
		return 0;
	}
	return -1;
}
/*
*	pelcowrite - normal entry point - write a block of data to the
*	serial port.
*/
error_code pelcoWrite( unsigned char *buf, u_int32 size )
{
	error_code ec = 0;
	int count = 0;
	char *f = "pelcoWrite";

	if( size == 0 ) return ec;

	if( ioctl(fdOut,TIOCMGET,&mdlns) )
	{
		mlog("ERROR: %s:TIOCMGET %s",f,strerror(errno));
		return -1;
	}
	mdlns |= TIOCM_RTS;
	if( ioctl(fdOut,TIOCMSET,&mdlns) )
	{
		mlog("ERROR: %s:TIOCMSET %s",f,strerror(errno));
		return -1;
	}
	usleep(1000);
	if( (count = write(fdOut,buf,size)) != size )
	{
		if( count < 0 )
		{
			mlog("ERROR: %s:WRITE %s",f,strerror(errno));
			ec = 1;
			sleep(2);
		}
	}
	pelcoRead(buf,size,fdIn);
	return ec;
}
/*
*	error_code pelcoget - fetch a char from the receive buffer or a
*	error code.
*/
error_code pelcoGet( unsigned char *c,int fd )
{
	char *f = "pelcoGet";
	if(fd)
	{
		if( read(fd,c,1) < 0 )
		{
			char buf[80];
			mlog("ERROR:%s[%u] %s",f,fd,strerror(errno));
			perror(buf);
			return -1;
		}
		return 0;
	}
	return -1;
}
/*
*	pelcowrite - normal entry point - write a block of data to the
*	serial port.
*/
error_code pelcoRead( unsigned char *buf, u_int32 size,int fd )
{
	u_int32 i;
	int count = 0;
	error_code ec = 0;
	char *f = "pelcoRead";

if( fd != fdIn ) return 0;
	memset(buf,0,size);
	while( !pelcoGet(&buf[0],fd) && (buf[0] != 0xff) );

	if( (count = read(fd,&buf[1],size-1)) != size-1)
	{
		mlog("WARNING:%s: %u vs. %u bytes\n",f,count,size);
		ec = 1;
	}
	if(0)
	{
		printf("READ [");
		for( i = 0; i < size; printf("%02X ",buf[i++]));
		printf("]\n");
	}
	mdlns &= ~TIOCM_RTS;
	if( ioctl(fd,TIOCMSET,&mdlns) )
	{
		mlog("ERROR:%s:pelcoRead:TIOCMSET %s",f,strerror(errno));
		return -1;
	}
	return ec;
}
void pelcoChecksum(void)
{
	int i;
	unsigned char c = 0;
	for( i = 1; i < (sizeof(pelco)-1); c += pelco[i++] );
	pelco[i] = c;
#if 0
	printf("WRITE: [");
	{
		for( i = 0; i < sizeof(pelco); printf("%02X ",pelco[i++]));
		printf("]\n");
	}
#endif
}

int pelcoOn(void)
{
	char *f = "pelcoOn";
	mlog("%s: start\n",f);
	memset(pelco,0,sizeof(pelco));
	pelco[p_sync] = 0xff;
	pelco[p_addr] = addr;
	pelco[p_cmd1] = 0x88;
	pelcoChecksum();
	pelcoWrite(pelco,sizeof(pelco));
	return 0;
}
int pelcoOff(void)
{
	char *f = "pelcoOff";
	mlog("%s: start\n",f);
	memset(pelco,0,sizeof(pelco));
	pelco[p_sync] = 0xff;
	pelco[p_addr] = addr;
	pelco[p_cmd1] = 0x08;
	pelcoChecksum();
	pelcoWrite(pelco,sizeof(pelco));
	return 0;
}
int pelcoLeft(unsigned char force)
{
	char *f = "pelcoLeft";
	mlog("%s: start\n",f);
	memset(pelco,0,sizeof(pelco));
	pelco[p_sync] = 0xff;
	pelco[p_addr] = addr;
	pelco[p_cmd1] = 0x00;
	pelco[p_cmd2] = 0x04;
	pelco[p_data1] = force;
	pelco[p_data2] = 0x00;
	pelcoChecksum();
	pelcoWrite(pelco,sizeof(pelco));
	return 0;
}
int pelcoRight(unsigned char force)
{
	char *f = "pelcoRight";
	mlog("%s: start\n",f);
	memset(pelco,0,sizeof(pelco));
	pelco[p_sync] = 0xff;
	pelco[p_addr] = addr;
	pelco[p_cmd1] = 0x00;
	pelco[p_cmd2] = 0x02;
	pelco[p_data1] = force;
	pelco[p_data2] = 0x00;
	pelcoChecksum();
	pelcoWrite(pelco,sizeof(pelco));
	return 0;
}
int pelcoUp(unsigned char force)
{
	char *f = "pelcoUp";
	mlog("%s: start\n",f);
	memset(pelco,0,sizeof(pelco));
	pelco[p_sync] = 0xff;
	pelco[p_addr] = addr;
	pelco[p_cmd1] = 0x00;
	pelco[p_cmd2] = 0x08;
	pelco[p_data1] = 0x00;
	pelco[p_data2] = force;
	pelcoChecksum();
	pelcoWrite(pelco,sizeof(pelco));
	return 0;
}
int pelcoDown(unsigned char force)
{
	char *f = "pelcoDown";
	mlog("%s: start\n",f);
	memset(pelco,0,sizeof(pelco));
	pelco[p_sync] = 0xff;
	pelco[p_addr] = addr;
	pelco[p_cmd1] = 0x00;
	pelco[p_cmd2] = 0x10;
	pelco[p_data1] = 0x00;
	pelco[p_data2] = force;
	pelcoChecksum();
	pelcoWrite(pelco,sizeof(pelco));
	return 0;
}
int pelcoIn(unsigned char force)
{
	char *f = "pelcoIn";
	mlog("%s: start\n",f);
	memset(pelco,0,sizeof(pelco));
	pelco[p_sync] = 0xff;
	pelco[p_addr] = addr;
	pelco[p_cmd1] = 0x00;
	pelco[p_cmd2] = 0x20;
	pelco[p_data1] = 0;
	pelco[p_data2] = 0;
	pelcoChecksum();
	pelcoWrite(pelco,sizeof(pelco));
	return 0;
}
int pelcoOut(unsigned char force)
{
	char *f = "pelcoIn";
	mlog("%s: start\n",f);
	memset(pelco,0,sizeof(pelco));
	pelco[p_sync] = 0xff;
	pelco[p_addr] = addr;
	pelco[p_cmd1] = 0x00;
	pelco[p_cmd2] = 0x40;
	pelco[p_data1] = 0;
	pelco[p_data2] = 0;
	pelcoChecksum();
	pelcoWrite(pelco,sizeof(pelco));
	return 0;
}
int pelcoNear(unsigned char force)
{
	char *f = "pelcoNear";
	mlog("%s: start\n",f);
	memset(pelco,0,sizeof(pelco));
	pelco[p_sync] = 0xff;
	pelco[p_addr] = addr;
	pelco[p_cmd1] = 0x01;
	pelco[p_cmd2] = 0x00;
	pelco[p_data1] = 0;
	pelco[p_data2] = 0;
	pelcoChecksum();
	pelcoWrite(pelco,sizeof(pelco));
	return 0;
}
int pelcoFar(unsigned char force)
{
	char *f = "pelcoFar";
	mlog("%s: start\n",f);
	memset(pelco,0,sizeof(pelco));
	pelco[p_sync] = 0xff;
	pelco[p_addr] = addr;
	pelco[p_cmd1] = 0x00;
	pelco[p_cmd2] = 0x80;
	pelco[p_data1] = 0;
	pelco[p_data2] = 0;
	pelcoChecksum();
	pelcoWrite(pelco,sizeof(pelco));
	return 0;
}
int pelcoStop(void)
{
	char *f = "pelcoStop";
	mlog("%s: start\n",f);
	memset(pelco,0,sizeof(pelco));
	pelco[p_sync] = 0xff;
	pelco[p_addr] = addr;
	pelco[p_cmd1] = 0x00;
	pelco[p_cmd2] = 0x00;
	pelco[p_data1] = 0x00;
	pelco[p_data2] = 0x00;
	pelcoChecksum();
	pelcoWrite(pelco,sizeof(pelco));
	return 0;
}
int pelcoSet(unsigned location)
{
	char *f = "pelcoSet";
	mlog("%s: %d\n",f,location);
	memset(pelco,0,sizeof(pelco));
	pelco[p_sync] = 0xff;
	pelco[p_addr] = addr;
	pelco[p_cmd1] = 0x00;
	pelco[p_cmd2] = 0x03;
	pelco[p_data1] = 0x00;
	pelco[p_data2] = location;
	pelcoChecksum();
	pelcoWrite(pelco,sizeof(pelco));
	return 0;
}
int pelcoGo(unsigned location)
{
	char *f = "pelcoGo";
	mlog("%s: %d\n",f,location);
	memset(pelco,0,sizeof(pelco));
	pelco[p_sync] = 0xff;
	pelco[p_addr] = addr;
	pelco[p_cmd1] = 0x00;
	pelco[p_cmd2] = 0x07;
	pelco[p_data1] = 0x00;
	pelco[p_data2] = location;
	pelcoChecksum();
	pelcoWrite(pelco,sizeof(pelco));
	return 0;
}
int pelcoClear(unsigned location)
{
	char *f = "pelcoGo";
	mlog("%s: %d\n",f,location);
	memset(pelco,0,sizeof(pelco));
	pelco[p_sync] = 0xff;
	pelco[p_addr] = addr;
	pelco[p_cmd1] = 0x00;
	pelco[p_cmd2] = 0x05;
	pelco[p_data1] = 0x00;
	pelco[p_data2] = location;
	pelcoChecksum();
	pelcoWrite(pelco,sizeof(pelco));
	return 0;
}

char *use[]=
{
	"\n"
	"pelco -p[port] -b[baud] -a[addr] command -f[force] -t[pulse-time]",
	"\n"
	"commmands: left, right, up, down, in, out, near, far, set, go, clear",
	"\n"
	"f - force level, 0 - 63",
	"t - time to assert in seconds",
	"\n"
	"Copyright 2002 Briggs Media Systems LLC",
	"Copyright 2022 OpenIPC team",
	""
};

void usage(void)
{
	int i;
	for( i = 0; i < sizeof(use)/sizeof(char *); printf("%s\n",use[i++]));
	exit(0);
}
int main(int argc,char *argv[])
{
	char *f = argv[0];
	int i;
	char *s;
	int port = 2;
	int baud = 9600;
	char *cmd = (char *)0;
	unsigned char force = 0x40/2;
	int pulse = 1;

	for( i = 1; i < argc; i++ )
	{
		if( *(s = argv[i]) == '-' ) switch( *(++s) )
		{
			case 'b': baud = atoi(++s); break;
			case 'p': port = atoi(++s); break;
			case 'a': addr = atoi(++s); break;
			case 'f': force = atoi(++s); break;
			case 't': pulse = atoi(++s); break;
			case '?':
			default: usage();
		}else cmd = s;
	}


	for(i=0; baud_rates[i].rate; i++) {
		if(baud_rates[i].rate == baud) {
			baud = baud_rates[i].cflag;
			break;
		}
	}
	if (baud_rates[i].rate == 0) {
		printf("Invalid baudrate\n");
		return -1;
	}

	pelcoInit(port, baud, &fdOut);
	fdIn = fdOut;
	if( !cmd ) usage();
	//mlog("main address[%d] cmd[%s] fdout[%u] fdIn[%u]\n",addr,cmd,fdOut,fdIn);
	mlog("main address[%d] cmd[%s]\n",addr,cmd);
	pelcoOn();
	if( !cmd ) usage();
	for( i = 0; i < pulse; i++ )
	{
		mlog("loop[%u] cmd[%s]\n",i,cmd);
		if( !strcmp(cmd,"left") ) pelcoLeft(force);
		else if( !strcmp(cmd,"right") ) pelcoRight(force);
		else if( !strcmp(cmd,"up") ) pelcoUp(force);
		else if( !strcmp(cmd,"down") ) pelcoDown(force);
		else if( !strcmp(cmd,"in") ) pelcoIn(force);
		else if( !strcmp(cmd,"out") ) pelcoOut(force);
		else if( !strcmp(cmd,"near") ) pelcoNear(force);
		else if( !strcmp(cmd,"far") ) pelcoFar(force);
		else if( !strcmp(cmd,"set") ) pelcoSet(force);
		else if( !strcmp(cmd,"go") ) pelcoGo(force);
		else if( !strcmp(cmd,"clear") ) pelcoClear(force);
		else mlog("WARNING:no command match for %s\n",cmd);
	}
	pelcoStop();
	pelcoOff();
	pelcoDelete(fdOut);
	pelcoDelete(fdIn);
}
static int mlog( char *s,... )
{
	char obuf[128];
	char buf[128];
	char *t,tbuf[128];
	va_list args;
	FILE *fp;
	char *mode = "a";
	char *file = "./pelco.log";
	time_t tm = time(0);
	static unsigned short ecnt = 0;

	va_start(args,s);
	vsprintf(obuf,s,args);
	va_end(args);
	/*
	*	limit log size to 256 entries
	*/
	if( ++ecnt == 0 )
	{
		mode = "w";
	}
	if( (fp = fopen(file,mode)) <= 0 )
	{
		printf("elog:fopen:ERROR: on %s\n",file);
		return -1;
	}
	if( (t = ctime(&tm)))
	{
		strcpy(tbuf,t);
		tbuf[strlen(tbuf)-1] = 0;
		sprintf(buf,"%s: %s",tbuf,obuf);
		fprintf(fp, "%s", buf);
	}
	else printf("pelco: bad ctime\n");
	fclose(fp);
	return 0;
}
