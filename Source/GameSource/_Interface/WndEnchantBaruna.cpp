
#include "StdAfx.h"

#ifdef __ENCHANT_BARUNA16
#include "WndEnchantBaruna.h"

//--------------------------------------------------------------------------------------------------------------
//CWndEnchantBaruna
CWndEnchantBaruna::CWndEnchantBaruna( ) : 
m_dwDeltaTime( 0 ),
m_bSendPacket(FALSE)
{
	for( int i = 0; i <= N_ITEM; ++i )
	{
		m_pItem[i] = NULL;
	}
}

CWndEnchantBaruna::~CWndEnchantBaruna( )
{
}

void CWndEnchantBaruna::OnDestroy()
{
	for( int i = 0; i <= N_ITEM; ++i )
	{
		if(m_pItem[i])
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_pItem[i] ) )
				m_pItem[i]->SetExtra(0);
		}
	}
	
}


void CWndEnchantBaruna::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	if(m_bSendPacket == FALSE)
	{
		if( PtInRect( &m_rectSlot_EnchantBaruna[N_CHASHITEM], point ) && m_pItem[N_CHASHITEM])
		{
			m_pItem[N_CHASHITEM]->SetExtra ( 0 );
			m_pItem[N_CHASHITEM] = NULL;
		}
		else if( PtInRect( &m_rectSlot_EnchantBaruna[N_OFFERSEED], point ) && m_pItem[N_OFFERSEED])
		{
			m_pItem[N_OFFERSEED]->SetExtra ( 0 );
			m_pItem[N_OFFERSEED] = NULL;
		}
		else if( PtInRect( &m_rectSlot_EnchantBaruna[N_ITEM], point ) && m_pItem[N_ITEM])
		{
			m_pItem[N_ITEM]->SetExtra ( 0 );
			m_pItem[N_ITEM] = NULL;
		}

		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
		if( m_pItem[N_OFFERSEED] != NULL &&
			m_pItem[N_ITEM] != NULL )
		{
			if( pBtnOK ) 
				pBtnOK->EnableWindow( TRUE );
		}
		else 
		{
			if( pBtnOK ) 
				pBtnOK->EnableWindow( FALSE );
		}
	}
}

void CWndEnchantBaruna::OnDraw( C2DRender* p2DRender ) 
{ 
	
//#ifdef BARUNA_ULTIMATE_UPDATE
//	CWndStatic* pWndSlot1 = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
//	CWndStatic* pWndSlot3 = (CWndStatic*)GetDlgItem( WIDC_STATIC4 );
//	if ( m_objID_EnchantBaruna[N_ITEM] != NULL_ID )
//	{
//		FLItemElem* pMainItem = g_pPlayer->m_Inventory.GetAtId( m_objID_EnchantBaruna[N_ITEM] );
//		
//		
//	
//		if ( pMainItem && pMainItem->GetAbilityOption() == 20 )
//		{
//			pWndSlot1->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "WndBarunaEnchant04.bmp" ) );
//			pWndSlot3->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "WndBarunaEnchant02.bmp" ) );
//		}
//		else
//		{
//			pWndSlot1->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "WndBarunaEnchant01.bmp" ) );
//			pWndSlot3->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "WndBarunaEnchant02.bmp" ) );
//		}
//		p2DRender->RenderTexture( m_rectSlot_EnchantBaruna[N_CHASHITEM].TopLeft(), pWndSlot1->GetTexture() );
//		p2DRender->RenderTexture( m_rectSlot_EnchantBaruna[N_OFFERSEED].TopLeft(), pWndSlot3->GetTexture() );
//	}
//	
//#endif

	for( int i = 0; i <= N_ITEM; ++i )
	{
		if( m_pItem[ i ] != NULL )
		{
			//if( IsUsableItem( pItem ) )
				p2DRender->RenderTexture( m_rectSlot_EnchantBaruna[i].TopLeft(), m_pItem[i]->GetTexture() );
		}
	}
} 

//---------------------------------------------------------------------------------------------------------
void CWndEnchantBaruna::InitCtrl()
{
	CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
	if( !pBtnOK )
		FLERROR_LOG( PROGRAM_NAME, _T( "CWndEnchantBaruna::InitCtrl()" ) );

	pBtnOK->EnableWindow( FALSE );
}

void CWndEnchantBaruna::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndStatic* pWndSlot1 = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
	CWndStatic* pWndSlot2 = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
	CWndStatic* pWndSlot3 = (CWndStatic*)GetDlgItem( WIDC_STATIC4 );

	if( !( pWndSlot1 && pWndSlot2 && pWndSlot3 ) )
		FLERROR_LOG( PROGRAM_NAME, _T( "CWndEnchantBaruna::OnInitialUpdate()" ) );

	m_rectSlot_EnchantBaruna[N_CHASHITEM] = pWndSlot1->GetWndRect( );
	m_rectSlot_EnchantBaruna[N_ITEM]= pWndSlot2->GetWndRect( );
	m_rectSlot_EnchantBaruna[N_OFFERSEED] = pWndSlot3->GetWndRect( );
//#ifdef BARUNA_ULTIMATE_UPDATE
//	pWndSlot1->m_strTexture	= "back_Slotitem.tga";
//	pWndSlot3->m_strTexture	= "back_Slotitem.tga";
//#endif	
	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
	InitCtrl();
} 

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndEnchantBaruna::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BARUNA_ENCHANT, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndEnchantBaruna::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( EN_RETURN == message )
	{
		DoModalConfirm( );
		return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
	}

	switch( nID )
	{
	case WIDC_BUTTON1:
		DoModalConfirm( );
		break;

	case WIDC_BUTTON2:
		Destroy();
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndEnchantBaruna::DoModalConfirm( )
{	
	if( m_pItem[N_ITEM] == NULL)
		return;

	int nEnchantCount = m_pItem[N_ITEM]->GetAbilityOption( );
//#ifdef BARUNA_ULTIMATE_UPDATE
//	if ( nEnchantCount == 20 )
//	{
//		//g_DPlay.SendUltimateTransWeapon(m_objID_EnchantBaruna[N_ITEM],m_objID_EnchantBaruna[N_OFFERSEED], m_objID_EnchantBaruna[N_CHASHITEM]);
//		SendUltimatePacket();
//	}
//	else
//	{
//#endif
	//flyingjin : +3이상부터 경고메세지 출력
	if( nEnchantCount >= _EBARUNA_SAFE_LEVEL && m_pItem[ N_CHASHITEM ] == NULL )		//제련+2이상이고 제련보호 캐쉬아이템을 안쓸경우 경고메시지("뽀개질수있음") 발생
	{
		SAFE_DELETE( g_WndMng.m_pWndConfirmVis );
		g_WndMng.m_pWndConfirmVis = new CWndConfirmVis;
		CWndConfirmVis* pWnd = g_WndMng.m_pWndConfirmVis;
		if( pWnd )
		{
			pWnd->m_eSection = CWndConfirmVis::ETC_BARUNASMELT_NOTICE;	
			pWnd->m_dwItemIndex = 	m_pItem[N_ITEM]->m_dwItemId;
			//pWnd->Initialize( &g_WndMng );
			pWnd->Initialize( this );

			CRect myRect = GetWndRect();
			CRect childRect = pWnd->GetWndRect();
			int x = myRect.left + ( ( myRect.Width() - childRect.Width() ) / 2 );
			int y = myRect.top - ( childRect.Height() + 10 );
			pWnd->Move( x, y );
		}
	}
	else
		SendPacket( );
//#ifdef BARUNA_ULTIMATE_UPDATE
//	}
//#endif
}

BOOL CWndEnchantBaruna::SendPacket( )
{
	m_bSendPacket = TRUE;

	FLPacketItemUpgradeGeneralEnchantReq kPacket;
	
	kPacket.dwMainItemObjID			= m_pItem[N_ITEM]->m_dwObjId;			//아이템
	kPacket.dwMaterialItemObjID		= m_pItem[N_OFFERSEED]->m_dwObjId;		//재료
	kPacket.dwProtectionItemObjID	= m_pItem[N_CHASHITEM] == NULL ? NULL_ID : m_pItem[N_CHASHITEM]->m_dwObjId;	//캐시 

	if( m_pParentWnd )
	{
		kPacket.dwUpgradeNpcObjID	= ((CWndBarunaNPC*)m_pParentWnd)->GetNpcID( );
	}

	g_DPlay.SendPacket( &kPacket );

	CWndButton* pBtn = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
	if( pBtn )
		pBtn->EnableWindow( FALSE );
	
	return	TRUE;
}
//#ifdef BARUNA_ULTIMATE_UPDATE
//BOOL CWndEnchantBaruna::SendUltimatePacket()
//{
//	m_bSendPacket = TRUE;
//
//	FLPacketItemUpgradeUltimateTransReq kPacket;
//	
//	kPacket.dwMainItemObjID			= m_objID_EnchantBaruna[N_ITEM];			//아이템
//	kPacket.dwMaterialItemObjID1	= m_objID_EnchantBaruna[N_OFFERSEED];		//재료1
//	kPacket.dwMaterialItemObjID2	= m_objID_EnchantBaruna[N_CHASHITEM];	//재료2
//
//	if( m_pParentWnd )
//	{
//		kPacket.dwUpgradeNpcObjID	= ((CWndBarunaNPC*)m_pParentWnd)->GetNpcID( );
//	}
//
//	g_DPlay.SendPacket( &kPacket );
//
//	CWndButton* pBtn = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
//	if( pBtn )
//		pBtn->EnableWindow( FALSE );
//	
//	return	TRUE;
//}
//#endif

void CWndEnchantBaruna::ReceivedPacket( )
{
	m_bSendPacket = FALSE;

	for( int i = 0; i <= N_ITEM; ++i )
	{
		if(m_pItem[i])
		{
			m_pItem[i]->SetExtra(0);
			m_pItem[i] = NULL;
		}
	}
}

BOOL CWndEnchantBaruna::Process()
{
	return CWndBase::Process( );
}

BOOL CWndEnchantBaruna::SetItem(FLItemElem* pItemElem)
{
	if( !IsUsableItem( pItemElem ) )
		return FALSE;

	PT_ITEM_SPEC pProp = pItemElem->GetProp();
	if( NULL == pProp )
		return FALSE;


	if( pProp->IsBaruna() && pItemElem->GetAbilityOption() < MAX_BARUNA_ENCHANT_LEVEL)	
	{
		if( m_pItem[N_ITEM] )
			m_pItem[N_ITEM]->SetExtra ( 0 );
		pItemElem->SetExtra(pItemElem->GetExtra() + 1);
		m_pItem[N_ITEM] = pItemElem;
	}
	else if( IK3_BARUNA_PROTECTION == pProp->dwItemKind3 || IK3_BARUNA_PROTECTION_KEEP == pProp->dwItemKind3 )
	{
		if( m_pItem[N_CHASHITEM] )
			m_pItem[N_CHASHITEM]->SetExtra ( 0 );
		pItemElem->SetExtra(pItemElem->GetExtra() + 1);
		m_pItem[N_CHASHITEM] = pItemElem;
	}
	else if( IK3_OPERCID == pProp->dwItemKind3 )	
	{
		if( m_pItem[N_OFFERSEED] )
			m_pItem[N_OFFERSEED]->SetExtra ( 0 );
		pItemElem->SetExtra(pItemElem->GetExtra() + 1);
		m_pItem[N_OFFERSEED] = pItemElem;
	}
	else
		return FALSE;

	
	

	if( m_pItem[N_OFFERSEED] != NULL && 
		m_pItem[N_ITEM] != NULL )
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
		if( pBtnOK )
			pBtnOK->EnableWindow( TRUE );
	}
	
	return TRUE;
}

BOOL CWndEnchantBaruna::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame	= pShortcut->m_pFromWnd->GetFrameWnd();
	if( !pWndFrame )
	{
		FLASSERT( 0 );
		return FALSE;
	}

	// 인벤에서 온 아이템만 허용
	//gmpbigsun( 20101116 ) : 휴대가방 검사 추가
	if( APP_INVENTORY != pWndFrame->GetWndId( ) && APP_BAG_EX != pWndFrame->GetWndId( ) )			
	{
		return FALSE;
	}
	
	//check valid item
	FLItemElem* pItem;
	pItem = g_pPlayer->m_Inventory.GetAtId( pShortcut->m_dwId );

	if( !IsUsableItem( pItem ) )
		return FALSE;

	PT_ITEM_SPEC pProp = pItem->GetProp();
	if( NULL == pProp )
		return FALSE;
//#ifdef BARUNA_ULTIMATE_UPDATE
//	FLItemElem* pMainItem;
//	
//	pMainItem = g_pPlayer->m_Inventory.GetAtId( m_objID_EnchantBaruna[N_ITEM] );
//#endif
	//check slot
	if( PtInRect( &m_rectSlot_EnchantBaruna[N_CHASHITEM], point ) )		// cash item
	{
//#ifdef BARUNA_ULTIMATE_UPDATE
//		if ( !pMainItem )
//		{
//			g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_NEWSMELT_NEWSMELT01) ); //"바루나 아이템을 등록해야합니다 
//			return FALSE; 
//		}
//
//		if ( pMainItem && pMainItem->GetAbilityOption() == 20 )
//			m_objID_EnchantBaruna[N_CHASHITEM] = pShortcut->m_dwId;
//		else 
//#endif
		if( IK3_BARUNA_PROTECTION == pProp->dwItemKind3 || IK3_BARUNA_PROTECTION_KEEP == pProp->dwItemKind3 )
		{
			pItem->SetExtra(pItem->GetExtra() + 1);
			m_pItem[N_CHASHITEM] = pItem;
		}
		else
		{
			g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_GHOUSE_BARUNASMELPROT) ); //바루나의 가호 등록
			return FALSE;
		}
	}
	else 
	if( PtInRect( &m_rectSlot_EnchantBaruna[N_ITEM], point ) )		//item( weapon )
	{
		if( pItem->GetAbilityOption() >= MAX_BARUNA_ENCHANT_LEVEL )
			return FALSE;

		if( pProp->IsBaruna() )	
		{
			pItem->SetExtra(pItem->GetExtra() + 1);
			m_pItem[N_ITEM] = pItem;
		}
		else						
		{
			g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_NEWSMELT_NEWSMELT01) ); //"바루나 아이템을 등록해야합니다 
			return FALSE; 
		}
	}
	else 
	if( PtInRect( &m_rectSlot_EnchantBaruna[N_OFFERSEED], point ) )		//offerseed
	{
//#ifdef BARUNA_ULTIMATE_UPDATE
//		if ( !pMainItem )
//		{
//			g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_NEWSMELT_NEWSMELT01) ); //"바루나 아이템을 등록해야합니다 
//			return FALSE; 
//		}
//#endif
		if( IK3_OPERCID == pProp->dwItemKind3 )	
		{
			pItem->SetExtra(pItem->GetExtra() + 1);
			m_pItem[N_OFFERSEED] = pItem;
		}
		else								
		{ 
			g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_NEWSMELT_NEWSMELT02) ); //오퍼시드 아이템을 등록해라이 
			return FALSE;
		}
	}

	

	//check enable button OK
//	for( int i = 0; i < N_MAX; ++i )
//	{
//		if( m_objID[ i ] == 0 )
//			return FALSE;
//	}
//#ifdef BARUNA_ULTIMATE_UPDATE
//
//	if ( m_objID_EnchantBaruna[N_ITEM] != NULL_ID )
//	{
//		pMainItem = g_pPlayer->m_Inventory.GetAtId( m_objID_EnchantBaruna[N_ITEM] );
//		CWndStatic* pWndSlot1 = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
//		CWndStatic* pWndSlot3 = (CWndStatic*)GetDlgItem( WIDC_STATIC4 );
//pWndSlot1->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "WndBarunaEnchant02.bmp" ) );
//		if( !( pWndSlot1 && pWndSlot3 ) )
//		{
//			FLERROR_LOG( PROGRAM_NAME, _T( "CWndEnchantBaruna::OnDropIcon()" ) );
//			return FALSE;
//		}
//	
//		if ( pMainItem && pMainItem->GetAbilityOption() == 20 )
//		{
//			pWndSlot1->m_strTexture	= "WndBarunaEnchant02.bmp";
//			pWndSlot3->m_strTexture	= "WndBarunaEnchant01.bmp";
//		}
//		else
//		{
//			pWndSlot1->m_strTexture	= "WndBarunaEnchant01.bmp";
//			pWndSlot3->m_strTexture	= "WndBarunaEnchant02.bmp";
//		}
//	}
//#endif
	
	if( m_pItem[N_OFFERSEED] != NULL && 
		m_pItem[N_ITEM] != NULL )
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
		if( pBtnOK )
			pBtnOK->EnableWindow( TRUE );
	}
	else 
		return TRUE;

	return TRUE;
}

#ifdef BARUNA_ULTIMATE_UPDATE
//--------------------------------------------------------------------------------------------------------------
//CWndBarunaUltimateTrans
CWndBarunaUltimateTrans::CWndBarunaUltimateTrans( ) : 
m_dwDeltaTime( 0 ),
m_bSendPacket(FALSE)
{
	memset( m_objID_Baruna, NULL_ID, sizeof( m_objID_Baruna ) );
}

CWndBarunaUltimateTrans::~CWndBarunaUltimateTrans( )
{
}

void CWndBarunaUltimateTrans::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	if(m_bSendPacket == FALSE)
	{
		if( PtInRect( &m_rectSlot_Baruna[N_OFFERSEED], point ) )
		{
			m_objID_Baruna[N_OFFERSEED] = NULL_ID;
		}
		else if( PtInRect( &m_rectSlot_Baruna[N_ITEM], point ) )
		{
			m_objID_Baruna[N_ITEM] = NULL_ID;
		}
		else if( PtInRect( &m_rectSlot_Baruna[N_JEWELRY], point ) )
		{
			m_objID_Baruna[N_JEWELRY] = NULL_ID;
		}

		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
		if( m_objID_Baruna[N_OFFERSEED] != NULL_ID &&
			m_objID_Baruna[N_ITEM] != NULL_ID &&
			m_objID_Baruna[N_JEWELRY] != NULL_ID)
		{
			if( pBtnOK ) 
				pBtnOK->EnableWindow( TRUE );
		}
		else 
		{
			if( pBtnOK ) 
				pBtnOK->EnableWindow( FALSE );
		}
	}
}

