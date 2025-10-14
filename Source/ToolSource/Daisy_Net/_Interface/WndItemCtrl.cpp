// WndBase.cpp: implementation of the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AppDefine.h"
#include "DefineText.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;

#include "WndManager.h"

#ifdef __V050322_CACHEQUEST
#include "WndText.h"
#endif // __V050322_CACHEQUEST

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndItemCtrl
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CTextureMng CWndItemCtrl::m_textureMng;

CWndItemCtrl::CWndItemCtrl() 
{
	m_nWndColor    = D3DCOLOR_TEMP( 255,  0x5 << 3,  0x5 << 3,  0x5 << 3 );
	m_nFontColor   = D3DCOLOR_TEMP( 255, 255, 255, 255 );
	m_nSelectColor = D3DCOLOR_TEMP( 255, 255, 255, 0   );
	m_nCurSelect = -1;
	m_pFocusItem = NULL;
	m_pItemContainer = NULL;
	m_bDrag = FALSE;
	m_nOnSelect = -1;

#ifdef __Y0402
	memset( m_pArrayItemBase, 0, sizeof(CItemBase*)*100 );
	m_nArrayCount = 0;
#endif
	m_bVisibleCount = TRUE;
}
CWndItemCtrl::~CWndItemCtrl()
{
//	ResetContent();
}
void CWndItemCtrl::Create( DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID )
{
	CWndBase::Create( WBS_CHILD | dwListCtrlStyle, rect, pParentWnd, nID );
}
void CWndItemCtrl::InitItem( CItemContainer<CItemElem>* pItemContainer, DWORD SendToId )
{
	m_pItemContainer = pItemContainer;
	m_pFocusItem	= NULL;
	m_dwFrame = SendToId;
	
	TCHAR	szTitle[3][10] = {_T("아이템"), _T("갯수"), _T("최소")};
	
//	LV_COLUMN  pColumn;
//	LV_ITEM	   lvitem;
//	TCHAR      strNum[32];
	//
	// Report Column을 입력 
	//
	/*
	for(int i = 0;  i < 3; i++)
	{
		pColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		pColumn.fmt = LVCFMT_LEFT;
		pColumn.iSubItem = i;
		if( i == 0 )
			pColumn.cx = 120;  // SubItem is twice as large
		else
			pColumn.cx = 30;  // SubItem is twice as large
		pColumn.pszText = szTitle[ i ];	
		InsertColumn( i, &pColumn );  // assumes return value is OK.
	}
	*/
#ifndef __ONLINE
	//
	// 소스 아이템을 입력
	//
	for( int i = 0; i < m_pItemContainer->m_dwIndexNum; i++ ) 
	{
		CItemElem* pItemElem = &m_pItemContainer->m_apItem[ i ] ;
		//pItemElem->SetTexture();
		//pItemElem->m_pTexture = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemElem->GetProp()->szIcon), 0xff000000 );
	}
#endif
//	SetWndRect( GetClientRect() );
}

void CWndItemCtrl::OnInitialUpdate()
{
	//CSize size = m_pSprPack->GetAt(13)->GetSize();
	//CRect rect = GetClientRect();
	//rect.left = rect.right - size.cx;
	CRect rect = GetWindowRect();
	//rect.right += 5;
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );//,m_pSprPack,-1);
}
void CWndItemCtrl::OnMouseWndSurface( CPoint point )
{
	if( !m_pItemContainer )
		return;

	CRect rect = GetClientRect();
	int nWidth = rect.Width() / 32;
	int nHeight = rect.Height() / 32;

	// 스크롤바 관련   
	CPoint pt;
	pt.y = 0;
	pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

#ifdef __Y0402
	CWndBase* pWndBase = this->GetParentWnd();
	
	int nParent;
	if( pWndBase )
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if( pWndBaseTemp )
			nParent = pWndBaseTemp->GetWndId();
		else
			return;
	}
	else
	{
		return;
	}

	if( nParent == APP_INVENTORY )
	{
		for( int i = pt.y; i < m_pItemContainer->GetSize(); i++ ) 
		{
			if( i < 0 ) continue;
			int x = ( i - pt.y ) % nWidth;
			int y = ( i - pt.y ) / nWidth;
			CItemBase* pItemBase = GetItem( i );
			if( pItemBase == NULL ) continue;
			CRect rectHittest = CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 );

			if( rectHittest.PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &rectHittest );
				
				if( m_dwFrame == APP_SHOP_ )		// 상점
					g_WndMng.PutToolTip_Item( pItemBase, point2, &rectHittest, APP_SHOP_ );
				else 
				if( m_dwFrame == APP_INVENTORY || m_dwFrame == APP_BANK )	// 나의 인벤토리
				{
					if( g_WndMng.GetWndBase( APP_REPAIR ) )
					{
						g_WndMng.PutToolTip_Item( pItemBase, point2, &rectHittest, APP_REPAIR );		
					}
					else
					{
						g_WndMng.PutToolTip_Item( pItemBase, point2, &rectHittest, APP_INVENTORY );		
					}
				}

				m_nOnSelect = i;
			}
		}
	}
	else
	{
		for( int i = 0; i < m_nArrayCount; i++ ) 
		{
			if( i < 0 ) continue;
			int x = ( i - pt.y ) % nWidth;
			int y = ( i - pt.y ) / nWidth;
			CItemBase* pItemBase = m_pArrayItemBase[i];
			if( pItemBase == NULL ) continue;
			CRect rectHittest = CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 );

			if( rectHittest.PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &rectHittest );
				
				if( m_dwFrame == APP_SHOP_ )		// 상점
					g_WndMng.PutToolTip_Item( pItemBase, point2, &rectHittest, APP_SHOP_ );
				else 
				if( m_dwFrame == APP_INVENTORY || m_dwFrame == APP_BANK
				 || m_dwFrame == APP_GUILD_BANK
				 || m_dwFrame == APP_COMMON_BANK
					)	// 나의 인벤토리
				{
					if( g_WndMng.GetWndBase( APP_REPAIR ) )
					{
						g_WndMng.PutToolTip_Item( pItemBase, point2, &rectHittest, APP_REPAIR );		
					}
					else
					{
						g_WndMng.PutToolTip_Item( pItemBase, point2, &rectHittest, APP_INVENTORY );		
					}
				}

				m_nOnSelect = i;
			}
		}
	}

