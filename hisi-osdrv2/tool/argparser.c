/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : argparser.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2005/7/1
  Last Modified :
  Description   : Argument parser
  Function List :
  History       :
  1.Date        : 2005/7/27
    Author      : T41030
    Modification: Created file

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hi.h"
#include "argparser.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cpluscplus */

#if 0
#define SETVALUE_INT(pValue, szArgv)  (*((INT32*)pValue) = atoi(szArgv)) 
#define SETVALUE_FLOAT(pValue, szArgv) (*((float*)pCurrOpt->pValue) = atof(szArgv))
#define SETVALUE_CHAR(pValue, szArgv) (*((CHAR*)pCurrOpt->pValue) = szArgv[0])
#define SETVALUE_STRING(pValue, szArgv) strcpy(pCurrOpt->pValue, szArgv)
#endif
char ARG_TYPE_STR[ARG_TYPE_END + 1][8] = 
{
    {"NO_OPT"},
    {"CHAR"},
    {"STRING"},
    {"INT"},
    {"FLOAT"},
    {"SINGLE"},
    {"HELP"},        
    {"END"}
};
#if 0
HI_RET _NOWARN_UNUSED ARG_CheckType(ARGOpt_t *pOpt, char* szArgv)
{
    (void) szArgv;
    switch (pOpt->type)
    {
        case ARG_TYPE_INT:
            break;
        case ARG_TYPE_FLOAT:
            break;
        case ARG_TYPE_CHAR:
            break;
        case ARG_TYPE_NO_OPT:   
        case ARG_TYPE_STRING:
            break;
        case ARG_TYPE_END:
            break;
        default:
            return ERR_ARG_UNKOWN_ARG;
    }    
    return HI_SUCCESS;        
}
#endif
HI_RET ARG_SetValue(ARGOpt_t *pOpt, char* szArgv)
{
    switch (pOpt->type)
    {
        case ARG_TYPE_INT:
            *((INT32*)pOpt->pValue) = atoi(szArgv);
            pOpt->isSet = TRUE;
            break;
        case ARG_TYPE_FLOAT:
            *((float*)pOpt->pValue) = (float)atof(szArgv);
            pOpt->isSet = TRUE;
            break;
        case ARG_TYPE_CHAR:
            *((CHAR*)pOpt->pValue) = szArgv[0];
            pOpt->isSet = TRUE;
            break;
        case ARG_TYPE_NO_OPT:   
        case ARG_TYPE_STRING:
            strcpy((char*)pOpt->pValue, szArgv);
            pOpt->isSet = TRUE;
            break;
        case ARG_TYPE_END:
            pOpt->isSet = FALSE;
            break;
        default:
            pOpt->isSet = FALSE;
            WRITE_LOG_ERROR("error opt type: %u\n",pOpt->type );
            return ERR_ARG_UNKOWN_ARG;
    }    
    return HI_SUCCESS;
}
/*
cmd -i 1003 xyz
             ^------------- 
    -i 1003 
     ^   ^    1003 arg
     ^       
     ^------  i    opt 
*/

/*****************************************************************************
 Prototype    : StrToArg
 Description  : Convert string to [argc, argv] pair, the INPUT str had chan-
                ged in this function
 Input        : CHAR* pSrc   
                int* argc    
                CHAR** argv  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2005/7/27
    Author       : T41030
    Modification : Created function

*****************************************************************************/
VOID StrToArg(CHAR* pSrc, int* argc, CHAR** argv)
{
    INT32 iNum = 0;
    BOOL bNewWord = FALSE;
    while(*pSrc)
    {
        if (*pSrc == ' ')
        {
            if (bNewWord == TRUE)
            {
                
                *pSrc = 0;
                bNewWord = FALSE;
            }
        }
        else
        {
            if (bNewWord == FALSE)
            {
                bNewWord = TRUE;
                argv[iNum] = pSrc;
                iNum++;
            }
        }
        pSrc++;
        
    }
    *argc = iNum;

    
}


/*****************************************************************************
 Prototype    : ARGFindOptStr
 Description  : Find option string in option lists, if not find, return
                NULL
 Input        : ARGOpt_t * opts  
                CHAR* strOpt     
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2005/7/27
    Author       : T41030
    Modification : Created function

*****************************************************************************/
static ARGOpt_t *ARGFindOptStr(ARGOpt_t * opts, CHAR* strOpt)
{
    INT32 i = 0;
    while (opts[i].type < ARG_TYPE_END)
    {
        if (strcmp(opts[i].sz_opt, strOpt) == 0 )
        {
            return &opts[i];        
        }
        i++;
    }
    return NULL;
}


