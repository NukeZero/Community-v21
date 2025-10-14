#include "StdAfx.h"

FLSpecManager *	FLSpecManager::GetInstance()
{
	static FLSpecManager	xSpecManager;

	return	( &xSpecManager );
}

FLSpecManager::FLSpecManager()
{
	ClearSpec();
}

FLSpecManager::~FLSpecManager()
{
	ClearSpec();
}

void	FLSpecManager::ClearSpec()
{
	m_SpecItemIdMap.clear();
	m_SpecItemNameMap.clear();
	m_mapTeleportMapPoint.clear();
	m_mapWorldExpandLayer.clear();
	m_mapChargeZoneWorldByPointID.clear();
	m_mapChargeZoneWorldByWorldID.clear();
	m_mapTreasureChest.clear();
	
	for( SpecChargeZoneTicketMapItr kItr = m_mapChargeZoneTicket.begin(); kItr != m_mapChargeZoneTicket.end(); ++kItr )
	{
		kItr->second.clear();
	}
	m_mapChargeZoneTicket.clear();
	
	for( SpecTreasureKeyMapItr kItr = m_mapTreasureKey.begin(); kItr != m_mapTreasureKey.end(); ++kItr )
	{
		kItr->second.clear();
	}
	m_mapTreasureKey.clear();

	// imParkth  - 2012. 05. 17 - ELSpecManager.cpp - ClearSpec
	m_mapVisGroupListByGroupID.clear();
	m_mapVisGroupListByVisItemID.clear();
	m_mapVisCheckList.clear();
}

bool	FLSpecManager::CheckProbTreasureKey()
{
	for( SpecTreasureKeyMapCItr kCItr = m_mapTreasureKey.begin(); kCItr != m_mapTreasureKey.end(); ++kCItr )
	{
		T_TREASURE_KEY kKey	= kCItr->second.back();

		if( kKey.dwProb != MAX_TREASURE_KEY_PROB )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Treasure Key Prob Invalid. ItemID:[%d], TotalProb:[%d], MaxProb:[%d]" )
				, kKey.dwItemID, kKey.dwProb, MAX_TREASURE_KEY_PROB );
			return false;
		}
	}

	return true;
}

bool	FLSpecManager::LoadSpecAll()
{
	if( LoadSpecItem() == false )
	{
		TCHAR szMsg[64] = { 0, };
		FLSPrintf( szMsg, _countof( szMsg ), "Item Property Load Failed" );
		AfxMessageBox( szMsg, MB_OK );

		return	false;
	}

	if( LoadSpecTeleportMap() == false )
	{
		TCHAR szMsg[64] = { 0, };
		FLSPrintf( szMsg, _countof( szMsg ), "TeleportMap Point Property Load Failed" );
		AfxMessageBox( szMsg, MB_OK );
		return false;
	}

	if( LoadSpecWorldExpandLayer() == false )
	{
		TCHAR szMsg[64] = { 0, };
		FLSPrintf( szMsg, _countof( szMsg ), "World Expand Layer Property Load Failed" );
		AfxMessageBox( szMsg, MB_OK );

		return	false;
	}

	if( LoadSpecChargeZoneWorld() == false )
	{
		TCHAR szMsg[64] = { 0, };
		FLSPrintf( szMsg, _countof( szMsg ), "Charge Zone World Property Load Failed" );
		AfxMessageBox( szMsg, MB_OK );

		return	false;
	}

	if( LoadSpecChargeZoneTicket() == false )
	{
		TCHAR szMsg[64] = { 0, };
		FLSPrintf( szMsg, _countof( szMsg ), "Charge Zone Ticket Property Load Failed" );
		AfxMessageBox( szMsg, MB_OK );

		return	false;
	}

	if( LoadSpecTreasureKey() == false )
	{
		TCHAR szMsg[64] = { 0, };
		FLSPrintf( szMsg, _countof( szMsg ), "Treasure Key Property Load Failed" );
		AfxMessageBox( szMsg, MB_OK );

		return	false;
	}

	if( LoadSpecTreasureChest() == false )
	{
		TCHAR szMsg[64] = { 0, };
		FLSPrintf( szMsg, _countof( szMsg ), "Treasure Chest Property Load Failed" );
		AfxMessageBox( szMsg, MB_OK );

		return	false;
	}

	// imParkth - 2012. 05. 17 - FLSpecManager.cpp - Function Load
	if( false == LoadSpecVisGroupList() )
	{
		TCHAR szMsg[64] = { 0, };
		FLSPrintf( szMsg, _countof( szMsg ), "VisGroupList Property Load Failed" );
		AfxMessageBox( szMsg, MB_OK );

		return	false;
	}

	// imParkth - 2012. 05. 17 - FLSpecManager.cpp - Function Load
	if( false == LoadSpecVisCheckList() )
	{
		TCHAR szMsg[64] = { 0, };
		FLSPrintf( szMsg, _countof( szMsg ), "VisCheckList Property Load Failed" );
		AfxMessageBox( szMsg, MB_OK );

		return	false;
	}

	return	true;
}

