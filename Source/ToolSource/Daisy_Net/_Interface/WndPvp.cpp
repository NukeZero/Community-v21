#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndPvp.h"
#include "WndManager.h"

#ifdef __ONLINE
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/****************************************************
  WndId : APP_PVP - PVP
  CtrlId : WIDC_TABCTRL1 - 
****************************************************/

CWndPvp::CWndPvp() 
{ 
} 
CWndPvp::~CWndPvp() 
{ 
} 
void CWndPvp::OnDraw( C2DRender* p2DRender ) 
{ 
	CRect rect = GetClientRect();
	rect.bottom -= 30;
	//	p2DRender->RenderFillRect( rect, 0xff00ffff );
	int y = 0, nNext = 16;
	DWORD dwColor = D3DCOLOR_ARGB(255,0,0,0);
	//	TCHAR szString[ 32 ];
	//	p2DRender->TextOut( 5, y, _T( "Name"  ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Style" ) ); y += nNext;
	//	p2DRender->TextOut( 5, y, _T( "Job"   ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Guild" ) ); y += nNext;
	//	p2DRender->TextOut( 5, y, _T( "Level" ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Exp1." ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Exp2." ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Gold"  ) ); y += nNext;

	char szBuff[32];
	y = 15;

	strcpy( szBuff, g_pPlayer->GetFameName() );
	if( IsEmpty(szBuff) ) {	szBuff[0] = '-'; szBuff[1] = NULL; }
	p2DRender->TextOut( 90, y, szBuff, dwColor); y += nNext;
	strcpy( szBuff, g_pPlayer->GetSlaughterName() );
	if( IsEmpty(szBuff) ) {	szBuff[0] = '-'; szBuff[1] = NULL; }
	p2DRender->TextOut( 90, y, szBuff  , dwColor ); y += nNext;

//	p2DRender->TextOut( 90, y, g_pPlayer->GetFameName()       , dwColor); y += nNext;
//	p2DRender->TextOut( 90, y, g_pPlayer->GetSlaughterName()  , dwColor ); y += nNext;
	y = 58;
	p2DRender->TextOut( 100, y, g_pPlayer->m_nFame	, dwColor ); y += nNext;
	p2DRender->TextOut( 100, y, g_pPlayer->m_nSlaughter	, dwColor ); y += nNext;
	p2DRender->TextOut( 100, y, g_pPlayer->m_nNumKill	, dwColor ); y += nNext;
	
} 
void CWndPvp::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	/*
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 15;
	CWndTabCtrl* lpTapCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	//m_wndTabCtrl.Create( WBS_NOFRAME, rect, this, 12 );
	m_wndStateBase.Create( WBS_CHILD | WBS_NOFRAME , rect, lpTapCtrl, 100000 );

	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = "PVP";
	tabTabItem.pWndBase = &m_wndStateBase;
	lpTapCtrl->InsertItem( 0, &tabTabItem );
	lpTapCtrl->InsertTexture( 0 ,MakePath( DIR_THEME, "TabCharacter1.bmp" ) );
	
	lpTapCtrl->SetButtonLength( 55 );
	*/

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CPoint point( rectRoot.left, rectRoot.top + 96 + 16 );
	Move( point );
	
	// 윈도를 중앙으로 옮기는 부분.
