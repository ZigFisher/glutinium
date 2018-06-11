
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
    int i = 0;
    unsigned int PwmDirector;
    unsigned int Period;
    PWM_DATA_S  stPwmData;

    if (argc < 3)
    {
        printf("usage: %s  <Director> <Period>  sample: %s 0x56 0x0 0x28\n", argv[0], argv[0]);
        return -1;
    }

    fd = open("/dev/pwm", 0);
    if (fd < 0)
    {
        printf("Open pwm error!\n");
        return -1;
    }

    if (StrToNumber(argv[1], &PwmDirector))
    {
        close(fd);
        return 0;
    }

    if (StrToNumber(argv[2], &Period))
    {
        close(fd);
        return 0;
    }

    printf("PWM_Director:0x%4x;  Period:0x%4x.\n", PwmDirector,  Period);
    stPwmData.pwm_num = 0;
    stPwmData.period = 1000;
    stPwmData.enable = 1;

    if (PwmDirector == 0)
    {
        stPwmData.duty = Period;
    }
    else
    {
        stPwmData.duty = 1;
    }

    //for(i = 0;i < Period;i++)
    while (i < 10)
    {
        /*
        	if(PwmDirector == 0)
        	{
        		stPwmData.duty--;
        	}
        	else
        	{
        		stPwmData.duty++;
        	}
        */
        stPwmData.duty = 420;
        ret = ioctl(fd, PWM_CMD_WRITE, &stPwmData);
        printf("ret :%d\n", ret);
        usleep(66000);
        stPwmData.duty = 610;
        ret = ioctl(fd, PWM_CMD_WRITE, &stPwmData);
        printf("ret :%d\n", ret);
        usleep(66000);
        i++;
    }

    //printf("device_addr:0x%2x; reg_addr:0x%2x; reg_value:0x%2x.\n", device_addr, reg_addr, reg_value);
    close(fd);

    return 0;
}