PT_ITEM_SPEC	FLSpecManager::GetSpecItem( DWORD dwItemCode )
{
	SpecItemIdItr	Itr;
	Itr = m_SpecItemIdMap.find( dwItemCode );

	PT_ITEM_SPEC pSpec	= NULL;

	if( Itr != m_SpecItemIdMap.end() )
	{
		pSpec = (&Itr->second);
	}

	return pSpec;
}

PT_ITEM_SPEC	FLSpecManager::GetSpecItem( const TCHAR* pItemName )
{
	SpecItemNameItr	Itr;
	Itr = m_SpecItemNameMap.find( pItemName );

	PT_ITEM_SPEC pSpec	= NULL;

	if( Itr != m_SpecItemNameMap.end() )
	{
		pSpec = (&Itr->second);
	}

	return pSpec;
}

PT_TELEPORTMAP_POINT	FLSpecManager::GetSpecTeleportMapPoint( DWORD dwPointID )
{
	PT_TELEPORTMAP_POINT pSpec = NULL;

	SpecTeleportMapPointItr kIter = m_mapTeleportMapPoint.find( dwPointID );
	if( kIter != m_mapTeleportMapPoint.end() )
	{
		pSpec = &(kIter->second);
	}

	return pSpec;
}

PT_WORLD_EXPAND_LAYER	FLSpecManager::GetSpecWorldExpandLayer( DWORD dwWorldID )
{
	PT_WORLD_EXPAND_LAYER pSpec = NULL;

	SpecWorldExpandLayerItr kIter = m_mapWorldExpandLayer.find( dwWorldID );
	if( kIter != m_mapWorldExpandLayer.end() )
	{
		pSpec = &( kIter->second );
	}

	return pSpec;
}

PT_CHARGE_ZONE_WORLD	FLSpecManager::GetSpecChargeZoneWorldByPointID( DWORD dwPointID )
{
	PT_CHARGE_ZONE_WORLD pSpec = NULL;

	SpecChargeZoneWorldItr kIter = m_mapChargeZoneWorldByPointID.find( dwPointID );
	if( kIter != m_mapChargeZoneWorldByPointID.end() )
	{
		pSpec = &( kIter->second );
	}

	return pSpec;
}

PT_CHARGE_ZONE_WORLD	FLSpecManager::GetSpecChargeZoneWorldByWorldID( DWORD dwWorldID )
{
	PT_CHARGE_ZONE_WORLD pSpec = NULL;

	SpecChargeZoneWorldItr kIter = m_mapChargeZoneWorldByWorldID.find( dwWorldID );
	if( kIter != m_mapChargeZoneWorldByWorldID.end() )
	{
		pSpec = &( kIter->second );
	}

	return pSpec;
}

bool	FLSpecManager::GetSpecChargeZoneWorldByItemID( DWORD dwItemID, SpecChargeZoneWorldMap* mapChargeZoneWorld )
{
	mapChargeZoneWorld->clear();
	
	SpecChargeZoneTicketVec vecTicket;

	if( GetSpecChargeZoneTicket( dwItemID, &vecTicket ) == false )
	{
		return false;
	}

	for( SpecChargeZoneTicketVecCItr kCItr = vecTicket.begin(); kCItr != vecTicket.end(); ++kCItr )
	{
		PT_CHARGE_ZONE_WORLD pSpec	= GetSpecChargeZoneWorldByPointID( kCItr->dwPointID );
		if( pSpec != NULL )
		{
			mapChargeZoneWorld->insert( SpecChargeZoneWorldMap::value_type( pSpec->dwWorldID, *pSpec ) );
		}
	}

	if( mapChargeZoneWorld->empty() == false )
	{
		return true;
	}

	return false;
}

bool	FLSpecManager::IsChargeZoneWorld( DWORD dwWorldID )
{
	SpecChargeZoneWorldCItr kCIter = m_mapChargeZoneWorldByWorldID.find( dwWorldID );
	if( kCIter != m_mapChargeZoneWorldByWorldID.end() )
	{
		return true;
	}

	return false;
}

