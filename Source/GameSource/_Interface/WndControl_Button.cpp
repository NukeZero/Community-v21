
#include "StdAfx.h"
#include "WndControl_Button.h"

using namespace WndButton;

//---------------------------------------------------------------------------------------------
// 생성자 
//---------------------------------------------------------------------------------------------
CWndButton::CWndButton()
:	m_pWndExecute( NULL )
,	m_bCheck( FALSE )
,	m_nTimePush( NULL )
,	m_bHighLight( FALSE )
,	m_ptPush( CPoint( 1, 1 ) )
,	m_nPushFontColor( WNDFONTCOLOR_WHITE )
,	m_nShadowFontColor( WNDFONTCOLOR_WHITE )
,	m_nHighlightFontColor( WNDFONTCOLOR_WHITE )
,	m_nDisableFontColor( WNDFONTCOLOR_WHITE )
,	m_pWndMenu( NULL )
,	m_bTopDown( FALSE )
,	m_cHotkey( NULL )
{
	m_bGroup	= FALSE;
	m_byWndType	= WTYPE_BUTTON;

	ZeroMemory( &m_shortcut, sizeof( m_shortcut ) );

	m_bForcedDrawTitle = FALSE;

	PFONTCOLOR_WNDBUTTON pFontColorWndButton( g_WndFontColorManager->GetWndButton() );

	m_nPushFontColor		= pFontColorWndButton->m_stPush.GetFontColor();
	m_nHighlightFontColor	= pFontColorWndButton->m_stHighLight.GetFontColor();
	m_nDisableFontColor		= pFontColorWndButton->m_stDisable.GetFontColor();
	m_nShadowFontColor		= pFontColorWndButton->m_stShadow.GetFontColor();
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

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		if( IsWndStyle( WBS_RADIO ) )
			SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("ButtRadio.bmp") ), 1 );
		if( IsWndStyle( WBS_CHECK ) )
			SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("ButtCheck.bmp") ), 1 );
	}

	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		if( IsWndStyle( WBS_RADIO ) )
			SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("ButtRadio.tga") ), 1 );
		if( IsWndStyle( WBS_CHECK ) )
			SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("ButtCheck.tga") ), 1 );
	}

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
	
	PFLCOLOR_WNDBUTTON		pColorWndButton = g_FLColorManager->GetWndButton();

	CString					string( _T("") );

	DWORD					dwColor( D3DCOLOR_TEMP( 200, 240, 240, 240 ) );

	m_bHighLight = FALSE;

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


		DWORD 					dwColor1 = ( m_bHighLight == TRUE ?	D3DCOLOR_ARGB( 155, 200, 200, 200 ) : D3DCOLOR_ARGB( 0, 0, 0, 0 ) );
		DWORD 					dwColor3 = ( m_bHighLight == TRUE ?	D3DCOLOR_ARGB( 155, 200, 200, 200 ) : D3DCOLOR_ARGB( 0, 0, 0, 0 ) );
		CRect 					rect( GetClientRect() );
		CPoint					pt(0);

		if( m_pWndMenu )
		{
			if( m_bTopDown )
			{
				p2DRender->RenderFillRect( rect, dwColor3, dwColor3, dwColor3, dwColor3 );
				p2DRender->RenderRoundRect( rect, dwColor );

				p2DRender->RenderLine(	CPoint( rect.right - 14, rect.top + 6 ), 
										CPoint( rect.right - 4 , rect.top + 6 ), 
										dwColor );
				
				p2DRender->RenderFillTriangle(	CPoint( rect.right - 14, rect.top + 8 ), 
												CPoint( rect.right - 4 , rect.top + 8 ), 
												CPoint( rect.right - 9 , rect.top + 14 ), 
												pColorWndButton->m_stTopDownMenuTriangleFill.GetColor() );
			}
			else
			{
				if( m_pWndMenu->IsVisible() )
				{
					dwColor1 = 	D3DCOLOR_TEMP( 155, 200, 200, 200 );
					p2DRender->RenderFillRect( rect, dwColor1, dwColor1, dwColor1, dwColor1 );
					dwColor = m_nPushFontColor;
				}

				p2DRender->RenderFillTriangle(	CPoint( rect.right - 10, rect.top    + 4 ), 
												CPoint( rect.right - 10, rect.bottom - 4 ), 
												CPoint( rect.right - 5, rect.top + rect.Height() / 2 ), 
												pColorWndButton->m_stMenuTriangleFill.GetColor() );

				DrawIcon( p2DRender, pt );
				DrawMenuText( p2DRender, pt );
			}

		}
		else
		{
			DrawHighLight( p2DRender, rect );
			DrawIcon( p2DRender, pt );
			DrawCheckIcon( p2DRender );
			DrawText( p2DRender, pt );
		}
	}
	else
		m_pTheme->RenderWndButton( p2DRender, this );
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
// Draw Text ( 텍스트 그리기 ) 
// param	: p2DRender, point 
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndButton::DrawText( C2DRender* p2DRender, CPoint ptStart )
{
	CD3DFont*				pFont( p2DRender->GetFont() );
	CSize					TitleStringSize( pFont->GetTextExtent( m_strTitle ) );
	PFONTCOLOR_WNDBUTTON	pFontColorWndButton( g_WndFontColorManager->GetWndButton() );

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		p2DRender->TextOut( ptStart.x + STRING_OFFSET + 20, ptStart.y + STRING_OFFSET, m_strTitle, pFontColorWndButton->m_stMainMenuDesc.GetFontColor() );

		if( m_cHotkey )
		{
			CString strHotkey = m_cHotkey;
			p2DRender->TextOut( m_rectClient.right - HOTKEY_OFFSET, ptStart.y + STRING_OFFSET, strHotkey, pFontColorWndButton->m_stMainMenuShortcut.GetFontColor() );
		}
	}

	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		UINT nStartPosY( (UINT)( ptStart.y + ( WndButton::ICONIMAGE_SIZE - TitleStringSize.cy ) * 0.5f ) );
		UINT nStartPosX( ptStart.x + WndButton::ICONIMAGE_SIZE + STRING_OFFSET );

		p2DRender->TextOut( nStartPosX, nStartPosY, m_strTitle, pFontColorWndButton->m_stMainMenuDesc.GetFontColor()  );

		if( m_cHotkey )
		{
			CString strHotkey = m_cHotkey;
			p2DRender->TextOut( m_rectClient.right - HOTKEY_OFFSET, nStartPosY, strHotkey, pFontColorWndButton->m_stMainMenuShortcut.GetFontColor() );
		}
	}
}



