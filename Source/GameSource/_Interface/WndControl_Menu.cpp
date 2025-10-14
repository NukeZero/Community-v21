
#include "stdafx.h"
#include "WndControl_Menu.h"
#include "WndControl_Button.h"

#include "GuildHouse.h"

namespace WndMenu
{
	static const BYTE		BUTTON_HEIGHT_SIZE( WndButton::ICONIMAGE_SIZE );					// 버튼 높이
	static const BYTE		BUTTON_BETWEEN_SIZE( 2 );											// 버튼 간의 거리

	static const BYTE		BUTTON_TOP_OFFSET( 10 );											// 버튼 위 오프셋
	static const BYTE		BUTTON_BOTTOM_OFFSET( BUTTON_TOP_OFFSET + BUTTON_BETWEEN_SIZE );	// 버튼 아래 오프셋
	static const BYTE		BUTTON_LEFT_OFFSET( 10 );											// 버튼 왼쪽 오프셋

	static const BYTE		BUTTONWIDTH_ADDSIZE( 10 );											// 버튼 가로 추가 길이
	static const BYTE		WNDWIDTH_ADDSIZE( 8 );												// 버튼 가로 추가 길이
}


using namespace WndMenu;
using namespace WndButton;


//---------------------------------------------------------------------------------------------
// 생성자
// ##	매뉴는 생성시 m_wndAray에 들어가지만 화면 좌표는 전역 좌표다. 
//		따라서 m_rectWindow, m_rectClient는 화면 상에서의 좌표이므로 바로 사용해두 된다.
//---------------------------------------------------------------------------------------------
CWndMenu::CWndMenu()
:	m_nLargeWidth( NULL )
,	m_wButtonWidth( NULL )
{
	m_byWndType = WTYPE_MENU;
	ZeroMemory( m_pTextureBackground, sizeof( CTexture* ) * WndMenu::MAX_BACKGROUND_TEXTRUE_COUNT );
}


//---------------------------------------------------------------------------------------------
// 파괴자
//---------------------------------------------------------------------------------------------
CWndMenu::~CWndMenu()
{
	for( int i = 0; i < m_awndMenuItem.GetSize(); i++)
		SAFE_DELETE_CONST( GetMenuItem( i ) );
}


//---------------------------------------------------------------------------------------------
// 메뉴 생성하기
// param	: 부모 윈도우
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CWndMenu::CreateMenu( CWndBase* pWndParent )
{
	BOOL bResult = CWndBase::Create( WBS_POPUP, CRect( 0, 0, 0, 0 ), pWndParent, 10 );
	SetVisible( FALSE );

	return bResult;
}


//---------------------------------------------------------------------------------------------
// 메뉴 삭제하기
// param	: nPosition, nFlags
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CWndMenu::DeleteMenu(UINT nPosition, UINT nFlags)
{	
	SAFE_DELETE_CONST( GetMenuItem( nPosition ) );

	m_awndMenuItem.RemoveAt( nPosition );

	for( int i = nPosition; i < m_awndMenuItem.GetSize(); i++)
	{
		CWndButton* pWndMenuItem( GetMenuItem( i ) );
		FLERROR_ASSERT_LOG_DO( pWndMenuItem, continue, _T("[윈도우 메뉴] 윈도우 메뉴 포인터가 유효하지 않습니다."))

		CRect rtRect = pWndMenuItem->GetWindowRect( TRUE );
		rtRect.OffsetRect( 0, -ICONIMAGE_SIZE);
		pWndMenuItem->SetWndRect( rtRect );  
	}

	CRect rtRect( GetWindowRect(TRUE) );
	rtRect.OffsetRect( 0, ICONIMAGE_SIZE);
	rtRect.bottom -= ICONIMAGE_SIZE;
	SetWndRect( rtRect );  

	return TRUE;
}


