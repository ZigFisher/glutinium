/* 
 * serialoverip.c - Serial port over UDP/IP socket symetric gateway
 * 
 * Copyright (C) 2011, Gateworks Corp
 *                     Tim Harvey <tharvey@gateworks.com>
 *
 * Notes:
 *   see man 3 termios for terminal io settings implemented in setserial
 *   I can disable echo input and CRLF conversion if necessary or provide
 *   a cmdline parameter
 *
 * TODO: 
 *   save/restore serial port settings - see xt.c
 *
 */
#include <getopt.h>
#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/wait.h>

#define VERSION  "1.00"
#define BUFLEN   2048
#define FORMAT   "8N1"              // default serial port format
#define BAUD     115200             // default baudrate
#define GROUP_ADDRESS "225.0.0.1"   // default mcast address
#define GROUP_PORT    5555          // default mcast port

/* globals
 */ 
char done = 0;
int verbose = 0;
struct sockaddr_in s_addr;
struct sockaddr_in r_addr;
struct sockaddr_in iface_addr;
int socket_fd = -1;
char read_only = 0;				// read-only (monitor mode)
int parent_pid = -1;

/* quit - clean up and exit
 */
void quit(int code)
{
	done = 1; // flag loops to exit
	exit(code);
}


/* banner - show copyright message, exit
 */
void banner(const char* app)
{
	printf("%s %s\n", app, VERSION);
	printf("Copyright (C) 2011 Gateworks Corp.\n");
	quit(0);
}


/* usage - show usage, exit
 */
void usage(char *app)
{
	printf("Usage: %s [options]\n", app);
	printf("\n");
	printf("Serial to Network (UDP/IP) client/server\n\n");
	printf("  -d, --device=DEV[:FMT]      serial device and format (default stdin)\n");
	printf("                              (ie '/dev/ttyS0:8N1')\n");
	printf("  -b, --baud=baudrate         serial port baudrate\n");
	printf("  -a, --address=ADDR[:PORT]]  mcast send/recv address/port\n");
	printf("                              (ie '224.0.0.1:5555')\n");
	printf("  -i, --iface=ADDR            interface for send/recv\n");
	printf("                              (ie '192.168.1.1')\n");
	printf("  -t, --ttl=TTL               multicast packet TTL\n");
	printf("  --flowcontrol=F             set serial port flow control\n");
	printf("                              ('H' for hardware or 'S' for software, 'N' for none)\n");
	printf("  -r                          read-only mode (does not send data to network\n");
	printf("\n");
	printf("  --version                   display version info\n");
	printf("  -v|--verbose                increase verbosity level\n");
	printf("  -?|--help                   This help\n");
	quit(1);
}


/* error - show error message
 */
void error(char *fmt, ...)
{
	va_list ap;
	
	va_start(ap, fmt);
	fprintf(stderr, "serialoverip: error - ");
	vfprintf(stderr, fmt, ap);
	if (errno)
		fprintf(stderr, ": %s\n", strerror(errno));
	else
		fprintf(stderr, "\n");
	va_end(ap);
}


/* signal_handler - handle caught signal
 * @param x - signal received
 */
void signal_handler(int x) {
	if (verbose)
		printf("%d: Got int: %d\n", getpid(), x);

	if (getpid() == parent_pid) {
		// drop multicast membership
		if (IN_MULTICAST(htonl(s_addr.sin_addr.s_addr))) {
			struct ip_mreq mreq;
			mreq.imr_multiaddr.s_addr = s_addr.sin_addr.s_addr;
			mreq.imr_interface.s_addr = iface_addr.sin_addr.s_addr;
			if ( setsockopt(socket_fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, 
					sizeof(mreq)) < 0) {
			} else {
				if (verbose)
					printf("Dropped multicast IP membership for %s\n", 
						inet_ntoa(s_addr.sin_addr));
			}
		}

		// kill process group
		kill(0, SIGTERM);
	}
	quit(1);
}


