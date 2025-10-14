
#include "StdAfx.h"
#include "FLTreasureChest.h"


FLTreasureChest::FLTreasureChest()
{

}

FLTreasureChest::~FLTreasureChest()
{

}

FLTreasureChest*	FLTreasureChest::GetInstance()
{
	static FLTreasureChest	sTreasureChest;

	return & sTreasureChest;
}

void	FLTreasureChest::OnDoUseTreasureChest( FLWSUser* pUser, const DWORD dwChestItemObjID ) const
{
	FLSnapshotTreasureChestOpenNoti kPacket;

	if( CanUseTreasureChestItem( pUser, dwChestItemObjID ) == true )
	{
		kPacket.dwChestItemObjID	= dwChestItemObjID;
	}
	else
	{
		kPacket.dwChestItemObjID	= NULL_ID;
	}

	pUser->AddPacket( &kPacket );
}

bool	FLTreasureChest::OnDoOpenTreasureChest( FLWSUser* pUser, const FLPacketTreasureChestOpenByKeyReq* pMsg ) const
{
	if( CanOpenTreasureChestByKey( pUser, pMsg ) == false )
	{
		return false;
	}

	return DoOpenTreasureChest( pUser, pMsg );
}

bool	FLTreasureChest::DoOpenTreasureChest( FLWSUser* pUser, const FLPacketTreasureChestOpenByKeyReq* pMsg ) const
{
	if( pMsg == NULL )
	{
		RELEASE_BREAK;
		return false;
	}

	FLItemElem* pKeyItem		= pUser->m_Inventory.GetAtId( pMsg->dwKeyItemObjID );
	if( IsUsableItem( pKeyItem ) == FALSE )
	{
		return false;
	}

//	T_TREASURE_ITEM kTreasureItem[ MAX_TREASURE_ITEM_COUNT ] = { 0 };
	
	TreasureItemVec vecTreasureItem;

	if( GetTreasureItem( pKeyItem->m_dwItemId, vecTreasureItem ) == false )
	{
		return false;
	}

	if( CreateTreasureItemToInventory( pUser, vecTreasureItem ) == false )
	{
		return false;
	}

	RemoveTreasureChestAndKey( pUser, pMsg->dwChestItemObjID, pMsg->dwKeyItemObjID );

	return true;
}

bool	FLTreasureChest::GetTreasureItem( const DWORD dwKeyItemID, TreasureItemVec & vecTreasureItem ) const
{
	SpecTreasureKeyVec vecTreasureKey;

	if( g_xSpecManager->GetSpecTreasureKey( dwKeyItemID, &vecTreasureKey ) == false )
	{
		return false;
	}

	const DWORD dwMaxLoopCount		= vecTreasureKey.begin()->dwLoopCount;
	if( dwMaxLoopCount > MAX_TREASURE_ITEM_COUNT )
	{
		return false;
	}

	vecTreasureItem.clear();
	vecTreasureItem.reserve( dwMaxLoopCount );
	DWORD dwGetCount	= 0;
	DWORD dwLoopCount	= 0;

	do 
	{
		const DWORD dwRandomProb	= xRandom( MAX_TREASURE_KEY_PROB );

		for( SpecTreasureKeyVec::iterator pos = vecTreasureKey.begin(); pos != vecTreasureKey.end(); ++pos )
		{
			const T_TREASURE_KEY kTreasureKey	= *pos;
			if( dwRandomProb >= kTreasureKey.dwProb )
			{
				continue;
			}

			if( static_cast<int>( kTreasureKey.dwLimitCount ) <= 0 )
			{
				break;
			}

			T_TREASURE_ITEM kTreasureItem;
			kTreasureItem.dwItemID		= kTreasureKey.dwTreasureItemID;
			kTreasureItem.dwQuantity	= xRandom( kTreasureKey.dwMaxQuantity ) + 1;
			kTreasureItem.bNotice		= kTreasureKey.bNotice;
			vecTreasureItem.push_back( kTreasureItem );

			--( pos->dwLimitCount );
			
			++dwGetCount;

			break;
		}
		
		// 루프는 최대 2배 까지만 돌자..
		++dwLoopCount;

	} while ( dwGetCount < dwMaxLoopCount && dwLoopCount < ( dwMaxLoopCount * 2 ) );

	if( vecTreasureItem.empty() == true )
	{
		return false;
	}

	return true;
}

