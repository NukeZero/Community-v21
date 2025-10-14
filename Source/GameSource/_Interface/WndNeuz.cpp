
#include "stdafx.h"
#include "WndHelp.h"


//---------------------------------------------------------------------------------------------
// 윈도우 타이틀 바 클레스
//---------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------
// 생성자 
//---------------------------------------------------------------------------------------------
CWndTitleBar::CWndTitleBar()
:	m_nButtonMax( NULL )
{
}



//---------------------------------------------------------------------------------------------
// 파괴자
//---------------------------------------------------------------------------------------------
CWndTitleBar::~CWndTitleBar()
{

}


//---------------------------------------------------------------------------------------------
// 윈도우가 초기화 될 때
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndTitleBar::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	TCHAR chPinButton		= NULL;
	TCHAR chMaxButton		= NULL;
	TCHAR chMinButton		= NULL;
	TCHAR chCloseButton		= NULL;
	TCHAR chReportButton	= NULL;
	TCHAR chQuestionButton	= NULL;
	TCHAR chExtentionButton	= NULL;

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		chPinButton			= _T('P');
		chMaxButton			= _T('|');
		chMinButton			= _T('_');
		chCloseButton		= _T('x');
		chReportButton		= _T('V');
		chQuestionButton	= _T('?');
		chExtentionButton	= _T('E');;
	}

	m_nButtonMax = 0;

	if(m_pParentWnd->IsWndStyle( WBS_PIN ) )
		m_awndButton[ WTBID_PIN - 10000 ].Create( &chPinButton, 0, CRect( m_nButtonMax * 16, 1, m_nButtonMax * 16 + 16, 20), this, WTBID_PIN ), m_nButtonMax++;

	if(m_pParentWnd->IsWndStyle( WBS_VIEW ) )
		m_awndButton[ WTBID_REPORT - 10000 ].Create( &chReportButton, 0, CRect( m_nButtonMax * 16, 1, m_nButtonMax * 16 + 16, 20), this, WTBID_REPORT ), m_nButtonMax++;

	if(m_pParentWnd->IsWndStyle( WBS_EXTENSION ) ) 
		m_awndButton[ WTBID_EXTENSION - 10000 ].Create( &chExtentionButton, WBS_CHECK | WBS_PUSHLIKE , CRect( m_nButtonMax * 15, 0, m_nButtonMax * 16 + 16, 20), this, WTBID_EXTENSION ), m_nButtonMax++;

	if(m_pParentWnd->IsWndStyle( WBS_HELP ) )
	{
		if( m_pParentWnd->m_bNoHelpButton == FALSE )
		{
			m_awndButton[ WTBID_HELP - 10000 ].Create( &chQuestionButton, 0, CRect( m_nButtonMax * 16, 1 , m_nButtonMax * 16 + 16, 20), this, WTBID_HELP ), m_nButtonMax++;
			m_awndButton[ WTBID_HELP - 10000 ].SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"ButtWndHelp.tga" ), TRUE );
			m_awndButton[ WTBID_HELP - 10000 ].FitTextureSize();
		}
	}

	if(m_pParentWnd->IsWndStyle( WBS_MINIMIZEBOX ) )
		m_awndButton[ WTBID_MIN - 10000 ].Create( &chMinButton, 0, CRect( m_nButtonMax * 16, 1 , m_nButtonMax* 16 + 16, 20), this, WTBID_MIN ), m_nButtonMax++;

	m_awndButton[ WTBID_MIN - 10000 ].SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "ButtWndMin.tga" ), TRUE );
	m_awndButton[ WTBID_MIN - 10000 ].FitTextureSize();

	if(m_pParentWnd->IsWndStyle( WBS_MAXIMIZEBOX ) )
	{
		m_awndButton[ WTBID_MAX - 10000 ].Create( &chMaxButton, 0, CRect( m_nButtonMax * 16, 1 , m_nButtonMax* 16 + 16, 20), this, WTBID_MAX ), m_nButtonMax++;
		m_awndButton[ WTBID_MAX - 10000 ].SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "ButtWndMax.tga" ), TRUE );
		m_awndButton[ WTBID_MAX - 10000 ].FitTextureSize();
	}

	if(m_pParentWnd->IsWndStyle( WBS_CAPTION ) )
	{
		if( m_pParentWnd->m_bNoCloseButton == FALSE )
		{
			m_awndButton[ WTBID_CLOSE - 10000 ].Create( &chCloseButton, 0, CRect( m_nButtonMax * 16, 1 , m_nButtonMax * 16 + 16, 20), this, WTBID_CLOSE ), m_nButtonMax++;
			m_awndButton[ WTBID_CLOSE - 10000 ].SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"ButtWndExit.tga" ), TRUE );
			m_awndButton[ WTBID_CLOSE - 10000 ].FitTextureSize();
		}
	}
	
	CRect rect( 0, 4, m_nButtonMax * 16, 20 ); 
	SetWndRect( rect );
}


