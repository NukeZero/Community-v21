#ifndef __VERSION_COMMON__
#define __VERSION_COMMON__
#define	__VER	21

#if !defined( _DEBUG )
	#define	__MAINSERVER
#endif

#if !defined( __MAINSERVER )
	#define __INTERNALSERVER
#endif

#define		__SERVER					// 클라이언트 전용코드를 빌드하지 않기 위한 define

#define		__S1108_BACK_END_SYSTEM		// 백앤드시스템 Trans, World, Neuz TODO CoreServer 미사용
#define		__SKILL_0205				// 스킬 변경 부분만 저장 TODO DatabaseServer만 사용
#define		__VENDOR_1106				// 개인상점 제목 허용 문자 TODO CoreServer 미사용
#define		__LAYER_1015				// 레이어 저장
#define		__PERIN_BUY_BUG				// 페냐 반복구매 버그 확인용 코드 TODO 클라이언트 미사용

#define		DAILY_QUEST

#define		PASSWORD_RESET_2ND

// 16차
#if __VER >= 16
	#define		__GUILD_HOUSE_MIDDLE		// 길드하우스 중형
	#define		__ENCHANT_BARUNA16			// 신제련 ( 바루나 )
#endif

// 20차
#if __VER >= 20
#define	INVENTORY_GENERAL54		// 인벤토리 42->54
#endif


#if __VER >= 21
#define COSTUME_UPGRADE_ENHANCEMENT_GEM
#define BARUNA_UPGRADE_ENHANCEMENT_GEM
#define INVENTORY_PET_COSTUME42
#endif


#if	  defined(__INTERNALSERVER)			// 내부 사무실 테스트서버

	#define		__GUILDVOTE				// 길드 투표 

#elif defined(__MAINSERVER)				// 외부 본섭 

	#define		__IDC

#endif	// end - 서버종류별 define 

#endif