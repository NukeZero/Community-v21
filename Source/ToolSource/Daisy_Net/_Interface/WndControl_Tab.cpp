
#include "StdAfx.h"
#include "WndControl_Tab.h"

//---------------------------------------------------------------------------------------------
// 생성자 
//---------------------------------------------------------------------------------------------
CWndTabCtrl::CWndTabCtrl()
:	m_nCurSelect( NULL )
,	m_nTabButtonLength( NULL )
,	m_eTabTitleAlign( ALIGN_LEFT )
{
	m_aTab.reserve( nReserveTabControlItem );
}


//---------------------------------------------------------------------------------------------
// 파괴자 
//---------------------------------------------------------------------------------------------
CWndTabCtrl::~CWndTabCtrl()
{
	for( UINT nIndex = 0; nIndex < m_aTab.size(); nIndex++ )
		SAFE_DELETE( m_aTab[nIndex] );
		
	m_aTab.clear();
}


//---------------------------------------------------------------------------------------------
// 생성하기
// param	: 윈도우 스타일, 영역, 부모 윈도우, 윈도우 아이디
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CWndTabCtrl::Create(DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	return CWndBase::Create( dwStyle | WBS_CHILD | WBS_NOFRAME, rect, pParentWnd, nID );
}


//---------------------------------------------------------------------------------------------
// 일반 탭 컨트롤 텍스쳐 로드
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndTabCtrl::LoadTabTexture()
{
	// 탭 텍스쳐 목록
	//"WndTabTile00.bmp"
	//"WndTabTile01.bmp"
	//"WndTabTile02.bmp"
	//"WndTabTile03.bmp"
	//"WndTabTile04.bmp"
	//"WndTabTile05.bmp"
	
	CString strTexture( _T("") );

	for( UINT nIndex = 0; nIndex < nTabTextureCount; nIndex++ )
	{
		strTexture.Format( _T("WndTabTile%02d.bmp"), nIndex );
		BOOL bResult = m_TextureTab[nIndex].LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(),strTexture ), dwTextureKeyColor, TRUE );
		FLERROR_ASSERT_LOG_DO( bResult,	continue, _T("[윈도우 탭 컨트롤] 탭 텍스쳐 생성 실패 - [ %s ]."), strTexture );	

		strTexture = _T("");
	}

	// 선택 탭 텍스쳐 목록
	//"WndTabTile10.bmp"
	//"WndTabTile11.bmp"
	//"WndTabTile12.bmp"
	//"WndTabTile13.bmp"
	//"WndTabTile14.bmp"
	//"WndTabTile15.bmp"


	for( UINT nIndex = 0; nIndex < nTabTextureCount; nIndex++ )
	{
		strTexture.Format( _T("WndTabTile%d.bmp"), nIndex + 10 );
		BOOL bResult = m_TextureTabSelect[nIndex].LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(),strTexture ), dwTextureKeyColor, TRUE );
		FLERROR_ASSERT_LOG_DO( bResult,	continue, _T("[윈도우 탭 컨트롤] 선택 탭 텍스쳐 생성 실패 - [ %s ]."), strTexture );	

		strTexture = _T("");
	}
}


//---------------------------------------------------------------------------------------------
// 인벤토리 탭 컨트롤 텍스쳐 로드
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndTabCtrl::LoadInventoryTabTexture()
{
	// 텍스쳐 목록
	//"WndInventab01.tga"
	//"WndInventab02.tga"
	
	m_aInventoryTexture[0].LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(), "WndInventab02.tga" ), dwTextureKeyColor, TRUE );	
	m_aInventoryTexture[1].LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(), "WndInventab01.tga" ), dwTextureKeyColor, TRUE );	
}


