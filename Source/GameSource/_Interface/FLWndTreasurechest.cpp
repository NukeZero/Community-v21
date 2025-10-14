#include "StdAfx.h"
#include "FLWndTreasurechest.h"

//-------------------------------------------------------------------------------------------
//					 보물 상자 아이템 [키 등록]
//						2011/06/27
//-------------------------------------------------------------------------------------------
CFLWndTreasurechest_Open::CFLWndTreasurechest_Open()
:m_pVertexBufferGauge(NULL),
m_nCurrGauge( 0 ),
m_pTexGauge( NULL ),
m_pTexGauge_Bg( NULL ),
m_bGaugeBarState(FALSE)
{
	m_bNoCloseButton = TRUE;
	memset( m_objID_TreasureChest, NULL_ID, sizeof( m_objID_TreasureChest ) );

}

//---------------------------------------------------------------------------------------------------------
CFLWndTreasurechest_Open::~CFLWndTreasurechest_Open()
{

}

//---------------------------------------------------------------------------------------------------------
HRESULT CFLWndTreasurechest_Open::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVertexBufferGauge == NULL )
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, 
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
		D3DFVF_TEXTUREVERTEX2, 
		D3DPOOL_DEFAULT, 
		&m_pVertexBufferGauge, 
		NULL);
	FLASSERT(m_pVertexBufferGauge != NULL);

	return S_OK;
}
//---------------------------------------------------------------------------------------------------------
HRESULT CFLWndTreasurechest_Open::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	SAFE_RELEASE( m_pVertexBufferGauge );
	return S_OK;
}
//---------------------------------------------------------------------------------------------------------
HRESULT CFLWndTreasurechest_Open::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVertexBufferGauge );
	return S_OK;
}

//---------------------------------------------------------------------------------------------------------
void CFLWndTreasurechest_Open::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate(); 

	if(GetWndBase( APP_BAG_EX )) 
		GetWndBase( APP_BAG_EX )->Destroy();

	if( g_WndMng.m_pWndTrade || g_WndMng.m_pWndBank || g_WndMng.m_pWndGuildBank || g_WndMng.GetWndVendorBase() )
	{
		Destroy();
		return;
	}

	//------------- 딜레이 게이지 세팅 ----------------
	m_pTexGauge = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndBar02.tga"   ), 0xffff00ff );
	m_pTexGauge_Bg = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndBar03.tga"   ), 0xffff00ff );
	if( !m_pTexGauge && ! m_pTexGauge_Bg)
		FLERROR_LOG( PROGRAM_NAME, _T( "m_pTexGuage(WndBar02.tga, WndBar03.tga) is NULL" ) );

	RestoreDeviceObjects( );

	CWndStatic* pWndGauage = (CWndStatic*)GetDlgItem( WIDC_PC_DELAY );
	if( pWndGauage )
	{
		CRect rect = pWndGauage->GetWndRect();
		m_nCurrGauge = rect.top;
	}
	//--------------------------------------------------


	// ------------ UI컨트롤 세팅 ----------------------
	// 슬롯의 아이템은 비운다.
	memset( m_objID_TreasureChest, NULL_ID, sizeof( m_objID_TreasureChest ) );

	m_objID_TreasureChest[0] = m_dwItemId;

	m_pWndNoticeItem = (CWndStatic*)GetDlgItem( WIDC_LB_NOTICE );	// 아이템 슬롯
	m_pWndKeyItem = (CWndStatic*)GetDlgItem( WIDC_CB_OPEN );	// 열쇠 아이템 슬롯
	m_pWndGauge = (CWndStatic*)GetDlgItem( WIDC_PC_DELAY );

	//하나라도 세팅이 안되있으면 에러로그!
	if( !( m_pWndNoticeItem 
		   && m_pWndGauge
		   && m_pWndKeyItem ) )
	{	
		FLERROR_LOG( PROGRAM_NAME, _T( "CFLWndTreasurechest_Open::OnInitialUpdate()" ) );
		return;
	}
	//--------------------------------------------------

	m_rectSlot_TreasureChest[0] = m_pWndNoticeItem->GetWndRect( );
	m_rectSlot_TreasureChest[1] = m_pWndKeyItem->GetWndRect( );

	
	// UI창을 인벤토리 옆에 붙이기 -> 요청 
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	if(pWndInventory)
	{
		CRect rectInventory = pWndInventory->GetWindowRect( TRUE );
		CPoint ptInventory = rectInventory.TopLeft();
		CPoint ptMove;

		CRect rect = GetWindowRect( TRUE );

		if( ptInventory.x > rect.Width() / 2 )
			ptMove = ptInventory - CPoint( rect.Width(), 0 );
		else
			ptMove = ptInventory + CPoint( rectInventory.Width(), 0 );

		Move( ptMove );

	}
	else 
	{
		CRect rectRoot = m_pWndRoot->GetLayoutRect();
		CRect rectWindow = GetWindowRect();
		CPoint point( rectRoot.right - rectWindow.Width(), 110 );
		Move( point );
		MoveParentCenter();
	}

	m_pWndGauge->SetVisible(FALSE);
	// 버튼은 비활성화 
	CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
	if( !pBtnOK )
		FLERROR_LOG( PROGRAM_NAME, _T( "CFLWndBaruna_Wakeup_Cancel::OnInitialUpdate()" ) );
	pBtnOK->EnableWindow( FALSE );
}

