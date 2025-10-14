// WndArcane.cpp: implementation of the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defineSkill.h"
#include "defineItem.h"
#include "defineSound.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndActiveDesktop.h"
#include "WndMessenger.h"
#include "WndCommand.h"
#include "WndNotice.h"
#include "WndNotice.h"
#include "WndMap.h"
#include "WndPartyConfirm.h"
#include "WndPartyChangeTroup.h"
#include "WndFriendConfirm.h"
#include "WndDuelConfirm.h"
#include "WndRegVend.h"
#ifdef __V050322_CACHEQUEST
#include "WndText.h"
#endif // __V050322_CACHEQUEST
#include "party.h"
#include "DPClient.h"
#include "DPLoginClient.h"
#include "dpcertified.h"
#include "ClientMsg.h"
#include "yUtil.h"

#ifdef __SM_ITEM_2ND
#include "WndSelectVillage.h"
#endif	// __SM_ITEM_2ND

#include "webbox.h"

extern	CParty	g_Party;

#ifdef __J0710
#include "sntp.h"
#endif	// __J0710

extern	CDPClient	g_DPlay;	 
extern	CDPLoginClient	g_dpLoginClient;
extern	CDPCertified	g_dpCertified;

#ifdef __ONLINE
#include "dpclient.h"
extern	CDPClient	g_DPlay;
#endif	// __ONLINE

#include "guild.h"
extern	CGuildMng	g_GuildMng;
#include "guildwar.h"
extern	CGuildWarMng	g_GuildWarMng;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern _ERROR_STATE g_Error_State;

extern DWORD FULLSCREEN_WIDTH;
extern DWORD FULLSCREEN_HEIGHT;

#define CLIENT_WIDTH FULLSCREEN_WIDTH
#define CLIENT_HEIGHT FULLSCREEN_HEIGHT

CWndMgr          g_WndMng;

const char* GetATKSpeedString( float fSpeed )
{
	if( fSpeed < 0.035 )
		return prj.GetText(TID_GAME_VERYSLOW);  // "아주 느림"
	else if( fSpeed < 0.050 )
		return prj.GetText(TID_GAME_SLOW);		//"느림";
	else if( fSpeed < 0.070 )
		return prj.GetText(TID_GAME_NORMALS);	// "보통";
	else if( fSpeed < 0.080 )
		return prj.GetText(TID_GAME_FAST);		//"빠름";
#ifdef __NEWWPN0818
	else if( fSpeed < 0.17 )
		return prj.GetText(TID_GAME_VERYFAST);	//"아주 빠름";
	else
		return prj.GetText(TID_GAME_FASTEST);	//"아주 빠름";
#else	// __NEWWPN0818
	else
		return prj.GetText(TID_GAME_VERYFAST);	//"아주 빠름";
#endif	// __NEWWPN0818
}

#ifdef __YCOOLTIME_0214	
CTexture*	   g_pCoolTexArry[128];
#endif// __YCOOLTIME_0214

#ifdef __YENCHANT_0608
CTexture*	   g_pEnchantTexArry[11];
void RenderEnchant( C2DRender* p2DRender, CPoint pt )
{
	static int i=0;
	
	CPoint ptBack = pt;
	CPoint ptBack_size( 32, 32 );

	i++;

	if( i>=11 )
		i = 0;
	
	p2DRender->RenderTextureEx( ptBack, ptBack_size, g_pEnchantTexArry[i], 255, 1.0f, 1.0f ) ;
}
#endif //__YENCHANT_0608

// 레이더 모양을 그린다.
void RenderRadar( C2DRender* p2DRender, CPoint pt, DWORD dwValue, DWORD dwDivisor )
{
	pt.y += 1;
	pt.x += 15;
	
	CPoint pt_size( 15, 15 );
	const BYTE bAlpha = 200;
	
	//	float fPercent = (float)dwValue / (float)dwDivisor;
	//	int nDrawIndex = (fPercent * 127);
	int nDrawIndex = MulDiv( dwValue, 127, dwDivisor );
	
	CPoint ptBack = pt;
	CPoint ptBack_size( 32, 32 );
	ptBack.x -= 15;
	p2DRender->RenderFillRect( CRect( ptBack, ptBack+ptBack_size ), D3DCOLOR_ARGB( 120, 255,255,255 ) );
	
	if( nDrawIndex >= 0 && nDrawIndex < 32 )
	{
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[nDrawIndex], bAlpha, 1.1f, 1.1f );
		
		pt.y+=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[32], bAlpha, 1.1f, 1.1f );
		
		pt.x-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[64], bAlpha, 1.1f, 1.1f );
		
		pt.y-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[96], bAlpha, 1.1f, 1.1f );
	}
	else
	if( nDrawIndex >= 32 && nDrawIndex <= 63 )
	{
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[31], bAlpha, 1.1f, 1.1f );
		
		pt.y+=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[nDrawIndex], bAlpha, 1.1f, 1.1f );
		
		pt.x-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[64], bAlpha, 1.1f, 1.1f );
		
		pt.y-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[96], bAlpha, 1.1f, 1.1f );
	}
	else
	if( nDrawIndex >= 64 && nDrawIndex <= 95 )
	{
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[31], bAlpha, 1.1f, 1.1f );
		
		pt.y+=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[63], bAlpha, 1.1f, 1.1f );
		
		pt.x-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[nDrawIndex], bAlpha, 1.1f, 1.1f );
		
		pt.y-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[96], bAlpha, 1.1f, 1.1f );
	}
	else
	if( nDrawIndex >= 96 && nDrawIndex <= 128 )
	{
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[31], bAlpha, 1.1f, 1.1f );
		
		pt.y+=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[63], bAlpha, 1.1f, 1.1f );
		
		pt.x-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[95], bAlpha, 1.1f, 1.1f );
		
		pt.y-=16;
		p2DRender->RenderTextureEx( pt, pt_size, g_pCoolTexArry[nDrawIndex], bAlpha, 1.1f, 1.1f );
	}	
				
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// CWndMgr : 매니저 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//#define TASKBAR_HEIGHT 38

CWndMgr::CWndMgr()
{
	m_bTitle = FALSE;
	m_pWndActiveDesktop = NULL;
	m_pWndShop = NULL;
	m_pWndDialog = NULL;
	m_pWndRevival = NULL;
#ifdef __NPP_050308
	m_pWndRecoverChao = NULL;
	m_pReWanted		  = NULL;
	m_pWanted		  = NULL;
	m_pWndResurrectionConfirm = NULL;
	m_pLogOutWaitting = NULL;
#endif //__NPP_050308
#ifdef __S050524_SM_ITEM
	m_pWndCommItemDlg = NULL;
	m_pWndChangeClass1 = NULL;
	m_pWndChangeClass2 = NULL;
#endif // __S050524_SM_ITEM
	m_pWndTradeGold = NULL;
	m_pWndDropItem = NULL;
	m_pWndQuickList = NULL;
	m_pWndCommand = NULL;
	m_pWndTaskBar = NULL;
	m_pWndTrade = NULL;
	//m_pWndNotice = NULL;
	////m_pWndTip = NULL;
	//m_bTitle = TRUE;
	m_pWndBeautyShop = NULL;
	m_pWndDropConfirm = NULL;
	m_nAlphaCount = m_nAlpha;
	m_pWndBank = NULL;
	m_pWndConfirmBank = NULL;
	m_pWndBankPassword = NULL;
	m_pWndConfirmTrade = NULL;

#ifdef __Y_POST_0912
	m_pWndPost = NULL;
#endif //__Y_POST_0912	
	
#ifdef __QUERYEQUIP0630	
	m_pWndQueryEquip = NULL;
#endif //__QUERYEQUIP0630
	

#ifdef __SM_ITEM_2ND
	m_pWndSelectVillage = NULL;
#endif // __SM_ITEM_2ND

#ifdef __S050504_QUESTITEM
	m_pWndQuestItemWarning = NULL;
#endif // __S050504_QUESTITEM
	
#ifdef __NPP_050308
	m_pWndPenaltyPK = NULL;
#endif // __NPP_050308

	m_pWndPartyChangeName = NULL;
	m_pWndPartyConfirm = NULL;
	m_pWndFriendConfirm = NULL;
	m_pWndDuelConfirm = NULL;
	m_pWndMessageNote = NULL;
	m_pWndMessengerNote = NULL;
	
//	m_pWndMessengerSearch	= NULL;
	m_pWndStateConfirm		= NULL;

	m_szTimerChat[ 0 ] = 0;
	m_timerChat.Set( 0 );
	m_bConnect = FALSE;
#ifdef __SDEBUG
	m_timerMessenger.Set( SEC( 20 ) );
#else // __SDEBUG
	m_timerMessenger.Set( MIN( 5 ) );
#endif // __SDEBUG

	for( int i = 0 ; i < MAX_SKILL ; i++ )
	{
		m_dwSkillTime[i] = 0;
	}
	
	m_pWndRankGuild = NULL;
	m_pWndRankInfo = NULL;
	m_pWndRankWar = NULL;
	m_pWndGuildBank = NULL;
	m_pWndGuildConfirm = NULL;
	m_pWndGuildWarPeaceConfirm = NULL;
	m_pWndGuildWarRequest = NULL;

#ifdef __GUILDCOMBAT
	m_pWndGuildWarState = NULL;
	m_pWndGuildCombatRanking = NULL;
#endif //__GUILDCOMBAT
	

	m_pWndUpgradeBase = NULL;
	
#ifdef __PIERCING_SM_TIME
	m_pWndPiercing = NULL;
#endif //__PIERCING_SM_TIME
	
	
	
#ifdef __PIERCING_SM_TIME
	m_pWndCommerialElem = NULL;
	m_pRemoveElem = NULL;
#endif // __PIERCING_SM_TIME

#ifdef __NQUESTITEM_INFO_040917
	m_pQuestItemInfo = NULL;
#endif//__NQUESTITEM_INFO_040917

#ifdef __V050322_CACHEQUEST
	m_pWndTextBook = NULL;
	m_pWndTextScroll = NULL;
	m_pWndTextLetter = NULL;
#endif // __V050322_CACHEQUEST

	m_pWndMessageBox = NULL;
	m_pWndMessageBoxUpper = NULL;

	m_dwSavePlayerTime = 0;

#ifdef __S0126_MOTION_AUTORUN
	m_bAutoRun = FALSE;
#endif // __S0126_MOTION_AUTORUN

#ifdef __J0519
	m_pWndChangeName			= NULL;
#endif	// __J0519
	InitSetItemTextColor();
}

void CWndMgr::InitSetItemTextColor( )
{
	
//	dwItemColor[FIRST_TC].dwName0				= D3DCOLOR_XRGB(   0,   0, 255 );			// 이름 0단계

//	dwItemColor[FIRST_TC].dwName0				= D3DCOLOR_XRGB(  12, 144, 231 );
	dwItemColor[FIRST_TC].dwName0				= D3DCOLOR_XRGB(  46, 112, 169 );
	dwItemColor[FIRST_TC].dwName1				= D3DCOLOR_XRGB(   0,  93,   0 );			// 이름 1단계

	dwItemColor[FIRST_TC].dwName2				= D3DCOLOR_XRGB( 182,   0, 255 );			// 이름 2단계

	dwItemColor[FIRST_TC].dwName3				= D3DCOLOR_XRGB(   0,   0, 255 );			// 이름 3단계

	dwItemColor[FIRST_TC].dwName4				= D3DCOLOR_XRGB(   0,   0, 255 );			// 이름 4단계

	dwItemColor[FIRST_TC].dwName5				= D3DCOLOR_XRGB(   0,   0, 255 );			// 이름 5단계

	dwItemColor[FIRST_TC].dwGeneral				= D3DCOLOR_XRGB(   0,   0,   0 );			// 일반
	dwItemColor[FIRST_TC].dwPlusOption			= D3DCOLOR_XRGB(   0,   0,   0 );			// 플러스 옵션
	dwItemColor[FIRST_TC].dwPiercing			= D3DCOLOR_XRGB( 178,   0, 255 );			// 피어싱
	dwItemColor[FIRST_TC].dwResist				= D3DCOLOR_XRGB(   0,   0,   0 );			// 속성 일반
	dwItemColor[FIRST_TC].dwResistFire			= D3DCOLOR_XRGB(   0,   0,   0 );			// 속성 불
	dwItemColor[FIRST_TC].dwResistWater			= D3DCOLOR_XRGB(   0,   0,   0 );			// 속성 물
	dwItemColor[FIRST_TC].dwResistEarth			= D3DCOLOR_XRGB(   0,   0,   0 );			// 속성 땅
	dwItemColor[FIRST_TC].dwResistElectricity	= D3DCOLOR_XRGB(   0,   0,   0 );			// 속성 전기

	dwItemColor[FIRST_TC].dwResistWind			= D3DCOLOR_XRGB(   0,   0,   0 );			// 속성 바람

	dwItemColor[FIRST_TC].dwResistSM			= D3DCOLOR_XRGB(   0, 158,   0 );			// 속성 상용화 무기

	dwItemColor[FIRST_TC].dwResistSM1			= D3DCOLOR_XRGB( 255,   0,   0 );			// 속성 상용화 방어구
	dwItemColor[FIRST_TC].dwTime				= D3DCOLOR_XRGB(   0,   200,   0 );			// 시간 및 효과

	dwItemColor[FIRST_TC].dwEffective0			= D3DCOLOR_XRGB(   0,   0,   0 );			// 아이템 효능
	dwItemColor[FIRST_TC].dwEffective1			= D3DCOLOR_XRGB(   0, 255,   0 );			// 아이템 효능1
	dwItemColor[FIRST_TC].dwEffective2			= D3DCOLOR_XRGB( 255,   0,   0 );			// 아이템 효능2
	dwItemColor[FIRST_TC].dwEffective3			= D3DCOLOR_XRGB( 128, 128, 128 );			// 아이템 효능3
	dwItemColor[FIRST_TC].dwRandomOption		= D3DCOLOR_XRGB(   0,   0,   0 );			// 랜덤 옵션
	dwItemColor[FIRST_TC].dwEnchantOption		= D3DCOLOR_XRGB(   0,   0,   0 );			// 인첸트 옵션
	dwItemColor[FIRST_TC].dwSetName				= D3DCOLOR_XRGB(   0,   0,   0 );			// 세트 이름
	dwItemColor[FIRST_TC].dwSetItem0			= D3DCOLOR_XRGB( 178, 178, 178 );			// 세트 목록(비착용시)
	dwItemColor[FIRST_TC].dwSetItem1			= D3DCOLOR_XRGB(   0,   0, 204 );			// 세트 목록(착용시)
	dwItemColor[FIRST_TC].dwSetEffect			= D3DCOLOR_XRGB( 255, 157,   0 );			// 세트 효과

	dwItemColor[FIRST_TC].dwGold				= D3DCOLOR_XRGB(   0,   0,   0 );			// 가격
	dwItemColor[FIRST_TC].dwCommand				= D3DCOLOR_XRGB(   0,   0,   0 );			// 설명

	dwItemColor[FIRST_TC].dwNotUse				= D3DCOLOR_XRGB( 255,  66,   0 );			// 사용못함				

	memcpy( &dwItemColor[SECOND_TC], &dwItemColor[FIRST_TC], sizeof( ToolTipItemTextColor ) );
	memcpy( &dwItemColor[THIRD_TC], &dwItemColor[FIRST_TC], sizeof( ToolTipItemTextColor ) );
	dwItemColor[SECOND_TC].dwName0				= D3DCOLOR_XRGB(  12, 144, 231 );
	dwItemColor[SECOND_TC].dwResistFire			= D3DCOLOR_XRGB( 230,   0,   0 );			// 속성 불
	dwItemColor[SECOND_TC].dwResistWater		= D3DCOLOR_XRGB(   0,   0, 182 );			// 속성 물
	dwItemColor[SECOND_TC].dwResistEarth		= D3DCOLOR_XRGB( 185, 118,   0 );			// 속성 땅
	dwItemColor[SECOND_TC].dwResistElectricity	= D3DCOLOR_XRGB(  21,  55, 152 );			// 속성 전기

	dwItemColor[SECOND_TC].dwResistWind			= D3DCOLOR_XRGB(   0, 217, 214 );			// 속성 바람

	dwItemColor[SECOND_TC].dwTime				= D3DCOLOR_XRGB( 146,   0,  38 );			// 시간 및 효과

	dwItemColor[SECOND_TC].dwPlusOption			= D3DCOLOR_XRGB( 199, 155,   0 );			// 플러스 옵션
	dwItemColor[SECOND_TC].dwPiercing			= D3DCOLOR_XRGB(  77,  11, 145 );			// 피어싱
	dwItemColor[SECOND_TC].dwRandomOption		= D3DCOLOR_XRGB( 199, 155,   0 );			// 랜덤 옵션
	dwItemColor[SECOND_TC].dwSetName			= D3DCOLOR_XRGB(  50, 178,   0 );			// 세트 이름
	dwItemColor[SECOND_TC].dwSetItem0			= D3DCOLOR_XRGB( 178, 178, 178 );			// 세트 목록(비착용시)
	dwItemColor[SECOND_TC].dwSetItem1			= D3DCOLOR_XRGB( 114, 186,  85 );			// 세트 목록(착용시)
	dwItemColor[SECOND_TC].dwSetEffect			= D3DCOLOR_XRGB(  50, 178,   0 );			// 세트 효과

	dwItemColor[SECOND_TC].dwCommand			= D3DCOLOR_XRGB( 199, 155,   0 );			// 설명

}

CWndMgr::~CWndMgr()
{
	AppletFunc* pApplet;
	DWORD dwIdApplet;
	POSITION pos = m_mapAppletFunc.GetStartPosition();
	while( pos )
	{
		m_mapAppletFunc.GetNextAssoc( pos, dwIdApplet, (void*&)pApplet );
		SAFE_DELETE( (AppletFunc*) pApplet );
	}
	Free();
}

void CWndMgr::AlighWindow( CRect rcOld, CRect rcNew )
{
	for(int i = 0; i < m_wndOrder.GetSize(); i++ )
	{
		CWndBase* pWndBase = (CWndBase*) m_wndOrder.GetAt( i );
		CRect rcWnd = pWndBase->GetWindowRect( TRUE );
		CPoint point = rcWnd.TopLeft();
		if( pWndBase != this && pWndBase != m_pWndTaskBar )
		{
			if( pWndBase->m_nWinSize == WSIZE_MAX )
			{
				pWndBase->SetWndRect( rcNew );
			}
			else
			{
				if( rcWnd.left   < rcNew.left   ) point.x = rcNew.left;
				if( rcWnd.top    < rcNew.top    ) point.y = rcNew.top;
				if( rcWnd.right  > rcNew.right  ) point.x = rcNew.right - rcWnd.Width();
				if( rcWnd.bottom > rcNew.bottom ) point.y = rcNew.bottom - rcWnd.Height();

				if( rcWnd.left   == rcOld.left   ) point.x = rcNew.left;
				if( rcWnd.top    == rcOld.top    ) point.y = rcNew.top;
				if( rcWnd.right  == rcOld.right  ) point.x = rcNew.right - rcWnd.Width();
				if( rcWnd.bottom == rcOld.bottom ) point.y = rcNew.bottom - rcWnd.Height();

				pWndBase->Move( point );
			}
		}
	}
}


