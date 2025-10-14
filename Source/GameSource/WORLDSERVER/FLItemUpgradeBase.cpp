
#include "StdAfx.h"
#include "FLItemUpgradeBase.h"
#include "../_CommonDefine/Packet/FLPacketItemUpgrade.h"
#include <limits>
#undef max


FLItemUpgradeBase::FLItemUpgradeBase()
{
}

FLItemUpgradeBase::~FLItemUpgradeBase()
{
}

bool	FLItemUpgradeBase::CanUpgradeItemLooksChange( FLWSUser* pUser, const FLPacketItemUpgradeLooksChangeReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID ) == false )
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

	if( pMainProp->bCanLooksChange == FALSE )
	{
		pUser->AddDefinedText( TID_MMI_LOOKCHANGE01 );
		return false;
	}

	if( pMainItem->GetLooksChangeItemID() != 0 || pMaterialItem->GetLooksChangeItemID() != 0 )
	{
		pUser->AddDefinedText( TID_MMI_LOOKCHANGE01 );
		return false;
	}

	if( pMaterialProp->bIsLooksChangeMaterial == FALSE )
	{
		pUser->AddDefinedText( TID_MMI_LOOKCHANGE08 );
		return false;
	}

	if( ( pMainProp->dwItemKind3 != pMaterialProp->dwItemKind3 ) || ( pMainProp->dwHanded != pMaterialProp->dwHanded ) )
	{
		pUser->AddDefinedText( TID_MMI_LOOKCHANGE07 );
		return false;
	}

	/*
	원본 아이템이 성별이 있을 경우 재료아이템은 성별이 같거나 공용이어야 하고
	원본 아이템이 공용일 경우 재료 아이템도 공용이어야 한다.
	*/
	const bool bSexSame = pMainProp->dwItemSex != NULL_ID
		? ( pMainProp->dwItemSex == pMaterialProp->dwItemSex || pMaterialProp->dwItemSex == NULL_ID )
		: ( pMainProp->dwItemSex == pMaterialProp->dwItemSex );

	if( bSexSame == false )
	{
		pUser->AddDefinedText( TID_MMI_LOOKCHAGE12 );
		return false;
	}

	if( pUser->m_Inventory.IsEquip( pMaterialItem->m_dwObjId ) == TRUE )
	{
		pUser->AddDefinedText( TID_GAME_EQUIPPUT );
		return false;
	}

	if( pMaterialItem->m_dwKeepTime != 0 || ( pMaterialProp->dwFlag & IP_FLAG_PERIOD ) )
	{
		pUser->AddDefinedText( TID_MMI_LOOKCHANGE08 );
		return false;
	}

	return true;
}

bool	FLItemUpgradeBase::CanUpgradeItemLooksInitialize( FLWSUser* pUser, const FLPacketItemUpgradeLooksInitializeReq* pMsg ) const
{
	if( IsValidStateUpgradeItem( pUser, pMsg->dwMainItemObjID ) == false )
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

	if( pMainItem->GetLooksChangeItemID() == 0 )
	{
		pUser->AddDefinedText( TID_MMI_LOOKCHANGE02 );
		return false;
	}

	if( pMaterialProp->dwItemKind3 != IK3_LOOKRESTOREITEM )
	{
		pUser->AddDefinedText( TID_MMI_LOOKCHANGE09 );
		return false;
	}

	return true;
}

DWORD	FLItemUpgradeBase::DoUpgradeItemLooksChange( FLWSUser* pUser, const FLPacketItemUpgradeLooksChangeReq* pMsg ) const
{
	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem	= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );

	pUser->UpdateItem( pMainItem->m_dwObjId, UI_LOOKS_CHANGE, pMaterialItem->m_dwItemId );

	g_DPSrvr.PutItemLog( pUser, "O", "LOOKS_CHANGE", pMainItem );

	// 재료 아이템 제거
	g_DPSrvr.PutItemLog( pUser, "O", "LOOKS_CHANGE_MATERIAL", pMaterialItem );
	pUser->RemoveItem( pMaterialItem->m_dwObjId, pMaterialItem->m_nItemNum );

	return FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
}

DWORD	FLItemUpgradeBase::DoUpgradeItemLooksInitialize( FLWSUser* pUser, const FLPacketItemUpgradeLooksInitializeReq* pMsg ) const
{
	FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( pMsg->dwMainItemObjID );
	FLItemElem* pMaterialItem	= pUser->m_Inventory.GetAtId( pMsg->dwMaterialItemObjID );

	pUser->UpdateItem( pMainItem->m_dwObjId, UI_LOOKS_CHANGE, 0 );

	g_DPSrvr.PutItemLog( pUser, "O", "LOOKS_CHANGE_INIT", pMainItem );

	// 재료 아이템 제거
	g_DPSrvr.PutItemLog( pUser, "O", "LOOKS_CHANGE_INIT_MATERIAL", pMaterialItem );
	pUser->RemoveItem( pMaterialItem->m_dwObjId, 1 );

	return FSC_ITEMUPGRADE_UPGRADE_SUCCESS;
}