/*	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();*/
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPvp::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PVP, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndPvp::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndPvp::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPvp::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPvp::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPvp::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndPvp::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// PVP정보
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndPvpBase::CWndPvpBase()
{
	m_bExpert = FALSE;
	m_pWndChangeJob = NULL;
	m_fWaitingConfirm	= FALSE;
	m_nDisplay = 1;

}
CWndPvpBase::~CWndPvpBase()
{
	SAFE_DELETE(m_pWndChangeJob);
}
void CWndPvpBase::OnDraw(C2DRender* p2DRender)
{
	if( g_pPlayer->IsBaseJob() )
	{
		if( g_pPlayer->GetLevel() >= MAX_JOB_LEVEL )
			m_wndChangeJob.EnableWindow( TRUE );
		else
			m_wndChangeJob.EnableWindow( FALSE );
	}
	else if( g_pPlayer->IsExpert() )
	{
		if( g_pPlayer->GetLevel() >= MAX_JOB_LEVEL + MAX_EXP_LEVEL )
			m_wndChangeJob.EnableWindow( TRUE );
		else
			m_wndChangeJob.EnableWindow( FALSE );
	}

	CRect rect = GetClientRect();
	rect.bottom -= 30;
	int y = 0, nNext = 15;
	DWORD dwColor = D3DCOLOR_ARGB(255,0,0,0);
	char szBuff[32];
	y = 22;

	int gap1 = 0;
	int gap2 = 0;
	
	if( g_Option.m_nLanguage == LANG_JAP )
	{
		gap1 = 10;
		gap2 = 33;
	}
	
#ifdef __NPP_050308	
	gap1 -= 10;
	gap2 -= 10;
	y = 15;
#endif //__NPP_050308	
	
#ifdef __NPP_050308	
	strcpy( szBuff, g_pPlayer->GetFameName() );
	if( IsEmpty(szBuff) ) {	szBuff[0] = '-'; szBuff[1] = NULL; }
	p2DRender->TextOut( 100+gap1, y, szBuff  , dwColor ); y += nNext;
	y = 33;
	p2DRender->TextOut( 100+gap2, y, g_pPlayer->m_nFame	, dwColor ); y += nNext;
	y += 4;
	p2DRender->TextOut( 100+gap2, y, g_pPlayer->m_nNumKill	, dwColor ); y += nNext;
	y += 4;
	p2DRender->TextOut( 100+gap2, y, g_pPlayer->GetSlaughterName(), dwColor ); y += nNext;
	y += 4;
	p2DRender->TextOut( 100+gap2, y, g_pPlayer->m_nSlaughter	, dwColor ); y += nNext;
#else //__NPP_050308
	strcpy( szBuff, g_pPlayer->GetFameName() );
	if( IsEmpty(szBuff) ) {	szBuff[0] = '-'; szBuff[1] = NULL; }
	p2DRender->TextOut( 95+gap1, y, szBuff, dwColor); y += nNext;
	strcpy( szBuff, g_pPlayer->GetSlaughterName() );
	if( IsEmpty(szBuff) ) {	szBuff[0] = '-'; szBuff[1] = NULL; }
	p2DRender->TextOut( 95+gap1, y, szBuff  , dwColor ); y += nNext;
	y = 65;
	p2DRender->TextOut( 95+gap2, y, g_pPlayer->m_nFame	, dwColor ); y += nNext;
	p2DRender->TextOut( 95+gap2, y, g_pPlayer->m_nSlaughter	, dwColor ); y += nNext;
	p2DRender->TextOut( 95+gap2, y, g_pPlayer->m_nNumKill	, dwColor ); y += nNext;
#endif //__NPP_050308
	
}
void CWndPvpBase::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	/*
	CString strDir = "Theme\\";
	switch( prj.m_nLanguage )
	{
	case LANG_KOR:
		strDir += "Korean\\";
		break;
	case LANG_ENG:
		strDir += "English\\";
		break;
	case LANG_JAP:
		strDir += "Japanese\\";
		break;
	case LANG_CHI:
		strDir += "Chinese\\";
		break;
	case LANG_THA:
		strDir += "Thailand\\";
		break;
	case LANG_TWN:
		strDir += "Taiwan\\";
		break;
	}
	
	SetTexture( m_pApp->m_pd3dDevice, MakePath( strDir, _T( "wndPvP.tga" ) ), TRUE );
	*/
	SetTexture( m_pApp->m_pd3dDevice, MakePath( "Theme\\", prj.m_nLanguage, _T( "wndPvP.tga" ) ), TRUE );
	


	//SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "wndPvP2.tga" ) ), TRUE );
	
	FitTextureSize();

	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize( 70, 25);

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

