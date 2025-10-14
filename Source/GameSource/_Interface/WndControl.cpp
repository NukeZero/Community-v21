// WndBase.cpp: implementation of the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "clipboard.h"
// 경고!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// 이 클레스는 기본 윈도 베이스 콘토롤이다. 이 콘트롤을 개조하기 위해 매인프레임의 펑션을 부른다거나
// 애플랫 단위에 접근을 시도하는 행위는 엄격히 금지함!!! 이미 몇부분 그러한 행각이 발생했으므로 
// 작성자는 수정하기 바람!!!! 기본 윈도 코딩에서 윈도의 콘트롤 박스나 리스트 박스가 맘에 안든다고
// API 자체를 수정하는 행위는 불가능하다. 윈도에서는 콜백, 훅. 오버라이드 같은 방식으로 기능에 변형을 가한다.
// 여기서는 클레스를 계승받아 오버라이드하여 재작성하는 방법이 적당하다.

#include "WndGuildHouse.h"

#define EDIT_HEIGHT		8

#define GETRANGE ( ( GetClientRect().Height() + m_nLineSpace ) / GetFontHeight() )

CWndStatic::CWndStatic() 
{ 
	m_byWndType = WTYPE_STATIC; 
	m_dwColor = D3DCOLOR_ARGB(255,46,112,169);//원래 기본폰트색
	m_strTexture = DEF_CTRL_GROUP;
	m_bTile = TRUE;
	m_nOffsetX = 0;
	m_nMaxOffset = 0;
	m_nTextSpeed = 1;
}
void CWndStatic::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	//rectWnd.DeflateRect( 2, 2 );
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	
	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}
