
#include "StdAfx.h"


bool	FLSpecManager::LoadSpecItem()
{
	CScript scanner;
	if( scanner.Load( "Spec_Item.txt", FALSE ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Item Property Load Failed." ) );
		return false;
	}

	int nVer = scanner.GetNumber();			// version

	T_ITEM_SPEC tItemSpec = { 0 };
	while( scanner.tok != FINISHED )
	{
		DWORD dwID = static_cast<DWORD>( scanner.GetInt64() );	// Item Index
		if( static_cast<int>( dwID ) <= 0 )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "ID:%d last_read(ID:%d name:%s)" ), dwID, tItemSpec.dwID, tItemSpec.szName );
			return false;
		}

		memset( &tItemSpec, 0, sizeof( tItemSpec ) );

		tItemSpec.nVer	= nVer;
		tItemSpec.dwID	= dwID;
		
		scanner.GetToken();
		scanner.Token.TrimRight();
		
		FLStrcpy( tItemSpec.szName, _countof( tItemSpec.szName ), (LPCSTR)scanner.Token );

		tItemSpec.dwNum							= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwPackMax						= static_cast<DWORD>( scanner.GetInt64() );
		if( static_cast<int>( tItemSpec.dwPackMax ) <= 0 )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ITEM PACK MAX. ITEM_ID(%u), dwPackMax(%u) ]" ), tItemSpec.dwID, tItemSpec.dwPackMax );
			return false;
		}
		tItemSpec.dwItemKind1					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwItemKind2					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwItemKind3					= static_cast<DWORD>( scanner.GetInt64() );

		tItemSpec.dwItemJob						= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.bPermanence					= static_cast<BOOL>( scanner.GetNumber() != 0 );

		tItemSpec.dwUseable						= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwItemSex						= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwCost						= static_cast<DWORD>( scanner.GetInt64() );
		
		tItemSpec.dwEndurance					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.nAbrasion						= scanner.GetNumber();
		tItemSpec.nMaxRepair					= scanner.GetNumber();
		tItemSpec.dwHanded						= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwFlag						= static_cast<DWORD>( scanner.GetInt64() );
		if( tItemSpec.dwFlag == NULL_ID )
		{
			tItemSpec.dwFlag = 0;
		}

		tItemSpec.dwParts						= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwPartsub						= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.bPartsFile					= static_cast<BOOL>( scanner.GetNumber() != 0 );

		tItemSpec.dwExclusive					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwBasePartsIgnore				= static_cast<DWORD>( scanner.GetInt64() );
		
		tItemSpec.dwItemLV						= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwItemRare					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwShopAble					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.nLog							= scanner.GetNumber();
		tItemSpec.bCharged						= static_cast<BOOL>( scanner.GetNumber() != 0 );

		tItemSpec.dwLinkKindBullet				= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwLinkKind					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwAbilityMin					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwAbilityMax					= static_cast<DWORD>( scanner.GetInt64() );
		
		tItemSpec.eItemType						= static_cast<SAI79::ePropType>(scanner.GetNumber());
		tItemSpec.wItemEatk						= static_cast<short>(scanner.GetNumber());
		tItemSpec.dwParry						= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwblockRating					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.nAddSkillMin					= scanner.GetNumber();
		
		tItemSpec.nAddSkillMax					= scanner.GetNumber();
		tItemSpec.dwAtkStyle					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwWeaponType					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwItemAtkOrder1				= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwItemAtkOrder2				= static_cast<DWORD>( scanner.GetInt64() );
		
		tItemSpec.dwItemAtkOrder3				= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwItemAtkOrder4				= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.tmContinuousPain				= scanner.GetNumber();
		tItemSpec.nShellQuantity				= scanner.GetNumber();
		tItemSpec.dwRecoil						= static_cast<DWORD>( scanner.GetInt64() );
		
		tItemSpec.dwLoadingTime					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.nAdjHitRate					= scanner.GetNumber();
		tItemSpec.fAttackSpeed					= scanner.GetFloat(); 
		tItemSpec.dwDmgShift					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwAttackRange					= static_cast<DWORD>( scanner.GetInt64() );
		
		tItemSpec.nProbability					= scanner.GetNumber();

		for( size_t Nth = 0; Nth < MAX_ITEM_PARAM; ++Nth )
		{
			tItemSpec.dwDestParam[ Nth ]		= static_cast<DWORD>( scanner.GetInt64() );
		}

		for( size_t Nth = 0; Nth < MAX_ITEM_PARAM; ++Nth )
		{
			tItemSpec.nAdjParamVal[ Nth ]		= scanner.GetNumber();
		}

		for( size_t Nth = 0; Nth < MAX_ITEM_PARAM; ++Nth )
		{
			tItemSpec.dwChgParamVal[ Nth ]		= static_cast<DWORD>( scanner.GetInt64() );
		}

		for( size_t Nth = 0; Nth < MAX_ITEM_PARAM; ++Nth )
		{
			tItemSpec.nDestData1[ Nth ]			= scanner.GetNumber();
		}