//---------------------------------------------------------------------------------------------
// 업데이트 전 초기화
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndTabCtrl::OnInitialUpdate()
{
	CWndBase* pWndBase = this->GetParentWnd();
	FLERROR_ASSERT_LOG_RETURN_VOID( pWndBase,	_T("[윈도우 탭 컨트롤] 부모 윈도우 포인터가 유효하지 않음.") );	

	LoadTabTexture();

	WTCITEM tabTabItem;
	InsertItem( 0, &tabTabItem );
	InsertItem( 1, &tabTabItem );
	InsertItem( 2, &tabTabItem );
	InsertItem( 3, &tabTabItem );
}


//---------------------------------------------------------------------------------------------
// 아이템 삽입
// param	: nItem, 탭 컨트롤 아이템
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CWndTabCtrl::InsertItem( int nItem, WTCITEM* pTabCtrlItem )
{
	if( nItem >= (int)( m_aTab.size() ) )
		m_aTab.resize( nItem + 1 );

	LPWTCITEM pItem = new WTCITEM;
	FLERROR_ASSERT_LOG_RETURN( pItem, FALSE,	_T("[윈도우 탭 컨트롤] 윈도우 탭 컨트롤 아이템 생성 실패.") );	

	memcpy( pItem, pTabCtrlItem, sizeof( WTCITEM ) );
	m_aTab[ nItem ] = pItem;

	if( pTabCtrlItem->pWndBase )
	{
		if( nItem == m_nCurSelect )
			pTabCtrlItem->pWndBase->SetVisible( TRUE );
		else
			pTabCtrlItem->pWndBase->SetVisible( FALSE );

		CRect rect = GetClientRect( TRUE );
		rect.bottom -= 18;
		rect.OffsetRect( -rect.TopLeft() );

		if( IsWndStyle( WBS_VSCROLL ) )
			rect.right -= 15;

		pItem->pWndBase->SetWndRect( rect );
		pItem->pWndBase->AddWndStyle( WBS_CHILDFRAME );

		if( m_aTab[ m_nCurSelect ] )
			m_aTab[ m_nCurSelect ]->pWndBase->SetFocus();
	}

	m_nTabButtonLength = GetClientRect().Width() / m_aTab.size();
	
	return TRUE;
}


//---------------------------------------------------------------------------------------------
// 탭의 사각영역 구하기
// param	: 렌더러
// return	: BOOL 
//---------------------------------------------------------------------------------------------
CRect CWndTabCtrl::GetWndTabCtrlWindowRect()
{
	CRect rtRect( 0, 0 ,0 ,0 );
	
	rtRect = GetWindowRect();
	rtRect.right -= 15;	// 스크롤 바 때문에 왼쪽으로 민거 같음 ㅡㅡ 이런 젠장...

	return rtRect;
}


//---------------------------------------------------------------------------------------------
// 선택 된 탭 그리기
// param	: 렌더러
// return	: BOOL 
//---------------------------------------------------------------------------------------------
void CWndTabCtrl::DrawTabSelect( C2DRender* p2DRender )
{
	CWndBase* pWndBase = this->GetParentWnd();
	FLERROR_ASSERT_LOG_RETURN_VOID( pWndBase,	_T("[윈도우 탭 컨트롤] 부모 윈도우 포인터가 유효하지 않음.") );	

	int		nParent( pWndBase->GetWndId() );
	int		nTabSize( m_aTab.size() );

	CRect rtTabCtrlRect = GetWndTabCtrlWindowRect();

	CTexture* pTexture = &m_TextureTabSelect[0];
	FLERROR_ASSERT_LOG_RETURN_VOID( pWndBase, _T("[윈도우 탭 컨트롤] 탭 텍스쳐 포인터가 유효하지 않음.") );	

	int nWidth( rtTabCtrlRect.Width() / pTexture->m_size.cx );
	int nHeight( rtTabCtrlRect.Height() / pTexture->m_size.cy );

	int nHeightOffset( rtTabCtrlRect.Height() % pTexture->m_size.cy );

	CRect rtTabRect = rtTabCtrlRect;
	rtTabRect.top += ( (pTexture->m_size.cy * (nHeight - 1)) + nHeightOffset );

	if( m_aTab.empty() )
		return ;
	
	UINT nOneTabBlockCount( nWidth / m_aTab.size() );

	CPoint ptLeftTop( rtTabRect.left, rtTabRect.top );
	for( UINT nCount = 0; nCount < m_aTab.size(); nCount++ )
	{
		if( m_nCurSelect != nCount )
		{
			ptLeftTop.x += ( pTexture->m_size.cx * nOneTabBlockCount );	
			continue;
		}

		for( UINT nBlockCount = 0; nBlockCount < nOneTabBlockCount; nBlockCount++ )
		{
			if( 0 == nBlockCount )								// 왼쪽 끝
				m_TextureTabSelect[ 0 ].Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
			else if( (nOneTabBlockCount - 1) == nBlockCount )	// 오른쪽 끝
				m_TextureTabSelect[ 5 ].Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
			else												// 가온데 텍스쳐
				m_TextureTabSelect[ 1 ].Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );

			ptLeftTop.x += pTexture->m_size.cx ;	
		}
	}
}