BOOL CWndStatic::Create( LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID ,int nR,int nG,int nB)	
{
	if( lpszCaption && strlen( lpszCaption ) ) m_strTitle = lpszCaption;
	if( !(dwStyle & WSS_GROUPBOX) )
	{
		m_strTexture = "";//WndEditTile200.tga";
	}

	SetFontColor( nR, nG, nB );
	return CWndBase::Create( dwStyle | WBS_CHILD | WBS_NOFRAME , rect,  pParentWnd, nID );
}
void CWndStatic::OnDraw( C2DRender* p2DRender )
{
	CPoint point( 4, 4 );
	
	if( IsWndStyle( WSS_GROUPBOX ) )
	{
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontText );

		int nFontMaxHeight = CWndBase::m_Theme.m_pFontText->GetMaxHeight();

		if( g_xFlyffConfig->GetMainLanguage() == LANG_FRE || g_xFlyffConfig->GetMainLanguage() == LANG_GER )
			point.y -= 2;
		else
		{
			if(nFontMaxHeight > 14)
				point.y -= nFontMaxHeight - 14;
		}

		if( IsWndStyle( WSS_ALIGNHRIGHT ) )
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			point.x	= ( rect.Width() - sizeTitle.cx - 4 );
			if( point.x < 2 )
				point.x = 2;
		}
		if( IsWndStyle( WSS_ALIGNHCENTER ) )
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			point.x	= (rect.Width() - sizeTitle.cx) / 2;
			if( point.x < 2 )
				point.x = 2;
		}
		if( IsWndStyle( WSS_ALIGNVBOTTOM ) )
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			point.y	= rect.Height() - sizeTitle.cy;
			if( point.y < 2 )
				point.y = 2;
		}
		if( IsWndStyle( WSS_ALIGNVCENTER ) )
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			point.y	= (rect.Height() - sizeTitle.cy + 4) / 2;
			if( point.y < 2 )
				point.y = 2;
		}

		//p2DRender->TextOut( 6, 0, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
		//p2DRender->TextOut( 7, 0, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
		//p2DRender->TextOut( 6, 2, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
		//p2DRender->TextOut( 8, 0, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
		//p2DRender->TextOut( 7, 2, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
		//p2DRender->TextOut( 8, 2, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
		p2DRender->TextOut( 0 + point.x, point.y, m_strTitle, m_dwColor );//m_dwColor );
		p2DRender->TextOut( 1 + point.x, point.y, m_strTitle, m_dwColor );
		p2DRender->SetFont( pOldFont );
			/*
		CRect rect = GetWindowRect();
		rect.DeflateRect( 0, 6, 1, 1 );
//fdef __CLIENT
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
		//p2DRender->TextOut( 9, 0, m_strTitle, 0xff6464ff );
		//p2DRender->TextOut( 9, 2, m_strTitle, 0xff6464ff );
		//p2DRender->TextOut( 11, 0, m_strTitle, 0xff6464ff );
		p2DRender->TextOut( 11, 2, m_strTitle, 0xffffffff );
		p2DRender->TextOut( 10, 1, m_strTitle, 0xffffffff );//m_dwColor );
		p2DRender->SetFont( pOldFont );
//#else		
//		p2DRender->TextOut( 10, 0, m_strTitle, 0xffffffff );
//#endif
		CSize size = p2DRender->m_pFont->GetTextExtent( m_strTitle );
		p2DRender->RenderLine( CPoint( rect.left,  rect.top    ), CPoint( rect.left , rect.bottom ), 0xffffffff );
		p2DRender->RenderLine( CPoint( rect.left,  rect.bottom ), CPoint( rect.right, rect.bottom ), 0xffffffff );
		p2DRender->RenderLine( CPoint( rect.right, rect.top    ), CPoint( rect.right, rect.bottom ), 0xffffffff );
		p2DRender->RenderLine( CPoint( rect.left + size.cx + 15,  rect.top    ), CPoint( rect.right, rect.top    ), 0xffffffff );
		p2DRender->RenderLine( CPoint( rect.left,  rect.top ), CPoint( rect.left + 5, rect.top    ), 0xffffffff );
		//p2DRender->RenderRoundRect( rect, 0xfff0f0f0 );
		*/
	}
	else
	{
		CPoint point( 2, 2 );

		if( IsWndStyle( WSS_ALIGNHRIGHT ) )
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			point.x	= ( rect.Width() - sizeTitle.cx - 4 );
			if( point.x < 2 )
				point.x = 2;
		}
		if( IsWndStyle( WSS_ALIGNHCENTER ) )
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			point.x	= (rect.Width() - sizeTitle.cx) / 2;
			if( point.x < 2 )
				point.x = 2;
		}
		if( IsWndStyle( WSS_ALIGNVBOTTOM ) )
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			point.y	= rect.Height() - sizeTitle.cy;
			if( point.y < 2 )
				point.y = 2;
		}
		if( IsWndStyle( WSS_ALIGNVCENTER ) )
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			point.y	= (rect.Height() - sizeTitle.cy + 4) / 2;
			if( point.y < 2 )
				point.y = 2;
		}

		if( IsWndStyle( WSS_MONEY ) )
		{
			CString str;
			str = GetNumberFormatEx(m_strTitle);

			int nOffset = 0;
			//gmpbigsun( 20110804 ) : 우측정렬일경우 ','때문에 다시 계산
			if( IsWndStyle( WSS_ALIGNHRIGHT ) )
			{
				CSize kOrgSize = p2DRender->m_pFont->GetTextExtent( m_strTitle );
				CSize kNewSize = p2DRender->m_pFont->GetTextExtent( str );
				nOffset = kNewSize.cx - kOrgSize.cx;
				if( nOffset < 0 )
					nOffset = 0;
			}

			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontText );
			p2DRender->TextOut( 0 + point.x - nOffset, point.y, str, m_dwColor );
			p2DRender->TextOut( 1 + point.x - nOffset, point.y, str, m_dwColor );
			p2DRender->SetFont( pOldFont );
		}
		else
		{
			CD3DFont* pOldFont = p2DRender->GetFont();
			CD3DFont* pNewFont = NULL;
			if( m_pFont )
				pNewFont = m_pFont;
			else 
				pNewFont = CWndBase::m_Theme.m_pFontText;

			p2DRender->SetFont( pNewFont );
			//p2DRender->TextOut( 6, 0, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
			//p2DRender->TextOut( 7, 0, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
			//p2DRender->TextOut( 6, 2, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
			//p2DRender->TextOut( 8, 0, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
			//p2DRender->TextOut( 7, 2, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
			//p2DRender->TextOut( 8, 2, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
			//p2DRender->TextOut( 1, 0, m_strTitle, 0xff10106f );//m_dwColor );
			//p2DRender->TextOut( 0, 0, m_strTitle, 0xff7070cf );//m_dwColor );

			//added by gmpbigsun( 20110627 )
			if( IsWndStyle( WSS_TORIGHT_AXISX ) )
			{
				CSize kSize = pNewFont->GetTextExtent( m_strTitle );
				m_nMaxOffset = ( kSize.cx > m_rectWindow.Width() ? kSize.cx : m_rectWindow.Width() );

				//base fixed frame
				m_nOffsetX += m_nTextSpeed;
				if( m_nOffsetX > m_nMaxOffset )
					m_nOffsetX = -m_nMaxOffset;
			}

			if( IsWndStyle( WSS_TOLEFT_AXISX ) )
			{
				CSize kSize = pNewFont->GetTextExtent( m_strTitle );
				m_nMaxOffset = ( kSize.cx > m_rectWindow.Width() ? kSize.cx : m_rectWindow.Width() );

				//base fixed frame
				m_nOffsetX -= m_nTextSpeed;
				if( m_nOffsetX < -m_nMaxOffset)
					m_nOffsetX = m_nMaxOffset;
			}

			p2DRender->TextOut( 0 + point.x + m_nOffsetX, point.y,  m_strTitle, m_dwColor );//m_dwColor );
//			p2DRender->TextOut( 1 + point.x + m_nOffsetX, point.y,  m_strTitle, m_dwColor );//m_dwColor );

			p2DRender->SetFont( pOldFont );
		}
	}
}
BOOL CWndStatic::IsPickupSpace( CPoint point )
{
	if( IsWndStyle( WSS_GROUPBOX ) || IsWndStyle( WSS_PICTURE ) )
	{
		return FALSE;
	}
	CRect rect = GetWindowRect();
	if( rect.PtInRect( point ) )
	{
		if( m_bLButtonDown )
		{
			int a = 0;
		}
		return TRUE;
	}
	return FALSE;
}
void CWndStatic::OnLButtonUp(UINT nFlags, CPoint point)
{
#ifdef __CLIENT
	if(m_pParentWnd)
		m_pParentWnd->SetFocus();
#endif
}
void CWndStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
#ifdef __CLIENT
	if(m_pParentWnd)
		m_pParentWnd->SetFocus();
