
#include "StdAfx.h"
#include "FLFlyffPieceEvent.h"
#include "../_CommonDefine/Packet/FLPacketFlyffPieceEvent.h"


FLFlyffPieceEvent::FLFlyffPieceEvent()
: m_bActivate( false )
{
}

FLFlyffPieceEvent::~FLFlyffPieceEvent()
{
}

FLFlyffPieceEvent*	FLFlyffPieceEvent::GetInstance()
{
	static	FLFlyffPieceEvent sFlyffPieceEvent;

	return & sFlyffPieceEvent;
}

void	FLFlyffPieceEvent::Init()
{
	m_mapExchangeItemData.clear();

	for( ExchangeCategoryMap::iterator pos = m_mapCommonCategoryData.begin(); pos != m_mapCommonCategoryData.end(); ++pos )
	{
		pos->second.clear();
	}
	m_mapCommonCategoryData.clear();

	for( ExchangeCategoryMap::iterator pos = m_mapBaseJobCategoryData.begin(); pos != m_mapBaseJobCategoryData.end(); ++pos )
	{
		pos->second.clear();
	}
	m_mapBaseJobCategoryData.clear();
}

bool	FLFlyffPieceEvent::IsActivate() const
{
	return m_bActivate;
}

void	FLFlyffPieceEvent::SetActivate( const bool bActivate )
{
	m_bActivate = bActivate;
}

bool	FLFlyffPieceEvent::LoadScript()
{
	const bool bResult = (		m_kExchangeItem.LoadScript( _T( "Flyffpiece.txt" ), m_mapExchangeItemData )
							&& ReprocessLoadData( m_mapExchangeItemData )		);

	SetActivate( bResult );

	if( bResult == false )
	{
		Init();
	}

	return bResult;
}

bool	FLFlyffPieceEvent::ReprocessLoadData( const ExchangeItemDataMap & mapExchangeItemData )
{
	for( ExchangeItemDataMap::const_iterator pos = mapExchangeItemData.begin(); pos != mapExchangeItemData.end(); ++pos )
	{
		const T_EXCHANGE_ITEM_DATA kValue	= pos->second;

		const PT_ITEM_SPEC pItemProp	= g_xSpecManager->GetSpecItem( kValue.kPayItem.dwItemID );
		if( pItemProp == NULL )
		{
			FLASSERT( 0 );
			return false;
		}

		const DWORD dwBaseJob	= GetBaseJob( pItemProp->dwItemJob );
		const DWORD	dwSexType	= GetSexType( pItemProp->dwItemSex );

		T_CATEGORY_KEY kKey( pItemProp->dwSubCategory, dwBaseJob, dwSexType );

		const bool bRet = InsertCatagoryData( kKey, kValue, ( dwBaseJob == E_JOB_VAGRANT ) ? m_mapCommonCategoryData : m_mapBaseJobCategoryData );
		if( bRet == false )
		{
			FLASSERT( 0 );
			return false;
		}
	}

	return true;
}

bool	FLFlyffPieceEvent::InsertCatagoryData( const T_CATEGORY_KEY & kKey, const T_EXCHANGE_ITEM_DATA & kValue, ExchangeCategoryMap & _Map ) const
{
	ExchangeCategoryMap::iterator itr = _Map.find( kKey );
	if( itr != _Map.end() )
	{
		itr->second.push_back( kValue );
	}
	else
	{
		ExchangeItemDataVec vecExchangeItemData;
		vecExchangeItemData.push_back( kValue );

		if( _Map.insert( ExchangeCategoryMap::value_type( kKey, vecExchangeItemData ) ).second == false )
		{
			FLASSERT( 0 );
			return false;
		}
	}

	return true;
}