// 		tItemSpec.dwDestParam[0]				= static_cast<DWORD>( scanner.GetNumber() );
// 		tItemSpec.dwDestParam[1]				= static_cast<DWORD>( scanner.GetNumber() );
// 		tItemSpec.dwDestParam[2]				= static_cast<DWORD>( scanner.GetNumber() );
// 
// 		tItemSpec.nAdjParamVal[0]				= scanner.GetNumber();
// 		tItemSpec.nAdjParamVal[1]				= scanner.GetNumber();
// 		tItemSpec.nAdjParamVal[2]				= scanner.GetNumber();
// 
// 		tItemSpec.dwChgParamVal[0]				= static_cast<DWORD>( scanner.GetNumber() );
// 		tItemSpec.dwChgParamVal[1]				= static_cast<DWORD>( scanner.GetNumber() );
// 		tItemSpec.dwChgParamVal[2]				= static_cast<DWORD>( scanner.GetNumber() );
// 
// 		tItemSpec.nDestData1[0]					= scanner.GetNumber();
// 		tItemSpec.nDestData1[1]					= scanner.GetNumber();
// 		tItemSpec.nDestData1[2]					= scanner.GetNumber();
		
		tItemSpec.dwActiveSkill					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwActiveSkillLv				= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwActiveSkillRate				= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwReqMp						= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwReqFp						= static_cast<DWORD>( scanner.GetInt64() );
		
		tItemSpec.dwReqDisLV					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwReSkill1					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwReSkillLevel1				= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwReSkill2					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwReSkillLevel2				= static_cast<DWORD>( scanner.GetInt64() );
		
		tItemSpec.dwSkillReadyType				= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwSkillReady					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec._dwSkillRange					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwSfxElemental				= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwSfxObj						= static_cast<DWORD>( scanner.GetInt64() );
		
		tItemSpec.dwSfxObj2						= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwSfxObj3						= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwSfxObj4						= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwSfxObj5						= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwUseMotion					= static_cast<DWORD>( scanner.GetInt64() );
		
		tItemSpec.dwCircleTime					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwSkillTime					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwExeTarget					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwUseChance					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwSpellRegion					= static_cast<DWORD>( scanner.GetInt64() );
		
		tItemSpec.dwSpellType					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwReferStat1					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwReferStat2					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwReferTarget1				= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwReferTarget2				= static_cast<DWORD>( scanner.GetInt64() );
		
		tItemSpec.dwReferValue1					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwReferValue2					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwSkillType					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.nItemResistElecricity			= static_cast<int>( scanner.GetFloat() * 100.0f );
		tItemSpec.nItemResistFire	    		= static_cast<int>( scanner.GetFloat() * 100.0f );
		
		tItemSpec.nItemResistWind	    		= static_cast<int>( scanner.GetFloat() * 100.0f );
		tItemSpec.nItemResistWater	    		= static_cast<int>( scanner.GetFloat() * 100.0f );
		tItemSpec.nItemResistEarth	    		= static_cast<int>( scanner.GetFloat() * 100.0f );
		tItemSpec.nEvildoing					= scanner.GetNumber();
		tItemSpec.dwExpertLV					= static_cast<DWORD>( scanner.GetInt64() );
		
		tItemSpec.dwExpertMax					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwSubDefine					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwExp							= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwComboStyle					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.fFlightSpeed					= scanner.GetFloat();
		
		tItemSpec.fFlightLRAngle				= scanner.GetFloat();
		tItemSpec.fFlightTBAngle				= scanner.GetFloat();
		tItemSpec.dwFlightLimit					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwFFuelReMax					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwAFuelReMax					= static_cast<DWORD>( scanner.GetInt64() );
		
		tItemSpec.dwFuelRe						= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwLimitLevel1					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.nReflect						= scanner.GetNumber();
		tItemSpec.dwSndAttack1					= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwSndAttack2					= static_cast<DWORD>( scanner.GetInt64() );

		// szIcon
		scanner.GetToken();
		scanner.GetToken();
		FLStrncpy( tItemSpec.szIcon, _countof( tItemSpec.szIcon ), scanner.token, _countof(tItemSpec.szIcon)-1 );
		tItemSpec.szCommand[sizeof(tItemSpec.szIcon)-1] = 0;
		scanner.GetToken();

		// dwQuestId
		tItemSpec.dwQuestId						= static_cast<DWORD>( scanner.GetInt64() );

		// szTextFileName 
		scanner.GetToken();
		scanner.GetToken();
		FLStrcpy( tItemSpec.szTextFileName, _countof( tItemSpec.szTextFileName ), scanner.token );
		scanner.GetToken();

		// szCommand
		scanner.GetToken();
		FLStrncpy( tItemSpec.szCommand, _countof( tItemSpec.szCommand ), scanner.token, _countof( tItemSpec.szCommand )-1 );
		tItemSpec.szCommand[ _countof(tItemSpec.szCommand)-1 ] = 0;


		tItemSpec.nMinLimitLevel				= scanner.GetNumber();
		if( tItemSpec.nMinLimitLevel <= 0 )
		{
			tItemSpec.nMinLimitLevel = 1;
		}
		tItemSpec.nMaxLimitLevel				= scanner.GetNumber();
		if( tItemSpec.nMaxLimitLevel > MAX_CHARACTER_LEVEL )
		{
			tItemSpec.nMaxLimitLevel = MAX_CHARACTER_LEVEL;
		}

		tItemSpec.nItemGroup					= scanner.GetNumber();
		tItemSpec.nUseLimitGroup				= scanner.GetNumber();

		tItemSpec.nMaxDuplication				= scanner.GetNumber();
		tItemSpec.nEffectValue					= scanner.GetNumber();
		tItemSpec.nTargetMinEnchant				= scanner.GetNumber();
		tItemSpec.nTargetMaxEnchant				= scanner.GetNumber();


		tItemSpec.bResetBind					= static_cast<BOOL>( scanner.GetNumber() != 0 );
		tItemSpec.nBindCondition				= scanner.GetNumber();
		tItemSpec.nResetBindCondition			= scanner.GetNumber();


		tItemSpec.dwHitActiveSkillId			= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwHitActiveSkillLv			= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwHitActiveSkillProb			= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwHitActiveSkillTarget		= static_cast<DWORD>( scanner.GetInt64() );

		tItemSpec.dwDamageActiveSkillId			= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwDamageActiveSkillLv			= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwDamageActiveSkillProb		= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwDamageActiveSkillTarget		= static_cast<DWORD>( scanner.GetInt64() );

		tItemSpec.dwEquipActiveSkillId			= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwEquipActiveSkillLv			= static_cast<DWORD>( scanner.GetInt64() );

		tItemSpec.dwGeneralEnchantMode			= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwAttributeEnchantMode		= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwGemEnchantMode				= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwPiercingMode				= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwRandomOptionMode			= static_cast<DWORD>( scanner.GetInt64() );

		tItemSpec.bAbsoluteTime					= static_cast<BOOL>( scanner.GetNumber() != 0 );

		tItemSpec.dwItemGrade					= static_cast<DWORD>( scanner.GetInt64() );
		if( tItemSpec.dwItemGrade == ITEM_GRADE_NONE )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "ItemGrade is None. ItemID:[%d], Name:[%s]" ), tItemSpec.dwID, tItemSpec.szName );
			return false;
		}
		tItemSpec.bCanTrade						= static_cast<BOOL>( scanner.GetNumber() != 0 );
		tItemSpec.dwMainCategory				= static_cast<DWORD>( scanner.GetInt64() );
		tItemSpec.dwSubCategory					= static_cast<DWORD>( scanner.GetInt64() );
		if( tItemSpec.dwMainCategory == TYPE1_NONE || tItemSpec.dwSubCategory == TYPE2_NONE )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Category Value Invalid. ItemID:[%d], Name:[%s], Main:[%d], Sub:[%d]" )
				, tItemSpec.dwID, tItemSpec.szName, tItemSpec.dwMainCategory, tItemSpec.dwSubCategory );
			return false;
		}

		tItemSpec.bCanHaveServerTransform		= static_cast<BOOL>( scanner.GetNumber() != 0 );

		tItemSpec.bCanSavePotion				= static_cast<BOOL>( scanner.GetNumber() != 0 );

		tItemSpec.bCanLooksChange				= static_cast<BOOL>( scanner.GetNumber() != 0 );
		tItemSpec.bIsLooksChangeMaterial		= static_cast<BOOL>( scanner.GetNumber() != 0 );


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


