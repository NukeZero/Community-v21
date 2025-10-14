
#if !defined(AFX_WNDMANAGER_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
#define AFX_WNDMANAGER_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_

#if _MSC_VER > 1000
	#pragma once
#endif 

//---------------------------------------------------------------------------------------------
// 헤더
//---------------------------------------------------------------------------------------------
#pragma region HeaderInclude

#include "rtmessenger.h"

#ifdef __GUILD_HOUSE_MIDDLE
	#include "WndHousing.h"
#endif 


#ifdef __ENCHANT_BARUNA16
	#include "WndEnchantBaruna.h"
#endif 

#include "WndPvp.h"						// PVP
#include "WndLord.h"					// 12차 군주
#include "WndParty.h"					// 파티
#include "WndBank.h"					// 은행
#include "WndHelp.h"					// 도움말
#include "WndGuild.h"					// 길드
#include "WndWorld.h"					// 월드
#include "WndQuest.h"					// 퀘스트
#include "WndExtra.h"			
#include "WndPetRes.h"			
#include "WndPetSys.h"			
#include "WndRankWar.h"
#include "WndSealChar.h"				// 11차 케릭터 봉인 거래 기능
#include "WndCommItem.h"
#include "WndRankInfo.h"
#include "WndPiercing.h"				// 피어싱
#include "WndSelectCh.h"
#include "WndRoomList.h"				// 13차 하우징
#include "WndQuitRoom.h"				
#include "WndMiniGame.h"				// 10차 미니게임
#include "WndRankGuild.h"				// 길드순위
#include "WndGuildBank.h"				// 길드은향
#include "WndAwakening.h"				// 11차 각성, 축복
#include "WndLvReqDown.h"
#include "FLWndCostume.h"				// 코스튬
#include "WndQuizEvent.h"				// 퀴즈 이벤트
#include "WndRepairItem.h"				// 수리창
#include "WndCollecting.h"				// 채집
#include "WndChangeName.h"				// 이름변경
#include "WndElldinsJar.h"				// 19차 엘딘의 항아리
#include "WndSecretRoom.h"				// 12차 비밀의 방
#include "WndRainbowRace.h"				// 13차 레인보우 레이스
#include "WndUpgradebase.h"
#include "WndSummonAngel.h"				// 8차 엔젤 소환
#include "WndMadrigalGift.h"			// 마드리갈의 선물
#include "WndQuestQuickInfo.h"			// 퀘스트 빠른 정보
#include "FLWndCancellation.h"			// 귀속해제
#include "WndBlessingCancel.h"	
#include "FLWndTeleporterMng.h"			// 텔레포트 지도
#include "FLWndTreasurechest.h"			// 보물상자
#include "WndGuildWarRequest.h"
#include "WndInvenRemoveItem.h"
#include "WndGuildCombat1to1.h"			// 11차 일대일 길드 대전
#include "WndCommonInputCount.h"		// 공용 개수 입력 메시지 박스
#include "WndCommonItemExchange.h"		// 범용 아이템 교환 
#include "FLWndInventorySlotProc.h"		// 인벤토리 확장 
#include "WndChattingBlockingList.h"
#include "WndGuildWarPeaceConfirm.h"
#include "FLWndCharServerMovement.h" 
#ifdef PAT_LOOTOPTION
#include "WndPetLootOption.h"
#endif // PAT_LOOTOPTION

#include "../Neuz/WebBox.h"
#include "../Neuz/WorldMap.h"

#include <Guildwar.h>
#include <EveSchool.h>

#pragma endregion HeaderInclude


//---------------------------------------------------------------------------------------------
// 전방선언
//---------------------------------------------------------------------------------------------
#pragma region ForwardDeclearation

class CWndMap;
class CWndMessage;
class CWndCommand;
class CWndTextBook;
class CWndDonation;
class CWndGHUpkeep;
class CWndAddFriend;
class CWndGHMainMenu;
class CWndDuelResult;
class CWndInstantMsg;
class CWndTextLetter;
class CWndTextScroll;
class CWndScoreTitle;
class CWndReawakening;
class CWndMessageNote;
class CWndDuelConfirm;
class CWndPartyConfirm;
class CWndMessengerNote;
class CWndFriendConFirm;
class CWndSelectVillage;
class CWndDonationRanking;
class CWndPartyChangeName;
class CWndPartyLeaveConfirm;
class CWndCloseExistingConnection;
class CWndColosseumModalBox;
class CWndColosseumJoin;
class CWndColosseumRetry;
class CWndColosseumRanking;
class CWndColosseumCleared;
class CWndColosseumModelessBox;
class CWndColosseumReadyToStart;
class CFLWndCostume_NPC;
class CFLWndCancellation;
class CFLWndTeleporterMng;
class CWndEArenaTeam;
class CWndEArenaUserMessage_Summons;
class CWndEArenaReferee;
class CWndEArenaTeamMng;
class CWndEArenaWinner;
class CWndConsignmentMarket;
class CWndComposeTwohandWeapon;
class CWndCancel_ComposeWeapon;
class CWndChangeLook;
class CWndTargetInfo_HUD;
class CFLWndTreasurechest_Open;
class CFLWndTreasurechest_Result;
class CFLWndCharServerMovement_Warning;
class CFLWndCharServerMovement_Select;
class CWndPartyUpgradeScroll;

#pragma endregion ForwardDeclearation


//---------------------------------------------------------------------------------------------
// 정의
//---------------------------------------------------------------------------------------------
#pragma region Definition

#define DECLAREAPPLET( AddMain_Func, AllocClass) CWndBase* AddMain_Func() { return AllocClass; }
#define REG_VERSION				1
#define MAX_SKILL				16
#define WID_MESSAGEBOX			10
#define CLIENT_WIDTH			FULLSCREEN_WIDTH
#define CLIENT_HEIGHT			FULLSCREEN_HEIGHT


