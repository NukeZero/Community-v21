
#include "StdAfx.h"
#include "FLExchangeEvent.h"
#include "FLItemUpgradeManager.h"


FLExchangeEvent::FLExchangeEvent()
{

}

FLExchangeEvent::~FLExchangeEvent()
{

}

FLExchangeEvent*	FLExchangeEvent::GetInstance()
{
	static FLExchangeEvent	kExchangeEvent;

	return & kExchangeEvent;
}

bool	FLExchangeEvent::LoadScript()
{
	return m_kExchange.LoadScript();
}

void	FLExchangeEvent::OnDoExchange( FLWSUser* pUser, const FLPacketItemExchangeReq* pMsg ) const
{
	if( IsValidObj( pUser ) == FALSE || pMsg == NULL || pMsg->dwMenuID < 0 || pMsg->dwMenuID >= MAX_MOVER_MENU)
	{
		return;
	}

	// chipi_130709 character.inc 파일 조작으로 비사용 교환 메뉴 사용 가능 문제 수정
	if( CNpcChecker::GetInstance()->IsCloseNpc( pMsg->dwMenuID, pUser->GetWorld(), pUser->GetPos() ) == FALSE )
		return;

	FLSnapshotItemExchangeAck toClient;
	toClient.dwMenuID	= pMsg->dwMenuID;
	toClient.dwListNth	= pMsg->dwListNth;
	toClient.bSuccess	= false;

	if( CanExchange( pUser, pMsg ) == false )
	{
		pUser->AddPacket( &toClient );
		return;
	}
	
	const bool bResult	= DoExchange( pUser, pMsg );

	toClient.bSuccess	= bResult;
	pUser->AddPacket( &toClient );
}

bool	FLExchangeEvent::CanExchange( FLWSUser* pUser, const FLPacketItemExchangeReq* pMsg ) const
{
	if( IsValidStateUser( pUser ) == false )
	{
		return false;
	}

	if( IsValidStateMsg( pMsg ) == false )
	{
		return false;
	}
	
	const T_EXCHANGE_DATA* pSetList					= m_kExchange.Find( pMsg->dwMenuID );

	ExchangeItemListCItr kCItr						= pSetList->mapExchangeItemList.find( pMsg->dwListNth );

	const T_EXCHANGE_ITEM_LIST kExchangeItemList	= kCItr->second;

	if( kExchangeItemList.byExchangeType <= EXCHANGE_TYPE_NONE || kExchangeItemList.byExchangeType >= EXCHANGE_TYPE_MAX )
	{
		return false;
	}

	if( kExchangeItemList.byExchangeType != EXCHANGE_TYPE_NORMAL_ITEM && pMsg->nExchangeCount > MAX_EXCHANGE_ENCHANT_MOVE )
	{
		return false;
	}

	if( CheckConditionItem( pUser, kExchangeItemList.mapConditionItem, kExchangeItemList.byExchangeType, pMsg->nExchangeCount ) == false )
	{
		// 조건 아이템이 충분치 않다.
		pUser->AddDefinedText( TID_MMI_EXCHAGESCRIPTSMELT_TEXT01 );
		return false;
	}

	if( CheckPaymentItem( pUser, kExchangeItemList.mapPaymentItem, pMsg->nExchangeCount ) == false )
	{
		// 지급 아이템을 넣을 공간이 충분치 않다.
		pUser->AddDefinedText( TID_GAME_LACKSPACE );
		return false;
	}

	return true;
}

