// InstanceDungeonBase.cpp: implementation of the CInstanceDungeonBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InstanceDungeonBase.h"

#ifdef __WORLDSERVER
#include "../WorldServer/User.h"
#include "worldmng.h"
#include "party.h"
#include "../worldserver/LinkMap.h"

#include "../_Common/Mover.h"
#include "../worldserver/FLItemUsing.h"

extern CWorldMng g_WorldMng;
extern	CPartyMng g_PartyMng;
#endif // __WORLDSERVER

#ifdef __CORESERVER
#include "../CoreServer/DPCoreSrvr.h"
extern CDPCoreSrvr g_dpCoreSrvr;
#endif // __CORESERVER

#include "../_CommonDefine/FlyffCommonDefine.h"


#undef max
#undef min


//////////////////////////////////////////////////////////////////////

INDUN_INFO::INDUN_INFO( const DWORD dwWI, const ULONG uMK, const DWORD dungeonID, const DWORD guildID, const DWORD dwDungeonLevel /*= DUNGEON_LEVEL_NORMAL */ ) 
: dwWorldId( dwWI ), uMultiKey( uMK ), dwDungeonID( dungeonID ), dwGuildID( guildID ), eDungeonLevel( dwDungeonLevel ) 
#ifdef __WORLDSERVER
, dwCreatedTick( 0 )
, nCurrStage( ID_NORMAL ), dwTimeLimitTick( 0 ), dwTurnaroundTime( 0 ), dwStartStageTick( 0 ), nPlayerCount( 0 ), nKillCount( 0 )
, m_pState( NULL ), bCreateAndEntered( FALSE ), bCallRealMonster( FALSE ), dw10000AdditionalRealMonsterRate( 0 )		//---- 콜로세움
, idLeaderPlayer( 0 )
, dwAddRealBossRate_ItemID( NULL_ID )
, dwAddRealBossRate_ItemObjID( NULL_ID )
, idPlayer_UseAddRealBossRateItem( 0 )
, bSuccess( FALSE )
#endif // __WORLDSERVER
{
#ifdef __WORLDSERVER
	tmStartedDate	= DATE_TIMER().GetCurrentTime();
#endif // __WORLDSERVER
}

void INDUN_INFO::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
		ar << dwWorldId << uMultiKey	<< dwGuildID << eDungeonLevel;
	else
		ar >> dwWorldId >> uMultiKey	>> dwGuildID >> eDungeonLevel;
}

#ifdef __WORLDSERVER
void INDUN_INFO::SetState( ITimeAlarmState * pState, const DWORD dwAlarmIntervalTick/*, const DWORD dwDungeonID*/ )
{
	if( IsValidState_( pState ) == FALSE )
	{
		m_pState			= NULL;
		return;
	}

	if( m_pState != NULL )
		m_pState->OnLeave( *( this )/*, dwDungeonID*/ );
	m_pState			= pState;
	if( m_pState != NULL )
	{
		m_pState->SetAlarmInterval( dwAlarmIntervalTick );
		m_pState->OnEnter( *( this )/*, dwDungeonID*/ );
	}
}

BOOL INDUN_INFO::IsRandomMonsterStage( const int nStage )
{
	PairMonsterVec & vec	= vec2DPairMonsterList.at( nStage );
	for( PairMonsterVec::const_iterator it = vec.begin(); it != vec.end(); ++it )
	{
		const PAIR_MONSTER & monster = *it;
		if( monster.bRandomMonster == FALSE )
			return FALSE;
	}

	return ( vec.size() == 0 ) ? FALSE : TRUE;
}

BOOL INDUN_INFO::IsValidState_( ITimeAlarmState * pState )
{
	if( pState == NULL 
		|| pState == &AutoStartState 
		|| pState == &StageWaitingState
		|| pState == &StageLimitState
		|| pState == &RetriedStageWaitingState
		|| pState == &CompleteAllWaitingState
		|| pState == &AllDieWaitingState
		|| pState == &ExpiredTimeWaitingState )
		return TRUE;

	//static int nSnappingMax = 5;
 //   if( nSnappingMax > 0)
	//{
	//	FLDumpInstaller::DumpNow( FLDUMP_LEVEL_LIGHT, 0, "", __FILE__, __LINE__, __FUNCTION__ );
	//	--nSnappingMax;
	//}

	FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID STATE (%x) : this(%x) ]" ), pState, this );
	return FALSE;
}
#endif //__WORLDSERVER




void COOLTIME_INFO::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
		ar << dwWorldId << dwDungeonId << ( dwCoolTime - GetTickCount() );
	else
	{
		ar >> dwWorldId >> dwDungeonId >> dwCoolTime;
		dwCoolTime += GetTickCount();
	}
}


//////////////////////////////////////////////////////////////////////
// CInstanceDungeonBase
//////////////////////////////////////////////////////////////////////
CInstanceDungeonBase::CInstanceDungeonBase( int nIDType )
: m_nIDType( nIDType )
#ifdef __CORESERVER
, m_nCoolTimeCount( 0 )
#endif // __CORESERVER
{
}

CInstanceDungeonBase::~CInstanceDungeonBase()
{
	m_mapID.clear();
	m_mapCTInfo.clear();
#ifdef __WORLDSERVER
	m_mapDungeonData.clear();
#endif // __WORLDSERVER
}

void CInstanceDungeonBase::SerializeAllInfo( CAr & ar )
{
	if( ar.IsStoring() )
	{
		// 던전 정보
		ar << m_mapID.size();
		for( MAP_IDBASE::iterator itMap=m_mapID.begin(); itMap!=m_mapID.end(); itMap++ )
		{
			ar << itMap->first;
			VEC_IDINFO* pvecTemp;
			pvecTemp = &itMap->second;
			ar << pvecTemp->size();
			for( VEC_IDINFO::iterator itVec=pvecTemp->begin(); itVec!=pvecTemp->end(); itVec++ )
			{
				//(*itVec).Serialize( ar );
				INDUN_INFO * pInfo = ( *itVec );
				if( pInfo != NULL )
					pInfo->Serialize( ar );
			}
		}

		// 플레이어 쿨타임 정보
		ar << m_mapCTInfo.size();
		for( MAP_CTINFO::iterator it=m_mapCTInfo.begin(); it!=m_mapCTInfo.end(); it++)
		{
			ar << it->first;
			VEC_CTINFO* pvecIDCT;
			pvecIDCT = &it->second;
			ar << pvecIDCT->size();
			for( VEC_CTINFO::iterator itVec=pvecIDCT->begin(); itVec!=pvecIDCT->end(); itVec++)
			{
				(*itVec).Serialize( ar );
			}
		}
	}
	else
	{
		DWORD	i = 0;
		// 던전 정보
		m_mapID.clear();
		size_t nSizeMap;
		ar >> nSizeMap;
		for( i=0; i<nSizeMap; i++ )
		{
			DWORD dwDungeonId;	ar >> dwDungeonId;
			size_t nSizeVec;	ar >> nSizeVec;
			for( DWORD j=0; j<nSizeVec; j++ )
			{
				INDUN_INFO ID_Info( NULL_ID, NULL_ID, dwDungeonId );
				ID_Info.Serialize( ar );
				CreateDungeon( dwDungeonId, ID_Info.dwWorldId, ID_Info.uMultiKey, ID_Info.dwGuildID, ID_Info.eDungeonLevel );
				//DWORD dwWorldId;
				//ULONG uMultiKey;
				//DWORD dwGuildID;
				//int eDungeonLevel;
				//ar >> dwWorldId >> uMultiKey >> dwGuildID >> eDungeonLevel;
				//CreateDungeon( dwDungeonId, dwWorldId, uMultiKey, dwGuildID, eDungeonLevel );
			}
		}

		// 플레이어 쿨타임 정보
		m_mapCTInfo.clear();
		ar >> nSizeMap;
		for( i=0; i<nSizeMap; i++ )
		{
			DWORD dwPlayerId;	ar >> dwPlayerId;
			size_t nSizeVec;	ar >> nSizeVec;
			for( DWORD j=0; j<nSizeVec; j++ )
			{
				COOLTIME_INFO CT_Info;
				CT_Info.Serialize( ar );
				SetDungeonCoolTimeInfo( CT_Info, dwPlayerId );
			}
		}
	}
}


#ifdef __WORLDSERVER
BOOL CInstanceDungeonBase::CreateDungeonLayer( INDUN_INFO & ID_Info, DWORD dwLayer
											//  , DWORD eDungeonLevel
											  )
{

	if( ID_Info.uMultiKey != g_uKey )
		return FALSE;	///////

	CWorld* pWorld = g_WorldMng.GetWorld( ID_Info.dwWorldId );
	if( !pWorld || pWorld->m_linkMap.GetLinkMap( static_cast<int>( dwLayer ) ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "LinkMap Exist! Type:%d, DungeonID:%d, WorldID:%d" ), GetType(), dwLayer, ID_Info.dwWorldId );
		return FALSE;
	}

	if( !pWorld->CreateLayer( static_cast<int>( dwLayer ) ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CreateLayer Failed! Type:%d, DungeonID:%d, WorldID:%d" ), GetType(), dwLayer, ID_Info.dwWorldId );
		return FALSE;
	}

	MAP_IDDATA::const_iterator it = m_mapDungeonData.find( ID_Info.dwWorldId );
	if( it == m_mapDungeonData.end() )
	{
		pWorld->ReleaseLayer( static_cast<int>( dwLayer ) );
		FLERROR_LOG( PROGRAM_NAME, _T( "INVALID DUNGEON DATA Type:%d, DungeonID:%d, WorldID:%d" ), GetType(), dwLayer, ID_Info.dwWorldId );
		return FALSE;
	}

	const CInstanceDungeonBase::DUNGEON_DATA & dungeonData = it->second;

	g_dpDBClient.SendLogInstanceDungeon( dwLayer, ID_Info.dwWorldId, g_uIdofMulti, GetType(), 'M' );
	//FLTRACE_LOG( PROGRAM_NAME, _T( "[ World : %d, Layer : %d ]" ), ID_Info.dwWorldId, dwLayer );
	INDUN_INFO* pInfo = GetDungeonInfo( dwLayer, ID_Info.dwWorldId );	
	if( pInfo == NULL )			//neodeath : add
	{
		pWorld->ReleaseLayer( static_cast<int>( dwLayer ) );
		FLERROR_LOG( PROGRAM_NAME, _T( "MUST NOT NULL : Type:%d, DungeonID:%d, WorldID:%d" ), GetType(), dwLayer, ID_Info.dwWorldId );
		return FALSE;
	}

	if( pInfo->eDungeonLevel >= DUNGEON_LEVEL_MAX )
	{
		pWorld->ReleaseLayer( static_cast<int>( dwLayer ) );
		FLERROR_LOG( PROGRAM_NAME, _T( "OVER DUNGEON LEVEL : Type:%d, DungeonID:%d, WorldID:%d" ), GetType(), dwLayer, ID_Info.dwWorldId );
		return FALSE;
	}

	//Generate Monster Table
	const DUNGEON_DATA::FLLevelProp & levelProp = dungeonData.arrLevelProp[ pInfo->eDungeonLevel ];
	pInfo->vec2DPairMonsterList	= levelProp.vec2DPairMonster;						//copy
		
	for( PairMonsterVec::const_iterator it = levelProp.vecRandomMonster.begin()	//random 적용
		; it != levelProp.vecRandomMonster.end()
		; ++it )
	{
		const size_t nRandom	= xRandom( pInfo->vec2DPairMonsterList.size() + 1 );
		const PAIR_MONSTER & randomMonster = *it;
		//randomMonster.nStage	= nRandom;

		PairMonsterVec2D::iterator it2D = pInfo->vec2DPairMonsterList.insert( pInfo->vec2DPairMonsterList.begin() + nRandom, PairMonsterVec() );
		PairMonsterVec & vec			= *it2D;
		vec.push_back( randomMonster );
	}
		
	//skip stage
//	for( PairMonsterVec2D::const_iterator it = pInfo->vec2DPairMonsterList.begin(); it != pInfo->vec2DPairMonsterList.end(); ++it )
//	{
//		const PairMonsterVec & vec	= *it;
//		if( ( vec.size() == 0 ) && ( pInfo->nCurrStage < GetObjCount_CurrStage( ID_Info.dwWorldId, dwLayer ) ) )
//			++pInfo->nCurrStage;
//	}
	
	if( dungeonData.bColosseumStyle == TRUE )
	{
		//ID_Info.AutoStartState.SetAlarmInterval( dungeonData.arrLevelProp[ ID_Info.eDungeonLevel ].dwAutoStart_WaitingTick );	//바로 시작
		ID_Info.SetState( &ID_Info.AutoStartState, dungeonData.arrLevelProp[ ID_Info.eDungeonLevel ].dwAutoStart_WaitingTick/*, dwLayer*/ );
	}
	else
	{		//처음 시작되는 스테이지까지 스킵
		while( GetObjCount_CurrStage( pInfo->dwWorldId, dwLayer ) <= 0 && pInfo->nCurrStage < ( dungeonData.arrLevelProp[ pInfo->eDungeonLevel ].nMaxStage )  )//&& pInfo->nStage < ID_BOSS )		
			pInfo->nCurrStage++;

		if( pInfo->nCurrStage >= dungeonData.arrLevelProp[ pInfo->eDungeonLevel ].nMaxStage )
		{
			++CInstanceDungeonHelper::GetInstance()->m_nIDLayerNum;
		//	pWorld->ReleaseLayer( static_cast<int>( dwLayer ) );
		//	FLERROR_LOG( PROGRAM_NAME, _T( "[ null stage : WorldID(%u) ]" ), pInfo->dwWorldId );
			return TRUE;
		}

		RespawnMonster_AndTrySwitchToStageLimit( dwLayer, ID_Info.dwWorldId );
	}
	++CInstanceDungeonHelper::GetInstance()->m_nIDLayerNum;
	return TRUE;
}
#else
BOOL CInstanceDungeonBase::CreateDungeonLayer( INDUN_INFO & /*ID_Info*/, DWORD /*dwLayer*/ )
{
	return TRUE;
}
#endif // __WORLDSERVER

#ifdef __WORLDSERVER
BOOL CInstanceDungeonBase::DestroyDungeonLayer( const INDUN_INFO & ID_Info, DWORD dwLayer )
{
	//if( ID_Info.uMultiKey != g_uKey )
	//	return TRUE;

	CWorld* pWorld = g_WorldMng.GetWorld( ID_Info.dwWorldId );
	if( !pWorld || !pWorld->m_linkMap.GetLinkMap( static_cast<int>( dwLayer ) ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "LinkMap Not Exist! Type:%d, DungeonID:%d, WorldID:%d" ), GetType(), dwLayer, ID_Info.dwWorldId );
		return FALSE;
	}

	pWorld->Invalidate( static_cast<int>( dwLayer ) );

	--CInstanceDungeonHelper::GetInstance()->m_nIDLayerNum;

	if( ID_Info.bSuccess == FALSE )
		CInstanceDungeonHelper::GetInstance()->DungeonFailLogQuery( ID_Info );

	g_dpDBClient.SendLogInstanceDungeon( dwLayer, ID_Info.dwWorldId, g_uIdofMulti, GetType(), 'R' );

	return TRUE;
}
#else
BOOL CInstanceDungeonBase::DestroyDungeonLayer( const INDUN_INFO & /*ID_Info*/, DWORD /*dwLayer*/ )
{
	return TRUE;
}
#endif // __WORLDSERVER

BOOL	CInstanceDungeonBase::GetDungeonLevel( const DWORD dwWorldID, const DWORD dwDungeonID, DWORD & o_dwDungoenLevel )
{
	VEC_IDINFO* pVecInfo = GetDungeonVector( dwDungeonID );
	if( pVecInfo == NULL )
		return FALSE;

	for( VEC_IDINFO::iterator it=pVecInfo->begin(); it!=pVecInfo->end(); it++ )
	{
		//INDUN_INFO & info		= (*it);
		//if( info.dwWorldId == dwWorldID )
		//{
		//	o_dwDungoenLevel			= info.eDungeonLevel;
		//	return TRUE;
		//}
		INDUN_INFO * pInfo		= (*it);
		if( pInfo == NULL )
			continue;

		if( pInfo->dwWorldId == dwWorldID )
		{
			o_dwDungoenLevel			= pInfo->eDungeonLevel;
			return TRUE;
		}
	}

	return FALSE;
}