/************************************************************************/
/*
*/
/************************************************************************/
bool	FLItemUpgradeBase::LoadGeneralEnchantData( CLuaBase & kLua, EnchantDataVec & vecGeneralEnchantData ) const
{
	kLua.GetGloabal( _T( "tGeneralEnchant" ) );
	kLua.PushNil();
	while( kLua.TableLoop( -2 ) )
	{
		T_ENCHANT_DATA kEnchantData;

		kEnchantData.dwItemKind1	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strItemKind1" ) ) );
		kEnchantData.dwItemKind2	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strItemKind2" ) ) );
		kEnchantData.dwItemKind3	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strItemKind3" ) ) );

		if( kEnchantData.dwItemKind1 == NULL_ID || kEnchantData.dwItemKind2 == NULL_ID || kEnchantData.dwItemKind3 == NULL_ID )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ITEM KIND : Kind1(%d), Kind2(%d), Kind3(%d) ]" )
				, kEnchantData.dwItemKind1, kEnchantData.dwItemKind2, kEnchantData.dwItemKind3 );
			RELEASE_BREAK;
			return false;
		}

		kEnchantData.kMaterialKind.dwItemKind1	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strMaterialItemKind1" ) ) );
		kEnchantData.kMaterialKind.dwItemKind2	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strMaterialItemKind2" ) ) );
		kEnchantData.kMaterialKind.dwItemKind3	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strMaterialItemKind3" ) ) );

		if( kEnchantData.kMaterialKind.dwItemKind1 == NULL_ID || kEnchantData.kMaterialKind.dwItemKind2 == NULL_ID || kEnchantData.kMaterialKind.dwItemKind3 == NULL_ID )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID MATERIAL ITEM KIND : Kind1(%d), Kind2(%d), Kind3(%d) ]" )
				, kEnchantData.kMaterialKind.dwItemKind1, kEnchantData.kMaterialKind.dwItemKind2, kEnchantData.kMaterialKind.dwItemKind3 );
			RELEASE_BREAK;
			return false;
		}

		kLua.GetField( -1, "tEnchantProb" );
		kLua.PushNil();
		for( DWORD dwValidLevel = 1; kLua.TableLoop( -2 ); ++dwValidLevel )
		{
			const DWORD dwLevel	= static_cast<DWORD>( kLua.ToNumber( -2 ) );
			if( dwValidLevel != dwLevel )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ MISSING ENCHANT LEVEL : Kind1(%d), Kind2(%d), Kind3(%d), Level(%d) ]" )
					, kEnchantData.dwItemKind1, kEnchantData.dwItemKind2, kEnchantData.dwItemKind3, dwValidLevel );
				RELEASE_BREAK;
				return false;
			}

			T_UPGRADE_PROBABILITY kProb;
			kProb.dwSuccessProb		= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwSuccessProb" ) );
			kProb.dwFailProb		= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwFailProb" ) );
			kProb.dwMinusProb		= MAX_UPGRADE_PROB - kProb.dwSuccessProb - kProb.dwFailProb;

			if( kProb.dwSuccessProb + kProb.dwFailProb + kProb.dwMinusProb != MAX_UPGRADE_PROB )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID PROBABILITY : Kind1(%d), Kind2(%d), Kind3(%d), Level(%d), Success:(%d), Fail:(%d), Minus:(%d) ]" )
					, kEnchantData.dwItemKind1, kEnchantData.dwItemKind2, kEnchantData.dwItemKind3, dwLevel, kProb.dwSuccessProb, kProb.dwFailProb, kProb.dwMinusProb );
				RELEASE_BREAK;
				return false;
			}

			if( kEnchantData.mapUpgradeProb.insert( UpgradeProbDataMap::value_type( dwLevel, kProb ) ).second == false )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED INSERT GENERAL ENCHANT PROB DATA. ENCHANT_LEVEL(%u) ]" ), dwLevel );
				RELEASE_BREAK;
				return false;
			}

			kLua.Pop( 1 );
		}
		kLua.Pop( 1 );

		vecGeneralEnchantData.push_back( kEnchantData );

		kLua.Pop( 1 );
	}
	kLua.Pop( 0 );

	if( vecGeneralEnchantData.empty() == true )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ EMPTY GENERAL ENCHANT DATA ]" ) );
		RELEASE_BREAK;
		return false;
	}

	return true;
}

bool	FLItemUpgradeBase::LoadAttributeEnchantData( CLuaBase & kLua, EnchantDataVec & vecAttributeEnchantData ) const
{
	kLua.GetGloabal( _T( "tAttributeEnchant" ) );
	kLua.PushNil();
	while( kLua.TableLoop( -2 ) )
	{
		T_ENCHANT_DATA kEnchantData;

		kEnchantData.dwItemKind1	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strItemKind1" ) ) );
		kEnchantData.dwItemKind2	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strItemKind2" ) ) );
		kEnchantData.dwItemKind3	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strItemKind3" ) ) );

		if( kEnchantData.dwItemKind1 == NULL_ID || kEnchantData.dwItemKind2 == NULL_ID || kEnchantData.dwItemKind3 == NULL_ID )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ITEM KIND : Kind1(%d), Kind2(%d), Kind3(%d) ]" )
				, kEnchantData.dwItemKind1, kEnchantData.dwItemKind2, kEnchantData.dwItemKind3 );
			RELEASE_BREAK;
			return false;
		}

		kEnchantData.kMaterialKind.dwItemKind1	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strMaterialItemKind1" ) ) );
		kEnchantData.kMaterialKind.dwItemKind2	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strMaterialItemKind2" ) ) );
		kEnchantData.kMaterialKind.dwItemKind3	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strMaterialItemKind3" ) ) );

		if( kEnchantData.kMaterialKind.dwItemKind1 == NULL_ID || kEnchantData.kMaterialKind.dwItemKind2 == NULL_ID || kEnchantData.kMaterialKind.dwItemKind3 == NULL_ID )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID MATERIAL ITEM KIND : Kind1(%d), Kind2(%d), Kind3(%d) ]" )
				, kEnchantData.kMaterialKind.dwItemKind1, kEnchantData.kMaterialKind.dwItemKind2, kEnchantData.kMaterialKind.dwItemKind3 );
			RELEASE_BREAK;
			return false;
		}

		kEnchantData.nAttributeRemovePenya				= static_cast<int>( kLua.GetFieldToNumber( -1, "nAttributeRemovePenya" ) );

		kLua.GetField( -1, "tEnchantProb" );
		kLua.PushNil();
		for( DWORD dwValidLevel = 1; kLua.TableLoop( -2 ); ++dwValidLevel )
		{
			const DWORD dwLevel	= static_cast<DWORD>( kLua.ToNumber( -2 ) );
			if( dwValidLevel != dwLevel )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ MISSING ENCHANT LEVEL : Kind1(%d), Kind2(%d), Kind3(%d), Level(%d) ]" )
					, kEnchantData.dwItemKind1, kEnchantData.dwItemKind2, kEnchantData.dwItemKind3, dwValidLevel );
				RELEASE_BREAK;
				return false;
			}

			T_UPGRADE_PROBABILITY kProb;

			kProb.dwSuccessProb		= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwSuccessProb" ) );
			kProb.dwFailProb		= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwFailProb" ) );
			kProb.dwMinusProb		= MAX_UPGRADE_PROB - kProb.dwSuccessProb - kProb.dwFailProb;

			if( kProb.dwSuccessProb + kProb.dwFailProb + kProb.dwMinusProb != MAX_UPGRADE_PROB )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID PROBABILITY : Kind1(%d), Kind2(%d), Kind3(%d), Level(%d), Success:(%d), Fail:(%d), Minus:(%d) ]" )
					, kEnchantData.dwItemKind1, kEnchantData.dwItemKind2, kEnchantData.dwItemKind3, dwLevel, kProb.dwSuccessProb, kProb.dwFailProb, kProb.dwMinusProb );
				RELEASE_BREAK;
				return false;
			}

			kEnchantData.mapUpgradeProb.insert( UpgradeProbDataMap::value_type( dwLevel, kProb ) );

			T_ITEM_ABILITY_PROPERTY kAbilityProp;

			kAbilityProp.dwAddDamageRate	= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwAddDamageRate" ) );
			kAbilityProp.dwAddDefenseRate	= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwAddDefenseRate" ) );
			kAbilityProp.dwAddAtkDmgRate	= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwAddAtkDmgRate" ) );
			
			if( kEnchantData.mapAbilityPropData.insert( AbilityPropDataMap::value_type( dwLevel, kAbilityProp ) ).second == false )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED INSERT ATTRIBUTE ENCHANT PROB DATA. ENCHANT_LEVEL(%u) ]" ), dwLevel );
				RELEASE_BREAK;
				return false;
			}

			kLua.Pop( 1 );
		}
		kLua.Pop( 1 );

		vecAttributeEnchantData.push_back( kEnchantData );

		kLua.Pop( 1 );
	}
	kLua.Pop( 0 );

	if( vecAttributeEnchantData.empty() == true )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ EMPTY ATTRIBUTE ENCHANT DATA ]" ) );
		RELEASE_BREAK;
		return false;
	}

	return true;
}