bool	FLExchangeEvent::DoExchange( FLWSUser* pUser, const FLPacketItemExchangeReq* pMsg ) const
{
	if( IsValidStateUser( pUser ) == false )
	{
		return false;
	}

	if( IsValidStateMsg( pMsg ) == false )
	{
		return false;
	}

	const T_EXCHANGE_DATA* pSetList		= m_kExchange.Find( pMsg->dwMenuID );
	if( pSetList == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "pSetList is NULL. dwMenuID [%d]" ), pMsg->dwMenuID );
		return false;
	}

	ExchangeItemListCItr kCItr			= pSetList->mapExchangeItemList.find( pMsg->dwListNth );
	if( kCItr == pSetList->mapExchangeItemList.end() )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "kCItr is NULL. dwMenuID [%d] dwListNth [%d]" ), pMsg->dwMenuID, pMsg->dwListNth );
		return false;
	}

	const T_EXCHANGE_ITEM_LIST kExchangeItemList = kCItr->second;
	if( kExchangeItemList.byExchangeType <= EXCHANGE_TYPE_NONE || kExchangeItemList.byExchangeType >= EXCHANGE_TYPE_MAX )
	{
		return false;
	}

	FLItemElem kRemoveItemElem;
	if( RemoveExchangeItem( pUser, kExchangeItemList.mapConditionItem, kExchangeItemList.byExchangeType, pMsg->nExchangeCount, kRemoveItemElem ) == false )
	{
		return false;
	}

	ExchangeItemMap mapPaymentItem;
	mapPaymentItem.clear();

	if( GetPaymentItemList( kExchangeItemList.mapPaymentItem, kExchangeItemList.dwPaymentLimitCount, pMsg->nExchangeCount, mapPaymentItem ) == false )
	{
		return false;
	}

	PayExchangeItem( pUser, mapPaymentItem, kExchangeItemList.byExchangeType, kRemoveItemElem );


	//////////////////////////////////////////////////////////////////////////

//	BYTE byAttributeKind = SAI79::NO_PROP;

// 	if( kExchangeItemList.byExchangeType == EXCHANGE_TYPE_NORMAL_ITEM || kExchangeItemList.byExchangeType == EXCHANGE_TYPE_ENCHANT_ITEM )
// 	{
// 		if( RemoveExchangeItem( pUser, kExchangeItemList.vecConditionItem, kExchangeItemList.byExchangeType, pMsg->nExchangeCount, &byAttributeKind ) == false )
// 		{
// 			return false;
// 		}
// 
// 		ExchangeItemVec vecPaymentItem;
// 		vecPaymentItem.clear();
// 
// 		if( GetPaymentItemList( kExchangeItemList.vecPaymentItem, kExchangeItemList.dwPaymentLimitCount, vecPaymentItem ) == false )
// 		{
// 			return false;
// 		}
// 
// 		PayExchangeItem( pUser, vecPaymentItem, kExchangeItemList.byExchangeType, byAttributeKind, pMsg->nExchangeCount );
// 	}
// 	else if( kExchangeItemList.byExchangeType == EXCHANGE_TYPE_ENCHANT_MOVE )
// 	{
//  		ExchangeItemCItr	kConditionItemItr	= kExchangeItemList.vecConditionItem.begin();
//  
//  		if( kConditionItemItr == kExchangeItemList.vecConditionItem.end() )
//  		{
//  			FLERROR_LOG( PROGRAM_NAME, _T( "kConditionItemItr is NULL. dwMenuID [%d] dwListNth [%d]" ), pMsg->dwMenuID, pMsg->dwListNth );
//  			return false;
//  		}
//  
// 		const T_EXCHANGE_ITEM tConditionItem	= *kConditionItemItr;
// 		PT_ITEM_SPEC pConditionItemSpec	= g_xSpecManager->GetSpecItem( tConditionItem.dwItemID );
// 
// 		if( pConditionItemSpec == NULL )
// 		{
// 			FLERROR_LOG( PROGRAM_NAME, _T( "pConditionItemSpec is NULL. dwMenuID [%d] dwListNth [%d] dwItemID [%d]" )
// 				, pMsg->dwMenuID, pMsg->dwListNth, tConditionItem.dwItemID );
// 			return false;
// 		}
// 
// 		ExchangeItemCItr	kPaymentItemItr		= kExchangeItemList.vecPaymentItem.begin();
// 
// 		if( kPaymentItemItr == kExchangeItemList.vecPaymentItem.end() )
// 		{
// 			FLERROR_LOG( PROGRAM_NAME, _T( "kPaymentItemItr is NULL. dwMenuID [%d] dwListNth [%d]" ), pMsg->dwMenuID, pMsg->dwListNth );
// 			return false;
// 		}
// 
// 		const T_EXCHANGE_ITEM tPaymentItem	= *kPaymentItemItr;
// 		PT_ITEM_SPEC pPaymentItemSpec	= g_xSpecManager->GetSpecItem( tPaymentItem.dwItemID );
// 
// 		if( pPaymentItemSpec == NULL )
// 		{
// 			FLERROR_LOG( PROGRAM_NAME, _T( "pPaymentItemSpec is NULL. dwMenuID [%d] dwListNth [%d] dwItemID [%d]" )
// 				, pMsg->dwMenuID, pMsg->dwListNth, tPaymentItem.dwItemID );
// 			return false;
// 		}
// 
// 		DWORD	dwRemoveItemObjID = GetRemoveItemObjID( pUser, tConditionItem, kExchangeItemList.byExchangeType );
// 
// 		FLItemElem* pRemoveItemElem		= NULL;
// 		pRemoveItemElem = pUser->m_Inventory.GetAtId( dwRemoveItemObjID );
// 
// 		if( pRemoveItemElem == NULL )
// 		{
// 			FLERROR_LOG( PROGRAM_NAME, _T( "[ REMOVE ITEM IS NULL. ItemID(%d), Quantity(%d) ]" ), tConditionItem.dwItemID, tConditionItem.nItemQuantity );
// 			return false;
// 		}
// 
// 		FLItemElem kItemElem;
// 		kItemElem.m_dwItemId		= tPaymentItem.dwItemID;
// 		kItemElem.m_nItemNum		= tPaymentItem.nItemQuantity;
// 		kItemElem.m_byFlag			= tPaymentItem.byItemFlag;
// 		kItemElem.m_bCharged		= kItemElem.GetProp()->bCharged;
// 
// 		FLRandomOption	kRandomOption;
// 		pRemoveItemElem->GetRandomOption( kRandomOption );
// 		kItemElem.CopyRandomOption( kRandomOption );
// 
// 		kItemElem.SetSerialNumber();
// 
// 		if( pUser->CreateItem( &kItemElem ) == TRUE )
// 		{
// 			g_DPSrvr.PutItemLog( pUser, "=", "EXCHANGE_REMOVE", pRemoveItemElem, tConditionItem.nItemQuantity );
// 			pUser->RemoveItem( pRemoveItemElem->m_dwObjId, tConditionItem.nItemQuantity );
// 
// 			g_DPSrvr.PutItemLog( pUser, "=", "EXCHANGE_CREATE", &kItemElem );
// 
// 			pUser->AddDefinedText( TID_EXCHANGE_SUCCESS, "\"%s\" %d", pPaymentItemSpec->szName, kItemElem.m_nItemNum );
// 		}
// 		else
// 		{
// 			g_DPSrvr.PutItemLog( pUser, "=", "EXCHANGE_CREATE_FAIL", &kItemElem );
// 		}
// 	}
// 	else
// 	{
// 		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID EXCHANGE TYPE. MENU_ID(%u), LIST_NTH(%u), TYPE(%d) ]" )
// 			, pMsg->dwMenuID, pMsg->dwListNth, kExchangeItemList.byExchangeType );
// 		FLASSERT( 0 );
// 	}

	return true;
}

