/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : strfunc.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2005/7/27
  Last Modified :
  Description   : String functions
  Function List :
  History       :
  1.Date        : 2005/7/27
    Author      : T41030
    Modification: Created file

******************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include "strfunc.h"

static int atoul(char* str, unsigned int* pulValue);
static int atoulx(char* str, unsigned int* pulValue);


/*****************************************************************************
 Prototype    : StrToNumber
 Description  : convert string to unsigned integer
 Input  args  : IN CHAR *str

 Output args  : U32* pulValue, result value
 Return value : HI_RET  HI_SUCCESS
                        HI_FAILURE
 Calls        : isdigit

 Called  By   :

 History        :
 1.Date         : 2005.07.10
   Author       : t41030
   Modification : Created function
*****************************************************************************/

int StrToNumber(char* str , unsigned int* pulValue)
{
    /* check hex string */
    if ( *str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X') )
    {
        if (*(str + 2) == '\0')
        {
            return -1;
        }
        else
        {
            return atoulx(str + 2, pulValue);
        }
    }
    else
    {
        return atoul(str, pulValue);
    }
}

/*****************************************************************************
 Prototype    : atoul
 Description  : convert decimal string to unsigned integer
 Input  args  : IN CHAR *str, can't support minus symbol
 Output args  : U32* pulValue, result value
 Return value : HI_RET  HI_SUCCESS
                        HI_FAILURE
 Calls        : isdigit

 Called  By   :

 History        :
 1.Date         : 2005.07.10
   Author       : t41030
   Modification : Created function
*****************************************************************************/
static int atoul(char* str, unsigned int* pulValue)
{
    unsigned int ulResult = 0;

    while (*str)
    {
        unsigned char ch = toupper(*str);

        if (isdigit(ch))
        {
            /* max value: 0xFFFFFFFF(4294967295),
               X * 10 + (*str)-48 <= 4294967295
               so, X = 429496729 */
            if ((ulResult < 429496729) || ((ulResult == 429496729) && (*str < '6')))
            {
                ulResult = ulResult * 10 + (*str) - 48;
            }
            else
            {
                *pulValue = ulResult;
                return -1;
            }
        }
        else
        {
            *pulValue = ulResult;
            return -1;
        }
        str++;
    }
    *pulValue = ulResult;
    return 0;
}



/*****************************************************************************
 Prototype    : atoulx
 Description  : Convert HEX string to unsigned integer. Hex string without "0x", ex ABCDE
 Input  args  : IN CHAR *str
 Output args  : U32* pulValue, result value
 Return value : HI_RET  HI_SUCCESS
                        HI_FAILURE
 Calls        : toupper
                isdigit

 Called  By   :

 History        :
 1.Date         : 2005.07.10
   Author       : t41030
   Modification : Created function
*****************************************************************************/
#define ASC2NUM(ch) (ch - '0')
#define HEXASC2NUM(ch) (ch - 'A' + 10)

int  atoulx(char* str, unsigned int* pulValue)
{
    unsigned int   ulResult = 0;
    unsigned char ch;

    while (*str)
    {
        ch = toupper(*str);
        if (isdigit(ch) || ((ch >= 'A') && (ch <= 'F' )))
        {
            if (ulResult < 0x10000000)
            {
                ulResult = (ulResult << 4) + ((ch <= '9') ? (ASC2NUM(ch)) : (HEXASC2NUM(ch)));
            }
            else
            {
                *pulValue = ulResult;
                return -1;
            }
        }
        else
        {
            *pulValue = ulResult;
            return -1;
        }
        str++;
    }

    *pulValue = ulResult;
    return 0;
}