#endif
}
void CWndStatic::PaintFrame( C2DRender* p2DRender )
{
	if( m_strTexture.IsEmpty() == FALSE || IsWndStyle( WSS_PICTURE ) == FALSE )
		return;
	CRect rect = GetWindowRect();
	p2DRender->RenderRect( rect, 0xff000000 );
}


void CWndCustom::OnDraw( C2DRender* p2DRender )
{
	p2DRender->RenderFillRect( GetWindowRect(), 0xff808080 );
	//return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndListCtrl
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndListCtrl::CWndListCtrl() 
{
	m_nWndColor    = D3DCOLOR_TEMP( 255,  0x5 << 3,  0x5 << 3,  0x5 << 3 );
	m_nFontColor   = D3DCOLOR_ARGB( 255, 64, 64, 64 );
	m_nSelectColor = D3DCOLOR_ARGB( 255, 255, 0, 0   );
	m_nCurSelect = -1;
	m_pFocusItem = NULL;
	m_byWndType  = WTYPE_LISTCTRL;
	m_nLineSpace = 3;
}
CWndListCtrl::~CWndListCtrl()
{
	LVCOLUMN* pColumn;
	LVITEM* pItems;
	for( int i = 0; i < m_aColumns.GetSize(); i++ )
	{
		pColumn = (LVCOLUMN*) m_aColumns.GetAt( i );
		SAFE_DELETE( pColumn->pszText );
		SAFE_DELETE( pColumn );
	}
	for( int i = 0; i < m_aItems.GetSize(); i++ )
	{
		pItems = (LVITEM*)m_aItems.GetAt ( i );
		for( int j = 0; j < m_aColumns.GetSize(); j++ )
		{
			if( pItems[ j ].pszText )
				SAFE_DELETE( pItems[ j ].pszText );
		}
		SAFE_DELETE_ARRAY( pItems );
	}
//	ResetContent();
}
void CWndListCtrl::Create( DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID )
{
	//m_dwListCtrlStyle = dwListCtrlStyle;
	CWndBase::Create( dwListCtrlStyle | WBS_CHILD, rect, pParentWnd, nID );
}
void CWndListCtrl::LoadListBoxScript(LPCTSTR lpFileName) 
{
/*Scanner scanner;
 	if(scanner.Load(lpFileName) == FALSE)
		return;
	scanner.GetToken(); // keyword
	do {
		InterpriteScript(scanner,m_listItemArray);
		scanner.GetToken(); // keyword
	} while(scanner.tok != FINISHED);
	*/
}	
void CWndListCtrl::InterpriteScript(CScanner& scanner,CPtrArray& ptrArray) 
{
	/*
	do {
		LPLISTITEM lpListItem = new LISTITEM;
		lpListItem->m_strWord = scanner.Token;
		ptrArray.Add(lpListItem);
		scanner.GetToken(); 
	} while(*scanner.token != '}' && scanner.tok != FINISHED);
	if(scanner.tok == FINISHED)
		return;
	scanner.GetToken(); 
	*/
}
void CWndListCtrl::OnInitialUpdate()
{
	//CSize size = m_pSprPack->GetAt(13)->GetSize();
	//CRect rect = GetClientRect();
	//rect.left = rect.right - size.cx;
	CRect rect = GetWindowRect();
	//rect.right += 5;
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );//,m_pSprPack,-1);
}