//---------------------------------------------------------------------------------------------
// 탭 그리기
// param	: 렌더러
// return	: BOOL 
//---------------------------------------------------------------------------------------------
void CWndTabCtrl::DrawTab( C2DRender* p2DRender )
{
	CWndBase* pWndBase = this->GetParentWnd();
	FLERROR_ASSERT_LOG_RETURN_VOID( pWndBase,	_T("[윈도우 탭 컨트롤] 부모 윈도우 포인터가 유효하지 않음.") );	

	int		nParent( pWndBase->GetWndId() );
	int		nTabSize( m_aTab.size() );


	CRect rtTabCtrlRect = GetWndTabCtrlWindowRect();

	CTexture* pTexture = &m_TextureTab[0];
	FLERROR_ASSERT_LOG_RETURN_VOID( pWndBase, _T("[윈도우 탭 컨트롤] 탭 텍스쳐 포인터가 유효하지 않음.") );	

	int nWidth( rtTabCtrlRect.Width() / pTexture->m_size.cx );
	int nHeight( rtTabCtrlRect.Height() / pTexture->m_size.cy );

	int nHeightOffset( rtTabCtrlRect.Height() % pTexture->m_size.cy );

	CRect rtTabRect = rtTabCtrlRect;
	rtTabRect.top += ( (pTexture->m_size.cy * (nHeight - 1)) + nHeightOffset );

	if( m_aTab.empty() )
		return ;
	
	UINT nOneTabBlockCount( nWidth / m_aTab.size() );

	CPoint ptLeftTop( rtTabRect.left, rtTabRect.top );
	for( UINT nCount = 0; nCount < m_aTab.size(); nCount++ )
	{
		for( UINT nBlockCount = 0; nBlockCount < nOneTabBlockCount; nBlockCount++ )
		{
			if( 0 == nBlockCount )								// 왼쪽 끝
				m_TextureTab[ 0 ].Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
			else if( (nOneTabBlockCount - 1) == nBlockCount )	// 오른쪽 끝
				m_TextureTab[ 5 ].Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
			else												// 가온데 텍스쳐
				m_TextureTab[ 1 ].Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );

			ptLeftTop.x += pTexture->m_size.cx; 
		}
	}
}


//---------------------------------------------------------------------------------------------
// 그리기
// param	: 렌더러
// return	: BOOL 
//---------------------------------------------------------------------------------------------
void CWndTabCtrl::OnDraw( C2DRender* p2DRender )
{
	FLERROR_ASSERT_LOG_RETURN_VOID( p2DRender,	_T("[윈도우 탭 컨트롤] 렌더러 포인터 유효하지 않음.") );	

	DrawTab( p2DRender );
	DrawTabSelect( p2DRender );
}


