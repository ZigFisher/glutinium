/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_dbg.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2005/5/30
  Last Modified :
  Function List :
  History       :
  1.Date        : 2005/5/30
    Author      : T41030
    Modification: Created file

******************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "hi.h"
#include "memmap.h"

#define	MAX_ROW	(0x8000000)	 /* =max_len/16 */

/*memory dump*/
HI_RET hi_md(IN VOID* pStart,IN  U32 ulLen,IN FD fd)
{
    if (NULL == pStart)
    {
        return ERR_GEN_INVALID_POINTER;
    }

    size_t wl = write(fd, pStart, ulLen);
    if (wl != ulLen)
    {
        WRITE_LOG_DEBUG("Dump memory error!wish len: %u, dump len:%u\n", ulLen, wl);
    }
    return HI_SUCCESS;
}

/*Memory Dump 2 file*/
HI_RET hi_md2file(IN VOID* pStart,IN  U32 ulLen,
                            IN char* strLabel, 
                            IN char* fn)

{
    if (NULL == fn || NULL == pStart)
    {
        return ERR_GEN_INVALID_POINTER;
    }
    FILE* fMd = fopen(fn, "a");
    if ( NULL == fMd )
    {
        WRITE_LOG_DEBUG("Open File %s to dump memory error.\n", fn);
        return HI_FAILURE;
    }
    if(strLabel)
    {
        fwrite(strLabel, strlen(strLabel), 1, fMd);
    }
    fwrite(pStart, ulLen, 1, fMd);
    fclose(fMd);
    return HI_SUCCESS;
    
}


VOID hi_hexdump(OUT FILE *stream, 
                        IN const void *src, IN size_t len, 
                        IN size_t width)
{
    unsigned int rows, pos, c, i;
    const char *start, *rowpos, *data;

    data = src;
    start = data;
    pos = 0;
    rows = (len % width) == 0 ? len / width : len / width + 1;
    for (i = 0; i < rows; i++) 
    {
        rowpos = data;
        fprintf(stream, "%05x: ", pos);
        do 
        {
            c = *data++ & 0xff;
            if ((size_t)(data - start) <= len) 
            {
                fprintf(stream, " %02x", c);
            } 
            else 
            {
                fprintf(stream, "   ");
            }
        } 
        while(((data - rowpos) % width) != 0);
        
        fprintf(stream, "  |");
        data -= width;
        do 
        {
            c = *data++;
            if (isprint(c) == 0 || c == '\t') 
            {
                c = '.';
            }
            if ((size_t)(data - start) <= len) 
            {
                fprintf(stream, "%c", c);
            } 
            else 
            {
                fprintf(stream, " ");
            }
        } 
        while(((data - rowpos) % width) != 0);
        fprintf(stream, "|\n");
        pos += width;
    }
    fflush(stream);
}

VOID hi_hexdump2(OUT FILE *stream, 
                        IN const void *src, IN size_t len, IN size_t width)
{
    unsigned int c, i, rows;
    const unsigned int *rowpos, *data;

    data = src;
    rows = (len % width) == 0 ? len / width : len / width + 1;

    if (MAX_ROW < rows)
    {
	printf("error:Input param(len) is greater than 2GB!\n");
	return;
    }

    for (i = 0; i < rows; i++) 
    {
        rowpos = data;
        fprintf(stream, "%04x: ", i*0x10);
        do 
        {
            c = *data++;
            fprintf(stream, " %08x", c);
        }while(((data - rowpos) % 4) != 0);
        fprintf(stream, "\n");
    }
    fflush(stream);
}



int hi_shexdump(IN const void *src, IN size_t len, 
                         IN size_t width, IN char *dlim,
                         OUT char *dst)

{
    unsigned int rows, pos, c, i;
    const char *start, *dst_start, *rowpos, *data;

    if (dlim <= dst) 
    {
        return 0;
    }
    dlim--;

    start = data = src;
    dst_start = dst;
    pos = 0;
    rows = (len % width) == 0 ? len / width : len / width + 1;
    for (i = 0; i < rows && dst < dlim; i++) 
    {
        rowpos = data;
        dst += snprintf(dst, dlim - dst, "%05x: ", pos);
        do 
        {
            c = *data++ & 0xff;
            if ((size_t)(data - start) <= len) 
            {
                dst += snprintf(dst, dlim - dst, " %02x", c);
            } 
            else 
            {
                dst += snprintf(dst, dlim - dst, "   ");
            }
        } 
        while(((data - rowpos) % width) != 0);
        dst += snprintf(dst, dlim - dst, "  |");
        data -= width;
        do 
        {
            c = *data++;
            if (isprint(c) == 0 || c == '\t') 
            {
                c = '.';
            }
            if ((size_t)(data - start) <= len) 
            {
                dst += snprintf(dst, dlim - dst, "%c", c);
            } 
            else 
            {
                *dst += ' ';
            }
        } 
        while(((data - rowpos) % width) != 0);
        dst += snprintf(dst, dlim - dst, "|\n");
        pos += width;
    }
    *dst = '\0';

    return dst - dst_start;
}




VOID hi_hexdump_log(IN const void *src, IN size_t len, 
                        IN size_t width)
{
    if ( LOG_GETLEVEL() < LOG_LEVEL_DEBUG2) 
        return;
    
    unsigned int rows, pos, c, i;
    const char *start, *rowpos, *data;

    data = src;
    start = data;
    pos = 0;
    rows = (len % width) == 0 ? len / width : len / width + 1;
    for (i = 0; i < rows; i++) 
    {
        rowpos = data;
        WRITE_LOG_NORMAL("%05x: ", pos);
        do 
        {
            c = *data++ & 0xff;
            if ((size_t)(data - start) <= len) 
            {
                WRITE_LOG_NORMAL(" %02x", c);
            } 
            else 
            {
                WRITE_LOG_NORMAL("   ");
            }
        } 
        while(((data - rowpos) % width) != 0);
        
        WRITE_LOG_NORMAL("  |");
        data -= width;
        do 
        {
            c = *data++;
            if (isprint(c) == 0 || c == '\t') 
            {
                c = '.';
            }
            if ((size_t)(data - start) <= len) 
            {
                WRITE_LOG_NORMAL("%c", c);
            } 
            else 
            {
                WRITE_LOG_NORMAL(" ");
            }
        } 
        while(((data - rowpos) % width) != 0);
        WRITE_LOG_NORMAL("|\n");
        pos += width;
    }
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