#ifdef __WORLDSERVER
#ifdef __INTERNALSERVER
		tItemSpec.nLog = -1;			// 로그 안남김..
#endif // __INTERNALSERVER
		
		if( g_eLocal.GetState( EVE_SCHOOL ) )
		{
			tItemSpec.dwShopAble = 1;
			if( tItemSpec.dwItemRare == NULL_ID )
			{
				tItemSpec.dwItemRare = tItemSpec.dwItemLV;
			}
		}
#endif	// __WORLDSERVER

		if( g_xFlyffConfig->GetMainLanguage() == LANG_THA )		// why???
		{
			if( tItemSpec.nMinLimitLevel > 0 && tItemSpec.nMinLimitLevel <= 5 )
			{
				tItemSpec.nMinLimitLevel	= 1;
			}
			else if( tItemSpec.nMinLimitLevel > 5 && tItemSpec.nMinLimitLevel <= 15 )
			{
				tItemSpec.nMinLimitLevel	= 5;
			}
			else if( tItemSpec.nMinLimitLevel > 15 && tItemSpec.nMinLimitLevel <= 30 )
			{
				tItemSpec.nMinLimitLevel	= 15;
			}
			else if( tItemSpec.nMinLimitLevel > 30 && tItemSpec.nMinLimitLevel <= 45 )
			{
				tItemSpec.nMinLimitLevel	= 30;
			}
			else if( tItemSpec.nMinLimitLevel > 45 && tItemSpec.nMinLimitLevel < 60 )
			{
				tItemSpec.nMinLimitLevel	= 45;
			}
		}

		if( nVer <= __VER )
		{
			m_SpecItemNameMap.insert( SpecItemNameMap::value_type( tItemSpec.szName, tItemSpec ) );

#ifdef __CLIENT
			if( tItemSpec.dwReferStat1 == WEAPON_ULTIMATE
				&& ( tItemSpec.dwItemKind2 == IK2_WEAPON_DIRECT || tItemSpec.dwItemKind2 == IK2_WEAPON_MAGIC ) )
			{
				CString strName	= tItemSpec.szName;
				int	nFind	= strName.Find( "@", 0 );
				if( nFind > -1 )
				{
					strName.Delete( nFind - 1, 2 );
					FLStrcpy( tItemSpec.szName, _countof( tItemSpec.szName ), (LPCTSTR)strName );
				}
			}
#endif	// __CLIENT

			bool bResult = m_SpecItemIdMap.insert( SpecItemIdMap::value_type( tItemSpec.dwID, tItemSpec ) ).second;
			if( bResult == false )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Exist Same Item. ID:[%d], Name:[%s]" ), tItemSpec.dwID, tItemSpec.szName );
				return false;
			}
		}

		nVer = scanner.GetNumber();		// version; 
	}

	if( m_SpecItemIdMap.empty() == false )
	{
		return true;
	}

	return false;
}


