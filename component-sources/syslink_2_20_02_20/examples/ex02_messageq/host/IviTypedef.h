//=============================================================================
//	THIS SOURCE CODE IS PROPRIETARY INFORMATION BELONGING TO INTERVIDEO, INC.
// 	ANY USE INCLUDING BUT NOT LIMITED TO COPYING OF CODE, CONCEPTS, AND/OR
//	ALGORITHMS IS PROHIBITED EXCEPT WITH EXPRESS WRITTEN PERMISSION BY THE 
//	COMPANY.
//
// 	THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// 	PURPOSE.
//
// 	Copyright (c) 2004 - 2006  InterVideo Corporation.  All Rights Reserved.
//
//-----------------------------------------------------------------------------

#ifndef __IVITYPEDEF_H__
#define __IVITYPEDEF_H__

/*****************************************************************************
 * Basic Types
 *****************************************************************************/
/*----------------------------------------------------------------------------
| For MSVC or ARM/RVDS
*---------------------------------------------------------------------------*/
#if defined(_MSC_VER) || defined(__arm)
	typedef unsigned char	    ivi8u;
	typedef signed char			ivi8s;
	typedef unsigned short		ivi16u;
	typedef signed short		ivi16s;
	typedef unsigned int		ivi32u;
	typedef signed int			ivi32s;
	typedef float				ivi32f;
#if defined(__APPLE__)
    typedef unsigned long long	ivi64u;
    typedef signed long long	ivi64s;
    typedef double				ivi64f;
#else
	typedef unsigned __int64	ivi64u;
	typedef signed __int64		ivi64s;
    typedef double				ivi64f;
#endif
/*----------------------------------------------------------------------------
| For GCC
*---------------------------------------------------------------------------*/
#elif defined(__GNUC__)
	typedef unsigned char		ivi8u;
	typedef signed char			ivi8s;
	typedef unsigned short		ivi16u;
	typedef signed short		ivi16s;
	typedef unsigned int		ivi32u;
	typedef signed int			ivi32s;
	typedef float				ivi32f;
	typedef unsigned long long	ivi64u;
	typedef signed long long	ivi64s;
	typedef double				ivi64f;
/*----------------------------------------------------------------------------
| not supported compiler - This is probably an user who tries to build
| the code in the wrong way.
*---------------------------------------------------------------------------*/
#else
#	error You are trying to compile the code without defining the compiler type.
#endif


/************************************************************************
 * Function return type
 ************************************************************************/
typedef ivi32s iviRet;


/************************************************************************
 * True & false enumerator, compatible with ANSI C 
 ************************************************************************/
typedef enum
{
	iviFalse	= 0,
	iviTrue		= 1
} iviBool;


/************************************************************************
 * For pointers
 ************************************************************************/
#ifndef NULL
#	define NULL		0
#endif


/************************************************************************
 * Inline function declaration
 ************************************************************************/
#if defined(__cplusplus)
#	define IVI_INLINE inline
#elif defined(_MSC_VER)
#	define IVI_INLINE static __forceinline
#elif defined(__GNUC__)
#	define IVI_INLINE static __inline__
#elif defined(__arm) || defined(__ICC) || defined(__ICL) || defined(__ECL)
#	define IVI_INLINE static __inline
#else
#	define IVI_INLINE static
#endif


/************************************************************************
 * Function call declaration
 ************************************************************************/
#if defined(_WIN32) || defined(_WIN64)
#	define __STDCALL  __stdcall
#	define __CDECL    __cdecl
#else
#	define __STDCALL
#	define __CDECL
#endif


/************************************************************************
* Get current time in milliseconds
* IVI_GETTIME	-- Enable/Disable this function
************************************************************************/
#ifndef IVI_GETTIME
#define IVI_GETTIME
#endif

#ifndef IVI_GETTIME

	/* non-defined branch, return ZERO */
	IVI_INLINE ivi32u __CDECL IviGetTime() { return 0; }

#else	/* IVI_GETTIME */

#	if defined(_WIN32_WCE) || defined(_WIN32) || defined(_WIN64)
#		include <Windows.h>
#	elif defined(__GNUC__)
#		include <sys/time.h>
#	endif

	IVI_INLINE ivi32u __CDECL IviGetTime()
	{
#	if defined(_WIN32_WCE) || defined(_WIN32) || defined(_WIN64)

		return GetTickCount();

#	elif defined(__GNUC__)

		struct timeval tv;
		gettimeofday(&tv, 0);	/* Get system time */
		return (tv.tv_sec * 1000 + tv.tv_usec / 1000);

#	else	/* non-defined branch, return ZERO */

		return 0;

#	endif
	}

#endif	/* IVI_GETTIME */


/************************************************************************
* Trace function definition
* IVI_TRACE		-- Enable/Disable this function
* TRACE_FILE	-- Write the trace strings into the file
* TRACE_OUTPUT	-- Write the trace strings into Output window of MSVC
************************************************************************/
#ifndef IVI_TRACE

	/* non-defined branch, empty function */
	IVI_INLINE void __CDECL CITrace(const char *szFormat, ...) {}

#else	/* IVI_TRACE */

