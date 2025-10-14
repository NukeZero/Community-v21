
#include "stdafx.h"
#include "WndControl_Menu.h"

// 매뉴는 생성시 m_wndArray에 들어가지만 화면 좌표는 전역 좌표다. 
// 따라서 m_rectWindow, m_rectClient는 화면 상에서의 좌표이므로 바로 사용해두 된다.
CWndMenu::CWndMenu()
{
	m_nLargeWidth = 0;
	m_byWndType = WTYPE_MENU;
}
CWndMenu::~CWndMenu()
{
	for( int i = 0; i < m_awndMenuItem.GetSize(); i++)
		delete GetMenuItem( i );
}
BOOL CWndMenu::CreateMenu( CWndBase* pWndParent )
{
	BOOL bResult = CWndBase::Create( WBS_POPUP/*|WBS_NOFRAME*/, CRect(0,0,0,0), pWndParent, 10);
	SetVisible( FALSE );
	return bResult;
}
void CWndMenu::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	CRect rectOld = m_rectClient;
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 3, 3 );
	m_rectLayout = m_rectClient;
	if( bOnSize && ( rectOld.Width() != m_rectClient.Width() || rectOld.Height() != m_rectClient.Height() ) )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
	MakeVertexBuffer();
}
BOOL CWndMenu::CreatePopupMenu()
{
	return TRUE;
}
BOOL CWndMenu::LoadMenu(LPCTSTR lpszResourceName)
{
	return TRUE;
}
BOOL CWndMenu::LoadMenu(UINT nIDResource)
{
	return TRUE;
}
BOOL CWndMenu::LoadMenuIndirect(const void* lpMenuTemplate)
{
	return TRUE;
}
BOOL CWndMenu::DestroyMenu()
{
	return TRUE;
}
void CWndMenu::DeleteAllMenu()
{	
	for( int i = 0; i < m_awndMenuItem.GetSize(); i++)
		delete GetMenuItem( i );
	m_awndMenuItem.RemoveAll();
}

