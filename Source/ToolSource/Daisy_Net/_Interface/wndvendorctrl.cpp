#include "stdafx.h"
#include "AppDefine.h"
#include "wndvendorctrl.h"
#include "wndvendor.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;

#include "WndManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif	// _DEBUG


CTextureMng	CWndVendorCtrl::m_textureMng;

CWndVendorCtrl::CWndVendorCtrl()
{
	m_nWndColor		= D3DCOLOR_TEMP( 255,  0x5 << 3,  0x5 << 3,  0x5 << 3 );
	m_nFontColor	= D3DCOLOR_TEMP( 255, 255, 255, 255 );
	m_nSelectColor	= D3DCOLOR_TEMP( 255, 255, 255, 0   );
	m_nCurSel	= -1;
	m_pFocusItem	= NULL;
	m_pMover    = NULL;
	m_bDrag		= FALSE;
}

void CWndVendorCtrl::Create( DWORD dwListCtrlStyle, RECT & rect, CWndBase* pParentWnd, UINT nID )
{
	m_dwListCtrlStyle	= dwListCtrlStyle;
	CWndBase::Create( m_dwListCtrlStyle | WBS_CHILD, rect, pParentWnd, nID );
}

void CWndVendorCtrl::InitVendor( CMover* pMover )
{
	m_pMover	 = pMover;
	m_pFocusItem = NULL;
}

CItemBase* CWndVendorCtrl::GetItem( BYTE i )
{
	ASSERT( m_pMover );
	return m_pMover->m_vtInfo.GetItem( i );
}

void CWndVendorCtrl::OnInitialUpdate( void )
{
	CRect rect	= GetWindowRect();
	rect.left	= rect.right - 15;
	
#ifdef __YNEWPERSONSHOP_EX1026
	m_pTex = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndPosMark.tga" ), 0xffff00ff );
#endif //__YNEWPERSONSHOP_EX1026	
}
	
void CWndVendorCtrl::OnDraw( C2DRender* p2DRender )
{
	if( m_pMover == NULL )
		return;

	CRect rect	= GetClientRect();
	int nWidth	= rect.Width() / 32;
	int nHeight		= rect.Height() / 32;

	for( int i = 0; i < MAX_VENDITEM; i++ )
	{
		int x	= i % 6;
		int y	= i / 6;
		//p2DRender->RenderRect( CRect( x * 32 + 7, y * 32 + 11, x * 32 + 32 + 5, y * 32 + 32 + 9 ), 0xff00ffff );
		CRect rectHittest	= CRect( x * 32 + 7, y * 32 + 11, x * 32 + 32 + 5, y * 32 + 32 + 9 );

		CPoint point	= GetMousePoint();

		if( rectHittest.PtInRect( point ) )
		{
			if( CWndBase::m_GlobalShortcut.m_dwData )
			{
				m_nCurSel = -1;
#ifdef __YNEWPERSONSHOP_EX1026
				CPoint ptx = CPoint(x * 32 + 7, y * 32 + 11);

				{
					m_pTex->Render( p2DRender, ptx );
				}

#else //__YNEWPERSONSHOP_EX1026
				p2DRender->RenderRect( CRect( x * 32 + 7, y * 32 + 11, x * 32 + 32 + 5, y * 32 + 32 + 9 ), 0xff00ffff );
#endif //__YNEWPERSONSHOP_EX1026
			}
		}			
		CItemBase* pItemBase = GetItem( i );
		if( pItemBase )
		{
				#ifdef __FINITEITEM0705
				if( ((CItemElem*)pItemBase)->IsFlag( CItemElem::expired ) )
				{
					pItemBase->GetTexture()->Render2( p2DRender, CPoint( x * 32 + 6, y * 32 + 10 ), D3DCOLOR_XRGB( 255, 100, 100 ) );
				}
				else
				#endif // __FINITEITEM0705
				{
					pItemBase->GetTexture()->Render( p2DRender, CPoint( x * 32 + 6, y * 32 + 10 ) );
				}

			CPoint point	= GetMousePoint();
			if( rectHittest.PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &rectHittest );
				g_WndMng.PutToolTip_Item( pItemBase, point2, &rectHittest, APP_VENDOR );
			}
			if( i == m_nCurSel )
				p2DRender->RenderRect( CRect( x * 32 + 7, y * 32 + 11, x * 32 + 32 + 5, y * 32 + 32 + 9 ), 0xff00ffff );
			CItemElem* pItemElem	= (CItemElem*)pItemBase;
			if( pItemElem->GetProp()->dwPackMax > 1 )
			{
				short nItemNum	= pItemBase->GetExtra();
				TCHAR szTemp[32];
				_stprintf( szTemp, "%d", nItemNum );
				CSize size	= p2DRender->m_pFont->GetTextExtent( szTemp );
				p2DRender->TextOut( x * 32 + 32 - size.cx+5, y * 32 + 32 - size.cy+12, szTemp, 0xff1010ff );
			}
		}
	}
}

void CWndVendorCtrl::OnMouseMove( UINT nFlags, CPoint point )
{
	if( m_bDrag == FALSE )
		return;
	m_bDrag		= FALSE;
	CPoint pt( 3, 3 );
	CRect rect;

	CItemBase* pItemBase	= m_pFocusItem;
	if( IsUsingItem( pItemBase ) )
	{
		m_GlobalShortcut.m_pFromWnd		= this;
		m_GlobalShortcut.m_dwShortcut	= SHORTCUT_ITEM;
		m_GlobalShortcut.m_dwIndex	= m_nCurSel;
		m_GlobalShortcut.m_dwType	= 0;
		m_GlobalShortcut.m_dwId		= pItemBase->m_dwObjId;
		m_GlobalShortcut.m_pTexture		= pItemBase->GetTexture();
		m_GlobalShortcut.m_dwData	= (DWORD)pItemBase;
		_tcscpy( m_GlobalShortcut.m_szString, pItemBase->GetProp()->szName );
	}
}

