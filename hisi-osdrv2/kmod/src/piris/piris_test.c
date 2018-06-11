
#include <stdio.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "strfunc.h"
#include "piris.h"


int main(int argc , char* argv[])
{
    int fd = -1;
    int ret = 0;
    unsigned int piris_pos;
    PIRIS_STATUS_E sta;
    int step = 0;
    int toclose = 0;
    int i = 0;
    unsigned int TestSample = 0;

    if (argc < 2)
    {
        printf("usage: %s \n <cmd(a: action, o: origin, s: status, t: test IRIS sample)> \n <pos> . \n sample: %s a 92\n", argv[0], argv[0]);
        return -1;
    }

    fd = open("/dev/piris", 0);
    if (fd < 0)
    {
        printf("Open piris error!\n");
        return -1;
    }


    if (*argv[1] == 'a')
    {
        if (StrToNumber(argv[2], (unsigned int*)&piris_pos))
        {
            close(fd);
            return 0;
        }
        ret = ioctl(fd, PIRIS_SET_ACT_ARGS, &piris_pos);
    }
    else if (*argv[1] == 'o')
    {
        ret = ioctl(fd, PIRIS_SET_ORGIN, &piris_pos);
    }
    else if (*argv[1] == 's')
    {
        ret = ioctl(fd, PIRIS_GET_STATUS, &sta);

        if (sta == PIRIS_BUSY)
        {
            printf("piris running!!!\n");
        }
        else
        {
            printf("piris idle!!\n");
        }
    }
    else if (*argv[1] == 't')
    {
        if (StrToNumber(argv[2], (unsigned int*)&TestSample))
        {
            close(fd);
            return 0;
        }
        switch (TestSample)
        {
            case 0:
                i = 0;
                step = 0;
                toclose = 0;
                while (i < 10000)
                {
                    ioctl(fd, PIRIS_GET_STATUS, &sta);
                    if (sta != PIRIS_BUSY)
                    {
                        if (toclose == 1)
                        {
                            step = 92;
                            toclose = 0;
                        }
                        else
                        {
                            step = 0;
                            toclose = 1;
                        }

                        piris_pos = step;
                        ioctl(fd, PIRIS_SET_ACT_ARGS, &piris_pos);
                        i++;
                    }
                }
                break;
            case 1:
                step = 0;
                toclose = 0;
                while (step != 92)
                {
                    ioctl(fd, PIRIS_GET_STATUS, &sta);
                    if (sta != PIRIS_BUSY)
                    {
                        if (toclose == 1)
                        {
                            step -= 19;
                            toclose = 0;
                        }
                        else
                        {
                            step += 20;
                            toclose = 1;
                        }

                        piris_pos = step;
                        ioctl(fd, PIRIS_SET_ACT_ARGS, &piris_pos);
                    }
                }
                break;

            default:
                break;
        }
    }

    close(fd);

    return ret;
}