void CWndMgr::Free()
{
//	DestroyApplet();
//	AppletFunc* pApplet;
//	DWORD dwIdApplet;
	POSITION pos = m_mapWndRegInfo.GetStartPosition();
	LPWNDREGINFO pWndRegInfo;
	DWORD dwWndId;
	while( pos )
	{
		m_mapWndRegInfo.GetNextAssoc( pos, dwWndId, (void*&)pWndRegInfo );
		SAFE_DELETE( pWndRegInfo->lpArchive );
		SAFE_DELETE( pWndRegInfo );
	}
	m_mapWndRegInfo.RemoveAll();
	
	SAFE_DELETE( m_pWndTaskBar );
	SAFE_DELETE( m_pWndActiveDesktop );
	SAFE_DELETE( m_pWndDialog );
	SAFE_DELETE( m_pWndShop );
	SAFE_DELETE( m_pWndRevival );




#ifdef __NPP_050308
	SAFE_DELETE(m_pWndRecoverChao);
	SAFE_DELETE(m_pReWanted);
	SAFE_DELETE(m_pWanted);
	SAFE_DELETE(m_pWndResurrectionConfirm);
	SAFE_DELETE(m_pLogOutWaitting);
#endif //__NPP_050308	
#ifdef __S050524_SM_ITEM
	SAFE_DELETE( m_pWndCommItemDlg );
	SAFE_DELETE( m_pWndChangeClass1 );
	SAFE_DELETE( m_pWndChangeClass2 );
#endif // __S050524_SM_ITEM
	
	SAFE_DELETE( m_pWndTradeGold );
	SAFE_DELETE( m_pWndQuickList );
	SAFE_DELETE( m_pWndMessageBox );
	SAFE_DELETE( m_pWndMessageBoxUpper );
	
	SAFE_DELETE( m_pWndCommand );
	SAFE_DELETE( m_pWndTrade );
	m_pWndWorld = NULL;
	SAFE_DELETE( m_pWndBeautyShop );

#ifdef __Y_POST_0912
	SAFE_DELETE( m_pWndPost );
#endif //__Y_POST_0912	
	

	SAFE_DELETE( m_pWndBank );
	SAFE_DELETE( m_pWndDropItem );

#ifdef __QUERYEQUIP0630	
	SAFE_DELETE(m_pWndQueryEquip);
#endif //__QUERYEQUIP0630
	

	SAFE_DELETE( m_pWndDropConfirm );

#ifdef __NPP_050308
	SAFE_DELETE( m_pWndPenaltyPK );
#endif // __NPP_050308
	SAFE_DELETE( m_pWndConfirmBank );
	SAFE_DELETE( m_pWndBankPassword );
	SAFE_DELETE( m_pWndPartyChangeName );
	SAFE_DELETE( m_pWndDuelConfirm );
	SAFE_DELETE( m_pWndConfirmTrade );

#ifdef __SM_ITEM_2ND
	SAFE_DELETE( m_pWndSelectVillage );
#endif // __SM_ITEM_2ND

#ifdef __S050504_QUESTITEM
	SAFE_DELETE( m_pWndQuestItemWarning );
#endif // __S050504_QUESTITEM
	SAFE_DELETE( m_pWndPartyConfirm );
	SAFE_DELETE( m_pWndFriendConfirm );
	//SAFE_DELETE( m_pWndCompanyInvite );
//	SAFE_DELETE( m_pWndMessengerSearch );
	SAFE_DELETE( m_pWndMessageNote );
	SAFE_DELETE( m_pWndMessengerNote );
	SAFE_DELETE( m_pWndAddFriend );
	SAFE_DELETE( m_pWndStateConfirm );
	CString strTemp;
	CWndBase* pWndBaseTemp;
	pos = m_mapMessage.GetStartPosition();
	while( pos )
	{
		m_mapMessage.GetNextAssoc( pos, strTemp, (void*&)pWndBaseTemp );
		SAFE_DELETE( pWndBaseTemp );//	요 라인에서 뒤지니까 함 확인해보셈. -xuzhu-
	}
	
	m_mapMessage.RemoveAll();

	pos = m_mapInstantMsg.GetStartPosition();
	while( pos )
	{
		m_mapInstantMsg.GetNextAssoc( pos, strTemp, (void*&)pWndBaseTemp );
		SAFE_DELETE( pWndBaseTemp );
	}
	m_mapInstantMsg.RemoveAll();
	
	pos = m_mapMap.GetStartPosition();
	while( pos )
	{
		m_mapMap.GetNextAssoc( pos, strTemp, (void*&)pWndBaseTemp );
		SAFE_DELETE( pWndBaseTemp );
	}
	m_mapMap.RemoveAll();

#ifdef	__NQUESTITEM_INFO_040917
	SAFE_DELETE( m_pQuestItemInfo );
#endif//__NQUESTITEM_INFO_040917
#ifdef __V050322_CACHEQUEST
	SAFE_DELETE( m_pWndTextBook );
	SAFE_DELETE( m_pWndTextScroll );
	SAFE_DELETE( m_pWndTextLetter );
#endif // __V050322_CACHEQUEST

	SAFE_DELETE(m_pWndRankGuild);
	SAFE_DELETE(m_pWndRankInfo);
	SAFE_DELETE(m_pWndRankWar);
	SAFE_DELETE(m_pWndGuildBank);
	SAFE_DELETE(m_pWndGuildConfirm);
	SAFE_DELETE(m_pWndGuildWarPeaceConfirm);
	SAFE_DELETE( m_pWndGuildWarRequest );

#ifdef __GUILDCOMBAT
	SAFE_DELETE(m_pWndGuildCombatRanking);
	SAFE_DELETE(m_pWndGuildWarState);
#endif //__GUILDCOMBAT
	

	SAFE_DELETE( m_pWndUpgradeBase );
	
#ifdef __PIERCING_SM_TIME
	SAFE_DELETE( m_pWndPiercing );
#endif //__PIERCING_SM_TIME
	

	SAFE_DELETE( m_pWndCommerialElem );
	SAFE_DELETE( m_pRemoveElem );

#ifdef __J0519
	SAFE_DELETE( m_pWndChangeName );
#endif	// __J0519
}
void CWndMgr::DestroyApplet()
{
	CWndNeuz* pWndBase;
	DWORD dwIdApplet;
	POSITION pos = m_mapWndApplet.GetStartPosition();
	while( pos )
	{
		m_mapWndApplet.GetNextAssoc( pos, dwIdApplet, (void*&)pWndBase );
		// 윈도가 종료될 때 WndRect정보를 저장하지 않게 해야한다.
		// 여기서 파괴되는 것은 오픈되어 있는 것으로 간주해야되는데,
		// 내부에서 파괴되면 크로즈로 판단하기 때문에 재실행시 윈도가 열리지 않게 된다.
		//pWndBase->SetPutRegInfo( FALSE );
		// 이제 여기서 임의로 오픈되어 있다고 저장하자.
		//PutRegInfo( dwIdApplet, pWndBase->GetWindowRect( TRUE), TRUE );
		if( pWndBase->IsPutRegInfo() )
			PutRegInfo( (CWndNeuz*)pWndBase, TRUE );//dwIdApplet, pWndBase->GetWindowRect( TRUE), TRUE );
		// 이제 다 끝났다. 파괴하자.
		delete pWndBase;
	}
	// 파괴할 때 인터페이스 정보가 저장된다. 따라서 파괴 이후 save하기.
	if( m_bTitle == FALSE )
		SaveRegInfo( "regInfo.dat" );
	m_mapWndApplet.RemoveAll();
}
BOOL CWndMgr::Initialize(CWndBase* pParentWnd)
{
	CWndBase::m_resMng.Load( _T( RESDATA_INC ) );
	Create( WBS_NOFRAME | WBS_MANAGER | WBS_NODRAWFRAME, CRect( 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT/* - TASKBAR_HEIGHT*/ ), this, 100 );
	return TRUE;
}
void CWndMgr::OnInitialUpdate()
{
	m_timerDobe.Set( SEC( 2 ) );
	AddAllApplet();
#ifdef __ACTIVEDESKTOP
	m_pWndActiveDesktop = new CWndDesktopMatrix;
	m_pWndActiveDesktop->Initialize( &g_WndMng, 101 );
#endif
//	g_WndMng.AddWnd( m_pWndActiveDesktop );

	m_dwSavePlayerTime = 0;

#ifdef __YCOOLTIME_0214	
	char filename[1024] = { 0 };

	for( int i=0; i<128; i++ )
	{
		sprintf( filename, "Icon_CoolTime_%.3d.tga", i );
		g_pCoolTexArry[i] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice,  MakePath( DIR_ICON, filename ), 0xffff00ff );
	}
#endif// __YCOOLTIME_0214

#ifdef __YENCHANT_0608
	for( i=0; i<11; i++ )
	{
		sprintf( filename, "Icon_ImgIncAni_%.2d.tga", i );
		g_pEnchantTexArry[i] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice,  MakePath( DIR_ICON, filename ), 0xffffffff );
	}	
#endif //__YENCHANT_0608
}

void CWndMgr::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}
void CWndMgr::OnDestroyChildWnd( CWndBase* pWndChild )
{
	/*
	if( m_pWndCompanyInvite == pWndChild )
	{
		m_pWndCompanyInvite = NULL;
		pWndChild = NULL;
	}
	*/
	BOOL bFocus = TRUE;
	if( pWndChild && pWndChild->IsWndStyle( WBS_NOFOCUS ) )
		bFocus = FALSE;
	if( m_pWndWorld == pWndChild )
	{
		SAFE_DELETE( m_pWndWorld );
		pWndChild = NULL;
	} else
	if( m_pWndBeautyShop == pWndChild )
	{
		SAFE_DELETE( m_pWndBeautyShop );
		pWndChild = NULL;
	}
	else
	if( m_pWndPartyConfirm == pWndChild )
	{
		SAFE_DELETE( m_pWndPartyConfirm );
		pWndChild = NULL;
	} else
	if( m_pWndFriendConfirm == pWndChild )
	{
		m_pWndFriendConfirm = NULL;
		pWndChild = NULL;
	} else
	if( m_pWndConfirmTrade == pWndChild )
	{
		m_pWndConfirmTrade = NULL;
		pWndChild = NULL;
	} else
#ifdef __Y_POST_0912
	if( m_pWndPost == pWndChild )
	{
		SAFE_DELETE( m_pWndPost );
		pWndChild = NULL;
	} else
#endif //__Y_POST_0912		
#ifdef __S050504_QUESTITEM
	if( m_pWndQuestItemWarning == pWndChild )
	{
		m_pWndQuestItemWarning = NULL;
		pWndChild = NULL;
	} else
#endif // __S050504_QUESTITEM
	if( m_pWndPartyChangeName == pWndChild )
	{
		SAFE_DELETE( m_pWndPartyChangeName );
		pWndChild = NULL;
	} else

	if( m_pWndBank == pWndChild )
	{
		SAFE_DELETE( m_pWndBank );
		pWndChild = NULL;
#ifdef __BUGFIX0725
		if( m_pWndTradeGold )
			m_pWndTradeGold->Destroy();
#endif	// __BUGFIX0725
//	} else
//	if( m_pWndMessengerSearch == pWndChild )
//	{
//		SAFE_DELETE( m_pWndMessengerSearch );
//		pWndChild = NULL;
	} else
	if( m_pWndMessageNote == pWndChild )
	{
		SAFE_DELETE( m_pWndMessageNote );
		pWndChild = NULL;
	} else
	if( m_pWndMessengerNote == pWndChild )
	{
		SAFE_DELETE( m_pWndMessengerNote );
		pWndChild = NULL;
	} else
	if( m_pWndAddFriend == pWndChild )
	{
		SAFE_DELETE( m_pWndAddFriend );
		pWndChild = NULL;
	} else
	if( m_pWndStateConfirm == pWndChild )
	{
		SAFE_DELETE( m_pWndStateConfirm );
		pWndChild = NULL;
	} else
	if( m_pWndConfirmBank == pWndChild )
	{
		SAFE_DELETE( m_pWndConfirmBank );
		pWndChild = NULL;
	} else
#ifdef __NPP_050308
	if( m_pWndPenaltyPK == pWndChild )
	{
		SAFE_DELETE( m_pWndPenaltyPK );
		pWndChild = NULL;
	} else
#endif // __NPP_050308
	if( m_pWndBankPassword == pWndChild )
	{
		SAFE_DELETE( m_pWndBankPassword );
		pWndChild = NULL;
	} else
	if( m_pWndMessageBox == pWndChild )
	{
		SAFE_DELETE( m_pWndMessageBox );
		pWndChild = NULL;
	} else
	if( m_pWndMessageBoxUpper == pWndChild )
	{
		SAFE_DELETE( m_pWndMessageBoxUpper );
		pWndChild = NULL;
	} else
	if( m_pWndRevival == pWndChild )
	{
		SAFE_DELETE( m_pWndRevival );
		pWndChild = NULL;
	} else
#ifdef __NPP_050308
	if( m_pLogOutWaitting == pWndChild )
	{
		SAFE_DELETE( m_pLogOutWaitting );
		pWndChild = NULL;
	} else
	if( m_pWndResurrectionConfirm == pWndChild )
	{
		SAFE_DELETE( m_pWndResurrectionConfirm );
		pWndChild = NULL;
	} else
	if( m_pWndRecoverChao == pWndChild )
	{
		SAFE_DELETE( m_pWndRecoverChao );
		pWndChild = NULL;
	} else
	if( m_pReWanted == pWndChild )
	{
		SAFE_DELETE( m_pReWanted );
		pWndChild = NULL;
	} else
	if( m_pWanted == pWndChild )
	{
		SAFE_DELETE( m_pWanted );
		pWndChild = NULL;
	} else
#endif //__NPP_050308	
#ifdef __S050524_SM_ITEM
	if( m_pWndCommItemDlg == pWndChild )
	{
		SAFE_DELETE( m_pWndCommItemDlg );
		pWndChild = NULL;
	} else
	if( m_pWndChangeClass1 == pWndChild )
	{
		SAFE_DELETE( m_pWndChangeClass1 );
		pWndChild = NULL;
	}else
	if( m_pWndChangeClass2 == pWndChild )
	{
		SAFE_DELETE( m_pWndChangeClass2 );
		pWndChild = NULL;
	}else		
#endif // __S050524_SM_ITEM
#ifdef __J0519
	if( m_pWndChangeName == pWndChild )
	{
		SAFE_DELETE( m_pWndChangeName );
		pWndChild = NULL;
	} else
#endif	// __J0519
	if( m_pWndDialog == pWndChild )
	{
		SAFE_DELETE( m_pWndDialog );
		pWndChild = NULL;
	} else
	if( m_pWndShop == pWndChild )
	{
		SAFE_DELETE( m_pWndShop );
		pWndChild = NULL;
	} else
	if( m_pWndTrade == pWndChild )
	{
		SAFE_DELETE( m_pWndTrade );
		pWndChild = NULL;
	} else
	if( m_pWndTradeGold == pWndChild )
	{
		SAFE_DELETE( m_pWndTradeGold );
		pWndChild = NULL;
	} else
	if( m_pWndDropItem == pWndChild )
	{
		SAFE_DELETE( m_pWndDropItem );
		m_pWndDropItem = NULL;
	} else
	if( m_pWndQuickList == pWndChild )
	{
		SAFE_DELETE( m_pWndQuickList );
		pWndChild = NULL;
	} else
	if( m_pWndCommand == pWndChild )
	{
		SAFE_DELETE( m_pWndCommand );
		pWndChild = NULL;
	} else
	if( m_pWndRankGuild == pWndChild )
	{
		SAFE_DELETE( m_pWndRankGuild );
		pWndChild = NULL;
	}else
	if( m_pWndRankInfo == pWndChild )
	{
		SAFE_DELETE( m_pWndRankInfo );
		pWndChild = NULL;
	}else
	if( m_pWndRankWar == pWndChild )
	{
		SAFE_DELETE( m_pWndRankWar );
		pWndChild = NULL;
	}else
	if( m_pWndGuildBank == pWndChild )
	{
#ifdef __ONLINE
		g_DPlay.SendCloseGuildBankWnd();
#endif // __ONLINE
		SAFE_DELETE( m_pWndGuildBank );
		pWndChild = NULL;
#ifdef __BUGFIX0725
		if( m_pWndTradeGold )
			m_pWndTradeGold->Destroy();
#endif	// __BUGFIX0725
	}else
	if( m_pWndGuildConfirm == pWndChild )
	{
		SAFE_DELETE( m_pWndGuildConfirm );
		pWndChild = NULL;
	}else
	if( m_pWndGuildWarPeaceConfirm == pWndChild )
	{
		SAFE_DELETE( m_pWndGuildWarPeaceConfirm );
		pWndChild = NULL;
	} else
	if( m_pWndGuildWarRequest == pWndChild )
	{
		SAFE_DELETE( m_pWndGuildWarRequest );
		pWndChild = NULL;
	} else
	if( m_pWndDuelConfirm == pWndChild )
	{
		SAFE_DELETE( m_pWndDuelConfirm );
		pWndChild = NULL;
	}

#ifdef __GUILDCOMBAT
	if( m_pWndGuildCombatRanking == pWndChild )
	{
		SAFE_DELETE( m_pWndGuildCombatRanking );
		pWndChild = NULL;
	}
	
	if( m_pWndGuildWarState == pWndChild )
	{
		SAFE_DELETE( m_pWndGuildWarState );
		pWndChild = NULL;
	}
#endif //__GUILDCOMBAT
	
	if( m_pWndUpgradeBase == pWndChild )
	{
		SAFE_DELETE( m_pWndUpgradeBase );
		pWndChild = NULL;
	}


#ifdef __PIERCING_SM_TIME
	if( m_pWndPiercing == pWndChild )
	{
		SAFE_DELETE( m_pWndPiercing );
		pWndChild = NULL;
	}
#endif //__PIERCING_SM_TIME
	
#ifdef __QUERYEQUIP0630	
	if( m_pWndQueryEquip == pWndChild )
	{
		SAFE_DELETE( m_pWndQueryEquip );
		pWndChild = NULL;
	}
#endif //__QUERYEQUIP0630
	
	


	if( m_pWndCommerialElem == pWndChild )
	{
		SAFE_DELETE( m_pWndCommerialElem );
		pWndChild = NULL;
	}

	if( m_pRemoveElem == pWndChild )
	{
		SAFE_DELETE( m_pRemoveElem );
		pWndChild = NULL;
	}

	if( m_pWndDropConfirm == pWndChild )
	{
		SAFE_DELETE( m_pWndDropConfirm );
		pWndChild = NULL;
	} else
	if( pWndChild )
	{
		CWndNeuz* pWndBase;
		if( pWndChild->GetWndId() == APP_MAP )
		{
			CWndMap* pWndMap = (CWndMap*) pWndChild;
			if( m_mapMap.Lookup( pWndMap->m_szMapFile, (void*&) pWndMap ) == TRUE )
			{
				m_mapMap.RemoveKey( pWndMap->m_szMapFile );
				SAFE_DELETE( pWndMap );
				//return;
			}
		}
		else
		if( pWndChild->GetWndId() == APP_PARTY_CONFIRM )
		{
			SAFE_DELETE( pWndChild );
			//return;
		}
		else
		if( m_mapWndApplet.Lookup( pWndChild->GetWndId(), (void*&) pWndBase ) == TRUE )
		{
			m_mapWndApplet.RemoveKey( pWndChild->GetWndId() );
			// 애플렛으로 등록된 윈도만 파괴할 때 윈도의 정보를 저장한다. 
			if( pWndBase->IsPutRegInfo() )
				PutRegInfo( pWndBase, FALSE );
			SAFE_DELETE( pWndBase );
			pWndChild = NULL;
			//return;
		}
		else
		if( pWndChild->GetWndId() == APP_MESSAGE )
		{
			CWndMessage* pWndMessage = (CWndMessage*) pWndChild;
			if( m_mapMessage.Lookup( pWndMessage->m_strPlayer, (void*&) pWndBase ) == TRUE )
			{
				if( pWndBase != pWndMessage )
					ADDERRORMSG( "APP_MESSAGE_Vampyre" );
				m_mapMessage.RemoveKey( pWndMessage->m_strPlayer );
				SAFE_DELETE( pWndBase );
				pWndMessage = NULL;
				//SAFE_DELETE( pWndMessage );
				//return;
			}
		}
		else
		if( pWndChild->GetWndId() == APP_INSTANTMSG )
		{
			CWndInstantMsg* pWndMessage = (CWndInstantMsg*) pWndChild;
			if( m_mapInstantMsg.Lookup( pWndMessage->m_strPlayer, (void*&) pWndBase ) == TRUE )
			{
				if( pWndBase != pWndMessage )
					ADDERRORMSG( "APP_INSTANTMSG_Vampyre" );
				m_mapInstantMsg.RemoveKey( pWndMessage->m_strPlayer );
				SAFE_DELETE( pWndBase );
				pWndMessage = NULL;
				//SAFE_DELETE( pWndMessage );
				//return;
			}
		}
	}
	if( m_bTitle == FALSE && bFocus )
		SetFocus();

#ifdef	__NQUESTITEM_INFO_040917
	SAFE_DELETE( m_pQuestItemInfo );
#endif//__NQUESTITEM_INFO_040917
#ifdef __V050322_CACHEQUEST
	SAFE_DELETE( m_pWndTextBook );
	SAFE_DELETE( m_pWndTextScroll );
	SAFE_DELETE( m_pWndTextLetter );
#endif // __V050322_CACHEQUEST

}	
#ifdef __V050322_CACHEQUEST
void CWndMgr::OpenTextBook( CWndBase* pWndParent , CItemBase* pItemBase )
{
	SAFE_DELETE( m_pWndTextBook );
	m_pWndTextBook = new CWndTextBook;
	m_pWndTextBook->Initialize( pWndParent, pItemBase );
}
void CWndMgr::OpenTextScroll( CWndBase* pWndParent, CItemBase* pItemBase )
{
	SAFE_DELETE( m_pWndTextScroll );
	m_pWndTextScroll = new CWndTextScroll;
	m_pWndTextScroll->Initialize( pWndParent, pItemBase );
}
void CWndMgr::OpenTextLetter( CWndBase* pWndParent, CItemBase* pItemBase )
{
	SAFE_DELETE( m_pWndTextLetter );
	m_pWndTextLetter = new CWndTextLetter;
	m_pWndTextLetter->Initialize( pWndParent, pItemBase );
}
void CWndMgr::ChangeTextBook( CItemBase* pItemBase )
{
	m_pWndTextBook->SetItemBase( pItemBase );
}
void CWndMgr::ChangeTextScroll( CItemBase* pItemBase )
{
	m_pWndTextScroll->SetItemBase( pItemBase );
}
void CWndMgr::ChangeTextLetter( CItemBase* pItemBase )
{
	m_pWndTextLetter->SetItemBase( pItemBase );
}
#endif // __V050322_CACHEQUEST
#ifdef	__NQUESTITEM_INFO_040917
void CWndMgr::OpenQuestItemInfo(CWndBase* pWndParent, CItemBase *pItemBase)
{
	SAFE_DELETE(m_pQuestItemInfo);
	m_pQuestItemInfo = new CWndQuestItemInfo;
	m_pQuestItemInfo->Initialize(pWndParent, pItemBase);
}

void CWndMgr::ChangeQuestItemInfo(CItemBase * pItemBase)
{
	m_pQuestItemInfo->SetItemBase(pItemBase);
}
#endif//__NQUESTITEM_INFO_040917

