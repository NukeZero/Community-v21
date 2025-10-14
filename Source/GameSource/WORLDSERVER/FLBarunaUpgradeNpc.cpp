#include "StdAfx.h"
#include "FLBarunaUpgradeNpc.h"


FLBarunaUpgradeNpc::FLBarunaUpgradeNpc()
: m_dwExpLimit( 0 )
{
	Clear();
}

FLBarunaUpgradeNpc::~FLBarunaUpgradeNpc()
{
	Clear();
}

FLBarunaUpgradeNpc*	FLBarunaUpgradeNpc::GetInstance()
{
	static FLBarunaUpgradeNpc	xBarunaUpgradeNpc;

	return & xBarunaUpgradeNpc;
}

void	FLBarunaUpgradeNpc::Clear()
{
	m_tblBarunaNPCLevel.clear();
	m_mapBarunaNPCExpEnchantGeneral.clear();
	m_mapBarunaNPCExpEnchantAttribute.clear();
	m_mapBarunaNPCExpRandomOption.clear();
}

bool	FLBarunaUpgradeNpc::LoadScript()
{
	CLuaBase kLua;

	const TCHAR szFileName[]	= _T( "BarunaNpc.lua" );

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), szFileName );

	if( kLua.RunScript( szFullPath ) != 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return false;
	}

	//레벨 
	char * szBarunaNPCLevel = "tBarunaNPCLevel";
	kLua.GetGloabal( szBarunaNPCLevel );
	kLua.PushNil();

	DWORD dwExpRequiredOld	= 0;
	for( DWORD dwValidLevel = 1; kLua.TableLoop( -2 ); ++dwValidLevel )
	{
		const DWORD dwLevel			= static_cast< DWORD >( kLua.ToNumber( -2 ) );
		if( dwValidLevel != dwLevel )			//누락된 레벨값 확인
		{
			//VMessageBox( MB_OK, _T( "[ MISSING LEVEL : ItemKind(%s) Level(%u) ]" ), szBarunaNPCLevel, dwValidLevel );	
			FLERROR_LOG( PROGRAM_NAME, _T( "[ MISSING LEVEL : ItemKind(%s) Level(%d) ]" ), szBarunaNPCLevel, dwValidLevel );	
			RELEASE_BREAK;
			return false;
		}

		T_BARUNA_NPC_LEVEL LevelElem = { 0 };
		//mem_set( &LevelElem, 0, sizeof( LevelElem ) );

		LevelElem.dwExpRequired	= static_cast< DWORD >( kLua.GetFieldToNumber( -1, "dwExpRequired" ) );
		if( LevelElem.dwExpRequired <= dwExpRequiredOld ) 
		{
			//VMessageBox( MB_OK, _T( "[ INVALID dwExpRequired : ItemKind(%s) Level(%u) dwExpRequired(%u) OldExpRequired(%u) ]" ), szBarunaNPCLevel, dwValidLevel, LevelElem.dwExpRequired, dwExpRequiredOld );	
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID dwExpRequired : ItemKind(%s) Level(%u) dwExpRequired(%u) OldExpRequired(%u) ]" ), szBarunaNPCLevel, dwValidLevel, LevelElem.dwExpRequired, dwExpRequiredOld );	
			RELEASE_BREAK;
			return false;
		}

		LevelElem.dwEachExp			= LevelElem.dwExpRequired - dwExpRequiredOld;
		dwExpRequiredOld				= LevelElem.dwExpRequired;
		m_dwExpLimit						= LevelElem.dwExpRequired;

		LevelElem.dwAdditionalSuccessProp= static_cast< DWORD >( kLua.GetFieldToNumber( -1, "dwAdditionalSuccessProp" ) );

		kLua.Pop( 1 );

		m_tblBarunaNPCLevel.insert( MapBarunaNPCLevelTbl::value_type( dwLevel, LevelElem ) );

	}
	kLua.Pop( 0 );

	//////////////////////////////////////////////////////////////////////////
	// 습득 경험치
	kLua.GetGloabal( "tBarunaNPCExpEnchantGeneral" );
	kLua.PushNil();
	while( kLua.TableLoop( -2 ) )
	{
		T_BARUNA_NPC_EXP kBarunaNPCExp;

		DWORD dwUpgradeLevel			= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwUpgradeLevel" ) );
		kBarunaNPCExp.dwSuccessExp		= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwSuccessExp" ) );
		kBarunaNPCExp.dwFailExp			= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwFailExp" ) );
		kBarunaNPCExp.dwMinusExp		= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwMinusExp" ) );

		m_mapBarunaNPCExpEnchantGeneral.insert( MapBarunaNPCExp::value_type( dwUpgradeLevel, kBarunaNPCExp ) );

		kLua.Pop( 1 );
	}
	kLua.Pop( 0 );

	kLua.GetGloabal( "tBarunaNPCExpEnchantAttribute" );
	kLua.PushNil();
	while( kLua.TableLoop( -2 ) )
	{
		T_BARUNA_NPC_EXP kBarunaNPCExp;

		DWORD dwUpgradeLevel			= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwUpgradeLevel" ) );
		kBarunaNPCExp.dwSuccessExp		= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwSuccessExp" ) );
		kBarunaNPCExp.dwFailExp			= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwFailExp" ) );
		kBarunaNPCExp.dwMinusExp		= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwMinusExp" ) );

		m_mapBarunaNPCExpEnchantAttribute.insert( MapBarunaNPCExp::value_type( dwUpgradeLevel, kBarunaNPCExp ) );

		kLua.Pop( 1 );
	}
	kLua.Pop( 0 );

	kLua.GetGloabal( "tBarunaNPCExpRandomOption" );
	kLua.PushNil();
	while( kLua.TableLoop( -2 ) )
	{
		T_BARUNA_NPC_EXP kBarunaNPCExp;

		DWORD dwUpgradeLevel			= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwUpgradeLevel" ) );
		kBarunaNPCExp.dwSuccessExp		= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwSuccessExp" ) );
		kBarunaNPCExp.dwFailExp			= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwFailExp" ) );
		kBarunaNPCExp.dwMinusExp		= static_cast<DWORD>( kLua.GetFieldToNumber( -1, "dwMinusExp" ) );

		m_mapBarunaNPCExpRandomOption.insert( MapBarunaNPCExp::value_type( dwUpgradeLevel, kBarunaNPCExp ) );

		kLua.Pop( 1 );
	}
	kLua.Pop( 0 );
	//////////////////////////////////////////////////////////////////////////

	return true;
}

