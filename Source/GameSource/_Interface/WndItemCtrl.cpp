// WndBase.cpp: implementation of the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WndManager.h"
#include "WndText.h"
#include "WndField2.h"

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
	m_nTabCurSel = -1;
	m_pFocusItem = NULL;
	m_pInvenSlotPorc = NULL;
	m_bDrag = FALSE;
	m_nOnSelect = -1;

	memset( m_pArrayItemElem, 0, sizeof(m_pArrayItemElem) );
	m_nArrayCount = 0;
	m_bVisibleCount = TRUE;
	m_useDieFlag = FALSE;
}
CWndItemCtrl::~CWndItemCtrl()
{
	g_toolTip.CancelToolTip();
	g_toolTipSub1.CancelToolTip();
	g_toolTipSub2.CancelToolTip();
//	ResetContent();
}
void CWndItemCtrl::Create( DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID )
{
	CWndBase::Create( WBS_CHILD | dwListCtrlStyle, rect, pParentWnd, nID );
}
void CWndItemCtrl::InitItem( CItemContainer<FLItemElem>* pItemContainer, DWORD SendToId)
{
	m_pInvenSlotPorc = new CFLWndInventorySlotProc;
	switch(_GetContentState(CT_NEW_INVENTORY))
	{
	case CS_VER1:
		{
			m_pInvenSlotPorc->m_pItemContainer = pItemContainer;
		}
		break;
	case CS_VER2:
		{
			m_pInvenSlotPorc->m_pItemContainer = pItemContainer;
			if(pItemContainer)
			{
				m_pInvenSlotPorc->SetSlotItem();
			}

		}
		break;
	}
	m_pFocusItem	= NULL;
	m_dwFrame = SendToId;
}

void CWndItemCtrl::OnInitialUpdate()
{
	CRect rect = GetWindowRect();
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );//,m_pSprPack,-1);
	m_pTexItemCheck = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), "WndItemCheck.bmp" ), 0xffff00ff );
}

//sun: 9차 전승관련 Clienet
void CWndItemCtrl::UpdateTooltip( void )
{
	CRect rect = GetClientRect();
	int nWidth = rect.Width() / 32;
	int nHeight = rect.Height() / 32;

	CPoint point= GetMousePoint();
//	ClientToScreen( &point );

	// 스크롤바 관련   
	CPoint pt;
	pt.y = 0;
	pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
	int nItemRender = 0;
	DWORD dwItemSize = 0;
	switch(_GetContentState(CT_NEW_INVENTORY))
	{
	case CS_VER1:
		{
			nItemRender = MAX_HUMAN_PARTS;
			dwItemSize = m_pInvenSlotPorc->m_pItemContainer->GetMax();
// 			for( DWORD i = pt.y; i < m_pInvenSlotPorc->m_pItemContainer->GetMax(); i++ ) 
// 			{
// 				if( (int)i < 0 )
// 					continue;
// 
// 				int x = ( i - pt.y ) % nWidth;
// 				int y = ( i - pt.y ) / nWidth;
// 
// 				FLItemElem* pItemElem = GetItem( i );
// 				if( pItemElem == NULL )
// 					continue;
// 
// 				CRect rectHittest = CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 );
// 				if( rectHittest.PtInRect( point ) )
// 				{
// 					CPoint point2 = point;
// 					ClientToScreen( &point2 );
// 					ClientToScreen( &rectHittest );
// 					if( m_dwFrame == APP_SHOP_ )		// 상점
// 						g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_SHOP_ );
// 					else if( m_dwFrame == APP_SHOP_CART_ )// _JIN_SHOP_CART
// 						g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_SHOP_CART_ );
// 					else if( m_dwFrame == APP_INVENTORY || m_dwFrame == APP_BANK )	// 나의 인벤토리
// 					{
// 						if( g_WndMng.GetWndBase( APP_REPAIR ) )
// 						{
// 							g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_REPAIR );		
// 						}
// 						else
// 						{
// 							g_WndMng.PutToolTipItemAndSubToolTip( pItemElem, point2, &rectHittest, APP_INVENTORY );
// 						}
// 					}
// 				}
// 			}
		}
		break;
	case CS_VER2:
		{
			int nItemRender = MAX_HUMAN_PARTS;
			switch(m_nTabCurSel)
			{
			case 0:
				nItemRender = MAX_HUMAN_PARTS;
				break;
			case 1:
				nItemRender = MAX_INVENINDEX_GENERAL;
				break;
			case 2:
				nItemRender = MAX_INVENINDEX_PET;
				break;
			case 3:
				nItemRender = MAX_INVENINDEX_COSTUME;
				break;
			}
			dwItemSize = m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel]->GetMax();
 		}
		break;
	}	

	int nCnt = 0;
	int x,y = 0;
	for( DWORD i = nItemRender; i < (int)( dwItemSize ); i++,nCnt++) 
	{
		if( (int)nCnt < 0 )
			continue;

		x = ( nCnt - pt.y ) % nWidth;
		y = ( nCnt - pt.y ) / nWidth;
		
		FLItemElem* pItemElem = GetItem( i );
		if( pItemElem == NULL )
			continue;

		CRect rectHittest = CRect( x * SLOTSIZE, y * SLOTSIZE_SY, x * SLOTSIZE + SLOTSIZE - 2, y * SLOTSIZE_SY + SLOTSIZE - 2 );
		if( rectHittest.PtInRect( point ) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &rectHittest );
			if( m_dwFrame == APP_SHOP_ )		// 상점
				g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_SHOP_ );
			else if( m_dwFrame == APP_SHOP_CART_ )// _JIN_SHOP_CART
				g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_SHOP_CART_ );
			else if( m_dwFrame == APP_INVENTORY || m_dwFrame == APP_BANK )	// 나의 인벤토리
			{
				if( g_WndMng.GetWndBase( APP_REPAIR ) )
				{
					g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_REPAIR );		
				}
				else
				{
					g_WndMng.PutToolTipItemAndSubToolTip( pItemElem, point2, &rectHittest, APP_INVENTORY );
				}
			}
		}
	}

}

void CWndItemCtrl::OnMouseWndSurface( CPoint point )
{
	if( !m_pInvenSlotPorc->m_pItemContainer )
		return;
	
	int nItemRender = 0;
	DWORD dwItemSize;
	CRect rect = GetClientRect();
	int nWidth = rect.Width() / 32;
	int nHeight = rect.Height() / 32;
	// 스크롤바 관련   
	CPoint pt;
	pt.y = nItemRender;
	pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
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

	// _JIN_SHOP_CART
	// 아이템이 선택되지 않을때에도 아이콘이 활성화 되어있는 버그 수정 
	// 임시로 장바구니의 기능만 수정함 
	if(g_WndMng.m_pWndShop_Cart != NULL)
	{
		CRect rectCtrl = g_WndMng.m_pWndShop_Cart->GetWndShopList()->m_rectCurrentClient;
		if(!rectCtrl.PtInRect(point))
		{
			m_nOnSelect = -1;
		}
	}

	if( nParent == APP_INVENTORY )
	{
		switch(_GetContentState(CT_NEW_INVENTORY))
		{
		case CS_VER1:
			{
				dwItemSize = m_pInvenSlotPorc->m_pItemContainer->GetMax();
				nItemRender = MAX_HUMAN_PARTS;
// 				for( DWORD i = pt.y; i < dwItemSize ; i++ ) 
// 				{
// 					if( i < 0 ) continue;
// 					int x = ( i - pt.y ) % nWidth;
// 					int y = ( i - pt.y ) / nWidth;
// 					FLItemElem* pItemElem = GetItem( i );
// 					if( pItemElem == NULL ) continue;
// 					CRect rectHittest = CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 );
// 
// 					if( rectHittest.PtInRect( point ) )
// 					{
// 						CPoint point2 = point;
// 						ClientToScreen( &point2 );
// 						ClientToScreen( &rectHittest );
// 
// 						if( m_dwFrame == APP_SHOP_ )		// 상점
// 							g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_SHOP_ );
// 						else if( m_dwFrame == APP_SHOP_CART_ )// _JIN_SHOP_CART
// 							g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_SHOP_CART_ );			
// 						else if( m_dwFrame == APP_INVENTORY || m_dwFrame == APP_BANK )	// 나의 인벤토리
// 						{
// 							if( g_WndMng.GetWndBase( APP_REPAIR ) )
// 							{
// 								g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_REPAIR );		
// 							}
// 							else
// 							{
// 								g_WndMng.PutToolTipItemAndSubToolTip( pItemElem, point2, &rectHittest, APP_INVENTORY );
// 							}
// 						}
// 
// 						m_nOnSelect = i;
// 					}
// 				}
			}
			break;
		case CS_VER2:
			{
				switch(m_nTabCurSel)
				{
				case 0:
					nItemRender = MAX_HUMAN_PARTS;
					break;
				case 1:
					nItemRender = MAX_INVENINDEX_GENERAL;
					break;
				case 2:
					nItemRender = MAX_INVENINDEX_PET;
					break;
				case 3:
					nItemRender = MAX_INVENINDEX_COSTUME;
					break;
				}	
				dwItemSize = m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel]->GetMax();
			}
			break;
		default:
			FLASSERT(0);
			break;
		}

		int nCnt = 0;
		int x,y = 0;
		for( DWORD i = nItemRender; i < dwItemSize ; i++ ,nCnt++) 
		{
			if( nCnt  < 0 ) continue;
			x = ( nCnt  - pt.y ) % nWidth;
			y = ( nCnt  - pt.y ) / nWidth;
			FLItemElem* pItemElem = GetItem( i );
			if( pItemElem == NULL ) continue;
			CRect rectHittest = CRect( x * SLOTSIZE, y * SLOTSIZE_SY, x * SLOTSIZE + SLOTSIZE - 2, y * SLOTSIZE_SY + SLOTSIZE - 2 );

			if( rectHittest.PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &rectHittest );

				if( m_dwFrame == APP_SHOP_ )		// 상점
					g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_SHOP_ );
				else if( m_dwFrame == APP_SHOP_CART_ )// _JIN_SHOP_CART
					g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_SHOP_CART_ );			
				else if( m_dwFrame == APP_INVENTORY || m_dwFrame == APP_BANK )	// 나의 인벤토리
				{
					if( g_WndMng.GetWndBase( APP_REPAIR ) )
					{
						g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_REPAIR );		
					}
					else
					{
						g_WndMng.PutToolTipItemAndSubToolTip( pItemElem, point2, &rectHittest, APP_INVENTORY );
					}
				}

				m_nOnSelect = i;
			}
		}
	}
