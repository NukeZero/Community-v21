// WndArcane.cpp: implementation of the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defineText.h"
#include "DialogMsg.h"
#include "WndField.h"
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

//extern TCHAR g_szExpertName[ MAX_EXPERT ][ 32 ];
//extern TCHAR g_szJobName[ MAX_JOB ][ 32 ];
//extern const JOB prj.m_aJob[ MAX_JOB ];

//#define WND_WIDTH 210
//#define TASKBAR_HEIGHT 44

CWndJob::CWndJob()
{
	m_bExpert = FALSE;
	m_pWndChangeJob = NULL;
	m_fWaitingConfirm	= FALSE;
	m_nDisplay = 1;
}
CWndJob::~CWndJob()
{
}
void CWndJob::OnDraw(C2DRender* p2DRender)
{
	int y = 35, nNext = 15;
	DWORD dwColor = D3DCOLOR_ARGB(255,0,0,0);
//	TCHAR szString[ 32 ];

	y = 112;
	p2DRender->TextOut( 105, y, g_pPlayer->GetRemainLP(), dwColor ); y += nNext;

	if( g_pPlayer->GetJobLevel() >= MAX_JOB_LEVEL )
		m_wndChangeJob.EnableWindow( TRUE );
	else
		m_wndChangeJob.EnableWindow( FALSE );

	dwColor = 0xff0000ff;	
	
	if( m_nDisplay != g_pPlayer->IsExpert() )
	{
		m_nDisplay = g_pPlayer->IsExpert();
		if( m_nDisplay ) // 1차전직
		{
			m_wndButton[ JOB_VAGRANT	].SetVisible( FALSE );
			m_wndButton[ JOB_MERCENARY	].SetVisible( FALSE );
			m_wndButton[ JOB_ACROBAT	].SetVisible( FALSE );
			m_wndButton[ JOB_ASSIST		].SetVisible( FALSE );
			m_wndButton[ JOB_MAGICIAN	].SetVisible( FALSE );
			m_wndButton[ JOB_PUPPETEER	].SetVisible( FALSE );
			m_wndButton[g_pPlayer->GetJob()].SetVisible( TRUE );
		}
	}
	
	int nJobExp[ MAX_EXPERT ];
	if( g_pPlayer->IsExpert() )
	{
		y = 32;//27; 
		for( int i = MAX_JOBBASE; i < MAX_EXPERT; i++, y += nNext )
		{
//			if( g_pPlayer->GetJobBase() == i )
			if( g_pPlayer->GetJob() == i )
				dwColor = 0xff0000ff;
			else
				dwColor = 0xff000000;
			
			p2DRender->TextOut( 10, y, prj.m_aJob[ i  ].szName, dwColor ); 
			
//			if( g_pPlayer->IsExpert() && g_pPlayer->GetJobBase() == i )
			if( g_pPlayer->IsExpert() && g_pPlayer->GetJob() == i )
			{
				int nJobLevel = g_pPlayer->GetJobLevel();
				nJobExp[ i ]= prj.m_aExpLPPoint[ nJobLevel ];
				m_wndButton[ i ].EnableWindow( ( g_pPlayer->GetRemainLP() >= nJobExp[ i ] ) ? TRUE : FALSE );
				if( nJobLevel >= MAX_EXP_LEVEL )
					m_wndButton[ i ].EnableWindow( FALSE );
				p2DRender->TextOut( 80, y, nJobLevel, dwColor ); 
				p2DRender->TextOut( 135,y, nJobExp[ i ], dwColor ); 
			}
			else
			{
				nJobExp[ i ]= prj.m_aExpLPPoint[ g_pPlayer->m_aJobLv[ i ] ];
				m_wndButton[ i ].EnableWindow( ( g_pPlayer->GetRemainLP() >= nJobExp[ i ] ) ? TRUE : FALSE );
				if( !g_pPlayer->IsExpert() && g_pPlayer->m_aJobLv[ i ] >= MAX_JOB_LEVEL )
					m_wndButton[ i ].EnableWindow( FALSE );
				if( g_pPlayer->IsExpert() && g_pPlayer->m_aJobLv[ i ] >= MAX_EXP_LEVEL )
					m_wndButton[ i ].EnableWindow( FALSE );
				p2DRender->TextOut( 80, y, g_pPlayer->m_aJobLv[ i ], dwColor ); 
				p2DRender->TextOut( 135, y, nJobExp[ i ], dwColor ); 
			}
		}
	}
	else
	{
		y = 32;//27; 
		int nJobLevel = g_pPlayer->GetJobLevel();
		p2DRender->TextOut( 80, y, nJobLevel, dwColor );
		p2DRender->TextOut( 10, y, prj.m_aJob[ JOB_VAGRANT  ].szName, dwColor );
	}
	/*
	else
	{
		y = 7;
		p2DRender->TextOut( 100, y, _T( "5" ), dwColor ); y += nNext;
		p2DRender->TextOut( 100, y, _T( "5" ), dwColor ); y += nNext;
		p2DRender->TextOut( 100, y, _T( "5" ), dwColor ); y += nNext;

		y = 7;
		p2DRender->TextOut( 150, y, _T( "99" ), dwColor ); y += nNext;
		p2DRender->TextOut( 150, y, _T( "99" ), dwColor ); y += nNext;
		p2DRender->TextOut( 150, y, _T( "99" ), dwColor ); y += nNext;
	}
	*/
}
void CWndJob::OnInitialUpdate()
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

	SetTexture( m_pApp->m_pd3dDevice, MakePath( strDir, _T( "wndCharacter3.bmp" ) ), TRUE );
	*/
	SetTexture( m_pApp->m_pd3dDevice, MakePath( "Theme\\", prj.m_nLanguage, _T( "wndCharacter3.bmp" ) ), TRUE );
	FitTextureSize();

	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(70,25);//m_pSprPack->GetAt(9)->GetSize();

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	//리스트 아이콘 작은아이콘 
	//m_wndButton1.Create( "리스트"  , 0, rect3_1, this, IDABORT );
	//m_wndButton2.Create( "아이콘", 0, rect3_1, this, IDRETRY );
	//m_wndButton3.Create( "작은아이콘"  , 0, rect3_1, this, IDIGNORE);

	//m_txInfo.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( "txInfo.bmp" ) ), 0 );

	y = 31; 
	m_wndButton[ JOB_VAGRANT	].Create  ( "<", 0, CRect( 105,   y, 80 + 60,   y + 13 ), this, JOB_MERCENARY  );
	m_wndButton[ JOB_MERCENARY  ].Create  ( "<", 0, CRect( 105,   y, 80 + 60,   y + 13 ), this, JOB_MERCENARY  ); y += 15;
	m_wndButton[ JOB_ACROBAT    ].Create  ( "<", 0, CRect( 105,   y, 80 + 60,   y + 13 ), this, JOB_ACROBAT    ); y += 15;
	m_wndButton[ JOB_ASSIST     ].Create  ( "<", 0, CRect( 105,   y, 80 + 60,   y + 13 ), this, JOB_ASSIST     ); y += 15;
	m_wndButton[ JOB_MAGICIAN   ].Create  ( "<", 0, CRect( 105,   y, 80 + 60,   y + 13 ), this, JOB_MAGICIAN   ); y += 15;
	m_wndButton[ JOB_PUPPETEER  ].Create  ( "<", 0, CRect( 105,   y, 80 + 60,   y + 13 ), this, JOB_PUPPETEER   ); y += 15;

	m_wndButton[ JOB_VAGRANT	].SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndButton[ JOB_MERCENARY  ].SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndButton[ JOB_ACROBAT    ].SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndButton[ JOB_ASSIST     ].SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndButton[ JOB_MAGICIAN   ].SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndButton[ JOB_PUPPETEER  ].SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );

	m_nDisplay = g_pPlayer->IsExpert();
	if( m_nDisplay ) // 1차전직
	{
		m_wndButton[ JOB_VAGRANT	].SetVisible( FALSE );
		m_wndButton[ JOB_MERCENARY	].SetVisible( FALSE );
		m_wndButton[ JOB_ACROBAT	].SetVisible( FALSE );
		m_wndButton[ JOB_ASSIST		].SetVisible( FALSE );
		m_wndButton[ JOB_MAGICIAN	].SetVisible( FALSE );
		m_wndButton[ JOB_PUPPETEER	].SetVisible( FALSE );
		m_wndButton[g_pPlayer->GetJob()].SetVisible( TRUE );
	}
	else
	{
		m_wndButton[ JOB_VAGRANT	].SetVisible( FALSE );
		m_wndButton[ JOB_MERCENARY	].SetVisible( FALSE );
		m_wndButton[ JOB_ACROBAT	].SetVisible( FALSE );
		m_wndButton[ JOB_ASSIST		].SetVisible( FALSE );
		m_wndButton[ JOB_MAGICIAN	].SetVisible( FALSE );
		m_wndButton[ JOB_PUPPETEER	].SetVisible( FALSE );
	}
	
	y = 105;
	if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
		m_wndChangeJob.Create( ">", 0, CRect( 135, y, 135+40, y + 13 ), this, 10  ); 
