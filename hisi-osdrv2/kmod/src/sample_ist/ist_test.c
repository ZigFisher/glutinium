
#include <stdio.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "strfunc.h"
#include "sample_ist.h"


int main(int argc , char* argv[])
{
    int fd = -1;
    unsigned int node_index;
    
    if (argc < 3)
    {
        printf("use like [./ist_test + 0] -> add node 0\n");
        printf("........ [./ist_test - 0] -> del node 0\n");
        return -1;
    }

    fd = open("/dev/sample_ist", 0);
    if (fd < 0)
    {
        printf("Open sample_ist error!\n");
        return -1;
    }

    if (StrToNumber(argv[2], &node_index))
    {
        close(fd);
        return 0;
    }

    if (*argv[1] == '+')
    {
        ioctl(fd, SAMPLE_IST_ADD_NODE, &node_index);
    }
    else if (*argv[1] == '-')
    {
        ioctl(fd, SAMPLE_IST_DEL_NODE, &node_index);
    }   

    close(fd);

    return 0;
}
