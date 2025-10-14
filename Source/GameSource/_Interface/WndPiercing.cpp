#include "stdafx.h"
#include "WndPiercing.h"

extern	CGuildMng	g_GuildMng;

///////////////////////////////////////////////////////////////////////////
// CPiercingMessageBox
//////////////////////////////////////////////////////////////////////////////
BOOL CPiercingMessageBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndMessageBox::Initialize( prj.GetText(TID_PIERCING_ERROR_NOTICE), 
		pWndParent, 
		MB_OKCANCEL );	
}

BOOL CPiercingMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{

	switch( nID )
	{
	case IDOK:
		{
			g_DPlay.SendPiercingSize( m_Objid[0], m_Objid[1], m_Objid[2] );
			Destroy();

			CWndPiercing* pWndPiercing = (CWndPiercing*)g_WndMng.GetWndBase( APP_PIERCING );
			if( pWndPiercing )
				pWndPiercing->Destroy();
		}
		break;
	case IDCANCEL:
		Destroy();
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 





///////////////////////////////////////////////////////////////////////////
// CWndPiercing
//////////////////////////////////////////////////////////////////////////////

CWndPiercing::CWndPiercing()
{
	SetPutRegInfo( FALSE );

	memset( m_pItemElem, 0, sizeof(m_pItemElem) );
	memset( m_Rect, 0, sizeof(m_Rect) );

	m_pPiercingMessageBox = NULL;
	m_pSfx = NULL;
}

CWndPiercing::~CWndPiercing()
{
//	SAFE_DELETE( m_pPiercingMessageBox );
	if( m_pSfx )
	{
		m_pSfx->Delete();
	}			
}

void CWndPiercing::OnDraw( C2DRender* p2DRender )
{
	for( int i=0; i<3; i++ )
	{
		if( m_pItemElem[i] && m_pItemElem[i]->GetTexture() )
		{
			m_pItemElem[i]->GetTexture()->Render( p2DRender, m_Rect[i].TopLeft(), 255 );

			if( m_pItemElem[i]->m_nItemNum > 1 )
			{
				TCHAR szTemp[32];
				FLSPrintf( szTemp, _countof( szTemp ), "%d", m_pItemElem[i]->GetExtra() );
				p2DRender->TextOut( m_Rect[i].right-11,  m_Rect[i].bottom-11 , szTemp, 0xff1010ff );
			}
			//*
			CRect hitrect = m_Rect[i];
			CPoint point = GetMousePoint();
			if( m_Rect[i].PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &hitrect );
				
				g_WndMng.PutToolTip_Item( (FLItemBase*)m_pItemElem[i], point2, &hitrect );
			}
			/**/
		}
	}

	CWndStatic* pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC9);		
	if( m_pItemElem[0] )
	{
		int nCost = 0;
		PT_ITEM_SPEC pItemProp = m_pItemElem[0]->GetProp();
		if( pItemProp )
//sun: 12, 무기 피어싱
			nCost = 100000;

		char buff[10] = { 0 };
		FLIntToString( nCost, buff, _countof( buff ), 10 );
		pWndFocusStatic->SetTitle( buff );		
	}
	else
	{
		pWndFocusStatic->SetTitle( "0" );		
	}	
}

void CWndPiercing::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate(); 

	CWndInventory* pWndInventory = (CWndInventory*)g_WndMng.CreateApplet( APP_INVENTORY );
	
	CRect rcInventory	= pWndInventory->GetWindowRect( TRUE );
	CRect rcVendor = GetWindowRect( TRUE );
	CPoint ptInventory	= rcInventory.TopLeft();
	CPoint point;
	if( ptInventory.x > m_pWndRoot->GetWndRect().Width() / 2 )
		point	= ptInventory - CPoint( rcVendor.Width(), 0 );
	else
		point	= ptInventory + CPoint( rcInventory.Width(), 0 );

	Move( point );

	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_STATIC5 );
	m_Rect[0] = pCustom->rect;
	pCustom = GetWndCtrl( WIDC_STATIC6 );
	m_Rect[1] = pCustom->rect;
	pCustom = GetWndCtrl( WIDC_STATIC7 );
	m_Rect[2] = pCustom->rect;

	CWndStatic* pGoldNum = (CWndStatic*) GetDlgItem( WIDC_STATIC9 );
	pGoldNum->AddWndStyle( WSS_MONEY );
	

	if( g_pPlayer )
		m_pSfx = CreateSfx( g_Neuz.m_pd3dDevice, XI_INDEX( 1716, XI_INT_INCHANT ), g_pPlayer->GetPos(), g_pPlayer->GetId(), g_pPlayer->GetPos(), g_pPlayer->GetId(), -1 );
} 

