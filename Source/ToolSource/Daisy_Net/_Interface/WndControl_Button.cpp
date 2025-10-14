
#include "StdAfx.h"
#include "WndControl_Button.h"

//---------------------------------------------------------------------------------------------
// 생성자 
//---------------------------------------------------------------------------------------------
CWndButton::CWndButton()
:	m_pWndExecute( NULL )
,	m_bCheck( FALSE )
,	m_nTimePush( NULL )
,	m_bHighLight( FALSE )
,	m_ptPush( CPoint( 1, 1 ) )
,	m_nFontColor( D3DCOLOR_ARGB( 255,  0,	0,  0 ) )
,	m_nPushColor( D3DCOLOR_ARGB( 255, 255, 255, 100 ) )
,	m_nHighlightColor( D3DCOLOR_ARGB( 255, 64 , 64 , 255 ) )
,	m_nDisableColor( D3DCOLOR_ARGB( 255, 140, 140, 140 ) )
,	m_pWndMenu( NULL )
,	m_bTopDown( FALSE )
,	m_cHotkey( NULL )
{
	m_bGroup	= FALSE;
	m_byWndType	= WTYPE_BUTTON;

	ZeroMemory( &m_shortcut, sizeof( m_shortcut ) );
}


//---------------------------------------------------------------------------------------------
// 파괴자 
//---------------------------------------------------------------------------------------------
CWndButton::~CWndButton()
{
}


//---------------------------------------------------------------------------------------------
// Create ( 생성하기 )
// param	: ...
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CWndButton::Create( LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID, int nFontColorR, int nFontColorG, int nFontColorB )
{
	FLERROR_ASSERT_LOG_RETURN( pParentWnd,	NULL, _T("[윈도우 버튼] 부모 윈도우 포인터 유효하지 않음.") );	

	dwStyle |= WBS_HIGHLIGHT;

	if( NULL == lpszCaption )
	{
		FLERROR_LOG( PROGRAM_NAME, _T("[윈도우 버튼] Caption 문자열 없음") );
		FLASSERT( lpszCaption );
	}	
	
	if( _tcslen(lpszCaption) )
		m_strTitle = lpszCaption;
	
	BOOL bResult = CWndBase::Create( dwStyle | WBS_CHILD | WBS_NOFRAME, rect, pParentWnd, nID);
	FLERROR_ASSERT_LOG_RETURN( bResult, FALSE, _T("[윈도우 버튼] 윈도우 버튼 생성 실패.") );			

	SetFontColor( nFontColorR, nFontColorG, nFontColorB );

	if( IsWndStyle( WBS_RADIO ) )
		SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(),DEF_CTRL_RADIO ), 1 );
	if( IsWndStyle( WBS_CHECK ) )
		SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(),DEF_CTRL_CHECK ), 1 );
	
	return bResult;
}


//---------------------------------------------------------------------------------------------
// Create ( 생성하기 )
// param	: pParentWnd, lpWndCtrl
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CWndButton::Create( CWndBase* pParentWnd, LPWNDCTRL lpWndCtrl )
{
	FLERROR_ASSERT_LOG_RETURN( pParentWnd,	NULL, _T("[윈도우 버튼] 부모 윈도우 포인터 유효하지 않음.") );	
	FLERROR_ASSERT_LOG_RETURN( lpWndCtrl,	NULL, _T("[윈도우 버튼] 컨트롤 윈도우 포인터 유효하지 않음.") );	

	DWORD dwWndStyle = lpWndCtrl->dwWndStyle;
	dwWndStyle |= WBS_HIGHLIGHT;

	m_strTitle = lpWndCtrl->strTitle;
	
	BOOL bResult = CWndBase::Create( dwWndStyle | WBS_CHILD | WBS_NOFRAME, lpWndCtrl->rect, pParentWnd, lpWndCtrl->dwWndId );
	FLERROR_ASSERT_LOG_RETURN( bResult, FALSE, _T("[윈도우 버튼] 윈도우 버튼 생성 실패.") );			
	
	SetFontColor( lpWndCtrl->m_nFontColorR, lpWndCtrl->m_nFontColorG, lpWndCtrl->m_nFontColorB );

	if( IsWndStyle( WBS_RADIO ) )
		SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(),DEF_CTRL_RADIO ), 1 );
	if( IsWndStyle( WBS_CHECK ) )
		SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(),DEF_CTRL_CHECK ), 1 );
	
	return bResult;
}


