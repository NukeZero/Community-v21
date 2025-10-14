// WndBase.cpp: implementation of the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defineSound.h" 
#include "AppDefine.h" // 여기서 이걸 왜 include하지? 
#include "clipboard.h"
//#ifndef __DAISY 
//#include "WndManager.h" // 하부 클래스에서 WndManager같은거 인크루드 하지 말기. 모듈화와 거리가 멀어짐.
//
// 경고!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// 이 클레스는 기본 윈도 베이스 콘토롤이다. 이 콘트롤을 개조하기 위해 매인프레임의 펑션을 부른다거나
// 애플랫 단위에 접근을 시도하는 행위는 엄격히 금지함!!! 이미 몇부분 그러한 행각이 발생했으므로 
// 작성자는 수정하기 바람!!!! 기본 윈도 코딩에서 윈도의 콘트롤 박스나 리스트 박스가 맘에 안든다고
// API 자체를 수정하는 행위는 불가능하다. 윈도에서는 콜백, 훅. 오버라이드 같은 방식으로 기능에 변형을 가한다.
// 여기서는 클레스를 계승받아 오버라이드하여 재작성하는 방법이 적당하다.

#define EDIT_HEIGHT 8
#define VSCROLL_WIDTH 16

#ifdef __V050322_CACHEQUEST
#define GETRANGE ( ( GetClientRect().Height() + m_nLineSpace ) / GetFontHeight() )
#else
#define GETRANGE ( GetClientRect().Height() / ( m_pFont->GetMaxHeight() + 3 ) )
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndTreeCtrl
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndTreeCtrl::CWndTreeCtrl()
{
	//m_nWndColor    = D3DCOLOR_TEMP( 255,  0x5 << 3,  0x5 << 3,  0x5 << 3 );
	m_byWndType    = WTYPE_TREECTRL;
#ifdef __V050322_CACHEQUEST
	m_strTexture = DEF_CTRL_TEXT;
	m_bTile = TRUE;
	m_nLineSpace = 3;
	m_nFontColor   = D3DCOLOR_ARGB( 255, 64, 64, 64 );
	m_nSelectColor = D3DCOLOR_ARGB( 255, 0, 0,   255 );
#else
	m_nFontColor   = D3DCOLOR_TEMP( 255, 255, 255, 255 );
	m_nSelectColor = D3DCOLOR_TEMP( 255, 255, 255,   0 );
#endif
} 

CWndTreeCtrl::~CWndTreeCtrl()
{
#ifdef __V050322_CACHEQUEST
	DeleteAllItems();
#else
//{{AFX
	FreeTree(m_treeElem.m_ptrArray);
	for(int i = 0; i < m_treeItemArray.GetSize(); i++)
		delete (LPTREEITEM)m_treeItemArray.GetAt(i);
//}}AFX
#endif
}
BOOL CWndTreeCtrl::Create(DWORD dwTextStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID, int nFontColorR, int nFontColorG, int nFontColorB )
{
	SetFontColor( nFontColorR, nFontColorG, nFontColorB );
	return CWndBase::Create( WBS_CHILD | dwTextStyle, rect, pParentWnd, nID );
}
#ifdef __V050322_CACHEQUEST
BOOL CWndTreeCtrl::DeleteAllItems()
{
	FreeTree(m_treeElem.m_ptrArray);
	for(int i = 0; i < m_treeItemArray.GetSize(); i++)
		delete (LPTREEITEM)m_treeItemArray.GetAt(i);
	m_pFocusElem = NULL;
	m_treeItemArray.RemoveAll();
	return TRUE;
}
LPTREEELEM CWndTreeCtrl::GetCurSel()
{
	return m_pFocusElem;
}
LPTREEELEM CWndTreeCtrl::GetRootElem() 
{ 
	return &m_treeElem; 
}
LPTREEELEM CWndTreeCtrl::GetNextElem( LPTREEELEM pElem, int& nPos )
{
	LPTREEELEM pGetElem = NULL;
	if( nPos < pElem->m_ptrArray.GetSize() ) 
	{
		pGetElem = (LPTREEELEM)pElem->m_ptrArray.GetAt( nPos );
		nPos++;
	}
	return pGetElem;
}
LPTREEELEM CWndTreeCtrl::FindTreeElem( DWORD dwData )
{
	return FindTreeElem( m_treeElem.m_ptrArray, dwData );
}
LPTREEELEM CWndTreeCtrl::FindTreeElem( CPtrArray& ptrArray, DWORD dwData )
{
	for(int i = 0; i < ptrArray.GetSize(); i++)
	{
		LPTREEELEM lpTreeElem = (LPTREEELEM)ptrArray.GetAt(i);
		if( lpTreeElem->m_dwData == dwData )
			return lpTreeElem;
		if( lpTreeElem->m_ptrArray.GetSize() )
		{
			lpTreeElem = FindTreeElem( lpTreeElem->m_ptrArray, dwData );
			if( lpTreeElem ) return lpTreeElem;
		}
	}
	return NULL;
}
LPTREEELEM CWndTreeCtrl::SetCurSel( DWORD dwData )
{
	m_pFocusElem = FindTreeElem( m_treeElem.m_ptrArray, dwData );
	return m_pFocusElem;
}
LPTREEELEM CWndTreeCtrl::SetCurSel( LPCTSTR lpszKeyword )
{
	m_pFocusElem = FindTreeElem( m_treeElem.m_ptrArray, lpszKeyword );
	return m_pFocusElem;
}
#endif
void CWndTreeCtrl::FreeTree( CPtrArray& ptrArray ) 
{
	for(int i = 0; i < ptrArray.GetSize(); i++)
	{
		LPTREEELEM lpTreeElem = (LPTREEELEM)ptrArray.GetAt(i);
		if(lpTreeElem->m_ptrArray.GetSize())
			FreeTree(lpTreeElem->m_ptrArray);
		delete lpTreeElem;
	}
	ptrArray.RemoveAll();
}
LPTREEELEM CWndTreeCtrl::FindTreeElem( LPCTSTR lpszKeyword )
{
	return FindTreeElem( m_treeElem.m_ptrArray, lpszKeyword );
}
LPTREEELEM CWndTreeCtrl::FindTreeElem( CPtrArray& ptrArray, LPCTSTR lpszKeyword )
{
	for(int i = 0; i < ptrArray.GetSize(); i++)
	{
		LPTREEELEM lpTreeElem = (LPTREEELEM)ptrArray.GetAt(i);
		if( lpTreeElem->m_strKeyword == lpszKeyword )
			return lpTreeElem;
		if( lpTreeElem->m_ptrArray.GetSize() )
		{
			lpTreeElem = FindTreeElem( lpTreeElem->m_ptrArray, lpszKeyword );
			if( lpTreeElem ) return lpTreeElem;
		}
	}
	return NULL;
}

LPTREEELEM CWndTreeCtrl::InsertItem( LPTREEELEM lpParent, LPCTSTR lpString, DWORD dwData )
{
	LPTREEELEM lpTreeElem = new TREEELEM;
#ifdef __V050322_CACHEQUEST
	lpTreeElem->m_lpParent = lpParent;
	lpTreeElem->m_dwColor = m_nFontColor;
#endif
	lpTreeElem->m_strKeyword = lpString;
	lpTreeElem->m_dwData = dwData;
	lpTreeElem->m_bOpen = FALSE;
	if( lpParent )
		lpParent->m_ptrArray.Add( lpTreeElem );
	else
		m_treeElem.m_ptrArray.Add( lpTreeElem );
	return lpTreeElem;
}
void CWndTreeCtrl::LoadTreeScript(LPCTSTR lpFileName) 
{
	CScript script;
 	if( script.Load( lpFileName ) == FALSE )
		return;
	//script.GetToken(); // keyword
	do {
		InterpriteScript( script, m_treeElem.m_ptrArray );
		script.GetToken(); // keyword
	} while( script.tok != FINISHED );

}
void CWndTreeCtrl::InterpriteScript( CScript& script, CPtrArray& ptrArray ) 
{
#ifdef __USE_STR_TBL0407
	LPTREEELEM lpTreeElem	= NULL;
	script.GetToken();	// keyword
	while( *script.token != '}' && script.tok != FINISHED )
	{
		lpTreeElem	= new TREEELEM;
		lpTreeElem->m_strKeyword	= script.Token;
		lpTreeElem->m_bOpen		= FALSE;
		lpTreeElem->m_dwColor	= m_nFontColor;
		ptrArray.Add(lpTreeElem);
		lpTreeElem->m_dwData	= 0;
		script.GetToken();	// keyword
		if( *script.token == '{' )
		{
			if( lpTreeElem )
				InterpriteScript( script, lpTreeElem->m_ptrArray );
		}
	}
	if( script.tok == FINISHED )
		return;
	script.GetToken();
#else	// __USE_STR_TBL0407
	int nLang;
	nLang = script.GetNumber();
	LPTREEELEM lpTreeElem = NULL;
	
	do {
		script.GetLastFull(); // keyword
#ifdef __DAISY
		if( nLang == 0 )
#else
		if( nLang == g_Option.m_nLanguage  )
#endif
		{
			lpTreeElem = new TREEELEM;
			lpTreeElem->m_strKeyword = script.Token;
			lpTreeElem->m_bOpen = FALSE;
#ifdef __V050322_CACHEQUEST
			lpTreeElem->m_dwColor = m_nFontColor;
#endif
			ptrArray.Add(lpTreeElem);
			lpTreeElem->m_dwData = 0;
		}
		//script.GetToken(); 
		nLang = script.GetNumber();
		if(*script.token == '{') // {이면 string은 keyword이다 
		{
			if( lpTreeElem )
			//script.GetToken(); // keyword
				InterpriteScript( script, lpTreeElem->m_ptrArray );
		}
		/*
		else
		if( nType == NUMBER )
		{
			script.PutBack();
			lpTreeElem->m_dwData = script.GetNumber();
			script.GetToken(); 
		}
		*/

	} while( *script.token != '}' && script.tok != FINISHED );

	if( script.tok == FINISHED)
		return;

	script.GetToken(); 
#endif	__USE_STR_TBL0407
}
void CWndTreeCtrl::OnInitialUpdate()
{
	//CSize size = m_pSprPack->GetAt(13)->GetSize();
	CRect rect = GetWindowRect();
#ifdef __V050322_CACHEQUEST
	m_pTexButtOpen  = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(),"ButtTreeOpen.tga"   ), 0xffff00ff );
	m_pTexButtClose = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(),"ButtTreeClose.tga"   ), 0xffff00ff );
#endif
	//AddWndStyle(WBS_CAPTION);
	//m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_DOCKING | WBS_VERT, rect, this, 1000 );//,m_pSprPack,-1);
#ifdef __V050322_CACHEQUEST
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
#endif
}
void CWndTreeCtrl::SetWndRect(CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 4, 4 );
#ifdef __V050322_CACHEQUEST
	if( IsWndStyle( WBS_VSCROLL ) ) 
		m_rectClient.right -= 15;
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
#endif // __V050322_CACHEQUEST

	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}
void CWndTreeCtrl::PaintFrame(C2DRender* p2DRender)
{
	CRect rect = GetWindowRect();
//	m_pTheme->RenderWndTextFrame( p2DRender, &rect );
/*
	DWORD dwColor1 = D3DCOLOR_ARGB( 158, 0, 0,  0 );//D3DCOLOR_TEMP( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 180, 240, 240,  30 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  30 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//

	p2DRender->RenderFillRect ( rect, dwColor1 );
	p2DRender->RenderRoundRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor3 );
*/
	//p2DRender->RenderFillRect ( rect, D3DCOLOR_TEMP( 128,  50,  50,  50 ) );
	//p2DRender->RenderRoundRect( rect, D3DCOLOR_TEMP( 128, 200, 200, 200 ) );
}
void CWndTreeCtrl::OnDraw(C2DRender* p2DRender) 
{
	CPoint pt( 3, 3);
	m_nFontHeight = GetFontHeight();
	pt.y -= (m_nFontHeight + 3) * m_wndScrollBar.GetScrollPos();
	for(int i = 0; i < m_treeItemArray.GetSize(); i++)
		delete m_treeItemArray.GetAt(i);
	m_treeItemArray.RemoveAll();
	PaintTree(p2DRender,pt,m_treeElem.m_ptrArray) ;

#ifdef __V050322_CACHEQUEST
	int nPage = GETRANGE; //GetClientRect().Height() / (m_nFontHeight + 3);
	int nRange = m_treeItemArray.GetSize();// - nPage;

	if(	IsWndStyle( WBS_VSCROLL ) )  
	{
		m_wndScrollBar.SetVisible( TRUE );
		m_wndScrollBar.SetScrollRange( 0, nRange );
		m_wndScrollBar.SetScrollPage( nPage );
	}
	else
		m_wndScrollBar.SetVisible( FALSE );
#else // __V050322_CACHEQUEST
//{{AFX
	int nPage = GetClientRect().Height() / (m_nFontHeight + 3);
	int nRange = m_treeItemArray.GetSize();// - nPage;
//	if(nRange - nPage <= 0)
//		m_wndScrollBar.SetVisible(FALSE);
//	else
	{
		m_wndScrollBar.SetVisible( TRUE );
		m_wndScrollBar.SetScrollRange( 0, nRange );
		m_wndScrollBar.SetScrollPage( nPage );
	}
	
//}}AFX
#endif // __V050322_CACHEQUEST
}