INDUN_INFO * CInstanceDungeonBase::CreateDungeon( const DWORD dwDungeonId, const DWORD dwWorldId, const ULONG uMultiKey, const DWORD dwGuildID, const int eDungeonLevel )
{
	//해당 파티에 귀속된 던전인가?
	VEC_IDINFO* pVecInfo = GetDungeonVector( dwDungeonId );
	if( pVecInfo )
	{
		for( VEC_IDINFO::iterator it=pVecInfo->begin(); it!=pVecInfo->end(); it++ )
		{
			if( (*it)->dwWorldId == dwWorldId )
				return NULL;
		}

		INDUN_INFO * pInfo	= new INDUN_INFO( dwWorldId, uMultiKey, dwDungeonId, dwGuildID, eDungeonLevel );
		pVecInfo->push_back( pInfo );
		if( CreateDungeonLayer( *pInfo, dwDungeonId ) == FALSE )
		{
			for( VEC_IDINFO::iterator it=pVecInfo->begin(); it!=pVecInfo->end(); it++ )
			{
				INDUN_INFO * p = *it;
				if( pInfo == p )
				{
					pVecInfo->erase( it );
					break;
				}
			}

			SAFE_DELETE( pInfo );
			return NULL;
		}
			
		return pInfo;
	}
	else
	{
		VEC_IDINFO vecTemp;
		INDUN_INFO * pInfo	= new INDUN_INFO( dwWorldId, uMultiKey, dwDungeonId, dwGuildID, eDungeonLevel );
		std::pair<MAP_IDBASE::iterator, bool> ret = m_mapID.insert( MAP_IDBASE::value_type( dwDungeonId, vecTemp ) );
		if( ret.second == false )
		{
			SAFE_DELETE( pInfo );
			return NULL;
		}
		VEC_IDINFO & vecInfo = ret.first->second;
		vecInfo.push_back( pInfo );
		
		if( CreateDungeonLayer( *pInfo, dwDungeonId ) == FALSE )
		{
			for( VEC_IDINFO::iterator it=vecInfo.begin(); it!=vecInfo.end(); it++ )
			{
				INDUN_INFO * p = *it;
				if( pInfo == p )
				{
					vecInfo.erase( it );
					break;
				}
			}

			SAFE_DELETE( pInfo );
			return NULL;
		}

		
		return pInfo;
	}

//	return NULL;
}

BOOL CInstanceDungeonBase::DestroyDungeon( INDUN_INFO & ID_Info, DWORD dwDungeonId )
{
	MAP_IDBASE::iterator it = m_mapID.find( dwDungeonId );
	if( it != m_mapID.end() )
	{
		//인던 리스트
		for( VEC_IDINFO::iterator itVec=it->second.begin(); itVec!=it->second.end(); itVec++ )
		{
			INDUN_INFO * pInfo = *itVec;
			if( pInfo == NULL )
				continue;

			if( pInfo->dwWorldId == ID_Info.dwWorldId )
			{
				DestroyDungeonLayer( *pInfo, dwDungeonId );// == FALSE )
				
				SAFE_DELETE( pInfo );

				it->second.erase( itVec );
				if( it->second.empty() )
					m_mapID.erase( it );

//				ResetDungeonCoolTimeInfo( ID_Info.dwWorldId, dwDungeonId );
#ifdef __CORESERVER
				CInstanceDungeonHelper::GetInstance()->SendInstanceDungeonDestroy( GetType(), dwDungeonId, ID_Info );
#endif // __CORESERVER
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CInstanceDungeonBase::DestroyAllDungeonByDungeonID( DWORD dwDungeonId )
{
	VEC_IDINFO* pvecTemp = GetDungeonVector( dwDungeonId );
	if( pvecTemp )
	{
		VEC_IDINFO vecTemp;
		vecTemp.assign( pvecTemp->begin(), pvecTemp->end() );
		for( VEC_IDINFO::iterator itVec=vecTemp.begin(); itVec!=vecTemp.end(); itVec++ )
		{
			INDUN_INFO* pInfo	= *itVec;
#ifdef __CORESERVER
			DestroyDungeon( *pInfo, dwDungeonId );		// 090525 현재까지 사용할 일 없다^^
			ResetDungeonCoolTimeInfo( pInfo->dwWorldId, dwDungeonId );
#endif // __CORESERVER
#ifdef __WORLDSERVER
		CInstanceDungeonHelper::GetInstance()->SendInstanceDungeonDestroy( GetType(), dwDungeonId, *pInfo );
#endif // __WORLDSERVER
		}
	}
}

void CInstanceDungeonBase::DestroyAllDungeonByMultiKey( ULONG uMultiKey )
{
	std::vector< std::pair<INDUN_INFO*, DWORD> > vecpairTemp;
	for( MAP_IDBASE::iterator it=m_mapID.begin(); it!=m_mapID.end(); ++it )
	{
		for( VEC_IDINFO::iterator itVec=it->second.begin(); itVec!=it->second.end(); ++itVec )
		{
			if( uMultiKey == (*itVec)->uMultiKey )
				vecpairTemp.push_back(std::make_pair( (*itVec), it->first ) );
		}
	}

	for( DWORD i=0; i<vecpairTemp.size(); i++ )
	{
#ifdef __CORESERVER
		DestroyDungeon( *vecpairTemp.at( i ).first, vecpairTemp.at( i ).second );
		ResetDungeonCoolTimeInfo( vecpairTemp.at( i ).first->dwWorldId, vecpairTemp.at( i ).second );
#endif // __CORESERVER
#ifdef __WORLDSERVER
		// 090525 현재까지 사용할 일 없다^^
		CInstanceDungeonHelper::GetInstance()->SendInstanceDungeonDestroy( GetType(), vecpairTemp.at( i ).second, *vecpairTemp.at( i ).first );
#endif // __WORLDSERVER
	}
}

VEC_IDINFO* CInstanceDungeonBase::GetDungeonVector( DWORD dwDungeonId )
{
	MAP_IDBASE::iterator it = m_mapID.find( dwDungeonId );
	if( it != m_mapID.end() )
		return &it->second;

	return NULL;
}

INDUN_INFO* CInstanceDungeonBase::GetDungeonInfo( DWORD dwDungeonId, DWORD dwWorldId )
{
	VEC_IDINFO* pVecInfo = GetDungeonVector( dwDungeonId );
	if( pVecInfo )
	{
		for( VEC_IDINFO::iterator it=pVecInfo->begin(); it!=pVecInfo->end(); it++ )
		{
			if( (*it)->dwWorldId == dwWorldId )
				return (*it);
		}
	}

	return NULL;
}



VEC_CTINFO* CInstanceDungeonBase::GetCoolTimeVector( DWORD dwPlayerId )
{
	MAP_CTINFO::iterator it = m_mapCTInfo.find( dwPlayerId );
	if( it != m_mapCTInfo.end() )
		return &it->second;

	return NULL;
}

COOLTIME_INFO* CInstanceDungeonBase::GetCoolTimeInfo( DWORD dwPlayerId, DWORD dwWorldId )
{
	VEC_CTINFO* pVecCT = GetCoolTimeVector( dwPlayerId );
	if( pVecCT )
	{
		for( VEC_CTINFO::iterator it=pVecCT->begin(); it!=pVecCT->end(); it++ )
		{
			if( (*it).dwWorldId == dwWorldId )
				return &(*it);
		}
	}
	return NULL;
}

void CInstanceDungeonBase::SetDungeonCoolTimeInfo( COOLTIME_INFO CT_Info, DWORD dwPlayerId )
{
	VEC_CTINFO* pVecCT = GetCoolTimeVector( dwPlayerId );
	if( pVecCT )
	{
		for( VEC_CTINFO::iterator it=pVecCT->begin(); it!=pVecCT->end(); it++ )
		{
			if( (*it).dwWorldId == CT_Info.dwWorldId && (*it).dwDungeonId != CT_Info.dwDungeonId )
			{
				(*it).dwDungeonId = CT_Info.dwDungeonId;
				(*it).dwCoolTime = CT_Info.dwCoolTime;
				return;
			}
		}
		pVecCT->push_back( CT_Info );
	}
	else
	{
		VEC_CTINFO vecTemp;
		vecTemp.push_back( CT_Info );
		m_mapCTInfo.insert( MAP_CTINFO::value_type( dwPlayerId, vecTemp ) );
	}
}

void CInstanceDungeonBase::ResetDungeonCoolTimeInfo( DWORD dwWorldId, DWORD dwDungeonId )
{
	for( MAP_CTINFO::iterator it=m_mapCTInfo.begin(); it!=m_mapCTInfo.end(); it++ )
	{
		for( VEC_CTINFO::iterator itVec=it->second.begin(); itVec!=it->second.end(); itVec++ )
		{
			if( (*itVec).dwWorldId == dwWorldId && (*itVec).dwDungeonId == dwDungeonId )
				(*itVec).dwDungeonId = NULL_ID;
		}
	}

#ifdef __CORESERVER
	g_dpCoreSrvr.SendInstanceDungeonResetCoolTimeInfo( GetType(), dwWorldId, dwDungeonId );
#endif // __CORESERVER
}

#ifdef __CORESERVER
void CInstanceDungeonBase::DungeonCoolTimeCountProcess()
{
	if( ++m_nCoolTimeCount >= COOLTIMECOUNT )
	{
		m_nCoolTimeCount = 0;
		UpdateDungeonCoolTimeInfo();
	}
}

void CInstanceDungeonBase::UpdateDungeonCoolTimeInfo()
{
	DWORD dwTickCount = GetTickCount();
	for( MAP_CTINFO::iterator itMap=m_mapCTInfo.begin(); itMap!=m_mapCTInfo.end(); )
	{
		BOOL bErase = TRUE;
		for( VEC_CTINFO::iterator itVec=itMap->second.begin(); itVec!=itMap->second.end(); itVec++ )
		{
			if( (*itVec).dwCoolTime > dwTickCount )
			{
				bErase = FALSE;
				break;
			}
		}

		if( bErase )
		{
			CInstanceDungeonHelper::GetInstance()->SendInstanceDungeonDeleteCoolTimeInfo( GetType(), itMap->first );
			m_mapCTInfo.erase( itMap++ );
		}
		else
			itMap++;
	}
}
#endif // __CORESERVER

#ifdef __WORLDSERVER
void CInstanceDungeonBase::LoadScript( const char* szFilename )
{
	CLuaBase Lua;

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), szFilename );

	if( Lua.RunScript( szFullPath ) != 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		exit( 0 );
	}

	m_nMaxInstanceDungeon = static_cast<BYTE>( Lua.GetGlobalNumber( "nMaxInstanceDungeon" ) );

	Lua.GetGloabal( "tDungeon" );
	Lua.PushNil();
	while( Lua.TableLoop( -2 ) )
	{
		DUNGEON_DATA null_data;// = { 0 };
		//	for( int i=ID_NORMAL;i<=ID_BOSS; i++ )							
		//		data.mapObjCount.insert( std::map<int,int>::value_type( i, 0 ) );
		DWORD dwWorldId	= static_cast<DWORD>( CScript::GetDefineNum( Lua.GetFieldToString( -1, "strWorldId" ) ) );
		MAP_IDDATA::iterator it = m_mapDungeonData.insert( MAP_IDDATA::value_type( dwWorldId, null_data ) ).first;
		if( it == m_mapDungeonData.end() )
		{
			it					= m_mapDungeonData.find( dwWorldId );
			if( it != m_mapDungeonData.end() )
				FLERROR_LOG( PROGRAM_NAME, _T( "[ CRITICAL ERROR : WorldID(%u) ]" ), dwWorldId );
		}

		const DWORD eDungeonLevel	= static_cast<DWORD> ( Lua.GetFieldToNumber( -1, "nDungeonLevel" ) );
		if( eDungeonLevel >= DUNGEON_LEVEL_MAX )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID DUNGEON LEVEL (%u) ]" ), eDungeonLevel );
			RELEASE_BREAK;
		}

		DUNGEON_DATA & realData	= it->second;

		realData.dwClass	= static_cast<DWORD>( Lua.GetFieldToNumber( -1, "dwClass" ) );
		realData.dwCoolTime	= static_cast<DWORD>( Lua.GetFieldToNumber( -1, "dwCoolTime" ) );

		realData.vStartPos.x	= static_cast< FLOAT >( Lua.GetFieldToNumber( -1, "nStartPositionX" ) );
		realData.vStartPos.y	= 0;
		realData.vStartPos.z	= static_cast< FLOAT >( Lua.GetFieldToNumber( -1, "nStartPositionZ" ) );

		realData.arrLevelProp[ eDungeonLevel ].nMinLevel	= static_cast<int>( Lua.GetFieldToNumber( -1, "nMinLevel" ) );
		realData.arrLevelProp[ eDungeonLevel ].nMaxLevel	= static_cast<int>( Lua.GetFieldToNumber( -1, "nMaxLevel" ) );
		realData.arrLevelProp[ eDungeonLevel ].dwAutoStart_WaitingTick	= static_cast<int>( Lua.GetFieldToNumber( -1, "dwAutoStart_WaitingTick" ) );
		realData.arrLevelProp[ eDungeonLevel ].dwStartStage_WaitingTick	= static_cast<int>( Lua.GetFieldToNumber( -1, "dwStartStage_WaitingTick" ) );
		//		data.arrLevelProp[ eDungeonLevel ].dwStageTimeLimit_WaitingTick	= static_cast<int>( Lua.GetFieldToNumber( -1, "dwStageTimeLimit_WaitingTick" ) );
		realData.arrLevelProp[ eDungeonLevel ].dwComplete_WaitingTick	= static_cast<int>( Lua.GetFieldToNumber( -1, "dwComplete_WaitingTick" ) );
		realData.arrLevelProp[ eDungeonLevel ].dwRetry_WaitingTick		= static_cast<int>( Lua.GetFieldToNumber( -1, "dwRetry_WaitingTick" ) );
		realData.arrLevelProp[ eDungeonLevel ].dwRetryStartStage_WaitingTick	= static_cast<int>( Lua.GetFieldToNumber( -1, "dwRetryStartStage_WaitingTick" ) );

		realData.arrLevelProp[ eDungeonLevel ].dwMailTitleTID	= static_cast<int>( CScript::GetDefineNum( Lua.GetFieldToString( -1, "strMailTitleTID" ) ) );
		realData.arrLevelProp[ eDungeonLevel ].dwMailTextTID	= static_cast<int>( CScript::GetDefineNum( Lua.GetFieldToString( -1, "strMailTextTID" ) ) );
#ifdef PARTY_WARP
		realData.bPartyWarp	= static_cast<int>( Lua.GetFieldToNumber( -1, "bPartyWarp" ) );
		//realData.nTicketIndex	= static_cast<int>( Lua.GetFieldToNumber( -1, "nTicketIndex" ) );
		realData.nTicketIndex	= static_cast<int>( CScript::GetDefineNum( Lua.GetFieldToString( -1, "strTicketIndex" ) ) );
#endif // PARTY_WARP		
		Lua.GetField( -1, "tTeleport" );
		Lua.PushNil();
		CWorld* pWorld = g_WorldMng.GetWorld( dwWorldId );
		while( Lua.TableLoop( -2 ) )
		{
			int	nStage		= static_cast<int>( Lua.GetFieldToNumber( -1, "nType" ) );
			D3DXVECTOR3 vPos;
			vPos.x			= static_cast<float>( Lua.GetFieldToNumber( -1, "x" ) );
			vPos.y			= static_cast<float>( Lua.GetFieldToNumber( -1, "y" ) );
			vPos.z			= static_cast<float>( Lua.GetFieldToNumber( -1, "z" ) );
			if( !pWorld && pWorld->VecInWorld( vPos ) == FALSE )
				FLERROR_LOG( PROGRAM_NAME, _T( "Invalid World Pos! WorldId : %d, x : %f, y : %f, z : %f" ), dwWorldId, vPos.x, vPos.y, vPos.z );
			realData.mapTeleportPos.insert( std::map<int, D3DXVECTOR3>::value_type( nStage, vPos ) );
			Lua.Pop( 1 );
		}
		Lua.Pop( 1 ); // tTeleport 스택에서 제거

		//-------------------------- start Monster --------------------------------------//
		Lua.GetField( -1, "tMonster" );
		Lua.PushNil();

		while( Lua.TableLoop( -2 ) )
		{
			PAIR_MONSTER monster = { 0 };
			//monster.nStage				= static_cast<int>( Lua.GetFieldToNumber( -1, "nStage" ) );
			const int nStage			= static_cast<int>( Lua.GetFieldToNumber( -1, "nStage" ) );
			monster.dwFakeMonsterID		= static_cast<DWORD>( CScript::GetDefineNum( Lua.GetFieldToString( -1, "strMonsterId" ) ) );
			monster.dwRealMonsterID		= NULL_ID;
			if( !prj.GetMoverProp( monster.dwFakeMonsterID ) )
				FLERROR_LOG( PROGRAM_NAME, _T( "Invalid FAKE Monster! MonsterId : %d" ), monster.dwFakeMonsterID );

			monster.dw10000RealMonsterRate	= 0;

			monster.dwLimitTime			= static_cast<DWORD>( Lua.GetFieldToNumber( -1, "dwTimeLimit" ) );
			monster.bRed				= static_cast<BOOL>( Lua.GetFieldToBool( -1, "bRed" ) );
			monster.vPos.x				= static_cast<float>( Lua.GetFieldToNumber( -1, "x" ) );
			monster.vPos.y				= static_cast<float>( Lua.GetFieldToNumber( -1, "y" ) );
			monster.vPos.z				= static_cast<float>( Lua.GetFieldToNumber( -1, "z" ) );

			monster.bRandomMonster		= FALSE;

			for( ;; )
			{
				if( nStage >= ( int )realData.arrLevelProp[ eDungeonLevel ].vec2DPairMonster.size() )
					realData.arrLevelProp[ eDungeonLevel ].vec2DPairMonster.push_back( PairMonsterVec() );
				else 
					break;
			}
			realData.arrLevelProp[ eDungeonLevel ].vec2DPairMonster[ nStage ].push_back( monster );

			Lua.Pop( 1 );
		}
		Lua.Pop( 1 ); // tMonster 스택에서 제거
		//-------------------------- end Monster --------------------------------------//

		Lua.GetField( -1, "tPairMonster" );
		Lua.PushNil();

		while( Lua.TableLoop( -2 ) )
		{
			PAIR_MONSTER monster = { 0 };
			//monster.nStage				= static_cast<int>( Lua.GetFieldToNumber( -1, "nStage" ) );
			const int nStage			= static_cast<int>( Lua.GetFieldToNumber( -1, "nStage" ) );
			monster.dwFakeMonsterID		= static_cast<DWORD>( CScript::GetDefineNum( Lua.GetFieldToString( -1, "strFakeMonsterId" ) ) );
			monster.dwRealMonsterID		= static_cast<DWORD>( CScript::GetDefineNum( Lua.GetFieldToString( -1, "strRealMonsterId" ) ) );
			if( !prj.GetMoverProp( monster.dwFakeMonsterID ) )
				FLERROR_LOG( PROGRAM_NAME, _T( "Invalid FAKE Monster! MonsterId : %d" ), monster.dwFakeMonsterID );
			if( !prj.GetMoverProp( monster.dwRealMonsterID ) )
				FLERROR_LOG( PROGRAM_NAME, _T( "Invalid Real Monster! MonsterId : %d" ), monster.dwRealMonsterID );

			monster.dw10000RealMonsterRate = static_cast<DWORD>( Lua.GetFieldToNumber( -1, "dw10000RealMonsterRate" ) );
			monster.dwLimitTime			= static_cast<DWORD>( Lua.GetFieldToNumber( -1, "dwTimeLimit" ) );
			monster.bRed				= static_cast<BOOL>( Lua.GetFieldToBool( -1, "bRed" ) );
			monster.vPos.x				= static_cast<float>( Lua.GetFieldToNumber( -1, "x" ) );
			monster.vPos.y				= static_cast<float>( Lua.GetFieldToNumber( -1, "y" ) );
			monster.vPos.z				= static_cast<float>( Lua.GetFieldToNumber( -1, "z" ) );

			monster.bRandomMonster		= FALSE;

			for( ;; )
			{
				if( nStage >= ( int )realData.arrLevelProp[ eDungeonLevel ].vec2DPairMonster.size() )
					realData.arrLevelProp[ eDungeonLevel ].vec2DPairMonster.push_back( PairMonsterVec() );
				else 
					break;
			}
			realData.arrLevelProp[ eDungeonLevel ].vec2DPairMonster[ nStage ].push_back( monster );
			realData.bColosseumStyle	= TRUE;


			Lua.Pop( 1 );
		}
		Lua.Pop( 1 ); // tPairMonster 스택에서 제거



		Lua.GetField( -1, "tRandomMonster" );
		Lua.PushNil();

		while( Lua.TableLoop( -2 ) )
		{
			PAIR_MONSTER monster = { 0 };
			//monster.nStage				= 0;//static_cast<int>( Lua.GetFieldToNumber( -1, "nStage" ) );
			monster.dwFakeMonsterID		= static_cast<DWORD>( CScript::GetDefineNum( Lua.GetFieldToString( -1, "strMonsterId" ) ) );
			monster.dwRealMonsterID		= static_cast<DWORD>( CScript::GetDefineNum( Lua.GetFieldToString( -1, "strMonsterId" ) ) );
			monster.dw10000RealMonsterRate	= 0;
			monster.dwLimitTime			= static_cast<DWORD>( Lua.GetFieldToNumber( -1, "dwTimeLimit" ) );
			monster.bRed				= static_cast<BOOL>( Lua.GetFieldToBool( -1, "bRed" ) );
			monster.vPos.x				= static_cast<float>( Lua.GetFieldToNumber( -1, "x" ) );
			monster.vPos.y				= static_cast<float>( Lua.GetFieldToNumber( -1, "y" ) );
			monster.vPos.z				= static_cast<float>( Lua.GetFieldToNumber( -1, "z" ) );

			monster.bRandomMonster		= TRUE;

			realData.arrLevelProp[ eDungeonLevel ].vecRandomMonster.push_back( monster );
			realData.bColosseumStyle	= TRUE;


			Lua.Pop( 1 );
		}
		Lua.Pop( 1 ); // tRandomMonster 스택에서 제거

		//---------------------------------- 보상 -----------------------------------------------------------//

		//Lua.GetField( -1, "tRewardItem" );
		//Lua.PushNil();

		//while( Lua.TableLoop( -2 ) )
		//{
		//	const DWORD dwItemID		= static_cast<DWORD>( CScript::GetDefineNum( Lua.GetFieldToString( -1, "strItemID" ) ) );
		//	const short wCount			= static_cast<short>( Lua.GetFieldToNumber( -1, "wCount" ) );

		//	realData.arrLevelProp[ eDungeonLevel ].vecReward.push_back( new FLReward_Item( dwItemID, wCount ) );
		//	Lua.Pop( 1 );
		//}
		//Lua.Pop( 1 ); // tMonster 스택에서 제거

		//const int nRewardGold			= static_cast<int>( Lua.GetFieldToNumber( -1, "nRewardGold" ) );	
		//if( nRewardGold > 0 )
		//{
		//	realData.arrLevelProp[ eDungeonLevel ].vecReward.push_back( new FLReward_Gold( nRewardGold ) );
		//}

		Lua.Pop( 1 );

		//------- 스테이지 갯수 설정 -------//
		realData.arrLevelProp[ eDungeonLevel ].nMaxStage	
			= realData.arrLevelProp[ eDungeonLevel ].vec2DPairMonster.size() + realData.arrLevelProp[ eDungeonLevel ].vecRandomMonster.size();

		CInstanceDungeonHelper::GetInstance()->SetDungeonType( dwWorldId, GetType() );
	}
	Lua.Pop( 0 );
}