bool	FLItemUpgradeBase::LoadPiercingData( CLuaBase & kLua, PiercingDataVec & vecPiercingData ) const
{
	kLua.GetGloabal( _T( "tPiercing" ) );
	kLua.PushNil();
	while( kLua.TableLoop( -2 ) )
	{
		T_PIERCING_DATA kPiercingData;

		kPiercingData.dwItemKind1	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strItemKind1" ) ) );
		kPiercingData.dwItemKind2	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strItemKind2" ) ) );
		kPiercingData.dwItemKind3	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strItemKind3" ) ) );

		if( kPiercingData.dwItemKind1 == NULL_ID || kPiercingData.dwItemKind2 == NULL_ID || kPiercingData.dwItemKind3 == NULL_ID )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ITEM KIND : Kind1(%d), Kind2(%d), Kind3(%d) ]" )
				, kPiercingData.dwItemKind1, kPiercingData.dwItemKind2, kPiercingData.dwItemKind3 );
			RELEASE_BREAK;
			return false;
		}

		kPiercingData.kMaterialKind.dwItemKind1	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strMaterialItemKind1" ) ) );
		kPiercingData.kMaterialKind.dwItemKind2	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strMaterialItemKind2" ) ) );
		kPiercingData.kMaterialKind.dwItemKind3	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strMaterialItemKind3" ) ) );

		if( kPiercingData.kMaterialKind.dwItemKind1 == NULL_ID || kPiercingData.kMaterialKind.dwItemKind2 == NULL_ID || kPiercingData.kMaterialKind.dwItemKind3 == NULL_ID )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID MATERIAL ITEM KIND : Kind1(%d), Kind2(%d), Kind3(%d) ]" )
				, kPiercingData.kMaterialKind.dwItemKind1, kPiercingData.kMaterialKind.dwItemKind2, kPiercingData.kMaterialKind.dwItemKind3 );
			RELEASE_BREAK;
			return false;
		}

		kPiercingData.kPiercedKind.dwItemKind1	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strPiercedItemKind1" ) ) );
		kPiercingData.kPiercedKind.dwItemKind2	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strPiercedItemKind2" ) ) );
		kPiercingData.kPiercedKind.dwItemKind3	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strPiercedItemKind3" ) ) );

		if( kPiercingData.kPiercedKind.dwItemKind1 == NULL_ID || kPiercingData.kPiercedKind.dwItemKind2 == NULL_ID || kPiercingData.kPiercedKind.dwItemKind3 == NULL_ID )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID PIERCED ITEM KIND : Kind1(%d), Kind2(%d), Kind3(%d) ]" )
				, kPiercingData.kPiercedKind.dwItemKind1, kPiercingData.kPiercedKind.dwItemKind2, kPiercingData.kPiercedKind.dwItemKind3 );
			RELEASE_BREAK;
			return false;
		}

		kPiercingData.nItemRemovePenya					= static_cast<int>( kLua.GetFieldToNumber( -1, "nItemRemovePenya" ) );
		kPiercingData.nSizeIncreasePenya				= static_cast<int>( kLua.GetFieldToNumber( -1, "nSizeIncreasePenya" ) );

		kLua.GetField( -1, "tPiercingSizeProb" );
		kLua.PushNil();
		for( DWORD dwValidSize = 1; kLua.TableLoop( -2 ); ++dwValidSize )
		{
			const DWORD dwSize	= static_cast<DWORD>( kLua.ToNumber( -2 ) );
			if( dwValidSize != dwSize )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ MISSING PIERCING SIZE : Kind1(%d), Kind2(%d), Kind3(%d), Size(%d) ]" )
					, kPiercingData.dwItemKind1, kPiercingData.dwItemKind2, kPiercingData.dwItemKind3, dwValidSize );
				RELEASE_BREAK;
				return false;
			}

			T_UPGRADE_PROBABILITY kProb;

			kProb.dwSuccessProb		= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwProb" ) );

			if( kProb.dwSuccessProb > MAX_UPGRADE_PROB )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID PROBABILITY : Kind1(%d), Kind2(%d), Kind3(%d), Size(%d), Prob:(%d) ]" )
					, kPiercingData.dwItemKind1, kPiercingData.dwItemKind2, kPiercingData.dwItemKind3, dwSize, kProb.dwSuccessProb );
				RELEASE_BREAK;
				return false;
			}

			if( kPiercingData.mapSizeProb.insert( UpgradeProbDataMap::value_type( dwSize, kProb ) ).second == false )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED INSERT PIERCING PROB DATA. PIERCING_SIZE(%u) ]" ), dwSize );
				RELEASE_BREAK;
				return false;
			}

			kLua.Pop( 1 );
		}
		kLua.Pop( 1 );

		vecPiercingData.push_back( kPiercingData );

		kLua.Pop( 1 );
	}
	kLua.Pop( 0 );

	if( vecPiercingData.empty() == true )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ EMPTY PIERCING DATA ]" ) );
		RELEASE_BREAK;
		return false;
	}

	return true;
}