//---------------------------------------------------------------------------------------------------------
void CFLWndTreasurechest_Open::ReceivedPacket( FLSnapshotTreasureChestOpenNoti* pkOpenTreasureChest )
{
	if(pkOpenTreasureChest->dwChestItemObjID != NULL_ID)
	{
		m_dwItemId = pkOpenTreasureChest->dwChestItemObjID;
		Initialize( &g_WndMng, APP_TREASURECHEST_OPEN );
	}
}
//---------------------------------------------------------------------------------------------------------
BOOL CFLWndTreasurechest_Open::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 0, 0 ), pWndParent );
}

//---------------------------------------------------------------------------------------------------------
BOOL CFLWndTreasurechest_Open::OnChildNotify( UINT message,UINT nID,LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_BT_OK:
		m_bGaugeBarState = TRUE;
		break;

	case WIDC_BT_CANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
//---------------------------------------------------------------------------------------------------------
BOOL CFLWndTreasurechest_Open::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{

	CWndBase* pWndFrame	= pShortcut->m_pFromWnd->GetFrameWnd();
	if( !pWndFrame )
	{
		FLASSERT( 0 );
		return FALSE;
	}

	// 인벤에서 온 아이템만 허용
	if( APP_INVENTORY != pWndFrame->GetWndId( ) )			
	{
		g_WndMng.OpenMessageBox( prj.GetText(TID_BARUNAPEARCING_ERRORMSG) );
		return FALSE;
	}
	//check valid item
	FLItemElem* pChestItem;
	FLItemElem* pKeyItem;

	pChestItem = g_pPlayer->m_Inventory.GetAtId( m_dwItemId );
	pKeyItem = g_pPlayer->m_Inventory.GetAtId( pShortcut->m_dwId );

	if( !IsUsableItem( pChestItem ) && !IsUsableItem( pKeyItem ) )
		return FALSE;

	PT_ITEM_SPEC pProp = pKeyItem->GetProp();
	if( NULL == pProp )
		return FALSE;

	if(pChestItem == NULL) return FALSE;

	PT_TREASURE_CHEST pChest	= g_xSpecManager->GetSpecTreasureChest( pChestItem->m_dwItemId );
	
	if( PtInRect( &m_rectSlot_TreasureChest[1], point ) )// 열쇠 아이템
	{
		if(m_objID_TreasureChest[1] == NULL_ID)
		{
			if( pChest != NULL )
			{
				if(pProp->dwItemKind3 == IK3_BOXOPEN)
				{
					if(pKeyItem->m_dwItemId == pChest->arrKeyItemID[pProp->dwItemLV])
					{
						m_objID_TreasureChest[1] = pShortcut->m_dwId;
					}
					else
					{
						g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_TREASURE_ERRORCOMBINE) );
						return FALSE;
					}
				}
				else 
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_TREASURE_ERRORTKEY) );
					return FALSE;
				}
			}
		}
		else 
		{
			g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_WAKEUP_E_TEXT02) );
			return FALSE;
		}
	}


	if( m_objID_TreasureChest[1] != NULL_ID )
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
		if( pBtnOK )
			pBtnOK->EnableWindow( TRUE );
	}
	else 
		return FALSE;

	return FALSE;
}
//---------------------------------------------------------------------------------------------------------
void CFLWndTreasurechest_Open::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	if( m_bGaugeBarState == FALSE )
	{
		if( PtInRect( &m_rectSlot_TreasureChest[1], point ) )// 각성 취소할 아이템
		{
			m_objID_TreasureChest[1] = NULL_ID;
			InitCtrl();
		}
	}
}

