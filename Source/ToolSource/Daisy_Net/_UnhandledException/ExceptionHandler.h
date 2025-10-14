#ifndef __EXCEPTION_HANDLER_H__
#define __EXCEPTION_HANDLER_H__

#ifndef _DEBUG
void	InitEH( void );
void	_AddErrorMsg( LPCTSTR str, LPCTSTR szHead = "lux-" );
#endif // not debug

void	_AddErrorSeq( int nIdx, int nVal );
void	_AddErrorTick( int nIdx, bool bStop );

#ifdef __DEADLOCK__
void	PrintErrorSeq( void );
#endif	// __DEADLOCK__

#ifdef __BEAST
	#define	ADDERRORMSG_NOHEAD(A)	/##/
#endif

//#if defined(__CLIENT ) || defined(__PATCHCLIENT ) 
//	#ifdef __ERRORSEQ
//	#define ADDERRORSEQ(A,B)	_AddErrorSeq( A, B );
//	#else
//	#define ADDERRORSEQ(A,B)	/##/
//	#endif
//
//	//#if defined(_DEBUG) || defined(__BEAST)
//	#ifdef _DEBUG
//	#define ADDERRORMSG(A)		((void)0)	///##/		// 디버그 모드일땐 무시하게 하자.
//	#define ADDERRORMSG_NOHEAD(A)	((void)0)	///##/	// 릴리즈일때만 함수 실행되게 하자.
//	#else	// _DEBUG
//	#define ADDERRORMSG(A)	_AddErrorMsg( A )	// 릴리즈일때만 함수 실행되게 하자.
//	#define ADDERRORMSG_NOHEAD(A)	_AddErrorMsg( A, "" )	// 릴리즈일때만 함수 실행되게 하자.
//	#endif	// _DEBUG
//#else
//	#ifdef __DEADLOCK__
//	 #define ADDERRORSEQ(A,B)	_AddErrorSeq( A, B );
//	 #define ADDERRORTICK(A,B)	_AddErrorTick( A, B );
//	 #define ADDERRORMSG(A)		/##/		
//	 #define ADDERRORMSG_NOHEAD(A)	/##/	// 릴리즈일때만 함수 실행되게 하자.
//	#else
//	 #define ADDERRORSEQ(A,B)	/##/
//	 #define ADDERRORTICK(A,B)	/##/
//	 #define ADDERRORMSG(A)		/##/		
//	 #define ADDERRORMSG_NOHEAD(A)	/##/	// 릴리즈일때만 함수 실행되게 하자.
//	#endif
//#endif


#endif // include