bool	FLTreasureChest::CreateTreasureItemToInventory( FLWSUser* pUser, const TreasureItemVec & vecTreasureItem ) const
{
	if( vecTreasureItem.empty() == true || vecTreasureItem.size() > MAX_TREASURE_ITEM_COUNT )
	{
		return false;
	}

	FLSnapshotTreasureChestOpenByKeyAck kPacket;
	DWORD dwCount = 0;

	for( TreasureItemVec::const_iterator pos = vecTreasureItem.begin(); pos != vecTreasureItem.end(); ++pos )
	{
		const T_TREASURE_ITEM kTreasureItem		= *pos;
		const PT_ITEM_SPEC pItemSpec			= g_xSpecManager->GetSpecItem( kTreasureItem.dwItemID );

		const int nQuantity						= ( kTreasureItem.dwQuantity <= pItemSpec->dwPackMax )
												? static_cast<int>( kTreasureItem.dwQuantity ) : static_cast<int>( pItemSpec->dwPackMax );
		FLItemElem kItemElem;
		kItemElem.m_dwItemId		= kTreasureItem.dwItemID;
		kItemElem.m_nItemNum		= nQuantity;

		kItemElem.SetSerialNumber();

		if( pUser->CreateItem( &kItemElem ) == FALSE )
		{
			FLASSERT( 0 );
			FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED CREATE TO INVENTORY. PLAYER_ID(%07d), ITEM_ID(%u), ITEM_NUM(%d) ]" )
				, pUser->m_idPlayer, kItemElem.m_dwItemId, kItemElem.m_nItemNum );
			continue;
		}

		g_DPSrvr.SendPutItemLog( pUser, "C", pUser->m_szName, "TREASURE_ITEM", &kItemElem, kItemElem.m_nItemNum );
		pUser->AddDefinedText( TID_MMI_TREASURE_TREASUREITEM, "\"%s\"", pItemSpec->szName );

		kPacket.arrTreasureItem[ dwCount ].dwItemID		= kTreasureItem.dwItemID;
		kPacket.arrTreasureItem[ dwCount ].dwQuantity	= kTreasureItem.dwQuantity;
		kPacket.arrTreasureItem[ dwCount ].bNotice		= kTreasureItem.bNotice;

		++dwCount;

		if( kTreasureItem.bNotice == true )
		{
			FLPacketBroadCastMessageNoti kPacket;
			kPacket.byBroadCastType		= BROADCAST_TYPE_NOTICE;
			FLSPrintf( kPacket.szMessage, _countof( kPacket.szMessage ), prj.GetText( TID_MMI_TREASURE_NOTICETREASUREITEM ), pUser->m_szName, pItemSpec->szName );
			g_DPCoreClient.SendPacket( &kPacket );
		}
	}

	pUser->AddPacket( &kPacket );

	return true;
}

void	FLTreasureChest::RemoveTreasureChestAndKey( FLWSUser* pUser, const DWORD dwChestItemObjID, const DWORD dwKeyItemObjID ) const
{
	FLItemElem* pItemElem	= NULL;

	pItemElem	= pUser->m_Inventory.GetAtId( dwChestItemObjID );
	g_DPSrvr.SendPutItemLog( pUser, "R", pUser->GetName(), "TREASURE_CHEST", pItemElem, 1 );
	pUser->RemoveItem( dwChestItemObjID, 1 );

	pItemElem	= pUser->m_Inventory.GetAtId( dwKeyItemObjID );
	g_DPSrvr.SendPutItemLog( pUser, "R", pUser->GetName(), "TREASURE_KEY", pItemElem, 1 );
	pUser->RemoveItem( dwKeyItemObjID, 1 );
}

bool	FLTreasureChest::CanUseTreasureChestItem( FLWSUser* pUser, const DWORD dwChestItemObjID ) const
{
	if( IsValidStateUser( pUser ) == false )
	{
		return false;
	}

	FLItemElem* pItemElem	= pUser->m_Inventory.GetAtId( dwChestItemObjID );
	if( IsValidStateChestItem( pUser, pItemElem ) == false )
	{
		pUser->AddDefinedText( TID_SBEVE_NOTUSEITEM );
		return false;
	}

	return true;
}

bool	FLTreasureChest::CanOpenTreasureChestByKey( FLWSUser* pUser, const FLPacketTreasureChestOpenByKeyReq* pMsg ) const
{
	if( pMsg == NULL )
	{
		RELEASE_BREAK;
		return false;
	}

	if( IsValidStateUser( pUser ) == false )
	{
		return false;
	}

	FLItemElem* pChestItem	= pUser->m_Inventory.GetAtId( pMsg->dwChestItemObjID );
	if( IsValidStateChestItem( pUser, pChestItem ) == false )
	{
		pUser->AddDefinedText( TID_SBEVE_NOTUSEITEM );
		return false;
	}

	FLItemElem* pKeyItem		= pUser->m_Inventory.GetAtId( pMsg->dwKeyItemObjID );
	if( IsValidStateKeyItem( pUser, pKeyItem ) == false )
	{
		pUser->AddDefinedText( TID_MMI_TREASURE_ERRORTKEY );
		return false;
	}

	if( IsMatching( pChestItem->m_dwItemId, pKeyItem->m_dwItemId ) == false )
	{
		pUser->AddDefinedText( TID_MMI_TREASURE_ERRORCOMBINE );
		return false;
	}

	if( CanCreateItemToInventory( pUser, pKeyItem->m_dwItemId ) == false )
	{
		pUser->AddDefinedText( TID_GAME_LACKSPACE );
		return false;
	}

	return true;
}

