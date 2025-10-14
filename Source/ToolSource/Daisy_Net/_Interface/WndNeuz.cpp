// WndArcane.cpp: implementation of the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resdata.h"
#include "WndStyle.h"
#include "WndHelp.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 타이틀 바 (축소, 최대, 종료 버튼)
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
#define WTBID_CLOSE  100
#define WTBID_MAX    101
#define WTBID_MIN    102
#define WTBID_HELP   103
#define WTBID_REPORT 104
#define WTBID_PIN    105
*/

CWndTitleBar::CWndTitleBar()
{
	
}

CWndTitleBar::~CWndTitleBar()
{

}
void CWndTitleBar::OnDraw(C2DRender* p2DRender)
{
}
void CWndTitleBar::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	m_nButtonMax = 0;
	if(m_pParentWnd->IsWndStyle( WBS_PIN ) )
		m_awndButton[ WTBID_PIN - 10000 ].Create( _T( "P" ), 0, CRect( m_nButtonMax * 16, 1, m_nButtonMax * 16 + 16, 20), this, WTBID_PIN ), m_nButtonMax++;

	if(m_pParentWnd->IsWndStyle( WBS_VIEW ) )
		m_awndButton[ WTBID_REPORT - 10000 ].Create( _T( "V" ), 0, CRect( m_nButtonMax * 16, 1, m_nButtonMax * 16 + 16, 20), this, WTBID_REPORT ), m_nButtonMax++;

	if(m_pParentWnd->IsWndStyle( WBS_EXTENSION ) ) 
		m_awndButton[ WTBID_EXTENSION - 10000 ].Create( _T( "E" ), WBS_CHECK | WBS_PUSHLIKE , CRect( m_nButtonMax * 15, 0, m_nButtonMax * 16 + 16, 20), this, WTBID_EXTENSION ), m_nButtonMax++;

	if(m_pParentWnd->IsWndStyle( WBS_HELP ) )
	{
		m_awndButton[ WTBID_HELP - 10000 ].Create( _T( "?" ), 0, CRect( m_nButtonMax * 16, 1 , m_nButtonMax * 16 + 16, 20), this, WTBID_HELP ), m_nButtonMax++;
//#ifdef __CLIENT
		m_awndButton[ WTBID_HELP - 10000 ].SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,CWndBase::m_resMng.GetSelectLang(),"ButtWndHelp.tga" ), TRUE );
		m_awndButton[ WTBID_HELP - 10000 ].FitTextureSize();
//#endif
	}
	if(m_pParentWnd->IsWndStyle( WBS_MINIMIZEBOX ) )
		m_awndButton[ WTBID_MIN - 10000 ].Create( _T( "_" ), 0, CRect( m_nButtonMax * 16, 1 , m_nButtonMax* 16 + 16, 20), this, WTBID_MIN ), m_nButtonMax++;

	if(m_pParentWnd->IsWndStyle( WBS_MAXIMIZEBOX ) )
	{
		m_awndButton[ WTBID_MAX - 10000 ].Create( _T( "|" ), 0, CRect( m_nButtonMax * 16, 1 , m_nButtonMax* 16 + 16, 20), this, WTBID_MAX ), m_nButtonMax++;
//#ifdef __CLIENT
		m_awndButton[ WTBID_MAX - 10000 ].SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(),"ButtWndMax.tga" ), TRUE );
		m_awndButton[ WTBID_MAX - 10000 ].FitTextureSize();
//#endif
	}

	if(m_pParentWnd->IsWndStyle( WBS_CAPTION ) )
	{
		if(m_pParentWnd->m_bNoCloseButton == FALSE)
		{
			m_awndButton[ WTBID_CLOSE - 10000 ].Create( _T( "x" ), 0, CRect( m_nButtonMax * 16, 1 , m_nButtonMax * 16 + 16, 20), this, WTBID_CLOSE ), m_nButtonMax++;
			m_awndButton[ WTBID_CLOSE - 10000 ].SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(),"ButtWndExit.tga" ),TRUE);
			m_awndButton[ WTBID_CLOSE - 10000 ].FitTextureSize();
		}
	
	}
	
	CRect rect( 0, 4, m_nButtonMax * 16, 20 ); 

	SetWndRect( rect );

		///wndTitleBar.Create( WBS_CHILD | WBS_DOCKING | WBS_NOFRAME, CRect(0,0,48,20), this, 100000);// ,m_pSprPack,9);
		//m_wndTitleBar.SetTitle("fuck");
		//m_wndTitleBar.Move(CPoint(GetWindowRect().Width()-52,3)); //SetWndRect(rect);

}
void CWndTitleBar::ViweCloseButton(BOOL bButtonView)
{
//	if(bButtonView)
// 	{
// 		m_awndButton[ WTBID_CLOSE - 10000 ].Create( _T( "x" ), 0, CRect( m_nButtonMax * 16, 1 , m_nButtonMax * 16 + 16, 20), this, WTBID_CLOSE ), m_nButtonMax++;
// 		m_awndButton[ WTBID_CLOSE - 10000 ].SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtWndExit.tga" ),FALSE);
// 		m_awndButton[ WTBID_CLOSE - 10000 ].FitTextureSize();
// 		CRect rect( 0, 4, m_nButtonMax * 16, 20 ); 
// 
// 		SetWndRect( rect );	
//	}
//	else
//	{
		
//	}

}