bool	FLItemUpgradeBase::LoadRandomOptionExtensionData( CLuaBase & kLua, DstParameterDataMap & mapDstParameterData, RandomOptionDataVec & vecRandomOptionData ) const
{
	kLua.GetGloabal( "tDstParameter" );
	kLua.PushNil();
	while( kLua.TableLoop( -2 ) )
	{
		const int nDstID		= CScript::GetDefineNum( kLua.GetFieldToString( -1, "strDst" ) );
		if( nDstID <= 0 || nDstID >= std::numeric_limits<WORD>::max() )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID DST ID : DstID(%d) ]" ), nDstID );
			RELEASE_BREAK;
			return false;
		}

		T_DST_PARAMETER_DATA kDstParameterData;

		kDstParameterData.wDstID	= static_cast<WORD>( nDstID );

		kLua.GetField( -1, "tAdjProb" );
		kLua.PushNil();
		while( kLua.TableLoop( -2 ) )
		{
			const int nAdjValue	= static_cast<int>( kLua.GetFieldToNumber( -1, "nAdjValue" ) );
			if( nAdjValue == 0 || nAdjValue >= std::numeric_limits<short>::max() )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ADJ VALUE : AdjValue(%d) ]" ), nAdjValue );
				RELEASE_BREAK;
				return false;
			}

			const DWORD dwProb		= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwProb" ) );
			const DWORD dwRetryProb	= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwRetryProb" ) );
			const DWORD dwDummyProb	= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwDummyProb" ) );
			if( dwProb > MAX_RANDOMOPTION_PROB || dwRetryProb > MAX_RANDOMOPTION_PROB || dwDummyProb > MAX_RANDOMOPTION_PROB )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ADJ Prob : AdjValue(%d), Prob(%d), RetryProb(%d), DummyProb(%d) ]" )
					, nAdjValue, dwProb, dwRetryProb, dwDummyProb );
				RELEASE_BREAK;
				return false;
			}

			T_ADJ_PROBABILITY kAdjProb;
			kAdjProb.shAdjValue			= static_cast<short>( nAdjValue );
			kAdjProb.dwProb				= dwProb;
			kDstParameterData.vecAdjProb.push_back( kAdjProb );

			T_ADJ_PROBABILITY kRetryAdjProb;
			kRetryAdjProb.shAdjValue	= static_cast<short>( nAdjValue );
			kRetryAdjProb.dwProb		= dwRetryProb;
			kDstParameterData.vecRetryAdjProb.push_back( kRetryAdjProb );

			T_ADJ_PROBABILITY kDummyAdjProb;
			kDummyAdjProb.shAdjValue	= static_cast<short>( nAdjValue );
			kDummyAdjProb.dwProb		= dwDummyProb;
			kDstParameterData.vecDummyAdjProb.push_back( kDummyAdjProb );

			kLua.Pop( 1 );
		}
		kLua.Pop( 1 );

		if( kDstParameterData.vecAdjProb.empty() == true || kDstParameterData.vecRetryAdjProb.empty() == true || kDstParameterData.vecDummyAdjProb.empty() == true )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ ADJ PROB EMPTY : DST_ID(%d) ]" ), kDstParameterData.wDstID );
			RELEASE_BREAK;
			return false;
		}

		if( mapDstParameterData.insert( DstParameterDataMap::value_type( kDstParameterData.wDstID, kDstParameterData ) ).second == false )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED INSERT DST PARAMETER DATA. DST_ID(%u) ]" ), kDstParameterData.wDstID );
			RELEASE_BREAK;
			return false;
		}

		kLua.Pop( 1 );
	}
	kLua.Pop( 0 );

	if( mapDstParameterData.empty() == true )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ EMPTY DST PARAMETER DATA ]" ) );
		RELEASE_BREAK;
		return false;
	}


	kLua.GetGloabal( "tRandomOptionExtension" );
	kLua.PushNil();
	while( kLua.TableLoop( -2 ) )
	{
		T_RANDOMOPTION_DATA kRandomOptionData;

		kRandomOptionData.dwItemKind1	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strItemKind1" ) ) );
		kRandomOptionData.dwItemKind2	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strItemKind2" ) ) );
		kRandomOptionData.dwItemKind3	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strItemKind3" ) ) );

		if( kRandomOptionData.dwItemKind1 == NULL_ID || kRandomOptionData.dwItemKind2 == NULL_ID || kRandomOptionData.dwItemKind3 == NULL_ID )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ITEM KIND : Kind1(%d), Kind2(%d), Kind3(%d) ]" )
				, kRandomOptionData.dwItemKind1, kRandomOptionData.dwItemKind2, kRandomOptionData.dwItemKind3 );
			RELEASE_BREAK;
			return false;
		}

		kRandomOptionData.kMaterialKind.dwItemKind1	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strMaterialItemKind1" ) ) );
		kRandomOptionData.kMaterialKind.dwItemKind2	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strMaterialItemKind2" ) ) );
		kRandomOptionData.kMaterialKind.dwItemKind3	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strMaterialItemKind3" ) ) );

		if( kRandomOptionData.kMaterialKind.dwItemKind1 == NULL_ID || kRandomOptionData.kMaterialKind.dwItemKind2 == NULL_ID || kRandomOptionData.kMaterialKind.dwItemKind3 == NULL_ID )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID MATERIAL ITEM KIND : Kind1(%d), Kind2(%d), Kind3(%d) ]" )
				, kRandomOptionData.kMaterialKind.dwItemKind1, kRandomOptionData.kMaterialKind.dwItemKind2, kRandomOptionData.kMaterialKind.dwItemKind3 );
			RELEASE_BREAK;
			return false;
		}

		kRandomOptionData.nRandomOptionGeneratePenya		= static_cast<int>( kLua.GetFieldToNumber( -1, "nRandomOptionGeneratePenya" ) );

		kRandomOptionData.dwDummyRandomOptionSize			= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwDummyRandomOptionSize" ) );

		kLua.GetField( -1, "tRandomOptionSizeProb" );
		kLua.PushNil();
		for( DWORD dwValidSize = 1; kLua.TableLoop( -2 ); ++dwValidSize )
		{
			const DWORD dwSize	= static_cast<DWORD>( kLua.ToNumber( -2 ) );
			if( dwValidSize != dwSize )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ MISSING RANDOMOPTION SIZE : Kind1(%d), Kind2(%d), Kind3(%d), Size(%d) ]" )
					, kRandomOptionData.dwItemKind1, kRandomOptionData.dwItemKind2, kRandomOptionData.dwItemKind3, dwValidSize );
				RELEASE_BREAK;
				return false;
			}

			if( dwSize > MAX_RANDOMOPTION_GENERAL_GENERATE_SIZE ) {
				FLERROR_LOG( PROGRAM_NAME, _T( "[ OVERFLOW RANDOMOPTION SIZE : Set Size(%d), Max Size(%d) ]" )
					, dwSize, MAX_RANDOMOPTION_GENERAL_GENERATE_SIZE );
				RELEASE_BREAK;
				return false;
			}

			T_UPGRADE_PROBABILITY kProb;

			kProb.dwSuccessProb		= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwProb" ) );

			if( kProb.dwSuccessProb > MAX_UPGRADE_PROB )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID SIZE PROBABILITY : Kind1(%d), Kind2(%d), Kind3(%d), Size(%d), Success:(%d) ]" )
					, kRandomOptionData.dwItemKind1, kRandomOptionData.dwItemKind2, kRandomOptionData.dwItemKind3, dwSize, kProb.dwSuccessProb );
				RELEASE_BREAK;
				return false;
			}

			if( kRandomOptionData.mapSizeProb.insert( UpgradeProbDataMap::value_type( dwSize, kProb ) ).second == false )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED INSERT RANDOMOPTION PROB DATA. RANDOMOPTION_SIZE(%u) ]" ), dwSize );
				RELEASE_BREAK;
				return false;
			}

			kLua.Pop( 1 );
		}
		kLua.Pop( 1 );

		if( kRandomOptionData.mapSizeProb.empty() == true )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ RANDOMOPTION SIZE IS ZERO : Kind1(%d), Kind2(%d), Kind3(%d) ]" )
				, kRandomOptionData.dwItemKind1, kRandomOptionData.dwItemKind2, kRandomOptionData.dwItemKind3 );
			RELEASE_BREAK;
			return false;
		}

		kLua.GetField( -1, "tDstProb" );
		kLua.PushNil();
		while( kLua.TableLoop( -2 ) )
		{
			const int nDstID		= CScript::GetDefineNum( kLua.GetFieldToString( -1, "strDst" ) );
			if( nDstID <= 0 || nDstID >= std::numeric_limits<WORD>::max() )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID DST ID. DST_ID(%d) ]" ), nDstID );
				RELEASE_BREAK;
				return false;
			}

			const WORD wDstID		= static_cast<WORD>( nDstID );
			DstParameterDataMap::const_iterator kCItr = mapDstParameterData.find( wDstID );
			if( kCItr == mapDstParameterData.end() )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ MISSING DST ID : Kind1(%d), Kind2(%d), Kind3(%d), DstID(%d) ]" )
					, kRandomOptionData.dwItemKind1, kRandomOptionData.dwItemKind2, kRandomOptionData.dwItemKind3, wDstID );
				RELEASE_BREAK;
				return false;
			}

			const DWORD dwProb		= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwProb" ) );
			const DWORD dwRetryProb	= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwRetryProb" ) );
			const DWORD dwDummyProb	= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwDummyProb" ) );
			if( dwProb > MAX_UPGRADE_PROB || dwRetryProb > MAX_UPGRADE_PROB || dwDummyProb > MAX_UPGRADE_PROB )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID DST PROB : Kind1(%d), Kind2(%d), Kind3(%d), DstID(%d), Prob:(%d), RetryProb(%d), DummyProb(%d) ]" )
					, kRandomOptionData.dwItemKind1, kRandomOptionData.dwItemKind2, kRandomOptionData.dwItemKind3, wDstID, dwProb, dwRetryProb, dwDummyProb );
				RELEASE_BREAK;
				return false;
			}

			T_DST_PROBABILITY kDstProb;
			kDstProb.wDstID			= wDstID;
			kDstProb.dwProb			= dwProb;
			kRandomOptionData.vecDstProb.push_back( kDstProb );

			T_DST_PROBABILITY kRetryDstProb;
			kRetryDstProb.wDstID	= wDstID;
			kRetryDstProb.dwProb	= dwRetryProb;
			kRandomOptionData.vecRetryDstProb.push_back( kRetryDstProb );

			T_DST_PROBABILITY kDummyDstProb;
			kDummyDstProb.wDstID	= wDstID;
			kDummyDstProb.dwProb	= dwDummyProb;
			kRandomOptionData.vecDummyDstProb.push_back( kDummyDstProb );

			kLua.Pop( 1 );
		}
		kLua.Pop( 1 );

		if( kRandomOptionData.vecDstProb.empty() == true || kRandomOptionData.vecRetryDstProb.empty() == true || kRandomOptionData.vecDummyDstProb.empty() == true )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ DST PROB EMPTY : Kind1(%d), Kind2(%d), Kind3(%d) ]" )
				, kRandomOptionData.dwItemKind1, kRandomOptionData.dwItemKind2, kRandomOptionData.dwItemKind3 );
			RELEASE_BREAK;
			return false;
		}

		vecRandomOptionData.push_back( kRandomOptionData );

		kLua.Pop( 1 );
	}
	kLua.Pop( 0 );

	if( vecRandomOptionData.empty() == true )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ EMPTY RANDOM OPTION DATA ]" ) );
		RELEASE_BREAK;
		return false;
	}

	return true;
}

bool	FLItemUpgradeBase::LoadCombineData( CLuaBase & kLua, CombineDataVec & vecCombineData ) const
{
	if( _GetContentState( CT_COMPOSE_TWOWEAPON19 ) == CS_NOT_SUPPORTED )
	{
		return false;
	}

	kLua.GetGloabal( "tCombine" );
	kLua.PushNil();
	while( kLua.TableLoop( -2 ) )
	{
		T_COMBINE_DATA kCombineData;

		kCombineData.dwItemKind1		= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strItemKind1" ) ) );
		kCombineData.dwItemKind2		= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strItemKind2" ) ) );
		kCombineData.dwItemKind3		= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strItemKind3" ) ) );
		kCombineData.dwCombineOption	= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwCombineOption" ) );
		kCombineData.dwSuccessProb		= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwSuccessProb" ) );
		kCombineData.nInitializePenya	= static_cast<int>( kLua.GetFieldToNumber( -1, "nInitializePenya" ) );

		if( kCombineData.dwItemKind1 == NULL_ID || kCombineData.dwItemKind2 == NULL_ID || kCombineData.dwItemKind3 == NULL_ID )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID COMBINE ITEM KIND : KIND1(%u), KIND2(%u), KIND3(%u) ]" )
				, kCombineData.dwItemKind1, kCombineData.dwItemKind2, kCombineData.dwItemKind3 );
			RELEASE_BREAK;
			return false;
		}

		if( kCombineData.dwCombineOption == 0 || kCombineData.dwSuccessProb > MAX_UPGRADE_PROB || kCombineData.nInitializePenya < 0 )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID COMBINE DATA : CombineOption(%u), Prob(%u), nInitializePenya(%d) ]" )
				, kCombineData.dwCombineOption, kCombineData.dwSuccessProb, kCombineData.nInitializePenya );
			RELEASE_BREAK;
			return false;
		}