int	CInstanceDungeonBase::GetCurrStage( DWORD dwWorldId, DWORD dwDungeonId )
{
	MAP_IDBASE::iterator it = m_mapID.find( dwDungeonId );
	if( it != m_mapID.end() )
	{
		for( DWORD i=0; i<it->second.size(); i++ )
		{
			if( (it->second).at( i )->dwWorldId == dwWorldId )
				return (it->second).at( i )->nCurrStage;
		}
	}

	return ID_NORMAL;
}


void CInstanceDungeonBase::TeleportToNextStage( INDUN_INFO & o_Info, const DWORD dwDungeonID )
{
	//텔레포트 시키고
	D3DXVECTOR3 vPrevPos = GetTeleportPosByStage( o_Info.dwWorldId, dwDungeonID, o_Info.nCurrStage );
	D3DXVECTOR3 vNextPos = GetTeleportPosByStage( o_Info.dwWorldId, dwDungeonID, o_Info.nCurrStage + 1 );

	if( vPrevPos != vNextPos && vNextPos != D3DXVECTOR3( 0, 0, 0 ) )
	{
		CWorld* pWorld = g_WorldMng.GetWorld( o_Info.dwWorldId );
		if( !pWorld )
			return;

		CObj* pObj;
		for( DWORD i=0; i<pWorld->m_dwObjNum; i++ )
		{
			pObj = pWorld->m_apObject[i];
			if( IsValidObj( pObj ) && pObj->GetLayer() == static_cast<int>( dwDungeonID )
				&& 	pObj->GetType() == OT_MOVER && static_cast<CMover*>( pObj )->IsPlayer() )
				static_cast<FLWSUser*>( pObj )->REPLACE( g_uIdofMulti, pWorld->GetID(), vNextPos, REPLACE_NORMAL, static_cast<int>( dwDungeonID ) );
		}
	}
}

void	CInstanceDungeonBase::Teleport( INDUN_INFO & o_Info/*, const DWORD dwDungeonID*/ )
{
	D3DXVECTOR3 vPos = GetTeleportPosByStage( o_Info.dwWorldId, o_Info.dwDungeonID, o_Info.nCurrStage );

	if( vPos != D3DXVECTOR3( 0, 0, 0 ) )
	{
		CWorld* pWorld = g_WorldMng.GetWorld( o_Info.dwWorldId );
		if( !pWorld )
			return;

		CObj* pObj;
		for( DWORD i=0; i<pWorld->m_dwObjNum; i++ )
		{
			pObj = pWorld->m_apObject[i];
			if( IsValidObj( pObj ) && pObj->GetLayer() == static_cast<int>( o_Info.dwDungeonID )
				&& 	pObj->GetType() == OT_MOVER && static_cast<CMover*>( pObj )->IsPlayer() )
				static_cast<FLWSUser*>( pObj )->REPLACE( g_uIdofMulti, pWorld->GetID(), vPos, REPLACE_NORMAL, static_cast<int>( o_Info.dwDungeonID ) );
		}
	}
}

void	CInstanceDungeonBase::UpdateState_OnlyColosseum()
{
//	const DWORD dwCurrTick		= ::GetTickCount();
	//	if( dwCurrTick < )

	for( MAP_IDBASE::iterator it = m_mapID.begin(); it != m_mapID.end(); ++it )
	{
		const DWORD	dwDungeonID				= it->first;
		VEC_IDINFO & vecInfo				= it->second;

		for( VEC_IDINFO::iterator infoIt = vecInfo.begin(); infoIt != vecInfo.end(); ++infoIt )
		{
			// 몬스터 킬은 이벤트로 온다.	NextStage

			INDUN_INFO * pInfo		= *infoIt;
			if( pInfo == NULL )
				continue;

			DUNGEON_DATA * pData			= GetDuneonData( *pInfo );
			if( pData == NULL )
				continue;

			// 상태 Update
			if( pData->bColosseumStyle == FALSE )
				continue;

			if( pInfo->m_pState == NULL )
				continue;

			if( pInfo->IsValidState_( pInfo->m_pState ) == FALSE )
				continue;

			if( pInfo->m_pState == &pInfo->AutoStartState && pInfo->nPlayerCount > 0 )
			{
				CParty * pParty	= g_PartyMng.GetParty( dwDungeonID );
				if( pParty == NULL )
					continue;

				CMover * pLeader		= pParty->GetLeader();
				if( IsValidObj( pLeader ) == FALSE )
					continue;

				//처음 들어온사람이 Creater냐?
				if( pLeader->GetWorld() == NULL )
					continue;

				if( pLeader->GetWorld()->GetID() != pInfo->dwWorldId )
					continue;

				pInfo->idLeaderPlayer		= pLeader->m_idPlayer;
				pInfo->bCreateAndEntered	= TRUE;
				//info.dwCreatedTick		= ::GetTickCount();
			}

			pInfo->m_pState->OnUpdate( *pInfo/*, dwDungeonID*/ );
			if( pInfo->bCreateAndEntered == TRUE  )
			{
				UpdateWhenChangedPartyLeader( *pInfo );
				if( CheckAllPlayerDie( pInfo->dwWorldId, dwDungeonID ) == TRUE )
				{
					DUNGEON_DATA * pData = GetDuneonData( *pInfo );	
					if( pData == NULL )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID WORLDID(%u) ]" ), pInfo->dwWorldId );
						pInfo->SetState( NULL, 0/*, dwDungeonID*/ );
						return;
					}

					if(  pInfo->m_pState != NULL && pInfo->m_pState != &pInfo->CompleteAllWaitingState && pInfo->m_pState != &pInfo->AllDieWaitingState )		//@@@@
						pInfo->SetState( &pInfo->AllDieWaitingState, pData->arrLevelProp[ pInfo->eDungeonLevel ].dwRetry_WaitingTick/*, dwDungeonID*/ );		
				}

				if( HasLeader( *pInfo ) == FALSE )	//자동 시작 시간에 파장이 나가면 인던 파괴
				{
					CInstanceDungeonHelper::GetInstance()->OnColosseumFail( *pInfo );
					pInfo->SetState( NULL, 0 );	
				}
			}
			//CInstanceDungeonParty::Process();		//파티가 없는 인원을 쫒아냄
		}
	}
}




void CInstanceDungeonBase::SetNextStage( INDUN_INFO* pInfo, DWORD dwDungeonId/*, DWORD eDungeonLevel*/	)
{
	if( pInfo == NULL )
		return;

	if( pInfo->eDungeonLevel >= DUNGEON_LEVEL_MAX )
		return;

	MAP_IDDATA::const_iterator it = m_mapDungeonData.find( pInfo->dwWorldId );
	if( it == m_mapDungeonData.end() )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "INVALID DUNGEON DATA Type:%d, DungeonID:%d, WorldID:%d" ), GetType(), dwDungeonId, pInfo->dwWorldId );
		return;
	}

	const CInstanceDungeonBase::DUNGEON_DATA & dungeonData = it->second;
	// 마지막 스테이지가 아니라면
	if( pInfo && pInfo->nCurrStage < dungeonData.arrLevelProp[ pInfo->eDungeonLevel ].nMaxStage )//pInfo->nStage < ID_BOSS )	
	{
		TeleportToNextStage( *pInfo, dwDungeonId );

		//if( pInfo->nKillCount >= GetObjCount( pInfo->dwWorldId, dwDungeonId ) )		

		if( dungeonData.bColosseumStyle == TRUE )
		{
			if( pInfo->IsRandomMonsterStage( pInfo->nCurrStage ) == FALSE  )
				pInfo->dwTurnaroundTime = pInfo->dwTurnaroundTime + ( ::GetTickCount() - pInfo->dwStartStageTick );	//소요 시간 기록

			//set next
			++pInfo->nCurrStage;
			pInfo->nKillCount			= 0;

			if( pInfo->nKillCount >= GetObjCount_CurrStage( pInfo->dwWorldId, dwDungeonId ) )	
				SetNextStage( pInfo, dwDungeonId );


			if( pInfo->nCurrStage < dungeonData.arrLevelProp[ pInfo->eDungeonLevel ].nMaxStage )
			{
				pInfo->SetState( &pInfo->StageWaitingState, dungeonData.arrLevelProp[ pInfo->eDungeonLevel ].dwStartStage_WaitingTick/*, dwDungeonId*/ );
			}
			else
			{
				pInfo->SetState( &pInfo->CompleteAllWaitingState, dungeonData.arrLevelProp[ pInfo->eDungeonLevel ].dwComplete_WaitingTick/*, dwDungeonId*/ );
			}
		}
		else 
		{
			//set next
			++pInfo->nCurrStage;
			pInfo->nKillCount			= 0;

			RespawnMonster_AndTrySwitchToStageLimit( dwDungeonId, pInfo->dwWorldId );
		}
	}
	else if( pInfo )	// 던전 클리어 로그(추후 삭제 예정)
	{
		g_dpDBClient.SendLogInstanceDungeon( dwDungeonId, pInfo->dwWorldId, g_uIdofMulti, GetType(), 'C' );
		//FLTRACE_LOG( PROGRAM_NAME, _T( "Instance Dungeon Clear - Party : %07d,\tWorld : %d" ), dwDungeonId, pInfo->dwWorldId );

	}
}