bool	FLExchangeEvent::IsValidStateUser( FLWSUser* pUser ) const
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return false;
	}

	if( pUser->IsDie() == TRUE || pUser->m_bAllAction == FALSE )
	{
		return false;
	}

	if( pUser->GetWorld() == NULL )
	{
		return false;
	}

	if( pUser->m_vtInfo.GetOther() != NULL || pUser->m_vtInfo.VendorIsVendor() != FALSE )
	{
		return false;
	}

	return true;
}

bool	FLExchangeEvent::IsValidStateMsg( const FLPacketItemExchangeReq* pMsg ) const
{
	if( pMsg == NULL )
	{
		return false;
	}

	if( pMsg->nExchangeCount <= 0 )
	{
		return false;
	}

	const T_EXCHANGE_DATA* pSetList	= m_kExchange.Find( pMsg->dwMenuID );
	if( pSetList == NULL )
	{
		return false;
	}

	ExchangeItemListCItr kCItr = pSetList->mapExchangeItemList.find( pMsg->dwListNth );
	if( kCItr == pSetList->mapExchangeItemList.end() )
	{
		return false;
	}

	return true;
}

bool	FLExchangeEvent::CheckConditionItem( FLWSUser* pUser, const ExchangeItemMap & mapExchangeItem, const BYTE byExchangeType, const int nExchangeCount ) const
{
	if( mapExchangeItem.empty() == true )
	{
		return false;
	}

	for( ExchangeItemCItr pos = mapExchangeItem.begin(); pos != mapExchangeItem.end(); ++pos )
	{
		const T_EXCHANGE_ITEM kExchangeItem		= pos->second;

		if( IsHasConditionItem( pUser, kExchangeItem, byExchangeType, nExchangeCount ) == false )
		{
			return false;
		}
	}

	return true;
}