// 채팅 스타일
#define CHATSTY_GENERAL   		0x0001
#define CHATSTY_WHISPER   		0x0002
#define CHATSTY_SHOUT     		0x0004
#define CHATSTY_PARTY     		0x0008
#define CHATSTY_GUILD     		0x0010
#define CHATSTY_ALL				0x0020

#define CHATSTY_HELP          	0x0060
#define CHATSTY_HELP_CLIENT   	0x0020
#define CHATSTY_HELP_CHAT     	0x0040
#define CHATSTY_SYSTEM        	0x0180
#define CHATSTY_SYSTEM_CLIENT 	0x0080
#define CHATSTY_SYSTEM_CHAT   	0x0100
#define CHATSTY_GAME          	0x0600
#define CHATSTY_GAME_CLIENT   	0x0200
#define CHATSTY_GAME_CHAT     	0x0400

#define PS_USE_MACRO			0x00000001
#define PS_NOT_MACRO			0x00000002
#define PS_REALADDING_CHATWND	0x00000004		//gmpbigsun: 채팅창에 실제로 들어가는 녀석

typedef CMap<DWORD, DWORD, void *, void *> CMapDWordToPtr;

#pragma endregion Definition


//---------------------------------------------------------------------------------------------
// 구조체
//---------------------------------------------------------------------------------------------
#pragma region Structure

typedef struct tagWNDREGINFO
{
	DWORD		dwWndId;
	CRect		rect;
	BOOL		bOpen;
	DWORD		dwVersion;
	DWORD		dwWndSize;
	DWORD		dwSize;
	BYTE*		lpArchive;
} WNDREGINFO,* LPWNDREGINFO;


struct AppletFunc
{
	CWndBase*	(*m_pFunc)();
	DWORD		m_dwIdApplet;
	LPCTSTR		m_pAppletName;
	LPCTSTR		m_pAppletDesc;
	LPCTSTR		m_pszIconName;
	CHAR		m_cHotkey;
};


struct ToolTipItemTextColor
{
	DWORD dwName0;					// 이름 0단계
	DWORD dwName1;					// 이름 1단계
	DWORD dwName2;					// 이름 2단계
	DWORD dwName3;					// 이름 3단계
	DWORD dwName4;					// 이름 4단계
	DWORD dwName5;					// 이름 5단계
	DWORD dwGeneral;				// 일반
	DWORD dwGeneralOption;			// 일반 옵션
	DWORD dwPiercing;				// 피어싱
	DWORD dwPlusOption;				// 플러스 옵션
	DWORD dwResist;					// 속성 일반
	DWORD dwResistFire;				// 속성 불
	DWORD dwResistWater;			// 속성 물
	DWORD dwResistEarth;			// 속성 땅
	DWORD dwResistElectricity;		// 속성 전기
	DWORD dwResistWind;				// 속성 바람
	DWORD dwResistSM;				// 속성 상용화 무기
	DWORD dwResistSM1;				// 속성 상용화 방어구
	DWORD dwTime;					// 시간 및 효과
	DWORD dwEffective0;				// 아이템 효능
	DWORD dwEffective1;				// 아이템 효능1
	DWORD dwEffective2;				// 아이템 효능2
	DWORD dwEffective3;				// 아이템 효능3
	DWORD dwRandomOption;			// 랜덤 옵션
	DWORD dwEnchantOption;			// 인첸트 옵션
	DWORD dwSetName;				// 세트 이름
	DWORD dwSetItem0;				// 세트 목록(비착용시)
	DWORD dwSetItem1;				// 세트 목록(착용시)
	DWORD dwSetEffect;				// 세트 효과
	DWORD dwGold;					// 가격
	DWORD dwCommand;				// 설명
	DWORD dwNotUse;					// 사용못함
	DWORD dwAddedOpt1;				//무기추가옵션1
	DWORD dwAddedOpt2;				//무기추가옵션2
	DWORD dwAddedOpt3;				//무기추가옵션3
	DWORD dwAddedOpt4;				//무기추가옵션4
	DWORD dwAddedOpt5;				//무기추가옵션5
	DWORD dwAddedOpt6;				//무기추가옵션6
	DWORD dwAddedOpt7;				//무기추가옵션7
	DWORD dwAddedOpt8;				//무기추가옵션8
	DWORD dwAddedOpt9;				//무기추가옵션9
	DWORD dwAwakening;				// 아이템 각성
	DWORD dwBlessing;				// 여신의 축복
	DWORD dwBlessingWarning;		// 여신의 축복 경고
	DWORD dwBarunaRunePiercing;		// 바루나 룬 피어싱 
};

#pragma endregion Structure


//---------------------------------------------------------------------------------------------
// 윈도우 메니져
//---------------------------------------------------------------------------------------------
#pragma region WindowManager

class CWndMgr : public CWndBase
{ 

	friend CWndApplet;


public:


	// 열거형
	enum 
	{
		FIRST_TC,
		SECOND_TC,
		THIRD_TC,
		MAX_TC
	};

	enum
	{ 
		BANNING_POINT							= 5,
		BANNING_2_POINT 						= 2 
	};
	
	enum
	{ 
		WARNING_MILLISECOND						= 700, 
		WARNING_2_MILLISECOND					= 1000, 
		SHORTCUT_WARNING_MILLISECOND			= 3000, 
		BANNING_MILLISECOND						= 180000
	};

	enum
	{ 
		TOOL_TIP_SWITCH_MAIN 					= 0, 
		TOOL_TIP_SWITCH_SUB1 					= 1, 
		TOOL_TIP_SWITCH_SUB2 					= 2
	};



public:



