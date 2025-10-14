#include "stdafx.h"

#if defined __WORLDSERVER && defined __AGGRO16

#include "FLAggro.h"
#include "lord.h"

#include "../WorldServer/User.h"


#include <limits>

#undef max

FLAggroProp::FLAggroProp()
{
	//memset( &m_Data, 0, sizeof( m_Data ) );
}

const FLAggroProp::FLData *		FLAggroProp::GetProp( const DWORD dwAggroID )
{
	PropDataMap::const_iterator it	= m_mapPropData.find( dwAggroID );
	if( it == m_mapPropData.end() )
		return NULL;

	return it->second;
}

DWORD FLAggroProp::StringToAggroID( const char * szAggroName )
{
	StringToID::const_iterator it = m_mapStringToID.find( std::string( szAggroName ) );
	if( it == m_mapStringToID.end() )
		return INVALID_AGGRO_ID;
	return it->second;
}

BOOL FLAggroProp::Load()
{
	const char * szFileName = "propAggro.txt";

	CScript script;
	if( script.Load( szFileName ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ NOT EXIST : (%s) ]" ), szFileName );
		return FALSE;
	}

	static DWORD s_dwAggroID		= 0;

	while( script.tok != FINISHED )
	{
		// ContributionID
		script.GetToken();
		if( script.Token == "" )
			continue;

		++s_dwAggroID;
		const std::string strAggroName	= script.Token.GetString();
			
		script.GetToken();		//skip next
		if( script.Token != "{" )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID TOKEN : pos({) (%s) ]" ), script.Token );
			return FALSE;
		}

        FLAggroProp::FLData * pData		= new FLAggroProp::FLData( s_dwAggroID, strAggroName.c_str() );
		for( ;; )
		{
			//완료 }
			script.GetToken();		//skip next
			if( script.Token == "}" )
				break;

			if( script.Token == "BUFF_AGGRO_RATE" )
			{
				script.GetToken();												//"="
				RELEASE_ASSERT( script.Token == "=" );
				pData->nBuffAggroRate	= script.GetNumber();
			}
			else if( script.Token == "DEBUFF_AGGRO_RATE" )
			{
				script.GetToken();												//"="
				RELEASE_ASSERT( script.Token == "=" );
				pData->nDebuffAggroRate	= script.GetNumber();
			}
			else if( script.Token == "FIRST_ATTACK_AGGRO_RATE" )
			{
				script.GetToken();												//"="
				RELEASE_ASSERT( script.Token == "=" );
				pData->nFirstAttackAggroRate	= script.GetNumber();
			}
			else if( script.Token == "FIRST_TARGET_AGGRO_RATE" )
			{
				script.GetToken();												//"="
				RELEASE_ASSERT( script.Token == "=" );
				pData->nFirstTargetAggroRate	= script.GetNumber();
			}
			else if( script.Token == "HEAL_RATE" )
			{
				script.GetToken();												//"="
				RELEASE_ASSERT( script.Token == "=" );
				pData->nHealRate				= script.GetNumber();
			}

			// dice 
			else if( script.Token == "SINGLE_HEAL_DICE" )
			{
				script.GetToken();												//"="
				RELEASE_ASSERT( script.Token == "=" );
				pData->nSingleHealDice			= script.GetNumber();
			}
			else if( script.Token == "GROUP_HEAL_DICE" )
			{
				script.GetToken();												//"="
				RELEASE_ASSERT( script.Token == "=" );
				pData->nGroupHealDice			= script.GetNumber();
			}
			else if( script.Token == "OVER_HEAL_ADDISION_DICE" )
			{
				script.GetToken();												//"="
				RELEASE_ASSERT( script.Token == "=" );
				pData->nOverHealAdditionDice	= script.GetNumber();
			}

			//skill
			else if( script.Token == "SI_LOD_SUP_ANGER" )
			{
				script.GetToken();												//"="
				RELEASE_ASSERT( script.Token == "=" );
				pData->nLodSupAnger				= script.GetNumber();
			}
			else if( script.Token == "SI_LOD_SUP_PULLING" )
			{
				script.GetToken();												//"="
				RELEASE_ASSERT( script.Token == "=" );
				pData->nLodSupPulling				= script.GetNumber();
			}
			else if( script.Token == "NEXT_TARET_AGGRO_RATE" )
			{
				script.GetToken();												//"="
				RELEASE_ASSERT( script.Token == "=" );
				pData->nNextTargetAggroRate			= script.GetNumber();
			}
		}

		//insert
		if( m_mapStringToID.insert( StringToID::value_type( strAggroName, s_dwAggroID ) ).second == false )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ DUPLICATE AGGRO STRING(%d)(%s) ]" ), s_dwAggroID, strAggroName.c_str() );
		}
		if( m_mapPropData.insert( PropDataMap::value_type( s_dwAggroID, pData ) ).second == false )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ DUPLICATE AGGRO STRING(%d)(%s) ]" ), s_dwAggroID, strAggroName.c_str() );
			SAFE_DELETE( pData );
		}
	}

	return TRUE;

	//FLIniFile ini( _T( "propAggro.txt" ) );

	//if( ini.IniReadInt( _T( "PROPERTY" ), _T( "BUFF_AGGRO_RATE" ), m_Data.nBuffAggroRate ) == FALSE ||
	//	ini.IniReadInt( _T( "PROPERTY" ), _T( "DEBUFF_AGGRO_RATE" ), m_Data.nDebuffAggroRate ) == FALSE ||
	//	ini.IniReadInt( _T( "PROPERTY" ), _T( "FIRST_ATTACK_AGGRO_RATE" ), m_Data.nFirstAttackAggroRate ) == FALSE ||
	//	ini.IniReadInt( _T( "PROPERTY" ), _T( "FIRST_TARGET_AGGRO_RATE" ), m_Data.nFirstTargetAggroRate ) == FALSE ||
	//	ini.IniReadInt( _T( "PROPERTY" ), _T( "HEAL_RATE" ), m_Data.nHealRate ) == FALSE ||
	//	ini.IniReadInt( _T( "PROPERTY" ), _T( "SINGLE_HEAL_DICE" ), m_Data.nSingleHealDice ) == FALSE ||
	//	ini.IniReadInt( _T( "PROPERTY" ), _T( "GROUP_HEAL_DICE" ), m_Data.nGroupHealDice ) == FALSE ||
	//	ini.IniReadInt( _T( "PROPERTY" ), _T( "OVER_HEAL_ADDISION_DICE" ), m_Data.nOverHealAdditionDice ) == FALSE ||
	//	ini.IniReadInt( _T( "PROPERTY" ), _T( "SI_LOD_SUP_ANGER" ), m_Data.nLodSupAnger ) == FALSE ||
	//	ini.IniReadInt( _T( "PROPERTY" ), _T( "SI_LOD_SUP_PULLING" ), m_Data.nLodSupPulling ) == FALSE )
	//	return FALSE;
	//return TRUE;

	
	


}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//