bool	FLExchangeEvent::CheckPaymentItem( FLWSUser* pUser, const ExchangeItemMap & mapExchangeItem, const int nExchangeCount ) const
{
	if( mapExchangeItem.empty() == true )
	{
		return false;
	}

	const size_t nSize	= mapExchangeItem.size();

	std::vector< T_ADDITEM_INFO > vecAddItemInfo;
	vecAddItemInfo.reserve( nSize );
	vecAddItemInfo.clear();

	for( ExchangeItemCItr pos = mapExchangeItem.begin(); pos != mapExchangeItem.end(); ++pos )
	{
		const T_EXCHANGE_ITEM kExchangeItem		= pos->second;

		T_ADDITEM_INFO kAddItemInfo;

		kAddItemInfo.dwItemId	= kExchangeItem.dwItemID;
		kAddItemInfo.nItemNum	= kExchangeItem.nItemQuantity * nExchangeCount;

		vecAddItemInfo.push_back( kAddItemInfo );
	}

	if( vecAddItemInfo.empty() == true || pUser->m_Inventory.IsEnoughEmptySpace( vecAddItemInfo ) == false )
	{
		return false;
	}

	return true;
}

bool	FLExchangeEvent::RemoveExchangeItem( FLWSUser* pUser, const ExchangeItemMap & mapExchangeItem, const BYTE byExchangeType, const int nExchangeCount, FLItemElem & kRemoveItemElem ) const
{
	if( mapExchangeItem.empty() == true )
	{
		return false;
	}

	for( ExchangeItemCItr pos = mapExchangeItem.begin(); pos != mapExchangeItem.end(); ++pos )
	{
		const T_EXCHANGE_ITEM kExchangeItem		= pos->second;

		PT_ITEM_SPEC pItemSpec					= g_xSpecManager->GetSpecItem( kExchangeItem.dwItemID );
		if( pItemSpec == NULL )
		{
			return false;
		}

		int nRemainNum			= kExchangeItem.nItemQuantity * nExchangeCount;

		DWORD dwRemoveItemObjID = GetRemoveItemObjID( pUser, kExchangeItem, byExchangeType );

		while( dwRemoveItemObjID != NULL_ID )
		{
			FLItemElem* pRemoveItemElem = pUser->m_Inventory.GetAtId( dwRemoveItemObjID );
			if( pRemoveItemElem == NULL )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ REMOVE ITEM IS NULL. ItemID(%d), Quantity(%d) ]" )
					, kExchangeItem.dwItemID, kExchangeItem.nItemQuantity );
				return false;
			}

			if( byExchangeType == EXCHANGE_TYPE_ENCHANT_ITEM )
			{
				const BYTE byItemResist		= pRemoveItemElem->GetItemResist();
				kRemoveItemElem.SetItemResist( byItemResist );
			}
			else if( byExchangeType == EXCHANGE_TYPE_ENCHANT_MOVE )
			{
				FLRandomOption	kRandomOption;
				pRemoveItemElem->GetRandomOption( kRandomOption );
				kRemoveItemElem.CopyRandomOption( kRandomOption );
			}

			const int nRemoveCount = pRemoveItemElem->m_nItemNum < nRemainNum ? pRemoveItemElem->m_nItemNum : nRemainNum;

			nRemainNum -= nRemoveCount;

			g_DPSrvr.PutItemLog( pUser, "=", "EXCHANGE_REMOVE", pRemoveItemElem, nRemoveCount );
			pUser->RemoveItem( dwRemoveItemObjID, nRemoveCount );

			if( nRemainNum <= 0 )
			{
				break;
			}

			dwRemoveItemObjID = GetRemoveItemObjID( pUser, kExchangeItem, byExchangeType );
		}
	}		

	return true;
}

