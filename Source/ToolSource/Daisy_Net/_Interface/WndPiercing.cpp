#include "stdafx.h"
#include "defineitem.h"
#include "defineText.h"
#include "defineObj.h"
#include "AppDefine.h"
#include "WndPiercing.h"
#include "DPClient.h"

extern	CDPClient	g_DPlay;
extern	CGuildMng	g_GuildMng;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


///////////////////////////////////////////////////////////////////////////
// CPiercingMessageBox
//////////////////////////////////////////////////////////////////////////////
class CPiercingMessageBox : public CWndMessageBox
{ 
public: 
	CPiercingMessageBox() {};
	~CPiercingMessageBox() {};
	OBJID m_Objid[3];
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

BOOL CPiercingMessageBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndMessageBox::Initialize( prj.GetText(TID_PIERCING_ERROR_NOTICE), 
		pWndParent, 
		MB_OKCANCEL );	
}

BOOL CPiercingMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
#ifdef __PIERCING_SM_TIME

	switch( nID )
	{
	case IDOK:
#ifdef __ONLINE
		{
			g_DPlay.SendPiercingSize( m_Objid[0], m_Objid[1], m_Objid[2] );
			Destroy();
			CWndPiercing* pWndPiercing = (CWndPiercing*)g_WndMng.CreateApplet( APP_PIERCING );

			if( pWndPiercing )
				pWndPiercing->Destroy();
		}
#endif
		break;
	case IDCANCEL:
		Destroy();
		break;
	}

#endif //__PIERCING_SM_TIME
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 





///////////////////////////////////////////////////////////////////////////
// CWndPiercing
//////////////////////////////////////////////////////////////////////////////

CWndPiercing::CWndPiercing()
{
	SetPutRegInfo( FALSE );

	memset( &m_pItemElem, 0, sizeof(CItemBase*)*3 );
	memset( &m_Rect, 0, sizeof(CRect)*3 );

	m_pSfx = NULL;
}

CWndPiercing::~CWndPiercing()
{
	if( m_pSfx )
	{
		m_pSfx->Delete();
	}			
}

void CWndPiercing::OnDraw( C2DRender* p2DRender )
{
#ifdef __PIERCING_SM_TIME
	for( int i=0; i<3; i++ )
	{
		if( m_pItemElem[i] && m_pItemElem[i]->GetTexture() )
		{
			m_pItemElem[i]->GetTexture()->Render( p2DRender, m_Rect[i].TopLeft(), 255 );

			if( m_pItemElem[i]->m_nItemNum > 1 )
			{
				TCHAR szTemp[32];
				_stprintf( szTemp, "%d", m_pItemElem[i]->GetExtra() );
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
				
				g_WndMng.PutToolTip_Item( (CItemBase*)m_pItemElem[i], point2, &hitrect );
			}
			/**/
		}
	}

	CWndStatic* m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC9);		
	if( m_pItemElem[0] )
	{
//		int nCost = m_pItemElem[0]->GetProp()->dwItemRare * ( 200  + ( m_pItemElem[0]->m_piercingInfo.nPiercedSize+1) * 200 );
		int nCost = m_pItemElem[0]->GetProp()->dwItemRare * ( 1000 + m_pItemElem[0]->m_piercingInfo.nPiercedSize * 1000 );		
		char buff[10] = { 0 };
		m_pWndFocusStatic->SetTitle( itoa( nCost, buff, 10 ) );		
	}
	else
	{
		m_pWndFocusStatic->SetTitle( "0" );		
	}	
#endif //__PIERCING_SM_TIME
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

	if( g_pPlayer )
		m_pSfx = CreateSfx( g_Neuz.m_pd3dDevice, XI_INT_INCHANT, g_pPlayer->GetPos(), g_pPlayer->GetId(), g_pPlayer->GetPos(), g_pPlayer->GetId(), -1 );
} 

BOOL CWndPiercing::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{
#ifdef __PIERCING_SM_TIME
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PIERCING, 0, 0, pWndParent );
#else
	return TRUE;