//sun: 11, 주머니
	if( nParent == APP_BAG_EX )
	{
		for( DWORD i = pt.y; i < m_pInvenSlotPorc->m_pItemContainer->GetMax(); i++ ) 
		{
			if( i < 0 ) continue;
			int x = ( i - pt.y ) % nWidth;
			int y = ( i - pt.y ) / nWidth;
			FLItemElem* pItemElem = GetItem( i );
			if( pItemElem == NULL ) continue;
			CRect rectHittest = CRect( x * SLOTSIZE, y * SLOTSIZE_SY, x * SLOTSIZE + SLOTSIZE - 2, y * SLOTSIZE_SY + SLOTSIZE - 2 );

			if( rectHittest.PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &rectHittest );
					
				g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_INVENTORY );		
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
			FLItemElem* pItemElem = m_pArrayItemElem[i];
			if( pItemElem == NULL ) continue;
			CRect rectHittest = CRect( x * SLOTSIZE, y * SLOTSIZE_SY, x * SLOTSIZE + SLOTSIZE - 2, y * SLOTSIZE_SY + SLOTSIZE - 2 );

			if( rectHittest.PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &rectHittest );
				
				if( m_dwFrame == APP_SHOP_ )		// 상점
					g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_SHOP_ );
				if( m_dwFrame == APP_SHOP_CART_ )// _JIN_SHOP_CART
					g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_SHOP_CART_ );		
				else if( m_dwFrame == APP_INVENTORY || m_dwFrame == APP_BANK
				 || m_dwFrame == APP_GUILD_BANK
				 || m_dwFrame == APP_COMMON_BANK
					)	// 나의 인벤토리
				{
					if( g_WndMng.GetWndBase( APP_REPAIR ) )
					{
						g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_REPAIR );		
					}
					else
					{
						g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, APP_INVENTORY );		
					}
				}

				m_nOnSelect = i;
			}
		}
	}



}

void CWndItemCtrl::RanderIcon( C2DRender* p2DRender, FLItemElem* pItemElem, int x, int y, int nParent, int nalpha ) 
{
	int bRander = 0;

	if( nParent == APP_SHOP_ )
	{
		PT_ITEM_SPEC pItemProp		= pItemElem->GetProp();
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
				if( pItemElem->CanUseLevel( g_pPlayer ) == false )
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
				if( g_pPlayer->GetFlightLv() < (int)( pItemProp->dwFlightLimit ) )
				{
					bRander = 1;
					break;
				}
				break;
			}
		}
 
		// 모든아이템 가격 비교
//sun: 11, 칩으로 상점 이용 하기 (길드 대전 관련)
		if( nParent == APP_SHOP_ )
		{
			CWndShop* pWndBase = (CWndShop*)g_WndMng.GetWndBase( APP_SHOP_ );
			LPCHARACTER lpCharacter = pWndBase->m_pMover->GetCharacter();
			if(lpCharacter && lpCharacter->m_nVenderType == VENDOR_TYPE_CHIP)
			{
				if( bRander == 0 && g_pPlayer->m_Inventory.GetItemNumByItemId( ITEM_INDEX( 26460, II_CHP_RED ) ) < (int)pItemElem->GetChipCost() )
					bRander = 1;
			}
		}
		else
		{
			if( bRander == 0 && g_pPlayer->GetGold() < (int)pItemProp->dwCost )
				bRander = 1;
		}
	}
	else if(nParent == APP_SHOP_CART_)// _JIN_SHOP_CART
	{
		PT_ITEM_SPEC pItemProp		= pItemElem->GetProp();
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
				if( pItemElem->CanUseLevel( g_pPlayer ) == false )
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
				if( g_pPlayer->GetFlightLv() < (int)( pItemProp->dwFlightLimit ) )
				{
					bRander = 1;
					break;
				}
				break;
			}
		}

		// 모든아이템 가격 비교
		//sun: 11, 칩으로 상점 이용 하기 (길드 대전 관련)
		if( nParent == APP_SHOP_CART_ )// _JIN_SHOP_CART
		{
			CFLWndShop_Cart* pWndBase = (CFLWndShop_Cart*)g_WndMng.GetWndBase( APP_SHOP_CART_ );
			LPCHARACTER lpCharacter = pWndBase->m_pMover->GetCharacter();
			if(lpCharacter && lpCharacter->m_nVenderType == 1)
			{
				if( bRander == 0 && g_pPlayer->m_Inventory.GetItemNumByItemId( ITEM_INDEX( 26460, II_CHP_RED ) ) < (int)pItemElem->GetChipCost() )
					bRander = 1;
			}
		}
		else
		{
			if( bRander == 0 && g_pPlayer->GetGold() < (int)pItemProp->dwCost )
				bRander = 1;
		}
	}

	if( pItemElem->IsFlag( FLItemElem::expired ) )
	{
		bRander = 1;
	}


	if( !g_eLocal.GetState(EVE_SCHOOL) )
	{
		if( bRander == 0 )
		{
			pItemElem->GetTexture()->Render( p2DRender, CPoint( x, y ), nalpha );
		}
		else
		if( bRander == 1 )	// 사용할수 없는것
		{
			pItemElem->GetTexture()->Render2( p2DRender, CPoint( x, y ), D3DCOLOR_ARGB( nalpha, 255, 100, 100 ) );
		}
	}
	else
		pItemElem->GetTexture()->Render( p2DRender, CPoint( x, y ), nalpha );
#ifdef ADD_INVENTORY_EDGE
	// 테두리 추가 예정
	CTexture* pTexture;
	if( pItemElem->GetProp()->dwItemGrade == ITEM_GRADE_UNIQUE )
	{
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, "Item_EdgeUnique.dds" ), 0xffff00ff );
		pTexture->Render( p2DRender, CPoint( x, y ), nalpha );
	}
	else if( pItemElem->GetProp()->IsUltimate() )
	{
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, "Item_EdgeUltimate.dds" ), 0xffff00ff );
		pTexture->Render( p2DRender, CPoint( x, y ), nalpha );
	}
	else if( pItemElem->GetProp()->IsBaruna() )
	{
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, "Item_EdgeBaruna.dds" ), 0xffff00ff );
		pTexture->Render( p2DRender, CPoint( x, y ), nalpha );
	}

	
#endif

}

