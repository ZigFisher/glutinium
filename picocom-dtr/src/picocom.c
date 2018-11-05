/* vi: set sw=4 ts=4:
 *
 * picocom.c
 *
 * simple dumb-terminal program. Helps you manually configure and test
 * stuff like modems, devices w. serial ports etc.
 *
 * by Nick Patavalis (npat@efault.net)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>
#include <stdarg.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>

#define _GNU_SOURCE
#include <getopt.h>


#ifdef __linux__
#include <termio.h>
#else
#include <termios.h>
#endif /* of __linux__ */

#include "term.h"

// Date of compilation, created by ccc script; added in order to track changes during rapid development phases.
// Not placed to makefile in order to save the changes.
#include "compiledate.h"
#define VERSION_STR_SHAD VERSION_STR"-ZFT_Lab.-"VERSION_DATE



/**********************************************************************/

//#define KEY_EXIT    '\x18' /* C-x: exit picocom */
//#define KEY_QUIT    '\x11' /* C-q: exit picocom without reseting port */
//#define KEY_PULSE   '\x10' /* C-p: pulse DTR */
//#define KEY_TOGGLE  '\x14' /* C-t: toggle DTR */
//#define KEY_BAUD_UP '\x15' /* C-u: increase baudrate (up) */
//#define KEY_BAUD_DN '\x04' /* C-d: decrase baudrate (down) */ 
//#define KEY_FLOW    '\x06' /* C-f: change flowcntrl mode */ 
//#define KEY_PARITY  '\x19' /* C-y: change parity mode */ 
//#define KEY_BITS    '\x02' /* C-b: change number of databits */ 
//#define KEY_LECHO   '\x03' /* C-c: toggle local echo */ 
//#define KEY_STATUS  '\x16' /* C-v: show program option */
//#define KEY_SEND    '\x13' /* C-s: send file */
//#define KEY_RECEIVE '\x12' /* C-r: receive file */
//#define KEY_BREAK   '\x1c' /* C-\: break */

#define KEY_BITS    '\x02' /* C-b: change number of databits */ 
#define KEY_LECHO   '\x03' /* C-c: toggle local echo */ 
#define KEY_BAUD_DN '\x04' /* C-d: decrase baudrate (down) */ 
//#define KEY_RTS     '\x05' /* C-e: toggle RTS */ 
#define KEY_RECEIVE '\x05' /* C-e: receive file */
#define KEY_FLOW    '\x06' /* C-f: change flowcntrl mode */ 
#define KEY_SETBAUD '\x07' /* C-g: enter arbitrary baud rate */ 
#define KEY_HEX     '\x08' /* C-h: enter hex string, send as binary */ 
#define KEY_PULSERTS   '\x09' /* C-i: pulse DTR */
#define KEY_TERMRESET '\x0f' /* C-o: reset terminal */
#define KEY_PULSE   '\x10' /* C-p: pulse DTR */
#define KEY_QUIT    '\x11' /* C-q: exit picocom without reseting port */
//#define KEY_RECEIVE '\x12' /* C-r: receive file */
#define KEY_RTS     '\x12' /* C-r: toggle RTS */ 
#define KEY_SEND    '\x13' /* C-s: send file */
#define KEY_TOGGLE  '\x14' /* C-t: toggle DTR */
#define KEY_BAUD_UP '\x15' /* C-u: increase baudrate (up) */
#define KEY_STATUS  '\x16' /* C-v: show program option */
#define KEY_EXIT    '\x18' /* C-x: exit picocom */
#define KEY_PARITY  '\x19' /* C-y: change parity mode */ 
#define KEY_SHELL   '\x1a' /* C-z: spawn shell */ 
#define KEY_BREAK   '\x1c' /* C-\: break */
#define KEY_HELP    '?'    /* ?:   show key mapping */

#define STO STDOUT_FILENO
#define STI STDIN_FILENO

/**********************************************************************/

/* implemented caracter mappings */
#define M_CRLF   (1 << 0) /* map CR  --> LF */
#define M_CRCRLF (1 << 1) /* map CR  --> CR + LF */
#define M_IGNCR  (1 << 2) /* map CR  --> <nothing> */
#define M_LFCR   (1 << 3) /* map LF  --> CR */
#define M_LFCRLF (1 << 4) /* map LF  --> CR + LF */
#define M_IGNLF  (1 << 5) /* map LF  --> <nothing> */
#define M_DELBS  (1 << 6) /* map DEL --> BS */
#define M_BSDEL  (1 << 7) /* map BS  --> DEL */
#define M_HEX    (1 << 8) /* enable hex dump as specified by opts.hexmap */
#define M_NFLAGS 9

/* default character mappings */
#define M_I_DFL M_HEX
#define M_O_DFL 0
#define M_E_DFL (M_DELBS | M_CRCRLF | M_HEX)

/* hex dump handling */
#define HEX_NONE 0
#define HEX_NOASCII 1
#define HEX_ALL 255

/* color handling */
#define COLOR_NONE 0
#define COLOR_NORMAL 1
#define COLOR_BRIGHT 2



/* character mapping names */
struct map_names_s {
	char *name;
	int flag;
} map_names[] = {
	{ "crlf", M_CRLF },
	{ "crcrlf", M_CRCRLF },
	{ "igncr", M_IGNCR },
	{ "lfcr", M_LFCR },
	{ "lfcrlf", M_LFCRLF },
	{ "ignlf", M_IGNLF },
	{ "delbs", M_DELBS },
	{ "bsdel", M_BSDEL },
	{ "hexmap", M_HEX },
	/* Sentinel */
	{ NULL, 0 } 
};

int
fd_printf (int fd, const char *format, ...);


int
parse_map (char *s)
{
	char *m, *t;
	int f, flags, i;

	flags = 0;
	while ( (t = strtok(s, ", \t")) ) {
		for (i=0; (m = map_names[i].name); i++) {
			if ( ! strcmp(t, m) ) {
				f = map_names[i].flag;
				break;
			}
		}
		if ( m ) flags |= f;
		else { flags = -1; break; }
		s = NULL;
	}

	return flags;
}

void
print_map (int flags)
{
	int i,j=0;

	for (i = 0; i < M_NFLAGS; i++)
		if ( flags & (1 << i) ) {
			if(j)fd_printf(STO,",");
			fd_printf(STO,"%s", map_names[i].name); j++;
		}
	fd_printf(STO,"\r\n");
}

/**********************************************************************/

struct {
	char port[128];
	int baud;
	enum flowcntrl_e flow;
	char *flow_str;
	enum parity_e parity;
	char *parity_str;
	int databits;
	int lecho;
	int noinit;
	int noreset;
#ifdef UUCP_LOCK_DIR
	int nolock;
#endif
	unsigned char escape;
	char send_cmd[128];
	char receive_cmd[128];
	char terminal_reset_cmd[128];
	int imap;
	int omap;
	int emap;
	int hexmap;
	int color;
	int initdtr;
	int initrts;
	int watchmodemlines;
} opts = {
	.port = "",
	.baud = 9600,
	.flow = FC_NONE,
	.flow_str = "none",
	.parity = P_NONE,
	.parity_str = "none",
	.databits = 8,
	.lecho = 0,
	.noinit = 0,
	.noreset = 0,
#ifdef UUCP_LOCK_DIR
	.nolock = 0,
#endif
	.escape = '\x01',
	.send_cmd = "sz -vv",
	.receive_cmd = "rz -vv",
	.terminal_reset_cmd = "reset",
	.hexmap = HEX_NOASCII,
	.color = COLOR_NONE,
	.initdtr = -1,
	.initrts = -1,
	.watchmodemlines = 1,
	.imap = M_I_DFL,
	.omap = M_O_DFL,
	.emap = M_E_DFL
};

int tty_fd;

/**********************************************************************/

