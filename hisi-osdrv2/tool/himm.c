/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : himd.c
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

#define DEFAULT_MD_LEN 128

HI_RET himm(int argc , char* argv[])
{
    U32 ulAddr = 0;
    U32 ulOld, ulNew;
    char strNew[16];
    VOID* pMem  = NULL;

    if (argc <= 1)
    {
        printf("usage: %s <address> <Value>. sample: %s 0x80040000 0x123\n", argv[0], argv[0]);
        EXIT("", -1);
    }
    if (argc == 2)
    {
        if( StrToNumber(argv[1], &ulAddr) == HI_SUCCESS)
        {
            printf("====dump memory %#lX====\n", ulAddr);
            #ifdef PC_EMULATOR
            #define SHAREFILE "../shm"
            printf("**** is Emulator, use share file : %s ****\n", SHAREFILE);
            pMem = mmapfile(SHAREFILE , DEFAULT_MD_LEN);
            if (NULL == pMem)
            {
                EXIT("Memory Map error.", -1);
            }
            pMem += ulAddr;
            #else        
            pMem = memmap(ulAddr, DEFAULT_MD_LEN);
            #endif
            ulOld = *(U32*)pMem;
            /*hi_hexdump(STDOUT, pMem, DEFAULT_MD_LEN, 16);*/
            printf("%s: 0x%08lX\n", argv[1], ulOld);
            printf("NewValue:");
            scanf("%s", strNew);
            if (StrToNumber(strNew, &ulNew) == HI_SUCCESS)
            {
                *(U32*)pMem = ulNew;
            }
            else
            {
                printf("Input Error\n");
            }
        }
        else
        {
            printf("Please input address like 0x12345\n");
        }
    }
    else if (argc == 3)
    {
        if( StrToNumber(argv[1], &ulAddr) == HI_SUCCESS &&  
            StrToNumber(argv[2], &ulNew) == HI_SUCCESS)
        {
            pMem = memmap(ulAddr, DEFAULT_MD_LEN);
            ulOld = *(U32*)pMem;
            /*hi_hexdump(STDOUT, pMem, DEFAULT_MD_LEN, 16);*/
            printf("%s: 0x%08lX --> 0x%08lX \n", argv[1], ulOld, ulNew);
            *(U32*)pMem = ulNew;
        }
    }
    else
    {
        printf("xxx\n");
    }
    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */





