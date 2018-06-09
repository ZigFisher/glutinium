#include <string.h>
#include "hi.h"


#include "cmdshell.h"



#include "argparser.h"
#include "btools.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define BTOOLS_NAME "btools"
#define BTOOLS_VERSION "ver0.0.1_20121120"

HI_RET print_help_message(int argc, char* argv[]);

LOCALFUNC HI_RET do_cmd_install(int argc, char* argv[]);

HI_RET do_cmd_uninstall(int argc, char* argv[]);

static ARGOpt_t opts[] =
{
    {"h", ARG_TYPE_SINGLE, FALSE, 0, 
            "\tprint help msg\n", NULL},
        
    {"i", ARG_TYPE_SINGLE, FALSE, 0, 
            "\tinstall board tools\n" ,NULL},
        
    {"u", ARG_TYPE_SINGLE, FALSE, 0, 
            "\tuninstall board tools\n",NULL},

    {"V", ARG_TYPE_SINGLE, FALSE, 0, 
            "\tprint version\n", NULL},
       // end add
       
    {"END", ARG_TYPE_END, FALSE, 0, 
            "\tEND\n", NULL}
};

    
static CMD_SHELL_T gBToolsCmds[] =
{
    CMD_SHELL_DEF("himc", CMD_ENABLE, himc,"memory clear")
    CMD_SHELL_DEF("himd", CMD_ENABLE, himd,"memory display (8bit)")
    CMD_SHELL_DEF("himd.l", CMD_ENABLE, himd_l,"memory display (32bit)")
    CMD_SHELL_DEF("himm", CMD_ENABLE, himm,"memory modify")
    CMD_SHELL_DEF("hivd", CMD_ENABLE, hivd,"video dump")    
    CMD_SHELL_DEF("himdb", CMD_ENABLE, himdb,"video dump")    
    CMD_SHELL_DEF("hiddrs", CMD_ENABLE, hiddrs,"ddr statistic")    
    CMD_SHELL_DEF("hil2s", CMD_ENABLE, hil2s,"l2 cache statistic")    
    CMD_SHELL_DEF("hiew", CMD_ENABLE, hiew,"extend device write")
    CMD_SHELL_DEF("hier", CMD_ENABLE, hier,"extend device read")
    CMD_SHELL_DEF("i2c_read", CMD_ENABLE, i2c_read,"i2c device read")
    CMD_SHELL_DEF("i2c_write", CMD_ENABLE, i2c_write,"i2c device read")
   CMD_SHELL_DEF("ssp_read", CMD_ENABLE, ssp_read,"ssp device read")
    CMD_SHELL_DEF("ssp_write", CMD_ENABLE, ssp_write,"ssp device read")
    {NULL, CMD_DISABLE,NULL,0}
};
HI_RET print_help_message(int argc, char* argv[])
{
    int i = 0;
    (void) argv;
    (void) argc;
    
    ARGPrintHelp(opts);
    
    printf("------------------------------------------------------\n");

    while(gBToolsCmds[i].cmdstr != NULL)
    {
        if (gBToolsCmds[i].isEnable == CMD_ENABLE)
        {
            printf(" %-10s : %s\n", gBToolsCmds[i].cmdstr, gBToolsCmds[i].helpstr);
        }
        i++;
    }
        
    return HI_SUCCESS;
}


HI_RET print_version_message(int argc, char* argv[] )
{
    (void) argc;
    (void) argv;
    printf("*** Board tools : %s *** \n",BTOOLS_VERSION);
    return HI_SUCCESS;
}

/*btools install*/
LOCALFUNC HI_RET do_cmd_install(int argc, char* argv[])
{
    (void) argv;
    (void) argc;
    int i = 0;
    int ret = 0;
    while(gBToolsCmds[i].cmdstr)
    {
        WRITE_LOG_INFO("%d:installing <%s>.\n", i, gBToolsCmds[i].cmdstr);
        if ( (ret = symlink(BTOOLS_NAME, gBToolsCmds[i].cmdstr)) == 0)
        {
            //WRITE_LOG_INFO(" install <%s> ok.\n", gBToolsCmds[i].cmdstr);
        }
        else if (ret == EEXIST)
        {
            WRITE_LOG_INFO(" <%s> exist.\n" , gBToolsCmds[i].cmdstr);
        }
        else
        {
            perror(" install error!");            
        }
        i++;
        
    }
    return HI_SUCCESS;
}

/*btools uninstall*/
HI_RET do_cmd_uninstall(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    char sRealFile[MAXPATHLEN];        
    int lenFn = 0;
    int i = 0;
    while(gBToolsCmds[i].cmdstr)
    {
        
        WRITE_LOG_INFO("%d:uninstalling <%s>.\n",i, gBToolsCmds[i].cmdstr);
        memset(sRealFile, 0, MAXPATHLEN);
        
        if ((lenFn = readlink(gBToolsCmds[i].cmdstr, sRealFile, MAXPATHLEN)) > 0)
        {
            //printf(" get link info ,%s len: %d\n",sRealFile, lenFn);
            if (lenFn < MAXPATHLEN)
            {
                sRealFile[lenFn] = 0;
                //WRITE_LOG_INFO(" uninstall <%s> ok.\n",  gBToolsCmds[i].cmdstr);
                if (strcmp(sRealFile, BTOOLS_NAME) == 0)
                {
                    if (unlink(gBToolsCmds[i].cmdstr))
                    {
                        perror(" uninstall error");
                    }                    
                }
                else
                {
                    WRITE_LOG_INFO(" %s not link to me(%s)\n", sRealFile, BTOOLS_NAME);
                }
            }
            else
            {
                WRITE_LOG_ERROR(" buffer is too small, %d\n", lenFn);
            }
        }
        else
        {
            perror(" read link error");
        }
        i++;

    }
    return HI_SUCCESS;
    
}

/*link */  

int main(int argc , char* argv[])
{
    if (HI_SUCCESS != LOG_CREATE(LOG_LEVEL_DEBUG, 2048))
    {
        printf("create logqueue error.\n");
        return -1;
    }
    
    HI_RET hiDo = HI_FAILURE;

    char* pCmdStr = argv[0];
    char* pTmp = NULL;

    /*»•µÙ√¸¡Ó––«∞◊∫*/
	for (pTmp = pCmdStr; *pTmp != '\0';) 
    {
		if (*pTmp++ == '/')
			pCmdStr = pTmp;
	}    

    
    if (strcmp(pCmdStr, BTOOLS_NAME) == 0)
    {

        if ( ARGParser(argc, argv, opts) != HI_SUCCESS)
        {
            (void)print_help_message(argc, argv );
            return HI_SUCCESS;
        }
        if (opts[0].isSet)
        {
            (void)print_help_message(argc, argv );
        }
        else if (opts[1].isSet)
        {
            (void)do_cmd_install(0, 0);
        }
        else if (opts[2].isSet)
        {
            (void)do_cmd_uninstall(0,0);
        }
        else if (opts[3].isSet)
        {
            (void)print_version_message(argc, argv);
        }
        else
        {
            (void)print_help_message(argc, argv );
            return HI_SUCCESS;
        }
        return HI_SUCCESS;
    }

    (void)print_version_message(argc, argv);
    hiDo = (HI_RET)CMD_SHELL_RUN_2(argc, argv, (CMD_SHELL_T*)gBToolsCmds);
    
    if (hiDo != HI_SUCCESS)
    {
        printf("\ndo errro\n");
    }
    printf("[END]\n");
    fflush(stdout);
    return 0;
}


/*
shell

*/
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