												CWndMgr( void );
	virtual										~CWndMgr( void );


	HRESULT 									InvalidateDeviceObjects( void );
	HRESULT 									DeleteDeviceObjects( void );
	HRESULT										RestoreDeviceObjects( void );

	// 윈도우 메시지
	void										AlighWindow( CRect rcOld, CRect rcNew );
	virtual	BOOL								Initialize( CWndBase* pWndParent = NULL );
	virtual	void								OnInitialUpdate( void );
	virtual void								OnDraw( C2DRender* p2DRender );
	virtual	void								OnDestroyChildWnd( CWndBase* pWndChild );
	virtual	void								OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	virtual void								OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags );
	virtual	void								OnSize( UINT nType, int cx, int cy );
	virtual BOOL    							OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
	virtual	BOOL								OnEraseBkgnd( C2DRender* p2DRender );
	virtual	BOOL								OnChildNotify( UINT message,UINT nID,LRESULT* pLResult );
	virtual	BOOL								OnCommand( UINT nID,DWORD dwMessage );
	virtual	BOOL    							Process( void );


	BOOL										IsShortcutCommand( void ) const;
	void 										SetWarningCounter( int nWarningCounter );
	void 										SetWarning2Counter( int nWarning2Counter );
	int 										GetWarningCounter( void ) const;
	int 										GetWarning2Counter( void ) const;
	CTimer& 									GetWarningTimer( void );
	CTimer& 									GetWarning2Timer( void );
	CTimer& 									GetShortcutWarningTimer( void );
	CTimer& 									GetBanningTimer( void );


	// 퀘스트
	void 										OpenQuestItemInfo( CWndBase* pWndParent = NULL, FLItemBase* pItemBase = NULL );
	void 										ChangeQuestItemInfo( FLItemBase* pItemBase = NULL );


	BOOL										IsConsignmentMarket_Register( void );									// 중개거래 중 등록을 이용중인가?
	BOOL										DoModalConsignmentMarket_Register( DWORD dwItemId );					// 해당 아이템으로 입력창을 띄워라


	void 										OpenTextBook( CWndBase* pWndParent = NULL, FLItemBase* pItemBase = NULL);
	void 										ChangeTextBook( FLItemBase* pItemBase = NULL );
	void 										OpenTextScroll( CWndBase* pWndParent = NULL, FLItemBase* pItemBase = NULL);
	void 										ChangeTextScroll( FLItemBase* pItemBase = NULL );
	void 										OpenTextLetter( CWndBase* pWndParent = NULL, FLItemBase* pItemBase = NULL);
	void 										ChangeTextLetter( FLItemBase* pItemBase = NULL );


	// 채팅
	void 										ParsingChat( CString string );
	void 										WordChange( CString& rString );
	void 										PutDefinedString( DWORD dwText, ... );
	void 										PutDestParam( DWORD dwDst1, DWORD dwDst2, DWORD dwAdj1, DWORD dwAdj2, CEditString &str );	
	void 										PutString( LPCTSTR lpszString, CObj* pObj = NULL, DWORD dwColor = 0xffffffff, DWORD dwChatStyle = CHATSTY_GAME, DWORD dwPStyle = 0x00000001 );


	BOOL										OpenMenu( void );
	void										OpenField( void );
	BOOL										OpenTaskBar( void );
	CWndMap*									OpenMap( LPCTSTR lpszMapFileName );


	void										OpenTitle( BOOL bFirstTime = FALSE );
	BOOL										IsTitle( void )				{	return m_bTitle;		}

	// 채집	
	CCollectingWnd*								OpenCollecting( void );
	BOOL										CloseCollecting(void);


	void										UpdateContentsByBuff( WORD wType, WORD wID );


	void										ObjectExecutor( DWORD dwShortcut, DWORD dwId, DWORD dwType = 0 );
	void										ObjectExecutor( LPSHORTCUT lpShortcut );
	void										UseSkillShortCut( DWORD dwType, DWORD dwSkillIdx );


	// 메신져
	CWndMessage*								GetMessage( LPCTSTR lpszFrom );
	CWndMessage*								OpenMessage( LPCTSTR lpszFrom );
	CWndInstantMsg*								GetInstantMsg( LPCTSTR lpszFrom );
	CWndInstantMsg*								OpenInstantMsg( LPCTSTR lpszFrom );
	BOOL										UpdateMessage( LPCTSTR pszOld, LPCTSTR pszNew );


	// 메시지 박스
	BOOL										OpenCustomBox( LPCTSTR strMessage, CWndMessageBox* pWndMessageBox, CWndBase* pWndParent = NULL );
	BOOL										OpenMessageBox( LPCTSTR strMessage, UINT nType = MB_OK, CWndBase* pWndParent = NULL );
	BOOL    									OpenMessageBoxUpper( LPCTSTR lpszMessage, UINT nType = MB_OK, BOOL bPostLogoutMsg = FALSE );
	BOOL										OpenMessageBoxWithTitle( LPCTSTR lpszMessage, const CString& strTitle, UINT nType = MB_OK, CWndBase* pWndParent = NULL );
	void										CloseMessageBox( void );
	CWndMessageBox*								GetMessageBox( void )		{	return 	m_pWndMessageBox;	}


	// 톨팁
	void 										MakeToolTipText( FLItemBase* pItemBase, CEditString& strEdit, int flag = 0 );
	void 										PutToolTip_Troupe( DWORD dwSkill, CPoint point, CRect* pRect );
	void 										PutToolTip_Item( FLItemBase* pItemBase, CPoint point, CRect* pRect, int flag = 0 );
	void 										PutToolTip_Item( DWORD dwType, DWORD dwId, CPoint point, CRect* pRect, int flag = 0 );
	void 										PutToolTip_Skill( DWORD dwSkill, DWORD dwLevel, CPoint point, CRect* pRect, BOOL bActive = TRUE );
	void 										PutToolTipItemAndSubToolTip( FLItemBase* pItemBase, CPoint point, CRect* pRect, int nFlag = 0 );
	void 										PutToolTipParts( FLItemBase* pPartsItemBase, CPoint point, CRect* pRect, const int nFlag, const int nSubToolTipFlag );
	void										PutToolTip_Character( int SelectCharacter, CPoint point, CRect* pRect );
	DWORD										PutItemName( FLItemElem* pItemElem, CEditString* pEdit );
	void										PutItemAbilityPiercing( FLItemElem* pItemElem, CEditString* pEdit, DWORD dwColorBuf, int nFlag = 0 );
	void										PutItemResist( FLItemElem* pItemElem, CEditString* pEdit );
	void										PutItemSpeed( FLItemElem* pItemElem, CEditString* pEdit );
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	void										PutItemMinMax( CMover* pMover, FLItemElem* pItemElem, CEditString* pEdit, bool bCostume = FALSE );
#else
	void										PutItemMinMax( CMover* pMover, FLItemElem* pItemElem, CEditString* pEdit );