//---------------------------------------------------------------------------------------------
// Draw Menu Text ( 메뉴 텍스트 그리기 ) 
// param	: p2DRender, point 
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndButton::DrawMenuText( C2DRender* p2DRender, CPoint ptStart )
{
	CD3DFont*				pFont( p2DRender->GetFont() );
	CSize					TitleStringSize( pFont->GetTextExtent( m_strTitle ) );
	PFONTCOLOR_WNDBUTTON	pFontColorWndButton( g_WndFontColorManager->GetWndButton() );

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		p2DRender->TextOut( ptStart.x + STRING_OFFSET + 20, ptStart.y + STRING_OFFSET, m_strTitle, pFontColorWndButton->m_stMainMenuDesc.GetFontColor() );

	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		UINT nStartPosY( (UINT)( ptStart.y + ( WndButton::ICONIMAGE_SIZE - TitleStringSize.cy ) * 0.5f ) );
		UINT nStartPosX( ptStart.x + WndButton::ICONIMAGE_SIZE + STRING_OFFSET );

		p2DRender->TextOut( nStartPosX, nStartPosY, m_strTitle, pFontColorWndButton->m_stMainMenuDesc.GetFontColor()  );
	}
}


//---------------------------------------------------------------------------------------------
// Draw Icon ( 아이콘 그리기 ) 
// param	: p2DRender, point 
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndButton::DrawIcon( C2DRender* p2DRender, CPoint ptStart )
{
	if( m_pTexture )
	{
		CSize size( m_pTexture->m_size );
		m_pTexture->m_size.cx = WndButton::ICONIMAGE_SIZE;
		m_pTexture->m_size.cy = WndButton::ICONIMAGE_SIZE;
		m_pTexture->Render( p2DRender, CPoint( ptStart.x, ptStart.y ), m_nAlphaCount );
		m_pTexture->m_size = size;
	}
}


//---------------------------------------------------------------------------------------------
// Draw Check Icon ( 체크 아이콘 그리기 ) 
// param	: p2DRender 
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndButton::DrawCheckIcon( C2DRender* p2DRender )
{
	PFLCOLOR_WNDBUTTON pColorWndButton = g_FLColorManager->GetWndButton();

	if( GetCheck() ) 
	{
		p2DRender->RenderLine( CPoint( 2,  2), CPoint( 5, 8), pColorWndButton->m_stMainMenuCheckIcon.GetColor() );
		p2DRender->RenderLine( CPoint( 2,  2), CPoint( 6, 8), pColorWndButton->m_stMainMenuCheckIcon.GetColor() );
		p2DRender->RenderLine( CPoint( 5,  8), CPoint( 8, 3), pColorWndButton->m_stMainMenuCheckIcon.GetColor() );
	}
}


//---------------------------------------------------------------------------------------------
// Draw High Light ( 하이라이트 그리기 ) 
// param	: p2DRender 
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndButton::DrawHighLight( C2DRender* p2DRender, CRect rtRect )
{
	PFLCOLOR_WNDBUTTON pColorWndButton = g_FLColorManager->GetWndButton();
		
	DWORD dwFillColor( pColorWndButton->m_stHighLightFill.GetColor() );
	DWORD dwRoundColor( pColorWndButton->m_stHighLightRound.GetColor() );
	
	if( m_bHighLight )
	{
		p2DRender->RenderFillRect(	rtRect,	dwFillColor, dwFillColor, dwFillColor, dwFillColor );
		p2DRender->RenderRoundRect( rtRect, dwRoundColor );
	}
}


//---------------------------------------------------------------------------------------------
// On Mouse Move ( 마우스 이동 할 때 ) 
// param	: nFlags, point 
// return	: void
//---------------------------------------------------------------------------------------------
void CWndButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if( ( m_dwStyle & WBS_MENUITEM ) && m_pWndMenu == NULL && IsPush() )
	{
		m_GlobalShortcut.m_dwShortcut	= m_shortcut.m_dwShortcut;
		m_GlobalShortcut.m_dwId			= m_nIdWnd;
		m_GlobalShortcut.m_pTexture		= m_pTexture;

		FLStrcpy( m_GlobalShortcut.m_szString, _countof( m_GlobalShortcut.m_szString ), GetTitle() );
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


BOOL CWndButton::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pParent = (CWndBase*)GetParentWnd();
	return pParent->OnChildNotify( WIN_ITEMDROP, m_nIdWnd, (LRESULT*)pShortcut ); 
}


LRESULT CWndButton::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CWndBase::DefWindowProc(message,wParam,lParam);
}


BOOL CWndButton::Process()
{
	return CWndBase::Process();
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
