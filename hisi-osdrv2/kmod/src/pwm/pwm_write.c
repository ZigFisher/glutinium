
#include <stdio.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "strfunc.h"
#include "pwm.h"


int main(int argc , char* argv[])
{
    int fd = -1;
    int ret = 0;

    unsigned int  PwmNum;
    unsigned int Duty;
    unsigned int Period;
    unsigned int  enable;

    PWM_DATA_S  stPwmData;


    if (argc < 5)
    {
        printf("usage: %s <PwmNum> <Duty> <Period> <enable>. sample: %s 0x56 0x0 0x28\n", argv[0], argv[0]);
        return -1;
    }

    fd = open("/dev/pwm", 0);
    if (fd < 0)
    {
        printf("Open pwm error!\n");
        return -1;
    }

    if (StrToNumber(argv[1], &PwmNum))
    {
        close(fd);
        return 0;
    }

    if (StrToNumber(argv[2], &Duty))
    {
        close(fd);
        return 0;
    }

    if (StrToNumber(argv[3], &Period))
    {
        close(fd);
        return 0;
    }

    if (StrToNumber(argv[4], &enable))
    {
        close(fd);
        return 0;
    }

    printf("PWM_NUM:0x%4x; Duty:0x%4x; Period:0x%4x;enable:0x%4x.\n", PwmNum, Duty, Period, enable);

    stPwmData.pwm_num = PwmNum;
    stPwmData.duty = Duty;
    stPwmData.period = Period;
    stPwmData.enable = enable;

    ret = ioctl(fd, PWM_CMD_WRITE, &stPwmData);
    printf("ret :%d\n", ret);

    //printf("device_addr:0x%2x; reg_addr:0x%2x; reg_value:0x%2x.\n", device_addr, reg_addr, reg_value);

    close(fd);

    return 0;
}
