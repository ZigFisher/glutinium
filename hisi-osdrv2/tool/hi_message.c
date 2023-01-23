#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <stdio.h> 
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> 
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#include "hi.h"
#include "hi_message.h"


DECLARE_GLOBALVAR LOG_QUEUE gLogQueue;
static const char *gppsz_type[] = 
            { 
              " ","[fatal]:", "[error]:", "[warn]:",
              "[info]:", "[debug]:" , "[debug1]:", "[debug2]:"
            };

LOCALFUNC VOID Log_PrintMsg (IO  LOG_QUEUE*  pLogQueue,IN U32 ucLogLevel, 
                            IN char *psz_format,IN va_list _args );

#if defined(MULTI_TASK_LOGQUEUE)                            
LOCALFUNC VOID Log_InitLock(IO LOG_QUEUE*  pLogQueue);
LOCALFUNC S32  Log_Lock(IO LOG_QUEUE* pLogQueue);
LOCALFUNC S32  Log_Unlock(IO LOG_QUEUE* pLogQueue);
LOCALFUNC VOID Log_DestroyLock(IO LOG_QUEUE* pLogQueue);

/*-------- Lock Function , Maybe Replace later ---*/


LOCALFUNC S32 Log_Lock(IO LOG_QUEUE* pLogQueue)
{
    #if defined(OS_LINUX)
    return pthread_mutex_lock(&(pLogQueue->mutex_lock));
    #else
    return 0;
    #endif
}

LOCALFUNC S32 Log_Unlock(IO LOG_QUEUE* pLogQueue)
{
    #if defined(OS_LINUX)
    return pthread_mutex_unlock(&(pLogQueue->mutex_lock));
    #else
    return 0;
    #endif
}

LOCALFUNC VOID Log_InitLock(IO LOG_QUEUE*  pLogQueue)
{
    #if defined(OS_LINUX)
    pthread_mutex_init(&pLogQueue->mutex_lock, NULL);
    #else
    return 0;
    #endif
}
LOCALFUNC VOID Log_DestroyLock(IO LOG_QUEUE* pLogQueue)
{
    Log_Unlock(pLogQueue);
    #if defined(OS_LINUX)
    pthread_mutex_destroy( &pLogQueue->mutex_lock );    
    #else
    #endif
}

#else
#define Log_Lock(p)
#define Log_Unlock(p)
#define Log_InitLock(p)
#define Log_DestroyLock(p)
#endif

/* --------- Message Functions -----*/
HI_RET Log_Create(IO LOG_QUEUE*  pLogQueue, IN U8 minLevel , IN LENGTH_T lenMsg)
{
    DBG_ASSERT(pLogQueue);
    Log_InitLock(pLogQueue);
    pLogQueue->ucMinLevel   = minLevel;
    pLogQueue->lenMsg = lenMsg;
    pLogQueue->pMsg = malloc(lenMsg);
    if (NULL == pLogQueue->pMsg)
    {
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

VOID Log_Destroy(IO  LOG_QUEUE*  pLogQueue )
{
    DBG_ASSERT(pLogQueue);
    free(pLogQueue->pMsg);
    Log_DestroyLock(pLogQueue);
	pLogQueue->pMsg = NULL;
}



VOID Log_Msg(IO  LOG_QUEUE*  pLogQueue,IN U32 ucLogLevel, 
                    IN char *psz_format, ... )
{
    DBG_ASSERT(pLogQueue);
    
    if (pLogQueue->ucMinLevel < ucLogLevel)
    {
        return;
    }
    va_list args;

    va_start( args, psz_format );
    Log_PrintMsg( pLogQueue, ucLogLevel, psz_format, args );
    va_end( args );
}

VOID Log_MsgVa(IO  LOG_QUEUE*  pLogQueue,IN U32 ucLogLevel, 
                      IN char *psz_format, IN va_list args )
{
    Log_PrintMsg( pLogQueue, ucLogLevel, psz_format, args );
}


/* following functions are local */


static VOID Log_PrintMsg (IO  LOG_QUEUE*  pLogQueue,IN U32 ucLogLevel, 
                            IN char *psz_format,IN va_list _args )
{
    
    va_list  args;
    Log_Lock(pLogQueue);
    va_copy( args, _args );
    vsnprintf( pLogQueue->pMsg, pLogQueue->lenMsg, psz_format, args );
    va_end( args );
    pLogQueue->pMsg[ pLogQueue->lenMsg - 1 ] = 0; 
    
    if (ucLogLevel)
        fprintf( STDOUT, "%s %s", gppsz_type[ucLogLevel], pLogQueue->pMsg);
    else
        fprintf( STDOUT, "%s", pLogQueue->pMsg);
    
    Log_Unlock(pLogQueue);
    
    fflush(STDOUT);
    return;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