FLAggroProp::FLData::FLData( const DWORD dwAggroID , const char * szAggroName ) : m_dwAggroID( dwAggroID ), m_strAggroName( szAggroName )
{
	nBuffAggroRate			= 0;
	nDebuffAggroRate		= 0;
	nFirstAttackAggroRate	= 0;
	nFirstTargetAggroRate	= 0;

	nHealRate				= 0;

	nSingleHealDice			= 0;
	nGroupHealDice			= 0;
	nOverHealAdditionDice	= 0;

	nLodSupAnger			= 0;
	nLodSupPulling			= 0;

	nNextTargetAggroRate	= 0;
}

FLAggroProp::FLData::~FLData()
{

}



//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//


FLAggro::FLAggro( const DWORD dwAggroID )//, const char * szAggroName )
: m_dwAggroID( dwAggroID )
//, m_strAggroName( szAggroName )
, m_OwnerOBJID( NULL_ID ), m_nFirstTargetObjID( NULL_ID ), m_nFirstAttackerObjID( NULL_ID )
{
}

FLAggro::~FLAggro()
{
	Clear();
}

void		FLAggro::Initialize( const OBJID ownerOBJID )
{
	FLASSERT( ownerOBJID != NULL_ID && ownerOBJID != 0 );	///이상한 ID들어오면 안됨.		//&& m_OwnerOBJID == NULL_ID ); //이상한 ID들어오면 안됨. 한번만 초기화 되어야함.
	Clear();

	m_OwnerOBJID				= ownerOBJID;
}

OBJID FLAggro::GetOwnerObjID() const
{
	return m_OwnerOBJID;
}