#else // __Y0402

	for( int i = pt.y; i < m_pItemContainer->GetSize(); i++ ) 
	{
		if( i < 0 ) continue;
		int x = ( i - pt.y ) % nWidth;
		int y = ( i - pt.y ) / nWidth;
		CItemBase* pItemBase = GetItem( i );
		if( pItemBase == NULL ) continue;
		CRect rectHittest = CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 );

		if( rectHittest.PtInRect( point ) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &rectHittest );
			
			if( m_dwFrame == APP_SHOP_ )		// 상점
				g_WndMng.PutToolTip_Item( pItemBase, point2, &rectHittest, APP_SHOP_ );
			else 
			if( m_dwFrame == APP_INVENTORY || m_dwFrame == APP_BANK
			 || m_dwFrame == APP_GUILD_BANK
			)	// 나의 인벤토리
			{
				if( g_WndMng.GetWndBase( APP_REPAIR ) )
				{
					g_WndMng.PutToolTip_Item( pItemBase, point2, &rectHittest, APP_REPAIR );		
				}
				else
				{
					g_WndMng.PutToolTip_Item( pItemBase, point2, &rectHittest, APP_INVENTORY );		
				}
			}
			m_nOnSelect = i;
		}
	}
#endif // __Y0402

}

void CWndItemCtrl::RanderIcon( C2DRender* p2DRender, CItemBase* pItemBase, int x, int y, int nParent, int nalpha ) 
{
	int bRander = 0;

	if( nParent == APP_SHOP_ )
	{
		ItemProp* pItemProp		= pItemBase->GetProp();
		if( pItemProp == NULL ) return;

		switch( pItemProp->dwItemKind2 )
		{
			case IK2_WEAPON_DIRECT:
//			case IK2_WEAPON_RANGE:
			case IK2_WEAPON_MAGIC:
//			case IK2_WEAPON_DOLL:
//			case IK2_WEAPON_GUN:
			case IK2_ARMOR:
			case IK2_ARMORETC:
			case IK2_CLOTH:
			case IK2_CLOTHETC:
			{
					
				// 무기 : 직업, 능력치 스텟, 가격
				if( pItemProp->dwItemSex != 0xffffffff && pItemProp->dwItemSex != g_pPlayer->GetSex() )
				{
					bRander = 1;
					break;
				}

				if( JOB_VAGRANT != pItemProp->dwItemJob )
				{
					if( pItemProp->dwItemJob != 0xffffffff && JOB_VAGRANT < pItemProp->dwItemJob && !(g_pPlayer->IsInteriorityJob( pItemProp->dwItemJob ) ) )
					{
						bRander = 1;
						break;
					}
				}
				if( pItemProp->dwLimitLevel1 != 0xffffffff && g_pPlayer->GetLevel() < pItemProp->dwLimitLevel1 )
				{
					bRander = 1;
					break;
				}
				break;
			}
			case IK2_RIDING:
//			case IK2_BOARDING:
//			case IK2_HATCH:
			{
				// 비행 : 비행레벨
				if( g_pPlayer->m_nFlightLv < pItemProp->dwFlightLimit )
				{
					bRander = 1;
					break;
				}
				break;
			}
		}
 
		// 모든아이템 가격 비교
		if( bRander == 0 && g_pPlayer->GetGold() < (int)pItemProp->dwCost )
			bRander = 1;
	}

#ifdef __FINITEITEM0705
		if( ((CItemElem*)pItemBase)->IsFlag( CItemElem::expired ) )
		{
			bRander = 1;
		}
#endif // __FINITEITEM0705




#ifndef __SCHOOL0701
	if( g_Option.m_nLanguage == LANG_CHI || g_Option.m_nLanguage == LANG_TWN )
	{
		pItemBase->m_pTexture->Render( p2DRender, CPoint( x, y ), nalpha );
	}
	else
	{
		if( bRander == 0 )
		{
			pItemBase->m_pTexture->Render( p2DRender, CPoint( x, y ), nalpha );
		}
		else
		if( bRander == 1 )	// 사용할수 없는것
		{
			pItemBase->m_pTexture->Render2( p2DRender, CPoint( x, y ), D3DCOLOR_ARGB( nalpha, 255, 100, 100 ) );
		}
	}
#else	// __SCHOOL0701
	if( !g_eLocal.GetState(EVE_SCHOOL) )
	{
		if( bRander == 0 )
		{
			pItemBase->GetTexture()->Render( p2DRender, CPoint( x, y ), nalpha );
		}
		else
		if( bRander == 1 )	// 사용할수 없는것
		{
			pItemBase->GetTexture()->Render2( p2DRender, CPoint( x, y ), D3DCOLOR_ARGB( nalpha, 255, 100, 100 ) );
		}
	}
	else
		pItemBase->GetTexture()->Render( p2DRender, CPoint( x, y ), nalpha );
#endif	// __SCHOOL0701
}