//---------------------------------------------------------------------------------------------
// 변경하기
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndTitleBar::Replace()
{
	if( m_pParentWnd )
	{
		int nCloseButtonWidthOffSetFromTop( NULL );
		int nCloseButtonHeightOffSetFromTop( NULL );
		
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			nCloseButtonWidthOffSetFromTop	= 7;
			nCloseButtonHeightOffSetFromTop	= 4;
		}
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			nCloseButtonWidthOffSetFromTop	= 10;
			nCloseButtonHeightOffSetFromTop = 7;
		}

		Move( CPoint( m_pParentWnd->GetWindowRect().Width() - m_nButtonMax * 16 - nCloseButtonWidthOffSetFromTop, nCloseButtonHeightOffSetFromTop ) ); 
	}
}


//---------------------------------------------------------------------------------------------
// 자식 윈도우에게 알려기
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
BOOL CWndTitleBar::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if(m_pParentWnd)
		m_pParentWnd->OnChildNotify( message, nID, pLResult );
	
	return TRUE;
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
	m_d3dFormat = D3DFMT_A4R4G4B4 ;
}

CWndNeuz::~CWndNeuz()
{
	// PLAYSND( SND_INF_CLOSE );
	for( int i = 0; i < m_wndArrayTemp.GetSize(); i++ )
		SAFE_DELETE_CONST( (CWndBase*)m_wndArrayTemp.GetAt( i ) );
#ifdef __CLIENT
	// 애플렛으로 등록된 윈도만 파괴할 때 윈도의 정보를 저장한다. 
	//if( g_WndMng.GetAppletFunc( GetWndId() ) && m_bPutRegInfo )
	//	g_WndMng.PutRegInfo( this, 1, FALSE );
	//	g_WndMng.PutRegInfo( GetWndId(), GetWindowRect( TRUE ), FALSE );
	// 타일 형태의 윈도는 윈도가 종료할 때 텍스춰를 파괴해야한다.
	if( m_strTexture.IsEmpty() == FALSE )
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
		
	UINT nDialogHalfWidthSize( static_cast<UINT>( ( WndRect.right - WndRect.left ) * 0.5f ) );
	UINT nTitleStringHalfWidthSize( static_cast<UINT>( TitleStringSize.cx * 0.5f ) );

	ResSize.x = nDialogHalfWidthSize - nTitleStringHalfWidthSize;
	ResSize.y = 8;

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
	
			if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
				m_pTheme->RenderWndBaseTitleBar( p2DRender, 10, 4, m_strTitle, m_dwColor );
			if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
				m_pTheme->RenderWndBaseTitleBar( p2DRender, TitleStringPosition.x, TitleStringPosition.y, m_strTitle, m_dwColor );

			p2DRender->SetFont( pOldFont );
		}
		else
		{
			CRect Rect = GetWindowRect();
			Rect.bottom = 21;

			if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
				m_pTheme->RenderWndBaseTitleBar( p2DRender, &Rect, 17, 7, m_strTitle, m_dwColor );
			if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
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
		SetTitle( lpWndApplet->strTitle );
		m_bTile = ( lpWndApplet->bTile != FALSE );
		m_strTexture = lpWndApplet->strTexture;
		m_d3dFormat = lpWndApplet->d3dFormat;

		for( int i = 0; i < lpWndApplet->ptrCtrlArray.GetSize(); i++ )
		{
			m_wndArrayTemp.Add(
				CreateControl( m_pApp->GetSafeHwnd(), (LPWNDCTRL)lpWndApplet->ptrCtrlArray.GetAt( i ) )
			);
		}
	}


//	SetFocus();
	//m_bWndTile = TRUE;
//	AdjustWndBase();
//sun: 8, //__Y_GAMMA_CONTROL_8	
	if( g_xFlyffConfig->GetMainLanguage() != LANG_KOR )
	{
		if(g_Neuz.m_bStartFullscreen)
		{
			g_Neuz.Drv_SetGamma( g_Neuz.GetSafeHwnd(), g_Option.m_fGamma, g_Option.m_nOverBright, g_Option.m_fContrast );
		}
	}
	else
	{
		g_Neuz.Drv_SetGamma( g_Neuz.GetSafeHwnd(), g_Option.m_fGamma, g_Option.m_nOverBright, g_Option.m_fContrast );
	}
}