/*
	m_wndButton2[JOB_MERCENARY  ].Create  ( ">", 0, CRect( 180,   y, 80 +112,   y + 16 ), this, 10 + JOB_MERCENARY ); y += 18;
	m_wndButton2[JOB_ACROBAT    ].Create  ( ">", 0, CRect( 180,   y, 80 +112,   y + 16 ), this, 10 + JOB_ACROBAT   ); y += 18;
	m_wndButton2[JOB_ASSIST     ].Create  ( ">", 0, CRect( 180,   y, 80 +112,   y + 16 ), this, 10 + JOB_ASSIST    ); y += 18;
	m_wndButton2[JOB_MAGICIAN   ].Create  ( ">", 0, CRect( 180,   y, 80 +112,   y + 16 ), this, 10 + JOB_MAGICIAN  ); y += 18;
	m_wndButton2[JOB_ENGINEER   ].Create  ( ">", 0, CRect( 180,   y, 80 +112,   y + 16 ), this, 10 + JOB_ENGINEER  ); y += 18;
	*/
/*
	y = 5;
	m_wndExpert[ 0 ].Create( _T( "" ), 0, CRect( 5,   y, 90,   y + 16 ), this, 150 ); y += 18;
	m_wndExpert[ 1 ].Create( _T( "" ), 0, CRect( 5,   y, 90,   y + 16 ), this, 151 ); y += 18;
	m_wndExpert[ 2 ].Create( _T( "" ), 0, CRect( 5,   y, 90,   y + 16 ), this, 152 ); y += 18; y += 18;

	//m_wndBack.Create( "Back" , 0, CRect( 5,   y, 90,   y + 16 ), this, 200 ); y += 18;

	m_wndExpert[ 0 ].SetVisible( FALSE );
	m_wndExpert[ 1 ].SetVisible( FALSE );
	m_wndExpert[ 2 ].SetVisible( FALSE );
*/
	//m_wndBack.SetVisible( FALSE );