/* setserial - configure serial port
 * @param s - device fd
 * @param cfg - termio struct to configure
 * @param baud - baudrate
 * @param fmt - data format (ie 8N1)
 * @param flow - flow control (h=hardware (CTS/RTS),s=software (XON/XOFF,n=none)
 * @return 0 on success error otherwise
 * @see termios(3)
 */
int setserial(int s, struct termios* cfg, int baud, char* fmt, char flow) 
{
	char parity = fmt[1]|32;
	int data = fmt[0] - '0';
	int stop_b = fmt[2] - '0';
	speed_t speed;

	/* default to 'raw' mode (see man 3 termios for details) */
	cfmakeraw(cfg);
	switch (baud) {
		case      0:  speed =      B0;  break;
		case     50:  speed =     B50;  break;
		case     75:  speed =     B75;  break;
		case    110:  speed =    B110;  break;
		case    134:  speed =    B134;  break;
		case    150:  speed =    B150;  break;
		case    200:  speed =    B200;  break;
		case    300:  speed =    B300;  break;
		case    600:  speed =    B600;  break;
		case   1200:  speed =   B1200;  break;
		case   1800:  speed =   B1800;  break;
		case   2400:  speed =   B2400;  break;
		case   4800:  speed =   B4800;  break;
		case   9600:  speed =   B9600;  break;
		case  19200:  speed =  B19200;  break;
		case  38400:  speed =  B38400;  break;
		case  57600:  speed =  B57600;  break;
		case 115200:  speed = B115200;  break;
//		case 230400:  speed = B230400;  break;
		default:
			error("unknown baud rate");
			quit(-1);
	}
	cfsetspeed(cfg, speed);
	switch(parity) {
		case 'n': cfg->c_cflag &= ~PARENB; break;	
		case 'e': cfg->c_cflag |= PARENB; cfg->c_cflag &= ~PARODD; break;	
		case 'o': cfg->c_cflag |= PARENB; cfg->c_cflag |= PARODD; break;	
		default:
			error("invalid parity");
			quit(-1);
			break;
	}
	switch(data) {
		case 5 : { cfg->c_cflag |= CS5; break; }
		case 6 : { cfg->c_cflag |= CS6; break; }
		case 7 : { cfg->c_cflag |= CS7; break; }
		case 8 : { cfg->c_cflag |= CS8; break; }
		default:
			error("invalid data bit size");
			quit(-1);
			break;
   }
	switch(stop_b) {
		case 1 : { cfg->c_cflag&=~CSTOPB; break; }
		case 2 : { cfg->c_cflag|=CSTOPB; break; }
		default:
			error("invalid stop bit size");
			quit(-1);
			break;
   }
	switch(flow|32) {
		case 's':
			if (verbose)
				printf("setting software flow control\n");
			cfg->c_iflag |= IXON;	// enable XON/XOFF flow control on output
			cfg->c_iflag |= IXOFF;	// enable XON/XOFF flow control on input 
			break;
		case 'h':
			if (verbose)
				printf("setting hardware flow control\n");
			cfg->c_cflag |= CRTSCTS;	// enable RTS/CTS (hardware) flow control
			break;
		case 'n':
			break;
		default:
			error("invalid flow control");
			quit(-1);
			break;
	}
	/* extra settings */
	cfg->c_oflag = 0;
	cfg->c_iflag = 0;
	cfg->c_cflag |= CLOCAL;	// ignore modem control lines
	cfg->c_cflag |= CREAD;	// enable receiver
//	cfg->c_oflag |= OCRNL;	// map CR to NL on output
//	cfg->c_iflag |= ICRNL;	// translate CR to NL on input
//	cfg->c_lflag |= ECHO;	// Echo input characters
	if (verbose > 2) {
		printf("oflag=%x\n", cfg->c_oflag);
		printf("iflag=%x\n", cfg->c_iflag);
		printf("lflag=%x\n", cfg->c_lflag);
	}
	tcsetattr(s, TCSAFLUSH, cfg);
	fcntl(s, F_SETFL, fcntl(s, F_GETFL, 0) & ~O_NONBLOCK);
	return tcsetattr(s,TCSANOW,cfg);
}


