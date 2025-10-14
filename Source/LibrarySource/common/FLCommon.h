#ifndef __FLCOMMON_H__
#define __FLCOMMON_H__


#ifndef	WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <tchar.h>


#ifndef	FLASSERT_ENABLE

#ifdef	_DEBUG
	#define	FLASSERT_ENABLE
#endif

#endif

#include "FLAssert.h"
#include "FLCommonTypes.h"


//////////////////////////////////////////////////////////////////////////
#ifndef	_countof
#define	_countof(X)			sizeof( (X) ) / sizeof( (*(X)) )
#endif


#define MAKEINT64(a, b)		((__int64)(((DWORD)((DWORD_PTR)(a) & 0xffffffff)) | ((__int64)((DWORD)((DWORD_PTR)(b) & 0xffffffff))) << 32))

#define LODWORD(l)			((DWORD)((__int64)(l) & 0xffffffff))
#define HIDWORD(l)			((DWORD)((__int64)(l) >> 32))


#define	SAFE_DELETE_CONST(p)		{ if(p) { delete (p); } }
#define SAFE_DELETE(p)				{ if(p) { delete (p); (p)=NULL; } }

#define	SAFE_DELETE_ARRARY_CONST(p)	{ if(p) { delete[] (p); } }
#define SAFE_DELETE_ARRAY(p)		{ if(p) { delete[] (p); (p)=NULL; } }

#define SAFE_RELEASE(p)				{ if(p) { (p)->Release(); (p)=NULL; } }


#define	CLOSE_HANDLE(h)		\
	{	\
		if( h )	\
		{	\
			CloseHandle( h );	\
			( h )	= (HANDLE)NULL;	\
		}	\
	}


#define GET_PLAYER_PAGE( n )		n / MAX_CHARACTER_SLOT
#define	GET_PLAYER_SLOT( n )		n % MAX_CHARACTER_SLOT



#endif