//-------------------------------------------------------------------------------------------
void CFLWndTreasurechest_Open::InitCtrl()
{
	RestoreDeviceObjects();

	CWndStatic* pWndGauage = (CWndStatic*)GetDlgItem( WIDC_PC_DELAY );
	if( pWndGauage )
	{
		CRect rect = pWndGauage->GetWndRect();
		m_nCurrGauge = rect.top;
	}

	CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
	if( !pBtnOK )
		FLERROR_LOG( PROGRAM_NAME, _T( "CFLWndTreasurechest_Open::OnInitialUpdate()" ) );

	pBtnOK->EnableWindow( FALSE );
}

//-------------------------------------------------------------------------------------------
void CFLWndTreasurechest_Open::SendPacket()
{
	FLPacketTreasureChestOpenByKeyReq pPacket;
	pPacket.dwChestItemObjID = m_objID_TreasureChest[0];
	pPacket.dwKeyItemObjID = m_objID_TreasureChest[1];
	g_DPlay.SendPacket(&pPacket);
	Destroy();	
}

//-------------------------------------------------------------------------------------------
void CFLWndTreasurechest_Open::OnDraw( C2DRender* p2DRender )
{
	//Draw Slot Image
	for( int i = 0; i < 2; ++i )
	{
		if( m_objID_TreasureChest[ i ] != NULL_ID )
		{
			FLItemElem* pItem;
			pItem = g_pPlayer->m_Inventory.GetAtId( m_objID_TreasureChest[ i ] );
			if( IsUsableItem( pItem ) )
				p2DRender->RenderTexture( m_rectSlot_TreasureChest[i].TopLeft(), pItem->GetTexture() );
		}
	}

	//Draw Gauge
	CRect rect;

	if(m_bGaugeBarState == TRUE)
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
		CWndButton* pBtnCancel = ( CWndButton* )GetDlgItem( WIDC_BT_CANCEL );
		if( !(pBtnOK && pBtnCancel) )
			FLERROR_LOG( PROGRAM_NAME, _T( "CFLWndTreasurechest_Open::OnDraw()" ) );

		pBtnOK->EnableWindow( FALSE );
		pBtnCancel->EnableWindow( FALSE );

		rect = m_pWndGauge->GetWndRect();
		m_pTheme->RenderGauge(p2DRender, &rect, 0xffffffff, m_pVertexBufferGauge, m_pTexGauge_Bg);
		if( m_pWndGauge )
		{
			m_nCurrGauge += 3;
			if(m_nCurrGauge > rect.Width()+18)
			{
				m_nCurrGauge = rect.Width()+18;
				SendPacket();
				m_bGaugeBarState = FALSE;
			}
			rect.right = m_nCurrGauge;	
			m_pTheme->RenderGauge(p2DRender, &rect, 0xffffffff, m_pVertexBufferGauge, m_pTexGauge);
		}
	}
}




//-------------------------------------------------------------------------------------------
//					 보물 상자 아이템 [결과창]
//						2011/07/12
//-------------------------------------------------------------------------------------------