void  FLAggro::SumBuffAggro( CMover & Behavior, CMover & Target )
{
	if( IsValidObj( &Behavior ) == FALSE || IsValidObj( &Target ) == FALSE || Behavior.IsDie() || Target.IsDie() )
	{
		return;
	}


	//buff
	for( FLAggro::MapAggroOwner::iterator it = Target.m_AggroOwnerList.begin(); it != Target.m_AggroOwnerList.end(); ++it )
	{
		CMover * pMonster	= prj.GetMover( it->first );		
		if( IsValidObj( pMonster ) == FALSE )					
			continue;

		if( pMonster->GetAggroMng() == NULL )
			continue;

		const FLAggroProp::FLData * pPropData = AGGRO_PROP().GetProp( pMonster->GetAggroMng()->m_dwAggroID );
		if( pPropData == NULL )
			continue;

	//	if( g_eLocal.GetState( EVE_STEAL ) == FALSE && Behavior.IsSteal( pMonster ) == TRUE )
	//		continue;

		const UINT64 u64Aggro	= ( UINT64 )( pMonster->GetMaxOriginHitPoint() * ( pPropData->nBuffAggroRate / 100.0f ) * AGGRO_UNIT_VALUE );
		SumAggro( *pMonster, Behavior, u64Aggro );
	}
}

void  FLAggro::SumDebuffAggro( CMover & Behavior, CMover & Target )
{
	const FLAggroProp::FLData * pPropData = AGGRO_PROP().GetProp( m_dwAggroID );
	if( pPropData == NULL )
		return;

	// debuff : 
	const UINT64 u64Aggro		= ( UINT64 )( Target.GetMaxOriginHitPoint() * ( pPropData->nDebuffAggroRate / 100.0f ) * AGGRO_UNIT_VALUE );	
	SumAggro( Target, Behavior, u64Aggro );
	return;
}

void  FLAggro::SumHealAggro( CMover & Healer, CMover & Target, const int nOverHeal, const DWORD dwExeTarget )
{
	if( IsValidObj( &Healer ) == FALSE || IsValidObj( &Target ) == FALSE || Healer.IsDie() || Target.IsDie() )
	{
		return;
	}


	for( FLAggro::MapAggroOwner::iterator it = Target.m_AggroOwnerList.begin(); it != Target.m_AggroOwnerList.end(); ++it )
	{
		// 몬스터 체크
		CMover * pMonster	= prj.GetMover( it->first );		
		if( IsValidObj( pMonster ) == FALSE )									
			continue;

		// AggroMng 찾기
		FLAggro * pAggro		= pMonster->GetAggroMng();
		if( pAggro == NULL )
			return;

		const FLAggroProp::FLData * pPropData = AGGRO_PROP().GetProp( pAggro->m_dwAggroID );
		if( pPropData == NULL )
			continue;

		// add aggro
		int nAggroRate			= ( nOverHeal > 0 ) ? pPropData->nOverHealAdditionDice : 0;
		//int nAggro				= 0;
		UINT64 u64Aggro				= 0;
		switch( dwExeTarget )
		{
			// 단일 힐
		case EXT_SELFCHGPARAMET:	case EXT_ANOTHER:		case EXT_ANOTHERWITH:
			{
				if( ( int )xRandom( 100 ) < ( pPropData->nSingleHealDice + nAggroRate ) )
					u64Aggro		= ( UINT64 )( pAggro->getMaxAggro() + ( pMonster->GetMaxOriginHitPoint() * ( pPropData->nHealRate / 100.0f  ) * AGGRO_UNIT_VALUE ) );
			}
			break;
			// 그룹 힐
		case EXT_TROUPE:			case EXT_TROUPEWITH:
			{
				if( ( int )xRandom( 100 ) < ( pPropData->nGroupHealDice + nAggroRate ) )
					u64Aggro		= ( UINT64 )( pAggro->getMaxAggro() + ( pMonster->GetMaxOriginHitPoint() * ( pPropData->nHealRate / 100.0f ) * AGGRO_UNIT_VALUE ) );
			}
			break;

		default:
			//FLERROR_LOG( PROGRAM_NAME, _T( "[ SumHealAggro : INVALID dwExeTarget(%d) ]" ), dwExeTarget );
			return;
		}

//		SumAggro( *pMonster, Healer, nAggro );

		//sumAggro_autoLink( *pMonster, Healer, ( nAggro == 0 ) ? 1 : nAggro );
		sumAggro_autoLink( *pMonster, Healer, u64Aggro );
	}

}