//---------------------------------------------------------------------------------------------
// 모든 메뉴 삭제하기
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndMenu::DeleteAllMenu()
{	
	for( int i = 0; i < m_awndMenuItem.GetSize(); i++)
		SAFE_DELETE_CONST( GetMenuItem( i ) );

	m_awndMenuItem.RemoveAll();

	m_wButtonWidth = NULL;
}


//---------------------------------------------------------------------------------------------
// 서브 메뉴 보이기 설정하기
// param	: 보이기
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndMenu::SetVisibleSub( BOOL bVisible )
{
	CWndBase::SetVisible( bVisible );
	for( int i = 0; i < m_awndMenuItem.GetSize(); i++)
	{
		if( GetMenuItem( i )->m_pWndMenu )
			GetMenuItem( i )->m_pWndMenu->SetVisibleSub( bVisible );
	}
}


//---------------------------------------------------------------------------------------------
// 모든 서브 메뉴 보이기 설정하기
// param	: 보이기
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndMenu::SetVisibleAllMenu( BOOL bVisible )
{
	CWndMenu* pMenu = this;
	CWndMenu* pMenu2 = this;
	while( pMenu2 && pMenu2->IsWndStyle( WBS_POPUP ) )
	{
		pMenu = pMenu2;
		pMenu2 = (CWndMenu*) pMenu->GetParentWnd();
	}
	if( pMenu )
	{
		pMenu->CWndBase::SetVisible( bVisible );
		for( int i = 0; i < pMenu->m_awndMenuItem.GetSize(); i++)
		{
			if( pMenu->GetMenuItem( i )->m_pWndMenu )
				pMenu->GetMenuItem( i )->m_pWndMenu->SetVisibleSub( bVisible );
		}
	}
}