/* socket_to_serial - read socket and write to serial
 * @param serial_fd - output fd
 * @param socket_fd - input fd
 *
 * Loops forever until global done is set
 */
void socket_to_serial(int serial_fd, int socket_fd) 
{
	char message[BUFLEN];

	if (verbose)
		printf("%d: %s\n", getpid(), __func__); 

	while (!done) {
		int rs;
		struct sockaddr_in from;
		socklen_t from_len = sizeof(from);
		if ((rs = recvfrom(socket_fd, &message, sizeof(message)-1, 0,
				(struct sockaddr*) &from, &from_len)) < 0) {
		}
		message[rs] = 0; // null terminate
		// skip packets from self
		if (from.sin_addr.s_addr == iface_addr.sin_addr.s_addr) 
			continue;
		if (rs > 0) {
			if (write(serial_fd, &message, rs) < 0) {
				error("device write error");
				return;
			}
			if (verbose > 1) {
				printf("socket: %d %s:%d '%s'\n", rs, inet_ntoa(from.sin_addr), ntohs(from.sin_port), message);
			}
		}
	}

}


/* serial_to_socket - read serial and write to socket
 * @param serial_fd - input fd
 * @param socket_fd - output fd
 *
 * Loops forever until global done is set
 */
void serial_to_socket(int serial_fd, int socket_fd)
{
	char message[BUFLEN];

	if (verbose)
		printf("%d: %s\n", getpid(), __func__); 

	while (!done) {
		int rs;
		if ((rs = read(serial_fd, &message, sizeof(message)-1)) < 0) {
			error("device read error");
			return;
		}
		if (rs > 0) {
			message[rs] = 0;
			if (sendto(socket_fd, message, rs, 0, 
					(struct sockaddr *) &s_addr, sizeof(s_addr)) < 0) {
				error("sendto");
				return;
			}
			if (verbose > 1) {
				printf("serial: %d: '%s'\n", rs, message);
			}
		}
	}
}


/* main
 */
