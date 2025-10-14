// WndArcane.h: interface for the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDMANAGER_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
#define AFX_WNDMANAGER_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "messenger.h"
#include "WndWorld.h"
#include "WndBank.h"
#include "WndRepairItem.h"	 // 수리창
#include "WndGuildWarPeaceConfirm.h"
#include "WndGuildWarRequest.h"
#include "WndUpgradebase.h"
#include "WndCommItem.h"
#ifdef __NPP_050308
#include "WndPvp.h"
#endif // __NPP_050308

#include "WndPiercing.h"


#define DECLAREAPPLET( AddMain_Func, AllocClass) CWndBase* AddMain_Func() { return AllocClass; }

class CWndMessage;
class CWndInstantMsg;
class CWndCommand;
class CWndMap;
class CWndPartyChangeName;
class CWndPartyConfirm;
class CWndPartyLeaveConfirm;
class CWndFriendConFirm;
class CWndDuelConfirm;
class CWndAddFriend;
class CWndCloseExistingConnection;
class CWndMessageNote;
class CWndMessengerNote;
class CWndTextBook;
class CWndTextScroll;
class CWndTextLetter;
class CWndSelectVillage;
 
#define REG_VERSION 1

#include "WndGuild.h"
#include "WndRankGuild.h"
#include "WndRankInfo.h"
#include "WndRankWar.h"
#include "WndGuildBank.h"

#ifdef __J0519
#include "WndChangeName.h"
#endif	// __J0519

// chat style
#define CHATSTY_GENERAL   0x0001
#define CHATSTY_WHISPER   0x0002
#define CHATSTY_SHOUT     0x0004
#define CHATSTY_PARTY     0x0008
#define CHATSTY_GUILD     0x0010

#define CHATSTY_HELP          0x0060
#define CHATSTY_HELP_CLIENT   0x0020
#define CHATSTY_HELP_CHAT     0x0040
#define CHATSTY_SYSTEM        0x0180
#define CHATSTY_SYSTEM_CLIENT 0x0080
#define CHATSTY_SYSTEM_CHAT   0x0100
#define CHATSTY_GAME          0x0600
#define CHATSTY_GAME_CLIENT   0x0200
#define CHATSTY_GAME_CHAT     0x0400

typedef struct tagWNDREGINFO
{
	DWORD dwWndId;
	CRect rect;
	BOOL  bOpen;
	DWORD dwVersion;
	DWORD dwWndSize;
	DWORD dwSize;
	BYTE* lpArchive;
} WNDREGINFO,* LPWNDREGINFO;

struct AppletFunc
{
	CWndBase* (*m_pFunc)();
	DWORD m_dwIdApplet;
	LPCTSTR m_pAppletName;
	LPCTSTR m_pAppletDesc;
	LPCTSTR m_pszIconName;
	CHAR m_cHotkey;
};

struct ToolTipItemTextColor
{
	DWORD dwName0;				// 이름 0단계
	DWORD dwName1;				// 이름 1단계
	DWORD dwName2;				// 이름 2단계
	DWORD dwName3;				// 이름 3단계
	DWORD dwName4;				// 이름 4단계
	DWORD dwName5;				// 이름 5단계
	DWORD dwGeneral;			// 일반
	DWORD dwPiercing;			// 피어싱
	DWORD dwPlusOption;			// 플러스 옵션
	DWORD dwResist;				// 속성 일반
	DWORD dwResistFire;			// 속성 불
	DWORD dwResistWater;		// 속성 물
	DWORD dwResistEarth;		// 속성 땅
	DWORD dwResistElectricity;	// 속성 전기
	DWORD dwResistWind;			// 속성 바람
	DWORD dwResistSM;			// 속성 상용화 무기
	DWORD dwResistSM1;			// 속성 상용화 방어구
	DWORD dwTime;				// 시간 및 효과
	DWORD dwEffective0;			// 아이템 효능
	DWORD dwEffective1;			// 아이템 효능1
	DWORD dwEffective2;			// 아이템 효능2
	DWORD dwEffective3;			// 아이템 효능3
	DWORD dwRandomOption;		// 랜덤 옵션
	DWORD dwEnchantOption;		// 인첸트 옵션
	DWORD dwSetName;			// 세트 이름
	DWORD dwSetItem0;			// 세트 목록(비착용시)
	DWORD dwSetItem1;			// 세트 목록(착용시)
	DWORD dwSetEffect;			// 세트 효과
	DWORD dwGold;				// 가격
	DWORD dwCommand;			// 설명
	DWORD dwNotUse;				// 사용못함	
};

enum 
{
	FIRST_TC,
	SECOND_TC,
	THIRD_TC,
	MAX_TC
};

