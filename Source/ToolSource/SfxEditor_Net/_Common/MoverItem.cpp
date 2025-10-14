#include "stdafx.h"
#include "mover.h"
#include "defineText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// pc, npc의 분리 
// m_nCost삭제 

// 거래 상대방을 얻는다.
CMover* CVTInfo::GetOther()
{
//	return m_pOther;
	if( m_objId == NULL_ID )
		return NULL;

	return prj.GetMover( m_objId );
}

// 거래 상대방을 정한다.
void CVTInfo::SetOther( CMover* pMover )
{
//	m_pOther = pMover;
	if( pMover )
	{
		m_objId = pMover->GetId();
	}
	else
	{
		m_objId = NULL_ID;
	}
}

CItemBase* CVTInfo::GetItem( BYTE i )
{
	return m_apItem_VT[i];
}

void CVTInfo::SetItem( BYTE i, CItemBase* pItemBase )
{
	m_apItem_VT[i] = pItemBase;
}

LPCTSTR	CVTInfo::GetTitle()
{
	return m_strTitle.c_str();
}

void CVTInfo::SetTitle( LPCTSTR szTitle )
{
	m_strTitle = szTitle;
}

BOOL CVTInfo::IsVendorOpen()
{
	return (m_strTitle.empty() != true);
}

void CVTInfo::Init( CMover* pOwner )
{
	m_pOwner = pOwner;
	ZeroMemory( m_apItem_VT, sizeof( m_apItem_VT ) );
	TradeClear();
	m_strTitle = "";
}




void CVTInfo::TradeClear()
{
	SetOther( NULL );
	for( int i = 0; i < MAX_TRADE; i++ )
	{
		if( m_apItem_VT[i] )
		{
			m_apItem_VT[i]->SetExtra( 0 );
			m_apItem_VT[i] = NULL;
		}
	}

	TradeSetGold( 0 );
	TradeSetState( TRADE_STEP_ITEM );
}

void CVTInfo::TradeSetGold( DWORD dwGold )
{
	m_dwTradeGold = dwGold;
}

int CVTInfo::TradeGetGold()
{
	return m_dwTradeGold;
}

void CVTInfo::TradeSetItem( BYTE nId, BYTE i, short nItemNum )
{
	CItemBase* pItemBase = m_pOwner->GetItemId( nId );
	if( pItemBase )
	{
		m_apItem_VT[i] = pItemBase;
		pItemBase->SetExtra( nItemNum );
	}
}

BOOL CVTInfo::TradeClearItem( BYTE i )
{
	CItemBase* pItemBase = m_apItem_VT[i];
	if( IsUsingItem( pItemBase ) )
	{
		pItemBase->SetExtra( 0 );		// clear - using flag 
		m_apItem_VT[i] = NULL;

		return TRUE;
	}
	else
		return FALSE;
}