#endif
	void										PutRandomOpt( FLItemElem* pItemElem, CEditString* pEdit, int nType = 0 );					// 양손무기 합성
	void										PutAwakeningBlessing( FLItemElem* pItemElem, CEditString* pEdit );							// 각성, 축복
	void										PutBubbleTime( CEditString* pEdit );														// 버블 타임 ( 휴식 경험치 )
	void 										PutPiercingOpt( FLItemElem* pItemElem, CEditString* pEdit, int nType = 0 );					// 양손무기 합성
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	void										PutGemOpt( FLItemElem* pItemElem, CEditString* pEdit );
#endif
	void 										PutEnchantOpt( CMover* pMover, FLItemElem* pItemElem, CEditString* pEdit, int flag );
	void 										PutSetItemOpt( CMover* pMover, FLItemElem* pItemElem, CEditString* pEdit );
	void 										PutBaseItemOpt( FLItemElem* pItemElem, CEditString* pEdit );
	void 										PutMedicine( FLItemElem* pItemElem, DWORD dwParam, LONG nParamVal, CEditString* pEdit );
	void 										PutBaseResist( FLItemElem* pItemElem, CEditString* pEdit );
	void 										PutItemGold( CMover* pMover, FLItemElem* pItemElem, CEditString* pEdit, int flag );
	void 										PutCoolTime( CMover* pMover, FLItemElem* pItemElem, CEditString* pEdit );
	void 										PutKeepTime( FLItemElem* pItemElem, CEditString* pEdit );
	void 										PutCommand( FLItemElem* pItemElem, CEditString* pEdit );
	void 										PutEndurance( FLItemElem* pItemElem, CEditString* pEdit, int flag );
	void 										PutSex( CMover* pMover, FLItemElem* pItemElem, CEditString* pEdit );
	void 										PutJob( CMover* pMover, FLItemElem* pItemElem, CEditString* pEdit );
	void 										PutLevel( CMover* pMover, FLItemElem* pItemElem, CEditString* pEdit );
	void 										PutChangedLook( FLItemElem* pItemElem, CEditString* pEdit );
	void 										PutWeapon( FLItemElem* pItemElem, CEditString* pEdit );
	void 										PutAddedOpt( FLItemElem* pItemElem, CEditString* pEdit );									// 9차 전승관련 Clienet
	void 										PutPetInfo( FLItemElem* pItemElem, CEditString* pEdit );
	void 										PutPetFeedPocket( FLItemElem* pItemElem, CEditString* pEdit );
	void 										PutSealChar( FLItemElem* pItemElem, CEditString* pEdit );									// 11차 케릭터 봉인 거래 기능
	void 										PutNeededVis( FLItemElem* pItemElem, CEditString* pEdit );
	void 										PutVisPetInfo( FLItemElem* pItemElem, CEditString* pEdit );
	void 										PutPetKind( FLItemElem* pItemElem, CEditString* pEdit );
	void 										PutBind( FLItemElem* pItemElem, CEditString* pEdit );										// 귀속 여부 출력
	void										PutHitPoint( FLItemElem* pItemElem, CEditString* pEdit );									// 19차 엘딘의 항아리
	void										PutBarunaPearcing( FLItemElem* pItemElem, CEditString* pEdit );								// 바루나 피어싱
	void										PutTHCombinedOption( FLItemElem* pItemElem, CEditString* pEdit );							// 양손무기 합성
	void										PutItemCharge( FLItemElem* pItemElem, CEditString* pEdit );
	void										PutEquipItemText( CEditString* pEdit );


	// 애플릿
	CWndBase*									CreateApplet( DWORD dwIdApplet );
	void										DestroyApplet( void );
	CWndBase*									GetApplet( DWORD dwIdApplet );
	DWORD										GetAppletId( TCHAR* lpszAppletName );
	void										AddAllApplet( void );
	void										AddAppletFunc( LPVOID pAppletFunc, DWORD dwIdApplet, LPCTSTR lpszKeyword, LPCTSTR pszIconName, LPCTSTR lpszAppletDesc = NULL, CHAR cHotkey = 0 );
	AppletFunc*									GetAppletFunc( DWORD dwIdApplet );


	// 윈도우 등록
	LPWNDREGINFO								GetRegInfo( DWORD dwWndId );
	BOOL										LoadRegInfo( LPCTSTR lpszFileName );
	BOOL										SaveRegInfo( LPCTSTR lpszFileName );
	BOOL 										PutRegInfo( DWORD dwWndId, CRect rect, BOOL bOpen );
	BOOL 										PutRegInfo( LPWNDREGINFO lpRegInfo );
	BOOL 										PutRegInfo( CWndNeuz* pWndNeuz, BOOL bOpen );


	// 종료
	void										PreDelete( CWndBase* pBase );					// 등록된 변수를 이용하지 않고 delete하는경우 불러줘야한다.
	void										CloseBoundWindow(void);							// 아이템이 걸려 있거나, 아이템을 조작할 가능성이 있는 모든 창을 강제로 닫는다.
	void										CloseFuncApp( void );
	void										ClearAllWnd( void );
	void										RestoreWnd( void );
	void 										Free( void );


	// 캡쳐
	BOOL										ScreenCapture( void );
	BOOL										SaveJPG( LPCTSTR lpszName );	
	BOOL										SaveBitmap( LPCTSTR lpszName );

	// 커스텀 차일드
	void 										RegisterCustomChild( CWndBase*& pWndChild );	//초기화 및 등록
	void 										DeleteAllCustomChild( void );	


	// HUD
	BOOL 										EnableHUD( BOOL bEnable );						// HUD window ON/OFF


	DWORD										GetDSTStringId( int nDstParam );				// 피어싱 제거 창 개선 및 얼터멋 보석 제거 창 추가
	BOOL										IsDstRate( int nDstParam );


	void 										InitSetItemTextColor( void );
	void 										SetMessengerAutoState( void );
	void										SetPlayer( CMover* pMover );
	BOOL										CheckConfirm( FLItemBase* pItem );
	CWndBase*									GetWndVendorBase( void );
	BOOL 										IsMakingRevivalWnd( void );						// 부활창을 띄워도 되는가? 사실 CWndRevival에 static함수로 지원되는편이 나아보이지만...
	CWndCommonInputCount*						GetWndCommonInputCount( CWndBase* pParentWnd );