void CWndMgr::OpenTitle( BOOL bFirstTime )
{
	if( m_bTitle == TRUE )
		return;

	// 처음 실행하는게 아니라면 게임 중 로그아웃한 것이다. 그렇다면 현재 열려 있는 필드를 삭제한다.
	if( bFirstTime == FALSE )
	{
		g_Option.Save( "neuz.ini" );
		DestroyApplet();
		Free();
		g_WorldMng.DestroyCurrentWorld();
		g_pPlayer = NULL;
		CMover::SetActiveObj( NULL );

		g_dpCertified.DeleteDPObject();
		g_dpLoginClient.DeleteDPObject();
		g_DPlay.DeleteDPObject();
		
		if( g_pBipedMesh )
		{
			g_pBipedMesh->DeleteDeviceObjects();
			SAFE_DELETE( g_pBipedMesh );
		}
		g_Party.InitParty();
		g_GuildWarMng.Clear();
		g_GuildMng.Clear();
	}
	// 데스크탑 사이즈 세팅 
	g_WndMng.SetWndRect( CRect( 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT ) );
	m_bTitle = TRUE;

	ObjectExecutor( SHORTCUT_APPLET, APP_LOGIN );
	// Delete 
	//CloseTitle();
	//CloseField();
	// Open
	//OpenLogin();
	// Flag set
	// 배경 음악  
	PlayMusic( BGM_TITLE, 0 );
	CWndBase::m_nAlpha = 255;
	m_bTitle = TRUE;
}
void CWndMgr::OpenField()
{
#ifdef __S0126_MOTION_AUTORUN
	m_bAutoRun = FALSE;
#endif // __S0126_MOTION_AUTORUN
	
	if( m_bTitle == FALSE )
		return;
	StopMusic();
	for( int i = 0; i < 5; i++ )
	{
		if( g_Neuz.m_apPlayer[i] )
			g_Neuz.m_apPlayer[i]->m_pModel = NULL; 
		SAFE_DELETE( g_Neuz.m_apPlayer[i] );
	}
	g_DialogMsg.ClearAllMessage();
	// 데스크탑 사이즈 세팅 
	SetWndRect( g_Neuz.GetDeviceRect() );
	//jectExecutor( SHORTCUT_APPLET, APP_TASKBAR );
	// Delete 
	//CloseTitle();
	//CloseField();
	// Open
	OpenTaskBar();
	
	m_aChatString.RemoveAll();
	m_aChatColor.RemoveAll();
	m_aChatStyle.RemoveAll();

//#ifdef __ONLINE
	if( LoadRegInfo( "regInfo.dat" ) == FALSE )
	{
		ObjectExecutor( SHORTCUT_APPLET, APP_COMMUNICATION_CHAT );
		ObjectExecutor( SHORTCUT_APPLET, APP_NAVIGATOR );
		ObjectExecutor( SHORTCUT_APPLET, APP_STATUS );
		ObjectExecutor( SHORTCUT_APPLET, APP_WORLD );
		ObjectExecutor( SHORTCUT_APPLET, APP_MESSENGER_ );		
	}
	/*
#else
	ObjectExecutor( SHORTCUT_APPLET, APP_COMMUNICATION_CHAT );
	ObjectExecutor( SHORTCUT_APPLET, APP_NAVIGATOR         );
	ObjectExecutor( SHORTCUT_APPLET, APP_STATUS           );
	ObjectExecutor( SHORTCUT_APPLET, APP_WORLD              );
#endif	
	*/

#ifndef __YGUIDESYSTEM0923
	if( g_Option.m_bTip )
		ObjectExecutor( SHORTCUT_APPLET, APP_HELPER_TIP );	
#endif //__YGUIDESYSTEM0923

	// 공지가 enable이라면?
	if( g_Option.m_bNotice )
		ObjectExecutor( SHORTCUT_APPLET, APP_INFO_NOTICE );
	else
	// 공지가 열지 않음으로 되어있어도 날짜가 갱신되었다면 강제 오픈
	{
		CFileStatus fileStatus;
		
		CString strFilter;
		strFilter = GetLangFileName( g_Option.m_nLanguage, FILE_NOTICE );
		
		if( CFile::GetStatus( "Client\\"+strFilter, fileStatus ) == TRUE )
		{
			if( g_Option.m_tNoticeTime != fileStatus.m_mtime.GetTime() )
			{
				g_Option.m_tNoticeTime = fileStatus.m_mtime.GetTime();
				g_Option.m_bNotice = TRUE;
				ObjectExecutor( SHORTCUT_APPLET, APP_INFO_NOTICE );
			}
		}
	}
	CWndBase::m_nAlpha = g_Option.m_nWindowAlpha;

	//GetWndBase( APP_COMMUNICATION_MESSAGE )->SetVisible( TRUE );
	//GetApplet( APP_WORLD )->OnCommand( 10001, 0 );
//	ObjectExecutor( SHORTCUT_APPLET, APP_DIALOG             );
	//OpenWnd1();
	//OpenCharacter();
	//OpenChat();
	// Flag set
	m_bTitle = FALSE;
}
BOOL CWndMgr::OnCommand(UINT nID,DWORD dwMessage)
{
	return TRUE;
}
/*
CItem* CWndMgr::GetFocusItem()
{
	if(m_pWndEquipment) return m_pWndEquipment->GetFocusItem();
	return NULL;
}
*/
void CWndMgr::CloseMessageBox()
{
	//m_pWndMessageBox->Destroy();
	SAFE_DELETE( m_pWndMessageBox );
	SAFE_DELETE( m_pWndMessageBoxUpper );
}
BOOL CWndMgr::OpenCustomBox( LPCTSTR lpszMessage, CWndMessageBox* pWndMessageBox, CWndBase* pWndParent ) 
{ 
	//m_pWndMessageBox->Destroy();
	SAFE_DELETE( m_pWndMessageBox );
	m_pWndMessageBox = pWndMessageBox;
#ifdef __V050707_BUGFIX_MESSAGEBOX
	pWndParent = this;
#else
	if( pWndParent == NULL )
		pWndParent = this;
#endif
	if(pWndMessageBox->Initialize( pWndParent ) == FALSE)
//{
	//if( pWndMessageBox->Initialize( strMessage, this ) == FALSE ) 
	//if
	{
		SAFE_DELETE( m_pWndMessageBox );
		return TRUE; 
	}
	//if( lpszMessage )
	//	pWndMessageBox->SetTitle( lpszMessage );
	return FALSE;
}
BOOL CWndMgr::OpenMessageBox( LPCTSTR lpszMessage, UINT nType, CWndBase* pWndParent ) 
{ 
	SAFE_DELETE( m_pWndMessageBox );
	m_pWndMessageBox = new CWndMessageBox;
#ifdef __V050707_BUGFIX_MESSAGEBOX
	pWndParent = this;
#else
	if( pWndParent == NULL )
		pWndParent = this;
#endif
	if( m_pWndMessageBox->Initialize( lpszMessage, pWndParent, nType ) == FALSE) 
	{
		SAFE_DELETE( m_pWndMessageBox );
		return TRUE; 
	}
	return FALSE;
}

BOOL CWndMgr::OpenMessageBoxUpper( LPCTSTR lpszMessage, UINT nType, BOOL bPostLogoutMsg ) 
{ 
	CWndBase* pWndParent = NULL;
	SAFE_DELETE( m_pWndMessageBoxUpper );
	m_pWndMessageBoxUpper = new CWndMessageBoxUpper;
#ifdef __V050707_BUGFIX_MESSAGEBOX
	pWndParent = this;
#else
	if( pWndParent == NULL )
		pWndParent = this;
#endif
	if( m_pWndMessageBoxUpper->Initialize( lpszMessage, pWndParent, nType, bPostLogoutMsg ) == FALSE) 
	{
		SAFE_DELETE( m_pWndMessageBoxUpper );
		return TRUE; 
	}
	return FALSE;
}
BOOL CWndMgr::OpenTaskBar() 
{ 
	if(m_pWndTaskBar)
		return FALSE;
	m_pWndTaskBar = new CWndTaskBar;
	if(m_pWndTaskBar->Initialize(this) == FALSE) 
	{
		SAFE_DELETE( m_pWndTaskBar );
		return TRUE; 
	}
	return TRUE;
}
BOOL CWndMgr::OpenMenu() 
{ 
	if( m_pWndMenu )
	{
		if( m_pWndMenu->IsVisible() )
			m_pWndMenu->SetVisible( FALSE );
		else
		{
			m_pWndMenu->SetVisible( TRUE );
			m_pWndMenu->SetFocus();
		}
		return FALSE;
	}
	m_pWndMenu = new CWndTaskMenu;
	if( m_pWndMenu->Initialize( this ) == FALSE ) 
	{
		SAFE_DELETE( m_pWndMenu );
		return TRUE; 
	}
	return TRUE;
}
void CWndMgr::OnSize(UINT nType, int cx, int cy)
{
	//m_pWndMessageBox->OnSize(nType,cx,cy);

	//if(m_pWndTaskBar)
	//	m_pWndTaskBar->SetWndRect(CRect(0,CLIENT_HEIGHT-TASKBAR_HEIGHT,CLIENT_WIDTH,CLIENT_HEIGHT));

	//m_pWndMenu->OnSize(nType,cx,cy);
	//m_pWnd1->OnSize(nType,cx,cy);
	//m_pWndCharacter->OnSize(nType,cx,cy);
	//m_pWndChat->OnSize(nType,cx,cy);

	//m_pWndMessanger->OnSize(nType,cx,cy);
	//m_pWndMail->OnSize(nType,cx,cy);
	//m_pWndSound->OnSize(nType,cx,cy);

	CWndBase::OnSize(nType,cx,cy);
}

HRESULT CWndMgr::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	
#ifdef __YDEBUG
	m_texture.InvalidateDeviceObjects();
	m_texCommand.InvalidateDeviceObjects();
	m_texIcon.InvalidateDeviceObjects();
	m_texWnd.InvalidateDeviceObjects();
#endif //__YDEBUG
	return S_OK;
}

HRESULT CWndMgr::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	
#ifdef __YDEBUG
	m_texture.DeleteDeviceObjects();
	m_texCommand.DeleteDeviceObjects();
	m_texIcon.DeleteDeviceObjects();
	m_texWnd.DeleteDeviceObjects();
#endif //__YDEBUG
	return S_OK;
}

HRESULT CWndMgr::RestoreDeviceObjects()
{
	//if( g_WndMng.IsTitle() )
	//	g_WndMng.SetWndRect( CRect( 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT ) );
	//else
	//	g_WndMng.SetWndRect( CRect( 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT - TASKBAR_HEIGHT ) );
	CWndBase::RestoreDeviceObjects();
	
#ifdef __YDEBUG
	m_texture.RestoreDeviceObjects(m_pApp->m_pd3dDevice);
	m_texCommand.RestoreDeviceObjects(m_pApp->m_pd3dDevice);
	m_texIcon.RestoreDeviceObjects(m_pApp->m_pd3dDevice);
	m_texWnd.RestoreDeviceObjects(m_pApp->m_pd3dDevice);
#endif //__YDEBUG
	
	return 0;
}
void* CWndMgr::GetAppletMainFunc( DWORD dwIdApplet )
{
	AppletFunc* pApplet;
	if( m_mapAppletFunc.Lookup( dwIdApplet, (void*&) pApplet ) == TRUE)
		return pApplet->m_pFunc;
	/*
	int nCount = 0;
	while( m_appletFunc[ nCount ].m_pFunc )
	{
		if( m_appletFunc[nCount].m_dwIdApplet == dwIdApplet )
			return (void*)m_appletFunc[nCount].m_pFunc;
		nCount++;
	}
	*/
	return NULL;
}
AppletFunc* CWndMgr::GetAppletFunc( DWORD dwIdApplet )
{
	AppletFunc* pApplet;
	if( m_mapAppletFunc.Lookup( dwIdApplet, (void*&) pApplet ) == TRUE)
		return pApplet;
	/*
	int nCount = 0;
	while( m_appletFunc[ nCount ].m_pFunc )
	{
		if( m_appletFunc[nCount].m_dwIdApplet == dwIdApplet )
			return &m_appletFunc[nCount];
		nCount++;
	}
	*/
	return NULL;
}
DWORD CWndMgr::GetAppletId( TCHAR* lpszAppletName )
{
	AppletFunc* pApplet;
	DWORD dwIdApplet;
	POSITION pos = m_mapAppletFunc.GetStartPosition();
	while( pos )
	{
		m_mapAppletFunc.GetNextAssoc( pos, dwIdApplet, (void*&)pApplet );
		if( !_tcscmp( pApplet->m_pAppletName, lpszAppletName ) )
			return pApplet->m_dwIdApplet;
	}
	/*
	int nCount = 0;
	while( m_appletFunc[ nCount ].m_pFunc )
	{
		if( !strcmp( m_appletFunc[nCount].m_pAppletName, lpszAppletName ) )
			return m_appletFunc[nCount].m_dwIdApplet;
		nCount++;
	}
	*/
	return 0xffffffff;
}
CWndBase* CWndMgr::GetApplet( DWORD dwIdApplet )
{
	CWndBase* pWndBase;
	if( m_mapWndApplet.Lookup( dwIdApplet, (void*&)pWndBase ) == FALSE )
		return NULL;
	return pWndBase;
}
void CWndMgr::AddAppletFunc( LPVOID pAppletFunc, DWORD dwIdApplet, LPCTSTR lpszAppletName, LPCTSTR pszIconName, LPCTSTR lpszAppletDesc, CHAR cHotkey )
{
	AppletFunc* pApplet = new AppletFunc;
	pApplet->m_pFunc       = (CWndBase* (*)()) pAppletFunc;
	pApplet->m_dwIdApplet  = dwIdApplet ;
	pApplet->m_pAppletName = lpszAppletName;
	pApplet->m_pAppletDesc = lpszAppletDesc;
	pApplet->m_pszIconName = pszIconName; 
	pApplet->m_cHotkey = cHotkey; 
	m_mapAppletFunc.SetAt( dwIdApplet, pApplet );
}
CWndBase* CWndMgr::CreateApplet( DWORD dwIdApplet )
{
	CWndNeuz* pWndBase = NULL;
	AppletFunc* pAppletFunc = NULL;
	if( m_mapAppletFunc.Lookup( dwIdApplet, (void*&)pAppletFunc ) == FALSE )
	{
		// Applet이 존재하지 않을 경우 생성 
		pAppletFunc = GetAppletFunc( dwIdApplet );
	}
	if( pAppletFunc )
	{
#ifdef __YGUIDESYSTEM0923
		CWndWorld* pWndWorld = (CWndWorld*)GetWndBase( APP_WORLD );
		
		if( pWndWorld ) 
			pWndWorld->m_pWndGuideSystem->SendGuideMessage(pAppletFunc->m_dwIdApplet);
#endif //__YGUIDESYSTEM0923

		pWndBase = (CWndNeuz*)GetWndBase( pAppletFunc->m_dwIdApplet );
		if( pWndBase == NULL )
		{
			pWndBase = (CWndNeuz*)(*pAppletFunc->m_pFunc)();
			if( pWndBase )
			{
				if( pWndBase->Initialize( this, dwIdApplet ) == FALSE )  
				{
					SAFE_DELETE( pWndBase );
					return pWndBase;
				}
				LPWNDREGINFO pWndRegInfo;
				if( m_mapWndRegInfo.Lookup( dwIdApplet, (void*&)pWndRegInfo ) == TRUE )
				{
					if( pWndRegInfo->dwSize )
					{
						// load
						CAr ar( pWndRegInfo->lpArchive, pWndRegInfo->dwSize );
						((CWndNeuz*)pWndBase)->SerializeRegInfo( ar, pWndRegInfo->dwVersion );
					}
					if( pWndBase->IsWndStyle( WBS_THICKFRAME ) )
					{
						if( pWndRegInfo->dwWndSize == WSIZE_WINDOW )
						{
							((CWndNeuz*)pWndBase)->SetSizeWnd();
							pWndBase->SetWndRect( pWndRegInfo->rect );
						}								
						if( pWndRegInfo->dwWndSize == WSIZE_MAX )
							((CWndNeuz*)pWndBase)->SetSizeMax();
					}
					else
					{
						CRect wndRect = pWndBase->GetWindowRect();
						pWndBase->SetWndRect( 
							CRect( 
							pWndRegInfo->rect.left, 
							pWndRegInfo->rect.top, 
							pWndRegInfo->rect.left + wndRect.Width(), 
							pWndRegInfo->rect.top + wndRect.Height() 
							) );
					}							
					//pWndBase->SetWndRect( pWndRegInfo->rect );
				}
				//LPWNDAPPLET lpApplet = m_resMng.GetAt( dwIdApplet );
				//if( lpApplet ) 
				//	pWndBase->SetToolTip( m_resMng.GetAt( dwIdApplet )->strToolTip );
				//pWndBase->SetToolTip( pAppletFunc->m_pAppletDesc );
				m_mapWndApplet.SetAt( dwIdApplet, pWndBase );
			}
		}
		else
			pWndBase->SetFocus();
	}
	return pWndBase;
}
void CWndMgr::ObjectExecutor( DWORD dwShortcut, DWORD dwId, DWORD dwType )
{
	SHORTCUT shortcut;
	ZeroMemory( &shortcut, sizeof( shortcut ) );
	shortcut.m_dwShortcut = dwShortcut;
	shortcut.m_dwId = dwId;
	shortcut.m_dwType = dwType;
	ObjectExecutor( &shortcut);
}
void CWndMgr::ObjectExecutor( LPSHORTCUT pShortcut )
{
	if( pShortcut->m_dwShortcut == SHORTCUT_APPLET )
	{
		if( pShortcut->m_dwId == APP_QUIT)
		{
			OpenCustomBox( NULL, new CWndQuit );
		}
		else
		if( pShortcut->m_dwId == APP_LOGOUT)
		{
#ifdef __NPP_050308
			// 공격중이면 로그아웃 불가
			if( g_pPlayer->m_nAtkCnt && g_pPlayer->m_nAtkCnt < (SEC1 * 5) )
			{
				g_WndMng.PutString( prj.GetText(TID_PK_COMBAT_END),NULL,0xffff0000);//, CHATSTY_SYSTEM_CHAT );
				return;
			}
			else
			{
				OpenCustomBox( NULL, new CWndLogOut );
			}
#else //__NPP_050308
			{
				OpenCustomBox( NULL, new CWndLogOut );
			}		
#endif //__NPP_050308
		}
		else
		{
			CWndBase* pWndBase = (CWndBase*)g_WndMng.GetApplet( pShortcut->m_dwId );
			if( pWndBase )
			{
				pWndBase->Destroy();
				SetFocus();
			}
			else
			if( CreateApplet( pShortcut->m_dwId ) == FALSE )
			{
			}
		}
	}
	else if( pShortcut->m_dwShortcut == SHORTCUT_CHAT )
	{
		{
		//	m_timerDobe.Reset();
#ifdef __YGUIDESYSTEM0923
			CWndWorld* pWndWorld = (CWndWorld*)GetWndBase( APP_WORLD );
			
			if( pWndWorld ) 
				pWndWorld->m_pWndGuideSystem->SendGuideMessage(pShortcut->m_dwId);
#endif //__YGUIDESYSTEM0923
			g_WndMng.ParsingChat( pShortcut->m_szString );
		}
	}
#ifdef __YEMOTICON_0223
	else if( pShortcut->m_dwShortcut == SHORTCUT_EMOTICON )
	{
		TCHAR szCmd[1024] = { 0 };
		_tcscat( szCmd, "/" );
		_tcscat( szCmd, g_DialogMsg.m_EmiticonCmd[pShortcut->m_dwId].m_szCommand );
		g_WndMng.ParsingChat( szCmd );
	}
#endif //__YEMOTICON_0223	
	else if( pShortcut->m_dwShortcut == SHORTCUT_ITEM  )
	{
		if( CMover::GetActiveMover()->m_vtInfo.VendorIsVendor() )
			return;

		CItemBase* pItemBase = g_pPlayer->GetItemId( pShortcut->m_dwId );
		if( IsUsableItem( pItemBase ) )
		{
			CCtrl* pCtrl = (CCtrl*)g_WorldMng.Get()->GetObjFocus();
			DWORD dwObjId = NULL_ID;
			if( pCtrl && pCtrl->GetType() != OT_OBJ )
				dwObjId = pCtrl->GetId();

			BOOL bAble = TRUE;
			if( pItemBase )
			{
				if( pItemBase->GetProp()->dwItemKind1 == IK1_RIDE )	// 사용한 아이템이 탈것인가.
					if( g_pPlayer->m_pActMover->IsFly() == FALSE )		// Ground상태인가?
						if( g_pPlayer->m_pActMover->m_bGround == 0 )		// 땅에 발을 안붙이고 있다면 못탄다.
							bAble = FALSE;	// 아이템사용 실패.
			}

			if( bAble )	// 성공판정 났을때만 보냄.
			{
				/*
			#ifndef __ONLINE
				g_pPlayer->DoUseItem( MAKELONG( 0, pItemBase->m_dwObjId ), dwObjId );
			#else	// __ONLINE
				int nPart = pItemBase->GetProp()->dwParts;
				g_DPlay.SendDoUseItem( MAKELONG( 0, pItemBase->m_dwObjId ), dwObjId, nPart );
			#endif	// __ONLINE
			*/
				int nPart = pItemBase->GetProp()->dwParts;
				BOOL bEquiped	= g_pPlayer->m_Inventory.IsEquip( pItemBase->m_dwObjId );
				if( bEquiped )
					nPart	= pItemBase->m_dwObjIndex - g_pPlayer->m_Inventory.m_dwIndexNum;
				
				g_DPlay.SendDoUseItem( MAKELONG( 0, pItemBase->m_dwObjId ), dwObjId, nPart );
			}
		}
	}
	else if( pShortcut->m_dwShortcut == SHORTCUT_SKILL )
	{
		if( CMover::GetActiveMover()->m_vtInfo.VendorIsVendor() )
			return;
		if( pShortcut->m_dwType == 2 )
		{
			if( g_Party.m_nSizeofMember >= 2 && g_Party.m_nKindTroup == 1 )
			{
				ItemProp* pItemProp =  prj.GetPartySkill( pShortcut->m_dwId );
				if( pItemProp )
				{
					if( g_Party.IsLeader(g_pPlayer->m_idPlayer) )
					{
	#ifndef __PARTYDEBUG
						if( int(g_Party.GetLevel() - pItemProp->dwReqDisLV) >= 0 )
	#endif // __PARTYDEBUG
						{
	#ifndef __PARTYDEBUG
							if( (g_Party.GetPoint() - pItemProp->dwExp) >= 0 )
	#endif // __PARTYDEBUG
							{
								// 스킬사용

								if( pShortcut->m_dwId != ST_SCOUT )
								{
									ItemProp* pItemProp = prj.GetPartySkill( pShortcut->m_dwId );

									if( timeGetTime() >= m_dwSkillTime[pShortcut->m_dwId] + pItemProp->dwCircleTime )
									{
										g_DPlay.SendPartySkillUse( pShortcut->m_dwId );
										m_dwSkillTime[pShortcut->m_dwId] = timeGetTime();
									}
									else
									{
										/*
										#define ST_CALL                            1
										#define ST_BLITZ                           2
										#define ST_RETREAT                         3
										#define ST_SCOUT                           4
										#define ST_SPHERECIRCLE                    5
										#define ST_LINKATTACK                      6
										#define ST_FORTUNECIRCLE                   7
										#define ST_STRETCHING                      8
										#define ST_GIFTBOX                         9
										*/
										if( pShortcut->m_dwId != ST_CALL    &&
											pShortcut->m_dwId != ST_BLITZ   &&
											pShortcut->m_dwId != ST_RETREAT )
										{
											CString str;
											str.Format( prj.GetText( TID_GAME_NOT_TIME ), pItemProp->szName );
											g_WndMng.PutString( (LPCTSTR)str, NULL, prj.GetTextColor( TID_GAME_NOT_TIME ) );
										}
									}
								}
								else
								{
									// 스카웃 이므로
									CCtrl* pFocusObj = (CCtrl*)g_WorldMng()->GetObjFocus();
									CMover* pFocusMover = (CMover*)pFocusObj;
									if( pFocusMover )
									{
										if( pFocusMover->IsPlayer() )
											g_DPlay.SendPartyMemberRequest( g_pPlayer, pFocusMover->m_idPlayer, TRUE );
										else
											g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0052) ) );
//											g_WndMng.OpenMessageBox( "플레이어를 선택하여 주십시요" );
									}
									else g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0052) ) );
//									else g_WndMng.OpenMessageBox( "플레이어를 선택하여 주십시요" );
								}
							}
	#ifndef __PARTYDEBUG
							else g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0049) ) );
//							else g_WndMng.OpenMessageBox( _T("포인트가 모자라 스킬을사용할수 없습니다.") );
	#endif // __PARTYDEBUG
						}
	#ifndef __PARTYDEBUG
						else // 렙이 모자라 스킬을 사용할수 없음
							g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0019) ) );
//							g_WndMng.OpenMessageBox( _T("사용한 스킬은 배우지를 못했습니다.") );
	#endif // __PARTYDEBUG
					}
					else // 단장이 아니다
						g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0047) ) );