bool	FLSpecManager::LoadSpecTeleportMap()
{
	if( _GetContentState( CT_TELEPORTER ) == CS_NOT_SUPPORTED )
	{
		return true;
	}

	CScript kScanner;
	if( kScanner.Load( "teleportscroll.txt", FALSE ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "TeleportMap Point Property Load Failed." ) );
		return false;
	}

	DWORD dwIndex = kScanner.GetNumber();

	while( kScanner.tok != FINISHED )
	{
		T_TELEPORTMAP_POINT tSpec = { 0 };
		//::mem_set( &tSpec, 0, sizeof( tSpec ) );

		tSpec.dwIndex = dwIndex;
		tSpec.byType = TELEPORT_MAP_TYPE_SPEC;

		tSpec.tPosition.dwWorldID = kScanner.GetNumber();

		tSpec.tPosition.tPos.x = kScanner.GetFloat();
		tSpec.tPosition.tPos.y = kScanner.GetFloat();
		tSpec.tPosition.tPos.z = kScanner.GetFloat();

		kScanner.GetToken();
		kScanner.Token.TrimRight();

		FLStrcpy( tSpec.szName, _countof( tSpec.szName ), kScanner.Token );

		bool bResult = m_mapTeleportMapPoint.insert( SpecTeleportMapPointMap::value_type( tSpec.dwIndex, tSpec ) ).second;
		if( bResult == false )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "TeleportMap Point insert failed. Index: %d" ), tSpec.dwIndex );
			return false;
		}

		dwIndex = kScanner.GetNumber();
	}

	return true;
}