class CWndMgr : public CWndBase
{ 
	CString m_strChatBackup;
	CTimer m_timerDobe;

public:
	CStringArray m_aChatString;
	CUIntArray   m_aChatColor ;
	CUIntArray   m_aChatStyle ;
	CMessenger m_Messenger;
	CMapStringToPtr m_mapMap;
	CMapStringToPtr m_mapInstantMsg;
	CMapStringToPtr m_mapMessage;
	CEditString m_ChatString;

#ifdef __S0126_MOTION_AUTORUN
	BOOL m_bAutoRun;
#endif // __S0126_MOTION_AUTORUN
	
	DWORD m_dwSavePlayerTime;
	
	TCHAR m_szTimerChat[ 128 ];
	CTimer m_timerChat;
	
	BOOL   m_bConnect;
	CTimer m_timerMessenger;
	ToolTipItemTextColor	dwItemColor[MAX_TC];

//static AppletFunc  m_appletFunc  [] ; 
	CPtrArray	m_awndShortCut;
	BOOL	m_bTitle        ;
	CMapDWordToPtr	m_mapAppletFunc ;

	void*	GetAppletMainFunc( DWORD dwIdApplet );
	AppletFunc*		GetAppletFunc( DWORD dwIdApplet );
	void	AddAllApplet();

	BOOL ScreenCapture();
	BOOL SaveBitmap( LPCTSTR lpszName );
	void InitSetItemTextColor();

	void Free();

	void SetMessengerAutoState();
public:
	// 윈도 
	//CWndNeuz*         m_pWndNeuz;
	DWORD		   m_dwSkillTime[MAX_SKILL];
	
	CWndBeautyShop* m_pWndBeautyShop;
	CWndDialog*    m_pWndDialog;
	CWndShop*      m_pWndShop;
	CWndBase*      m_pWndActiveDesktop;
	CWndTrade*     m_pWndTrade;
	CWndTradeGold* m_pWndTradeGold;
	CWndDropItem*  m_pWndDropItem;
	CWndConfirmTrade * m_pWndConfirmTrade;
	CWndTradeConfirm * m_pWndTradeConfirm;
	CWndQuickList* m_pWndQuickList;
	CWndCommand*   m_pWndCommand;
	CWndBank*	   m_pWndBank;
	CWndWorld*     m_pWndWorld;
	
#ifdef __Y_POST_0912
	CWndPost* m_pWndPost;
#endif //__Y_POST_0912	

#ifdef __QUERYEQUIP0630	
	CWndQueryEquip* m_pWndQueryEquip;
#endif //__QUERYEQUIP0630

#ifdef __SM_ITEM_2ND
	CWndSelectVillage*		m_pWndSelectVillage;
#endif // __SM_ITEM_2ND

#ifdef __S050504_QUESTITEM
	CWndQuestItemWarning* m_pWndQuestItemWarning;
#endif // __S050504_QUESTITEM
#ifdef __J0519
	CWndChangeName*		m_pWndChangeName;
#endif	// __J0519
//	CWndMessengerSearch*		m_pWndMessengerSearch;

	CWndDropConfirm*  m_pWndDropConfirm;

	CWndUpgradeBase*  m_pWndUpgradeBase;
	
#ifdef __PIERCING_SM_TIME
	CWndPiercing*	  m_pWndPiercing;
#endif //__PIERCING_SM_TIME

	CWndCommercialElem* m_pWndCommerialElem;
	CWndRemoveElem* m_pRemoveElem;
	CWndRepairItem* m_pWndRepairItem;
	CWndConfirmBank* m_pWndConfirmBank;
	CWndBankPassword* m_pWndBankPassword;
	
#ifdef __NPP_050308
	CWndPenaltyPK* m_pWndPenaltyPK;
#endif // __NPP_050308
	
	CWndRankGuild* m_pWndRankGuild;
	CWndRankInfo* m_pWndRankInfo;
	CWndRankWar* m_pWndRankWar;
	CWndGuildBank*	m_pWndGuildBank;
	CWndGuildConfirm* m_pWndGuildConfirm;
	CWndGuildWarPeaceConfirm* m_pWndGuildWarPeaceConfirm;
	CWndGuildWarRequest* m_pWndGuildWarRequest;
	CWndGuildNickName* m_pWndGuildNickName;
	
#ifdef __GUILDCOMBAT	
	CWndGuildCombatRanking* m_pWndGuildCombatRanking;
	CWndGuildWarState* m_pWndGuildWarState;
#endif //__GUILDCOMBAT

	CWndPartyChangeName* m_pWndPartyChangeName;
	CWndPartyConfirm* m_pWndPartyConfirm;
	