//  					g_WndMng.OpenMessageBox( _T("파티스킬은 단장만 사용할수 있습니다.") );
				}
			}
		}
		else
		{

			// 숏컷이 스킬일 경우

			UseSkillShortCut( pShortcut->m_dwType, pShortcut->m_dwId );

		}
	}
	else
	if( pShortcut->m_dwShortcut == SHORTCUT_MOTION )
	{
		{
			
			CWndBase* pWndBase	= GetWndBase( APP_VENDOREX );

			if( pWndBase )
			{
				// 상점창이 열린 상태에서는 모션 실행 금지

				return;
			}
		}
		MotionProp* pMotionProp = prj.GetMotionProp( pShortcut->m_dwId );
		if( pMotionProp->szName[ 0 ] )
		{
			strcpy( m_szTimerChat, pMotionProp->szName );
			m_timerChat.Set( 1000 );
		}
		bool fBehavior	= false;
		if( pShortcut->m_dwId == MOT_BASE_PICKUP )
			g_pPlayer->DoPickupItemAround();
		else		
		if( pShortcut->m_dwId == MOT_BASE_BATTLE )
		{
			if( g_pPlayer->m_pActMover->IsStateFlag( OBJSTAF_COMBAT ) ) {
				if( g_pPlayer->SendActMsg( OBJMSG_MODE_PEACE ) == 1 ) {
					fBehavior	= true;
				}
			}
			else {
				if( g_pPlayer->SendActMsg( OBJMSG_MODE_COMBAT ) == 1 ) {
					fBehavior	= true;
				}
			}
		}
		else
		// 앉기/서기 전환
		if( pShortcut->m_dwId == MOT_BASE_SIT )
		{
			if( g_pPlayer->m_pActMover->IsStateFlag( OBJSTAF_SIT ) ) {	// 앉아있는 상태일때
				if( FALSE == g_pPlayer->m_pActMover->IsActJump() &&
					FALSE == g_pPlayer->m_pActMover->IsActAttack() && 
					FALSE == g_pPlayer->m_pActMover->IsActDamage() &&
					FALSE == g_pPlayer->m_pActMover->IsAction() &&
					!( g_pPlayer->m_dwReqFlag & REQ_USESKILL ) )
				{
					g_DPlay.SendMotion( OBJMSG_STANDUP );
				}
			}
			else {
				if( FALSE == g_pPlayer->m_pActMover->IsActJump() &&
					FALSE == g_pPlayer->m_pActMover->IsActAttack() && 
					FALSE == g_pPlayer->m_pActMover->IsActDamage() &&
					FALSE == g_pPlayer->m_pActMover->IsAction() &&
					!( g_pPlayer->m_dwReqFlag & REQ_USESKILL ) )
				{
					g_DPlay.SendMotion( OBJMSG_SITDOWN );
				}
			}
		} 
		else
		if( pShortcut->m_dwId == MOT_BASE_COLLECT )
		{
			CObj *pFocus = g_pPlayer->GetWorld()->GetObjFocus();	// 현재 잡힌 타겟.
			g_pPlayer->CMD_SetCollect( pFocus );
		}
#ifdef __YESCAPE_MOTION
		else
		if( pShortcut->m_dwId == MOT_BASE_ESCAPE )
		{
			g_DPlay.SendDoEscape();
		}
#endif // xCollect0705
#ifdef __CHEER0615
		else
		if( pShortcut->m_dwId == MOT_BASE_CHEER )
		{
			CObj *pFocus = g_pPlayer->GetWorld()->GetObjFocus();	// 현재 잡힌 타겟.

			if( pFocus && pFocus->GetType() == OT_MOVER && ((CMover*)pFocus)->IsPlayer() )
			{
				g_DPlay.SendCheering( ((CMover*)pFocus)->GetId() );
				fBehavior	= false;
			}
			else
			{
				g_WndMng.PutString( prj.GetText( TID_CHEER_NO2 ), NULL, prj.GetTextColor( TID_CHEER_NO2 ) );
			}
		}
#endif // xCollect0705
		else
		{
			// 기타 이모션.
			MotionProp *pProp = prj.GetMotionProp( pShortcut->m_dwId );
			if( pProp )
			{
				int nMotion = pProp->dwMotion;
				int nPlayMode = 0;
				switch( pProp->dwPlay )
				{
				case 0:	nPlayMode = ANILOOP_1PLAY;	break;
				case 1:	nPlayMode = ANILOOP_CONT;	break;
				case 2:	nPlayMode = ANILOOP_LOOP;	break;
				default: ANILOOP_1PLAY;
				}

				if( g_pPlayer->SendActMsg( OBJMSG_MOTION, nMotion, nPlayMode ) == 1 )
				{
#ifdef __Y0414_MOTION
					switch(nMotion)
					{
						case MTI_POINTWAYNORTH:
							g_pPlayer->SetAngle(180.0f);
							break;
						case MTI_POINTWAYSOUTH:
							g_pPlayer->SetAngle(0.0f);
							break;
						case MTI_POINTWAYEAST:
							g_pPlayer->SetAngle(90.0f);
							break;
						case MTI_POINTWAYWEST:
							g_pPlayer->SetAngle(270.0f);
							break;
					}
#endif
					fBehavior	= true;
				}
			}
		}
				
#ifdef __ONLINE
		if( fBehavior ) {
			g_pPlayer->ClearDest();
  #ifdef __J0823
			g_DPlay.ClearPlayerDestPos();
  #endif	// __J0823
		}
		if( fBehavior )
			g_DPlay.SendPlayerBehavior();
#endif	// __ONLINE
		
	}
	else
	if( pShortcut->m_dwShortcut == SHORTCUT_OBJECT )
	{
		CMover* CMover = prj.GetMover( pShortcut->m_dwId );
		if( CMover )
		{
			CWndWorld* pWndWorld = (CWndWorld*) GetWndBase( APP_WORLD );
			//if( g_WorldMng()->GetObjFocus() != CMover )
			//	pWndWorld->m_bNewSelect = TRUE;
			g_WndMng.OpenMessage( pShortcut->m_szString );

			//pWndMessage->AddMessage( lpString );

			//g_WorldMng()->SetObjFocus( CMover );
			

			/*
			if( pObj->GetType() == OT_MOVER )
			{
				CMover* pMover = (CMover*) pObj;
				// 자신이라면 상태창  
				if( pMover == g_Neuz.m_pPlayer )
				{
					CreateApplet( APP_EQUIPMENT );
				}
				elseaaaaaaaaaaaaaaa
				// NPC라면 대화 혹은 전투 
				if( pMover->IsNPC() )
				{
					g_World.SetObjFocus( pMover );
					CMover* pMover = (CMover*) pObj;
					g_WndMng.CreateApplet( APP_DIALOG ); 					
					
					//CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
					//if( pWndWorld )
					//{
					//	if( pWndWorld->m_bBattle )
					//	{
						//}

				}
				else
				// 플레이어라면 쪽지 전송 
				{

				}
			}
			*/
		}
	}
	else
	if( pShortcut->m_dwShortcut == SHORTCUT_SKILLFUN )
	{
		g_WndMng.m_pWndWorld->UseSkill();
	}

}
BOOL CWndMgr::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	return TRUE;
	//CRect rect = GetClientRect();
	//rect.DeflateRect( 5, 5);
	for( int i = 0; i < m_awndShortCut.GetSize(); i++)
	{
		if( ((CWndBase*)m_awndShortCut.GetAt( i ))->GetWndId() == pShortcut->m_dwIndex )
		{

			((CWndBase*)m_awndShortCut.GetAt( i ))->Move( CPoint( point.x - 12, point.y - 12 ) );
			return TRUE;
		}
	}
	CWndButton* pWndButton = new CWndButton;
	pWndButton->Create( pShortcut->m_szString,WBS_MENUITEM,CRect( point.x - 10, point.y - 10, point.x + 10, point.y + 10),this, pShortcut->m_dwIndex);
	pWndButton->m_shortcut.m_dwShortcut = pShortcut->m_dwShortcut;
	pWndButton->AddWndStyle( WBS_MOVE );
	m_awndShortCut.Add( pWndButton );
	pWndButton->m_strToolTip = pWndButton->GetTitle();
	return TRUE;
}
BOOL CWndMgr::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	if( message == WNM_CLICKED )
	{
	//	CWndButton* pWndButton = (CWndButton*)pLResult;
	//	ObjectExecutor( pWndButton->m_n,  nID );
	}
	return TRUE;
}

/*
BOOL CWndMgr::WndMessageBoxToTitle(CString strMessage) 
{ 
	if(m_pWndMessageBox) delete m_pWndMessageBox;
	m_pWndMessageBox = new CWndToTitleBox;
	if(m_pWndMessageBox->Initialize(strMessage,this) == FALSE) 
	{
		delete m_pWndMessageBox;
		m_pWndMessageBox = NULL;
		return TRUE; 
	}
	return FALSE;
}


*/
BOOL CWndMgr::OnEraseBkgnd( C2DRender* p2DRender )
{
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
	// 월드가 없거나 풀화면이 아닐 때
	if( pWndWorld == NULL && m_bTitle ) //|| pWndWorld->m_nWinSize != WSIZE_MAX )
	{
		if( m_pWndActiveDesktop )
			m_pWndActiveDesktop->SetVisible( TRUE );
		//if( m_bTitle == FALSE )
		else
			m_pTheme->RenderDesktop( &g_Neuz.m_2DRender );
	}
	else
	if( pWndWorld )
	{
		//DWORD dwColor = CWorld::GetDiffuseColor();
		//m_pApp->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, dwColor /*D3DCOLOR_ARGB( 255, 255, 255, 255 )*/, 1.0f, 0 ) ;

		//if( pWndWorld->m_nWinSize != WSIZE_MAX )
		m_pApp->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOLOR_ARGB( 255, 0, 0, 0 ), 1.0f, 0 ) ;
		// 월드가 화면을 완전히 가리고 있을 때 
		if( m_pWndActiveDesktop )
			m_pWndActiveDesktop->SetVisible( FALSE );
	}
	else
		m_pApp->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOLOR_ARGB( 255, 0, 0, 0 ), 1.0f, 0 ) ;
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	return TRUE;
}
BOOL CWndMgr::Process()
{
#ifdef __YNEWPERSONSHOP_EX1026
	CWndRegVend* pWndRegVend = (CWndRegVend*)g_WndMng.GetWndBase(APP_VENDOREX_SELL);
	CWndRegVend* pWndVendorBuy = (CWndRegVend*)g_WndMng.GetWndBase(APP_VENDOREX_BUY);
	
	if( pWndRegVend || pWndVendorBuy )
	{
		return CWndBase::Process();
	}
#endif //__YNEWPERSONSHOP_EX1026
	
	
	if( g_pPlayer )
	{
	#ifdef __NPP_050308		
		if( g_bKeyTable[ VK_ESCAPE ] && m_pLogOutWaitting == NULL )
	#else //__NPP_050308
		if( g_bKeyTable[ VK_ESCAPE ] )
	#endif //__NPP_050308
		{
			//TRACE( "Push ESCAPE \n" );
			// 부활 매뉴 
			g_bKeyTable[ VK_ESCAPE ] = 0;
			if( g_pPlayer->IsDie() )
			{
				if( m_pWndRevival == NULL )
				{
					m_pWndRevival = new CWndRevival;
					m_pWndRevival->Initialize( this, 0 );
				}
			}
			else
			// 태스크 매뉴 
			if( m_pWndMenu == NULL || m_pWndMenu->IsVisible() == FALSE )
			{
				CWorld* pWorld = g_WorldMng();
				CObj* pObj = pWorld->GetObjFocus();
				if( pObj )
				{
					{
						pWorld->SetObjFocus(  NULL );
						pWorld->m_idObjFocusOld = NULL_ID;
						g_pPlayer->ClearDest();
						
						if( g_pPlayer->m_dwFlag & MVRF_TRACKING )	// 이미 실행중이면 해제.
						{
							g_pPlayer->m_dwFlag &= (~MVRF_TRACKING);		// 추적모드해제.
							g_pPlayer->m_idTracking = NULL_ID;
						}
							
					}
				}
				else
					g_WndMng.OpenMenu();
			}
		}
		if( m_szTimerChat[ 0 ] && m_timerChat.IsTimeOut() )
		{
			g_DPlay.SendChat( m_szTimerChat );
			m_szTimerChat[ 0 ] = 0;
			m_timerChat.Set( 0 );
		}

		if( m_bConnect && m_timerMessenger.IsTimeOut() )
		{
#ifdef __SDEBUG
			m_timerMessenger.Set( SEC( 20 ) );
#else // __SDEBUG
			m_timerMessenger.Set( MIN( 5 ) );


//			if( g_WndMng.m_Messenger.GetState() == FRS_ONLINE )
//				g_DPlay.SendSetState( FRS_AUTOABSENT );
			// 여기서 메세지를 보내야 함( 자리비움 상태 )
#endif // __SDEBUG
		}
	}


  #ifdef __NPP_050308
	if( m_pLogOutWaitting ) // 종료진행중이면 테스크바 슬롯체인지 불가능
		return CWndBase::Process();
  #endif //__NPP_050308


	if( m_pWndTaskBar )
	{
#ifdef __Y0420		
//		if( g_bKeyTable[ VK_BACK ] )	// 지훈씨가 빼라고 했음.
//		{
//			g_bKeyTable[ VK_BACK ] = 0;
//			g_Option.m_nInterface = !g_Option.m_nInterface;
//		}
		if( g_bKeyTable[ '1' ] )
		{
			g_bKeyTable[ '1' ] = 0;
			m_pWndTaskBar->SetItemSlot( 0 );
		}
		if( g_bKeyTable[ '2' ] )
		{
			g_bKeyTable[ '2' ] = 0;
			m_pWndTaskBar->SetItemSlot( 1 );
		}
		if( g_bKeyTable[ '3' ] )
		{
			g_bKeyTable[ '3' ] = 0;
			m_pWndTaskBar->SetItemSlot( 2 );
		}
		if( g_bKeyTable[ '4' ] )
		{
			g_bKeyTable[ '4' ] = 0;
			m_pWndTaskBar->SetItemSlot( 3 );
		}
		if( g_bKeyTable[ '1' ] )
		{
			g_bKeyTable[ '1' ] = 0;
			ObjectExecutor( SHORTCUT_APPLET, APP_HELPER_HELP );
		}
		if( g_bKeyTable[ '0' ] )
		{
			g_bKeyTable[ '0' ] = 0;
			ScreenCapture();
		}
		if( g_bKeyTable[ '9' ] )
		{
			g_bKeyTable[ '9' ] = 0;
			FILEOUT( "Postion.txt", "Pos x = %f, z = %f", g_pPlayer->GetPos().x, g_pPlayer->GetPos().z );
			PutString( prj.GetText(TID_ADMIN_COORDINATESAVE), NULL, prj.GetTextColor(TID_ADMIN_COORDINATESAVE) );
		}
		if( GetWndBase(APP_WEBBOX)
#ifdef __TAIWAN_WEBBOX
			|| GetWndBase(APP_WEBBOX2)
#endif	// __TAIWAN_WEBBOX
		)
		{
			if( !CWebBox::GetInstance()->Get_Start() )
				CWebBox::GetInstance()->Set_Start();
		}


#else // __Y0420
		if( g_bKeyTable[ VK_F1 ] )
		{
			g_bKeyTable[ VK_F1 ] = 0;
			m_pWndTaskBar->SetItemSlot( 0 );
		}
		if( g_bKeyTable[ VK_F2 ] )
		{
			g_bKeyTable[ VK_F2 ] = 0;
			m_pWndTaskBar->SetItemSlot( 1 );
		}
		if( g_bKeyTable[ VK_F3 ] )
		{
			g_bKeyTable[ VK_F3 ] = 0;
			m_pWndTaskBar->SetItemSlot( 2 );
		}
		if( g_bKeyTable[ VK_F4 ] )
		{
			g_bKeyTable[ VK_F4 ] = 0;
			m_pWndTaskBar->SetItemSlot( 3 );
		}
		//}
		//else
		//{	
		if( g_bKeyTable[ VK_F1 ] )
		{
			g_bKeyTable[ VK_F1 ] = 0;
			ObjectExecutor( SHORTCUT_APPLET, APP_HELPER_HELP );
		}
		if( g_bKeyTable[ VK_F11 ] )
		{
			g_bKeyTable[ VK_F11 ] = 0;
			ScreenCapture();
		}
		if( g_bKeyTable[ VK_F9 ] )
		{
			g_bKeyTable[ VK_F9 ] = 0;
			FILEOUT( "Postion.txt", "Pos x = %f, z = %f", g_pPlayer->GetPos().x, g_pPlayer->GetPos().z );
			//			PutString( "현재 좌표를 Postion.txt에 저장했습니다." );
			PutString( prj.GetText(TID_ADMIN_COORDINATESAVE), NULL, prj.GetTextColor(TID_ADMIN_COORDINATESAVE) );
			
		}
#endif // __Y0420
	}
	return CWndBase::Process();
}
void CWndMgr::SetPlayer( CMover* pMover )
{
	CWndBase* pWndBase;

	if( pMover != NULL )
	{
		if( pWndBase = GetApplet( APP_GAUGE ) )
		{
			pWndBase->SetTitle( pMover->GetName() );
			//( (CWndStatus*)pWndBase )->m_pModel	= prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, pMover->m_dwType, pMover->m_dwIndex, TRUE );
			//prj.m_modelMng.LoadMotion( ( (CWndStatus*)pWndBase )->m_pModel, pMover->m_dwType, pMover->m_dwIndex, 0 );
		}
		
		if( pWndBase = GetApplet( APP_SKILL ) )
		{
			( (CWndSkillTreeEx*)pWndBase )->InitItem( pMover->GetJob(), pMover->m_aJobSkill );
		}
		
		

		if( pWndBase = GetApplet( APP_INVENTORY ) )
		{
			( (CWndInventory*)pWndBase )->m_wndItemCtrl.InitItem( &pMover->m_Inventory, APP_INVENTORY );
		}

		g_Neuz.m_camera.SetPos( pMover->GetPos() );		//

		if( CMover::GetActiveMover() == NULL )
		{
			LPCTSTR szErr = Error( "SetPlayer : ActiveMover 없음" ); 
			ADDERRORMSG( szErr );
		}
		if( CMover::GetActiveMover()->m_pActMover == NULL )
		{
			LPCTSTR szErr = Error( "SetPlayer : ActionMover 없음" ); 
			ADDERRORMSG( szErr );
		}

		/*
		m_bLock = TRUE;
		CMover* pMoverTarget = (CMover*)g_WorldMng.Get()->GetObjFocus() ;
		if( !(pMoverTarget && CMover::GetActiveMover()->m_pActMover->IsFly()) ) {
			FLOAT fAngle = CMover::GetActiveMover()->GetAngle(); 
			m_fRotx=-fAngle;
		}
		*/
//		Error( "WorldName-%s, Targer-%s, ActMover-%s, ActionMover-%s\n"  )

		if( pMover->m_pActMover->IsFly() )
			g_Neuz.m_camera.Unlock();
		else
			g_Neuz.m_camera.Lock();

		// 월드가 가동하기 전에 프로젝션과 뷰 매트릭스를 만들어주어야 
		// 문제가 없다. CWndWorld는 매시지 처리기이기 때문에 미처 행렬들이
		// 세팅되기도 전에 우선된 매시지에서 행렬을 사용할 우려가 있다.(PickObject 따위들)
		//CWndWorld* pWndWorld = (CWndWorld*)GetWndBase( APP_WORLD );
		//pWndWorld->Projection( D3DDEVICE );
		g_Neuz.m_camera.Reset();
		g_Neuz.m_camera.Process( D3DDEVICE );
		g_Neuz.m_camera.Transform( D3DDEVICE, g_WorldMng.Get() );

	}
	else 
	{
		if( pWndBase = GetApplet( APP_SKILL ) )
		{
			( (CWndSkillTreeEx*)pWndBase )->InitItem( 0, NULL );
		}

		if( pWndBase = GetApplet( APP_GAUGE ) )
		{
			SAFE_DELETE( ( (CWndStatus*)pWndBase )->m_pModel );
		}
		if( pWndBase = GetApplet( APP_INVENTORY ) )
		{
			( (CWndInventory*)pWndBase )->m_wndItemCtrl.InitItem( NULL, APP_INVENTORY );
		}
	}
}
void CWndMgr::WordChange( CString& string )
{
	for( int i = 0; i < prj.m_aWordFilter.GetSize(); i++)
	{
		LPFILTER lpFilter = prj.m_aWordFilter.GetAt( i );
		int nFind;
		if( ( nFind = string.Find( lpFilter->m_szSrc ) ) >= 0 )
		{
			if( g_Option.m_nSlangWord == 1 ) 
			{
				TCHAR szWord[ 128 ];
				int nLength = strlen( lpFilter->m_szSrc );
				for( int i = 0; i < nLength; i++ )
					szWord[ i ] = '*';
				szWord[ i ] = 0;
				string.Replace( lpFilter->m_szSrc,szWord );
			}
			else
			{
				string.Replace( lpFilter->m_szSrc, lpFilter->m_szDst );
			}
		}
	}
}
// 챗에서 또는 매크로로 입력된 텍스트가 이 함수를 용도에 맞게 분류된다.
void CWndMgr::ParsingChat( CString string )
{
	string.TrimLeft( ' ' );
	if( string.IsEmpty() == TRUE )
		return;
	// 문장길이가 너무 길면 64바이트로 줄임.
	SetStrNull( string, 120 );
/*
	if( string.GetLength() > 120 )
		string = string.Left( 120 );
		*/
	if( string.Find("#l") != -1 )
	{
		int mCut = 0;
		mCut = string.Find('/');

		if( mCut != -1 )
		{
			string = string.Right( string.GetLength() - mCut );
		}
	}


	if( string[0] == '/' )
	{
		ParsingCommand( string.LockBuffer(), g_pPlayer );
		string.UnlockBuffer();
	}
	else 
	// 문자중에 /가 포함되어 있으면 영자 명령일 수 있다.
	// 영자 명령의 누수를 막기 위해 /가 포함되어 있으면 전송하지 않는다.
//	if( string.Find( '/' ) == -1 )
	{
		if( string != m_strChatBackup || m_timerDobe.IsTimeOut() || g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ))
		{
			m_timerDobe.Reset();
			m_strChatBackup = string;
			WordChange( string );
			// 문장길이가 너무 길면 64바이트로 줄임.
			SetStrNull( string, 120 );

			//if( string.GetLength() > 120 )
			//	string = string.Left( 120 );
			
			/*
			// 속어 체인져 
			for( int i = 0; i < prj.m_aWordFilter.GetSize(); i++)
			{
				LPFILTER lpFilter = prj.m_aWordFilter.GetAt( i );
				int nFind;
				if( ( nFind = string.Find( lpFilter->m_szSrc ) ) >= 0 )
				{
					if( g_Option.m_nSlangWord == 1 ) 
					{
						TCHAR szWord[ 128 ];
						int nLength = strlen( lpFilter->m_szSrc );
						for( int i = 0; i < nLength; i++ )
							szWord[ i ] = '*';
						szWord[ i ] = 0;
						string.Replace( lpFilter->m_szSrc,szWord );
					}
					else
					{
						string.Replace( lpFilter->m_szSrc, lpFilter->m_szDst );
					}
				}
			}
			*/
#ifdef __ONLINE
			g_DPlay.SendChat( (LPCSTR)string );
		//	Parsing( string );
#else
			CString strText;
			strText += g_pPlayer->GetName();
			strText += " : ";
			strText += string;
			g_WndMng.PutString( strText, g_pPlayer, 0xffffffff );
		//	Parsing( string );
#endif
		}
		else
		{
			CWndChat* pWndChat = (CWndChat*)g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
			if( pWndChat )
				pWndChat->PutString( prj.GetText(TID_GAME_CHATSAMETEXT), prj.GetTextColor(TID_GAME_CHATSAMETEXT) );
				//pWndChat->PutString( "연속으로 같은 내용을 입력할 수 없습니다.", 0xffffffa0 );
		}
	}
}
void CWndMgr::PutDefinedString( DWORD dwText, ... )
{
	TCHAR szBuffer[ 512 ];
	TCHAR szFormat[ 512 ];
	strcpy( szFormat, prj.GetText( dwText ) );
	va_list args;
	va_start( args, szFormat );
	int nBuf = _vsntprintf( szBuffer, 512, szFormat, args );
	va_end( args );

	DWORD dwColor	= prj.GetTextColor( dwText );
	g_WndMng.PutString( (LPCTSTR)szBuffer, NULL, dwColor );
}
/*
// Msg
m_bMsgSys
m_bMsgHelp
m_bMsg
// Chat
m_bChatGeneral
m_bChatSay
m_bChatParty
m_bChatGuild
*/
//DWORD g_Option.m_dwChatFilter = CHATSTY_GENERAL | CHATSTY_HELP | 
//	CHATSTY_SYSTEM | CHATSTY_GAME | CHATSTY_CLIENTMSG;
void CWndMgr::PutString( LPCTSTR lpszString, CObj* pObj, DWORD dwColor, DWORD dwChatStyle )
{
	CMover* pMover = (CMover*)pObj;
	CString str = lpszString;
	DWORD dwStyle = 0;
//	BOOL bChat = FALSE;

	// Text Message Style 
	#define TMS_CLIENT   0x00000001
	#define TMS_DIALOG   0x00000002
	#define TMS_CAPTION  0x00000004
	#define TMS_CHAT     0x00000008

#ifdef __VCHATFILTER_20050316
	int nChannel = 0;
	switch( CWndChat::m_nChatChannel )
	{
		case CHATSTY_GENERAL: nChannel = 0; break;
		case CHATSTY_WHISPER: nChannel = 1; break;
		case CHATSTY_SHOUT  : nChannel = 2; break;
		case CHATSTY_PARTY  : nChannel = 3; break;
		case CHATSTY_GUILD  : nChannel = 4; break;
	}
	DWORD dwChatFilter = g_Option.m_dwChatFilter[ nChannel ];
	if( ( dwChatFilter & CHATSTY_GENERAL       ) == dwChatStyle ) { dwStyle |= TMS_CHAT; } 
	if( ( dwChatFilter & CHATSTY_WHISPER       ) == dwChatStyle ) { dwStyle |= TMS_CHAT; } 
	if( ( dwChatFilter & CHATSTY_SHOUT         ) == dwChatStyle ) { dwStyle |= TMS_CHAT; } 
	if( ( dwChatFilter & CHATSTY_PARTY         ) == dwChatStyle ) { dwStyle |= TMS_CHAT; } 
	if( ( dwChatFilter & CHATSTY_GUILD         ) == dwChatStyle ) { dwStyle |= TMS_CHAT; } 
#ifdef __V050523_BUGFIX_CHATFILTER
	DWORD dwChatFilterEtc = g_Option.m_dwChatFilterEtc;
	if( ( dwChatFilterEtc & CHATSTY_SYSTEM_CLIENT ) && ( dwChatStyle == CHATSTY_SYSTEM ) ) { dwStyle |= TMS_CLIENT; dwChatStyle = CHATSTY_SYSTEM_CHAT; }
	if( ( dwChatFilterEtc & CHATSTY_SYSTEM_CHAT   ) && ( dwChatStyle == CHATSTY_SYSTEM ) ) { dwStyle |= TMS_CHAT  ; dwChatStyle = CHATSTY_SYSTEM_CHAT; }
	if( ( dwChatFilterEtc & CHATSTY_HELP_CLIENT   ) && ( dwChatStyle == CHATSTY_HELP   ) ) { dwStyle |= TMS_CLIENT; dwChatStyle = CHATSTY_HELP_CHAT  ; }   
	if( ( dwChatFilterEtc & CHATSTY_HELP_CHAT     ) && ( dwChatStyle == CHATSTY_HELP   ) ) { dwStyle |= TMS_CHAT  ; dwChatStyle = CHATSTY_HELP_CHAT  ; }  
	if( ( dwChatFilterEtc & CHATSTY_GAME_CLIENT   ) && ( dwChatStyle == CHATSTY_GAME   ) ) { dwStyle |= TMS_CLIENT; dwChatStyle = CHATSTY_GAME_CHAT  ; }  
	if( ( dwChatFilterEtc & CHATSTY_GAME_CHAT     ) && ( dwChatStyle == CHATSTY_GAME   ) ) { dwStyle |= TMS_CHAT  ; dwChatStyle = CHATSTY_GAME_CHAT  ; } 
	BOOL bRecord = TRUE; // 무조건 기록. 나중에 이 변수 없애자.
#else
//{{AFX
	if( ( dwChatFilter & CHATSTY_SYSTEM_CLIENT ) && ( dwChatStyle == CHATSTY_SYSTEM ) ) { dwStyle |= TMS_CLIENT; } 
	if( ( dwChatFilter & CHATSTY_SYSTEM_CHAT   ) && ( dwChatStyle == CHATSTY_SYSTEM ) ) { dwStyle |= TMS_CHAT  ; } 
	if( ( dwChatFilter & CHATSTY_HELP_CLIENT   ) && ( dwChatStyle == CHATSTY_HELP   ) ) { dwStyle |= TMS_CLIENT; }  
	if( ( dwChatFilter & CHATSTY_HELP_CHAT     ) && ( dwChatStyle == CHATSTY_HELP   ) ) { dwStyle |= TMS_CHAT  ; } 
	if( ( dwChatFilter & CHATSTY_GAME_CLIENT   ) && ( dwChatStyle == CHATSTY_GAME   ) ) { dwStyle |= TMS_CLIENT; } 
	if( ( dwChatFilter & CHATSTY_GAME_CHAT     ) && ( dwChatStyle == CHATSTY_GAME   ) ) { dwStyle |= TMS_CHAT  ; }
	BOOL bRecord = FALSE;
	if( ( dwChatStyle & CHATSTY_GENERAL ) ) { bRecord = TRUE; } 
	if( ( dwChatStyle & CHATSTY_WHISPER ) ) { bRecord = TRUE; } 
	if( ( dwChatStyle & CHATSTY_SHOUT   ) ) { bRecord = TRUE; } 
	if( ( dwChatStyle & CHATSTY_PARTY   ) ) { bRecord = TRUE; } 
	if( ( dwChatStyle & CHATSTY_GUILD   ) ) { bRecord = TRUE; } 

	if( ( dwChatStyle & CHATSTY_SYSTEM  ) && ( dwChatFilter & CHATSTY_SYSTEM_CHAT ) ) { bRecord = TRUE; }
	if( ( dwChatStyle & CHATSTY_HELP    ) && ( dwChatFilter & CHATSTY_HELP_CHAT   ) ) { bRecord = TRUE; }
	if( ( dwChatStyle & CHATSTY_GAME    ) && ( dwChatFilter & CHATSTY_GAME_CHAT   ) ) { bRecord = TRUE; }
//}}AFX
#endif

#else // __VCHATFILTER_20050316
//{{AFX
	if( ( g_Option.m_dwChatFilter & CHATSTY_GENERAL       ) == dwChatStyle ) { dwStyle |= TMS_CHAT; } 
	if( ( g_Option.m_dwChatFilter & CHATSTY_WHISPER       ) == dwChatStyle ) { dwStyle |= TMS_CHAT; } 
	if( ( g_Option.m_dwChatFilter & CHATSTY_SHOUT         ) == dwChatStyle ) { dwStyle |= TMS_CHAT; } 
	if( ( g_Option.m_dwChatFilter & CHATSTY_PARTY         ) == dwChatStyle ) { dwStyle |= TMS_CHAT; } 
	if( ( g_Option.m_dwChatFilter & CHATSTY_GUILD         ) == dwChatStyle ) { dwStyle |= TMS_CHAT; } 
	if( ( g_Option.m_dwChatFilter & CHATSTY_SYSTEM_CLIENT ) && ( dwChatStyle == CHATSTY_SYSTEM ) ) { dwStyle |= TMS_CLIENT; } 
	if( ( g_Option.m_dwChatFilter & CHATSTY_SYSTEM_CHAT   ) && ( dwChatStyle == CHATSTY_SYSTEM ) ) { dwStyle |= TMS_CHAT  ; } 
	if( ( g_Option.m_dwChatFilter & CHATSTY_HELP_CLIENT   ) && ( dwChatStyle == CHATSTY_HELP   ) ) { dwStyle |= TMS_CLIENT; }  
	if( ( g_Option.m_dwChatFilter & CHATSTY_HELP_CHAT     ) && ( dwChatStyle == CHATSTY_HELP   ) ) { dwStyle |= TMS_CHAT  ; } 
	if( ( g_Option.m_dwChatFilter & CHATSTY_GAME_CLIENT   ) && ( dwChatStyle == CHATSTY_GAME   ) ) { dwStyle |= TMS_CLIENT; } 
	if( ( g_Option.m_dwChatFilter & CHATSTY_GAME_CHAT     ) && ( dwChatStyle == CHATSTY_GAME   ) ) { dwStyle |= TMS_CHAT  ; }
	BOOL bRecord = FALSE;
	if( ( dwChatStyle & CHATSTY_GENERAL ) ) { bRecord = TRUE; } 
	if( ( dwChatStyle & CHATSTY_WHISPER ) ) { bRecord = TRUE; } 
	if( ( dwChatStyle & CHATSTY_SHOUT   ) ) { bRecord = TRUE; } 
	if( ( dwChatStyle & CHATSTY_PARTY   ) ) { bRecord = TRUE; } 
	if( ( dwChatStyle & CHATSTY_GUILD   ) ) { bRecord = TRUE; } 

	if( ( dwChatStyle & CHATSTY_SYSTEM  ) && ( g_Option.m_dwChatFilter & CHATSTY_SYSTEM_CHAT ) ) { bRecord = TRUE; }
	if( ( dwChatStyle & CHATSTY_HELP    ) && ( g_Option.m_dwChatFilter & CHATSTY_HELP_CHAT   ) ) { bRecord = TRUE; }
	if( ( dwChatStyle & CHATSTY_GAME    ) && ( g_Option.m_dwChatFilter & CHATSTY_GAME_CHAT   ) ) { bRecord = TRUE; }
//}}AFX
#endif // __VCHATFILTER_20050316
	CWndChat* pWndChat = (CWndChat*)g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
	CEditString* pChatString = &m_ChatString;

	//
	//  예외 상황 처리(가능상황에서 정해진것이 여기서 무효가 될 수도 있다.)
	//
	if( pObj == NULL ) // 오브젝트가 널이면 말풍선 금지 
		dwStyle &= ~TMS_DIALOG; 
	else
	if( pObj->GetType() == OT_MOVER )
	{
		if( dwChatStyle & CHATSTY_GENERAL )
			dwStyle |= TMS_DIALOG;
		if( ( (CMover*)pObj )->IsPlayer() == FALSE ) // 플래이어가 아니라면 챗 금지 
		{
			dwStyle &= ~TMS_CHAT;
			bRecord = FALSE;
		}
		if( ( (CMover*)pObj )->m_vtInfo.IsVendorOpen() ) // 개인 상점이면 말풍선 금지 
			dwStyle &= ~TMS_DIALOG;
	}		
	/*******************************************
	    출력 
	*/
	// 클라이언트 매시지 
	if( dwStyle & TMS_CLIENT )
		g_ClientMsg.AddMessage( lpszString, dwColor );

	// 채팅 매시지, bRecord가 TRUE이면 채팅 창에 출력할 매시지라는 의미 (반드시 현재 채널에 출력되는 것은 아님)
	CString str1	= lpszString;
	str1.Replace( "\n", " " );
	str1.Replace( '\n', ' ' );
	if( bRecord )
	{
		// 글 내용이 1000줄을 넘어가면 500라인을 잘라낸다.
		if( m_aChatString.GetSize() > 500 )
		{
			m_aChatString.RemoveAt( 0, 250 );
			m_aChatColor.RemoveAt( 0, 250 );
			m_aChatStyle.RemoveAt( 0, 250 );
		}
		m_aChatString.Add( str1 );
//		m_aChatString.Add( lpszString );
		m_aChatColor.Add( dwColor );
		m_aChatStyle.Add( dwChatStyle );
	}
	if( dwStyle & TMS_CHAT )
	{
		if( pWndChat )
			pWndChat->PutString( str1, dwColor );
//			pWndChat->PutString( lpszString, dwColor );
		//else
		//{
		//	m_ChatString.AddString( lpszString, dwColor );
		//	m_ChatString.AddString( "\n", dwColor );// += '\n';
		//}
	}		
	// 캡션 매시지 
	if( dwStyle & TMS_CAPTION )
	{
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	#ifndef __VTITLECAPTION_JAPAN
		g_Caption1.AddCaption( lpszString, pWndWorld->m_pFontAPITitle );
	#else //__VTITLECAPTION_JAPAN
		g_Caption1.AddCaption( lpszString, NULL );
	#endif //__VTITLECAPTION_JAPAN
	}
	// 다이얼로그 매시지 
	if( dwStyle & TMS_DIALOG )
	{
		if( pObj->GetType() == OT_MOVER )// && ((CMover*)pObj)->m_nQuestEmoticonIndex == -1 )
		{
			TCHAR* lpszChat	= _tcschr( lpszString, _T( ':' ) );
			lpszChat += sizeof(TCHAR) * 2;	// Skip ": "
			g_DialogMsg.AddMessage( pObj, lpszChat, 0xffffffff );
		}
	}
}