bool	FLSpecManager::LoadSpecWorldExpandLayer()
{
	CScript kScanner;

	if( kScanner.Load( "layer.inc", FALSE ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "World Expand Layer Property Load Failed." ) );
		return false;
	}

	DWORD dwIndex = static_cast<DWORD>( kScanner.GetNumber() );

	while( kScanner.tok != FINISHED )
	{
		T_WORLD_EXPAND_LAYER	kLayer = { 0 };
		//mem_set( &kLayer, 0, sizeof( kLayer ) );

		kLayer.dwWorldID			= dwIndex;
		kLayer.byExpandLayerCount	= static_cast<BYTE>( kScanner.GetNumber() );

		bool bResult = m_mapWorldExpandLayer.insert( SpecWorldExpandLayerMap::value_type( kLayer.dwWorldID, kLayer ) ).second;
		if( bResult == false )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "World Expand Layer insert failed. WorldID:[%d]" ), kLayer.dwWorldID );
			return false;
		}

		dwIndex = static_cast<DWORD>( kScanner.GetNumber() );
	}

	return true;
}

bool	FLSpecManager::LoadSpecChargeZoneWorld()
{
	CScript kScanner;

	if( kScanner.Load( "PaidWorldSet.txt", FALSE ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Charge Zone World Property Load Failed." ) );
		return false;
	}

	DWORD dwIndex = static_cast<DWORD>( kScanner.GetNumber() );

	while( kScanner.tok != FINISHED )
	{
		T_CHARGE_ZONE_WORLD		kWorld = { 0 };
		//mem_set( &kWorld, 0, sizeof( kWorld ) );

		kWorld.dwIndex			= dwIndex;
		kWorld.dwWorldID		= static_cast<DWORD>( kScanner.GetNumber() );
		kWorld.kStartingPos.x	= kScanner.GetFloat();
		kWorld.kStartingPos.y	= kScanner.GetFloat();
		kWorld.kStartingPos.z	= kScanner.GetFloat();

		kScanner.GetToken();
		kScanner.Token.TrimRight();

		FLStrcpy( kWorld.szWorldName, _countof( kWorld.szWorldName ), kScanner.Token );

		PT_WORLD_EXPAND_LAYER pSpec	= GetSpecWorldExpandLayer( kWorld.dwWorldID );
		if( pSpec != NULL )
		{
			kWorld.byChannelCount	= pSpec->byExpandLayerCount + 1;	// 확장 레이어 수 이므로 디폴트 레이어를 더해준다.
		}
		else
		{
			kWorld.byChannelCount	= 1;	// 확장 레이어가 없으므로 디폴트 레이어만 있다.
		}

		bool bResult = m_mapChargeZoneWorldByPointID.insert( SpecChargeZoneWorldMap::value_type( kWorld.dwIndex, kWorld ) ).second;
		if( bResult == false )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Charge Zone World insert failed. Index:[%d], WorldID:[%d]" ), kWorld.dwIndex, kWorld.dwWorldID );
			return false;
		}

		m_mapChargeZoneWorldByWorldID.insert( SpecChargeZoneWorldMap::value_type( kWorld.dwWorldID, kWorld ) );

		dwIndex = static_cast<DWORD>( kScanner.GetNumber() );
	}

	return true;
}