BOOL CVTInfo::TradeConsent()
{
	CMover* pTrader	= GetOther();
	if( pTrader == NULL )
		return FALSE;

	int cbI	= 0, cbYou	= 0;
	CItemContainer<CItemElem> a( ITYPE_ITEM, MAX_TRADE );
			
	CItemBase* pItemBase;
	for( int i = 0; i < MAX_TRADE; i++ )
	{
		pItemBase = m_apItem_VT[i];
		if( !pItemBase )
			continue;

		m_apItem_VT[i] = NULL;
		CItemElem* pItemElem = ( CItemElem* )pItemBase;
		if( pItemElem->GetProp()->dwPackMax > 1 )
		{
			short nTradeNum = pItemElem->m_nItemNum - (short)pItemBase->GetExtra();
			pItemElem->m_nItemNum = pItemBase->GetExtra();
			a.Add( pItemElem );
			pItemElem->m_nItemNum = nTradeNum;
			pItemElem->SetExtra( 0 );
			if( nTradeNum == 0 )
				m_pOwner->m_Inventory.RemoveAtId( pItemBase->m_dwObjId );	// 제거
		}
		else
		{
			a.Add( pItemElem );
			m_pOwner->m_Inventory.RemoveAtId( pItemBase->m_dwObjId );
		}
	}
		
	for( i = 0; i < MAX_TRADE; i++ )
	{
		pItemBase = pTrader->m_vtInfo.GetItem( i );
		if( pItemBase == NULL )
			continue;

		pTrader->m_vtInfo.SetItem( i, NULL );
		CItemElem* pItemElem = ( CItemElem* )pItemBase;
		if( pItemElem->GetProp()->dwPackMax > 1 )
		{
			short nTradeNum = pItemElem->m_nItemNum - (short)pItemBase->GetExtra();
			pItemElem->m_nItemNum = pItemBase->GetExtra();
			m_pOwner->m_Inventory.Add( pItemElem );
			pItemElem->m_nItemNum = nTradeNum;
			pItemElem->SetExtra( 0 );
			if( nTradeNum == 0 )
				pTrader->m_Inventory.RemoveAtId( pItemBase->m_dwObjId );	// 제거
		}
		else
		{
			m_pOwner->m_Inventory.Add( pItemElem );
			pTrader->m_Inventory.RemoveAtId( pItemBase->m_dwObjId );
		}
	}

	cbI		= a.GetCount();
	for( i = 0; i < cbI; i++ )
	{
		pItemBase	= a.GetAtId( i );
		pTrader->m_Inventory.Add( (CItemElem*)pItemBase );
	}
	
	// step1. 줄돈과 뺄돈을 구해둔다.
	int nThisGold = pTrader->m_vtInfo.TradeGetGold();
	nThisGold -= (int)TradeGetGold();

	int nTraderGold = TradeGetGold();
	nTraderGold -= (int)pTrader->m_vtInfo.TradeGetGold();

	// step2. m_dwTradeGold를 clear
	TradeClear();
	pTrader->m_vtInfo.TradeClear();

	// step3. 돈을 더한다.
	m_pOwner->AddGold( nThisGold, FALSE );						
	pTrader->AddGold( nTraderGold, FALSE );

	return TRUE;
}

BOOL CVTInfo::TradeSetItem2( BYTE nId, BYTE i, short nItemNum )
{
	CItemBase* pItemBase = m_pOwner->GetItemId( nId );
	if( IsUsableItem( pItemBase ) == FALSE || m_apItem_VT[i] != NULL )  
		return FALSE;

	if( m_pOwner->m_Inventory.IsEquip( pItemBase->m_dwObjId ) ) 
		return FALSE;
	
	if( ( (CItemElem*)pItemBase )->IsQuest() )
		return FALSE;

	if( ( (CItemElem*)pItemBase )->IsBinds() )
		return FALSE;

	if( pItemBase->GetProp()->dwItemKind3 == IK3_CLOAK && ( (CItemElem*)pItemBase )->m_idGuild != 0 )
		return FALSE;
	
	if( pItemBase->m_dwItemId == II_RID_RID_BOR_EVEINSHOVER || pItemBase->m_dwItemId == II_RID_RID_BOR_LADOLF )
		return FALSE;
	
	if( nItemNum < 1)
		nItemNum = 1;
	if( nItemNum > ( (CItemElem*)pItemBase )->m_nItemNum )
		nItemNum = ( (CItemElem*)pItemBase )->m_nItemNum;
	
	TradeSetItem( nId, i, nItemNum );
	return TRUE;
}

void TradeLog( CAr & ar, CItemBase* pItemBase, short nItemCount )
{
	ar << nItemCount;
	ar << ( (CItemElem*)pItemBase )->GetAbilityOption();
	ar << int(( (CItemElem*)pItemBase )->m_bItemResist);
	ar << int(( (CItemElem*)pItemBase )->m_nResistAbilityOption);
	ar << int((float)( (CItemElem*)pItemBase )->m_nHitPoint * 100 / (float)( (CItemElem*)pItemBase )->GetProp()->dwEndurance );
#ifdef __PIERCING_SM_TIME
	ar << ( (CItemElem*)pItemBase )->m_bCharged;
	ar << ( (CItemElem*)pItemBase )->m_dwKeepTime;
	ar << ( (CItemElem*)pItemBase )->m_piercingInfo.nPiercedSize;
	ar << ( (CItemElem*)pItemBase )->m_piercingInfo.adwItemId[0];
	ar << ( (CItemElem*)pItemBase )->m_piercingInfo.adwItemId[1];
	ar << ( (CItemElem*)pItemBase )->m_piercingInfo.adwItemId[2];
	ar << ( (CItemElem*)pItemBase )->m_piercingInfo.adwItemId[3];
#else // __PIERCING_SM_TIME
	ar << (BOOL)0;
	ar << (DWORD)0;
	ar << (int)0;
	ar << (DWORD)0;
	ar << (DWORD)0;
	ar << (DWORD)0;
	ar << (DWORD)0;
#endif // __PIERCING_SM_TIME
#ifdef __RANDOMOPTITEM0628
	ar << ( (CItemElem*)pItemBase )->m_nRandomOptItemId;
#else // __RANDOMOPTITEM0628
	ar << (int)0;
#endif // __RANDOMOPTITEM0628
}