#ifdef __HYPERLINK_ITEM16
	DWORD										GetItemTitleTextColor( FLItemElem* pItemElem );
#endif 


protected:

	void										OpenMadrigalGift( void );
	void										OpenQuestQuickInfo( void );	
	
	bool										IsDontDestroy( DWORD dwWndId );
	void										RegisterAllCustomChild( void );							


	void										__HotKeyChange( DWORD dwId, char* pch );
	void										FormalizeChatString( CString & str );


public:


	// 윈도우 변수
	CWndWorld*     								m_pWndWorld;							// 월드
	CWndFaceShop*								m_pWndFaceShop;							// 외형변경
	CWndBeautyShop*								m_pWndBeautyShop;						// 외형변경
	CWndUseCouponConfirm*						m_pWndUseCouponConfirm;					// 쿠폰사용
	CWndSummonAngel*							m_pWndSummonAngel;						// 천사소환
	CWndShop*									m_pWndShop;								// 상점
	CFLWndShop_Cart*							m_pWndShop_Cart;						// 상점 (카트)
	CWndTrade*     								m_pWndTrade;							// 교환
	CWndTradeGold* 								m_pWndTradeGold;						// 교환 (돈)
	CWndConfirmTrade * 							m_pWndConfirmTrade;						// 교환
	CWndTradeConfirm * 							m_pWndTradeConfirm;						// 교환
	CWndBank*	   								m_pWndBank;								// 은행
	CWndGuildBank*								m_pWndGuildBank;						// 길드은행
	CWndPost*									m_pWndPost;								// 우편
	CCollectingWnd*								m_pWndCollecting;						// 채집
	CWndAwakening*								m_pWndAwakening;						// 각성
	CWndDropItem*  								m_pWndDropItem;							// 아이템 드롭
	CWndDropConfirm*  							m_pWndDropConfirm;						// 아이템 드롭
	CWndSmeltJewel*								m_pWndSmeltJewel;						// 보석제련
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	CWndNewSmeltJewel*							m_pWndNewSmeltJewel;						// 보석제련(아이콘)
#endif
	CWndMixJewel*								m_pWndSmeltMixJewel;					// 보석제련
#ifdef COSTUME_UPGRADE_MIX
	CWndItemMix*								m_pWndSmeltItemMix;						// 아이템합성