#ifdef UUCP_LOCK_DIR

/* use HDB UUCP locks  .. see
 * <http://www.faqs.org/faqs/uucp-internals> for details
 */

char lockname[_POSIX_PATH_MAX] = "";

int
uucp_lockname(const char *dir, const char *file)
{
	char *p, *cp;
	struct stat sb;

	if ( ! dir || *dir == '\0' || stat(dir, &sb) != 0 )
		return -1;

	/* cut-off initial "/dev/" from file-name */
	p = strchr(file + 1, '/');
	p = p ? p + 1 : (char *)file;
	/* replace '/'s with '_'s in what remains (after making a copy) */
	p = cp = strdup(p);
	do { if ( *p == '/' ) *p = '_'; } while(*p++);
	/* build lockname */
	snprintf(lockname, sizeof(lockname), "%s/LCK..%s", dir, cp);
	/* destroy the copy */
	free(cp);

	return 0;
}

int
uucp_lock(void)
{
	int r, fd, pid;
	char buf[16];
	mode_t m;

	if ( lockname[0] == '\0' ) return 0;

	fd = open(lockname, O_RDONLY);
	if ( fd >= 0 ) {
		r = read(fd, buf, sizeof(buf)); 
		close(fd);
		/* if r == 4, lock file is binary (old-style) */
		pid = (r == 4) ? *(int *)buf : strtol(buf, NULL, 10);
		if ( pid > 0 
			 && kill((pid_t)pid, 0) < 0 
			 && errno == ESRCH ) {
			/* stale lock file */
			printf("Removing stale lock: %s\n", lockname);
			sleep(1);
			unlink(lockname);
		} else {
			lockname[0] = '\0';
			errno = EEXIST;
			return -1;
		}
	}
	/* lock it */
	m = umask(022);
	fd = open(lockname, O_WRONLY|O_CREAT|O_EXCL, 0666);
	if ( fd < 0 ) { lockname[0] = '\0'; return -1; }
	umask(m);
	snprintf(buf, sizeof(buf), "%04d\n", getpid());
	write(fd, buf, strlen(buf));
	close(fd);

	return 0;
}

int
uucp_unlock(void)
{
	if ( lockname[0] ) unlink(lockname);
	return 0;
}

#endif /* of UUCP_LOCK_DIR */

/**********************************************************************/

#define COL_RESET 0
#define COL_IN  1
#define COL_IN_HEX 2
#define COL_OUT 3
#define COL_OUT_HEX 4
#define COL_SYS 5
#define COL_MODEMLINES 6

int color_last=COL_RESET;
int color=COL_RESET;

ssize_t
writen_ni(int fd, const void *buff, size_t n)
{
	size_t nl; 
	ssize_t nw;
	const char *p;

	p = buff;
	nl = n;
	while (nl > 0) {
		do {
			nw = write(fd, p, nl);
		} while ( nw < 0 && errno == EINTR );
		if ( nw <= 0 ) break;
		nl -= nw;
		p += nw;
	}
	
	return n - nl;
}

#define ANSI_COLOR_RED      "\x1b[0;31m"
#define ANSI_COLOR_GREEN    "\x1b[0;32m"
#define ANSI_COLOR_YELLOW   "\x1b[0;33m"
#define ANSI_COLOR_BLUE     "\x1b[0;34m"
#define ANSI_COLOR_MAGENTA  "\x1b[0;35m"
#define ANSI_COLOR_CYAN     "\x1b[0;36m"
#define ANSI_COLOR_BRED     "\x1b[1;31m"
#define ANSI_COLOR_BGREEN   "\x1b[1;32m"
#define ANSI_COLOR_BYELLOW  "\x1b[1;33m"
#define ANSI_COLOR_BBLUE    "\x1b[1;34m"
#define ANSI_COLOR_BMAGENTA "\x1b[1;35m"
#define ANSI_COLOR_BCYAN    "\x1b[1;36m"
#define ANSI_COLOR_RESET    "\x1b[0m"


char*getcolor(int color)
{
	switch(color) {
	case COL_IN:
		if(opts.color==COLOR_BRIGHT)return ANSI_COLOR_BGREEN;
		return ANSI_COLOR_GREEN;
	case COL_OUT:
		if(opts.color==COLOR_BRIGHT)return ANSI_COLOR_BRED;
		return ANSI_COLOR_RED;
	case COL_IN_HEX:
		if(opts.color==COLOR_BRIGHT)return ANSI_COLOR_GREEN;
		return ANSI_COLOR_BGREEN;
	case COL_OUT_HEX:
		if(opts.color==COLOR_BRIGHT)return ANSI_COLOR_RED;
		return ANSI_COLOR_BRED;
	case COL_SYS:
		return ANSI_COLOR_BYELLOW;
	case COL_MODEMLINES:
		return ANSI_COLOR_CYAN;
	default:
		return ANSI_COLOR_RESET;
	}
}

void printcolor()
{
	char*sp;
	if(opts.color==COLOR_NONE)return;
	if(color_last==color)return;
	sp=getcolor(color);
	writen_ni(STO,sp,strlen(sp));
//char s[32];sprintf(s,"[col:%i:%i]",color,color_last);writen_ni(STO,s,strlen(s));
	color_last=color;
}

void printsetcolor(int col)
{
	char*sp;
	if(opts.color==COLOR_NONE)return;
	color=col;
//char s[32];sprintf(s,"{%i}",col);writen_ni(STO,s,strlen(s));
//char s[32];sprintf(s,"[col:%i:%i[%c]]",color,color_last,c);writen_ni(STO,s,strlen(s));
	if(color_last==color)return;
	sp=getcolor(color);
	writen_ni(STO,sp,strlen(sp));
	color_last=color;
}

/*
void printsetcolor2(int col)
{
	char*sp;
	color=col;
char s[32];sprintf(s,"[col:%i:%i:%i]",color,color_last,col);writen_ni(STO,s,strlen(s));
	if(color_last==color)return;
	sp=getcolor(color);
	writen_ni(STO,sp,strlen(sp));
	color_last=color;
}
*/

void printdebug(char c,char c2,char*s1)
{
char s[32];sprintf(s,"{%c|%c|%s}",c,c2,s1);writen_ni(STO,s,strlen(s));
}



int
fd_printf (int fd, const char *format, ...)
{
	char buf[256];
	va_list args;
	int len;
	
	va_start(args, format);
	len = vsnprintf(buf, sizeof(buf), format, args);
	buf[sizeof(buf) - 1] = '\0';
	va_end(args);
	
	if(fd==STO)printcolor();
	return writen_ni(fd, buf, len);
}


void
fatal (const char *format, ...)
{
	char *s, buf[256];
	va_list args;
	int len;

	term_reset(STO);
	term_reset(STI);
	
	va_start(args, format);
	len = vsnprintf(buf, sizeof(buf), format, args);
	buf[sizeof(buf) - 1] = '\0';
	va_end(args);
	
	printsetcolor(COL_SYS);
	s = "\r\nFATAL: ";
	writen_ni(STO, s, strlen(s));
	writen_ni(STO, buf, len);
	s = "\r\n";
	writen_ni(STO, s, strlen(s));

	/* wait a bit for output to drain */
	sleep(1);

#ifdef UUCP_LOCK_DIR
	uucp_unlock();
#endif
	
	exit(EXIT_FAILURE);
}

#define cput(fd, c) do { int cl = c; write((fd), &(cl), 1); } while(0)