void CWndItemCtrl::OnDraw(C2DRender* p2DRender) 
{
	if( !m_pItemContainer )
		return;

	DWORD dwCur = GetTickCount();
	CPoint pt( 3, 3 );

	CWndBase* pWndBase = this->GetParentWnd();

	int nParent;
	if( pWndBase )
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if( pWndBaseTemp )
			nParent = pWndBaseTemp->GetWndId();
		else
			return;
	}
	else
	{
		return;
	}

	//
	// 리포트 출력 
	//
	if( 0 ) //m_dwListCtrlStyle == WLVS_REPORT )
	{
		m_nFontHeight = GetFontHeight( );
		pt.y -= ( m_nFontHeight + 3 ) * m_wndScrollBar.GetScrollPos();
		for( int i = 0; i < m_pItemContainer->m_dwIndexNum; i++ ) 
		{
			//LVITEM* pItems = (LVITEM*)m_aItems.GetAt( m_pItemContainer->m_apIndex[ i ] );
			DWORD dwColor = 0xffffffff;
			if( i == m_nCurSelect )
				dwColor = 0xff00ff00; 
			CItemBase* pItemElem = &m_pItemContainer->m_apItem[ i ] ;
			if(  pItemElem->m_dwItemId )
			{
				for( int i2 = 0, x = 0; i2 < m_aColumns.GetSize(); i2++ ) 
				{
					switch( i2 )
					{
					case 0:
						p2DRender->TextOut( x + 2, pt.y, prj.GetItemProp( pItemElem->m_dwItemId )->szName, dwColor ); 
						break;
					case 1:
						break;
					case 2:
						break;
					}
					LVCOLUMN* pColumn = (LVCOLUMN*)m_aColumns.GetAt( i2 );
					x += pColumn->cx + 7;
				}
			}
			pt.y += m_nFontHeight + 3;
		}
	}
	//
	// 아이콘 출력
	// CCtrllist
	if( 1 ) //m_dwListCtrlStyle == WLVS_ICON )
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 32;
		int nHeight = rect.Height() / 32;

		// 스크롤바 관련   
		int nPage = nHeight;
		int nRange = m_pItemContainer->m_dwIndexNum / nWidth;// - nPage;
		if( m_pItemContainer->m_dwIndexNum % nWidth )
			nRange++;
		m_wndScrollBar.SetScrollRange( 0, nRange );
		m_wndScrollBar.SetScrollPage( nPage );

		pt.y = 0;
		pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

#ifdef __Y0402
		if( nParent == APP_INVENTORY )
		{
			for( int i = pt.y; i < m_pItemContainer->GetSize(); i++ ) 
			{
				if( i < 0 ) continue;
				int x = ( i - pt.y ) % nWidth;
				int y = ( i - pt.y ) / nWidth;
				CItemBase* pItemBase = GetItem( i );
				
				if( pItemBase == NULL ) continue;

				if( pItemBase->GetTexture() )
				{
					if( IsUsingItem( pItemBase ) || ((CItemElem*)pItemBase)->m_bRepair )
						RanderIcon( p2DRender, pItemBase, x * 32, y * 32, nParent, 60 );
					else if( m_nOnSelect == i )
					{
						pItemBase->GetTexture()->m_size.cx += 4;
						pItemBase->GetTexture()->m_size.cy += 4;
						RanderIcon( p2DRender, pItemBase, x * 32 - 2, y * 32 - 2, nParent );
						pItemBase->GetTexture()->m_size.cx -= 4;
						pItemBase->GetTexture()->m_size.cy -= 4;
					}
					else
						RanderIcon( p2DRender, pItemBase, x * 32, y * 32, nParent );

					CItemElem* pItemElem = (CItemElem*)pItemBase;
					if( pItemElem->GetProp()->dwPackMax > 1 )
					{
						short m_nItemNum	= pItemElem->m_nItemNum;
						if( pItemBase->GetExtra() > 0 )
							m_nItemNum	-= (short)pItemBase->GetExtra();			
						TCHAR szTemp[ 32 ];
						_stprintf( szTemp, "%d", m_nItemNum );
						CSize size = m_p2DRender->m_pFont->GetTextExtent( szTemp );
						m_p2DRender->TextOut( x * 32 + 32 - size.cx, y * 32 + 32 - size.cy, szTemp, 0xff0000ff );
						m_p2DRender->TextOut( x * 32 + 31 - size.cx, y * 32 + 31 - size.cy, szTemp, 0xffb0b0f0 );
					}

#ifdef __ITEMCOOLTIME_VER2_050308					
					DWORD dwGroup = g_pPlayer->m_cooltimeMgr.GetGroup( pItemElem->GetProp() );
					if( dwGroup )
					{
						DWORD dwEnd = g_pPlayer->m_cooltimeMgr.GetTime( dwGroup );		// 이벤트 종료 시각 
						if( dwEnd != 0 && dwEnd > dwCur )
						{
							DWORD dwBase = g_pPlayer->m_cooltimeMgr.GetBase( dwGroup );	// 이벤트 시작 시각 
							CPoint pt( x * 32, y * 32 );
							RenderRadar( p2DRender, pt, dwCur - dwBase, dwEnd - dwBase );
						}
					}
#endif	// __ITEMCOOLTIME_VER2_050308

#ifdef __YENCHANT_0608
					CWndInventory* pWndInventory	= (CWndInventory*)GetWndBase( APP_INVENTORY );
					if( pWndInventory )
					{
						if( pWndInventory->m_pUpgradeItem && pItemBase )
						{
							if( pWndInventory->m_pUpgradeItem == pItemBase )
							{
								if( pWndInventory->m_dwEnchantWaitTime != 0xffffffff )
								{
									CPoint pt( x * 32, y * 32 );
									RenderEnchant( p2DRender, pt );
								}
							}
						}
					}
#endif //__YENCHANT_0608
				}
				CRect rectHittest = CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 );
			}
		}
		else
		{
			m_nArrayCount = 0;

			if( nParent == APP_SHOP_ )
			{
				CWndShop* pWndBase = (CWndShop*)g_WndMng.GetWndBase( APP_SHOP_ );
				for( int i = 0; i < m_pItemContainer->GetSize(); i++ ) 
				{
					CItemBase* pItemBase = GetItem( i );
					if( pItemBase == NULL ) 
						continue;

					ItemProp* pItemProp		= pItemBase->GetProp();
					if( pItemProp == NULL ) 
						continue;

					if( pWndBase )
					{
						if( pWndBase->m_bSexSort && pWndBase->m_bLevelSort )
						{
							if( pItemProp->dwItemSex != 0xffffffff && pItemProp->dwLimitLevel1 != 0xffffffff )
							{
								if( pItemProp->dwItemSex == g_pPlayer->GetSex() && pItemProp->dwLimitLevel1 <= g_pPlayer->m_nLevel )
									m_pArrayItemBase[m_nArrayCount++] = pItemBase;
							}
							else
							if( pItemProp->dwItemSex != 0xffffffff )
							{
								if( pItemProp->dwItemSex == g_pPlayer->GetSex() )
									m_pArrayItemBase[m_nArrayCount++] = pItemBase;
							}
							else
							if( pItemProp->dwLimitLevel1 != 0xffffffff )
							{
								if( pItemProp->dwLimitLevel1 <= g_pPlayer->m_nLevel )
									m_pArrayItemBase[m_nArrayCount++] = pItemBase;
							}
							else
								m_pArrayItemBase[m_nArrayCount++] = pItemBase;
						}
						else
						if( pWndBase->m_bSexSort )
						{
							if( pItemProp->dwItemSex != 0xffffffff )
							{
								if( pItemProp->dwItemSex == g_pPlayer->GetSex() )
									m_pArrayItemBase[m_nArrayCount++] = pItemBase;
							}
							else
								m_pArrayItemBase[m_nArrayCount++] = pItemBase;
						}
						else
						if( pWndBase->m_bLevelSort )
						{
							if( pItemProp->dwLimitLevel1 != 0xffffffff )
							{
								if( pItemProp->dwLimitLevel1 <= g_pPlayer->m_nLevel )
									m_pArrayItemBase[m_nArrayCount++] = pItemBase;
							}
							else
								m_pArrayItemBase[m_nArrayCount++] = pItemBase;
						}
						else
							m_pArrayItemBase[m_nArrayCount++] = pItemBase;
					}
				}
			}
			else
			{
				for( int i = 0; i < m_pItemContainer->GetSize(); i++ ) 
				{
					CItemBase* pItemBase = GetItem( i );	
					if( pItemBase == NULL ) 
						continue;
					
					ItemProp* pItemProp		= pItemBase->GetProp();					
					if( pItemProp == NULL ) 
						continue;
					
					m_pArrayItemBase[m_nArrayCount++] = pItemBase;
				}
			}

			for( int i = 0; i < m_nArrayCount; i++ ) 
			{
				int x = ( i - pt.y ) % nWidth;
				int y = ( i - pt.y ) / nWidth;
				CItemBase* pItemBase = m_pArrayItemBase[i];
				if( pItemBase == NULL ) 
					continue;

				if( pItemBase->m_pTexture )
				{
					if( IsUsingItem( pItemBase ) || ((CItemElem*)pItemBase)->m_bRepair )
						RanderIcon( p2DRender, pItemBase, x * 32, y * 32, nParent, 60 );
					else 
					if( m_nOnSelect == i )
					{
						pItemBase->GetTexture()->m_size.cx += 4;
						pItemBase->GetTexture()->m_size.cy += 4;
						RanderIcon( p2DRender, pItemBase, x * 32 - 2, y * 32 - 2, nParent );
						pItemBase->GetTexture()->m_size.cx -= 4;
						pItemBase->GetTexture()->m_size.cy -= 4;
					}
					else
						RanderIcon( p2DRender, pItemBase, x * 32, y * 32, nParent );

					CItemElem* pItemElem = (CItemElem*)pItemBase;
					if( pItemElem->GetProp()->dwPackMax > 1 )
					{
						short m_nItemNum	= pItemElem->m_nItemNum;
						if( pItemBase->GetExtra() > 0 )
							m_nItemNum	-= pItemBase->GetExtra();			
						if( m_bVisibleCount )  
						{
							TCHAR szTemp[ 32 ];
							_stprintf( szTemp, "%d", m_nItemNum );
							CSize size = m_p2DRender->m_pFont->GetTextExtent( szTemp );
							m_p2DRender->TextOut( x * 32 + 32 - size.cx, y * 32 + 32 - size.cy, szTemp, 0xff0000ff );
							m_p2DRender->TextOut( x * 32 + 31 - size.cx, y * 32 + 31 - size.cy, szTemp, 0xffb0b0f0 );
						}
					}
				}
				//CRect rectHittest = CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 );
			}
		}
			