bool	FLSpecManager::LoadSpecChargeZoneTicket()
{
	CScript kScanner;

	if( kScanner.Load( "PaidWorldTicket.txt", FALSE ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Charge Zone Ticket Property Load Failed." ) );
		return false;
	}

	PT_ITEM_SPEC pItemSpec	= NULL;

	DWORD dwIndex = static_cast<DWORD>( kScanner.GetNumber() );

	while( kScanner.tok != FINISHED )
	{
		T_CHARGE_ZONE_TICKET	kTicket = { 0 };
		//mem_set( &kTicket, 0, sizeof( kTicket ) );

		kTicket.dwItemID	= dwIndex;
		kTicket.dwPointID	= static_cast<DWORD>( kScanner.GetNumber() );

		pItemSpec	= GetSpecItem( kTicket.dwItemID );
		if( pItemSpec == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Charge Zone Ticket Item Not Found. ItemID:[%d]" ), kTicket.dwItemID );
			return false;
		}

		// 유료 지역 월드 데이터가 있는지 체크
		PT_CHARGE_ZONE_WORLD pSpec	= GetSpecChargeZoneWorldByPointID( kTicket.dwPointID );
		if( pSpec == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Charge Zone Ticket Point Index Invalid. ItemID:[%d], PointID:[%d]" )
				, kTicket.dwItemID, kTicket.dwPointID );
			return false;
		}

		SpecChargeZoneTicketMapItr kItr	= m_mapChargeZoneTicket.find( kTicket.dwItemID );
		if( kItr != m_mapChargeZoneTicket.end() )
		{
			kItr->second.push_back( kTicket );
		}
		else
		{
			SpecChargeZoneTicketVec vecTicket;
			vecTicket.clear();
			vecTicket.push_back( kTicket );
			
			bool bResult = m_mapChargeZoneTicket.insert( SpecChargeZoneTicketMap::value_type( kTicket.dwItemID, vecTicket ) ).second;
			if( bResult == false )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Charge Zone Ticket insert failed. ItemID:[%d], PointID:[%d]" ), kTicket.dwItemID, kTicket.dwPointID );
				return false;
			}
		}

		dwIndex = static_cast<DWORD>( kScanner.GetNumber() );
	}

	return true;
}

bool	FLSpecManager::LoadSpecTreasureKey()
{
	CScript kScanner;

	if( kScanner.Load( "TreasureItem.txt", FALSE ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Treasure Key Property Load Failed." ) );
		return false;
	}

	PT_ITEM_SPEC pItemSpec	= NULL;

	DWORD dwIndex = static_cast<DWORD>( kScanner.GetNumber() );

	while( kScanner.tok != FINISHED )
	{
		T_TREASURE_KEY	kKey = { 0 };
		//mem_set( &kKey, 0, sizeof( kKey ) );

		kKey.dwItemID			= dwIndex;
		kKey.dwTreasureItemID	= static_cast<DWORD>( kScanner.GetNumber() );
		kKey.dwProb				= static_cast<DWORD>( kScanner.GetNumber() );
		kKey.dwMaxQuantity		= static_cast<DWORD>( kScanner.GetNumber() );
		kKey.dwLoopCount		= static_cast<DWORD>( kScanner.GetNumber() );
		kKey.dwLimitCount		= static_cast<DWORD>( kScanner.GetNumber() );
		kKey.bNotice			= static_cast<bool>( kScanner.GetNumber() != 0 );

		pItemSpec	= GetSpecItem( kKey.dwItemID );
		if( pItemSpec == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Treasure Key Not Found. KeyItemID:[%d]" ), kKey.dwItemID );
			return false;
		}

		pItemSpec	= GetSpecItem( kKey.dwTreasureItemID );
		if( pItemSpec == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Treasure Item Not Found. KeyItemID:[%d], TreasureItemID:[%d]" )
				, kKey.dwItemID, kKey.dwTreasureItemID );
			return false;
		}

		// 최대 수량 체크
		if( kKey.dwMaxQuantity == 0 || kKey.dwMaxQuantity > pItemSpec->dwPackMax )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Treasure Item Max Quantity Invalid. ItemID:[%d], MaxQuantity:[%d], PackMax:[%d]" )
				, kKey.dwTreasureItemID, kKey.dwMaxQuantity, pItemSpec->dwPackMax );
			return false;
		}

		// 굴림 횟수 체크
		if( kKey.dwLoopCount == 0 || kKey.dwLoopCount > MAX_TREASURE_ITEM_COUNT )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Treasure Key Loop Count Invalid. ItemID:[%d], LoopCount:[%d]" )
				, kKey.dwItemID, kKey.dwLoopCount );
			return false;
		}

		// 제한 수량 체크
		if( kKey.dwLimitCount == 0 || kKey.dwLimitCount > kKey.dwLoopCount )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Treasure Item Limit Count Overflow. ItemID:[%d], LimitCount:[%d], LoopCount:[%d]" )
				, kKey.dwTreasureItemID, kKey.dwLimitCount, kKey.dwLoopCount );
			return false;
		}

		SpecTreasureKeyMapItr kItr	= m_mapTreasureKey.find( kKey.dwItemID );
		if( kItr != m_mapTreasureKey.end() )
		{
			// 굴림 횟수 설정 체크
			if( kItr->second.begin()->dwLoopCount == kKey.dwLoopCount )
			{
				T_TREASURE_KEY kTreasureKey	= kItr->second.back();

				kKey.dwProb	= kKey.dwProb + kTreasureKey.dwProb;

				// 확률 체크
				if( kKey.dwProb > MAX_TREASURE_KEY_PROB )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "Treasure Key Prob Invalid. ItemID:[%d], CurrentProb:[%d], MaxProb:[%d]" )
						, kKey.dwItemID, kKey.dwProb, MAX_TREASURE_KEY_PROB );
					return false;
				}

				kItr->second.push_back( kKey );
			}
			else
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Treasure Key Loop Count Different. ItemID:[%d], LoopCount:[%d]" )
					, kKey.dwItemID, kKey.dwLoopCount );
				return false;
			}
		}
		else
		{
			SpecTreasureKeyVec vecKey;
			vecKey.clear();
			vecKey.push_back( kKey );

			bool bResult	= m_mapTreasureKey.insert( SpecTreasureKeyMap::value_type( kKey.dwItemID, vecKey ) ).second;
			if( bResult == false )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Treasure Key insert failed. ItemID:[%d]" ), kKey.dwItemID );
				return false;
			}
		}

		dwIndex = static_cast<DWORD>( kScanner.GetNumber() );
	}

	// 확률 체크
	if( CheckProbTreasureKey() == false )
	{
		return false;
	}

	return true;
}