DWORD	FLBarunaUpgradeNpc::GetExp( FLWSUser* pUser )
{
	DWORD dwExp = 0;

	const DWORD dwBarunaNpcItemObjid = pUser->GetBarunaNPCSummoner().GetNPCItemOBJID();
	
	FLItemElem* pBarunaNpcItem = pUser->m_Inventory.GetAtId( dwBarunaNpcItemObjid );
	if( IsUsableItem( pBarunaNpcItem ) == TRUE && pBarunaNpcItem->m_pPet != NULL )
	{
		dwExp = pBarunaNpcItem->m_pPet->GetExp();
	}

	return dwExp;
}

DWORD	FLBarunaUpgradeNpc::ExpToLevel( const DWORD dwExp )
{
	for( MapBarunaNPCLevelTbl::const_iterator it = m_tblBarunaNPCLevel.begin(); it != m_tblBarunaNPCLevel.end(); ++it )
	{
		const T_BARUNA_NPC_LEVEL & level = it->second;
		if( level.dwExpRequired >= dwExp )
		{
			return it->first;
		}
	}

	return m_tblBarunaNPCLevel.size();				//최고 레벨 리턴
}

DWORD	FLBarunaUpgradeNpc::GetEachExp( const DWORD dwLevel ) const
{
	MapBarunaNPCLevelTbl::const_iterator it = m_tblBarunaNPCLevel.find( dwLevel );
	if( it != m_tblBarunaNPCLevel.end() )
	{
		const T_BARUNA_NPC_LEVEL & level = it->second;

		return level.dwEachExp;
	}

	return 0;
}

DWORD	FLBarunaUpgradeNpc::GetExpRequired(  const DWORD dwLevel  ) const
{
	MapBarunaNPCLevelTbl::const_iterator it = m_tblBarunaNPCLevel.find( dwLevel );
	if( it != m_tblBarunaNPCLevel.end() )
	{
		const T_BARUNA_NPC_LEVEL & level = it->second;

		return level.dwExpRequired;
	}

	return 0;
}

