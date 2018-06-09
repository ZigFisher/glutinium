/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : himd.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2005/7/27
  Last Modified :
  Description   : HI Memory Dump
  Function List :
  History       :
  1.Date        : 2005/7/27
    Author      : T41030
    Modification: Created file

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "memmap.h"
#include "hi.h"
#include "strfunc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define DEFAULT_MD_LEN 256

HI_RET himd_l(int argc , char* argv[])
{
    U32 ulAddr = 0;
    VOID* pMem  = NULL;
    LENGTH_T len;

    if (argc < 2)
    {
        printf("usage: %s <address> [length]. sample: %s 0x80040000 0x100\n", argv[0], argv[0]);
        EXIT("", -1);
    }


    if (argc == 3)
    {
         if ( StrToNumber(argv[2], &len) != HI_SUCCESS)
        {
            len = DEFAULT_MD_LEN;
        }
    }
    else
    {
        len = DEFAULT_MD_LEN;
    }
    

    if( StrToNumber(argv[1], &ulAddr) == HI_SUCCESS)
    {
        printf("====dump memory 0x%008lX====\n", ulAddr);
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
        
        hi_hexdump2(STDOUT, pMem, len, 16);
    }
    else
    {
        printf("Please input address like 0x12345\n");
    }
    
    return 0;
}


HI_RET himd(int argc , char* argv[])
{
    U32 ulAddr = 0;
    VOID* pMem  = NULL;
    LENGTH_T len;

    if (argc < 2)
    {
        printf("usage: %s <address>. sample: %s 0x80040000\n", argv[0], argv[0]);
        EXIT("", -1);
    }


    if (argc == 3)
    {
         if ( StrToNumber(argv[2], &len) != HI_SUCCESS)
        {
            len = DEFAULT_MD_LEN;
        }
    }
    else
    {
        len = DEFAULT_MD_LEN;
    }
    

    if( StrToNumber(argv[1], &ulAddr) == HI_SUCCESS)
    {
        printf("====dump memory %#010lX====\n", ulAddr);
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
        
        hi_hexdump(STDOUT, pMem, len, 16);
    }
    else
    {
        printf("Please input address like 0x12345\n");
    }
    
    return 0;
}

/*memory dump bin*/
HI_RET himdb(int argc , char* argv[])
{
    U32 ulAddr = 0;
    VOID* pMem  = NULL;
    VOID* pBase = NULL;
    LENGTH_T len = DEFAULT_MD_LEN;

    FILENAME_T fn;
    if (argc >= 3)
    {
        if ( StrToNumber(argv[2], &len) != HI_SUCCESS)
        {
            len = DEFAULT_MD_LEN;
        }

        if (argc == 4)
        {
            strcpy(fn, argv[3]);
        }
        else
        {
            sprintf(fn, "md_%s-%s", argv[1], argv[2]);
        }
    }
    else
    {
        printf("usage: %s <address> <len> [filename]. sample: %s 0x80040000 \n", argv[0], argv[0]);
        EXIT("", -1);
    }
    

    if( StrToNumber(argv[1], &ulAddr) == HI_SUCCESS)
    {
        printf("====dump memory:<%#010lX><%lu> to file:<%s>====\n", 
                        ulAddr,
                        len,
                        fn);
        
        #ifdef PC_EMULATOR
        #define SHAREFILE "../shm"
        printf("**** is Emulator, use share file : %s ****\n", SHAREFILE);
        pBase  = mmapfile(SHAREFILE , len);
        pMem = pBase ;
        if (NULL == pMem)
        {
            EXIT("Memory Map error.", -1);
        }
        pMem = pBase + ulAddr;
        #else        
        pBase = memmap(ulAddr, len);
        pMem = pBase ;
        if (NULL == pMem)
        {
            EXIT("Memory Map error.", -1);
        }     
        
        #endif
        
        (void)hi_md2file(pMem, len, NULL, fn);
    }
    else
    {
        printf("Please input address like 0x12345\n");
    }

    (void)munmap(pBase, len);
    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
