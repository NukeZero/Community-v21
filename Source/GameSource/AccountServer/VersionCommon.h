#ifndef __VERSION_COMMON_H__
#define __VERSION_COMMON_H__
#if !defined( _DEBUG )
	#define	__MAINSERVER
#endif
#define	__VER	19

#if !defined( __MAINSERVER )
	#define __INTERNALSERVER
#endif

// 공통 설정

#define		__SERVER			// 클라이언트 전용코드를 빌드하지 않기 위한 define



#if	  defined(__INTERNALSERVER)	// 내부 사무실 테스트서버


#elif defined(__MAINSERVER)		// 외부 본섭


#endif	// end - 서버종류별 define 

#endif	// __VERSION_COMMON_H__