//---------------------------------------------------------------------------------------------
// 배경 이미지 추가하기
// param	: 윈도우 탭 영역 ...
// return	: void
//---------------------------------------------------------------------------------------------
void CWndTabCtrl::AdditionalSkinTexture_Background( CRect rtRect, LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat )
{
	FLERROR_ASSERT_LOG_RETURN_VOID( pDest,	_T("[윈도우 탭 컨트롤] 대상 포인터 유효하지 않음.") );	

	const UINT			nImageCount = 9;
	LPIMAGE				lpImage[nImageCount] = { NULL, };

	CString				strFileName( _T("") );
	int					nImageWidth( NULL );
	int					nImageHeight( NULL );

	for( UINT nIndex = 0; nIndex < nImageCount; nIndex++ )
	{
		strFileName.Format( "WndEditTile%02d.tga", nIndex );
		if( m_strWndTileMap.Lookup( strFileName, (void*&)lpImage[nIndex] ) == FALSE )
		{
			lpImage[nIndex] = new IMAGE;
			FLERROR_ASSERT_LOG_DO( lpImage[nIndex], continue, _T("[윈도우 탭 컨트롤] 이미지 생성 실패 [ %s ]."), strFileName );	
			
			if( LoadImage( m_pApp->GetDevice(), MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(),strFileName ), lpImage[nIndex] ) == FALSE )
				FLERROR_ASSERT_LOG_DO( FALSE, continue, _T("[윈도우 탭 컨트롤] 이미지 불러오기 실패 [ %s ]."), strFileName );	

			m_strWndTileMap.SetAt( strFileName, lpImage[nIndex] );
		}

		if( NULL == nIndex)	// 모든 이미지의 크기가 같다는 전제
		{
			nImageWidth		= lpImage[nIndex]->size.cx;
			nImageHeight	= lpImage[nIndex]->size.cy;
		}

		strFileName = _T("");
	}


	int nWidth( rtRect.Width() / nImageWidth );
	int nHeight( rtRect.Height() / nImageHeight );

	CPoint point( 0, 0 );
	for( int i = 0; i < nHeight; i++ )
	{
		for( int j = 0; j < nWidth; j++ )
		{
			point = CPoint( j * 8, i * nImageHeight ) + rtRect.TopLeft();
			if( i == 0 )
			{
				if( j == 0 )
					PaintTexture( pDest, lpImage[ 0 ], point, sizeSurface, d3dFormat );
				else
					if( j == nWidth - 1 )
						PaintTexture( pDest, lpImage[ 2 ], point, sizeSurface, d3dFormat );
					else
						PaintTexture( pDest, lpImage[ 1 ], point, sizeSurface, d3dFormat );
			}
			else
			if( i == nHeight - 1 )
			{
				if( j == 0 )
					PaintTexture( pDest, lpImage[ 6 ], point, sizeSurface, d3dFormat );
				else
					if( j == nWidth - 1 )
						PaintTexture( pDest, lpImage[ 8 ], point, sizeSurface, d3dFormat );
					else
						PaintTexture( pDest, lpImage[ 7 ], point, sizeSurface, d3dFormat );					
			}
			else
			{
				if( j == 0 )
					PaintTexture( pDest, lpImage[ 3 ], point, sizeSurface, d3dFormat );
				else
					if( j == nWidth - 1 )
						PaintTexture( pDest, lpImage[ 5 ], point, sizeSurface, d3dFormat );
					else
						PaintTexture( pDest, lpImage[ 4 ], point, sizeSurface, d3dFormat );				
			}
		}
	}
}