BOOL CWndVendorCtrl::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	int nDstId	= -1;
	CRect rect	= GetClientRect();
	int nWidth	= rect.Width() / 32;
	int nHeight		= rect.Height() / 32;
	CPoint pt( 3, 3 );
	if( pShortcut->m_dwType == ITYPE_ITEM && pShortcut->m_dwData == 0 )
	{
//		CWndBase* pParent	= (CWndBase*)GetParentWnd();
//		pParent->OnChildNotify( WIN_ITEMDROP, m_nIdWnd, (LRESULT*)pShortcut );
		return TRUE;
	}
	else
	{
		CItemBase* pItemBase = g_pPlayer->GetItemId( pShortcut->m_dwId );
		if( pItemBase )
		{
			for( int i = 0; i < MAX_VENDITEM; i++ )
			{
				int x	= i % 6;
				int y	= i / 6;
				rect.SetRect( x * 32 + 7, y * 32 + 11, x * 32 + 32 + 5, y * 32 + 32 + 9 );
				
				if( rect.PtInRect( point ) && GetItem( i ) == NULL )
				{
					pShortcut->m_dwData		= i + 100;
					CWndBase* pParent	= (CWndBase*)GetParentWnd();
					pParent->OnChildNotify( 100, m_nIdWnd, (LRESULT*)pShortcut );
				}
			}
			return TRUE;
		}
	}
	return TRUE;
}

void CWndVendorCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	if( m_pMover == g_pPlayer )
		return;

	if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 )
		return;

	if( CWndBase::m_GlobalShortcut.IsEmpty() == FALSE )
		return;
	int nItem	= HitTest( point );
	if( nItem == -1 )
		return;

	CItemBase* pItemBase	= GetItem( nItem );
	if( pItemBase )
	{
		m_pFocusItem	= pItemBase;
		m_nCurSel	= nItem;
		m_bDrag		= TRUE;
	}
}

void CWndVendorCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
	m_bDrag		= FALSE;
	int nItem	= HitTest( point );
	if( nItem == -1 )
		return;
	CItemBase* pItemBase	= GetItem( nItem );
	if( pItemBase )
	{
		if( ( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 ) )
		{
			m_nCurSel	= nItem;
			m_pFocusItem	= pItemBase;
		}
	}
}

void CWndVendorCtrl::OnRButtonUp( UINT nFlags, CPoint point )
{
	int nItem	= HitTest( point );
	if( nItem == -1 )
		return;

	CItemBase* pItemBase	= GetItem( nItem );
	if( pItemBase )
	{
		{
			m_nCurSel	= nItem;
			m_pFocusItem	= pItemBase;
		}
	}

#ifdef __ONLINE
	g_DPlay.SendUnregisterPVendorItem( nItem );
#endif //__ONLINE	
}

void CWndVendorCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{

}

void CWndVendorCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if( m_pMover == g_pPlayer )
		return;

	int nItem	= HitTest( point );
	if( nItem == -1 )
		return;

	CItemBase* pItemBase	= GetItem( nItem );
	if( pItemBase )
	{
		m_pFocusItem	= pItemBase;
		m_nCurSel	= nItem;
		if( m_pFocusItem )
		{
			CWndVendor* pWndVendor	= (CWndVendor*)GetParentWnd();
			SAFE_DELETE( pWndVendor->m_pWndVendorBuy );
			pWndVendor->m_pWndVendorBuy		= new CWndVendorBuy( pItemBase, nItem );
			pWndVendor->m_pWndVendorBuy->Initialize( pWndVendor, APP_VENDOR_BUY );
//			pParent->OnChildNotify( WIN_DBLCLK, m_nIdWnd, (LRESULT*)m_pFocusItem );
		}
	}
}

void CWndVendorCtrl::OnRButtonDblClk( UINT nFlags, CPoint point )
{

}

void CWndVendorCtrl::OnSize( UINT nType, int cx, int cy )
{
	CWndBase::OnSize( nType, cx, cy );
}

void CWndVendorCtrl::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow	= rectWnd;
	m_rectClient	= m_rectWindow;
//	m_rectClient.DeflateRect( 3, 3 );

	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}

void CWndVendorCtrl::PaintFrame( C2DRender* p2DRender )
{
	CRect rect	= GetWindowRect();

	DWORD dwColor1	= D3DCOLOR_ARGB( 100, 0, 0,  0 );
	DWORD dwColor2	= D3DCOLOR_ARGB( 255, 240, 240,  240 );
	DWORD dwColor3	= D3DCOLOR_ARGB( 100, 200, 200,  200 );

	p2DRender->RenderFillRect ( rect, dwColor1 );
	p2DRender->RenderRoundRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor3 );
}

BOOL CWndVendorCtrl::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}

int CWndVendorCtrl::HitTest( CPoint point )
{
	CRect rect	= GetClientRect();
	int nWidth	= rect.Width() / 32;
	int nHeight		= rect.Height() / 32;

	for( int i = 0; i < MAX_VENDITEM; i++ )
	{
		int x	= i % 6;
		int y	= i / 6;
		rect.SetRect( x * 32 + 7, y * 32 + 11, x * 32 + 32 + 5, y * 32 + 32 + 9 );
		
		if( rect.PtInRect( point ) )
			return i;
	}
	return -1;
}