int
fd_readline (int fdi, int fdo, char *b, int bsz)
{
	int r;
	unsigned char c;
	unsigned char *bp, *bpe;
	
	bp = (unsigned char *)b;
	bpe = (unsigned char *)b + bsz - 1;

	while (1) {
		r = read(fdi, &c, 1);
		if ( r <= 0 ) { r--; goto out; }

		switch (c) {
		case '\b':
			if ( bp > (unsigned char *)b ) { 
				bp--;
				cput(fdo, c); cput(fdo, ' '); cput(fdo, c);
			} else {
				cput(fdo, '\x07');
			}
			break;
		case '\r':
			*bp = '\0';
			r = bp - (unsigned char *)b; 
			goto out;
		default:
			if ( bp < bpe ) { *bp++ = c; cput(fdo, c); }
			else { cput(fdo, '\x07'); }
			break;
		}
	}

out:
	return r;
}

#undef cput

int ishexdigit(char c)
{ if(c>='0')if(c<='9')return 1;
  if(c>='A')if(c<='F')return 1;
  if(c>='a')if(c<='f')return 1;
  return 0;
}

int gethexdigit(char c)
{ if(c>='0')if(c<='9')return c-'0';
  if(c>='A')if(c<='F')return c-'A'+0x0a;
  if(c>='a')if(c<='f')return c-'a'+0x0a;
  return 0;
}

int gethex(char*sp)
{ //int c=0;
  return (gethexdigit(sp[0])<<4) + gethexdigit(sp[1]);
}


/* maximum number of chars that can replace a single characted
   due to mapping */
#define M_MAXMAP 6

int do_map_hex(char*b,char c)
{
	if(color==COL_IN) color=COL_IN_HEX; else
	if(color==COL_OUT) color=COL_OUT_HEX;
	sprintf(b,"[%02X]",(unsigned char)c);
	return 4;
}

int
do_map (char *b, int map, char c)
{
	int n;

	if( map & M_HEX ) if(opts.hexmap==HEX_ALL) {
		return do_map_hex(b,c);
	}

	switch (c) {
	case '\x7f':
		/* DEL mapings */
		if ( map & M_DELBS ) {
			b[0] = '\x08'; n = 1;
		} else {
			b[0] = c; n = 1;
		}
		break;
	case '\x08':
		/* BS mapings */
		if ( map & M_BSDEL ) {
			b[0] = '\x7f'; n = 1;
		} else {
			b[0] = c; n = 1;
		}
		break;
	case '\x0d':
		/* CR mappings */
		if ( map & M_CRLF ) {
			b[0] = '\x0a'; n = 1;
		} else if ( map & M_CRCRLF ) {
			b[0] = '\x0d'; b[1] = '\x0a'; n = 2;
		} else if ( map & M_IGNCR ) {
			n = 0;
		} else {
			b[0] = c; n = 1;
		}
		break;
	case '\x0a':
		/* LF mappings */
		if ( map & M_LFCR ) {
			b[0] = '\x0d'; n = 1;
		} else if ( map & M_LFCRLF ) {
			b[0] = '\x0d'; b[1] = '\x0a'; n = 2;
		} else if ( map & M_IGNLF ) {
			n = 0;
		} else {
			b[0] = c; n = 1;
		}
		break;
	default:
		if( map & M_HEX ) if(opts.hexmap) {
//			if((c<0x20)||((c>=0x80)&&(c<0xA0))){
			if(((unsigned char)c<0x20)||((unsigned char)c>=0x80)){
//				sprintf(b,"[%02X]",(unsigned char)c);
				n=do_map_hex(b,c);
				break;
			}
		}
		b[0] = c; n = 1;
		break;
	}

	return n;
}

void 
map_and_write (int fd, int map, char c)
{
	char b[M_MAXMAP];
	int n;
		
	n = do_map(b, map, c);
	if(fd==STO)printcolor();
	if ( n )
		if ( writen_ni(fd, b, n) < n )
			fatal("write to stdout failed: %s", strerror(errno));		
}

void 
map_and_write_color (int fd, int map, char c,int col)
{
//	printsetcolor(col);
	color=col;
	map_and_write(fd,map,c);
}


/**********************************************************************/

#define BAUDCOUNT 64

/*
int baudrates[BAUDCOUNT]={
  75, 150, 300, 600, 1200, 2400, 4800, 9600,
  19200, 38400, 57600, 115200, 230400, 250000, 460800, 921600,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};*/
int baudrates[BAUDCOUNT];

// list the baud rates in the array
void baud_dump()
{
	int t;
	fd_printf(STO,"baudrates: ");
	for(t=0;t<BAUDCOUNT;t++){
		if(!baudrates[t])continue;
		if(t)fd_printf(STO,",");
		fd_printf(STO,"%i",baudrates[t]);
	}
	fd_printf(STO,"\r\n");
}

void baud_clear()
{
	int t;
	for(t=0;t<BAUDCOUNT;t++)baudrates[t]=0;
}

// add a value to the array
void baud_add(int baud)
{
	int t,t1;
	if(baud<baudrates[0])return;
	for(t=0;t<BAUDCOUNT;t++){
		if(baudrates[t]==baud)return;
		if(baudrates[t]>baud){
			for(t1=BAUDCOUNT-1;t1>t;t1--){
				baudrates[t1]=baudrates[t1-1];
			}
			baudrates[t]=baud;
			return;
		}
		if(baudrates[t]==0){baudrates[t]=baud;return;}
	}
}

// see list of baud rates for CP2102: https://www.silabs.com/Support%20Documents/TechnicalDocs/an205.pdf
// modem - from http://digital.ni.com/public.nsf/allkb/D37754FFA24F7C3F86256706005B9BE7
//#define BAUDLIST_DEFAULT "300,600,1200,2400,4800,9600,19200,38400,57600,115200,230400,460800,921600"
#define BAUDLIST_DEFAULT "300,600,1200,2400,4800,9600,19200,38400,57600,115200,230400,250000,460800,500000,921600"
#define BAUDLIST_CP2101_ADD "1800,7200,14400,28800,56000,128000"
#define BAUDLIST_CP2102_ADD "4000,16000,51200,64000,76800,153600,250000,256000,500000,576000"
#define BAUDLIST_MIDI "31250,125000,250000"
#define BAUDLIST_CAN_ADD "128000,153600,250000,256000,500000,1000000"
#define BAUDLIST_ESP8266_ADD "74880,1843200,2686400"
#define BAUDLIST_NICAN "5000,6150,7813,8000,10000,12500,15625,16000,20000,25000,31250,40000,50000,62500,80000,100000,125000,160000,200000,250000,400000,500000,800000,1000000"


void baud_addstrstr(char*s)
{
	char*sp;
	int i;
	do{
		sp=strchr(s,',');
		i=atoi(s);
		s=sp;s++;
		if(!i)continue;
		baud_add(i);
	}while(sp);
}

void baud_addstr(char*s,int clear)
{
	if(clear)baud_clear();
	if(!strcasecmp(s,"DEFAULT")){baud_addstrstr(BAUDLIST_DEFAULT);return;}
	if(!strcasecmp(s,"CP2101")){baud_addstrstr(BAUDLIST_DEFAULT);baud_addstrstr(BAUDLIST_CP2101_ADD);return;}
	if(!strcasecmp(s,"CP2102")){baud_addstrstr(BAUDLIST_DEFAULT);baud_addstrstr(BAUDLIST_CP2101_ADD);baud_addstrstr(BAUDLIST_CP2102_ADD);return;}
	if(!strcasecmp(s,"MIDI")){baud_addstrstr(BAUDLIST_MIDI);return;}
	if(!strcasecmp(s,"CAN")){baud_addstrstr(BAUDLIST_DEFAULT);baud_addstrstr(BAUDLIST_CAN_ADD);return;}
	if(!strcasecmp(s,"ESP8266")){baud_addstrstr(BAUDLIST_DEFAULT);baud_addstrstr(BAUDLIST_ESP8266_ADD);return;}
// NI-CAN baud rates from National Instruments - http://www.ni.com/tutorial/4304/en/
	if(!strcasecmp(s,"NICAN")){baud_addstrstr(BAUDLIST_NICAN);return;}
	baud_addstrstr(s);
}