void CWndItemCtrl::OnDraw(C2DRender* p2DRender) 
{
	CMover* pPlayer		= g_pPlayer;
	if( !pPlayer )
		return;
	CWorld* pWorld	= pPlayer->GetWorld();
	if( !pWorld )
		return;

	if( !m_pInvenSlotPorc->m_pItemContainer )
		return;

	PFONTCOLOR_WNDITEMCTRL pFontColorWndItemCtrl = g_WndFontColorManager->GetWndItemCtrl();


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

	// 리포트 출력 
	//
	if( 0 ) //m_dwListCtrlStyle == WLVS_REPORT )
	{
		m_nFontHeight = GetFontHeight( );
		pt.y -= ( m_nFontHeight + 3 ) * m_wndScrollBar.GetScrollPos();

		for( DWORD i = 0; i < m_pInvenSlotPorc->m_pItemContainer->GetMax(); i++ ) 
		{
			//LVITEM* pItems = (LVITEM*)m_aItems.GetAt( m_pItemContainer->m_apIndex[ i ] );
			DWORD dwColor = 0xffffffff;
			if( i == m_nCurSelect )
				dwColor = 0xff00ff00; 
			FLItemElem* pItemElem = m_pInvenSlotPorc->m_pItemContainer->GetAtId( i );
			if( pItemElem != NULL && pItemElem->IsEmpty() == FALSE )
			{
				for( int i2 = 0, x = 0; i2 < m_aColumns.GetSize(); i2++ ) 
				{
					switch( i2 )
					{
					case 0:
						p2DRender->TextOut( x + 2, pt.y, pItemElem->GetName(), dwColor ); 
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
		//탭에 따라 출력되는 아이콘이 틀려야함 
		int nPage;
		int nRange;
		int nItemRender = 0;
		DWORD dwItemSize = 0;
		DWORD dwItemMaxSize = 0;
	
// 		nPage = nHeight;
// 		nRange = m_pItemContainer->GetSize() / nWidth;// - nPage;
// 		if( m_pItemContainer->GetSize() % nWidth )
// 			nRange++;
// 		m_wndScrollBar.SetScrollRange( 0, nRange );
// 		m_wndScrollBar.SetScrollPage( nPage );
// 
// 		pt.y = 0;
// 		pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
		switch(_GetContentState(CT_NEW_INVENTORY))
		{
		case CS_VER1:
			{
				m_nTabCurSel = 0;		
				nItemRender = MAX_HUMAN_PARTS;
				dwItemSize = m_pInvenSlotPorc->m_pItemContainer->GetSize();
				dwItemMaxSize = m_pInvenSlotPorc->m_pItemContainer->GetMax();
				nPage = nHeight;
				nRange =  dwItemSize / nWidth;// - nPage;
				if( dwItemSize % nWidth )
					nRange++;
				m_wndScrollBar.SetScrollRange( 0, nRange );
				m_wndScrollBar.SetScrollPage( nPage );

				pt.y = 0;
				pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
			}
			break;
		case CS_VER2:
			{
				CWndInventory* pWndInventory	= (CWndInventory*)g_WndMng.GetWndBase( APP_INVENTORY );
				
				if(pWndInventory)
				{
					dwItemMaxSize = m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel]->GetMax();
					m_nTabCurSel = pWndInventory->GetTabCtrl()->GetCurSel();			
					switch( m_nTabCurSel )
					{
					case 0:
						{
							nPage = nHeight;
							dwItemSize = MAX_INVENTORY_GENERAL;
							nRange = dwItemSize / nWidth;// - nPage;
							if( dwItemSize % nWidth )
								nRange++;
							m_wndScrollBar.SetScrollRange( 0, nRange );
							m_wndScrollBar.SetScrollPage( nPage );
							nItemRender = MAX_HUMAN_PARTS;
							pt.y = 0;
							pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
						}
						break;
					case 1:
						{
							nPage = nHeight;
							dwItemSize = MAX_INVENTORY_PET;
							nRange = dwItemSize / nWidth;// - nPage;
							if( m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel]->GetSize() % nWidth )
								nRange++;
							m_wndScrollBar.SetScrollRange( 0, nRange );
							m_wndScrollBar.SetScrollPage( nPage );
  							nItemRender = MAX_INVENINDEX_GENERAL;
							pt.y = 0;
							pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
						}
						break;
					case 2:
						{
							dwItemSize = MAX_INVENTORY_COSTUME;
							nPage = nHeight;
							nRange = dwItemSize / nWidth;// - nPage;
							if( m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel]->GetSize() % nWidth )
								nRange++;
							m_wndScrollBar.SetScrollRange( 0, nRange );
							m_wndScrollBar.SetScrollPage( nPage );
							nItemRender = MAX_INVENINDEX_PET;
							pt.y = 0;
							pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
						}
						break;
					case 3:
						{
							dwItemSize = MAX_INVENTORY_QUEST;
							nPage = nHeight;
							nRange = dwItemSize / nWidth;// - nPage;
							if( m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel]->GetSize() % nWidth )
								nRange++;
							m_wndScrollBar.SetScrollRange( 0, nRange );
							m_wndScrollBar.SetScrollPage( nPage );
							nItemRender = MAX_INVENINDEX_COSTUME;
							pt.y = 0;
							pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
						}
						break;
					}
				}
            }
			break;
		default:
			FLASSERT(0);
			break;
		}

		if( nParent == APP_INVENTORY )
		{
			int nCnt = 0;
			int x,y;
			for( DWORD i = nItemRender;i < dwItemMaxSize ; i++ ,nCnt++) 
			{
				if( nCnt  < 0 ) 
					continue;

				x = ( nCnt  - pt.y ) % nWidth;
				y = ( nCnt  - pt.y ) / nWidth;
				
				FLItemElem* pItemElem = GetItem( i );
				
				if( pItemElem == NULL ) continue;

				if( pItemElem->GetTexture() )
				{
					//sun: 8차 엔젤 소환 Neuz, World, Trans
					if( ( pItemElem->m_nItemNum == pItemElem->GetExtra() || !m_useDieFlag ) 
						&& ( IsUsingItem(pItemElem) || pItemElem->m_bRepair 
						|| ( nParent == APP_INVENTORY && g_pPlayer->GetPetId() == pItemElem->m_dwObjId )	//sun: 9, 9-10차 펫
						|| pItemElem->IsFlag( FLItemElem::is_using ) != FALSE ) )
					{
						RanderIcon( p2DRender, pItemElem, x * SLOTSIZE, y * SLOTSIZE_SY, nParent, 180 );

						if( m_pTexItemCheck )
							m_pTexItemCheck->Render( p2DRender, CPoint( x * SLOTSIZE, y * SLOTSIZE_SY ) );

					}
//					if( IsUsingItem( pItemElem ) || pItemElem->m_bRepair )		//sun: 8차 엔젤 소환 Neuz, World, Trans

					else if( m_nOnSelect == i )
					{
						pItemElem->GetTexture()->m_size.cx += 4;
						pItemElem->GetTexture()->m_size.cy += 4;
//sun: 9차 전승관련 Clienet
						if( ( pItemElem->m_dwItemId == ITEM_INDEX( 21035, II_SYS_SYS_SCR_PET_FEED_POCKET ) && pItemElem->m_dwKeepTime <= 0 )
							|| ( g_xSpecManager->IsChargeZoneTicket( pItemElem->m_dwItemId ) != false && pItemElem->IsFlag( FLItemElem::is_using ) != FALSE )
							)
							RanderIcon( p2DRender, pItemElem, x * SLOTSIZE - 2, y * SLOTSIZE_SY - 2, nParent, 100 );
						else
							RanderIcon( p2DRender, pItemElem, x * SLOTSIZE - 2, y * SLOTSIZE_SY - 2, nParent );


						pItemElem->GetTexture()->m_size.cx -= 4;
						pItemElem->GetTexture()->m_size.cy -= 4;
					}
					else
					{
//sun: 9차 전승관련 Clienet
						if(pItemElem->m_dwItemId == ITEM_INDEX( 21035, II_SYS_SYS_SCR_PET_FEED_POCKET ) && pItemElem->m_dwKeepTime <= 0) //먹이 주머니 아이템이 사용안된 상태인가?
							RanderIcon( p2DRender, pItemElem, x * SLOTSIZE, y * SLOTSIZE_SY, nParent, 100 );
						else						
							RanderIcon( p2DRender, pItemElem, x * SLOTSIZE, y * SLOTSIZE_SY, nParent );
					}

					if( pItemElem->GetProp()->dwPackMax > 1 )
					{
						int m_nItemNum	= pItemElem->m_nItemNum;
						if( pItemElem->GetExtra() > 0 )
							m_nItemNum	-= pItemElem->GetExtra();			
						TCHAR szTemp[ 32 ];
						FLSPrintf( szTemp, _countof( szTemp ), "%d", m_nItemNum );

						CSize size = m_p2DRender->m_pFont->GetTextExtent( szTemp );
						m_p2DRender->TextOut( x * SLOTSIZE + SLOTSIZE - size.cx, y * SLOTSIZE_SY + SLOTSIZE - size.cy, szTemp, pFontColorWndItemCtrl->m_stItemCountShadow.GetFontColor() );
						m_p2DRender->TextOut( x * SLOTSIZE + (SLOTSIZE-1) - size.cx, y * SLOTSIZE_SY + (SLOTSIZE-1) - size.cy, szTemp, pFontColorWndItemCtrl->m_stItemCount.GetFontColor() );
					}

					DWORD dwGroup = g_pPlayer->m_cooltimeMgr.GetGroup( pItemElem->GetProp() );
					if( dwGroup )
					{
						DWORD dwEnd = g_pPlayer->m_cooltimeMgr.GetTime( dwGroup );		// 이벤트 종료 시각 
						if( dwEnd != 0 && dwEnd > dwCur )
						{
							DWORD dwBase = g_pPlayer->m_cooltimeMgr.GetBase( dwGroup );	// 이벤트 시작 시각 
							CPoint pt( x * SLOTSIZE, y * SLOTSIZE_SY );
							RenderRadar( p2DRender, pt, dwCur - dwBase, dwEnd - dwBase );
						}
					}

					CWndInventory* pWndInventory	= (CWndInventory*)GetWndBase( APP_INVENTORY );
					if( pWndInventory )
					{
						if( pWndInventory->m_pUpgradeItem && pItemElem )
						{
							if( pWndInventory->m_pUpgradeItem == pItemElem )
							{
								if( pWndInventory->m_dwEnchantWaitTime != 0xffffffff )
								{
									CPoint pt( x * SLOTSIZE, y * SLOTSIZE_SY );
									RenderEnchant( p2DRender, pt );
								}
							}
						}
					}
				}
				CRect rectHittest = CRect( x * SLOTSIZE, y * SLOTSIZE_SY, x * SLOTSIZE + SLOTSIZE - 2, y * SLOTSIZE_SY + SLOTSIZE - 2 );
			}
		}
		else if(nParent == APP_BAG_EX )
		{
			CPoint pt_bag;
			int nWidth = rect.Width() / 32;
			int nHeight = rect.Height() / 32;
			m_wndScrollBar.SetScrollRange( 0, 0 );
			m_wndScrollBar.SetScrollPage( 0 );
			// 스크롤바 관련   
			pt_bag.y = 0;
			pt_bag.y += m_wndScrollBar.GetScrollPos() * nWidth;

			for( DWORD i = pt_bag.y;i < m_pInvenSlotPorc->m_pItemContainer->GetMax() ; i++ ) 
			{
				if( i  < 0 ) continue;
				int x = ( i  - pt_bag.y ) % nWidth;
				int y = ( i  - pt_bag.y ) / nWidth;
				FLItemElem* pItemElem = GetItem( i );

				if( pItemElem == NULL ) continue;

				if( pItemElem->GetTexture() )
				{
					//sun: 8차 엔젤 소환 Neuz, World, Trans
					if( ( pItemElem->m_nItemNum == pItemElem->GetExtra() || !m_useDieFlag ) 
						&& ( IsUsingItem(pItemElem) || pItemElem->m_bRepair 
						|| ( nParent == APP_INVENTORY && g_pPlayer->GetPetId() == pItemElem->m_dwObjId )	//sun: 9, 9-10차 펫
						|| ( g_xSpecManager->IsChargeZoneTicket( pItemElem->m_dwItemId ) != false && pItemElem->IsFlag( FLItemElem::is_using ) != FALSE ) ))
						RanderIcon( p2DRender, pItemElem, x * SLOTSIZE, y * SLOTSIZE_SY, nParent, 60 );

					//					if( IsUsingItem( pItemElem ) || pItemElem->m_bRepair )		//sun: 8차 엔젤 소환 Neuz, World, Trans

					else if( m_nOnSelect == i )
					{
						pItemElem->GetTexture()->m_size.cx += 4;
						pItemElem->GetTexture()->m_size.cy += 4;
						//sun: 9차 전승관련 Clienet
						if( ( pItemElem->m_dwItemId == ITEM_INDEX( 21035, II_SYS_SYS_SCR_PET_FEED_POCKET ) && pItemElem->m_dwKeepTime <= 0 )
							|| ( g_xSpecManager->IsChargeZoneTicket( pItemElem->m_dwItemId ) != false && pItemElem->IsFlag( FLItemElem::is_using ) != FALSE )
							)
							RanderIcon( p2DRender, pItemElem, x * SLOTSIZE - 2, y * SLOTSIZE_SY - 2, nParent, 100 );
						else
							RanderIcon( p2DRender, pItemElem, x * SLOTSIZE - 2, y * SLOTSIZE_SY - 2, nParent );


						pItemElem->GetTexture()->m_size.cx -= 4;
						pItemElem->GetTexture()->m_size.cy -= 4;
					}
					else
					{
						//sun: 9차 전승관련 Clienet
						if(pItemElem->m_dwItemId == ITEM_INDEX( 21035, II_SYS_SYS_SCR_PET_FEED_POCKET ) && pItemElem->m_dwKeepTime <= 0) //먹이 주머니 아이템이 사용안된 상태인가?
							RanderIcon( p2DRender, pItemElem, x * SLOTSIZE, y * SLOTSIZE_SY, nParent, 100 );
						else						
							RanderIcon( p2DRender, pItemElem, x * SLOTSIZE, y * SLOTSIZE_SY, nParent );
					}

					if( pItemElem->GetProp()->dwPackMax > 1 )
					{
						int m_nItemNum	= pItemElem->m_nItemNum;
						if( pItemElem->GetExtra() > 0 )
							m_nItemNum	-= pItemElem->GetExtra();			
						TCHAR szTemp[ 32 ];
						FLSPrintf( szTemp, _countof( szTemp ), "%d", m_nItemNum );
						CSize size = m_p2DRender->m_pFont->GetTextExtent( szTemp );
						m_p2DRender->TextOut( x * SLOTSIZE + SLOTSIZE - size.cx, y * SLOTSIZE_SY + SLOTSIZE - size.cy, szTemp, pFontColorWndItemCtrl->m_stItemCountShadow.GetFontColor() );
						m_p2DRender->TextOut( x * SLOTSIZE + (SLOTSIZE-1) - size.cx, y * SLOTSIZE_SY + (SLOTSIZE-1) - size.cy, szTemp, pFontColorWndItemCtrl->m_stItemCount.GetFontColor() );
					}

					DWORD dwGroup = g_pPlayer->m_cooltimeMgr.GetGroup( pItemElem->GetProp() );
					if( dwGroup )
					{
						DWORD dwEnd = g_pPlayer->m_cooltimeMgr.GetTime( dwGroup );		// 이벤트 종료 시각 
						if( dwEnd != 0 && dwEnd > dwCur )
						{
							DWORD dwBase = g_pPlayer->m_cooltimeMgr.GetBase( dwGroup );	// 이벤트 시작 시각 
							CPoint pt( x * SLOTSIZE, y * SLOTSIZE_SY );
							RenderRadar( p2DRender, pt, dwCur - dwBase, dwEnd - dwBase );
						}
					}

					CWndInventory* pWndInventory	= (CWndInventory*)GetWndBase( APP_INVENTORY );
					if( pWndInventory )
					{
						if( pWndInventory->m_pUpgradeItem && pItemElem )
						{
							if( pWndInventory->m_pUpgradeItem == pItemElem )
							{
								if( pWndInventory->m_dwEnchantWaitTime != 0xffffffff )
								{
									CPoint pt( x * SLOTSIZE, y * SLOTSIZE_SY );
									RenderEnchant( p2DRender, pt );
								}
							}
						}
					}
				}
				CRect rectHittest = CRect( x * SLOTSIZE, y * SLOTSIZE_SY, x * SLOTSIZE + SLOTSIZE - 2, y * SLOTSIZE_SY + SLOTSIZE - 2 );
			}
		}
		else
		{
			m_nArrayCount = 0;

			if( nParent == APP_SHOP_ )
			{
				CWndShop* pWndBase = (CWndShop*)g_WndMng.GetWndBase( APP_SHOP_ );
				for( DWORD i = 0; i < m_pInvenSlotPorc->m_pItemContainer->GetMax(); i++ ) 
				{
					FLItemElem* pItemElem = GetItem( i );
					if( pItemElem == NULL ) 
						continue;

					PT_ITEM_SPEC pItemProp		= pItemElem->GetProp();
					if( pItemProp == NULL ) 
						continue;

					if( pWndBase )
					{
						if( pWndBase->m_bSexSort && pWndBase->m_bLevelSort )
						{
							if( pItemProp->dwItemSex != 0xffffffff )
							{
								if( pItemProp->dwItemSex == g_pPlayer->GetSex() && pItemElem->CanUseLevel( g_pPlayer ) != false )
									m_pArrayItemElem[m_nArrayCount++] = pItemElem;
							}
							else if( pItemProp->dwItemSex != 0xffffffff )
							{
								if( pItemProp->dwItemSex == g_pPlayer->GetSex() )
									m_pArrayItemElem[m_nArrayCount++] = pItemElem;
							}
							else if( pItemElem->CanUseLevel( g_pPlayer ) != false )
							{
								m_pArrayItemElem[m_nArrayCount++] = pItemElem;
							}
							else
								m_pArrayItemElem[m_nArrayCount++] = pItemElem;
						}
						else
						if( pWndBase->m_bSexSort )
						{
							if( pItemProp->dwItemSex != 0xffffffff )
							{
								if( pItemProp->dwItemSex == g_pPlayer->GetSex() )
									m_pArrayItemElem[m_nArrayCount++] = pItemElem;
							}
							else
								m_pArrayItemElem[m_nArrayCount++] = pItemElem;
						}
						else
						if( pWndBase->m_bLevelSort )
						{
							if( pItemElem->CanUseLevel( g_pPlayer ) != false )
							{
								m_pArrayItemElem[m_nArrayCount++] = pItemElem;
							}
							else
								m_pArrayItemElem[m_nArrayCount++] = pItemElem;
						}
						else
							m_pArrayItemElem[m_nArrayCount++] = pItemElem;
					}
				}
			}
			else if(nParent == APP_SHOP_CART_) // _JIN_SHOP_CART
			{
				CFLWndShop_Cart* pWndBase = (CFLWndShop_Cart*)g_WndMng.GetWndBase( APP_SHOP_CART_ );
				for( DWORD i = 0; i < m_pInvenSlotPorc->m_pItemContainer->GetMax(); i++ ) 
				{
					int n = m_pInvenSlotPorc->m_pItemContainer->GetSize();
					FLItemElem* pItemElem = GetItem( i );
					if( pItemElem == NULL ) 
						continue;

					PT_ITEM_SPEC pItemProp		= pItemElem->GetProp();
					if( pItemProp == NULL ) 
						continue;

					if( pWndBase )
					{
						if( pWndBase->m_bSexSort && pWndBase->m_bLevelSort )
						{
							if( pItemProp->dwItemSex != 0xffffffff )
							{
								if( pItemProp->dwItemSex == g_pPlayer->GetSex() && pItemElem->CanUseLevel( g_pPlayer ) != false )
									m_pArrayItemElem[m_nArrayCount++] = pItemElem;
							}
							else
								if( pItemProp->dwItemSex != 0xffffffff )
								{
									if( pItemProp->dwItemSex == g_pPlayer->GetSex() )
										m_pArrayItemElem[m_nArrayCount++] = pItemElem;
								}
								else
									if( pItemElem->CanUseLevel( g_pPlayer ) != false )
									{
										m_pArrayItemElem[m_nArrayCount++] = pItemElem;
									}
									else
										m_pArrayItemElem[m_nArrayCount++] = pItemElem;
						}
						else
						{
							if( pWndBase->m_bSexSort )
							{
								if( pItemProp->dwItemSex != 0xffffffff )
								{
									if( pItemProp->dwItemSex == g_pPlayer->GetSex() )
										m_pArrayItemElem[m_nArrayCount++] = pItemElem;
								}
								else
									m_pArrayItemElem[m_nArrayCount++] = pItemElem;
							}
							else if( pWndBase->m_bLevelSort )
							{
								if( pItemElem->CanUseLevel( g_pPlayer ) != false )
								{
									m_pArrayItemElem[m_nArrayCount++] = pItemElem;
								}
								else
									m_pArrayItemElem[m_nArrayCount++] = pItemElem;
							}
							else
								m_pArrayItemElem[m_nArrayCount++] = pItemElem;
						}
					}
				}
			}
			else
			{
				for( DWORD i = 0; i < m_pInvenSlotPorc->m_pItemContainer->GetMax(); i++ ) 
				{
					FLItemElem* pItemElem = GetItem( i );	
					if( pItemElem == NULL ) 
						continue;
					
					PT_ITEM_SPEC pItemProp		= pItemElem->GetProp();					
					if( pItemProp == NULL ) 
						continue;
					
					m_pArrayItemElem[m_nArrayCount++] = pItemElem;
				}
			}	
			CPoint pt_cart;
			int nWidth = rect.Width() / 32;
			int nHeight = rect.Height() / 32;
			int nPage_Cart = nHeight;
			int nRange_Cart = 0;
			
			//gmpbigsun( 20110819 ) : 개인창고(뱅크)의 경우 최대 저장량까지 비어있대래도 보여줘야한다.( #16112 )
			//기존에는 MAX_SHOP_BASESLOT를 기준값으로 사용했으나, 어떤경우에는 현재수량과 상관없이 맥스량만큼 보여줘야하는경우 
			//처리가 안되어있었다.( 위쪽코드를 보면 인벤도 아니고, BAK_EX도 아닌경우다 )
			int nMaxBaseSlot = MAX_SHOP_BASESLOT;
			if( APP_COMMON_BANK == nParent )
				nMaxBaseSlot = MAX_BANK;

			if(m_nArrayCount < nMaxBaseSlot)		//아이템갯수가 ShopPage의 고정값보다 작으면 
			{
				nRange_Cart = nMaxBaseSlot / nWidth; //고정값으로 계산을한다. 그렇지 않으면 페이지의 길이가 늘어남
				if( nMaxBaseSlot % nWidth )
					nRange_Cart++;
			}	
			else
			{ 
				nRange_Cart = m_nArrayCount / nWidth;
				if( m_nArrayCount % nWidth )
					nRange_Cart++;
			}			 
		
			m_wndScrollBar.SetScrollRange( 0, nRange_Cart );
			m_wndScrollBar.SetScrollPage( nPage_Cart );
			pt_cart.y = 0;
			pt_cart.y += m_wndScrollBar.GetScrollPos() * nWidth;

			for( int i = 0; i < m_nArrayCount; i++ ) 
			{
				// 스크롤바 관련   
				int x = ( i - pt_cart.y ) % nWidth;
				int y = ( i - pt_cart.y ) / nWidth;
				FLItemElem* pItemElem = m_pArrayItemElem[i];
				if( pItemElem == NULL ) 
					continue;

				if( pItemElem->m_pTexture )
				{
					if( IsUsingItem( pItemElem ) || pItemElem->m_bRepair )
						RanderIcon( p2DRender, pItemElem, x * SLOTSIZE, y * SLOTSIZE_SY, nParent, 60 );
					else 
						if( m_nOnSelect == i )
						{
							pItemElem->GetTexture()->m_size.cx += 4;
							pItemElem->GetTexture()->m_size.cy += 4;
							RanderIcon( p2DRender, pItemElem, x * SLOTSIZE - 2, y * SLOTSIZE_SY - 2, nParent );
							pItemElem->GetTexture()->m_size.cx -= 4;
							pItemElem->GetTexture()->m_size.cy -= 4;
						}
						else
							RanderIcon( p2DRender, pItemElem, x * SLOTSIZE, y * SLOTSIZE_SY, nParent );

					if( pItemElem->GetProp()->dwPackMax > 1 )
					{
						int m_nItemNum	= pItemElem->m_nItemNum;
						if( pItemElem->GetExtra() > 0 )
							m_nItemNum	-= pItemElem->GetExtra();			
						if( m_bVisibleCount )  
						{
							TCHAR szTemp[ 32 ];
							FLSPrintf( szTemp, _countof( szTemp ), "%d", m_nItemNum );
							CSize size = m_p2DRender->m_pFont->GetTextExtent( szTemp );
							m_p2DRender->TextOut( x * SLOTSIZE + 32 - size.cx, y * SLOTSIZE_SY + 32 - size.cy, szTemp, pFontColorWndItemCtrl->m_stItemCountShadow.GetFontColor() );
							m_p2DRender->TextOut( x * SLOTSIZE + 31 - size.cx, y * SLOTSIZE_SY + 31 - size.cy, szTemp, pFontColorWndItemCtrl->m_stItemCount.GetFontColor() );
						}
					}
				}
			}
		}
	}
}