#else // __Y0402
		for( int i = pt.y; i < m_pItemContainer->GetSize(); i++ ) 
		{
			if( i < 0 ) continue;
			int x = ( i - pt.y ) % nWidth;
			int y = ( i - pt.y ) / nWidth;
			CItemBase* pItemBase = GetItem( i );
			if( pItemBase == NULL ) continue;

//			if( pItemBase->m_bEquip )
//				m_p2DRender->RenderFillRect( CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 ), D3DCOLOR_XRGB( 0x80, 0, 0 ) );
			if( pItemBase->m_pTexture )
			{
				if( IsUsingItem( pItemBase )  || ((CItemElem*)pItemBase)->m_bRepair )
					RanderIcon( p2DRender, pItemBase, x * 32, y * 32, nParent, 60 );
				else 
				if( m_nOnSelect == i )
				{
					pItemBase->m_pTexture->m_size.cx += 4;
					pItemBase->m_pTexture->m_size.cy += 4;
					RanderIcon( p2DRender, pItemBase, x * 32 - 2, y * 32 - 2, nParent );
					pItemBase->m_pTexture->m_size.cx -= 4;
					pItemBase->m_pTexture->m_size.cy -= 4;
				}
				else
					RanderIcon( p2DRender, pItemBase, x * 32, y * 32, nParent );

				CItemElem* pItemElem = (CItemElem*)pItemBase;
				if( pItemElem->GetProp()->dwPackMax > 1 )
				{
					short m_nItemNum	= pItemElem->m_nItemNum;
					if( pItemBase->GetExtra() > 0 )
						m_nItemNum	-= pItemBase->GetExtra();			
					TCHAR szTemp[ 32 ];
					_stprintf( szTemp, "%d", m_nItemNum );
					CSize size = m_p2DRender->m_pFont->GetTextExtent( szTemp );
					m_p2DRender->TextOut( x * 32 + 32 - size.cx, y * 32 + 32 - size.cy, szTemp, 0xff0000ff );
					m_p2DRender->TextOut( x * 32 + 31 - size.cx, y * 32 + 31 - size.cy, szTemp, 0xffb0b0f0 );
				}
			}
			CRect rectHittest = CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 );
			//if( i == m_nCurSelect )

			// 선택을 했을시 사각형 그리기
			//if( IsSelectedItem( i ) )
			//	p2DRender->RenderRect( rectHittest, 0xffa0a0a0 );
		}
#endif // __Y0402
		
		//
		// 아이콘 출력
		// CCtrllist
		rect = GetClientRect();
		nWidth = rect.Width() / 32;
		nHeight = rect.Height() / 32;