DWORD	FLFlyffPieceEvent::GetBaseJob( const DWORD dwJob ) const
{
	DWORD dwBaseJob = 0;

	switch( dwJob )
	{
	case JOB_MERCENARY:
	case JOB_KNIGHT:
	case JOB_BLADE:
	case JOB_KNIGHT_MASTER:
	case JOB_BLADE_MASTER:
	case JOB_KNIGHT_HERO:
	case JOB_BLADE_HERO:
	case JOB_LORDTEMPLER_HERO:
	case JOB_STORMBLADE_HERO:
		{
			dwBaseJob	= E_JOB_MERCENARY;
		}
		break;

	case JOB_ACROBAT:
	case JOB_JESTER:
	case JOB_RANGER:
	case JOB_JESTER_MASTER:
	case JOB_RANGER_MASTER:
	case JOB_JESTER_HERO:
	case JOB_RANGER_HERO:
	case JOB_WINDLURKER_HERO:
	case JOB_CRACKSHOOTER_HERO:
		{
			dwBaseJob	= E_JOB_ACROBAT;
		}
		break;

	case JOB_ASSIST:
	case JOB_RINGMASTER:
	case JOB_BILLPOSTER:
	case JOB_RINGMASTER_MASTER:
	case JOB_BILLPOSTER_MASTER:
	case JOB_RINGMASTER_HERO:
	case JOB_BILLPOSTER_HERO:
	case JOB_FLORIST_HERO:
	case JOB_FORCEMASTER_HERO:
		{
			dwBaseJob	= E_JOB_ASSIST;
		}
		break;

	case JOB_MAGICIAN:
	case JOB_PSYCHIKEEPER:
	case JOB_ELEMENTOR:
	case JOB_PSYCHIKEEPER_MASTER:
	case JOB_ELEMENTOR_MASTER:
	case JOB_PSYCHIKEEPER_HERO:
	case JOB_ELEMENTOR_HERO:
	case JOB_MENTALIST_HERO:
	case JOB_ELEMENTORLORD_HERO:
		{
			dwBaseJob	= E_JOB_MAGICIAN;
		}
		break;

	default:
		{
			dwBaseJob	= E_JOB_VAGRANT;
		}
		break;
	}

	return dwBaseJob;
}

DWORD	FLFlyffPieceEvent::GetSexType( const DWORD dwSex ) const
{
	DWORD dwSexType = 0;

	if( dwSex == SEX_MALE )
	{
		dwSexType = E_SEX_MALE;
	}
	else if( dwSex == SEX_FEMALE )
	{
		dwSexType = E_SEX_FEMALE;
	}
	else
	{
		dwSexType = E_SEX_SEXLESS;
	}

	return dwSexType;
}

bool	FLFlyffPieceEvent::GetExchangeItemData( const DWORD dwIndex, T_EXCHANGE_ITEM_DATA & kExchangeItemData ) const
{
	ExchangeItemDataMap::const_iterator itr = m_mapExchangeItemData.find( dwIndex );
	if( itr == m_mapExchangeItemData.end() )
	{
		return false;
	}

	kExchangeItemData = itr->second;

	return true;
}

bool	FLFlyffPieceEvent::GetExchangeCategoryData( const T_CATEGORY_KEY kKey, ExchangeItemDataVec & vecExchangeItemData ) const
{
	vecExchangeItemData.clear();

	const T_CATEGORY_KEY kCommon( kKey.dwCategoey, E_JOB_VAGRANT, kKey.dwSex );

	ExchangeCategoryMap::const_iterator itrCommon = m_mapCommonCategoryData.find( kCommon );
	if( itrCommon != m_mapCommonCategoryData.end() )
	{
		vecExchangeItemData.assign( itrCommon->second.begin(), itrCommon->second.end() );
	}

	ExchangeCategoryMap::const_iterator itrBaseJob = m_mapBaseJobCategoryData.find( kKey );
	if( itrBaseJob != m_mapBaseJobCategoryData.end() )
	{
		vecExchangeItemData.insert( vecExchangeItemData.end(), itrBaseJob->second.begin(), itrBaseJob->second.end() );
	}

	if( vecExchangeItemData.empty() == true )
	{
		return false;
	}

	return true;
}