void	FLExchangeEvent::PayExchangeItem( FLWSUser* pUser, const ExchangeItemMap & mapExchangeItem, const BYTE byExchangeType, const FLItemElem & kRemoveItemElem ) const
{
	for( ExchangeItemCItr pos = mapExchangeItem.begin(); pos != mapExchangeItem.end(); ++pos )
	{
		const T_EXCHANGE_ITEM kExchangeItem		= pos->second;

		const T_ITEM_SPEC* pItemSpec			= g_xSpecManager->GetSpecItem( kExchangeItem.dwItemID );
		if( pItemSpec == NULL )
		{
			return;
		}

		int nTotalPayQuantity		= kExchangeItem.nItemQuantity;
		for(  ; nTotalPayQuantity > 0;  )
		{
			const int nPayQuantity	= ( nTotalPayQuantity > static_cast<int>( pItemSpec->dwPackMax ) ) ? static_cast<int>( pItemSpec->dwPackMax ) : nTotalPayQuantity;

			FLItemElem kItemElem;
			kItemElem.m_dwItemId		= kExchangeItem.dwItemID;
			kItemElem.m_nItemNum		= nPayQuantity;
			kItemElem.m_byFlag			= kExchangeItem.byItemFlag;
			kItemElem.m_bCharged		= pItemSpec->bCharged;
			kItemElem.SetSerialNumber();

			if( byExchangeType == EXCHANGE_TYPE_ENCHANT_ITEM )
			{
				if( pItemSpec->IsUpgradeAble( IUTYPE_ENCHANT_GENERAL ) == TRUE )
				{
					const DWORD dwRandomGeneralEnchantLevel		= ::xRandom( kExchangeItem.dwMinGeneralEnchant, kExchangeItem.dwMaxGeneralEnchant );
					const DWORD dwMaxGeneralEnchantLevel		= g_pItemUpgradeManager->GetMaxSizeGeneralEnchant( kExchangeItem.dwItemID );

					const DWORD dwSetEnchantLevel				= ( dwRandomGeneralEnchantLevel > dwMaxGeneralEnchantLevel )
						? dwMaxGeneralEnchantLevel : dwRandomGeneralEnchantLevel;

					kItemElem.SetAbilityOption( dwSetEnchantLevel );
				}

				if( pItemSpec->IsUpgradeAble( IUTYPE_ENCHANT_ATTRIBUTE ) == TRUE )
				{
					const DWORD dwRandomAttributeEnchantLevel	= ::xRandom( kExchangeItem.dwMinAttributeEnchant, kExchangeItem.dwMaxAttributeEnchant );
					const DWORD dwMaxAttributeEnchantLevel		= g_pItemUpgradeManager->GetMaxSizeAttributeEnchant( kExchangeItem.dwItemID );

					const DWORD dwSetEnchantLevel				= ( dwRandomAttributeEnchantLevel > dwMaxAttributeEnchantLevel )
						? dwMaxAttributeEnchantLevel : dwRandomAttributeEnchantLevel;

					kItemElem.SetResistAbilityOption( dwSetEnchantLevel );

					const BYTE byAttributeKind		= kRemoveItemElem.GetItemResist();

					if( kExchangeItem.bCheckScriptAttribute == false && byAttributeKind > SAI79::NO_PROP && byAttributeKind < SAI79::END_PROP )
					{
						kItemElem.SetItemResist( byAttributeKind );
					}
					else
					{
						kItemElem.SetItemResist( kExchangeItem.byAttributeKind );
					}
				}
			}
			else if( byExchangeType == EXCHANGE_TYPE_ENCHANT_MOVE )
			{
				FLRandomOption	kRandomOption;
				kRemoveItemElem.GetRandomOption( kRandomOption );
				kItemElem.CopyRandomOption( kRandomOption );
			}

			if( pUser->CreateItem( &kItemElem ) == TRUE )
			{
				g_DPSrvr.PutItemLog( pUser, "=", "EXCHANGE_CREATE", &kItemElem );
			}
			else
			{
				g_DPSrvr.PutItemLog( pUser, "=", "EXCHANGE_CREATE_FAIL", &kItemElem );
			}

			nTotalPayQuantity	-= nPayQuantity;
		}

		pUser->AddDefinedText( TID_EXCHANGE_SUCCESS, "\"%s\" %d", pItemSpec->szName, ( kExchangeItem.nItemQuantity ) );
	}
}