#endif
	CWndLordTender*								m_pWndLordTender;						// 군주
	CWndLordVote*								m_pWndLordVote;							// 군주
	CWndLordState*								m_pWndLordState;						// 군주
	CWndLordEvent*								m_pWndLordEvent;						// 군주
	CWndLordSkillConfirm*						m_pWndLordSkillConfirm;					// 군주
	CWndLordInfo*								m_pWndLordInfo;							// 군주
	CWndLordRPInfo*								m_pWndLordRPInfo;						// 군주
	CWndSecretRoomInfoMsgBox*					m_pWndSecretRoomMsg;					// 비밀의 방
	CWndSecretRoomSelection*					m_pWndSecretRoomSelection;				// 비밀의 방
	CWndSecretRoomOffer*						m_pWndSecretRoomOffer;					// 비밀의 방
	CWndSecretRoomOfferState*					m_pWndSecretRoomOfferState;				// 비밀의 방
	CWndSecretRoomChangeTaxRate*				m_pWndSecretRoomChangeTaxRate;			// 비밀의 방
	CWndSecretRoomCheckTaxRate*					m_pWndSecretRoomCheckTaxRate;			// 비밀의 방
	CWndSecretRoomBoard*						m_pWndSecretRoomBoard;					// 비밀의 방
	CWndSecretRoomQuick*						m_pWndSecretRoomQuick;					// 비밀의 방
	CWndSmeltSafety*							m_pWndSmeltSafety;
	CWndSmeltSafetyConfirm*						m_pWndSmeltSafetyConfirm;
	CWndQuickList* 								m_pWndQuickList;
	CWndExtraction*								m_pWndExtraction;
	CWndChangeWeapon*							m_pWndChangeWeapon;
	CWndPetRes*    								m_pPetRes;
	CWndPetStatus*								m_pWndPetStatus;
	CWndPetMiracle*								m_pWndPetMiracle;
	CWndChangePetName*							m_pWndChangePetName;	
	CWndFoodConfirm*							m_pWndFoodConfirm;
	CWndPetFoodMill*							m_pWndPetFoodMill;
	CWndPetLifeConfirm*							m_pWndPetLifeConfirm;
	CWndHeroSkillUp*							m_pWndHeroSkillUp;		
	CWndRemovePiercing*							m_pWndRemovePiercing;	
	CWndRemoveJewel*							m_pWndRemoveJewel;		
	CWndPetTransEggs*							m_pWndPetTransEggs;		
	CWndHeavenTower*							m_pWndHeavenTower;	
	CWndChatLog*								m_pWndChatLog;					
	CFLWndChatNotice*							m_pWndChatNotice;
	CWndRemoveAttribute*						m_pWndRemoveAttribute;	
	CWndQueryEquip*								m_pWndQueryEquip;
	CWndSelectVillage*							m_pWndSelectVillage;
	CWndQuestItemWarning*						m_pWndQuestItemWarning;
	CWndChangeName*								m_pWndChangeName;
	CWndUpgradeBase*  							m_pWndUpgradeBase;
	CWndPiercing*	  							m_pWndPiercing;
	CWndSealChar*								m_pWndSealChar;
	CWndSealCharSelect*							m_pWndSealCharSelect;
	CWndSealCharSend*							m_pWndSealCharSend;
	CWndSealCharSet*							m_pWndSealCharSet;
	CWndInvenRemoveItem*						m_pWndInvenRemoveItem;
	CWndCommercialElem*							m_pWndCommerialElem;
	CWndRemoveElem* 							m_pRemoveElem;
	CWndRepairItem* 							m_pWndRepairItem;
	CWndConfirmBank*							m_pWndConfirmBank;
	CWndBankPassword*							m_pWndBankPassword;
	CWndPenaltyPK* 								m_pWndPenaltyPK;
	CWndRankGuild* 								m_pWndRankGuild;
	CWndRankInfo*								m_pWndRankInfo;
	CWndRankWar*								m_pWndRankWar;
	CWndLvReqDown*								m_pWndLvReqDown;
	CFLWndCancellation*							m_pWndCancellation;
	CWndFunnyCoinConfirm*						m_pFunnyCoinConfirm;
	CWndGuildNickName*							m_pWndGuildNickName;
	CWndGuildBankLog*							m_pWndGuildBankLog;		
	CWndGuildConfirm*							m_pWndGuildConfirm;
	CWndGuildWarPeaceConfirm*					m_pWndGuildWarPeaceConfirm;
	CWndGuildWarRequest*						m_pWndGuildWarRequest;
	CWndGuildCombatState*						m_pWndGuildWarState;	
	CWndGuildCombatRank_Person*					m_pWndGuildCombatRanking;				// 길드 대전
	CWndGuildCombat1to1Offer*					m_pWndGuildCombatOffer;					// 길드 대전
	CWndGuildCombatSelection*					m_pWndGuildCombatSelection;				// 길드 대전
	CWndGuildCombatJoinSelection*				m_pWndGuildCombatJoinSelection;			// 길드 대전
	CWndGuildCombatBoard*						m_pWndGuildCombatBoard;					// 길드 대전
	CGuildCombatInfoMessageBox2*				m_pWndGuildCombatInfoMessageBox2;		// 길드 대전
	CGuildCombatInfoMessageBox*					m_pWndGuildCombatInfoMessageBox;		// 길드 대전
	CWndGuildCombatResult*						n_pWndGuildCombatResult;				// 길드 대전
	CWndGuildCombat1to1Selection*				m_pWndGuildCombat1to1Selection;			// 길드 대전
	CWndGuildCombat1to1Offer*					m_pWndGuildCombat1to1Offer;				// 길드 대전
	CWndBlessingCancel*							m_pWndBlessingCancel;
	CWndPetAwakCancel*							m_pWndPetAwakCancel;	
	CWndSelectCh*								m_pWndSelectCh;
	CWndDiceGame*								m_pWndDiceGame;
	CWndPuzzleGame*								m_pWndPuzzleGame;
	CWndFindWordGame*							m_pWndFindWordGame;
	CWndKawiBawiBoGame*							m_pWndKawiBawiBoGame;
	CWndKawiBawiBoGameConfirm*					m_pWndKawiBawiBoGameConfirm;
	CWndRainbowRaceOffer*						m_pWndRainbowRaceOffer;					// 레인보우 레이스
	CWndRainbowRaceInfo*						m_pWndRainbowRaceInfo;					// 레인보우 레이스
	CWndRainbowRaceRule*						m_pWndRainbowRaceRule;					// 레인보우 레이스
	CWndRainbowRaceRanking*						m_pWndRainbowRaceRanking;				// 레인보우 레이스
	CWndRainbowRacePrize*						m_pWndRainbowRacePrize;					// 레인보우 레이스
	CWndRainbowRaceMiniGameButton*				m_pWndRainbowRaceMiniGameButton;		// 레인보우 레이스
	CWndRainbowRaceMiniGame*					m_pWndRainbowRaceMiniGame;				// 레인보우 레이스
	CWndRainbowRaceMiniGameEnd*					m_pWndRainbowRaceMiniGameEnd;			// 레인보우 레이스
	CWndRRMiniGameKawiBawiBo*					m_pWndRRMiniGameKawiBawiBo;				// 레인보우 레이스 (가위, 바위, 보)
	CWndRRMiniGameDice*							m_pWndRRMiniGameDice;					// 레인보우 레이스 (주사위)
	CWndRRMiniGameArithmetic*					m_pWndRRMiniGameArithmetic;				// 레인보우 레이스 (산수)
	CWndRRMiniGameStopWatch*					m_pWndRRMiniGameStopWatch;				// 레인보우 레이스 (스톱워치)
	CWndRRMiniGameTyping*						m_pWndRRMiniGameTyping;					// 레인보우 레이스 (타이핑) 
	CWndRRMiniGameCard*							m_pWndRRMiniGameCard;					// 레인보우 레이스 (카드)
	CWndRRMiniGameLadder*						m_pWndRRMiniGameLadder;					// 레인보우 레이스 순위
	CWndCoupleMessage*							m_pWndCoupleMessage;					// 커플 시스템
	CWndChangeAttribute*						m_pWndChangeAttribute;					// 제련 확장(속성, 일반)
	CWndReSkillWarning*							m_pWndReSkillWarning;	
	CWndFontEdit*								m_pWndFontEdit;
	CWndPartyChangeName*						m_pWndPartyChangeName;
	CWndPartyConfirm*							m_pWndPartyConfirm;
	CWndPartyLeaveConfirm*						m_pWndPartyLeaveConfirm;
	CWndMessageNote*							m_pWndMessageNote;
	CWndMessengerNote*							m_pWndMessengerNote;
	CWndFriendConFirm*							m_pWndFriendConfirm;
	CWndDuelConfirm*							m_pWndDuelConfirm;
	CWndDuelResult*								m_pWndDuelResult;
	CWndAddFriend*								m_pWndAddFriend;
	CWndStateConfirm*							m_pWndStateConfirm;
	CWndCloseExistingConnection*				m_pWndCloseExistingConnection;
	CWndRandomScrollConfirm*					m_pWndRandomScrollConfirm;
	CWndHelpInstant*							m_pWndHelpInstant;
	CWndRoomList*								m_pWndRoomList;							// 하우징 시스템
	CWndQuitRoom*								m_pWndQuitRoom;
	CWndEquipBindConfirm*						m_pWndEquipBindConfirm;
	CWndRestateConfirm*							m_pWndRestateConfirm;
	CWndQuizEventConfirm*						m_pWndQuizEventConfirm;					// 퀴즈 이벤트
	CWndQuizEventQuestionOX*					m_pWndQuizEventQuestionOX;				// 퀴즈 이벤트
	CWndQuizEventQuestion4C*					m_pWndQuizEventQuestion4C;				// 퀴즈 이벤트
	CWndQuizEventButton*						m_pWndQuizEventButton;					// 퀴즈 이벤트
	CWndQuestDetail*							m_pWndQuestDetail;						// 퀘스트 (자세히)
	CWndQuestQuickInfo*							m_pWndQuestQuickInfo;					// 퀘스트 (간단히)
	CWndCampusInvitationConfirm*				m_pWndCampusInvitationConfirm;
	CWndCampusSeveranceConfirm*					m_pWndCampusSeveranceConfirm;
	CWndBuffPetStatus*							m_pWndBuffPetStatus;
	CWndConfirmVis*								m_pWndConfirmVis;
	CWndGHMainMenu*								m_pWndGHMain;
	CWndGHUpkeep*								m_pWndUpkeep;
	CWndSelectAwakeCase*						m_pWndSelectAwakeCase;
	CWndChattingBlockingList*					m_pWndChattingBlockingList;
	CWndColosseumJoin*							m_pWndColosseumJoin;					// 콜로세움
	CWndColosseumModelessBox*					m_pWndColosseumModeless;				// 콜로세움
	CWndColosseumModalBox*						m_pWndColosseumModal;					// 콜로세움
	CWndColosseumCleared*						m_pWndColosseumCleared;					// 콜로세움
	CWndColosseumReadyToStart*					m_pWndColosseumReadyToStart;			// 콜로세움
	CWndColosseumRetry*							m_pWndColosseumRetry;					// 콜로세움
	CWndColosseumRanking*						m_pWndColosseumRanking;					// 콜로세움
	CWndEArenaTeam*								m_pWndEArenaTeam;						// 이벤트 아레나 (유저)
	CWndEArenaUserMessage_Summons*				m_pWndEArenaMessageBox;					// 이벤트 아레나 (소환메시지 박스)
	CWndEArenaWinner*							m_pWndEArenaWinner;						// 이벤트 아레나 (승자패자 표시)
	CWndScoreTitle*								m_pWndScoreTitle;						// 이벤트 아레나 (전광판)
	CWndEArenaReferee*							m_pWndEArenaReferee;					// 이벤트 아레나 (관리자 메인)
	CWndEArenaTeamMng*							m_pWndEArenaMng;						// 이벤트 아레나 (관리자 팀정보)
	CFLWndTreasurechest_Open*					m_pWndTreasurechest;					// 보물상자
	CFLWndTreasurechest_Result*					m_pWndTreasurechest_Result;				// 보물상자
	CFLWndCharServerMovement_Warning*			m_pWndCharServerMovement_Warning;		// 보물상자
	CFLWndCharServerMovement_Select*			m_pWndCharServerMovement_Select;		// 보물상자
	CWndTargetInfo_HUD*							m_pWndTargetHUD;						// 타겟정보
	CFLWndCostume_NPC*							m_pCostume_NPC;							// 코스튬
	CWndDonation*								m_pWndDonation;							// 기부	
	CWndDonationRanking*						m_pWndDonationRanking;					// 기부 순위
	CFLWndTeleporterMng*						m_pWndTeleporterMng;
	CWndConsignmentMarket*						m_pWndProxyMarket;
	CWndComposeTwohandWeapon*					m_pWndComposeTwoWeapon;
	CWndCancel_ComposeWeapon*					m_pWndCancel_CTW;
	CWndChangeLook*								m_pWndChangeLook;						
	CWndQuestItemInfo*							m_pQuestItemInfo;
	CWndTextBook*								m_pWndTextBook;
	CWndTextScroll*								m_pWndTextScroll;
	CWndTextLetter*								m_pWndTextLetter;
	CWndRevival*								m_pWndRevival;
	CWndResurrectionConfirm*					m_pWndResurrectionConfirm;
	CWndReWanted*								m_pReWanted;
	CWndWanted*									m_pWanted;
	CWndLogOutWaitting* 						m_pLogOutWaitting;
	CWndCommItemDlg*							m_pWndCommItemDlg;
	CWndChangeClass1*							m_pWndChangeClass1;
	CWndChangeClass2*							m_pWndChangeClass2;
	CWndCoupon100722*							m_pWndCoupon1000722;
	CWndReawakening*							m_pWndReawakening;						// 각성 재 굴림
	CWndPartyUpgradeScroll*						m_pWndPartyUpgradeScroll;				// 극단 성장의 두루마리
	CWndCommonInputCount*						m_pWndCommonInputCount;

	CWndTaskMenu*								m_pWndMenu;								// 메뉴
	CWndDialog*									m_pWndDialog;							// 다이얼로그	
	CWndCommonItemExchange*						m_pWndCommonItemExchange;				// 범용 아이템 교환
	CWndTaskBar*								m_pWndTaskBar;							// 작업 바
	CWndCommand*   								m_pWndCommand;							// 명령
	CWndMessageBox*								m_pWndMessageBox;						// 메시지 박스
	CWndMessageBoxUpper*						m_pWndMessageBoxUpper;					// 메시지 박스
	CWndBase*      								m_pWndActiveDesktop;
	CWndStatDice*								m_pWndStatDice;