//---------------------------------------------------------------------------------------------
// 메뉴 추가하기
// param	: 보이기
// return	: void 
//---------------------------------------------------------------------------------------------
CWndButton* CWndMenu::AppendMenu(UINT nFlags, UINT nIDNewItem,	LPCTSTR lpszNewItem )
{
	FLERROR_ASSERT_LOG_RETURN( lpszNewItem, NULL, _T("[윈도우 메뉴] 아이템 포인터가 유효하지 않습니다.") );

	CWndButton* pWndButton = new CWndButton;
	FLERROR_ASSERT_LOG_RETURN( pWndButton, NULL, _T("[윈도우 메뉴] 윈도우 버튼 생성 실패") );

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		int nCount = m_awndMenuItem.GetSize();
		CSize size = m_pFont->GetTextExtent( lpszNewItem );
	
		if( size.cx + 60 > m_nLargeWidth )
			m_nLargeWidth = size.cx + 60;

		pWndButton->Create(lpszNewItem, WBS_MENUITEM | WBS_HIGHLIGHT/* | WBS_NOMENUICON*/, CRect( 2, 2 + ( nCount * 22 ), m_nLargeWidth, 2 + ( nCount * 22 ) + 20 ), this, nIDNewItem );
		pWndButton->DelWndStyle( WBS_NODRAWFRAME );
		m_awndMenuItem.Add( pWndButton );

		for( int i = 0; i < m_awndMenuItem.GetSize(); i++ )
		{
			pWndButton = (CWndButton*)m_awndMenuItem.GetAt( i );
			CRect rect = pWndButton->GetWindowRect( TRUE );
			rect.right = rect.left + m_nLargeWidth; 
			pWndButton->SetWndRect( rect );
		}

		SetWndRect( CRect( 0, 0, m_nLargeWidth, 5 + 5 + ( ( nCount + 1 ) * 22 ) ) );
	}
	
	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		CSize	StringSize( m_pFont->GetTextExtent( lpszNewItem ) );
		WORD	wCurrentMenuItemCount( m_awndMenuItem.GetSize() );
		WORD	wButtonWidth( static_cast<WORD>( ICONIMAGE_SIZE + StringSize.cx + HOTKEY_OFFSET + BUTTONWIDTH_ADDSIZE ) );

		// 버튼 가로길이 체크
		if( wButtonWidth > m_wButtonWidth )	
			m_wButtonWidth = wButtonWidth;

		// 다른 버튼에 바뀐 가로길이 적용
		for( int i = 0; i < m_awndMenuItem.GetSize(); i++ )
		{
			CWndButton* pWndButton = (CWndButton*)m_awndMenuItem.GetAt( i );
			FLERROR_ASSERT_LOG_DO( pWndButton, continue, _T("[윈도우 메뉴] 메뉴 버튼 아이템 포인터가 유효하지 않습니다.") );

			CRect rtRect = pWndButton->GetWindowRect( TRUE );
			rtRect.right = rtRect.left + m_wButtonWidth; 
			pWndButton->SetWndRect( rtRect );
		}

		WORD	wTotalButtonHeightSize( wCurrentMenuItemCount * BUTTON_HEIGHT_SIZE );
		WORD	wTotalButtonBetweenSize( ((wCurrentMenuItemCount * 2) + 1) * BUTTON_BETWEEN_SIZE );

		WORD	wTop( BUTTON_TOP_OFFSET + wTotalButtonBetweenSize + wTotalButtonHeightSize);
		WORD	wLeft( BUTTON_LEFT_OFFSET ); 
		WORD	wRight( BUTTON_LEFT_OFFSET + m_wButtonWidth );
		WORD	wBottton( wTop + BUTTON_HEIGHT_SIZE );

		CRect	rtButtonRect( wLeft, wTop, wRight, wBottton );

		// 버튼 생성
		pWndButton->Create(lpszNewItem, WBS_MENUITEM | WBS_HIGHLIGHT, rtButtonRect, this, nIDNewItem );
		pWndButton->DelWndStyle( WBS_NODRAWFRAME );
		m_awndMenuItem.Add( pWndButton );

		// 윈도우 생성
		// 배경 텍스쳐 높이가 윈도우 높이와 나누어 딱 떨어지는지 확인
		WORD	wWndAddSize( NULL );
		WORD	wWndWidth( BUTTON_LEFT_OFFSET + m_wButtonWidth + WNDWIDTH_ADDSIZE );

		CTexture* pTexture = m_pTextureBackground[0];
		if( pTexture )
		{
			WORD wRemainWidth( static_cast<WORD>( wWndWidth % pTexture->m_size.cy ) );

			if( NULL != wRemainWidth )
				wWndAddSize = static_cast<WORD>( pTexture->m_size.cy - wRemainWidth );
		}

		WORD	nWndRight( wWndWidth + wWndAddSize );
		WORD	nWndBottom( BUTTON_TOP_OFFSET + BUTTON_BOTTOM_OFFSET + wTotalButtonBetweenSize + wTotalButtonHeightSize + BUTTON_HEIGHT_SIZE ); 

		SetWndRect( CRect( 0, 0, nWndRight, nWndBottom ) );
	}

	return pWndButton;
}