bool	FLSpecManager::LoadSpecTreasureChest()
{
	CScript kScanner;

	if( kScanner.Load( "TreasureCombine.txt", FALSE ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Treasure Chest Property Load Failed." ) );
		return false;
	}

	PT_ITEM_SPEC pItemSpec	= NULL;

	DWORD dwIndex = static_cast<DWORD>( kScanner.GetNumber() );

	while( kScanner.tok != FINISHED )
	{
		T_TREASURE_CHEST	kChest = { 0 };
		//mem_set( &kChest, 0, sizeof( kChest ) );

		kChest.dwItemID		= dwIndex;

		pItemSpec	= GetSpecItem( kChest.dwItemID );
		if( pItemSpec == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Treasure Chest Item Not Found. ItemID:[%d]" ), kChest.dwItemID );
			return false;
		}

		DWORD dwKeySize		= 0;

		for( DWORD dwGrade = TREASURE_KEY_LOW_LEVEL; dwGrade < TREASURE_KEY_MAX; ++dwGrade )
		{
			kChest.arrKeyItemID[ dwGrade ]	= static_cast<DWORD>( kScanner.GetNumber() );

			// 키가 설정이 되어있다면
			if( kChest.arrKeyItemID[ dwGrade ] != NULL_ID )
			{
				pItemSpec	= GetSpecItem( kChest.arrKeyItemID[ dwGrade ] );
				if( pItemSpec == NULL )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "Treasure Key Item Not Found. ChestItemID:[%d], KeyItemID:[%d]" )
						, kChest.dwItemID, kChest.arrKeyItemID[ dwGrade ] );
					return false;
				}

				if( IsTreasureKey( kChest.arrKeyItemID[ dwGrade ] ) == false )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "Treasure Key Item is Not Treasure Key. ChestItemID:[%d], KeyItemID:[%d]" )
						, kChest.dwItemID, kChest.arrKeyItemID[ dwGrade ] );
					return false;
				}

				// 키 등급 체크
				if( pItemSpec->dwItemLV != dwGrade )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "Treasure Key Item Grade Invalid. ChestItemID:[%d], KeyItemID:[%d], KeyGrade:[%d], PropKeyGrade:[%d]" )
						, kChest.dwItemID, kChest.arrKeyItemID[ dwGrade ], dwGrade, pItemSpec->dwItemLV );
					return false;
				}

				++dwKeySize;
			}
		}

		// 키 설정이 하나도 없다
		if( dwKeySize == 0 )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Treasure Chest Key Size is Zero. ChestItemID:[%d]" ), kChest.dwItemID );
			return false;
		}

		bool bResult	= m_mapTreasureChest.insert( SpecTreasureChestMap::value_type( kChest.dwItemID, kChest ) ).second;
		if( bResult == false )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Treasure Chest insert failed. ItemID:[%d]" ), kChest.dwItemID );
			return false;
		}

		dwIndex = static_cast<DWORD>( kScanner.GetNumber() );
	}

	return true;
}




