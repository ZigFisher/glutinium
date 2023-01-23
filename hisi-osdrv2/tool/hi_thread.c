/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_thread.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2005/7/13
  Last Modified :
  Description   : Hi Thread
  Function List :
  History       :
  1.Date        : 2005/7/13
    Author      : T41030
    Modification: Created file

******************************************************************************/
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include "hi.h"

#include "hi_thread.h"

/*** Mutex Port Begin **/

INT32 hi_thread_mutex_init(HI_THREAD_MUTEX* pMutex, HI_THREAD_MUTEXATTR* pMutexattr)
{
    return (pthread_mutex_init(pMutex, pMutexattr));
}

INT32 hi_thread_mutex_lock(HI_THREAD_MUTEX* pMutex)
{
    return (pthread_mutex_lock(pMutex));
}

INT32 hi_thread_mutex_trylock(HI_THREAD_MUTEX* pMutex)
{
    return (pthread_mutex_trylock(pMutex));
}

INT32 hi_thread_mutex_unlock(HI_THREAD_MUTEX* pMutex)
{
    return (pthread_mutex_unlock(pMutex));
}

INT32 hi_thread_mutex_destroy(HI_THREAD_MUTEX* pMutex)
{
    return (pthread_mutex_destroy(pMutex));
}

INT32 hi_thread_cond_init(HI_THREAD_COND* pCond, HI_THREAD_CONDATTR* pCondattr)
{
    return pthread_cond_init(pCond, pCondattr);
}

INT32 hi_thread_cond_destroy(HI_THREAD_COND* pCond)
{
    return pthread_cond_destroy(pCond);
}
INT32 hi_thread_cond_broadcast(HI_THREAD_COND* pCond)
{
    return    pthread_cond_broadcast(pCond);
}

INT32 hi_thread_cond_wait(HI_THREAD_COND* pCond, HI_THREAD_MUTEX *pMutex, U32 msecs)
{
    struct timeval now;
    struct timespec timeout;
    
    if (msecs == 0)
    {
        return pthread_cond_wait(pCond, pMutex);
    }
    else
    {
        gettimeofday(&now, NULL);
        timeout.tv_sec = now.tv_sec;
        timeout.tv_nsec = ( now.tv_usec + (msecs * 1000 )) * 1000;
        return pthread_cond_timedwait(pCond, pMutex, &timeout);
    }
        
}


/*** Mutex Port END **/


INT32 hi_thread_join(HI_THREAD_T *pThread, void **thread_return)
{
    return (pthread_join(pThread->threadid, thread_return));
}
INT32 hi_thread_create(HI_THREAD_T *pThread,const pthread_attr_t*__attr,
                      void*__arg)
{
    if (NULL == pThread)
    {
        WRITE_LOG_ERROR("No routine to be run for create thread. \n");        
        return HI_FAILURE;
    }

    if (NULL == pThread->pf_run)
    {
        WRITE_LOG_ERROR("No routine to be run for create thread. \n");
        return HI_FAILURE;
    }
    #if 0
    if (0 == pthread_create(&(pThread->threadid), __attr, pThread->pf_run, __arg))
    {
        WRITE_LOG_DEBUG("thread %s create, id:%u\n", pThread->szThreadName, pThread->threadid);
        return HI_SUCCESS;
    }
    else
    {
        return HI_FAILURE;
    }
    #endif
    
    pThread->args = __arg;
    
    if (0 == pthread_create(&(pThread->threadid), __attr, 
                              hi_thread_run_handle, 
                              (void*)(pThread))
                            )
    {
        WRITE_LOG_DEBUG("thread %s create, threadid:%u.\n", pThread->szThreadName, pThread->threadid);
        return HI_SUCCESS;
    }
    else
    {
        return HI_FAILURE;
    }
    
}

INT32 hi_thread_cancel(HI_THREAD_T *pThread)
{
    if (pThread == NULL)
    {
        return HI_FAILURE;
    }

    WRITE_LOG_DEBUG("cancel thread %s. pid:%d,tid:%d.\n", 
            pThread->szThreadName, pThread->pid, pThread->threadid);
    
    return pthread_cancel(pThread->threadid);
}

VOID* hi_thread_run_handle(VOID* arg)
{
    if (NULL == arg)
    {
        return NULL;
    }
    
    HI_THREAD_T *pThread = (HI_THREAD_T*)arg;
    
    pThread->pid = getpid();
    pThread->threadid = pthread_self();

    WRITE_LOG_DEBUG("thread %s run, pid:%u, threadid:%u\n", 
                    pThread->szThreadName, 
                    pThread->pid,
                    pThread->threadid);

    if (pThread->pf_run)
    {
        return pThread->pf_run(pThread->args);
    }
    else
    {
        return NULL;
    }
}

EXTERNFUNC HI_RET hi_thread_init(HI_THREAD_T *pThread, char* szName, 
                        THREAD_RUN pf, INT32 priority, BOOL bStart)
{
    pThread->pf_run = pf;
    pThread->priority = priority;
    pThread->bStart = bStart;
    strcpy(pThread->szThreadName, szName);
    return HI_SUCCESS;       
}

/* work at 100MHz */
EXTERNFUNC void hi_delay(U32 ulUsDelay)
{
    U32 ulLoop = 0;

    while(ulLoop<ulUsDelay * 10)
    {
        ulLoop++;
    }
    //return HI_SUCCESS;       
}


#if defined(SYNC_USE_COND)
EXTERNFUNC HI_RET hi_sync_create(IO HI_SYNC_T *pWait)
{
    hi_thread_cond_init(&pWait->_cond, NULL);
    hi_thread_mutex_init(&pWait->_mutex, NULL);
    return HI_SUCCESS;
}
EXTERNFUNC HI_RET hi_sync_wait(IO HI_SYNC_T *pWait,IN U32 msec)
{
    return hi_thread_cond_wait(&pWait->_cond, &pWait->_mutex, msec);
}

EXTERNFUNC HI_RET hi_sync_notify(IO HI_SYNC_T *pWait)
{
    return hi_thread_cond_broadcast(&pWait->_cond);
}

EXTERNFUNC HI_RET hi_sync_destroy(IO HI_SYNC_T *pWait)
{
    hi_thread_cond_init(&pWait->_cond, NULL);
    hi_thread_mutex_init(&pWait->_mutex, NULL);
    return HI_SUCCESS;
}
#else
EXTERNFUNC HI_RET hi_sync_create(IO HI_SYNC_T *pWait)
{
    sem_init(&pWait->_sem, 0, 0);
    return HI_SUCCESS;
}
EXTERNFUNC HI_RET hi_sync_wait(IO HI_SYNC_T *pWait,IN U32 msec)
{
    sem_wait(&pWait->_sem);
    return HI_SUCCESS;
}

EXTERNFUNC HI_RET hi_sync_notify(IO HI_SYNC_T *pWait)
{
    sem_post(&pWait->_sem);
    return HI_SUCCESS;
}

EXTERNFUNC HI_RET hi_sync_destroy(IO HI_SYNC_T *pWait)
{
    sem_destroy(&pWait->_sem);
    return HI_SUCCESS;
}

#endif
