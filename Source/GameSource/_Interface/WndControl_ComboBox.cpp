
#include "StdAfx.h"
#include "WndControl_ComboBox.h"


#define COMBOBOX_BUTTONPOS_OFFSET_X		2
#define COMBOBOX_BASIC_LISTBOX_HEIGHT	100


//---------------------------------------------------------------------------------------------
// 생성자 
//---------------------------------------------------------------------------------------------
CWndComboBox::CWndComboBox()
:	m_nComboListBoxMaxLine( 10 )
,	m_bOpen( FALSE )
,	m_bDrawCallFromOutSide( FALSE )
{
	m_byWndType = WTYPE_COMBOBOX;
	ZeroMemory( m_pTextureBackground, sizeof( CTexture* ) * WndComboBox::nBackgroundTextureCount );
}


//---------------------------------------------------------------------------------------------
// 파괴자 
//---------------------------------------------------------------------------------------------
CWndComboBox::~CWndComboBox()
{
}


//---------------------------------------------------------------------------------------------
// 콤보박스 용 버튼 생성
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndComboBox::CreateButton()
{
	CTexture* pTexture = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "ButtQuickListDn.tga" ), 0xFFFF00FF, TRUE );
	FLERROR_ASSERT_LOG_RETURN_VOID( pTexture, _T("[윈도우 콤보박스] 버튼 텍스쳐 로드 실패") );

	CRect rtRect	= GetWindowRect();
	rtRect.top		= ((rtRect.bottom - pTexture->m_size.cy ) / 2) - 1;
	rtRect.left		= rtRect.right - ( pTexture->m_size.cx / 4 ) - COMBOBOX_BUTTONPOS_OFFSET_X;		// 버튼이 4개 붙어있는 이미지라 / 4

	m_wndButton.AddWndStyle( WBS_DOCKING );
	m_wndButton.Create( _T( "" ), WBS_CHILD, rtRect, this, 0 );
	m_wndButton.m_pTexture = pTexture;
}


//---------------------------------------------------------------------------------------------
// 콤보박스 용 리스트 박스 생성
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndComboBox::CreateListBox()
{
	CRect rtRect = GetWindowRect();
	ClientToScreen( &rtRect );
	rtRect.top = rtRect.bottom;
	rtRect.bottom += COMBOBOX_BASIC_LISTBOX_HEIGHT;

	m_wndListBox.CWndBase::Create( WBS_POPUP | WBS_VSCROLL, rtRect, this, 1 );
	m_wndListBox.m_strTitle		= _T( "Combo ListBox" );
	m_wndListBox.m_strTexture	= m_strTexture;
	m_wndListBox.m_bTile		= m_bTile;
	m_wndListBox.AdjustWndBase();
	m_wndListBox.RestoreDeviceObjects();
	m_wndListBox.SetVisible( FALSE );	
}


//---------------------------------------------------------------------------------------------
// 배경 텍스쳐 로딩하기
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndComboBox::LoadBackgroundTexture()
{
	CString strTexture( _T("") );

	for( UINT nIndex = 0; nIndex < WndComboBox::nBackgroundTextureCount; nIndex++ )
	{
		strTexture.Format( _T("WndEditTile%02d.tga"), nIndex );
		
		m_pTextureBackground[nIndex] = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),strTexture ), WNDCOLOR_DEFAULT_KEY, TRUE );
		FLERROR_ASSERT_LOG_DO( m_pTextureBackground[nIndex], continue, _T("[윈도우 콤보박스] 배경 텍스쳐 생성 실패 - [ %s ]."), strTexture );	
		strTexture = _T("");
	}	
}


//---------------------------------------------------------------------------------------------
// 업데이트 전 초기화
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndComboBox::OnInitialUpdate()
{
	CreateButton();
	CreateListBox();
	LoadBackgroundTexture();
}	