//---------------------------------------------------------------------------------------------
// 메뉴 삽입하기
// param	: 보이기
// return	: void 
//---------------------------------------------------------------------------------------------
BOOL CWndMenu::InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem,	LPCTSTR lpszNewItem )
{
	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		CWndButton* pWndButton = new CWndButton;
		pWndButton->Create(lpszNewItem, WBS_MENUITEM | WBS_HIGHLIGHT, CRect( 5, 50 + 5 + ( nPosition * 22 ), 175, 50 + 5 + ( nPosition * 22 ) + 20 ), this, nIDNewItem );
		pWndButton->DelWndStyle( WBS_NODRAWFRAME );
		
		m_awndMenuItem.InsertAt( nPosition, pWndButton );
		
		for( int i = nPosition + 1; i < m_awndMenuItem.GetSize(); i++)
		{
			CWndButton* pWndMenuItem = GetMenuItem( i );
			CRect rect = pWndMenuItem->GetWindowRect(TRUE);
			rect.OffsetRect( 0, 22);
			pWndMenuItem->SetWndRect( rect );  
		}
		CRect rect = GetWindowRect(TRUE);
		rect.OffsetRect( 0, -22);
		rect.bottom += 22;
		SetWndRect( rect );  
	}
		
	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		CSize	StringSize( m_pFont->GetTextExtent( lpszNewItem ) );
		WORD	wButtonWidth( static_cast<WORD>( StringSize.cx + HOTKEY_OFFSET + ICONIMAGE_SIZE + BUTTON_LEFT_OFFSET + BUTTONWIDTH_ADDSIZE ) );

		if( wButtonWidth > m_wButtonWidth )
			m_wButtonWidth = wButtonWidth;

		for( int i = 0; i < m_awndMenuItem.GetSize(); i++ )
		{
			CWndButton* pWndButton = (CWndButton*)m_awndMenuItem.GetAt( i );
			FLERROR_ASSERT_LOG_DO( pWndButton, continue, _T("[윈도우 메뉴] 메뉴 버튼 아이템 포인터가 유효하지 않습니다.") );

			CRect rtRect = pWndButton->GetWindowRect( TRUE );
			rtRect.right = rtRect.left + m_wButtonWidth; 
			pWndButton->SetWndRect( rtRect );
		}

		CWndButton* pWndButton = new CWndButton;
		FLERROR_ASSERT_LOG_RETURN( pWndButton, FALSE, _T("[윈도우 메뉴] 윈도우 버튼 포인터가 유효하지 않습니다.") );
		
		CWndButton* pWndMenuItem = GetMenuItem( nPosition );
		FLERROR_ASSERT_LOG_RETURN( pWndMenuItem, FALSE, _T("[윈도우 메뉴] 찾는 메뉴 아이템이 존재하지 않습니다 - 위치 [ %u ]"), nPosition );

		pWndButton->Create(lpszNewItem, WBS_MENUITEM | WBS_HIGHLIGHT, pWndMenuItem->GetClientRect( TRUE ), this, nIDNewItem );
		pWndButton->DelWndStyle( WBS_NODRAWFRAME );

		m_awndMenuItem.InsertAt( nPosition, pWndButton );

		for( int nIndex = nPosition + 1; nIndex < m_awndMenuItem.GetSize(); nIndex++ )
		{
			CWndButton* pWndMenuItem = GetMenuItem( nIndex );
			FLERROR_ASSERT_LOG_DO( pWndButton, continue, _T("[윈도우 메뉴] 메뉴 버튼 아이템 포인터가 유효하지 않습니다.") );
		
			CRect rtRect = pWndMenuItem->GetWindowRect( TRUE );
			rtRect.OffsetRect( 0, BUTTON_HEIGHT_SIZE + ( BUTTON_BETWEEN_SIZE * 2 ) );
			pWndMenuItem->SetWndRect( rtRect );  
		}

		// 윈도우 생성
		WORD	wCurrentMenuItemCount( m_awndMenuItem.GetSize() );
		WORD	wTotalButtonHeightSize( wCurrentMenuItemCount * BUTTON_HEIGHT_SIZE );
		WORD	wTotalButtonBetweenSize( ((wCurrentMenuItemCount * 2) + 1) * BUTTON_BETWEEN_SIZE );

		// 배경 텍스쳐 높이가 윈도우 높이와 나누어 딱 떨어지는지 확인
		WORD	wWndAddSize( NULL );
		WORD	wWndWidth( BUTTON_LEFT_OFFSET + m_wButtonWidth + WNDWIDTH_ADDSIZE  );

		CTexture* pTexture = m_pTextureBackground[0];
		if( pTexture )
		{
			WORD wRemainWidth( static_cast<WORD>( wWndWidth % pTexture->m_size.cy ) );

			if( NULL != wRemainWidth )
				wWndAddSize = static_cast<WORD>( pTexture->m_size.cy - wRemainWidth );
		}


		WORD	nWndRight( wWndWidth + wWndAddSize );
		WORD	nWndBottom( BUTTON_TOP_OFFSET + BUTTON_BOTTOM_OFFSET + wTotalButtonBetweenSize + wTotalButtonHeightSize + BUTTON_HEIGHT_SIZE ); 

		SetWndRect( CRect( 0, 0, nWndRight, nWndBottom ) );
	}

	return TRUE;
}