/*
		for( i = 1; i < nHeight + 1; i++ )
		{
			p2DRender->RenderLine( CPoint( 0, i * 32 ), CPoint( rect.Width(), i * 32 ), 0x30ffffff );
		}
		for( i = 1; i < nWidth ; i++ )
		{
			p2DRender->RenderLine( CPoint( i * 32, 0 ), CPoint( i * 32, rect.Height() ), 0x30ffffff );
		}
*/
	}
}
void CWndItemCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	if( !g_pPlayer )
		return;

	if( !m_pItemContainer )
		return;

	CPoint pt( 3, 3 );
	CRect rect;
	if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 )
		return;
	if(	CWndBase::m_GlobalShortcut.IsEmpty() == FALSE )
		return;
	//SetCapture();
	int nItem = HitTest( point );

	if( nItem == -1 )	return;

#ifdef __Y0402
	CWndBase* pWndBase = this->GetParentWnd();
	
	int nParent;
	if( pWndBase )
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if( pWndBaseTemp )
			nParent = pWndBaseTemp->GetWndId();
		else
			return;
	}
	else
	{
		return;
	}

	CItemBase* pItemBase = NULL;

#ifdef __YENCHANT_0608
	CWndInventory* pWndInventory	= (CWndInventory*)GetWndBase( APP_INVENTORY );
#endif //__YENCHANT_0608

	if( nParent == APP_INVENTORY )
	{
#ifdef __YENCHANT_0608
		if( pWndInventory->m_dwEnchantWaitTime != 0xffffffff )
		{
			g_WndMng.PutString( prj.GetText(TID_MMI_NOTUPGRADE), NULL, prj.GetTextColor(TID_MMI_NOTUPGRADE) );			
			m_pFocusItem = NULL;
			return;
		}
#endif //__YENCHANT_0608
		
		pItemBase = GetItem( nItem );
	}
	else
	{
		pItemBase = m_pArrayItemBase[nItem];
	}
	
#else // __Y0402
	CItemBase* pItemBase = GetItem( nItem );
#endif // __Y0402

	if( pItemBase )
	{
		if( IsUsingItem( pItemBase ) == FALSE && ((CItemElem*)pItemBase)->m_bRepair == FALSE )
		{
			if( IsSelectedItem( nItem ) == FALSE )
			{
				m_dwSelectAry.RemoveAll();
				m_dwSelectAry.Add( nItem );
				m_nCurSelect = nItem;
				m_pFocusItem = pItemBase;
			}
			m_bDrag = TRUE;
		}

#ifdef __YENCHANT_0608
		// 인첸트할 아이템을 선택하였다면 실행한다.
		// 클라에서 4초정도 이펙트 출력후 패킷을 날린다..
		if( nParent == APP_INVENTORY && pWndInventory )
		{
			CPoint pt( 3, 3 );

			CRect rect = GetClientRect();
			int nWidth = rect.Width() / 32;
			int nHeight = rect.Height() / 32;
			
			// 인첸, 피어싱관련 이펙트 출력위치   
			int nPage = nHeight;
			int nRange = m_pItemContainer->m_dwIndexNum / nWidth;
			if( m_pItemContainer->m_dwIndexNum % nWidth )
				nRange++;
			m_wndScrollBar.SetScrollRange( 0, nRange );
			m_wndScrollBar.SetScrollPage( nPage );
			
			pt.y = 0;
			pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
			
			int x = ( nItem - pt.y ) % nWidth;
			int y = ( nItem - pt.y ) / nWidth;
									
			pWndInventory->RunUpgrade(m_pFocusItem, x, y);	
		}
#endif //__YENCHANT_0608
		
		

	}
	else
	{
		m_dwSelectAry.RemoveAll();
		m_nCurSelect	= nItem;
		m_pFocusItem	= NULL;
//		m_bDrag		= FALSE;
	}
	//SetCapture();
}

void CWndItemCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
	if( !m_pItemContainer )
		return;

	//ReleaseCapture();
	m_bDrag = FALSE;
	//if(	CWndBase::m_GlobalShortcut.IsEmpty() == FALSE )
	//	return;
	int nItem = HitTest( point );
	if( nItem == -1 ) return;

#ifdef __Y0402
	CWndBase* pWndBase = this->GetParentWnd();
	
	int nParent;
	if( pWndBase )
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if( pWndBaseTemp )
			nParent = pWndBaseTemp->GetWndId();
		else
			return;
	}
	else
	{
		return;
	}

	CItemBase* pItemBase = NULL;

	if( nParent == APP_INVENTORY )
		pItemBase = GetItem( nItem );
	else
		pItemBase = m_pArrayItemBase[nItem];
	
#else // __Y0402	
	CItemBase* pItemBase = GetItem( nItem );
#endif // __Y0402


	if( pItemBase )
	{
		if( (GetAsyncKeyState( VK_LCONTROL ) & 0x8000 ) )
		{
			for( int j = 0; j < m_dwSelectAry.GetSize(); j++ )
			{
				if( m_dwSelectAry.GetAt( j ) == nItem )
					break;
			}
			if( j == m_dwSelectAry.GetSize() )
				m_dwSelectAry.Add( nItem );
			else
			if( m_nCurSelect != nItem )
				m_dwSelectAry.RemoveAt( j );
			m_nCurSelect = nItem;
			m_pFocusItem = pItemBase;
		//	m_rect = rect;
		}
		else
		if( IsSelectedItem( nItem ) )
		{
			m_dwSelectAry.RemoveAll();
			m_dwSelectAry.Add( nItem );
			m_nCurSelect = nItem;
			m_pFocusItem = pItemBase;
		}
	}
}
void CWndItemCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if( !m_pItemContainer )
		return;

	if( m_bDrag == FALSE )
		return;
	//TRACE( " hello \n" );
	m_bDrag = FALSE;
	CPoint pt( 3, 3 );
	CRect rect;
	//if( m_rect.PtInRect( point ) )
	//{
