
#include "StdAfx.h"
#include "FLItemUpgradeSystemPet.h"
#include "FLItemUsing.h"
#include "../_CommonDefine/Packet/FLPacketItemUpgrade.h"


FLItemUpgradeSystemPet::FLItemUpgradeSystemPet()
{
}

FLItemUpgradeSystemPet::~FLItemUpgradeSystemPet()
{
}

FLItemUpgradeSystemPet*	FLItemUpgradeSystemPet::GetInstance()
{
	static FLItemUpgradeSystemPet	sItemUpgradeSystemPet;

	return & sItemUpgradeSystemPet;
}

void	FLItemUpgradeSystemPet::Clear()
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

bool	FLItemUpgradeSystemPet::LoadScript( const TCHAR* pszFileName )
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

DWORD	FLItemUpgradeSystemPet::OnDoUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemRandomOptionGenerate( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemRandomOptionGenerate( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeSystemPet::OnDoUpgradeItemRandomOptionGenerateRetry( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateRetryReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemRandomOptionGenerateRetry( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemRandomOptionGenerateRetry( pUser, pMsg );
	}

	return dwResult;
}

DWORD	FLItemUpgradeSystemPet::OnDoUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	if( CanUpgradeItemRandomOptionInitialize( pUser, pMsg ) != false )
	{
		dwResult = DoUpgradeItemRandomOptionInitialize( pUser, pMsg );
	}

	return dwResult;
}

bool	FLItemUpgradeSystemPet::GetRandomOptionDummy( const FLItemElem & kItemElem, const BYTE byIndex, RandomOptionExtensionVec & vecDummyRandomOption, BYTE & byDummyIndex ) const
{
	if( SetDummyRandomOptionExtension( vecDummyRandomOption, kItemElem, m_vecRandomOptionData, m_mapDstParameterData ) == false )
	{
		return false;
	}

	if( vecDummyRandomOption.empty() == true )
	{
		return false;
	}

	const size_t nDummySize	= vecDummyRandomOption.size();

	const size_t nPos		= ::xRandom( nDummySize );

	const WORD wDstID		= kItemElem.GetRandomOptionExtensionDstID( byIndex );
	const short shAdjValue	= kItemElem.GetRandomOptionExtensionAdjValue( byIndex );

	if( wDstID == 0 || shAdjValue == 0 )
	{
		return false;
	}

	byDummyIndex								= static_cast<BYTE>( nPos );
	vecDummyRandomOption.at( nPos ).wDstID		= wDstID;
	vecDummyRandomOption.at( nPos ).shAdjValue	= shAdjValue;

	return true;
}

size_t	FLItemUpgradeSystemPet::GetMaxSizeRandomOption( const DWORD dwItemID ) const
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

bool	FLItemUpgradeSystemPet::CanUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const
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

	if( pMainItem->m_pPet == NULL || pMainItem->m_pPet->GetLevel() < PL_C )
	{
		pUser->AddDefinedText( TID_GAME_AWAKE_OR_BLESSEDNESS01 );
		return false;
	}

	if( pMainItem->IsSetRandomOptionExtension() == true )
	{
		pUser->AddDefinedText( TID_GAME_AWAKE_OR_BLESSEDNESS01 );
		return false;
	}

	const T_RANDOMOPTION_DATA* pRandomOptionData = GetRandomOptionData( pMainProp, m_vecRandomOptionData );
	if( pRandomOptionData == NULL )
	{
		pUser->AddDefinedText( TID_GAME_AWAKE_OR_BLESSEDNESS01 );
		return false;
	}

	if( pRandomOptionData->kMaterialKind.dwItemKind1 != pMaterialProp->dwItemKind1
		|| pRandomOptionData->kMaterialKind.dwItemKind2 != pMaterialProp->dwItemKind2
		|| pRandomOptionData->kMaterialKind.dwItemKind3 != pMaterialProp->dwItemKind3 )
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	return true;
}