//---------------------------------------------------------------------------------------------
// 초기화
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndMenu::OnInitialUpdate()
{
	LoadBackgroundTexture();
	CWndBase::OnInitialUpdate();
}


//---------------------------------------------------------------------------------------------
// 배경 텍스쳐 로딩하기
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndMenu::LoadBackgroundTexture()
{
	CString strTexture( _T("") );

	for( UINT nIndex = 0; nIndex < WndMenu::MAX_BACKGROUND_TEXTRUE_COUNT; nIndex++ )
	{
		strTexture.Format( _T("WndTile%02d.tga"), nIndex );
		m_pTextureBackground[nIndex] = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),strTexture ), WNDCOLOR_DEFAULT_KEY, TRUE );
		FLERROR_ASSERT_LOG_DO( m_pTextureBackground[nIndex],	continue, _T("[윈도우 콤보박스] 배경 텍스쳐 생성 실패 - [ %s ]."), strTexture );	

		strTexture = _T("");
	}	
}


//---------------------------------------------------------------------------------------------
// 그리기
// param	: 렌더러
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndMenu::PaintFrame( C2DRender* p2DRender )
{
	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		p2DRender->RenderFillRect( GetWindowRect(), D3DCOLOR_ARGB( 255, 226,198,181 ) );

	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		DrawBackground( p2DRender );
}


//---------------------------------------------------------------------------------------------
// 배경 지울 때
// param	: 렌더러
// return	: void 
//---------------------------------------------------------------------------------------------
BOOL CWndMenu::OnEraseBkgnd( C2DRender* p2DRender )
{
	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		p2DRender->RenderFillRect( GetClientRect(), D3DCOLOR_ARGB( 255, 255, 0, 0 ) );

	return TRUE;
}


