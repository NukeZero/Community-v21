#ifndef __VERSION_COMMON_H__
#define __VERSION_COMMON_H__

#define	__VER	21

#if !defined( _DEBUG )
	#define	__MAINSERVER
#endif
#if !defined( __MAINSERVER )
	#define __INTERNALSERVER
#endif


#define		D3DDEVICE	NULL

#define		__SERVER					// 클라이언트 전용코드를 빌드하지 않기 위한 define
#define		__X15

#define		__S1108_BACK_END_SYSTEM		// 백앤드시스템 Trans, World, Neuz
#define		__RES0807					// 리스폰 개선
#define		__CPU_UTILDOWN_060502		// CPU 사용률 감소작업 
#define		__SLIDE_060502				// 땅으로 꺼지는 문제 수정 


#define		__VENDOR_1106				// 개인상점 허용 문자

#define		__EVENTLUA_GIFT				// 특정 레벨로 레벨업시 아이템 지급 TODO worldserver만 정의
#define		__EVENTLUA_CHEEREXP			// 루아 이벤트 - 응원 경험치 설정 TODO worldserver만 정의
#define		__EVENTLUA_SPAWN			// 아이템 및 몬스터 스폰 이벤트
#define		__EVENTLUA_KEEPCONNECT		// 누적 접속 아이템 지급 이벤트


#define		__EVENT_MONSTER				// 이벤트 몬스터(WorldServer) TODO worldserver만 정의


#define		__LAYER_1015				// 동적 객체 층
#define		__LAYER_1021				// __LAYER_1015 리스폰


#define		__PERIN_BUY_BUG				// 페냐 반복구매 버그 확인용 코드
#define		__GLOBAL_COUNT_0705			// 타이머가 timeGetTime()을 직접 호출하지 않도록 수정
#define		__EVENT_0117				// 구정 이벤트 TODO worldserver만 정의



// 16차
#if __VER >= 16
	#define		__GUILD_HOUSE_MIDDLE		// 길드하우스 중형
	#define		__MOVER_STATE_EFFECT		// 무버 상태값 변화에 따른 이펙트 적용
	#define		__ENCHANT_BARUNA16			// 신제련 ( 바루나 )
	#define		__HYPERLINK_ITEM16			// 아이템 링크..
	#define		__AGGRO16					// 어그로 추가
	#define		__ADDWEAPON_CROSSBOW16		// 16차 추가무기 크로스보우
#endif

// 20차
#if __VER >= 20

#define METEONYKER_SUBSUMMON		// meteonyker몬스터의 서버몬스터 소환가능
#define	PARTY_WARP			// PartyDungeon.lua에 SetPartyWarp( 1 )로 셋팅 시 파티단위로 워프가능
#define SCRIPT_ENTER_INSTANCEDUNGEON	// 스크립트에서 던전 입장
#define	DAILY_QUEST			// 일일 퀘스트 관련
#define	INVENTORY_GENERAL54		// 인벤토리 42->54
#define	INVENTORY_ITEM_ALIGN		// 인벤토리 아이템 정렬
#define CARD_UPGRADE_SYSTEM			// 카드 업그레이드 시스템 
#define KEYBOARD_SET
//#define PAT_LOOTOPTION			// 펫 옵션추가
//#define	NEW_GUILD_WINLOG		// 새로운 길드대전 승리자
//#define	SKILL_BUFF21			// 스킬버프증가 14->21
//#define BARUNA_ULTIMATE_UPDATE		// 바루나 얼터멋 추가
#define		PASSWORD_RESET_2ND
#endif

#if __VER >= 21
#define PAT_LOOTOPTION			// 펫 옵션추가
#define	SKILL_BUFF21
#define COSTUME_UPGRADE_ENHANCEMENT_GEM
#define COSTUME_UPGRADE_MIX
#define	ENCHANT_ABSOLUTE
#define ENCHANT_ABSOLUTE_MIX
#define BATTERY_PREMIUM
// 21.2 Version
#define GUILD_WINNER_BUFF		// 21차 길드전 버프 테스트
#define BARUNA_UPGRADE_ENHANCEMENT_GEM
#define BARUNA_UPGRADE_SUIT_PIERCING
#define BARUNA_UPGRADE_SMELT_SAFETY
#define INVENTORY_PET_COSTUME42

#endif

#if	  defined(__INTERNALSERVER)	// 내부 사무실 테스트서버

	#define		__GUILDVOTE					// 길드 투표

	#define		__IAOBJ0622					// 상대 목표 좌표 전송	// 월드, 뉴즈
	#define		__SKILL0517

	#define		__Y_HAIR_BUG_FIX
	

	#define		__PERF_0226

#elif defined(__MAINSERVER)	// 외부 본섭
 
	// 해외 국내 공통 정의
	#define		__ON_ERROR
	#define		__IDC

#endif	// end - 서버종류별 define 

// 임시 - 여기다 넣지 말것 
#define		__REMOVE_SCIRPT_060712		 

#endif