void CWndTreeCtrl::PaintTree(C2DRender* p2DRender,CPoint& pt,CPtrArray& ptrArray) 
{
	LPTREEELEM pTreeElem;
	LPTREEITEM pTreeItem;
	for(int i = 0; i < ptrArray.GetSize(); i++)
	{
		pTreeElem = (LPTREEELEM)ptrArray.GetAt( i );
		pTreeItem = new TREEITEM;
		CSize sizeStr;
		p2DRender->m_pFont->GetTextExtent( pTreeElem->m_strKeyword, &sizeStr);
		pTreeItem->m_rect.SetRect( pt.x, pt.y, pt.x + m_nFontHeight + 5 + sizeStr.cx, pt.y + sizeStr.cy );
		pTreeItem->m_lpTreeElem = pTreeElem;
		m_treeItemArray.Add( pTreeItem );
		if( pTreeElem->m_ptrArray.GetSize() )
		{
#ifdef __V050322_CACHEQUEST
			if( pTreeElem->m_bOpen )
				p2DRender->RenderTexture( pt, m_pTexButtOpen );
			else
				p2DRender->RenderTexture( pt, m_pTexButtClose );
				/*
			CRect rect( pt.x + 2, pt.y, pt.x + m_nFontHeight - 4, pt.y + m_nFontHeight - 6 );
			p2DRender->RenderRect( rect, m_nFontColor );
			p2DRender->RenderLine
				( 
				CPoint( rect.left + 2, rect.top + ( rect.Height() / 2) ), 
				CPoint( rect.left + rect.Width() - 2, rect.top + ( rect.Height() / 2) ), 
				m_nFontColor 
				);
			if(!pTreeElem->m_bOpen)
			{
				p2DRender->RenderLine
					(
					CPoint( rect.left + ( rect.Height() / 2 ), rect.top + 2 ),
					CPoint( rect.left + ( rect.Height() / 2 ), rect.top + rect.Height() - 2), 
					m_nFontColor
					);
			}
			*/
#else // __V050322_CACHEQUEST
//{{AFX
			CRect rect( pt.x + 2, pt.y + 2, pt.x + m_nFontHeight - 3, pt.y + m_nFontHeight - 3 );
			p2DRender->RenderRect
				( 
				rect, 
				m_nFontColor 
				);
			p2DRender->RenderLine
				( 
				CPoint( rect.left + 2, rect.top + ( rect.Height() / 2) ), 
				CPoint( rect.left + rect.Width() - 2, rect.top + ( rect.Height() / 2) ), 
				m_nFontColor 
				);
			if(!pTreeElem->m_bOpen)
			{
				p2DRender->RenderLine
					(
					CPoint( rect.left + ( rect.Height() / 2 ), rect.top + 2 ),
					CPoint( rect.left + ( rect.Height() / 2 ), rect.top + rect.Height() - 2), 
					m_nFontColor
					);
			}
//}}AFX
#endif // __V050322_CACHEQUEST
		}
		if( m_pFocusElem == pTreeElem )
		{
			p2DRender->TextOut( pt.x + m_nFontHeight + 2, pt.y, pTreeElem->m_strKeyword, m_nSelectColor );
		}
		else
		{
#ifdef __V050322_CACHEQUEST
			p2DRender->TextOut( pt.x + m_nFontHeight + 2, pt.y, pTreeElem->m_strKeyword, pTreeElem->m_dwColor );
#else
			p2DRender->TextOut( pt.x + m_nFontHeight + 2, pt.y, pTreeElem->m_strKeyword, m_nFontColor );
#endif
		}
#ifdef __V050322_CACHEQUEST
		pt.y += GetFontHeight();
#else
		pt.y += m_nFontHeight + 3;
#endif
		if( pTreeElem->m_ptrArray.GetSize() && pTreeElem->m_bOpen )
		{
#ifdef __V050322_CACHEQUEST
			pt.x += GetFontHeight();
			PaintTree(p2DRender,pt,pTreeElem->m_ptrArray) ;
			pt.x -= GetFontHeight();

#else
			pt.x += m_nFontHeight - 3;
			PaintTree(p2DRender,pt,pTreeElem->m_ptrArray) ;
			pt.x -= m_nFontHeight - 3;
#endif
		}
	}
}
void CWndTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	LPTREEITEM pTreeItem;
	for(int i = 0; i < m_treeItemArray.GetSize(); i++)
	{
		pTreeItem = (LPTREEITEM)m_treeItemArray.GetAt(i);
		if(pTreeItem->m_rect.PtInRect(point))
		{
			if(m_pFocusElem == pTreeItem->m_lpTreeElem)
			{
				// 부모가 차일드 윈도가 아니어야 OnCommand 메시지를 받는다.
				CWndBase* pWnd = m_pParentWnd;
				while(pWnd->GetStyle() & WBS_CHILD)
					pWnd = pWnd->GetParentWnd();
				pWnd->OnChildNotify( WNM_CLICKED, m_nIdWnd, (LRESULT*)m_pFocusElem );
				return;
			}
		}
	}
}
void CWndTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	LPTREEITEM pTreeItem;
	for(int i = 0; i < m_treeItemArray.GetSize(); i++)
	{
		pTreeItem = (LPTREEITEM)m_treeItemArray.GetAt(i);
		if(pTreeItem->m_rect.PtInRect(point))
		{
			CRect rect = pTreeItem->m_rect;
			rect.SetRect(rect.left,rect.top,rect.left+m_nFontHeight,rect.top+m_nFontHeight);
			if(rect.PtInRect(point))
				pTreeItem->m_lpTreeElem->m_bOpen = !pTreeItem->m_lpTreeElem->m_bOpen;
			else
				m_pFocusElem = pTreeItem->m_lpTreeElem;
		}
	}
}

void CWndTreeCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
}

void CWndTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
}
void CWndTreeCtrl::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	LPTREEITEM pTreeItem;
	for(int i = 0; i < m_treeItemArray.GetSize(); i++)
	{
		pTreeItem = (LPTREEITEM)m_treeItemArray.GetAt(i);
		if(pTreeItem->m_rect.PtInRect(point))
		{
			m_pFocusElem = pTreeItem->m_lpTreeElem;
			m_pFocusElem->m_bOpen = !m_pFocusElem->m_bOpen;
			CWndBase* pWnd = m_pParentWnd;
			while( pWnd->GetStyle() & WBS_CHILD )
				pWnd = pWnd->GetParentWnd();
			pWnd->OnChildNotify( WNM_DBLCLK, m_nIdWnd, (LRESULT*)m_pFocusElem );
		}
	}
}
void CWndTreeCtrl::OnRButtonDblClk( UINT nFlags, CPoint point)
{
}
void CWndTreeCtrl::OnSize(UINT nType, int cx, int cy)
{
	//m_string.Reset( m_pFont, &GetClientRect() );
	CRect rect = GetWindowRect();
#ifdef __V050322_CACHEQUEST
	if( IsWndStyle( WBS_VSCROLL ) ) 
		rect.left = rect.right - VSCROLL_WIDTH;
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
#else // __V050322_CACHEQUEST
//{{AFX
	rect.left = rect.right - 15;
//}}AFX
#endif // __V050322_CACHEQUEST
	m_wndScrollBar.SetWndRect( rect );
	CWndBase::OnSize( nType, cx, cy);
}
#ifdef __Y0602
BOOL CWndTreeCtrl::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	if( zDelta < 0 )
	{
		if( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() > m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()+1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() );
	}
	else
	{
		if( m_wndScrollBar.GetMinScrollPos() < m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()-1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMinScrollPos() );
	}
	
	return TRUE;
}
#endif
BOOL CWndTreeCtrl::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}
#ifndef __V050322_CACHEQUEST
//{{AFX
void CWndTreeCtrl::AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface )
{
	LPIMAGE lpImage[12];
	CSize sizeBmp[12];
	CString m_strTile = "WndEditTile06.tga";
	CString strFileName;
	
	int nImgBit = IMGBIT_32;
	for( int i = 0; i < 9; i++ )
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
	}
	///////////////////////////////////////////////////////
	CRect rect = GetWindowRect( TRUE );
	rect += ( m_pParentWnd->GetClientRect( TRUE ).TopLeft() - m_pParentWnd->GetWindowRect( TRUE ).TopLeft() );
	//ect = GetClientRect();
	rect.right -= 15;
	int nWidth = rect.Width() / 8;
	int nHeight = rect.Height() / EDIT_HEIGHT;
	CPoint point;
	for( i = 0; i < nHeight; i++ )
	{
		for( int j = 0; j < nWidth; j++ )
		{
			point = CPoint( j * 8, i * EDIT_HEIGHT ) + rect.TopLeft();
			if( i == 0 )
			{
				if( j == 0 )
					PaintTexture( pDest, lpImage[ 0 ], point, sizeSurface );
				else
					if( j == nWidth - 1 )
						PaintTexture( pDest, lpImage[ 2 ], point, sizeSurface );
					else
						PaintTexture( pDest, lpImage[ 1 ], point, sizeSurface );
			}
			else
			if( i == nHeight - 1 )
			{
				if( j == 0 )
					PaintTexture( pDest, lpImage[ 6 ], point, sizeSurface );
				else
					if( j == nWidth - 1 )
						PaintTexture( pDest, lpImage[ 8 ], point, sizeSurface );
					else
						PaintTexture( pDest, lpImage[ 7 ], point, sizeSurface );					
			}
			else
			{
				if( j == 0 )
					PaintTexture( pDest, lpImage[ 3 ], point, sizeSurface );
				else
					if( j == nWidth - 1 )
						PaintTexture( pDest, lpImage[ 5 ], point, sizeSurface );
					else
						PaintTexture( pDest, lpImage[ 4 ], point, sizeSurface );					
			}
		}
	}
}
//}}AFX
#endif // __V050322_CACHEQUEST

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndScrollBar
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndScrollBar::CWndScrollBar()
{
	m_nScrollPos = 0;
	m_nScrollMinPos = 0;
	m_nScrollMaxPos = 0;
	m_nScrollPage = 0;
	//m_dwScrollStyle = 0;
	m_byWndType = WTYPE_SCROLLBAR;
}
CWndScrollBar::~CWndScrollBar()
{
}
BOOL CWndScrollBar::Create(DWORD dwStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID)
{
	CRect rc = rect;
	rc.left = rc.right - 15;//VSCROLL_WIDTH;
	return CWndBase::Create( dwStyle | WBS_CHILD | WBS_DOCKING, rc, pParentWnd, nID );
}
int CWndScrollBar::GetScrollPos() const // Retrieves the current position of a scroll box. 
{
	return m_nScrollPos;
}
int CWndScrollBar::SetScrollPos(int nPos, BOOL bRedraw) // Sets the current position of a scroll box. 
{
	m_nScrollPos = nPos;
	return 1;
}
void CWndScrollBar::SetMinScrollPos() 
{
	m_nScrollPos = m_nScrollMinPos;
}
void CWndScrollBar::SetMaxScrollPos() 
{
	m_nScrollPos = m_nScrollMaxPos - m_nScrollPage;
	if( m_nScrollPos < 0 ) m_nScrollPos = 0;
}
int CWndScrollBar::GetMinScrollPos() 
{
	return m_nScrollMinPos;
}
int CWndScrollBar::GetMaxScrollPos() 
{
	return m_nScrollMaxPos;
}
int CWndScrollBar::GetScrollRange() const
{
	return m_nScrollMaxPos - m_nScrollMinPos;
}
int CWndScrollBar::GetScrollAbiliableRange() 
{
	CRect rect = GetClientRect();
	rect.top += m_wndArrow1.GetClientRect().Height(); 
	rect.bottom -= m_wndArrow2.GetClientRect().Height();
	int nHeight = rect.Height();
	int nPadHeight = ( m_nScrollPage * nHeight / GetScrollRange() );
	if( nPadHeight < 6 ) nPadHeight = 6;
	rect.bottom = rect.top + nPadHeight;
	// 역산해서 길이를 조정하자.
	return nHeight * m_nScrollPage / nPadHeight;
}
int CWndScrollBar::GetScrollAbiliablePos() 
{
	// Range : pos = AbiliableRange : AbiliablePos
	return m_nScrollPos * GetScrollAbiliableRange() / GetScrollRange();
}
int CWndScrollBar::GetScrollAbiliablePage() 
{
	// Range : page = AbiliableRange : AbiliablePage
	return m_nScrollPage * GetScrollAbiliableRange() / GetScrollRange();
}
void CWndScrollBar::GetScrollRange(LPINT lpMinPos, LPINT lpMaxPos) const // Retrieves the current minimum and maximum scroll-bar positions for the given scroll bar. 
{
	*lpMinPos = m_nScrollMinPos;
	*lpMinPos = m_nScrollMaxPos;
}
void CWndScrollBar::SetScrollRange(int nMinPos, int nMaxPos, BOOL bRedraw) // Sets minimum and maximum position values for the given scroll bar. 
{
	m_nScrollMinPos = nMinPos;
	m_nScrollMaxPos = nMaxPos;
	/*
	// auto hide
	if( GetScrollRange() - m_nScrollPage <= 0 )
		SetVisible( FALSE );
	else
		SetVisible( TRUE );
	*/
}
void CWndScrollBar::ShowScrollBar(BOOL bShow) // Shows or hides a scroll bar. 
{
}
BOOL CWndScrollBar::EnableScrollBar(UINT nArrowFlags) // Enables or disables one or both arrows of a scroll bar. 
{
	return TRUE;
}
BOOL CWndScrollBar::SetScrollInfo(LPSCROLLINFO lpScrollInfo, BOOL bRedraw) // Sets information about the scroll bar. 
{
	return TRUE;
}
BOOL CWndScrollBar::GetScrollInfo(LPSCROLLINFO lpScrollInfo, UINT nMask) // Retrieves information about the scroll bar. 
{
	return TRUE;
}
int CWndScrollBar::GetScrollLimit() // Retrieves the limit of the scroll bar
{
	return 1;
}
int CWndScrollBar::GetScrollPage() const
{
	return m_nScrollPage;
}	
void CWndScrollBar::SetScrollPage(int nPage)
{
	m_nScrollPage = nPage;
	/*
	// auto hide
	if( GetScrollRange() - m_nScrollPage <= 0 )
		SetVisible( FALSE );
	else
		SetVisible( TRUE );
	*/
}	
/*
CSprite* CWndScrollBar::GetSprVertArrow1()
{
	return m_pSprPack->GetAt(m_nScrollBarIdx+0);
}
CSprite* CWndScrollBar::GetSprVertArrow2()
{
	return m_pSprPack->GetAt(m_nScrollBarIdx+2);
}
CSprite* CWndScrollBar::GetSprVertPad()
{
	return m_pSprPack->GetAt(m_nScrollBarIdx+4);
}
CSprite* CWndScrollBar::GetSprVertBar()
{
	return m_pSprPack->GetAt(m_nScrollBarIdx+6);
}
*/
void CWndScrollBar::SetWndRect(CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	//m_rectClient.DeflateRect( 1, 1 );
	if( bOnSize )
		OnSize(0,m_rectClient.Width(),m_rectClient.Height());
}
void CWndScrollBar::PaintFrame(C2DRender* p2DRender)
{
	static const int TEXTURE_V_SCROLL_ARROW_SIZE_Y = 14;
	static const int TEXTURE_V_SCROLL_BAR_SIZE_Y = m_pTexButtVScrBar->m_size.cy;

	CRect rect = GetClientRect();
	CPoint point = rect.TopLeft();
	FLOAT fHeightSize = static_cast<FLOAT>( ( rect.Height() - ( TEXTURE_V_SCROLL_ARROW_SIZE_Y * 2 ) ) / TEXTURE_V_SCROLL_BAR_SIZE_Y );

	p2DRender->RenderTexture( CPoint( point.x, point.y + TEXTURE_V_SCROLL_ARROW_SIZE_Y ), m_pTexButtVScrBar, 255, 1.0F, fHeightSize );
}
void CWndScrollBar::OnDraw(C2DRender* p2DRender)
{
	if( GetScrollRange() && GetScrollRange() - m_nScrollPage > 0 )
	{
		CRect rect = GetClientRect();
		rect.top += m_wndArrow1.GetClientRect().Height();
		rect.bottom -= m_wndArrow2.GetClientRect().Height();

		int nHeight = rect.Height();
		int nRange = GetScrollRange();
		int nPos = GetScrollPos();
		int nPage = GetScrollPage();

		int nPadHeight = ( nPage * nHeight / nRange );
		if( nPadHeight < 6 )
		{
			nHeight -= 6 - nPadHeight;
			nPadHeight = 6;
		}
		rect.bottom = rect.top + nPadHeight;
		int nPadPos =  ( nPos * nHeight / nRange );

		rect.OffsetRect( 0, nPadPos );

		static const int TEXTURE_SCROLL_P_UP_SIZE_Y = m_pTexButtVScrPUp->m_size.cy;
		static const int TEXTURE_SCROLL_P_BAR_SIZE_Y = m_pTexButtVScrPBar->m_size.cy;
		static const int TEXTURE_SCROLL_P_DOWN_SIZE_Y = m_pTexButtVScrPDown->m_size.cy;

		CPoint point = rect.TopLeft();
		FLOAT fHeightSize = static_cast<FLOAT>( ( rect.Height() / TEXTURE_SCROLL_P_BAR_SIZE_Y ) - 1 );

		if( nHeight > 0 )
		{
			p2DRender->RenderTexture( CPoint( point.x, point.y ), m_pTexButtVScrPUp );
			p2DRender->RenderTexture( CPoint( point.x, point.y + TEXTURE_SCROLL_P_UP_SIZE_Y ), m_pTexButtVScrPBar, 255, 1.0F, fHeightSize );
			p2DRender->RenderTexture( CPoint( point.x, point.y + ( rect.Height() - TEXTURE_SCROLL_P_DOWN_SIZE_Y ) ), m_pTexButtVScrPDown );
		}
	}
}
//CTexturePack CWndScrollBar::m_texturePack;