//---------------------------------------------------------------------------------------------
// 배경 그리기 
// param	: 렌더러
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndMenu::DrawBackground( C2DRender* p2DRender )
{
	if( FALSE == m_bVisible )
		return ;

	CTexture* pTexture = m_pTextureBackground[0];
	FLERROR_ASSERT_LOG_RETURN_VOID( pTexture, _T("[윈도우 콤보박스] 배경 텍스쳐 포인터가 유효하지 않음.") );	

	CRect rtRect = GetWindowRect();
	
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
					m_pTextureBackground[ 9 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
				
				else if( nWidth == (nWidthCount - 1) )
					m_pTextureBackground[ 11 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
				else
					m_pTextureBackground[ 10 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
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
					m_pTextureBackground[ 6 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
				else if( nWidth == (nWidthCount - 1) )
					m_pTextureBackground[ 8 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
				else
					m_pTextureBackground[ 7 ]->Render( p2DRender, ptLeftTop, CWndBase::m_nAlpha );
			}
			ptLeftTop.x += pTexture->m_size.cx;	
		}
		ptLeftTop.x = rtRect.left;
		ptLeftTop.y += pTexture->m_size.cy;

		if( nHeight == (nHeightCount - 2) )
			ptLeftTop.y = ptLeftTop.y - nRemainHeight;
	}
}


UINT CWndMenu::CheckMenuItem(UINT nIDCheckItem, UINT nCheck)
{
	CWndButton* pWndButton = (CWndButton*)m_awndMenuItem.GetAt( nIDCheckItem );
	pWndButton->SetCheck( nCheck );
	return TRUE;
}


UINT CWndMenu::GetMenuState(UINT nID, UINT nFlags) const
{
	CWndButton* pWndButton = (CWndButton*)m_awndMenuItem.GetAt( nID );
	return pWndButton->GetCheck();//( nCheck );
}

CWndButton* CWndMenu::GetMenuItem( int nPos )
{
	return (CWndButton*) m_awndMenuItem.GetAt( nPos );
}

void CWndMenu::OnKillFocus(CWndBase* pNewWnd)
{
	if( pNewWnd == NULL )
		SetVisibleAllMenu( FALSE );
	else
	if( pNewWnd->IsWndStyle( WBS_POPUP ) == FALSE )
		SetVisibleAllMenu( FALSE );
}

BOOL CWndMenu::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	BOOL bResult = FALSE;
	if( m_pParentWnd )
	{
		bResult = m_pParentWnd->OnChildNotify( message, nID, pLResult );
		m_pParentWnd->SetFocus();
	}

	return bResult;
}
BOOL CWndMenu::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{	
	//gmpbigsun: 길드하우징 콘트롤 오브젝트에 대해 메세지전파를 차단, 새로운방식으로 작동해야 하기때문에...
	if( MMI_GHOUSE_INFO == nID )
	{
		// sgook : 길드가구창을 열어주면 댄다고 함. 2009_11_18
		CWndGuildHousing* pWnd = (CWndGuildHousing*)g_WndMng.GetApplet( APP_GH_FURNITURE_STORAGE );
		if( !pWnd )
		{
			g_WndMng.CreateApplet( APP_GH_FURNITURE_STORAGE ); 
			pWnd = (CWndGuildHousing*)g_WndMng.GetApplet( APP_GH_FURNITURE_STORAGE );
			FLASSERT( pWnd );

			pWnd->InitBySection( CWndGuildHousing::GS_FURNITURE );
		}

	}
	else if( MMI_GHOUSE_REINSTALL == nID )		//재설치
	{
		GuildHouse->Reset( );
		return TRUE;
	}
	else if( MMI_GHOUSE_RECALL == nID )		//회수 
	{
		GuildHouse->Remove( );
		return TRUE;
	}

	BOOL bResult = FALSE;
	if( m_pParentWnd )
	{
		bResult = m_pParentWnd->OnCommand( nID, dwMessage, this );
		if(bResult != -1)
			m_pParentWnd->SetFocus();
	}
	return bResult;
}

void CWndMenu::SetWndRect( CRect rectWnd, BOOL bOnSize )
{	
	//gmpbigsun( 20110322 ) --BEGIN
	//#0012525: [국내_정섭_17차] 상대방(PC)에게 우클릭 했을시 뜨는 UI(거래하기,극단초청 등등..) 가 짤려서 나오는 현상.
	CWndBase* pWndWorld = g_WndMng.GetWndBase( APP_WORLD );
	if( pWndWorld )
	{
		CRect rectWorld = pWndWorld->GetWndRect( );

		if( rectWnd.right > rectWorld.right )	//우측으로 넘어간경우
		{
			CRect rectCorrect = rectWnd;
			rectCorrect.left = rectWnd.left - rectWnd.Width();
			rectCorrect.right = rectCorrect.left + rectWnd.Width();

			rectWnd = rectCorrect;
		}
		else if( rectWnd.left < rectWorld.left )	//좌측으로 넘어간 경우
		{
			CRect rectCorrect = rectWnd;
			rectCorrect.left = rectWnd.left + rectWnd.Width();
			rectCorrect.right = rectCorrect.left + rectWnd.Width();

			rectWnd = rectCorrect;
		}
	}
	// --END

	CRect rectOld = m_rectClient;
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	/// 꼭 확인
	//m_rectClient.DeflateRect( 3, 3 );
	m_rectLayout = m_rectClient;
	if( bOnSize && ( rectOld.Width() != m_rectClient.Width() || rectOld.Height() != m_rectClient.Height() ) )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
	
	MakeVertexBuffer();
}