#ifdef PAT_LOOTOPTION
	CWndPetLootOption*							m_pWndPetLootOption;				// 펫 루팅 옵션 박스
#endif // PAT_LOOTOPTION
	
#ifdef CARD_UPGRADE_SYSTEM
	CWndCardUpgrade*							m_pWndCardUpgrade;					// 카드 업그레이드
#endif	// CARD_UPGRADE_SYSTEM

#ifdef __GUILD_HOUSE_MIDDLE
	CWndGuildHouseBid*							m_pWndGHBid;
	CWndGuildHouseNotice*						m_pWndGHNotice;
#endif 

#ifdef __ENCHANT_BARUNA16
	CWndBarunaNPC*								m_pWndBarunaNPC;
#endif 


public:


	BOOL										m_bTitle;
	BOOL										m_bAutoRun;
	BOOL										m_bConnect;
	BOOL										m_clearFlag;
	BOOL										m_bAllAction;
	BOOL										m_bWaitRequestMail;						// 우편 요청을 기다리고 있는가?

	bool										m_bIsOpenLordSkill;						// 군주 스킬이 열렸나?

	DWORD										m_dwSavePlayerTime;
	DWORD										m_dwSkillTime[MAX_SKILL];


	TCHAR										m_szTimerChat[128];
	ToolTipItemTextColor						dwItemColor[MAX_TC];

	CTexturePack 								m_texture;
	CTexturePack 								m_texCommand;
	CTexturePack 								m_texIcon;
	CTexturePack 								m_texWnd;

	CMapStringToPtr 							m_mapMap;
	CMapStringToPtr 							m_mapMessage;
	CMapStringToPtr 							m_mapInstantMsg;

	CMapDWordToPtr								m_mapAppletFunc;
	CMapDWordToPtr  							m_mapWndRegInfo;
	CMapDWordToPtr								m_mapWndApplet;

	std::list<int>									m_tempWndId;							// 8차 'T'키 사용, 상대방 레벨 안보이기, 화면 모든 창 On/Off

	CUIntArray   								m_aChatColor ;
	CUIntArray   								m_aChatStyle ;
	CStringArray 								m_aChatString;

	CEditString									m_ChatString;

	CRTMessenger								m_RTMessenger;

	CTimer										m_timerChat;
	CTimer										m_timerMessenger;

	CPtrArray									m_awndShortCut;


	typedef std::vector< CWndBase** >				vecWndContainer;
	typedef vecWndContainer::iterator			vecWndIter;
	vecWndContainer								m_vecChildWnds;

	int											m_nMaketOpen;