void CWndScrollBar::OnInitialUpdate()
{
	//m_pTexButtVScrUp    = m_texturePack.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtVScrUp.bmp"    ), 0xffff00ff );
	//m_pTexButtVScrDown  = m_texturePack.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtVScrDown.bmp"  ), 0xffff00ff );
	m_pTexButtVScrBar   = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,CWndBase::m_resMng.GetSelectLang(), "ButtVScrBar.bmp"   ), 0xffff00ff );
	m_pTexButtVScrPUp   = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,CWndBase::m_resMng.GetSelectLang(), "ButtVScrPUp.bmp"   ), 0xffff00ff );
	m_pTexButtVScrPDown = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,CWndBase::m_resMng.GetSelectLang(), "ButtVScrPDown.bmp" ), 0xffff00ff );
	m_pTexButtVScrPBar  = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(),"ButtVScrPBar.bmp"  ), 0xffff00ff );

	CRect rect = GetClientRect();
	rect.bottom = rect.bottom / 4 * 4;
	//rect.DeflateRect( 1, 1 );
	//CSize size;// = m_pSprPack->GetAt(13+2)->GetSize();
	m_wndArrow1.Create( "",0,CRect( rect.left, rect.top, rect.right, rect.top + rect.Width()),this,1000);//,m_pSprPack,13+0);
	m_wndArrow1.SetTexture( m_pApp->m_pd3dDevice,MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(),"ButtVScrUp.tga" ), TRUE );
	m_wndArrow1.FitTextureSize();
	//rect.top = rect.bottom - size.cy;
	rect.top = 0;
	m_wndArrow2.Create( "",0,CRect( rect.left, rect.bottom - rect.Width(), rect.right, rect.bottom),this,1001);//,m_pSprPack,13+2);
	m_wndArrow2.SetTexture( m_pApp->m_pd3dDevice,MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(),"ButtVScrDown.tga" ), TRUE );
	m_wndArrow2.FitTextureSize();

	m_wndArrow1.SetPushTime(500);
	m_wndArrow2.SetPushTime(500);

	m_wndArrow1.DelWndStyle( WBS_NODRAWFRAME );
	m_wndArrow2.DelWndStyle( WBS_NODRAWFRAME );
	m_bPushPad = FALSE;
}
void CWndScrollBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_bPushPad = FALSE;
}
void CWndScrollBar::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	OnLButtonDown( nFlags, point );
}
void CWndScrollBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	CRect rect = GetClientRect();
	rect.top += m_wndArrow1.GetClientRect().Height();
	rect.bottom -= m_wndArrow2.GetClientRect().Height();
	if(rect.PtInRect(point) && GetScrollRange())
	{
		rect = GetClientRect();
		rect.top += m_wndArrow1.GetClientRect().Height(); 
		rect.bottom -= m_wndArrow2.GetClientRect().Height();
		int nHeight = rect.Height();
		int nRange = GetScrollRange();
		int nPos = GetScrollPos();
		int nPage = GetScrollPage();
		// nRange : nPage = rect.Height() : x
		int nPadHeight = ( nPage * nHeight / nRange );
		if( nPadHeight < 6 ) 		
		{ 
			nHeight -= 6 - nPadHeight;
			nPadHeight = 6;
		}
		rect.bottom = rect.top + nPadHeight;
		// nRange : m_nScrollPos = rect.Height() : x
		int nPadPos =  ( nPos * nHeight / nRange );
		rect.OffsetRect( 0, nPadPos );
		
		if( rect.PtInRect( point ) )
		{
			SetCapture();
			m_nPos = point.y - rect.top; 
			m_bPushPad = TRUE;
		}
		else
		if( point.y < rect.top )
		{
			m_nScrollPos -= m_nScrollPage;
			if(m_nScrollPos < m_nScrollMinPos)
				m_nScrollPos = m_nScrollMinPos;
		}
		else
		if( point.y > rect.bottom )
		{
			m_nScrollPos += m_nScrollPage;
			if(m_nScrollPos > m_nScrollMaxPos - m_nScrollPage)
				m_nScrollPos = m_nScrollMaxPos - m_nScrollPage;
		}
	}
}
void CWndScrollBar::OnRButtonUp(UINT nFlags, CPoint point)
{
}
void CWndScrollBar::OnRButtonDown(UINT nFlags, CPoint point)
{
}
void CWndScrollBar::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bPushPad)
	{
		point.y -= ( m_wndArrow1.GetClientRect().Height() + m_nPos );
  		CRect rect = GetClientRect();
		rect.top += m_wndArrow1.GetClientRect().Height(); 
		rect.bottom -= m_wndArrow2.GetClientRect().Height();
		int nHeight = rect.Height();
		// rect.Height() : pt.y = GetScrollRange() : x
		int nPadHeight = ( m_nScrollPage * nHeight / GetScrollRange() );
		if( nPadHeight < 6 ) 		
			nHeight -= 6 - nPadHeight;
		m_nScrollPos = point.y * GetScrollRange() / nHeight;
		if( m_nScrollPos < m_nScrollMinPos ) m_nScrollPos = m_nScrollMinPos;
		if( m_nScrollPos > m_nScrollMaxPos - m_nScrollPage ) m_nScrollPos = m_nScrollMaxPos - m_nScrollPage;
	}
}
BOOL CWndScrollBar::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	switch(nID)
	{
		case 1000: 
			if(--m_nScrollPos < m_nScrollMinPos)
				m_nScrollPos = m_nScrollMinPos;
			//TRACE("%d\n",m_nScrollPos);
			break;
		case 1001:
			if(++m_nScrollPos > m_nScrollMaxPos - m_nScrollPage)
				m_nScrollPos--;// = m_nScrollMaxPos - m_nScrollPage;
			//TRACE("%d\n",m_nScrollPos);
			break;
	}
	return TRUE;
}
void CWndScrollBar::SetScrollDown()
{
	if(++m_nScrollPos > m_nScrollMaxPos - m_nScrollPage)
		m_nScrollPos--;// = m_nScrollMaxPos - m_nScrollPage;
}
void CWndScrollBar::OnSize(UINT nType, int cx, int cy)
{
	CRect rect = GetClientRect();
	rect.bottom = rect.bottom / 4 * 4;

	m_wndArrow1.SetWndRect( CRect( rect.left, rect.top, rect.right, rect.top + rect.Width() ) );
	m_wndArrow2.SetWndRect( CRect( rect.left, rect.bottom - rect.Width(), rect.right, rect.bottom) );

//	m_string.Reset( m_pFont, &GetClientRect() );
	CWndBase::OnSize( nType, cx, cy);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndListBox
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndListBox::CWndListBox()
{
	m_nWndColor    = D3DCOLOR_ARGB( 255,  0x5 << 3,  0x5 << 3,  0x5 << 3 );
	m_nFontColor   = D3DCOLOR_ARGB( 255, 64, 64, 64 );
	//m_nSelectColor = D3DCOLOR_ARGB( 255, 64, 64, 0   );
	m_nSelectColor = D3DCOLOR_ARGB( 255, 64, 64,  255   );
	m_nCurSelect = -1;
	m_pFocusItem = NULL;
	m_byWndType = WTYPE_LISTBOX;
	m_nLineSpace = 1;
#ifdef __V050322_CACHEQUEST
	m_strTexture = DEF_CTRL_TEXT;
	m_bTile = TRUE;
#endif
}
CWndListBox::~CWndListBox()
{
	ResetContent();
}
void CWndListBox::Create( DWORD dwListBoxStyle, RECT& rect, CWndBase* pParentWnd, UINT nID, int nR, int nG, int nB )
{
	SetFontColor( nR, nG, nB );
	CWndBase::Create( dwListBoxStyle | WBS_CHILD, rect, pParentWnd, nID );
}

void CWndListBox::LoadListBoxScript(LPCTSTR lpFileName) 
{
	CScanner scanner;
 	if(scanner.Load(lpFileName) == FALSE)
		return;
	scanner.GetToken(); // keyword
	do {
		InterpriteScript(scanner,m_listItemArray);
		scanner.GetToken(); // keyword
	} while(scanner.tok != FINISHED);

}
void CWndListBox::InterpriteScript(CScanner& scanner,CPtrArray& ptrArray) 
{
	do {
		LPLISTITEM lpListItem = new LISTITEM;
		lpListItem->m_strWord = scanner.Token;
		ptrArray.Add(lpListItem);
		scanner.GetToken(); 
	} while(*scanner.token != '}' && scanner.tok != FINISHED);
	if(scanner.tok == FINISHED)
		return;
	scanner.GetToken(); 
}
void CWndListBox::OnInitialUpdate()
{
	CRect rect = GetWindowRect();

	m_wndScrollBar.Create( WBS_DOCKING | WBS_VERT, rect, this, 1000 );
#ifdef __V050322_CACHEQUEST
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
#endif
}
void CWndListBox::OnDraw(C2DRender* p2DRender)
{
	CWndBase* pWnd = GetParentWnd();
	m_nFontHeight = GetFontHeight() + m_nLineSpace;
	{
		CPoint pt(3,3);
		pt.y -= m_nFontHeight * m_wndScrollBar.GetScrollPos();
		PaintListBox(p2DRender,pt,m_listItemArray);

		int nPage = GetClientRect().Height() / m_nFontHeight;
		int nRange = m_listItemArray.GetSize();
#ifdef __V050322_CACHEQUEST
		if(	IsWndStyle( WBS_VSCROLL ) )
		{
			m_wndScrollBar.SetVisible( TRUE );
			m_wndScrollBar.SetScrollRange( 0, nRange );
			m_wndScrollBar.SetScrollPage( nPage );
		}
		else
			m_wndScrollBar.SetVisible( FALSE );
#else // __V050322_CACHEQUEST
//{{AFX
		{
			m_wndScrollBar.SetVisible( TRUE );
			m_wndScrollBar.SetScrollRange( 0, nRange );
			m_wndScrollBar.SetScrollPage( nPage );
		}
//}}AFX
#endif // __V050322_CACHEQUEST
	}

	p2DRender->TextOut( 5, 5, _T("[리스트 박스]\n폰트색상 미리보기"), m_dwColor );

}

void CWndListBox::PaintListBox(C2DRender* p2DRender,CPoint& pt,CPtrArray& ptrArray) 
{
	LPLISTITEM pListItem;
	for(int i = 0; i < ptrArray.GetSize(); i++)
	{
		pListItem = (LPLISTITEM)ptrArray.GetAt(i);
		if( i == m_nCurSelect ) //m_pFocusItem == pListItem)
		{
			p2DRender->SetTextColor(m_nSelectColor);
			p2DRender->TextOut(pt.x,pt.y,pListItem->m_strWord,m_nSelectColor);
		}
		else
		{
			p2DRender->SetTextColor(m_nFontColor);
			p2DRender->TextOut(pt.x,pt.y,pListItem->m_strWord,m_nFontColor);
		}
		pt.y += m_nFontHeight;
	}
}
void CWndListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWndBase* pWnd = GetParentWnd();
	{
		CPoint pt(3,3);
		pt.y -= m_nFontHeight * m_wndScrollBar.GetScrollPos();
		CRect rect;
		LPLISTITEM pListItem;
		for(int i = 0; i < m_listItemArray.GetSize(); i++)
		{
			pListItem = (LPLISTITEM)m_listItemArray.GetAt(i);
#ifdef __V050823_JAPAN_ATOK
			int nScrollBarWidth = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetClientRect().Width() : 0;
			rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - nScrollBarWidth, pt.y + m_nFontHeight );
#else
//{{AFX
			rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
//}}AFX
#endif
			if(rect.PtInRect(point))
			{
				if(m_pFocusItem == pListItem)
				{
					// 부모가 차일드 윈도가 아니어야 OnCommand 메시지를 받는다.
					CWndBase* pWnd = m_pParentWnd;
					pWnd->OnChildNotify(WNM_SELCHANGE,m_nIdWnd,(LRESULT*)pListItem);
					return;
				}
			}
			pt.y += m_nFontHeight;
		}
	}
}
void CWndListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	CPoint pt(3,3);
	pt.y -= m_nFontHeight * m_wndScrollBar.GetScrollPos();
	CRect rect;
	LPLISTITEM pListItem;
	for(int i = 0; i < m_listItemArray.GetSize(); i++)
	{
		pListItem = (LPLISTITEM)m_listItemArray.GetAt(i);
#ifdef __V050823_JAPAN_ATOK
		int nScrollBarWidth = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetClientRect().Width() : 0;
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - nScrollBarWidth, pt.y + m_nFontHeight );
#else
//{{AFX
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
//}}AFX
#endif
		if(rect.PtInRect(point))
		{
			m_nCurSelect = i;
			m_pFocusItem = pListItem;
		}
		pt.y += m_nFontHeight;
	}
}
void CWndListBox::OnRButtonUp(UINT nFlags, CPoint point)
{
	CPoint pt(3,3);
	pt.y -= m_nFontHeight * m_wndScrollBar.GetScrollPos();
	CRect rect;
	LPLISTITEM pListItem;
	for(int i = 0; i < m_listItemArray.GetSize(); i++)
	{
		pListItem = (LPLISTITEM)m_listItemArray.GetAt(i);
#ifdef __V050823_JAPAN_ATOK
		int nScrollBarWidth = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetClientRect().Width() : 0;
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - nScrollBarWidth, pt.y + m_nFontHeight );
#else
//{{AFX
		rect.SetRect(pt.x,pt.y,pt.x+m_rectWindow.Width()-m_wndScrollBar.GetClientRect().Width(),pt.y+m_nFontHeight);
//}}AFX
#endif
		if(rect.PtInRect(point))
		{
			if(m_pFocusItem == pListItem)
			{
				// 부모가 차일드 윈도가 아니어야 OnCommand 메시지를 받는다.
				CWndBase* pWnd = m_pParentWnd;
				pListItem->m_rect.left = point.x;
				pListItem->m_rect.top = point.y;				
				//while(pWnd->GetStyle() & WBS_CHILD)
				//	pWnd = pWnd->GetParentWnd();
				pWnd->OnChildNotify(WNM_SELCANCEL,m_nIdWnd,(LRESULT*)pListItem);//m_pFocusItem); 
				return;
			}
		}
		pt.y += m_nFontHeight;
	}
}
void CWndListBox::OnRButtonDown(UINT nFlags, CPoint point)
{
}
void CWndListBox::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	CPoint pt(3,3);
	pt.y -= m_nFontHeight * m_wndScrollBar.GetScrollPos();
	CRect rect;
	LPLISTITEM pListItem;
	for(int i = 0; i < m_listItemArray.GetSize(); i++)
	{
		pListItem = (LPLISTITEM)m_listItemArray.GetAt(i);
#ifdef __V050823_JAPAN_ATOK
		int nScrollBarWidth = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetClientRect().Width() : 0;
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - nScrollBarWidth, pt.y + m_nFontHeight );
#else
//{{AFX
		rect.SetRect(pt.x,pt.y,pt.x+m_rectWindow.Width()-m_wndScrollBar.GetClientRect().Width(),pt.y+m_nFontHeight);