// 		kLua.GetField( -1, "tItemKind3" );
// 		kLua.PushNil();
// 		while( kLua.TableLoop( -2 ) )
// 		{
// 			const DWORD dwItemKind3	= static_cast<DWORD>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strItemKind3" ) ) );
// 
// 			if( dwItemKind3 == NULL_ID || dwItemKind3 >= MAX_ITEM_KIND3 )
// 			{
// 				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ITEM KIND : Kind3(%d)]" ), dwItemKind3 );
// 				RELEASE_BREAK;
// 				return false;
// 			}
// 
// 			kCombineData.vecItemKind3.push_back( dwItemKind3 );
// 
// 			kLua.Pop( 1 );
// 		}
// 		kLua.Pop( 1 );
// 
// 		if( kCombineData.vecItemKind3.empty() == true )
// 		{
// 			FLERROR_LOG( PROGRAM_NAME, _T( "[ EMPTY COMBINE ITEM KIND ]" ) );
// 			RELEASE_BREAK;
// 			return false;
// 		}

		vecCombineData.push_back( kCombineData );

		kLua.Pop( 1 );
	}
	kLua.Pop( 0 );

	if( vecCombineData.empty() == true )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ EMPTY COMBINE DATA ]" ) );
		RELEASE_BREAK;
		return false;
	}

	return true;
}

#ifdef CARD_UPGRADE_SYSTEM
bool	FLItemUpgradeBase::LoadCardCombineData( CLuaBase & kLua, CardCombineDataVec & vecCardCombineData ) const
{
	if( _GetContentState( CT_COMPOSE_TWOWEAPON19 ) == CS_NOT_SUPPORTED )
	{
		return false;
	}

	kLua.GetGloabal( "tCardCombine" );
	kLua.PushNil();
	while( kLua.TableLoop( -2 ) )
	{
		T_CARD_COMBINE_DATA kCardCombineData;

		
		kCardCombineData.dwItemIndex	= static_cast<DWORD>( kLua.GetFieldToNumber ( -1, "dwItemIndex" ) );
		kCardCombineData.dwSuccessProb		= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwSuccessProb" ) );

		vecCardCombineData.push_back( kCardCombineData );

		kLua.Pop( 1 );
	}
	kLua.Pop( 0 );

	if( vecCardCombineData.empty() == true )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ EMPTY CARD COMBINE DATA ]" ) );
		RELEASE_BREAK;
		return false;
	}

	return true;
}
#endif	// CARD_UPGRADE_SYSTEM
const T_ENCHANT_DATA*	FLItemUpgradeBase::GetEnchantData( const T_ITEM_SPEC* pItemSpec, const EnchantDataVec & vecEnchantData ) const
{
	if( pItemSpec == NULL )
	{
		return NULL;
	}

	for( EnchantDataVec::const_iterator pos = vecEnchantData.begin(); pos != vecEnchantData.end(); ++pos )
	{
		if( pos->dwItemKind1 == pItemSpec->dwItemKind1
			&& ( pos->dwItemKind2 == pItemSpec->dwItemKind2 || pos->dwItemKind2 == IK2_NONE )
			&& ( pos->dwItemKind3 == pItemSpec->dwItemKind3 || pos->dwItemKind3 == IK3_NONE ) )
		{
			return &( *pos );
		}
	}

	return NULL;
}

const T_PIERCING_DATA*	FLItemUpgradeBase::GetPiercingData( const T_ITEM_SPEC* pItemSpec, const PiercingDataVec & vecPiercingData ) const
{
	if( pItemSpec == NULL )
	{
		return NULL;
	}

	for( PiercingDataVec::const_iterator pos = vecPiercingData.begin(); pos != vecPiercingData.end(); ++pos )
	{
		if( pos->dwItemKind1 == pItemSpec->dwItemKind1
			&& ( pos->dwItemKind2 == pItemSpec->dwItemKind2 || pos->dwItemKind2 == IK2_NONE )
			&& ( pos->dwItemKind3 == pItemSpec->dwItemKind3 || pos->dwItemKind3 == IK3_NONE ) )
		{
			return &( *pos );
		}
	}

	return NULL;
}

const T_RANDOMOPTION_DATA*	FLItemUpgradeBase::GetRandomOptionData( const T_ITEM_SPEC* pItemSpec, const RandomOptionDataVec & vecRandomOptionData ) const
{
	if( pItemSpec == NULL )
	{
		return NULL;
	}

	for( RandomOptionDataVec::const_iterator pos = vecRandomOptionData.begin(); pos != vecRandomOptionData.end(); ++pos )
	{
		if( pos->dwItemKind1 == pItemSpec->dwItemKind1
			&& ( pos->dwItemKind2 == pItemSpec->dwItemKind2 || pos->dwItemKind2 == IK2_NONE )
			&& ( pos->dwItemKind3 == pItemSpec->dwItemKind3 || pos->dwItemKind3 == IK3_NONE ) )
		{
			return &( *pos );
		}
	}

	return NULL;
}

const T_DST_PARAMETER_DATA*	FLItemUpgradeBase::GetDstParameterData( const WORD wDstID, const DstParameterDataMap & mapDstParameterData ) const
{
	DstParameterDataMap::const_iterator kCItr = mapDstParameterData.find( wDstID );
	if( kCItr != mapDstParameterData.end() )
	{
		return &( kCItr->second );
	}

	return NULL;
}