//---------------------------------------------------------------------------------------------
// Create ( 생성하기 )
// param	: ...
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CWndComboBox::Create( DWORD dwComboBoxStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID, int nFontColorR, int nFontColorG, int nFontColorB )
{
	return CWndEdit::Create( pParentWnd->m_pApp->GetSafeHwnd(), dwComboBoxStyle | WBS_CHILD, rect, pParentWnd, nID, nFontColorR, nFontColorG, nFontColorB );
}


//---------------------------------------------------------------------------------------------
// 그리기
// param	: 렌더러
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndComboBox::PaintFrame( C2DRender* p2DRender )
{
	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
#ifdef __IMPROVE_MAP_SYSTEM
		m_pTheme->RenderWndEditFrame( p2DRender, &GetWindowRect() );
#else 
		CWndEdit::PaintFrame( p2DRender);
#endif
		return;
	}

	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		CWndEdit::PaintFrame( p2DRender);
}


//---------------------------------------------------------------------------------------------
// 그리기
// param	: 렌더러
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndComboBox::OnDraw( C2DRender* p2DRender )
{
	CWndEdit::OnDraw( p2DRender);
}


//---------------------------------------------------------------------------------------------
// 문자열 추가
// param	: 문자열
// return	: int
//---------------------------------------------------------------------------------------------
int CWndComboBox::AddString( LPCTSTR lpszString )
{
	int nListItemsIndex = m_wndListBox.AddString( lpszString );
	int nListItemsNumber = nListItemsIndex + 1;
	if( nListItemsNumber > m_nComboListBoxMaxLine )
	{
		nListItemsNumber = m_nComboListBoxMaxLine;
	}
	FLASSERT( nListItemsNumber > 0 );

	
	CRect rect = m_wndListBox.GetWindowRect( );
	rect.bottom = rect.top + ( WndListBox::TEXTOUT_OFFSET * 2 ) + ( nListItemsNumber * ( m_wndListBox.GetFontHeight() + m_wndListBox.m_nLineSpace ) );
	m_wndListBox.SetWndRect( rect );

	return nListItemsIndex;
}


//---------------------------------------------------------------------------------------------
// 문자열 삭제
// param	: 인덱스
// return	: int
//---------------------------------------------------------------------------------------------
int CWndComboBox::DeleteString( UINT nIndex )
{
	return m_wndListBox.DeleteString( nIndex );
}


//---------------------------------------------------------------------------------------------
// 윈도우 영역 설정하기
// param	: 윈도우 영역, OnSize 호출여부
// return	: BOOL 
//---------------------------------------------------------------------------------------------
void CWndComboBox::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	CWndEdit::SetWndRect( rectWnd, bOnSize );
}