//}}AFX
#endif
		if(rect.PtInRect(point))
		{
			if(m_pFocusItem == pListItem)
			{
				// 부모가 차일드 윈도가 아니어야 OnCommand 메시지를 받는다.
				CWndBase* pWnd = m_pParentWnd;
				while(pWnd->GetStyle() & WBS_CHILD)
					pWnd = pWnd->GetParentWnd();
				pWnd->OnChildNotify(WNM_DBLCLK,m_nIdWnd,(LRESULT*)m_pFocusItem); 
				return;
			}
		}
		pt.y += m_nFontHeight;
	}
}
void CWndListBox::OnRButtonDblClk( UINT nFlags, CPoint point)
{
}
#ifdef __Y0602
BOOL CWndListBox::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	if( zDelta < 0 )
	{
		if( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() > m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()+1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() );
	}
	else
	{
		if( m_wndScrollBar.GetMinScrollPos() < m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()-1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMinScrollPos() );
	}
	
	return TRUE;
}
#endif

int CWndListBox::GetCount() const
{
	return m_listItemArray.GetSize();
}
DWORD CWndListBox::GetItemData(int nIndex) const
{
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	return lpListItem->m_dwData;
}

void* CWndListBox::GetItemDataPtr(int nIndex) const
{
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	return (void*)lpListItem->m_dwData;
}

int CWndListBox::SetItemData(int nIndex,DWORD dwItemData)
{
	if(nIndex < 0 || nIndex >= m_listItemArray.GetSize())
		return LB_ERR;
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	lpListItem->m_dwData = dwItemData;
	return 0;
}

int CWndListBox::SetItemDataPtr(int nIndex,void* pData)
{
	if(nIndex < 0 || nIndex >= m_listItemArray.GetSize())
		return LB_ERR;
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	lpListItem->m_dwData = (DWORD) pData;
	return 0;
}

int CWndListBox::GetItemRect(int nIndex,LPRECT lpRect) const
{
		return 1;
}

int CWndListBox::GetSel(int nIndex) const
{
		return 1;
}

int CWndListBox::GetText(int nIndex,LPSTR lpszBuffer) const
{
		return 1;
}

void CWndListBox::GetText(int nIndex,CString& rString) const
{
	if(nIndex >= 0 && nIndex < m_listItemArray.GetSize())
	{
		LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
		rString = lpListItem->m_strWord;
	}
}

int CWndListBox::GetTextLen(int nIndex) const
{
		return 1;
}


int CWndListBox::GetCurSel() const
{
	return m_nCurSelect;
}

int CWndListBox::SetCurSel(int nSelect)
{
	if(nSelect >= 0 && nSelect < m_listItemArray.GetSize())
	{	
		m_pFocusItem = (LPLISTITEM)m_listItemArray.GetAt(nSelect);
		m_nCurSelect = nSelect;
		return m_nCurSelect;
	}

	Error( "CWndListBox::SetCurSel(int nSelect) 범위 넘어섬 %d", nSelect );

	return 0;
}

int CWndListBox::SetSel(int nIndex,BOOL bSelect)
{
		return 1;
}

int CWndListBox::GetSelCount() const
{
		return 1;
}

int CWndListBox::GetSelItems(int nMaxItems,LPINT rgIndex) const
{
		return 1;
}


int CWndListBox::AddString(LPCTSTR lpszItem)
{
	LPLISTITEM lpListItem = new LISTITEM;
	lpListItem->m_strWord = lpszItem;
	m_listItemArray.Add(lpListItem);
	return m_listItemArray.GetSize()-1;
}