void CWndTitleBar::Replace()
{
	if( m_pParentWnd )
		Move( CPoint( m_pParentWnd->GetWindowRect().Width() - m_nButtonMax * 16 - 10, 7 ) ); 
}
BOOL CWndTitleBar::Initialize(CWndBase* pWndParent,DWORD dwStyle)
{
	return TRUE;
}
BOOL CWndTitleBar::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	//if(m_pParentWnd) m_pParentWnd->OnCommand(nID,dwMessage);
	return TRUE;
}
BOOL CWndTitleBar::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if(m_pParentWnd) m_pParentWnd->OnChildNotify( message, nID, pLResult );
	return TRUE;
}
void CWndTitleBar::OnSize(UINT nType, int cx, int cy)
{
}
BOOL CWndTitleBar::OnEraseBkgnd(C2DRender* p2DRender)
{
	return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Neuz 윈도 기본 베이스 클레스 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndNeuz::CWndNeuz()
{
	m_bFullMax = FALSE;	
	m_dwWID = 0;
	m_ptMouseCenter = CPoint( -1, -1 );
//	m_bWndTile = FALSE;//TRUE;
	m_dwColor = 0xffffffff;
	m_bPutRegInfo = TRUE;
#ifdef __V050322_CACHEQUEST
	m_d3dFormat = D3DFMT_A4R4G4B4 ;
#endif // __V050322_CACHEQUEST
}

CWndNeuz::~CWndNeuz()
{
	// PLAYSND( SND_INF_CLOSE );
	for( int i = 0; i < m_wndArrayTemp.GetSize(); i++ )
		delete (CWndBase*)m_wndArrayTemp.GetAt( i );
#ifdef __CLIENT
	// 애플렛으로 등록된 윈도만 파괴할 때 윈도의 정보를 저장한다. 
	//if( g_WndMng.GetAppletFunc( GetWndId() ) && m_bPutRegInfo )
	//	g_WndMng.PutRegInfo( this, 1, FALSE );
	//	g_WndMng.PutRegInfo( GetWndId(), GetWindowRect( TRUE ), FALSE );
	// 타일 형태의 윈도는 윈도가 종료할 때 텍스춰를 파괴해야한다.
#ifdef __V050322_CACHEQUEST
	if( m_strTexture.IsEmpty() == FALSE )
#else
	if( m_strTile.IsEmpty() == FALSE )
#endif
	{
		// 타일 형태는 파일을 읽는 것이 아니므로 자기 포인터가 텍스춰 ID로 사용되었다.
		CString strTextureId;
		strTextureId.Format( "%p", this );
		m_textureMng.RemoveTexture( strTextureId );
	}
#endif
}
#ifdef __CLIENT
void CWndNeuz::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	if( ar.IsLoading() )
	{

	}
	else
	{
		dwVersion = 0;
	}
}
#endif
void CWndNeuz::OnDraw(C2DRender* p2DRender)
{
}

//---------------------------------------------------------------------------------------------
// 윈도우 타이틀 바 글씨 위치 구하기 ( 현재 가온데로 정렬 됨 )
// param	: void
// return	: CPoint
// Desc		: 현재 높이는 구하기가 어렵워서 하드코딩으로 합니다. 추후 수정이 필요
//---------------------------------------------------------------------------------------------
CPoint CWndNeuz::CalcTitleStringPosition()
{
	CPoint ResSize( 0, 0 );

	

	CD3DFont* pCurrentFont = CWndBase::m_Theme.m_pFontWndTitle;
	FLERROR_ASSERT_LOG_RETURN( pCurrentFont, ResSize, _T("[윈도우 Neuz] 폰트 포인터가 유효하지 않습니다.") );

	CRect WndRect( GetClientRect( TRUE ) );
	CSize TitleStringSize( pCurrentFont->GetTextExtent( m_strTitle ) );
		
	UINT nDialogHalfWidthSize( ( static_cast<UINT>( WndRect.right - WndRect.left ) * 0.5f ) );
	UINT nTitleStringHalfWidthSize( static_cast<UINT>( TitleStringSize.cx * 0.5f ) );

#ifdef __FREE_TYPE_FONT
	CRect rtCaptionBar = WndRect;
	rtCaptionBar.bottom = rtCaptionBar.top + 23;

	UINT nCaptionHalfHeightSize( ( static_cast<UINT>( rtCaptionBar.bottom - rtCaptionBar.top ) * 0.5f ) );

	ResSize.y = nCaptionHalfHeightSize;
#else
	ResSize.y = 8;
#endif

	ResSize.x = nDialogHalfWidthSize - nTitleStringHalfWidthSize;
	

	return ResSize;
}


//---------------------------------------------------------------------------------------------
// 그리는 프레임
// param	: p2DRender
// return	: void
//---------------------------------------------------------------------------------------------
void CWndNeuz::PaintFrame( C2DRender* p2DRender )
{
	FLERROR_ASSERT_LOG_RETURN_VOID( p2DRender, _T("[윈도우 Neuz] 2D Render 포인터가 유효하지 않습니다.") );

	if( IsFullMax() && m_nWinSize == WSIZE_MAX )
		return;

	CWndBase::PaintFrame( p2DRender );

	if( IsWndStyle( WBS_CAPTION ) )	
	{
		CPoint TitleStringPosition( CalcTitleStringPosition() );

		if( m_pTexture )
		{
			CD3DFont* pOldFont = p2DRender->GetFont();
			FLERROR_ASSERT_LOG_RETURN_VOID( pOldFont, _T("[윈도우 Neuz] 이전 폰트 포인터가 유효하지 않습니다.") );

			CD3DFont* pNewFont = CWndBase::m_Theme.m_pFontWndTitle;
			FLERROR_ASSERT_LOG_RETURN_VOID( pOldFont, _T("[윈도우 Neuz] 새 폰트 포인터가 유효하지 않습니다.") );

			p2DRender->SetFont( pNewFont );
	
			m_pTheme->RenderWndBaseTitleBar( p2DRender, TitleStringPosition.x, TitleStringPosition.y, m_strTitle, m_dwColor );

			p2DRender->SetFont( pOldFont );
		}
		else
		{
			CRect Rect = GetWindowRect();
			Rect.bottom = 21;
			m_pTheme->RenderWndBaseTitleBar( p2DRender, &Rect, TitleStringPosition.x, TitleStringPosition.y, m_strTitle, m_dwColor );
		}
	}
}


