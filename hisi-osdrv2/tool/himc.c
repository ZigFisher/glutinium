/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : himc.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2005/7/1
  Last Modified :
  Description   : HI Memory Modify
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

HI_RET himc(int argc , char* argv[])
{
    U32 ulAddr = 0;
    VOID* pMem  = NULL;
    LENGTH_T len ;
    U32 c;
    int i = 0;
    
    if (argc < 3)
    {
        printf(" clear(set) memory. \n"
                 "usage: %s <address> <length> [value]. \n"
                 "Address: Phy address. 0x80040000 \n"
                 "Length: the length to set \n"
                 "Value: default 0 \n"
                 "sample: %s 0x80040000 100 1\n"
                 "Clear memory from 0x80040000, to 0x80040064\n", argv[0], argv[0]);
        EXIT("", -1);
    }

    len = 0;
    if (StrToNumber(argv[2], &len) != HI_SUCCESS)
    {
        printf("length is invalid.\n");
        exit(1);
    }

    if (len == 0)
    {
        printf("length is invalid.\n");
        exit(1);
    }

    if (len != ALIGN_LENGTH(len , ALIGNTYPE_2BYTE))
    {
        printf("length %#lX is invalid.must 2 byte aligned\n", len);
        exit(1);
    
    }

    c = 0;
    if (argc ==4)
    {
        if (StrToNumber(argv[3], &c) != HI_SUCCESS)
        {
            printf("value %s is invalid.\n", argv[4]);
            exit(1);
        }        
    }

    if( StrToNumber(argv[1], &ulAddr) == HI_SUCCESS)
    {
        printf("====clear memory "STRFMT_ADDR32", length %lu====\n", ulAddr, len);
        #ifdef PC_EMULATOR
        #define SHAREFILE "../shm"
        printf("**** is Emulator, use share file : %s ****\n", SHAREFILE);
        pMem = mmapfile(SHAREFILE , len);
        if (NULL == pMem)
        {
            EXIT("Memory Map error.", -1);
        }
        pMem += ulAddr;
        #else        
        pMem = memmap(ulAddr, len);
        if (NULL == pMem)
        {
            EXIT("Memory Map error.", -1);
        }        
        #endif


        int zlen = LEN_C2Z(len);
        ZWORD* pData = (ZWORD*) pMem;
        for (i = 0; i < zlen; i++)
        {
            pData[i] = (ZWORD)c;
        }
                
        hi_hexdump(STDOUT, pMem, len, 16);
        
    }
    else
    {
        printf("Please input address like 0x12345\n");
    }
    
    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