void CWndBarunaUltimateTrans::OnDraw( C2DRender* p2DRender ) 
{ 
	
	
	for( int i = 0; i <= N_JEWELRY; ++i )
	{
		if( m_objID_Baruna[ i ] != NULL_ID )
		{
			FLItemElem* pItem;
		
			pItem = g_pPlayer->m_Inventory.GetAtId( m_objID_Baruna[ i ] );
		
			if( IsUsableItem( pItem ) )
				p2DRender->RenderTexture( m_rectSlot_Baruna[i].TopLeft(), pItem->GetTexture() );
		}
	}
} 

//---------------------------------------------------------------------------------------------------------
void CWndBarunaUltimateTrans::InitCtrl()
{
	memset( m_objID_Baruna, NULL_ID, sizeof( m_objID_Baruna ) );

	CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
	if( !pBtnOK )
		FLERROR_LOG( PROGRAM_NAME, _T( "CWndBarunaUltimateTrans::InitCtrl()" ) );

	pBtnOK->EnableWindow( FALSE );
}
void CWndBarunaUltimateTrans::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndStatic* pWndSlot1 = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
	CWndStatic* pWndSlot2 = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
	CWndStatic* pWndSlot3 = (CWndStatic*)GetDlgItem( WIDC_STATIC4 );

	if( !( pWndSlot1 && pWndSlot2 && pWndSlot3 ) )
		FLERROR_LOG( PROGRAM_NAME, _T( "CWndBarunaUltimateTrans::OnInitialUpdate()" ) );

	m_rectSlot_Baruna[N_OFFERSEED] = pWndSlot1->GetWndRect( );
	m_rectSlot_Baruna[N_ITEM]= pWndSlot2->GetWndRect( );
	m_rectSlot_Baruna[N_JEWELRY] = pWndSlot3->GetWndRect( );

	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
	InitCtrl();
} 

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndBarunaUltimateTrans::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BARUNA_ULTIMATETRANS, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndBarunaUltimateTrans::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( EN_RETURN == message )
	{
		DoModalConfirm( );
		return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
	}

	switch( nID )
	{
	case WIDC_BUTTON1:
		DoModalConfirm( );
		break;

	case WIDC_BUTTON2:
		Destroy();
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndBarunaUltimateTrans::DoModalConfirm( )
{
	FLItemElem* pItem;
	
	pItem = g_pPlayer->m_Inventory.GetAtId( m_objID_Baruna[N_ITEM] );
	
	if( !pItem )
		return;

	int nEnchantCount = pItem->GetAbilityOption( );

	if ( nEnchantCount == 20 )
	{
		//g_DPlay.SendUltimateTransWeapon(m_objID_Baruna[N_ITEM],m_objID_Baruna[N_OFFERSEED], m_objID_Baruna[N_JEWELRY]);
		SendPacket();
	}

}



BOOL CWndBarunaUltimateTrans::SendPacket()
{
	m_bSendPacket = TRUE;

	FLPacketItemUpgradeUltimateTransReq kPacket;
	
	kPacket.dwMainItemObjID			= m_objID_Baruna[N_ITEM];			//아이템
	kPacket.dwMaterialItemObjID1	= m_objID_Baruna[N_OFFERSEED];		//재료1
	kPacket.dwMaterialItemObjID2	= m_objID_Baruna[N_JEWELRY];		//재료2

	if( m_pParentWnd )
	{
		kPacket.dwUpgradeNpcObjID	= ((CWndBarunaNPC*)m_pParentWnd)->GetNpcID( );
	}

	g_DPlay.SendPacket( &kPacket );

	CWndButton* pBtn = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
	if( pBtn )
		pBtn->EnableWindow( FALSE );
	
	return	TRUE;
}


void CWndBarunaUltimateTrans::ReceivedPacket( )
{
	m_bSendPacket = FALSE;
	memset( m_objID_Baruna, NULL_ID, sizeof( m_objID_Baruna ) );
}

BOOL CWndBarunaUltimateTrans::Process()
{
	return CWndBase::Process( );
}

BOOL CWndBarunaUltimateTrans::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame	= pShortcut->m_pFromWnd->GetFrameWnd();
	if( !pWndFrame )
	{
		FLASSERT( 0 );
		return FALSE;
	}

	// 인벤에서 온 아이템만 허용
	//gmpbigsun( 20101116 ) : 휴대가방 검사 추가
	if( APP_INVENTORY != pWndFrame->GetWndId( ) && APP_BAG_EX != pWndFrame->GetWndId( ) )			
	{
		return FALSE;
	}
	
	//check valid item
	FLItemElem* pItem;
	pItem = g_pPlayer->m_Inventory.GetAtId( pShortcut->m_dwId );

	if( !IsUsableItem( pItem ) )
		return FALSE;

	PT_ITEM_SPEC pProp = pItem->GetProp();
	if( NULL == pProp )
		return FALSE;


	//check slot
	if( PtInRect( &m_rectSlot_Baruna[N_JEWELRY], point ) )		// cash item
	{
		//if( IK3_BARUNA_PROTECTION == pProp->dwItemKind3 || IK3_BARUNA_PROTECTION_KEEP == pProp->dwItemKind3 )
		if ( pItem->m_nItemNum >= 5 )
			m_objID_Baruna[N_JEWELRY] = pShortcut->m_dwId;
		/*else
		{
			g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_GHOUSE_BARUNASMELPROT) ); //바루나의 가호 등록
			return FALSE;
		}*/
	}
	else 
	if( PtInRect( &m_rectSlot_Baruna[N_ITEM], point ) )		//item( weapon )
	{
		if( pProp->IsBaruna( ) )	
			m_objID_Baruna[N_ITEM] = pShortcut->m_dwId;
		else						
		{
			g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_NEWSMELT_NEWSMELT01) ); //"바루나 아이템을 등록해야합니다 
			return FALSE; 
		}
	}
	else 
	if( PtInRect( &m_rectSlot_Baruna[N_OFFERSEED], point ) )		//offerseed
	{

		if( IK3_OPERCID == pProp->dwItemKind3 )	
			m_objID_Baruna[N_OFFERSEED] = pShortcut->m_dwId; 
		else								
		{ 
			g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_NEWSMELT_NEWSMELT02) ); //오퍼시드 아이템을 등록해라이 
			return FALSE;
		}
	}

	
	if( m_objID_Baruna[N_OFFERSEED] != NULL_ID && 
		m_objID_Baruna[N_ITEM] != NULL_ID )
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
		if( pBtnOK )
			pBtnOK->EnableWindow( TRUE );
	}
	else 
		return TRUE;

	return TRUE;
}

//--------------------------------------------------------------------------------------------------------------
#endif	// BARUNA_ULTIMATE_UPDATE
//CWndProduceOfferSeed
CWndProduceOfferSeed::CWndProduceOfferSeed( ) : m_bSendPacket(FALSE)
{
	for( int i = 0; i <= N_MEAL; ++i )
	{
		m_pItem[i] = NULL;
	}
}

CWndProduceOfferSeed::~CWndProduceOfferSeed( )
{
}

void CWndProduceOfferSeed::OnDestroy()
{
	for( int i = 0; i <= N_MEAL; ++i )
	{
		if(m_pItem[i])
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_pItem[i] ) )
				m_pItem[i]->SetExtra(0);
		}
	}
}
void CWndProduceOfferSeed::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	if(m_bSendPacket == FALSE)
	{
		if( PtInRect( &m_rectSlot[N_OFFER], point ) && m_pItem[N_OFFER])
		{
			m_pItem[N_OFFER]->SetExtra ( 0 );
			m_pItem[N_OFFER] = NULL;
		}
		else if( PtInRect( &m_rectSlot[N_CID], point ) && m_pItem[N_CID])
		{
			m_pItem[N_CID]->SetExtra ( 0 );
			m_pItem[N_CID] = NULL;
		} 
		else if( PtInRect( &m_rectSlot[N_MEAL], point ) && m_pItem[N_MEAL])
		{
			m_pItem[N_MEAL]->SetExtra ( 0 );
			m_pItem[N_MEAL] = NULL;
		} 

		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
		if( m_pItem[N_OFFER] && m_pItem[N_CID] && m_pItem[N_MEAL])
		{
			if( pBtnOK ) 
				pBtnOK->EnableWindow( TRUE );
		}
		else 
		{
			if( pBtnOK ) 
				pBtnOK->EnableWindow( FALSE );
		}
	}
}


void CWndProduceOfferSeed::OnDraw( C2DRender* p2DRender ) 
{
	for( int i = 0; i <= N_MEAL; ++i )
	{
		if( m_pItem[ i ] )
		{	
			//if( IsUsableItem( pItem ) )
			p2DRender->RenderTexture( m_rectSlot[i].TopLeft(), m_pItem[ i ]->GetTexture() );
		}
	}
} 

void CWndProduceOfferSeed::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndStatic* pWndSlot1 = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
	CWndStatic* pWndSlot2 = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
	CWndStatic* pWndSlot3 = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );

	if( !( pWndSlot1 && pWndSlot2 && pWndSlot3 ) )
		FLERROR_LOG( PROGRAM_NAME, _T( "CWndProduceOfferSeed::OnInitialUpdate()" ) );

	m_rectSlot[N_OFFER] = pWndSlot1->GetWndRect( );
	m_rectSlot[N_CID] = pWndSlot2->GetWndRect( );
	m_rectSlot[N_MEAL] = pWndSlot3->GetWndRect( );

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

	CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
	if( !pBtnOK )
		FLERROR_LOG( PROGRAM_NAME, _T( "CWndProduceOfferSeed::OnInitialUpdate()" ) );
	for( int i = 0; i <= N_MEAL; ++i )
	{
		m_pItem[i] = NULL;
	}

	pBtnOK->EnableWindow( FALSE );
} 

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndProduceOfferSeed::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BARUNA_OFFERSEED, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndProduceOfferSeed::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( EN_RETURN == message )
	{
		SendPacket();
		return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
	}

	switch( nID )
	{
	case WIDC_BUTTON1:
		SendPacket();
		break;

	case WIDC_BUTTON2:
		Destroy();
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

BOOL CWndProduceOfferSeed::SendPacket( )
{
	m_bSendPacket = TRUE;
	T_PACKET_OPERCID_CREATE kPacket = { 0 };
	//mem_set( &kPacket, 0, sizeof( kPacket ) );
	kPacket.dwOperObjid = m_pItem[N_OFFER]->m_dwObjId;
	kPacket.dwCidObjid = m_pItem[N_CID]->m_dwObjId;
	kPacket.dwBaryummealObjid = m_pItem[N_MEAL]->m_dwObjId;

	g_DPlay.SendReqOperCidCreate( &kPacket );

	CWndButton* pBtn = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
	if( pBtn )
		pBtn->EnableWindow( FALSE );

	return	TRUE;
}

void CWndProduceOfferSeed::ReceivedPacket( )
{
	m_bSendPacket = FALSE;
	for( int i = 0; i <= N_MEAL; ++i )
	{
		if( m_pItem[i] )
			m_pItem[i]->SetExtra ( 0 );
		m_pItem[i] = NULL;
	}
}

BOOL CWndProduceOfferSeed::Process()
{
	return TRUE;
}

BOOL CWndProduceOfferSeed::SetItem(FLItemElem* pItem)
{
	if( !IsUsableItem( pItem ) )
		return FALSE;

	PT_ITEM_SPEC pProp = pItem->GetProp();
	if( NULL == pProp )
		return FALSE;

	if( IK3_OPER == pProp->dwItemKind3 )
	{
		if( m_pItem[N_OFFER] )
			m_pItem[N_OFFER]->SetExtra ( 0 );
		pItem->SetExtra(pItem->GetExtra() + 1);
		m_pItem[N_OFFER] = pItem; 
	}
	else if( IK3_CID == pProp->dwItemKind3 )
	{
		if( m_pItem[N_CID] )
			m_pItem[N_CID]->SetExtra ( 0 );
		pItem->SetExtra(pItem->GetExtra() + 1);
		m_pItem[N_CID] = pItem; 
	}
	else if( IK3_MEAL == pProp->dwItemKind3 )
	{
		if( m_pItem[N_MEAL] )
			m_pItem[N_MEAL]->SetExtra ( 0 );
		pItem->SetExtra(pItem->GetExtra() + 1);
		m_pItem[N_MEAL] = pItem; 
	}
	else
		return FALSE;

	
	if( m_pItem[N_OFFER] && m_pItem[N_CID] && m_pItem[N_MEAL] )
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
		if( pBtnOK )
			pBtnOK->EnableWindow( TRUE );
	}

	return TRUE;
}

BOOL CWndProduceOfferSeed::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame	= pShortcut->m_pFromWnd->GetFrameWnd();
	if( !pWndFrame )
	{
		FLASSERT( 0 );
		return FALSE;
	}

	//gmpbigsun( 20101116 ) : 휴대가방 검사 추가
	if( APP_INVENTORY == pWndFrame->GetWndId( ) || APP_BAG_EX == pWndFrame->GetWndId( ) )	// 인벤에서 온 아이템은 조건검사 
	{
		FLItemElem* pItem;
		pItem = g_pPlayer->m_Inventory.GetAtId( pShortcut->m_dwId );
		
		if( !IsUsableItem( pItem ) )
			return FALSE;

		PT_ITEM_SPEC pProp = pItem->GetProp();
		if( !pProp )
			return FALSE;

       	//check slot
		if( PtInRect( &m_rectSlot[N_OFFER], point ) )		// offer
		{	
			if( IK3_OPER == pProp->dwItemKind3 )
			{
				if( m_pItem[N_OFFER] )
					m_pItem[N_OFFER]->SetExtra ( 0 );
				pItem->SetExtra(pItem->GetExtra() + 1);

				m_pItem[N_OFFER] = pItem; 
			}
			else									
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_NEWSMELT_OPERCID01) );		//오퍼를 등록해야함
				return FALSE; 
			}
		}
		else if( PtInRect( &m_rectSlot[N_CID], point ) )	//seed
		{
			if( IK3_CID == pProp->dwItemKind3 )	
			{
				if( m_pItem[N_CID] )
					m_pItem[N_CID]->SetExtra ( 0 );
				pItem->SetExtra(pItem->GetExtra() + 1);

				m_pItem[N_CID] = pItem; 
			}
			else									
			{ 
				g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_NEWSMELT_OPERCID02) );		//시드를 등록해야함 
				return FALSE; 
			}
		}
		else if( PtInRect( &m_rectSlot[N_MEAL], point ) )	//바륨의 가루
		{
			if( IK3_MEAL == pProp->dwItemKind3 )
			{
				if( m_pItem[N_CID] )
					m_pItem[N_CID]->SetExtra ( 0 );
				pItem->SetExtra(pItem->GetExtra() + 1);

				m_pItem[N_MEAL] = pItem; 
			}
			else									
			{ 
				g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_NEWSMELT_OPERCID03) );		//바륨의가루를 등록해야함 
				return FALSE; 
			}
		}

	}
	else
	{
		//AfxMessageBox( "No inventory item" );
		return FALSE;
	}

// 	//check enable button OK
// 	for( int i = 0; i < N_MAX; ++i )
// 	{
// 		if( m_objID[ i ] == 0 )
// 			return FALSE;
// 	}

	if( m_pItem[N_OFFER] && m_pItem[N_CID] && m_pItem[N_MEAL] )
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
		if( pBtnOK )
			pBtnOK->EnableWindow( TRUE );
	}
	else 
		return FALSE;
	
	return TRUE;
}

//--------------------------------------------------------------------------------------------------------------
//CWndProduceOffer
CWndProduceOffer::CWndProduceOffer( ) : m_bSendPacket(FALSE)
{
	for( int i = 0; i <= N_OFFERMIX; ++i )
	{
		m_pItem[i] = NULL;
	}
}

CWndProduceOffer::~CWndProduceOffer( )
{
}

void CWndProduceOffer::OnDestroy()
{
	for( int i = 0; i <= N_OFFERMIX; ++i )
	{
		if(m_pItem[i])
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_pItem[i] ) )
				m_pItem[i]->SetExtra(0);
		}
	}
}
void CWndProduceOffer::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	if(m_bSendPacket == FALSE)
	{
		if( PtInRect( &m_rectSlot[N_OFFER], point ) && m_pItem[N_OFFER])
		{
			m_pItem[N_OFFER]->SetExtra ( 0 );
			m_pItem[N_OFFER] = NULL;
		}
		else if( PtInRect( &m_rectSlot[N_OFFERMIX], point ) && m_pItem[N_OFFERMIX])
		{
			m_pItem[N_OFFERMIX]->SetExtra ( 0 );
			m_pItem[N_OFFERMIX] = NULL;
		} 
		

		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
		if( m_pItem[N_OFFER] && m_pItem[N_OFFERMIX] )
		{
			if( pBtnOK ) 
				pBtnOK->EnableWindow( TRUE );
		}
		else 
		{
			if( pBtnOK ) 
				pBtnOK->EnableWindow( FALSE );
		}
	}
}


void CWndProduceOffer::OnDraw( C2DRender* p2DRender ) 
{
	for( int i = 0; i <= N_OFFERMIX; ++i )
	{
		if( m_pItem[i] )
		{
			//if( IsUsableItem( pItem ) )
				p2DRender->RenderTexture( m_rectSlot[i].TopLeft(), m_pItem[i]->GetTexture() );
		}
	}
} 

