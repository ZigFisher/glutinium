#ifndef __HI_PIRIS_H__
#define __HI_PIRIS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */


typedef enum hiPIRIS_STATUS_E
{
    PIRIS_IDLE = 0,
    PIRIS_BUSY,

    PIRIS_BUTT,
} PIRIS_STATUS_E;


typedef struct hiPIRIS_DATA_S
{
    unsigned char ZeroIsMax;
    unsigned int  TotalStep;
    int  CurPos;
} PIRIS_DATA_S;



#define PIRIS_IOC_MAGIC  'p'

#define PIRIS_SET_ACT_ARGS _IOW(PIRIS_IOC_MAGIC,  1, int)
#define PIRIS_SET_ORGIN    _IOW(PIRIS_IOC_MAGIC,  2, int)
#define PIRIS_SET_CLOSE    _IOW(PIRIS_IOC_MAGIC,  3, int)
#define PIRIS_GET_STATUS   _IOR(PIRIS_IOC_MAGIC,  5, int)

#define PIRIS_IOC_MAXNR 5


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif	/* __HI_PIRIS_H__ */