void CWndNeuz::AdjustWndBase( D3DFORMAT d3dFormat )
{
	if( m_strTexture.IsEmpty() || IsVisible() == FALSE || IsWndStyle( WBS_NODRAWFRAME ) )
	{
		CWndBase::AdjustWndBase( m_d3dFormat );
		return;
	}

	if( m_bTile == FALSE )
	{
		if( m_bLocalizeNoTileIMG )	//_SUN_LOCALIZE_WNDSTATUS
			SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME , g_xFlyffConfig->GetMainLanguage(), m_strTexture ) );
		
		if( !m_pTexture )	
            SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),m_strTexture ), TRUE );

		return;
	}
	CWndBase::AdjustWndBase( m_d3dFormat );
}


void CWndNeuz::AdditionalSkinTexture( LPWORD pDest, CSize size1, D3DFORMAT d3dFormat )
{
	if( m_strTexture.IsEmpty() )
		return;

	int i = 0;
	CRect rect = GetWindowRect( TRUE );
	CSize size2 = CSize( rect.Width(), rect.Height() );

	LPIMAGE lpImage[12];
	CSize sizeBmp[1]; 
	CString strFileName;
	/*
	0 1 2
	3 4 5
	6 7 8
	9 0 1
	*/
//	m_strTile = "WndTile00.bmp";
	int nImgBit = IMGBIT_32;
	for( ; i < 12; i++ )
	{
		CString strTemp1 = m_strTexture.Left( m_strTexture.GetLength() - 6 );
		CString strTemp2 = m_strTexture.Right( 4 );
		strFileName.Format( "%s%02d%s", strTemp1, i, strTemp2 );
		if( m_strWndTileMap.Lookup( strFileName, (void*&)lpImage[i] ) == FALSE )
		{
			lpImage[i] = new IMAGE;
			lpImage[i]->size.cx = 99;
			if( LoadImage( MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),strFileName ), lpImage[i] ) == FALSE )
				FLERROR_LOG( PROGRAM_NAME, _T( "%s Open1 실패" ), strFileName );

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
			PaintTexture( pDest, lpImage[i], point, size1 );
			point.x += 16;
		}
		point.y += 16;
	}

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
					if(g_WndMng.m_pWndHelpInstant)
						SAFE_DELETE(g_WndMng.m_pWndHelpInstant);

					g_WndMng.m_pWndHelpInstant = new CWndHelpInstant;
					g_WndMng.m_pWndHelpInstant->m_strHelpKey = lpWndApplet->strToolTip;
					g_WndMng.m_pWndHelpInstant->Initialize();
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
					Destroy();
				}

				break;
			case IDCANCEL:
#ifdef __BS_CONSOLE
				g_Console.Log("cancel");
#endif // __BS_CONSOLE
				break;
			case IDOK:
#ifdef __BS_CONSOLE
				g_Console.Log("ok");