/*	if( !g_pPlayer->IsExpert() )
	{
		m_wndButton[ JOB_MERCENARY	].SetVisible( FALSE );
		m_wndButton[ JOB_ACROBAT	].SetVisible( FALSE );
		m_wndButton[ JOB_ASSIST		].SetVisible( FALSE );
		m_wndButton[ JOB_MAGICIAN	].SetVisible( FALSE );
		m_wndButton[ JOB_PUPPETEER	].SetVisible( FALSE );
	}
	else
	{
		int nJobExp[ 5 ];
		for( int i = MAX_JOBBASE; i < MAX_EXPERT; i++ )
		{
//			if( g_pPlayer->IsExpert() && g_pPlayer->GetJobBase() == i )
			if( g_pPlayer->IsExpert() && g_pPlayer->GetJob() == i )
			{
				int nJobLevel = g_pPlayer->GetJobLevel();
				nJobExp[ i ]= prj.m_aExpLPPoint[ nJobLevel ];
				if( m_fWaitingConfirm == TRUE ) {
					m_wndButton[ i ].EnableWindow( FALSE );
				}
				else {
					m_wndButton[ i ].EnableWindow( ( g_pPlayer->GetRemainLP() >= nJobExp[ i ] ) ? TRUE : FALSE );
					if( nJobLevel >= MAX_EXP_LEVEL )
						m_wndButton[ i ].EnableWindow( FALSE );
				}
			}
			else
			{
				if( m_fWaitingConfirm == TRUE ) {
					m_wndButton[ i ].EnableWindow( FALSE );
				}
				else {
					nJobExp[ i ]= prj.m_aExpLPPoint[ g_pPlayer->m_aJobLv[ i ] ];
					m_wndButton[ i ].EnableWindow( ( g_pPlayer->GetRemainLP() >= nJobExp[ i ] ) ? TRUE : FALSE );
					if( !g_pPlayer->IsExpert() && g_pPlayer->m_aJobLv[ i ] >= MAX_JOB_LEVEL )
						m_wndButton[ i ].EnableWindow( FALSE );
					if( g_pPlayer->IsExpert() && g_pPlayer->m_aJobLv[ i ] >= MAX_EXP_LEVEL )
						m_wndButton[ i ].EnableWindow( FALSE );
				}
			}
		}	
	}
*/
	CWndBase::OnInitialUpdate();
}
BOOL CWndJob::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	//CRect rect( 240, 0, 240 + 240, 90 ); 
	CRect rect( 240, 0, 240 + 205, 142 ); 
	//m_wndUserArray.SetSize(5);