CFLWndTreasurechest_Result::CFLWndTreasurechest_Result()
{

}
//-------------------------------------------------------------------------------------------
CFLWndTreasurechest_Result::~CFLWndTreasurechest_Result()
{

}
//-------------------------------------------------------------------------------------------
void CFLWndTreasurechest_Result::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate(); 

	// ------------ UI컨트롤 세팅 ----------------------
	CWndStatic* pWndResultItem_01 = (CWndStatic*)GetDlgItem( WIDC_CB_OPEN03 );	// 열쇠 아이템 슬롯
	CWndStatic* pWndResultItem_02 = (CWndStatic*)GetDlgItem( WIDC_CB_OPEN02 );
	CWndStatic* pWndResultItem_03 = (CWndStatic*)GetDlgItem( WIDC_CB_OPEN );

	//하나라도 세팅이 안되있으면 에러로그!
	if( !( pWndResultItem_01 
		&& pWndResultItem_02
		&& pWndResultItem_03 ) )
	{	
		FLERROR_LOG( PROGRAM_NAME, _T( "CFLWndTreasurechest_Result::OnInitialUpdate()" ) );
		return;
	}

	m_rectSlot_TreasureChest_Result[0] = pWndResultItem_01->GetWndRect();
	m_rectSlot_TreasureChest_Result[1] = pWndResultItem_02->GetWndRect();
	m_rectSlot_TreasureChest_Result[2] = pWndResultItem_03->GetWndRect();

	int n= 0;
	memset( m_objID_TreasureChest_Result, NULL_ID, sizeof( m_objID_TreasureChest_Result ) );

	//--------------------------------------------------
	// UI창을 화면 중앙에 놓음
	//--------------------------------------------------

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();


	m_pTexItemEmptySLot = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, "WndEmptySlot.dds"), 0xffff00ff );

}
//---------------------------------------------------------------------------------------------------------
void CFLWndTreasurechest_Result::ReceivedPacket( FLSnapshotTreasureChestOpenByKeyAck* pkOpenChestByKey )
{

	for(int n = 0; n < MAX_TREASURE_ITEM_COUNT; n++)
	{
		T_TREASURE_ITEM tTreasureItem;
		tTreasureItem.bNotice  = pkOpenChestByKey->arrTreasureItem[n].bNotice;
		if(pkOpenChestByKey->arrTreasureItem[n].dwItemID == 0)
		{
			tTreasureItem.dwItemID = NULL_ID;
		}
		else 
		{
			tTreasureItem.dwItemID = pkOpenChestByKey->arrTreasureItem[n].dwItemID;
		}
		tTreasureItem.dwQuantity = pkOpenChestByKey->arrTreasureItem[n].dwQuantity;
		
		m_vecTreasureItemInfo.push_back(tTreasureItem);
	}
	Initialize( &g_WndMng, APP_TREASURECHEST_RESULT);
}
//-------------------------------------------------------------------------------------------
BOOL CFLWndTreasurechest_Result::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 0, 0 ), pWndParent );
}

//-------------------------------------------------------------------------------------------
BOOL CFLWndTreasurechest_Result::OnChildNotify( UINT message,UINT nID,LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_BT_CANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

//-------------------------------------------------------------------------------------------
void CFLWndTreasurechest_Result::OnDraw( C2DRender* p2DRender )
{
	CPoint pointMouse = GetMousePoint();

	CString strItemCount;

	int nCNT = 0;
	int nX = 0;
	for(m_itrItemInfo = m_vecTreasureItemInfo.begin(); m_itrItemInfo != m_vecTreasureItemInfo.end(); m_itrItemInfo++)
	{
		if( m_itrItemInfo->dwItemID != NULL_ID )
		{
			FLItemElem* pItem;
			pItem = g_pPlayer->m_Inventory.GetAtByItemId( m_itrItemInfo->dwItemID );
			if( IsUsableItem( pItem ) )
			{
				p2DRender->RenderTexture( m_rectSlot_TreasureChest_Result[nCNT].TopLeft(), pItem->GetTexture() );
				strItemCount.Format("%d",m_itrItemInfo->dwQuantity);
				if(strlen(strItemCount.GetString()) >= 2)
				{
					nX = 15;
				}
				else 
				{
					nX = 5;
				}
				p2DRender->TextOut(m_rectSlot_TreasureChest_Result[nCNT].BottomRight().x-nX, m_rectSlot_TreasureChest_Result[nCNT].BottomRight().y-13, strItemCount,m_dwColor);			
				p2DRender->TextOut(m_rectSlot_TreasureChest_Result[nCNT].BottomRight().x-nX, m_rectSlot_TreasureChest_Result[nCNT].BottomRight().y-14, strItemCount,0xff00cc00);			
			}
 			//TOOLTIP
			if(m_rectSlot_TreasureChest_Result[nCNT].PtInRect(pointMouse))
			{
				CPoint pointA = pointMouse;
				CRect m_Rect_TempA = m_rectSlot_TreasureChest_Result[nCNT];

				ClientToScreen( &pointA );
				ClientToScreen( &m_Rect_TempA );

				if(pItem != NULL)
				{
					g_WndMng.PutToolTip_Item( pItem,  pointA, &m_Rect_TempA, APP_TREASURECHEST_RESULT );
				}
			}
		}
		else
		{
			if( m_pTexItemEmptySLot )
				m_pTexItemEmptySLot->Render( p2DRender, m_rectSlot_TreasureChest_Result[nCNT].TopLeft() );
		}
		nCNT++;
	}
}