private:


	CString										m_strChatBackup;

	BOOL										m_bShortcutCommand;

	int 										m_nWarningCounter;
	int 										m_nWarning2Counter;

	CTimer										m_timerDobe;
	CTimer 										m_timerBanning;
	CTimer 										m_timerWarning;
	CTimer 										m_timerWarning2;
	CTimer 										m_timerShortcutWarning;

};

#pragma endregion WindowManager


//---------------------------------------------------------------------------------------------
int												_GetHeadQuest( const int nQuest );		
int												_CalcQuestEmotionOffset( const int nHeadQuest, const int nOffset );	

extern CParty									g_Party;
extern CWndMgr									g_WndMng; 
extern CPtrArray								m_wndOrder;
extern CGuildMng								g_GuildMng;
extern CGuildWarMng								g_GuildWarMng;
extern _ERROR_STATE								g_Error_State;
extern CGuildCombat								g_GuildCombatMng;

extern DWORD									FULLSCREEN_WIDTH;
extern DWORD									FULLSCREEN_HEIGHT;

extern void										RenderEnchant( C2DRender* p2DRender, CPoint pt );
extern void										RenderRadar( C2DRender* p2DRender, CPoint pt, DWORD dwValue, DWORD dwDivisor );
//---------------------------------------------------------------------------------------------


#endif 