void CWndNeuz::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();
	
	m_rectBackup = m_rectWindow;

	int x = m_rectWindow.Width() / 2;
	int y = m_rectWindow.Height() - 45;
	CSize size = CSize( 60, 25 );//m_pSprPack->GetAt(9)->GetSize();
//	//DWORD dwStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID);
	if( IsWndStyle( WBS_CAPTION ) )
	{ 
		m_wndTitleBar.Create( WBS_CHILD | WBS_DOCKING | WBS_NODRAWFRAME | WBS_NOFRAME, CRect(0,0,0,0), this, 100000 );
		m_wndTitleBar.SetTitle( "fuck" );
		m_wndTitleBar.Replace(); 
	}
//	m_wndButton.Create("OK",0,CRect(0,0,0+size.cx,0+size.cy),this,IDOK);//,m_pSprPack,9);
//	m_wndClose.SetDefault(TRUE);

	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( GetWndId() );
	if( lpWndApplet )
	{
//#ifndef __NEWINTERFACE
//		lpWndApplet->m_bTile = FALSE;
//#endif
		SetTitle( lpWndApplet->strTitle );
#ifdef __V050322_CACHEQUEST
		m_bTile = ( lpWndApplet->m_bTile != FALSE );
		m_strTexture = lpWndApplet->strTexture;
		m_d3dFormat = lpWndApplet->d3dFormat;
#else
//{{AFX
		if( lpWndApplet->m_bTile )
			m_strTile = lpWndApplet->strTexture;
		else
		if( lpWndApplet->strTexture.IsEmpty() == FALSE )
			SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(),lpWndApplet->strTexture ), TRUE );
//}}AFX
#endif


		for( int i = 0; i < lpWndApplet->ptrCtrlArray.GetSize(); i++ )
		{
			m_wndArrayTemp.Add(CreateControl( m_pApp->GetSafeHwnd(), (LPWNDCTRL)lpWndApplet->ptrCtrlArray.GetAt( i ) ) );
		}
	}


//	SetFocus();
	//m_bWndTile = TRUE;
//	AdjustWndBase();
}
#ifdef __V050322_CACHEQUEST
void CWndNeuz::AdjustWndBase( D3DFORMAT d3dFormat )
{
	if( m_strTexture.IsEmpty()|| IsVisible() == FALSE || IsWndStyle( WBS_NODRAWFRAME ) )
	{
		return;
	}

	if( m_bTile == FALSE )
	{
#ifndef __TOON
		SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(), m_strTexture ) );
		return ;
#endif

#ifdef __TOON

		SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(), m_strTexture ) );
		return ;

		m_textureMng.RemoveTexture( MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(), m_strTexture ) );

		CRect rect = GetWindowRect( TRUE );
		CSize size2, size1 = CSize( rect.Width(), rect.Height() );
		size2 = size1;

		AdjustSize( &size1 );

		SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(), m_strTexture ), TRUE );

		CPoint point( 0, 0);
		D3DLOCKED_RECT lockedRect;
		if( m_pTexture->m_pTexture->LockRect( 0, &lockedRect, 0, 0 ) == D3D_OK )// Lock
		{
			LPWORD pDest = (LPWORD)lockedRect.pBits;
			for( int i = 0; i < m_wndArray.GetSize(); i++ )
			{
				CWndBase* pWndBase = (CWndBase*)m_wndArray.GetAt( i );

				if( pWndBase->IsDestroy() == FALSE && pWndBase->IsVisible() )
					pWndBase->AdditionalSkinTexture( pDest, size1, m_d3dFormat );
			}

			AfterSkinTexture( pDest, size1 );
			SetWndRect( rect );
			m_pTexture->m_pTexture->UnlockRect( 0 );
		}

		return ;