//---------------------------------------------------------------------------------------------
// 탭 버튼 이미지 추가하기
// param	: 윈도우 탭 영역 ...
// return	: void
//---------------------------------------------------------------------------------------------
void CWndTabCtrl::AdditionalSkinTexture_TabImage( CRect rtRect, LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat )
{
	FLERROR_ASSERT_LOG_RETURN_VOID( pDest,	_T("[윈도우 탭 컨트롤] 대상 포인터 유효하지 않음.") );	

	const UINT			nTabButtonImageCount = 6;
	LPIMAGE				lpImage[nTabButtonImageCount] = { NULL, };

	CString				strFileName( _T("") );
	int					nImageWidth( NULL );
	int					nImageHeight( NULL );

	for( UINT nIndex = 0; nIndex < nTabButtonImageCount; nIndex++ )
	{
		strFileName.Format( "WndTabTile%02d.bmp", nIndex );
		if( m_strWndTileMap.Lookup( strFileName, (void*&)lpImage[nIndex] ) == FALSE )
		{
			lpImage[nIndex] = new IMAGE;
			FLERROR_ASSERT_LOG_DO( lpImage[nIndex], continue, _T("[윈도우 탭 컨트롤] 탭 버튼 이미지 생성 실패 [ %s ]."), strFileName );	
			
			if( LoadImage( m_pApp->GetDevice(), MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(),strFileName ), lpImage[nIndex] ) == FALSE )
				FLERROR_ASSERT_LOG_DO( FALSE, continue, _T("[윈도우 탭 컨트롤] 탭 버튼 이미지 불러오기 실패 [ %s ]."), strFileName );	

			m_strWndTileMap.SetAt( strFileName, lpImage[nIndex] );
		}

		if( NULL == nIndex)	
		{
			nImageWidth		= lpImage[nIndex]->size.cx;
			nImageHeight	= lpImage[nIndex]->size.cy;
		}

		strFileName = _T("");
	}

	int nWidth = rtRect.Width() / nImageWidth;
	int nHeight = ( rtRect.Height() / nImageHeight );

	CRect rtTabRect = rtRect;
	rtTabRect.top +=  (nImageHeight * (nHeight - 1));

	if( m_aTab.empty() )
		return ;
	
	UINT nOneTabBlockCount( nWidth / m_aTab.size() );

	CPoint ptLeftTop( rtTabRect.left, rtTabRect.top );
	for( UINT nCount = 0; nCount < m_aTab.size(); nCount++ )
	{
		for( UINT nBlockCount = 0; nBlockCount < nOneTabBlockCount; nBlockCount++ )
		{
			if( 0 == nBlockCount )	// 왼쪽 끝
				PaintTexture( pDest, lpImage[ 0 ], ptLeftTop, sizeSurface, d3dFormat );
			else if( (nOneTabBlockCount - 1) == nBlockCount )	// 오른쪽 끝
				PaintTexture( pDest, lpImage[ 5 ], ptLeftTop, sizeSurface, d3dFormat );
			else 
				PaintTexture( pDest, lpImage[ 1 ], ptLeftTop, sizeSurface, d3dFormat );

			ptLeftTop.x += nImageWidth;	
		}
	}
}


//---------------------------------------------------------------------------------------------
// 스킨 텍스쳐 추가
// param	: 윈도우 탭 영역 ...
// return	: void
//---------------------------------------------------------------------------------------------
void CWndTabCtrl::AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat )
{
	CRect rtRect = GetWindowRect( TRUE );
	rtRect += ( m_pParentWnd->GetClientRect( TRUE ).TopLeft() - m_pParentWnd->GetWindowRect( TRUE ).TopLeft() );
	rtRect.right -= 15;	// 스크롤 바 때문에 왼쪽으로 민거 같음 ㅡㅡ 이런 젠장...

	AdditionalSkinTexture_Background( rtRect, pDest, sizeSurface, d3dFormat );
}


void CWndTabCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( m_aTab.size() == 0 ) return;

	CRect rectClient = GetClientRect();
	CRect rect;
	CRect rect2 = rectClient;
	rect2.bottom -= 18;
	int y = rectClient.bottom - 18;

	LPWTCITEM pItem;
	int nSelect = -1;
	if( m_aTab.size() )
	{
		for( int i = 0; i < (int)( m_aTab.size() ); i++ )
		{
			pItem = m_aTab[ i ];
			if( pItem )
			{
				rect.SetRect( i * m_nTabButtonLength, y, m_nTabButtonLength + i * m_nTabButtonLength, rectClient.bottom );
				if( rect.PtInRect( point ) )
					nSelect = i;
			}
		}
	}
	if( nSelect != -1 )
	{
		SetCurSel( nSelect );
		CWndBase* pParent = (CWndBase*)GetParentWnd();
		pParent->OnChildNotify( WNM_SELCHANGE, GetWndId(), (LRESULT*) this ); 
	}
}


