
#ifndef __FT_COMMON_H__
#define __FT_COMMON_H__

#ifdef WIN_CE
#include <winbase.h>
#include <mmsystem.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

#if !defined(WIN_CE) && !defined(WIN32)
#include <pthread.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#else
#define pthread_mutex_t CRITICAL_SECTION
#define pthread_mutex_lock EnterCriticalSection
#define pthread_mutex_unlock LeaveCriticalSection
#endif

#include "ft_malloc.h"

#ifdef __cplusplus
extern "C"{
#endif

#define NOT_IMPLEMENT() \
    printf("[%s,%d]Not implement!\n", __FILE__, __LINE__);

#ifdef __cplusplus
}
#endif

#endif // __FT_COMMON_H__