//---------------------------------------------------------------------------------------------
// 마우스 왼쪽 버튼 눌름
// param	: nFlags, point ( 마우스 좌표 )
// return	: void
//---------------------------------------------------------------------------------------------
void CWndItemCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	FLERROR_ASSERT_LOG_RETURN_VOID( g_pPlayer, _T("[윈도우 아이템 컨트롤] 플레이어 포인터가 유효하지 않음") );
	
	if(_GetContentState(CT_NEW_INVENTORY) == CS_VER2)
	{
		if( !m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel] )
			return;
	}
	else
	{
		if( !m_pInvenSlotPorc->m_pItemContainer )
			return;
	}

	CPoint pt( 3, 3 );
	CRect rect;

	if(	CWndBase::m_GlobalShortcut.IsEmpty() == FALSE )
		return;

	int nItem = HitTest( point );
	if( nItem == -1 )
		return;

	CWndBase*	pWndBase( this->GetParentWnd() );
	int			nParent( NULL );
	
	if( pWndBase )
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();

		if( pWndBaseTemp )
			nParent = pWndBaseTemp->GetWndId();
		else
			return;
	}
	else
		return;

	FLItemElem* pItemElem = NULL;

	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
//sun: 11, 주머니
	if( nParent == APP_INVENTORY || nParent == APP_BAG_EX)
	{
		if(!(nParent == APP_BAG_EX)) 
			if( pWndInventory->m_dwEnchantWaitTime != 0xffffffff || GetWndBase(APP_SMELT_SAFETY_CONFIRM) != NULL )
			{
				g_WndMng.PutString( prj.GetText(TID_MMI_NOTUPGRADE), NULL, prj.GetTextColor(TID_MMI_NOTUPGRADE) );			
				m_pFocusItem = NULL;
				return;
			}

		if( GetWndBase(APP_EQUIP_BIND_CONFIRM) != NULL )
		{
			g_WndMng.PutString( prj.GetText(TID_TOOLTIP_EQUIPBIND_ERROR01), NULL, prj.GetTextColor(TID_TOOLTIP_EQUIPBIND_ERROR01) );
			m_pFocusItem = NULL;
			return;
		}

		if( GetWndBase(APP_COMMITEM_DIALOG) != NULL )
		{
			g_WndMng.PutString( prj.GetText(TID_TOOLTIP_ITEM_USING_ERROR), NULL, prj.GetTextColor(TID_TOOLTIP_ITEM_USING_ERROR) );
			m_pFocusItem = NULL;
			return;
		}

		pItemElem = GetItem( nItem );
	}
	else
	{
		pItemElem = m_pArrayItemElem[nItem];
	}
	
	if( pItemElem )
	{
		if( pItemElem->m_bControlForbidden == TRUE )
		{
			return;
		}

#ifdef __HYPERLINK_ITEM16
		//gmpbigsun(20101116) : 인벤아이템만 링크허용
		if( ( GetAsyncKeyState( VK_CONTROL ) & 0x8000 ) && ( nParent == APP_INVENTORY ) ) //|| GetAsyncKeyState( VK_RSHIFT ) & 0x8000 )
		{
			CWndChat* pWndChat = (CWndChat*)g_WndMng.CreateApplet( APP_COMMUNICATION_CHAT );
			if( pWndChat == NULL )
			{
				return;
			}

			pWndChat->SetVisible( TRUE );		//링크를 시도함과 동시에 채팅창을 활성화 한다.

			CString strFullItemName = _T( "" );
			CString strItemName = pItemElem->GetName();
			if( pItemElem->GetAbilityOption() > 0 )
			{
				CString strAbilityOption = _T( "" );
				strAbilityOption.Format( _T( " %+d" ), pItemElem->GetAbilityOption() );
				strItemName += strAbilityOption;
			}

			/// 아이템 링크도 색상을 쓰기 때문에 걸러진다.
			/// 아래와 같이 아이템링크시에는 $a, $na로 안걸러지게 처리한다.
			DWORD dwItemNameTextColor = g_WndMng.GetItemTitleTextColor( pItemElem );
			strFullItemName.Format( _T( "$a$i%x%0.8d%0.4d<%s>$ni$na" ), dwItemNameTextColor, g_pPlayer->m_idPlayer, pItemElem->m_dwObjId, strItemName );
			
			//아이템 링크를 시도함
			//gmpbigsun(100719)_WKG BEGIN: 채팅 문자열을 보내는 주체는 엔터와 동시에 해당글을 클라side에서 바로 채팅윈도로 올린다
			// 원본 문자열만 보존해놓고 보내는 주체이기에 ( 채팅을 받았다고 치고 파싱 ) 그대로 AddString한다.
			HLITEM_DATA& pData = pWndChat->m_kTempHLData_Send;
			if( 0 != pData._masterID )
			{
//				g_WndMng.OpenMessageBox( "일단 한개만 하세요 ㅠ_ㅠ" );
				return;
			}

			PT_ITEM_SPEC pProp = pItemElem->GetProp( );
			if( !pProp )
				return;

			pData._szOrg = strFullItemName;		// < 소라의검+3 > => $i< ffffffff(masterid)(itemoid)소라의검+3 >$ni
			pData._masterID = g_pPlayer->m_idPlayer;
			//END

			pWndChat->m_wndEdit.AddString( strFullItemName );	//AddString안에서 파싱된다.
			pWndChat->m_wndEdit.SetFocus();
			return;
		}
#endif // __HYPERLINK_ITEM16

		//sun: 8차 엔젤 소환 Neuz, World, Trans
		if( (pItemElem->m_nItemNum != pItemElem->GetExtra() && m_useDieFlag) || 
			( IsUsingItem( pItemElem ) == FALSE && pItemElem->m_bRepair == FALSE) )
		{
			//if( m_useDieFlag || IsSelectedItem( nItem ) == FALSE )
			{
				m_dwSelectAry.RemoveAll();
				m_dwSelectAry.Add( nItem );
				m_nCurSelect = nItem;
				m_pFocusItem = pItemElem;
			}
			m_bDrag = TRUE;
		}

		if( OnLButtonDownEventProcess_UnSeal( nParent ) )
			return ;

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
			int nRange = 0;
			if(_GetContentState(CT_NEW_INVENTORY) == CS_VER2)
			{
				nRange = m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel]->GetSize()/ nWidth;
				if( m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel]->GetSize() % nWidth )
					nRange++;
			}
			else
			{
				nRange = m_pInvenSlotPorc->m_pItemContainer->GetSize()/ nWidth;
				if( m_pInvenSlotPorc->m_pItemContainer->GetSize() % nWidth )
					nRange++;
			}
			
			m_wndScrollBar.SetScrollRange( 0, nRange );
			m_wndScrollBar.SetScrollPage( nPage );

			pt.y = 0;
			pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

			int x = ( nItem - pt.y ) % nWidth;
			int y = ( nItem - pt.y ) / nWidth;

			if(pWndInventory->m_bIsUpgradeMode && pWndInventory->m_pUpgradeMaterialItem != NULL && m_pFocusItem != NULL)
			{
				if(!g_pPlayer->HasBuff(BUFF_ITEM, ITEM_INDEX( 10464, II_SYS_SYS_SCR_SMELPROT )) && 
					( pWndInventory->m_pUpgradeMaterialItem->m_dwItemId == ITEM_INDEX( 2035, II_GEN_MAT_ORICHALCUM01 ) || pWndInventory->m_pUpgradeMaterialItem->m_dwItemId == ITEM_INDEX( 2082, II_GEN_MAT_ORICHALCUM01_1 ) ) && 
					m_pFocusItem->GetAbilityOption() >= 3 && FLItemElem::IsDiceRefineryAble(m_pFocusItem->GetProp()))
				{
					if(g_WndMng.m_pWndSmeltSafetyConfirm)
						SAFE_DELETE(g_WndMng.m_pWndSmeltSafetyConfirm)

					g_WndMng.m_pWndSmeltSafetyConfirm = new CWndSmeltSafetyConfirm(CWndSmeltSafetyConfirm::WND_ERROR1);

					if(g_WndMng.m_pWndSmeltSafetyConfirm)
					{
						CWndBase::m_GlobalShortcut.Empty();
						m_bDrag = FALSE;
						g_WndMng.m_pWndSmeltSafetyConfirm->SetWndMode(m_pFocusItem);
						g_WndMng.m_pWndSmeltSafetyConfirm->Initialize(NULL);
					}
				}
				else if(!g_pPlayer->HasBuff(BUFF_ITEM, ITEM_INDEX( 10488, II_SYS_SYS_SCR_SMELPROT3 )) && pWndInventory->m_pUpgradeMaterialItem->m_dwItemId == ITEM_INDEX( 2034, II_GEN_MAT_ORICHALCUM02 ) &&
						m_pFocusItem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE)
				{
					if(g_WndMng.m_pWndSmeltSafetyConfirm)
						SAFE_DELETE(g_WndMng.m_pWndSmeltSafetyConfirm)

					g_WndMng.m_pWndSmeltSafetyConfirm = new CWndSmeltSafetyConfirm(CWndSmeltSafetyConfirm::WND_ERROR2);

					if(g_WndMng.m_pWndSmeltSafetyConfirm)
					{
						CWndBase::m_GlobalShortcut.Empty();
						m_bDrag = FALSE;
						g_WndMng.m_pWndSmeltSafetyConfirm->SetWndMode(m_pFocusItem);
						g_WndMng.m_pWndSmeltSafetyConfirm->Initialize(NULL);
					}
				}
				else if(!g_pPlayer->HasBuff(BUFF_ITEM, ITEM_INDEX( 26473, II_SYS_SYS_SCR_SMELPROT4 )) && !g_pPlayer->HasBuff(BUFF_ITEM, ITEM_INDEX( 25233, II_SYS_SYS_SCR_SMELPROT6 )) && m_pFocusItem->GetAbilityOption() >= 3 && 
						(pWndInventory->m_pUpgradeMaterialItem->m_dwItemId == ITEM_INDEX( 2036, II_GEN_MAT_MOONSTONE ) || pWndInventory->m_pUpgradeMaterialItem->m_dwItemId == ITEM_INDEX( 2083, II_GEN_MAT_MOONSTONE_1 )) &&
						m_pFocusItem->GetProp()->dwItemKind2 == IK2_JEWELRY)
				{
					if(g_WndMng.m_pWndSmeltSafetyConfirm)
						SAFE_DELETE(g_WndMng.m_pWndSmeltSafetyConfirm)

					g_WndMng.m_pWndSmeltSafetyConfirm = new CWndSmeltSafetyConfirm(CWndSmeltSafetyConfirm::WND_ERROR3);

					if(g_WndMng.m_pWndSmeltSafetyConfirm)
					{
						CWndBase::m_GlobalShortcut.Empty();
						m_bDrag = FALSE;
						g_WndMng.m_pWndSmeltSafetyConfirm->SetWndMode(m_pFocusItem);
						g_WndMng.m_pWndSmeltSafetyConfirm->Initialize(NULL);
					}
				}
				else if( g_pPlayer->HasBuff( BUFF_ITEM, ITEM_INDEX( 10464, II_SYS_SYS_SCR_SMELPROT ) ) == FALSE && 
						 m_pFocusItem->GetResistAbilityOption() >= 3 && 
						 FLItemElem::IsElementalCard( pWndInventory->m_pUpgradeMaterialItem->m_dwItemId ) == TRUE && 
						 FLItemElem::IsEleRefineryAble( m_pFocusItem->GetProp() ) == TRUE )
				{
					if( g_WndMng.m_pWndSmeltSafetyConfirm )
						SAFE_DELETE( g_WndMng.m_pWndSmeltSafetyConfirm )

					g_WndMng.m_pWndSmeltSafetyConfirm = new CWndSmeltSafetyConfirm( CWndSmeltSafetyConfirm::WND_ERROR1 );
					if( g_WndMng.m_pWndSmeltSafetyConfirm )
					{
						CWndBase::m_GlobalShortcut.Empty();
						m_bDrag = FALSE;
						g_WndMng.m_pWndSmeltSafetyConfirm->SetWndMode( m_pFocusItem );
						g_WndMng.m_pWndSmeltSafetyConfirm->Initialize( NULL );
					}
				}
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
				else if( g_pPlayer->HasBuff( BUFF_ITEM, ITEM_INDEX( 25336, II_SYS_SYS_SCR_COSTUMEPROT ) ) == FALSE && 
						 m_pFocusItem->GetAbilityOption() >= 3 && 
						 pWndInventory->m_pUpgradeMaterialItem->m_dwItemId == ITEM_INDEX( 25334, II_GEN_MAT_LIGHTNINGSTONE ) && 
						 m_pFocusItem->GetProp()->IsCostumeEnhanceParts() )
				{
					if( g_WndMng.m_pWndSmeltSafetyConfirm )
						SAFE_DELETE( g_WndMng.m_pWndSmeltSafetyConfirm )

					g_WndMng.m_pWndSmeltSafetyConfirm = new CWndSmeltSafetyConfirm( CWndSmeltSafetyConfirm::WND_ERROR4 );
					if( g_WndMng.m_pWndSmeltSafetyConfirm )
					{
						CWndBase::m_GlobalShortcut.Empty();
						m_bDrag = FALSE;
						g_WndMng.m_pWndSmeltSafetyConfirm->SetWndMode( m_pFocusItem );
						g_WndMng.m_pWndSmeltSafetyConfirm->Initialize( NULL );
					}
				}
#endif
				else
					pWndInventory->RunUpgrade(m_pFocusItem);
			}
		}
	}
	else
	{
		m_dwSelectAry.RemoveAll();
		m_nCurSelect	= nItem;
		m_pFocusItem	= NULL;
	}
}