	CWndMessageNote* m_pWndMessageNote;
	CWndMessengerNote* m_pWndMessengerNote;

	CWndPartyLeaveConfirm * m_pWndPartyLeaveConfirm;
	CWndFriendConFirm* m_pWndFriendConfirm;
	CWndDuelConfirm* m_pWndDuelConfirm;
	CWndAddFriend* m_pWndAddFriend;
	CWndStateConfirm* m_pWndStateConfirm;
	CWndCloseExistingConnection*	m_pWndCloseExistingConnection;

	// 인터페이스 텍스춰
	CTexturePack  m_texture;
	CTexturePack  m_texCommand;
	CTexturePack  m_texIcon;
	CTexturePack  m_texWnd;

	// 메지시 윈도
	CWndMessageBox*		m_pWndMessageBox;
	CWndMessageBoxUpper*		m_pWndMessageBoxUpper;

	//	퀘스트 아이템 정보
#ifdef	__NQUESTITEM_INFO_040917
	CWndQuestItemInfo*	m_pQuestItemInfo;

	void OpenQuestItemInfo(CWndBase* pWndParent = NULL, CItemBase* pItemBase = NULL);
	void ChangeQuestItemInfo( CItemBase* pItemBase = NULL );
#endif//__NQUESTITEM_INFO_040917

#ifdef __V050322_CACHEQUEST
	CWndTextBook* m_pWndTextBook;
	CWndTextScroll* m_pWndTextScroll;
	CWndTextLetter* m_pWndTextLetter;
	void OpenTextBook     ( CWndBase* pWndParent = NULL, CItemBase* pItemBase = NULL);
	void OpenTextScroll   ( CWndBase* pWndParent = NULL, CItemBase* pItemBase = NULL);
	void OpenTextLetter   ( CWndBase* pWndParent = NULL, CItemBase* pItemBase = NULL);
	void ChangeTextBook   ( CItemBase* pItemBase = NULL );
	void ChangeTextScroll ( CItemBase* pItemBase = NULL );
	void ChangeTextLetter ( CItemBase* pItemBase = NULL );
#endif
	// Field
	CWndTaskBar*	m_pWndTaskBar;
	CWndTaskMenu*	m_pWndMenu;
	CWndRevival*    m_pWndRevival;
#ifdef __NPP_050308
	CWndResurrectionConfirm* m_pWndResurrectionConfirm;
	CWndRecoverChao*    m_pWndRecoverChao;
	CWndReWanted*		m_pReWanted;
	CWndWanted*			m_pWanted;
	CWndLogOutWaitting* m_pLogOutWaitting;
#endif //__NPP_050308	
#ifdef __S050524_SM_ITEM
	CWndCommItemDlg*	m_pWndCommItemDlg;
	CWndChangeClass1*	m_pWndChangeClass1;
	CWndChangeClass2*	m_pWndChangeClass2;
#endif // __S050524_SM_ITEM

	CMapDWordToPtr	m_mapWndApplet ;
	CMapDWordToPtr  m_mapWndRegInfo;

	void PutDefinedString( DWORD dwText, ... );
	void PutString( LPCTSTR lpszString, CObj* pObj = NULL, DWORD dwColor = 0xffffffff, DWORD dwChatStyle = CHATSTY_GAME );
	void ParsingChat( CString string );
	void WordChange( CString& string );

	BOOL	IsTitle()	{	return m_bTitle;	}

	void	OpenTitle( BOOL bFirstTime = FALSE );
	void	OpenField();

	BOOL	OpenTaskBar();
	BOOL	OpenMenu();

	void	ObjectExecutor( DWORD dwShortcut, DWORD dwId, DWORD dwType = 0 );
	void	ObjectExecutor( LPSHORTCUT lpShortcut );
	void	UseSkillShortCut( DWORD dwType, DWORD dwSkillIdx );
		
	// map
	CWndMap* OpenMap( LPCTSTR lpszMapFileName );	

	// massenger
	CWndMessage* GetMessage( LPCTSTR lpszFrom );
	CWndMessage* OpenMessage( LPCTSTR lpszFrom );
#ifdef __J0519
	BOOL	UpdateMessage( LPCTSTR pszOld, LPCTSTR pszNew );
#endif	// __J0519
	CWndInstantMsg* GetInstantMsg( LPCTSTR lpszFrom );
	CWndInstantMsg* OpenInstantMsg( LPCTSTR lpszFrom );

	// MessageBox
	BOOL	OpenCustomBox( LPCTSTR strMessage, CWndMessageBox* pWndMessageBox, CWndBase* pWndParent = NULL );
	BOOL	OpenMessageBox( LPCTSTR strMessage, UINT nType = MB_OK, CWndBase* pWndParent = NULL );
	BOOL    OpenMessageBoxUpper( LPCTSTR lpszMessage, UINT nType = MB_OK, BOOL bPostLogoutMsg = FALSE );
		