BOOL FLAggro::SumAttackAggro( CMover & Monster, const OBJID nAttackerObjid, const int nDamage )
{
	const FLAggroProp::FLData * pPropData = AGGRO_PROP().GetProp( m_dwAggroID );
	if( pPropData == NULL )
		return FALSE;

	return SumAggro( Monster, nAttackerObjid, nDamage * AGGRO_UNIT_VALUE );
}

BOOL FLAggro::SumFirstAttackerAggro( CMover & Monster, const OBJID nAttackerObjid )
{
	//// 몬스터 체크
	if( IsValidObj( & Monster ) == FALSE || nAttackerObjid == NULL_ID )		
	{
		return FALSE;
	}

	if( Monster.IsDie() )//|| Monster.IsMonster() != TRUE )
	{
		return FALSE;
	}

	const FLAggroProp::FLData * pPropData = AGGRO_PROP().GetProp( m_dwAggroID );
	if( pPropData == NULL )
		return FALSE;


	// aggro 찾기
	FLAggro * pAggro		= Monster.GetAggroMng();
	if( pAggro == NULL )
		return FALSE;

	// add aggro
	if( pAggro->m_nFirstAttackerObjID != NULL_ID )
	{
		return FALSE;
	}

	//if( pAggro->m_nFirstTargetObjID != NULL_ID )
	//{
	//	return FALSE;
	//}

	// add aggro
	const UINT64 u64Aggro			= ( UINT64 )( Monster.GetMaxOriginHitPoint() * ( pPropData->nFirstAttackAggroRate / 100.0f ) * AGGRO_UNIT_VALUE );
	if( SumAggro( Monster, nAttackerObjid, u64Aggro ) == FALSE )
	{
		return FALSE;
	}

	pAggro->m_nFirstAttackerObjID	= nAttackerObjid;
	return TRUE;
}

BOOL FLAggro::SumFirstTargetAggro( CMover & Monster, const OBJID nTargetObjid )
{
	// 몬스터 체크
	if( IsValidObj( & Monster ) == FALSE || nTargetObjid == NULL_ID )		
		return FALSE;

	if( Monster.IsDie() )//|| Monster.IsMonster() != TRUE )// || Monster.m_pAIInterface == NULL )
		return FALSE;

	const FLAggroProp::FLData * pPropData = AGGRO_PROP().GetProp( m_dwAggroID );
	if( pPropData == NULL )
		return FALSE;


	// aggro manager 찾기
	FLAggro * pAggro		= Monster.GetAggroMng();
	if( pAggro == NULL)
		return FALSE;

	// add aggro
	if( pAggro->m_nFirstTargetObjID != NULL_ID )
		return FALSE;

	if( pAggro->m_nFirstAttackerObjID != NULL_ID )
		return FALSE;
	//
	const UINT64 u64Aggro			= ( UINT64 )( Monster.GetMaxOriginHitPoint() * ( pPropData->nFirstTargetAggroRate / 100.0f ) * AGGRO_UNIT_VALUE );
	if( SumAggro( Monster, nTargetObjid, u64Aggro ) == FALSE )
		return FALSE;
	
	pAggro->m_nFirstTargetObjID		= nTargetObjid;
	return TRUE;
}

BOOL FLAggro::SumLodSupAnger( CMover & Monster, CMover & Player )
{
	const FLAggroProp::FLData * pPropData = AGGRO_PROP().GetProp( m_dwAggroID );
	if( pPropData == NULL )
		return FALSE;

	const UINT64 u64Aggro		= ( UINT64 )( Monster.GetMaxHitPoint() * ( pPropData->nLodSupAnger / 100.0f ) * AGGRO_UNIT_VALUE );
	SumAggro( Monster, Player, u64Aggro );	
	return TRUE;
}

BOOL FLAggro::SumLodSupPulling( CMover & Monster, CMover & Player )
{
	const FLAggroProp::FLData * pPropData = AGGRO_PROP().GetProp( m_dwAggroID );
	if( pPropData == NULL )
		return FALSE;

	const UINT64 u64Aggro		= ( UINT64 )( Monster.GetMaxHitPoint() * ( pPropData->nLodSupPulling / 100.0f ) * AGGRO_UNIT_VALUE );
	SumAggro( Monster, Player, u64Aggro );
	return TRUE;
}