BOOL CWndPiercing::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PIERCING, 0, 0, pWndParent );
}

BOOL CWndPiercing::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

void CWndPiercing::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}

void CWndPiercing::OnLButtonUp( UINT nFlags, CPoint point ) 
{

}

void CWndPiercing::OnLButtonDown( UINT nFlags, CPoint point ) 
{

}
BOOL CWndPiercing::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame =  pShortcut->m_pFromWnd->GetFrameWnd();

	// 아이템이 인벤토리에서 왔는가?
	if( !(pShortcut->m_dwShortcut == SHORTCUT_ITEM) && !(pWndFrame->GetWndId() == APP_INVENTORY) )
		return FALSE;
		
	if( g_pPlayer->m_Inventory.IsEquip( pShortcut->m_dwId ) )
	{
		g_WndMng.PutString( prj.GetText(TID_GAME_EQUIPPUT), NULL, prj.GetTextColor(TID_GAME_EQUIPPUT) );
		SetForbid( TRUE );
		return FALSE;
	}

	FLItemElem* pItemElem  = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
	if(pItemElem == NULL)
		return FALSE;

	// 아이템( 방어구, 무기구 )
	if( PtInRect(&m_Rect[0], point) )
	{
// //sun: 9,10차 제련 __ULTIMATE
// 		if( pItemElem->GetProp()->dwItemKind3 != IK3_SUIT )
// 		{
// 			g_WndMng.PutString( prj.GetText(TID_PIERCING_POSSIBLE_ITEM), NULL, prj.GetTextColor(TID_PIERCING_POSSIBLE_ITEM) );
// 			return FALSE;
// 		}
		
		// 4개 초과로 피어싱 할수 없음
//sun: 12, 무기 피어싱
		if( pItemElem->GetProp()->IsBaruna() == TRUE 
#ifdef BARUNA_UPGRADE_SUIT_PIERCING
			&& pItemElem->GetProp()->dwItemKind3 != IK3_SUIT
#endif
)
		{
			g_WndMng.PutString( prj.GetText( TID_PIERCING_POSSIBLE_ITEM ), NULL, prj.GetTextColor( TID_PIERCING_POSSIBLE_ITEM ) );
			return FALSE;
		}

		if( !pItemElem->IsPierceAble( NULL_ID, TRUE ) )
		{
			g_WndMng.PutString( prj.GetText(TID_PIERCING_POSSIBLE), NULL, prj.GetTextColor(TID_PIERCING_POSSIBLE) );
			return FALSE;
		}
		
		if( pItemElem->GetProp()->dwItemRare == 0xffffffff )
		{
			g_WndMng.PutString( prj.GetText(TID_PIERCING_POSSIBLE_ITEM), NULL, prj.GetTextColor(TID_PIERCING_POSSIBLE_ITEM) );
			return FALSE;
		}

		if( m_pItemElem[0] )
			m_pItemElem[0]->SetExtra( 0 );

		pItemElem->SetExtra( 1 );
		m_pItemElem[0]	= pItemElem;	
		
		if( m_pItemElem[1] )
		{
			m_pItemElem[1]->SetExtra( 0 );
			m_pItemElem[1] = NULL;
		}
		if( m_pItemElem[2] )
		{
			m_pItemElem[2]->SetExtra( 0 );
			m_pItemElem[2] = NULL;
		}
	}
	else
	if( PtInRect(&m_Rect[1], point) )
	{
		// 파워다이스 8, 10만 사용할수 있음
		if( m_pItemElem[0] )
		{
//sun: 8차 제련 아이템 변경
			if( pItemElem->GetProp()->dwID != ITEM_INDEX( 2036, II_GEN_MAT_MOONSTONE ) && pItemElem->GetProp()->dwID != ITEM_INDEX( 2083, II_GEN_MAT_MOONSTONE_1 ) 
#ifdef BARUNA_UPGRADE_SUIT_PIERCING
				&& pItemElem->GetProp()->dwID != ITEM_INDEX( 25580, II_GEN_MAT_COSMOSTONE)
#endif
				)
//			if( pItemElem->GetProp()->dwID != II_GEN_MAT_DIE_EIGHT && pItemElem->GetProp()->dwID != II_GEN_MAT_DIE_TEN )	//sun: 8차 제련 아이템 변경( < 8 )
			{
				g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				return FALSE;
			}
			if( m_pItemElem[1] )
				m_pItemElem[1]->SetExtra( 0 );

			pItemElem->SetExtra( 1 );
			m_pItemElem[1]	= pItemElem;
		}
	}
	else
	if( PtInRect(&m_Rect[2], point) )
	{
		if( m_pItemElem[0] )
		{
			// 보조아이템은 상용화 아이템만 검사
			if( pItemElem->GetProp()->dwID != ITEM_INDEX( 10465, II_SYS_SYS_SCR_PIEPROT ) && pItemElem->GetProp()->dwID != ITEM_INDEX( 25234, II_SYS_SYS_SCR_PIEPROT2 ) 
#ifdef BARUNA_UPGRADE_SUIT_PIERCING
				&& pItemElem->GetProp()->dwID != ITEM_INDEX( 25581, II_SYS_SYS_SCR_PIEPROT3 )
#endif
				)
			{
				g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				return FALSE;
			}

			if( m_pItemElem[2] )
				m_pItemElem[2]->SetExtra( 0 );
			
			pItemElem->SetExtra( 1 );
			m_pItemElem[2]	= pItemElem;
		}
	}		

	return TRUE;
}