void CWndListCtrl::OnDraw(C2DRender* p2DRender) 
{
	CPoint pt( 3, 3 );
	m_nFontHeight = GetFontHeight();
	//
	// 리포트 출력 
	//
//	if( IsWndStyle( WLVS_REPORT ) )
	{
		pt.y -= (m_nFontHeight + 3) * m_wndScrollBar.GetScrollPos();
		for( int i = 0; i < m_aItems.GetSize(); i++ ) 
		{
			LVITEM* pItems = (LVITEM*)m_aItems.GetAt( i );
			for( int i2 = 0, x = 0; i2 < m_aColumns.GetSize(); i2++ ) 
			{
				if( i == m_nCurSelect )
					p2DRender->TextOut( x + 2, pt.y, pItems[ i2 ].pszText, m_nSelectColor ); 
				else
					p2DRender->TextOut( x + 2, pt.y, pItems[ i2 ].pszText, m_nFontColor ); 
				LVCOLUMN* pColumn = (LVCOLUMN*)m_aColumns.GetAt( i2 );
				x += pColumn->cx + 7;
			}
			pt.y += m_nFontHeight + 3;
		}
		// 스크롤바 관련 
		int nPage = GETRANGE;//GetClientRect().Height() / (m_nFontHeight + 3);
		int nRange = m_aItems.GetSize();// - nPage;

		if(	IsWndStyle( WBS_VSCROLL ) )  
		{
			m_wndScrollBar.SetVisible( TRUE );
			m_wndScrollBar.SetScrollRange( 0, nRange );
			m_wndScrollBar.SetScrollPage( nPage );
		}
		else
			m_wndScrollBar.SetVisible( FALSE );
	}
	//
	// 아이콘 출력
	// CCtrllist
	if( IsWndStyle( WLVS_ICON ) )
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 32;
		int nHeight = rect.Height() / 32;
		pt.y = 0;
		pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
		for( int i = pt.y; i < m_aItems.GetSize(); i++ ) 
		{
			int x = ( i - pt.y ) % nWidth;
			int y = ( i - pt.y ) / nWidth;
			LVITEM* pItems = (LVITEM*)m_aItems.GetAt( i );
			if( i == m_nCurSelect )
				p2DRender->RenderFillRect( CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 ), 0xff707070 );
			else
				p2DRender->RenderFillRect( CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 ), 0xffffffff );
			//p2DRender->TextOut( x * 32, y * 32, pItems->pszText, 0xff000000 );
		}
		// 스크롤바 관련 
		int nPage = nHeight;
		int nRange = m_aItems.GetSize() / nWidth;// - nPage;
		if(nRange - nPage <= 0)
			m_wndScrollBar.SetVisible(FALSE);
		else
		{
			m_wndScrollBar.SetVisible( TRUE );
			m_wndScrollBar.SetScrollRange( 0, nRange );
			m_wndScrollBar.SetScrollPage( nPage );
		}
	}
}
int CWndListCtrl::GetCurSel() const
{
	return m_nCurSelect;
}
int CWndListCtrl::SetCurSel( int nSelect )
{
	m_pFocusItem = (LVITEM*)m_aItems.GetAt( nSelect );
	m_nCurSelect = nSelect;
	return m_nCurSelect;
}
CString CWndListCtrl::GetItemText(int nItem, int nSubItem) const
{
	LVITEM* pItem = (LVITEM*)m_aItems.GetAt( nItem );
	return pItem[ nSubItem ].pszText;
}
void CWndListCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
	CPoint pt( 3, 3 );
	CRect rect;
	//
	// 리포트 출력 
	//
	//if( IsWndStyle( WLVS_REPORT ) )
	{
		pt.y -= (m_nFontHeight + 3) * m_wndScrollBar.GetScrollPos();
		for( int i = 0; i < m_aItems.GetSize(); i++ ) 
		{
			LVITEM* pItems = (LVITEM*)m_aItems.GetAt( i );
			rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
			if( rect.PtInRect( point ) )
			{
				if(m_pFocusItem == pItems)
				{
					// 부모가 차일드 윈도가 아니어야 OnCommand 메시지를 받는다.
					CWndBase* pWnd = m_pParentWnd;
					pWnd->OnChildNotify(WNM_SELCHANGE,m_nIdWnd,(LRESULT*)m_pFocusItem); 
					return;
				}
			}
			pt.y += m_nFontHeight + 3;
		}
	}
	//
	// 아이콘 출력
	// CCtrllist
	if( IsWndStyle( WLVS_ICON ) )
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 32;
		int nHeight = rect.Height() / 32;
		pt.y = 0;
		pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
		for( int i = pt.y; i < m_aItems.GetSize(); i++ ) 
		{
			int x = ( i - pt.y ) % nWidth;
			int y = ( i - pt.y ) / nWidth;
			LVITEM* pItems = (LVITEM*)m_aItems.GetAt( i );
			rect.SetRect( x * 32, y * 32, x * 32 + 32, y * 32 + 32 );
			if( rect.PtInRect( point ) )
			{
				if(m_pFocusItem == pItems)
				{
					// 부모가 차일드 윈도가 아니어야 OnCommand 메시지를 받는다.
					CWndBase* pWnd = m_pParentWnd;
					pWnd->OnChildNotify(WNM_SELCHANGE,m_nIdWnd,(LRESULT*)m_pFocusItem); 
					return;
				}
			}
		}
	}
}
void CWndListCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	CPoint pt( 3, 3 );
	CRect rect;
	//
	// 리포트 출력 
	//
	//if( IsWndStyle( WLVS_REPORT ) )
	{
		pt.y -= (m_nFontHeight + 3) * m_wndScrollBar.GetScrollPos();
		for( int i = 0; i < m_aItems.GetSize(); i++ ) 
		{
			LVITEM* pItems = (LVITEM*)m_aItems.GetAt( i );
			rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
			if( rect.PtInRect( point ) )
			{
				m_nCurSelect = i;
				m_pFocusItem = pItems;
				break;
			}
			pt.y += m_nFontHeight + 3;
		}
	}
	//
	// 아이콘 출력
	// CCtrllist
	if( IsWndStyle( WLVS_ICON ) )
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 32;
		int nHeight = rect.Height() / 32;
		pt.y = 0;
		pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
		for( int i = pt.y; i < m_aItems.GetSize(); i++ ) 
		{
			int x = ( i - pt.y ) % nWidth;
			int y = ( i - pt.y ) / nWidth;
			LVITEM* pItems = (LVITEM*)m_aItems.GetAt( i );
			rect.SetRect( x * 32, y * 32, x * 32 + 32, y * 32 + 32 );
			if( rect.PtInRect( point ) )
			{
				m_nCurSelect = i;
				m_pFocusItem = pItems;
				break;
			}
		}
	}
} 
void CWndListCtrl::OnRButtonUp( UINT nFlags, CPoint point )
{
}
void CWndListCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{
}
void CWndListCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
}
void CWndListCtrl::OnRButtonDblClk( UINT nFlags, CPoint point)
{
}
void CWndListCtrl::OnSize( UINT nType, int cx, int cy )
{
	CRect rect = GetWindowRect();

	if( IsWndStyle( WBS_VSCROLL ) ) 
		rect.left = rect.right - VSCROLL_WIDTH;
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
	m_wndScrollBar.SetWndRect( rect );

	CWndBase::OnSize( nType, cx, cy);
}
void CWndListCtrl::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 4, 4 );

	//if( IsWndStyle( WLVS_REPORT ) )
	{
		int nPage = GETRANGE;//GetClientRect().Height() / (m_nFontHeight + 3);
		int nRange = m_aItems.GetSize();// - nPage;
		if( !( nRange - nPage <= 0 ) )
			m_rectClient.right -= 15; // 스크롤 바가 보이면 
		m_rectClient.top += 15; // 리포트 칼럼 
	}
	if( IsWndStyle( WLVS_ICON ) )
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 32;
		int nHeight = rect.Height() / 32;
		int nPage = nHeight;
		int nRange = m_aItems.GetSize() / nWidth;// - nPage;
		if( !( nRange - nPage <= 0 ) )
			m_rectClient.right -= 15; // 스크롤 바가 보이면 
	}
	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}