BOOL FLAggro::SumNextTagerAggro( CMover & Monster, CMover & ValidTarget )
{
	const FLAggroProp::FLData * pPropData = AGGRO_PROP().GetProp( m_dwAggroID );
	if( pPropData == NULL )
		return FALSE;

	const UINT64 u64Aggro		= ( int )( Monster.GetMaxHitPoint() * ( pPropData->nNextTargetAggroRate / 100.0f ) ) * AGGRO_UNIT_VALUE;
	return SumAggro( Monster, ValidTarget, u64Aggro );	
}



BOOL FLAggro::SumAggro( CMover & Monster, CMover & Player, const UINT64 u64Aggro )
{
	// 몬스터 체크
	if( IsValidObj( & Monster ) == FALSE || IsValidObj( & Player ) == FALSE )		
		return FALSE;

	if( Monster.IsDie() )//|| Monster.IsMonster() != TRUE )//|| Monster.m_pAIInterface == NULL )
		return FALSE;

	// aggro manager
	FLAggro * pAggro		= Monster.GetAggroMng();
	if( pAggro == NULL)
		return FALSE;

	// add aggro
	if( sumAggro_autoLink( Monster, Player, u64Aggro ) == FALSE )
		return FALSE;

	return TRUE;
}

BOOL FLAggro::SumAggro( CMover & Monster, const OBJID objid, const UINT64 u64Aggro )
{
	if( objid == NULL_ID )			
		return FALSE;

	CMover * pTarget			= prj.GetMover( objid );
	if( IsValidObj( pTarget ) == FALSE )
		return FALSE;

	return SumAggro( Monster, *pTarget, u64Aggro );
}



BOOL FLAggro::sumAggro_autoLink( CMover & Monster, CMover & Player, const UINT64 u64Aggro )
{
	if( Monster.IsDie() )//||	Monster.IsMonster() == FALSE )//|| Monster.m_pAIInterface == NULL )
	{
		return FALSE;
	}

	FLAggro * pAggro		= Monster.GetAggroMng();
	if( pAggro == NULL)
		return FALSE;

	// add aggro
	if( pAggro->sumAggro_autoInsert( Player.GetId(), u64Aggro ) == FALSE )
		return FALSE;

	Player.m_AggroOwnerList.insert( FLAggro::MapAggroOwner::value_type( Monster.GetId(), 0 ) );
	return TRUE;
}


void FLAggro::RemoveMover( /*CMover & Monster, */const OBJID objid )
{
	// remove
	if( removeMover( objid ) == TRUE )
	{
		CMover * pPlayer		= prj.GetMover( objid );
		if( pPlayer != NULL )
		{
			pPlayer->m_AggroOwnerList.erase( m_OwnerOBJID );
		}
	}
}


void FLAggro::Clear()
{
	CMover * pOwner			= prj.GetMover( m_OwnerOBJID );
	if( pOwner == NULL )
	{
		return;
	}


	// 어그로 관련 AI인가 확인.
	FLAggro * pAggro		= pOwner->GetAggroMng();
	if( pAggro == NULL )
		return;

	// clear
	MapOBJIDToAggroValue & aggroList		= pAggro->m_OBJIDToAggroValueMap;
	for( MapOBJIDToAggroValue::const_iterator it = aggroList.begin(); it != aggroList.end(); ++it )
	{
		CMover * pPlayer		= prj.GetMover( it->first );
		if( pPlayer != NULL )
		{
			pPlayer->m_AggroOwnerList.erase( m_OwnerOBJID );
		}
	}

	m_nFirstTargetObjID		= NULL_ID;	
	m_nFirstAttackerObjID	= NULL_ID;

	aggroList.clear();
}


BOOL FLAggro::GetMaxAggro( OBJID & o_objid )
{
	MapOBJIDToAggroValue::const_iterator max_it = m_OBJIDToAggroValueMap.begin();
	if( max_it == m_OBJIDToAggroValueMap.end() )
	{
		o_objid						= NULL_ID;		
		return FALSE;
	}

	for( MapOBJIDToAggroValue::const_iterator it = m_OBJIDToAggroValueMap.begin(); it != m_OBJIDToAggroValueMap.end(); ++it )
	{
		if( max_it == it )
			continue;

		if( max_it->second < it->second )
			max_it					= it;
	}

	o_objid							= max_it->first;
	return TRUE;
}