int CWndListBox::DeleteString(UINT nIndex)
{
	delete (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	m_listItemArray.RemoveAt(nIndex);
	if(nIndex == m_nCurSelect)
	{
		m_nCurSelect = -1;
		m_pFocusItem = NULL;
	}
	return 1;
}

void CWndListBox::SetString( int nIndex, LPCTSTR lpszItem )
{
	LPLISTITEM lpListItem	= (LPLISTITEM)m_listItemArray.GetAt( nIndex );
	lpListItem->m_strWord	= lpszItem;
}

int CWndListBox::InsertString(int nIndex,LPCTSTR lpszItem)
{
		return 1;
} 

void CWndListBox::ResetContent()
{
	for(int i = 0; i < m_listItemArray.GetSize(); i++)
		delete (LPLISTITEM)m_listItemArray.GetAt(i);
	m_nCurSelect = -1;
	m_pFocusItem = NULL;
	m_listItemArray.RemoveAll();
	m_wndScrollBar.SetScrollPos( 0, FALSE );
}

int CWndListBox::FindString( int nStartAfter, LPCTSTR lpszItem ) const
{
	if( m_listItemArray.GetSize() == 0 )
		return -1;
	LPLISTITEM lpListItem;
	int nCmp = 0;
	int nBegin = 0, nEnd = m_listItemArray.GetSize() - 1;
	int nDiv = nBegin + ( ( nEnd - nBegin ) / 2 );
	do
	{
		if( nCmp ==  1 ) nEnd = nDiv;
		if( nCmp == -1 ) nBegin = nDiv;
		nDiv = nBegin + ( ( nEnd - nBegin ) / 2 );
		lpListItem = (LPLISTITEM)m_listItemArray.GetAt( nDiv );
		nCmp = strcmp( lpListItem->m_strWord, lpszItem );
		if( nCmp ==  0 ) return nDiv;
	}
	while( ( nEnd - nBegin ) > 1 );
	int nResult;
	lpListItem = (LPLISTITEM)m_listItemArray.GetAt( nBegin );
	nCmp = strcmp( lpListItem->m_strWord, lpszItem );
	if( nCmp >= 0 ) 
		nResult = nBegin;
	else
	{
		lpListItem = (LPLISTITEM)m_listItemArray.GetAt( nEnd );
		nResult = nEnd;
	}
	if( lpszItem[ 0 ] != lpListItem->m_strWord[ 0 ] )
		nResult = -1;
	return nResult;
}
/*

input d 

0 5 = 2
2 5 = 3
2 3 = 2

0 a
1 b
2 c
3 f
4 f
5 f

0 5 = 2
2 5 = 3
3 5 = 4
3 4 = 3

0 a
1 b
2 c
3 c
4 f
5 f
*/
int QSortListBox( const VOID* arg1, const VOID* arg2 )
{
	//return _stricmp( * ( char** ) arg1, * ( char** ) arg2 );
	LPCTSTR pSrc = (*(CWndListBox::LISTITEM**)arg1)->m_strWord;
    LPCTSTR pDst = (*(CWndListBox::LISTITEM**)arg2)->m_strWord;

	return _stricmp( pSrc, pDst );
}
void CWndListBox::SortListBox() 
{
    qsort( m_listItemArray.GetData(), m_listItemArray.GetSize(), sizeof(LPLISTITEM), QSortListBox ); 
}
int CWndListBox::FindStringExact(int nIndexStart,LPCTSTR lpszItem) const
{
	return 1;
}

int CWndListBox::SelectString(int nStartAfter,LPCTSTR lpszItem)
{
	m_nCurSelect = FindString( nStartAfter, lpszItem );
	if( m_nCurSelect == -1 )
		return -1;
	int nPos = m_wndScrollBar.GetScrollPos();
	int nRange = m_wndScrollBar.GetScrollRange();
	int nPage = m_wndScrollBar.GetScrollPage();
	if( ( m_nCurSelect < nPos || m_nCurSelect > nPos + nPage ) )
	{
		if( m_nCurSelect < nRange - nPage )
			m_wndScrollBar.SetScrollPos( m_nCurSelect < 0 ? 0 : m_nCurSelect );
		else
			m_wndScrollBar.SetScrollPos( nRange - nPage  );
	}
	return m_nCurSelect;
}
void CWndListBox::OnSize(UINT nType, int cx, int cy)
{
//	m_string.Reset( m_pFont, &GetClientRect() );

	CRect rect = GetWindowRect();
#ifdef __V050322_CACHEQUEST
	if( IsWndStyle( WBS_VSCROLL ) ) 
		rect.left = rect.right - VSCROLL_WIDTH;
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
#else // __V050322_CACHEQUEST
//{{AFX
	rect.left = rect.right - 15;
//}}AFX
#endif // __V050322_CACHEQUEST
	m_wndScrollBar.SetWndRect( rect );
	CWndBase::OnSize( nType, cx, cy);
}
void CWndListBox::SetWndRect(CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 4, 4 );
#ifdef __V050322_CACHEQUEST
	if( IsWndStyle( WBS_VSCROLL ) ) 
		m_rectClient.right -= 15;
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
//}}AFX
#endif // __V050322_CACHEQUEST

	if( bOnSize )
		OnSize(0,m_rectClient.Width(),m_rectClient.Height());
}
void CWndListBox::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	//rect.right -= 15;
#ifndef __V050322_CACHEQUEST
	m_pTheme->RenderWndTextFrame( p2DRender, &rect );
#endif
	//DWORD dwColor1 = D3DCOLOR_ARGB(  88,  50,  50,  50 );//D3DCOLOR_ARGB( 255,   0,   0,  50 );//
	//DWORD dwColor1 = D3DCOLOR_ARGB( CWndBase::m_nAlpha,  79, 121, 192 );//D3DCOLOR_ARGB( 255,   0,   0,  50 );//
	//DWORD dwColor2 = D3DCOLOR_ARGB( CWndBase::m_nAlpha, 250, 250, 250 );//D3DCOLOR_ARGB( 255,  80,  80, 120 );//

	//p2DRender->RenderFillRect ( rect, dwColor1 );
	//p2DRender->RenderRoundRect( rect, dwColor2 );
	CWndBase::PaintFrame( p2DRender );
}
BOOL CWndListBox::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}
void CWndListBox::OnSetFocus( CWndBase* pOldWnd )
{
}
#ifndef __V050322_CACHEQUEST
//{{AFX
void CWndListBox::AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface )
{
	LPIMAGE lpImage[12];
	CSize sizeBmp[12];

	CString m_strTile = "WndEditTile06.tga";
	CString strFileName;
	
	int nImgBit = IMGBIT_32;
	for( int i = 0; i < 9; i++ )
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
	}
	///////////////////////////////////////////////////////

	CRect rect = GetWindowRect( TRUE );
	rect += ( m_pParentWnd->GetClientRect( TRUE ).TopLeft() - m_pParentWnd->GetWindowRect( TRUE ).TopLeft() );
	//ect = GetClientRect();
	rect.right -= 15;
	int nWidth = rect.Width() / 8;
	int nHeight = rect.Height() / EDIT_HEIGHT;
	CPoint point;
	for( i = 0; i < nHeight; i++ )
	{
		for( int j = 0; j < nWidth; j++ )
		{
			point = CPoint( j * 8, i * EDIT_HEIGHT ) + rect.TopLeft();
			if( i == 0 )
			{
				if( j == 0 )
					PaintTexture( pDest, lpImage[ 0 ], point, sizeSurface );
				else
					if( j == nWidth - 1 )
						PaintTexture( pDest, lpImage[ 2 ], point, sizeSurface );
					else
						PaintTexture( pDest, lpImage[ 1 ], point, sizeSurface );
			}
			else
			if( i == nHeight - 1 )
			{
				if( j == 0 )
					PaintTexture( pDest, lpImage[ 6 ], point, sizeSurface );
				else
					if( j == nWidth - 1 )
						PaintTexture( pDest, lpImage[ 8 ], point, sizeSurface );
					else
						PaintTexture( pDest, lpImage[ 7 ], point, sizeSurface );					
			}
			else
			{
				if( j == 0 )
					PaintTexture( pDest, lpImage[ 3 ], point, sizeSurface );
				else
					if( j == nWidth - 1 )
						PaintTexture( pDest, lpImage[ 5 ], point, sizeSurface );
					else
						PaintTexture( pDest, lpImage[ 4 ], point, sizeSurface );					
			}
		}
	}
}
//}}AFX
#endif // __V050322_CACHEQUEST

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndScrollBar
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndText::CWndText()
{
	//CString str;
	m_dwOffset = 0;
	m_dwBlockBegin = 0;
	m_dwBlockEnd = 0;
	m_bLButtonDown = FALSE;
	m_timerCaret.Set(1000);
	m_bCaretVisible = FALSE;
	m_ptCaret = 0;
	m_nLineSpace = 5;
	m_byWndType = WTYPE_TEXT;
	m_bEnableClipboard = FALSE;
	//m_bNativeMode = FALSE;
	memset(m_szCaret, 0, sizeof(char) * 3);
	
	m_szCaret[0] = 0;
#ifdef __V050322_CACHEQUEST
	m_strTexture = DEF_CTRL_TEXT;
	m_bTile = TRUE;
	m_ptDeflateRect = CPoint( 6, 6 );
#endif
}
CWndText::~CWndText()
{
}

void CWndText::SetFontColor(int nR,int nG,int nB)
{
	CWndBase::SetFontColor( nR, nG, nB );
	m_string.SetColor( m_dwColor );
}

BOOL CWndText::Create(DWORD dwStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID, int nFontColorR, int nFontColorG, int nFontColorB )
{
	SetFontColor( nFontColorR, nFontColorG, nFontColorB ); 
	return CWndBase::Create( dwStyle | WBS_CHILD, rect, pParentWnd, nID );
}

void CWndText::OnInitialUpdate()
{
	m_string.Init( m_pFont, &GetClientRect() );
	CRect rect = GetClientRect();
#ifdef __V050322_CACHEQUEST
	m_wndScrollBar.Create( WBS_DOCKING | WBS_VERT, rect, this, 1000 );
	UpdateScrollBar();
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
#else
//{{AFX
	if(	IsWndStyle( WBS_VSCROLL ) )
	{
		m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );
		UpdateScrollBar();
	}
//}}AFX
#endif
	
	m_bScr = TRUE;
	m_nLineRefresh = 0;
/*
	m_apTexture[ 0 ] = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "WndEditTile00.tga" ), 0, TRUE );
	m_apTexture[ 1 ] = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "WndEditTile01.tga" ), 0, TRUE );
	m_apTexture[ 2 ] = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "WndEditTile02.tga" ), 0, TRUE );
	m_apTexture[ 3 ] = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "WndEditTile03.tga" ), 0, TRUE );
	m_apTexture[ 4 ] = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "WndEditTile04.tga" ), 0, TRUE );
	m_apTexture[ 5 ] = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "WndEditTile05.tga" ), 0, TRUE );
	m_apTexture[ 6 ] = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "WndEditTile06.tga" ), 0, TRUE );
	m_apTexture[ 7 ] = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "WndEditTile07.tga" ), 0, TRUE );
	m_apTexture[ 8 ] = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "WndEditTile08.tga" ), 0, TRUE );
	*/
/*
	m_wndArrow1.Create( "", 0, CRect( rect.left, rect.top, rect.right, rect.top + rect.Width() ), this, 1000 );//,m_pSprPack,13+0);
	m_wndArrow1.SetTexture( m_pApp->m_pd3dDevice,MakePath( DIR_THEME, "ButtVScrUp.tga" ), TRUE );
	m_wndArrow1.FitTextureSize();
	rect.top = rect.bottom - size.cy;

	m_wndArrow2.Create( "",0,CRect( rect.left, rect.bottom - rect.Width(), rect.right, rect.bottom ), this, 1001 );//,m_pSprPack,13+2);
	m_wndArrow2.SetTexture( m_pApp->m_pd3dDevice,MakePath( DIR_THEME, "ButtVScrDown.tga" ), TRUE );
	m_wndArrow2.FitTextureSize();
*/	
}
void CWndText::OnSize( UINT nType, int cx, int cy )
{
	CRect rect = GetClientRect();
	m_string.Init( m_pFont, &rect );
	rect = GetWindowRect();
#ifdef __V050322_CACHEQUEST
	if( IsWndStyle( WBS_VSCROLL ) ) 
		rect.left = rect.right - VSCROLL_WIDTH;
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
#else // __V050322_CACHEQUEST
//{{AFX
	rect.left = rect.right - 15;
//}}AFX
#endif // __V050322_CACHEQUEST
	m_wndScrollBar.SetWndRect( rect );
	UpdateScrollBar();
	CWndBase::OnSize( nType, cx, cy);
}
void CWndText::SetWndRect(CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
#ifdef __V050322_CACHEQUEST
	if( !IsWndStyle( WBS_NOFRAME ) )
		m_rectClient.DeflateRect( m_ptDeflateRect.x, m_ptDeflateRect.y );

	if( IsWndStyle( WBS_VSCROLL ) ) 
		m_rectClient.right -= 15;
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
#else // __V050322_CACHEQUEST
//{{AFX
	if( !IsWndStyle( WBS_NOFRAME ) )
		m_rectClient.DeflateRect( 6, 6 );

	if( m_wndScrollBar.IsVisible() )
		m_rectClient.right -= 16; 
//}}AFX
#endif // __V050322_CACHEQUEST
	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
	SetCaretPos( ptCaret );
	
}
#ifndef __V050322_CACHEQUEST
//{{AFX
void CWndText::AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface )
{
	LPIMAGE lpImage[12];
	CSize sizeBmp[12];
	CString m_strTile = "WndEditTile06.tga";
	CString strFileName;

	int nImgBit = IMGBIT_32;
	for( int i = 0; i < 9; i++ )
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
	}
	///////////////////////////////////////////////////////
	CRect rect = GetWindowRect( TRUE );
	rect += ( m_pParentWnd->GetClientRect( TRUE ).TopLeft() - m_pParentWnd->GetWindowRect( TRUE ).TopLeft() );
	//ect = GetClientRect();
	if(	IsWndStyle( WBS_VSCROLL ) )
		rect.right -= 15;
	int nWidth = rect.Width() / 8;
	int nHeight = rect.Height() / EDIT_HEIGHT;
	CPoint point;
	for( i = 0; i < nHeight; i++ )
	{
		for( int j = 0; j < nWidth; j++ )
		{
			point = CPoint( j * 8, i * EDIT_HEIGHT ) + rect.TopLeft();
			if( i == 0 )
			{
				if( j == 0 )
					PaintTexture( pDest, lpImage[ 0 ], point, sizeSurface );
				else
				if( j == nWidth - 1 )
					PaintTexture( pDest, lpImage[ 2 ], point, sizeSurface );
				else
					PaintTexture( pDest, lpImage[ 1 ], point, sizeSurface );
			}
			else
			if( i == nHeight - 1 )
			{
				if( j == 0 )
					PaintTexture( pDest, lpImage[ 6 ], point, sizeSurface );
				else
				if( j == nWidth - 1 )
					PaintTexture( pDest, lpImage[ 8 ], point, sizeSurface );
				else
					PaintTexture( pDest, lpImage[ 7 ], point, sizeSurface );					
			}
			else
			{
				if( j == 0 )
					PaintTexture( pDest, lpImage[ 3 ], point, sizeSurface );
				else
				if( j == nWidth - 1 )
					PaintTexture( pDest, lpImage[ 5 ], point, sizeSurface );
				else
					PaintTexture( pDest, lpImage[ 4 ], point, sizeSurface );					
			}
		}
	}
}
//}}AFX
#endif // __V050322_CACHEQUEST