//---------------------------------------------------------------------------------------------
// 배경 그리기 ( 강제로 계속 그려야 할 때가 있어서... )
// param	: 렌더러
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndComboBox::DrawBackground( C2DRender* p2DRender )
{
	if( FALSE == m_bVisible )
		return ;

	CTexture* pTexture = m_pTextureBackground[0];
	FLERROR_ASSERT_LOG_RETURN_VOID( pTexture, _T("[윈도우 콤보박스] 배경 텍스쳐 포인터가 유효하지 않음.") );	

	CRect rtRect = GetWindowRect( TRUE );
	
	int nWidthCount( rtRect.Width() / pTexture->m_size.cx );
	int nHeightCount( (rtRect.Height() / pTexture->m_size.cy) + 1);
	int nRemainHeight( pTexture->m_size.cy - ( rtRect.Height() % pTexture->m_size.cy ) );

	CPoint ptLeftTop( rtRect.left, rtRect.top );
	for( int nHeight = 0; nHeight < nHeightCount; nHeight++ )
	{
		for( int nWidth = 0; nWidth < nWidthCount; nWidth++ )
		{
			if( nHeight == 0 )							// 맨 위에 줄 그리기
			{
				if( nWidth == 0 )
					m_pTextureBackground[ 0 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
				else if( nWidth == (nWidthCount - 1) )
					m_pTextureBackground[ 2 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
				else
					m_pTextureBackground[ 1 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
			
			}
			else if( nHeight == (nHeightCount - 1) )	// 마지막 줄 그리기
			{
				if( nWidth == 0 )
					m_pTextureBackground[ 6 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
				
				else if( nWidth == (nWidthCount - 1) )
					m_pTextureBackground[ 8 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
				else
					m_pTextureBackground[ 7 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
			}
			else if( nHeight == (nHeightCount - 2) )	// 마지막 바로 위에 줄 그리기	
			{
				// 윈도우 사이즈가 배경 텍스쳐 사이즈로 나누어서 떨어지지 않을 경우 
				// 맨 밑에줄과 그 바로 위에줄을 위로 올려 배경 텍스쳐의 사이즈를 맞춘다
				if( nWidth == 0 )
				{
					p2DRender->RenderTextureUVEx(	ptLeftTop,
													CPoint( m_pTextureBackground[ 6 ]->m_size.cx, m_pTextureBackground[ 6 ]->m_size.cy - nRemainHeight ), 
													m_pTextureBackground[ 6 ],
													0.0f,
													0.0f,
													1.0f,
													1.0f
												);
				}
				else if( nWidth == (nWidthCount - 1) )
				{
					p2DRender->RenderTextureUVEx(	ptLeftTop,
													CPoint( m_pTextureBackground[ 8 ]->m_size.cx, m_pTextureBackground[ 8 ]->m_size.cy - nRemainHeight ), 
													m_pTextureBackground[ 8 ],
													0.0f,
													0.0f,
													1.0f,
													1.0f
												);
				}
				else
				{
					p2DRender->RenderTextureUVEx(	ptLeftTop,
													CPoint( m_pTextureBackground[ 7 ]->m_size.cx, m_pTextureBackground[ 7 ]->m_size.cy - nRemainHeight ), 
													m_pTextureBackground[ 7 ],
													0.0f,
													0.0f,
													1.0f,
													1.0f
												);
				}
			}
			else
			{
				if( nWidth == 0 )
					m_pTextureBackground[ 3 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
				else if( nWidth == (nWidthCount - 1) )
					m_pTextureBackground[ 5 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
				else
					m_pTextureBackground[ 4 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
			}
			ptLeftTop.x += pTexture->m_size.cx;	
		}
		ptLeftTop.x = rtRect.left;
		ptLeftTop.y += pTexture->m_size.cy;

		if( nHeight == (nHeightCount - 2) )
			ptLeftTop.y = ptLeftTop.y - nRemainHeight;
	}
}



void CWndComboBox::ResetContent()
{
#ifdef __IMPROVE_MAP_SYSTEM
	SetString( _T( "" ) );
#endif // __IMPROVE_MAP_SYSTEM
	m_wndListBox.ResetContent();
}
#ifdef __IMPROVE_MAP_SYSTEM
void CWndComboBox::SelectItem( const CString& strItem )
{
	int nIndex = m_wndListBox.GetItemIndex( strItem );
	if( nIndex == -1 )
	{
		return;
	}

	m_wndListBox.SetCurSel( nIndex );
	SetSelectedItemInformation();
}
void CWndComboBox::SelectItem( DWORD dwItem, BOOL bMessage )
{
	int nIndex = m_wndListBox.GetItemIndex( dwItem );
	if( nIndex == -1 )
	{
		return;
	}

	m_wndListBox.SetCurSel( nIndex );
	SetSelectedItemInformation(bMessage);
}
void CWndComboBox::SetSelectedItemInformation( BOOL bMessage )
{
	CString strSelectedItem = _T( "" );
	m_wndListBox.GetText( m_wndListBox.GetCurSel(), strSelectedItem );
	SetString( strSelectedItem );
	CWndBase* pWnd = m_pParentWnd;
	if( pWnd && bMessage )
	{
		pWnd->OnChildNotify( WNM_SELCHANGE, m_nIdWnd, ( LRESULT* )this );
	}
}
#endif // __IMPROVE_MAP_SYSTEM
DWORD CWndComboBox::GetItemData( int nIndex ) const
{
	return m_wndListBox.GetItemData( nIndex );
}
int CWndComboBox::SetItemData( int nIndex, DWORD dwItemData )
{
	return m_wndListBox.SetItemData( nIndex, dwItemData );
}
int CWndComboBox::GetCurSel() const
{
	return m_wndListBox.GetCurSel();
}
int CWndComboBox::SetCurSel( int nSelect )
{
	FLASSERT( /*nSelect >= 0 &&*/ nSelect < m_wndListBox.GetCount() );

	CString string;
	m_wndListBox.GetText( nSelect, string );
	SetString( string );
	return m_wndListBox.SetCurSel( nSelect );
}

void CWndComboBox::OpenListBox( void )
{
	CRect rect = GetScreenRect();
	rect.top = rect.bottom;
	rect.bottom += 200;

	m_wndListBox.Move( rect.TopLeft() );
	m_wndListBox.SetVisible( TRUE );
	m_wndListBox.m_bTile = m_bTile;
	m_wndListBox.AdjustWndBase();
	m_wndListBox.SetFocus();
	m_wndListBox.LoadBackgroundTexture();
}

#ifdef __IMPROVE_MAP_SYSTEM
DWORD CWndComboBox::GetSelectedItemData( void ) const
{
	int nSelectedListNumber = m_wndListBox.GetCurSel();
	if( nSelectedListNumber == -1 )
	{
		return 0;
	}
	return m_wndListBox.GetItemData( nSelectedListNumber );
}
void CWndComboBox::GetListBoxText( int nIndex, CString& strString ) const
{
	m_wndListBox.GetText( nIndex, strString );
}
int CWndComboBox::GetListBoxTextLength( int nIndex ) const
{
	return m_wndListBox.GetTextLen( nIndex );
}
int CWndComboBox::GetListBoxSize( void ) const
{
	return m_wndListBox.GetCount();
}
#endif // __IMPROVE_MAP_SYSTEM

void CWndComboBox::SetComboListBoxMaxLine( int nComboListBoxMaxLine )
{
	m_nComboListBoxMaxLine = nComboListBoxMaxLine;
}

int CWndComboBox::GetComboListBoxMaxLine( void ) const
{
	return m_nComboListBoxMaxLine;
}


void CWndComboBox::OnLButtonDown( UINT nFlags, CPoint point )
{
	if( m_bOpen == TRUE )
		m_bOpen = FALSE;
	else
	{
		m_bOpen = TRUE;
		OpenListBox();
	}
}


BOOL CWndComboBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( nID == 0 && message == WNM_CLICKED) // list
	{
		if( m_bOpen == TRUE )
			m_bOpen = FALSE;
		else
		{
			m_bOpen = TRUE;
			OpenListBox();
		}
	}
	if( nID == 1 && message == WNM_SELCHANGE) // list
	{
		if( m_bOpen == TRUE )
		{
#ifdef __IMPROVE_MAP_SYSTEM
			SetSelectedItemInformation();
#else // __IMPROVE_MAP_SYSTEM
			CString string;
			m_wndListBox.GetText( m_wndListBox.GetCurSel(), string );
			SetString( string );
			CWndBase* pWnd = m_pParentWnd;
			pWnd->OnChildNotify( WNM_SELCHANGE, m_nIdWnd, ( LRESULT* )this );
#endif // __IMPROVE_MAP_SYSTEM
			SetFocus();
			m_wndListBox.SetVisible( FALSE );
			m_bOpen = FALSE;
		}
	}

	if( nID == 1 && message == WNM_KILLFOCUS )
	{
		CPoint point = GetMousePoint();
		CRect rtComboBox = GetClientRect();
		CRect rtButton = m_wndButton.GetClientRect();
		if( rtComboBox.PtInRect( point ) == TRUE || rtButton.PtInRect( point ) == TRUE )
			m_bOpen = TRUE;
		else
		{
			if( m_bOpen == TRUE )
				m_bOpen = FALSE;
		}
	}

	return TRUE;
}
