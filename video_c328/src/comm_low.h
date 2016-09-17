#ifndef _COMM_LOW_H
#define _COMM_LOW_H

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>

typedef union {
	unsigned short s;
	unsigned char c[2];
} conv;

int initport(char *device, int speed, int cflag);
int recvbyte(int port, unsigned char *data, long timeout);
int sendbyte(int port, unsigned char *data);
int recvbytes(int port, unsigned char *data, int len, long timeout);
int sendbytes(int port, unsigned char *data, int len);
int recvline(int port, unsigned char *data, long timeout);
int sendline(int port, unsigned char *data, int len);

#endif /* _COMM_LOW_H */