void CWndText::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
//ect = GetClientRect();
	/*
	rect.right -= 15;
	int nWidth = rect.Width() / 8;
	int nHeight = rect.Height() / 8;
	for( int i = 0; i < nHeight; i++ )
	{
		for( int j = 0; j < nWidth; j++ )
		{
			if( i == 0 )
			{
				if( j == 0 )
					p2DRender->RenderTexture( CPoint( j * 8, i * 8 ), m_apTexture[ 0 ] );
				else
				if( j == nWidth - 1 )
					p2DRender->RenderTexture( CPoint( j * 8, i * 8 ), m_apTexture[ 2 ] );
				else
					p2DRender->RenderTexture( CPoint( j * 8, i * 8 ), m_apTexture[ 1 ] );					
			}
			else
			if( i == nHeight - 1 )
			{
				if( j == 0 )
					p2DRender->RenderTexture( CPoint( j * 8, i * 8 ), m_apTexture[ 6 ] );
				else
				if( j == nWidth - 1 )
					p2DRender->RenderTexture( CPoint( j * 8, i * 8 ), m_apTexture[ 8 ] );
				else
					p2DRender->RenderTexture( CPoint( j * 8, i * 8 ), m_apTexture[ 7 ] );					
			}
			else
			{
				if( j == 0 )
					p2DRender->RenderTexture( CPoint( j * 8, i * 8 ), m_apTexture[ 3 ] );
				else
				if( j == nWidth - 1 )
					p2DRender->RenderTexture( CPoint( j * 8, i * 8 ), m_apTexture[ 5 ] );
				else
					p2DRender->RenderTexture( CPoint( j * 8, i * 8 ), m_apTexture[ 4 ] );					
			}
		}
	}
	*/
	//m_pTheme->RenderWndTextFrame( p2DRender, &rect );
}
void CWndText::OnDraw( C2DRender* p2DRender )
{
	CString string;
	DWORD dwMaxHeight = GetFontHeight();
	DWORD dwOffset = 0;
	TCHAR strHan[3];
	DWORD dwBegin = 0;
	DWORD dwCurOffset = 0;
	DWORD dwOffsetLine = m_string.OffsetToLine( m_dwOffset );
	CPoint ptCaret = 0;
	DWORD dwLineCount = m_string.GetLineCount();
	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont( m_pFont );

#ifdef __V050322_CACHEQUEST
	int nPos = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetScrollPos() : 0;
	int nLines = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetScrollPage() : GETRANGE;// GetClientRect().Height() + m_nLineSpace ) / GetFontHeight();

	int nPage = GETRANGE;//( GetClientRect().Height() + m_nLineSpace ) / GetFontHeight();
	int nRange = m_string.GetLineCount();

	if(	IsWndStyle( WBS_VSCROLL ) ) 
	{
		m_wndScrollBar.SetVisible( TRUE );
		m_wndScrollBar.SetScrollRange( 0, nRange );
		m_wndScrollBar.SetScrollPage( nPage );

		if( GetParentWnd()->m_nIdWnd == APP_MESSAGE_NOTE )
		{
			SetWndRect( GetWindowRect( TRUE ), FALSE);
			if( m_nLineRefresh < (int)( m_string.GetLineCount() ) )
			{
				m_wndScrollBar.SetScrollDown();
				m_nLineRefresh = m_string.GetLineCount();
			}
		}
	}
	else
		m_wndScrollBar.SetVisible( FALSE );
#else // __V050322_CACHEQUEST
//{{AFX
	int nPos = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetScrollPos() : 0;
	int nLines = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetScrollPage() : dwLineCount;
//	UpdateScrollBar();

	if(	IsWndStyle( WBS_VSCROLL ) )
	{
		if( GetParentWnd()->m_nIdWnd == APP_MESSAGE_NOTE )
		{
			int nPage = GetClientRect().Height() / GetFontHeight();
			int nRange = m_string.GetLineCount();// - nPage;
			m_wndScrollBar.SetVisible( TRUE );
			m_wndScrollBar.SetScrollRange( 0, nRange );
			m_wndScrollBar.SetScrollPage( nPage );
			SetWndRect( GetWindowRect( TRUE ) , FALSE);
			if( m_nLineRefresh < m_string.GetLineCount() )
			{
				m_wndScrollBar.SetScrollDown();
				m_nLineRefresh = m_string.GetLineCount();
			}
		}
	}
//}}AFX
#endif // __V050322_CACHEQUEST
	if( IsWndStyle( EBS_PASSWORD ) )
	{
		for(int i = nPos, y = 0; i < (int)( dwLineCount ); i++, y++)
		{
			dwOffset = m_string.GetLineOffset( i );
			string = m_string.GetLine(i);
			dwBegin = 0;
			int nLength = string.GetLength();
#if !defined(_MULTI_LANGUAGE_)
			for( int i2 = 0; i2 < nLength; i2++)
			{
				if( IsDBCSLeadByte( string[i2] ) )
				{
					strHan[0] = '*'; i2++;//string[i2++];
					strHan[1] = '*';//string[i2];
					strHan[2] = '\0';
					dwCurOffset = dwOffset + (i2 - 1);
				}
				else
				{
					strHan[0] = '*';//string[i2];
					strHan[1] = '\0';
					dwCurOffset = dwOffset + i2;
				}
				CSize size = m_pFont->GetTextExtent( strHan );

				DWORD dwBlockBegin;
				DWORD dwBlockEnd;
				if(m_dwBlockBegin > m_dwBlockEnd)
					dwBlockBegin = m_dwBlockEnd, dwBlockEnd = m_dwBlockBegin;
				else
					dwBlockBegin = m_dwBlockBegin, dwBlockEnd = m_dwBlockEnd;

				DWORD dwColor = m_string.m_adwColor[dwCurOffset];
				DWORD dwStyle = m_string.m_abyStyle[dwCurOffset];
				
				if(dwCurOffset >= dwBlockBegin && dwCurOffset < dwBlockEnd)
				{
					DWORD dwBkgr = dwColor;
					dwColor = ~dwColor | 0xff000000;
					p2DRender->RenderFillRect(CRect( dwBegin, 0 + y * dwMaxHeight, dwBegin + size.cx, 0 + y * dwMaxHeight + size.cy), dwBkgr );
				}
				p2DRender->TextOut( dwBegin, 0 + y * dwMaxHeight, strHan, dwColor ); 
				if( dwStyle & ESSTY_BOLD )
					p2DRender->TextOut( dwBegin + 1, 0 + y * dwMaxHeight, strHan, dwColor ); 
				if( dwStyle & ESSTY_UNDERLINE )
					p2DRender->RenderLine( CPoint( dwBegin, y * dwMaxHeight + size.cy ), CPoint( dwBegin + size.cx, y * dwMaxHeight + size.cy ), dwColor );
				

				// 문장 내에서 캐럿과 위치가 같다면 출력 위치다.
				if(dwCurOffset == m_dwOffset)
				{
					strcpy( m_szCaret, strHan );
					ptCaret = CPoint( dwBegin, 0 + y * dwMaxHeight );
				}
				dwBegin += size.cx;
			}
			// 캐럿이 문장 내에 있지 않고 문장의 끝에 새로운 입력을 기다려야하는 위치이다.
			if( m_dwOffset == m_string.GetLength() )
			{
				if( !m_dwOffset || m_string.GetAt( m_dwOffset - 1 ) != '\n' )
				{
					m_szCaret[ 0 ] = 0;
					ptCaret = CPoint( dwBegin, y * dwMaxHeight );
				}
				else
				{
					m_szCaret[ 0 ] = 0;
					ptCaret = CPoint( 0, ( y + 1 ) * dwMaxHeight );
				}
			}
#else //_MULTI_LANGUAGE_
			const char* begin = string;
			const char* end = begin + string.GetLength();
			const char* iter = begin;

			while(*iter) {

				WORD wCodePage = m_string.m_awCodePage[dwCurOffset];

				dwCurOffset = dwOffset + (iter - begin);

				const char* next = CharNextEx( iter, wCodePage );

				strHan[0] = '*';
				strHan[1] = 0;

				CSize size;
				p2DRender->m_pFont->GetTextExtent( "*", &size );
				size.cx *= next - iter;

				DWORD dwBlockBegin;
				DWORD dwBlockEnd;
				if(m_dwBlockBegin > m_dwBlockEnd)
					dwBlockBegin = m_dwBlockEnd, dwBlockEnd = m_dwBlockBegin;
				else
					dwBlockBegin = m_dwBlockBegin, dwBlockEnd = m_dwBlockEnd;

				DWORD dwColor = m_string.m_adwColor[dwCurOffset];
				DWORD dwStyle = m_string.m_abyStyle[dwCurOffset];
				
				if(dwCurOffset >= dwBlockBegin && dwCurOffset < dwBlockEnd)
				{
					DWORD dwBkgr = dwColor;
					dwColor = ~dwColor | 0xff000000;
					p2DRender->RenderFillRect(CRect( dwBegin, 0 + y * dwMaxHeight, dwBegin + size.cx, 0 + y * dwMaxHeight + size.cy), dwBkgr );
				}
				p2DRender->TextOut( dwBegin, 0 + y * dwMaxHeight, strHan, dwColor ); 
				if( dwStyle & ESSTY_BOLD )
					p2DRender->TextOut( dwBegin + 1, 0 + y * dwMaxHeight, strHan, dwColor ); 
				if( dwStyle & ESSTY_UNDERLINE )
					p2DRender->RenderLine( CPoint( dwBegin, y * dwMaxHeight + size.cy ), CPoint( dwBegin + size.cx, y * dwMaxHeight + size.cy ), dwColor );
				

				// 문장 내에서 캐럿과 위치가 같다면 출력 위치다.
				if(dwCurOffset == m_dwOffset)
				{
					_tcscpy_s( m_szCaret, _countof(m_szCaret), strHan );
					ptCaret = CPoint( dwBegin, 0 + y * dwMaxHeight );
				}
				dwBegin += size.cx;

				iter = next;
			}
#endif //_MULTI_LANGUAGE_
		}
		 //m_ptCaret = ptCaret; 
		 //m_timerCaret.Reset(); 
	}
	else
	{
		if( nLines == 6 )
		{
			int a = 0;
		}

		//ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
		m_string.SetColor( m_dwColor );
		p2DRender->TextOut_EditString( 0, 0, m_string, nPos, nLines, m_nLineSpace );
	}
	//SetCaretPos( ptCaret );
	DrawCaret( p2DRender );
	p2DRender->SetFont( pOldFont );
}
/*
void CWndText::SetString( LPCTSTR lpszText )
{
	m_string.SetString( lpszText );
}
void CWndText::AdjustString()
{
	UpdateScrollBar();
}
*/
void CWndText::UpdateScrollBar()
{
	if(	IsWndStyle( WBS_VSCROLL ) )
	{
#ifdef __V050322_CACHEQUEST
		int nPage = GETRANGE;//( GetClientRect().Height() + m_nLineSpace ) / GetFontHeight();
		int nRange = m_string.GetLineCount();// - nPage;

		if(	IsWndStyle( WBS_VSCROLL ) )  
		{
			m_wndScrollBar.SetVisible( TRUE );
			m_wndScrollBar.SetScrollRange( 0, nRange );
			m_wndScrollBar.SetScrollPage( nPage );
		}
		else
			m_wndScrollBar.SetVisible( FALSE );
#else // __V050322_CACHEQUEST
//{{AFX
		int nPage = ( GetClientRect().Height() + m_nLineSpace ) / GetFontHeight();
		int nRange = m_string.GetLineCount();// - nPage;

		if( 0 ) //nRange - nPage <= 0)
		{
			//if( m_wndScrollBar.IsVisible() )
			{
				m_wndScrollBar.SetVisible( FALSE );
				SetWndRect( GetWindowRect( TRUE ) , FALSE);
				//m_string.Reset( m_pFont, &GetClientRect() );
			}
		}
		else
		//if( m_wndScrollBar.IsVisible() == FALSE)
		{
			m_wndScrollBar.SetVisible( TRUE );
			m_wndScrollBar.SetScrollRange( 0, nRange );
			m_wndScrollBar.SetScrollPage( nPage );
			SetWndRect( GetWindowRect( TRUE ) , FALSE);
			//m_string.Reset( m_pFont, &GetClientRect() );
		}
//}}AFX
#endif // __V050322_CACHEQUEST
#ifdef __VBUGFIX_20050201
		if( m_wndScrollBar.GetScrollPos() > ( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() ) )
			m_wndScrollBar.SetMaxScrollPos();
#else
		m_wndScrollBar.SetScrollPos( 0 );
#endif
	}
	//CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
	//SetCaretPos( ptCaret );
}
CPoint CWndText::OffsetToPoint( DWORD dwSetOffset, TCHAR* pszStr )
{
	CString string;
	DWORD dwMaxHeight = GetFontHeight();
	DWORD dwOffset = 0;
	TCHAR strHan[3];
	DWORD dwBegin = 0;
	DWORD dwCurOffset = 0;
	DWORD dwOffsetLine = m_string.OffsetToLine( m_dwOffset );
	CPoint ptCaret( 0, 0);
	for(int i = m_wndScrollBar.GetScrollPos(); i < (int)( m_string.GetLineCount() ); i++)
	{
		dwOffset = m_string.GetLineOffset( i );
		string = m_string.GetLine(i);
		dwBegin = 0;
#if !defined(_MULTI_LANGUAGE_)
		for( int i2 = 0; i2 < string.GetLength(); i2++)
		{
			if( IsWndStyle( EBS_PASSWORD ) )
			{
				if( IsDBCSLeadByte( string[i2] ) )
				{
					strHan[0] = '*'; i2++;//string[i2++];
					strHan[1] = '*';//string[i2];
					strHan[2] = '\0';
					dwCurOffset = dwOffset + (i2 - 1);
				}
				else
				{
					strHan[0] = '*';//string[i2];
					strHan[1] = '\0';
					dwCurOffset = dwOffset + i2;
				}
			}
			else
			{
				if( IsDBCSLeadByte( string[i2] ) )
				{
					strHan[0] = string[i2++];
					strHan[1] = string[i2];
					strHan[2] = '\0';
					dwCurOffset = dwOffset + (i2 - 1);
				}
				else
				{
					strHan[0] = string[i2];
					strHan[1] = '\0';
					dwCurOffset = dwOffset + i2;
				}
			}
			CSize size = m_pFont->GetTextExtent(strHan);

			// 문장 내에서 캐럿과 위치가 같다면 출력 위치다.
			if( dwCurOffset == dwSetOffset )
			{
				if( pszStr )
					strcpy( pszStr, strHan );
				return CPoint( dwBegin, 0 + i * dwMaxHeight );// + GetScreenRect().TopLeft();
			}
			dwBegin += size.cx;
		}
#else //_MULTI_LANGUAGE_
		strHan[0] = 0;
		const char* begin = string;
		const char* end = begin + string.GetLength();
		const char* iter = begin;

		while( *iter && iter < end ) {

			WORD wCodePage = m_string.m_awCodePage[dwOffset + (iter-begin)];

			const char* next = CharNextEx(iter, wCodePage );

			SIZE size;

			if( IsWndStyle( EBS_PASSWORD ) ) {
				m_pFont->GetTextExtent("*", &size);
				size.cx *= next-iter;
			} else {
				char temp[16];
				memcpy(temp, iter, next-iter);
				temp[next-iter] = 0;
				m_pFont->GetTextExtent(temp, &size, wCodePage);
			}

			dwCurOffset = dwOffset + (iter-begin);

			// 문장 내에서 캐럿과 위치가 같다면 출력 위치다.
			if( dwCurOffset == dwSetOffset )
			{
				if( pszStr )
					strcpy( pszStr, strHan );
				return CPoint( dwBegin, 0 + i * dwMaxHeight );// + GetScreenRect().TopLeft();
			}
			dwBegin += size.cx;
			iter = next;
		}
#endif //_MULTI_LANGUAGE_
		// 캐럿이 문장 내에 있지 않고 문장의 끝에 새로운 입력을 기다려야하는 위치이다.
		if( dwSetOffset == m_string.GetLength() )
		{
			if( !m_dwOffset || m_string.GetAt( m_dwOffset - 1 ) != '\n' )
			{
				if( pszStr )
					pszStr[0] = 0;
				ptCaret = CPoint( dwBegin, i * dwMaxHeight );// + GetScreenRect().TopLeft();
			}
			else
			{
				if( pszStr )
					pszStr[0] = 0;
				ptCaret = CPoint( 0, ( i + 1 ) * dwMaxHeight );// + GetScreenRect().TopLeft();
			}
		}
	}
	return ptCaret;
}