DWORD	FLBarunaUpgradeNpc::GetAdditionalSuccessProb( FLWSUser* pUser )
{
	DWORD dwAdditionalSuccessProp = 0;

	if( IsValidObj( pUser ) == TRUE )
	{
		const DWORD dwBarunaNpcExp	= GetExp( pUser );

		const DWORD dwLevel			= ExpToLevel( dwBarunaNpcExp );

		MapBarunaNPCLevelTbl::const_iterator it = m_tblBarunaNPCLevel.find( dwLevel );
		if( it != m_tblBarunaNPCLevel.end() )
		{
			dwAdditionalSuccessProp = it->second.dwAdditionalSuccessProp;
		}
	}

	return dwAdditionalSuccessProp;
}

void	FLBarunaUpgradeNpc::AddExpToBarunaNPC( FLWSUser* pUser, DWORD dwUpgradeType, DWORD dwUpgradeLevel, DWORD dwUpgradeResult )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	const DWORD dwBarunaNpcItemObjid	= pUser->GetBarunaNPCSummoner().GetNPCItemOBJID();
	FLItemElem* pBarunaNpcItem			= pUser->m_Inventory.GetAtId( dwBarunaNpcItemObjid );

	if( IsUsableItem( pBarunaNpcItem ) == FALSE || pBarunaNpcItem->m_pPet == NULL )
	{
		return;
	}

	const DWORD dwBarunaNpcExp			= pBarunaNpcItem->m_pPet->GetExp();
	const DWORD dwBarunaNPCLevelOld		= pBarunaNpcItem->m_pPet->GetLevel();

	DWORD dwAddExp = 0;
	
	if( dwUpgradeType == IUTYPE_ENCHANT_GENERAL )
	{
		dwAddExp = GetAcquisitionExpEnchantGeneral( dwUpgradeLevel, dwUpgradeResult );
	}
	else if( dwUpgradeType == IUTYPE_ENCHANT_ATTRIBUTE )
	{
		dwAddExp = GetAcquisitionExpEnchantAttribute( dwUpgradeLevel, dwUpgradeResult );
	}
	else if( dwUpgradeType == IUTYPE_RANDOM_OPTION_EXTENSION )
	{
		dwAddExp = GetAcquisitionExpRandomOption( dwUpgradeLevel, dwUpgradeResult );
	}

	if( dwAddExp > 0 )
	{
		const DWORD dwNewExp		= min( GetLimitExp(), ( dwBarunaNpcExp + dwAddExp ) );

		pBarunaNpcItem->m_pPet->SetExp( dwNewExp );
		pBarunaNpcItem->m_pPet->SetLevel( static_cast<BYTE>( ExpToLevel(  pBarunaNpcItem->m_pPet->GetExp() ) ) );
		if( dwBarunaNPCLevelOld != pBarunaNpcItem->m_pPet->GetLevel() )
		{
			g_dpDBClient.CalluspPetLog( pUser->m_idPlayer, pBarunaNpcItem->GetSerialNumber(), 0, PETLOGTYPE_LEVELUP, pBarunaNpcItem->m_pPet );
		}

		pUser->AddBarunaNPCInfo( /*dwBarunaNPCLevelOld*/ );
	}
}

bool	FLBarunaUpgradeNpc::IsValidStateUpgradeNPC( FLWSUser* pUser, const DWORD dwUpgradeNpcObjID )
{
	if( pUser->GetBarunaNPCSummoner().GetNPCOBJID() == NULL_ID || pUser->GetBarunaNPCSummoner().GetNPCOBJID() != dwUpgradeNpcObjID )
	{
		pUser->AddDefinedText( TID_GAME_CALLED_NPC_IS_NOT_MINE );
		return false;
	}

	CMover* pBarunaNpc = prj.GetMover( dwUpgradeNpcObjID );
	if( pBarunaNpc == NULL )
	{
		pUser->AddDefinedText( TID_GAME_CALLED_NPC_IS_NOT_MINE );
		return false;
	}

	const D3DXVECTOR3 vDist	= pUser->GetPos() - pBarunaNpc->GetPos();
	if( fabs( (double)D3DXVec3LengthSq( &vDist ) ) > MAX_LEN_MOVER_MENU )
	{
		pUser->AddDefinedText( TID_GAME_CALLED_NPC_IS_NOT_MINE );
		return false;
	}

	FLItemElem* pBarunaNpcItem = pUser->m_Inventory.GetAtId( pUser->GetBarunaNPCSummoner().GetNPCItemOBJID() );
	if( pBarunaNpcItem == NULL || pBarunaNpcItem->m_pPet == NULL )
	{
		pUser->AddDefinedText( TID_GAME_CALLED_NPC_IS_NOT_MINE );
		return false;
	}

	return true;
}