void CWndMgr::PutToolTip_Skill( DWORD dwSkill, DWORD dwLevel, CPoint point, CRect* pRect, BOOL bActive )
{
	ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
	if( pSkillProp == NULL )
	{
		Error( "CWndMgr::PutToolTip_Skill : 스킬(%d)의 프로퍼티가 업다.", dwSkill );
		return;	// property not found
	}

	AddSkillProp* pAddSkillProp = prj.GetAddSkillProp( pSkillProp->dwSubDefine, dwLevel );
	if( pAddSkillProp == NULL )
	{
		Error( "CWndMgr::PutToolTip_Skill : add스킬(%d)의 프로퍼티가 업다.", dwSkill );
		return;	// property not found
	}

	DWORD dwaf = D3DCOLOR_XRGB( 199, 155,   0 );
	CString str;
	CString strstr;
	CString string;
#ifdef __TOOLTIPTEXTURE
	if( bActive )
		string.Format( "#b#cff2fbe6d%s   Lv %d#nb#nc", pSkillProp->szName, dwLevel );
	else
		string.Format( "#b#cff2fbe6d%s#nb#nc", pSkillProp->szName );
#else // __TOOLTIPTEXTURE
	string.Format( "#b#cff0000ff%s#nb#nc", pSkillProp->szName );
#endif // __TOOLTIPTEXTURE
	char statebuf[30] = {0,};
	if( pAddSkillProp->nReqMp != 0xffffffff )	
	{
		sprintf( statebuf, "\nMP : %d", g_pPlayer->GetReqMp( pAddSkillProp->nReqMp ) );
		string += statebuf;
	}
	if( pAddSkillProp->nReqFp != 0xffffffff )	
	{
		sprintf( statebuf, "\nFP : %d", g_pPlayer->GetReqFp( pAddSkillProp->nReqFp ) );
		string += statebuf;
	}
	
#ifndef __TOOLTIPTEXTURE
	if( bActive )
	{
		strstr.Format( prj.GetText(TID_GAME_SKILLLEVEL), dwLevel );
		str.Format( "\n%s", strstr );
		string += str;
	}
#endif // __TOOLTIPTEXTURE
	
	strstr.Format( prj.GetText(TID_GAME_BASESKILLLEVEL), pSkillProp->dwReqDisLV );
	if( g_pPlayer->GetLevel() < pSkillProp->dwReqDisLV )
		str.Format( "\n#cffff0000%s#nc", strstr );
	else
		str.Format( "\n%s", strstr );
	string += str;

	if( pSkillProp->dwReSkill1 != -1 )
	{
		LPSKILL pskill = g_pPlayer->GetSkill(pSkillProp->dwReSkill1);

		if( pskill )
		{
			if( pskill->dwLevel < pSkillProp->dwReSkillLevel1 )
			{
				strstr.Format( prj.GetText(TID_GAME_SKILLDIS), pskill->GetProp()->szName, pSkillProp->dwReSkillLevel1 );
				str.Format( "\n#cffff0000%s#nc", strstr );
				string += str;
			}
		}
	}
	if( pSkillProp->dwReSkill2 != -1 )
	{
		LPSKILL pskill = g_pPlayer->GetSkill(pSkillProp->dwReSkill2);
		
		if( pskill )
		{
			if( pskill->dwLevel < pSkillProp->dwReSkillLevel2 )
			{
				strstr.Format( prj.GetText(TID_GAME_SKILLDIS), pskill->GetProp()->szName, pSkillProp->dwReSkillLevel2 );
				str.Format( "\n#cffff0000%s#nc", strstr );
				string += str;
			}
		}
	}


	if( _stricmp( pSkillProp->szCommand, "=" ) )
	{
//#ifdef __TOOLTIPTEXTURE
//		str.Format( "\n#cffc79b00%s#nc", pSkillProp->szCommand );
//		string += str;
//#else // __TOOLTIPTEXTURE
		string += "\n";
		string += pSkillProp->szCommand;
//#endif // __TOOLTIPTEXTURE
	}
	
#ifdef __YCOOLTIME_0214	
	int nSkillIdx = g_pPlayer->GetSkillIdx( dwSkill );

	if( nSkillIdx != -1 )
	{
		if( g_pPlayer->m_tmReUseDelay[ nSkillIdx ] > 0 )
		{
			CTimeSpan ct( (long)(g_pPlayer->m_tmReUseDelay[ nSkillIdx ] / 1000.0f) );		// 남은시간을 초단위로 변환해서 넘겨줌
		#ifdef __ITEMCOOLTIME_VER2_050308
			str.Format( prj.GetText(TID_TOOLTIP_COOLTIME), ct.GetMinutes(), ct.GetSeconds() );		// 남은시간을 분/초 형태로 출력.
			string += "\n";
			string += str;
		#else
			str.Format( "\nCoolTime : %.2d:%.2d", ct.GetMinutes(), ct.GetSeconds() );		// 남은시간을 분/초 형태로 출력.
			string += str;
		#endif
		}	
	}
#endif //__YCOOLTIME_0214
	
	CEditString strEdit;
	strEdit.SetParsingString( string );
	
	g_toolTip.PutToolTip( dwSkill, strEdit, *pRect, point, 0 );
}
void CWndMgr::PutToolTip_Troupe( DWORD dwSkill, CPoint point, CRect* pRect )
{
	ItemProp* pSkillProp = prj.GetPartySkill( dwSkill );
	
	if( pSkillProp == NULL )
		return;
	
	CString string;
#ifdef __TOOLTIPTEXTURE
	string.Format( "#b#cff2fbe6d%s#nb#nc", pSkillProp->szName );
#else // __TOOLTIPTEXTURE
	string.Format( "#b#cff0000ff%s#nb#nc", pSkillProp->szName );
#endif // __TOOLTIPTEXTURE

	//string = pSkillProp->szName;
	char statebuf[30] = {0,};
	if( pSkillProp->dwReqMp != 0xffffffff )	
	{
		sprintf( statebuf, "\nMP : %d", pSkillProp->dwReqMp );
		string += statebuf;
	}
	if( pSkillProp->dwReqFp != 0xffffffff )	
	{
		sprintf( statebuf, "\nFP : %d", pSkillProp->dwReqFp );
		string += statebuf;
	}
	if( _stricmp( pSkillProp->szCommand, "=" ) )
	{
		CString str;
//#ifdef __TOOLTIPTEXTURE
//		str.Format( "\n#cffc79b00%s#nc", pSkillProp->szCommand );
//		string += str;
//#else // __TOOLTIPTEXTURE
		string += "\n";
		string += pSkillProp->szCommand;
//#endif // __TOOLTIPTEXTURE
	}
	CEditString strEdit;
	strEdit.SetParsingString( string );
	
	g_toolTip.PutToolTip( dwSkill, strEdit, *pRect, point, 0 );
}
void CWndMgr::PutToolTip_Item( DWORD dwType, DWORD dwId, CPoint point, CRect* pRect, int flag )
{
	CItemBase* pItemBase = g_pPlayer->GetItemId( dwId );
	PutToolTip_Item( pItemBase, point, pRect, flag );
}

struct DST_STRING
{
	int nDstParam;		// ex) DST_STR
//  char *szName;	// ex) "힘"
	int nNameIndex;
};