void	FLFlyffPieceEvent::OnViewList( FLWSUser* pUser, const FLPacketFlyffPieceExchangeListReq* pMsg ) const
{
	if( IsValidObj( pUser ) == FALSE || pMsg == NULL )
	{
		return;
	}

	if( IsActivate() == false )
	{
		pUser->AddText( _T( "deactivate state this system" ) );
		return;
	}

	FLSnapshotFlyffPieceExchangeListAck toClient;

	const DWORD dwCurrTick	= ::GetTickCount();
	if( pUser->m_kMenuInputCheck.dwLastMenuInputTick + MIN_MENU_INPUT_TICK > dwCurrTick )
	{
		pUser->AddDefinedText( TID_MMI_TRADESYSTEM_EXCEPTIONTEXT01 );
		pUser->AddPacket( &toClient );
		return;
	}

	const T_CATEGORY_KEY kKey( pMsg->dwSubCategory, GetBaseJob( pUser->m_nJob ), GetSexType( pUser->m_bySex ) );

	toClient.bResult	= GetExchangeCategoryData( kKey, toClient.vecExchangeItemData );

	pUser->AddPacket( &toClient );

	pUser->m_kMenuInputCheck.dwLastMenuInputTick	= dwCurrTick;
}

void	FLFlyffPieceEvent::OnDoExchangeItem( FLWSUser* pUser, const FLPacketExchangeFlyffPieceItemReq* pMsg ) const
{
	if( IsValidObj( pUser ) == FALSE || pMsg == NULL )
	{
		return;
	}

	if( IsActivate() == false )
	{
		pUser->AddText( _T( "deactivate state this system" ) );
		return;
	}

	FLSnapshotExchangeFlyffPieceItemAck toClient;
	toClient.bResult = DoExchangeItem( pUser, pMsg );

	pUser->AddPacket( &toClient );
}

bool	FLFlyffPieceEvent::DoExchangeItem( FLWSUser* pUser, const FLPacketExchangeFlyffPieceItemReq* pMsg ) const
{
	if( IsValidObj( pUser ) == FALSE || pMsg == NULL )
	{
		return false;
	}

	ExchangeItemDataMap::const_iterator itr = m_mapExchangeItemData.find( pMsg->dwIndex );
	if( itr == m_mapExchangeItemData.end() )
	{
		return false;
	}

	const T_EXCHANGE_ITEM_DATA kData	= itr->second;

	if( IsHasConditionItem( pUser, kData.kCondItem ) == false )
	{
		pUser->AddDefinedText( TID_MMI_FLYFFPIECE_TEXT02 );
		return false;
	}

	FLItemElem kPayItemElem;
	kPayItemElem.m_dwItemId		= kData.kPayItem.dwItemID;
	kPayItemElem.m_nItemNum		= kData.kPayItem.nQuantity;
	kPayItemElem.m_byFlag		= ( kData.bBinds == true ) ? static_cast<BYTE>( FLItemElem::binds ) : static_cast<BYTE>( FLItemElem::normal );
	kPayItemElem.SetSerialNumber();

	if( pUser->m_Inventory.IsFull( &kPayItemElem, kPayItemElem.m_nItemNum ) )	{
		pUser->AddDefinedText( TID_GAME_EXTRACTION_ERROR );
		return false;
	}

	if( RemoveCondItem( pUser, kData.kCondItem ) == false )
	{
		pUser->AddDefinedText( TID_MMI_FLYFFPIECE_TEXT02 );
		return false;
	}

	const PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( kData.kPayItem.dwItemID );
	if( pItemProp == NULL )
	{
		return false;
	}

	/*
	FLItemElem kPayItemElem;
	kPayItemElem.m_dwItemId		= kData.kPayItem.dwItemID;
	kPayItemElem.m_nItemNum		= kData.kPayItem.nQuantity;
	kPayItemElem.m_byFlag		= ( kData.bBinds == true ) ? static_cast<BYTE>( FLItemElem::binds ) : static_cast<BYTE>( FLItemElem::normal );
	kPayItemElem.SetSerialNumber();
	*/
	if( pUser->CreateItem( &kPayItemElem ) == FALSE )
	{
		pUser->AddDefinedText( TID_GAME_EXTRACTION_ERROR );
		return false;
	}

	pUser->AddDefinedText( TID_MMI_FLYFFPIECE_TEXT05, "\"%s\"", pItemProp->szName );

	// log
	g_DPSrvr.PutItemLog( pUser, "C", "FLYFF_PIECE_EXCHANGE_PAY", &kPayItemElem );

	return true;
}