#endif 
	}

	CWndBase::AdjustWndBase( m_d3dFormat );
}
#else // __V050322_CACHEQUEST
//{{AFX
void CWndNeuz::AdjustWndBase()
{
	if( m_strTile.IsEmpty()  )
		return;
	CString strTextureId;
	strTextureId.Format( "%p", this );
	m_textureMng.RemoveTexture( strTextureId );

	CRect rect = GetWindowRect( TRUE );
	CSize size2, size1 = CSize( rect.Width(), rect.Height() );
	size2 = size1;

	AdjustSize( &size1 );

	CTexture* pTexture = new CTexture;
	pTexture->CreateTexture( m_pApp->m_pd3dDevice, size1.cx, size1.cy, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED );

	SetTexture( m_pApp->m_pd3dDevice, strTextureId, pTexture );


	LPIMAGE lpImage[12];
	CSize sizeBmp[12];
	CString strFileName;
	/*
	0 1 2
	3 4 5
	6 7 8
	9 0 1
	*/
//	m_strTile = "WndTile00.bmp";
	int nImgBit = IMGBIT_32;
	for( int i = 0; i < 12; i++ )
	{
		CString strTemp1 = m_strTile.Left( m_strTile.GetLength() - 6 );
		CString strTemp2 = m_strTile.Right( 4 );
		strFileName.Format( "%s%02d%s", strTemp1, i, strTemp2 );
		if( m_strWndTileMap.Lookup( strFileName, (void*&)lpImage[i] ) == FALSE )
		{
			lpImage[i] = new IMAGE;
			LoadImage( MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(),strFileName ), lpImage[i] );//, &sizeBmp[i], &nImgBit );
			m_strWndTileMap.SetAt( strFileName, lpImage[i] );
		}
		//else
		//{
		//	sizeBmp[i] = CSize(16,16);
		//}
	}
	sizeBmp[0] = CSize(16,16);
	CSize sizeDiv = size2;
	sizeDiv.cx %= sizeBmp[ 0 ].cx;
	sizeDiv.cy %= sizeBmp[ 0 ].cy;
	size2.cx /= sizeBmp[ 0 ].cx; size2.cx *= sizeBmp[ 0 ].cx;
	size2.cy /= sizeBmp[ 0 ].cy; size2.cy *= sizeBmp[ 0 ].cy;
	if( sizeDiv.cx ) size2.cx += sizeBmp[ 0 ].cx;
	if( sizeDiv.cy ) size2.cy += sizeBmp[ 0 ].cy;
	// 텍스춰 만들기 
	CPoint point( 0, 0);
	D3DLOCKED_RECT lockedRect;
	if( pTexture->m_pTexture->LockRect( 0, &lockedRect, 0, 0 ) == D3D_OK )// Lock
	{
		LPWORD pDest = (LPWORD)lockedRect.pBits;
		i = 0;
		CSize sizeTile = size2;
		sizeTile.cx /= 16;
		sizeTile.cy /= 16;
		for( int y = 0; y < sizeTile.cy; y++ )
		{
			point.x = 0;
			for( int x = 0; x < sizeTile.cx; x++)
			{
				///////////////////////////////
				if( x == 0 && y == 0 ) i = 0; else
				if( x < sizeTile.cx - 1 && y == 0 ) i = 1; else
				if( x == sizeTile.cx - 1 && y == 0 ) i = 2; else
				///////////////////////////////
				if( x == 0 && y == 1 ) i = 3; else
				if( x < sizeTile.cx - 1 && y == 1 ) i = 4; else
				if( x == sizeTile.cx - 1 && y == 1 ) i = 5;	else
				///////////////////////////////
				if( x == 0 && y < sizeTile.cy - 1 )	i = 6; else
				if( x < sizeTile.cx - 1 && y < sizeTile.cy - 1 ) i = 7; else
				if( x == sizeTile.cx - 1 && y < sizeTile.cy - 1 ) i = 8; else
				///////////////////////////////
				if( x == 0 && y == sizeTile.cy - 1 ) i = 9; else
				if( x < sizeTile.cx - 1 && y == sizeTile.cy - 1 ) i = 10; else
				if( x == sizeTile.cx - 1 && y == sizeTile.cy - 1 ) i = 11;
				//if( i == 0 )
				PaintTexture( pDest, lpImage[i], point, size1 );
				point.x += 16;
			}
			point.y += 16;
		}
		pTexture->m_pTexture->UnlockRect( 0);
	}
	
	//for( i = 0; i < 12; i++ )
	//	SAFE_DELETE( lpData[i] );
	rect.bottom = rect.top + size2.cy;
	rect.right = rect.left + size2.cx;
	//SetWndRect( rect, FALSE );
}
//}}AFX
#endif // __V050322_CACHEQUEST

void CWndNeuz::AdditionalSkinTexture( LPWORD pDest, CSize size1, D3DFORMAT d3dFormat )
{
#ifdef __V050322_CACHEQUEST

	CRect rect = GetWindowRect( TRUE );
	CSize size2 = CSize( rect.Width(), rect.Height() );

	LPIMAGE lpImage[12];
	CSize sizeBmp[12];
	CString strFileName;
	//DEBUGOUT( "%s -----------------------------------------------------------------\n", m_strTexture );
	/*
	0 1 2
	3 4 5
	6 7 8
	9 0 1
	*/
//	m_strTile = "WndTile00.bmp";
	int nImgBit = IMGBIT_32;
	int i = 0;
	for( ; i < 12; i++ )
	{
		CString strTemp1 = m_strTexture.Left( m_strTexture.GetLength() - 6 );
		CString strTemp2 = m_strTexture.Right( 4 );
		strFileName.Format( "%s%02d%s", strTemp1, i, strTemp2 );
		if( m_strWndTileMap.Lookup( strFileName, (void*&)lpImage[i] ) == FALSE )
		{
			lpImage[i] = new IMAGE;
			lpImage[i]->size.cx = 99;
			LoadImage( m_pApp->GetDevice(), MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(),strFileName ), lpImage[i] );//, &sizeBmp[i], &nImgBit );
			m_strWndTileMap.SetAt( strFileName, lpImage[i] );
			//DEBUGOUT( "%x, %d, %d \n",lpImage[0]->lpData, lpImage[0]->size.cx, lpImage[0]->size.cy );
		}
		//else
		//{
		//	sizeBmp[i] = CSize(16,16);
		//}
	}
	sizeBmp[0] = CSize(16,16);
	CSize sizeDiv = size2;
	sizeDiv.cx %= sizeBmp[ 0 ].cx;
	sizeDiv.cy %= sizeBmp[ 0 ].cy;
	size2.cx /= sizeBmp[ 0 ].cx; size2.cx *= sizeBmp[ 0 ].cx;
	size2.cy /= sizeBmp[ 0 ].cy; size2.cy *= sizeBmp[ 0 ].cy;
	if( sizeDiv.cx ) size2.cx += sizeBmp[ 0 ].cx;
	if( sizeDiv.cy ) size2.cy += sizeBmp[ 0 ].cy;

	i = 0;
	CSize sizeTile = size2;
	sizeTile.cx /= 16;
	sizeTile.cy /= 16;
	CPoint point( 0, 0);
	
	for( int y = 0; y < sizeTile.cy; y++ )
	{
		point.x = 0;
		for( int x = 0; x < sizeTile.cx; x++)
		{
			///////////////////////////////
			if( x == 0 && y == 0 ) i = 0; else
			if( x < sizeTile.cx - 1 && y == 0 ) i = 1; else
			if( x == sizeTile.cx - 1 && y == 0 ) i = 2; else
			///////////////////////////////
			if( x == 0 && y == 1 ) i = 3; else
			if( x < sizeTile.cx - 1 && y == 1 ) i = 4; else
			if( x == sizeTile.cx - 1 && y == 1 ) i = 5;	else
			///////////////////////////////
			if( x == 0 && y < sizeTile.cy - 1 )	i = 6; else
			if( x < sizeTile.cx - 1 && y < sizeTile.cy - 1 ) i = 7; else
			if( x == sizeTile.cx - 1 && y < sizeTile.cy - 1 ) i = 8; else
			///////////////////////////////
			if( x == 0 && y == sizeTile.cy - 1 ) i = 9; else
			if( x < sizeTile.cx - 1 && y == sizeTile.cy - 1 ) i = 10; else
			if( x == sizeTile.cx - 1 && y == sizeTile.cy - 1 ) i = 11;
			//if( i == 0 )
			//DEBUGOUT( "%d, %d, %d, %d, %d, %x, %d, %d \n",size1.cx, size1.cy, point.x, point.y, i, lpImage[0]->lpData, lpImage[0]->size.cx, lpImage[0]->size.cy );
			PaintTexture( pDest, lpImage[i], point, size1 );
			point.x += 16;
		}
		point.y += 16;
	}