//dwCurOffset = dwOffset + i2;
//if( dwCurOffset == m_string.GetLength() && dwCurOffset == m_dwOffset )
void CWndText::DrawCaret(C2DRender* p2DRender)
{
}
LONG CWndText::GetOffset(CPoint point)
{
	CString string;
	int dwMaxHeight = GetFontHeight();
	CPoint pt = point;
	pt.y /= dwMaxHeight;
	pt.y += m_wndScrollBar.GetScrollPos();
	DWORD dwOffset1 = m_string.GetLineOffset( pt.y );
	DWORD dwOffset2 = m_string.GetLineOffset( pt.y + 1);
	DWORD dwBegin = 0;
#if !defined(_MULTI_LANGUAGE_)
	TCHAR strHan[3];
	for( int i = dwOffset1; i < dwOffset2; i++ )
	{
		if( m_string[i] == '\r' || m_string[i] == '\n')
			return i;
		if( IsDBCSLeadByte( m_string[i] ) )
		{
			strHan[0] = m_string[i++];
			strHan[1] = m_string[i];
			strHan[2] = '\0';
			dwBegin += m_pFont->GetTextExtent(strHan).cx;
			if(dwBegin > pt.x) 
				return i - 1; 
		}
		else
		{
			strHan[0] = m_string[i];
			strHan[1] = '\0';
			dwBegin += m_pFont->GetTextExtent(strHan).cx;
			if(dwBegin > pt.x) 
				return i; 
		}
	}
	return i;
#else //_MULTI_LANGUAGE_
	const char* begin = m_string;
	const char* end = begin + dwOffset2;
	const char* iter = begin + dwOffset1;

	while(*iter && iter < end) {

		if( *iter == '\r' || *iter == '\n')
		{
			return iter-begin;
		}

		WORD wCodePage = m_string.m_awCodePage[iter-begin];

		const char* next = CharNextEx( iter, wCodePage );

		SIZE size;

		if( IsWndStyle( EBS_PASSWORD ) ) {
			m_pFont->GetTextExtent("*", &size);
			size.cx *= next-iter;
		} else {
			char temp[16];
			memcpy(temp, iter, next-iter);
			temp[next-iter] = 0;
			m_pFont->GetTextExtent(temp, &size, wCodePage);
		}

		if((int)( dwBegin+size.cx ) > pt.x) {

			if(pt.x-dwBegin < (dwBegin+size.cx)-pt.x)
				return iter-begin;
			else 
				return next-begin;
		}

		dwBegin += size.cx;
		iter = next;
	}
	return iter-begin;
#endif //_MULTI_LANGUAGE_
}
void CWndText::OnChar( UINT nChar  )
{
	if( nChar == 3 ) // CTRL-C : copy
	{
		if( m_dwBlockBegin != m_dwBlockEnd )
		{
			DWORD dwBlockBegin;
			DWORD dwBlockEnd;
			if(m_dwBlockBegin > m_dwBlockEnd)
				dwBlockBegin = m_dwBlockEnd, dwBlockEnd = m_dwBlockBegin;
			else
				dwBlockBegin = m_dwBlockBegin, dwBlockEnd = m_dwBlockEnd;
			CString strClipboard;
			strClipboard = m_string.Mid( dwBlockBegin, dwBlockEnd - dwBlockBegin );
			if( m_bEnableClipboard ) 
				CClipboard::SetText( strClipboard );
		}
	}
}
void CWndText::OnLButtonDown( UINT nFlags, CPoint point )
{
	LONG lOffset = GetOffset(point);
	if(lOffset != -1)
		m_dwBlockBegin = m_dwBlockEnd = m_dwOffset = lOffset; 
	m_bLButtonDown = TRUE;
	SetCapture();
	m_TextComp[ 0 ] = 0;
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
	SetCaretPos( ptCaret );
}
void CWndText::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLButtonDown = FALSE;
	ReleaseCapture();
	CWndBase* pWnd = m_pParentWnd;
	pWnd->OnChildNotify( STN_CLICKED, m_nIdWnd, (LRESULT*)this); 
}
void CWndText::OnMouseMove(UINT nFlags, CPoint point)
{
	//return;
	if(m_bLButtonDown == TRUE)
	{
		LONG lOffset = GetOffset(point);
		if(lOffset != -1)
			m_dwBlockEnd = m_dwOffset = lOffset; 
	}
	else
	{
		int dwOffset;
		int dwBlockBegin;
		int dwBlockEnd;
		LONG lOffset = GetOffset( point );
		BYTE chr1, chr2;

		if(lOffset >= 0 && lOffset < m_string.GetLength() ) 
		{
			if( !IsWhite( m_string.GetAt( lOffset ) ) ) 
			{
				dwBlockBegin = lOffset;
				dwBlockEnd = lOffset;

				while( dwBlockBegin > 1 )
				{
					chr1 = m_string.GetAt ( dwBlockBegin - 1 );
					chr2 = m_string.GetAt ( dwBlockBegin - 2 ); 

					if( IsDBCSLeadByte( chr1 ) && IsDBCSLeadByte( chr2 ) )
						dwBlockBegin -= 2;
					else
					if( !IsWhite( chr1 ) )
						dwBlockBegin -= 1;
					else
						break;
				}
				/*
				while(dwBlockEnd < m_string.GetLength() )
				{
					chr1 = m_string.GetAt ( dwBlockEnd );

					if( IsDBCSLeadByte( chr1 ) )
						dwBlockEnd += 2;
					else
					if( !IsWhite( chr1 ) )
						dwBlockEnd += 1;
					else
						break;
				}
				*/
				dwOffset = dwBlockEnd; 

				CString strFind = m_string.Mid( dwBlockBegin, dwBlockEnd - dwBlockBegin );
				/*
				CStringArray* pStrArray = prj.GetHelp( strFind );
				if(pStrArray)
				{
					GET_CLIENT_POINT( m_pApp->GetSafeHwnd(), point);
					CPoint pt1 = OffsetToPoint(dwBlockBegin);
					CPoint pt2 = OffsetToPoint(dwBlockEnd);
					g_toolTip.PutToolTip( 100, pStrArray->GetAt(0), CRect
						(	pt1.x, pt1.y, pt2.x, pt2.y + m_pFont->GetMaxHeight() )
						, point);
				}
				*/
			}
		}
	}
}
void CWndText::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	LONG lOffset = GetOffset(point);
	BYTE chr1;

	if(lOffset >= 0 && lOffset < m_string.GetLength() ) 
	{
		if( !IsWhite( m_string.GetAt( lOffset ) ) ) 
		{
#if !defined(_MULTI_LANGUAGE_)
			m_dwBlockBegin = m_dwBlockEnd = lOffset;

			while(m_dwBlockBegin > 0)
			{
				chr1 = m_string.GetAt ( m_dwBlockBegin - 1 );
				//chr2 = m_string.GetAt ( m_dwBlockBegin - 2 ); 

				//if( IsDBCSLeadByte( chr1 ) && IsDBCSLeadByte( chr2 ) )
				//	m_dwBlockBegin -= 2;
				//else
				if( IsWhite( chr1 ) )
					break;
				else
					m_dwBlockBegin -= 1;
				//else
				//	break;
			}
			while(m_dwBlockEnd < m_string.GetLength() )
			{
				chr1 = m_string.GetAt ( m_dwBlockEnd );

				if( IsDBCSLeadByte( chr1 ) )
					m_dwBlockEnd += 2;
				else
				if( !IsWhite( chr1 ) )
					m_dwBlockEnd += 1;
				else
					break;
			}
#else //_MULTI_LANGUAGE_
			chr1 = 0;
			const char* begin = m_string;
			const char* end = begin + m_string.GetLength();
			const char* iter = begin;

			m_dwBlockBegin = 0;

			// Get m_dwBlockBegin
			while(*iter && iter < end) {

				if( IsWhite( *iter ) )
					m_dwBlockBegin = iter - begin;

				WORD wCodePage = m_string.m_awCodePage[iter-begin];

				iter = CharNextEx( iter, wCodePage );
				if(iter - begin > lOffset) break;
			}

			m_dwBlockEnd = m_dwBlockBegin;

			while(*iter && iter < end) {

				if( IsWhite( *iter ) ) {
					m_dwBlockEnd = iter - begin;
					break;
				}

				WORD wCodePage = m_string.m_awCodePage[iter-begin];

				iter = CharNextEx( iter, wCodePage );
				if(iter - begin > lOffset) break;
			}
#endif //_MULTI_LANGUAGE_
			m_dwOffset = m_dwBlockEnd;
		}
	}
}
#ifdef __Y0602
BOOL CWndText::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	if( zDelta < 0 )
	{
		if( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() > m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()+1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() );
	}
	else
	{
		if( m_wndScrollBar.GetMinScrollPos() < m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()-1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMinScrollPos() );
	}
	
	return TRUE;
}
#endif