void CWndProduceOffer::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	CWndStatic* pWndSlot1 = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
	CWndStatic* pWndSlot2 = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );

	if( !( pWndSlot1 && pWndSlot2 ) )
		FLERROR_LOG( PROGRAM_NAME, _T( "CWndProduceOfferSeed::OnInitialUpdate()" ) );

	m_rectSlot[N_OFFER] = pWndSlot1->GetWndRect( );
	m_rectSlot[N_OFFERMIX] = pWndSlot2->GetWndRect( );

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

	CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
	if( !pBtnOK )
		FLERROR_LOG( PROGRAM_NAME, _T( "CWndProduceOffer::OnInitialUpdate()" ) );
	for( int i = 0; i <= N_OFFERMIX; ++i )
	{
		m_pItem[i] = NULL;
	}

	pBtnOK->EnableWindow( FALSE );
} 

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndProduceOffer::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BARUNA_OFFER, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndProduceOffer::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( EN_RETURN == message )
	{
		SendPacket();
		return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
	}

	switch( nID )
	{
	case WIDC_BUTTON1:
		SendPacket();
		break;

	case WIDC_BUTTON2:
		Destroy();
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

BOOL CWndProduceOffer::SendPacket( )
{
	m_bSendPacket = TRUE;

	T_PACKET_OPER_CREATE kPacket = { 0 };
	//mem_set( &kPacket, 0, sizeof( kPacket ) );
	kPacket.dwOperPieceObjid= m_pItem[N_OFFER]->m_dwObjId;
	kPacket.dwOperPieceCombineObjid = m_pItem[N_OFFERMIX]->m_dwObjId;

	g_DPlay.SendReqOperCreate( &kPacket );

	CWndButton* pBtn = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
	if( pBtn )
		pBtn->EnableWindow( FALSE );

	return	TRUE;
}

BOOL CWndProduceOffer::Process()
{
	return TRUE;
}

BOOL CWndProduceOffer::SetItem(FLItemElem* pItem)
{
	if( !IsUsableItem( pItem ) )
		return FALSE;

	PT_ITEM_SPEC pProp = pItem->GetProp();
	if( NULL == pProp )
		return FALSE;

	if( IK3_OPERCRYSTAL == pProp->dwItemKind3 )
	{
		if( m_pItem[N_OFFER] )
			m_pItem[N_OFFER]->SetExtra ( 0 );
		pItem->SetExtra(pItem->GetExtra() + 1);
		m_pItem[N_OFFER] = pItem; 
	}
	else if( IK3_OPERMIX == pProp->dwItemKind3 )
	{
		if( m_pItem[N_OFFERMIX] )
			m_pItem[N_OFFERMIX]->SetExtra ( 0 );
		pItem->SetExtra(pItem->GetExtra() + 1);
		m_pItem[N_OFFERMIX] = pItem; 
	}
	else
		return FALSE;

	
	if( m_pItem[N_OFFER] && m_pItem[N_OFFERMIX] )
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
		if( pBtnOK )
			pBtnOK->EnableWindow( TRUE );
	}

	return TRUE;
}

BOOL CWndProduceOffer::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame	= pShortcut->m_pFromWnd->GetFrameWnd();
	if( !pWndFrame )
	{
		FLASSERT( 0 );
		return FALSE;
	}

	//gmpbigsun( 20101116 ) : 휴대가방 검사 추가
	if( APP_INVENTORY == pWndFrame->GetWndId( ) || APP_BAG_EX == pWndFrame->GetWndId() )			// 인벤에서 온 아이템은 조건검사 
	{
		FLItemElem* pItem;
	
		pItem = g_pPlayer->m_Inventory.GetAtId( pShortcut->m_dwId );
			
		if( !IsUsableItem( pItem ) )
			return FALSE;

		PT_ITEM_SPEC pProp = pItem->GetProp();
		if( !pProp )
			return FALSE;

       	//check slot
		if( PtInRect( &m_rectSlot[N_OFFER], point ) )		// offer결정체
		{	
			if( IK3_OPERCRYSTAL == pProp->dwItemKind3 )
			{
				if( m_pItem[N_OFFER] )
					m_pItem[N_OFFER]->SetExtra ( 0 );
				pItem->SetExtra(pItem->GetExtra() + 1);
				m_pItem[N_OFFER] = pItem; 
			}
			else										
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_GHOUSE_OPERPIECE) );	//등록 할 수 없는 아이템입니다.
				return FALSE; 
			}
		}
		else if( PtInRect( &m_rectSlot[N_OFFERMIX], point ) )	//오퍼융합제
		{
			if( IK3_OPERMIX == pProp->dwItemKind3 )
			{
				if( m_pItem[N_OFFERMIX] )
					m_pItem[N_OFFERMIX]->SetExtra ( 0 );
				pItem->SetExtra(pItem->GetExtra() + 1);
				m_pItem[N_OFFERMIX] = pItem; 
			}
			else									
			{
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_GHOUSE_OPERMIX) );	//등록 할 수 없는 아이템입니다.
				return FALSE; 
			}
		}
	
	}
	else
	{
		//AfxMessageBox( "No inventory item" );
		return FALSE;
	}

// 	//check enable button OK
// 	for( int i = 0; i < N_MAX; ++i )
// 	{
// 		if( m_objID[ i ] == 0 )
// 			return FALSE;
// 	}

	if( m_pItem[N_OFFER] && m_pItem[N_OFFERMIX] )
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
		if( pBtnOK )
			pBtnOK->EnableWindow( TRUE );
	}
	else 
		return TRUE;
	
	return TRUE;
}

void CWndProduceOffer::ReceivedPacket( )
{
	m_bSendPacket = FALSE;
	for( int i = 0; i <= N_OFFERMIX; ++i )
	{
		if( m_pItem[i] )
			m_pItem[i]->SetExtra ( 0 );
		m_pItem[i] = NULL;
	}
}


//--------------------------------------------------------------------------------------------------------------
//CWndProduceSeed
CWndProduceSeed::CWndProduceSeed( ) : m_bSendPacket(FALSE)
{
	for( int i = 0; i <= N_CIDMIX; ++i )
	{
		m_pItem[i] = NULL;
	}
}

CWndProduceSeed::~CWndProduceSeed( )
{
}

void CWndProduceSeed::OnDestroy()
{
	for( int i = 0; i <= N_CIDMIX; ++i )
	{
		if(m_pItem[i])
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_pItem[i] ) )
				m_pItem[i]->SetExtra(0);
		}
	}
}

void CWndProduceSeed::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	if(m_bSendPacket == FALSE)
	{
		if( PtInRect( &m_rectSlot[N_CID], point ) && m_pItem[N_CID])
		{
			m_pItem[N_CID]->SetExtra ( 0 );
			m_pItem[N_CID] = NULL;
		}
		else if( PtInRect( &m_rectSlot[N_CIDMIX], point ) && m_pItem[N_CIDMIX])
		{
			m_pItem[N_CIDMIX]->SetExtra ( 0 );
			m_pItem[N_CIDMIX] = NULL;
		} 
		

		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
		if( m_pItem[N_CID] && m_pItem[N_CIDMIX] )
		{
			if( pBtnOK ) 
				pBtnOK->EnableWindow( TRUE );
		}
		else 
		{
			if( pBtnOK ) 
				pBtnOK->EnableWindow( FALSE );
		}
	}
}

void CWndProduceSeed::OnDraw( C2DRender* p2DRender ) 
{ 
	for( int i = 0; i <= N_CIDMIX; ++i )
	{
		if( m_pItem[ i ] )
		{
			//if( IsUsableItem( m_pItem[ i ] ) )
				p2DRender->RenderTexture( m_rectSlot[i].TopLeft(), m_pItem[ i ]->GetTexture() );
		}
	}
} 

void CWndProduceSeed::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	CWndStatic* pWndSlot1 = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
	CWndStatic* pWndSlot2 = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );

	if( !( pWndSlot1 && pWndSlot2 ) )
		FLERROR_LOG( PROGRAM_NAME, _T( "CWndProduceOfferSeed::OnInitialUpdate()" ) );

	m_rectSlot[N_CID] = pWndSlot1->GetWndRect( );
	m_rectSlot[N_CIDMIX] = pWndSlot2->GetWndRect( );

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

	CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
	if( !pBtnOK )
		FLERROR_LOG( PROGRAM_NAME, _T( "CWndProduceSeed::OnInitialUpdate()" ) );
	for( int i = 0; i <= N_CIDMIX; ++i )
	{
		m_pItem[i] = NULL;
	}

	pBtnOK->EnableWindow( FALSE );
} 

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndProduceSeed::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BARUNA_SEED, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndProduceSeed::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( EN_RETURN == message )
	{
		SendPacket();
		return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
	}

	switch( nID )
	{
	case WIDC_BUTTON1:
		SendPacket();
		break;

	case WIDC_BUTTON2:
		Destroy();
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

BOOL CWndProduceSeed::SendPacket( )
{
	m_bSendPacket = TRUE;

	if( !m_pItem[N_CID] )
		return FALSE;

	PT_ITEM_SPEC pProp = m_pItem[N_CID]->GetProp();
	if( !pProp )
		FLERROR_LOG( PROGRAM_NAME, _T( "CWndProduceSeed::SendPacket" ) );

	//아이템 레벨이 1보다 크면 상위단계로 업그레이드 요청
	if( pProp->dwItemLV > 1 )	
	{
		T_PACKET_CIDPIECE_UPGRADE kPacket = { 0 };
		//mem_set( &kPacket, 0, sizeof( kPacket ) );
		kPacket.dwCidPieceObjid = m_pItem[N_CID]->m_dwObjId;
		kPacket.dwCidPieceCombineObjid = m_pItem[N_CIDMIX]->m_dwObjId;
		g_DPlay.SendReqCidPieceUpgrade( &kPacket );
	}
	// 1이면 시드제작으로 요청
	else if ( pProp->dwItemLV == 1 )
	{
		T_PACKET_CID_CREATE kPacket = { 0 };
		//mem_set( &kPacket, 0, sizeof( kPacket ) );
		kPacket.dwCidObjid = m_pItem[N_CID]->m_dwObjId;
		kPacket.dwCidCombineObjid = m_pItem[N_CIDMIX]->m_dwObjId;

		g_DPlay.SendReqCidCreate( &kPacket );
	}
	//이리 들어오면 데이터 오류
	else 
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CWndProduceSeed::SendPacket" ) );
		return FALSE;
	}

	CWndButton* pBtn = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
	if( pBtn )
		pBtn->EnableWindow( FALSE );

	return	TRUE;
}

BOOL CWndProduceSeed::Process()
{
	return TRUE;
}

BOOL CWndProduceSeed::SetItem(FLItemElem* pItem)
{
	if( !IsUsableItem( pItem ) )
		return FALSE;

	PT_ITEM_SPEC pProp = pItem->GetProp();
	if( NULL == pProp )
		return FALSE;

	if( IK3_CIDCRYSTAL == pProp->dwItemKind3 )	
	{		
		if( !pItem )
		{
			g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_NEWSMELT_CID05) );	//수량 부족( 10개여야함 )
			return FALSE;
		}

		if( pProp->dwItemLV > 1 && 10 > pItem->m_nItemNum )
		{
			g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_NEWSMELT_CID05) );	//수량 부족( 10개여야함 )
			return FALSE;
		}

		if( m_pItem[N_CID] )
			m_pItem[N_CID]->SetExtra ( 0 );
		pItem->SetExtra(pItem->GetExtra() + 10);
		m_pItem[N_CID] = pItem; 
	}
	else if( IK3_CIDMIX == pProp->dwItemKind3 )	
	{
		// 결정체가 등록되어있다면 등급을 체크
		if( m_pItem[N_CID] )
		{
			PT_ITEM_SPEC pPropCry = m_pItem[N_CID]->GetProp();

			if( !(pPropCry && pProp) )
				return FALSE;

			//결정체의 레벨과 융합제의 레벨이 틀리면 등록불가
			if( pPropCry->dwItemLV != pProp->dwItemLV )
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_NEWSMELT_CID06) ); //등록되어진 시드 결정체에 사용할 수 없는 등급의 시드 융합제입니다.
				return FALSE;
			}

			if( m_pItem[N_CIDMIX] )
				m_pItem[N_CIDMIX]->SetExtra ( 0 );
			pItem->SetExtra(pItem->GetExtra() + 1);
			m_pItem[N_CIDMIX] = pItem; 
		}
	}
	else
		return FALSE;

	
	if( m_pItem[N_CID] && m_pItem[N_CIDMIX] )
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
		if( pBtnOK )
			pBtnOK->EnableWindow( TRUE );
	}

	return TRUE;
}

BOOL CWndProduceSeed::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame	= pShortcut->m_pFromWnd->GetFrameWnd();
	if( !pWndFrame )
	{
		FLASSERT( 0 );
		return FALSE;
	}

	//gmpbigsun( 20101116 ) : 휴대가방 검사 추가
	if( APP_INVENTORY == pWndFrame->GetWndId( ) || APP_BAG_EX == pWndFrame->GetWndId() )			// 인벤에서 온 아이템은 조건검사 
	{
		FLItemElem* pItem;
		pItem = g_pPlayer->m_Inventory.GetAtId( pShortcut->m_dwId );
		
		if( !IsUsableItem( pItem ) )
			return FALSE;

		PT_ITEM_SPEC pProp = pItem->GetProp();
		if( !pProp )
			return FALSE;

       	//check slot
		if( PtInRect( &m_rectSlot[N_CID], point ) )		// cid결정체
		{	
			if( IK3_CIDCRYSTAL == pProp->dwItemKind3 )	
			{
				FLItemElem* pItem;
				
				pItem = g_pPlayer->m_Inventory.GetAtId( pShortcut->m_dwId );
				
				if( !pItem )
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_NEWSMELT_CID05) );	//수량 부족( 10개여야함 )
					return FALSE;
				}

				if( pProp->dwItemLV > 1 && 10 > pItem->m_nItemNum )
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_NEWSMELT_CID05) );	//수량 부족( 10개여야함 )
					return FALSE;
				}

				if( m_pItem[N_CID] )
					m_pItem[N_CID]->SetExtra ( 0 );
				pItem->SetExtra(pItem->GetExtra() + 10);
				m_pItem[N_CID] = pItem; 
			}
			else								
			{ 
				g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_NEWSMELT_CID01) );	//등록할수 없는 아이템
				return FALSE; 
			}
		}
		else if( PtInRect( &m_rectSlot[N_CIDMIX], point ) )	//cid융합제
		{
			if( IK3_CIDMIX == pProp->dwItemKind3 )	
			{
                // 결정체가 등록되어있다면 등급을 체크
			
				if( m_pItem[N_CID] )
				{
					PT_ITEM_SPEC pPropCry = m_pItem[N_CID]->GetProp();

					if( !(pPropCry && pProp) )
						return FALSE;

					//결정체의 레벨과 융합제의 레벨이 틀리면 등록불가
					if( pPropCry->dwItemLV != pProp->dwItemLV )
					{
						g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_NEWSMELT_CID06) ); //등록되어진 시드 결정체에 사용할 수 없는 등급의 시드 융합제입니다.
						return FALSE;
					}

					if( m_pItem[N_CIDMIX] )
						m_pItem[N_CIDMIX]->SetExtra ( 0 );
					pItem->SetExtra(pItem->GetExtra() + 1);
					m_pItem[N_CIDMIX] = pItem; 
				}

			}
			
			else							
			{ 
				g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_NEWSMELT_CID01) );	//등록할수 없는 아이템
				return FALSE; 
			}
		}
	
	}
	else
	{
//		AfxMessageBox( "No inventory item" );
		return FALSE;
	}

// 	//check enable button OK
// 	for( int i = 0; i < N_MAX; ++i )
// 	{
// 		if( m_objID[ i ] == 0 )
// 			return FALSE;
// 	}

	if( m_pItem[N_CID] && m_pItem[N_CIDMIX] )
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
		if( pBtnOK )
			pBtnOK->EnableWindow( TRUE );
	}
	else 
	{
		return TRUE;
	}
	
	
	return TRUE;
}

void CWndProduceSeed::ReceivedPacket( )
{
	m_bSendPacket = FALSE;
	for( int i = 0; i <= N_CIDMIX; ++i )
	{
		if( m_pItem[i] )
			m_pItem[i]->SetExtra ( 0 );
		m_pItem[i] = NULL;
	}
}

//--------------------------------------------------------------------------------------------------------------
//CWndExtractOffer
CWndExtractOffer::CWndExtractOffer( ) : m_bSendPacket(FALSE)
{
	for( int i = 0; i <= N_EXTOFFER; ++i )
	{
		m_pItem[i] = NULL;
	}
}

CWndExtractOffer::~CWndExtractOffer( )
{
}

void CWndExtractOffer::OnDestroy()
{
	for( int i = 0; i <= N_EXTOFFER; ++i )
	{
		if(m_pItem[i])
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_pItem[i] ) )
				m_pItem[i]->SetExtra(0);
		}
	}
}

void CWndExtractOffer::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	if(m_bSendPacket == FALSE)
	{
		if( PtInRect( &m_rectSlot[N_EXTOFFER], point ) && m_pItem[N_EXTOFFER])
		{
			m_pItem[N_EXTOFFER]->SetExtra ( 0 );
			m_pItem[N_EXTOFFER] = NULL;
		}
		

		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
		if( m_pItem[N_EXTOFFER] )
		{
			if( pBtnOK ) 
				pBtnOK->EnableWindow( TRUE );
		}
		else 
		{
			if( pBtnOK ) 
				pBtnOK->EnableWindow( FALSE );
		}
	}
}

void CWndExtractOffer::OnDraw( C2DRender* p2DRender ) 
{
	for( int i = 0; i <= N_EXTOFFER; ++i )
	{
		if( m_pItem[i] )
		{		
//			if( IsUsableItem( m_pItem[i] ) )
				p2DRender->RenderTexture( m_rectSlot[i].TopLeft(), m_pItem[i]->GetTexture() );
		}
	}
} 

void CWndExtractOffer::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndStatic* pWndSlot1 = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );

	if( !pWndSlot1 )
		FLERROR_LOG( PROGRAM_NAME, _T( "CWndProduceOfferSeed::OnInitialUpdate()" ) );

	m_rectSlot[N_EXTOFFER] = pWndSlot1->GetWndRect( );

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

	CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
	if( !pBtnOK )
		FLERROR_LOG( PROGRAM_NAME, _T( "CWndExtractOffer::OnInitialUpdate()" ) );
	for( int i = 0; i <= N_EXTOFFER; ++i )
	{
		m_pItem[i] = NULL;
	}

	pBtnOK->EnableWindow( FALSE );
} 

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndExtractOffer::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BARUNA_EXTRACT_OFFER, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndExtractOffer::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( EN_RETURN == message )
	{
		SendPacket();
		return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
	}

	switch( nID )
	{
	case WIDC_BUTTON1:
		SendPacket();
		break;

	case WIDC_BUTTON2:
		Destroy();
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

BOOL CWndExtractOffer::SendPacket( )
{
	m_bSendPacket = TRUE;

	T_PACKET_OPER_EXTRACT kPacket = { 0 };
	//mem_set( &kPacket, 0, sizeof( kPacket ) );
	kPacket.dwMaterialObjid = m_pItem[N_EXTOFFER]->m_dwObjId;
	g_DPlay.SendReqOperExtract( &kPacket );

	CWndButton* pBtn = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
	if( pBtn )
		pBtn->EnableWindow( FALSE );

	return	TRUE;
}

BOOL CWndExtractOffer::Process()
{
	return TRUE;
}

BOOL CWndExtractOffer::SetItem(FLItemElem* pItemElem)
{
	if( !IsUsableItem( pItemElem ) )
		return FALSE;

	PT_ITEM_SPEC pProp = pItemElem->GetProp();
	if( NULL == pProp )
		return FALSE;

	if( pProp->dwItemKind1 == IK1_ARMOR || pProp->dwReferStat1 == WEAPON_GENERAL)
	{
		if( pItemElem->IsAccessory( ) == TRUE || pProp->IsAccessory() == TRUE )
			return FALSE;
		else
		{
			if( m_pItem[N_EXTOFFER] )
				m_pItem[N_EXTOFFER]->SetExtra ( 0 );
			pItemElem->SetExtra(pItemElem->GetExtra() + 1);
			m_pItem[N_EXTOFFER] = pItemElem; 
		}
	}
	
	if(m_pItem[N_EXTOFFER])
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
		if( pBtnOK )
			pBtnOK->EnableWindow( TRUE );
	}

	return TRUE;
}