bool	FLFlyffPieceEvent::IsHasConditionItem( FLWSUser* pUser, const T_ITEM_BASE_INFO & kCondItemInfo ) const
{
	int		nItemQuantity	= 0;
	DWORD	dwStart			= 0;
	DWORD	dwEnd			= 0;

	PT_ITEM_SPEC pItemSpec = g_xSpecManager->GetSpecItem( kCondItemInfo.dwItemID );

	pUser->m_Inventory.GetObjIndexRange( pItemSpec, &dwStart, &dwEnd );

	for( DWORD dwObjIndex = dwStart; dwObjIndex < dwEnd; ++dwObjIndex )
	{
		FLItemElem* pItemElem = pUser->m_Inventory.GetAt( dwObjIndex );
		if( IsUsableItem( pItemElem ) == FALSE || pItemElem->m_dwItemId != kCondItemInfo.dwItemID || pUser->m_Inventory.IsEquip( pItemElem->m_dwObjId ) == TRUE )
		{
			continue;
		}

		nItemQuantity += pItemElem->m_nItemNum;

		if( nItemQuantity >= kCondItemInfo.nQuantity )
		{
			return true;
		}
	}

	return false;
}

bool	FLFlyffPieceEvent::RemoveCondItem( FLWSUser* pUser, const T_ITEM_BASE_INFO & kCondItemInfo ) const
{
	const T_ITEM_SPEC* pItemSpec = g_xSpecManager->GetSpecItem( kCondItemInfo.dwItemID );
	if( pItemSpec == NULL )
	{
		return false;
	}
	
	int nRemainNum			= kCondItemInfo.nQuantity;

	DWORD dwRemoveItemObjID = GetRemoveItemObjID( pUser, kCondItemInfo );
	if( dwRemoveItemObjID == NULL_ID )
	{
		return false;
	}

	while( dwRemoveItemObjID != NULL_ID )
	{
		FLItemElem* pRemoveItemElem = pUser->m_Inventory.GetAtId( dwRemoveItemObjID );
		if( pRemoveItemElem == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ REMOVE ITEM IS NULL. ItemID(%d), Quantity(%d) ]" )
				, kCondItemInfo.dwItemID, kCondItemInfo.nQuantity );
			return false;
		}

		const int nRemoveCount = pRemoveItemElem->m_nItemNum < nRemainNum ? pRemoveItemElem->m_nItemNum : nRemainNum;

		nRemainNum -= nRemoveCount;

		g_DPSrvr.PutItemLog( pUser, "R", "FLYFF_PIECE_EXCHANGE_COND", pRemoveItemElem, nRemoveCount );

		pUser->RemoveItem( dwRemoveItemObjID, nRemoveCount );

		if( nRemainNum <= 0 )
		{
			break;
		}

		dwRemoveItemObjID = GetRemoveItemObjID( pUser, kCondItemInfo );
	}

	return true;
}

DWORD	FLFlyffPieceEvent::GetRemoveItemObjID( FLWSUser* pUser, const T_ITEM_BASE_INFO & kCondItemInfo ) const
{
	DWORD dwRemoveItemObjID	= NULL_ID;

	DWORD dwStart	= 0;
	DWORD dwEnd		= 0;

	PT_ITEM_SPEC pItemSpec = g_xSpecManager->GetSpecItem( kCondItemInfo.dwItemID );

	pUser->m_Inventory.GetObjIndexRange( pItemSpec, &dwStart, &dwEnd );

	for( DWORD dwObjIndex = dwStart; dwObjIndex < dwEnd; ++dwObjIndex )
	{
		FLItemElem* pItemElem = pUser->m_Inventory.GetAt( dwObjIndex );
		if( IsUsableItem( pItemElem ) == FALSE || pItemElem->m_dwItemId != kCondItemInfo.dwItemID || pUser->m_Inventory.IsEquip( pItemElem->m_dwObjId ) == TRUE )
		{
			continue;
		}

		dwRemoveItemObjID = pItemElem->m_dwObjId;

		break;
	}

	return dwRemoveItemObjID;
}