/************************************************************************/
// 일  자 : 2012. 05. 17
// 작업자 : imParkth
// 설  명 : VisGroupList.txt 파일 Load
/************************************************************************/
bool	FLSpecManager::LoadSpecVisGroupList()
{
	CScript kScanner;

	if( kScanner.Load( "VisGroupList.txt", FALSE ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "VisGroupList Property Load Failed." ) );
		return false;
	}

	DWORD dwIndex = static_cast<DWORD>( kScanner.GetNumber() );
	while( kScanner.tok != FINISHED )
	{
		T_VIS_GROUP_LIST kVisGroup = { 0, };
		T_VIS_GROUP_LIST_BY_ITEM_ID kVisItemID = { 0, };

		kVisItemID.dwVisGroupID = dwIndex;
		kVisGroup.dwVisGroupID = dwIndex;
		kVisGroup.dwGroupName = static_cast<DWORD>( kScanner.GetNumber() );

		DWORD dwData = NULL_ID;
		for( int iIndex = 0; iIndex < MAX_VIS_GROUP_ITEM_COUNT ; ++iIndex )
		{
			dwData = static_cast<DWORD>( kScanner.GetNumber() );
			kVisGroup.dwVisID[iIndex] = dwData;

			// NULL_ID면 m_mapVisGroupListByVistItemID에 넣지 않는다
			if( NULL_ID == dwData )
				continue;

			kVisItemID.dwVisItemID = dwData;

			bool bGroupResult = m_mapVisGroupListByVisItemID.insert( SpecVisGroupMapByVisItemID::value_type( kVisItemID.dwVisItemID, kVisItemID ) ).second;
			if( false == bGroupResult )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "VisGroupList insert failed. SpecVisGroupMapByVisItemID VisItemID:[%d]" ), kVisItemID.dwVisItemID );
				return false;
			}
		}

		bool bResult = m_mapVisGroupListByGroupID.insert( SpecVisGroupMap::value_type( kVisGroup.dwVisGroupID, kVisGroup ) ).second;
		if( false == bResult )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "VisGroupList insert failed. SpecVisGroupMap VisGroupID:[%d]" ), kVisGroup.dwVisGroupID );
			return false;
		}

		dwIndex = static_cast<DWORD>( kScanner.GetNumber() );
	}

	return true;
}


/************************************************************************/
// 일  자 : 2012. 05. 17
// 작업자 : imParkth
// 설  명 : VisCheckList.txt 파일 Load
/************************************************************************/
bool	FLSpecManager::LoadSpecVisCheckList()
{
	CScript kScanner;

	if( kScanner.Load( "VisCheckList.txt", FALSE ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "VisCheckList Property Load Failed." ) );
		return false;
	}

	DWORD dwIndex = static_cast<DWORD>( kScanner.GetNumber() );
	while( kScanner.tok != FINISHED )
	{
		T_VIS_CHECK_LIST kVisCheckList = {0, };

		kVisCheckList.dwVisItemID = dwIndex;
		for( int iIndex = 0; iIndex < MAX_VIS_CONDITION_COUNT ; ++iIndex )
			kVisCheckList.dwGroupID[iIndex] = static_cast<DWORD>( kScanner.GetNumber() );

		bool bResult = m_mapVisCheckList.insert( SpecVisCheckListMap::value_type( kVisCheckList.dwVisItemID, kVisCheckList ) ).second;
		if( bResult == false )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "VisCheckList insert failed. VisItemID:[%d]" ), kVisCheckList.dwVisItemID );
			return false;
		}

		dwIndex = static_cast<DWORD>( kScanner.GetNumber() );
	}

	return true;
}