BOOL CWndExtractOffer::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame	= pShortcut->m_pFromWnd->GetFrameWnd();
	if( !pWndFrame )
	{
		FLASSERT( 0 );
		return FALSE;
	}

	//gmpbigsun( 20101116 ) : 휴대가방 검사 추가
	if( APP_INVENTORY == pWndFrame->GetWndId( ) || APP_BAG_EX == pWndFrame->GetWndId() )			// 인벤에서 온 아이템은 조건검사 
	{
		FLItemElem* pItem;

		pItem = g_pPlayer->m_Inventory.GetAtId( pShortcut->m_dwId );
		
		if( !IsUsableItem( pItem ) )
			return FALSE;

		PT_ITEM_SPEC pProp = pItem->GetProp();
		if( !pProp )
			return FALSE;

       	//check slot
		if( PtInRect( &m_rectSlot[N_EXTOFFER], point ) )		// 일반 아이템
		{	
			if( pProp->dwItemKind1 == IK1_ARMOR || pProp->dwReferStat1 == WEAPON_GENERAL)
			{
				if( pItem->IsAccessory( ) == TRUE || pProp->IsAccessory() == TRUE )
					g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_GHOUSE_OPERENCHANT) );	//유니크 이하의 무기및 방어구 만 추출이 가능
				else
				{
					if( m_pItem[N_EXTOFFER] )
						m_pItem[N_EXTOFFER]->SetExtra ( 0 );
					pItem->SetExtra(pItem->GetExtra() + 1);
					m_pItem[N_EXTOFFER] = pItem; 
				}
			}
			else
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_GHOUSE_OPERENCHANT) );	//유니크 이하의 무기및 방어구 만 추출이 가능
			}
		}
	
	}
	else
	{
	//	AfxMessageBox( "No inventory item" );
		return FALSE;
	}
// 
// 	//check enable button OK
// 	for( int i = 0; i < N_MAX; ++i )
// 	{
// 		if( m_objID[ i ] == 0 )
// 			return FALSE;
// 	}
	
	if(m_pItem[N_EXTOFFER])
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
		if( pBtnOK )
			pBtnOK->EnableWindow( TRUE );
	}

	return TRUE;
}

void CWndExtractOffer::ReceivedPacket( )
{
	m_bSendPacket = FALSE;
	for( int i = 0; i <= N_EXTOFFER; ++i )
	{
		m_pItem[i] = NULL;
	}
}

//--------------------------------------------------------------------------------------------------------------
//CWndBarunaNPC
CWndBarunaNPC::CWndBarunaNPC( ) :
m_idNPC( 0 ),
m_bWait( FALSE ),
m_dwExpRatio( 100 ),
m_pGaugeEXP( NULL ),
m_nWidth( 0 )
{
	m_pEnchantBaruna = NULL;
	m_pOperCid = NULL;
	m_pOper = NULL;
	m_pCid = NULL;
	m_pExtractOper = NULL;
	//_JIN_NEW_BARUNA_PEARCING
	m_pPearcing = NULL;
	m_bNoCloseButton = TRUE;
	m_pWakeup = NULL;
	m_pWakeup_Cancel = NULL;
	m_pElement_Smelt = NULL;
#ifdef BARUNA_ULTIMATE_UPDATE
	m_pUltimateTrans = NULL;
#endif
}

CWndBarunaNPC::~CWndBarunaNPC( )
{
	SAFE_DELETE( m_pEnchantBaruna );
	SAFE_DELETE( m_pOperCid );
	SAFE_DELETE( m_pOper );
	SAFE_DELETE( m_pCid );
	SAFE_DELETE( m_pExtractOper );
	//_JIN_NEW_BARUNA_PEARCING
	SAFE_DELETE( m_pPearcing );
	SAFE_DELETE( m_pWakeup );
	SAFE_DELETE( m_pWakeup_Cancel );
	SAFE_DELETE( m_pElement_Smelt );
#ifdef BARUNA_ULTIMATE_UPDATE
	SAFE_DELETE( m_pUltimateTrans );
#endif
	DeleteDeviceObjects();
}

void CWndBarunaNPC::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	if(GetWndBase( APP_BAG_EX )) 
		GetWndBase( APP_BAG_EX )->Destroy();
	if(GetWndBase( APP_DROP_CONFIRM )) 
		GetWndBase( APP_DROP_CONFIRM )->Destroy();
	if(GetWndBase( APP_DROP_ITEM )) 
		GetWndBase( APP_DROP_ITEM )->Destroy();
	if(GetWndBase( APP_BANK)) 
		GetWndBase( APP_BANK )->Destroy();
	if(GetWndBase( APP_BANK_PASSWORD )) 
		GetWndBase( APP_BANK_PASSWORD )->Destroy();
	if(GetWndBase( APP_TRADE )) 
		GetWndBase( APP_TRADE )->Destroy();
	if(GetWndBase( APP_GUILD_BANK )) 
		GetWndBase( APP_GUILD_BANK )->Destroy();

	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	
    
	CWndStatic* pWndLevel = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
	if( pWndLevel )
	{
		pWndLevel->SetTitle( "0" );
	}

	// 윈도를 중앙으로 옮기는 부분.
//	CRect rectRoot = m_pWndRoot->GetLayoutRect();
//	CRect rectWindow = GetWindowRect();
	CPoint point( 0, 110 );
	Move( point );
} 

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndBarunaNPC::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BARUNA_NPC, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndBarunaNPC::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

BOOL CWndBarunaNPC::Process()
{
	//윈도우가 만들어진후 객체가 생성되므로 객체가 생성되면 관련정보를 갱신한다.
	if( m_bWait )
	{
		CMover* pBaruna = prj.GetMover( m_idNPC );
		if( pBaruna )
		{
			m_bWait = FALSE;
			SetNPCInfo( m_idNPC, m_dwLevel, m_dwExpRatio );
		}
	}

	return CWndBase::Process( );
}

BOOL CWndBarunaNPC::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	//main window는 처리할것이없다.
	//혹시나 자식윈도우에서 ::OnDropIcon 에서 FALSE을 뱉을경우 이함수가 불린다.
	return FALSE;
}

void CWndBarunaNPC::OnDraw( C2DRender* p2DRender ) 
{ 
	//gauge
	m_pTheme->RenderGauge( p2DRender, &m_rectEXP, 0xff22ae11, m_pGaugeEXP, &m_texGauFillNormal );

	char szTemp[16] = { 0, };
	FLSPrintf(szTemp, _countof( szTemp ), "%d%%", m_dwExpRatio );
	p2DRender->TextOut( m_rectEXP.left + 20, m_rectEXP.top, szTemp, 0xffdd32ee, 0xff000000 );

	//face
	CMover* pBaruna = prj.GetMover( m_idNPC );
	if( !pBaruna )
		return;

	CModelObject* pModelNPC = (CModelObject*)pBaruna->GetModel( );
	if( !pModelNPC )
		return;

	LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM2 );
	if( !lpFace )
		return;

	CPoint point;
	point = CPoint( lpFace->rect.left-12, lpFace->rect.top-22 );
	
//	if(m_pTexPetStatusBg != NULL)
//		m_pTexPetStatusBg->Render( p2DRender,  point );		

	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

	// 뷰포트 세팅 
	D3DVIEWPORT9 viewport;

	viewport.X      = p2DRender->m_ptOrigin.x + lpFace->rect.left;
	viewport.Y      = p2DRender->m_ptOrigin.y + lpFace->rect.top;
	viewport.Width  = lpFace->rect.Width();
	viewport.Height = lpFace->rect.Height();
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;

	pd3dDevice->SetViewport(&viewport);

	// 프로젝션 
	D3DXMATRIX matProj;
	D3DXMatrixIdentity( &matProj );
	FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;

	//gmpbigsun(20110114)모델 반지름, fov값을 모델 크기비율로 설정
	float fRadius = pModelNPC->GetRadius( );

	// 너무 작을경우 보정
	if( fRadius < 0.8f )
		fRadius = 0.8f;

	fRadius *= 3.0f;
	if( fRadius <= 0.0f || fRadius > 10.0f )		//무효인값일땐 걍 4.0
		fRadius = 4.0f;

	FLOAT fov = D3DX_PI/fRadius;//(4.0f) 796.0f;
	FLOAT h = cos(fov/2.0f) / sin(fov/2.0f);
	FLOAT w = h * fAspect;
	D3DXMatrixOrthoLH( &matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	D3DXMATRIX  matView;

	// 월드 
	D3DXMATRIXA16 matWorld;
//	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot1, matRot2;
	D3DXMATRIXA16 matTrans;

	// 초기화 
//	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matRot1);
	D3DXMatrixIdentity(&matRot2);
	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixIdentity(&matWorld);

	//펫 종류에 따라 설정.
	D3DXVECTOR3 vecEye( .0f, .0f, .0f );
	D3DXVECTOR3 vecLookAt;
//	float fScale = 1.0f;

	D3DXMATRIX* pMat = pModelNPC->GetMatrixBone( 0 );
	vecLookAt.x = pMat->_41;
	vecLookAt.y = pMat->_42;
	vecLookAt.z = pMat->_43;
	vecEye = vecLookAt;
	vecLookAt.z += 0.5f;

//	vecEye.x += 0.5f;
	vecEye.y += 0.5f;
	vecEye.z -= 3.0f; //* fRadius );

//	fScale = (1.0f / fRadius ) * 1.2f; // ( 2.0f * fRadius );	
//	D3DXMatrixScaling(&matScale, fScale, fScale, fScale);
	D3DXMatrixLookAtLH( &matView, &vecEye, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	
//	D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld,&matRot1);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	// 랜더링 
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, CWorld::m_dwBgColor, 1.0f, 0 ) ;

	::SetTransformView( matView );
	::SetTransformProj( matProj );

	//gmpbigsun : 윈도 페이스 고정라이트
 	::SetLight( FALSE );
 	::SetFog( FALSE );
 	SetDiffuse( 1.0f, 1.0f, 1.0f );
 	SetAmbient( 1.0f, 1.0f, 1.0f );

	pModelNPC->SetTextureEx( pModelNPC->m_pModelElem->m_nTextureEx );
	pModelNPC->Render(pd3dDevice, &matWorld);
	
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
} 

void CWndBarunaNPC::SetNPCInfo( const OBJID id, DWORD level, DWORD expratio )
{
	m_idNPC = id;
	m_dwLevel = level;
	m_dwExpRatio = expratio;	// 0 ~ 100 

	CMover* pBaruna = prj.GetMover( id );
	if( !pBaruna )
	{
		m_bWait = TRUE;
		return;
	}

	//윈도우 타이틀
	SetTitle( pBaruna->GetName( ) );

	//레벨
	CWndStatic* pWndLevel = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
	if( pWndLevel )
	{
		CString strTemp;
		strTemp.Format( "%d", m_dwLevel );
		pWndLevel->SetTitle( strTemp );
	}

	MakeGaugeRect( );
}

CWndNeuz* CWndBarunaNPC::CreateWnd( BARUNA_WNDS eWnd )
{
	//본인의 정령인지 확인하고 아닐경우는 생성하지 않음
	if( !g_pPlayer )
		return NULL;

	CWorld* pWorld = g_pPlayer->GetWorld( );
	if( !pWorld )
		return NULL;

	CObj* pObj = pWorld->GetObjFocus( );
	if( !pObj )
		return NULL;

	if( pObj->m_dwType != OT_MOVER )
		return NULL;

	CMover* pFocusNpc = (CMover*)pObj;
	if( pFocusNpc->GetId() != m_idNPC )
		return NULL;
	
	//기존창을 없애고 새로 만든다.
	switch( eWnd )
	{
	case BW_BARUNA:		
		SAFE_DELETE( m_pEnchantBaruna );
		m_pEnchantBaruna = new CWndEnchantBaruna;
		m_pEnchantBaruna->Initialize( this );
		return m_pEnchantBaruna;
	case BW_OPERCID:
		SAFE_DELETE( m_pOperCid );
		m_pOperCid = new CWndProduceOfferSeed;
		m_pOperCid->Initialize( this );
		return m_pOperCid;

	case BW_OPER:
		SAFE_DELETE( m_pOper );
		m_pOper = new CWndProduceOffer;
		m_pOper->Initialize( this );
		return m_pOper;

	case BW_CID:
		SAFE_DELETE( m_pCid );
		m_pCid = new CWndProduceSeed;
		m_pCid->Initialize( this );
		return m_pCid;

	case BW_EXTRACT_OPER:
		SAFE_DELETE( m_pExtractOper );
		m_pExtractOper = new CWndExtractOffer;
		m_pExtractOper->Initialize( this );
		return m_pExtractOper;
	case BW_BARUNA_PEARCING://_JIN_NEW_BARUNA_PEARCING
		SAFE_DELETE( m_pPearcing );
		m_pPearcing = new FLWndBarunaPearcing;
		m_pPearcing->Initialize( this );
		return m_pPearcing;
	case BW_BARUNA_WAKEUP:
		SAFE_DELETE( m_pWakeup );
		m_pWakeup = new CFLWndBaruna_Wakeup;
		m_pWakeup->Initialize( this );
		return m_pWakeup;
	case BW_BARUNA_WAKEUP_CANCEL:
		SAFE_DELETE( m_pWakeup_Cancel );
		m_pWakeup_Cancel = new CFLWndBaruna_Wakeup_Cancel;
		m_pWakeup_Cancel->Initialize( this );
		return m_pWakeup_Cancel;
	case BW_BARUNA_ELEMENT_SMELT:
		SAFE_DELETE( m_pElement_Smelt );
		m_pElement_Smelt = new CFLWndBaruna_Element_Smelt;
		m_pElement_Smelt->Initialize( this );
		return m_pElement_Smelt;
#ifdef BARUNA_ULTIMATE_UPDATE
	case BW_BARUNA_ULTIMATE_UPDATETRANS:
		SAFE_DELETE( m_pUltimateTrans );
		m_pUltimateTrans = new CWndBarunaUltimateTrans;
		m_pUltimateTrans->Initialize( this );
		return m_pUltimateTrans;
#endif
	default:
		FLERROR_LOG( PROGRAM_NAME, _T( "CWndBarunaNPC::CreateWnd" ) );
		return NULL;
	}

	return NULL;
}

void CWndBarunaNPC::ReceivedPacket( BARUNA_WNDS eWnd, const DWORD dwResult )
{
	//성공 실패에 따라 효과음 연출
	//rst ? 

	PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());

	switch( eWnd )
	{
	case BW_BARUNA:												
		if( m_pEnchantBaruna )
			m_pEnchantBaruna->ReceivedPacket( );
		break; 
	case BW_OPERCID:		
		if( m_pOperCid )
			m_pOperCid->ReceivedPacket();		
		break;

	case BW_OPER:
		if( m_pOper )
			m_pOper->ReceivedPacket();			
		break;

	case BW_CID:			
		if( m_pCid )
			m_pCid->ReceivedPacket( );			
		break;

	case BW_EXTRACT_OPER:	
		if( m_pExtractOper )
			m_pExtractOper->ReceivedPacket( );	
		break;
	case BW_BARUNA_PEARCING://_JIN_NEW_BARUNA_PEARCING
		if( m_pPearcing )
			m_pPearcing->ReceivedPacket( dwResult );	
		break;
	case BW_BARUNA_WAKEUP:
		if( m_pWakeup )
			m_pWakeup->ReceivedPacket();	
		break;
	case BW_BARUNA_WAKEUP_CANCEL:
		if( m_pWakeup_Cancel )
			m_pWakeup_Cancel->ReceivedPacket();	
		break;
	case BW_BARUNA_ELEMENT_SMELT:
		if( m_pElement_Smelt )
			m_pElement_Smelt->ReceivedPacket();	
		break;
#ifdef BARUNA_ULTIMATE_UPDATE
	case BW_BARUNA_ULTIMATE_UPDATETRANS:
		if( m_pUltimateTrans )
			m_pUltimateTrans->ReceivedPacket();
		break;
#endif
	default:
		FLERROR_LOG( PROGRAM_NAME, _T( "CWndBarunaNPC::ReceivedPacket" ) );
		break;
	}
}

void CWndBarunaNPC::MakeGaugeRect( )
{
	LPWNDCTRL lpEXP   = GetWndCtrl( WIDC_CUSTOM1 );
	m_rectEXP = lpEXP->rect;

	int nWidthClient = lpEXP->rect.Width();
	float fOffsetRight = ( nWidthClient * m_dwExpRatio ) * 0.01f;

	m_rectEXP.right = m_rectEXP.left + (int)fOffsetRight;
	if( nWidthClient != m_nWidth )
	{
		m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &m_rectEXP, 0x64ff0000, m_pGaugeEXP, &m_texGauFillNormal );
		m_nWidth = nWidthClient;
	}
}

HRESULT CWndBarunaNPC::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pGaugeEXP == NULL )
	{
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
			D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pGaugeEXP, NULL );
	}
	
	return S_OK;
}
HRESULT CWndBarunaNPC::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pGaugeEXP );

	return S_OK;//return S_OK;
}
HRESULT CWndBarunaNPC::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}
//---------------------------------------------------------------------------------------------------------
//
//					바루나 피어싱
//			  _JIN_NEW_BARUNA_PEARCING			
//
//---------------------------------------------------------------------------------------------------------
FLWndBarunaPearcing::FLWndBarunaPearcing() : m_bSendPacket(FALSE)
{
	m_bNoCloseButton = TRUE;
	for( int i = 0; i <= N_ITEM; ++i )
	{
		m_pItem[i] = NULL;
	}
}
//---------------------------------------------------------------------------------------------------------
FLWndBarunaPearcing::~FLWndBarunaPearcing()
{

}

