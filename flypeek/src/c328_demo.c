#include <stdio.h>
#include <unistd.h>

#include "comm_low.h"

#define CMD_HEADER		0
#define	CMD_ID			1
#define CMD_PARM1		2
#define	CMD_PARM2		3
#define CMD_PARM3		4
#define	CMD_PARM4		5

#define C328_CMD_HEADER		0xaa
#define C328_CMD_SYNC		0x0d
#define C328_CMD_ACK		0x0e

int Send_SYNC_Cmd();
int Send_Cmd(unsigned char *SendCmd);
int Wait_For_ACK(unsigned char *RecvData, int TimeOut);
int Send_ACK_Cmd();
int serial;

//-----------------------------------------------------------------------------------------------

int Send_SYNC_Cmd()		// use this function to connect with c328. if success, return true, else return false.
{
	int ret;
	unsigned char SendCmd[] = { C328_CMD_HEADER, C328_CMD_SYNC, 0x00, 0x00, 0x00, 0x00 };
	unsigned char RecvCmd[6];
	int i = 0;
printf("Send_SYNC_Cmd()\n");
	while(1) {
		i++;
		if(i > 60) {	// you can edit the max times of sending sync command
			ret = -1;
			break;
		}

printf("sendbytes()\n");
		ret = sendbytes(serial, SendCmd, 6);
printf("ret = %d\n", ret);

		if(ret == -1) {
			break;
		}

		usleep(50);
		ret = Wait_For_ACK(RecvCmd, 2);	// read ACK command 

		if(
			!ret ||
			RecvCmd[CMD_HEADER] != C328_CMD_HEADER ||
			RecvCmd[CMD_ID] != C328_CMD_ACK ||
			RecvCmd[CMD_PARM1] != C328_CMD_SYNC
		) {
			continue;
		}

		ret = Wait_For_ACK(RecvCmd, 2);	// read SYNC command

		if(
			ret == -1 ||
			RecvCmd[CMD_ID] != C328_CMD_SYNC
		) {
			continue;
		}

		ret = Send_ACK_Cmd();		// send ACK command
		break;
	}

	usleep(50);
	return ret;
}

int Send_ACK_Cmd()				// use this function to send ACK command
{
	int ret;
	unsigned char SendCmd[] = { C328_CMD_HEADER, C328_CMD_ACK, 0x00, 0x00, 0x00, 0x00 };

	ret = sendbytes(serial, SendCmd, 6);

	usleep(1);
	return ret;
}

int Wait_For_ACK(unsigned char *RecvData, int TimeOut)	// use this function to get ACK command
{
	int ret;
	int cnt;

	unsigned char RecvCmd[6] = {0, 0, 0, 0, 0, 0};
	int TotalBytesNumberToRead = 6;
printf("Wait_For_ACK()\n");
	ret = recvbytes(serial, RecvCmd, 6, 2000000);
printf("ret = %d\n", ret);
	if(!ret) {
		return -1;
	}

	if(ret != TotalBytesNumberToRead) {
		return -1;
	}

	if(RecvCmd[CMD_HEADER] == 0xaa && RecvCmd[CMD_ID] >= 0x01 && RecvCmd[CMD_ID] <= 0x0F) {
		ret = 0;
		for(cnt = 0; cnt < TotalBytesNumberToRead; cnt++) {
			printf("0x%02x ", RecvCmd[cnt]);
		}
		printf("\n");
//		memcpy(pRet_Resp_Cmd, Temp_Cmd_Buff, 6);
	} else {
		ret = -1;
	}

	return ret;
}


int main()
{
	if((serial = initport("/dev/ttyUSB0", B115200, 0)) < 0) {
		return -1;
	}
	Send_SYNC_Cmd();
	return 0;
}
