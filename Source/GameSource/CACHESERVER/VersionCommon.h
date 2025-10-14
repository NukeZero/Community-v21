#ifndef __VERSION_COMMON_H__
#define __VERSION_COMMON_H__
#if !defined( _DEBUG )
	#define	__MAINSERVER
#endif
#define	__VER	19

#if !defined( __MAINSERVER )
	#define __INTERNALSERVER
#endif

#define		__SERVER					// 클라이언트 전용코드를 빌드하지 않기 위한 define

// 네트워크 설정
#define		__NETWORK_MASSIVE


#define		__MAP_SIZE					// CServerdesc사용 때문에



#if	  defined(__INTERNALSERVER)			// 내부 사무실 테스트서버

	#define		__GUILDVOTE				// 길드 투표(TODO 이 아이는 뭔가?)

#elif defined(__MAINSERVER)				// 외부 본섭

#endif	// end - 서버종류별 define 


#endif
