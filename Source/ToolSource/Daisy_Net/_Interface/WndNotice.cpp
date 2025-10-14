#include "stdafx.h"
#include "AppDefine.h"
#include "WndNotice.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/****************************************************
  WndId : APP_INFO_NOTICE - Notice
  CtrlId : WIDC_TEXT1 - 
  CtrlId : WIDC_CHECK1 - 다시 열지 않음
****************************************************/

CWndInfoNotice::CWndInfoNotice() 
{ 
#ifdef __YBANNER
	m_vecStrBanner.clear();
	m_dwChangeBannerTime = timeGetTime()+SEC(10);
	m_atexPannel = NULL;
#endif //__YBANNER
} 
CWndInfoNotice::~CWndInfoNotice() 
{ 
#ifdef __YBANNER
	SAFE_DELETE( m_atexPannel );	
#endif //__YBANNER
} 
void CWndInfoNotice::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndInfoNotice::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndButton* pWndCheck = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
	pWndCheck->SetCheck( !g_Option.m_bNotice );
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

#ifdef __YNOTICE_UNI1026
	CString strnotice;
	strnotice = GetLangFileName( g_Option.m_nLanguage, FILE_NOTICE );
		
	CScript scanner;
	if( scanner.Load( "Client\\"+strnotice ) == FALSE )
		return;
	
	pWndText->m_string.AddParsingString( scanner.m_pProg  );
	pWndText->ResetString();
#else //__YNOTICE_UNI1026
	CFileStatus fileStatus;
	
	CString strnotice;
	strnotice = GetLangFileName( g_Option.m_nLanguage, FILE_NOTICE );
	
	if( CFile::GetStatus( "Client\\"+strnotice, fileStatus ) == TRUE )
	{
		if( g_Option.m_tNoticeTime != fileStatus.m_mtime.GetTime() )
			g_Option.m_tNoticeTime = fileStatus.m_mtime.GetTime();

		CFileIO file;

		strnotice = GetLangFileName( g_Option.m_nLanguage, FILE_NOTICE );
		
		if( file.Open( "Client\\"+strnotice, "rb" ) )
		{
			int nLength = file.GetLength();
			TCHAR* pChar = new TCHAR[ nLength + 1];
			file.Read( pChar, nLength );
			pChar[ nLength ] = 0;
			//pWndText->SetString( pChar );
			pWndText->m_string.AddParsingString( pChar );
			pWndText->ResetString();
			delete pChar;
		}
	}
#endif //__YNOTICE_UNI1026

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

/////////////////////////////////////////////////////////////////////////////////////////
#ifdef __YBANNER
	int nCount = 0;	
	CScript script;
	if( script.Load(MakePath(DIR_THEME, "TexBannerList.inc" )) )
	{
		int nLang;
		nLang = script.GetNumber();
		do 
		{
			if( nLang == g_Option.m_nLanguage )
			{
				script.GetToken();
				
				nCount = atoi( script.token );
				
				script.GetToken();				
				
				for( int i=0; i<nCount; i++ )
				{
					CString addStr = script.token;
					m_vecStrBanner.push_back( addStr );
					script.GetToken();
				}
				
				if( nCount <= 0 )
				{
					Error( "TexBannerList.inc의 갯수가 0이다" );
					return;
				}
				
				break;
			}
			else
				script.GetLastFull();

			nLang = script.GetNumber();
		} while( script.tok != FINISHED );		
	}
	
	SAFE_DELETE( m_atexPannel );
	if( nCount > 0 )
	{
		m_atexPannel = new IMAGE;
		LoadImage( MakePath( DIR_THEME, m_vecStrBanner[xRandom(nCount)] ), m_atexPannel );
		AdjustWndBase();	
	}
#endif //__YBANNER
/////////////////////////////////////////////////////////////////////////////////////////	
} 

#ifdef __V050322_CACHEQUEST
void CWndInfoNotice::AfterSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat )
#else
void CWndInfoNotice::AdditionalSkinTexture( LPWORD pDest, CSize size )
#endif
{
#ifdef __YBANNER
	CPoint pt;
	LPWNDCTRL lpWndCtrl;
	
	CPoint pt2 = m_rectClient.TopLeft() - m_rectWindow.TopLeft();
	
	lpWndCtrl = GetWndCtrl( WIDC_COMMER_BANNER1 );
	pt = lpWndCtrl->rect.TopLeft() + pt2;
	
	if( m_atexPannel ) 
		PaintTexture( pDest, m_atexPannel, pt, size );	
#endif //__YBANNER
}

BOOL CWndInfoNotice::Process()
{
#ifdef __YBANNER
	if( timeGetTime() > m_dwChangeBannerTime )
	{
		m_dwChangeBannerTime = timeGetTime()+SEC(10);
		SAFE_DELETE( m_atexPannel );
		m_atexPannel = new IMAGE;
		LoadImage( MakePath( DIR_THEME, m_vecStrBanner[xRandom(m_vecStrBanner.size())] ), m_atexPannel );
		AdjustWndBase();	
	}
#endif //__YBANNER
	
	return TRUE;
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndInfoNotice::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_INFO_NOTICE, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndInfoNotice::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndInfoNotice::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndInfoNotice::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndInfoNotice::OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
#if defined(__KOREA__)
#ifdef __YBANNER
	DWORD dwCursor = 0xffffffff;

	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_COMMER_BANNER1 );
	if( PtInRect(&pCustom->rect, GetMousePoint() ) )
	{
		dwCursor = CUR_SELECT; 
	}	
	
	if( dwCursor == 0xffffffff )
		CWndNeuz::OnSetCursor( pWndBase, nHitTest, message );
	else
		SetMouseCursor( dwCursor );
#endif //__YBANNER
#endif 
	return TRUE;
}
void CWndInfoNotice::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
#if defined(__KOREA__)
	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_COMMER_BANNER1 );
	if( PtInRect(&pCustom->rect, point) )
	{
		g_WndMng.ObjectExecutor( SHORTCUT_APPLET, APP_WEBBOX );
	}	
#endif 
} 
BOOL CWndInfoNotice::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_CHECK1:
		g_Option.m_bNotice = !g_Option.m_bNotice;
		Destroy();		
		break;
	case WIDC_CLOSE:
		Destroy();
		return TRUE;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

