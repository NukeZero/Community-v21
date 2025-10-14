#include "stdafx.h"
#include "WndNotice.h"


/****************************************************
  WndId : APP_INFO_NOTICE - Notice
  CtrlId : WIDC_TEXT1 - 
  CtrlId : WIDC_CHECK1 - 다시 열지 않음
****************************************************/

CWndInfoNotice::CWndInfoNotice() 
{ 
	m_vecStrBanner.clear();
	m_dwChangeBannerTime = g_tmCurrent+SEC(10);
	m_atexPannel = NULL;
} 
CWndInfoNotice::~CWndInfoNotice() 
{ 
	SAFE_DELETE( m_atexPannel );	
} 
void CWndInfoNotice::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndInfoNotice::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndButton* pWndCheck = NULL;
	CWndText* pWndText = NULL;
	if(_GetContentState(CT_INFO_NOTICE)==CS_VER1)
	{
		pWndCheck = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
		pWndCheck->SetCheck( !g_Option.m_bNotice );
		pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	}
	else
	{
		pWndCheck = (CWndButton*)GetDlgItem( WIDC_CB_OPEN );
		pWndCheck->SetCheck( !g_Option.m_bNotice );
		pWndText = (CWndText*)GetDlgItem( WIDC_TEXT_NOTICE );
	}
	
	#ifdef __YNOTICE_UNI1026
	CString strnotice;
 	strnotice = GetLangFileName( g_xFlyffConfig->GetMainLanguage(), FILE_NOTICE );
		
	CScript scanner;
	if( scanner.Load( "Client\\"+strnotice ) == FALSE )
		return;
	
	pWndText->m_string.AddParsingString( scanner.m_pProg, pWndText->GetFontColor() );
	pWndText->ResetString();
#else //__YNOTICE_UNI1026
	CFileStatus fileStatus;
	
	CString strnotice;
	strnotice = GetLangFileName( g_xFlyffConfig->GetMainLanguage(), FILE_NOTICE );
	
	if( CFile::GetStatus( "Client\\"+strnotice, fileStatus ) == TRUE )
	{
		if( g_Option.m_tNoticeTime != fileStatus.m_mtime.GetTime() )
			g_Option.m_tNoticeTime = static_cast< time_t >( fileStatus.m_mtime.GetTime() );

		CFileIO file;

		strnotice = GetLangFileName( g_xFlyffConfig->GetMainLanguage(), FILE_NOTICE );
	
		if(g_xFlyffConfig->GetMainLanguage() == LANG_VTN)
		{
			if( file.Open( "Client\\"+strnotice, "rb" ) )
			{
				int nLength = file.GetLength();
				TCHAR* pChar = new TCHAR[ nLength + 2 ];
				file.Read( pChar, nLength );
				*(pChar + nLength ) = '\0';
				*(pChar + nLength + 1 ) = '\0';

				char* lpMultiByte = new char[ nLength + 2 ];

				int nResult = WideCharToMultiByteEx( g_xFlyffConfig->GetCodePage(), 0, 
					                               (LPWSTR)(pChar+2), -1, 
						                           lpMultiByte, nLength, 
												   NULL, NULL );

				if( nResult > 0 )
				{
					lpMultiByte[nResult-1] = 0;
					pWndText->m_string.AddParsingString( lpMultiByte );
					pWndText->ResetString();
				}
				SAFE_DELETE_ARRAY(pChar);
				SAFE_DELETE_ARRAY(lpMultiByte);
			}
		}
		else
		{
			if( file.Open( "Client\\"+strnotice, "rb" ) )
			{
				int nLength = file.GetLength();
				TCHAR* pChar = new TCHAR[ nLength + 1];
				file.Read( pChar, nLength );
				pChar[ nLength ] = 0;
				//pWndText->SetString( pChar );
				pWndText->m_string.AddParsingString( pChar );
				pWndText->ResetString();
				SAFE_DELETE( pChar );
			}
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
//  flyingjin :배너 삭제
// 	int nCount = 0;	
// 	CScript script;
// 	if( script.Load(MakePath(DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "TexBannerList.inc" )) )
// 	{
// 		int nLang;
// 		nLang = script.GetNumber();
// 		do 
// 		{
// 			if( nLang == g_xFlyffConfig->GetMainLanguage() )
// 			{
// 				script.GetToken();
// 				
// 				nCount = atoi( script.token );
// 				
// 				script.GetToken();				
// 				
// 				for( int i=0; i<nCount; i++ )
// 				{
// 					CString addStr = script.token;
// 					m_vecStrBanner.push_back( addStr );
// 					script.GetToken();
// 				}
// 				
// 				if( nCount <= 0 )
// 				{
// 					FLERROR_LOG( PROGRAM_NAME, _T( "TexBannerList.inc의 갯수가 0이다" ) );
// 					return;
// 				}
// 				
// 				break;
// 			}
// 			else
// 				script.GetLastFull();
// 
// 			nLang = script.GetNumber();
// 		} while( script.tok != FINISHED );		
// 	}
// 	
// 	SAFE_DELETE( m_atexPannel );
// 	if( nCount > 0 )
// 	{
// 		m_atexPannel = new IMAGE;
// 		LoadImage( MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),m_vecStrBanner[xRandom(nCount)] ), m_atexPannel );
// 		AdjustWndBase();	
// 	}
/////////////////////////////////////////////////////////////////////////////////////////	
} 

void CWndInfoNotice::AfterSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat )
{
//  flyingjin : 배너 삭제
// 	CPoint pt;
// 	LPWNDCTRL lpWndCtrl;
// 	
// 	CPoint pt2 = m_rectClient.TopLeft() - m_rectWindow.TopLeft();
// 	
// 	lpWndCtrl = GetWndCtrl( WIDC_COMMER_BANNER1 );
// 	pt = lpWndCtrl->rect.TopLeft() + pt2;
// 	
// 	if( m_atexPannel ) 
// 		PaintTexture( pDest, m_atexPannel, pt, size );	
}

