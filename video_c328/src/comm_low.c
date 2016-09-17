/*
	03.06.27 17:07 set parity bit to 0 in initport
*/

#include "comm_low.h"

int initport(char *device, int speed, int cflag)
{
	struct termios tio;
	int port;
	printf("open %s\n", device);
	if((port = open(device, O_RDWR, O_NOCTTY, O_SYNC)) < 0) {
		perror(device);
		return -1;
	}
	tcgetattr(port, &tio);
	cfmakeraw(&tio);
	tio.c_cflag = CS8 | CLOCAL | CREAD;
	tio.c_iflag = IGNPAR;
	tio.c_oflag = 0;
	tio.c_lflag = 0;
	cfsetispeed(&tio, speed);
	cfsetospeed(&tio, speed);
	tcflush(port, TCIOFLUSH);
	tcsetattr(port, TCSANOW, &tio);
	return port;
}

int recvbyte(int port, unsigned char *data, long timeout)
{
	int result;
	fd_set input;
	struct timeval tout;

	tout.tv_sec = 0;
	tout.tv_usec = timeout;
	FD_ZERO(&input);
	FD_SET(port, &input);
//	printf("to=%ld\n", tout.tv_usec);
	if((result = select(port+1, &input, NULL, NULL, &tout)) <= 0) {
//		printf("timeout or select failed\n");
		return -1;
	}
	return read(port, data, 1);
}

int sendbyte(int port, unsigned char *data)
{

//	tcflush(port, TCIOFLUSH);
	write(port, data, 1);
//	printf("send line: %s\n", data);
	return 0;
}

int recvbytes(int port, unsigned char *data, int len, long timeout)
{
	int result;
	fd_set input;
	struct timeval tout;

	tout.tv_sec = 0;
	tout.tv_usec = timeout;
	FD_ZERO(&input);
	FD_SET(port, &input);
//	printf("to=%ld\n", tout.tv_usec);
	if((result = select(port+1, &input, NULL, NULL, &tout)) <= 0) {
//		printf("timeout or select failed\n");
		return -1;
	}
	return read(port, data, len);
}

int sendbytes(int port, unsigned char *data, int len)
{

//	tcflush(port, TCIOFLUSH);
	write(port, data, len);
//	printf("send line: %s\n", data);
	return 0;
}

int recvline(int port, unsigned char *data, long timeout)
{
	int len = 0;
	do {
		if((recvbyte(port, data+len, timeout)) < 0) {
			data[len] = 0;
//			printf("RECVLINE timeout at position: %d\n", len);
//			printf("recv line: %s[%d]\n", data, len);
			return -1;
		}
//		printf("%02x ", data[len]);
		if(data[len] == '\r')
			continue;
		len++;
	} while (data[len-1] != '\n');
	len--;
	data[len] = 0;
//	printf("recv line: %s[%d]\n", data, len);
	return len;
}

int sendline(int port, unsigned char *data, int len)
{
//	tcflush(port, TCIOFLUSH);
	write(port, data, len);
//	printf("%s\n", data);
//	printf("send line: %s\n", data);
	return len;
}