const DST_STRING g_DstString[] = 
{
	DST_STR                     , TID_TOOLTIP_STR, 
	DST_DEX                     , TID_TOOLTIP_DEX,
	DST_INT                     , TID_TOOLTIP_INT,
	DST_STA                     , TID_TOOLTIP_STA,
//	DST_GENDER                  , TID_TOOLTIP_GENDER,
	DST_SPEED                   , TID_TOOLTIP_SPEED,

	DST_ABILITY_MIN             , TID_TOOLTIP_ABILITYMIN,
	DST_ABILITY_MAX             , TID_TOOLTIP_ABILITYMAX,

	DST_ATTACKSPEED             , TID_TOOLTIP_ATKSPEED,
	DST_ADJDEF                  , TID_TOOLTIP_DEFENCE,

	DST_RESIST_MAGIC            , TID_TOOLTIP_DEFMAGIC, 
	DST_RESIST_ELECTRICITY      , TID_TOOLTIP_DEFELECTRICITY,
#ifdef __X0715
	DST_RESIST_ALL              , TID_TOOLTIP_DEFALLELEMETAL,
#else
	DST_RESIST_DARK             , TID_TOOLTIP_DEFDARK,
#endif
	DST_RESIST_FIRE             , TID_TOOLTIP_DEFFIFE,
	DST_RESIST_WIND             , TID_TOOLTIP_DEFWIND,
	DST_RESIST_WATER            , TID_TOOLTIP_DEFWATER, 
	DST_RESIST_EARTH            , TID_TOOLTIP_DEFEARTH,

	DST_HP_MAX                  , TID_TOOLTIP_MAXHP,
	DST_MP_MAX                  , TID_TOOLTIP_MAXMP,
	DST_FP_MAX                  , TID_TOOLTIP_MAXFP,
	DST_HP                      , TID_TOOLTIP_HP,
	DST_MP                      , TID_TOOLTIP_MP,
	DST_FP                      , TID_TOOLTIP_FP,
	DST_HP_RECOVERY             , TID_TOOLTIP_HPRECOVERY,
	DST_MP_RECOVERY             , TID_TOOLTIP_MPRECOVERY,
	DST_FP_RECOVERY             , TID_TOOLTIP_FPRECOVERY,
	DST_HP_RECOVERY_RATE         , TID_TOOLTIP_HPRECOVERYRATE,
	DST_MP_RECOVERY_RATE         , TID_TOOLTIP_MPRECOVERYRATE,
	DST_FP_RECOVERY_RATE         , TID_TOOLTIP_FPRECOVERYRATE,
#ifdef __S_DST_ADD
	DST_ALL_RECOVERY			, TID_TOOLTIP_ALL_RECOVERY,		
	DST_ALL_RECOVERY_RATE		, TID_TOOLTIP_ALL_RECOVERY_RATE,
	DST_KILL_HP					, TID_TOOLTIP_KILL_HP,		
	DST_KILL_MP					, TID_TOOLTIP_KILL_MP,					
	DST_KILL_FP					, TID_TOOLTIP_KILL_FP,					
	DST_KILL_ALL				, TID_TOOLTIP_KILL_ALL,				
	DST_KILL_HP_RATE			, TID_TOOLTIP_KILL_HP_RATE,
	DST_KILL_MP_RATE			, TID_TOOLTIP_KILL_MP_RATE,			
	DST_KILL_FP_RATE			, TID_TOOLTIP_KILL_FP_RATE,			
	DST_KILL_ALL_RATE			, TID_TOOLTIP_KILL_ALL_RATE,			
	DST_ALL_DEC_RATE			, TID_TOOLTIP_ALL_DEC_RATE,
#endif // __S_DST_ADD

	DST_ADJ_HITRATE             , TID_TOOLTIP_HITRATE,
	DST_ADJ_SPELLRATE           , TID_TOOLTIP_MAGICHITRATE,

//	DST_ENDURANCE               , TID_TOOLTIP_ENDURANCE,
	DST_ABRASION                , TID_TOOLTIP_ABRASION,
	DST_GROGGY                  , TID_TOOLTIP_STUN,

	DST_CHR_DMG                 , TID_TOOLTIP_CHRDMG,

	DST_CHRSTATE                , TID_TOOLTIP_CHRSTATE,
	DST_PARRY                   , TID_TOOLTIP_PARRY,
	DST_ATKPOWER_RATE           , TID_TOOLTIP_ATKPOWER,
	DST_REATTACKDELAY           , TID_TOOLTIP_REATTACKDELAY,

	DST_JUMPING                 , TID_TOOLTIP_JUMPING,
//	DST_AIRJUMP                 , TID_TOOLTIP_AIRJUMP,
	DST_BLOCK_MELEE				, TID_GAME_TOOLTIP_BLOCK,
	DST_BLOCK_RANGE				, TID_GAME_TOOLTIP_BLOCKRANGE,

	DST_STAT_ALLUP				, TID_GAME_TOOLTIPALLSTAT,
	DST_HP_MAX_RATE				, TID_TOOLTIP_DST_HP_MAX_RATE,
	DST_ADDMAGIC				, TID_GAME_TOOLTIPCONDITIONRATE,
	DST_ADJDEF_RATE				, TID_TOOLTIP_DST_ADJDEF_RATE,
	DST_HP_MAX_RATE				, TID_TOOLTIP_DST_HP_MAX_RATE,
	DST_MP_MAX_RATE				, TID_TOOLTIP_DST_MP_MAX_RATE, 
	DST_FP_MAX_RATE				, TID_TOOLTIP_DST_FP_RATE,
	DST_CHR_CHANCECRITICAL		, TID_TOOLTIP_DST_CRITICAL_RATE,

	DST_CHR_WEAEATKCHANGE		,	TID_TOOLTIP_DST_CHR_WEAEATKCHANGE,
	DST_MASTRY_EARTH			,	TID_TOOLTIP_DST_MASTRY_EARTH,
	DST_MASTRY_FIRE				,	TID_TOOLTIP_DST_MASTRY_FIRE,
	DST_MASTRY_WATER			,	TID_TOOLTIP_DST_MASTRY_WATER,
	DST_MASTRY_ELECTRICITY		,	TID_TOOLTIP_DST_MASTRY_ELECTRICITY,
	DST_MASTRY_WIND				,	TID_TOOLTIP_DST_MASTRY_WIND,
	DST_REFLECT_DAMAGE			,	TID_TOOLTIP_DST_REFLECT_DAMAGE,

	DST_MP_DEC_RATE					,	TID_TOOLTIP_DST_MP_DEC_RATE,
	DST_FP_DEC_RATE					,	TID_TOOLTIP_DST_FP_DEC_RATE,
	DST_SPELL_RATE					,	TID_TOOLTIP_DST_SPELL_RATE,
	DST_CAST_CRITICAL_RATE		,	TID_TOOLTIP_DST_CAST_CRITICAL_RATE,
	DST_CRITICAL_BONUS,		TID_TOOLTIP_DST_CRITICAL_BONUS,
	DST_YOY_DMG,		TID_TOOLTIP_DST_YOY_DMG,
	DST_BOW_DMG,		TID_TOOLTIP_DST_BOW_DMG,
	DST_KNUCKLE_DMG,		TID_TOOLTIP_DST_KNUCKLE_DMG,
	DST_SWD_DMG,		TID_TOOLTIP_DST_SWD_DMG,
	DST_AXE_DMG,		TID_TOOLTIP_DST_AXE_DMG
#ifdef __NEWWPN1024
	,DST_MASTRY_ALL,	TID_TOOLTIP_DST_MASTRY_ALL
#endif	// __NEWWPN1024
};

#ifdef __S_DST_ADD
#ifdef __NEWWPN1024
#define MAX_DST_RATE		39
#else	// __NEWWPN1024
#define MAX_DST_RATE		38
#endif	// __NEWWPN1024
#else // __S_DST_ADD
#ifdef __NEWWPN1024
#define MAX_DST_RATE		31
#else	// __NEWWPN1024
#define MAX_DST_RATE		30
#endif	// __NEWWPN1024
#endif // __S_DST_ADD

BOOL IsDst_Rate( int nDstParam )
{
	static int nDstRate[MAX_DST_RATE] = {
			DST_ARM_RATE,
			DST_LEG_RATE,
			DST_ADJ_HITRATE,
			DST_ADJ_SPELLRATE,
			DST_ATKPOWER_RATE,
			DST_ADJDEF_RATE,
			DST_DEFHITRATE_DOWN,
			DST_HP_MAX_RATE,
			DST_MP_MAX_RATE,		
			DST_FP_MAX_RATE,		
			DST_HP_RECOVERY_RATE,
			DST_MP_RECOVERY_RATE,
			DST_FP_RECOVERY_RATE,
			DST_CHR_CHANCECRITICAL,
			DST_MASTRY_EARTH,
			DST_MASTRY_FIRE,
			DST_MASTRY_WATER,
			DST_MASTRY_ELECTRICITY,
			DST_MASTRY_WIND,
			DST_ATTACKSPEED,
			DST_MP_DEC_RATE,
			DST_FP_DEC_RATE,
			DST_SPELL_RATE,
			DST_CAST_CRITICAL_RATE,
			DST_CRITICAL_BONUS,
			DST_YOY_DMG,
			DST_BOW_DMG,
			DST_KNUCKLE_DMG,
			DST_SWD_DMG,
			DST_AXE_DMG
#ifdef __NEWWPN1024
			,DST_MASTRY_ALL
#endif	// __NEWWPN1024
#ifdef __S_DST_ADD
			,DST_ALL_RECOVERY_RATE
			,DST_KILL_HP_RATE	
			,DST_KILL_MP_RATE	
			,DST_KILL_FP_RATE
			,DST_KILL_ALL_RATE
			,DST_ALL_DEC_RATE
			,DST_BLOCK_MELEE
			,DST_BLOCK_RANGE			
#endif // __S_DST_ADD
	};

	for( int i=0; i<MAX_DST_RATE; i++ )
	{
		if( nDstRate[i] == nDstParam )
			return TRUE;
	}

	return FALSE;
}
// dst 파라메터이름을 스트링으로 리턴.
// ex) nDstparam = DST_STR -> "힘"
char *FindDstString( int nDstParam )
{
	int		i;
	const DST_STRING *pList = g_DstString;

	int nSize = sizeof( g_DstString ) / sizeof(DST_STRING);
	for( i = 0; i < nSize; i ++ )
	{
		if( pList->nDstParam == nDstParam )	// 같은걸 찾았으면 그 이름을 리턴.
		{
			return (char*)prj.GetText(pList->nNameIndex);
			//return pList->szName;
		}
		pList++;
	}
	return "";
}

// 아이템 툴립 출력할것
// 랜덤 옵션 이름, 아이템 이름( 길드 망토면 길드 이름 ), (세트)
// 예 ) 곰의 코튼 슈트(세트)
DWORD CWndMgr::PutItemName( CItemElem* pItemElem, CEditString* pEdit )
{
	CString str;
	CString strTemp;
	DWORD dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwName0;
	dwItemColor[g_Option.m_nToolTipText].dwName0;
	if( pItemElem->GetProp()->dwItemRare == 200 )
		dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwName1;
	else if( pItemElem->GetProp()->dwItemRare == 300 )
		dwColorbuf = dwItemColor[g_Option.m_nToolTipText].dwName2;
	
	// 랜덤 옵션
#ifdef __RANDOMOPTITEM0628
	PRANDOMOPTITEM pRandomOptItem	= CRandomOptItemGen::GetInstance()->GetRandomOptItem( pItemElem->m_nRandomOptItemId );
	if( pRandomOptItem )
		strTemp.Format( "%s ", pRandomOptItem->pszString );
#endif	// __RANDOMOPTITEM0628

	if( pItemElem->GetProp()->dwItemKind3 == IK3_CLOAK && pItemElem->m_idGuild )	// 길드번호가 박혀있으면.
	{
		CGuild *pGuild = g_GuildMng.GetGuild( pItemElem->m_idGuild );
		if( pGuild )
			str.Format( prj.GetText( TID_GUILD_CLOAK ), pGuild->m_szGuild );
		else
			str.Format( "%s", pItemElem->GetProp()->szName );
	}
	else
#ifdef __YITEMTOOLTIP_COLOR
	{
		str.Format( "%s", pItemElem->GetProp()->szName );
	}
#else //__YITEMTOOLTIP_COLOR
	str.Format( "%s", pItemElem->GetProp()->szName );
#endif //__YITEMTOOLTIP_COLOR
	strTemp += str;
//	str	+= strTemp;
//	pEdit->AddString( strTemp, dwColorbuf, ESSTY_BOLD );
#ifdef __NEWWPN0818
//	pEdit->AddString( strTemp, dwColorbuf, ESSTY_SHADOW );
	pEdit->AddString( strTemp, dwColorbuf, ESSTY_BOLD );
#else	// __NEWWPN0818
//	pEdit->AddString( str, dwColorbuf, ESSTY_BOLD );
	pEdit->AddString( strTemp, dwColorbuf, ESSTY_BOLD );
#endif	// __NEWWPN0818
	
	return dwColorbuf;
}

void CWndMgr::PutItemAbilityPiercing( CItemElem* pItemElem, CEditString* pEdit, DWORD dwColorBuf )
{
	CString strTemp;
	if( pItemElem->GetAbilityOption() )
	{
		strTemp.Format( " %+d", pItemElem->GetAbilityOption() );
		pEdit->AddString( strTemp, dwColorBuf, ESSTY_BOLD );
	}
	
#ifdef __PIERCING_SM_TIME
	PIERCINGINFO piercinginfo = pItemElem->m_piercingInfo;
	int nSize = piercinginfo.nPiercedSize;
	int nCount = 0;
	for( int j = 0; j < nSize; j++ )
	{
		if( piercinginfo.adwItemId[j] != 0 )
			nCount++;
	}
	if( nSize > 0 )
	{
		strTemp.Format( "    (%d/%d)", nCount, nSize );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwPiercing );
	}
#endif // __PIERCING_SM_TIME	
}

void CWndMgr::PutItemResist( CItemElem* pItemElem, CEditString* pEdit )
{
	CString str;
	CString strTemp;
	DWORD dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResist;
	str = "";
	switch( pItemElem->m_bItemResist )
	{
	case SAI79::FIRE:
		{
			str = prj.GetText(TID_UPGRADE_FIRE);
			dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResistFire;
		}					
		break;
	case SAI79::WATER:
		{
			str = prj.GetText(TID_UPGRADE_WATER);
			dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResistWater;
		}					
		break;
	case SAI79::EARTH:
		{
			str = prj.GetText(TID_UPGRADE_EARTH);
			dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResistEarth;
		}
		break;
	case SAI79::ELECTRICITY:
		{
			str = prj.GetText(TID_UPGRADE_ELECTRICITY);
			dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResistElectricity;
		}					
		break;
	case SAI79::WIND:
		{
			str = prj.GetText(TID_UPGRADE_WIND);
			dwResistColor = dwItemColor[g_Option.m_nToolTipText].dwResistWind;
		}					
		break;
	}
	
	if( pItemElem->GetProp()->dwItemKind1 == IK1_WEAPON  || pItemElem->GetProp()->dwItemKind1 == IK1_ARMOR )
	{
		if( !str.IsEmpty() )
		{
			BOOL bBold = FALSE;
			strTemp.Format( "\n%s%+d", str, pItemElem->m_nResistAbilityOption );
			if( pItemElem->m_nResistSMItemId != 0 )
				bBold = TRUE;

			if( bBold )
				pEdit->AddString( strTemp, dwResistColor, ESSTY_BOLD );
			else
				pEdit->AddString( strTemp, dwResistColor );
		}
	}
	else
	{
		strTemp.Format( "\n%s Lv%d", str,  pItemElem->GetProp()->dwItemLV );
		pEdit->AddString( strTemp, dwResistColor );
	}
}

void CWndMgr::PutItemSpeed( CItemElem* pItemElem, CEditString* pEdit )
{
	CString strTemp;
	if( pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT || pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC )
	{	// 무기

		if( pItemElem->GetProp()->fAttackSpeed != 0xffffffff ) // 공격속도
		{
			pEdit->AddString( "\n" );
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ATTACKSPEED), GetATKSpeedString( pItemElem->GetProp()->fAttackSpeed ) );
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral ); 
		}
	}
}

void CWndMgr::PutRandomOpt( CItemElem* pItemElem, CEditString* pEdit )
{
#ifdef __RANDOMOPTITEM0628
	CString strTemp;
	PRANDOMOPTITEM pRandomOptItem	= CRandomOptItemGen::GetInstance()->GetRandomOptItem( pItemElem->m_nRandomOptItemId );
	if( pRandomOptItem ) // 2. 랜덤 옵션의 내용을 출력한다.
	{
		for( int i = 0; i < pRandomOptItem->ia.nSize; i++ )
		{
			int nDst = (int)pRandomOptItem->ia.anDstParam[i];
			int nAdj = (int)pRandomOptItem->ia.anAdjParam[i];
			
			if( IsDst_Rate(nDst) )
			{
				if( nDst == DST_ATTACKSPEED )
					strTemp.Format( "\n%s%+d%%", FindDstString(nDst), nAdj / 10 );
				else
					strTemp.Format( "\n%s%+d%%", FindDstString(nDst), nAdj );					
			}
			else
			{
				strTemp.Format( "\n%s+%d", FindDstString(nDst), nAdj );								
			}
	
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwRandomOption );
		}
	}
#endif	// __RANDOMOPTITEM0628
}
void CWndMgr::PutPiercingOpt( CItemElem* pItemElem, CEditString* pEdit )
{
#ifdef __PIERCING_SM_TIME			
	CString strTemp;
	PIERCINGAVAIL pa;
	memset( &pa, 0, sizeof(PIERCINGAVAIL) );
	pItemElem->GetPiercingAvail( &pa );
	for( int i = 0; i < pa.nSize; i++ )
	{
		int nDst = (int)pa.anDstParam[i];
		int nAdj = (int)pa.anAdjParam[i];
		
		if( IsDst_Rate(nDst) )
		{
			if( nDst == DST_ATTACKSPEED )
				strTemp.Format( "\n%s%+d%%", FindDstString( nDst ), nAdj / 10 );
			else
				strTemp.Format( "\n%s%+d%%", FindDstString( nDst ), nAdj );
		}
		else
		{
			strTemp.Format( "\n%s+%d", FindDstString( nDst ), nAdj );
		}
		
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwPiercing );
	}
#endif //__PIERCING_SM_TIME	
}

void CWndMgr::PutEnchantOpt( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit, int flag )
{
#ifdef __J0608
	CString str;
	CString strTemp;
	BOOL bPSetItem = FALSE;
	int nAbilityOption	= pMover->GetSetItemClient();
	if( pMover->IsActiveMover() )
	{
//		if( flag == 2 && pMover->m_Inventory.IsEquip(pItemElem->m_dwObjId) && pMover->IsSetItemPart( pItemElem->GetProp()->dwParts )
		if( flag == APP_INVENTORY && pMover->m_Inventory.IsEquip(pItemElem->m_dwObjId) && pMover->IsSetItemPart( pItemElem->GetProp()->dwParts )
			&& nAbilityOption > 0 )
			bPSetItem = TRUE;
	}
	else
	{
#ifdef __QUERYEQUIP0630
		if( flag == APP_QUERYEQUIP && pMover->IsSetItemPart( pItemElem->GetProp()->dwParts )
			&& nAbilityOption > 0 )
			bPSetItem = TRUE;
#endif //__QUERYEQUIP0630
	}
	if( bPSetItem ) // 출력
	{	
		PSETITEMAVAIL psa = prj.GetSetItemAvail( nAbilityOption );
		if( psa ) // DST_MAX_HITRATE
		{					
			if( psa->nHitRate > 0 )
			{
				strTemp.Format( "\n%s+%d%%", FindDstString( (int)DST_ADJ_HITRATE ), (int)psa->nHitRate );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwEnchantOption );
			}
			if( psa->nBlock > 0 )
			{
				str = prj.GetText( TID_GAME_TOOLTIPBLOCKRATE );
				strTemp.Format( "\n%s+%d%%", str, (int)psa->nBlock );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwEnchantOption );
			}
			if( psa->nMaxHitPointRate > 0 )
			{
				strTemp.Format( "\n%s+%d%%", FindDstString( (int)DST_HP_MAX_RATE ), (int)psa->nMaxHitPointRate );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwEnchantOption );
			}
			if( psa->nAddMagic > 0 )
			{
				strTemp.Format( "\n%s+%d", FindDstString( (int)DST_ADDMAGIC ), (int)psa->nAddMagic );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwEnchantOption );
			}
			if( psa->nAdded > 0 )
			{
				strTemp.Format( "\n%s+%d", FindDstString( (int)DST_STAT_ALLUP ), (int)psa->nAdded );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwEnchantOption );
			}
		}
	}			
#endif //__J0608
}

void CWndMgr::PutSetItemOpt( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit )
{
#ifdef __SETITEM0624
	CString strTemp;
	// 2. 세트 아이템의 구성 요소 인가?
	CSetItem* pSetItem	= CSetItemFinder::GetInstance()->GetSetItemByItemId( pItemElem->m_dwItemId );
	if( pSetItem )
	{
		// 해당 세트 아이템의 구성 요소를 모두 출력 하되 장착 된 것과 안된 것을 색으로 구별하여 출력한다.
		BOOL pbEquiped[MAX_HUMAN_PARTS];
		memset( pbEquiped, 0, sizeof(pbEquiped) );
		int nEquiped;
		pMover->GetEquipedSetItem( pSetItem->m_nId, pbEquiped, &nEquiped );
		// 3. 세트아이템 타이틀 출력
		strTemp.Format( "\n\n%s (%d/%d)", pSetItem->GetString(), pMover->GetEquipedSetItemNumber( pSetItem ), pSetItem->m_nElemSize );
//		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwSetName, ESSTY_BOLD );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwSetName );
		
		for( int i = 0; i < pSetItem->m_nElemSize; i++ )
		{
			ItemProp* pItemProp	= prj.GetItemProp( pSetItem->m_adwItemId[i] );
			if( pItemProp )
			{
				strTemp.Format( "\n   %s", pItemProp->szName  );
				if( pbEquiped[i] ) // 장착되어있는 세트 아이템
					pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwSetItem1 );
				else
					pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwSetItem0 );
			}
		}
		// 4. 추가 능력치 출력
		ITEMAVAIL itemAvail;
		memset( &itemAvail, 0, sizeof(itemAvail) );
		pSetItem->GetItemAvail( &itemAvail, nEquiped, TRUE );
		for( i = 0; i < itemAvail.nSize; i++ )
		{
			int nDst = (int)itemAvail.anDstParam[i];
			int nAdj = (int)itemAvail.anAdjParam[i];
			
			if( IsDst_Rate(nDst) )
			{
				if( nDst == DST_ATTACKSPEED )
					strTemp.Format( "\n%s: %s% +d%%", prj.GetText(TID_TOOLTIP_SET), FindDstString( nDst ), nAdj / 10 );	
				else
					strTemp.Format( "\n%s: %s% +d%%", prj.GetText(TID_TOOLTIP_SET), FindDstString( nDst ), nAdj );	
			}
			else
			{
				strTemp.Format( "\n%s: %s +%d", prj.GetText(TID_TOOLTIP_SET), FindDstString( nDst ), nAdj );
			}
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwSetEffect );
		}
	}
#endif	// __SETITEM0624	
}

void CWndMgr::PutItemCard( CItemElem* pItemElem, CEditString* pEdit )
{
#ifdef __S_ADD_PIERCING_ADD_CARD
	if( pItemElem->GetProp()->dwItemKind3 == IK3_SOCKETCARD )
	{ 
		CString strTemp;
		if( pItemElem->GetProp()->dwReferStat1 == PIERCING_1 )
			strTemp.Format( prj.GetText(TID_PIERCING_SOCKET_CARD0) );
		else if( pItemElem->GetProp()->dwReferStat1 == PIERCING_2 )
			strTemp.Format( prj.GetText(TID_PIERCING_SOCKET_CARD1) );

		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral ); 
	}
#endif // __S_ADD_PIERCING_ADD_CARD	
}


void CWndMgr::PutItemMinMax( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit )
{
	pEdit->AddString( "\n" );
	CString strTemp;
	if( pItemElem->GetProp()->dwAbilityMin != 0xffffffff && pItemElem->GetProp()->dwAbilityMax != 0xffffffff 
		&& pItemElem->GetProp()->dwEndurance != 0xffffffff )
	{	
		DWORD dwColorMinMax = dwItemColor[g_Option.m_nToolTipText].dwEffective3;	// 회색

		float f = pMover->GetItemMultiplier( pItemElem );
		
		int nMin	= (int)( pMover->GetItemAbilityMin( pItemElem->GetProp()->dwID ) * f );
		int nMax	= (int)( pMover->GetItemAbilityMax( pItemElem->GetProp()->dwID ) * f );
		if( 1.0f <= f )
			dwColorMinMax = dwItemColor[g_Option.m_nToolTipText].dwEffective0;		// 검정
		else if( 0.8f <= f)
			dwColorMinMax = dwItemColor[g_Option.m_nToolTipText].dwEffective1;		// 노랑

		else if( 0.6f <= f )
			dwColorMinMax = dwItemColor[g_Option.m_nToolTipText].dwEffective2;		// 적색

		
		if( pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT || pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC )
		{	// 공격력
			int nOpt = 0;
#ifdef __BALANCE_050614 
			if( pItemElem->GetAbilityOption() > 0 )
			{
				int nAdd = (int)pow( pItemElem->GetAbilityOption(), 1.5 );
				nMin += nAdd;
				nMax += nAdd;
			}
#else
			nOpt = ::GetWeaponPlusDamage( 1, FALSE, pItemElem->GetProp(), nOption );
#endif //!__BALANCE_050614 
			if( nOpt )
			{
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ATTACKRANGE1) );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
				strTemp.Format( " (%d ~ %d)+%d", nMin, nMax, nOpt );
			}
			else
			{
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ATTACKRANGE2) );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
				strTemp.Format( " %d ~ %d", nMin, nMax );
			}
		}
		else
		{	// 방어력
#ifdef __BALANCE_050614 
			int nOpt = 0;			
			if( pItemElem->GetAbilityOption() > 0 )
			{
				int nAdd = (int)pow( pItemElem->GetAbilityOption(), 1.5 );
				nMin += nAdd;
				nMax += nAdd;
			}
#else
			int nOpt = prj.GetExpUpItem( pItemElem->GetProp()->dwItemKind3, pItemElem->GetAbilityOption() );			
#endif
			if( nOpt )
			{	
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_DEFENSE_A) );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
				strTemp.Format( " (%d ~ %d)+%d", nMin, nMax, nOpt );
			}
			else
			{
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_DEFENSE_B) );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
				strTemp.Format( " %d ~ %d", nMin, nMax );
			}
		}
		pEdit->AddString( strTemp, dwColorMinMax );
	}
}