void CWndText::OnSetFocus(CWndBase* pOldWnd)
{
	CWndBase::OnSetFocus(pOldWnd);
}
void CWndText::OnKillFocus(CWndBase* pNewWnd)
{
	m_dwBlockBegin = m_dwBlockEnd = m_dwOffset;
	CWndBase::OnKillFocus( pNewWnd );
}
void CWndText::SetString( LPCTSTR pszString, DWORD dwColor )
{
	// Init의 목적은 Align이 아니라 m_pfont와 Rect세팅이다.
	m_string.Init( m_pFont, &GetClientRect() );
	// 이 내부에서 Align을 한다.
	m_string.SetParsingString( pszString, dwColor );
	if( IsWndStyle( WBS_VSCROLL ) )
		UpdateScrollBar();
	m_dwOffset = m_string.GetLength();
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
	SetCaretPos( ptCaret );
	
	//m_dwBlockBegin = m_dwBlockEnd = m_dwOffset = 0;
	//SetCaretPos( CPoint(0,0) );
	//m_string.Reset( m_pFont, &GetClientRect() );
}
void CWndText::AddString(LPCTSTR pszString, DWORD dwColor)
{
	int nLine = m_string.GetLineCount() - 1;
	m_string.AddParsingString( pszString, dwColor );//+= pszString;
	//m_string.Align( m_pFont, nLine );
	UpdateScrollBar();
	m_dwOffset = m_string.GetLength();
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
	SetCaretPos( ptCaret );
}
void CWndText::ResetString()
{
	//m_string.Reset( m_pFont, &GetClientRect() );
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
	SetCaretPos( ptCaret );
	UpdateScrollBar();
}


////////////////////////////


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
		delete pColumn->pszText;
		delete pColumn;
	}
	for( int i = 0; i < m_aItems.GetSize(); i++ )
	{
		pItems = (LVITEM*)m_aItems.GetAt ( i );
		for( int j = 0; j < m_aColumns.GetSize(); j++ )
		{
			if( pItems[ j ].pszText )
				delete pItems[ j ].pszText;
		}
		delete pItems;
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
#ifndef __V050322_CACHEQUEST
//{{AFX
void CWndListCtrl::AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface )
{
	LPIMAGE lpImage[12];
	CSize sizeBmp[12];
	CString m_strTile = "WndEditTile06.tga";
	CString strFileName;
	
	int nImgBit = IMGBIT_32;
	for( int i = 0; i < 9; i++ )
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
	}
	///////////////////////////////////////////////////////
	CRect rect = GetWindowRect( TRUE );
	rect += ( m_pParentWnd->GetClientRect( TRUE ).TopLeft() - m_pParentWnd->GetWindowRect( TRUE ).TopLeft() );
	//ect = GetClientRect();
	rect.right -= 15;
	int nWidth = rect.Width() / 8;
	int nHeight = rect.Height() / EDIT_HEIGHT;
	CPoint point;
	for( i = 0; i < nHeight; i++ )
	{
		for( int j = 0; j < nWidth; j++ )
		{
			point = CPoint( j * 8, i * EDIT_HEIGHT ) + rect.TopLeft();
			if( i == 0 )
			{
				if( j == 0 )
					PaintTexture( pDest, lpImage[ 0 ], point, sizeSurface );
				else
					if( j == nWidth - 1 )
						PaintTexture( pDest, lpImage[ 2 ], point, sizeSurface );
					else
						PaintTexture( pDest, lpImage[ 1 ], point, sizeSurface );
			}
			else
				if( i == nHeight - 1 )
				{
					if( j == 0 )
						PaintTexture( pDest, lpImage[ 6 ], point, sizeSurface );
					else
						if( j == nWidth - 1 )
							PaintTexture( pDest, lpImage[ 8 ], point, sizeSurface );
						else
							PaintTexture( pDest, lpImage[ 7 ], point, sizeSurface );					
				}
				else
				{
					if( j == 0 )
						PaintTexture( pDest, lpImage[ 3 ], point, sizeSurface );
					else
						if( j == nWidth - 1 )
							PaintTexture( pDest, lpImage[ 5 ], point, sizeSurface );
						else
							PaintTexture( pDest, lpImage[ 4 ], point, sizeSurface );					
				}
		}
	}
}
//}}AFX
#endif // __V050322_CACHEQUEST

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

#ifdef __V050322_CACHEQUEST
		if(	IsWndStyle( WBS_VSCROLL ) )  
		{
			m_wndScrollBar.SetVisible( TRUE );
			m_wndScrollBar.SetScrollRange( 0, nRange );
			m_wndScrollBar.SetScrollPage( nPage );
		}
		else
			m_wndScrollBar.SetVisible( FALSE );

#else // __V050322_CACHEQUEST
//{{AFX
		if(nRange - nPage <= 0)
			m_wndScrollBar.SetVisible(FALSE);
		else
		{
			m_wndScrollBar.SetVisible( TRUE );
			m_wndScrollBar.SetScrollRange( 0, nRange );
			m_wndScrollBar.SetScrollPage( nPage );
		}
//}}AFX
#endif // __V050322_CACHEQUEST
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
//	m_string.Reset( m_pFont, &GetClientRect() );

	CRect rect = GetWindowRect();
#ifdef __V050322_CACHEQUEST
	if( IsWndStyle( WBS_VSCROLL ) ) 
		rect.left = rect.right - VSCROLL_WIDTH;
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
#else // __V050322_CACHEQUEST
//{{AFX
	rect.left = rect.right - 15;
//}}AFX
#endif // __V050322_CACHEQUEST
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
			delete pItems[ pItem->iSubItem ].pszText;
		memcpy( &pItems[ pItem->iSubItem ], pItem, sizeof( LVITEM ) );
		pItems[ pItem->iSubItem ].pszText = new _TCHAR[ _tcslen( pItem->pszText ) + sizeof( _TCHAR ) ];
		_tcscpy( pItems[ pItem->iSubItem ].pszText, pItem->pszText );
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
	pNewItems[ pItem->iSubItem ].pszText = new _TCHAR[ _tcslen( pItem->pszText ) + sizeof( _TCHAR ) ];
	_tcscpy( pNewItems[ pItem->iSubItem ].pszText, pItem->pszText );
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
	pNewColumn->pszText = new _TCHAR[ _tcslen( pColumn->pszText ) + sizeof( _TCHAR ) ];
	_tcscpy( pNewColumn->pszText, pColumn->pszText );
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

//////////////////////////////////////////////////////////////////////////////
// CWndComboBox
//////////////////////////////////////////////////////////////////////////////

CWndComboBox::CWndComboBox()
{
//	m_dwComboBoxStyle = 0; 
	m_byWndType = WTYPE_COMBOBOX;
}
CWndComboBox::~CWndComboBox()
{
}
void CWndComboBox::OnInitialUpdate()
{
	CRect rect = GetWindowRect();
	rect.left = rect.right - 20;
	m_wndButton.AddWndStyle( WBS_DOCKING );
	m_wndButton.Create( _T( "V" ), WBS_CHILD, rect, this, 0 );
	m_wndButton.m_pTexture = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(),"ButtQuickListDn.tga" ), 0xffff00ff, TRUE );
	rect = GetWindowRect();
	ClientToScreen( &rect );
	rect.top = rect.bottom;
	rect.bottom += 100;
	//m_wndListBox.Create( 0, rect, m_pWndRoot, 1 );
#ifdef __V050322_CACHEQUEST
	m_wndListBox.CWndBase::Create( WBS_POPUP | WBS_VSCROLL, rect, this, 1 );
	m_wndListBox.m_strTitle = _T( "Combo ListBox" );
	m_wndListBox.m_strTexture = m_strTexture;
	m_wndListBox.m_bTile = m_bTile;
	m_wndListBox.AdjustWndBase();
	m_wndListBox.RestoreDeviceObjects();
#else //  __V050322_CACHEQUEST
	m_wndListBox.CWndBase::Create( WBS_POPUP, rect, this, 1 );
#endif
	m_wndListBox.SetVisible( FALSE );	

}	

void CWndComboBox::Create( DWORD dwComboBoxStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID, int nR, int nG, int nB )
{
#ifdef __DAISY
	dwComboBoxStyle = dwComboBoxStyle;// & ~WBS_NODRAWFRAME;
#endif
	CWndEdit::Create( pParentWnd->m_pApp->GetSafeHwnd(), dwComboBoxStyle | WBS_CHILD, rect, pParentWnd, nID, nR, nG, nB );
}


void CWndComboBox::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	CWndEdit::SetWndRect( rectWnd, bOnSize );
}

void CWndComboBox::PaintFrame( C2DRender* p2DRender )
{
	CWndEdit::PaintFrame( p2DRender);
}

void CWndComboBox::OnDraw( C2DRender* p2DRender )
{
	CWndEdit::OnDraw( p2DRender);
}

void CWndComboBox::OnLButtonDown( UINT nFlags, CPoint point )
{
}

void CWndComboBox::OnLButtonUp( UINT nFlags, CPoint point )
{
}
BOOL CWndComboBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( nID == 0 && message == WNM_CLICKED) // list
	{
		CRect rect = GetScreenRect();//GetWindowRect( );
	//	ClientToScreen( &rect );
		rect.top = rect.bottom;
		rect.bottom += 200;

		m_wndListBox.Move( rect.TopLeft() );
		m_wndListBox.SetVisible( TRUE );
#ifdef __V050322_CACHEQUEST
		m_wndListBox.m_bTile = m_bTile;
		m_wndListBox.AdjustWndBase();
#endif
		m_wndListBox.SetFocus();
		/*
		DWORD dwIdApplet = m_wndList.GetItemData( m_wndList.GetCurSel());
		SHORTCUT shortcut;
		shortcut.m_dwType = SHORTCUT_APPLET;
		shortcut.m_dwIndex = dwIdApplet;
		g_WndMng.ObjectExecutor( &shortcut );//SHORTCUT_APPLET, dwIdApplet);
		*/
	}
	if( nID == 1 && message == WNM_SELCHANGE) // list
	{
		CString string;
		m_wndListBox.GetText( m_wndListBox.GetCurSel(), string );
		SetString( string );
		m_wndListBox.SetVisible( FALSE );
		CWndBase* pWnd = m_pParentWnd;
		pWnd->OnChildNotify( WNM_SELCHANGE, m_nIdWnd, (LRESULT*)this); 
	}
	return TRUE;
}
// manipulating listbox items
int CWndComboBox::AddString( LPCTSTR lpszString )
{
	int nNum = m_wndListBox.AddString( lpszString );
	CRect rect = m_wndListBox.GetWindowRect( TRUE );
	rect.bottom = rect.top + ( ( nNum + 1 ) * ( m_pFont->GetMaxHeight() + 3 ) ) + 8;
	m_wndListBox.SetWndRect( rect );
	return nNum;
}
int CWndComboBox::DeleteString( UINT nIndex )
{
	return m_wndListBox.DeleteString( nIndex );
}
int CWndComboBox::InsertString( int nIndex, LPCTSTR lpszString )
{
	return 1;
}
void CWndComboBox::ResetContent()
{
	m_wndListBox.ResetContent();
}
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
	CString string;
	m_wndListBox.GetText( nSelect, string );
	SetString( string );
	return m_wndListBox.SetCurSel( nSelect );
}


















