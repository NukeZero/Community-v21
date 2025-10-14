
#ifndef	__FLTHREADCOMMON_H__
#define	__FLTHREADCOMMON_H__


#include "../common/FLCommon.h"

#include <process.h>


typedef unsigned (__stdcall *PTHREAD_START) (void *);

#define chBEGINTHREADEX(psa, cbStack, pfnStartAddr, pvParam, fdwCreate, pdwThreadId) \
	((HANDLE)_beginthreadex(                   \
	(void *)        (psa),                     \
	(unsigned)      (cbStack),                 \
	(PTHREAD_START) (pfnStartAddr),            \
	(void *)        (pvParam),                 \
	(unsigned)      (fdwCreate),               \
	(unsigned *)    (pdwThreadId)))



#define	CLOSE_THREAD( hThread, hClose )	\
	{	\
		if( hThread )	\
		{	\
			SetEvent( hClose );	\
			WaitForSingleObject( hThread, INFINITE );	\
			CloseHandle( hThread );	\
			CloseHandle( hClose );	\
			hThread		= hClose	= (HANDLE)NULL;	\
		}	\
	}






#endif