bool	FLExchangeEvent::IsHasConditionItem( FLWSUser* pUser, const T_EXCHANGE_ITEM & kExchangeItem, const BYTE byExchangeType, const int nExchangeCount ) const
{
	const T_ITEM_SPEC* pItemSpec	= g_xSpecManager->GetSpecItem( kExchangeItem.dwItemID );
	if( pItemSpec == NULL )
	{
		return false;
	}

	DWORD	dwStart			= 0;
	DWORD	dwEnd			= 0;
	if( pUser->m_Inventory.GetObjIndexRange( pItemSpec, &dwStart, &dwEnd ) == false )
	{
		return false;
	}

	int		nItemQuantity	= kExchangeItem.nItemQuantity * nExchangeCount;

	for( DWORD dwObjIndex = dwStart; dwObjIndex < dwEnd; ++dwObjIndex )
	{
		FLItemElem* pItemElem = pUser->m_Inventory.GetAt( dwObjIndex );
		if( IsUsableItem( pItemElem ) == FALSE || pItemElem->m_dwItemId != kExchangeItem.dwItemID || pUser->m_Inventory.IsEquip( pItemElem->m_dwObjId ) == TRUE )
		{
			continue;
		}

		if( byExchangeType == EXCHANGE_TYPE_ENCHANT_ITEM )
		{
			if( kExchangeItem.dwMaxGeneralEnchant > 0 )
			{
				const DWORD dwAbilityOption				= pItemElem->GetAbilityOption();
				if( dwAbilityOption < kExchangeItem.dwMinGeneralEnchant || dwAbilityOption > kExchangeItem.dwMaxGeneralEnchant )
				{
					continue;
				}
			}

			if( kExchangeItem.dwMaxAttributeEnchant > 0 )
			{
				const DWORD dwResistAbilityOption		= pItemElem->GetResistAbilityOption();
				const BYTE	byResist					= pItemElem->GetItemResist();
				if( dwResistAbilityOption < kExchangeItem.dwMinAttributeEnchant || dwResistAbilityOption > kExchangeItem.dwMaxAttributeEnchant )
				{
					continue;
				}
				else if( kExchangeItem.bCheckScriptAttribute == true && byResist != kExchangeItem.byAttributeKind )
				{
					continue;
				}
			}
		}

		nItemQuantity -= pItemElem->m_nItemNum;

		if( nItemQuantity <= 0 )
		{
			return true;
		}
	}

	return false;
}