D3DXVECTOR3 CInstanceDungeonBase::GetTeleportPosByStage( DWORD dwWorldId, DWORD /*dwDungeonId*/, const DWORD dwStage )
{
	MAP_IDDATA::iterator it = m_mapDungeonData.find( dwWorldId );
	if( it == m_mapDungeonData.end() )
		return D3DXVECTOR3( 0, 0, 0 );

	std::map<int, D3DXVECTOR3>::iterator it2 = it->second.mapTeleportPos.find( dwStage/*GetState( dwWorldId, dwDungeonId )*/ );
	if( it2 == it->second.mapTeleportPos.end() )
		return D3DXVECTOR3( 0, 0, 0 );

	return it2->second;
}

BOOL CInstanceDungeonBase::CheckClassLevel( FLWSUser* pUser, DWORD dwWorldId, DWORD eDungeonLevel )
{
	if( eDungeonLevel >= DUNGEON_LEVEL_MAX )
		return FALSE;


	MAP_IDDATA::iterator it = m_mapDungeonData.find( dwWorldId );
	if( it == m_mapDungeonData.end() )
		return FALSE;

	WORD wClass = 0x0000;
	switch( pUser->GetLegendChar() )
	{
	case LEGEND_CLASS_NORMAL : wClass = CLASS_NORMAL; break;		
	case LEGEND_CLASS_MASTER : wClass = CLASS_MASTER; break;	
	case LEGEND_CLASS_HERO : wClass = CLASS_HERO; break;
	case LEGEND_CLASS_LEGEND_HERO : wClass = CLASS_LEGEND_HERO; break;

	default : wClass = 0x0000;
	}

	const DUNGEON_DATA & data		= it->second;

	if( wClass & data.dwClass )
	{
		DUNGEON_DATA & data		= it->second;
		if( pUser->GetLevel() < data.arrLevelProp[ eDungeonLevel ].nMinLevel || pUser->GetLevel() > data.arrLevelProp[ eDungeonLevel ].nMaxLevel )
		{
			pUser->AddDefinedText( TID_GAME_INSTANCE_LEVEL, "%d %d", data.arrLevelProp[ eDungeonLevel ].nMinLevel, data.arrLevelProp[ eDungeonLevel ].nMaxLevel );
			return FALSE;
		}

		return TRUE;
	}
	else
		pUser->AddDefinedText( TID_GAME_INSTANCE_CLASS );

	return FALSE;
}

BOOL CInstanceDungeonBase::IncreasePlayerCount( DWORD dwDungeonId, DWORD dwWorldId )
{
	INDUN_INFO* pInfo = GetDungeonInfo( dwDungeonId, dwWorldId );
	if( pInfo )
	{
		pInfo->nPlayerCount++;
		//FLTRACE_LOG( PROGRAM_NAME, _T( "[ World : %d, Layer : %d ] - Count : %d" ), dwWorldId, dwDungeonId, pInfo->nPlayerCount );
		return TRUE;
	}

	return FALSE;
}

BOOL CInstanceDungeonBase::DecreasePlayerCount( FLWSUser* pUser, DWORD dwDungeonId, DWORD dwWorldId )
{
	INDUN_INFO* pInfo = GetDungeonInfo( dwDungeonId, dwWorldId );
	if( pInfo )
	{
		//FLTRACE_LOG( PROGRAM_NAME, _T( "[ World : %d, Layer : %d ] - Count : %d" ), dwWorldId, dwDungeonId, pInfo->nPlayerCount-1 );
		if( --pInfo->nPlayerCount <= 0 )
		{
			if( GetType() == IDTYPE_PARTY )
			{
				CParty* pParty = g_PartyMng.GetParty( dwDungeonId );
				if( pParty != NULL )
				{
					for( int i = 0; i < pParty->GetSizeofMember(); ++i )
					{
						FLWSUser* pPartyMember = g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( i ) );
						if( IsValidObj( pPartyMember ) == TRUE && pPartyMember != pUser )
						{
							if( pPartyMember->GetWorld() != NULL && pPartyMember->GetWorld()->GetID() == dwWorldId
								&& static_cast< DWORD >( pPartyMember->GetLayer() ) == dwDungeonId )
							{
								return FALSE;
							}
						}
					}
				}
				else
				{
					return TRUE;
				}
			}
			return TRUE;
		}
	}

	return FALSE;
}




void CInstanceDungeonBase::RespawnMonster_AndTrySwitchToStageLimit( DWORD dwDungeonId, DWORD dwWorldId )
{
	CWorld* pWorld = g_WorldMng.GetWorld( dwWorldId );
	if( !pWorld || !pWorld->m_linkMap.GetLinkMap( static_cast<int>( dwDungeonId ) ) )
		return;

	INDUN_INFO* pInfo = GetDungeonInfo( dwDungeonId, dwWorldId );
	if( pInfo == NULL )
		return;

	if( pInfo->eDungeonLevel >= DUNGEON_LEVEL_MAX )
		return;

	CInstanceDungeonBase::DUNGEON_DATA * pData = GetDuneonData( *pInfo );	//@@@@@@@ 상위 객체 참조 -_-;
	if( pData == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID WORLDID(%u) ]" ), pInfo->dwWorldId );
		return;
	}

	const int nStage = GetCurrStage( dwWorldId, dwDungeonId );
	if( ( int )pInfo->vec2DPairMonsterList.size() <= nStage )
		return;

	DWORD dwTimeLimit		= UNLIMIT_TIME;
	const PairMonsterVec vec = pInfo->vec2DPairMonsterList.at( nStage );
	for( PairMonsterVec::const_iterator monIt = vec.begin()
		; monIt != vec.end()
		; ++monIt )
	{
		const PAIR_MONSTER & monster	= *monIt;
		const BOOL bRet		= respawnOneMonster( *pWorld, monster, pInfo->nCurrStage, *pInfo );//( *pWorld, monster, pInfo->nCurrStage, dwDungeonId, dwWorldId, pInfo->dw10000AdditionalRealMonsterRate );
		if( bRet == FALSE )
		{
			//++pInfo->nKillCount;
			continue;
		}

		dwTimeLimit			= std::min( monster.dwLimitTime, dwTimeLimit );	
	}

	//상태 전환
	if( pData->bColosseumStyle == TRUE )
	{
		pInfo->dwTimeLimitTick	= dwTimeLimit;
		pInfo->SetState( &pInfo->StageLimitState, dwTimeLimit/*, dwDungeonId*/ );
	}
}



BOOL CInstanceDungeonBase::respawnOneMonster( CWorld & World, const PAIR_MONSTER & Monster, const int /*nStage*/, INDUN_INFO & io_info )
{
	//	if( Monster.nStage != nStage )
	//		return FALSE;

	const DWORD dwRealMonsterRate	= Monster.dw10000RealMonsterRate + io_info.dw10000AdditionalRealMonsterRate;

	DWORD dwMonsterID			= Monster.dwFakeMonsterID;
	io_info.bCallRealMonster	= FALSE;
	if( dwRealMonsterRate > xRandom( 10000 ) ) 
	{
		dwMonsterID					= Monster.dwRealMonsterID;
		io_info.bCallRealMonster	= TRUE;
	}

	MoverProp* pMoverProp = prj.GetMoverProp( dwMonsterID );	
	if( pMoverProp == NULL )
		return FALSE;

	CObj* pObj	= CreateObj( D3DDEVICE, OT_MOVER, pMoverProp->dwID );
	if( NULL == pObj )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAIL respawnOneMonster MI(%u)]" ), pMoverProp->dwID );
		return FALSE;
	}

	pObj->SetPos( Monster.vPos );
	pObj->InitMotion( MTI_STAND );
	pObj->UpdateLocalMatrix();

	((CMover*)pObj)->m_bActiveAttack	= Monster.bRed;
	if( World.ADDOBJ( pObj, TRUE, io_info.dwDungeonID ) == FALSE )
	{
		SAFE_DELETE( pObj );
		return FALSE;
	}

	return TRUE;
}

void CInstanceDungeonBase::ClearCalledMonster( DWORD dwDungeonId, DWORD dwWorldId )
{
	CWorld* pWorld = g_WorldMng.GetWorld( dwWorldId );
	if( !pWorld || !pWorld->m_linkMap.GetLinkMap( static_cast<int>( dwDungeonId ) ) )
		return;

	INDUN_INFO* pInfo = GetDungeonInfo( dwDungeonId, dwWorldId );
	if( pInfo == NULL )
		return;


	for( size_t i = 0; i < pWorld->m_dwObjNum; ++i )
	{
		CObj* pObj			= pWorld->m_apObject[i];
		if( IsValidObj( pObj ) == FALSE )
			continue;

		if( pObj->GetType() != OT_MOVER )
			continue;

		CMover * pMover		= static_cast< CMover *>( pObj );
		if( pMover->GetLayer() == static_cast<int>( dwDungeonId )
			&& pMover->IsNPC()
			&& pMover->GetCharacter() == NULL 
			&& pMover->m_dwAIInterface != AII_NONE 
			&& pMover->m_dwAIInterface != AII_PET
			)	//몬스터 라면
		{
			pMover->Delete();
		}
	}

	//	m_apObject
	//		if( IsValidObj( pObj ) && pObj->GetLayer() == static_cast<int>( dwDungeonID )
	//			&& 	pObj->GetType() == OT_MOVER && static_cast<CMover*>( pObj )->IsPlayer() )
	//
	////	for( INDUN_INFO::CalledMonsterVec::const_iterator it = pInfo->vecCalledMonster.begin(); it != pInfo->vecCalledMonster.end(); ++it )
	////	{
	//		const OBJID	objid	= *it;
	//		CMover * pMover		= prj.GetMover( objid );
	//		if( pMover != NULL )
	//			pMover->Delete();
	////	}

	//	pInfo->vecCalledMonster.clear();
}