bool	FLTreasureChest::CanCreateItemToInventory( FLWSUser* pUser, const DWORD dwKeyItemID ) const
{
	SpecTreasureKeyVec vecTreasureKey;

	if( g_xSpecManager->GetSpecTreasureKey( dwKeyItemID, &vecTreasureKey ) == false )
	{
		return false;
	}

	bool arrCheck[ INVEN_TYPE_MAX ]			= { false, };

	for( SpecTreasureKeyVecCItr pos = vecTreasureKey.begin(); pos != vecTreasureKey.end(); ++pos )
	{
		const T_TREASURE_KEY kTreasureKey	= *pos;

		const PT_ITEM_SPEC pItemSpec		= g_xSpecManager->GetSpecItem( kTreasureKey.dwTreasureItemID );
		if( pItemSpec == NULL )
		{
			return false;
		}

		const int nInvenType				= pItemSpec->GetInventoryType();
		if( nInvenType < INVEN_TYPE_NONE || nInvenType >= INVEN_TYPE_MAX )
		{
			return false;
		}

		if( arrCheck[ nInvenType ] == true )
		{
			continue;
		}
		
		const DWORD dwEmptyCount			= pUser->m_Inventory.GetEmptyCountByInventoryType( nInvenType );

		if( dwEmptyCount < kTreasureKey.dwLoopCount )
		{
			return false;
		}

		arrCheck[ nInvenType ]				= true;
	}

	return true;
}

bool	FLTreasureChest::IsValidStateUser( FLWSUser* pUser ) const
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return false;
	}

	if( pUser->IsDie() != FALSE )
	{
		return false;
	}

	if( pUser->m_bAllAction == FALSE )
	{
		return false;
	}

	if( pUser->m_vtInfo.GetOther() != NULL || pUser->m_vtInfo.VendorIsVendor() != FALSE )
	{
		pUser->AddDefinedText( TID_GAME_TRADELIMITUSING );
		return false;
	}

	return true;
}

bool	FLTreasureChest::IsValidStateChestItem( FLWSUser* pUser, FLItemElem* pItemElem ) const
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return false;
	}

	if( IsUsableItem( pItemElem ) == FALSE )
	{
		return false;
	}

	PT_ITEM_SPEC pItemSpec	= pItemElem->GetProp();
	if( pItemSpec == NULL )
	{
		return false;
	}

	if( pItemElem->IsExpiring() != FALSE || pItemElem->IsFlag( FLItemElem::expired ) != FALSE )
	{
		return false;
	}

	if( g_xSpecManager->IsTreasureChest( pItemElem->m_dwItemId ) == false )
	{
		return false;
	}

	return true;
}

bool	FLTreasureChest::IsValidStateKeyItem( FLWSUser* pUser, FLItemElem* pItemElem ) const
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return false;
	}

	if( IsUsableItem( pItemElem ) == FALSE )
	{
		return false;
	}

	PT_ITEM_SPEC pItemSpec	= pItemElem->GetProp();
	if( pItemSpec == NULL )
	{
		return false;
	}

	if( pItemElem->IsExpiring() != FALSE || pItemElem->IsFlag( FLItemElem::expired ) != FALSE )
	{
		return false;
	}

	if( g_xSpecManager->IsTreasureKey( pItemElem->m_dwItemId ) == false )
	{
		return false;
	}

	if( pItemSpec->dwItemLV >= TREASURE_KEY_MAX )
	{
		return false;
	}

	return true;
}

bool	FLTreasureChest::IsMatching( const DWORD dwChestItemID, const DWORD dwKeyItemID ) const
{
	PT_TREASURE_CHEST pChestSpec	= g_xSpecManager->GetSpecTreasureChest( dwChestItemID );
	PT_ITEM_SPEC pKeySpec			= g_xSpecManager->GetSpecItem( dwKeyItemID );
	if( pChestSpec == NULL || pKeySpec == NULL )
	{
		return false;
	}

	if( pKeySpec->dwItemLV >= TREASURE_KEY_MAX )
	{
		return false;
	}
	
	if( pChestSpec->arrKeyItemID[ pKeySpec->dwItemLV ] != dwKeyItemID )
	{
		return false;
	}

	return true;
}