//	m_txInfo.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( "txInfo.bmp" ) ), 0 );
/*
	y = 53;
	m_wndStr.Create  ( "<", 0, CRect( 100,   y, 145 + 20, y + 16 ), this, 100 ); y += 13;
	m_wndSta.Create  ( "<", 0, CRect( 100,   y, 145 + 20, y + 16 ), this, 101 ); y += 13;
	m_wndDex.Create  ( "<", 0, CRect( 100,   y, 145 + 20, y + 16 ), this, 102 ); y += 13;
	m_wndInt.Create  ( "<", 0, CRect( 100,   y, 145 + 20, y + 16 ), this, 103 ); y += 13;

	m_wndStr.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "wndPlus.bmp" ) ), TRUE );
	m_wndSta.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "wndPlus.bmp" ) ), TRUE );
	m_wndDex.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "wndPlus.bmp" ) ), TRUE );
	m_wndInt.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "wndPlus.bmp" ) ), TRUE );

	LPJOBABILITY pJobAbillity;
	if( g_pPlayer->IsExpert() )
		pJobAbillity = &prj.m_aJobAbillity[ g_pPlayer->GetJob() ];
	else
		pJobAbillity = &prj.m_aJobAbillity[ JOB_VAGRANT ];
	m_wndInt.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwInt ) ? TRUE : FALSE );
	m_wndStr.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwStr ) ? TRUE : FALSE );
	m_wndSta.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwSta ) ? TRUE : FALSE );
	m_wndDex.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwDex ) ? TRUE : FALSE );
*/	
	y = 105;
	if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
		m_wndChangeJob.Create( ">", 0, CRect( 135, y, 135+40, y + 13 ), this, 10  ); 
	


}
BOOL CWndPvpBase::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 240, 0, 240 + 330, 255 - 135 ); 
	//SetTitle( GETTEXT( TID_APP_CHARACTER )  );
	return CWndBase::Create(WBS_THICKFRAME|WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_EXTENSION,rect,pWndParent,dwWndId);
}
BOOL CWndPvpBase::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{ 
	if( message == WNM_CLICKED )
	{
		switch( nID )
		{
		case JOB_MERCENARY: 
		case JOB_ACROBAT: 
		case JOB_ASSIST: 
		case JOB_MAGICIAN: 
		case JOB_PUPPETEER:
			{
				if( nID != g_pPlayer->GetJob() )
				{
					g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0037) ) );
					//						g_WndMng.OpenMessageBox( "자신의 직업만 올릴수 있습니다" );
				}
#ifndef __ONLINE
				g_pPlayer->IncJobLevel( nID );
#else	// __ONLINE
				if( m_fWaitingConfirm == FALSE ) {
					g_DPlay.SendIncJobLevel( nID );
					m_fWaitingConfirm = TRUE;
				}
#endif	// __ONLINE
				break;
			}
		case 10: // 전직 
			// 방랑자의 레벨이 15이상인것을 찾음
			if( g_pPlayer->GetJobLevel() >= MAX_JOB_LEVEL )
			{
				SAFE_DELETE(m_pWndChangeJob);
				m_pWndChangeJob = new CWndChangeJob( g_pPlayer->GetJob() ); 
				m_pWndChangeJob->Initialize( this, 1106 );
			}
			break;
		}
	}
	return CWndBase::OnChildNotify( message, nID, pLResult );
}
BOOL CWndPvpBase::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndBase::OnCommand(nID,dwMessage,pWndBase);
}
void CWndPvpBase::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize( 70, 25);

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	CWndBase::OnSize(nType,cx,cy);
}
void CWndPvpBase::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndPvpBase::OnLButtonDown(UINT nFlags, CPoint point)
{
}