void CWndListCtrl::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	m_pTheme->RenderWndTextFrame( p2DRender, &rect );

	//if( IsWndStyle( WLVS_REPORT ) )
	{
		// 컬럼 출력
		int x = 0;
		for( int i = 0; i < m_aColumns.GetSize(); i++ )
		{
			LVCOLUMN* pColumn = (LVCOLUMN*)m_aColumns.GetAt( i );
			p2DRender->TextOut( x + 4, 4, pColumn->pszText ); 
			m_pTheme->GradationRect( p2DRender, &CRect( x, 0, x + pColumn->cx + 5, 20 ), 
				D3DCOLOR_ARGB( 50, 100, 100, 100 ),
				D3DCOLOR_ARGB( 90, 255, 255, 255 ),
				D3DCOLOR_ARGB( 50, 000, 000, 000 ), 20 );
			x += pColumn->cx + 6;
		}
		m_pTheme->GradationRect( p2DRender, &CRect( x, 0, rect.Width(), 20 ), 
			D3DCOLOR_ARGB( 50, 100, 100, 100 ),
			D3DCOLOR_ARGB( 90, 255, 255, 255 ),
			D3DCOLOR_ARGB( 50, 000, 000, 000 ), 20 );
	}
}
/*
typedef struct tagLVCOLUMNA
{
    UINT mask;
    int fmt;
    int cx;
    LPSTR pszText;
    int cchTextMax;
    int iSubItem;
#if (_WIN32_IE >= 0x0300)
    int iImage;
    int iOrder;
#endif
} LVCOLUMNA, FAR* LPLVCOLUMNA;
typedef struct tagLVITEMA
{
    UINT mask;
    int iItem;
    int iSubItem;
    UINT state;
    UINT stateMask;
    LPSTR pszText;
    int cchTextMax;
    int iImage;
    LPARAM lParam;
#if (_WIN32_IE >= 0x0300)
    int iIndent;
#endif
} LVITEMA, FAR* LPLVITEMA;
*/
BOOL CWndListCtrl::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}
BOOL CWndListCtrl::SetItem( const LVITEM* pItem )
{
	if( pItem->iItem < m_aItems.GetSize() && m_aItems.GetAt( pItem->iItem ) == NULL ) 
		return FALSE; // 존재하지 않는다.
	LVITEM* pItems = (LVITEM*)m_aItems.GetAt( pItem->iItem );
	if( pItems )
	{
		if( pItems[ pItem->iSubItem ].pszText )
			SAFE_DELETE( pItems[ pItem->iSubItem ].pszText );
		memcpy( &pItems[ pItem->iSubItem ], pItem, sizeof( LVITEM ) );

		size_t nTextSize = _tcslen( pItem->pszText ) + 1;
		pItems[ pItem->iSubItem ].pszText = new _TCHAR[ nTextSize ];
		FLStrcpy( pItems[ pItem->iSubItem ].pszText, nTextSize, pItem->pszText );
	}
	return TRUE;
}
int CWndListCtrl::InsertItem( const LVITEM* pItem )
{
//	if( pItem->iItem < m_aItems.GetSize() && m_aItems.GetAt( pItem->iItem ) ) 
	if( pItem->iItem > m_aItems.GetSize() ) 
			return -1; // 이미 존재한다.
	LVITEM* pNewItems = new LVITEM[ m_aColumns.GetSize() ];
	ZeroMemory( pNewItems, sizeof( LVITEM ) * m_aColumns.GetSize() );
	memcpy( &pNewItems[ pItem->iSubItem ], pItem, sizeof( LVITEM ) );

	size_t nTextSize = _tcslen( pItem->pszText ) + 1;
	pNewItems[ pItem->iSubItem ].pszText = new _TCHAR[ nTextSize ];
	FLStrcpy( pNewItems[ pItem->iSubItem ].pszText, nTextSize, pItem->pszText );
	m_aItems.SetAtGrow( pItem->iItem, (void*)pNewItems );
	return pItem->iItem;
}
int CWndListCtrl::GetItemCount() const
{
	return m_aItems.GetSize();
}
int CWndListCtrl::InsertColumn( int nCol, const LVCOLUMN* pColumn )
{
	LVCOLUMN* pNewColumn = new LVCOLUMN;
	memcpy( pNewColumn, pColumn, sizeof( LVCOLUMN ) );

	size_t nTextSize = _tcslen( pColumn->pszText ) + 1;
	pNewColumn->pszText = new _TCHAR[ nTextSize ];
	FLStrcpy( pNewColumn->pszText, nTextSize, pColumn->pszText );
	if( nCol < m_aColumns.GetSize() && m_aColumns.GetAt( nCol ) ) 
		return -1; // 이미 존재한다.
	m_aColumns.SetAtGrow( nCol, (void*)pNewColumn );
	// 컬럼 버튼 생성 
	/*
	CPtrArray m_aWndColumns;
	CRect rect;// = GetWindowRect();
	CWndButton* pWndButton = new CWndButton;
	pWndButton->AddWndStyle( WBS_DOCKING );
	rect = CRect( 0, 0, pColumn->cx, 20 );
	if( nCol )
	{
		LVCOLUMN* pNewColumn = (LVCOLUMN*)m_aColumns.GetAt( nCol - 1 );
		if( 
		i
	}
	pWndButton->Create( pColumn->pszText, 0, rect , this, nCol );
	m_aWndColumns.SetAtGrow( nCol, pWndButton );
	*/
	return nCol;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

