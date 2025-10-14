#ifndef __VERSION_COMMON_H__
#define __VERSION_COMMON_H__
#pragma once

#if !defined( _DEBUG )
#define	__MAINSERVER
#endif
#define	__VER 21


#if !defined( __MAINSERVER )
#define __INTERNALSERVER
#endif


#define __S1108_BACK_END_SYSTEM
#define __VENDOR_1106

#define __SFX_OPT		
#define __CPU_UTILDOWN_060502

// 16차
#if __VER >= 16
#define		__IMPROVE_MAP_SYSTEM			// 향상된 지도 시스템
#define		__GUILD_HOUSE_MIDDLE			// 길드하우스 중형
#define		__BS_ADDOBJATTR_INVISIBLE		// CObj Invisible 에 관한 속성추가   --> 16차 예정 
#define		__BS_EFFECT_LUA					// 오브젝트 상태별 효과연출 ( Lua base ) : CLIENT ONLY!!!!! EVER!!! ABSOLUTE!!!
#define		__BS_ADD_CONTINENT_WEATHER		// 대륙 날씨 추가 ( 온난화로 인한 태양날씨, 다른 어떤이유로든 변하지 않음 )
#define		__BS_CHANGEABLE_WORLD_SEACLOUD	// 변경가능한 월드 바다구름 
#define		__ADDWEAPON_CROSSBOW16			// 16차 추가무기 크로스보우
#define		__SKILL_UI16					// 16차 SKILL UI
#define		__HYPERLINK_ITEM16				// 아이템 링크..
#define		__ENCHANT_BARUNA16				// 신제련 ( 바루나 )
#define		__AGGRO16			//어그로
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
#define ADD_CHARACTER_INFO_DISPLAY
#define COSTUME_UPGRADE_ENHANCEMENT_GEM
#define COSTUME_UPGRADE_MIX
#define ADD_INVENTORY_EDGE
#define ENCHANT_ABSOLUTE_MIX
#define BATTERY_PREMIUM
// 21.2 Version
#define GUILD_WINNER_BUFF
#define BARUNA_UPGRADE_ENHANCEMENT_GEM
#define BARUNA_UPGRADE_SUIT_PIERCING
#define BARUNA_UPGRADE_SMELT_SAFETY
#define INVENTORY_PET_COSTUME42
#endif

#if	  defined(__INTERNALSERVER)	// 내부 사무실 테스트서버

// BEGIN =======================================================================================
// note: 다음 전처리기는 주석처리가 되어있더라도 손상을 입히거나 코드에서 제거되선 안됩니다. ( Only client )
//	#define		__YENV
//	#define		__YENV_WITHOUT_BUMP
//	#define		__Y_INTERFACE_VER3			// 인터페이스 버전 3.0 - Neuz
//	#define		__CSC_UPDATE_WORLD3D		// World3D Object Culling부분 업데이트
//	#define		__FLYFF_INITPAGE_EXT
//	#define		__BS_DEATH_ACTION				// die 상태로 진입시 연출 효과 
//	#define		__BS_CONSOLE
// END =========================================================================================

#ifndef	NO_GAMEGUARD
	#define		NO_GAMEGUARD
#endif

//	#define		__CPU_UTILDOWN_060502		// CPU 사용률 감소작업 TODO 요건 왜 내부만?
	#define		__SLIDE_060502				// 땅으로 꺼지는 문제 수정 TODO 요건 왜 내부만?

	#define		__GUILDVOTE					// 길드 투표 

	#define		__IAOBJ0622					// 상대 목표 좌표 전송	// 월드, 뉴즈
	#define		__YNOTICE_UNI1026			// 공지사항 유니코드 지원
	#define		__SKILL0517					// 스킬 레벨 파라미터

	#define		__TRAFIC_1218				// 서버에서 보낸 패킷 정보
	#define		__Y_HAIR_BUG_FIX			// TODO 이건 테스트 용인가?


	#define		__GLOBAL_COUNT_0705			// CTime::GetTimer TODO 왜 사용안하지?

	#define		__ATTACH_MODEL				// 모델에 다른 모델 붙이기 (날개...)

	#define		__BS_ADJUST_COLLISION		// 충돌 루틴 개선 ( 2009. 07. 28 )