int baud_up (int baud)
{
	int t;
	if(baud<baudrates[0])return baudrates[0];
	for(t=0;t<BAUDCOUNT;t++){
		if(baudrates[t]==0)break;
		if(baudrates[t]>baud)return baudrates[t];
	}
	return baud;
}

int baud_down (int baud)
{
	int t;
	if(baud<baudrates[0])return baudrates[0];
	for(t=BAUDCOUNT-1;t>=0;t--){
		if(baudrates[t]==0)continue;
		if(baudrates[t]<baud)return baudrates[t];
	}
	return baudrates[0];
}

int
flow_next (int flow, char **flow_str)
{
	switch(flow) {
	case FC_NONE:
		flow = FC_RTSCTS;
		*flow_str = "RTS/CTS";
		break;
	case FC_RTSCTS:
		flow = FC_XONXOFF;
		*flow_str = "xon/xoff";
		break;
	case FC_XONXOFF:
		flow = FC_NONE;
		*flow_str = "none";
		break;
	default:
		flow = FC_NONE;
		*flow_str = "none";
		break;
	}

	return flow;
}

int
parity_next (int parity, char **parity_str)
{
	switch(parity) {
	case P_NONE:
		parity = P_EVEN;
		*parity_str = "even";
		break;
	case P_EVEN:
		parity = P_ODD;
		*parity_str = "odd";
		break;
	case P_ODD:
		parity = P_NONE;
		*parity_str = "none";
		break;
	default:
		parity = P_NONE;
		*parity_str = "none";
		break;
	}

	return parity;
}

int
bits_next (int bits)
{
	bits++;
	if (bits > 8) bits = 5;

	return bits;
}


void getportopts()
{
	
}



/**********************************************************************/

void
child_empty_handler (int signum)
{
}

void
establish_child_signal_handlers (void)
{
	struct sigaction empty_action;
	
	/* Set up the structure to specify the "empty" action. */
    empty_action.sa_handler = child_empty_handler;
	sigemptyset (&empty_action.sa_mask);
	empty_action.sa_flags = 0;
	
	sigaction (SIGINT, &empty_action, NULL);
	sigaction (SIGTERM, &empty_action, NULL);
}

int
run_cmd(int fd, ...)
{
	pid_t pid;
	sigset_t sigm, sigm_old;

	/* block signals, let child establish its own handlers */
	sigemptyset(&sigm);
	sigaddset(&sigm, SIGTERM);
	sigprocmask(SIG_BLOCK, &sigm, &sigm_old);

	pid = fork();
	if ( pid < 0 ) {
		sigprocmask(SIG_SETMASK, &sigm_old, NULL);
		fd_printf(STO, "*** cannot fork: %s\n", strerror(errno));
		return -1;
	} else if ( pid ) {
		/* father: picocom */
		int r;

		/* reset the mask */
		sigprocmask(SIG_SETMASK, &sigm_old, NULL);
		/* wait for child to finish */
		waitpid(pid, &r, 0);
		/* reset terminal (back to raw mode) */
		term_apply(STI);
		/* check and report child return status */
		if ( WIFEXITED(r) ) { 
			fd_printf(STO, "\r\n*** exit status: %d\r\n", 
					  WEXITSTATUS(r));
			return WEXITSTATUS(r);
		} else {
			fd_printf(STO, "\r\n*** abnormal termination: 0x%x\r\n", r);
			return -1;
		}
	} else {
		/* child: external program */
		int r;
		long fl;
		char cmd[512];

		establish_child_signal_handlers();
		sigprocmask(SIG_SETMASK, &sigm_old, NULL);
		/* unmanage terminal, and reset it to canonical mode */
		term_remove(STI);
		/* unmanage serial port fd, without reset */
		term_erase(fd);
		/* set serial port fd to blocking mode */
		fl = fcntl(fd, F_GETFL); 
		fl &= ~O_NONBLOCK;
		fcntl(fd, F_SETFL, fl);
		/* connect stdin and stdout to serial port */
		close(STI);
		close(STO);
		dup2(fd, STI);
		dup2(fd, STO);
		{
			/* build command-line */
			char *c, *ce;
			const char *s;
			int n;
			va_list vls;
			
			c = cmd;
			ce = cmd + sizeof(cmd) - 1;
			va_start(vls, fd);
			while ( (s = va_arg(vls, const char *)) ) {
				n = strlen(s);
				if ( c + n + 1 >= ce ) break;
				memcpy(c, s, n); c += n;
				*c++ = ' ';
			}
			va_end(vls);
			*c = '\0';
		}
		/* run extenral command */
		fd_printf(STDERR_FILENO, "%s\n", cmd);
		r = system(cmd);
		if ( WIFEXITED(r) ) exit(WEXITSTATUS(r));
		else exit(128);
	}
}

/**********************************************************************/

#define TTY_Q_SZ 256

struct tty_q {
	int len;
	unsigned char buff[TTY_Q_SZ];
} tty_q;

/**********************************************************************/

void printhelp(char*key,char*desc)
{
	fd_printf(STO,"    %-6s:  %s\r\n",key,desc);
}

void printhelp_section(char*desc)
{
	fd_printf(STO,"%s\r\n",desc);
}

int getdtr(int tty)
{
	int i;
	i=term_get_modem_flags(tty);
	if(i&TIOCM_DTR)return 1;
	return 0;
}

int getrts(int tty)
{
	int i;
	i=term_get_modem_flags(tty);
	if(i&TIOCM_RTS)return 1;
	return 0;
}

void print_modem_flags_raw(int tty)
{
	int i;
	i=term_get_modem_flags(tty);
	//fd_printf(STO,"[%02X] ",i);
	if(i&TIOCM_DTR)fd_printf(STO,"DTR+ ");else fd_printf(STO,"dtr- ");
	if(i&TIOCM_RTS)fd_printf(STO,"RTS+ ");else fd_printf(STO,"rts- ");
	if(i&TIOCM_CTS)fd_printf(STO,"CTS+ ");else fd_printf(STO,"cts- ");
	if(i&TIOCM_CAR)fd_printf(STO,"DCD+ ");else fd_printf(STO,"dcd- ");
	if(i&TIOCM_DSR)fd_printf(STO,"DSR+ ");else fd_printf(STO,"dsr- ");
	if(i&TIOCM_RNG)fd_printf(STO,"RI+ ");else fd_printf(STO,"ri- ");
}

void print_modem_flags(int tty, int laststars)
{
	fd_printf(STO,"*** modem: ");
	print_modem_flags_raw(tty);
	if(laststars)fd_printf(STO,"***");
	fd_printf(STO,"\r\n");
}

int modemflags_old;

void modemflags_printdiff(int old,int i,int flag,char*nameu,char*namel)
{
	int bitnew,bitold;
	bitold=old&flag;
	bitnew=i&flag;
	if(bitold==bitnew)return;
	if(bitnew){fd_printf(STO,"[%s+]",nameu);return;}
	fd_printf(STO,"[%s-]",namel);
}

void printmodemflagsdiff(int tty)
{
	int i;
	i=term_get_modem_flags(tty);
	if(modemflags_old!=i){
		printsetcolor(COL_MODEMLINES);
		modemflags_printdiff(modemflags_old,i,TIOCM_DTR,"DTR","dtr");
		modemflags_printdiff(modemflags_old,i,TIOCM_RTS,"RTS","rts");
		modemflags_printdiff(modemflags_old,i,TIOCM_CTS,"CTS","cts");
		modemflags_printdiff(modemflags_old,i,TIOCM_CAR,"DCD","dcd");
		modemflags_printdiff(modemflags_old,i,TIOCM_DSR,"DSR","dsr");
		modemflags_printdiff(modemflags_old,i,TIOCM_RNG,"RI","ri");
	}
	modemflags_old=i;
}