BOOL CInstanceDungeonBase::TeleportToDungeon( FLWSUser* pUser, DWORD dwWorldId, DWORD dwDungeonId, const DWORD dwGuildID, DWORD eDungeonLevel )
{
	if( pUser->HasBuffByIk3( IK3_TEXT_DISGUISE ) )
	{
		pUser->AddDefinedText( TID_GAME_INSTANCE_DISGUISE02 );	
		return FALSE;
	}

	CWorld* pWorld = g_WorldMng.GetWorld( dwWorldId );
	if( !pWorld )
		return FALSE;

	COOLTIME_INFO* pCT_Info = GetCoolTimeInfo( pUser->m_idPlayer, dwWorldId );
	if( pCT_Info )
	{
		if( pCT_Info->dwDungeonId != dwDungeonId )//월드는 같은데 다른 인스턴스 던전 이라면. 입장 불가.
		{
			DWORD dwRemainCoolTime = pCT_Info->dwCoolTime - GetTickCount();
			if( static_cast<int>( dwRemainCoolTime ) > 0 )
			{
				DWORD dwRemainMin = dwRemainCoolTime / MIN( 1 );
				DWORD dwRemainSec =  ( dwRemainCoolTime % MIN( 1 ) ) / SEC( 1 );
				pUser->AddDefinedText( TID_GAME_INSTANCE_COOLTIME, "%d %d", dwRemainMin, dwRemainSec );
				return FALSE;
			}
		}
	}



	INDUN_INFO* pInfo = GetDungeonInfo( dwDungeonId, dwWorldId );
	if( pInfo )	// 해당 던전이 존재하면 채널 검사...
	{
		if( pInfo->uMultiKey != g_uKey )	// 해당 채널에 생성된 던전이 아니면 입장 불가
		{
			pUser->AddDefinedText( TID_GAME_INSTANCE_CHANNEL );
			return FALSE;
		}


		//콜로세움 타입  관련
		DUNGEON_DATA * pData	= GetDuneonData( *pInfo );
		if( pData == NULL )
			return FALSE;

		if( pData->bColosseumStyle == TRUE && pInfo->m_pState != &pInfo->AutoStartState )
		{
			pUser->AddDefinedText( TID_COLOSSEUM_ALREADYPLAYNG );
			return FALSE;
		}

		if( pData->bColosseumStyle == TRUE && pInfo->nPlayerCount >= MAX_PTMEMBER_SIZE )
		{
			pUser->AddDefinedText( TID_MMI_COLOMAX );
			return FALSE;
		}


		if( !CheckClassLevel( pUser, dwWorldId, pInfo->eDungeonLevel ) )
			return FALSE;

		if( pInfo->dwGuildID != 0 )
		{
			if( pUser->GetGuild() == NULL )
			{
				pUser->AddDefinedText( TID_COLOSSEUM_COLOGUILDGROUP01 );
				return FALSE;
			}

			if( pInfo->dwGuildID != pUser->GetGuild()->GetGuildId() )
			{
				pUser->AddDefinedText( TID_COLOSSEUM_COLOGUILDGROUP01 );
				return FALSE;
			}
		}

		//텔레포트 위치 찾기
		D3DXVECTOR3 vPos		= D3DXVECTOR3( 0, 0, 0 );

		if( pData->bColosseumStyle == TRUE 
#ifdef PARTY_WARP
		|| pData->bPartyWarp 
#endif // PARTY_WARP
		)
		{
			vPos				= pData->vStartPos;				
		}
		else
		{
			vPos = GetTeleportPosByStage( dwWorldId, dwDungeonId, pInfo->nCurrStage );
			if( vPos == D3DXVECTOR3( 0, 0, 0 ) )
			{
				REGIONELEM* pPortkey = pUser->UpdateRegionAttr();
				if( pPortkey && pPortkey->m_dwIdTeleWorld == dwWorldId )
					vPos = pPortkey->m_vTeleWorld;
				else
					return FALSE;
			}
		}

		CLinkMap* pLinkMap = pWorld->m_linkMap.GetLinkMap( static_cast<int>( dwDungeonId ) );

		if( pLinkMap != NULL && pLinkMap->IsInvalid() == FALSE )
		{
#ifdef PARTY_WARP

			if ( pData->bPartyWarp )
			{
				CParty * pParty	= g_PartyMng.GetParty( dwDungeonId );
				if( !pParty )
					return FALSE;

				FLWSUser *pMember;
				for ( int i = 0; i < pParty->GetSizeofMember(); i++ )
				{
					pMember	= g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId(i) );

					if( IsValidObj( pMember ) )
					{
						SetLeaveMarkingPos( pMember, dwWorldId, vPos );		//@@@@ 콜로세움에서는 Leave 마킹이 안됨
						int nRandx = xRandom(4) - 2;
						int nRandz = xRandom(4) - 2;
						vPos += D3DXVECTOR3( (float)( nRandx ), (float)( 0 ), (float)( nRandz ) );
						if ( pMember->GetWorld() != NULL && pMember->GetWorld()->GetID() == dwWorldId )		// 지금 이동하려는 지역에 있으면 이동시키지 않습니다.
							continue;
		
						if( pMember->REPLACE( g_uIdofMulti, dwWorldId, vPos, REPLACE_NORMAL, static_cast<int>( dwDungeonId ) ) == FALSE )
						{
							pMember->AddDefinedText( TID_GAME_INSTANCE_NOT_TELEPORT );
							return FALSE;
						}
						//IncreasePlayerCount( static_cast<DWORD>( pMember->m_idparty ), dwWorldId );
						if( !pCT_Info || pCT_Info->dwDungeonId != dwDungeonId )	// 090813 던전이 유지되고 있는 상태에서의 재입장은 set하지 않음 
						{
							COOLTIME_INFO cooltime( dwWorldId, dwDungeonId, ( GetCoolTime( dwWorldId ) + GetTickCount() ) );
							CInstanceDungeonHelper::GetInstance()->SendInstanceDungeonSetCoolTimeInfo( g_uKey, GetType(), pMember->m_idPlayer, cooltime );
						}
					}
				}
			}
			else
#endif // PARTY_WARP
			{
				SetLeaveMarkingPos( pUser, dwWorldId, vPos );		//@@@@ 콜로세움에서는 Leave 마킹이 안됨
				int nRandx = xRandom(4) - 2;
				int nRandz = xRandom(4) - 2;
				vPos += D3DXVECTOR3( (float)( nRandx ), (float)( 0 ), (float)( nRandz ) );
				if( pUser->REPLACE( g_uIdofMulti, dwWorldId, vPos, REPLACE_NORMAL, static_cast<int>( dwDungeonId ) ) == FALSE )
				{
					pUser->AddDefinedText( TID_GAME_INSTANCE_NOT_TELEPORT );
					return FALSE;
				}
				//IncreasePlayerCount( static_cast<DWORD>( pUser->m_idparty ), dwWorldId );
				if( !pCT_Info || pCT_Info->dwDungeonId != dwDungeonId )	// 090813 던전이 유지되고 있는 상태에서의 재입장은 set하지 않음 
				{
					COOLTIME_INFO cooltime( dwWorldId, dwDungeonId, ( GetCoolTime( dwWorldId ) + GetTickCount() ) );
					CInstanceDungeonHelper::GetInstance()->SendInstanceDungeonSetCoolTimeInfo( g_uKey, GetType(), pUser->m_idPlayer, cooltime );
				}
			}
			if( pData->bColosseumStyle == TRUE )
			{
				if( pInfo->dwCreatedTick == 0 )
					pInfo->dwCreatedTick	= ::GetTickCount();

				DWORD dwRemainTick	= pData->arrLevelProp[ pInfo->eDungeonLevel ].dwAutoStart_WaitingTick - ( ::GetTickCount() - pInfo->dwCreatedTick );
				if( dwRemainTick > pData->arrLevelProp[ pInfo->eDungeonLevel ].dwAutoStart_WaitingTick ) //overflow
				{
					dwRemainTick	= 0;
				}

				pUser->AddColosseum_AutoStartWaitingTick( dwRemainTick );
				pUser->AddColosseum_RegistAdditionalRealMonsterRateItemNoti( pInfo->dwAddRealBossRate_ItemID );
			}

			return TRUE;
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ LinkMap Invalid. WorldID(%d), DungeonID(%d) ]" ), dwWorldId, dwDungeonId );
			return FALSE;
		}
	}
	else	// 해당 던전이 존재하지 않으면 CoreServer로 생성 할 것을 요청 한다.
	{
		if( !CheckClassLevel( pUser, dwWorldId, eDungeonLevel ) )
			return FALSE;

		if( CInstanceDungeonHelper::GetInstance()->m_nIDLayerNum < m_nMaxInstanceDungeon )
		{
			DUNGEON_DATA * pData	= GetDuneonData( dwWorldId, 0 );
			if ( pData->nTicketIndex > 0 )
			{
				const int nRemain = pUser->RemoveItemByItemID (pData->nTicketIndex, 1, "O", "USE_DUNGEON_TICKET" );
				if ( nRemain ) return FALSE;
			}
			INDUN_INFO info( dwWorldId, g_uKey, dwDungeonId, dwGuildID, eDungeonLevel );
			CInstanceDungeonHelper::GetInstance()->SendInstanceDungeonCreate( GetType(), dwDungeonId, info, pUser->m_idPlayer );
		}
		else
		{
			pUser->AddDefinedText( TID_GAME_INSTANCE_MAX );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CInstanceDungeonBase::LeaveToOutside( FLWSUser* pUser, DWORD dwWorldId, DWORD dwDungeonId )
{
	if( DecreasePlayerCount( pUser, dwDungeonId, dwWorldId ) )
	{
		INDUN_INFO info( dwWorldId, g_uKey, dwDungeonId );
		CInstanceDungeonHelper::GetInstance()->SendInstanceDungeonDestroy( GetType(), dwDungeonId, info );
		return TRUE;
	}
	return FALSE;
}

void CInstanceDungeonBase::SetLeaveMarkingPos( FLWSUser* pUser, DWORD dwWorldId, D3DXVECTOR3 vPos )
{
	pUser->m_idMarkingWorld = WI_WORLD_MADRIGAL;
	pUser->m_vMarkingPos = D3DXVECTOR3( 6983.0f, 0.0f, 3330.0f );

	CWorld* pWorld = g_WorldMng.GetWorld( dwWorldId );
	if( !pWorld )
		return;

	float fDist = static_cast<float>( INT_MAX );
	for( int i = 0; i < pWorld->m_aRegion.GetSize(); i++ )
	{
		LPREGIONELEM lpRegionElem = pWorld->m_aRegion.GetAt( i );
		if( !lpRegionElem || lpRegionElem->m_dwIdTeleWorld == WI_WORLD_NONE )
			continue;

		const D3DXVECTOR3 vDist = vPos - lpRegionElem->m_vPos;
		float fTemp = D3DXVec3LengthSq( &vDist );
		if( fTemp < fDist )
		{
			fDist = fTemp;
			pUser->m_vMarkingPos = lpRegionElem->m_vTeleWorld;
			pUser->m_idMarkingWorld = lpRegionElem->m_dwIdTeleWorld;
		}
	}
}


DWORD CInstanceDungeonBase::GetCoolTime( DWORD dwWorldId )
{
	MAP_IDDATA::iterator it = m_mapDungeonData.find( dwWorldId );
	if( it != m_mapDungeonData.end() )
		return it->second.dwCoolTime;

	return 0;
}

BOOL CInstanceDungeonBase::HasNowStateMonsterInfo( DWORD dwWorldId, DWORD dwDungeonId, DWORD dwMonsterId )
{
	INDUN_INFO * pInfo = GetDungeonInfo( dwDungeonId, dwWorldId );
	if( pInfo == NULL )
		return FALSE;

	const int nCurrStage	= GetCurrStage( dwWorldId, dwDungeonId );
	if( ( int )pInfo->vec2DPairMonsterList.size() <= nCurrStage )
		return FALSE;

	if( pInfo->vec2DPairMonsterList.at( nCurrStage ).size() < 0 )
		return FALSE;

	for( PairMonsterVec::const_iterator it = pInfo->vec2DPairMonsterList.at( nCurrStage ).begin()
		; it != pInfo->vec2DPairMonsterList.at( nCurrStage ).end()
		; ++it
		)
	{	
		const PAIR_MONSTER & monster = *it;
		if( pInfo->bCallRealMonster == FALSE && monster.dwFakeMonsterID == dwMonsterId )
			return TRUE;

		if( pInfo->bCallRealMonster == TRUE && monster.dwRealMonsterID == dwMonsterId )
			return TRUE;
	}

	return FALSE;


	//MAP_IDDATA::iterator it = m_mapDungeonData.find( dwWorldId );
	//if( it == m_mapDungeonData.end() )
	//	return FALSE;

	//int nNowState = GetCurrStage( dwWorldId, dwDungeonId );
	//for( std::vector<DUNGEON_DATA::MONSTER>::iterator i=it->second.vecMonster.begin(); i!=it->second.vecMonster.end(); i++ )
	//{
	//	if( (*i).nStage == nNowState && (*i).dwMonsterId == dwMonsterId )
	//		return TRUE;
	//}

	//return FALSE;
}


int CInstanceDungeonBase::GetObjCount_CurrStage( const DWORD dwWorldId, const DWORD dwDungeonId )
{
	INDUN_INFO* pInfo = GetDungeonInfo( dwDungeonId, dwWorldId );
	if( pInfo == NULL )
		return 0;

	if( ( int ) pInfo->vec2DPairMonsterList.size() <= pInfo->nCurrStage )
		return 0;

	return ( int )pInfo->vec2DPairMonsterList.at( pInfo->nCurrStage ).size();
}

void CInstanceDungeonBase::SetInstanceDungeonKill( DWORD dwWorldId, DWORD dwDungeonId, DWORD dwMonsterId )
{
	if( HasNowStateMonsterInfo( dwWorldId, dwDungeonId, dwMonsterId ) == TRUE )
	{
		INDUN_INFO* pInfo = GetDungeonInfo( dwDungeonId, dwWorldId );
		if( pInfo )
		{
			DUNGEON_DATA * pData	= GetDuneonData( *pInfo );
			if( pData == NULL )
				return;

			pInfo->nKillCount++;
			//if( pInfo->nKillCount >= GetObjCount( dwWorldId, dwDungeonId ) )
			if( pInfo->nKillCount >= GetObjCount_CurrStage( dwWorldId, dwDungeonId ) )
			{				
				if( pData->bColosseumStyle == TRUE )
				{
					CParty* pParty = g_PartyMng.GetParty( pInfo->dwDungeonID );
					if( pParty == NULL )
						return;

					for( int i = 0; i < pParty->GetSizeofMember(); ++i )
					{
						FLWSUser* pMember = g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( i ) );
						if( IsValidObj( pMember ) == FALSE )
							continue;	

						if( pMember->GetWorld() == NULL )
							continue;

						if( pMember->GetWorld()->GetID() == pInfo->dwWorldId )
							pMember->AddDefinedText( TID_COLOSSEUM_COLOSUCCESS01, "%d", pInfo->nCurrStage + 1 );
					}
				}

				SetNextStage( pInfo, dwDungeonId );			
			}
		}
	}
}

void CInstanceDungeonBase::DeleteDungeonCoolTimeInfo( DWORD dwPlayerId )
{
	MAP_CTINFO::iterator itMap = m_mapCTInfo.find( dwPlayerId );
	if( itMap != m_mapCTInfo.end() )
		m_mapCTInfo.erase( itMap );
}

CInstanceDungeonBase::DUNGEON_DATA * CInstanceDungeonBase::GetDuneonData( const INDUN_INFO & info )
{
	MAP_IDDATA::iterator it = m_mapDungeonData.find( info.dwWorldId );
	if( it == m_mapDungeonData.end() )
		return NULL;

	if( info.eDungeonLevel >= DUNGEON_LEVEL_MAX )
		return NULL;

	DUNGEON_DATA & data	= it->second;
	return &data;
}

CInstanceDungeonBase::DUNGEON_DATA * CInstanceDungeonBase::GetDuneonData( const DWORD dwWorldID, const DWORD dwDungeonLevel )
{
	if( dwDungeonLevel >= DUNGEON_LEVEL_MAX )
	{
		return NULL;
	}

	MAP_IDDATA::iterator it = m_mapDungeonData.find( dwWorldID );
	if( it == m_mapDungeonData.end() )
	{
		return NULL;
	}

	DUNGEON_DATA & data	= it->second;
	return &data;
}

BOOL CInstanceDungeonBase::HasLeader( INDUN_INFO & io_info )
{
	CParty* pParty = g_PartyMng.GetParty( io_info.dwDungeonID );
	if( pParty == NULL )
		return FALSE;

	CMover * pLeader		= pParty->GetLeader();				//오프라인하고 리더 이전 타이밍이 혹시 비동기인가?
	if( IsValidObj( pLeader ) == FALSE )
		return FALSE;

	if( pLeader->GetWorld() == NULL )
		return FALSE;

	if( pLeader->GetWorld()->GetID() != io_info.dwWorldId )
		return FALSE;

	return TRUE;
}

BOOL CInstanceDungeonBase::CheckAllPlayerDie( const DWORD dwWorldID, const DWORD dwDungeonID )
{
	CParty * pParty	= g_PartyMng.GetParty( dwDungeonID );
	if( pParty == NULL )
		return TRUE;

	for( int i = 0; i < pParty->m_nSizeofMember; ++i ) 
	{
		const u_long uPlayerID	= pParty->m_aMember[i].m_uPlayerId;
		FLWSUser *pUser			= g_xWSUserManager->GetUserByPlayerID( uPlayerID );	
		if( pUser == NULL )
			continue;

		if( IsValidObj( pUser ) == FALSE )
			continue;

		if( pUser->IsDie() == TRUE )
			continue;

		if( pUser->GetWorld() == NULL )
			continue;

		CWorld * pWorld			= pUser->GetWorld();
		if( pWorld->GetID() != dwWorldID )
			continue;

		return FALSE;
	}

	return TRUE;
}


BOOL CInstanceDungeonBase::UpdateWhenChangedPartyLeader( INDUN_INFO & info )
{
	CParty * pParty	= g_PartyMng.GetParty( info.dwDungeonID );
	if( pParty == NULL )
		return FALSE;

	CMover * pLeader = pParty->GetLeader();
	if( IsValidObj( pLeader ) == FALSE )
		return FALSE;

	if( info.idLeaderPlayer == pLeader->m_idPlayer )
		return FALSE;

	info.idLeaderPlayer		= pLeader->m_idPlayer;


	if( info.dwAddRealBossRate_ItemObjID != NULL_ID && info.idPlayer_UseAddRealBossRateItem != 0 && info.dwAddRealBossRate_ItemID != NULL_ID )
	{
		info.idPlayer_UseAddRealBossRateItem = NULL_ID;
		info.dwAddRealBossRate_ItemObjID = NULL_ID;
		info.idPlayer_UseAddRealBossRateItem = 0;

		for( int i = 0; i < pParty->GetSizeofMember(); ++i )
		{
			FLWSUser* pMember = g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( i ) );
			if( IsValidObj( pMember ) == FALSE )
				continue;

			if( pMember->GetWorld() == NULL )
				continue;

			if( pMember->GetWorld()->GetID() == info.dwWorldId )
				pMember->AddColosseum_RegistAdditionalRealMonsterRateItemNoti( NULL_ID );
		}
	}
	return TRUE;
}

#endif // __WORLDSERVER


//////////////////////////////////////////////////////////////////////
// CInstanceDungeonHelper
//////////////////////////////////////////////////////////////////////
#include "InstanceDungeonParty.h"

CInstanceDungeonHelper::CInstanceDungeonHelper()
{
#ifdef __WORLDSERVER
	m_uCertifyKey = NULL_ID;
	m_nIDLayerNum = 0;
#endif // __WORLDSERVER
}

CInstanceDungeonHelper::~CInstanceDungeonHelper()
{
#ifdef __WORLDSERVER
	m_mapDungeonType.clear();
#endif // __WORLDSERVER
}

CInstanceDungeonHelper* CInstanceDungeonHelper::GetInstance()
{
	static CInstanceDungeonHelper sIDH;
	return &sIDH;
}

CInstanceDungeonBase* CInstanceDungeonHelper::GetDungeonPtr( int nType )
{
	switch( nType )
	{
	case IDTYPE_PARTY : return CInstanceDungeonParty::GetInstance();
	}
	return NULL;
}

#ifdef __WORLDSERVER
void CInstanceDungeonHelper::IncreasePlayerCount( const DWORD dwWorldID, const DWORD dwDungeonId )
{
	CInstanceDungeonBase* pID = GetDungeonPtr( GetType( dwWorldID ) );
	if( pID )
		pID->IncreasePlayerCount( dwDungeonId, dwWorldID );
}
#endif //__WORLDSERVER

//void CInstanceDungeonHelper::OnCreateDungeon( int nType, INDUN_INFO & ID_Info, DWORD dwDungeonId, const DWORD dwPlayerID )
void CInstanceDungeonHelper::OnCreateDungeon( int nType, DWORD dwDungeonId, const DWORD dwPlayerID
											 , const DWORD dwWorld, const ULONG uMultiKey, const DWORD dwGuildID, const int eDungeonLevel )
{
#ifdef __WORLDSERVER
	//SetCertifyKey( ID_Info.uMultiKey );
	//SetCertifyKey( uMultiKey );
#endif // __WORLDSERVER
	CInstanceDungeonBase* pID = GetDungeonPtr( nType );
	if( pID == NULL )
		return;


	//INDUN_INFO * pInfo = pID->CreateDungeon( ID_Info, dwDungeonId, ID_Info.eDungeonLevel );
	INDUN_INFO * pInfo = pID->CreateDungeon( dwDungeonId, dwWorld, uMultiKey, dwGuildID, eDungeonLevel );
	if( pInfo == NULL )
	{
#ifdef __WORLDSERVER
		INDUN_INFO kIDInfo( dwWorld, uMultiKey, dwDungeonId );
		SendInstanceDungeonDestroy( pID->GetType(), dwDungeonId, kIDInfo );
#endif // __WORLDSERVER
		return;
	}

#ifdef __CORESERVER
	INDUN_INFO info( dwWorld, uMultiKey, dwDungeonId, dwGuildID, eDungeonLevel );
	SendInstanceDungeonCreate( nType, dwDungeonId, info, dwPlayerID );
	return;
#endif // __CORESERVER



#ifdef __WORLDSERVER
	// 텔레포트 ( 지역으로 입장하지 않고 NPC등으로 간접 입장하는 경우
	FLWSUser *pUser			= g_xWSUserManager->GetUserByPlayerID( dwPlayerID );	
	if( IsValidObj( pUser ) == FALSE )
	{
		SendInstanceDungeonDestroy( pID->GetType(), dwDungeonId, *pInfo );
		pID->DestroyDungeon( *pInfo, dwDungeonId );
		return;
	}

	if( pUser->GetWorld() == NULL )
	{
		SendInstanceDungeonDestroy( pID->GetType(), dwDungeonId, *pInfo );
		pID->DestroyDungeon( *pInfo, dwDungeonId );
		return;
	}

	if( pUser->GetWorld()->GetID() != pInfo->dwWorldId )
	{
		CInstanceDungeonBase::DUNGEON_DATA * pData	= pID->GetDuneonData( *pInfo );
		if( pData == NULL )
		{
			SendInstanceDungeonDestroy( pID->GetType(), dwDungeonId, *pInfo );
			pID->DestroyDungeon( *pInfo, dwDungeonId );
			return;
		}

		if( pID->EnteranceDungeon( pUser, pInfo->dwWorldId, pInfo->dwGuildID, pInfo->eDungeonLevel ) == FALSE )
		{
			SendInstanceDungeonDestroy( pID->GetType(), dwDungeonId, *pInfo );
			pID->DestroyDungeon( *pInfo, dwDungeonId );
			return;
		}
		//pUser->REPLACE( g_uIdofMulti, ID_Info.dwWorldId, pData->vStartPos, REPLACE_NORMAL, dwDungeonId );
	}
#endif // __WORLDSERVER
}

