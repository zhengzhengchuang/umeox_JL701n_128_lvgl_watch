/*
* hrv_type_def.h
*/

#ifndef _GOODIX_IOT_SYS_TYPE_DEF_H_
#define _GOODIX_IOT_SYS_TYPE_DEF_H_

#include <stdint.h>
#include <stddef.h>

typedef unsigned char			GBOOL;
typedef          char			GCHAR;
typedef unsigned char			GUCHAR;
typedef signed   char			GS8;
typedef unsigned char			GU8;

typedef signed   short			GS16;
typedef unsigned short			GU16;

typedef signed   int			GS32;
typedef unsigned int			GU32;

typedef signed   long long		GS64;
typedef unsigned long long		GU64;

typedef          float			GF32;
typedef          double			GF64;
typedef long     double			GF128;

/* ��ӡ���� */
#define IOT_SYS_DEBUG 1

#if defined(_WIN32)	|| defined(_WIN64)
#include <malloc.h>
#ifndef IOT_SYS_MALLOC
#define IOT_SYS_MALLOC		malloc
#endif
#ifndef IOT_SYS_FREE
#define IOT_SYS_FREE	            	free
#endif
#ifndef IOT_SYS_DLL_API
#define IOT_SYS_DLL_API		    extern _declspec(dllexport)
#endif

#if IOT_SYS_DEBUG
#include <stdio.h>
#ifndef IOT_SYS_PRINT
#define IOT_SYS_PRINT	            	printf
#endif
#endif

#elif defined(__linux__) || defined(ANDROID)
#include <malloc.h>
#ifndef IOT_SYS_MALLOC
#define IOT_SYS_MALLOC		malloc
#endif
#ifndef IOT_SYS_FREE
#define IOT_SYS_FREE		free
#endif
#ifndef IOT_SYS_DLL_API
#define IOT_SYS_DLL_API
#endif

#ifndef __ANDROID_NATIVE__ // for app
#include <android/log.h>
#ifndef IOT_SYS_LOG_TAG
#define IOT_SYS_LOG_TAG "IoTSys"
#ifndef IOT_SYS_PRINT
#define IOT_SYS_PRINT(...)	__android_log_print(ANDROID_LOG_DEBUG, IOT_SYS_LOG_TAG, __VA_ARGS__)
#endif
#endif
#else // for exe on cell phone
#ifndef IOT_SYS_PRINT
#define IOT_SYS_PRINT		printf
#endif
#endif

#else
/*
* Roma-II
*/
//extern void *GmAlgMemMalloc(GU32 unSize);
//extern void GmAlgMemFree(void *pmemaddr);
//
//#define IOT_SYS_MALLOC(x)		GmAlgMemMalloc((x))
//#define IOT_SYS_FREE(p)			GmAlgMemFree((p))
//#define IOT_SYS_DLL_API

/*
* Cardiff
*/
extern void* goodix_mem_malloc(int32_t size);
extern void goodix_mem_free(void* mem_addr);

#ifndef IOT_SYS_MALLOC
#define IOT_SYS_MALLOC(x)		goodix_mem_malloc((x))
#endif
#ifndef IOT_SYS_FREE
#define IOT_SYS_FREE(p)			goodix_mem_free((p))
#endif
#ifndef IOT_SYS_DLL_API
#define IOT_SYS_DLL_API
#endif

#if IOT_SYS_DEBUG
extern int SlaverLog(const char * lpsbLog, ...);

#define IOT_SYS_LOG_SUPPORT_LEN			128
#define IOT_SYS_LOG_PARAM(...)			do { \
											uint8_t nLogStr[IOT_SYS_LOG_SUPPORT_LEN] = {0}; \
											snprintf(nLogStr, IOT_SYS_LOG_SUPPORT_LEN, \
											"[GH3X2X_ALGOS]: "__VA_ARGS__); \
											SlaverLog(nLogStr); \
										} while(0)

#ifndef IOT_SYS_PRINT
#define IOT_SYS_PRINT		IOT_SYS_LOG_PARAM
#endif
#endif

#endif

// ����궨��
#define SINGLE_PRECISION_SUPPORT	// ����ɿ�ʱʹ�ܣ���VS�����У�Ӱ�� LMS_FILTER_STEP_FACTOR������VS��MCU�Բ���

/********END OF FILE********* (C) COPYRIGHT 2015-2025 .********/
#endif  //_GOODIX_IOT_SYS_TYPE_DEF_H_