//---------------------------------------------------------------------------------------------------------
void FLWndBarunaPearcing::OnDestroy()
{
	for( int i = 0; i <= N_ITEM; ++i )
	{
		if(m_pItem[i])
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_pItem[i] ) )
				m_pItem[i]->SetExtra(0);
		}
	}
}

//---------------------------------------------------------------------------------------------------------
void FLWndBarunaPearcing::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	if(m_bSendPacket == FALSE)
	{
		if( PtInRect( &m_rectSlot_Pearcing[N_PEARCINGITEM], point ) && m_pItem[N_PEARCINGITEM])
		{
			m_pItem[N_PEARCINGITEM]->SetExtra ( 0 );
			m_pItem[N_PEARCINGITEM] = NULL;
		}
		else if( PtInRect( &m_rectSlot_Pearcing[N_RUNE], point ) && m_pItem[N_RUNE])
		{
			m_pItem[N_RUNE]->SetExtra ( 0 );
			m_pItem[N_RUNE] = NULL;
		} 
		else if( PtInRect( &m_rectSlot_Pearcing[N_PROTECT], point ) && m_pItem[N_PROTECT])
		{
			m_pItem[N_PROTECT]->SetExtra ( 0 );
			m_pItem[N_PROTECT] = NULL;
		} 
		else if( PtInRect( &m_rectSlot_Pearcing[N_ITEM], point ) && m_pItem[N_ITEM])
		{
			m_pItem[N_ITEM]->SetExtra ( 0 );
			m_pItem[N_ITEM] = NULL;
		} 
	

		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
		if( m_pItem[N_PEARCINGITEM] && m_pItem[N_RUNE] )
		{
			if( pBtnOK ) 
				pBtnOK->EnableWindow( TRUE );
		}
		else 
		{
			if( pBtnOK ) 
				pBtnOK->EnableWindow( FALSE );
		}
	}
}

//---------------------------------------------------------------------------------------------------------
BOOL FLWndBarunaPearcing::Initialize( CWndBase* pWndParent, DWORD nType)
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BARUNA_PIERCE, 0, CPoint( 0, 0 ), pWndParent );
}

//---------------------------------------------------------------------------------------------------------
BOOL FLWndBarunaPearcing::SendPacket( )
{
	m_bSendPacket = TRUE;
	FLPacketItemUpgradePiercingItemInsertReq kPacket;

	kPacket.dwMainItemObjID			= m_pItem[N_PEARCINGITEM]->m_dwObjId;		//피어싱할 아템
	kPacket.dwMaterialItemObjID		= m_pItem[N_RUNE]->m_dwObjId;				//고대 문자석
	kPacket.dwProtectionItemObjID	= m_pItem[N_PROTECT] == NULL ? NULL_ID : m_pItem[N_PROTECT]->m_dwObjId;			//문자석 보호아템 
	
	if(m_pItem[N_ITEM])
	{
		kPacket.dwProbIncreaseItemObjID = m_pItem[N_ITEM]->m_dwObjId;	//성공확률증가아템
	}

	if( m_pParentWnd )
	{
		kPacket.dwUpgradeNpcObjID	= ((CWndBarunaNPC*)m_pParentWnd)->GetNpcID( );
	}

	g_DPlay.SendPacket( &kPacket );

	CWndButton* pBtn = ( CWndButton* )GetDlgItem( WIDC_BT_START );
	if( pBtn )
		pBtn->EnableWindow( FALSE );

	return	TRUE;
}

//---------------------------------------------------------------------------------------------------------
void FLWndBarunaPearcing::ReceivedPacket( const DWORD dwResult )
{
	m_bSendPacket = FALSE;
	for( int i = 0; i <= N_ITEM; ++i )
	{
		if( m_pItem[i] )
			m_pItem[i]->SetExtra ( 0 );
		m_pItem[i] = NULL;
	}
}

//---------------------------------------------------------------------------------------------------------
BOOL FLWndBarunaPearcing::OnCommand	( UINT nID ,DWORD dwMessage, CWndBase* pWndBase )
{
	switch(nID)
	{
	case IDOK:
		{
			SendPacket();
		}
		break;
	case IDCANCEL:
		break;
	}
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}

//---------------------------------------------------------------------------------------------------------
BOOL FLWndBarunaPearcing::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if( EN_RETURN == message )
	{
		StartPearcing();
		Destroy();
		return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
	}

	switch( nID )
	{
	case WIDC_BT_START:
		StartPearcing();
		break;

	case WIDC_BT_CANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

//---------------------------------------------------------------------------------------------------------
void FLWndBarunaPearcing::StartPearcing()
{
	if(m_pItem[N_PROTECT] == NULL)
	{
		g_WndMng.OpenMessageBox( prj.GetText(TID_BW_PEARCING_QUESTION_MSG_1) ,MB_OKCANCEL,this);
	}	
	else
	{
		SendPacket();
	}
}

//---------------------------------------------------------------------------------------------------------
void FLWndBarunaPearcing::OnDraw( C2DRender* p2DRender )
{
	for( int i = 0; i <= N_ITEM; ++i )
	{
		if( m_pItem[ i ] )
		{
			//if( IsUsableItem( pItem ) )
			p2DRender->RenderTexture( m_rectSlot_Pearcing[i].TopLeft(), m_pItem[ i ]->GetTexture() );
		}
	}
}

//---------------------------------------------------------------------------------------------------------
void FLWndBarunaPearcing::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate(); 
	CWndStatic* pWndBarunaPierce_01 = (CWndStatic*)GetDlgItem( WIDC_PC_BARUNAPIERCE01);
	CWndStatic* pWndBarunaPierce_02 = (CWndStatic*)GetDlgItem( WIDC_PC_BARUNAPIERCE02);
	CWndStatic* pWndBarunaPierce_03 = (CWndStatic*)GetDlgItem( WIDC_PC_BARUNAPIERCE03);
	CWndStatic* pWndBarunaPierce_04 = (CWndStatic*)GetDlgItem( WIDC_PC_BARUNAPIERCE04);

	if( !( pWndBarunaPierce_01 && pWndBarunaPierce_02 && pWndBarunaPierce_03 && pWndBarunaPierce_04) )
	{	
		FLERROR_LOG( PROGRAM_NAME, _T( "FLWndBarunaPearcing::OnInitialUpdate()" ) );
	}

	m_rectSlot_Pearcing[N_PEARCINGITEM] = pWndBarunaPierce_01->GetWndRect( );
	m_rectSlot_Pearcing[N_RUNE] = pWndBarunaPierce_02->GetWndRect( );
	m_rectSlot_Pearcing[N_PROTECT] = pWndBarunaPierce_03->GetWndRect( );
	m_rectSlot_Pearcing[N_ITEM] = pWndBarunaPierce_04->GetWndRect( );


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

	CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_START );
	if( !pBtnOK )
		FLERROR_LOG( PROGRAM_NAME, _T( "FLWndBarunaPearcing::OnInitialUpdate()" ) );

	pBtnOK->EnableWindow( FALSE );
	for( int i = 0; i <= N_ITEM; ++i )
	{
		m_pItem[i] = NULL;
	}

}

//---------------------------------------------------------------------------------------------------------
BOOL FLWndBarunaPearcing::Process()
{
	return FALSE;
}

//---------------------------------------------------------------------------------------------------------
BOOL FLWndBarunaPearcing::SetItem(FLItemElem* pItem)
{
	if( !IsUsableItem( pItem ) )
		return FALSE;

	PT_ITEM_SPEC pProp = pItem->GetProp();
	if( NULL == pProp )
		return FALSE;

	if( pProp->IsBaruna( ) )	
	{
		//피어싱가능한지 남은 피어싱횟수체크 
		int nPRSize = pItem->GetGeneralPiercingSize();
		int nCount = 0;
		for( size_t j = 0; j < pItem->GetGeneralPiercingSize(); j++ )
		{
			if(pItem->GetGeneralPiercingItemID( j ) != 0 )
				nCount++;
		}
		if(nPRSize != 0)
		{
			if(nPRSize == nCount)
			{
				g_WndMng.OpenMessageBox(prj.GetText(TID_BW_PEARCING_ERROR_MSG_3));
				return FALSE;
			}
		}			
		else//피어싱할아이템 슬롯에 올렸는데
		{
			if(m_pItem[N_RUNE])//고대 문자석이 슬롯에 있으면 
			{
				PT_ITEM_SPEC pProp_Slot = m_pItem[N_RUNE]->GetProp();
				switch(pProp->dwItemKind1)
				{
				case IK1_WEAPON: //무기이고 
					{
						if(pProp_Slot->dwItemKind3 == IK3_BARUNAWEA_PIERCE_RUNE)
						{
							if( m_pItem[N_PEARCINGITEM] )
								m_pItem[N_PEARCINGITEM]->SetExtra ( 0 );
							pItem->SetExtra(pItem->GetExtra() + 1);
							m_pItem[N_PEARCINGITEM] = pItem;
							PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
						}
						else
						{
							g_WndMng.OpenMessageBox( prj.GetText(TID_BW_PEARCING_ERROR_MSG_4) );
							return FALSE; 
						}
					}
					break;
				case IK1_ARMOR://방어구 
					{
						if(pProp_Slot->dwItemKind3 == IK3_BARUNAAMO_PIERCE_RUNE)
						{
							if( m_pItem[N_PEARCINGITEM] )
								m_pItem[N_PEARCINGITEM]->SetExtra ( 0 );
							pItem->SetExtra(pItem->GetExtra() + 1);
							m_pItem[N_PEARCINGITEM] = pItem;
							PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
						}
						else
						{
							g_WndMng.OpenMessageBox( prj.GetText(TID_BW_PEARCING_ERROR_MSG_4) );
							return FALSE; 
						}						
					}
					break;
				}
			}
			else//고대 문자석이 슬롯에없으면 그냥 슬롯을 채운다
			{
				m_pItem[N_PEARCINGITEM] = pItem;
				PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
			}
		}				
	}
	else if(pProp->dwItemKind3 == IK3_BARUNAAMO_PIERCE_RUNE || pProp->dwItemKind3 == IK3_BARUNAWEA_PIERCE_RUNE )
	{
		if(m_pItem[N_PEARCINGITEM])
		{
			PT_ITEM_SPEC pProp_Slot = m_pItem[N_PEARCINGITEM]->GetProp();
			switch(pProp->dwItemKind3)
			{
			case IK3_BARUNAWEA_PIERCE_RUNE:
				{
					if(pProp_Slot->dwItemKind1 == IK1_WEAPON)
					{
						if( m_pItem[N_RUNE] )
							m_pItem[N_RUNE]->SetExtra ( 0 );
						pItem->SetExtra(pItem->GetExtra() + 1);
						m_pItem[N_RUNE] = pItem;
						PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
					}
					else
					{
						g_WndMng.OpenMessageBox( prj.GetText(TID_BW_PEARCING_ERROR_MSG_6) );
						return FALSE; 
					}
				}
				break;
			case IK3_BARUNAAMO_PIERCE_RUNE:
				{
					if(pProp_Slot->dwItemKind1 == IK1_ARMOR)
					{
						if( m_pItem[N_RUNE] )
							m_pItem[N_RUNE]->SetExtra ( 0 );
						pItem->SetExtra(pItem->GetExtra() + 1);
						m_pItem[N_RUNE] = pItem;
						PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
					}
					else
					{
						g_WndMng.OpenMessageBox( prj.GetText(TID_BW_PEARCING_ERROR_MSG_6) );
						return FALSE; 
					}
				}
				break;
			}
		}
		else
		{
			if( m_pItem[N_RUNE] )
				m_pItem[N_RUNE]->SetExtra ( 0 );
			pItem->SetExtra(pItem->GetExtra() + 1);
			m_pItem[N_RUNE] = pItem; 
			PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
		}
	}
	else if(pItem->GetProp()->dwItemKind3 == IK3_BARUNA_PROTECTION_RUNE)
	{
		if( m_pItem[N_PROTECT] )
			m_pItem[N_PROTECT]->SetExtra ( 0 );
		pItem->SetExtra(pItem->GetExtra() + 1);
		m_pItem[N_PROTECT] = pItem; 
		PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
	}
	else if(pItem->GetProp()->dwItemKind3 == IK3_BARUNA_PIERCE_RATE)
	{
		if( m_pItem[N_ITEM] )
			m_pItem[N_ITEM]->SetExtra ( 0 );
		pItem->SetExtra(pItem->GetExtra() + 1);
		m_pItem[N_ITEM] = pItem; 
		PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
	}
	else
		return FALSE;

	
	if( m_pItem[N_PEARCINGITEM] && m_pItem[N_RUNE] )
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
		if( pBtnOK )
			pBtnOK->EnableWindow( TRUE );
	}

	return TRUE;
}

//---------------------------------------------------------------------------------------------------------
BOOL FLWndBarunaPearcing::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
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
	FLItemElem* pItem;
	pItem = g_pPlayer->m_Inventory.GetAtId( pShortcut->m_dwId );
		
	if( !IsUsableItem( pItem ) )
		return FALSE;

	PT_ITEM_SPEC pProp = pItem->GetProp();
	if( NULL == pProp )
		return FALSE;

	//check slot
	if( PtInRect( &m_rectSlot_Pearcing[N_PEARCINGITEM], point ) )		// 피어싱할아이템
	{
		if( pProp->IsBaruna( ) )	
		{
			//피어싱가능한지 남은 피어싱횟수체크 
			int nPRSize = pItem->GetGeneralPiercingSize();
			int nCount = 0;
			for( size_t j = 0; j < pItem->GetGeneralPiercingSize(); j++ )
			{
				if(pItem->GetGeneralPiercingItemID( j ) != 0 )
					nCount++;
			}
			if(nPRSize != 0)
			{
				if(nPRSize == nCount)
				{
					g_WndMng.OpenMessageBox(prj.GetText(TID_BW_PEARCING_ERROR_MSG_3));
					return FALSE;
				}
			}			
			else//피어싱할아이템 슬롯에 올렸는데
			{
				if(m_pItem[N_RUNE])//고대 문자석이 슬롯에 있으면 
				{
					PT_ITEM_SPEC pProp_Slot = m_pItem[N_RUNE]->GetProp();
					switch(pProp->dwItemKind1)
					{
					case IK1_WEAPON: //무기이고 
						{
							if(pProp_Slot->dwItemKind3 == IK3_BARUNAWEA_PIERCE_RUNE)
							{
								if( m_pItem[N_PEARCINGITEM] )
									m_pItem[N_PEARCINGITEM]->SetExtra ( 0 );
								pItem->SetExtra(pItem->GetExtra() + 1);
								m_pItem[N_PEARCINGITEM] = pItem;
								PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
							}
							else
							{
								g_WndMng.OpenMessageBox( prj.GetText(TID_BW_PEARCING_ERROR_MSG_4) );
								return FALSE; 
							}
						}
						break;
					case IK1_ARMOR://방어구 
						{
							if(pProp_Slot->dwItemKind3 == IK3_BARUNAAMO_PIERCE_RUNE)
							{
								if( m_pItem[N_PEARCINGITEM] )
									m_pItem[N_PEARCINGITEM]->SetExtra ( 0 );
								pItem->SetExtra(pItem->GetExtra() + 1);
								m_pItem[N_PEARCINGITEM] = pItem;
								PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
							}
							else
							{
								g_WndMng.OpenMessageBox( prj.GetText(TID_BW_PEARCING_ERROR_MSG_4) );
								return FALSE; 
							}						
						}
						break;
					}
				}
				else//고대 문자석이 슬롯에없으면 그냥 슬롯을 채운다
				{
					m_pItem[N_PEARCINGITEM] = pItem;
					PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
				}
			}				
		}
		else						
		{
			g_WndMng.OpenMessageBox( prj.GetText(TID_BW_PEARCING_ERROR_MSG_2) ); //"바루나 아이템을 등록해야합니다 
			return FALSE; 
		}		
		
	}
	else if( PtInRect( &m_rectSlot_Pearcing[N_RUNE], point ) )		//고대문자석
	{
		if(pProp->dwItemKind3 == IK3_BARUNAAMO_PIERCE_RUNE ||  
			pProp->dwItemKind3 == IK3_BARUNAWEA_PIERCE_RUNE )
		{
			if(m_pItem[N_PEARCINGITEM])
			{
				PT_ITEM_SPEC pProp_Slot = m_pItem[N_PEARCINGITEM]->GetProp();
				switch(pProp->dwItemKind3)
				{
				case IK3_BARUNAWEA_PIERCE_RUNE:
					{
						if(pProp_Slot->dwItemKind1 == IK1_WEAPON)
						{
							if( m_pItem[N_RUNE] )
								m_pItem[N_RUNE]->SetExtra ( 0 );
							pItem->SetExtra(pItem->GetExtra() + 1);
							m_pItem[N_RUNE] = pItem;
							PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
						}
						else
						{
							g_WndMng.OpenMessageBox( prj.GetText(TID_BW_PEARCING_ERROR_MSG_6) );
							return FALSE; 
						}
					}
					break;
				case IK3_BARUNAAMO_PIERCE_RUNE:
					{
						if(pProp_Slot->dwItemKind1 == IK1_ARMOR)
						{
							if( m_pItem[N_RUNE] )
								m_pItem[N_RUNE]->SetExtra ( 0 );
							pItem->SetExtra(pItem->GetExtra() + 1);
							m_pItem[N_RUNE] = pItem;
							PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
						}
						else
						{
							g_WndMng.OpenMessageBox( prj.GetText(TID_BW_PEARCING_ERROR_MSG_6) );
							return FALSE; 
						}
					}
					break;
				}
			}
			else
			{
				if( m_pItem[N_RUNE] )
					m_pItem[N_RUNE]->SetExtra ( 0 );
				pItem->SetExtra(pItem->GetExtra() + 1);
				m_pItem[N_RUNE] = pItem; 
				PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
			}
		}			
		else
		{
			g_WndMng.OpenMessageBox( prj.GetText(TID_BW_PEARCING_ERROR_MSG_5) );
			return FALSE; 
		}
	}
	else if( PtInRect( &m_rectSlot_Pearcing[N_PROTECT], point ) )		//문자석 보호 아이템
	{
		if(pItem->GetProp()->dwItemKind3 == IK3_BARUNA_PROTECTION_RUNE)
		{
			if( m_pItem[N_PROTECT] )
				m_pItem[N_PROTECT]->SetExtra ( 0 );
			pItem->SetExtra(pItem->GetExtra() + 1);
			m_pItem[N_PROTECT] = pItem; 
			PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
		}
	}
	else if( PtInRect( &m_rectSlot_Pearcing[N_ITEM], point ) )		//성공확률증가 아이템
	{
		if(pItem->GetProp()->dwItemKind3 == IK3_BARUNA_PIERCE_RATE)
		{
			if( m_pItem[N_ITEM] )
				m_pItem[N_ITEM]->SetExtra ( 0 );
			pItem->SetExtra(pItem->GetExtra() + 1);
			m_pItem[N_ITEM] = pItem; 
			PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
		}
	}

	if( m_pItem[N_PEARCINGITEM] && m_pItem[N_RUNE] )
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_START );
		if( pBtnOK )
			pBtnOK->EnableWindow( TRUE );
	}
	else 
		return TRUE;

	return TRUE;
}
//---------------------------------------------------------------------------------------------------------
//			바루나 각성
//---------------------------------------------------------------------------------------------------------
CFLWndBaruna_Wakeup::CFLWndBaruna_Wakeup():
m_pVertexBufferGauge(NULL),
m_nCurrGauage( 0 ),
m_pTexGuage( NULL ),
m_bGauageBarState(FALSE)
{
	m_bNoCloseButton = TRUE;

	m_pItem = NULL;
}
//---------------------------------------------------------------------------------------------------------
CFLWndBaruna_Wakeup::~CFLWndBaruna_Wakeup()
{

}