//---------------------------------------------------------------------------------------------
// Paint Frame ( 그리는 프레임 )
// param	: ...
// return	: BOOL 
//---------------------------------------------------------------------------------------------
void CWndButton::PaintFrame( C2DRender* p2DRender )
{
	if(m_pWndExecute)
	{
		if(m_pWndExecute->OnDrawIcon( this, p2DRender ) )
			return ;
	}

	GET_CLIENT_POINT( m_pApp->GetSafeHwnd(),  point );

	m_bHighLight = FALSE;

	DWORD		dwColor( D3DCOLOR_TEMP( 200, 240, 240, 240 ) );
	int			nFontColor( p2DRender->GetTextColor() );
	int			nFontHeight( GetFontHeight() );
	CString		string;

	if( ( m_dwStyle & WBS_HIGHLIGHT ) &&
		m_GlobalShortcut.m_dwShortcut == SHORTCUT_NONE )
	{
		if( GetClientRect().PtInRect( m_ptMouse ) && m_bEnable == TRUE )
			m_bHighLight = TRUE;
	}

	if(m_dwStyle & WBS_RADIO)
		m_pTheme->RenderWndButtonRadio( p2DRender, this );
	else if(m_dwStyle & WBS_CHECK)
		m_pTheme->RenderWndButtonCheck( p2DRender, this );
	else if(m_dwStyle & WBS_TEXT)
		m_pTheme->RenderWndButtonText( p2DRender, this );
	else if(m_dwStyle & WBS_MENUITEM)
	{
		CPoint pt(0);

		DWORD dwColor1 = ( m_bHighLight == TRUE ?	D3DCOLOR_ARGB( 155, 200, 200, 200 ) : D3DCOLOR_ARGB( 0, 0, 0, 0 ) );
		DWORD dwColor2 = ( m_bHighLight == TRUE ?	D3DCOLOR_ARGB( 155, 200, 200, 200 ) : D3DCOLOR_ARGB( 0, 0, 0, 0 ) );
		DWORD dwColor3 = ( m_bHighLight == TRUE ?	D3DCOLOR_ARGB( 155, 200, 200, 200 ) : D3DCOLOR_ARGB( 0, 0, 0, 0 ) );
		CRect rect = GetClientRect();

		if( m_pWndMenu )
		{
			if( m_bTopDown )
			{
				p2DRender->RenderFillRect( rect, dwColor3, dwColor3, dwColor3, dwColor3 );
				p2DRender->RenderRoundRect( rect, dwColor );

				if( m_bTopDown == 1 )
				{
					p2DRender->RenderLine( 
						CPoint( rect.right - 14, rect.top + 6 ), 
						CPoint( rect.right - 4 , rect.top + 6 ), 
						dwColor
						);
					p2DRender->RenderFillTriangle( 
						CPoint( rect.right - 14, rect.top + 8 ), 
						CPoint( rect.right - 4 , rect.top + 8 ), 
						CPoint( rect.right - 9 , rect.top + 14 ), 
						dwColor
						);
				}
				else
				{
					p2DRender->RenderLine( 
						CPoint( rect.right - 14, rect.top + 13 ), 
						CPoint( rect.right - 4 , rect.top + 13 ), 
						dwColor
						);
					p2DRender->RenderFillTriangle( 
						CPoint( rect.right - 14, rect.top + 12 ), 
						CPoint( rect.right - 3 , rect.top + 12 ), 
						CPoint( rect.right - 9 , rect.top + 6 ), 
						dwColor
						);
				}
			}
			else
			{
				if( m_pWndMenu->IsVisible() )
				{
					dwColor1 = 	D3DCOLOR_TEMP( 155, 200, 200, 200 );
					p2DRender->RenderFillRect( rect, dwColor1, dwColor1, dwColor1, dwColor1 );
					dwColor = m_nPushColor;
				}
				p2DRender->RenderFillTriangle( 
					CPoint( rect.right - 10, rect.top    + 4 ), 
					CPoint( rect.right - 10, rect.bottom - 4 ), 
					CPoint( rect.right - 5, rect.top + rect.Height() / 2 ), 
					0xffa0a0ff //dwColor
					);
				// 매뉴 아이템의 아이콘 
				if( m_pTexture )
				{
					CSize size = m_pTexture->m_size;
					m_pTexture->m_size.cx = 20;
					m_pTexture->m_size.cy = 20;
					m_pTexture->Render( p2DRender, CPoint( pt.x, pt.y     ), m_nAlphaCount );
					m_pTexture->m_size = size;
				}
			}
			p2DRender->TextOut( pt.x + 4 + 20, pt.y + 4, m_strTitle, 0xff000000  );
		}
		else
		{
			if( m_bHighLight )
			{
				p2DRender->RenderFillRect( rect, dwColor2, dwColor2, dwColor2, dwColor2 );
				p2DRender->RenderRoundRect( rect, dwColor );
			}
			// 매뉴 아이템의 아이콘 
			if( m_pTexture )
			{
				CSize size = m_pTexture->m_size;
				m_pTexture->m_size.cx = 20;
				m_pTexture->m_size.cy = 20;
				m_pTexture->Render( p2DRender, CPoint( pt.x, pt.y ), m_nAlphaCount );
				m_pTexture->m_size = size;
			}
			// 체크 
			if( GetCheck() ) 
			{
				p2DRender->RenderLine( CPoint( 2,  2), CPoint( 5, 8), 0xff000000 );
				p2DRender->RenderLine( CPoint( 2,  2), CPoint( 6, 8), 0xff000000 );
				p2DRender->RenderLine( CPoint( 5,  8), CPoint( 8, 3), 0xff000000 );
			}
			p2DRender->TextOut( pt.x + 4 + 20, pt.y + 4, m_strTitle, 0xff000000 );
			if( m_cHotkey )
			{
				CString strHotkey = m_cHotkey;
				p2DRender->TextOut( m_rectClient.right - 32, pt.y + 4, strHotkey, 0xff000000 );
			}
		}
	}
	else
		m_pTheme->RenderWndButton( p2DRender, this );
}



void CWndButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( IsPush() )
	{
///		PLAYSND( m_strSndEffect, NULL );

		if( m_pWndMenu && m_bTopDown )
		{
			CWndMenu* pWndMenu = (CWndMenu*)m_pParentWnd;
			// 공간 넓히기 
			for( int i = 0; i < pWndMenu->m_awndMenuItem.GetSize(); i++ )
			{
				if( pWndMenu->GetMenuItem( i ) == this )
				{
					for(int i2 = 0; i2 < m_pWndMenu->m_awndMenuItem.GetSize(); i2++ )
					{
						if(m_bTopDown == 1)
						{
							CWndButton* pMenuIns = m_pWndMenu->GetMenuItem( i2 );
							pWndMenu->InsertMenu( ++i, 0, pMenuIns->GetWndId(), pMenuIns->GetTitle() );
						}
						else
							pWndMenu->DeleteMenu( i + 1, 0 );
					}
					break;
				}
			}
			if(m_bTopDown == 1)
				m_bTopDown = 2;
			else
				m_bTopDown = 1;
			m_bHighLight = FALSE;
			m_bPush = FALSE;
			return;
		}
		if( m_pWndExecute )
			m_pWndExecute->OnSystemNotify( WM_LBUTTONUP, 0, NULL );

		if( m_dwStyle & WBS_CHECK )
			m_bCheck = !m_bCheck;

		CWndBase* pWnd = m_pParentWnd;
		//if( ! (pWnd->GetStyle() & WBS_CHILD ) )
			pWnd->OnChildNotify( WNM_CLICKED, m_nIdWnd, (LRESULT*)this); 

		if( (m_dwStyle & WBS_RADIO) && m_pParentWnd)
		{
			CPtrArray* pWndArray = m_pParentWnd->GetWndArray();
			// 내 버튼의 인덱스를 페어런트로부터 찾는다.
			for( int i = 0; i < pWndArray->GetSize(); i++)
			{
				CWndButton* pWndBase = (CWndButton*)pWndArray->GetAt(i);
				if( pWndBase == this )
				{
					// 앞쪽으로 그룹 선언을 찾는다.
					for( int i2 = i; i2 >= 0; i2-- )
					{
						pWndBase = (CWndButton*)pWndArray->GetAt(i2);
						if(pWndBase->IsGroup() && pWndBase->IsWndStyle(WBS_RADIO))
						{
							// 라디오 버튼이 아니거나 새로운 그룹이 나타날 때까지 검색 
							for(int i3 = i2; i3 < pWndArray->GetSize(); i3++)
							{
								pWndBase = (CWndButton*)pWndArray->GetAt(i3);
								if(i3 > i2 && pWndBase->IsGroup())
									break;
								if(pWndBase->IsWndStyle(WBS_RADIO))
									((CWndButton*)pWndBase)->SetCheck(0);
								else
									break;
							}
							SetCheck(TRUE);
							return;
						}
					}
				}
			}
		}
	}
}