#else
	CWndBase::AdditionalSkinTexture( pDest, size );
#endif

}
BOOL CWndNeuz::Initialize(CWndBase* pWndParent,DWORD dwStyle)
{
	CRect rect(0,0,300,300);
	//m_strMessage = lpszMessage;
	//m_nType = nType;
	//SetTitle("메지시 윈도");
	return CWndBase::Create(dwStyle | WBS_MOVE|/*WBS_MODAL|*/WBS_SOUND|WBS_CAPTION,rect,m_pWndRoot,10);
}
BOOL CWndNeuz::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return TRUE;
}
BOOL CWndNeuz::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == WNM_CLICKED )
	{
		switch(nID)
		{
			case WTBID_HELP: // 핼프 버튼
				{
#ifdef __CLIENT					
					LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( GetWndId() );

					// 헬프 이중생성 방지
					for( int i=0; i<g_vecHelpInsKey.size(); i++ )
					{
						if( g_vecHelpInsKey[i] == lpWndApplet->strToolTip )
							return FALSE;
					}

					CWndHelpInstant* pHelpInstant = new CWndHelpInstant;
					pHelpInstant->m_strHelpKey = lpWndApplet->strToolTip;

					// 헬프 이중생성 방지
					g_vecHelpInsKey.push_back( pHelpInstant->m_strHelpKey );

					pHelpInstant->Initialize();
#endif
				}
				break;
			case WTBID_MIN: // 최소화 
				break;
			case WTBID_MAX: // 최대화 
				if( m_nWinSize == WSIZE_WINDOW )
					SetSizeMax();
				else
					SetSizeWnd();
				break;
			case WTBID_CLOSE: // 종료
				{
#ifdef __CLIENT					
					// 헬프 이중생성 방지
					if( GetWndId() == APP_HELP_INSTANT )
					{
						CString strKey = ((CWndHelpInstant*)this)->m_strHelpKey;
						vector<CString>::iterator where = find(g_vecHelpInsKey.begin(), g_vecHelpInsKey.end(), strKey );
						g_vecHelpInsKey.erase(where);
					}
#endif
					
					Destroy();
				}

				break;
		}
	}


	return TRUE;
}
void CWndNeuz::SetSizeMax()
{
	if( m_bFullMax )
	{
		if( m_nWinSize == WSIZE_WINDOW )
		{
			m_rectBackup = GetWindowRect(TRUE);
			CRect rect;
			CWndBase* pWndFull 
				= m_pWndRoot->FindFullWnd();
			if(pWndFull)
				rect = pWndFull->GetWindowRect(TRUE);
			//rect = pWndFull->GetLayoutRect( TRUE );
			else
				if(IsWndStyle(WBS_CHILD))
					rect = m_pParentWnd->GetClientRect(TRUE);
				else
				{
					//rect = m_pWndRoot->GetWindowRect();
					rect = m_pWndRoot->GetLayoutRect();
					//rect.left = rect.Width() - 200;
				}
				m_wndTitleBar.GetDlgItem( WTBID_MAX )->SetTitle( _T( "#" ) );
				m_bFullWnd = TRUE;
				m_nWinSize = WSIZE_MAX;
				SetWndRect(rect);
		}
	}
	else
	if( m_nWinSize == WSIZE_WINDOW )
	{
		m_rectBackup = GetWindowRect(TRUE);
		CRect rect;
		CWndBase* pWndFull 
			= m_pWndRoot->FindFullWnd();
		if(pWndFull)
			rect = pWndFull->GetWindowRect(TRUE);
		else
			if(IsWndStyle(WBS_CHILD))
				rect = m_pParentWnd->GetClientRect(TRUE);
			else
			{
				//rect = m_pWndRoot->GetWindowRect();
				rect = m_pWndRoot->GetLayoutRect();
				rect.left = rect.Width() - 200;
			}
			SetWndRect(rect);
			m_wndTitleBar.GetDlgItem( WTBID_MAX )->SetTitle( _T( "#" ) );
			AddWndStyle(WBS_MANAGER);
			m_bFullWnd = TRUE;
			m_nWinSize = 2;
	}
}
void CWndNeuz::SetSizeWnd()
{
	if( m_bFullMax )
	{
		if( m_nWinSize != WSIZE_WINDOW )
		{
			m_nWinSize = WSIZE_WINDOW;
			SetWndRect(m_rectBackup);
			m_wndTitleBar.GetDlgItem( WTBID_MAX )->SetTitle( _T( "#" ) );
		}
	}
	else
	{
		if( m_nWinSize != WSIZE_WINDOW )
		{
			m_nWinSize = WSIZE_WINDOW;
			SetWndRect(m_rectBackup);
			m_wndTitleBar.GetDlgItem( WTBID_MAX )->SetTitle( _T( "#" ) );
			DelWndStyle(WBS_MANAGER);
		}
	}
}
void CWndNeuz::OnSize(UINT nType, int cx, int cy)
{
	if(IsWndRoot())
		return;

	CRect rectWnd = GetWndRect();

#ifdef __V050322_CACHEQUEST
	if( m_bTile ) //m_strTexture.IsEmpty() == FALSE )
	{
		CSize size2( rectWnd.Width(), rectWnd.Height() );
		CSize sizeDiv = size2;
		sizeDiv.cx %= 16;
		sizeDiv.cy %= 16;
		size2.cx /= 16; size2.cx *= 16;
		size2.cy /= 16; size2.cy *= 16;
		if( sizeDiv.cx ) size2.cx += 16;
		if( sizeDiv.cy ) size2.cy += 16;
		rectWnd.bottom = rectWnd.top + size2.cy;
		rectWnd.right = rectWnd.left + size2.cx;
		SetWndRect( rectWnd, FALSE );
	}
	AdjustWndBase();
#else
//{{AFX
	if( m_strTile.IsEmpty() == FALSE )
	{
		CSize size2( rectWnd.Width(), rectWnd.Height() );
		CSize sizeDiv = size2;
		sizeDiv.cx %= 16;
		sizeDiv.cy %= 16;
		size2.cx /= 16; size2.cx *= 16;
		size2.cy /= 16; size2.cy *= 16;
		if( sizeDiv.cx ) size2.cx += 16;
		if( sizeDiv.cy ) size2.cy += 16;
		rectWnd.bottom = rectWnd.top + size2.cy;
		rectWnd.right = rectWnd.left + size2.cx;
		SetWndRect( rectWnd, FALSE );
		AdjustWndBase();
	}
//}}AFX
#endif
	m_wndTitleBar.Replace(); 

	LPWNDAPPLET pWndApplet = GetWndApplet();
	if( pWndApplet )
	{
		pWndApplet->size.cx = rectWnd.right;
		pWndApplet->size.cy = rectWnd.bottom;
	}

	CWndBase::OnSize( nType, cx, cy );

//	if( rectOld.Width() != m_rectClient.Width() || rectOld.Height() != m_rectClient.Height() )


	// 차일드 윈도우들의 사이즈를 조절 
	/*
	for(int i = 0; i < m_wndArray.GetSize(); i++)
	{
		CWndBase* pWnd = (CWndBase*)m_wndArray.GetAt(i);
		if(pWnd->IsWndStyle(WBS_DOCKED))// && pWnd->IsWndStyle(WBS_CHILD))
		{
			CRect rect = pWnd->GetWindowRect(TRUE);
			rect.SetRect(0,0,cx,cy);
			pWnd->SetWndRect(rect);
		}
	}
	*/
	/*
	if(IsWndStyle(WBS_DOCKED))// && IsWndStyle(WBS_CHILD))
	{
		m_wndTitleBar.m_wndMinimize.SetVisible(FALSE);
		m_wndTitleBar.m_wndMaximize.SetVisible(FALSE);
	}
	*/
}
void CWndNeuz::OnNonClientLButtonDblClk( UINT nFlags, CPoint point )
{
	if( IsWndStyle( WBS_MAXIMIZEBOX ) )
		OnChildNotify( WNM_CLICKED, WTBID_MAX, NULL );
}
// 윈도 사이즈를 세팅한다.
// 클라이언트 사이즈도 자동으로 조정된다.
void CWndNeuz::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	CRect rectOld = m_rectClient;
	if( m_bFullMax  && m_nWinSize == 2 )
	{
		m_rectWindow = rectWnd;
		m_rectClient = m_rectWindow;
		if( bOnSize && ( rectOld.Width() != m_rectClient.Width() || rectOld.Height() != m_rectClient.Height() ) )
			OnSize(0,m_rectClient.Width(),m_rectClient.Height());
	}
	else
		CWndBase::SetWndRect( rectWnd, bOnSize );
}