void CFLWndBaruna_Wakeup::OnDestroy()
{
	if(m_pItem)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItem ) )
			m_pItem->SetExtra(0);
	}
}

//---------------------------------------------------------------------------------------------------------
BOOL CFLWndBaruna_Wakeup::Initialize		( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BARUNA_WAKEUP, 0, CPoint( 0, 0 ), pWndParent );
}

//---------------------------------------------------------------------------------------------------------
void CFLWndBaruna_Wakeup::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	if(m_bGauageBarState == FALSE)
	{
		if( PtInRect( &m_rectSlot_BrunaITem, point ) )// 각성할 아이템
		{
			InitCtrl();
		}
	}
}

//---------------------------------------------------------------------------------------------------------
BOOL CFLWndBaruna_Wakeup::OnChildNotify	( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_BT_OK:
		m_bGauageBarState = TRUE;
		break;

	case WIDC_BT_CANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 

}
//---------------------------------------------------------------------------------------------------------
void CFLWndBaruna_Wakeup::SetWakeup(BOOL bSend)
{
	ULONGLONG ulUserCost = g_pPlayer->GetGold();
	if( ulUserCost < 100000 )
	{
		g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_WAKEUP_E_TEXT05) );
	}
	else
	{
		if( bSend == TRUE)
		{
			FLPacketItemUpgradeRandomOptionGenerateReq kPacket;

			kPacket.dwMainItemObjID			= m_pItem->m_dwObjId;

			if( m_pParentWnd )
			{
				kPacket.dwUpgradeNpcObjID	= ((CWndBarunaNPC*)m_pParentWnd)->GetNpcID( );
			}

			g_DPlay.SendPacket( &kPacket );
		}		
	}
}
//---------------------------------------------------------------------------------------------------------
void CFLWndBaruna_Wakeup::InitCtrl()
{
	RestoreDeviceObjects( );

	CWndStatic* pWndGauage = (CWndStatic*)GetDlgItem( WIDC_PC_DELAY );
	if( pWndGauage )
	{
		CRect rect = pWndGauage->GetWndRect();
		m_nCurrGauage = rect.top;
	}

	m_strPenya.Format("0");
	CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
	if( !pBtnOK )
		FLERROR_LOG( PROGRAM_NAME, _T( "CFLWndBaruna_Wakeup::OnInitialUpdate()" ) );

		if(m_pItem)
			m_pItem->SetExtra(0);
		m_pItem = NULL;

	pBtnOK->EnableWindow( FALSE );
}

//---------------------------------------------------------------------------------------------------------
void CFLWndBaruna_Wakeup::OnDraw			( C2DRender* p2DRender )
{

	if( m_pItem )
	{
		m_pWndBarunaPenya->SetTileColor(m_dwColor);
		m_pWndBarunaPenya->SetTitle(m_strPenya);

		//if( IsUsableItem( pItem ) )
			p2DRender->RenderTexture( m_rectSlot_BrunaITem.TopLeft(), m_pItem->GetTexture() );
	}


	FLItemElem* pItemElem = NULL;
	CPoint pointMouse = GetMousePoint();
	//슬롯에 툴팁 띄우기

	if(m_rectSlot_BrunaITem.PtInRect(pointMouse))
	{
		CPoint pointA = pointMouse;
		CRect m_Rect_TempA = m_rectSlot_BrunaITem;

		ClientToScreen( &pointA );
		ClientToScreen( &m_Rect_TempA );

		if(m_pItem)
		{
			g_WndMng.PutToolTip_Item( m_pItem,  pointA, &m_Rect_TempA, APP_BARUNA_WAKEUP );
		}
	}


	CWndStatic* pWndGauage = (CWndStatic*)GetDlgItem( WIDC_PC_DELAY );
	CRect rect;

	if(m_bGauageBarState == TRUE)
	{
		 rect = pWndGauage->GetWndRect();
		if( pWndGauage )
		{
			m_nCurrGauage += 3;
			if(m_nCurrGauage > rect.Width()+18)
			{
                m_nCurrGauage = rect.Width()+18;
				SetWakeup(TRUE);
				m_bGauageBarState = FALSE;
			}
			
			rect.right = m_nCurrGauage;	
			m_pTheme->RenderGauge(p2DRender, &rect, 0xffffffff, m_pVertexBufferGauge, m_pTexGuage);
		}
	}
}
//---------------------------------------------------------------------------------------------------------
HRESULT CFLWndBaruna_Wakeup::RestoreDeviceObjects()
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
HRESULT CFLWndBaruna_Wakeup::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	SAFE_RELEASE( m_pVertexBufferGauge );
	return S_OK;
}
//---------------------------------------------------------------------------------------------------------
HRESULT CFLWndBaruna_Wakeup::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVertexBufferGauge );
	return S_OK;
}
//---------------------------------------------------------------------------------------------------------
void CFLWndBaruna_Wakeup::OnInitialUpdate( )
{
	CWndNeuz::OnInitialUpdate(); 


	//------------- 딜레이 게이지 세팅 ----------------
	m_pTexGuage = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndBar02.tga"   ), 0xffff00ff );

	if( !m_pTexGuage )
		FLERROR_LOG( PROGRAM_NAME, _T( "m_pTexGuage(WndBar02.tga) is NULL" ) );

	RestoreDeviceObjects( );

	CWndStatic* pWndGauage = (CWndStatic*)GetDlgItem( WIDC_PC_DELAY );
	if( pWndGauage )
	{
		CRect rect = pWndGauage->GetWndRect();
		m_nCurrGauage = rect.top;
	}
	//--------------------------------------------------

    // ------------ UI컨트롤 세팅 ----------------------
	CWndStatic* pWndBarunaItem = (CWndStatic*)GetDlgItem( WIDC_PC_BARUNA02 );	// 아이템 슬롯
	m_pWndBarunaPenya = (CWndStatic*)GetDlgItem( WIDC_ST_PENYA );				// 필요 페냐
	
	//하나라도 세팅이 안되있으면 에러로그!
	if( !( pWndBarunaItem && m_pWndBarunaPenya ) )
	{	
		FLERROR_LOG( PROGRAM_NAME, _T( "CFLWndBaruna_Wakeup::OnInitialUpdate()" ) );
	}

	// 각성할 아이템 슬롯 위치 
	m_rectSlot_BrunaITem = pWndBarunaItem->GetWndRect( );

	// UI창을 화면 중앙에 놓음
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

	//필요페냐 초기화
	m_strPenya.Format("0");

	// 버튼은 비활성화 
	CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
	if( !pBtnOK )
		FLERROR_LOG( PROGRAM_NAME, _T( "CFLWndBaruna_Wakeup::OnInitialUpdate()" ) );

	// 슬롯의 아이템은 비운다.
	m_pItem = NULL;

	pBtnOK->EnableWindow( FALSE );
	//--------------------------------------------------
	InitCtrl();
}

//---------------------------------------------------------------------------------------------------------
BOOL CFLWndBaruna_Wakeup::Process( )
{
	return FALSE;
}

//---------------------------------------------------------------------------------------------------------
BOOL CFLWndBaruna_Wakeup::SetItem(FLItemElem* pItem)
{
	if( !IsUsableItem( pItem ) )
		return FALSE;

	PT_ITEM_SPEC pProp = pItem->GetProp();
	if( NULL == pProp )
		return FALSE;

	if(pProp->IsBaruna())
	{
		if(pItem->IsSetRandomOptionExtension() == false)
		{
			switch(pProp->dwItemKind1)
			{
			case IK1_WEAPON: //무기이고 
				{
					m_pItem = pItem;
					PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
					m_strPenya.Format("100000");
				}
				break;
			case IK1_ARMOR:
				{
					m_pItem = pItem;
					PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
					m_strPenya.Format("100000");
				}
				break;
			default:
				//무기랑 방어구만 등록가능함
				g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_WAKEUP_E_TEXT01) );
				break;
			}
		}
		else 
		{
			//	이미 각성되어있는 아이템입니다. 각성 취소 후 사용해 주세요.
			g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_WAKEUP_E_TEXT04) );
		}
	}
	else
	{
		//바루나 아이템만 등록 가능합니다. 아이템 확인후 다시 시도해 주세요.
		g_WndMng.OpenMessageBox( prj.GetText(TID_BW_PEARCING_ERROR_MSG_2) );
	}
	
	if( m_pItem )
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
		if( pBtnOK )
			pBtnOK->EnableWindow( TRUE );
	}

	return TRUE;
}

//---------------------------------------------------------------------------------------------------------
BOOL CFLWndBaruna_Wakeup::OnDropIcon		( LPSHORTCUT pShortcut, CPoint point )
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
	FLItemElem* pItem;
	pItem = g_pPlayer->m_Inventory.GetAtId( pShortcut->m_dwId );

	if( !IsUsableItem( pItem ) )
		return FALSE;

	PT_ITEM_SPEC pProp = pItem->GetProp();
	if( NULL == pProp )
		return FALSE;
	if( PtInRect( &m_rectSlot_BrunaITem, point ) )// 각성할 아이템
	{
		if(m_pItem == NULL)
		{
			if(pProp->IsBaruna())
			{
				if(pItem->IsSetRandomOptionExtension() == false)
				{
					switch(pProp->dwItemKind1)
					{
					case IK1_WEAPON: //무기이고 
						{
							m_pItem = pItem;
							PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
							m_strPenya.Format("100000");
						}
						break;
					case IK1_ARMOR:
						{
							m_pItem = pItem;
							PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
							m_strPenya.Format("100000");
						}
						break;
					default:
						//무기랑 방어구만 등록가능함
						g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_WAKEUP_E_TEXT01) );
						break;
					}
				}
				else 
				{
					//	이미 각성되어있는 아이템입니다. 각성 취소 후 사용해 주세요.
					g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_WAKEUP_E_TEXT04) );
				}
			}
			else
			{
				//바루나 아이템만 등록 가능합니다. 아이템 확인후 다시 시도해 주세요.
				g_WndMng.OpenMessageBox( prj.GetText(TID_BW_PEARCING_ERROR_MSG_2) );
			}
		}
		else
		{
			//이미 아이템이 있음
			g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_WAKEUP_E_TEXT02) );
		}
	}


	if( m_pItem )
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
		if( pBtnOK )
			pBtnOK->EnableWindow( TRUE );
	}
	else 
		return TRUE;
	return TRUE;
}
//---------------------------------------------------------------------------------------------------------
BOOL CFLWndBaruna_Wakeup::OnCommand		( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{	
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}

void CFLWndBaruna_Wakeup::ReceivedPacket()
{
	InitCtrl();
}

//---------------------------------------------------------------------------------------------------------
//			바루나 각성 취소
//---------------------------------------------------------------------------------------------------------
CFLWndBaruna_Wakeup_Cancel::CFLWndBaruna_Wakeup_Cancel():
m_nCurrGauage(0),
m_pVertexBufferGauge(NULL),
m_pTexGuage( NULL ),
m_bGauageBarState(FALSE)
{
	m_bNoCloseButton = TRUE;
	for( int i = 0; i <= N_OBLIVION; ++i )
	{
		m_pItem[i] = NULL;
	}
}
//---------------------------------------------------------------------------------------------------------
CFLWndBaruna_Wakeup_Cancel::~CFLWndBaruna_Wakeup_Cancel()
{

}

//---------------------------------------------------------------------------------------------------------
void CFLWndBaruna_Wakeup_Cancel::OnDestroy()
{
	for( int i = 0; i <= N_OBLIVION; ++i )
	{
		if(m_pItem[i])
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_pItem[i] ) )
				m_pItem[i]->SetExtra(0);
		}
	}
}

//---------------------------------------------------------------------------------------------------------
BOOL CFLWndBaruna_Wakeup_Cancel::Initialize		( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BARUNA_WAKEUP_CANCEL, 0, CPoint( 0, 0 ), pWndParent );

}
//---------------------------------------------------------------------------------------------------------
void CFLWndBaruna_Wakeup_Cancel::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	if( m_bGauageBarState == FALSE )
	{
		if( PtInRect( &m_rectSlot_BrunaITem_Cancel[N_WAKEUP_CANCEL], point ) )// 각성 취소할 아이템
		{
			if(m_pItem[N_WAKEUP_CANCEL])
				m_pItem[N_WAKEUP_CANCEL]->SetExtra ( 0 );
			m_pItem[N_WAKEUP_CANCEL] = NULL;
			InitCtrl();
		}
		else if( PtInRect( &m_rectSlot_BrunaITem_Cancel[N_OBLIVION], point ) )
		{
			if(m_pItem[N_OBLIVION])
				m_pItem[N_OBLIVION]->SetExtra ( 0 );
			m_pItem[N_OBLIVION] = NULL;
			InitCtrl();
		}
	}
}
//---------------------------------------------------------------------------------------------------------
BOOL CFLWndBaruna_Wakeup_Cancel::OnChildNotify	( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_BT_OK:
		m_bGauageBarState = TRUE;
		break;

	case WIDC_BT_CANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 

}
//---------------------------------------------------------------------------------------------------------
void CFLWndBaruna_Wakeup_Cancel::OnDraw			( C2DRender* p2DRender )
{
	for( int i = 0; i <= N_OBLIVION; ++i )
	{
		if( m_pItem[ i ] )
		{
			//if( IsUsableItem( pItem ) )
				p2DRender->RenderTexture( m_rectSlot_BrunaITem_Cancel[i].TopLeft(), m_pItem[ i ]->GetTexture() );
		}
	}

	FLItemElem* pItemElem = NULL;
	CPoint pointMouse = GetMousePoint();
	//슬롯에 툴팁 띄우기
	for( int i = 0; i <= N_OBLIVION; ++i )
	{
		if(m_rectSlot_BrunaITem_Cancel[i].PtInRect(pointMouse))
		{
			CPoint pointA = pointMouse;
			CRect m_Rect_TempA = m_rectSlot_BrunaITem_Cancel[i];

			ClientToScreen( &pointA );
			ClientToScreen( &m_Rect_TempA );

			if(m_pItem[ i ])
			{
				g_WndMng.PutToolTip_Item( m_pItem[ i ],  pointA, &m_Rect_TempA, APP_BARUNA_WAKEUP_CANCEL );
			}
		}
	}

	CWndStatic* pWndGauage = (CWndStatic*)GetDlgItem( WIDC_PC_DELAY );
	CRect rect;

	if(m_bGauageBarState == TRUE)
	{
		rect = pWndGauage->GetWndRect();
		if( pWndGauage )
		{
			m_nCurrGauage += 3;
			if(m_nCurrGauage > rect.Width()+18)
			{
				m_nCurrGauage = rect.Width()+18;
				SendWakeupCancel(TRUE);				
				m_bGauageBarState = FALSE;
			}

			rect.right = m_nCurrGauage;	
			m_pTheme->RenderGauge(p2DRender, &rect, 0xffffffff, m_pVertexBufferGauge, m_pTexGuage);
		}
	}
}
//---------------------------------------------------------------------------------------------------------
void CFLWndBaruna_Wakeup_Cancel::SendWakeupCancel(BOOL bSend)
{
	if( bSend == TRUE)
	{
		FLPacketItemUpgradeRandomOptionInitializeReq kPacket;

		kPacket.dwMainItemObjID			= m_pItem[N_WAKEUP_CANCEL]->m_dwObjId;
		kPacket.dwMaterialItemObjID		= m_pItem[N_OBLIVION]->m_dwObjId;

		if( m_pParentWnd )
		{
			kPacket.dwUpgradeNpcObjID	= ((CWndBarunaNPC*)m_pParentWnd)->GetNpcID( );
		}

		g_DPlay.SendPacket( &kPacket );
		InitCtrl();
	}		
}

//---------------------------------------------------------------------------------------------------------
void CFLWndBaruna_Wakeup_Cancel::InitCtrl()
{
	RestoreDeviceObjects( );

	CWndStatic* pWndGauage = (CWndStatic*)GetDlgItem( WIDC_PC_DELAY );
	if( pWndGauage )
	{
		CRect rect = pWndGauage->GetWndRect();
		m_nCurrGauage = rect.top;
	}

	CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
	if( !pBtnOK )
		FLERROR_LOG( PROGRAM_NAME, _T( "CFLWndBaruna_Wakeup_Cancel::OnInitialUpdate()" ) );

	pBtnOK->EnableWindow( FALSE );
}