bool	FLSpecManager::GetSpecChargeZoneTicket( DWORD dwItemID, SpecChargeZoneTicketVec* vecChargeZoneTicket )
{
	vecChargeZoneTicket->clear();

	SpecChargeZoneTicketMapCItr kCItr = m_mapChargeZoneTicket.find( dwItemID );
	if( kCItr != m_mapChargeZoneTicket.end() )
	{
		vecChargeZoneTicket->assign( kCItr->second.begin(), kCItr->second.end() );
	}

	if( vecChargeZoneTicket->empty() == false )
	{
		return true;
	}

	return false;
}

bool	FLSpecManager::IsChargeZoneTicket( DWORD dwItemID )
{
	SpecChargeZoneTicketMapCItr kCItr = m_mapChargeZoneTicket.find( dwItemID );
	if( kCItr != m_mapChargeZoneTicket.end() )
	{
		return true;
	}

	return false;
}

PT_TREASURE_CHEST	FLSpecManager::GetSpecTreasureChest( DWORD dwItemID )
{
	PT_TREASURE_CHEST pSpec	= NULL;

	SpecTreasureChestItr kIter = m_mapTreasureChest.find( dwItemID );
	if( kIter != m_mapTreasureChest.end() )
	{
		pSpec = &( kIter->second );
	}

	return pSpec;
}

bool	FLSpecManager::IsTreasureChest( DWORD dwItemID )
{
	SpecTreasureChestCItr kCItr = m_mapTreasureChest.find( dwItemID );
	if( kCItr != m_mapTreasureChest.end() )
	{
		return true;
	}

	return false;
}

bool	FLSpecManager::GetSpecTreasureKey( DWORD dwItemID, SpecTreasureKeyVec* vecTreasureKey )
{
	vecTreasureKey->clear();

	SpecTreasureKeyMapCItr kCItr = m_mapTreasureKey.find( dwItemID );
	if( kCItr != m_mapTreasureKey.end() )
	{
		vecTreasureKey->assign( kCItr->second.begin(), kCItr->second.end() );
	}

	if( vecTreasureKey->empty() == false )
	{
		return true;
	}

	return false;
}

bool	FLSpecManager::IsTreasureKey( DWORD dwItemID )
{
	SpecTreasureKeyMapCItr kCItr = m_mapTreasureKey.find( dwItemID );
	if( kCItr != m_mapTreasureKey.end() )
	{
		return true;
	}

	return false;
}


/************************************************************************/
// 일  자 : 2012. 05. 17
// 작업자 : imParkth
// 설  명 : ItemID를 이용해 확인 Group를 얻는 함수
/************************************************************************/
PT_VIS_CHECK_LIST	FLSpecManager::GetVisCheckList( DWORD _dwItemID )
{
	PT_VIS_CHECK_LIST pCheckList = NULL;
	SpecVisCheckListMapItr kIter = m_mapVisCheckList.find( _dwItemID );
	if( kIter != m_mapVisCheckList.end() )
	{
		pCheckList = &(kIter->second);
	}

	return pCheckList;
}

/************************************************************************/
// 일  자 : 2012. 05. 17
// 작업자 : imParkth
// 설  명 : ItemID를 이용해 Group를 얻는 함수
/************************************************************************/
DWORD	FLSpecManager::GetVisGroupID( DWORD _dwItemID )
{
	T_VIS_GROUP_LIST_BY_ITEM_ID kVisGroupID;
	SpecVisGroupMapByVisItemIDItr kIter = m_mapVisGroupListByVisItemID.find( _dwItemID );
	if( kIter != m_mapVisGroupListByVisItemID.end() )
	{
		kVisGroupID = kIter->second;
		return kVisGroupID.dwVisGroupID;
	}
	return NULL_ID;
}


/************************************************************************/
// 일  자 : 2012. 05. 18
// 작업자 : imParkth
// 설  명 : GroupID를 이용해 정보를 얻는 함수
/************************************************************************/
PT_VIS_GROUP_LIST	FLSpecManager::GetVisGroupInfo( DWORD _dwGroupID )
{
	PT_VIS_GROUP_LIST pGroupList = NULL;
	SpecVisGroupMapItr kIter = m_mapVisGroupListByGroupID.find( _dwGroupID );
	if( kIter != m_mapVisGroupListByGroupID.end() )
	{
		pGroupList = &(kIter->second);
	}

	return pGroupList;
}