//	#define		__USE_SOUND_LIB_FMOD		// change the FMOD

//	#define		__BS_TEST_MTE

#elif defined(__MAINSERVER)  // 외부 본섭

#endif	// end - 서버종류별 define 

#ifndef NO_GAMEGUARD 
	#define	__NPROTECT_VER	4	
#endif	

#endif // VERSION_COMMON_H

//================================================================================================================================================================
//Note BEGIN
// 다음의 기호는 해당코드가 주석의 의미라는것을 알려줍니다. ( 코드의 간결성을 위해 #define을 사용하지 않습니다 )
// 작업 편의성 및 다른 사용자를 위해 차수에 상관없이 사용됩니다.
//	_SUN_CHECKDATA_	,				gmpbigsun( 20100705 ) : 데이터 검증 및 오류 보완  
//  _SUN_LOCALIZE_WNDSTATUS ,		gmpbigsun( 20100727 ) : Tile을 쓰지않는 윈도우에 대해서 통짜이미지 localizing
//  _SUN_DEFAULT_WINDOW,			gmpbigsun( 20100830 ) : 최초 실행시 창모드로
//  _SUN_SKILLSFX_TO_LUA,			gmpbigsun( 20100928 ) : // skill sfx -> lua ( 루틴개선, XI_SKILL_ 제거 ), PropSkill->sfx필드들 삭제
//	_SUN_PICKING_WITH_WND,			gmpbigsun( 20101006 ) : 오브젝트 피킹 시도시 윈도우검사
//  _SUN_RESPWANINFO_VER8,			gmpbigsun( 20101012 ) : 리스폰 정보 추가( AI )
//	_SUN_JAPAN_HANGAME_UI,			gmpbigsun( 20101021 ) : 일본 한게임 유저 UI기능변경
//	_JIN_SHOP_CART,					flyingjin( 20101012 ) : 17차 장바구니 구현
//	_JIN_ITEM_CANCELLATION,			flyingjin( 20101109 ) : 17차 귀속해제 스크롤
//	_JIN_NEW_INVENTORY,				flyingjin( 201011010 ): 17차 인벤토리 확장
//  _JIN_NEW_BARUNA_PEARCING,		flyingjin( 20101115 ) : 17차 바루나 피어싱
//  _JIN_ITEM_CHARGE,				flyingjin( 20101221 ) : 17차 아이템유료화
//  _SUN_ENCHANT_EFFECT17,			gmpbigsun( 20110116 ) : 17차 신 제련이펙트
//  _JIN_COSTUME_MIX,				flyingjin( 20110119 ) : 17차 코스튬합성
//	_JIN_TELEPORT,					flyingjin( 20110221 ) : 17차 텔레포트 지도
//  _SUN_RIDER_ANIMATION			gmpbigsun( 20110328 ) : 라이더 에니메이션 추가
//  _SUN_RENDER_QUESTEMOTICON		gmpbigsun( 20110414 ) : 퀘스트 이모티콘 렌더( 캐릭터, 미니맵, 전체맵 )
//  _SUN_SEMI_INDOOR				gmpbigsun( 20110414 ) : outdoor 와 같으나 일부 ( light오브젝트같은 )가 인도어처럼 동작
//	_JIN_WORLD_FREETICKET			flyingjin( 20010628 ) : 18차 유료지역 자유이용권
//	_JIN_TREASURE_CHEST				flyingjin( 20010712 ) : 18차 보물상자 아이템
//  _SUN_UPGRADE_WNDMANAGER			gmpbigsun( 20110725 ) : CWndManager 개선
//  _JIN_CHAR_SERVER_MOVEMENT       flyingjin( 20110822 ) : 캐릭터 서버 이전
//	_SUN_CIRCLE_MINIMAP				gmpbigsun( 20111020 ) : 원형 미니맵

//Note END
//================================================================================================================================================================