size_t					
FLAggro::Size() const
{
	return m_OBJIDToAggroValueMap.size();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------//
UINT64 FLAggro::getMaxAggro() const
{
	MapOBJIDToAggroValue::const_iterator max_it = m_OBJIDToAggroValueMap.begin();
	if( max_it == m_OBJIDToAggroValueMap.end() )
	{
		return 0;
	}

	for( MapOBJIDToAggroValue::const_iterator it = m_OBJIDToAggroValueMap.begin(); it != m_OBJIDToAggroValueMap.end(); ++it )
	{
		if( max_it == it )
			continue;

		if( max_it->second < it->second )
			max_it					= it;
	}

	return max_it->second;
}


BOOL FLAggro::getAggro( const OBJID objid, UINT64 & o_u64Aggro )
{
	MapOBJIDToAggroValue::const_iterator it = m_OBJIDToAggroValueMap.find( objid );
	if( it == m_OBJIDToAggroValueMap.end() )
	{
		return FALSE;
	}

	o_u64Aggro						= it->second;
	return TRUE;
}


BOOL FLAggro::initAggro( const OBJID objid )
{
	MapOBJIDToAggroValue::iterator it = m_OBJIDToAggroValueMap.find( objid );
	if( it == m_OBJIDToAggroValueMap.end() )	
	{
		return FALSE;
	}

	it->second					= 0;
	return TRUE;
}


BOOL FLAggro::sumAggro_autoInsert( const OBJID objid, const UINT64 u64Aggro )
{	
	if( objid == NULL_ID )			
	{
		return FALSE;
	}

	if( findMover( objid ) == FALSE )
	{
		if( insertMover( objid ) == FALSE )
		{
			return FALSE;
		}
	}

	MapOBJIDToAggroValue::iterator it = m_OBJIDToAggroValueMap.find( objid );
	if( it == m_OBJIDToAggroValueMap.end() )
	{
		return FALSE;
	}

	UINT64 & u64Tot				= it->second;

	if( ( u64Tot + u64Aggro ) < u64Tot	)		//overflow인지 검사
		u64Tot					= std::numeric_limits< UINT64 >::max();
	else
		u64Tot					= u64Tot + u64Aggro;


	//어그로 모니터링 GM 명령
	CMover * pMover			= prj.GetMover( objid );
	if( IsValidObj( pMover ) == TRUE || pMover->IsPlayer() == TRUE )
	{
		FLWSUser * pUser			= static_cast< FLWSUser * >( pMover );
		if( pUser->m_bDisplayAggro == TRUE )
		{
			char szOut[ 1024 ] = { 0 };
			FLSPrintf( szOut, _countof( szOut ), "objid(%u) aggro(%I64u) tot(%I64u)", objid, u64Aggro, u64Tot );

			g_DPCoreClient.SendSetSnoop( pUser->m_idPlayer, pUser->m_idPlayer, FALSE );
			g_DPCoreClient.SendChat( pUser->m_idPlayer, pUser->m_idPlayer, szOut );
			g_DPCoreClient.SendSetSnoop( pUser->m_idPlayer, pUser->m_idPlayer, TRUE );
		}
	}
	return TRUE;
}



BOOL FLAggro::insertMover( const OBJID objid )
{
	if( m_OBJIDToAggroValueMap.insert( MapOBJIDToAggroValue::value_type( objid, 0 ) ).second == false )
		return FALSE;

	return TRUE;
}

BOOL FLAggro::removeMover( const OBJID objid )
{
	MapOBJIDToAggroValue::iterator it = m_OBJIDToAggroValueMap.find( objid );
	if( it == m_OBJIDToAggroValueMap.end() )
		return FALSE;

	m_OBJIDToAggroValueMap.erase( it );
	return TRUE;
}


BOOL FLAggro::findMover( const OBJID objid )
{
	MapOBJIDToAggroValue::iterator it = m_OBJIDToAggroValueMap.find( objid );
	if( it == m_OBJIDToAggroValueMap.end() )
		return FALSE;

	return TRUE;
}


void FLAggro::clear()
{
	m_OBJIDToAggroValueMap.clear();
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------//


#endif //__AGGRO16