void CWndMgr::PutMedicine( CItemElem* pItemElem, DWORD dwParam, LONG nParamVal, CEditString* pEdit )
{
	CString strTemp;
	if( dwParam != 0xffffffff )
	{	//  치료량
		if( nParamVal != 0xffffffff )
		{
			if( DST_MP == dwParam )
			{ // MP 치료량
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RECOVMP), nParamVal );
				pEdit->AddString( "\n" );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
			}
			else if( DST_HP == dwParam )
			{ // HP 치료량
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RECOVHP), nParamVal );
				pEdit->AddString( "\n" );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
			}
			else if( DST_FP == dwParam )
			{ // FP 치료량
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RECOVFP), nParamVal );
				pEdit->AddString( "\n" );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
			}
		}

		if( pItemElem->GetProp()->dwAbilityMin != 0xffffffff )
		{	// 최대회복량
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_MAXRECOVER), pItemElem->GetProp()->dwAbilityMin );
			pEdit->AddString( "\n" );
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
		}
	}
}

void CWndMgr::PutBaseItemOpt( CItemElem* pItemElem, CEditString* pEdit )
{
	CString str;
	CString strTemp;
	for( int i = 0; i < 2; i ++ )
	{
		if( pItemElem->GetProp()->dwDestParam[i] != 0xffffffff )		
		{
#ifdef __X1012
			if( pItemElem->GetProp()->dwDestParam[i] == DST_STAT_ALLUP )
			{
				str.Format( "\n%s%+d", FindDstString( DST_STR ), (int)pItemElem->GetProp()->nAdjParamVal[i] );
				strTemp = str;
				str.Format( "\n%s%+d", FindDstString( DST_DEX ), (int)pItemElem->GetProp()->nAdjParamVal[i] );
				strTemp += str;
				str.Format( "\n%s%+d", FindDstString( DST_INT ), (int)pItemElem->GetProp()->nAdjParamVal[i] );
				strTemp += str;
				str.Format( "\n%s%+d", FindDstString( DST_STA ), (int)pItemElem->GetProp()->nAdjParamVal[i] );
				strTemp += str;
			}
			else
#endif // x1012
			{
				int nDst	= (int)pItemElem->GetProp()->dwDestParam[i];
				if( IsDst_Rate( nDst ) )
				{
					if( nDst == DST_ATTACKSPEED )
						strTemp.Format( "\n%s%+d%%", FindDstString( nDst ), (int)pItemElem->GetProp()->nAdjParamVal[i] / 10 );
					else
						strTemp.Format( "\n%s%+d%%", FindDstString( nDst ), (int)pItemElem->GetProp()->nAdjParamVal[i] );
				}
				else
				{
					strTemp.Format( "\n%s%+d", FindDstString( (int)pItemElem->GetProp()->dwDestParam[i] ), (int)pItemElem->GetProp()->nAdjParamVal[i] );
				}
			}
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
		}
	}
}

void CWndMgr::PutBaseResist( CItemElem* pItemElem, CEditString* pEdit )
{
	CString strTemp;
	if( pItemElem->GetProp()->nItemResistElecricity )
	{
		strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ELEC_RES), pItemElem->GetProp()->nItemResistElecricity );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistElectricity );
	}
	if( pItemElem->GetProp()->nItemResistFire )
	{
		strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_FIRE_RES), pItemElem->GetProp()->nItemResistFire );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistFire );
	}
	if( pItemElem->GetProp()->nItemResistWater )
	{
		strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_WATER_RES), pItemElem->GetProp()->nItemResistWater );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistWater );
	}
	if( pItemElem->GetProp()->nItemResistWind )
	{
		strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_WIND_RES), pItemElem->GetProp()->nItemResistWind );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistWind );
	}
	if( pItemElem->GetProp()->nItemResistEarth )
	{
		strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_EARTH_RES), pItemElem->GetProp()->nItemResistEarth );
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwResistEarth );
	}
}

void CWndMgr::PutItemGold( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit, int flag )
{
	CString str;
	CString strTemp;
	if( flag == APP_SHOP_ ) // 가격은 나는 안나오게 하고 상점 인터페이스에서는 나오게함 42은 상점의 Invantory 개수

	{	// 상점
		if( pItemElem->GetCost() != 0xffffffff ) 
		{	// 가격
			int nCost = pItemElem->GetCost();
			nCost = (int)(nCost * prj.m_fShopCost );
#ifdef __NPP_050308
			KarmaProp* pProp = prj.GetKarmaProp( pMover->m_nSlaughter );
			if( pProp )
			{
				if( pProp->fDiscountRate != 0 )
					nCost = nCost  * pProp->fDiscountRate;
			}
#else // __NPP_050308
			if( g_eLocal.GetState( EVE_18 ) == 1 && pMover->m_nSlaughter > 0 )	// 빨갱이는 물건살때 더 비싸다.
				nCost = (int)(nCost * 1.8f);
#endif // __NPP_050308
			
			if( nCost <= 0 )
				nCost = 1;
			
#ifdef __YDECIMAL1223			
			char szbuffer[1024];
			_itoa( nCost, szbuffer, 10 );
			str = GetNumberFormatEx(szbuffer);
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_COST2), str );
#else //__YDECIMAL1223
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_COST), nCost );			
#endif //__YDECIMAL1223
			pEdit->AddString( "\n" );
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGold );
		}
	}
	else if( flag == APP_INVENTORY )	// 인벤토리 / 뱅크 툴팁?
	{
		CWndShop* pWndShop	= (CWndShop*)GetWndBase( APP_SHOP_ );
		if( pWndShop )	// 상점을 열엇을때 파는가격도 나와야함
		{
			if( pItemElem->GetCost() != 0xffffffff )
			{	//	파는가격
#ifdef __YDECIMAL1223			
				char szbuffer[1024];
				DWORD dwCostTem = 0;
				dwCostTem = pItemElem->GetCost() / 4;
#ifdef __NPP_050308
				KarmaProp* pProp = prj.GetKarmaProp( pMover->m_nSlaughter );
				if( pProp && pProp->fSellPenaltyRate != 0 )
					dwCostTem *= pProp->fSellPenaltyRate;
				
				if( dwCostTem < 1 )
					dwCostTem = 1;
#endif // __NPP_050308
				_itoa( dwCostTem, szbuffer, 10 );
				str = GetNumberFormatEx(szbuffer);
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_SHELLCOST2), str );
#else //__YDECIMAL1223
				strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_SHELLCOST), pItemElem->GetCost() / 4 );				
#endif //__YDECIMAL1223
				pEdit->AddString( "\n" );
				pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGold );
			}
		}
	}
	else if( flag == APP_VENDOR )
	{
		if( pItemElem->m_nCost != 0xffffffff )
		{	// 파는가격
#ifdef __YDECIMAL1223			
			char szbuffer[1024];
			DWORD dwCostTem = 0;
			dwCostTem = pItemElem->m_nCost;
			_itoa( dwCostTem, szbuffer, 10 );
			str = GetNumberFormatEx(szbuffer);
			
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_SHELLCOST2), str );
#else //__YDECIMAL1223
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_SHELLCOST), pItemElem->m_nCost );
#endif //__YDECIMAL1223
			pEdit->AddString( "\n" );
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGold );
		}
	}
}

void CWndMgr::PutCoolTime( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit )
{
#ifdef __ITEMCOOLTIME_VER2_050308	
	CString strTemp;
	DWORD dwGroup = pMover->m_cooltimeMgr.GetGroup( pItemElem->GetProp() );
	if( dwGroup )
	{
		DWORD dwCur = timeGetTime();
		DWORD dwEnd = pMover->m_cooltimeMgr.GetTime( dwGroup );		// 이벤트 종료 시각 
		if( dwEnd != 0 && dwEnd > dwCur )
		{
			DWORD dwBase = pMover->m_cooltimeMgr.GetBase( dwGroup );	// 이벤트 시작 시각 
			
			CTimeSpan ct( (dwEnd - dwCur + 500) / 1000 );		// 남은시간을 초단위로 변환해서 넘겨줌, +500 반올림 
			strTemp.Format( prj.GetText(TID_TOOLTIP_COOLTIME), ct.GetMinutes(), ct.GetSeconds() );		// 남은시간을 분/초 형태로 출력.
			pEdit->AddString( "\n" );
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );	
		}
	}
#endif //__YCOOLTIME_0214
}

void CWndMgr::PutKeepTime( CItemElem* pItemElem, CEditString* pEdit )
{
	CString str, strTemp;
	if( pItemElem->GetProp()->dwCircleTime != 0xffffffff )
	{
		pEdit->AddString( "\n" );
		if( pItemElem->GetProp()->dwCircleTime == 1 )
		{
#ifdef __S050524_SM_ITEM
			pEdit->AddString( prj.GetText( TID_GAME_COND_USE ), dwItemColor[g_Option.m_nToolTipText].dwTime ); // 사망시 효과 적용

#endif // __S050524_SM_ITEM
		}
		else
		{
			CTimeSpan ct( pItemElem->GetProp()->dwCircleTime );
			strTemp.Format( "%s : ", prj.GetText( TID_TOOLTIP_ITEMTIME ) );	// 지속시간 : 
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime ); 
			strTemp.Format( prj.GetText( TID_TOOLTIP_DATE ), ct.GetDays(), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	// 지속시간 : 
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime ); 
		}
	}
#ifdef __FINITEITEM0705
	time_t t	= pItemElem->m_dwKeepTime - time( NULL );
	if( pItemElem->m_dwKeepTime && !pItemElem->IsFlag( CItemElem::expired ) )
	{
		if( t > 0 )
		{
			CTimeSpan time( t );
			if( time.GetDays() )
				str.Format( prj.GetText(TID_PK_LIMIT_DAY), time.GetDays()+1 );
			else if( time.GetHours() )
				str.Format( prj.GetText(TID_PK_LIMIT_HOUR), time.GetHours() );
			else if( time.GetMinutes() > 10 )
				str.Format( prj.GetText(TID_PK_LIMIT_MINUTE), time.GetMinutes() );
			else
				str.Format( prj.GetText(TID_TOOLTIO_PERIODOFMIN), 10 );
		}
		else
		{
			str.Format( prj.GetText(TID_TOOLTIO_PERIODOFMIN), 10 );
		}
		strTemp = str + prj.GetText(TID_TOOLTIP_PERIOD);	
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwTime );
	}
#endif //__FINITEITEM0705
}

void CWndMgr::PutEndurance( CItemElem* pItemElem, CEditString* pEdit, int flag )
{
	// 내구력 나는 나오게 하고 상점 인터페이스에서는 안나오게함 56은 나의 Invantory 개수

	if(flag != APP_SHOP_ )
	{	// 내구력
#ifdef __QUERYEQUIP0630
		if( flag != APP_QUERYEQUIP )
		{
#endif //__QUERYEQUIP0630
			if( pItemElem->GetProp()->dwEndurance != 0xffffffff )
			{
				CString strTemp;
				int nEndurance = int((float)pItemElem->m_nHitPoint * 100 / (float)pItemElem->GetProp()->dwEndurance );
				strTemp.Format( "%s %d%%", prj.GetText(TID_GAME_TOOLTIP_DURABILITY), nEndurance );
				pEdit->AddString( "\n" );
				if( nEndurance != 0 )
					pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
				else
					pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwNotUse );
			}
#ifdef __QUERYEQUIP0630
		}
#endif //__QUERYEQUIP0630
	}	
}

void CWndMgr::PutSex( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit )
{
	if( pItemElem->GetProp()->dwItemSex != 0xffffffff )
	{	// 성별
		CString strTemp;
		if( pItemElem->GetProp()->dwItemSex == 0 )	// 남
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_SEXMALE) );
		else	// 여
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_SEXFEMALE) );
		pEdit->AddString( "\n" );
		if( pItemElem->GetProp()->dwItemSex == pMover->GetSex() )
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral ); 
		else
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwNotUse ); 
	}
}

void CWndMgr::PutJob( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit )
{
	if( pItemElem->GetProp()->dwItemJob != 0xffffffff )
	{	// 직업

		CString strTemp;
		switch( pItemElem->GetProp()->dwItemJob )
		{
		case JOB_VAGRANT:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_REGVANG) );
			break;
		case JOB_MERCENARY:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_REGMERSER) );
			break;
		case JOB_ACROBAT:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ACRO) );
			break;
		case JOB_ASSIST:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ASSIST) );
			break;
		case JOB_MAGICIAN:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_MAG) );
			break;
		case JOB_PUPPETEER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_PUPPET) );
			break;

		case JOB_KNIGHT:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_KNIGHT) );
			break;
		case JOB_BLADE: 
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_BLADE) );
			break;
		case JOB_JESTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_JASTER) );
			break;
		case JOB_RANGER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RANGER) );
			break;
		case JOB_RINGMASTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_RINGMAS) );
			break;
		case JOB_BILLPOSTER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_BILLPOS) );
			break;
		case JOB_PSYCHIKEEPER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_PSYCHIKEEPER) );
			break;
		case JOB_ELEMENTOR:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ELEMENTOR) );
			break;
		case JOB_GATEKEEPER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_GATE) );
			break;
		case JOB_DOPPLER:
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_DOPPLER) );
			break;

		} // switch
		pEdit->AddString( "\n" );
		if( pMover->IsInteriorityJob( pItemElem->GetProp()->dwItemJob ) )
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
		else 
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwNotUse );
	}
}

void CWndMgr::PutLevel( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit )
{
	if( pItemElem->GetProp()->dwLimitLevel1 != 0xffffffff ) 
	{
		CString strTemp;
		strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_REQLEVEL), pItemElem->GetProp()->dwLimitLevel1 );
		pEdit->AddString( "\n" );
		if( pMover->GetLevel() < pItemElem->GetProp()->dwLimitLevel1 )
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwNotUse );
		else
			pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral );
	}
}

void CWndMgr::PutWeapon( CItemElem* pItemElem, CEditString* pEdit )
{
	if( pItemElem->GetProp()->dwHanded != 0xffffffff )
	{ // 한손무기용 / 양손무기용

		CString strTemp;
		if( HD_ONE == pItemElem->GetProp()->dwHanded )
		{
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_ONEHANDWEAPON) );
		}
		else if( HD_TWO == pItemElem->GetProp()->dwHanded )
		{
			strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_TWOHANDWEAPON) );
		}
		pEdit->AddString( "\n" );
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwGeneral ); 
	}
}

void CWndMgr::PutCommand( CItemElem* pItemElem, CEditString* pEdit )
{
	CString strTemp;
	if( strlen( pItemElem->GetProp()->szCommand ) )
	{
		pEdit->AddString( "\n" );
		strTemp.Format( prj.GetText( TID_ITEM_INFO ), pItemElem->GetProp()->szCommand );	// 설명 :
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwCommand );
	}

#ifdef __OVERLAPPING_EXPITEM_S0601
	if( pItemElem->GetProp()->dwID == II_SYS_SYS_SCR_AMPESA || pItemElem->GetProp()->dwID == II_SYS_SYS_SCR_AMPESB
		|| pItemElem->GetProp()->dwID == II_SYS_SYS_SCR_AMPESC || pItemElem->GetProp()->dwID == II_SYS_SYS_SCR_AMPESD )
	{
		pEdit->AddString( "\n" );
		strTemp.Format( prj.GetText( TID_GAME_EXP_LEVELUSE ) );	// 레벨에 따라 중복 가능
		pEdit->AddString( strTemp, dwItemColor[g_Option.m_nToolTipText].dwCommand );
	}
#endif // __OVERLAPPING_EXPITEM_S0601	
}

void CWndMgr::PutToolTip_Item( CItemBase* pItemBase, CPoint point, CRect* pRect, int flag )
{
	if( pItemBase == NULL )
		return;
	ItemProp *pItemProp = pItemBase->GetProp();
	if( pItemProp == NULL )
	{
		LPCTSTR szErr = Error( "PutToolTip_Item : 프로퍼티가 널 %d", pItemBase->m_dwItemId );
		ADDERRORMSG( szErr );
		return;
	}
	
	CMover* pMover = g_pPlayer;
#ifdef __QUERYEQUIP0630
	if( flag == APP_QUERYEQUIP )
	{
		CWndQueryEquip * pWndQueryEquip = (CWndQueryEquip *)g_WndMng.GetWndBase( APP_QUERYEQUIP );
		if( pWndQueryEquip )
			pMover = pWndQueryEquip->GetMover();
	}
#endif //__QUERYEQUIP0630	
	if( pMover == NULL )
		return;

//#ifdef __SETITEM0624
//	CSetItem* pSetItem	= CSetItemFinder::GetInstance()->GetSetItemByItemId( pItemProp->dwID );
//#endif //__SETITEM0624

	CItemElem* pItemElem	= (CItemElem*)pItemBase;

	CString str;
	CString strTemp;
	CString strEnter = '\n';
	CEditString strEdit;	// 전체를 저장하는 String
#ifdef __SDEBUG
	if( ((CItemElem*)pItemBase)->m_bCharged )
	{
		strEdit.AddString( "Debug만 나옴 상용화", 0xffff0000 );
		strEdit += strEnter;
	}
#endif // __SDEBUG

	DWORD dwColorBuf;
	dwColorBuf = PutItemName( pItemElem, &strEdit );
	PutItemAbilityPiercing( pItemElem, &strEdit, dwColorBuf );
	PutWeapon( pItemElem, &strEdit );
	PutSex( pMover, pItemElem, &strEdit );
	
	switch( pItemProp->dwItemKind2 )
	{
		case IK2_WEAPON_DIRECT:
		case IK2_WEAPON_MAGIC:
		case IK2_ARMORETC:
		case IK2_CLOTHETC:
		case IK2_ARMOR:
		case IK2_CLOTH:
		case IK2_BLINKWING:
		{
			PutItemMinMax( pMover, pItemElem, &strEdit );
			PutItemSpeed( pItemElem, &strEdit );
			if( pItemProp->dwItemKind3 == IK3_ELECARD )
				PutItemResist( pItemElem, &strEdit );
			else if( pItemElem->m_nResistAbilityOption && ( pItemProp->dwItemKind1 == IK1_WEAPON  || pItemProp->dwItemKind1 == IK1_ARMOR ) )
				PutItemResist( pItemElem, &strEdit );

			PutBaseResist( pItemElem, &strEdit );	// 속성 저항력
			
			PutBaseItemOpt( pItemElem, &strEdit );
			PutPiercingOpt( pItemElem, &strEdit );
			PutRandomOpt( pItemElem, &strEdit );			
			PutEnchantOpt( pMover, pItemElem, &strEdit, flag );
			break;
		}
	case IK2_REFRESHER:
	case IK2_FOOD:
	case IK2_POTION:
		{
			PutMedicine( pItemElem, pItemElem->GetProp()->dwDestParam1,  pItemProp->nAdjParamVal1, &strEdit );
			PutMedicine( pItemElem, pItemElem->GetProp()->dwDestParam2,  pItemProp->nAdjParamVal2, &strEdit );
			break;
		}
	case IK2_JEWELRY:
		{
			PutBaseItemOpt( pItemElem, &strEdit );
			break;
		}
	default:
		{
			break;
		}
	}	// switch( pItemProp->dwItemKind2 )

	PutCoolTime( pMover, pItemElem, &strEdit );			// 쿨타임
	PutEndurance( pItemElem, &strEdit, flag );			// 내구력
	PutKeepTime( pItemElem, &strEdit );					// 사용할수 있는 시간
	PutJob( pMover, pItemElem, &strEdit );
	PutLevel( pMover, pItemElem, &strEdit );	
	PutCommand( pItemElem, &strEdit );					// 용도 
	PutItemGold( pMover, pItemElem, &strEdit, flag ); //가격
	PutSetItemOpt( pMover, pItemElem, &strEdit );
	PutItemCard( pItemElem, &strEdit );					// 카드


	g_toolTip.PutToolTip( pItemBase->m_dwItemId, strEdit, *pRect, point, 0 );
}

void CWndMgr::PutToolTip_Character( int SelectCharacter, CPoint point, CRect* pRect )
{
	CString string;
	char statebuf[30] = {0,};
	// 블록 상태( 블록상태면 빨간색 )
	if( g_Neuz.m_nCharacterBlock[SelectCharacter] )
//		string.Format( "#b#cff2e9200%s#nc#nb", g_Neuz.m_apPlayer[SelectCharacter]->GetName() );
		string.Format( "#b#cff0000ff%s#nc#nb", g_Neuz.m_apPlayer[SelectCharacter]->GetName() );
	else
		string.Format( "#b#cffff0000%s\nConnet Cut#nc#nb", g_Neuz.m_apPlayer[SelectCharacter]->GetName() );
	// 성별
	if(	g_Neuz.m_apPlayer[SelectCharacter]->GetSex() )
		sprintf( statebuf, prj.GetText(TID_GAME_TOOLTIP_SEXFEMALE) );
	else
		sprintf( statebuf, prj.GetText(TID_GAME_TOOLTIP_SEXMALE) );
	string += '\n'; string += statebuf;

	// 레벨
	sprintf( statebuf, prj.GetText(TID_GAME_TOOL_EX_LEVEL), g_Neuz.m_apPlayer[SelectCharacter]->m_nLevel);
	string += '\n'; string += statebuf;

	// Str
	sprintf( statebuf, prj.GetText(TID_GAME_TOOL_EX_STR), g_Neuz.m_apPlayer[SelectCharacter]->m_nStr);
	string += '\n'; string += statebuf;
	
	// Sta 
	sprintf( statebuf, prj.GetText(TID_GAME_TOOL_EX_STA), g_Neuz.m_apPlayer[SelectCharacter]->m_nSta);
	string += '\n'; string += statebuf;
	
	// Dex
	sprintf( statebuf, prj.GetText(TID_GAME_TOOL_EX_DEX), g_Neuz.m_apPlayer[SelectCharacter]->m_nDex);
	string += '\n'; string += statebuf;
	
	// Int
	sprintf( statebuf, prj.GetText(TID_GAME_TOOL_EX_INT), g_Neuz.m_apPlayer[SelectCharacter]->m_nInt);
	string += '\n'; string += statebuf;
				
	// 직업

	sprintf( statebuf, "%s : %s", prj.GetText(TID_APP_CHARACTER_JOB), prj.m_aJob[ g_Neuz.m_apPlayer[SelectCharacter]->m_nJob ].szName );
	string += '\n'; string += statebuf;
/*	
	// 레벨
	sprintf( statebuf, prj.GetText(TID_GAME_TOOLTIP_JOBLEVEL), g_Neuz.m_apPlayer[SelectCharacter]->m_aJobLv[g_Neuz.m_apPlayer[SelectCharacter]->m_nJob]);
	string += '\n'; string += statebuf;
*/	
	CEditString strEdit;
	strEdit.SetParsingString( string );
	g_toolTip.PutToolTip( SelectCharacter, strEdit, *pRect, point, 3 );
}