//---------------------------------------------------------------------------------------------
// CreateControl ( 컨트롤 생성하기 )
// param	: hWnd, lpWndCtrl
// return	: CWndBase* 
// 스위치 케이스 완전 짜증난다 나중에 Tempalate로 다 바꾸어 버릴꺼임
//---------------------------------------------------------------------------------------------
CWndBase* CWndNeuz::CreateControl( HWND hWnd, LPWNDCTRL lpWndCtrl ) 
{
	DWORD dwWndStyle = lpWndCtrl->dwWndStyle;

	CWndBase* pWndBase( NULL );

	switch( lpWndCtrl->dwWndType )
	{
	case WTYPE_BUTTON:
		{
			pWndBase = new CWndButton;
			FLERROR_ASSERT_LOG_RETURN( pWndBase, NULL, _T("[윈도우 Neuz] 윈도우 버튼 생성 실패.") );	
			
			((CWndButton*)pWndBase)->Create( this, lpWndCtrl );
			
			if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
				((CWndButton*)pWndBase)->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,CWndBase::m_resMng.GetSelectLang(), lpWndCtrl->strTexture ), TRUE );

#ifdef __V050322_CACHEQUEST
			pWndBase->m_bTile = (lpWndCtrl->m_bTile != FALSE);
#endif
			((CWndButton*)pWndBase)->SetGroup( lpWndCtrl->m_bGroup );
		}
		break;

	case WTYPE_TEXT:
		{
			pWndBase = new CWndText;
			((CWndText*)pWndBase)->Create( dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId, lpWndCtrl->m_nFontColorR, lpWndCtrl->m_nFontColorG, lpWndCtrl->m_nFontColorB );
			((CWndText*)pWndBase)->m_string = lpWndCtrl->strTitle;
#ifdef __V050322_CACHEQUEST
			if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
				((CWndStatic*)pWndBase)->m_strTexture = lpWndCtrl->strTexture;
			pWndBase->m_bTile = (lpWndCtrl->m_bTile != FALSE);
#endif
		}
		break;

	case WTYPE_STATIC:
		{
			pWndBase = new CWndStatic;
			FLERROR_ASSERT_LOG_RETURN( pWndBase, NULL, _T("[윈도우 Neuz] 윈도우 스태틱 생성 실패.") );	

			((CWndStatic*)pWndBase)->Create( lpWndCtrl->strTitle, 
											 dwWndStyle | WBS_CAPTION, 
											 lpWndCtrl->rect, 
											 this, 
											 lpWndCtrl->dwWndId , 
											 lpWndCtrl->m_nFontColorR,
											 lpWndCtrl->m_nFontColorG,
											 lpWndCtrl->m_nFontColorB );

			if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
				((CWndStatic*)pWndBase)->m_strTexture = lpWndCtrl->strTexture;

#ifdef __V050322_CACHEQUEST
		pWndBase->m_bTile = (lpWndCtrl->m_bTile != FALSE);
#endif	
		}
		break;
	case WTYPE_EDITCTRL:
		{
			pWndBase = new CWndEdit;
			FLERROR_ASSERT_LOG_RETURN( pWndBase, NULL, _T("[윈도우 Neuz] 윈도우 에디트 컨트롤 생성 실패.") );	

			((CWndEdit*)pWndBase)->Create(	hWnd, 
											dwWndStyle, 
											lpWndCtrl->rect, 
											this, 
											lpWndCtrl->dwWndId, 
											lpWndCtrl->m_nFontColorR, 
											lpWndCtrl->m_nFontColorG, 
											lpWndCtrl->m_nFontColorB
										 );

#ifdef __V050322_CACHEQUEST
			if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
				((CWndStatic*)pWndBase)->m_strTexture = lpWndCtrl->strTexture;

			pWndBase->m_bTile = (lpWndCtrl->m_bTile != FALSE);
#endif
		}
		break;
	case WTYPE_COMBOBOX:
		{
			pWndBase = new CWndComboBox;
			FLERROR_ASSERT_LOG_RETURN( pWndBase, NULL, _T("[윈도우 Neuz] 윈도우 콤보박스 컨트롤 생성 실패.") );	

			((CWndComboBox*)pWndBase)->Create(	dwWndStyle, 
												lpWndCtrl->rect, 
												this, 
												lpWndCtrl->dwWndId, 
												lpWndCtrl->m_nFontColorR,
												lpWndCtrl->m_nFontColorG,
												lpWndCtrl->m_nFontColorB );

#ifdef __V050322_CACHEQUEST																			  
			if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
				((CWndStatic*)pWndBase)->m_strTexture = lpWndCtrl->strTexture;
			
			pWndBase->m_bTile = (lpWndCtrl->m_bTile != FALSE);
#endif		
		}
		break;

	case WTYPE_LISTBOX:
		{
			pWndBase = new CWndListBox;
			FLERROR_ASSERT_LOG_RETURN( pWndBase, NULL, _T("[윈도우 Neuz] 윈도우 리스트 박수 컨트롤 생성 실패.") );	

			((CWndListBox*)pWndBase)->Create(	dwWndStyle, 
												lpWndCtrl->rect, 
												this, 
												lpWndCtrl->dwWndId, 
												lpWndCtrl->m_nFontColorR,
												lpWndCtrl->m_nFontColorG,
												lpWndCtrl->m_nFontColorB );

#ifdef __V050322_CACHEQUEST
		if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
			((CWndStatic*)pWndBase)->m_strTexture = lpWndCtrl->strTexture;
		
		pWndBase->m_bTile = (lpWndCtrl->m_bTile != FALSE);
#endif	
		}
		break;

	case WTYPE_LISTCTRL:
		pWndBase = new CWndListCtrl;
		((CWndListCtrl*)pWndBase)->Create( dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );
#ifdef __V050322_CACHEQUEST
		if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
			((CWndStatic*)pWndBase)->m_strTexture = lpWndCtrl->strTexture;
		pWndBase->m_bTile = (lpWndCtrl->m_bTile != FALSE);
#endif		
		break;
	case WTYPE_TABCTRL:
		pWndBase = new CWndTabCtrl;
		((CWndTabCtrl*)pWndBase)->Create( dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );
		break;

	case WTYPE_TREECTRL:
		{
			pWndBase = new CWndTreeCtrl;
			FLERROR_ASSERT_LOG_RETURN( pWndBase, NULL, _T("[윈도우 Neuz] 윈도우 리스트 박수 컨트롤 생성 실패.") );	

			((CWndTreeCtrl*)pWndBase)->Create(	dwWndStyle, 
												lpWndCtrl->rect, 
												this, 
												lpWndCtrl->dwWndId, 
												lpWndCtrl->m_nFontColorR,
												lpWndCtrl->m_nFontColorG,
												lpWndCtrl->m_nFontColorB );


#ifdef __V050322_CACHEQUEST
		if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
			((CWndStatic*)pWndBase)->m_strTexture = lpWndCtrl->strTexture;

		pWndBase->m_bTile = (lpWndCtrl->m_bTile != FALSE);
#endif		
		}
		break;
	
	case WTYPE_CUSTOM:
		{
			pWndBase = new CWndCustom;
			FLERROR_ASSERT_LOG_RETURN( pWndBase, NULL, _T("[윈도우 Neuz] 윈도우 커스텀 컨트롤 생성 실패.") );	

			((CWndCustom*)pWndBase)->Create(lpWndCtrl->strTitle,
											dwWndStyle, 
											lpWndCtrl->rect, 
											this, 
											lpWndCtrl->dwWndId, 
											lpWndCtrl->m_nFontColorR,
											lpWndCtrl->m_nFontColorG,
											lpWndCtrl->m_nFontColorB );

		}
		break;
	} 

	if( pWndBase )
		pWndBase->m_strToolTip = lpWndCtrl->strToolTip;

	return pWndBase;
}