const T_COMBINE_DATA*	FLItemUpgradeBase::GetCombineData( const T_ITEM_SPEC* pItemSpec, const CombineDataVec & vecCombineData ) const
{
	if( pItemSpec == NULL )
	{
		return NULL;
	}

	for( CombineDataVec::const_iterator pos = vecCombineData.begin(); pos != vecCombineData.end(); ++pos )
	{
		if( pos->dwItemKind1 == pItemSpec->dwItemKind1
			&& ( pos->dwItemKind2 == pItemSpec->dwItemKind2 || pos->dwItemKind2 == IK2_NONE )
			&& ( pos->dwItemKind3 == pItemSpec->dwItemKind3 || pos->dwItemKind3 == IK3_NONE ) )
		{
			return &( *pos );
		}
	}

	return NULL;
}
#ifdef CARD_UPGRADE_SYSTEM
const T_CARD_COMBINE_DATA*	FLItemUpgradeBase::GetCardCombineData( const T_ITEM_SPEC* pItemSpec, const CardCombineDataVec & vecCardCombineData ) const
{
	if( pItemSpec == NULL )
	{
		return NULL;
	}

	for( CardCombineDataVec::const_iterator pos = vecCardCombineData.begin(); pos != vecCardCombineData.end(); ++pos )
	{
		if( pos->dwItemIndex == pItemSpec->dwID )
		{
			return &( *pos );
		}
	}

	return NULL;
}
#endif	// CARD_UPGRADE_SYSTEM
DWORD	FLItemUpgradeBase::DetermineRandomOptionSize( const UpgradeProbDataMap & mapSizeProb ) const
{
	DWORD dwRandomOptionSize = 0;

	if( mapSizeProb.empty() == true )
	{
		return dwRandomOptionSize;
	}

	const DWORD dwRandomProb = ::xRandom( MAX_UPGRADE_PROB );

	for( UpgradeProbDataMap::const_reverse_iterator pos = mapSizeProb.rbegin(); pos != mapSizeProb.rend(); ++pos )
	{
		if( dwRandomProb < pos->second.dwSuccessProb )
		{
			dwRandomOptionSize = pos->first;
			break;
		}
	}

	return dwRandomOptionSize;
}

WORD	FLItemUpgradeBase::DetermineRandomOptionDst( const DSTProbVec & vecDstProb ) const
{
	WORD wDstID	= 0;

	if( vecDstProb.empty() == true )
	{
		return wDstID;
	}

	const DWORD dwRandomProb = ::xRandom( MAX_UPGRADE_PROB );

	for( DSTProbVec::const_iterator pos = vecDstProb.begin(); pos != vecDstProb.end(); ++pos )
	{
		if( dwRandomProb < pos->dwProb )
		{
			wDstID	= pos->wDstID;
			break;
		}
	}

	return wDstID;
}

short	FLItemUpgradeBase::DetermineRandomOptionAdjValue( const ADJProbVec & vecAdjProb, const bool bDecreaseAdjValue ) const
{
	short shAdjValue	= 0;

	if( vecAdjProb.empty() == true )
	{
		return shAdjValue;
	}

	const DWORD dwRandomProb = ::xRandom( MAX_RANDOMOPTION_PROB );

	short shPrevAdjValue = 0;
	for( ADJProbVec::const_iterator pos = vecAdjProb.begin(); pos != vecAdjProb.end(); ++pos )
	{
		if( dwRandomProb < pos->dwProb )
		{
			shAdjValue = bDecreaseAdjValue == true ? shPrevAdjValue : pos->shAdjValue;
		}

		if( shAdjValue != 0 )
		{
			break;
		}

		shPrevAdjValue = pos->shAdjValue;
	}

	return shAdjValue;
}

bool	FLItemUpgradeBase::SetRandomOptionExtension( FLItemElem & kItemElem, const RandomOptionDataVec & vecRandomOptionData, const DstParameterDataMap & mapDstParameterData ) const
{
	const T_ITEM_SPEC* pItemSpec	= g_xSpecManager->GetSpecItem( kItemElem.m_dwItemId );
	if( pItemSpec == NULL )
	{
		return false;
	}

	const T_RANDOMOPTION_DATA* pRandomOptionData = GetRandomOptionData( pItemSpec, vecRandomOptionData );
	if( pRandomOptionData == NULL )
	{
		return false;
	}

	const DWORD dwSize = DetermineRandomOptionSize( pRandomOptionData->mapSizeProb );
	if( dwSize == 0 )
	{
		return false;
	}

	FLRandomOption kRandomOptionBackup;
	kItemElem.GetRandomOption( kRandomOptionBackup );

	kItemElem.InitializeRandomOptionExtension();

	for( DWORD dwIndex = 0; dwIndex < dwSize; ++dwIndex )
	{
		T_RANDOMOPTION_EXT  kRandomOption;

		kRandomOption.wDstID	= DetermineRandomOptionDst( pRandomOptionData->vecDstProb );

		const T_DST_PARAMETER_DATA* pDstParameterData = GetDstParameterData( kRandomOption.wDstID, mapDstParameterData );
		if( pDstParameterData == NULL )
		{
			kItemElem.CopyRandomOption( kRandomOptionBackup );
			return false;
		}

		kRandomOption.shAdjValue = DetermineRandomOptionAdjValue( pDstParameterData->vecAdjProb, false );

		if( kItemElem.SetRandomOptionExtension( kRandomOption ) == false )
		{
			kItemElem.CopyRandomOption( kRandomOptionBackup );
			return false;
		}
	}

	return true;
}

bool	FLItemUpgradeBase::SetNewRandomOptionExtension( FLItemElem & kItemElem, const RandomOptionDataVec & vecRandomOptionData, const DstParameterDataMap & mapDstParameterData ) const
{
	const T_ITEM_SPEC* pItemSpec	= g_xSpecManager->GetSpecItem( kItemElem.m_dwItemId );
	if( pItemSpec == NULL )
	{
		return false;
	}

	const T_RANDOMOPTION_DATA* pRandomOptionData = GetRandomOptionData( pItemSpec, vecRandomOptionData );
	if( pRandomOptionData == NULL )
	{
		return false;
	}

	const DWORD dwSize = DetermineRandomOptionSize( pRandomOptionData->mapSizeProb );
	if( dwSize == 0 )
	{
		return false;
	}

	FLRandomOption kRandomOptionBackup;
	kItemElem.GetRandomOption( kRandomOptionBackup );

	const DWORD dwDecreaseNth = ::xRandom( dwSize );

	kItemElem.InitializeNewRandomOptionExtension();

	for( DWORD dwIndex = 0; dwIndex < dwSize; ++dwIndex )
	{
		T_RANDOMOPTION_EXT  kRandomOption;

		kRandomOption.wDstID	= DetermineRandomOptionDst( pRandomOptionData->vecDstProb );

		const T_DST_PARAMETER_DATA* pDstParameterData = GetDstParameterData( kRandomOption.wDstID, mapDstParameterData );
		if( pDstParameterData == NULL )
		{
			kItemElem.CopyRandomOption( kRandomOptionBackup );
			return false;
		}

		const bool bDecreaseAdjValue = ( dwDecreaseNth == dwIndex ) ? true : false;

		kRandomOption.shAdjValue = DetermineRandomOptionAdjValue( pDstParameterData->vecAdjProb, bDecreaseAdjValue );

		if( kItemElem.SetNewRandomOptionExtension( kRandomOption ) == false )
		{
			kItemElem.CopyRandomOption( kRandomOptionBackup );
			return false;
		}
	}

	return true;
}