static ARGOpt_t *ARGFindOpt_NoArg(ARGOpt_t * opts)
{
    INT32 i = 0;
    while (opts[i].type < ARG_TYPE_END)
    {
        if (opts[i].type == ARG_TYPE_NO_OPT)
        {
            return &opts[i];        
        }
        i++;
    }
    return NULL;
}

VOID ARGClearOpts(ARGOpt_t *opts)
{
    INT32 i = 0;
    while (opts[i].type < ARG_TYPE_END)
    {
        opts[i].isSet = 0;
        i++;
    }

}

HI_RET ARGParser(int argc, char **argv, ARGOpt_t *opts)
{
    HI_RET ret;
    INT32 iArgIdx = 0;
    CHAR* pCurrArg = NULL;
    ARGOpt_t* pCurrOpt = NULL;  

    BOOL  bHasParseArg = FALSE;
    INT32   iNO_OPT_Cnt = 0;
        
    if (argc == 1)
    {
        return HI_SUCCESS;
    }

    //argv[0] is execute command
    for (iArgIdx = 1; iArgIdx < argc ; iArgIdx++)
    {

        pCurrArg = argv[iArgIdx];
        
        if (*pCurrArg == '/' || *pCurrArg == '-')
        {
            pCurrArg++;

            /*the argv[n] as '-' */
            if (*pCurrArg == 0)
            {
                WRITE_LOG_DEBUG("Wrong Args x: %s\n", argv[iArgIdx]);
                return ERR_ARG_WRONG_FMT;
            }
            
            pCurrOpt = ARGFindOptStr(opts, pCurrArg);
            if (pCurrOpt == NULL)
            {
                WRITE_LOG_DEBUG("Wrong Args y: %s\n", argv[iArgIdx]);
                return ERR_ARG_WRONG_FMT;                
            }
            if (pCurrOpt->type == ARG_TYPE_SINGLE)
            {
                bHasParseArg = FALSE;
                pCurrOpt->isSet = TRUE;
            }
            else
            {
                bHasParseArg = TRUE;
            }
            continue;
        }
        else if (bHasParseArg == FALSE)
        {
            pCurrOpt = ARGFindOpt_NoArg(opts);
            if (pCurrOpt == NULL)
            {
                WRITE_LOG_DEBUG("Wrong Args z: %s\n", argv[iArgIdx]);
                return ERR_ARG_NOT_DEFINE_NO_OPT;                
            }           
            bHasParseArg = TRUE;
            iNO_OPT_Cnt++;
            if (iNO_OPT_Cnt >1)
            {
                return ERR_ARG_MULTI_NO_OPT;
            }            
        }

        if (TRUE == bHasParseArg )
        {
            ret = ARG_SetValue(pCurrOpt, argv[iArgIdx]);
            if ( ret != HI_SUCCESS)
            {
                return ret;
            }
            
            bHasParseArg = FALSE;
        }
            
    }
    return HI_SUCCESS;
}

/*return opts[]*/
VOID ARGPrintHelp(ARGOpt_t *opts)
{
    INT32 i = 0;
    while (opts[i].type < ARG_TYPE_END)
    {
        //fprintf(stdout, "-%s\n%s", opts[i].sz_opt,opts[i].pHelpMsg);
        WRITE_LOG_NORMAL("-%s\n%s", opts[i].sz_opt,opts[i].pHelpMsg);
        fflush(stdout);
        i++;
    }
    
}

VOID ARGPrintOpt(ARGOpt_t *pOpt)
{
    switch (pOpt->type)
    {
        case ARG_TYPE_INT:
            WRITE_LOG_DEBUG("opt:%s, argtype:%s, Set:%i, argValue:%i. hlp:%s\n", 
                    pOpt->sz_opt, ARG_TYPE_STR[(int)pOpt->type], 
                    pOpt->isSet, *((INT32*)pOpt->pValue),
                    (char*)pOpt->pHelpMsg);
            break;
        case ARG_TYPE_FLOAT:
            WRITE_LOG_DEBUG("opt:%s, argtype:%s, Set:%u, argValue:%f. hlp:%s\n", 
                    pOpt->sz_opt, ARG_TYPE_STR[(int)pOpt->type], 
                    pOpt->isSet, *((float*)pOpt->pValue),
                    pOpt->pHelpMsg);        
            break;
        case ARG_TYPE_CHAR:
            WRITE_LOG_DEBUG("opt:%s, argtype:%s, Set:%u, argValue:%c. hlp:%s\n", 
                    pOpt->sz_opt, ARG_TYPE_STR[(int)pOpt->type], 
                    pOpt->isSet, *((char*)pOpt->pValue),
                    pOpt->pHelpMsg);               
            break;
        case ARG_TYPE_NO_OPT:
        case ARG_TYPE_STRING:
            WRITE_LOG_DEBUG("opt:%s, argtype:%s, Set:%u, argValue:%s. hlp:%s\n", 
                    pOpt->sz_opt, ARG_TYPE_STR[(int)pOpt->type], 
                    pOpt->isSet, (char*)pOpt->pValue,
                    pOpt->pHelpMsg);           
            break;          
        case ARG_TYPE_SINGLE:
            WRITE_LOG_DEBUG("opt:%s, argtype:%s, Set:%u, hlp:%s\n", 
                    pOpt->sz_opt, 
                    ARG_TYPE_STR[(int)pOpt->type], 
                    pOpt->isSet, 
                    pOpt->pHelpMsg);           
            break;
        case ARG_TYPE_END:
            WRITE_LOG_DEBUG("END\n");              
            break;
        default:
            break;
    }    
}