DWORD	FLBarunaUpgradeNpc::GetAcquisitionExpEnchantGeneral( DWORD dwUpgradeLevel, DWORD dwUpgradeResult )
{
	DWORD dwAcquisitionExp			= 0;

	BarunaNPCExpItr it = m_mapBarunaNPCExpEnchantGeneral.find( dwUpgradeLevel );
	if( it != m_mapBarunaNPCExpEnchantGeneral.end() )
	{
		T_BARUNA_NPC_EXP kBarunaNPCExp = it->second;

		switch( dwUpgradeResult )
		{
		case FSC_ITEMUPGRADE_UPGRADE_SUCCESS:
			{
				dwAcquisitionExp = kBarunaNPCExp.dwSuccessExp;
			}
			break;

		case FSC_ITEMUPGRADE_UPGRADE_FAIL:
			{
				dwAcquisitionExp = kBarunaNPCExp.dwFailExp;
			}
			break;

		case FSC_ITEMUPGRADE_UPGRADE_MINUS:
			{
				dwAcquisitionExp = kBarunaNPCExp.dwMinusExp;
			}
			break;

		default:
			break;
		}
	}

	return dwAcquisitionExp;
}

DWORD	FLBarunaUpgradeNpc::GetAcquisitionExpEnchantAttribute( DWORD dwUpgradeLevel, DWORD dwUpgradeResult )
{
	DWORD dwAcquisitionExp			= 0;

	BarunaNPCExpItr it = m_mapBarunaNPCExpEnchantAttribute.find( dwUpgradeLevel );
	if( it != m_mapBarunaNPCExpEnchantAttribute.end() )
	{
		T_BARUNA_NPC_EXP kBarunaNPCExp = it->second;

		switch( dwUpgradeResult )
		{
		case FSC_ITEMUPGRADE_UPGRADE_SUCCESS:
			{
				dwAcquisitionExp = kBarunaNPCExp.dwSuccessExp;
			}
			break;

		case FSC_ITEMUPGRADE_UPGRADE_FAIL:
			{
				dwAcquisitionExp = kBarunaNPCExp.dwFailExp;
			}
			break;

		case FSC_ITEMUPGRADE_UPGRADE_MINUS:
			{
				dwAcquisitionExp = kBarunaNPCExp.dwMinusExp;
			}
			break;

		default:
			break;
		}
	}

	return dwAcquisitionExp;
}

DWORD	FLBarunaUpgradeNpc::GetAcquisitionExpRandomOption( DWORD dwUpgradeLevel, DWORD dwUpgradeResult )
{
	DWORD dwAcquisitionExp			= 0;

	BarunaNPCExpItr it = m_mapBarunaNPCExpRandomOption.find( dwUpgradeLevel );
	if( it != m_mapBarunaNPCExpRandomOption.end() )
	{
		T_BARUNA_NPC_EXP kBarunaNPCExp = it->second;

		switch( dwUpgradeResult )
		{
		case FSC_ITEMUPGRADE_UPGRADE_SUCCESS:
			{
				dwAcquisitionExp = kBarunaNPCExp.dwSuccessExp;
			}
			break;

		case FSC_ITEMUPGRADE_UPGRADE_FAIL:
			{
				dwAcquisitionExp = kBarunaNPCExp.dwFailExp;
			}
			break;

		case FSC_ITEMUPGRADE_UPGRADE_MINUS:
			{
				dwAcquisitionExp = kBarunaNPCExp.dwMinusExp;
			}
			break;

		default:
			{
				dwAcquisitionExp = 0;
			}
			break;
		}
	}

	return dwAcquisitionExp;
}