//---------------------------------------------------------------------------------------------
// 마우스 왼쪽 버튼 눌름 이벤트 처리 : 봉인 해제
// param	: nParent, pWndInventory 
// return	: bool
//---------------------------------------------------------------------------------------------
bool CWndItemCtrl::OnLButtonDownEventProcess_UnSeal( int nParent )
{
	if( nParent != APP_INVENTORY )
		return false;

	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	FLERROR_ASSERT_LOG_RETURN( pWndInventory, false, _T("[윈도우 아이템 컨트롤] 윈도우 인벤토리 포인터가 유효하지 않습니다.") );

	const FLItemBase* pUnSealKeyItem = pWndInventory->GetUnSealKeyItem();
	if( NULL == pUnSealKeyItem )
		return false;

	PT_ITEM_SPEC pItemSpec = m_pFocusItem->GetProp();
	FLERROR_ASSERT_LOG_RETURN( pItemSpec, false, _T("[윈도우 아이템 컨트롤] 아이템 스펙을 얻어올 수 없습니다.") );
	
	if( pItemSpec->dwItemKind2 == IK2_ELLDINPOTION &&
		pItemSpec->dwItemKind3 == IK3_SAVEPOTION )
	{
		if( FALSE == g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_ELLDINPOTION_TEXT11 ), MB_OKCANCEL, this ) )
		{
			FLERROR_ASSERT_LOG_RETURN( NULL, false, _T("[윈도우 아이템 컨트롤] 아이템 사용 메시지 박스 생성 실패.") );
		}
		else
		{
			g_WndMng.GetMessageBox()->AddWndStyle( WBS_MODAL );
			g_WndMng.GetMessageBox()->AddWndStyle( WBS_TOPMOST );

			pWndInventory->SetUnSealItem( m_pFocusItem );
		}
	}
	else
	{
		g_WndMng.PutString( prj.GetText(TID_MMI_ELLDINPOTION_TEXT07), NULL, prj.GetTextColor(TID_MMI_ELLDINPOTION_TEXT07) );
		pWndInventory->CancelUnSeal();
	}

	m_dwSelectAry.RemoveAll();
	m_nCurSelect = NULL;
	m_pFocusItem = NULL;

	return true;
}