void CInstanceDungeonHelper::OnDestroyDungeon( int nType, INDUN_INFO & ID_Info, DWORD dwDungeonId )
{
	CInstanceDungeonBase* pID = GetDungeonPtr( nType );
	if( pID )
		pID->DestroyDungeon( ID_Info, dwDungeonId );
}

void CInstanceDungeonHelper::OnSetDungeonCoolTimeInfo( ULONG uKey, int nType, COOLTIME_INFO CT_Info, DWORD dwPlayerId )
{
#ifdef __WORLDSERVER
	UNREFERENCED_PARAMETER( uKey );
//	SetCertifyKey( uKey );
#endif // __WORLDSERVER
	CInstanceDungeonBase* pID = GetDungeonPtr( nType );
	if( pID )
	{
		pID->SetDungeonCoolTimeInfo( CT_Info, dwPlayerId );
#ifdef __CORESERVER
		SendInstanceDungeonSetCoolTimeInfo( uKey, nType, dwPlayerId, CT_Info );
		pID->DungeonCoolTimeCountProcess();
#endif // __CORESERVER
	}
}

void	CInstanceDungeonHelper::OnResetDungeonCoolTimeInfo( int nType, DWORD dwWorldId, DWORD dwDungeonId )
{
	CInstanceDungeonBase* pID = GetDungeonPtr( nType );
	if( pID != NULL )
	{
		pID->ResetDungeonCoolTimeInfo( dwWorldId, dwDungeonId );
	}
}

#ifdef __WORLDSERVER
void CInstanceDungeonHelper::OnColosseumFail( INDUN_INFO & info )
{
	GoOutAll( info.dwWorldId, info.dwDungeonID );
}
#else
void CInstanceDungeonHelper::OnColosseumFail( INDUN_INFO & /*info*/ )
{
}
#endif //__WORLDSERVER




void CInstanceDungeonHelper::SendInstanceDungeonCreate( int nType, DWORD dwDungeonId, INDUN_INFO & ID_Info, const DWORD dwPlayerID )
{
#ifdef __WORLDSERVER
	//if( !IsCertifying() )
	{
		//SetCertifyKey();
 		g_DPCoreClient.SendInstanceDungeonCreate( nType, dwDungeonId, ID_Info, dwPlayerID );
	}
#endif // __WORLDSERVER
#ifdef __CORESERVER
	g_dpCoreSrvr.SendInstanceDungeonCreate( nType, dwDungeonId, ID_Info, dwPlayerID );
#endif// __CORESERVER
}

void CInstanceDungeonHelper::SendInstanceDungeonDestroy( int nType, DWORD dwDungeonId, INDUN_INFO & ID_Info )
{
#ifdef __WORLDSERVER
	g_DPCoreClient.SendInstanceDungeonDestroy( nType, dwDungeonId, ID_Info );
#endif // __WORLDSERVER
#ifdef __CORESERVER
	g_dpCoreSrvr.SendInstanceDungeonDestroy( nType, dwDungeonId, ID_Info );
#endif// __CORESERVER
}

void CInstanceDungeonHelper::SendInstanceDungeonSetCoolTimeInfo( ULONG uKey, int nType, DWORD dwPlayerId, COOLTIME_INFO & CT_Info )
{
#ifdef __WORLDSERVER
	UNREFERENCED_PARAMETER( uKey );

	//if( !IsCertifying() )
	{
		//SetCertifyKey();
		g_DPCoreClient.SendInstanceDungeonSetCoolTimeInfo( nType, dwPlayerId, CT_Info );
	}
#endif // __WORLDSERVER
#ifdef __CORESERVER
	g_dpCoreSrvr.SendInstanceDungeonSetCoolTimeInfo( uKey, nType, dwPlayerId, CT_Info );
#endif// __CORESERVER
}

void	CInstanceDungeonHelper::SendInstanceDungeonResetCoolTimeInfo( const int nType, const DWORD dwWorldId, const DWORD dwDungeonId )
{
#ifdef __WORLDSERVER
	g_DPCoreClient.SendInstanceDungeonResetCoolTimeInfo( nType, dwWorldId, dwDungeonId );
#endif // __WORLDSERVER
#ifdef __CORESERVER
	g_dpCoreSrvr.SendInstanceDungeonResetCoolTimeInfo( nType, dwWorldId, dwDungeonId );
#endif// __CORESERVER
}

void CInstanceDungeonHelper::DestroyAllDungeonByMultiKey( ULONG uMultiKey )
{
	for( int i=IDTYPE_SOLO; i<IDTYPE_MAX; i++ )
	{
		CInstanceDungeonBase* pID = GetDungeonPtr( i );
		if( pID )
			pID->DestroyAllDungeonByMultiKey( uMultiKey );
	}
}

#ifdef __CORESERVER
void CInstanceDungeonHelper::SendInstanceDungeonAllInfo( DWORD dpId )
{
	for( int i=IDTYPE_SOLO; i<IDTYPE_MAX; i++ )
	{
		CInstanceDungeonBase* pID = GetDungeonPtr( i );
		if( pID )
			g_dpCoreSrvr.SendInstanceDungeonAllInfo( i, pID, dpId );
	}
}

void CInstanceDungeonHelper::SendInstanceDungeonDeleteCoolTimeInfo( int nType, DWORD dwPlayerId )
{
	g_dpCoreSrvr.SendInstanceDungeonDeleteCoolTimeInfo( nType, dwPlayerId );
}
#endif // __CORESERVER

#ifdef __WORLDSERVER
void CInstanceDungeonHelper::OnInstanceDungeonAllInfo( CAr & ar )
{
	int nType;
	ar >> nType;
	CInstanceDungeonBase* pID = GetDungeonPtr( nType );
	if( pID )
		pID->SerializeAllInfo( ar );
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Wrong Party Type - %d" ), nType );
		exit( 0 );
	}
}

//void CInstanceDungeonHelper::SetCertifyKey( ULONG uMultiKey )
//{
//	if( uMultiKey == NULL_ID )
//		m_uCertifyKey = g_uKey;
//	else if( m_uCertifyKey == uMultiKey )
//		m_uCertifyKey = NULL_ID;
//}

void CInstanceDungeonHelper::SetDungeonType( DWORD dwWorldId, int nType )
{
	m_mapDungeonType.insert( MAP_WORLD2TYPE::value_type( dwWorldId, nType ) );
}

int CInstanceDungeonHelper::GetType( DWORD dwWorldId )
{
	MAP_WORLD2TYPE::iterator it = m_mapDungeonType.find( dwWorldId );
	if( it == m_mapDungeonType.end() )
		return NULL_ID;

	return it->second;
}

BOOL CInstanceDungeonHelper::GetDungeonLevel( const DWORD dwWorldID, const DWORD dwDungeonID, DWORD & o_dwDungoenLevel )
{
	CInstanceDungeonBase* pID = GetDungeonPtr( GetType( dwWorldID ) );
	if( pID == NULL )
		return FALSE;

	return pID->GetDungeonLevel( dwWorldID, dwDungeonID, o_dwDungoenLevel );
}


BOOL	CInstanceDungeonHelper::SetAdditinalRealMonsterRate( FLWSUser* pUser, const DWORD n10000AdditionalRate )
{
	if( IsValidObj( pUser ) == FALSE )
		return FALSE;

	if( pUser->GetWorld() == NULL )
		return FALSE;


	const DWORD dwWorldID		= pUser->GetWorld()->GetID();
	const DWORD dwDungeonID		= pUser->GetPartyId();

	CInstanceDungeonBase* pID = GetDungeonPtr( GetType( dwWorldID ) );
	if( pID == NULL )
		return FALSE;

	INDUN_INFO * pInfo		= pID->GetDungeonInfo( dwDungeonID, dwWorldID );	//인던이 없음
	if( pInfo == NULL )
		return FALSE;

	if( pInfo->dw10000AdditionalRealMonsterRate != 0 )				//중복 사용
		return FALSE;

	CInstanceDungeonBase::DUNGEON_DATA * pData = pID->GetDuneonData( *pInfo );					//인던 DATA가 없음
	if( pData == NULL )
	{
		return FALSE;
	}

	if( pData->bColosseumStyle == FALSE )							//콜로세움 스타일이 아님.
	{
		return FALSE;
	}

	pInfo->dw10000AdditionalRealMonsterRate = n10000AdditionalRate;
	return TRUE;
}

BOOL	CInstanceDungeonHelper::RetryCurrStage( FLWSUser & User )
{
	if( IsValidObj( &User ) == FALSE )
		return FALSE;

	if( User.GetWorld() == NULL )
		return FALSE;

	const DWORD dwWorldID		= User.GetWorld()->GetID();
	const DWORD dwDungeonID		= User.GetPartyId();

	CInstanceDungeonBase* pID = GetDungeonPtr( GetType( dwWorldID ) );
	if( pID == NULL )
		return FALSE;

	const u_long nPartyID	= User.GetPartyId();
	CParty* pParty = g_PartyMng.GetParty( nPartyID );
	if( pParty == NULL )
		return FALSE;
	//파티 장인지.
	if( pParty->IsLeader( User.m_idPlayer ) == FALSE )											//리더가 아니면 사용할수 없음
		return FALSE;

	//	const DWORD dwDungeonID = nPartyID;
	//파티인지 자동 체크 됨.
	INDUN_INFO * pInfo			= pID->GetDungeonInfo( dwDungeonID, User.GetWorld()->GetID() );		//인던이 없음
	if( pInfo == NULL )
		return FALSE;

	const CInstanceDungeonBase::DUNGEON_DATA * pData = pID->GetDuneonData( *pInfo );			//인던 정보가 없음
	if( pData == NULL )
		return FALSE;

	if( pData->bColosseumStyle == FALSE )																	//콜로세움 인던이 아님
		return FALSE;

	// Retry 상태 이면
	if( pInfo->m_pState != &pInfo->RetriedStageWaitingState && pInfo->m_pState != &pInfo->AllDieWaitingState )		//RETRY할 수 있는 상태가 아님
		return FALSE;

	//pUser->AddColosseum_Retry( T_PACKET_COLOSSEUM_RETRY_ACK::RST_SUCESS );
	pInfo->SetState( & pInfo->RetriedStageWaitingState, pData->arrLevelProp[ pInfo->eDungeonLevel ].dwRetryStartStage_WaitingTick );
	return TRUE;
}

BOOL CInstanceDungeonHelper::RegistAdditionalRealMonsterRateItem( FLWSUser & User, const DWORD dwAddRealBossRate_ItemObjID )
{
	if( IsValidObj( &User ) == FALSE )
		return FALSE;

	if( User.GetWorld() == NULL )
		return FALSE;

	const DWORD dwWorldID		= User.GetWorld()->GetID();
	const DWORD dwDungeonID		= User.GetPartyId();

	CInstanceDungeonBase* pID	= GetDungeonPtr( GetType( dwWorldID ) );
	if( pID == NULL )
		return FALSE;

	const u_long nPartyID		= User.GetPartyId();
	CParty* pParty = g_PartyMng.GetParty( nPartyID );
	if( pParty == NULL )
		return FALSE;

	//파티 장인지.
	if( pParty->IsLeader( User.m_idPlayer ) == FALSE )											//리더가 아니면 사용할수 없음
		return FALSE;

	//파티인지 자동 체크 됨.
	INDUN_INFO * pInfo				= pID->GetDungeonInfo( dwDungeonID, User.GetWorld()->GetID() );		//인던이 없음
	if( pInfo == NULL )
		return FALSE;

	if( pInfo->m_pState != &pInfo->AutoStartState )
	{
		User.AddDefinedText( TID_COLOSSEUM_CANT_USE_ITEM );
		return FALSE;
	}

	const CInstanceDungeonBase::DUNGEON_DATA * pData = pID->GetDuneonData( *pInfo );			//인던 정보가 없음
	if( pData == NULL )
		return FALSE;

	if( pData->bColosseumStyle == FALSE )																	//콜로세움 인던이 아님
		return FALSE;

	FLItemElem* pItem	= User.m_Inventory.GetAtId( dwAddRealBossRate_ItemObjID );
	if( pItem == NULL )
		return FALSE;



	pInfo->dwAddRealBossRate_ItemID				= pItem->GetItemIndex();
	pInfo->idPlayer_UseAddRealBossRateItem		= User.m_idPlayer;
	pInfo->dwAddRealBossRate_ItemObjID			= dwAddRealBossRate_ItemObjID;


	for( int i = 0; i < pParty->GetSizeofMember(); ++i )
	{
		FLWSUser* pMember = g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( i ) );
		if( IsValidObj( pMember ) == FALSE )
			continue;

		if( pMember->GetWorld() == NULL )
			continue;

		if( pMember->GetWorld()->GetID() == pInfo->dwWorldId )
			pMember->AddColosseum_RegistAdditionalRealMonsterRateItemNoti( pItem->GetItemIndex() );
	}
	return TRUE;
}

BOOL CInstanceDungeonHelper::UnRegistAdditionalRealMonsterRateItem( FLWSUser & User )
{
	if( IsValidObj( &User ) == FALSE )
		return FALSE;

	if( User.GetWorld() == NULL )
		return FALSE;

	const DWORD dwWorldID		= User.GetWorld()->GetID();
	const DWORD dwDungeonID		= User.GetPartyId();

	CInstanceDungeonBase* pID = GetDungeonPtr( GetType( dwWorldID ) );
	if( pID == NULL )
		return FALSE;

	const u_long nPartyID		= User.GetPartyId();
	CParty* pParty = g_PartyMng.GetParty( nPartyID );
	if( pParty == NULL )
		return FALSE;

	//파티 장인지.
	if( pParty->IsLeader( User.m_idPlayer ) == FALSE )											//리더가 아니면 사용할수 없음
		return FALSE;

	//파티인지 자동 체크 됨.
	INDUN_INFO * pInfo			= pID->GetDungeonInfo( dwDungeonID, User.GetWorld()->GetID() );		//인던이 없음
	if( pInfo == NULL )
		return FALSE;

	const CInstanceDungeonBase::DUNGEON_DATA * pData = pID->GetDuneonData( *pInfo );			//인던 정보가 없음
	if( pData == NULL )
		return FALSE;

	if( pData->bColosseumStyle == FALSE )																	//콜로세움 인던이 아님
		return FALSE;

	pInfo->dwAddRealBossRate_ItemID				= NULL_ID;
	pInfo->idPlayer_UseAddRealBossRateItem		= 0;
	pInfo->dwAddRealBossRate_ItemObjID			= NULL_ID;

	for( int i = 0; i < pParty->GetSizeofMember(); ++i )
	{
		FLWSUser* pMember = g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( i ) );
		if( IsValidObj( pMember ) == FALSE )
			continue;

		if( pMember->GetWorld() == NULL )
			continue;

		if( pMember->GetWorld()->GetID() == pInfo->dwWorldId )
			pMember->AddColosseum_RegistAdditionalRealMonsterRateItemNoti( NULL_ID );
	}

	return TRUE;
}

