
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

void printdata(struct UPSData *d) {
    printf("------------------------------------------------\n");
    printf("Input Voltage:\t%3.1f\nInp.FaultVolts:\t%3.1f\n"
           "Output Voltage:\t%3.1f\nOutputCurrent%%:\t%d\nInput Freq.:\t%2.2f\n"
           "BatteryVoltage:\t%2.1f\nTemperatyre:\t%2.1f\nPower Failure:\t%d\n"
           "Battery Low:\t%d\nBypass Active:\t%d\nUPS Failed:\t%d\n"
           "UPS typ StdBy:\t%d\n"
           "Test in Progr.:\t%d\nShutdn Active:\t%d\nBeeper On:\t%d\n",
           d->IPVolts,d->IPFaultVolts,d->OPVolts,d->OPCurr,d->IPFreq,d->BatVolts,d->Temp,
           d->PowerFail,d->BatLow,d->BypassActive,d->UPSFailed,d->UPSisSTBY,d->TestActive,
           d->ShutdownActive,d->BeeperOn);
    printf("------------------------------------------------\n");
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
    printf("------------------------------------------------\n");
}

int fd;
char buf[512];
int main (int argc, char* argv[])
{

//    char BUF[256] = "(234.8 234.8 196.9 000 50.0 2.22 48.0 00000001";
//    struct UPSData data;
//    parsedata(BUF, &data);
//    printdata(&data);
//    printjson(&data);
//    return 0;

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

        if(argc>=3)
        {
            if (!strcmp(argv[2],"status"))
            {
                iOut = write(fd, "Q1\r", 3);
                usleep(800000);
                if (iOut < 0)  fputs("write() of 4 bytes failed!\n", stderr);
                iIn=read(fd,buf,250);

                strncpy(Params,&buf[38],11);
                Params[46]=0;
            }
            else if (!strcmp(argv[2],"help"))
            {
                printf ("help\r\n");
            }
            else if (!strcmp(argv[2],"name"))
            {
                iOut = write(fd, "I\r", 3);
                usleep(800000);
                if (iOut < 0)  fputs("write() of 4 bytes failed!\n", stderr);
                iIn=read(fd,buf,250);
                strncpy(Params,&buf[0],60);

            }
            else if (!strcmp(argv[2],"stat2"))
            {
                iOut = write(fd, "F\r", 3);
                usleep(800000);
                if (iOut < 0)  fputs("write() of 4 bytes failed!\n", stderr);
                iIn=read(fd,buf,250);
                strncpy(Params,&buf[1],60);
                Params[20]=0;
            }
            else
            {
                printf ("WTF ?\r\n");
            }
        }
        else
        {
            usleep(1800);
            iIn=read(fd,buf,250);
            usleep(1800);
            iOut = write(fd, "Q1\r", 3);
            usleep(800000);
            if (iOut < 0)  fputs("write() of 4 bytes failed!\n", stderr);
            iIn=read(fd,buf,250);

            strncpy(Params,&buf[0],50);
            Params[50]=' ';
        }
        close(fd);



        struct UPSData data;
        printf("%s\r\n",Params);
        parsedata(Params, &data);
     /* printdata(&data); */
        printjson(&data);
    }
    else
        printf("Usage %s /dev/ttySx", argv[0]);

}
