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
#include "hi.h"
#include "strfunc.h"

LOCALFUNC HI_RET atoul(IN CHAR *str,OUT U32 * pulValue);
LOCALFUNC HI_RET atoulx(IN CHAR *str,OUT U32 * pulValue);


/*****************************************************************************
 Prototype    : StrToNumber
 Calls        : isdigit
                
 Called  By   : 
                
 History        : 
   Author       : t41030
   Modification : Created function
*****************************************************************************/

HI_RET StrToNumber(IN CHAR *str , OUT U32 * pulValue)
{
    if ( *str == '0' && (*(str+1) == 'x' || *(str+1) == 'X') )
    {
        if (*(str+2) == '\0')
        {
            return HI_FAILURE;
        }
        else
        {
            return atoulx(str+2,pulValue);
        }
    }
    else
    {
        return atoul(str,pulValue);
    }
}

/*****************************************************************************
 Prototype    : atoul
 Calls        : isdigit
                
 Called  By   : 
                
 History        : 
   Author       : t41030
   Modification : Created function
*****************************************************************************/
HI_RET atoul(IN CHAR *str,OUT U32 * pulValue)
{
    U32 ulResult=0;

    while (*str)
    {
        if (isdigit((int)*str))
        {
            if ((ulResult<429496729) || ((ulResult==429496729) && (*str<'6')))
            {
                ulResult = ulResult*10 + (*str)-48;
            }
            else
            {
                *pulValue = ulResult;
                return HI_FAILURE;
            }
        }
        else
        {
            *pulValue=ulResult;
            return HI_FAILURE;
        }
        str++;
    }
    *pulValue=ulResult;
    return HI_SUCCESS;
}



/*****************************************************************************
 Prototype    : atoulx
 Calls        : toupper
                isdigit
                
 Called  By   : 
                
 History        : 
   Author       : t41030
   Modification : Created function
*****************************************************************************/
#define ASC2NUM(ch) (ch - '0')
#define HEXASC2NUM(ch) (ch - 'A' + 10)

HI_RET  atoulx(IN CHAR *str,OUT U32 * pulValue)
{
    U32   ulResult=0;
    UCHAR ch;

    while (*str)
    {
        ch=toupper(*str);
        if (isdigit(ch) || ((ch >= 'A') && (ch <= 'F' )))
        {
            if (ulResult < 0x10000000)
            {
                ulResult = (ulResult << 4) + ((ch<='9')?(ASC2NUM(ch)):(HEXASC2NUM(ch)));
            }
            else
            {
                *pulValue=ulResult;
                return HI_FAILURE;
            }
        }
        else
        {
            *pulValue=ulResult;
            return HI_FAILURE;
        }
        str++;
    }
    
    *pulValue=ulResult;
    return HI_SUCCESS;
}