void get_opts_from_port(int tty)
{
	int cflag,iflag;
	int f;
	opts.baud=term_get_baudrate(tty);
	cflag=term_get_cflag(tty);
	iflag=term_get_iflag(tty);
	f=cflag&CSIZE;
	if(f==CS5)opts.databits=5;else
	if(f==CS6)opts.databits=6;else
	if(f==CS7)opts.databits=7;else
	if(f==CS8)opts.databits=8;
	if(!(cflag&PARENB)){opts.parity=P_NONE;opts.parity_str="none";}else {
			if(cflag&PARODD){opts.parity=P_EVEN;opts.parity_str="even";}
				else    {opts.parity=P_ODD;opts.parity_str="odd";}
	}
	opts.flow=FC_NONE;opts.flow_str="none";
	if(iflag&IXON){opts.flow=FC_XONXOFF;opts.flow_str="xon/xoff";}
	if(cflag&CRTSCTS){opts.flow=FC_RTSCTS;opts.flow_str="RTS/CTS";}
	
}


void
loop(void)
{
	enum {
		ST_COMMAND,
		ST_TRANSPARENT
	} state;
	int dtr_up;
	int rts_up;
	fd_set rdset, wrset;
	int newbaud, newflow, newparity, newbits;
	char *newflow_str, *newparity_str;
	char fname[128];
	int r, n;
	unsigned char c;
//	struct timeval tv = {0,100000}; // timeout 100 msec (in usec) for the read loop, to monitor modem lines
	struct timeval tv = {0,100000000}; // timeout 100 msec (in nsec) for the read loop, to monitor modem lines

	tty_q.len = 0;
	state = ST_TRANSPARENT;
	//dtr_up = 0;
	dtr_up = getdtr(tty_fd);
	rts_up = getrts(tty_fd);
	modemflags_old=term_get_modem_flags(tty_fd);

	for (;;) {
		FD_ZERO(&rdset);
		FD_ZERO(&wrset);
		FD_SET(STI, &rdset);
		FD_SET(tty_fd, &rdset);
		if ( tty_q.len ) FD_SET(tty_fd, &wrset);

//		if (select(tty_fd + 1, &rdset, &wrset, NULL, NULL) < 0)
		if (select(tty_fd + 1, &rdset, &wrset, NULL, &tv) < 0)
			fatal("select failed: %d : %s", errno, strerror(errno));
		if(opts.watchmodemlines)printmodemflagsdiff(tty_fd);

		if ( FD_ISSET(STI, &rdset) ) {

			/* read from terminal */

			do {
				n = read(STI, &c, 1);
//Shad's mod
//fd_printf(STO,"[%02x]",c);
			} while (n < 0 && errno == EINTR);
			if (n == 0) {
				fatal("stdin closed");
			} else if (n < 0) {
				/* is this really necessary? better safe than sory! */
				if ( errno != EAGAIN && errno != EWOULDBLOCK ) 
					fatal("read from stdin failed: %s", strerror(errno));
				else
					goto skip_proc_STI;
			}

			switch (state) {

			case ST_COMMAND:
				if ( c == opts.escape ) {
					state = ST_TRANSPARENT;
					/* pass the escape character down */
					if (tty_q.len + M_MAXMAP <= TTY_Q_SZ) {
						n = do_map((char *)tty_q.buff + tty_q.len, 
								   opts.omap, c);
						tty_q.len += n;
						if ( opts.lecho ) 
							map_and_write(STO, opts.emap, c);
					} else 
						fd_printf(STO, "\x07");
					break;
				}
				state = ST_TRANSPARENT;
				color=COL_SYS;
				switch (c) {
				case KEY_EXIT:
					return;
				case KEY_QUIT:
					term_set_hupcl(tty_fd, 0);
					term_flush(tty_fd);
					term_apply(tty_fd);
					term_erase(tty_fd);
					return;
				case KEY_STATUS:
					opts.baud=term_get_baudrate(tty_fd); // reread, to be sure
					get_opts_from_port(tty_fd); // reread, to be sure
					dtr_up=getdtr(tty_fd);
					printsetcolor(COL_SYS);
					fd_printf(STO, "\r\n");
					fd_printf(STO, "*** baud: %d\r\n", opts.baud);
					fd_printf(STO, "*** flow: %s\r\n", opts.flow_str);
					fd_printf(STO, "*** parity: %s\r\n", opts.parity_str);
					fd_printf(STO, "*** databits: %d\r\n", opts.databits);
					fd_printf(STO, "*** dtr: %s\r\n", dtr_up ? "up" : "down");
					print_modem_flags(tty_fd,0);
					break;
				case KEY_PULSE:
					//printsetcolor(COL_SYS);
					//fd_printf(STO, "\r\n*** pulse DTR ***\r\n");
					//if ( term_pulse_dtr(tty_fd) < 0 )
					//	fd_printf(STO, "*** FAILED\r\n");
					////print_modem_flags(tty_fd,1);
					dtr_up=getdtr(tty_fd);
					if ( dtr_up )r = term_lower_dtr(tty_fd);
						else r = term_raise_dtr(tty_fd);
					if ( r >= 0 ) dtr_up = ! dtr_up;
					printsetcolor(COL_SYS);
					if(opts.watchmodemlines)printmodemflagsdiff(tty_fd);
					sleep(1);
					if ( dtr_up )r = term_lower_dtr(tty_fd);
						else r = term_raise_dtr(tty_fd);
					if ( r >= 0 ) dtr_up = ! dtr_up;
					printsetcolor(COL_SYS);
					if(opts.watchmodemlines)printmodemflagsdiff(tty_fd);
					break;
				case KEY_PULSERTS:
					rts_up=getrts(tty_fd);
					if ( rts_up )r = term_lower_rts(tty_fd);
						else r = term_raise_rts(tty_fd);
					if ( r >= 0 ) rts_up = ! rts_up;
					printsetcolor(COL_SYS);
					if(opts.watchmodemlines)printmodemflagsdiff(tty_fd);
					sleep(1);
					if ( rts_up )r = term_lower_rts(tty_fd);
						else r = term_raise_rts(tty_fd);
					if ( r >= 0 ) rts_up = ! rts_up;
					printsetcolor(COL_SYS);
					if(opts.watchmodemlines)printmodemflagsdiff(tty_fd);
					break;
				case KEY_TOGGLE:
					dtr_up=getdtr(tty_fd);
					if ( dtr_up )
						r = term_lower_dtr(tty_fd);
					else
						r = term_raise_dtr(tty_fd);
					if ( r >= 0 ) dtr_up = ! dtr_up;
					printsetcolor(COL_SYS);
					if(opts.watchmodemlines)printmodemflagsdiff(tty_fd);
					else fd_printf(STO, "\r\n*** DTR: %s ***\r\n", 
							  dtr_up ? "up" : "down");
					//print_modem_flags(tty_fd,1);
					break;
				case KEY_RTS:
					rts_up=getrts(tty_fd);
					if ( rts_up )
						r = term_lower_rts(tty_fd);
					else
						r = term_raise_rts(tty_fd);
					if ( r >= 0 ) rts_up = ! rts_up;
					printsetcolor(COL_SYS);
					if(opts.watchmodemlines)printmodemflagsdiff(tty_fd);
					else fd_printf(STO, "\r\n*** RTS: %s ***\r\n", 
							  rts_up ? "up" : "down");
					//print_modem_flags(tty_fd,1);
					break;
				case KEY_BAUD_UP:
					newbaud = baud_up(opts.baud);
					term_set_baudrate(tty_fd, newbaud);
					tty_q.len = 0; term_flush(tty_fd);
					if ( term_apply(tty_fd) >= 0 ) opts.baud = newbaud;
					printsetcolor(COL_SYS);
					fd_printf(STO, "\r\n*** baud: %d ***\r\n", opts.baud);
					break;
				case KEY_BAUD_DN:
					newbaud = baud_down(opts.baud);
					term_set_baudrate(tty_fd, newbaud);
					tty_q.len = 0; term_flush(tty_fd);
					if ( term_apply(tty_fd) >= 0 ) opts.baud = newbaud;
					printsetcolor(COL_SYS);
					fd_printf(STO, "\r\n*** baud: %d ***\r\n", opts.baud);
					break;
				case KEY_FLOW:
					newflow = flow_next(opts.flow, &newflow_str);
					term_set_flowcntrl(tty_fd, newflow);
					tty_q.len = 0; term_flush(tty_fd);
					if ( term_apply(tty_fd) >= 0 ) {
						opts.flow = newflow;
						opts.flow_str = newflow_str;
					}
					printsetcolor(COL_SYS);
					fd_printf(STO, "\r\n*** flow: %s ***\r\n", opts.flow_str);
					break;
				case KEY_PARITY:
					newparity = parity_next(opts.parity, &newparity_str);
					term_set_parity(tty_fd, newparity);
					tty_q.len = 0; term_flush(tty_fd);
					if ( term_apply(tty_fd) >= 0 ) {
						opts.parity = newparity;
						opts.parity_str = newparity_str;
					}
					printsetcolor(COL_SYS);
					fd_printf(STO, "\r\n*** parity: %s ***\r\n", 
							  opts.parity_str);
					break;
				case KEY_BITS:
					newbits = bits_next(opts.databits);
					term_set_databits(tty_fd, newbits);
					tty_q.len = 0; term_flush(tty_fd);
					if ( term_apply(tty_fd) >= 0 ) opts.databits = newbits;
					get_opts_from_port(tty_fd); // read actual bits from the port
					printsetcolor(COL_SYS);
					fd_printf(STO, "\r\n*** databits: %d ***\r\n", 
							  opts.databits);
					break;
				case KEY_LECHO:
					opts.lecho = ! opts.lecho;
					printsetcolor(COL_SYS);
					fd_printf(STO, "\r\n*** local echo: %s ***\r\n", 
							  opts.lecho ? "yes" : "no");
					break;
				case KEY_SEND:
					fd_printf(STO, "\r\n*** file: ");
					r = fd_readline(STI, STO, fname, sizeof(fname));
					fd_printf(STO, "\r\n");
					if ( r < -1 && errno == EINTR ) break;
					if ( r <= -1 )
						fatal("cannot read filename: %s", strerror(errno));
					run_cmd(tty_fd, opts.send_cmd, fname, NULL);
					break;
				case KEY_RECEIVE:
					fd_printf(STO, "*** file: ");
					r = fd_readline(STI, STO, fname, sizeof(fname));
					fd_printf(STO, "\r\n");
					if ( r < -1 && errno == EINTR ) break;
					if ( r <= -1 )
						fatal("cannot read filename: %s", strerror(errno));
					if ( fname[0] )
						run_cmd(tty_fd, opts.receive_cmd, fname, NULL);
					else
						run_cmd(tty_fd, opts.receive_cmd, NULL);
					break;

				case KEY_TERMRESET:
					printsetcolor(COL_RESET);
					fd_printf(STO, "\r\n*** resetting terminal... ***\r\n");
					run_cmd(STO, opts.terminal_reset_cmd, NULL);
					printsetcolor(COL_SYS);
					fd_printf(STO, "*** terminal reset ***\r\n");
					break;
				case KEY_SHELL:
					printsetcolor(COL_SYS);
					//fd_printf(STO,"\r\n*** Command to send: ");
					//r=fd_readline(STI,STO,fname,sizeof(fname));
					fd_printf(STO, "\r\n*** running shell, return back via \"exit\" ***\r\n");
					printsetcolor(COL_RESET);
					run_cmd(STDERR_FILENO, getenv("SHELL"), NULL);
					//if(fname[0])run_cmd(STDERR_FILENO, fname, NULL);
					//if(fname[0])run_cmd(tty_fd, fname, NULL);
					//run_cmd(STO, "/bin/bash --debug --verbose -c \"exec /bin/bash\"", NULL);
					printsetcolor(COL_SYS);
					fd_printf(STO, "*** return from shell ***\r\n");
					break;
				case KEY_HEX:
					printsetcolor(COL_SYS);
					fd_printf(STO,"Hex to send: ");
					r=fd_readline(STI,STO,fname,sizeof(fname));
					fd_printf(STO, "\r\n");
					if ( r < -1 && errno == EINTR ) break;
					if ( r <= -1 )
						fatal("cannot read string: %s", strerror(errno));
					char*sp;
					unsigned char c;
					sp=fname;
					while(sp[0]) {
						if(!ishexdigit(sp[0])){sp++;continue;}
						c=gethex(sp);if(sp[0])sp++;if(sp[0])sp++;
						//printdebug('x',c,sp);
						if (tty_q.len + M_MAXMAP <= TTY_Q_SZ) {
							tty_q.buff[tty_q.len]=c;
							tty_q.len += 1;
							if ( opts.lecho ) 
								map_and_write_color(STO, opts.emap, c,COL_OUT);
						} else 
							fd_printf(STO, "\x07");
					}
					//printdebug('1','2',fname);
					break;
				case KEY_SETBAUD:
					printsetcolor(COL_SYS);
					fd_printf(STO,"Set baud rate: ");
					r=fd_readline(STI,STO,fname,sizeof(fname));
					fd_printf(STO, "\r\n");
					if ( r < -1 && errno == EINTR ) break;
					if ( r <= -1 )
						fatal("cannot read string: %s", strerror(errno));
					newbaud=atoi(fname);
					if(newbaud<=0){
						fd_printf(STO,"Invalid baudrate: %i\r\n",newbaud);
					} else {
						term_set_baudrate(tty_fd, newbaud);
						tty_q.len = 0; term_flush(tty_fd);
						if ( term_apply(tty_fd) >= 0 ) opts.baud = term_get_baudrate(tty_fd);
						fd_printf(STO, "\r\n*** baud: %d ***\r\n", opts.baud);
						baud_add(opts.baud);
					}
					break;
				case KEY_HELP:
					printsetcolor(COL_SYS);
					fd_printf(STO,"\r\n");

					printhelp_section("Port setting");
					printhelp("Ctrl-B","cycle through databits (5,6,7,8)");
					printhelp("Ctrl-F","cycle through flowcontrol modes");
					printhelp("Ctrl-D","baudrate decrease");
					printhelp("Ctrl-G","set arbitrary baudrate");
					printhelp("Ctrl-U","baudrate increase");
					printhelp("Ctrl-Y","cycle through parity modes");

					printhelp_section("Control signals, data send/receive");
					printhelp("Ctrl-I","pulse RTS");
					printhelp("Ctrl-R","toggle RTS");
					printhelp("Ctrl-P","pulse DTR");
					printhelp("Ctrl-T","toggle DTR");
					printhelp("Ctrl-H","enter hex string to send as binary");
					printhelp("Ctrl-E","receive file");
					printhelp("Ctrl-S","send file");
					printhelp("Ctrl-\\","send break");

					printhelp_section("Terminal");
					printhelp("Ctrl-C","toggle local echo");
					printhelp("Ctrl-O","reset terminal");

					printhelp_section("Other");
					printhelp("Ctrl-Q","quit without reseting port");
					printhelp("Ctrl-X","exit");
					printhelp("Ctrl-Z","run shell");
					printhelp("Ctrl-V","show current options");
					printhelp("?","this help");
					break;

				case KEY_BREAK:
					term_break(tty_fd);
					printsetcolor(COL_SYS);
					fd_printf(STO, "\r\n*** break sent ***\r\n");
					break;
				default:
					break;
				}
				break;

			case ST_TRANSPARENT:
				if ( c == opts.escape ) {
					state = ST_COMMAND;
				} else {
					if (tty_q.len + M_MAXMAP <= TTY_Q_SZ) {
						n = do_map((char *)tty_q.buff + tty_q.len, 
								   opts.omap, c);
						tty_q.len += n;
						if ( opts.lecho ) 
							map_and_write_color(STO, opts.emap, c,COL_OUT);
					} else 
						fd_printf(STO, "\x07");
				}
				break;

			default:
				assert(0);
				break;
			}
		}
	skip_proc_STI:

		if ( FD_ISSET(tty_fd, &rdset) ) {

			/* read from port */

			do {
				n = read(tty_fd, &c, 1);
			} while (n < 0 && errno == EINTR);
			if (n == 0) {
				fatal("term closed");
			} else if ( n < 0 ) {
				if ( errno != EAGAIN && errno != EWOULDBLOCK )
					fatal("read from term failed: %s", strerror(errno));
			} else {
				map_and_write_color(STO, opts.imap, c,COL_IN);
			}
		}

		if ( FD_ISSET(tty_fd, &wrset) ) {

			/* write to port */

			do {
				n = write(tty_fd, tty_q.buff, tty_q.len);
			} while ( n < 0 && errno == EINTR );
			if ( n <= 0 )
				fatal("write to term failed: %s", strerror(errno));
			memcpy(tty_q.buff, tty_q.buff + n, tty_q.len - n);
			tty_q.len -= n;
		}
	}
}