	void	CloseMessageBox();

	// tooltip
	void PutToolTip_Troupe( DWORD dwSkill, CPoint point, CRect* pRect );
	void PutToolTip_Skill( DWORD dwSkill, DWORD dwLevel, CPoint point, CRect* pRect, BOOL bActive = TRUE );
	void PutToolTip_Item( DWORD dwType, DWORD dwId, CPoint point, CRect* pRect, int flag = 0 );
	void PutToolTip_Item( CItemBase* pItemBase, CPoint point, CRect* pRect, int flag = 0 );
	void PutToolTip_Character( int SelectCharacter, CPoint point, CRect* pRect );

	DWORD PutItemName( CItemElem* pItemElem, CEditString* pEdit );
	void PutItemAbilityPiercing( CItemElem* pItemElem, CEditString* pEdit, DWORD dwColorBuf );
	void PutItemResist( CItemElem* pItemElem, CEditString* pEdit );
	void PutItemSpeed( CItemElem* pItemElem, CEditString* pEdit );
	void PutItemMinMax( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit );
	void PutRandomOpt( CItemElem* pItemElem, CEditString* pEdit );
	void PutPiercingOpt( CItemElem* pItemElem, CEditString* pEdit );
	void PutEnchantOpt( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit, int flag );
	void PutSetItemOpt( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit );
	void PutBaseItemOpt( CItemElem* pItemElem, CEditString* pEdit );
	void PutMedicine( CItemElem* pItemElem, DWORD dwParam, LONG nParamVal, CEditString* pEdit );
	void PutBaseResist( CItemElem* pItemElem, CEditString* pEdit );
	void PutItemGold( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit, int flag );
	void PutCoolTime( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit );
	void PutKeepTime( CItemElem* pItemElem, CEditString* pEdit );
	void PutCommand( CItemElem* pItemElem, CEditString* pEdit );
	void PutEndurance( CItemElem* pItemElem, CEditString* pEdit, int flag );
	void PutSex( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit );
	void PutJob( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit );
	void PutLevel( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit );
	void PutWeapon( CItemElem* pItemElem, CEditString* pEdit );
	void PutItemCard( CItemElem* pItemElem, CEditString* pEdit );
	
	CWndMgr();
	virtual	~CWndMgr();

	void	DestroyApplet();

	void AlighWindow( CRect rcOld, CRect rcNew );

	//virtual CItem* GetFocusItem();
	virtual	BOOL    Process();
	virtual	BOOL	Initialize(CWndBase* pWndParent = NULL);
	virtual	void	OnInitialUpdate();
	virtual	void	OnDestroyChildWnd(CWndBase* pWndChild);
	virtual	void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual	BOOL	OnCommand(UINT nID,DWORD dwMessage);
	virtual	void	OnSize(UINT nType, int cx, int cy);
	virtual BOOL    OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
	virtual	BOOL	OnEraseBkgnd(C2DRender* p2DRender);
	virtual	BOOL	OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);

	DWORD	GetAppletId( TCHAR* lpszAppletName );
	CWndBase* GetApplet( DWORD dwIdApplet );
	CWndBase* CreateApplet( DWORD dwIdApplet );
	void	AddAppletFunc( LPVOID pAppletFunc, DWORD dwIdApplet, LPCTSTR lpszKeyword, LPCTSTR pszIconName, LPCTSTR lpszAppletDesc = NULL, CHAR cHotkey = 0 );

	HRESULT	RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	
//	Initialize player
	void SetPlayer( CMover* pMover );

	BOOL LoadRegInfo( LPCTSTR lpszFileName );
	BOOL SaveRegInfo( LPCTSTR lpszFileName );
	LPWNDREGINFO GetRegInfo( DWORD dwWndId );
	BOOL PutRegInfo( DWORD dwWndId, CRect rect, BOOL bOpen );
	BOOL PutRegInfo( LPWNDREGINFO lpRegInfo );
	BOOL PutRegInfo( CWndNeuz* pWndNeuz, BOOL bOpen );

		
	friend CWndApplet;
};

extern CWndMgr          g_WndMng      ; // 윈도 매니저 클래스 
#ifdef __YENCHANT_0608
extern void RenderEnchant( C2DRender* p2DRender, CPoint pt );
#endif //__YENCHANT_0608
extern void RenderRadar( C2DRender* p2DRender, CPoint pt, DWORD dwValue, DWORD dwDivisor );
#define WID_MESSAGEBOX        10

#endif // !defined(AFX_WNDMANAGER_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
