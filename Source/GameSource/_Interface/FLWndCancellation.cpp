
#include "StdAfx.h"

#include "FLWndCancellation.h"

CFLWndCancellation::CFLWndCancellation()
{
	memset( &m_objID, NULL_ID, sizeof( m_objID ) );
	m_bNoCloseButton = TRUE;
}

CFLWndCancellation::~CFLWndCancellation()
{

}

void CFLWndCancellation::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate(); 
	CWndStatic* pWndSlotItem = (CWndStatic*)GetDlgItem(WIDC_STATIC_SLOTITEM);
	if( !( pWndSlotItem ) )
		FLERROR_LOG( PROGRAM_NAME, _T( "CFLWndCancellation::OnInitialUpdate()" ) );
	
	m_rcSlot = pWndSlotItem->GetWndRect();

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

	CWndButton* pBtnClear  = ( CWndButton* )GetDlgItem( WIDC_BT_CLEAR );
	if( !pBtnClear )
		FLERROR_LOG( PROGRAM_NAME, _T( "CFLWndCancellation::OnInitialUpdate()" ) );

	pBtnClear->EnableWindow( FALSE );

}

BOOL CFLWndCancellation::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CANCELLATION, 0, CPoint( 0, 0 ), pWndParent );
} 
void CFLWndCancellation::ItemClear()
{
	_T_PACKET_RESET_BIND	tResetBind;
	tResetBind.dwSrcObjid = m_pScrItem->m_dwObjId;
	tResetBind.dwDestObjid = m_objID;
	tResetBind.bResult = TRUE;
	g_DPlay.SendReqResetBind(&tResetBind);
}

void CFLWndCancellation::ReceivedPacket(PT_PACKET_RESET_BIND  ptBind)
{
	if(ptBind->bResult == TRUE)
	{
		CString str;

		FLItemElem*  pItem = g_pPlayer->m_Inventory.GetAtId(ptBind->dwDestObjid);
		if(pItem->GetProp()->dwParts == NULL_ID)
		{
			//사용시 귀속
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_ITEM_CANCEL_CLEAR_USE) ) );		
		}
		else
		{
			//착용시귀속
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_ITEM_CANCEL_CLEAR_EQUIP) ) );		
		}
		CWndButton* pBtn = ( CWndButton* )GetDlgItem( WIDC_BT_CLEAR );
		if( pBtn )
			pBtn->EnableWindow( FALSE );		
		
	}
	memset( &m_objID, NULL_ID, sizeof( m_objID ) );
	Destroy();
}

BOOL CFLWndCancellation::OnChildNotify( UINT message,UINT nID,LRESULT* pLResult )
{
	if( EN_RETURN == message )
	{
		ItemClear();
	}
	switch(nID)
	{
	case WIDC_BT_CLEAR:
		ItemClear();
		break;
	case WIDC_BT_CANCEL:
		Destroy();
		break;
	}
	return TRUE;
}
void CFLWndCancellation::OnDraw( C2DRender* p2DRender )
{
	if( m_objID != 0 )
	{
		FLItemElem* pItem = g_pPlayer->m_Inventory.GetAtId( m_objID );
		if( IsUsableItem( pItem ) )
			p2DRender->RenderTexture( m_rcSlot.TopLeft(), pItem->GetTexture() );		
	}
}

BOOL CFLWndCancellation::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{	
	CWndBase* pWndFrame	= pShortcut->m_pFromWnd->GetFrameWnd();
	if(!pWndFrame)
	{
		FLASSERT( 0 );
		return FALSE;
	}

	// 인벤에서 온 아이템만 허용
	if( APP_INVENTORY != pWndFrame->GetWndId( ) )			
	{
		AfxMessageBox( "No inventory item" );
		return FALSE;
	}

	//check valid item
	m_pSlotItem = new FLItemElem;
	m_pSlotItem = g_pPlayer->m_Inventory.GetAtId( pShortcut->m_dwId );
		
	if( !IsUsableItem( m_pSlotItem ) )
		return FALSE;

	PT_ITEM_SPEC pProp = m_pSlotItem->GetProp();
	if( NULL == pProp )
		return FALSE;

	if(m_objID == NULL_ID)
	{
		//check slot
		if( PtInRect( &m_rcSlot, point ) )
		{
			if( m_pSlotItem->IsOwnState() )
			{
				//귀속상태이면
				CWndButton* pBtn = ( CWndButton* )GetDlgItem( WIDC_BT_CLEAR );
				if( pBtn )
					pBtn->EnableWindow( TRUE );
				m_objID = pShortcut->m_dwId;
				m_pScrItem = g_pPlayer->m_Inventory.GetAtByItemId(ITEM_INDEX( 20793, II_SYS_SYS_SCR_CANCELLATION ));
			}	
			else
			{
				//현재 귀속상태가 아님
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_ITEM_CANCEL_ERROR1) ) );	
			}
		}
	}
	else
	{
		//slot에 이미 아이템이 있음
		g_WndMng.OpenMessageBox( _T( prj.GetText(TID_ITEM_CANCEL_ERROR3) ) );	
	}

	return TRUE;
}
