/*ringbuf .c*/

#include<stdio.h>
#include<ctype.h>
#include <stdlib.h>
#include <string.h>
#include "ringfifo.h"
#include "rtputils.h"
#include "sample_comm.h"
#define NMAX 32

int iput = 0; /* 环形缓冲区的当前放入位置 */
int iget = 0; /* 缓冲区的当前取出位置 */
int n = 0; /* 环形缓冲区中的元素总数量 */

struct ringbuf ringfifo[NMAX];
extern int UpdateSpsOrPps(unsigned char *data,int frame_type,int len);
/* 环形缓冲区的地址编号计算函数，如果到达唤醒缓冲区的尾部，将绕回到头部。
环形缓冲区的有效地址编号为：0到(NMAX-1)
*/
//分配环形缓冲区，总共32个，每个大小为size
void ringmalloc(int size)
{
    int i;
    for(i =0; i<NMAX; i++)
    {
        ringfifo[i].buffer = malloc(size);
        ringfifo[i].size = 0;
        ringfifo[i].frame_type = 0;
       // printf("FIFO INFO:idx:%d,len:%d,ptr:%x\n",i,ringfifo[i].size,(int)(ringfifo[i].buffer));
    }
    iput = 0; /* 环形缓冲区的当前放入位置 */
    iget = 0; /* 缓冲区的当前取出位置 */
    n = 0; /* 环形缓冲区中的元素总数量 */
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
void ringreset()
{
    iput = 0; /* 环形缓冲区的当前放入位置 */
    iget = 0; /* 缓冲区的当前取出位置 */
    n = 0; /* 环形缓冲区中的元素总数量 */
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
void ringfree(void)
{
    int i;
    printf("begin free mem\n");
    for(i =0; i<NMAX; i++)
    {
       // printf("FREE FIFO INFO:idx:%d,len:%d,ptr:%x\n",i,ringfifo[i].size,(int)(ringfifo[i].buffer));
        free(ringfifo[i].buffer);
        ringfifo[i].size = 0;
    }
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
int addring(int i)
{
    return (i+1) == NMAX ? 0 : i+1;
}

/**************************************************************************************************
**
**
**
**************************************************************************************************/
/* 从环形缓冲区中取一个元素 */

int ringget(struct ringbuf *getinfo)
{
    int Pos;
    if(n>0)
    {
        Pos = iget;
        iget = addring(iget);
        n--;
        getinfo->buffer = (ringfifo[Pos].buffer);
        getinfo->frame_type = ringfifo[Pos].frame_type;
        getinfo->size = ringfifo[Pos].size;
        //printf("Get FIFO INFO:idx:%d,len:%d,ptr:%x,type:%d\n",Pos,getinfo->size,(int)(getinfo->buffer),getinfo->frame_type);
        return ringfifo[Pos].size;
    }
    else
    {
        //printf("Buffer is empty\n");
        return 0;
    }
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
/* 向环形缓冲区中放入一个元素*/
void ringput(unsigned char *buffer,int size,int encode_type)
{

    if(n<NMAX)
    {
        memcpy(ringfifo[iput].buffer,buffer,size);
        ringfifo[iput].size= size;
        ringfifo[iput].frame_type = encode_type;
        //printf("Put FIFO INFO:idx:%d,len:%d,ptr:%x,type:%d\n",iput,ringfifo[iput].size,(int)(ringfifo[iput].buffer),ringfifo[iput].frame_type);
        iput = addring(iput);
        n++;
    }
    else
    {
        //  printf("Buffer is full\n");
    }
}

/**************************************************************************************************
**将H264流数据放到ringfifo[iput].buffer里以便schedule_do线程从ringfifo[iput].buffer里取出数据发送出去
**同是在DESCRIBE步骤中会对SPS,PPS编码发送给客户端，后面好像就只编码但没有发送出去
**
**************************************************************************************************/
HI_S32 HisiPutH264DataToBuffer(VENC_STREAM_S *pstStream)
{
	HI_S32 i,j;
	HI_S32 len=0,off=0,len2=2;
	unsigned char *pstr;
	int iframe=0;
	for (i = 0; i < pstStream->u32PackCount; i++)
	{
		len+=pstStream->pstPack[i].u32Len-pstStream->pstPack[i].u32Offset;
	}

	int testlen=0;
    if(n<NMAX)
    {
		for (i = 0; i < pstStream->u32PackCount; i++)
		{
			
			memcpy(ringfifo[iput].buffer+off,pstStream->pstPack[i].pu8Addr+pstStream->pstPack[i].u32Offset,pstStream->pstPack[i].u32Len-pstStream->pstPack[i].u32Offset);
			//printf("\nff=%x",pstStream->pstPack[i].u32Offset);
			pstr=pstStream->pstPack[i].pu8Addr+pstStream->pstPack[i].u32Offset;//计算当前PACK的有效数据的首地址
			/*
			if(pstr[4]==0x68)
			{
				printf("\nx=%p",ringfifo[iput].buffer+off);
				for(testlen=0;testlen<pstStream->pstPack[i].u32Len-pstStream->pstPack[i].u32Offset;testlen++)
					printf(" %x ",*(ringfifo[iput].buffer+off+testlen));
			}
			*/
			//pstr=pstStream->pstPack[i].pu8Addr+pstStream->pstPack[i].u32Offset;//这一行一定要，要不然后面的if判断pstr[4]
			//如果在这里不要，pstr[4]会不存在，从而提示段错误
/*
			if(pstr[4]==0x67)
			{
				printf("a=%d,%p ",*(ringfifo[iput].buffer+off),ringfifo[iput].buffer+off);
			}
			*/
			off+=pstStream->pstPack[i].u32Len-pstStream->pstPack[i].u32Offset;//计算下个PACK存放到ring里的首地址
			//上面那个off计算应该移到下面//aaaaaaaaaaaaaa处???????????????????????????????
			//pstr=pstStream->pstPack[i].pu8Addr+pstStream->pstPack[i].u32Offset;//计算当前PACK的有效数据的首地址

			if(pstr[4]==0x67)
			{//使用 base64 对 data 进行编码,设计此种编码是为了使二进制数据可以通过
			//非纯 8-bit 的传输层传输，例如电子邮件的主体
			//在网络上基本上是非纯8位传输，所以要将数据在服务器编码为
			//base64(非8位的，6位的)，然后由客户端解码
			//将H264数据用base64编码然后才发送
				UpdateSps(ringfifo[iput].buffer+off,9);
				//printf("b=%d,%p ",*(ringfifo[iput].buffer+off),ringfifo[iput].buffer+off);
				iframe=1;
			}
			if(pstr[4]==0x68)
			{
				//printf("\ny=%p",ringfifo[iput].buffer+off);
				//for(testlen=0;testlen<pstStream->pstPack[i].u32Len-pstStream->pstPack[i].u32Offset;testlen++)
				//	printf(" %x ",*(ringfifo[iput].buffer+off+testlen));
				UpdatePps(ringfifo[iput].buffer+off,4);
			}
			//off+=pstStream->pstPack[i].u32Len-pstStream->pstPack[i].u32Offset;//计算下个PACK存放到ring里的首地址
			//aaaaaaaaaaaaaa
			// if (pstStream->pstPack[i].u32Len[1] > 0)
			// {
				// memcpy(ringfifo[iput].buffer+off,pstStream->pstPack[i].pu8Addr[1], pstStream->pstPack[i].u32Len[1]);
				// off+=pstStream->pstPack[i].u32Len[1];
			// }
		}

        ringfifo[iput].size= len;
		//printf("iframe=%d\n",iframe);
		if(iframe)
		{
			ringfifo[iput].frame_type = FRAME_TYPE_I;
		}
        	
		else
			ringfifo[iput].frame_type = FRAME_TYPE_P;
        iput = addring(iput);
        n++;
    }

	 return HI_SUCCESS;
}