//---------------------------------------------------------------------------------------------------------
HRESULT CFLWndBaruna_Wakeup_Cancel::RestoreDeviceObjects()
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
HRESULT CFLWndBaruna_Wakeup_Cancel::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	SAFE_RELEASE( m_pVertexBufferGauge );
	return S_OK;
}
//---------------------------------------------------------------------------------------------------------
HRESULT CFLWndBaruna_Wakeup_Cancel::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVertexBufferGauge );
	return S_OK;
}
//---------------------------------------------------------------------------------------------------------
void CFLWndBaruna_Wakeup_Cancel::OnInitialUpdate( )
{
	CWndNeuz::OnInitialUpdate(); 

	//------------- 딜레이 게이지 세팅 ----------------
	m_pTexGuage = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndBar02.tga"   ), 0xffff00ff );

	if( !m_pTexGuage )
		FLERROR_LOG( PROGRAM_NAME, _T( "m_pTexGuage(Wndguage.tga) is NULL" ) );

	RestoreDeviceObjects( );

	CWndStatic* pWndGauage = (CWndStatic*)GetDlgItem( WIDC_PC_DELAY );
	if( pWndGauage )
	{
		CRect rect = pWndGauage->GetWndRect();
		m_nCurrGauage = rect.top;
	}
	//--------------------------------------------------

	// ------------ UI컨트롤 세팅 ----------------------
	CWndStatic* pWndBarunaItem = (CWndStatic*)GetDlgItem( WIDC_PC_BARUNA );		// 바루나 장비슬롯
	CWndStatic* pWndBarunaItem_Oblivion = (CWndStatic*)GetDlgItem( WIDC_PC_OBLIVION );	// 망각제

	//하나라도 세팅이 안되있으면 에러로그!
	if( !( pWndBarunaItem && pWndBarunaItem_Oblivion ) )
	{	
		FLERROR_LOG( PROGRAM_NAME, _T( "CFLWndBaruna_Wakeup_Cancel::OnInitialUpdate()" ) );
	}

	// 각성할 아이템 슬롯 위치 
	m_rectSlot_BrunaITem_Cancel[N_WAKEUP_CANCEL] = pWndBarunaItem->GetWndRect( );
	m_rectSlot_BrunaITem_Cancel[N_OBLIVION] = pWndBarunaItem_Oblivion->GetWndRect( );

	// UI창을 화면 중앙에 놓음
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

	// 버튼은 비활성화 
	CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
	if( !pBtnOK )
		FLERROR_LOG( PROGRAM_NAME, _T( "CFLWndBaruna_Wakeup_Cancel::OnInitialUpdate()" ) );

	// 슬롯의 아이템은 비운다.
	for( int i = 0; i <= N_OBLIVION; ++i )
	{
		m_pItem[i] = NULL;
	}

	pBtnOK->EnableWindow( FALSE );
	InitCtrl();
}
//---------------------------------------------------------------------------------------------------------
void CFLWndBaruna_Wakeup_Cancel::ReceivedPacket()
{
	InitCtrl();
	for( int i = 0; i <= N_OBLIVION; ++i )
	{
		if( m_pItem[i] )
			m_pItem[i]->SetExtra ( 0 );
		m_pItem[i] = NULL;
	}
}

//---------------------------------------------------------------------------------------------------------
BOOL CFLWndBaruna_Wakeup_Cancel::Process		( )
{
	return FALSE;

}

//---------------------------------------------------------------------------------------------------------
BOOL CFLWndBaruna_Wakeup_Cancel::SetItem(FLItemElem* pItem)
{
	if( !IsUsableItem( pItem ) )
		return FALSE;

	PT_ITEM_SPEC pProp = pItem->GetProp();
	if( NULL == pProp )
		return FALSE;

	if(pProp->IsBaruna())
	{
		if(pItem->IsSetRandomOptionExtension() == true)
		{
			switch(pProp->dwItemKind1)
			{
			case IK1_WEAPON: //무기이고 
				{
					m_pItem[N_WAKEUP_CANCEL] = pItem;
					PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
				}
				break;
			case IK1_ARMOR:
				{
					m_pItem[N_WAKEUP_CANCEL] = pItem;
					PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
				}
				break;
			default:
				//무기랑 방어구만 등록가능함
				g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_WAKEUP_E_TEXT01) );
				break;
			}
		}
		else 
		{
			//	각성된 장비만 등록 가능합니다. 아이템을 확인해 주세요.
			g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_WAKEUPCANCEL_E_TEXT01) );
		}
	}
	else if(pProp->dwItemKind3 == IK3_BARUNA_RANDOMOPTION_INIT)
	{
		m_pItem[N_OBLIVION] = pItem;
		PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
	}
	else
		return FALSE;

	
	if( m_pItem[N_WAKEUP_CANCEL] && m_pItem[N_OBLIVION] )
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
		if( pBtnOK )
			pBtnOK->EnableWindow( TRUE );
	}

	return TRUE;
}

//---------------------------------------------------------------------------------------------------------
BOOL CFLWndBaruna_Wakeup_Cancel::OnDropIcon		( LPSHORTCUT pShortcut, CPoint point )
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
	FLItemElem* pItem;
	pItem = g_pPlayer->m_Inventory.GetAtId( pShortcut->m_dwId );

	if( !IsUsableItem( pItem ) )
		return FALSE;

	PT_ITEM_SPEC pProp = pItem->GetProp();
	if( NULL == pProp )
		return FALSE;
	
	if( PtInRect( &m_rectSlot_BrunaITem_Cancel[N_WAKEUP_CANCEL], point ) )// 각성할 아이템
	{
		if(m_pItem[N_WAKEUP_CANCEL] == NULL)
		{
			if(pProp->IsBaruna())
			{
				if(pItem->IsSetRandomOptionExtension() == true)
				{
					switch(pProp->dwItemKind1)
					{
					case IK1_WEAPON: //무기이고 
						{
							m_pItem[N_WAKEUP_CANCEL] = pItem;
							PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
						}
						break;
					case IK1_ARMOR:
						{
							m_pItem[N_WAKEUP_CANCEL] = pItem;
							PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
						}
						break;
					default:
						//무기랑 방어구만 등록가능함
						g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_WAKEUP_E_TEXT01) );
						break;
					}
				}
				else 
				{
					//	각성된 장비만 등록 가능합니다. 아이템을 확인해 주세요.
					g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_WAKEUPCANCEL_E_TEXT01) );
				}
			}
			else
			{
				//바루나 아이템만 등록 가능합니다. 아이템 확인후 다시 시도해 주세요.
				g_WndMng.OpenMessageBox( prj.GetText(TID_BW_PEARCING_ERROR_MSG_2) );
			}
		}
		else
		{
			//이미 아이템이 있음
			g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_WAKEUP_E_TEXT02) );
		}
	}
	else if(PtInRect( &m_rectSlot_BrunaITem_Cancel[N_OBLIVION], point ))//망각제
	{
		if(m_pItem[N_OBLIVION] == NULL)
		{
			if(pProp->dwItemKind3 == IK3_BARUNA_RANDOMOPTION_INIT)
			{
				m_pItem[N_OBLIVION] = pItem;
				PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
			}
			else 
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_WAKEUPCANCEL_E_TEXT02) );
			}
		}
	}

	if( m_pItem[N_WAKEUP_CANCEL] && m_pItem[N_OBLIVION] )
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
		if( pBtnOK )
			pBtnOK->EnableWindow( TRUE );
	}
	else 
		return TRUE;

	return FALSE;
}
//---------------------------------------------------------------------------------------------------------
BOOL CFLWndBaruna_Wakeup_Cancel::OnCommand		( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return FALSE;

}

//---------------------------------------------------------------------------------------------------------
//			바루나 속성 제련 
//---------------------------------------------------------------------------------------------------------
CFLWndBaruna_Element_Smelt::CFLWndBaruna_Element_Smelt():
m_nCurrGauage(0),
m_pVertexBufferGauge(NULL),
m_pTexGuage(NULL),
m_bGauageBarState(FALSE)
{
	m_bNoCloseButton = TRUE;

	for( int i = 0; i <= N_REJU; ++i )
	{
		m_pItem[i] = NULL;
	}
}
//---------------------------------------------------------------------------------------------------------
CFLWndBaruna_Element_Smelt::~CFLWndBaruna_Element_Smelt()
{
	
}

//---------------------------------------------------------------------------------------------------------
void CFLWndBaruna_Element_Smelt::OnDestroy()
{
	for( int i = 0; i <= N_REJU; ++i )
	{
		if(m_pItem[i])
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_pItem[i] ) )
				m_pItem[i]->SetExtra(0);
		}
	}
}

//---------------------------------------------------------------------------------------------------------
HRESULT CFLWndBaruna_Element_Smelt::RestoreDeviceObjects()
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
HRESULT CFLWndBaruna_Element_Smelt::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVertexBufferGauge );
	return S_OK;
}
//---------------------------------------------------------------------------------------------------------
HRESULT CFLWndBaruna_Element_Smelt::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	SAFE_RELEASE( m_pVertexBufferGauge );
	return S_OK;
}
//---------------------------------------------------------------------------------------------------------
BOOL CFLWndBaruna_Element_Smelt::Initialize		( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BARUNA_ELEMENT_SMELT, 0, CPoint( 0, 0 ), pWndParent );

}
//---------------------------------------------------------------------------------------------------------
void	CFLWndBaruna_Element_Smelt::StartSmelt()
{

	if( !m_pItem[N_BARUNA_ITEM] )
		return;

	int nEnchantCount = m_pItem[N_BARUNA_ITEM]->GetResistAbilityOption( );
	//flyingjin : +3이상부터 경고메세지 출력
	if( nEnchantCount >= _EBARUNA_SAFE_LEVEL && m_pItem[ N_GRACE ] == NULL )		//제련+2이상이고 제련보호 캐쉬아이템을 안쓸경우 경고메시지("뽀개질수있음") 발생
	{
		SAFE_DELETE( g_WndMng.m_pWndConfirmVis );
		g_WndMng.m_pWndConfirmVis = new CWndConfirmVis;
		CWndConfirmVis* pWnd = g_WndMng.m_pWndConfirmVis;
		if( pWnd )
		{
			pWnd->m_eSection = CWndConfirmVis::ETC_BARUNAELEMENT_SMELT_NOTICE;	
			pWnd->m_dwItemIndex = 	m_pItem[N_BARUNA_ITEM]->m_dwItemId;
			pWnd->Initialize( this );

			CRect myRect = GetWndRect();
			CRect childRect = pWnd->GetWndRect();
			int x = myRect.left + ( ( myRect.Width() - childRect.Width() ) / 2 );
			int y = myRect.top - ( childRect.Height() + 10 );
			pWnd->Move( x, y );
		}
	}
	else 
	{
		SetGauageBarState(TRUE);
	}
}
//---------------------------------------------------------------------------------------------------------
void	CFLWndBaruna_Element_Smelt::SendPacket(BOOL bSend)
{
	if(bSend)
	{
		FLPacketItemUpgradeAttributeEnchantReq kPacket;

		kPacket.dwMainItemObjID			= m_pItem[N_BARUNA_ITEM]->m_dwObjId;
		kPacket.dwMaterialItemObjID		= m_pItem[N_ORB]->m_dwObjId;
		kPacket.dwProtectionItemObjID   = m_pItem[N_GRACE] == NULL ? NULL_ID : m_pItem[N_GRACE]->m_dwObjId;
		kPacket.dwProbIncreaseItemObjID	= m_pItem[N_REJU] == NULL ? NULL_ID : m_pItem[N_REJU]->m_dwObjId;

		if( m_pParentWnd )
		{
			kPacket.dwUpgradeNpcObjID	= ((CWndBarunaNPC*)m_pParentWnd)->GetNpcID( );
		}

		g_DPlay.SendPacket( &kPacket );
		InitCtrl();
	}	
}
//---------------------------------------------------------------------------------------------------------
void CFLWndBaruna_Element_Smelt::InitCtrl()
{
	RestoreDeviceObjects( );

	CWndStatic* pWndGauage = (CWndStatic*)GetDlgItem( WIDC_PC_DELAY );
	if( pWndGauage )
	{
		CRect rect = pWndGauage->GetWndRect();
		m_nCurrGauage = rect.top;
	}

	CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
	if( !pBtnOK )
		FLERROR_LOG( PROGRAM_NAME, _T( "CFLWndBaruna_Element_Smelt::InitCtrl()" ) );

	pBtnOK->EnableWindow( FALSE );
}

//---------------------------------------------------------------------------------------------------------
BOOL CFLWndBaruna_Element_Smelt::OnChildNotify	( UINT message, UINT nID, LRESULT* pLResult )
{
	if( EN_RETURN == message )
	{
		Destroy();
		return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
	}

	switch( nID )
	{
	case WIDC_BT_OK:
		StartSmelt();
		break;

	case WIDC_BT_CANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
//---------------------------------------------------------------------------------------------------------
void CFLWndBaruna_Element_Smelt::OnDraw			( C2DRender* p2DRender )
{
	for( int i = 0; i <= N_REJU; ++i )
	{
		if( m_pItem[ i ] )
		{

			//if( IsUsableItem( pItem ) )
			p2DRender->RenderTexture( m_rect_Element_Smelt[i].TopLeft(), m_pItem[ i ]->GetTexture() );
		}
	}

	CWndStatic* pWndGauage = (CWndStatic*)GetDlgItem( WIDC_PC_DELAY );
	CRect rect;

	CPoint pointMouse = GetMousePoint();
	//슬롯에 툴팁 띄우기
	for( int i = 0; i < N_MAX; ++i )
	{
		if(m_rect_Element_Smelt[i].PtInRect(pointMouse))
		{
			CPoint pointA = pointMouse;
			CRect m_Rect_TempA = m_rect_Element_Smelt[i];

			ClientToScreen( &pointA );
			ClientToScreen( &m_Rect_TempA );

			if(m_pItem[ i ] != NULL)
			{
				g_WndMng.PutToolTip_Item( m_pItem[ i ],  pointA, &m_Rect_TempA, APP_BARUNA_WAKEUP );
			}
		}
	}


	if(GetGauageBarState() == TRUE)
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
		if( !pBtnOK )
			FLERROR_LOG( PROGRAM_NAME, _T( "CFLWndBaruna_Element_Smelt::OnDraw()" ) );
		pBtnOK->EnableWindow( FALSE );

		CWndButton* pBtnCancel  = ( CWndButton* )GetDlgItem( WIDC_BT_CANCEL );
		if( !pBtnCancel )
			FLERROR_LOG( PROGRAM_NAME, _T( "CFLWndBaruna_Wakeup::OnDraw()" ) );
		pBtnCancel->EnableWindow( FALSE );

		rect = pWndGauage->GetWndRect();
		if( pWndGauage )
		{
			m_nCurrGauage += 3;
			if(m_nCurrGauage > rect.Width()+18)
			{	
				m_nCurrGauage = rect.Width()+18;
				SendPacket(TRUE);
				SetGauageBarState(FALSE);
			}

			rect.right = m_nCurrGauage;	
			m_pTheme->RenderGauge(p2DRender, &rect, 0xffffffff, m_pVertexBufferGauge, m_pTexGuage);
		}
	}
}
void CFLWndBaruna_Element_Smelt::ReceivedPacket( )
{
	InitCtrl();
	for( int i = 0; i <= N_REJU; ++i )
	{
		if( m_pItem[i] )
			m_pItem[i]->SetExtra ( 0 );
		m_pItem[i] = NULL;
	}
	m_objID_Confirm = NULL_ID;
	CWndButton* pBtnCancel  = ( CWndButton* )GetDlgItem( WIDC_BT_CANCEL );
	if( !pBtnCancel )
		FLERROR_LOG( PROGRAM_NAME, _T( "CFLWndBaruna_Wakeup::OnDraw()" ) );
	pBtnCancel->EnableWindow( TRUE );

}
//---------------------------------------------------------------------------------------------------------
void CFLWndBaruna_Element_Smelt::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	if( m_bGauageBarState == FALSE )
	{
		if( PtInRect( &m_rect_Element_Smelt[N_BARUNA_ITEM], point ) )//바루나 장비
		{
			if( m_pItem[N_BARUNA_ITEM] )
				m_pItem[N_BARUNA_ITEM]->SetExtra ( 0 );
			m_pItem[N_BARUNA_ITEM] = NULL;
		}
		else if( PtInRect( &m_rect_Element_Smelt[N_ORB], point ) )//속성오브
		{
			if( m_pItem[N_ORB] )
				m_pItem[N_ORB]->SetExtra ( 0 );
			m_pItem[N_ORB] = NULL;
		}
		else if( PtInRect( &m_rect_Element_Smelt[N_GRACE], point ) )//바루나의 은총
		{
			if( m_pItem[N_GRACE] )
				m_pItem[N_GRACE]->SetExtra ( 0 );
			m_pItem[N_GRACE] = NULL;
		}
		else if( PtInRect( &m_rect_Element_Smelt[N_REJU], point ) )//원소 활력제
		{
			if( m_pItem[N_REJU] )
				m_pItem[N_REJU]->SetExtra ( 0 );
			m_pItem[N_REJU] = NULL;
		}


		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
		if( m_pItem[N_BARUNA_ITEM] && m_pItem[N_ORB] )
		{
			if( pBtnOK ) 
				pBtnOK->EnableWindow( TRUE );
		}
		else 
		{
			if( pBtnOK ) 
				pBtnOK->EnableWindow( FALSE );
		}
	}
}

//---------------------------------------------------------------------------------------------------------
void CFLWndBaruna_Element_Smelt::OnInitialUpdate( )
{
	CWndNeuz::OnInitialUpdate(); 
	//------------- 딜레이 게이지 세팅 ----------------
	m_pTexGuage = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndBar02.tga"   ), 0xffff00ff );

	if( !m_pTexGuage )
		FLERROR_LOG( PROGRAM_NAME, _T( "m_pTexGuage(Wndguage.tga) is NULL" ) );

	RestoreDeviceObjects( );

	CWndStatic* pWndGauage = (CWndStatic*)GetDlgItem( WIDC_PC_DELAY );
	if( pWndGauage )
	{
		CRect rect = pWndGauage->GetWndRect();
		m_nCurrGauage = rect.top;
	}
	//--------------------------------------------------

	// ------------ UI컨트롤 세팅 ----------------------
	CWndStatic* pWndBarunaItem			= (CWndStatic*)GetDlgItem( WIDC_PC_BARUNA );	// 바루나 장비슬롯
	CWndStatic* pWndBarunaItem_Orb		= (CWndStatic*)GetDlgItem( WIDC_PC_ORB );		// 속성오브
	CWndStatic* pWndBarunaItem_Grace	= (CWndStatic*)GetDlgItem( WIDC_PC_GRACE );		// 바루나의 은총
	CWndStatic* pWndBarunaItem_Reju		= (CWndStatic*)GetDlgItem( WIDC_PC_REJU );		// 원소활력제

	//하나라도 세팅이 안되있으면 에러로그!
	if( !( pWndBarunaItem && 
		   pWndBarunaItem_Orb &&
	       pWndBarunaItem_Grace &&
		   pWndBarunaItem_Reju ) )
	{	
		FLERROR_LOG( PROGRAM_NAME, _T( "CFLWndBaruna_Element_Smelt::OnInitialUpdate( )" ) );
	}

	// 각성할 아이템 슬롯 위치 
	m_rect_Element_Smelt[N_BARUNA_ITEM]	= pWndBarunaItem->GetWndRect( );
	m_rect_Element_Smelt[N_ORB]			= pWndBarunaItem_Orb->GetWndRect( );
	m_rect_Element_Smelt[N_GRACE]		= pWndBarunaItem_Grace->GetWndRect( );
	m_rect_Element_Smelt[N_REJU]		= pWndBarunaItem_Reju->GetWndRect( );

	// UI창을 화면 중앙에 놓음
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

	// 버튼은 비활성화 
	CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
	if( !pBtnOK )
		FLERROR_LOG( PROGRAM_NAME, _T( "CFLWndBaruna_Wakeup_Cancel::OnInitialUpdate()" ) );

	// 슬롯의 아이템은 비운다.
	for( int i = 0; i <= N_REJU; ++i )
	{
		m_pItem[i] = NULL;
	}
	m_objID_Confirm = NULL_ID;

	pBtnOK->EnableWindow( FALSE );
	InitCtrl();

}
//---------------------------------------------------------------------------------------------------------
BOOL CFLWndBaruna_Element_Smelt::Process		( )
{
	return FALSE;
}