BOOL CWndMenu::DeleteMenu(UINT nPosition, UINT nFlags)
{	
	delete GetMenuItem( nPosition );
	m_awndMenuItem.RemoveAt( nPosition );
	for( int i = nPosition; i < m_awndMenuItem.GetSize(); i++)
	{
		CWndButton* pWndMenuItem = GetMenuItem( i );
		CRect rect = pWndMenuItem->GetWindowRect(TRUE);
		rect.OffsetRect( 0, -22);
		pWndMenuItem->SetWndRect( rect );  
	}
	CRect rect = GetWindowRect(TRUE);
	rect.OffsetRect( 0, 22);
	rect.bottom -= 22;
	SetWndRect( rect );  
	return TRUE;
}
CWndButton* CWndMenu::AppendMenu(UINT nFlags, UINT nIDNewItem,	LPCTSTR lpszNewItem)
{
	CWndButton* pWndButton = new CWndButton;
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
	return pWndButton;
}
UINT CWndMenu::CheckMenuItem(UINT nIDCheckItem, UINT nCheck)
{
	CWndButton* pWndButton = (CWndButton*)m_awndMenuItem.GetAt( nIDCheckItem );
	pWndButton->SetCheck( nCheck );
	return TRUE;
}
UINT CWndMenu::EnableMenuItem(UINT nIDEnableItem, UINT nEnable)
{
	return TRUE;
}
UINT CWndMenu::GetMenuItemCount() const
{
	return TRUE;
}
UINT CWndMenu::GetMenuItemID(int nPos) const
{
	return TRUE;
}
UINT CWndMenu::GetMenuState(UINT nID, UINT nFlags) const
{
	CWndButton* pWndButton = (CWndButton*)m_awndMenuItem.GetAt( nID );
	return pWndButton->GetCheck();//( nCheck );
	//return TRUE;
}
int  CWndMenu::GetMenuString(UINT nIDItem, LPTSTR lpString, int nMaxCount,	UINT nFlags) const
{
	return TRUE;
}
int  CWndMenu::GetMenuString(UINT nIDItem, CString& rString, UINT nFlags) const
{
	return NULL;
}
CWndMenu* CWndMenu::GetSubMenu(int nPos) const
{
	return NULL;
}
BOOL CWndMenu::InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem,	LPCTSTR lpszNewItem)
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
	return TRUE;
}
BOOL CWndMenu::ModifyMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem,	LPCTSTR lpszNewItem)
{
	return TRUE;
}
BOOL CWndMenu::RemoveMenu(UINT nPosition, UINT nFlags)
{
	return TRUE;
}
BOOL CWndMenu::CheckMenuRadioItem(UINT nIDFirst, UINT nIDLast, UINT nIDItem, UINT nFlags)
{
	return TRUE;
}
BOOL CWndMenu::SetDefaultItem(UINT uItem, BOOL fByPos)
{
	return TRUE;
}
UINT CWndMenu::GetDefaultItem(UINT gmdiFlags, BOOL fByPos)
{
	return TRUE;
}
CWndButton* CWndMenu::GetMenuItem( int nPos )
{
	return (CWndButton*) m_awndMenuItem.GetAt( nPos );
}
void CWndMenu::SetVisibleSub( BOOL bVisible )
{
	CWndBase::SetVisible( bVisible );
	for( int i = 0; i < m_awndMenuItem.GetSize(); i++)
	{
		if( GetMenuItem( i )->m_pWndMenu )
			GetMenuItem( i )->m_pWndMenu->SetVisibleSub( bVisible );
	}
}
void CWndMenu::SetVisibleAllMenu( BOOL bVisible )
{
	CWndMenu* pMenu = this;
	CWndMenu* pMenu2 = this;
	while( pMenu2 && pMenu2->IsWndStyle( WBS_POPUP ) )
	{
		pMenu = pMenu2;
		pMenu2 = (CWndMenu*) pMenu->GetParentWnd();
			//pMenu = dynamic_cast<CWndMenu*>(pMenu->GetParentWnd());
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
void CWndMenu::OnKillFocus(CWndBase* pNewWnd)
{
	if( pNewWnd == NULL )
		SetVisibleAllMenu( FALSE );
	else
	if( pNewWnd->IsWndStyle( WBS_POPUP ) == FALSE )
		SetVisibleAllMenu( FALSE );
}

void CWndMenu::OnDraw(C2DRender* p2DRender)
{

/*
//	if( m_bHighLight && ( m_pWndMenu == FALSE || m_pWndMenu->IsVisible( ) == FALSE ) )
	{
		//if( m_pParentWnd->IsWndStyle(WBS_POPUP) )
		{
			// 매뉴의 버튼을 훑어서 켜진거 다 끈다.
		//	CWndMenu* pMenu = (CWndMenu*)m_pParentWnd;
			for( int i = 0; i < m_awndMenuItem.GetSize(); i++)
			{
				if( ((CWndButton*)m_awndMenuItem.GetAt( i ) )->m_pWndMenu )
					((CWndButton*)m_awndMenuItem.GetAt( i ) )->m_pWndMenu->SetVisibleSub( FALSE );
			}
		}
	}
*/
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
	BOOL bResult = FALSE;
	if( m_pParentWnd )
	{
		bResult = m_pParentWnd->OnCommand( nID, dwMessage, this );
		m_pParentWnd->SetFocus();
	}
	return bResult;
}
void CWndMenu::PaintFrame( C2DRender* p2DRender )
{
	p2DRender->RenderFillRect( GetWindowRect(), D3DCOLOR_ARGB( 255, 226,198,181 ) );
	
	//p2DRender->RenderRoundRect( GetWindowRect(), D3DCOLOR_ARGB( 255, 238,163,128 ) );
	//CRect rect = GetWindowRect();
	//m_pTheme->RenderWndMenu( p2DRender, &rect );
//	CWndBase::PaintFrame( p2DRender );
}
BOOL CWndMenu::OnEraseBkgnd( C2DRender* p2DRender )
{
	p2DRender->RenderFillRect( GetClientRect(), D3DCOLOR_ARGB( 255, 255,255,255 ) );
	//CRect rect = GetClientRect();
//	p2DRender->RenderFillRect(rect, D3DCOLOR_ARGB( 255, 255,255,255 ) );
	return TRUE;
}