//---------------------------------------------------------------------------------------------
// On Mouse Left Button Down ( 마우스 왼쪽 버튼 눌렀을 때 ) 
// param	: nFlags, point 
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	PLAYSND( SND_INF_CLICK );
}


//---------------------------------------------------------------------------------------------
// On Mouse Move ( 마우스 이동 할 때 ) 
// param	: nFlags, point 
// return	: void
//---------------------------------------------------------------------------------------------
void CWndButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_dwStyle & WBS_MENUITEM	&& 
		m_pWndMenu == NULL			&&
		IsPush() )
	{
		m_GlobalShortcut.m_dwShortcut	= m_shortcut.m_dwShortcut;
		m_GlobalShortcut.m_dwId			= m_nIdWnd;
		m_GlobalShortcut.m_pTexture		= m_pTexture;

		_tcscpy_s( m_GlobalShortcut.m_szString, _countof(m_GlobalShortcut.m_szString), GetTitle() );
	}
}


void CWndButton::FitTextureSize() 
{
	if( m_pTexture )
	{
		CRect rect = GetWindowRect( TRUE );
		if( IsWndStyle( WBS_RADIO ) | IsWndStyle( WBS_CHECK ) )
			rect.right = rect.left + ( m_pTexture->m_size.cx / 6 );
		else
			rect.right = rect.left + ( m_pTexture->m_size.cx / 4 );
		SetWndRect( rect );
	}
}


BOOL CWndButton::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pParent = (CWndBase*)GetParentWnd();
	return pParent->OnChildNotify( WIN_ITEMDROP, m_nIdWnd, (LRESULT*)pShortcut ); 
}


BOOL CWndButton::Process()
{
	return CWndBase::Process();
}


LRESULT CWndButton::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CWndBase::DefWindowProc(message,wParam,lParam);
}


void CWndButton::SetMenu(CWndMenu* pWndMenu)
{
	m_pWndMenu = pWndMenu;
}


void CWndButton::SetWndExecute(CWndBase* pWndBase) 
{ 
	m_pWndExecute = pWndBase; 
	m_pWndExecute->Initialize(this);
	m_pWndExecute->SetVisible(FALSE);
}


void CWndButton::SetPushTime(int nTime) 
{ 
	m_nTimePush = nTime; 
}


int CWndButton::GetCheck() const 
{
	return m_bCheck;
}


void CWndButton::SetCheck(int nCheck)
{
	m_bCheck = nCheck;
}