#	include <stdio.h>
#	include <stdarg.h>
#	if defined(_WIN32_WCE) || defined(_WIN32) || defined(_WIN64)
#		include <Windows.h>
#	endif

	IVI_INLINE void __CDECL CITrace(const char *szFormat, ...)
	{
		char szBuffer[256]; 
		va_list vl; 

#	if defined(TRACE_FILE)

		FILE* fpTrace;
		va_start(vl, szFormat);
		if (NULL != (fpTrace = fopen(TRACE_FILE, "a")))
		{
			vsnprintf(szBuffer,255,szFormat,vl);
			szBuffer[255]=0;
			fprintf(fpTrace, "%s", szBuffer);
			fclose(fpTrace);
		}

#	elif defined(_WIN32_WCE)

		WCHAR szBufferW[256];
		va_start(vl, szFormat);
		_vsnprintf(szBuffer,255,szFormat,vl);
		szBuffer[255]=0;
		MultiByteToWideChar(CP_ACP,0,szBuffer,-1,szBufferW,255);
		szBufferW[255]=0;
		OutputDebugString(szBufferW);

#	elif ( defined(_WIN32) || defined(_WIN64) ) && defined(TRACE_OUTPUT)

		va_start(vl, szFormat);
		_vsnprintf(szBuffer,255,szFormat,vl);
		szBuffer[255]=0;
		OutputDebugString(szBuffer);

#	else

		va_start(vl, szFormat);
		vsnprintf(szBuffer,255,szFormat,vl);
		szBuffer[255]=0;
		printf(szBuffer);

#	endif

		va_end(vl);
	}
#endif	/* IVI_TRACE */

#define IviTrace CITrace

/************************************************************************
* Lock type and function definition
* ivilock		-- Lock type, may be a CRITICAL_SECTION in Windows or a mutex in Linux
* IviInitLock	-- Initialize the lock
* IviDeleteLock	-- De-initialize the lock
* IviLock		-- lock
* IviUnlock		-- unlock
************************************************************************/
#ifndef IVI_LOCK

	/* non-defined branch, empty function */
	typedef ivi32u ivilock;
	IVI_INLINE void IviInitLock(void* pLock)	{}
	IVI_INLINE void IviDeleteLock(void* pLock)	{}
	IVI_INLINE void IviLock(void* pLock)	{}
	IVI_INLINE void IviUnlock(void* pLock)	{}

#elif defined(_WIN32) || defined(_WIN32_WCE) || defined(_WIN64)

	typedef CRITICAL_SECTION ivilock;

	IVI_INLINE void IviInitLock(void* pLock) {InitializeCriticalSection((LPCRITICAL_SECTION)pLock);}

	IVI_INLINE void IviDeleteLock(void* pLock) {DeleteCriticalSection((LPCRITICAL_SECTION)pLock);}

	IVI_INLINE void IviLock(void* pLock) {EnterCriticalSection((LPCRITICAL_SECTION)pLock);}

	IVI_INLINE void IviUnlock(void* pLock) {LeaveCriticalSection((LPCRITICAL_SECTION)pLock);}


#elif defined(__GNUC__)
#	include <pthread.h>
#   include <assert.h>

	typedef struct tagCRITICAL_SECTION {
		pthread_mutex_t     _mutex;
		pthread_mutexattr_t _mutexattr;
	} ivilock;

	IVI_INLINE void IviInitLock(void* pLock)
	{
		int ret;
		IviLockObject* pMutex = (IviLockObject*)pLock;
		ret = pthread_mutexattr_init(&(pMutex->_mutexattr));
		assert(ret==0);
		pMutex->_mutexattr.__mutexkind = PTHREAD_MUTEX_RECURSIVE_NP;
		ret = pthread_mutex_init(&(pMutex->_mutex),&(pMutex->_mutexattr));
		assert(ret==0); 
	}

	IVI_INLINE void IviDeleteLock(void* pLock)
	{
		int ret;
		IviLockObject* pMutex = (IviLockObject*)pLock;
		ret = pthread_mutex_destroy(&(pMutex->_mutex));
		assert(ret==0);
		ret = pthread_mutexattr_destroy(&(pMutex->_mutexattr));
		assert(ret==0);
	}

	IVI_INLINE void IviLock(void* pLock)
	{
		int ret;
		IviLockObject* pMutex = (IviLockObject*)pLock;
		ret = pthread_mutex_lock(&(pMutex)->_mutex);
		assert(ret==0);
	}

	IVI_INLINE void IviUnlock(void* pLock)
	{
		int ret;
		IviLockObject* pMutex = (IviLockObject*)pLock;
		ret = pthread_mutex_unlock(&(pMutex->_mutex));
		// ret==1 means mutex is owned by another thread!
	}

#elif defined( __arm )	/* not implemented in ADS/RVDS now */

	typedef ivi32u ivilock;
	IVI_INLINE void IviInitLock(void* pLock)	{}
	IVI_INLINE void IviDeleteLock(void* pLock)	{}
	IVI_INLINE void IviLock(void* pLock)	{}
	IVI_INLINE void IviUnlock(void* pLock)	{}

#else
	error You are trying to use IVI_LOCK without defining the compiler type
#endif



#endif /* __IVITYPEDEF_H__ */