//---------------------------------------------------------------------------------------------------------
void	CFLWndBaruna_Element_Smelt::Element_Smelt_Check_Orb(FLItemElem* pItemBaruna)
{


	PT_ITEM_SPEC pPropBaruna = pItemBaruna->GetProp();
	if( NULL == pPropBaruna )
		return;

	if(m_pItem[N_ORB])
	{

		PT_ITEM_SPEC pPropOrb = m_pItem[N_ORB]->GetProp();

		if(pItemBaruna->m_byItemResist == SAI79::NO_PROP)
		{
			//속성오브아이템과 바루나아이템 속성이 같다면 
			switch(pPropBaruna->dwItemKind1)
			{
			case IK1_WEAPON:
				{
					ItemRegister(N_BARUNA_ITEM, pItemBaruna);
				}
				break;
			case IK1_ARMOR:
				{
					if(pPropBaruna->dwItemKind3 == IK3_SUIT)
					{
						ItemRegister(N_BARUNA_ITEM, pItemBaruna);
					}
					else
					{
						//  해당 슬롯은 바루나 방어구 중 슈트만 등록할 수 있습니다.
						g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_E_TEXT12) );
					}
				}
				break;
			default: 
				//무기랑 방어구만 등록가능함
				g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_WAKEUP_E_TEXT01) );
				break;
			}
		}
		else if(pPropOrb->eItemType == pItemBaruna->m_byItemResist)
		{									
			//속성오브아이템과 바루나아이템 속성이 같다면 
			switch(pPropBaruna->dwItemKind1)
			{
			case IK1_WEAPON:
				{
					ItemRegister(N_BARUNA_ITEM, pItemBaruna);
				}
				break;
			case IK1_ARMOR:
				{
					if(pPropBaruna->dwItemKind3 == IK3_SUIT)
					{
						ItemRegister(N_BARUNA_ITEM, pItemBaruna);
					}
					else
					{
						//  해당 슬롯은 바루나 방어구 중 슈트만 등록할 수 있습니다.
						g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_E_TEXT12) );
					}
				}
				break;
			default: 
				//무기랑 방어구만 등록가능함
				g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_WAKEUP_E_TEXT01) );
				break;
			}
		}
		else 
		{
			//	이미 등록되어 있는 속성 오브와 해당 아이템의 속성이 동일하지 않습니다. 확인 후 다시 시도해 주세요
			g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_E_TEXT05) );
			return;
		}
		
	}
	else// 속성 오브 슬롯에 아이템이 없다면  
	{
		//속성오브아이템과 바루나아이템 속성이 같다면 
		switch(pPropBaruna->dwItemKind1)
		{
		case IK1_WEAPON:
			{
				ItemRegister(N_BARUNA_ITEM, pItemBaruna);
			}
			break;
		case IK1_ARMOR:
			{
				if(pPropBaruna->dwItemKind3 == IK3_SUIT)
				{
					ItemRegister(N_BARUNA_ITEM, pItemBaruna);
				}
				else
				{
					//  해당 슬롯은 바루나 방어구 중 슈트만 등록할 수 있습니다.
					g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_E_TEXT12) );
				}
			}
			break;
		default: 
			//무기랑 방어구만 등록가능함
			g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_WAKEUP_E_TEXT01) );
			break;
		}
	}
}

//---------------------------------------------------------------------------------------------------------
BOOL CFLWndBaruna_Element_Smelt::SetItem(FLItemElem* pItem)
{
	if( !IsUsableItem( pItem ) )
		return FALSE;

	PT_ITEM_SPEC pProp = pItem->GetProp();
	if( NULL == pProp )
		return FALSE;

	m_objID_Confirm = pItem->m_dwObjId;

	if(pProp->IsBaruna())//바루나 아이템인가?
	{
		if(pProp->IsUpgradeAble( IUTYPE_ENCHANT_ATTRIBUTE ) == TRUE)//속성제련가능한지?
		{
			if(pItem->GetResistAbilityOption() < 20)
			{
				if( pItem->GetResistAbilityOption() < _EBARUNA_SAFE_LEVEL )
				{
					if( m_pItem[N_GRACE] )//은총있다면 
					{
						g_WndMng.OpenMessageBox( _T( prj.GetText(TID_MMI_BARUNA_ELESMELT_Q_TEXT01) ) ,MB_OKCANCEL, this);	
					}
					else
					{
						Element_Smelt_Check_Orb( pItem );
					}
				}
				else //제련수치 3 이상이면
				{
					if(m_pItem[N_REJU])//원소 활력제가 등록되어있으면 
					{
						if(pItem->GetResistAbilityOption() >= 10)//제련수치 체크함 10이상이면
						{
							g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_E_TEXT11) );
							return FALSE;
						}
						else 
						{
							Element_Smelt_Check_Orb( pItem );
						}

					}
					else
					{
						Element_Smelt_Check_Orb( pItem );
					}
				}
			}
			else 
			{
				//	해당 아이템은 이미 속성 제련을 20까지 한 아이템으로 더 이상 제련이 불가능합니다.
				g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_E_TEXT04) );
				return FALSE;
			}
		}
		else 
		{
			//	해당 아이템은 제련 가능한 아이템이 아닙니다. 아이템 정보를 확인하세요.
			g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_E_TEXT03) );
			return FALSE;
		}
	}
	else if(pProp->dwItemKind3 == IK3_ELEORB)//속성오브인가
	{
		if( m_pItem[N_BARUNA_ITEM] )// 바루나 아이템이 등록되어있다면 
		{
			if(m_pItem[N_BARUNA_ITEM]->m_byItemResist == SAI79::NO_PROP)
			{
				ItemRegister(N_ORB, pItem);
			}
			else if(m_pItem[N_BARUNA_ITEM]->m_byItemResist == pProp->eItemType)//속성이 같은지 체크
			{
				ItemRegister(N_ORB, pItem);
			}
			else
			{
				//	등록되어 있는 장비 아이템과 해당 아이템의 속성이 동일하지 않습니다. 확인 후 다시 시도해 주세요
				g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_E_TEXT08) );
				return FALSE;
			}			
		}
		else // 바루나 아이템 슬롯이 비어있으면 그냥 등록 
		{
			ItemRegister(N_ORB, pItem);
		}
	}
	else if(pProp->dwItemKind3 == IK3_BARUNA_ELE_PROPTECTION || pProp->dwItemKind3 == IK3_BARUNA_ELE_PROPTECTION_KEEP )//은총아이템인가?
	{
		if( m_pItem[N_BARUNA_ITEM] )
		{
			if(m_pItem[N_BARUNA_ITEM]->GetResistAbilityOption() < _EBARUNA_SAFE_LEVEL )
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_MMI_BARUNA_ELESMELT_Q_TEXT01) ) ,MB_OKCANCEL, this);	
			}
			else
			{
				ItemRegister(N_GRACE, pItem);
			}	
		}
		else 
		{
			ItemRegister(N_GRACE, pItem);
		}				
	}
	else if( pProp->dwItemKind3 == IK3_BARUNA_ELE_PROP_INCREASE)//원소활력제 아이템인가?
	{
		if( m_pItem[N_BARUNA_ITEM] )
		{
			if( m_pItem[N_BARUNA_ITEM]->GetResistAbilityOption() >= 10 )//제련수치 10이상이면 
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_E_TEXT06) );
				return FALSE;
			}
			else
			{
				if( m_pItem[N_BARUNA_ITEM]->GetResistAbilityOption() < _EBARUNA_SAFE_LEVEL )// 제련수치 3미만이면 쓸필요없다는 메세지
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_TEXT01 ) );

				}
				ItemRegister(N_REJU, pItem);
			}
		}
		else
		{
			ItemRegister(N_REJU, pItem);
		}
	}
	else
		return FALSE;

	
	if( m_pItem[N_BARUNA_ITEM] && m_pItem[N_ORB] )
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
		if( pBtnOK ) 
			pBtnOK->EnableWindow( TRUE );
	}

	return TRUE;
}

//---------------------------------------------------------------------------------------------------------
BOOL CFLWndBaruna_Element_Smelt::OnDropIcon		( LPSHORTCUT pShortcut, CPoint point )
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
	FLItemElem* pItem;
	pItem = g_pPlayer->m_Inventory.GetAtId( pShortcut->m_dwId );

	if( !IsUsableItem( pItem ) )
		return FALSE;

	PT_ITEM_SPEC pProp = pItem->GetProp();
	if( NULL == pProp )
		return FALSE;

	m_objID_Confirm = pShortcut->m_dwId;
	
	if( PtInRect( &m_rect_Element_Smelt[N_BARUNA_ITEM], point ) )//바루나 장비
	{
		if(m_pItem[N_BARUNA_ITEM] == NULL)// 슬롯이 비어있는지
		{	
			if(pProp->IsBaruna())//바루나 아이템인가?
			{
				if(pProp->IsUpgradeAble( IUTYPE_ENCHANT_ATTRIBUTE ) == TRUE)//속성제련가능한지?
				{
					if(pItem->GetResistAbilityOption() < 20)
					{
						if( pItem->GetResistAbilityOption() < _EBARUNA_SAFE_LEVEL )
						{
							if( m_pItem[N_GRACE] )//은총있다면 
							{
								g_WndMng.OpenMessageBox( _T( prj.GetText(TID_MMI_BARUNA_ELESMELT_Q_TEXT01) ) ,MB_OKCANCEL, this);	
							}
							else
							{
								Element_Smelt_Check_Orb( pItem );
							}
						}
						else //제련수치 3 이상이면
						{
							if(m_pItem[N_REJU])//원소 활력제가 등록되어있으면 
							{
								if(pItem->GetResistAbilityOption() >= 10)//제련수치 체크함 10이상이면
								{
									g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_E_TEXT11) );
									return FALSE;
								}
								else 
								{
									Element_Smelt_Check_Orb( pItem );
								}

							}
							else
							{
								Element_Smelt_Check_Orb( pItem );
							}
						}
					}
					else 
					{
						//	해당 아이템은 이미 속성 제련을 20까지 한 아이템으로 더 이상 제련이 불가능합니다.
						g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_E_TEXT04) );
						return FALSE;
					}
				}
				else 
				{
					//	해당 아이템은 제련 가능한 아이템이 아닙니다. 아이템 정보를 확인하세요.
					g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_E_TEXT03) );
					return FALSE;
				}
			}
			else 
			{
				//	해당 슬롯은 바루나 아이템만 등록할 수 있습니다.
				g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_E_TEXT01) );
				return FALSE;
			}
		}
		else 
		{
			//	이미 등록되어 있는 아이템이 있습니다. 
			g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_E_TEXT02) );
			return FALSE;
		}
	}
	else if(PtInRect( &m_rect_Element_Smelt[N_ORB], point ))//속성오브
	{
		if(m_pItem[N_ORB] == NULL)// 슬롯이 비어있는지
		{
			if(pProp->dwItemKind3 == IK3_ELEORB)//속성오브인가
			{
				if( m_pItem[N_BARUNA_ITEM] )// 바루나 아이템이 등록되어있다면 
				{
					if(m_pItem[N_BARUNA_ITEM]->m_byItemResist == SAI79::NO_PROP)
					{
						ItemRegister(N_ORB, pItem);
					}
					else if(m_pItem[N_BARUNA_ITEM]->m_byItemResist == pProp->eItemType)//속성이 같은지 체크
					{
						ItemRegister(N_ORB, pItem);
					}
					else
					{
						//	등록되어 있는 장비 아이템과 해당 아이템의 속성이 동일하지 않습니다. 확인 후 다시 시도해 주세요
						g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_E_TEXT08) );
						return FALSE;
					}			
				}
				else // 바루나 아이템 슬롯이 비어있으면 그냥 등록 
				{
					ItemRegister(N_ORB, pItem);
				}
			}
			else
			{
				//	해당 슬롯은 속성 오브만 등록 할 수 있습니다.
				g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_E_TEXT07) );
				return FALSE;
			}
		}
		else
		{
			//	이미 등록되어 있는 아이템이 있습니다. 
			g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_E_TEXT02) );
			return FALSE;
		}
	}
	else if(PtInRect( &m_rect_Element_Smelt[N_GRACE], point ))//은총
	{
		if(m_pItem[N_GRACE] == NULL)// 슬롯이 비어있는지
		{
			if(pProp->dwItemKind3 == IK3_BARUNA_ELE_PROPTECTION || pProp->dwItemKind3 == IK3_BARUNA_ELE_PROPTECTION_KEEP )//은총아이템인가?
			{
				if( m_pItem[N_BARUNA_ITEM] )
				{
					if(m_pItem[N_BARUNA_ITEM]->GetResistAbilityOption() < _EBARUNA_SAFE_LEVEL )
					{
						g_WndMng.OpenMessageBox( _T( prj.GetText(TID_MMI_BARUNA_ELESMELT_Q_TEXT01) ) ,MB_OKCANCEL, this);	
					}
					else
					{
						ItemRegister(N_GRACE, pItem);
					}	
				}
				else 
				{
					ItemRegister(N_GRACE, pItem);
				}				
			}
			else 
			{
				//	해당 아이템은 바루나의 은총 or 축복받은 바루나의 은총만 등록 가능합니다.
				g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_E_TEXT09) );
				return FALSE;
			}
		}
		else 
		{
			//	이미 등록되어 있는 아이템이 있습니다. 
			g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_E_TEXT02) );
			return FALSE;
		}
	}
	else if(PtInRect( &m_rect_Element_Smelt[N_REJU], point ))//원소활력제
	{
		if( m_pItem[N_REJU] == NULL )// 슬롯이 비어있는지
		{
			if( m_pItem[N_BARUNA_ITEM] )
			{
				if( m_pItem[N_BARUNA_ITEM]->GetResistAbilityOption() >= 10 )//제련수치 10이상이면 
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_E_TEXT06) );
					return FALSE;
				}
				else 
				{
					if( pProp->dwItemKind3 == IK3_BARUNA_ELE_PROP_INCREASE )//원소활력제 아이템인가?
					{
						if( m_pItem[N_BARUNA_ITEM]->GetResistAbilityOption() < _EBARUNA_SAFE_LEVEL )// 제련수치 3미만이면 쓸필요없다는 메세지
						{
							g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_TEXT01 ) );

						}
						ItemRegister(N_REJU, pItem);
					}
					else 
					{
						//	해당 아이템은 원소 활력제 종류만 등록 가능합니다.
						g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_E_TEXT10) );
						return FALSE;
					}
				}				
			}
			else 
			{
				if( pProp->dwItemKind3 == IK3_BARUNA_ELE_PROP_INCREASE )//원소활력제 아이템인가?
				{
					ItemRegister(N_REJU, pItem);
				}
			}
		}
		else 
		{
			//	이미 등록되어 있는 아이템이 있습니다. 
			g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_E_TEXT02) );
			return FALSE;
		}
	}


	if( m_pItem[N_BARUNA_ITEM] && m_pItem[N_ORB] )
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
		if( pBtnOK ) 
			pBtnOK->EnableWindow( TRUE );
	}
	else 
		return TRUE;

	return FALSE;
}

//---------------------------------------------------------------------------------------------------------
void CFLWndBaruna_Element_Smelt::ItemRegister(int nSlotID, FLItemElem* pItem)
{

	if( m_pItem[nSlotID] )
		m_pItem[nSlotID]->SetExtra ( 0 );
	pItem->SetExtra(pItem->GetExtra() + 1);
	m_pItem[nSlotID] = pItem; 

	PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
}

//---------------------------------------------------------------------------------------------------------
void	CFLWndBaruna_Element_Smelt::ItemConfirmRegister(DWORD dwItemID)
{
	//check valid item
	FLItemElem* pItem;
	pItem = g_pPlayer->m_Inventory.GetAtId( dwItemID );

	if( !IsUsableItem( pItem ) )
		return;

	PT_ITEM_SPEC pProp = pItem->GetProp();
	if( NULL == pProp )
		return;
	
	if(pProp->IsBaruna())
	{
		switch(pProp->dwItemKind1)
		{
		case IK1_WEAPON:
			{
				ItemRegister(N_BARUNA_ITEM, pItem);
			}
			break;
		case IK1_ARMOR:
			{
				if(pProp->dwItemKind3 == IK3_SUIT)
				{
					ItemRegister(N_BARUNA_ITEM, pItem);
				}
				else
				{
					//  해당 슬롯은 바루나 방어구 중 슈트만 등록할 수 있습니다.
					g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_ELESMELT_E_TEXT12) );
					return;
				}
			}
			break;
		default: 
			//무기랑 방어구만 등록가능함
			g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_BARUNA_WAKEUP_E_TEXT01) );
			break;
		}
	}
	else 
	{
		ItemRegister(N_GRACE, pItem);
	}

	if( m_pItem[N_BARUNA_ITEM] && m_pItem[N_ORB] )
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
		if( pBtnOK )
			pBtnOK->EnableWindow( TRUE );
	}
	else 
		return;
}
//---------------------------------------------------------------------------------------------------------
BOOL CFLWndBaruna_Element_Smelt::OnCommand		( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	switch(nID)
	{
	case IDOK:
		ItemConfirmRegister(m_objID_Confirm);
		break;
	case IDCANCEL:
		break;
	}

	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}

#endif //__ENCHANT_BARUNA16