int main (int argc, char* argv[] )
{
	char *device = NULL;
	char *address = GROUP_ADDRESS;
	char *iface = NULL;	
	long baud = BAUD;
	short port = GROUP_PORT;
	char flow = 'n';
	long mcast_ttl = 1;
	char *p;
	int c;

	// parse command line
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			//  name, has_arg, flag, val
			{"verbose",	0, &verbose, 1},
			{"iface", 1, 0, 'i'},
			{"address", 1, 0, 'a'},
			{"baud", 1, 0, 'b'},
			{"ttl", 1, 0, 't'},
			{"device", 1, 0, 'd'},
			{"help",	0, 0, '?'},
			{"version",	0, 0, 0},
			{"flowcontrol", 1, 0, 'f'},
			{0, 0, 0, 0}
		};

		c = getopt_long (argc, argv, "d:b:a:i:t:srv", long_options,&option_index);
		if (c == -1)
			break;
		switch (c) {
		case 0:
			if (!strcmp(long_options[option_index].name, "version"))
				banner(argv[0]);
			else
				usage(argv[0]);
			break;

		case 'd':	// set serial device:format
			device = optarg;
			break;
		case 'a':	// set multicast address:port
			address = optarg;
			break;
		case 'b':	// set baud rate
			baud = strtol(optarg, 0, 10);
			break;	
		case 'f':	// set flow control
			flow = optarg[0];
			break;
		case 'i':	// iface
			iface = optarg;
			break;
		case 'r':	// read-only (monitor mode)
			read_only = 1;
			break;
		case 't':	// mcast ttl
			mcast_ttl = atoi(optarg);
			break;
		case 'v':	// verbosity
			verbose++;
			break;
		case '?':	// unknown argument
			usage(argv[0]);
		}        
	}

	if (!address)
		usage(argv[0]);

	// parse device, format
	char *dev = NULL, *fmt, *addr;
	char buf[64];
	if (device && !read_only) {
		sprintf(buf, "%s", device);
		p = strchr(buf, ':');
		if (p) {
			*p++ = 0;
			dev = strdup(buf);
			fmt = strdup(p);
		} else {
			dev = device;
			fmt = FORMAT;
		}
	}

	// address, port
	sprintf(buf, "%s", address);
	p = strchr(buf, ':');
	if (p) {
		*p++ = 0;
		addr = strdup(buf);
		port = atoi(p);
	} else 
		addr = address;

	// iface
	iface_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (iface) {
		struct in_addr inaddr;
		if (inet_aton(iface, &inaddr) < 0) {
			error("invalid iface address");
			quit(-1);
		}
		iface_addr.sin_addr = inaddr;	
	}

	// open serial device, set to transparent mode
	int serial_fd = STDIN_FILENO;
	if (dev) {
		if ( (serial_fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0) {
			error("could not open device");
			quit(-1);
		}
		struct termios cfg;
		memset(&cfg, 0, sizeof(cfg));
		if (setserial(serial_fd, &cfg, baud, fmt, flow) < 0) {
			error("could not initialize device");
			quit(-1);
		}
	}
  
	// create UDP socket
	if ( (socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		error("can't open datagram socket");
		quit(-1);
	}

	// set up destination address
	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = inet_addr(addr);
	s_addr.sin_port = htons(port);

	// bind to receive address
	memset(&r_addr, 0, sizeof(r_addr));
	r_addr.sin_family = AF_INET;
	r_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	r_addr.sin_port = htons(port);
	if ( bind(socket_fd, (struct sockaddr*) &r_addr, sizeof(r_addr)) < 0) {
		error("can't bind local address");
		quit(-1);
	}

	// request to join multicast group
	if (IN_MULTICAST(htonl(s_addr.sin_addr.s_addr))) {
		struct ip_mreq mreq;
		mreq.imr_multiaddr.s_addr = s_addr.sin_addr.s_addr;
		mreq.imr_interface.s_addr = iface_addr.sin_addr.s_addr;
		if ( setsockopt(socket_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, 
				sizeof(mreq)) < 0) {
			error("can't add IP multicast membership");
			quit(-1);
		} else {
			if (verbose)
				printf("Added multicast IP membership for %s\n", inet_ntoa(s_addr.sin_addr));
		}

		// set mcast ttl
		if ( setsockopt(socket_fd, IPPROTO_IP, IP_MULTICAST_TTL, &mcast_ttl, 
				sizeof(mcast_ttl)) < 0) {
			error("can't set multicast TTL");
			quit(-1);
		}

		// disable multicast loop
		u_char loop = 0;
		if (setsockopt(socket_fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)) < 0)
			perror("setsockopt IP_MULTICAST_LOOP");
	}

	// display info
//	if (verbose)
	{
		if (IN_MULTICAST(htonl(s_addr.sin_addr.s_addr))) {
			printf("Connected to multicast: %s:%d", inet_ntoa(s_addr.sin_addr), 
				ntohs(s_addr.sin_port));
		} else {
			printf("Connected to: %s:%d", inet_ntoa(r_addr.sin_addr), ntohs(r_addr.sin_port));
		}
		if (iface)
			printf(" (iface:%s)\n", iface);
		else
			printf("\n");
		if (dev)
			printf("Connected to to %s:%s at %ld bps flow=%c\n", dev, fmt, baud, flow);
		else
			printf("Connected to stdin\n");
		if (read_only)
			printf("read-only mode\n");
	}

	done = 0;
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	parent_pid = getpid();
	switch (read_only?0:fork())
	{ 
		// error in fork()
		case -1: {
			error("fork");
			quit(-1);
		}

		// child process - read socket and write to serial
		case 0: 
			socket_to_serial(serial_fd, socket_fd);
			// shut down after sending SIGCHLD to parent
			exit(0);

		// parent process -- read serial port and write to socket
		default: {
			// prepare handler for child's termination
			struct sigaction sa;
			sa.sa_handler = signal_handler;
			sa.sa_flags = 0;
			sigaction(SIGCHLD, &sa, NULL);

			serial_to_socket(serial_fd, socket_fd);
			// wait for child to die, then exit with success
			wait(NULL);
			quit(0);
		}
	}
	return 0;
}