//---------------------------------------------------------------------------------------------
// 명령이 왔을 때의 이벤트 처리
// param	: 윈도우 아이디, 메시지, 기본 윈도우
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CWndItemCtrl::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	if( pWndBase )
	{
		if( pWndBase == g_WndMng.GetMessageBox() )
		{
			CWndInventory* pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
			FLERROR_ASSERT_LOG_RETURN( pWndInventory, FALSE, _T("[윈도우 아이템 컨트롤] 윈도우 인벤토리 포인터가 유효하지 않습니다.") );

			if( IDOK == nID )
				pWndInventory->DoUnSeal();

			if( IDCANCEL == nID )
				pWndInventory->CancelUnSeal();
		}
	}

 	return CWndBase::OnCommand( nID, dwMessage, pWndBase );
}


void CWndItemCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{

	if(_GetContentState(CT_NEW_INVENTORY) == CS_VER2)
	{
		if( !m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel] )
			return;
	}
	else
	{
		if( !m_pInvenSlotPorc->m_pItemContainer )
			return;
	}
	
	//ReleaseCapture();
	m_bDrag = FALSE;
	//if(	CWndBase::m_GlobalShortcut.IsEmpty() == FALSE )
	//	return;
	int nItem = HitTest( point );
	if( nItem == -1 ) return;

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

	FLItemElem* pItemElem = NULL;

//sun: 11, 주머니
	// _JIN_SHOP_CART
	if( nParent == APP_INVENTORY || nParent == APP_BAG_EX|| nParent == APP_SHOP_CART_)
		pItemElem = GetItem( nItem );
	else
		pItemElem = m_pArrayItemElem[nItem];
	
	if( pItemElem )
	{
		if( (GetAsyncKeyState( VK_LCONTROL ) & 0x8000 ) )
		{
			int j = 0;
			for( ; j < m_dwSelectAry.GetSize(); j++ )
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
			m_pFocusItem = pItemElem;
		//	m_rect = rect;
		}
		else
		if( IsSelectedItem( nItem ) )
		{
			m_dwSelectAry.RemoveAll();
			m_dwSelectAry.Add( nItem );
			m_nCurSelect = nItem;
			m_pFocusItem = pItemElem;
		}
	}
}
void CWndItemCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if(_GetContentState(CT_NEW_INVENTORY) == CS_VER2)
	{
		if( !m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel] )
			return;
	}
	else
	{
		if( !m_pInvenSlotPorc->m_pItemContainer )
			return;
	}
	
	if( m_bDrag == FALSE )
		return;
	//FLTRACE_LOG( PROGRAM_NAME, _T( "hello" ) );
	m_bDrag = FALSE;
	CPoint pt( 3, 3 );
	CRect rect;
	//if( m_rect.PtInRect( point ) )
	//{
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

	FLItemElem* pItemElem = NULL;

//sun: 11, 주머니
	if( nParent == APP_INVENTORY || nParent == APP_BAG_EX )// _JIN_SHOP_CART
		pItemElem = GetItem( m_nCurSelect ); //
	else
		pItemElem = m_pArrayItemElem[m_nCurSelect];
	