/**********************************************************************/

void
deadly_handler(int signum)
{
	kill(0, SIGTERM);
	sleep(1);
#ifdef UUCP_LOCK_DIR
	uucp_unlock();
#endif
	exit(EXIT_FAILURE);
}

void
establish_signal_handlers (void)
{
        struct sigaction exit_action, ign_action;

        /* Set up the structure to specify the exit action. */
        exit_action.sa_handler = deadly_handler;
        sigemptyset (&exit_action.sa_mask);
        exit_action.sa_flags = 0;

        /* Set up the structure to specify the ignore action. */
        ign_action.sa_handler = SIG_IGN;
        sigemptyset (&ign_action.sa_mask);
        ign_action.sa_flags = 0;

        sigaction (SIGTERM, &exit_action, NULL);

        sigaction (SIGINT, &ign_action, NULL); 
        sigaction (SIGHUP, &ign_action, NULL);
        sigaction (SIGALRM, &ign_action, NULL);
        sigaction (SIGUSR1, &ign_action, NULL);
        sigaction (SIGUSR2, &ign_action, NULL);
        sigaction (SIGPIPE, &ign_action, NULL);
}

/**********************************************************************/

void
show_usage(char *name)
{
	char *s;

	s = strrchr(name, '/');
	s = s ? s+1 : name;

	printf("picocom v" VERSION_STR_SHAD "\n");
	printf("Usage is: %s [options] <tty device>\n", s);
	printf("Options are:\n");
	printf("  --<b>aud <baudrate>\n");
	printf("  --<f>low s (=soft) | h (=hard) | n (=none)\n");
	printf("  --<p>arity o (=odd) | e (=even) | n (=none)\n");
	printf("  --<d>atabits 5 | 6 | 7 | 8\n");
	printf("  --<e>scape <char>\n");
	printf("  --e<c>ho\n");
	printf("  --no<i>nit\n");
	printf("  --no<r>eset\n");
	printf("  --no<l>ock\n");
	printf("  --<s>end-cmd <command>\n");
	printf("  --recei<v>e-cmd <command>\n");
	printf("  --nohex, --hexall\n");
	printf("  --color (-C), --colorbright (-B)\n");
	printf("  --dtr <1|0>\n");
	printf("  --rts <1|0>\n");
	printf("  --nomodem\n");
	printf("  --baudadd <list_of_baudrates|set_name>\n");
	printf("  --baudlist <list_of_baudrates|set_name>\n");
	printf("  --imap <map> (input mappings)\n");
	printf("  --omap <map> (output mappings)\n");
	printf("  --emap <map> (local-echo mappings)\n");
	printf("  --<h>elp\n");
	printf("<map> is a comma-separated list of one or more of:\n");
	printf("  crlf : map CR --> LF\n");
	printf("  crcrlf : map CR --> CR + LF\n");
	printf("  igncr : ignore CR\n");
	printf("  lfcr : map LF --> CR\n");
	printf("  lfcrlf : map LF --> CR + LF\n");
	printf("  ignlf : ignore LF\n");
	printf("  bsdel : map BS --> DEL\n");
	printf("  delbs : map DEL --> BS\n");
	printf("<?> indicates the equivalent short option.\n");
	printf("Short options are prefixed by \"-\" instead of by \"--\".\n");
}