#ifdef __Y0402
	CWndBase* pWndBase = this->GetParentWnd();
	
	int nParent;
	if( pWndBase )
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if( pWndBaseTemp )
			nParent = pWndBaseTemp->GetWndId();
		else
			return;
	}
	else
	{
		return;
	}

	CItemBase* pItemElem = NULL;

	if( nParent == APP_INVENTORY )
		pItemElem = GetItem( m_nCurSelect ); //
	else
		pItemElem = m_pArrayItemBase[m_nCurSelect];
	
#else // __Y0402
	CItemBase* pItemElem = GetItem( m_nCurSelect ); //
#endif // __Y0402

	if( IsUsableItem( pItemElem ) )
	{
		m_GlobalShortcut.m_pFromWnd   = this;
		m_GlobalShortcut.m_dwShortcut = SHORTCUT_ITEM;
		m_GlobalShortcut.m_dwIndex    = m_nCurSelect;// pItemElem->m_dwItemId; //m_nCurSelect;// m_nCurSelect;//(DWORD)pItemElem;//->m_dwItemId;
		m_GlobalShortcut.m_dwType     = 0;// m_nCurSelect;//(DWORD)pItemElem;//->m_dwItemId;
		m_GlobalShortcut.m_dwId       = pItemElem->m_dwObjId;//(DWORD)pItemElem;
		m_GlobalShortcut.m_pTexture   = pItemElem->GetTexture();
		m_GlobalShortcut.m_dwData     = (DWORD) pItemElem;
		
		if(pItemElem->GetProp() == NULL)
		{
			CString strErr;
			strErr.Format( "CWndItemCtrl::OnMouseMove에서 GetProp가 NULL\nItemType : %d, ItemId : %d ", 0, pItemElem->m_dwItemId );
			ADDERRORMSG(strErr);
		}
		_tcscpy( m_GlobalShortcut.m_szString, pItemElem->GetProp()->szName);
	}
}
CItemBase* CWndItemCtrl::GetItem( DWORD dwIndex )
{
	return m_pItemContainer->GetAt( dwIndex );
}

#ifdef __Y0402
CItemBase* CWndItemCtrl::GetItemFromArr( DWORD dwIndex )
{
	ASSERT(dwIndex < 100);
	return m_pArrayItemBase[dwIndex];
}
#endif

int CWndItemCtrl::HitTest( CPoint point )
{
	if( !m_pItemContainer )
		return -1;

	int nDstId = -1;
	CPoint pt( 3, 3 );
	CRect rect;
	//
	// 리포트  
	//
	if( 0 ) //m_dwListCtrlStyle == WLVS_REPORT )
	{
		pt.y -= (m_nFontHeight + 3) * m_wndScrollBar.GetScrollPos();
		for( int i = 0; i < m_pItemContainer->m_dwIndexNum; i++ ) 
		{
			CItemBase* pItemElem = &m_pItemContainer->m_apItem[ m_pItemContainer->m_apIndex[ i ] ] ;
			rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
			if( rect.PtInRect( point ) )
			{
				nDstId = i;
				break;
			}
			pt.y += m_nFontHeight + 3;
		}
	}
	//
	// 아이콘 
	//
	if( 1 ) //m_dwListCtrlStyle == WLVS_ICON )
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 32;
		int nHeight = rect.Height() / 32;
		CPoint pt( 3, 3 );

		pt.y = 0;
		pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

#ifdef __Y0402
		CWndBase* pWndBase = this->GetParentWnd();
		
		int nParent;
		if( pWndBase )
		{
			CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
			if( pWndBaseTemp )
				nParent = pWndBaseTemp->GetWndId();
			else
				return nDstId;
		}
		else
		{
			return nDstId;
		}

		if( nParent == APP_INVENTORY )
		{
			for( int i = pt.y; i < m_pItemContainer->m_dwIndexNum; i++ ) 
			{
				int x = ( i - pt.y ) % nWidth;
				int y = ( i - pt.y ) / nWidth;
				rect.SetRect( x * 32, y * 32, x * 32 + 32, y * 32 + 32 );
				if( rect.PtInRect( point ) )
				{
					nDstId = i;
					break;
				}
			}
		}
		else
		{
			for( int i = pt.y; i < m_nArrayCount; i++ ) 
			{
				int x = ( i - pt.y ) % nWidth;
				int y = ( i - pt.y ) / nWidth;
				rect.SetRect( x * 32, y * 32, x * 32 + 32, y * 32 + 32 );
				if( rect.PtInRect( point ) )
				{
					nDstId = i;
					break;
				}
			}
		}
#else // __Y0402
		for( int i = pt.y; i < m_pItemContainer->m_dwIndexNum; i++ ) 
		{
			int x = ( i - pt.y ) % nWidth;
			int y = ( i - pt.y ) / nWidth;
			rect.SetRect( x * 32, y * 32, x * 32 + 32, y * 32 + 32 );
			if( rect.PtInRect( point ) )
			{
				nDstId = i;
				break;
			}
		}
#endif // __Y0402
	}
	return nDstId;
}
BOOL CWndItemCtrl::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	if( !m_pItemContainer )
		return FALSE;

	if( pShortcut->m_dwShortcut == SHORTCUT_ITEM )
	{
		int nDstIndex = HitTest( point );//
		if( nDstIndex == -1 ) nDstIndex = m_pItemContainer->m_dwIndexNum;
//			return FALSE;

		if( pShortcut->m_pFromWnd == this )
		{

#ifndef __ONLINE
			m_pItemContainer->Swap( pShortcut->m_dwIndex, nDstIndex );
#else
			CWndBase* pWndFrame =  pShortcut->m_pFromWnd->GetFrameWnd();
			if( pWndFrame->GetWndId() == APP_BANK ) // 42 // Bank
			{
				g_DPlay.SendMoveBankItem( pShortcut->m_dwIndex, (DWORD)nDstIndex );
			}
			else if( pWndFrame->GetWndId() == APP_INVENTORY )	// inventory -> inventory
			{
				CItemBase* pItemBase	= m_pItemContainer->GetAt( pShortcut->m_dwIndex );
				if( IsUsingItem( pItemBase ) == FALSE )
					g_DPlay.SendMoveItem( 0, pShortcut->m_dwIndex, (DWORD)nDstIndex );
			}
			
#endif
		}
		else
		{
			CWndBase* pParent = (CWndBase*)GetParentWnd();
			pParent->OnChildNotify( WIN_ITEMDROP, m_nIdWnd, (LRESULT*)pShortcut ); 
		}
		return TRUE;
	}
	return FALSE;
}
/*
void CWndItemCtrl::OnDropIcon( DWORD dwShorCutType, DWORD dwShortCutIndex, CPoint point )
{
	if( dwShorCutType == SHORTCUT_ITEM )
	{
		if( m_dwListCtrlStyle == WLVS_ICON )
		{
			CRect rect = GetClientRect();
			int nWidth = rect.Width() / 32;
			int nHeight = rect.Height() / 32;
	CPoint pt( 3, 3 );

			pt.y = 0;
			pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
			for( int i = pt.y; i < m_pItemContainer->m_dwIndexNum; i++ ) 
			{
				int x = ( i - pt.y ) % nWidth;
				int y = ( i - pt.y ) / nWidth;
				CItemElem* pItemElem = &m_pItemContainer->m_apItem[ m_pItemContainer->m_apIndex[ i ] ] ;
				rect.SetRect( x * 32, y * 32, x * 32 + 32, y * 32 + 32 );
				if( rect.PtInRect( point ) )
				{
					for( int j = m_pItemContainer->m_dwIndexNum; j > i; j-- )
						m_pItemContainer->m_apIndex[ j ] = m_pItemContainer->m_apIndex[ j - 1 ];
					m_pItemContainer->m_apIndex[ j ] = dwItemOld;
					(m_pItemContainer->m_dwIndexNum)++;
					return;
				}
			}
			m_pItemContainer->m_apIndex[ m_pItemContainer->m_dwIndexNum ] = dwItemOld;
			(m_pItemContainer->m_dwIndexNum)++;
		}



		_pItemContainer->Swap( CWndBase::m_GlobalShortcut.m_dwData, 
		
		CItemElem* pItemOld = &m_apItem[ CWndBase::m_GlobalShortcut.m_dwData ];
		for( int i = 0; m_pItemContainer->m_dwIndexNum; i++ )
		{
			CItemElem* pItemElem = &m_apItem[ i ];
			if( pItemElem->m_dwItemId )
			{
				*pItemElem = *pItemOld;
				pItemOld->m_dwItemId = 0;
			}
		}
		
	}
}*/
BOOL CWndItemCtrl::IsSelectedItem( int iItem )
{
	for( int i = 0; i < m_dwSelectAry.GetSize(); i++ )
		if( m_dwSelectAry.GetAt( i ) == iItem )
			return TRUE;
	return FALSE;
}
int CWndItemCtrl::GetSelectedItem( int i )
{
	return m_dwSelectAry.GetAt( i );
}
UINT CWndItemCtrl::GetSelectedCount()
{
	return m_dwSelectAry.GetSize();
}

