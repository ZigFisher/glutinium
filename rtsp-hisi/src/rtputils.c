#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <net/if.h>
#include <netinet/in.h>
#include <pthread.h>

#include "rtspservice.h"
#include "rtputils.h"
#include "rtsputils.h"
#ifdef __cplusplus
extern "C" {
#endif

//#define SAVE_NALU 0

typedef struct
{
    /**//* byte 0 */
    unsigned char u4CSrcLen:4;      /**//* expect 0 */
    unsigned char u1Externsion:1;   /**//* expect 1, see RTP_OP below */
    unsigned char u1Padding:1;      /**//* expect 0 */
    unsigned char u2Version:2;      /**//* expect 2 */
    /**//* byte 1 */
    unsigned char u7Payload:7;      /**//* RTP_PAYLOAD_RTSP */
    unsigned char u1Marker:1;       /**//* expect 1 */
    /**//* bytes 2, 3 */
    unsigned short u16SeqNum;
    /**//* bytes 4-7 */
    unsigned long long u32TimeStamp;
    /**//* bytes 8-11 */
    unsigned long u32SSrc;          /**//* stream number is used here. */
} StRtpFixedHdr;

typedef struct
{
    //byte 0
    unsigned char u5Type:5;
    unsigned char u2Nri:2;
    unsigned char u1F:1;
} StNaluHdr; /**/ /* 1 BYTES */

typedef struct
{
    //byte 0
    unsigned char u5Type:5;
    unsigned char u2Nri:2;
    unsigned char u1F:1;
} StFuIndicator; /**/ /* 1 BYTES */

typedef struct
{
    //byte 0
    unsigned char u5Type:5;
    unsigned char u1R:1;
    unsigned char u1E:1;
    unsigned char u1S:1;
} StFuHdr; /**/ /* 1 BYTES */

typedef struct _tagStRtpHandle
{
    int                 s32Sock;
    struct sockaddr_in  stServAddr;
    unsigned short      u16SeqNum;
    unsigned long long        u32TimeStampInc;
    unsigned long long        u32TimeStampCurr;
    unsigned long long      u32CurrTime;
    unsigned long long      u32PrevTime;
    unsigned int        u32SSrc;
    StRtpFixedHdr       *pRtpFixedHdr;
    StNaluHdr           *pNaluHdr;
    StFuIndicator       *pFuInd;
    StFuHdr             *pFuHdr;
    EmRtpPayload        emPayload;
#ifdef SAVE_NALU
    FILE                *pNaluFile;
#endif
} StRtpObj, *HndRtp;
/**************************************************************************************************
**返回hndRtp结构体的首地址，是个数字。然后外部使用时又将这个数字强制转为hndRtp结构体指针
**功能:创建RTP套接字(里面设置IP,端口，负荷，时间)
**参数:
	u32IP:	IN,目的端IP
	s32Port:	IN,目的端端口
	emPayload:IN,负荷类型
返回:成功返回HndRtp句柄，失败返回0
**************************************************************************************************/
unsigned int RtpCreate(unsigned int u32IP, int s32Port, EmRtpPayload emPayload)
{
    HndRtp hRtp = NULL;
    struct timeval stTimeval;
    struct ifreq stIfr;
    int s32Broadcast = 1;

    hRtp = (HndRtp)calloc(1, sizeof(StRtpObj));
    if(NULL == hRtp)
    {
        printf("Failed to create RTP handle\n");
        goto cleanup;
    }


    hRtp->s32Sock = -1;
    if((hRtp->s32Sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("Failed to create socket\n");
        goto cleanup;
    }

    if(0xFF000000 == (u32IP & 0xFF000000))
    {
        if(-1 == setsockopt(hRtp->s32Sock, SOL_SOCKET, SO_BROADCAST, (char *)&s32Broadcast, sizeof(s32Broadcast)))
        {
            printf("Failed to set socket\n");
            goto cleanup;
        }
    }

    hRtp->stServAddr.sin_family = AF_INET;
    hRtp->stServAddr.sin_port = htons(s32Port);
    hRtp->stServAddr.sin_addr.s_addr = u32IP;
    bzero(&(hRtp->stServAddr.sin_zero), 8);

    //初始化序号
    hRtp->u16SeqNum = 0;
    //初始化时间戳
    hRtp->u32TimeStampInc = 0;
    hRtp->u32TimeStampCurr = 0;

    //获取当前时间
    if(gettimeofday(&stTimeval, NULL) == -1)
    {
        printf("Failed to get os time\n");
        goto cleanup;
    }

    hRtp->u32PrevTime = stTimeval.tv_sec * 1000 + stTimeval.tv_usec / 1000;

    hRtp->emPayload = emPayload;

    //获取本机网络设备名
    strcpy(stIfr.ifr_name, "eth0");
    if(ioctl(hRtp->s32Sock, SIOCGIFADDR, &stIfr) < 0)
    {
        //printf("Failed to get host ip\n");
        strcpy(stIfr.ifr_name, "wlan0");
        if(ioctl(hRtp->s32Sock, SIOCGIFADDR, &stIfr) < 0)
        {
            printf("Failed to get host eth0 or wlan0 ip\n");
            goto cleanup;
        }
    }

    hRtp->u32SSrc = htonl(((struct sockaddr_in *)(&stIfr.ifr_addr))->sin_addr.s_addr);

    //hRtp->u32SSrc = htonl(((struct sockaddr_in *)(&stIfr.ifr_addr))->sin_addr.s_addr);
    //printf("rtp create:addr:%x,port:%d,local%x\n",u32IP,s32Port,hRtp->u32SSrc);
#ifdef SAVE_NALU
    hRtp->pNaluFile = fopen("nalu.264", "wb+");
    if(NULL == hRtp->pNaluFile)
    {
        printf("Failed to open nalu file!\n");
        goto cleanup;
    }
#endif
    printf("<><><><>success creat RTP<><><><>\n");

    return (unsigned int)hRtp;

cleanup:
    if(hRtp)
    {
        if(hRtp->s32Sock >= 0)
        {
            close(hRtp->s32Sock);
        }

        free(hRtp);
    }

    return 0;
}
/**************************************************************************************************
**取消视频发送:只要找到RTP的会话，关闭其套接字，然后释放HndRtp即可
**
**
**************************************************************************************************/
void RtpDelete(unsigned int u32Rtp)
{
    HndRtp hRtp = (HndRtp)u32Rtp;

    if(hRtp)
    {
#ifdef SAVE_NALU
        if(hRtp->pNaluFile)
        {
            fclose(hRtp->pNaluFile);
        }
#endif

        if(hRtp->s32Sock >= 0)
        {
            close(hRtp->s32Sock);
        }

        free(hRtp);
    }
}
/**************************************************************************************************
**将H264NALU变成RTP,需要加一些东西
**参数:
	hRtp:		in/out需要填写的RTP结构体
	pNalBuf:		in H264NALU的内容
	s32NalBufSize:in H264NALU的大小
**
//注意这里的pNalBuf包含了00 00 00 01这几个首字节
抓包第0x2a-0x35=12个字节表示RTP头
		第0x36为FA-U指示器(需要H264第一个00参与计算)
		第0x37为FA-U头(需要H264第一个00参与计算)
		第0x38以后为真正的H264数据，如果是第一个FA-U包的话的，因为H264的第一个00未编进来，
		所以抓包看H264开头为00 00 01，少了一个00
发送的缓冲区pSendBuf第0-11字节为RTP头
							第12字节为FA-U指示器
							第13字节为FA-U头部
							第14字节开始与H264数据的第1个字节一一对应(第0字节没有存进来)
查资料感觉FA-U头低5位表示这个FA-U包的类型，应该与真正的H264的NALU的类型一致，即应该是
00 00 00 01 67的第4个字节67参与计算
**************************************************************************************************/
static int SendNalu264(HndRtp hRtp, char *pNalBuf, int s32NalBufSize)
{
    char *pNaluPayload;
    char *pSendBuf;
    int s32Bytes = 0;
    int s32Ret = 0;
    struct timeval stTimeval;
    char *pNaluCurr;
    int s32NaluRemain;
    unsigned char u8NaluBytes;

    pSendBuf = (char *)calloc(MAX_RTP_PKT_LENGTH + 100, sizeof(char));
    if(NULL == pSendBuf)
    {
        s32Ret = -1;
        goto cleanup;
    }
//由H264NALU变成RTP，需要增加一些东西，具体内容网上查RTP协议
//pRtpFixedHdr:RTP固定头部fix固定，Hdr:header头部
//pNaluHdr:Nalu包头部
//pFuInd:FU-A indicator	FU-A 包指示器
//pFuHdr:FU-A header 		FU-A 包头部
//FU-A=FU-A indicator+FU-A header +分片单元荷载
/*pSendBuf指向一块大小1500字节的缓冲区
	hRtp->pRtpFixedHdr = (StRtpFixedHdr *)pSendBuf;表明hRtp->pRtpFixedHdr指向了pSendBuf的前面12字节
	单个NAL包:
	hRtp->pNaluHdr                  = (StNaluHdr *)(pSendBuf + 12);表明hRtp->pNaluHdr 指向了pSendBuf的第12字节(从0开始计算)
	pNaluPayload = (pSendBuf + 13);表明真正的H264数据存在pSendBuf的第13字节处
	FU-A包:
	hRtp->pFuInd            = (StFuIndicator *)(pSendBuf + 12);表明fu indicator指向了pSendBuf的第12字节
	hRtp->pFuHdr            = (StFuHdr *)(pSendBuf + 13);表明fu header位置指向了pSendBuf的第13字节
	pNaluPayload = (pSendBuf + 14);表明真正的H264数据存在pSendBuf的第14字节处
*/
	
  	hRtp->pRtpFixedHdr = (StRtpFixedHdr *)pSendBuf;
	hRtp->pRtpFixedHdr->u2Version   = 2;//RTP协议的版本号

	hRtp->pRtpFixedHdr->u1Marker    = 0;//对视频来说表示标记一帧的结束，后面会被改写为1
    	hRtp->pRtpFixedHdr->u7Payload   = H264;//有效荷载类型,简单点用96表示无类型

	//hRtp->pRtpFixedHdr->u16SeqNum每一个包不同，在后面填写
	
    	//计算时间戳
    	hRtp->pRtpFixedHdr->u32TimeStamp = htonl(hRtp->u32TimeStampCurr * (90000 / 1000));//时间戳
	//printf("sendnalu264 timestamp:%lld\n",hRtp->u32TimeStampCurr);

	hRtp->pRtpFixedHdr->u32SSrc     = hRtp->u32SSrc;//同步信源(SSRC)标识符,一般填客户机的IP
	//hRtp->pRtpFixedHdr->u32CSrc//协议里还有特约信源(CSRC)标识符,这里没有，不填
    
    if(gettimeofday(&stTimeval, NULL) == -1)
    {
        printf("Failed to get os time\n");
        s32Ret = -1;
        goto cleanup;
    }

    //保存nalu首byte
    u8NaluBytes = *pNalBuf;//注意这里的pNalBuf包含了00 00 00 01这几个首字节,所以u8NaluBytes=00
    //设置未发送的Nalu数据指针位置
    pNaluCurr = pNalBuf + 1;
/*这里的pNaluCurr是真正的H264数据的第二个，第一个被赋值给了u8NaluBytes参与FU-A indicator与FU-A header 	的计算
如果原来的H264数据为:00 00 00 01 xx xx xx......
那么u8NaluBytes为第一个00
pNaluCurr则为00 00 01 xx xx xx......
所以后面 memcpy(pNaluPayload, pNaluCurr, s32Bytes)时pNaluPayload也为00 00 01 67 xx xx xx......这样发送出去后抓包发现
FA-U的第一个包为00 00 01 xx xx xx......
*/
    //设置剩余的Nalu数据数量
    s32NaluRemain = s32NalBufSize - 1;

    //NALU包小于等于最大包长度，直接发送
    if(s32NaluRemain <= MAX_RTP_PKT_LENGTH)
    {
        hRtp->pRtpFixedHdr->u1Marker    = 1;//对视频来说表示标记一帧的结束，这里一只一个NALU包，一包即结束，所以为1
        hRtp->pRtpFixedHdr->u16SeqNum   = htons(hRtp->u16SeqNum ++);//标识发送者所发送的RTP报文的序列号
        															//用网络字节表示
        hRtp->pNaluHdr                  = (StNaluHdr *)(pSendBuf + 12);
        hRtp->pNaluHdr->u1F             = (u8NaluBytes & 0x80) >> 7;
        hRtp->pNaluHdr->u2Nri           = (u8NaluBytes & 0x60) >> 5;
        hRtp->pNaluHdr->u5Type          = u8NaluBytes & 0x1f;

        pNaluPayload = (pSendBuf + 13);
        memcpy(pNaluPayload, pNaluCurr, s32NaluRemain);

        s32Bytes = s32NaluRemain + 13;
	/*int sendto ( socket s , const void * msg, int len, unsigned int flags, const struct sockaddr * to , int tolen ) ;
		s 套接字
		msg 待发送数据的缓冲区
		len 缓冲区长度
		flags 调用方式标志位, 一般为0, 改变Flags，将会改变Sendto发送的形式
		to （可选）指针，指向目的套接字的地址
		tolen 所指目的地址的长度
		返回:成功则返回实际传送出去的字符数，失败返回－1，错误原因存于errno 中
	*/
        if(sendto(hRtp->s32Sock, pSendBuf, s32Bytes, 0, (struct sockaddr *)&hRtp->stServAddr, sizeof(hRtp->stServAddr)) < 0)
        {
            s32Ret = -1;
            goto cleanup;
        }
#ifdef SAVE_NALU
        fwrite(pSendBuf, s32Bytes, 1, hRtp->pNaluFile);
#endif
    }
    //NALU包大于最大包长度，分批发送  FA-U模式 
    else
    {
        //指定fu indicator位置
        hRtp->pFuInd            = (StFuIndicator *)(pSendBuf + 12);
        hRtp->pFuInd->u1F       = (u8NaluBytes & 0x80) >> 7;
        hRtp->pFuInd->u2Nri     = (u8NaluBytes & 0x60) >> 5;
        hRtp->pFuInd->u5Type    = 28;//前面说明了u8NaluBytes=00，计算hRtp->pFuInd=28= 0x1c,抓包发现第一个会有0x1c(抓包的第0x36个)

        //指定fu header位置
        hRtp->pFuHdr            = (StFuHdr *)(pSendBuf + 13);
        hRtp->pFuHdr->u1R       = 0;//协议规定必须为0
        hRtp->pFuHdr->u5Type    = u8NaluBytes & 0x1f;//前面说明了u8NaluBytes=00,所以hRtp->pFuHdr->u5Type =0

        //指定payload位置
        pNaluPayload = (pSendBuf + 14);

        //当剩余Nalu数据多于0时分批发送nalu数据
        while(s32NaluRemain > 0)
        {
            /*配置fixed header*/
            //每个包序号增1
            hRtp->pRtpFixedHdr->u16SeqNum = htons(hRtp->u16SeqNum ++);
            hRtp->pRtpFixedHdr->u1Marker = (s32NaluRemain <= MAX_RTP_PKT_LENGTH) ? 1 : 0;//最后一个包u1Marker才置1

            /*配置fu header*/
            //最后一批数据则置1
            hRtp->pFuHdr->u1E       = (s32NaluRemain <= MAX_RTP_PKT_LENGTH) ? 1 : 0;//u1E:end,代表最后一批，不是最后一批数据则为0
            //第一批数据则置1
            hRtp->pFuHdr->u1S       = (s32NaluRemain == (s32NalBufSize - 1)) ? 1 : 0;//u1S:start,代表第一批，第一批数据则为1
            /*
            	hRtp->pFuHdr->u5Type    = u8NaluBytes & 0x1f;//前面说明了u8NaluBytes=00，所以hRtp->pFuHdr->u5Type=0
		hRtp->pFuHdr->u1R       = 0;
       	hRtp->pFuHdr->u1E       = (s32NaluRemain <= MAX_RTP_PKT_LENGTH) ? 1 : 0;//不是最后一批数据则为0
       	hRtp->pFuHdr->u1S       = (s32NaluRemain == (s32NalBufSize - 1)) ? 1 : 0;//第一批数据则为1
       	这样计算起来FA-U包第一个包为10000000=0x80，所以抓包看到0x80(抓包的第0x37个)
       							第二个包为00000000=0x00,	所以抓包看到0x00(抓包的第0x37个)
       							最后一个包为01000000=0x40,所以抓包看到0x00(抓包的第0x37个)
		*/

            s32Bytes = (s32NaluRemain < MAX_RTP_PKT_LENGTH) ? s32NaluRemain : MAX_RTP_PKT_LENGTH;


            memcpy(pNaluPayload, pNaluCurr, s32Bytes);

            //发送本批次
            s32Bytes = s32Bytes + 14;
            if(sendto(hRtp->s32Sock, pSendBuf, s32Bytes, 0, (struct sockaddr *)&hRtp->stServAddr, sizeof(hRtp->stServAddr)) < 0)
            {
                s32Ret = -1;
                goto cleanup;
            }
#ifdef SAVE_NALU
            fwrite(pSendBuf, s32Bytes, 1, hRtp->pNaluFile);
#endif

            //指向下批数据
            pNaluCurr += MAX_RTP_PKT_LENGTH;
            //计算剩余的nalu数据长度
            s32NaluRemain -= MAX_RTP_PKT_LENGTH;
        }
    }

cleanup:
    if(pSendBuf)
    {
        free((void *)pSendBuf);
    }

    return s32Ret;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
static int SendNalu711(HndRtp hRtp, char *buf, int bufsize)
{
    char *pSendBuf;
    int s32Bytes = 0;
    int s32Ret = 0;

    pSendBuf = (char *)calloc(MAX_RTP_PKT_LENGTH + 100, sizeof(char));
    if(NULL == pSendBuf)
    {
        s32Ret = -1;
        goto cleanup;
    }
    hRtp->pRtpFixedHdr = (StRtpFixedHdr *)pSendBuf;
    hRtp->pRtpFixedHdr->u7Payload     = G711;
    hRtp->pRtpFixedHdr->u2Version     = 2;

    hRtp->pRtpFixedHdr->u1Marker = 1;   //标志位，由具体协议规定其值。

    hRtp->pRtpFixedHdr->u32SSrc = hRtp->u32SSrc;

    hRtp->pRtpFixedHdr->u16SeqNum  = htons(hRtp->u16SeqNum ++);

    memcpy(pSendBuf + 12, buf, bufsize);

    hRtp->pRtpFixedHdr->u32TimeStamp = htonl(hRtp->u32TimeStampCurr);
    //printf("SendNalu711 timestamp:%lld\n",hRtp->pRtpFixedHdr->u32TimeStamp);
    s32Bytes = bufsize + 12;
    if(sendto(hRtp->s32Sock, pSendBuf, s32Bytes, 0, (struct sockaddr *)&hRtp->stServAddr, sizeof(hRtp->stServAddr)) < 0)
    {
        printf("Failed to send!");
        s32Ret = -1;
        goto cleanup;
    }
cleanup:
    if(pSendBuf)
    {
        free((void *)pSendBuf);
    }
    return s32Ret;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
unsigned int RtpSend(unsigned int u32Rtp, char *pData, int s32DataSize, unsigned int u32TimeStamp)
{
    int s32NalSize = 0;
    char *pNalBuf, *pDataEnd;
    HndRtp hRtp = (HndRtp)u32Rtp;
    unsigned int u32NaluToken;

    hRtp->u32TimeStampCurr = u32TimeStamp;

    if(_h264 == hRtp->emPayload)//发送H264文件，有多个NALU单元，需要找出00000001来分离NALU
    {//printf("\n\t\th264");
        pDataEnd = pData + s32DataSize;
        //搜寻第一个nalu起始标志0x01000000
        for(; pData < pDataEnd-5; pData ++)
        {
            memcpy(&u32NaluToken, pData, 4 * sizeof(char));
            if(0x01000000 == u32NaluToken)
            {
                //标记nalu起始位置
                pData += 4;
                pNalBuf = pData;
                break;
            }
        }
        //发送nalu
        for(; pData < pDataEnd-5; pData ++)
        {
            //搜寻第二个nalu起始标志0x01000000，找到nalu起始位置，发送该nalu数据
            //
            memcpy(&u32NaluToken, pData, 4 * sizeof(char));
            if(0x01000000 == u32NaluToken)
            {
                s32NalSize = (int)(pData - pNalBuf);//二者一相减就是第一个nalu的内容
                if(SendNalu264(hRtp, pNalBuf, s32NalSize) == -1)
                {
                    return -1;
                }

                //标记nalu起始位置
                pData += 4;
                pNalBuf = pData;
            }
        }//while
//最后一个nalu
        if(pData > pNalBuf)
        {
            s32NalSize = (int)(pData - pNalBuf);
            if(SendNalu264(hRtp, pNalBuf, s32NalSize) == -1)
            {
                return -1;
            }
        }
    }
    else if(_h264nalu == hRtp->emPayload)//直接发送NALU单元，所以不需要分享NALU
    {//在rtsp的setup阶段时创建RTP套接字时设置了负荷类型为_h264nalu，所以程序执行这个分支
    //原因3518编码已经帮我们分开了每一个H264的slice，即直接是一个NALU,所以直接添加东西组成RTP即可
    	//printf("\n\t\th264nalu");经打印也验证是执行这个分支，即是_h264nalu
        if(SendNalu264(hRtp, pData, s32DataSize) == -1)
        {
            return -1;
        }
    }
    else if(_g711 == hRtp->emPayload)
    {printf("\n\t\tg711");
        if(SendNalu711(hRtp, pData, s32DataSize) == -1)
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }

    return 0;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/

#ifdef __cplusplus
}
#endif