// nMinus - 나갈 돈 
// nPlus  - 들어올 돈 
BOOL CheckTradeGold( CMover* pMover, int nMinus, int nPlus )
{
	if( nMinus >= 0 )
	{
		if( pMover->GetGold() >= nMinus )
		{
			int nGold = pMover->GetGold() - nMinus;
			int nResult = nGold + nPlus;
			if( nPlus >= 0 )
			{
				if( nResult >= nGold )	// overflow가 아니면?
					return TRUE;
			}
			else
			{
				if( nResult >= 0 )		// underflow가 아니면?
					return TRUE;
			}

		}
	}

	return FALSE;
}

//로그와 체크를 때어내면 클라와 같다. 통합해서 refactoring하자.
TRADE_CONFIRM_TYPE CVTInfo::TradeLastConfirm( CAr & ar )
{
	TRADE_CONFIRM_TYPE		result = TRADE_CONFIRM_ERROR;
	CMover*					pTrader	= GetOther();

	// gold_step1. GetGold(), AddGold() 함수가 원하는데로 동작되도록 보관 후에 clear해둔다.
	int nTraderGold        = pTrader->m_vtInfo.TradeGetGold();
	int nUserGold          = TradeGetGold();
	
	pTrader->m_vtInfo.TradeSetGold( 0 );
	TradeSetGold( 0 );

	// gold_step2. 줄돈과 뺄돈을 구해둔다.
	int nThisAdd   = nTraderGold - nUserGold;
	int nTraderAdd = nUserGold   - nTraderGold;

	// gold_step3. overflow를 검사 
	if( CheckTradeGold( m_pOwner, nUserGold, nTraderGold ) == FALSE ||
		CheckTradeGold( pTrader, nTraderGold, nUserGold ) == FALSE )
	{
		TradeClear();
		pTrader->m_vtInfo.TradeClear();
		return result;
	}

	// 교환 할 만큼 양쪽의 인벤토리 슬롯이 여유가 있는 지를 검사한다. 
	int nPlayers = 0;
	int nTraders = 0;

	for( int i = 0; i < MAX_TRADE; i++ )
	{
		if( GetItem( i ) )
			nPlayers++;

		if( pTrader->m_vtInfo.GetItem( i ) )
			nTraders++;
	}

	if( ( pTrader->m_Inventory.GetSize() - pTrader->m_Inventory.GetCount() ) < nPlayers )
		result = TRADE_CONFIRM_ERROR;
	else if( ( m_pOwner->m_Inventory.GetSize() - m_pOwner->m_Inventory.GetCount() ) < nTraders )
		result = TRADE_CONFIRM_ERROR;
	else
		result = TRADE_CONFIRM_OK;

	if( result == TRADE_CONFIRM_OK )
	{
		CItemContainer<CItemElem> u( ITYPE_ITEM, MAX_TRADE );

		// gold_step4. 돈을 더한다.
		m_pOwner->AddGold( nThisAdd, FALSE );						
		pTrader->AddGold( nTraderAdd, FALSE );

		ar.WriteString( "T" );
		ar.WriteString( pTrader->GetName() );
		ar.WriteString( m_pOwner->GetName() );
		ar << m_pOwner->GetWorld()->GetID();
//		ar << pTrader->m_vtInfo.TradeGetGold() << TradeGetGold();		// raiders_060116
		ar << nTraderGold << nUserGold;
		ar << pTrader->GetGold() << m_pOwner->GetGold();
		
		u_long uSize1	= 0;
		u_long uOffset1	= ar.GetOffset();
		ar << (DWORD)0;
		u_long uSize2	= 0;
		u_long uOffset2	= ar.GetOffset();
		ar << (DWORD)0;

		// item_step1. m_pOwner->임시 
		CItemBase* pItemBase;
		for( int i = 0; i < MAX_TRADE; i++ )
		{
			pItemBase = m_apItem_VT[i];
			if( pItemBase == NULL )
				continue;

			m_apItem_VT[i] = NULL;
			CItemElem* pItemElem = ( CItemElem* )pItemBase;
			if( pItemElem->GetProp()->dwPackMax > 1 )
			{
				short nTradeNum = pItemElem->m_nItemNum - pItemBase->GetExtra();
				pItemElem->m_nItemNum = pItemBase->GetExtra();
				u.Add( pItemElem );
				pItemElem->m_nItemNum = nTradeNum;
				pItemElem->SetExtra( 0 );
				if( nTradeNum == 0 )
					m_pOwner->m_Inventory.RemoveAtId( pItemBase->m_dwObjId );	// 제거
			}
			else
			{
				u.Add( pItemElem );	// 임시 버퍼에 추가
				m_pOwner->m_Inventory.RemoveAtId( pItemBase->m_dwObjId );	// 제거
			}
		}
		
		// item_step2. pTrader -> m_pOwner
		for( i = 0; i < MAX_TRADE; i++ )
		{
			pItemBase = pTrader->m_vtInfo.GetItem( i );
			if( pItemBase == NULL )
				continue;

			pTrader->m_vtInfo.SetItem( i, NULL );
			uSize1++;
			ar << (DWORD)pItemBase->m_dwSerialNumber;
			ar.WriteString( pItemBase->GetProp()->szName );

			CItemElem* pItemElem = ( CItemElem* )pItemBase;
			if( pItemElem->GetProp()->dwPackMax > 1 )
			{
				int nTradeNum = pItemElem->m_nItemNum - pItemBase->GetExtra();
				pItemElem->m_nItemNum = pItemBase->GetExtra();
				m_pOwner->m_Inventory.Add( pItemElem );
				pItemElem->m_nItemNum = (short)nTradeNum;

				TradeLog( ar, pItemBase, pItemBase->GetExtra() );
				
				pItemElem->SetExtra( 0 );
				if( nTradeNum == 0 )
					pTrader->m_Inventory.RemoveAtId( pItemBase->m_dwObjId );	
			}
			else
			{
				TradeLog( ar, pItemBase, 1 );
				
				m_pOwner->m_Inventory.Add( pItemElem );	// pUser에 pTrader가 준 아이템을 추가
				pTrader->m_Inventory.RemoveAtId( pItemBase->m_dwObjId );	
			}
		}

		// item_step3. 임시 -> pTrader
		nPlayers = u.GetCount();	// 합침을 고려해서 구해둔다.
		for( i = 0; i < nPlayers; i++ )
		{
			pItemBase = u.GetAtId( i );
			pTrader->m_Inventory.Add( (CItemElem*)pItemBase );
			uSize2++;

			ar << (DWORD)pItemBase->m_dwSerialNumber;
			ar.WriteString( pItemBase->GetProp()->szName );
			TradeLog( ar, pItemBase, ((CItemElem*)pItemBase)->m_nItemNum );
		}

//		GETBLOCK( ar, lpBlock, nBlockSize );
		int nBufSize;	
		LPBYTE lpBlock	= ar.GetBuffer( &nBufSize );

		*(UNALIGNED u_long*)( lpBlock + uOffset1 )	= uSize1;
		*(UNALIGNED u_long*)( lpBlock + uOffset2 )	= uSize2;	
	}

	TradeClear();
	pTrader->m_vtInfo.TradeClear();
	return result;
}

