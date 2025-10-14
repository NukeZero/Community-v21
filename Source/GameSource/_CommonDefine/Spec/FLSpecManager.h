#ifndef __FLSPECMANAGER_H__
#define __FLSPECMANAGER_H__


//////////////////////////////////////////////////////////////////////////
typedef std::map< DWORD, T_ITEM_SPEC >		SpecItemIdMap;
typedef std::map< std::string, T_ITEM_SPEC >		SpecItemNameMap;
typedef SpecItemIdMap::iterator				SpecItemIdItr;
typedef SpecItemNameMap::iterator			SpecItemNameItr;

typedef	std::map< DWORD, T_TELEPORTMAP_POINT >		SpecTeleportMapPointMap;
typedef	SpecTeleportMapPointMap::iterator			SpecTeleportMapPointItr;
typedef	SpecTeleportMapPointMap::const_iterator		SpecTeleportMapPointCItr;

typedef std::map< DWORD, T_WORLD_EXPAND_LAYER >		SpecWorldExpandLayerMap;
typedef	SpecWorldExpandLayerMap::iterator			SpecWorldExpandLayerItr;
typedef	SpecWorldExpandLayerMap::const_iterator		SpecWorldExpandLayerCItr;

typedef std::map< DWORD, T_CHARGE_ZONE_WORLD >		SpecChargeZoneWorldMap;
typedef	SpecChargeZoneWorldMap::iterator			SpecChargeZoneWorldItr;
typedef	SpecChargeZoneWorldMap::const_iterator		SpecChargeZoneWorldCItr;

typedef std::vector< T_CHARGE_ZONE_TICKET >			SpecChargeZoneTicketVec;
typedef	SpecChargeZoneTicketVec::iterator			SpecChargeZoneTicketVecItr;
typedef	SpecChargeZoneTicketVec::const_iterator		SpecChargeZoneTicketVecCItr;

typedef std::map< DWORD, SpecChargeZoneTicketVec >	SpecChargeZoneTicketMap;
typedef	SpecChargeZoneTicketMap::iterator			SpecChargeZoneTicketMapItr;
typedef	SpecChargeZoneTicketMap::const_iterator		SpecChargeZoneTicketMapCItr;

typedef std::map< DWORD, T_TREASURE_CHEST >			SpecTreasureChestMap;
typedef	SpecTreasureChestMap::iterator				SpecTreasureChestItr;
typedef	SpecTreasureChestMap::const_iterator		SpecTreasureChestCItr;

typedef std::vector< T_TREASURE_KEY >				SpecTreasureKeyVec;
typedef	SpecTreasureKeyVec::iterator				SpecTreasureKeyVecItr;
typedef	SpecTreasureKeyVec::const_iterator			SpecTreasureKeyVecCItr;

typedef std::map< DWORD, SpecTreasureKeyVec >		SpecTreasureKeyMap;
typedef	SpecTreasureKeyMap::iterator				SpecTreasureKeyMapItr;
typedef	SpecTreasureKeyMap::const_iterator			SpecTreasureKeyMapCItr;

// imParkth - 2012. 05. 17 - FLSpecManager.h - Map
typedef std::map< DWORD, T_VIS_GROUP_LIST >			SpecVisGroupMap;
typedef	SpecVisGroupMap::iterator					SpecVisGroupMapItr;
typedef	SpecVisGroupMap::const_iterator				SpecVisGroupMapCItr;

typedef std::map< DWORD, T_VIS_GROUP_LIST_BY_ITEM_ID >			SpecVisGroupMapByVisItemID;
typedef	SpecVisGroupMapByVisItemID::iterator					SpecVisGroupMapByVisItemIDItr;
typedef	SpecVisGroupMapByVisItemID::const_iterator				SpecVisGroupMapByVisItemIDCItr;