BOOL CWndInfoNotice::Process()
{
//  flyingjin : 배너 삭제
// 	if( g_tmCurrent > m_dwChangeBannerTime )
// 	{
// 		m_dwChangeBannerTime = g_tmCurrent+SEC(10);
// 		SAFE_DELETE( m_atexPannel );
// 		m_atexPannel = new IMAGE;
// 		LoadImage( MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),m_vecStrBanner[xRandom(m_vecStrBanner.size())] ), m_atexPannel );
// 		AdjustWndBase();	
// 	}
// 	
	return TRUE;
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndInfoNotice::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	DWORD	dwWndID;
	// Daisy에서 설정한 리소스로 윈도를 연다.
	switch(_GetContentState(CT_INFO_NOTICE))	
	{
	case CS_VER1:
		dwWndID = APP_INFO_NOTICE;
		break;
	case CS_VER2:
		dwWndID = APP_NOTICE;
		break;
	}
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndID, 0, CPoint( 0, 0 ), pWndParent );
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
//  flyingjin : 배너 삭제
// 	if( g_xFlyffConfig->GetMainLanguage() == LANG_KOR )
// 	{
// 		DWORD dwCursor = 0xffffffff;
// 
// 		LPWNDCTRL pCustom = NULL;
// 		pCustom = GetWndCtrl( WIDC_COMMER_BANNER1 );
// 		if( PtInRect(&pCustom->rect, GetMousePoint() ) )
// 		{
// 			dwCursor = CUR_SELECT; 
// 		}	
// 		
// 		if( dwCursor == 0xffffffff )
// 			CWndNeuz::OnSetCursor( pWndBase, nHitTest, message );
// 		else
// 			SetMouseCursor( dwCursor );
// 	}
	return TRUE;
}
void CWndInfoNotice::OnLButtonDown( UINT nFlags, CPoint point ) 
{
//  flyingjin : 배너 삭제
// 	if( g_xFlyffConfig->GetMainLanguage() == LANG_KOR )
// 	{
// 		LPWNDCTRL pCustom = NULL;
// 		pCustom = GetWndCtrl( WIDC_COMMER_BANNER1 );
// 		if( PtInRect(&pCustom->rect, point) )
// 		{
// 			g_WndMng.ObjectExecutor( SHORTCUT_APPLET, APP_WEBBOX );
// 		}	
// 	}
} 
BOOL CWndInfoNotice::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch(_GetContentState(CT_INFO_NOTICE))
	{
	case CS_VER1:
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
		}
		break;
	case CS_VER2:
		{
			switch( nID )
			{
			case WIDC_CB_OPEN:
				g_Option.m_bNotice = !g_Option.m_bNotice;
				Destroy();		
				break;
			case WIDC_BT_CANCEL:
				Destroy();
				return TRUE;
			}
		}
		break;

	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