BOOL CWndPiercing::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
		case WIDC_OK:
			{
				if( !m_pItemElem[0] || !m_pItemElem[1] )
					return FALSE;

//				SAFE_DELETE( m_pPiercingMessageBox );
				m_pPiercingMessageBox = new CPiercingMessageBox;
				m_pPiercingMessageBox->m_Objid[0] = m_pItemElem[0]->m_dwObjId;
				m_pPiercingMessageBox->m_Objid[1] = m_pItemElem[1]->m_dwObjId;

				if( m_pItemElem[2] )
					m_pPiercingMessageBox->m_Objid[2] = m_pItemElem[2]->m_dwObjId;
				else
					m_pPiercingMessageBox->m_Objid[2] = NULL_ID;

				g_WndMng.OpenCustomBox( "", m_pPiercingMessageBox, this );				
			}
			break;
		case WIDC_CANCEL:
			{
				Destroy();
			}
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndPiercing::OnDestroyChildWnd( CWndBase* pWndChild )
{
}

void CWndPiercing::OnDestroy( void )
{
	for( int i=0; i<3; i++ )
	{
		if( m_pItemElem[i] )
			m_pItemElem[i]->SetExtra( 0 );
	}
}

void CWndPiercing::OnRButtonUp( UINT nFlags, CPoint point )
{
	if( m_pItemElem[0] && PtInRect(&m_Rect[0], point) )
	{
		m_pItemElem[0]->SetExtra( 0 );
		m_pItemElem[0] = NULL;

		if( m_pItemElem[1] )
		{
			m_pItemElem[1]->SetExtra( 0 );
			m_pItemElem[1] = NULL;
		}
		if( m_pItemElem[2] )
		{
			m_pItemElem[2]->SetExtra( 0 );
			m_pItemElem[2] = NULL;
		}
	}
	else
	if( m_pItemElem[1] && PtInRect(&m_Rect[1], point) )
	{
		m_pItemElem[1]->SetExtra( 0 );
		m_pItemElem[1] = NULL;
	}
	else
	if( m_pItemElem[2] && PtInRect(&m_Rect[2], point) )
	{
		m_pItemElem[2]->SetExtra( 0 );
		m_pItemElem[2] = NULL;
	}
}