typedef std::map< DWORD, T_VIS_CHECK_LIST >			SpecVisCheckListMap;
typedef	SpecVisCheckListMap::iterator				SpecVisCheckListMapItr;
typedef	SpecVisCheckListMap::const_iterator			SpecVisCheckListMapCItr;

//////////////////////////////////////////////////////////////////////////


class	FLSpecManager
{
public:
	static	FLSpecManager *	GetInstance();

public:
	FLSpecManager();
	~FLSpecManager();

	bool					LoadSpecAll();

	PT_ITEM_SPEC			GetSpecItem( DWORD dwItemCode );
	PT_ITEM_SPEC			GetSpecItem( const TCHAR* pItemName );
	
	PT_TELEPORTMAP_POINT	GetSpecTeleportMapPoint( DWORD dwPointID );

	PT_WORLD_EXPAND_LAYER	GetSpecWorldExpandLayer( DWORD dwWorldID );
	
	PT_CHARGE_ZONE_WORLD	GetSpecChargeZoneWorldByPointID( DWORD dwPointID );
	PT_CHARGE_ZONE_WORLD	GetSpecChargeZoneWorldByWorldID( DWORD dwWorldID );
	bool					GetSpecChargeZoneWorldByItemID( DWORD dwItemID, SpecChargeZoneWorldMap* mapChargeZoneWorld );
	bool					IsChargeZoneWorld( DWORD dwWorldID );
	
	bool					GetSpecChargeZoneTicket( DWORD dwItemID, SpecChargeZoneTicketVec* vecChargeZoneTicket );
	bool					IsChargeZoneTicket( DWORD dwItemID );

	PT_TREASURE_CHEST		GetSpecTreasureChest( DWORD dwItemID );
	bool					IsTreasureChest( DWORD dwItemID );

	bool					GetSpecTreasureKey( DWORD dwItemID, SpecTreasureKeyVec* vecTreasureKey );
	bool					IsTreasureKey( DWORD dwItemID );


	SpecItemIdMap				m_SpecItemIdMap;
	SpecTeleportMapPointMap		m_mapTeleportMapPoint;

	// imParkth - 2012. 05. 17 - FLSpecManager.h - GetFunction
	PT_VIS_CHECK_LIST		GetVisCheckList( DWORD _dwItemID );
	DWORD					GetVisGroupID( DWORD _dwItemID );
	PT_VIS_GROUP_LIST		GetVisGroupInfo( DWORD _dwGroupID );



private:
	void					ClearSpec();

	bool					LoadSpecItem();
	bool					LoadSpecTeleportMap();
	bool					LoadSpecWorldExpandLayer();
	bool					LoadSpecChargeZoneWorld();
	bool					LoadSpecChargeZoneTicket();
	bool					LoadSpecTreasureKey();
	bool					LoadSpecTreasureChest();

	bool					CheckProbTreasureKey();

	// imParkth - 2012. 05. 17 - FLSpecManager.h - LoadSpecFunc
	bool					LoadSpecVisGroupList();
	bool					LoadSpecVisCheckList();


private:

	SpecItemNameMap				m_SpecItemNameMap;
	SpecWorldExpandLayerMap		m_mapWorldExpandLayer;
	SpecChargeZoneWorldMap		m_mapChargeZoneWorldByPointID;
	SpecChargeZoneWorldMap		m_mapChargeZoneWorldByWorldID;
	SpecChargeZoneTicketMap		m_mapChargeZoneTicket;
	SpecTreasureChestMap		m_mapTreasureChest;
	SpecTreasureKeyMap			m_mapTreasureKey;

	// imParkth - 2012. 05. 17 - FLSpecManager.h - Variable
	SpecVisGroupMap				m_mapVisGroupListByGroupID;
	SpecVisGroupMapByVisItemID	m_mapVisGroupListByVisItemID;
	SpecVisCheckListMap			m_mapVisCheckList;

};

#define g_xSpecManager		FLSpecManager::GetInstance()

#endif	//__FLSPECMANAGER_H__