DWORD	FLExchangeEvent::GetRemoveItemObjID( FLWSUser* pUser, const T_EXCHANGE_ITEM & kExchangeItem, const BYTE byExchangeType ) const
{
	const T_ITEM_SPEC* pItemSpec	= g_xSpecManager->GetSpecItem( kExchangeItem.dwItemID );
	if( pItemSpec == NULL )
	{
		return NULL_ID;
	}

	DWORD dwStart	= 0;
	DWORD dwEnd		= 0;
	if( pUser->m_Inventory.GetObjIndexRange( pItemSpec, &dwStart, &dwEnd ) == false )
	{
		return NULL_ID;
	}

	DWORD dwRemoveItemObjID			= NULL_ID;

	for( DWORD dwObjIndex = dwStart; dwObjIndex < dwEnd; ++dwObjIndex )
	{
		FLItemElem* pItemElem = pUser->m_Inventory.GetAt( dwObjIndex );
		if( IsUsableItem( pItemElem ) == FALSE || pItemElem->m_dwItemId != kExchangeItem.dwItemID || pUser->m_Inventory.IsEquip( pItemElem->m_dwObjId ) == TRUE )
		{
			continue;
		}

		if( byExchangeType == EXCHANGE_TYPE_NORMAL_ITEM || byExchangeType == EXCHANGE_TYPE_ENCHANT_MOVE )
		{
			return pItemElem->m_dwObjId;
		}
		else if( byExchangeType == EXCHANGE_TYPE_ENCHANT_ITEM )
		{
			volatile bool bFind = false;

			if( kExchangeItem.dwMaxGeneralEnchant > 0 && kExchangeItem.dwMaxAttributeEnchant > 0 )
			{
				if( static_cast<DWORD>( pItemElem->GetAbilityOption() ) >= kExchangeItem.dwMinGeneralEnchant
					&& static_cast<DWORD>( pItemElem->GetAbilityOption() ) <= kExchangeItem.dwMaxGeneralEnchant
					&& static_cast<DWORD>( pItemElem->GetResistAbilityOption() ) >= kExchangeItem.dwMinAttributeEnchant
					&& static_cast<DWORD>( pItemElem->GetResistAbilityOption() ) <= kExchangeItem.dwMaxAttributeEnchant
					)
				{
					if( kExchangeItem.bCheckScriptAttribute == true )
					{
						if( pItemElem->GetItemResist() == kExchangeItem.byAttributeKind )
						{
							bFind = true;
						}
					}
					else
					{
						bFind = true;
					}
				}
			}
			else if( kExchangeItem.dwMaxGeneralEnchant > 0 && kExchangeItem.dwMaxAttributeEnchant == 0 )
			{
				if( static_cast<DWORD>( pItemElem->GetAbilityOption() ) >= kExchangeItem.dwMinGeneralEnchant
					&& static_cast<DWORD>( pItemElem->GetAbilityOption() ) <= kExchangeItem.dwMaxGeneralEnchant
					)
				{
					bFind = true;
				}
			}
			else if( kExchangeItem.dwMaxGeneralEnchant == 0 && kExchangeItem.dwMaxAttributeEnchant > 0 )
			{
				if( static_cast<DWORD>( pItemElem->GetResistAbilityOption() ) >= kExchangeItem.dwMinAttributeEnchant
					&& static_cast<DWORD>( pItemElem->GetResistAbilityOption() ) <= kExchangeItem.dwMaxAttributeEnchant
					)
				{
					if( kExchangeItem.bCheckScriptAttribute == true )
					{
						if( pItemElem->GetItemResist() == kExchangeItem.byAttributeKind )
						{
							bFind = true;
						}
					}
					else
					{
						bFind = true;
					}
				}
			}
			else
			{
				bFind = true;
			}

			if( dwRemoveItemObjID == NULL_ID && bFind == true )
			{
				dwRemoveItemObjID = pItemElem->m_dwObjId;
			}
			else if( dwRemoveItemObjID != NULL_ID && bFind == true )
			{
				FLItemElem* pPrevItemElem = pUser->m_Inventory.GetAtId( dwRemoveItemObjID );
				if( pItemElem->GetAbilityOption() < pPrevItemElem->GetAbilityOption() )
				{
					dwRemoveItemObjID = pItemElem->m_dwObjId;
				}
				else if( pItemElem->GetAbilityOption() == pPrevItemElem->GetAbilityOption()
					&& pItemElem->GetResistAbilityOption() < pPrevItemElem->GetResistAbilityOption() )
				{
					dwRemoveItemObjID = pItemElem->m_dwObjId;
				}
			}
		}
	}

	return dwRemoveItemObjID;
}

bool	FLExchangeEvent::GetPaymentItemList( const ExchangeItemMap & mapExchangeItem, const DWORD dwPaymentLimitCount, const int nExchangeCount, ExchangeItemMap & mapPaymentItem ) const
{
	mapPaymentItem.clear();

	for( int Nth = 0; Nth < nExchangeCount; ++Nth )
	{
		const DWORD dwRandomProb	= ::xRandom( MAX_PAYMENT_PROB );
		DWORD dwCount				= 0;

		for( ExchangeItemCItr pos = mapExchangeItem.begin(); pos != mapExchangeItem.end(); ++pos )
		{
			const T_EXCHANGE_ITEM kExchangeItem		= pos->second;

			if( dwRandomProb < kExchangeItem.dwPaymentProb )
			{
				ExchangeItemItr itr = mapPaymentItem.find( kExchangeItem.dwItemID );
				if( itr != mapPaymentItem.end() )
				{
					itr->second.nItemQuantity	+= kExchangeItem.nItemQuantity;
				}
				else
				{
					mapPaymentItem.insert( ExchangeItemMap::value_type( kExchangeItem.dwItemID, kExchangeItem ) );
				}

				++dwCount;
			}

			if( dwCount >= dwPaymentLimitCount )
			{
				break;
			}
		}
	}

	if( mapPaymentItem.empty() == true )
	{
		return false;
	}

	return true;
}
