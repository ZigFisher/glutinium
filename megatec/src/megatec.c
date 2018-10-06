
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>


struct UPSData{
    float IPVolts,IPFaultVolts,OPVolts,IPFreq,BatVolts,Temp;
    int OPCurr;
    unsigned char PowerFail,BatLow,BypassActive,UPSFailed,UPSisSTBY,TestActive;
    unsigned char ShutdownActive,BeeperOn;
};

int parsedata(char* buf, struct UPSData *d) {
    int r =sscanf(buf,"(%f%f%f%d%f%f%f %c%c%c%c%c%c%c%c",
                  &d->IPVolts,&d->IPFaultVolts,&d->OPVolts,&d->OPCurr,&d->IPFreq,&d->BatVolts,&d->Temp,
                  &d->PowerFail,&d->BatLow,&d->BypassActive,&d->UPSFailed,&d->UPSisSTBY,&d->TestActive,
                  &d->ShutdownActive,&d->BeeperOn);
    d->PowerFail-='0'; d->BatLow-='0'; d->BypassActive-='0'; d->UPSFailed-='0';
    d->UPSisSTBY-='0'; d->TestActive-='0'; d->ShutdownActive-='0'; d->BeeperOn-='0';
    if (r<14) {
        d->PowerFail-='0'; d->BatLow-='0'; d->BypassActive-='0'; d->UPSFailed-='0';
        d->UPSisSTBY-='0'; d->TestActive-='0'; d->ShutdownActive-='0'; d->BeeperOn-='0';
        return -2;
    }
    return 0;
}

void printjson(struct UPSData *d) {
    printf("{\n"
           "    \"InputVoltage\": %3.1f,\n"
           "    \"InpFaultVolts\": %3.1f,\n"
           "    \"OutputVoltage\": %3.1f,\n"
           "    \"OutputCurrent\": %d,\n"
           "    \"InputFreq\": %2.2f,\n"
           "    \"BatteryVoltage\": %2.1f,\n"
           "    \"Temperatyre\": %2.1f,\n"
           "    \"PowerFailure\": %d,\n"
           "    \"BatteryLow\": %d,\n"
           "    \"BypassActive\": %d,\n"
           "    \"UPSFailed\": %d,\n"
           "    \"UPSTypStdBy\": %d,\n"
           "    \"TestInProgr\": %d,\n"
           "    \"ShutdnActive\": %d,\n"
           "    \"BeeperOn\": %d\n"
           "}\n",
           d->IPVolts,d->IPFaultVolts,d->OPVolts,d->OPCurr,d->IPFreq,d->BatVolts,d->Temp,
           d->PowerFail,d->BatLow,d->BypassActive,d->UPSFailed,d->UPSisSTBY,d->TestActive,
           d->ShutdownActive,d->BeeperOn);
}

int fd;
char buf[512];
int main (int argc, char* argv[])
{

    int iIn,iOut;
    if (argc>=2)
    {
        fd = open(argv[1], O_RDWR | O_NOCTTY | O_NDELAY);
        if (fd == -1)   {
            printf("error port\n");
            perror("open_port: Unable to open port - ");   }
        else
        {
            struct termios options;
            tcgetattr(fd, &options);

            cfsetispeed(&options, B2400);

            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            options.c_cflag &= ~CSIZE;
            options.c_cflag |= CS8;
            tcsetattr(fd, TCSANOW, &options);
        }

        char Params[64];

        usleep(1800);
        iIn=read(fd,buf,250);
        usleep(1800);
        iOut = write(fd, "Q1\r", 3);
        usleep(800000);
        if (iOut < 0)  fputs("write() of 4 bytes failed!\n", stderr);
        iIn=read(fd,buf,250);

        strncpy(Params,&buf[0],50);
        Params[50]=' ';

        close(fd);

     // char BUF[256] = "(234.8 234.8 196.9 000 50.0 2.22 48.0 00000001";
        struct UPSData data;
     /* printf("%s\r\n",Params); */
        parsedata(Params, &data);
        printjson(&data);
    }
    else
        printf("Usage %s /dev/ttySx", argv[0]);

}