CWndBase* CWndNeuz::DestroyControl(LPWNDCTRL lpWndCtrl )
{
	DWORD dwWndStyle = lpWndCtrl->dwWndStyle;// & ~WBS_NODRAWFRAME;
	CWndBase* pWndBase = NULL;
	switch( lpWndCtrl->dwWndType )
	{
	case WTYPE_BUTTON:
		pWndBase = new CWndButton;
		((CWndButton*)pWndBase)->Destroy();
		break;
	}
	if( pWndBase )
		pWndBase->m_strToolTip = lpWndCtrl->strToolTip;
	return pWndBase;
}

/*
BOOL CWndNeuz::InitDialog( HWND hWnd, LPWNDAPPLET LPWNDAPPLET )
{
	CRect rect(0,0,300,300);
	SetTitle( LPWNDAPPLET->strTitle );
	BOOL bResult = CWndNeuz::Create( 0 | WBS_MOVE | WBS_SOUND | WBS_THICKFRAME | WBS_CAPTION, rect, NULL, LPWNDAPPLET->dwWndId);
	for( int i = 0; i < LPWNDAPPLET->ptrCtrlArray.GetSize(); i++ )
		CreateControl( hWnd, (LPWNDCTRL)LPWNDAPPLET->ptrCtrlArray.GetAt( i ) );
	return bResult;
}
*/
BOOL CWndNeuz::InitDialog( HWND hWnd, DWORD dwWID, DWORD dwStyle, CPoint ptLeftTop, CWndBase* pWndParent )
{
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( dwWID );
	CRect rect = CRect( ptLeftTop.x, ptLeftTop.y, ptLeftTop.x + lpWndApplet->size.cx, ptLeftTop.y + lpWndApplet->size.cy );
	if( m_ptMouseCenter.x != -1 )
	{
		GET_CLIENT_POINT( hWnd, point );
		rect.OffsetRect( -point );
	}
	//m_strToolTip = lpWndApplet->strToolTip;
	if( lpWndApplet->strToolTip.IsEmpty() )
		return CWndNeuz::Create( lpWndApplet->dwWndStyle | dwStyle, rect, pWndParent, lpWndApplet->dwWndId );
	return CWndNeuz::Create( lpWndApplet->dwWndStyle | dwStyle | WBS_HELP, rect, pWndParent, lpWndApplet->dwWndId );
}  
BOOL CWndNeuz::OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
#ifndef __DAISY
	if( IsWndStyle( WBS_THICKFRAME ) && ( IsFullMax() == FALSE || m_nWinSize != WSIZE_MAX ) )
	{
		if( m_bLButtonDown == FALSE )
		{
			CPoint point = GetMousePoint();
			ClientToScreen( &point );
			CRect rectWindow = GetWindowRect( TRUE );
			point -= rectWindow.TopLeft();
			int nResizeDir = GetResizeDir( point );
			if( nResizeDir )
			{
				// 1 = top
				// 2 = bottom
				// 3 = left
				// 4 = right
				// 5 = topLeft
				// 6 = topRight
				// 7 = bottomLeft
				// 8 = bottomRigh;
				switch( nResizeDir )
				{
				case 1:
				case 2:
					SetMouseCursor( CUR_RESIZE_VERT );
					break;
				case 3:
				case 4:
					SetMouseCursor( CUR_RESIZE_HORI );
					break;
				case 5:
				case 8:
					SetMouseCursor( CUR_RESIZE_HV1 );
					break;
				case 6:
				case 7:
					SetMouseCursor( CUR_RESIZE_HV2 );
					break;
						
				}
			}
			else
				CWndBase::OnSetCursor( pWndBase, nHitTest, message );
		}
	}
	else
		CWndBase::OnSetCursor( pWndBase, nHitTest, message );
#endif
	return TRUE;
}
//enum { IDD = IDD_PROP_APPLET_GENERAL };