/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : himd.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2005/7/27
  Last Modified :
  Description   : HI ZSP Encoder Image's Memory Dump
  Function List :
  History       :
  1.Date        : 2005/7/27
    Author      : T41030
    Modification: Created file

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memmap.h"
#include "hi.h"
#include "strfunc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */




#define DEFAULT_STRIDE  1024
#define DEFAULT_WIDTH   352
#define DEFAULT_HEIGHT  288

#define DEFAULT_SAVEFILE    "vdump.cif"

HI_RET hivd(int argc , char* argv[])
{
    U32 ulYAddress, ulCAddress;
    U32 ulYStride = DEFAULT_STRIDE;
    U32 ulCStride = DEFAULT_STRIDE;
    U32 ulWidth = DEFAULT_WIDTH;
    U32 ulHeight = DEFAULT_HEIGHT;

    char savefile[256] = DEFAULT_SAVEFILE;

    FILE * pfile;

    VOID* pYMem  = NULL;
    VOID* pCMem  = NULL;
    LENGTH_T YLen, CLen;


    UINT8 *pMemContent;
    
    UINT32 i, j;
    



    if (argc < 3)
    {
        /* Y_address and C_address must be given */
        printf("usage: %s <YAddr> <CAddr> <YStirde> <CStride> <width> <height> <savefile>\n"
                "sample: %s 0x63010000 0x63020000 1024 1024 352 288 dump.cif\n",
                argv[0], argv[0]);
        EXIT("", -1);
    }

    if(argc >= 8)
    {
        strcpy(savefile, argv[7]);
    }

    if(argc >= 7)
    {
        if(StrToNumber(argv[6], &ulHeight) != HI_SUCCESS )
        {
            printf("Please input height like 0x100 or 256\n");
            return 0;            
        }
        else
        {
            printf("ulHeight = %lu\n", ulHeight);
        }
    }

    if(argc >= 6)
    {
        if(StrToNumber(argv[5], &ulWidth) != HI_SUCCESS )
        {
            printf("Please input width like 0x100 or 256\n");
            return 0;            
        }
        else
        {
            printf("ulWidth = %lu\n", ulWidth);
        }
        
    }

    if(argc >= 5)
    {
        if(StrToNumber(argv[4], &ulCStride) != HI_SUCCESS )
        {
            printf("Please input C_stride like 0x100 or 256\n");
            return 0;
        }
        else
        {
            //ulCStride *= 2;
            printf("ulCStride = %lu\n", ulCStride);
        }
    }

    if(argc >= 4)
    {
        if(StrToNumber(argv[3], &ulYStride) != HI_SUCCESS )
        {
            printf("Please input Y_stride like 0x100 or 256\n");
            return 0;
        }
        else
        {
            printf("ulYStride = %lu\n", ulYStride);
        }
    }

    if((StrToNumber(argv[1], &ulYAddress) != HI_SUCCESS)
        ||(StrToNumber(argv[2], &ulCAddress) != HI_SUCCESS))
    {
        printf("Please input address like 0x12345\n");
        return 0;
    }
    else
    {
        printf("ulYAddress = %#lx\n", ulYAddress);
        printf("ulCAddress = %#lx\n", ulCAddress);        
    }

    YLen = ulYStride * ulHeight;
    CLen = ulCStride * ulHeight;

    pYMem = (VOID *)memmap(ulYAddress, YLen);
    pCMem = (VOID *)memmap(ulCAddress, CLen);
    
    if((pfile = fopen(savefile, "wb")) == NULL)
    {
        printf("open file %s error!\n", savefile);
        return 0;
    }

    /* save file */
    printf("\nBegin to dump video image to file :%s\n", savefile);

    /* dump Y component */
    for(i=0; i<ulHeight; i++)
    {
        pMemContent = (UINT8 *)((U32)pYMem + i*ulYStride);
        fwrite(pMemContent, ulWidth, 1, pfile);
    }
    //printf("Y OK.\n");
       

    /* dump U component */
    for(i=0; i<ulHeight/2; i++)
    {
        pMemContent = (UINT8 *)((U32)pCMem + i*ulCStride + 1);
        for(j=0; j<ulWidth/2; j++)
        {
            fwrite(pMemContent, 1, 1, pfile);
            pMemContent += 2;
        }
    }
    //printf("U OK.\n");

    /* dump V component */
    for(i=0; i<ulHeight/2; i++)
    {
        pMemContent = (UINT8 *)((U32)pCMem + i*ulCStride);
        for(j=0; j<ulWidth/2; j++)
        {
            fwrite(pMemContent, 1, 1, pfile);
            pMemContent += 2;
        }
    }
    //printf("V OK.\n");

    printf("Dump done!\n");

    fclose(pfile);
    
    return 0;
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