#endif // __BS_CONSOLE
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

	if( m_bTile ) //m_strTexture.IsEmpty() == FALSE )
	{
		CRect rectWnd = GetWndRect();
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
	m_wndTitleBar.Replace(); 

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
// 아.. 스위치 케이스 짜증나.. 나중에 19차 올라가면 다 템플릿으로 바꾸어 버릴껍니다.
//---------------------------------------------------------------------------------------------
CWndBase* CWndNeuz::CreateControl( HWND hWnd, LPWNDCTRL lpWndCtrl ) 
{
	FLERROR_ASSERT_LOG_RETURN( hWnd,		NULL, _T("[윈도우 Neuz] 윈도우 핸들이 유효하지 않습니다.") );	
	FLERROR_ASSERT_LOG_RETURN( lpWndCtrl,	NULL, _T("[윈도우 Neuz] 윈도우 컨트롤 정보 포인터가 유효하지 않습니다") );	

	DWORD dwWndStyle( lpWndCtrl->dwWndStyle );

	CWndBase* pWndBase = NULL;

	switch( lpWndCtrl->dwWndType )
	{
	case WTYPE_BUTTON:
		{
			pWndBase = new CWndButton;
			FLERROR_ASSERT_LOG_RETURN( pWndBase, NULL, _T("[윈도우 Neuz] 윈도우 버튼 할당 실패.") );	

			CWndButton* pWndButton = static_cast<CWndButton*>( pWndBase );

			BOOL bResult( FALSE );
			if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
				bResult = pWndButton->Create( lpWndCtrl->strTitle, dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );
			if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			{
				bResult = pWndButton->Create(	lpWndCtrl->strTitle,
												dwWndStyle,
												lpWndCtrl->rect,
												this,
												lpWndCtrl->dwWndId,
												lpWndCtrl->m_nFontColorR,
												lpWndCtrl->m_nFontColorG,
												lpWndCtrl->m_nFontColorB
											 );
			}

			if( FALSE == bResult )
			{
				SAFE_DELETE( pWndBase );
				FLERROR_ASSERT_LOG_RETURN( bResult, NULL, _T("[윈도우 Neuz] 윈도우 버튼 생성 실패.") );	
			}
			
			if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
				pWndButton->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),lpWndCtrl->strTexture ), TRUE );

			pWndBase->m_bTile = (lpWndCtrl->bTile != FALSE);
			pWndButton->SetGroup( lpWndCtrl->m_bGroup );
		}
		break;
	
	case WTYPE_TEXT:
		{
			pWndBase = new CWndText;
			FLERROR_ASSERT_LOG_RETURN( pWndBase, NULL, _T("[윈도우 Neuz] 윈도우 텍스트 할당 실패.") );	
	
			CWndText* pWndText = static_cast<CWndText*>( pWndBase );

			BOOL bResult( FALSE );
			if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
				bResult = pWndText->Create( dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );
			if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			{
				bResult = pWndText->Create(	dwWndStyle,
											lpWndCtrl->rect,
											this,
											lpWndCtrl->dwWndId,
											lpWndCtrl->m_nFontColorR,
											lpWndCtrl->m_nFontColorG,
											lpWndCtrl->m_nFontColorB
										  );
			}

			if( FALSE == bResult )
			{
				SAFE_DELETE( pWndBase );
				FLERROR_ASSERT_LOG_RETURN( bResult, NULL, _T("[윈도우 Neuz] 윈도우 텍스트 생성 실패.") );	
			}
			
			pWndText->m_string.SetString( lpWndCtrl->strTitle, pWndText->GetFontColor() ); 

			if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
				pWndText->m_strTexture = lpWndCtrl->strTexture;	

			pWndBase->m_bTile = (lpWndCtrl->bTile != FALSE);
		}
		break;

	case WTYPE_STATIC:
		{
			pWndBase = new CWndStatic;
			FLERROR_ASSERT_LOG_RETURN( pWndBase, NULL, _T("[윈도우 Neuz] 윈도우 스태틱 할당 실패.") );	

			CWndStatic* pWndStatic = static_cast<CWndStatic*>( pWndBase );
			
			if( FALSE == pWndStatic->Create(lpWndCtrl->strTitle, 
											dwWndStyle | WBS_CAPTION, 
											lpWndCtrl->rect, 
											this, 
											lpWndCtrl->dwWndId,
											lpWndCtrl->m_nFontColorR,
											lpWndCtrl->m_nFontColorG,
											lpWndCtrl->m_nFontColorB
											) )
			{
				SAFE_DELETE( pWndBase );
				FLERROR_ASSERT_LOG_RETURN( NULL, NULL, _T("[윈도우 Neuz] 윈도우 스태틱 생성 실패.") );	
			}

			if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
				pWndStatic->m_strTexture = lpWndCtrl->strTexture;

			pWndBase->m_bTile = (lpWndCtrl->bTile != FALSE);
		}
		break;
	case WTYPE_EDITCTRL:
		{
			pWndBase = new CWndEdit;
			FLERROR_ASSERT_LOG_RETURN( pWndBase, NULL, _T("[윈도우 Neuz] 윈도우 에디트 컨트롤 할당 실패.") );	

			CWndEdit* pWndEdit = static_cast<CWndEdit*>( pWndBase );
			
			BOOL bResult( FALSE );
			if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
				bResult = pWndEdit->Create( hWnd, dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );
			if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			{
				bResult = pWndEdit->Create(	hWnd,
											dwWndStyle,
											lpWndCtrl->rect,
											this,
											lpWndCtrl->dwWndId,
											lpWndCtrl->m_nFontColorR,
											lpWndCtrl->m_nFontColorG,
											lpWndCtrl->m_nFontColorB
										  );
			}

			if( FALSE == bResult )
			{
				SAFE_DELETE( pWndBase );
				FLERROR_ASSERT_LOG_RETURN( bResult, NULL, _T("[윈도우 Neuz] 윈도우 버튼 생성 실패.") );	
			}

			if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
				pWndEdit->m_strTexture = lpWndCtrl->strTexture;

			pWndBase->m_bTile = (lpWndCtrl->bTile != FALSE);
		}
		break;

	case WTYPE_COMBOBOX:
		{
			pWndBase = new CWndComboBox;
			FLERROR_ASSERT_LOG_RETURN( pWndBase, NULL, _T("[윈도우 Neuz] 윈도우 에디트 컨트롤 할당 실패.") );	

			CWndComboBox* pWndComboBox = static_cast<CWndComboBox*>( pWndBase );
			
			BOOL bResult( FALSE );
			if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
				bResult = pWndComboBox->Create( dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );
			if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			{
				bResult = pWndComboBox->Create(	dwWndStyle,
												lpWndCtrl->rect,
												this,
												lpWndCtrl->dwWndId,
												lpWndCtrl->m_nFontColorR,
												lpWndCtrl->m_nFontColorG,
												lpWndCtrl->m_nFontColorB
											  );
			}

			if( FALSE == bResult )
			{
				SAFE_DELETE( pWndBase );
				FLERROR_ASSERT_LOG_RETURN( bResult, NULL, _T("[윈도우 Neuz] 윈도우 버튼 생성 실패.") );	
			}
			if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
				pWndComboBox->m_strTexture = lpWndCtrl->strTexture;

			pWndBase->m_bTile = (lpWndCtrl->bTile != FALSE);
		}
		break;

	case WTYPE_LISTBOX:
		{
			pWndBase = new CWndListBox;
			FLERROR_ASSERT_LOG_RETURN( pWndBase, NULL, _T("[윈도우 Neuz] 윈도우 에디트 컨트롤 할당 실패.") );	

			CWndListBox* pWndListBox = static_cast<CWndListBox*>( pWndBase );
			
			BOOL bResult( FALSE );
			if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
				bResult = pWndListBox->Create( dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );
			if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			{
				bResult = pWndListBox->Create(	dwWndStyle,
												lpWndCtrl->rect,
												this,
												lpWndCtrl->dwWndId,
												lpWndCtrl->m_nFontColorR,
												lpWndCtrl->m_nFontColorG,
												lpWndCtrl->m_nFontColorB
											  );
			}

			if( FALSE == bResult )
			{
				SAFE_DELETE( pWndBase );
				FLERROR_ASSERT_LOG_RETURN( bResult, NULL, _T("[윈도우 Neuz] 윈도우 버튼 생성 실패.") );	
			}
			if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
				pWndListBox->m_strTexture = lpWndCtrl->strTexture;

			pWndBase->m_bTile = (lpWndCtrl->bTile != FALSE);
		}		
		break;
	case WTYPE_LISTCTRL:
		pWndBase = new CWndListCtrl;
		((CWndListCtrl*)pWndBase)->Create( dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );
		//((CWndButton*)pWndBase)->Create( _T( "Button" ), 0, CRect( 0, 0, 100, 20), &m_dialogWnd, 10 );
		if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
			((CWndStatic*)pWndBase)->m_strTexture = lpWndCtrl->strTexture;
		pWndBase->m_bTile = (lpWndCtrl->bTile != FALSE);
		break;
	case WTYPE_TABCTRL:
		pWndBase = new CWndTabCtrl;
		((CWndTabCtrl*)pWndBase)->Create( dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );
		//((CWndButton*)pWndBase)->Create( _T( "Button" ), 0, CRect( 0, 0, 100, 20), &m_dialogWnd, 10 );
		break;
	case WTYPE_TREECTRL:
		pWndBase = new CWndTreeCtrl;
		((CWndTreeCtrl*)pWndBase)->Create( dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );
		//((CWndButton*)pWndBase)->Create( _T( "Button" ), 0, CRect( 0, 0, 100, 20), &m_dialogWnd, 10 );
		if( lpWndCtrl->strTexture.IsEmpty() == FALSE )
			((CWndStatic*)pWndBase)->m_strTexture = lpWndCtrl->strTexture;
		pWndBase->m_bTile = (lpWndCtrl->bTile != FALSE);
		break;
	} 
	if( pWndBase )
		pWndBase->m_strToolTip = lpWndCtrl->strToolTip;
	
	return pWndBase;
}


BOOL CWndNeuz::InitDialog( HWND hWnd, DWORD dwWID, DWORD dwStyle, CPoint ptLeftTop, CWndBase* pWndParent )
{
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( dwWID );
	if( NULL ==  lpWndApplet )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "failed initdialog WND ID : %x" ), dwWID );
		return FALSE;
	}

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

	return TRUE;
}
//enum { IDD = IDD_PROP_APPLET_GENERAL };