/**********************************************************************/

void
parse_args(int argc, char *argv[])
{
	static struct option longOptions[] =
	{
		{"receive-cmd", required_argument, 0, 'v'},
		{"send-cmd", required_argument, 0, 's'},
        {"imap", required_argument, 0, 'I' },
        {"omap", required_argument, 0, 'O' },
        {"emap", required_argument, 0, 'E' },
		{"escape", required_argument, 0, 'e'},
		{"echo", no_argument, 0, 'c'},
		{"noinit", no_argument, 0, 'i'},
		{"noreset", no_argument, 0, 'r'},
		{"nolock", no_argument, 0, 'l'},
		{"flow", required_argument, 0, 'f'},
		{"baud", required_argument, 0, 'b'},
		{"parity", required_argument, 0, 'p'},
		{"databits", required_argument, 0, 'd'},
		{"nohex", no_argument, 0, 'N'},
		{"hexall", no_argument, 0, 'H'},
		{"color", no_argument, 0, 'C'},
		{"colorbright", no_argument, 0, 'B'},
		{"dtr", required_argument, 0, 'D'},
		{"rts", required_argument, 0, 'R'},
		{"nomodem", no_argument, 0, 'M'},
		{"baudadd", required_argument, 0, 'K'},
		{"baudlist", required_argument, 0, 'L'},
		{"help", no_argument, 0, 'h'},
		{0, 0, 0, 0}
	};

	baud_addstr("DEFAULT",1); // populate baud rates array for modifications
	while (1) {
		int optionIndex = 0;
		int c;
		int map;

		/* no default error messages printed. */
		opterr = 0;

		c = getopt_long(argc, argv, "hirlcCBNHv:s:r:e:f:b:p:d:D:R:",
						longOptions, &optionIndex);

		if (c < 0)
			break;

		switch (c) {
		case 's':
			strncpy(opts.send_cmd, optarg, sizeof(opts.send_cmd));
			opts.send_cmd[sizeof(opts.send_cmd) - 1] = '\0';
			break;
		case 'v':
			strncpy(opts.receive_cmd, optarg, sizeof(opts.receive_cmd));
			opts.receive_cmd[sizeof(opts.receive_cmd) - 1] = '\0';
			break;
		case 'N':
			opts.hexmap=HEX_NONE;
			break;
		case 'M':
			opts.watchmodemlines=0;
			break;
		case 'H':
			opts.hexmap=HEX_ALL;
			break;
		case 'C':
			opts.color=COLOR_NORMAL;
			break;
		case 'B':
			opts.color=COLOR_BRIGHT;
			break;
		case 'D':
			if(atoi(optarg))opts.initdtr=1;
				else opts.initdtr=0;
			break;
		case 'K':
			baud_addstr(optarg,0);
			break;
		case 'L':
			baud_addstr(optarg,1);
			break;
		case 'R':
			if(atoi(optarg))opts.initrts=1;
				else opts.initrts=0;
			break;
		case 'I':
			map = parse_map(optarg);
			if (map >= 0) opts.imap = map;
			else fprintf(stderr, "Invalid imap, ignored\n");
			break;
		case 'O':
			map = parse_map(optarg);
			if (map >= 0) opts.omap = map;
			else fprintf(stderr, "Invalid omap, ignored\n");
			break;
		case 'E':
			map = parse_map(optarg);
			if (map >= 0) opts.emap = map;
			else fprintf(stderr, "Invalid emap, ignored\n");
			break;
		case 'c':
			opts.lecho = 1;
			break;
		case 'i':
			opts.noinit = 1;
			break;
		case 'r':
			opts.noreset = 1;
			break;
		case 'l':
#ifdef UUCP_LOCK_DIR
			opts.nolock = 1;
#endif
			break;
		case 'e':
			opts.escape = optarg[0] & 0x1f;
			break;
		case 'f':
			switch (optarg[0]) {
			case 'X':
			case 'x':
				opts.flow_str = "xon/xoff";
				opts.flow = FC_XONXOFF;
				break;
			case 'H':
			case 'h':
				opts.flow_str = "RTS/CTS";
				opts.flow = FC_RTSCTS;
				break;
			case 'N':
			case 'n':
				opts.flow_str = "none";
				opts.flow = FC_NONE;
				break;
			default:
				fprintf(stderr, "--flow '%c' ignored.\n", optarg[0]);
				fprintf(stderr, "--flow can be one off: 'x', 'h', or 'n'\n");
				break;
			}
			break;
		case 'b':
			opts.baud = atoi(optarg);
			break;
		case 'p':
			switch (optarg[0]) {
			case 'e':
				opts.parity_str = "even";
				opts.parity = P_EVEN;
				break;
			case 'o':
				opts.parity_str = "odd";
				opts.parity = P_ODD;
				break;
			case 'n':
				opts.parity_str = "none";
				opts.parity = P_NONE;
				break;
			default:
				fprintf(stderr, "--parity '%c' ignored.\n", optarg[0]);
				fprintf(stderr, "--parity can be one off: 'o', 'e', or 'n'\n");
				break;
			}
			break;
		case 'd':
			switch (optarg[0]) {
			case '5':
				opts.databits = 5;
				break;
			case '6':
				opts.databits = 6;
				break;
			case '7':
				opts.databits = 7;
				break;
			case '8':
				opts.databits = 8;
				break;
			default:
				fprintf(stderr, "--databits '%c' ignored.\n", optarg[0]);
				fprintf(stderr, "--databits can be one of: 5, 6, 7 or 8\n");
				break;
			}
			break;
		case 'h':
			show_usage(argv[0]);
			exit(EXIT_SUCCESS);
		case '?':
		default:
			fprintf(stderr, "Unrecognized option.\n");
			fprintf(stderr, "Run with '--help'.\n");
			exit(EXIT_FAILURE);
		}
	} /* while */

	if ( (argc - optind) < 1) {
		fprintf(stderr, "No port given\n");
		exit(EXIT_FAILURE);
	}
	strncpy(opts.port, argv[optind], sizeof(opts.port) - 1);
	opts.port[sizeof(opts.port) - 1] = '\0';
	if(baudrates[0]==0)baud_addstr("DEFAULT",1);
}