bool	FLItemUpgradeSystemPet::CanUpgradeItemRandomOptionGenerateRetry( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateRetryReq* pMsg ) const
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

	if( pMainItem->m_pPet == NULL || pMainItem->m_pPet->GetLevel() < PL_C )
	{
		pUser->AddDefinedText( TID_GAME_AWAKE_OR_BLESSEDNESS01 );
		return false;
	}

	if( pUser->IsUsing( pMainItem ) == TRUE )
	{
		pUser->AddDefinedText( TID_MMI_RANDOMOPTIONRETRY_ERRORTEXT05 );
		return false;
	}

	const T_RANDOMOPTION_DATA* pRandomOptionData = GetRandomOptionData( pMainProp, m_vecRandomOptionData );
	if( pRandomOptionData == NULL )
	{
		pUser->AddDefinedText( TID_GAME_AWAKE_OR_BLESSEDNESS01 );
		return false;
	}

	if( pMainItem->IsSetRandomOptionExtension() == false )
	{
		pUser->AddDefinedText( TID_MMI_RANDOMOPTIONRETRY_ERRORTEXT02 );	// 부여된 랜덤 옵션이 없다.
		return false;
	}

	const size_t nRandomOptionSize	= pMainItem->GetRandomOptionExtensionSize();
	if( nRandomOptionSize < MIN_SIZE_RANDOMOPTION_RETRY )
	{
		pUser->AddDefinedText( TID_MMI_RANDOMOPTIONRETRY_ERRORTEXT03 );	// 부여된 랜덤 옵션이 적다.
		return false;
	}

	if( pMainItem->IsSetSafeGuardRandomOptionExtensionFlag() == true )
	{
		pUser->AddDefinedText( TID_MMI_RANDOMOPTIONRETRY_ERRORTEXT04 );	// 각성 보호 상태이다.
		return false;
	}

	if( nRandomOptionSize <= pMsg->byIndex )
	{
		// 재굴림 할 옵션 인덱스가 사이즈를 넘어선다.
		return false;
	}

	if( pMaterialProp->dwItemKind3 != IK3_RANDOMOPTION_RETRY )
	{
		pUser->AddDefinedText( TID_MMI_RANDOMOPTIONRETRY_ERRORTEXT01 );	// 각성 재굴림의 두루마리가 아니다.
		return false;
	}

	return true;
}

bool	FLItemUpgradeSystemPet::CanUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const
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

	if( pMaterialProp->dwItemKind3 != IK3_SYSTEMPET_RANDOMOPTION_INIT )		// TODO
	{
		pUser->AddDefinedText( TID_GAME_INVALID_TARGET_ITEM );
		return false;
	}

	if( pMainItem->IsSetRandomOptionExtension() == false )
	{
		pUser->AddDefinedText( TID_GAME_PETAWAKECANCEL_E00 );
		return false;
	}

	return true;
}

DWORD	FLItemUpgradeSystemPet::DoUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const
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
		g_DPSrvr.PutItemLog( pUser, "r", "PETAWAKE", pMainItem );
		g_DPSrvr.PutItemLog( pUser, "u", "OnDoUseItemTarget", pMaterialItem );
		g_pItemUsing->CompleteUseItem( pUser, pMaterialItem->m_dwObjId );

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

DWORD	FLItemUpgradeSystemPet::DoUpgradeItemRandomOptionGenerateRetry( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateRetryReq* pMsg ) const
{
	DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR;

	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem	= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );

	const bool bResult = SetRetryRandomOptionExtension( *pMainItem, pMsg->byIndex, m_vecRandomOptionData, m_mapDstParameterData );

	if( bResult == true )
	{
		g_DPSrvr.PutItemLog( pUser, "r", "AWAKE_RETRY", pMainItem, pMainItem->m_nItemNum );
		g_DPSrvr.PutItemLog( pUser, "u", "AWAKE_RETRY_MATERIAL", pMaterialItem );
		g_pItemUsing->CompleteUseItem( pUser, pMaterialItem->m_dwObjId );

//		pUser->AddDefinedText( TID_GAME_AWAKENING_SUCCESS );

		dwResult = FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ SET RETRY RANDOMOPTION FAILED. Player:(%07d), ItemId:(%d), SerialNumber(%d) ]" )
			, pUser->m_idPlayer, pMainItem->m_dwItemId, pMainItem->GetSerialNumber() );
	}

	return dwResult;
}

DWORD	FLItemUpgradeSystemPet::DoUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const
{
	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem	= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );

	if( pUser->IsUsing( pMainItem ) == TRUE )
	{
		pUser->ResetDestParamRandomOptExtension( pMainItem );
	}
	
	pMainItem->InitializeRandomOptionExtension();

	g_DPSrvr.PutItemLog( pUser, "r", "PETAWAKECANCEL", pMainItem );
	g_DPSrvr.PutItemLog( pUser, "u", "OnDoUseItemTarget", pMaterialItem );
	g_pItemUsing->CompleteUseItem( pUser, pMaterialItem->m_dwObjId );

	pUser->AddDefinedText( TID_GAME_PETAWAKECANCEL_S00 );
	
	return FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
}