TRADE_STATE CVTInfo::TradeGetState()
{
	return m_state;
}

void CVTInfo::TradeSetState( TRADE_STATE state )
{
	m_state = state;
}

///////////////////////////////////////////////////////////////////////////////
// 개인상점 
///////////////////////////////////////////////////////////////////////////////

//void CDPClient::OnUnregisterPVendorItem( OBJID objid, CAr & ar )
BOOL CVTInfo::VendorClearItem( BYTE i )
{
	CItemBase* pItemBase = m_apItem_VT[i];
	if( pItemBase )
	{
		pItemBase->SetExtra( 0 );
		pItemBase->m_nCost = 0;
		m_apItem_VT[i]     = NULL;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// void CDPClient::OnRegisterPVendorItem( OBJID objid, CAr & ar )
void CVTInfo::VendorSetItem( BYTE nId, BYTE i, short nNum, int nCost )
{
	CItemBase* pItemBase = m_pOwner->GetItemId( nId );
	if( pItemBase )
	{
		m_apItem_VT[i] = pItemBase;
		pItemBase->SetExtra( nNum );
		pItemBase->m_nCost = nCost;
	}
}

//void CDPClient::OnPVendorItemNum( OBJID objid, CAr & ar )
// nNum - 남은 갯수 
void CVTInfo::VendorItemNum( BYTE i, short nNum )
{
	CItemBase* pItemBase = m_apItem_VT[i];
	if( pItemBase )
	{
		pItemBase->SetExtra( nNum );
		if( nNum == 0 )
		{
		#ifdef __CLIENT
			if( m_pOwner->IsActiveMover() == FALSE )
				SAFE_DELETE( m_apItem_VT[i] );
		#endif
			m_apItem_VT[i] = NULL;
		}
	}
}

// 데이타 카피를 해서 보관?
// void CDPClient::OnPVendorItem( OBJID objid, CAr & ar )
void CVTInfo::VendorCopyItems( CItemBase** ppItemVd )
{
	memcpy( (void*)m_apItem_VT, ppItemVd, sizeof(m_apItem_VT) );
}


void CVTInfo::VendorClose( BOOL bClearTitle )
{
	for( int i = 0; i < MAX_VENDITEM; i++ )
	{
		if( m_apItem_VT[i] )
		{
			m_apItem_VT[i]->SetExtra( 0 );
			m_apItem_VT[i]->m_nCost	= 0;
		#ifdef __CLIENT
			if( FALSE == m_pOwner->IsActiveMover() )
				SAFE_DELETE( m_apItem_VT[i] );
		#endif	// __CLIENT
			m_apItem_VT[i]	= NULL;
		}
	}

	if( bClearTitle )
		m_strTitle = "";

	SetOther( NULL );	
}


// 나는 판매자 인가? 
BOOL CVTInfo::VendorIsVendor() 
{
	for( int i=0; i<MAX_VENDITEM; ++i )
	{
		if( m_apItem_VT[i] )	// 등록한 아이템이 있는가?
			return TRUE;
	}

	return FALSE;
}

//CDPSrvr::OnBuyPVendorItem
#ifdef __WORLDSERVER
BOOL CVTInfo::VendorSellItem( CMover* pBuyer, BYTE i, DWORD dwItemId, short nNum, VENDOR_SELL_RESULT& result )
{
	result.nRemain    = 0;
	result.nErrorCode = 0;

	if( IsVendorOpen() == FALSE )
		return FALSE;

	CItemBase* pItemBase = m_apItem_VT[i];
	if( IsUsingItem( pItemBase ) == FALSE || pItemBase->m_dwItemId != dwItemId )
		return FALSE;

	if( nNum < 1 )
		nNum = 1;
	if( nNum > pItemBase->GetExtra() )
		nNum = (short)pItemBase->GetExtra();
	if( pItemBase->m_nCost > 0 && (pBuyer->GetGold() < (nNum * pItemBase->m_nCost)) )
		nNum = (short)( pBuyer->GetGold() / pItemBase->m_nCost );
	if( nNum == 0 )
	{
		result.nErrorCode = TID_GAME_LACKMONEY;
		return FALSE;
	}

	CItemElem* pItemElem = (CItemElem*)pItemBase;
	CItemElem itemElem   = *pItemElem;
	itemElem.m_nItemNum	 = nNum;

	if( pBuyer->CreateItem( &itemElem ) == FALSE )
	{
		result.nErrorCode = TID_GAME_LACKSPACE;
		return FALSE;
	}

	pBuyer->AddGold( -( pItemBase->m_nCost * nNum ) );
	m_pOwner->AddGold( pItemBase->m_nCost * nNum );

	pItemBase->SetExtra( pItemBase->GetExtra() - nNum );
	int nRemain = pItemBase->GetExtra();
	if( nRemain <= 0 )
		m_apItem_VT[i] = NULL;
	m_pOwner->RemoveItem( (BYTE)pItemBase->m_dwObjId, nNum );

	result.item = itemElem;
	result.nRemain = nRemain;
	return TRUE;
}
#endif // __WORLDSERVER


