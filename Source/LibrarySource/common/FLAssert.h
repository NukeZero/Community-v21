#ifndef __FLASSERT_H__
#define __FLASSERT_H__


#include "FLCommon.h"


//////////////////////////////////////////////////////////////////////////
bool	FLAssertFunctionA(	bool		bExp,
							const char*	szExpression,
							const char*	szDescription,
							const char*	szFunction,
							const char*	szFile,
							int			nLine,
							bool*		pIgnoreAlways );

bool	FLAssertFunctionW(	bool			bExp,
							const wchar_t*	szExpression,
							const wchar_t*	szDescription,
							const wchar_t*	szFunction,
							const wchar_t*	szFile,
							int				nLine,
							bool *			pIgnoreAlways );


#if defined(_UNICODE) || defined(UNICODE)
#define FLAssertFunction	FLAssertFunctionW
#else
#define FLAssertFunction	FLAssertFunctionA
#endif
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
typedef	bool (__cdecl *FLAssertHooker)( const wchar_t* szExpression,
									   const wchar_t* szDescription,
									   const wchar_t* szFunction,
									   const wchar_t* szFile,
									   int nLine,
									   bool* pIgnoreAlways );

FLAssertHooker	FLSetAssertHooker( FLAssertHooker pHooker );
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
#define FLASSERT_IMPL( bExp, szDescription )					\
	{															\
		static bool	bIgnoreAlways = false;						\
		if( bIgnoreAlways == false )							\
		{														\
			if( FLAssertFunction( !!(bExp),						\
								_T( #bExp ),					\
								_T( szDescription ),			\
								_T( __FUNCTION__ ),				\
								_T( __FILE__ ),					\
								__LINE__,						\
								&bIgnoreAlways ) == false )		\
			{													\
				::DebugBreak();									\
			}													\
		}														\
	}

//////////////////////////////////////////////////////////////////////////
#ifndef	FLASSERT

#ifdef	FLASSERT_ENABLE
	#define FLASSERT( bExp )	FLASSERT_IMPL( bExp, #bExp )
#else
	#define FLASSERT( bExp )	__noop
#endif	//FLASSERT_ENABLE

#endif	//FLASSERT
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
#ifndef	FLVERIFY

#ifdef	FLASSERT_ENABLE
	#define	FLVERIFY( bExp )	FLASSERT( bExp )
#else
	#define	FLVERIFY( bExp )	((void)(bExp))
#endif	//FLASSERT_ENABLE

#endif	//FLVERIFY
//////////////////////////////////////////////////////////////////////////


#endif