//sun: 8차 엔젤 소환 Neuz, World, Trans
//sun: 9차 전승관련 Clienet
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
//sun: 11, 주머니
	if(!pWndInventory && nParent == APP_BAG_EX && pItemElem != NULL)
	{
		if(IsUsableItem( pItemElem ))
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
				FLERROR_LOG( PROGRAM_NAME, _T( "GetProp가 NULL ItemType : %d, ItemId : %d" ), 0, pItemElem->m_dwItemId );
			}
			FLStrcpy( m_GlobalShortcut.m_szString, _countof( m_GlobalShortcut.m_szString ), pItemElem->GetName() );
		}
		return;
	}

	if( ((pItemElem != NULL && m_useDieFlag) || IsUsableItem( pItemElem )) && !pWndInventory->m_bRemoveJewel)
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
			FLERROR_LOG( PROGRAM_NAME, _T( "GetProp가 NULL\nItemType : %d, ItemId : %d" ), 0, pItemElem->m_dwItemId );
		}
		FLStrcpy( m_GlobalShortcut.m_szString, _countof( m_GlobalShortcut.m_szString ), pItemElem->GetName() );
	}
}
FLItemElem* CWndItemCtrl::GetItem( DWORD dwIndex )
{
	FLItemElem* pItem;
	return	pItem = m_pInvenSlotPorc->m_pItemContainer->GetAt( dwIndex );
}

FLItemElem* CWndItemCtrl::GetItemFromArr( DWORD dwIndex )
{
	FLASSERT(dwIndex < 100);
	return m_pArrayItemElem[dwIndex];
}

int CWndItemCtrl::HitTest( CPoint point )
{
	if(_GetContentState(CT_NEW_INVENTORY) == CS_VER2)
	{
		if( !m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel] )
			return -1;
	}
	else
	{
		if( !m_pInvenSlotPorc->m_pItemContainer )
			return -1;
	}
	
	int nDstId = -1;
	CPoint pt( 3, 3 );
	CRect rect;
	int nItemRender = 0;
	DWORD dwItemSize = 0;
	//
	// 리포트  
	//
	if( 0 ) //m_dwListCtrlStyle == WLVS_REPORT )
	{
		if(_GetContentState(CT_NEW_INVENTORY) == CS_VER2)
		{
			dwItemSize = m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel]->GetSize();
// 			switch(m_nTabCurSel)
// 			{
// 			case 0:
// 				nItemRender = MAX_HUMAN_PARTS;
// 				break;
// 			case 1:
// 				nItemRender = MAX_INVENINDEX_GENERAL;
// 				break;
// 			case 2:
// 				nItemRender = MAX_INVENINDEX_PET;
// 				break;
// 			case 3:
// 				nItemRender = MAX_INVENINDEX_COSTUME;
// 				break;
// 			}
		}
		else
		{
			dwItemSize = m_pInvenSlotPorc->m_pItemContainer->GetSize();
		}
		pt.y -= (m_nFontHeight + 3) * m_wndScrollBar.GetScrollPos();
		for( int i = 0; i < (int)( dwItemSize ); i++ ) 
		{
			DWORD dwObjId = NULL_ID;
			FLItemElem* pItemElem = NULL;
			if(_GetContentState(CT_NEW_INVENTORY) == CS_VER2)
			{
				dwObjId = m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel]->m_apIndex[ i ];
				if( dwObjId >= dwItemSize )
				{
					continue;
				}
				pItemElem = m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel]->GetAtId(i);
			}
			else
			{
				dwObjId = m_pInvenSlotPorc->m_pItemContainer->m_apIndex[ i ];
				if( dwObjId >= dwItemSize )
				{
					continue;
				}
				pItemElem = m_pInvenSlotPorc->m_pItemContainer->GetAtId(i);
			}

			
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
		
	//sun: 11, 주머니
		if( nParent == APP_INVENTORY)
		{
			switch(_GetContentState(CT_NEW_INVENTORY))
			{
			case CS_VER1:
				{
					nItemRender = MAX_HUMAN_PARTS;
					dwItemSize = m_pInvenSlotPorc->m_pItemContainer->GetMax();
// 					for( int i = pt.y; i < (int)( m_pInvenSlotPorc->m_pItemContainer->GetMax() ); i++ ) 
// 					{
// 						int x = ( i - pt.y ) % nWidth;
// 						int y = ( i - pt.y ) / nWidth;
// 						rect.SetRect( x * 32, y * 32, x * 32 + 32, y * 32 + 32 );
// 						if( rect.PtInRect( point ) )
// 						{
// 							nDstId = i;
// 							break;
// 						}
// 					}
				}
				break;
			case CS_VER2:
				{
					switch(m_nTabCurSel)
					{
					case 0:
						nItemRender = MAX_HUMAN_PARTS;
						break;
					case 1:
						nItemRender = MAX_INVENINDEX_GENERAL;
						break;
					case 2:
						nItemRender = MAX_INVENINDEX_PET;
						break;
					case 3:
						nItemRender = MAX_INVENINDEX_COSTUME;
						break;
					}
					dwItemSize = m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel]->GetMax();
				}
				break;
			}
			int nCnt = 0;
			int x,y = 0;
			for( DWORD i = nItemRender; i < (int)( dwItemSize ); i++ ,nCnt++) 
			{
				x = ( nCnt  - pt.y ) % nWidth;
				y = ( nCnt  - pt.y ) / nWidth;
			
				rect.SetRect( x * SLOTSIZE, y * SLOTSIZE_SY, x * SLOTSIZE + SLOTSIZE, y * SLOTSIZE_SY + SLOTSIZE );
				if( rect.PtInRect( point ) )
				{
					nDstId = i;
					break;
				}
			}
		}
		else if( nParent == APP_BAG_EX )
		{
			for( int i = pt.y; i < (int)( m_pInvenSlotPorc->m_pItemContainer->GetMax() ); i++ ) 
			{
				int x = ( i - pt.y ) % nWidth;
				int y = ( i - pt.y ) / nWidth;
				rect.SetRect( x * SLOTSIZE, y * SLOTSIZE_SY, x * SLOTSIZE + SLOTSIZE, y * SLOTSIZE_SY + SLOTSIZE );
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
				rect.SetRect( x * SLOTSIZE, y * SLOTSIZE_SY, x * SLOTSIZE + SLOTSIZE, y * SLOTSIZE_SY + SLOTSIZE );
				if( rect.PtInRect( point ) )
				{
					nDstId = i;
					break;
				}
			}
		}
	}
	return nDstId;
}
BOOL CWndItemCtrl::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	if(_GetContentState(CT_NEW_INVENTORY) == CS_VER2)
	{
		if( !m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel] )
			return FALSE;
	}
	else
	{
		if( !m_pInvenSlotPorc->m_pItemContainer )
			return FALSE;
	}

	if( pShortcut->m_dwShortcut == SHORTCUT_ITEM )
	{
		int nDstIndex = HitTest( point );
		DWORD dwItemSize = 0;
		if(_GetContentState(CT_NEW_INVENTORY) == CS_VER2)
		{
			dwItemSize = m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel]->GetSize();
		}
		else
		{
			dwItemSize = m_pInvenSlotPorc->m_pItemContainer->GetSize();
		}
		if( nDstIndex == -1 ) nDstIndex = dwItemSize;
//			return FALSE;

		if( pShortcut->m_pFromWnd == this )
		{
			CWndBase* pWndFrame =  pShortcut->m_pFromWnd->GetFrameWnd();
			if( pWndFrame->GetWndId() == APP_BANK ) // 42 // Bank
			{
				g_DPlay.SendMoveBankItem( (BYTE)( pShortcut->m_dwIndex ), (BYTE)( (DWORD)nDstIndex ) );
			}
			else if( pWndFrame->GetWndId() == APP_INVENTORY )	// inventory -> inventory
			{
				FLItemElem* pItemElem = NULL;
				if(_GetContentState(CT_NEW_INVENTORY) == CS_VER2)
				{
					pItemElem = m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel]->GetAt( pShortcut->m_dwIndex );
				}
				else
				{
					pItemElem = m_pInvenSlotPorc->m_pItemContainer->GetAt( pShortcut->m_dwIndex );
				}
				if( IsUsingItem( pItemElem ) == FALSE )
					g_DPlay.SendMoveItem( pShortcut->m_dwIndex, (DWORD)nDstIndex );
			}
			else if( pWndFrame->GetWndId() == APP_SHOP_CART_ )// _JIN_SHOP_CART
			{
				FLItemElem* pItemElem	= m_pInvenSlotPorc->m_pItemContainer->GetAt( pShortcut->m_dwIndex );
				if( IsUsingItem( pItemElem ) == FALSE )
					g_DPlay.SendMoveItem( pShortcut->m_dwIndex, (DWORD)nDstIndex );
			}
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
				FLItemElem* pItemElem = &m_pItemContainer->m_apItem[ m_pItemContainer->m_apIndex[ i ] ] ;
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
		
		FLItemElem* pItemOld = &m_apItem[ CWndBase::m_GlobalShortcut.m_dwData ];
		for( int i = 0; m_pItemContainer->m_dwIndexNum; i++ )
		{
			FLItemElem* pItemElem = &m_apItem[ i ];
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
	if( !g_pPlayer )
		return;

	if(_GetContentState(CT_NEW_INVENTORY) == CS_VER2)
	{
		if( !m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel] )
			return;
	}
	else
	{
		if( !m_pInvenSlotPorc->m_pItemContainer )
			return;
	}


	if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 )
		return;
	if(	CWndBase::m_GlobalShortcut.IsEmpty() == FALSE )
		return;

	int nItem = HitTest( point );

	if( nItem == -1 )	return;
	CWndBase* pWndBase = this->GetParentWnd();

	int nParent = -1;
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

	FLItemElem* pItemElem = NULL;
	if(nParent == APP_SHOP_CART_)
	{
		pItemElem = m_pArrayItemElem[nItem];
	}
	else 
	{
		pItemElem = GetItem(nItem);
	}


	if( pItemElem == NULL )
	{
		return;
	}

	if( pItemElem->m_bControlForbidden == TRUE )
	{
		return;
	}

	if( IsUsableItem( pItemElem ) || (m_useDieFlag && pItemElem != NULL))
	{
		if( IsSelectedItem( nItem ) == FALSE )
		{
			m_dwSelectAry.RemoveAll();
			m_dwSelectAry.Add( nItem );
			m_nCurSelect = nItem;
			m_pFocusItem = pItemElem;
		}

		CWndBase* pParent = (CWndBase*)GetParentWnd();
		pParent->OnChildNotify( WIN_RCLK,m_nIdWnd,(LRESULT*)pItemElem); 
	}

	////중개거래
	//DoConsignmentMarketWorking( point );

	////양손무기합성
	//DoComposeTwohandWeaponWorking( point );

	////외형변경
	//DoChangeLookWorking( point );
	return;
}

void CWndItemCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{
	// 인벤창이 열려있고 인첸트 모드이면 커서모양 변경
	CWndInventory* pWndInventory	= (CWndInventory*)GetWndBase( APP_INVENTORY );

	if( pWndInventory && pWndInventory->m_dwEnchantWaitTime == 0xffffffff )
	{
		pWndInventory->BaseMouseCursor();
		if(g_WndMng.m_pWndSmeltSafetyConfirm != NULL)
			SAFE_DELETE(g_WndMng.m_pWndSmeltSafetyConfirm)
	}
	
	if(_GetContentState(CT_NEW_INVENTORY) == CS_VER2)
	{
		if( !m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel] )
			return;
	}
	else
	{
		if( !m_pInvenSlotPorc->m_pItemContainer )
			return;
	}


	CRect rectCtrl = GetClientRect();

	int nWidth = rectCtrl.Width() / 32;
	int nHeight = rectCtrl.Height() / 32;

	CPoint pt;
	pt.y = 0;
	pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
	int nItemRender = 0;
	DWORD dwItemSize = 0;
	switch(_GetContentState(CT_NEW_INVENTORY))
	{
	case CS_VER1:
		{
			nItemRender = MAX_HUMAN_PARTS;
			dwItemSize = m_pInvenSlotPorc->m_pItemContainer->GetMax(); 
		}
		break;
	case CS_VER2:
		{
			switch(m_nTabCurSel)
			{
			case 0:
				nItemRender = MAX_HUMAN_PARTS;
				break;
			case 1:
				nItemRender = MAX_INVENINDEX_GENERAL;
				break;
			case 2:
				{
					nItemRender = MAX_INVENINDEX_PET;
					//_JIN_COSTUME_MIX
					if(_GetContentState(CT_COSTUME_MIX) == CS_VER1)
					{
						int nItem = HitTest( point );
						if(nItem == -1 ) return;			
						FLItemElem* pItem = GetItem(nItem);
						if(pItem == NULL) return;
						if(g_WndMng.m_pCostume_NPC == NULL) return;
						CFLWndCostume_Mix* pWndCostume_Mix = (CFLWndCostume_Mix*)g_WndMng.GetWndBase(APP_COSTUME_MIXMENU);
						if(pWndCostume_Mix == NULL) return;
						g_WndMng.m_pCostume_NPC->GetCostumeMix()->SetSelCostumeItem( pItem );
						g_WndMng.m_pCostume_NPC->OnRButtonDown( nFlags, point );
					}
				}
				break;
			case 3:
				nItemRender = MAX_INVENINDEX_COSTUME;
				break;
			}
			dwItemSize = m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel]->GetMax(); 
		}
		break;
	}

	int nCnt = 0;
	int x,y = 0;
	for( DWORD i = nItemRender ; i < dwItemSize ; i++,nCnt++ )
	{
		if( nCnt < 0 ) continue;
		x = ( nCnt - pt.y ) % nWidth;
		y = ( nCnt - pt.y ) / nWidth;

		FLItemElem* pItemElem = GetItem( i );
		if( pItemElem == NULL ) continue;

		PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
	
		CRect rectHittest = CRect( x * SLOTSIZE, y * SLOTSIZE_SY, x * SLOTSIZE + SLOTSIZE - 2, y * SLOTSIZE_SY + SLOTSIZE - 2 );

		if( rectHittest.PtInRect( point ) )
		{
			if (pItemProp == NULL )
				continue;

			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &rectHittest );

			if( pItemProp->dwItemKind2 == IK2_TEXT ) 
			{
				CString strText = pItemProp->szTextFileName;
				//strText = strText.Left( 5 );
				strText.MakeLower();
				if( strText.Find( "book_" ) != -1 )
				{
					if ( !g_WndMng.m_pWndTextBook || g_WndMng.m_pWndTextBook->IsVisible() == FALSE)
						g_WndMng.OpenTextBook(this, pItemElem) ;
					else
						g_WndMng.ChangeTextBook(pItemElem) ;
				}
				else
				if( strText.Find( "scroll_" ) != -1  )
				{
					if ( !g_WndMng.m_pWndTextScroll || g_WndMng.m_pWndTextScroll->IsVisible() == FALSE)
						g_WndMng.OpenTextScroll(this, pItemElem) ;
					else
						g_WndMng.ChangeTextScroll(pItemElem) ;
				}
				else
				if( strText.Find( "letter_" ) != -1 )
				{
					if ( !g_WndMng.m_pWndTextLetter || g_WndMng.m_pWndTextLetter->IsVisible() == FALSE)
						g_WndMng.OpenTextLetter(this, pItemElem) ;
					else
						g_WndMng.ChangeTextLetter(pItemElem) ;
				}

			}
			else
			if( pItemProp->dwItemKind3 == IK3_QUEST )
			{
				if ( !g_WndMng.m_pQuestItemInfo || g_WndMng.m_pQuestItemInfo->IsVisible() == FALSE)
					g_WndMng.OpenQuestItemInfo(this, pItemElem) ;
				else
					g_WndMng.ChangeQuestItemInfo(pItemElem) ;
			}
		}
	}
}
BOOL CWndItemCtrl::OnSetCursor ( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
	// 인벤창이 열려있고 인첸트 모드이면 커서모양 변경
	CWndInventory* pWndInventory	= (CWndInventory*)GetWndBase( APP_INVENTORY );	
	if( pWndInventory )
		pWndInventory->ChangeMouseCursor();
	
	return TRUE;
}

void CWndItemCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if( !g_pPlayer )
		return;

	if(_GetContentState(CT_NEW_INVENTORY) == CS_VER2)
	{
		if( !m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel] )
			return;
	}
	else
	{
		if( !m_pInvenSlotPorc->m_pItemContainer )
			return;
	}


	if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 )
		return;
	if(	CWndBase::m_GlobalShortcut.IsEmpty() == FALSE )
		return;

	int nItem = HitTest( point );

	if( nItem == -1 )	return;
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

	FLItemElem* pItemElem = NULL;
	if(nParent == APP_SHOP_CART_)
	{
		pItemElem = m_pArrayItemElem[nItem];
	}
	else 
	{
		pItemElem = GetItem(nItem);
	}


	if( pItemElem == NULL )
	{
		return;
	}

	if( pItemElem->m_bControlForbidden == TRUE )
	{
		return;
	}

	if( IsUsableItem( pItemElem ) || (m_useDieFlag && pItemElem != NULL))
	{
		if( IsSelectedItem( nItem ) == FALSE )
		{
			m_dwSelectAry.RemoveAll();
			m_dwSelectAry.Add( nItem );
			m_nCurSelect = nItem;
			m_pFocusItem = pItemElem;
		}

		CWndBase* pParent = (CWndBase*)GetParentWnd();
		pParent->OnChildNotify( WIN_DBLCLK,m_nIdWnd,(LRESULT*)pItemElem); 
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

	if(m_pInvenSlotPorc == NULL) return;
	switch(_GetContentState(CT_NEW_INVENTORY))
	{
	case CS_VER1:
		{
			if( m_pInvenSlotPorc->m_pItemContainer ) 
			{
				if( 0 ) //m_dwListCtrlStyle == WLVS_REPORT )
				{
					int nPage = GetClientRect().Height() / (m_nFontHeight + 3);
					int nRange = m_pInvenSlotPorc->m_pItemContainer->GetSize();// - nPage;
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
					int nRange = m_pInvenSlotPorc->m_pItemContainer->GetSize() / nWidth;// - nPage;
					//if( !( nRange - nPage <= 0 ) )
					m_rectClient.right -= 15; // 스크롤 바가 보이면 
				}
			}

		}
		break;
	case CS_VER2:
		{
			if( m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel] ) 
			{
				if( 0 ) //m_dwListCtrlStyle == WLVS_REPORT )
				{
					int nPage = GetClientRect().Height() / (m_nFontHeight + 3);
					int nRange = m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel]->GetSize();// - nPage;
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
					int nRange = m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel]->GetSize() / nWidth;// - nPage;
					//if( !( nRange - nPage <= 0 ) )
					m_rectClient.right -= 15; // 스크롤 바가 보이면 
				}
			}
		}
		break;
	default:
		FLASSERT(0);
		break;
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
	if(_GetContentState(CT_NEW_INVENTORY) == CS_VER2)
	{
		if( 0 ) //m_dwListCtrlStyle == WLVS_REPORT )
		{
			nPage = GetClientRect().Height() / ( GetFontHeight( ) + 3 );
			nRange = m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel]->GetSize();// - nPage;
		}
		if( 1 ) //m_dwListCtrlStyle == WLVS_ICON )
		{
			CRect rect = GetClientRect();
			nPage = rect.Height() / 32;
			nRange = m_pInvenSlotPorc->m_InvenSlot[m_nTabCurSel]->GetSize() / ( rect.Width() / 32 );// - nPage;
		}
	}
	else
	{
		if( 0 ) //m_dwListCtrlStyle == WLVS_REPORT )
		{
			nPage = GetClientRect().Height() / ( GetFontHeight( ) + 3 );
			nRange = m_pInvenSlotPorc->m_pItemContainer->GetSize();// - nPage;
		}
		if( 1 ) //m_dwListCtrlStyle == WLVS_ICON )
		{
			CRect rect = GetClientRect();
			nPage = rect.Height() / 32;
			nRange = m_pInvenSlotPorc->m_pItemContainer->GetSize() / ( rect.Width() / 32 );// - nPage;
		}
	}
	
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );

	if( 0 ) //m_dwListCtrlStyle == WLVS_REPORT )
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

	size_t nTextSize = _tcslen( pItem->pszText ) + 1;
	pItems[ pItem->iSubItem ].pszText = new _TCHAR[ nTextSize ];
	FLStrcpy( pItems[ pItem->iSubItem ].pszText, nTextSize, pItem->pszText );
	return TRUE;
}
/*
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
*/
int CWndItemCtrl::GetItemCount() const
{
	return m_aItems.GetSize();
}
int CWndItemCtrl::InsertColumn( int nCol, const LVCOLUMN* pColumn )
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

void CWndItemCtrl::SetDieFlag2(BOOL flag)
{
	this->m_useDieFlag = flag;
}