#endif //__PIERCING_SM_TIME
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
#ifdef __PIERCING_SM_TIME
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
		
	CItemElem* pItemElem;
	pItemElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
	
	if(pItemElem == NULL)
		return FALSE;

	// 아이템( 방어구, 무기구 )
	if( PtInRect(&m_Rect[0], point) )
	{
#ifdef __S_ADD_PIERCING_ADD_CARD
		// 카드가 들어갈 아이템이( dwReferStat1 값이 PIERCING_1, PIERCING_2 ) 인지 검사
		if( !( pItemElem->GetProp()->dwReferStat1 == PIERCING_1 || pItemElem->GetProp()->dwReferStat1 == PIERCING_2 ) )
		{
			g_WndMng.PutString( prj.GetText(TID_PIERCING_POSSIBLE_ITEM), NULL, prj.GetTextColor(TID_PIERCING_POSSIBLE_ITEM) );
			return FALSE;
		}
#else // __S_ADD_PIERCING_ADD_CARD
		if( pItemElem->GetProp()->dwItemKind3 != IK3_SUIT )
		{
			g_WndMng.PutString( prj.GetText(TID_PIERCING_POSSIBLE_ITEM), NULL, prj.GetTextColor(TID_PIERCING_POSSIBLE_ITEM) );
			return FALSE;
		}
#endif // __S_ADD_PIERCING_ADD_CARD

		// 4개 초과로 피어싱 할수 없음
		if( pItemElem->m_piercingInfo.nPiercedSize >= MAX_PIERCING )
		{
			g_WndMng.PutString( prj.GetText(TID_PIERCING_POSSIBLE), NULL, prj.GetTextColor(TID_PIERCING_POSSIBLE) );
			return FALSE;
		}

		if( m_pItemElem[0] )
			m_pItemElem[0]->SetExtra( 0 );

		pItemElem->SetExtra( 1 );
		m_pItemElem[0]	= pItemElem;			
	}
	else
	if( PtInRect(&m_Rect[1], point) )
	{
		// 파워다이스 8, 10만 사용할수 있음
		if( pItemElem->GetProp()->dwID != II_GEN_MAT_DIE_EIGHT && pItemElem->GetProp()->dwID != II_GEN_MAT_DIE_TEN )
		{
			g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
			return FALSE;
		}
		
		if( m_pItemElem[1] )
			m_pItemElem[1]->SetExtra( 0 );

		pItemElem->SetExtra( 1 );
		m_pItemElem[1]	= pItemElem;
	}
	else
	if( PtInRect(&m_Rect[2], point) )
	{
		// 보조아이템은 상용화 아이템만 검사
#ifdef __PIERCING_SM_TIME

#ifdef __SM_ITEM_2ND
		if( pItemElem->GetProp()->dwID != II_SYS_SYS_SCR_PIEPROT )
#else //__SM_ITEM_2ND
		if( pItemElem->m_bCharged != 1 )
#endif //__SM_ITEM_2ND
			
#else // __PIERCING_SM_TIME
		if( pItemElem->GetProp()->bCharged != 1 )
#endif // __PIERCING_SM_TIME
		{
			g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
			return FALSE;
		}

		if( m_pItemElem[2] )
			m_pItemElem[2]->SetExtra( 0 );
		
		pItemElem->SetExtra( 1 );
		m_pItemElem[2]	= pItemElem;			
	}		

#endif //__PIERCING_SM_TIME
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

				CPiercingMessageBox* pBox = new CPiercingMessageBox;
				pBox->m_Objid[0] = m_pItemElem[0]->m_dwObjId;
				pBox->m_Objid[1] = m_pItemElem[1]->m_dwObjId;

				if( m_pItemElem[2] )
					pBox->m_Objid[2] = m_pItemElem[2]->m_dwObjId;
				else
					pBox->m_Objid[2] = NULL_ID;

				g_WndMng.OpenCustomBox( "", pBox, this );				
			}
			break;
		case WIDC_CANCEL:
			{
				Destroy();
			}
			break;
	};

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