void CWndItemCtrl::OnRButtonUp( UINT nFlags, CPoint point )
{
}
void CWndItemCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{
#ifdef __YENCHANT_0608
	// 인벤창이 열려있고 인첸트 모드이면 커서모양 변경
	CWndInventory* pWndInventory	= (CWndInventory*)GetWndBase( APP_INVENTORY );
	
	if( pWndInventory && pWndInventory->m_dwEnchantWaitTime == 0xffffffff )
	{
		pWndInventory->BaseMouseCursor();
	}
#endif //__YENCHANT_0608
	
	if( !m_pItemContainer )
		return;

#ifdef	__NQUESTITEM_INFO_040917
	CRect rectCtrl = GetClientRect();

	int nWidth = rectCtrl.Width() / 32;
	int nHeight = rectCtrl.Height() / 32;

	CPoint pt;
	pt.y = 0;
	pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

	for( int i=pt.y ; i < m_pItemContainer->GetSize() ; i++ )
	{
		if( i < 0 ) continue;
		int x = ( i - pt.y ) % nWidth;
		int y = ( i - pt.y ) / nWidth;

		CItemBase* pItemBase = GetItem( i );
		if( pItemBase == NULL ) continue;
		
		CRect rectHittest = CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 );

		if( rectHittest.PtInRect( point ) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &rectHittest );
			ItemProp* pItemProp = pItemBase->GetProp();
#ifdef __V050322_CACHEQUEST
			if (pItemBase == NULL )
				continue;
			if( pItemProp->dwItemKind2 == IK2_TEXT ) 
			{
				CString strText = pItemProp->szTextFileName;
				//strText = strText.Left( 5 );
				strText.MakeLower();
				if( strText.Find( "book_" ) != -1 )
				{
					if ( !g_WndMng.m_pWndTextBook || g_WndMng.m_pWndTextBook->IsVisible() == FALSE)
						g_WndMng.OpenTextBook(this, pItemBase) ;
					else
						g_WndMng.ChangeTextBook(pItemBase) ;
				}
				else
				if( strText.Find( "scroll_" ) != -1  )
				{
					if ( !g_WndMng.m_pWndTextScroll || g_WndMng.m_pWndTextScroll->IsVisible() == FALSE)
						g_WndMng.OpenTextScroll(this, pItemBase) ;
					else
						g_WndMng.ChangeTextScroll(pItemBase) ;
				}
				else
				if( strText.Find( "letter_" ) != -1 )
				{
					if ( !g_WndMng.m_pWndTextLetter || g_WndMng.m_pWndTextLetter->IsVisible() == FALSE)
						g_WndMng.OpenTextLetter(this, pItemBase) ;
					else
						g_WndMng.ChangeTextLetter(pItemBase) ;
				}

			}
			else
			if( pItemProp->dwItemKind3 == IK3_QUEST )
			{
				if ( !g_WndMng.m_pQuestItemInfo || g_WndMng.m_pQuestItemInfo->IsVisible() == FALSE)
					g_WndMng.OpenQuestItemInfo(this, pItemBase) ;
				else
					g_WndMng.ChangeQuestItemInfo(pItemBase) ;
			}
#else // __V050322_CACHEQUEST
//{{AFX
			if (pItemBase == NULL || pItemProp->dwItemKind3 != IK3_QUEST)
				continue;
			if (!g_WndMng.m_pQuestItemInfo || g_WndMng.m_pQuestItemInfo->IsVisible() == FALSE)
				g_WndMng.OpenQuestItemInfo(this, pItemBase) ;
			else
				g_WndMng.ChangeQuestItemInfo(pItemBase) ;
//}}AFX
#endif // __V050322_CACHEQUEST
			//g_WndMng.ObjectExecutor( SHORTCUT_APPLET, APP_QUESTITEM_INFO );
		}
	}