// 숏컷으로 스킬 실행하기.
void CWndMgr::UseSkillShortCut( DWORD dwType, DWORD dwSkillIdx )
{
	LPSKILL lpSkill = g_pPlayer->GetSkill( dwType, dwSkillIdx );
	if( lpSkill )
	{
		CCtrl* pCtrl = (CCtrl*)g_WorldMng.Get()->GetObjFocus();
		DWORD dwObjId = NULL_ID;
		if( pCtrl && pCtrl->GetType() != OT_OBJ )
			dwObjId = pCtrl->GetId();

		{
			// 스킬바 사용중일때는 핫키로 스킬사용 안됨. / 타겟이 무버여야만함.
			if( ((CWndTaskBar *)g_WndMng.m_pWndTaskBar)->m_nExecute == 0 && (pCtrl == NULL || (pCtrl && pCtrl->GetType() == OT_MOVER)) )	
			{
//				if( ((CWndTaskBar *)g_WndMng.m_pWndTaskBar)->m_nExecute )	// 단축키로 스킬을 실행했을때 액션슬롯으로 실행상태였다면 그것을 취소시킴.
//					((CWndTaskBar *)g_WndMng.m_pWndTaskBar)->OnCancelSkill();
				
#ifdef __ACROBAT_0504
				if( g_pPlayer->IsBullet( lpSkill->GetProp() ) == FALSE )
				{
					return;
				}
#endif // __ACROBAT_0504
				if( g_pPlayer->CMD_SetUseSkill( dwObjId, dwSkillIdx ) )		// 실행할 명령을 셋팅. 이동 + 스킬사용이 합쳐진 명령.
				{
					g_WndMng.m_pWndWorld->SetNextSkill( NEXTSKILL_NONE );
				} else
				// 공격중이라 스킬 사용이 씹혔으면

				if( g_pPlayer->m_pActMover->IsActAttack() )
				{
					// 다음 턴때 이 스킬을 예약시켜둠.
					g_WndMng.m_pWndWorld->SetNextSkill( dwSkillIdx );
				}
			}
				
		}
	}
}

BOOL CWndMgr::ScreenCapture()
{
	static long	bmpcnt = 0;
	char	filename[ _MAX_PATH ] = "";
	FILE	*fp;

	//if(bmpcnt == -1)
	{
		CreateDirectory( "Capture", NULL );
		while(1)
		{
			sprintf( filename, "Capture\\flyff%05d.bmp", bmpcnt );
			if( (fp = fopen(filename,"r")) == NULL ) break; else fclose(fp);
			bmpcnt ++;
		}
	}
	sprintf( filename, "Capture\\flyff%05d.bmp", bmpcnt );
	SaveBitmap( filename );
	CString string;
	//string.Format( "flyff%05d.bmp를 Capture폴더에 저장했습니다.", bmpcnt++ );
	//PutString( string );
	
	string.Format( prj.GetText(TID_ADMIN_SCREENSHOTSAVE), bmpcnt++ );
	PutString( string, NULL, prj.GetTextColor(TID_ADMIN_SCREENSHOTSAVE) );
				
	return TRUE;
}
BOOL CWndMgr::SaveBitmap( LPCTSTR lpszName )
{
	LPBYTE  lpData = new BYTE[ g_Option.m_nResWidth * g_Option.m_nResHeight * 4 ];
	LPDWORD lpdwData = (LPDWORD) lpData;
	memset( lpData, 255, g_Option.m_nResWidth * g_Option.m_nResHeight * 4 );
	
	IDirect3DSurface9* pDestSurface;
	m_pApp->m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pDestSurface );

	D3DLOCKED_RECT lockedRect;
	CRect rectIn ( 0, 0, m_pApp->m_d3dsdBackBuffer.Width, m_pApp->m_d3dsdBackBuffer.Height );//cb.bih.biWidth, abs(cb.bih.biHeight) );
	HRESULT hr = pDestSurface->LockRect( &lockedRect, NULL, 0 ) ;
	if( hr == D3D_OK )// Lock
	{
		LPBYTE pSrcByte = (LPBYTE)lockedRect.pBits; 
		LPWORD pSrcWord = (LPWORD)lockedRect.pBits; // 16bit
		//LPDWORD pSrcDWord = (LPDWORD)lockedRect.pBits; // 32bit

		DWORD dwOffset = 0;
		DWORD dwBit = 0;
		switch( g_Neuz.m_d3dpp.BackBufferFormat )
		{
			case D3DFMT_R8G8B8: dwBit = 24; break; // 20 24-bit RGB pixel format with 8 bits per channel. 
			case D3DFMT_A8R8G8B8: dwBit = 32; break; //  21 32-bit ARGB pixel format with alpha, using 8 bits per channel. 
			case D3DFMT_X8R8G8B8: dwBit = 32; break; //  22 32-bit RGB pixel format, where 8 bits are reserved for each color. 
			case D3DFMT_R5G6B5: dwBit = 16; break; //  23 16-bit RGB pixel format with 5 bits for red, 6 bits for green, and 5 bits for blue. 
			case D3DFMT_X1R5G5B5: dwBit = 16; break; //  24 16-bit pixel format where 5 bits are reserved for each color. 
			case D3DFMT_A1R5G5B5: dwBit = 16; break; //  25 16-bit pixel format where 5 bits are reserved for each color and 1 bit is reserved for alpha. 
			case D3DFMT_A4R4G4B4: dwBit = 16; break; //  26 16-bit ARGB pixel format with 4 bits for each channel. 
			case D3DFMT_R3G3B2: dwBit = 8; break; //  27 8-bit RGB texture format using 3 bits for red, 3 bits for green, and 2 bits for blue. 
			case D3DFMT_A8: dwBit = 8; break; //  28 8-bit alpha only. 
			case D3DFMT_A8R3G3B2: dwBit = 16; break; //  29 16-bit ARGB texture format using 8 bits for alpha, 3 bits each for red and green, and 2 bits for blue. 
			case D3DFMT_X4R4G4B4: dwBit = 16; break; //  30 16-bit RGB pixel format using 4 bits for each color. 
			case D3DFMT_A2B10G10R10: dwBit = 32; break; //  31 32-bit pixel format using 10 bits for each color and 2 bits for alpha. 
			case D3DFMT_A8B8G8R8: dwBit = 32; break; //  32 32-bit ARGB pixel format with alpha, using 8 bits per channel. 
			case D3DFMT_X8B8G8R8: dwBit = 32; break; //  33 32-bit RGB pixel format, where 8 bits are reserved for each color. 
			case D3DFMT_G16R16: dwBit = 32; break; //  34 32-bit pixel format using 16 bits each for green and red. 
			case D3DFMT_A2R10G10B10: dwBit = 32; break; //  35 32-bit pixel format using 10 bits each for red, green, and blue, and 2 bits for alpha. 
			case D3DFMT_A16B16G16R16: dwBit = 64; break; //  36 64-bit pixel format using 16 bits for each component. 
			case D3DFMT_A8P8: dwBit = 8; break; //  40 8-bit color indexed with 8 bits of alpha. 
			case D3DFMT_P8: dwBit = 8; break; //  41 8-bit color indexed. 
			case D3DFMT_L8: dwBit = 8; break; //  50 8-bit luminance only. 
			case D3DFMT_L16: dwBit = 16; break; //  81 16-bit luminance only. 
			case D3DFMT_A8L8: dwBit = 16; break; //  51 16-bit using 8 bits each for alpha and luminance. 
			case D3DFMT_A4L4: dwBit = 8; break; //  8-bit using 4 bits each for alpha and luminance.
		}	
		if( dwBit == 16 || dwBit == 32 )
		{
			int nPitch = lockedRect.Pitch;
			if( dwBit == 32 )
			{
				for(int y = 0; y < g_Option.m_nResHeight; y++ )
				{
					for(int x = 0; x < g_Option.m_nResWidth; x++, dwOffset += 3 )
					{
						// 32 bit color code
						BYTE byData3 = pSrcByte[ y * nPitch + ( x * 4 ) + 0 ];
						BYTE byData2 = pSrcByte[ y * nPitch + ( x * 4 ) + 1 ];
						BYTE byData1 = pSrcByte[ y * nPitch + ( x * 4 ) + 2 ];
						lpData[ dwOffset + 0 ] = byData3;
						lpData[ dwOffset + 1 ] = byData2;
						lpData[ dwOffset + 2 ] = byData1;
					}
				}
			}
			else
			{
				nPitch /= 2;
				for(int y = 0; y < g_Option.m_nResHeight; y++ )
				{
					for(int x = 0; x < g_Option.m_nResWidth; x++, dwOffset += 3 )
					{
						WORD wData = pSrcWord[ y * nPitch + x ];
							
						BYTE byData1 = ( wData & 0xf800 ) >> 8;
						BYTE byData2 = ( wData & 0x07e0 ) >> 3;
						BYTE byData3 = ( wData & 0x001f ) << 3 ;
						
						lpData[ dwOffset + 0 ] = byData3;
						lpData[ dwOffset + 1 ] = byData2;
						lpData[ dwOffset + 2 ] = byData1;
					}
				}
			}
		}
		pDestSurface->UnlockRect();
	}
	pDestSurface->Release();
	BOOL bResult = SaveBMP( lpszName, lpData,CSize( g_Option.m_nResWidth, g_Option.m_nResHeight ) );
	SAFE_DELETE( lpData );
	return bResult;
}
CWndMap* CWndMgr::OpenMap( LPCTSTR lpszMapFileName )
{
	CWndMap* pWndMap = NULL;
	if( m_mapMap.Lookup( lpszMapFileName, (void*&)pWndMap ) == FALSE )
	{
		pWndMap = new CWndMap;
		strcpy( pWndMap->m_szMapFile, lpszMapFileName );
		pWndMap->Initialize();
		m_mapMap.SetAt( lpszMapFileName, pWndMap );
	}
	return pWndMap;
}
CWndMessage* CWndMgr::GetMessage( LPCTSTR lpszFrom )
{
	CString string;
	CWndMessage* pWndMessage = NULL;
	m_mapMessage.Lookup( lpszFrom, (void*&)pWndMessage );
	return pWndMessage;
}
CWndMessage* CWndMgr::OpenMessage( LPCTSTR lpszFrom )
{
	CString string;
	CWndMessage* pWndMessage = NULL;
	if( m_mapMessage.Lookup( lpszFrom, (void*&)pWndMessage ) == FALSE )
	{
		pWndMessage = new CWndMessage;
		pWndMessage->Initialize();
		m_mapMessage.SetAt( lpszFrom, pWndMessage );
		string = pWndMessage->GetTitle();
		string += " - ";
		string += lpszFrom;
		pWndMessage->SetTitle( string );
		pWndMessage->m_strPlayer = lpszFrom;
	}
	else
	{
		pWndMessage->InitSize();
	}
	return pWndMessage;
}

#ifdef __J0519
BOOL CWndMgr::UpdateMessage( LPCTSTR pszOld, LPCTSTR pszNew )
{
	CWndMessage* pWndMessage	= NULL;
	if( m_mapMessage.Lookup( pszOld, (void*&)pWndMessage ) )
	{
		m_mapMessage.RemoveKey( pWndMessage->m_strPlayer );
		m_mapMessage.SetAt( pszNew, pWndMessage );
		CString string	= pWndMessage->GetTitle();
		string.Replace( pszOld, pszNew );
		pWndMessage->SetTitle( string );
		pWndMessage->m_strPlayer	= pszNew;
		return TRUE;
	}
	return FALSE;
}
#endif	// __J0519

CWndInstantMsg* CWndMgr::GetInstantMsg( LPCTSTR lpszFrom )
{
	CString string;
	CWndInstantMsg* pWndMessage = NULL;
	m_mapInstantMsg.Lookup( lpszFrom, (void*&)pWndMessage );
	return pWndMessage;
}
CWndInstantMsg* CWndMgr::OpenInstantMsg( LPCTSTR lpszFrom )
{
	CString string;
	CWndInstantMsg* pWndMessage = NULL;
	if( m_mapInstantMsg.Lookup( lpszFrom, (void*&)pWndMessage ) == FALSE )
	{
		CWndBase* pWndFocus = GetFocusWnd();
		pWndMessage = new CWndInstantMsg;
		pWndMessage->Initialize();
		pWndFocus->SetFocus();
		pWndMessage->m_strPlayer = lpszFrom;
		int nNumber = m_mapInstantMsg.GetCount();
		CRect rcMsg = pWndMessage->GetWindowRect();
		CRect rcWnd = GetLayoutRect();

		int nNumHeight = rcWnd.Height() / rcMsg.Height();
		int nNumWidth = rcWnd.Width() / rcMsg.Width();

		int nCount = ( nNumber / nNumHeight ) + 1; // +1안해주면 화면을 벗어난 곳에 열림 
		int nCount2 = ( nNumber % nNumHeight ) + 1;

		CPoint pt( rcWnd.right - ( rcMsg.Width() * nCount ) , rcWnd.bottom - ( rcMsg.Height() * nCount2 ) );
 		pWndMessage->Move( pt );
		m_mapInstantMsg.SetAt( lpszFrom, pWndMessage );
		
	}
	return pWndMessage;
}
// 레지스트 인포를 얻는다.
LPWNDREGINFO CWndMgr::GetRegInfo( DWORD dwWndId  )
{
	LPWNDREGINFO pWndRegInfo = NULL;
	m_mapWndRegInfo.Lookup( dwWndId, (void*&)pWndRegInfo );
	return pWndRegInfo;
}		
// 컬런트 윈도가 닫힐 때 호출 됨 
BOOL CWndMgr::PutRegInfo( CWndNeuz* pWndNeuz, BOOL bOpen )
{
	LPWNDREGINFO pWndRegInfo = NULL;
	BOOL bLookup = FALSE;
	if( m_mapWndRegInfo.Lookup( pWndNeuz->GetWndId(), (void*&)pWndRegInfo ) == TRUE )
	{
		SAFE_DELETE( pWndRegInfo->lpArchive );
		pWndRegInfo->dwSize = 0;
		bLookup = TRUE;
	}
	else
		pWndRegInfo = new WNDREGINFO;
	pWndRegInfo->dwWndId = pWndNeuz->GetWndId();
	pWndRegInfo->rect = pWndNeuz->GetWindowRect( TRUE );
	pWndRegInfo->bOpen = bOpen;
	pWndRegInfo->dwWndSize = pWndNeuz->m_nWinSize;
	pWndRegInfo->dwVersion = 0;
	CAr ar;
	int nSize;
	LPBYTE lpData;
	// Write
	pWndNeuz->SerializeRegInfo( ar, pWndRegInfo->dwVersion );
	lpData = ar.GetBuffer( &nSize );
	if( nSize )
	{
		pWndRegInfo->lpArchive = new BYTE[ nSize ];
		pWndRegInfo->dwSize = nSize;
		memcpy( pWndRegInfo->lpArchive, lpData, nSize );
	}
	else
	{
		pWndRegInfo->lpArchive = NULL;
		pWndRegInfo->dwSize = 0;
	}
	if( bLookup == FALSE )
		m_mapWndRegInfo.SetAt( pWndNeuz->GetWndId(), pWndRegInfo );
	return TRUE;
}
// 로드한 레지스트리인포를 세트할때 
BOOL CWndMgr::PutRegInfo( LPWNDREGINFO lpRegInfo )
{
	LPWNDREGINFO pWndRegInfo = NULL;
	if( m_mapWndRegInfo.Lookup( lpRegInfo->dwWndId, (void*&)pWndRegInfo ) == TRUE )
	{
		// 갱신 
		SAFE_DELETE( pWndRegInfo->lpArchive );
		memcpy( pWndRegInfo, lpRegInfo, sizeof( WNDREGINFO ) );
		if( lpRegInfo->dwSize )
		{
			pWndRegInfo->lpArchive = new BYTE[ lpRegInfo->dwSize ];
			memcpy( pWndRegInfo->lpArchive, lpRegInfo->lpArchive, lpRegInfo->dwSize );
		}
		else
			pWndRegInfo->lpArchive = NULL;
		return FALSE;
	}
	pWndRegInfo = new WNDREGINFO;
	memcpy( pWndRegInfo, lpRegInfo, sizeof( WNDREGINFO ) );
	if( lpRegInfo->dwSize )
	{
		pWndRegInfo->lpArchive = new BYTE[ lpRegInfo->dwSize ];
		memcpy( pWndRegInfo->lpArchive, lpRegInfo->lpArchive, lpRegInfo->dwSize );
	}
	else
		pWndRegInfo->lpArchive = NULL;
	m_mapWndRegInfo.SetAt( lpRegInfo->dwWndId, pWndRegInfo );
	return TRUE;
}
BOOL CWndMgr::SaveRegInfo( LPCTSTR lpszFileName )
{
	CFileIO file;
	if( file.Open( lpszFileName, "wb" ) == FALSE )
		return FALSE;
	POSITION pos = m_mapWndRegInfo.GetStartPosition();
	LPWNDREGINFO pWndRegInfo = NULL;
	//CRect* pRect;
	DWORD dwWndId;
	file.PutDW( REG_VERSION );
	file.PutDW( m_mapWndRegInfo.GetCount() );
	while( pos )
	{
		m_mapWndRegInfo.GetNextAssoc( pos, dwWndId, (void*&)pWndRegInfo );
		file.Write( &pWndRegInfo->dwWndId, sizeof( pWndRegInfo->dwWndId ) );
		file.Write( &pWndRegInfo->rect, sizeof( pWndRegInfo->rect ) );
		file.Write( &pWndRegInfo->dwWndSize, sizeof( pWndRegInfo->dwWndSize ) );
		file.Write( &pWndRegInfo->bOpen, sizeof( pWndRegInfo->bOpen ) );
		file.Write( &pWndRegInfo->dwVersion, sizeof( pWndRegInfo->dwVersion ) );
		file.Write( &pWndRegInfo->dwSize, sizeof( pWndRegInfo->dwSize ) );
		if( pWndRegInfo->dwSize )
			file.Write( pWndRegInfo->lpArchive, pWndRegInfo->dwSize );
	}
	file.Close();
	// resolution 파일 만들기. 현재 사이즈로 저장

	if( file.Open( "resolution", "wb" ) == FALSE )
		return FALSE;
	file.PutDW( g_Option.m_nResWidth );
	file.PutDW( g_Option.m_nResHeight );
	file.Close();
	return TRUE;
}
BOOL CWndMgr::LoadRegInfo( LPCTSTR lpszFileName )
{
	CFileIO file;
	BOOL bResult = FALSE;
	if( file.Open( "resolution", "rb" ) )
	{
		int nWidth = file.GetDW();
		int nHeight = file.GetDW();
		file.Close();
		// 해상도가 같으면 변경이 없는 상태다. 레지스트 파일 읽기 
		if( g_Option.m_nResWidth == nWidth && g_Option.m_nResHeight == nHeight )
		{
			bResult = TRUE;
			// 사이즈 저장 파일 로드 
			if( file.Open( lpszFileName, "rb" ) == FALSE )
				return FALSE;
			DWORD dwRegVersion = file.GetDW();
			if( dwRegVersion != REG_VERSION )
				return FALSE;
			WNDREGINFO wndRegInfo;
			CWndBase* pWndBase; 
			int nNum = file.GetDW();
			for( int i = 0; i < nNum; i++ )
			{
				file.Read( &wndRegInfo.dwWndId, sizeof( wndRegInfo.dwWndId ) );
				file.Read( &wndRegInfo.rect, sizeof( wndRegInfo.rect ) );
				file.Read( &wndRegInfo.dwWndSize, sizeof( wndRegInfo.dwWndSize ) );
				file.Read( &wndRegInfo.bOpen, sizeof( wndRegInfo.bOpen ) );
				file.Read( &wndRegInfo.dwVersion, sizeof( wndRegInfo.dwVersion ) );
				file.Read( &wndRegInfo.dwSize, sizeof( wndRegInfo.dwSize ) );
				if( wndRegInfo.dwSize )
				{
					wndRegInfo.lpArchive = new BYTE[ wndRegInfo.dwSize ];
					file.Read( wndRegInfo.lpArchive, wndRegInfo.dwSize );
				}
				else
					wndRegInfo.lpArchive = NULL;
				if( wndRegInfo.bOpen )
				{
					pWndBase = CreateApplet( wndRegInfo.dwWndId );
					if( pWndBase )
					{
						if( wndRegInfo.dwSize )
						{
							// load
							CAr ar( wndRegInfo.lpArchive, wndRegInfo.dwSize );
							((CWndNeuz*)pWndBase)->SerializeRegInfo( ar, wndRegInfo.dwVersion );
						}
						if( pWndBase->IsWndStyle( WBS_THICKFRAME ) )
						{
							if( wndRegInfo.dwWndSize == WSIZE_WINDOW )
							{
								((CWndNeuz*)pWndBase)->SetSizeWnd();
								pWndBase->SetWndRect( wndRegInfo.rect );
							}								
							if( wndRegInfo.dwWndSize == WSIZE_MAX )
								((CWndNeuz*)pWndBase)->SetSizeMax();
						}
						else
						{
							CRect wndRect = pWndBase->GetWindowRect();
							pWndBase->SetWndRect( 
								CRect( 
								wndRegInfo.rect.left, 
								wndRegInfo.rect.top, 
								wndRegInfo.rect.left + wndRect.Width(), 
								wndRegInfo.rect.top + wndRect.Height() 
								) );
						}							
					}
				}
				PutRegInfo( &wndRegInfo );//.dwWndId, wndRegInfo.rect, wndRegInfo.bOpen );
				SAFE_DELETE( wndRegInfo.lpArchive );
				//PutRegInfo( wndRegInfo.dwWndId, wndRegInfo.rect, wndRegInfo.bOpen );
			}
			file.Close();
		}
	}
	return bResult;
}

void CWndMgr::SetMessengerAutoState()
{
	if( m_bConnect && m_Messenger.GetState() == FRS_AUTOABSENT )
	{
		g_DPlay.SendSetState( FRS_ONLINE );	
#ifdef __SDEBUG
		m_timerMessenger.Set( SEC( 20 ) );
#else // __SDEBUG
		m_timerMessenger.Set( MIN( 5 ) );
#endif // __SDEBUG
	}
}