BOOL CWndTabCtrl::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	CWndBase* pParent = (CWndBase*)GetParentWnd();
	return pParent->OnChildNotify( message,nID,pLResult); 
}


int CWndTabCtrl::SetCurSel( int nItem )
{
	int nOldSelect = m_nCurSelect;
	LPWTCITEM pItem;
	CRect rect;
	m_nCurSelect = nItem;
	if( m_aTab[ m_nCurSelect ]->pWndBase )
	{
		pItem = m_aTab[ nOldSelect ];
		pItem->pWndBase->SetVisible( FALSE );
		
		if( m_nCurSelect >= (int)( m_aTab.size() ) )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "nItem이 범위를 넘어섬 : nItem(%d) m_aTabSize(%d)" ), m_nCurSelect, m_aTab.size() );
		}

		pItem = m_aTab[ m_nCurSelect ];
		pItem->pWndBase->SetVisible( TRUE );
		rect = GetWindowRect( TRUE );
		if( IsWndStyle( WBS_VSCROLL ) )
			rect.right -= 15;
		rect.bottom -= 18;
		for( int i = 0; i < (int)( m_aTab.size() ); i++ )
		{
			LPWTCITEM pItem = m_aTab[ i ];
			if( pItem && pItem->pWndBase )
			{
				rect.OffsetRect( -rect.TopLeft() );
				pItem->pWndBase->SetWndRect( rect );
			}
		}
		GetFrameWnd()->AdjustWndBase();
		m_aTab[ m_nCurSelect ]->pWndBase->SetFocus();
	}
	else
		m_nCurSelect = nOldSelect;
	return nItem;
}


BOOL CWndTabCtrl::GetItem(int nItem, WTCITEM* pTabCtrlItem) const
{
	if( nItem >= (int)( m_aTab.size() ) )
		return FALSE;
	memcpy( pTabCtrlItem, m_aTab[ nItem ], sizeof( WTCITEM ) );
	return TRUE;
}


void CWndTabCtrl::OnSize(UINT nType, int cx, int cy)
{
	CRect rect = GetClientRect( TRUE );
	rect.bottom -= 18;

	for( int i = 0; i < (int)( m_aTab.size() ); i++ )
	{
		LPWTCITEM pItem = m_aTab[ i ];
		if( pItem && pItem->pWndBase )
		{
			rect.OffsetRect( -rect.TopLeft() );
			pItem->pWndBase->SetWndRect( rect );
		}
	}
	if(  m_aTab.size() )
		m_nTabButtonLength = GetClientRect().Width() / m_aTab.size();
	
	CWndBase::OnSize(nType,cx,cy);
}


LPWTCITEM CWndTabCtrl::GetTabItem( int nItemNumber ) const
{
	if( nItemNumber < 0 || nItemNumber >= (int)( m_aTab.size() ) )
		return NULL;
	return m_aTab[ nItemNumber ];
}


LPWTCITEM CWndTabCtrl::GetSelectedTab( void ) const
{
	return m_aTab[ m_nCurSelect ];
}


void CWndTabCtrl::SetTabTitleAlign( const CWndTabCtrl::TabTitleAlign eTabTitleAlign )
{
	m_eTabTitleAlign = eTabTitleAlign;
}


const CWndTabCtrl::TabTitleAlign CWndTabCtrl::GetTabTitleAlign( void ) const
{
	return m_eTabTitleAlign;
}


int CWndTabCtrl::GetCurSel() const
{
	return m_nCurSelect;
}

void CWndTabCtrl::PaintFrame(C2DRender* p2DRender)
{
	return ;
}