//BOOL	CInstanceDungeonHelper::IsPartyDungeon( FLWSUser* pUser, DWORD dwWorldId  )
//{
//	if( IsValidObj( pUser ) == FALSE )
//		return FALSE;
//
//	const DWORD dwDungeonID		= User->GetPartyId();
//
//	CInstanceDungeonBase* pID = GetDungeonPtr( GetType( dwWorldId ) );
//	if( pID == NULL )
//		return FALSE;
//
//	/*const u_long nPartyID		= pUser->GetPartyId();
//	CParty* pParty = g_PartyMng.GetParty( nPartyID );
//	if( pParty == NULL )
//		return FALSE;*/
//
//	//파티인지 자동 체크 됨.
//	INDUN_INFO * pInfo			= pID->GetDungeonInfo( dwDungeonID, pUser->GetWorld()->GetID() );		//인던이 없음
//	if( pInfo == NULL )
//		return FALSE;
//
//	DUNGEON_DATA * pData	= GetDuneonData( *pInfo );
//	if( pData == NULL )
//		return FALSE;
//	return TRUE;
//}

BOOL CInstanceDungeonHelper::EnteranceDungeon( FLWSUser* pUser, DWORD dwWorldId, const DWORD dwGuildID, DWORD eDungeonLevel )
{
	CInstanceDungeonBase* pID = GetDungeonPtr( GetType( dwWorldId ) );
	if( IsValidObj( pUser ) && pID )
	{
		return pID->EnteranceDungeon( pUser, dwWorldId, dwGuildID, eDungeonLevel );
	}

	return FALSE;
}

BOOL CInstanceDungeonHelper::LeaveDungeon( FLWSUser* pUser, DWORD dwWorldId )
{
	CInstanceDungeonBase* pID = GetDungeonPtr( GetType( dwWorldId ) );
	if( pID )
	{
		if( pID->LeaveDungeon( pUser, dwWorldId ) )
			return TRUE;
	}

	return FALSE;
}

// OnJoin()에서만 dwWorldId 에 값이 존재한다. 해당 던전에서 입장 캐릭터수를 증가시키지 않고 강퇴시키기위해 입장 캐릭터수를 미리 증가시킨다.
void CInstanceDungeonHelper::GoOut( FLWSUser* pUser )
{
	if( pUser->GetWorld() == NULL )
		return;

	pUser->REPLACE( g_uIdofMulti, pUser->m_idMarkingWorld, pUser->m_vMarkingPos, REPLACE_FORCE, nDefaultLayer );
}

void CInstanceDungeonHelper::GoOutAll( const DWORD dwWorldID, const DWORD dwDungeonID )
{
	CParty* pParty = g_PartyMng.GetParty( dwDungeonID );
	if( pParty == NULL )
		return;

	for( int i = 0; i < pParty->GetSizeofMember(); ++i )
	{
		FLWSUser* pMember = g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( i ) );
		if( IsValidObj( pMember ) == FALSE )
			continue;

		if( pMember->GetWorld() == NULL )
			continue;

		if( IsInstanceDungeon( dwWorldID ) == FALSE )
			continue;

		if( pMember->IsDie() == TRUE )
			pMember->TryRevival( 100.0f );

		if( pMember->GetWorld()->GetID() == dwWorldID )
		{
			GoOut( pMember );
			pMember->AddDefinedText( TID_COLOSSEUM_TIMEOVERCOLOGATE01 );
		}
	}
}

void CInstanceDungeonHelper::SetInstanceDungeonKill( DWORD dwWorldId, DWORD dwDungeonId, DWORD dwMonsterId )
{
	CInstanceDungeonBase* pID = GetDungeonPtr( GetType( dwWorldId ) );
	if( pID )
		pID->SetInstanceDungeonKill( dwWorldId, dwDungeonId, dwMonsterId );
}

void CInstanceDungeonHelper::OnDeleteDungeonCoolTimeInfo( int nType, DWORD dwPlayerId )
{
	CInstanceDungeonBase* pID = GetDungeonPtr( nType );
	if( pID )
		pID->DeleteDungeonCoolTimeInfo( dwPlayerId );
}

int	CInstanceDungeonHelper::GetDungeonKind( FLWSUser* pUser )
{
	if( IsValidObj( pUser ) == TRUE )
	{
		CWorld* pWorld = pUser->GetWorld();
		if( pWorld != NULL )
		{
			CInstanceDungeonBase* pID = GetDungeonPtr( GetType( pWorld->GetID() ) );
			if( pID != NULL )
			{
				return pID->GetDungeonKind( pUser );
			}
		}
	}

	return IDKIND_GENERAL;
}

#ifdef __WORLDSERVER
void		CInstanceDungeonHelper::DungeonFailLogQuery( const INDUN_INFO & info )
{
	//콜로세움 실패 로그
	CInstanceDungeonBase::DUNGEON_DATA * pData = CInstanceDungeonParty::GetInstance()->GetDuneonData( info );	
	if( pData == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID WORLDID(%u) ]" ), info.dwWorldId );
		return;
	}

	if( pData->bColosseumStyle == FALSE )
		return;

	if( info.dwGuildID == 0 || info.dwGuildID == NULL_ID )
		return;

	BEFORESENDDUAL( ar, PACKETTYPE_RECORD_DUNGEON_PLAY_INFO, DPID_UNKNOWN, DPID_UNKNOWN );
	T_W2DB_PACKET_RECORD_DUNGEON_PLAY_INFO noti;
	noti.dwGuildID			= info.dwGuildID;
	noti.dwWorldID			= info.dwWorldId;
	noti.eDungeonLevel		= info.eDungeonLevel;
	noti.dwCompleteTimeTick	= info.dwTurnaroundTime;
	noti.tmStartedDate		= info.tmStartedDate;	
	noti.tmFinishedDate		= DATE_TIMER().GetCurrentTime();
	noti.bSuccess			= FALSE;

	ar.Write( & noti, sizeof( noti ) );

	SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );	
}

void	CInstanceDungeonHelper::OnDungeonBestCompleteTimeAck( const T_W2DB_PACKET_RECORD_DUNGEON_PLAY_INFO & fromTrans )
{
	CInstanceDungeonBase::DUNGEON_DATA * pData = CInstanceDungeonParty::GetInstance()->GetDuneonData( fromTrans.dwWorldID, fromTrans.eDungeonLevel );
	if( pData == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID WORLD_ID(%u), DUNGEON_LEVEL(%u) ]" ), fromTrans.dwWorldID, fromTrans.eDungeonLevel );
		return;
	}

	CParty* pParty = g_PartyMng.GetParty( fromTrans.dwPartyID );
	if( pParty == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID PARTY_ID(%u) ]" ), fromTrans.dwPartyID );
		return;
	}

	T_PACKET_COLOSSEUM_ALL_CLEAR_HEADER toClient( pData->arrLevelProp[ fromTrans.eDungeonLevel ].dwComplete_WaitingTick
		, fromTrans.dwCompleteTimeTick, true, fromTrans.nWeeklyRank );

	if( fromTrans.nWeeklyRank <= 0 )
	{
		FLWARNING_LOG( PROGRAM_NAME, _T( "[ INVALID WEEKLY RANKING INFO. RANKING(%d), GUILD(%06u), WORLD(%u), LEVEL(%d) ]" )
			, fromTrans.nWeeklyRank, fromTrans.dwGuildID, fromTrans.dwWorldID, fromTrans.eDungeonLevel );
		toClient.bDisplayRank	= false;
		toClient.nWeeklyRank	= 0;
	}

	for( int Nth = 0; Nth < pParty->GetSizeofMember(); ++Nth )
	{
		FLWSUser* pMember = g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( Nth ) );
		if( IsValidObj( pMember ) == FALSE )
			continue;

		if( pMember->GetWorld() == NULL )
			continue;

		if( pMember->GetWorld()->GetID() == fromTrans.dwWorldID )
		{
			pMember->AddDefinedText( TID_COLOSSEUM_COLOINVITE01 );
			pMember->AddColosseum_AllClear( toClient );
		}

// 		typedef CInstanceDungeonBase::DUNGEON_DATA::FLLevelProp::RewardVec  RewardVec;
// 		for( RewardVec::iterator it = pData->arrLevelProp[ dwDungeonLevel ].vecReward.begin()
// 			; it != pData->arrLevelProp[ dwDungeonLevel ].vecReward.end()
// 			; ++it )
// 		{
// 			IDungeonReward * p  = *it;
// 			p->Reward( info, *pMember );
// 		}
	}
}
#endif __WORLDSERVER


//-------------------------------------------------- 상태 ------------------------------------------------------------------//
void ITimeAlarmState::OnUpdate( INDUN_INFO & info/*, const DWORD dwDungeonID*/ )
{
	if( m_bEnable == TRUE && m_dwAlarmTick < ::GetTickCount() )
	{
		OnAlarm( info/*, info.dwDungeonID*/ );
		m_bEnable		= FALSE;
	}
}

void ITimeAlarmState::SetAlarmInterval( const DWORD dwAlarmInterval )
{
	m_bEnable			= TRUE;
	m_dwAlarmTick		= dwAlarmInterval + ::GetTickCount();
}

//-----------------------------------------------------------------------------------------------------------------------//

void FLAutoStartState::OnEnter( INDUN_INFO & info/*, const DWORD dwDungeonID*/ )
{
	CInstanceDungeonBase::DUNGEON_DATA * pData = CInstanceDungeonParty::GetInstance()->GetDuneonData( info );	//@@@@@@@ 상위 객체 참조 -_-;
	if( pData == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID WORLDID(%u) ]" ), info.dwWorldId );
		info.SetState( NULL, 0/*, dwDungeonID*/ );
		return;
	}


	CParty* pParty = g_PartyMng.GetParty( info.dwDungeonID );
	if( pParty == NULL )
		return;

	CMover * pLeader		= pParty->GetLeader();
	if( IsValidObj( pLeader ) == FALSE )
		return;

	for( int i = 0; i < pParty->GetSizeofMember(); ++i )
	{
		FLWSUser * pMember = g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( i ) );
		if( IsValidObj( pMember ) == FALSE )
			continue;

		if( pMember->GetWorld() == NULL )
			continue;

		if( pParty->IsLeader( pMember->m_idPlayer ) == FALSE )
		{
			if( info.dwGuildID == 0 )
				pMember->AddColosseum_AutoInviteReq( pLeader->GetName(), info.dwWorldId );		//초대
			else 
			{
				if( pMember->GetGuild() != NULL && info.dwGuildID == pMember->GetGuild()->GetGuildId() )
				{
					pMember->AddColosseum_AutoInviteReq( pLeader->GetName(), info.dwWorldId );		//초대
				}
			}
			//pMember->AddColosseum_AutoStartWaitingTick( pData->arrLevelProp[ info.eDungeonLevel ].dwAutoStart_WaitingTick );
		}

	}
}

void FLAutoStartState::OnLeave( INDUN_INFO & /*info*//*, const DWORD dwDungeonID*/ )
{
	__noop;
}

void FLAutoStartState::OnAlarm( INDUN_INFO & info/*, const DWORD dwDungeonID*/ )
{
	CInstanceDungeonBase::DUNGEON_DATA * pData = CInstanceDungeonParty::GetInstance()->GetDuneonData( info );	//@@@@@@@ 상위 객체 참조 -_-;
	if( pData == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID WORLDID(%u) ]" ), info.dwWorldId );
		info.SetState( NULL, 0/*, dwDungeonID*/ );
		return;
	}

//--------------------------------------------------------------------------------------------------------//

	//info.StageWaitingState.SetAlarmInterval( pData->arrLevelProp[ info.eDungeonLevel ].dwStartStage_WaitingTick );
	info.SetState( & info.StageWaitingState, pData->arrLevelProp[ info.eDungeonLevel ].dwStartStage_WaitingTick/*, dwDungeonID*/ );
}


//-----------------------------------------------------------------------------------------------------------------------//

void FLStageWaitingState::OnEnter( INDUN_INFO & info/*, const DWORD dwDungeonID*/ )
{
	CInstanceDungeonBase::DUNGEON_DATA * pData = CInstanceDungeonParty::GetInstance()->GetDuneonData( info );	//@@@@@@@ 상위 객체 참조 -_-;
	if( pData == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID WORLDID(%u) ]" ), info.dwWorldId );
		info.SetState( NULL, 0/*, dwDungeonID*/ );
		return;
	}

//---------------------------------------------------------------
	if( pData->bColosseumStyle == TRUE )
	{
		CParty* pParty = g_PartyMng.GetParty( info.dwDungeonID );
		if( pParty == NULL )
			return;

		for( int i = 0; i < pParty->GetSizeofMember(); ++i )
		{
			FLWSUser* pMember = g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( i ) );
			if( IsValidObj( pMember ) == FALSE )
				continue;

			if( pMember->GetWorld() == NULL )
				continue;

			if( pMember->GetWorld()->GetID() == info.dwWorldId )
			{
				pMember->AddColosseum_StageWaitingTick( info.nCurrStage, pData->arrLevelProp[ info.eDungeonLevel ].dwStartStage_WaitingTick );
				//pMember->AddDefinedText( TID_COLOSSEUM_WATINGSTAGESTART01, "%d", info.nCurrStage );
			}
		}
	}

//---------------------------------------------------------------
	//info.bNotEnter			= TRUE;
//	info.bRealMonster		= FALSE;
	CInstanceDungeonParty::GetInstance()->Teleport( info/*, info.dwDungeonID*/ );

}

void FLStageWaitingState::OnLeave( INDUN_INFO & /*info*//*, const DWORD dwDungeonID*/ )
{
	__noop;
}

void FLStageWaitingState::OnAlarm( INDUN_INFO & info/*, const DWORD dwDungeonID*/ )
{
	CInstanceDungeonBase::DUNGEON_DATA * pData = CInstanceDungeonParty::GetInstance()->GetDuneonData( info );	//@@@@@@@ 상위 객체 참조 -_-;
	if( pData == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID WORLDID(%u) ]" ), info.dwWorldId );
		info.SetState( NULL, 0/*, dwDungeonID*/ );
		return;
	}

	// 진 몬스터 확률 증가 아이템 자동 사용
	FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( info.idPlayer_UseAddRealBossRateItem );
	if( IsValidObj( pUser ) == TRUE )
	{
		FLItemElem* pAdditionalRateItem	= pUser->m_Inventory.GetAtId( info.dwAddRealBossRate_ItemObjID );
		if( pAdditionalRateItem != NULL )
		{
			const int nResult	= g_xUseActiveItem->OnDoUseActiveItem( pUser, pAdditionalRateItem );
			if( nResult == FLUseActiveItem::ERR_SUCCESS_COLOSSEUM_BOSS_RATE )
			{
				g_DPSrvr.PutItemLog( pUser, "N", "ColosseumBossRate_Use", pAdditionalRateItem );
				g_pItemUsing->CompleteUseItem( pUser, pAdditionalRateItem->m_dwObjId );
			}
            info.idPlayer_UseAddRealBossRateItem		= 0;	
			info.dwAddRealBossRate_ItemObjID			= NULL_ID;
		}
	}

	// 몬스터 젠
	CInstanceDungeonParty::GetInstance()->RespawnMonster_AndTrySwitchToStageLimit( info.dwDungeonID, info.dwWorldId );
}


//--------------------------------------------------------------------------------------//

void FLStageLimitState::OnEnter( INDUN_INFO & info/*, const DWORD dwDungeonID*/ )
{
	CInstanceDungeonBase::DUNGEON_DATA * pData = CInstanceDungeonParty::GetInstance()->GetDuneonData( info );	//@@@@@@@ 상위 객체 참조 -_-;
	if( pData == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID WORLDID(%u) ]" ), info.dwWorldId );
		info.SetState( NULL, 0/*, dwDungeonID*/ );
		return;
	}

	info.dwStartStageTick	= ::GetTickCount();

	//---------------------------------------------------------------
	if( pData->bColosseumStyle == FALSE )
		return;


	//스테이지 시작 메세지 보내기
	CParty* pParty = g_PartyMng.GetParty( info.dwDungeonID );
	if( pParty == NULL )
		return;

	for( int i = 0; i < pParty->GetSizeofMember(); ++i )
	{
		FLWSUser* pMember = g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( i ) );
		if( IsValidObj( pMember ) == FALSE )
			continue;

		if( pMember->GetWorld() == NULL )
			continue;

		if( pMember->GetWorld()->GetID() == info.dwWorldId )
		{
			pMember->AddColosseum_StatedStage( info.nCurrStage, info.dwTimeLimitTick );
		}
	}


}