#endif//__NQUESTITEM_INFO_040917
}
BOOL CWndItemCtrl::OnSetCursor ( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
#ifdef __YENCHANT_0608
	// 인벤창이 열려있고 인첸트 모드이면 커서모양 변경
	CWndInventory* pWndInventory	= (CWndInventory*)GetWndBase( APP_INVENTORY );
	
	if( pWndInventory )
		pWndInventory->SetEnchantCursor();
#endif //__YENCHANT_0608
	
	return TRUE;
}

void CWndItemCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if( !m_pItemContainer )
		return;

	if( IsUsableItem( m_pFocusItem ) )
	{
		CWndBase* pParent = (CWndBase*)GetParentWnd();
		pParent->OnChildNotify( WIN_DBLCLK,m_nIdWnd,(LRESULT*)m_pFocusItem); 
	}
}
void CWndItemCtrl::OnRButtonDblClk( UINT nFlags, CPoint point)
{
}
void CWndItemCtrl::OnSize( UINT nType, int cx, int cy )
{
//	m_string.Reset( g_2DRender.m_pFont, &GetClientRect() );

	CRect rect = GetWindowRect();
	rect.left = rect.right - 15;
	m_wndScrollBar.SetWndRect( rect );

	CWndBase::OnSize( nType, cx, cy);
}
void CWndItemCtrl::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 4, 0 );

	if( m_pItemContainer ) 
	{
		if( 0 ) //m_dwListCtrlStyle == WLVS_REPORT )
		{
			int nPage = GetClientRect().Height() / (m_nFontHeight + 3);
			int nRange = m_pItemContainer->m_dwIndexNum;// - nPage;
			if( !( nRange - nPage <= 0 ) )
				m_rectClient.right -= 15; // 스크롤 바가 보이면 
			m_rectClient.top += 15; // 리포트 칼럼 
		}
		if( 1 ) //m_dwListCtrlStyle == WLVS_ICON )
		{
			CRect rect = GetClientRect();
			int nWidth = rect.Width() / 32;
			int nHeight = rect.Height() / 32;
			int nPage = nHeight;
			int nRange = m_pItemContainer->m_dwIndexNum / nWidth;// - nPage;
			//if( !( nRange - nPage <= 0 ) )
				m_rectClient.right -= 15; // 스크롤 바가 보이면 
		}
	}
	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}
void CWndItemCtrl::PaintFrame( C2DRender* p2DRender )
{
	return;
	CRect rect = GetWindowRect();
	//m_pTheme->RenderWndTextFrame( p2DRender, &rect );
	/*
	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );//D3DCOLOR_TEMP( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 180, 240, 240,  240 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//

	p2DRender->RenderFillRect ( rect, dwColor1 );
	p2DRender->RenderRoundRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor3 );
	*/
	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );//D3DCOLOR_TEMP( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//

	p2DRender->RenderFillRect ( rect, dwColor1 );
	p2DRender->RenderRoundRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor3 );

	int nPage, nRange;
	if( 0 ) //m_dwListCtrlStyle == WLVS_REPORT )
	{
		nPage = GetClientRect().Height() / ( GetFontHeight( ) + 3 );
		nRange = m_pItemContainer->m_dwIndexNum;// - nPage;
	}
	if( 1 ) //m_dwListCtrlStyle == WLVS_ICON )
	{
		CRect rect = GetClientRect();
		nPage = rect.Height() / 32;
		nRange = m_pItemContainer->m_dwIndexNum / ( rect.Width() / 32 );// - nPage;
	}
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );

	if( 0 ) //m_dwListCtrlStyle == WLVS_REPORT )
	{
		// 컬럼 출력 
		for( int i = 0, x = 0; i < m_aColumns.GetSize(); i++ )
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
BOOL CWndItemCtrl::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}
BOOL CWndItemCtrl::SetItem( const LVITEM* pItem )
{
	if( pItem->iItem < m_aItems.GetSize() && m_aItems.GetAt( pItem->iItem ) == NULL ) 
		return FALSE; // 존재하지 않는다.
	LVITEM* pItems = (LVITEM*)m_aItems.GetAt( pItem->iItem );
	memcpy( &pItems[ pItem->iSubItem ], pItem, sizeof( LVITEM ) );
	pItems[ pItem->iSubItem ].pszText = new _TCHAR[ _tcslen( pItem->pszText ) + sizeof( _TCHAR ) ];
	_tcscpy( pItems[ pItem->iSubItem ].pszText, pItem->pszText );
	return TRUE;
}
int CWndItemCtrl::InsertItem( const LVITEM* pItem )
{
	if( pItem->iItem < m_aItems.GetSize() && m_aItems.GetAt( pItem->iItem ) ) 
		return -1; // 이미 존재한다.
	LVITEM* pNewItems = new LVITEM[ 3 ]; // m_aColumns.GetSize()
	memcpy( &pNewItems[ pItem->iSubItem ], pItem, sizeof( LVITEM ) );
	pNewItems[ pItem->iSubItem ].pszText = new _TCHAR[ _tcslen( pItem->pszText ) + sizeof( _TCHAR ) ];
	_tcscpy( pNewItems[ pItem->iSubItem ].pszText, pItem->pszText );
	m_aItems.SetAtGrow( pItem->iItem, (void*)pNewItems );
	return pItem->iItem;
}
int CWndItemCtrl::GetItemCount() const
{
	return m_aItems.GetSize();
}
int CWndItemCtrl::InsertColumn( int nCol, const LVCOLUMN* pColumn )
{
	LVCOLUMN* pNewColumn = new LVCOLUMN;
	memcpy( pNewColumn, pColumn, sizeof( LVCOLUMN ) );
	pNewColumn->pszText = new _TCHAR[ _tcslen( pColumn->pszText ) ];
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
#ifdef __Y0602
BOOL CWndItemCtrl::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
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

