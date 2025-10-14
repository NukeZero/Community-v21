
#include "StdAfx.h"
#include "FLItemUpgradeEatPet.h"
#include "FLItemUsing.h"
#include "../_CommonDefine/Packet/FLPacketItemUpgrade.h"


FLItemUpgradeEatPet::FLItemUpgradeEatPet()
{
}

FLItemUpgradeEatPet::~FLItemUpgradeEatPet()
{
}

FLItemUpgradeEatPet*	FLItemUpgradeEatPet::GetInstance()
{
	static FLItemUpgradeEatPet	sItemUpgradeEatPet;

	return & sItemUpgradeEatPet;
}

void	FLItemUpgradeEatPet::Clear()
{
	for( RandomOptionDataVec::iterator pos = m_vecRandomOptionData.begin(); pos != m_vecRandomOptionData.end(); ++pos )
	{
		pos->mapSizeProb.clear();
		pos->vecDstProb.clear();
		pos->vecRetryDstProb.clear();
		pos->vecDummyDstProb.clear();
	}
	m_vecRandomOptionData.clear();

	for( DstParameterDataMap::iterator pos = m_mapDstParameterData.begin(); pos != m_mapDstParameterData.end(); ++pos )
	{
		pos->second.vecAdjProb.clear();
		pos->second.vecRetryAdjProb.clear();
		pos->second.vecDummyAdjProb.clear();
	}
	m_mapDstParameterData.clear();
}

bool	FLItemUpgradeEatPet::LoadScript( const TCHAR* pszFileName )
{
	CLuaBase kLua;

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), pszFileName );

	if( kLua.RunScript( szFullPath ) != 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return false;
	}

	// Load RandomOption Data
	if( LoadRandomOptionExtensionData( kLua, m_mapDstParameterData, m_vecRandomOptionData ) == false )
	{
		return false;
	}

	return true;
}

DWORD	FLItemUpgradeEatPet::OnDoUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemRandomOptionGenerate( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemRandomOptionGenerate( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeEatPet::OnDoUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemRandomOptionInitialize( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemRandomOptionInitialize( pUser, pMsg );
	}

	return dwResult;
}

size_t	FLItemUpgradeEatPet::GetMaxSizeRandomOption( const DWORD dwItemID ) const
{
	const T_ITEM_SPEC* pItemSpec					= g_xSpecManager->GetSpecItem( dwItemID );
	const T_RANDOMOPTION_DATA* pRandomOptionData	= GetRandomOptionData( pItemSpec, m_vecRandomOptionData );
	if( pRandomOptionData == NULL )
	{
		return 0;
	}

	const size_t nSize								= pRandomOptionData->mapSizeProb.size();

	return nSize;
}

bool	FLItemUpgradeEatPet::CanUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID, IUTYPE_RANDOM_OPTION_EXTENSION ) == false )
	{
		return false;
	}

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem	= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );
	if( IsUsableItem( pMainItem ) == FALSE || IsUsableItem( pMaterialItem ) == FALSE )
	{
		return false;
	}

	const T_ITEM_SPEC* pMainProp		= pMainItem->GetProp();
	const T_ITEM_SPEC* pMaterialProp	= pMaterialItem->GetProp();
	if( pMainProp == NULL || pMaterialProp == NULL )
	{
		return false;
	}

	const T_RANDOMOPTION_DATA* pRandomOptionData = GetRandomOptionData( pMainProp, m_vecRandomOptionData );
	if( pRandomOptionData == NULL )
	{
		pUser->AddDefinedText( TID_GAME_AWAKE_OR_BLESSEDNESS01 );
		return false;
	}

	if( pMaterialProp->dwItemKind3 != IK3_EATPET_RANDOMOPTION_GEN )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	return true;
}

bool	FLItemUpgradeEatPet::CanUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID, IUTYPE_RANDOM_OPTION_EXTENSION ) == false )
	{
		return false;
	}

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );

	if( pMsg->dwMaterialItemObjID != NULL_ID )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	if( pMainItem->IsSetRandomOptionExtension() == false )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	return true;
}

DWORD	FLItemUpgradeEatPet::DoUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem	= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );

	if( pUser->IsUsing( pMainItem ) == TRUE )
	{
		pUser->ResetDestParamRandomOptExtension( pMainItem );
	}

	const bool bResult = SetRandomOptionExtension( *pMainItem, m_vecRandomOptionData, m_mapDstParameterData );

	if( bResult == true )
	{
		g_DPSrvr.PutItemLog( pUser, "r", "EATPETAWAKE", pMainItem, pMainItem->m_nItemNum );
		g_DPSrvr.PutItemLog( pUser, "u", "OnDoUseItemTarget", pMaterialItem );
		pUser->RemoveItem( pMaterialItem->m_dwObjId, 1 );

		pUser->AddDefinedText( TID_GAME_AWAKENING_SUCCESS );
		
		dwResult = FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ SET RANDOMOPTION FAILED. Player:(%07d), ItemId:(%d), SerialNumber(%d) ]" )
			, pUser->m_idPlayer, pMainItem->m_dwItemId, pMainItem->GetSerialNumber() );
	}

	if( pUser->IsUsing( pMainItem ) == TRUE )
	{
		pUser->SetDestParamRandomOptExtension( pMainItem );
	}

	return dwResult;
}

DWORD	FLItemUpgradeEatPet::DoUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const
{
	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );

	if( pUser->IsUsing( pMainItem ) == TRUE )
	{
		pUser->ResetDestParamRandomOptExtension( pMainItem );
	}

	pMainItem->InitializeRandomOptionExtension();

	g_DPSrvr.PutItemLog( pUser, "r", "::AwakeCancel", pMainItem, pMainItem->m_nItemNum );

	pUser->AddDefinedText( TID_GAME_PETAWAKECANCEL_S00 );
	
	return FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
}