//	SetTitle( GETTEXT( TID_APP_JOB )  );
	return CWndBase::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_EXTENSION, rect, pWndParent, dwWndId );
}
void CWndJob::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( pWndChild == m_pWndChangeJob )
	{
		/*
		m_wndJob[ g_pPlayer->GetJob() ].SetTitle( g_pPlayer->GetJobString() );
		int nNum = 0;
		LPSKILL lpSkill;
		for( int i = 1; i < prj.m_aPropSkill.GetSize(); i++ )
		{
			ItemProp* pSkillProp = prj.m_aPropSkill.GetAt( i );
			if( pSkillProp && pSkillProp->dwItemKind3 == g_pPlayer->m_nExpert )
			{
				lpSkill = &g_pPlayer->m_aJobSkill[ nNum++ ];
				lpSkill->dwSkill = pSkillProp->dwID;
			}
		}
		*/
		SAFE_DELETE( m_pWndChangeJob );
	}
}
BOOL CWndJob::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
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
					m_pWndChangeJob = new CWndChangeJob( g_pPlayer->GetJob() ); 
					m_pWndChangeJob->Initialize( this, 1106 );
				}
				break;
		}
	}
	return CWndBase::OnChildNotify( message, nID, pLResult );
}
BOOL CWndJob::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndBase::OnCommand( nID, dwMessage, pWndBase );
}
void CWndJob::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(70,25);//m_pSprPack->GetAt(9)->GetSize();

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	//m_wndButton1.SetWndRect( rect3_1 );
	//m_wndButton2.SetWndRect( rect3_2 );
	//m_wndButton3.SetWndRect( rect3_3 );

	CWndBase::OnSize(nType,cx,cy);
}
void CWndJob::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;

	//if(IsWndStyle(WBS_CAPTION) && m_bPickup)
	{//
	//	m_wndTitleBar.m_wndMinimize.SetVisible(TRUE);
		//m_wndTitleBar.m_wndMaximize.SetVisible(TRUE);
	}
}
void CWndJob::OnLButtonDown(UINT nFlags, CPoint point)
{
//	CWndBase::OnLButtonDown(nFlags,point
	if(IsWndRoot())
		return;
//	return;

}