bool	FLItemUpgradeBase::SetRetryRandomOptionExtension( FLItemElem & kItemElem, const BYTE byIndex, const RandomOptionDataVec & vecRandomOptionData, const DstParameterDataMap & mapDstParameterData ) const
{
	const T_ITEM_SPEC* pItemSpec	= g_xSpecManager->GetSpecItem( kItemElem.m_dwItemId );
	if( pItemSpec == NULL )
	{
		return false;
	}

	const T_RANDOMOPTION_DATA* pRandomOptionData = GetRandomOptionData( pItemSpec, vecRandomOptionData );
	if( pRandomOptionData == NULL )
	{
		return false;
	}

	FLRandomOption kRandomOptionBackup;
	kItemElem.GetRandomOption( kRandomOptionBackup );

	kItemElem.InitializeRandomOptionExtension();

	const DWORD dwSize	= kRandomOptionBackup.GetRandomOptionExtensionSize();

	for( DWORD dwIndex = 0; dwIndex < dwSize; ++dwIndex )
	{
		T_RANDOMOPTION_EXT  kRandomOption;

		if( dwIndex == byIndex )
		{
			kRandomOption.wDstID		= DetermineRandomOptionDst( pRandomOptionData->vecRetryDstProb );
			
			const T_DST_PARAMETER_DATA* pDstParameterData = GetDstParameterData( kRandomOption.wDstID, mapDstParameterData );
			if( pDstParameterData == NULL )
			{
				kItemElem.CopyRandomOption( kRandomOptionBackup );
				return false;
			}

			kRandomOption.shAdjValue	= DetermineRandomOptionAdjValue( pDstParameterData->vecRetryAdjProb, false );
		}
		else
		{
			kRandomOption.wDstID		= kRandomOptionBackup.GetRandomOptionExtensionDstID( dwIndex );
			kRandomOption.shAdjValue	= kRandomOptionBackup.GetRandomOptionExtensionAdjValue( dwIndex );
		}

		if( kItemElem.SetRandomOptionExtension( kRandomOption ) == false )
		{
			kItemElem.CopyRandomOption( kRandomOptionBackup );
			return false;
		}
	}

	return true;
}

bool	FLItemUpgradeBase::SetDummyRandomOptionExtension( RandomOptionExtensionVec & vecDummyRandomOption, const FLItemElem & kItemElem, const RandomOptionDataVec & vecRandomOptionData, const DstParameterDataMap & mapDstParameterData ) const
{
	const T_ITEM_SPEC* pItemSpec	= g_xSpecManager->GetSpecItem( kItemElem.m_dwItemId );
	if( pItemSpec == NULL )
	{
		return false;
	}

	const T_RANDOMOPTION_DATA* pRandomOptionData = GetRandomOptionData( pItemSpec, vecRandomOptionData );
	if( pRandomOptionData == NULL )
	{
		return false;
	}

	vecDummyRandomOption.clear();

	for( DWORD dwIndex = 0; dwIndex < pRandomOptionData->dwDummyRandomOptionSize; ++dwIndex )
	{
		T_RANDOMOPTION_EXT  kRandomOption;

		kRandomOption.byIndex		= static_cast<BYTE>( dwIndex );
		kRandomOption.wDstID		= DetermineRandomOptionDst( pRandomOptionData->vecDummyDstProb );
		
		const T_DST_PARAMETER_DATA* pDstParameterData = GetDstParameterData( kRandomOption.wDstID, mapDstParameterData );
		if( pDstParameterData == NULL )
		{
			return false;
		}

		kRandomOption.shAdjValue	= DetermineRandomOptionAdjValue( pDstParameterData->vecDummyAdjProb, false );

		vecDummyRandomOption.push_back( kRandomOption );
	}

	if( vecDummyRandomOption.empty() == true )
	{
		return false;
	}

	return true;
}

bool	FLItemUpgradeBase::IsValidStateUpgradeItem( FLWSUser* pUser, const DWORD dwMainItemObjID, const DWORD dwUpgradeType ) const
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return false;
	}

	if( pUser->IsDie() == TRUE )
	{
		return false;
	}

	if( pUser->m_bAllAction == FALSE )
	{
		return false;
	}

	CWorld* pWorld = pUser->GetWorld();
	if( pWorld == NULL || pWorld->GetID() == WI_WORLD_QUIZ )
	{
		pUser->AddDefinedText( TID_GAME_LIMITZONE_USE );
		return false;
	}

	if( pUser->m_vtInfo.GetOther() != NULL || pUser->m_vtInfo.VendorIsVendor() == TRUE )
	{
		pUser->AddDefinedText( TID_GAME_TRADELIMITUSING );
		return false;
	}

	FLItemElem* pItemElem	= pUser->m_Inventory.GetAtId( dwMainItemObjID );
	if( IsUsableItem( pItemElem ) == FALSE )
	{
		return false;
	}

	const T_ITEM_SPEC* pItemSpec	= g_xSpecManager->GetSpecItem( pItemElem->m_dwItemId );
	if( pItemSpec == NULL )
	{
		return false;
	}

	if( dwUpgradeType != NULL_ID && pItemSpec->IsUpgradeAble( dwUpgradeType ) == FALSE )
	{
		pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return false;
	}

	if( pItemElem->IsFlag( FLItemElem::period ) == TRUE || pItemElem->IsFlag( FLItemElem::expired ) == TRUE )
	{
		pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return false;
	}

	if( pItemElem->m_nResistSMItemId != 0 )
	{
		pUser->AddDefinedText( TID_GAME_NOTUPGRADE );
		return false;
	}

	if( pUser->m_Inventory.IsEquip( pItemElem->m_dwObjId ) == TRUE )
	{
		pUser->AddDefinedText( TID_GAME_EQUIPPUT );
		return false;
	}

	return true;
}