void
show_settings(int tty)
{
	fd_printf(STO,"picocom v%s\n", VERSION_STR_SHAD);
	fd_printf(STO,"\r\n");
	fd_printf(STO,"port is        : %s\r\n", opts.port);
	fd_printf(STO,"flowcontrol    : %s\r\n", opts.flow_str);
	fd_printf(STO,"baudrate is    : %d\r\n", opts.baud);
	fd_printf(STO,"parity is      : %s\r\n", opts.parity_str);
	fd_printf(STO,"databits are   : %d\r\n", opts.databits);
	fd_printf(STO,"escape is      : C-%c\r\n", 'a' + opts.escape - 1);
	fd_printf(STO,"local echo is  : %s\r\n", opts.lecho ? "yes" : "no");
	fd_printf(STO,"noinit is      : %s\r\n", opts.noinit ? "yes" : "no");
	fd_printf(STO,"noreset is     : %s\r\n", opts.noreset ? "yes" : "no");
#ifdef UUCP_LOCK_DIR
	fd_printf(STO,"nolock is      : %s\r\n", opts.nolock ? "yes" : "no");
#endif
	fd_printf(STO,"send_cmd is    : %s\r\n", opts.send_cmd);
	fd_printf(STO,"receive_cmd is : %s\r\n", opts.receive_cmd);
	fd_printf(STO,"imap is        : "); print_map(opts.imap);
	fd_printf(STO,"omap is        : "); print_map(opts.omap);
	fd_printf(STO,"emap is        : "); print_map(opts.emap);
	fd_printf(STO,"modem flags are: "); print_modem_flags_raw(tty);
	fd_printf(STO,"\r\n");
	fd_printf(STO,"\r\n");
}

/**********************************************************************/


int
main(int argc, char *argv[])
{
	int r;

	parse_args(argc, argv);

	establish_signal_handlers();

	r = term_lib_init();
	if ( r < 0 )
		fatal("term_init failed: %s", term_strerror(term_errno, errno));

#ifdef UUCP_LOCK_DIR
	if ( ! opts.nolock ) uucp_lockname(UUCP_LOCK_DIR, opts.port);
	if ( uucp_lock() < 0 )
		fatal("cannot lock %s: %s", opts.port, strerror(errno));
#endif

	tty_fd = open(opts.port, O_RDWR | O_NONBLOCK | O_NOCTTY);
	if (tty_fd < 0)
		fatal("cannot open %s: %s", opts.port, strerror(errno));

	if ( opts.noinit ) {
		r = term_add(tty_fd);
		//opts.baud=term_get_baudrate(tty_fd);
		get_opts_from_port(tty_fd);
	} else {
		r = term_set(tty_fd,
					 1,              /* raw mode. */
					 opts.baud,      /* baud rate. */
					 opts.parity,    /* parity. */
					 opts.databits,  /* data bits. */
					 opts.flow,      /* flow control. */
					 1,              /* local or modem */
					 !opts.noreset); /* hup-on-close. */
	}
	if ( r < 0 )
		fatal("failed to add device %s: %s", 
			  opts.port, term_strerror(term_errno, errno));
	r = term_apply(tty_fd);
	if ( r < 0 )
		fatal("failed to config device %s: %s", 
			  opts.port, term_strerror(term_errno, errno));
	
	r = term_add(STI);
	if ( r < 0 )
		fatal("failed to add I/O device: %s", 
			  term_strerror(term_errno, errno));
	term_set_raw(STI);
	r = term_apply(STI);
	if ( r < 0 )
		fatal("failed to set I/O device to raw mode: %s",
			  term_strerror(term_errno, errno));

	//get_opts_from_port(tty_fd); // make sure we have real values from the port
	color=COL_SYS;
	//if value=-1 then do not set
	if(opts.initdtr==0)term_lower_dtr(tty_fd);else
	if(opts.initdtr==1)term_raise_dtr(tty_fd);
	if(opts.initrts==0)term_lower_rts(tty_fd);else
	if(opts.initrts==1)term_raise_rts(tty_fd);
	baud_add(opts.baud); // if port runs on nonstandard baud rate, set it into the up/down array
	show_settings(tty_fd);
	//baud_dump();
	fd_printf(STO, "Terminal ready\r\n");
	loop();

	color=COL_SYS;
	fd_printf(STO, "\r\n");
	if ( opts.noreset ) {
		fd_printf(STO, "Skipping tty reset...\r\n");
		term_erase(tty_fd);
	}

	fd_printf(STO, "Thanks for using picocom\r\n");
	/* wait a bit for output to drain */
	sleep(1);

#ifdef UUCP_LOCK_DIR
	uucp_unlock();
#endif

	printsetcolor(COL_RESET);
	return EXIT_SUCCESS;
}

/**********************************************************************/

/*
 * Local Variables:
 * mode:c
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