void FLStageLimitState::OnLeave( INDUN_INFO & /*info*//*, const DWORD dwDungeonID*/ )
{
	__noop;
}

void FLStageLimitState::OnAlarm( INDUN_INFO & info/*, const DWORD dwDungeonID*/ )
{
	CInstanceDungeonBase::DUNGEON_DATA * pData = CInstanceDungeonParty::GetInstance()->GetDuneonData( info );	//@@@@@@@ 상위 객체 참조 -_-;
	if( pData == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID WORLDID(%u) ]" ), info.dwWorldId );
		info.SetState( NULL, 0/*, dwDungeonID*/ );
		return;
	}

//------------------------------------------------------------------------
	if( pData->bColosseumStyle == TRUE )
	{


		CParty* pParty = g_PartyMng.GetParty( info.dwDungeonID );
		if( pParty == NULL )
			return;

		for( int i = 0; i < pParty->GetSizeofMember(); ++i )
		{
			FLWSUser* pMember = g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( i ) );
			if( IsValidObj( pMember ) == FALSE )
				continue;

			if( pMember->GetWorld() == NULL )
				continue;

			if( pMember->GetWorld()->GetID() == info.dwWorldId )
			{
				pMember->AddColosseum_StageTimeLimit( info.nCurrStage, info.dwTimeLimitTick );
			}
		}
	}

	//info.RetriedStageWaitingState.SetAlarmInterval( pData->arrLevelProp[ info.eDungeonLevel ].dwRetry_WaitingTick );
	info.SetState( & info.ExpiredTimeWaitingState, pData->arrLevelProp[ info.eDungeonLevel ].dwRetry_WaitingTick/*, dwDungeonID*/ );
}

//--------------------------------------------------------------------------------------//

void FLRetriedStageWaitingState::OnEnter( INDUN_INFO & info/*, const DWORD dwDungeonID*/ )
{
	CInstanceDungeonParty::GetInstance()->ClearCalledMonster( info.dwDungeonID, info.dwWorldId );

	CInstanceDungeonBase::DUNGEON_DATA * pData = CInstanceDungeonParty::GetInstance()->GetDuneonData( info );	//@@@@@@@ 상위 객체 참조 -_-;
	if( pData == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID WORLDID(%u) ]" ), info.dwWorldId );
		info.SetState( NULL, 0/*, dwDungeonID*/ );
		return;
	}

	//------------------------------------------------------------------------
	if( pData->bColosseumStyle == TRUE )
	{
		CParty* pParty = g_PartyMng.GetParty( info.dwDungeonID );
		if( pParty == NULL )
			return;

		for( int i = 0; i < pParty->GetSizeofMember(); ++i )
		{
			FLWSUser* pMember = g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( i ) );
			if( IsValidObj( pMember ) == FALSE )
				continue;

			if( pMember->GetWorld() == NULL )
				continue;

			if( pMember->GetWorld()->GetID() == info.dwWorldId )
			{	
				if( pMember->IsDie() == TRUE )
				{
					pMember->TryRevival( 100.0f );
				}

				pMember->AddColosseum_Retried( info.nCurrStage, pData->arrLevelProp[ info.eDungeonLevel ].dwRetryStartStage_WaitingTick );
				//pMember->AddDefinedText( TID_COLOSSEUM_WATINGSTAGERESTART01, "%d", info.nCurrStage + 1 );
			}
		}
	}
	
}

void FLRetriedStageWaitingState::OnLeave( INDUN_INFO & /*info*//*, const DWORD dwDungeonID*/ )
{
	__noop;
}

void FLRetriedStageWaitingState::OnAlarm( INDUN_INFO & info/*, const DWORD dwDungeonID*/ )
{
	CInstanceDungeonBase::DUNGEON_DATA * pData = CInstanceDungeonParty::GetInstance()->GetDuneonData( info );	//@@@@@@@ 상위 객체 참조 -_-;
	if( pData == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID WORLDID(%u) ]" ), info.dwWorldId );
		info.SetState( NULL, 0/*, dwDungeonID*/ );
		return;
	}

	//------------------------------------------------------------------------
	if( pData->bColosseumStyle == TRUE )
	{
		CInstanceDungeonParty::GetInstance()->RespawnMonster_AndTrySwitchToStageLimit( info.dwDungeonID, info.dwWorldId );
	}

	//info.SetState( NULL, 0/*, dwDungeonID*/ );
}
//------------------------------------------------------------------------------------------------------//

void FLAllDieWaitingState::OnEnter( INDUN_INFO & info/*, const DWORD dwDungeonID*/ )
{	
	CInstanceDungeonParty::GetInstance()->ClearCalledMonster( info.dwDungeonID, info.dwWorldId );

	CInstanceDungeonBase::DUNGEON_DATA * pData = CInstanceDungeonParty::GetInstance()->GetDuneonData( info );	//@@@@@@@ 상위 객체 참조 -_-;
	if( pData == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID WORLDID(%u) ]" ), info.dwWorldId );
		info.SetState( NULL, 0/*, dwDungeonID*/ );
		return;
	}

	//------------------------------------------------------------------------
	if( pData->bColosseumStyle == TRUE )
	{
		CParty* pParty = g_PartyMng.GetParty( info.dwDungeonID );
		if( pParty == NULL )
			return;

		for( int i = 0; i < pParty->GetSizeofMember(); ++i )
		{
			FLWSUser* pMember = g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( i ) );
			if( IsValidObj( pMember ) == FALSE )
				continue;

			if( pMember->GetWorld() == NULL )
				continue;

			if( pMember->GetWorld()->GetID() == info.dwWorldId )
			{
				pMember->AddColosseum_AllDie( info.nCurrStage, pData->arrLevelProp[ info.eDungeonLevel ].dwRetry_WaitingTick );
			}
		}
	}

}

void FLAllDieWaitingState::OnLeave( INDUN_INFO & /*info*//*, const DWORD dwDungeonID*/ )
{
	__noop;
}

void FLAllDieWaitingState::OnAlarm( INDUN_INFO & info/*, const DWORD dwDungeonID*/ )
{
	CInstanceDungeonBase::DUNGEON_DATA * pData = CInstanceDungeonParty::GetInstance()->GetDuneonData( info );	//@@@@@@@ 상위 객체 참조 -_-;
	if( pData == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID WORLDID(%u) ]" ), info.dwWorldId );
		info.SetState( NULL, 0 );
		return;
	}

	//------------------------------------------------------------------------
	if( pData->bColosseumStyle == TRUE )
	{
		CInstanceDungeonHelper::GetInstance()->OnColosseumFail( info );
		info.SetState( NULL, 0 );
	}

	info.SetState( NULL, 0 );
}

//------------------------------------------------------------------------------------------------------//
void FLExpiredTimeWaitingState::OnEnter( INDUN_INFO & info )
{
	CInstanceDungeonParty::GetInstance()->ClearCalledMonster( info.dwDungeonID, info.dwWorldId );

	CInstanceDungeonBase::DUNGEON_DATA * pData = CInstanceDungeonParty::GetInstance()->GetDuneonData( info );	//@@@@@@@ 상위 객체 참조 -_-;
	if( pData == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID WORLDID(%u) ]" ), info.dwWorldId );
		info.SetState( NULL, 0 );
		return;
	}

	//------------------------------------------------------------------------
	if( pData->bColosseumStyle == TRUE )
	{
		CParty* pParty = g_PartyMng.GetParty( info.dwDungeonID );
		if( pParty == NULL )
			return;

		for( int i = 0; i < pParty->GetSizeofMember(); ++i )
		{
			FLWSUser* pMember = g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( i ) );
			if( IsValidObj( pMember ) == FALSE )
				continue;

			if( pMember->GetWorld() == NULL )
				continue;

			if( pMember->GetWorld()->GetID() == info.dwWorldId )
			{
				pMember->AddColosseum_StageTimeLimit( info.nCurrStage, pData->arrLevelProp[ info.eDungeonLevel ].dwRetry_WaitingTick );
				pMember->AddDefinedText( TID_COLOSSEUM_COLOFAILD01 );
			}
		}
	}
}

void FLExpiredTimeWaitingState::OnLeave( INDUN_INFO & /*info*//*, const DWORD dwDungeonID*/ )
{
	__noop;
}

void FLExpiredTimeWaitingState::OnAlarm( INDUN_INFO & info/*, const DWORD dwDungeonID*/ )
{
	CInstanceDungeonHelper::GetInstance()->OnColosseumFail( info );
	info.SetState( NULL, 0 );
}
//------------------------------------------------------------------------------------------------------//
void FLCompleteAllWaitingState::OnEnter( INDUN_INFO & info/*, const DWORD dwDungeonID*/ )
{
	CInstanceDungeonBase::DUNGEON_DATA * pData = CInstanceDungeonParty::GetInstance()->GetDuneonData( info );	//@@@@@@@ 상위 객체 참조 -_-;
	if( pData == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID WORLDID(%u) ]" ), info.dwWorldId );
		info.SetState( NULL, 0 );
		return;
	}

	CParty* pParty = g_PartyMng.GetParty( info.dwDungeonID );
	if( pParty == NULL )
	{
		info.SetState( NULL, 0 );
		return;
	}

// 	for( int i = 0; i < pParty->GetSizeofMember(); ++i )
// 	{
// 		FLWSUser* pMember = g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( i ) );
// 		if( IsValidObj( pMember ) == FALSE )
// 			continue;
// 
// 		if( pMember->GetWorld() == NULL )
// 			continue;
// 
// 		if( pMember->GetWorld()->GetID() == info.dwWorldId )
// 		{
// 			pMember->AddDefinedText( TID_COLOSSEUM_COLOINVITE01 );
// 			pMember->AddColosseum_AllClear( pData->arrLevelProp[ info.eDungeonLevel ].dwComplete_WaitingTick, info.dwTurnaroundTime, 
// 				pData->arrLevelProp[ info.eDungeonLevel ].vecReward	);
// 		}
// 
// 		typedef CInstanceDungeonBase::DUNGEON_DATA::FLLevelProp::RewardVec  RewardVec;
// 		for( RewardVec::iterator it = pData->arrLevelProp[ info.eDungeonLevel ].vecReward.begin()
// 			; it != pData->arrLevelProp[ info.eDungeonLevel ].vecReward.end()
// 			; ++it )
// 		{
// 			IDungeonReward * p  = *it;
// 			p->Reward( info, *pMember );
// 		}
// 	}

	//기록 갱신 시도
	// W->DB
	info.bSuccess			= TRUE;

	if( info.dwGuildID != 0 )		// 길드 모드로 완료했을시엔 DB에 랭킹 정보 등록 후 ack
	{
		BEFORESENDDUAL( ar, PACKETTYPE_RECORD_DUNGEON_PLAY_INFO, DPID_UNKNOWN, DPID_UNKNOWN );
		T_W2DB_PACKET_RECORD_DUNGEON_PLAY_INFO noti = { 0, };

		noti.dwGuildID			= info.dwGuildID;
		noti.dwWorldID			= info.dwWorldId;
		noti.eDungeonLevel		= info.eDungeonLevel;
		noti.dwCompleteTimeTick	= info.dwTurnaroundTime;
		noti.tmStartedDate		= info.tmStartedDate;	
		noti.tmFinishedDate		= DATE_TIMER().GetCurrentTime();
		noti.bSuccess			= TRUE;

		noti.dwPartyID			= info.dwDungeonID;

		ar.Write( & noti, sizeof( noti ) );

		SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );	
	}
	else
	{
		const T_PACKET_COLOSSEUM_ALL_CLEAR_HEADER toClient( pData->arrLevelProp[ info.eDungeonLevel ].dwComplete_WaitingTick, info.dwTurnaroundTime, false, 0 );

		for( int i = 0; i < pParty->GetSizeofMember(); ++i )
		{
			FLWSUser* pMember = g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( i ) );
			if( IsValidObj( pMember ) == FALSE )
				continue;

			if( pMember->GetWorld() == NULL )
				continue;

			if( pMember->GetWorld()->GetID() == info.dwWorldId )
			{
				pMember->AddDefinedText( TID_COLOSSEUM_COLOINVITE01 );
				pMember->AddColosseum_AllClear( toClient );
			}

// 			typedef CInstanceDungeonBase::DUNGEON_DATA::FLLevelProp::RewardVec  RewardVec;
// 			for( RewardVec::iterator it = pData->arrLevelProp[ info.eDungeonLevel ].vecReward.begin()
// 				; it != pData->arrLevelProp[ info.eDungeonLevel ].vecReward.end()
// 				; ++it )
// 			{
// 				IDungeonReward * p  = *it;
// 				p->Reward( info, *pMember );
// 			}
		}
	}
	
}

void FLCompleteAllWaitingState::OnLeave( INDUN_INFO & /*info*//*, const DWORD dwDungeonID*/ )
{
	__noop;
}

void FLCompleteAllWaitingState::OnAlarm( INDUN_INFO & info )
{
	CInstanceDungeonBase::DUNGEON_DATA * pData = CInstanceDungeonParty::GetInstance()->GetDuneonData( info );	//@@@@@@@ 상위 객체 참조 -_-;
	if( pData == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID WORLDID(%u) ]" ), info.dwWorldId );
		info.SetState( NULL, 0 );
		return;
	}

	//------------------------------------------------------------------------
	if( pData->bColosseumStyle == TRUE )
		CInstanceDungeonHelper::GetInstance()->OnColosseumFail( info );

	info.SetState( NULL, 0 );
}

//--------------------------------------------------------------------------------------//







//--------------------------------------------------------------------------------------//
//FLReward_Item::FLReward_Item( const DWORD dwItemID, const short nItemCount ) : m_dwItemID( dwItemID ), m_nItemCount( nItemCount )
//{
//
//}
//
//FLReward_Item::~FLReward_Item()
//{
//
//}
//
//BOOL FLReward_Item::Reward( INDUN_INFO & info, FLWSUser & User )
//{
//	CInstanceDungeonBase::DUNGEON_DATA * pData = CInstanceDungeonParty::GetInstance()->GetDuneonData( info );	//@@@@@@@ 상위 객체 참조 -_-;
//	if( pData == NULL )
//	{
//		Error( "[ ItemReward : INVALID WORLDID(%u) ]", info.dwWorldId );
////		info.SetState( NULL );
//		return FALSE;
//	}
//
//	////Post Item  W->DB
//	BEFORESENDDUAL( ar, PACKETTYPE_REWARD_ITEM_NOTI, DPID_UNKNOWN, DPID_UNKNOWN );
//	T_W2DB_PACKET_REWARD_ITEM_NOTI noti;
//	noti.idReceiver			= User.m_idPlayer;
//	noti.dwMailTitleTID		= pData->arrLevelProp[ info.eDungeonLevel ].dwMailTitleTID;
//	noti.dwMailTextTID		= pData->arrLevelProp[ info.eDungeonLevel ].dwMailTextTID;
//	noti.dwItemID			= m_dwItemID;
//	noti.nItemCount			= m_nItemCount;
//
//	ar.Write( & noti, sizeof( noti ) );
//
//	SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );	
//		
//	return TRUE;
//}
//
//void FLReward_Item::PushSnapshotTail( CAr & ar )
//{
//	ar << /*E_REWARD_TYPE:*/REWARD_ITEM;
//	ar << m_dwItemID << m_nItemCount;
//}

//-----
//
//FLReward_Gold::FLReward_Gold( const int nGold ) : m_nGold( nGold )
//{
//
//}
//
//FLReward_Gold::~FLReward_Gold()
//{
//
//}
//
//BOOL FLReward_Gold::Reward( INDUN_INFO & Info, FLWSUser & User )
//{
//	const int nRewardGold	= std::min( std::numeric_limits< int >::max() - User.GetGold(), m_nGold );		//ignore overflow
//	User.AddGold( nRewardGold, FALSE );
//	return TRUE;
//}
//
//void FLReward_Gold::PushSnapshotTail( CAr & ar )
//{
//	ar << /*E_REWARD_TYPE:*/REWARD_GOLD;
//	ar << m_nGold;
//}
#endif // __WORLDSERVER