#if 0
int main(int argc, char* argv[])
{
    float fps = 0;
    int   bitrate = 0;
    char *strName[16], *strFileName[16];
    char  typec = 0;
    memset(strName, 0 ,16);
    memset(strFileName, 0 ,16);
    int width=0;
    #define MAX_TEST_NUM 7
    char cmd[MAX_TEST_NUM][32] ;
    char help = 0;
    
    ARGOpt_t opts[] =
    {
        {"h", ARG_TYPE_SINGLE, FALSE, 0, "help\n", (VOID*)(&help)},
        {"f", ARG_TYPE_FLOAT, FALSE, 0, "set fps\n", (VOID*)(&fps) },
        {"n", ARG_TYPE_STRING, FALSE, 0, "set name\n",(VOID*)strName},
        {"b", ARG_TYPE_INT, FALSE, 0, "bitrate\n", (VOID*)(&bitrate)},
        {"width", ARG_TYPE_INT, FALSE, 0, "width\n", (VOID*)(&width)},
        {"c", ARG_TYPE_CHAR, FALSE, 0, "type char\n", (VOID*)(&typec)},
        {"x", ARG_TYPE_SINGLE, FALSE, 0, "type char\n", (VOID*)(&typec)},
        {"x3", ARG_TYPE_SINGLE, FALSE, 0, "type char\n", (VOID*)(&typec)},
        {"NO_OPT", ARG_TYPE_NO_OPT, FALSE, 0, "NO_OPT fileName\n", (VOID*)(strFileName)},
        {"END", ARG_TYPE_END, FALSE, 0, "END\n", NULL},
    };
    
    sprintf(cmd[0],"cmd1 -b 100 -f 2 -c 3");
    sprintf(cmd[1],"cmd2 -f -x3");
    sprintf(cmd[2],"cmd3");
    sprintf(cmd[3],"cmd4 ddsfs ");
    sprintf(cmd[4],"cmd4 -width 100 -b 2");
    sprintf(cmd[5],"cmd4 -width 100 -b 2 -x UUUU");
    sprintf(cmd[6],"cmd4 -h");

    
    char* Xargv[10];
    //unsigned long Xargv[10];
    int Xargc;
    int i,j;

    #ifdef AUTOTEST
    for(i = 0; i< MAX_TEST_NUM; i++)
    {
        ARGClearOpts(opts);
        printf(">>>>>>>%s\n", cmd[i]);
        StrToArg(cmd[i], &Xargc, Xargv);
        
        for (j = 0; j< Xargc; j++)
        {
            printf("j:%i, argv[%i]:%s\n", j, j, Xargv[j]);
        }
        
        if( ARGParser(Xargc, Xargv, opts) != HI_SUCCESS)
        {
            WRITE_LOG_ERROR("***Wrong***\n");
        }
    #else
        if( ARGParser(argc, argv, opts) != HI_SUCCESS)
        {
            WRITE_LOG_ERROR("***Wrong***\n");
        }
    #endif
    
        if (opts[0].isSet)
        {
            printf("**************************\n");
            ARGPrintHelp(opts);
            #ifdef AUTOTEST
            continue;
            #endif
        }

        j = 0;
        while (opts[j].type < ARG_TYPE_END)
        {
            ARGPrintOpt(&opts[j]);
            j++;
        }
    #ifdef AUTOTEST
    }
    #endif
    getc(stdin);
    return 0;
        
}
#endif

#ifdef __cplusplus
}
#endif /* __cpluscplus */

