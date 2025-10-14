#include "StdAfx.h"

#include "FLMonsterDropItem.h"

#include <Project.h>

#include <limits>






FLMonsterDropSet_Mng::FLMonsterDropSet_Mng() : m_dwIndex( 0 ), m_dwGoldMainProb( 0 ), m_nGoldMin( 0 ), m_nGoldMax( 0 )
{
}

FLMonsterDropSet_Mng::~FLMonsterDropSet_Mng()
{

}

void FLMonsterDropSet_Mng::DropItem( CMover & kMonster, FLWSUser & kAttacker )
{
	for( DropSetVec::iterator it = m_vecDropSet.begin(); it != m_vecDropSet.end(); ++it )
	{
		FLDropSet & kDropSet	= *it;
		kDropSet.DropItem( kMonster, kAttacker );
	}
}

void FLMonsterDropSet_Mng::DropGold( CMover & kMonster, FLWSUser & kAttacker )
{
	//1. 외부 드랍률 계산
	const DWORD dwMainProb	= static_cast< DWORD >( m_dwGoldMainProb * FLDoDrop::GetGoldDropRateFactor( kMonster, kAttacker ) );

	const DWORD dwRand		= xRandom( DROP_PROB_MAX );
	if( dwRand >= dwMainProb )
		return;

	const int nGold		= xRandom( m_nGoldMin, m_nGoldMax );

	FLDoDrop::DropGold( kMonster, kAttacker, kAttacker.GetId(), nGold );
}

//---------------------------------------------------------------------------------------------------------------//

FLMonsterDropItem_Mng::FLMonsterDropItem_Mng()
{

}

FLMonsterDropItem_Mng::~FLMonsterDropItem_Mng()
{

}

FLMonsterDropItem_Mng & FLMonsterDropItem_Mng::GetInstance()
{
	static FLMonsterDropItem_Mng inst;
	return inst;
}

void FLMonsterDropItem_Mng::Load()
{
	//몬스터와 아이템이 로드된 후

	const TCHAR szFileName[] = _T( "DropItem/propDropMonsterName.csv" );

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), szFileName );

	CScript kScript;
	if( kScript.Load( szFullPath ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return;
	}

	DWORD dwMonsterID		= 0;
	int	nLastDropSet_Index	= -1;

	//kScript.GetToken( TRUE );			//SKIP TITLE

	//kScript.GetToken();

	if( kScript.tok == FINISHED )
		FLERROR_LOG( PROGRAM_NAME, _T( "ERROR FINISHIED" ) );

	for( int line = 0; kScript.tok != FINISHED; ++line )
	{
		//1. preread
		const DWORD dwCurrMonsterID	= static_cast< DWORD >(	kScript.GetNumber( TRUE ) );				// ( kScript.GetNumber( TRUE ) );//CScript::GetDefineNum( kScript.Token ) );//( kScript.GetNumber( TRUE ) );//( CScript::GetDefineNum( kScript.Token ) );	//( kScript.GetNumber( TRUE ) );
		if( kScript.Token == "" )
			continue;

		const DWORD dwGoldMainProb	= static_cast< DWORD >( kScript.GetNumber( TRUE ) );
		int nGoldMin			= kScript.GetNumber( TRUE );
		int nGoldMax			= kScript.GetNumber( TRUE );

		kScript.GetToken( TRUE );
		std::string strMainProbName = kScript.Token;
		const FLDropSet::E_DROP_SET_KIND eSetKind = FLDropSet::StringToKindEnum( strMainProbName );

		const DWORD dwMainProb	= static_cast< DWORD >( kScript.GetNumber( TRUE ) );

		kScript.GetToken( TRUE );
		std::string strItemID = kScript.Token;
		const DWORD dwItemID	= ::atoi( kScript.Token );	//= static_cast< DWORD >( kScript.GetNumber( TRUE ) );	//CScript::GetDefineNum( kScript.Token ) )

		const DWORD dwProb		= static_cast< DWORD >( kScript.GetNumber( TRUE ) );
		const int nItemCountMin	= kScript.GetNumber( TRUE );
		const int nItemCountMax	= kScript.GetNumber( TRUE );
		const int nQuestID		= kScript.GetNumber( TRUE );

		//if( line == 1 )				//제목 스킵
		//	continue;

		//2. insert
		if( dwCurrMonsterID != NULL_ID && dwCurrMonsterID != 0 )
		{
			if( nGoldMin > nGoldMax )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ GoldMin Greater Than GoldMax : MonsterID(%u) ]" ), dwCurrMonsterID );
				std::swap( nGoldMin, nGoldMax );
			}

			FLMonsterDropSet_Mng kDropSetMng;
			kDropSetMng.m_dwIndex		= dwCurrMonsterID;
			kDropSetMng.m_dwGoldMainProb= dwGoldMainProb;
			kDropSetMng.m_nGoldMin		= nGoldMin;
			kDropSetMng.m_nGoldMax		= nGoldMax;

			dwMonsterID					= dwCurrMonsterID;
		
			const bool bInsert = m_mapDropSetMng.insert( DropSetMngMap::value_type( dwCurrMonsterID, kDropSetMng ) ).second;
			if( bInsert == false )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ NOT EXIST MONSTERID(%u) line(%d) ]" ), dwCurrMonsterID, line );
			}
			continue;
		}

		if( dwMainProb != 0 )
		{
			DropSetMngMap::iterator it	= m_mapDropSetMng.find( dwMonsterID );
			if( it == m_mapDropSetMng.end() )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ NOT EXIST MONSTERID(%u) line(%d) ]" ), dwMonsterID, line );
				return;
			}

			FLMonsterDropSet_Mng & kDropSetMng	= it->second;
			kDropSetMng.m_vecDropSet.push_back( FLDropSet() );
			nLastDropSet_Index			= kDropSetMng.m_vecDropSet.size() - 1;

			FLDropSet & kDropSet		= kDropSetMng.m_vecDropSet.at( nLastDropSet_Index );
			kDropSet.m_strMainProbName	= strMainProbName;
			kDropSet.m_dwMainProb		= dwMainProb;
			kDropSet.m_eSetKind			= eSetKind;

			if( eSetKind == FLDropSet::KIND_NONE )
				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID eKind(%s) line(%d) ]" ), strMainProbName.c_str(), line );
			continue;
		}

		if( strItemID == "NONE" || ( dwItemID != NULL_ID && dwItemID != 0 )  )
		{
			DropSetMngMap::iterator it	= m_mapDropSetMng.find( dwMonsterID );
			if( it == m_mapDropSetMng.end() )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ NOT EXIST MONSTERID(%u) line(%d) ]" ), dwMonsterID, line );
				return;
			}

			FLMonsterDropSet_Mng & kDropSetMng	= it->second;
			FLDropSet & kDropSet		= kDropSetMng.m_vecDropSet.at( nLastDropSet_Index );
			if( strItemID != "NONE" && kDropSet.Check_Item( dwItemID, nQuestID ) == FALSE )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ FAIL - CHECK ITEM : strItemID(%s) line(%d) ]" ), strItemID.c_str(), line );
			}

			if( nItemCountMax <= 0 )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ FAIL - ITEM COUNT 0 : line(%d) ]" ), line );
			}

			if( nItemCountMin > nItemCountMax )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ FAIL - ITEM COUNT MIN(%d) MAX(%d) 0 : line(%d) ]" ), nItemCountMin, nItemCountMax, line );
			}


			FLDropItem_Data kDropItem;
			kDropItem.m_dwProb			= dwProb;
			kDropItem.m_dwItemID		= dwItemID;
			kDropItem.m_nItemCountMin	= max( nItemCountMin, 0 );
			kDropItem.m_nItemCountMax	= max( nItemCountMax, 0 );
			kDropItem.m_nQuestID		= nQuestID;

			kDropSet.m_vecDropItem.push_back( kDropItem );
			continue;
		}
	}

	//검증
	for( DropSetMngMap::const_iterator it = m_mapDropSetMng.begin(); it != m_mapDropSetMng.end(); ++it )
	{
		const FLMonsterDropSet_Mng & kDropSetMng	= it->second;
		for( FLMonsterDropSet_Mng::DropSetVec::const_iterator itSet = kDropSetMng.m_vecDropSet.begin(); itSet != kDropSetMng.m_vecDropSet.end(); ++itSet )
		{
			const FLDropSet & kDropSet	= *itSet;
			if( kDropSet.Check_Prob() == FALSE )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ ERROR - INVALID MONSTER DROP PROB MONSTERID(%d) ]" ), kDropSetMng.m_dwIndex );
			}
		}
	}

	FLINFO_LOG( PROGRAM_NAME, _T( "[ LOADED (%s) count(%u) ]" ), szFileName, m_mapDropSetMng.size() );
}

void FLMonsterDropItem_Mng::DropItemNGold( CMover & kMonster, FLWSUser & kUser )
{
	if( kMonster.GetProp() == NULL )
		return;

	DropSetMngMap::iterator it	= m_mapDropSetMng.find( kMonster.GetProp()->dwID );
	if( it == m_mapDropSetMng.end() )
		return;

	FLMonsterDropSet_Mng & kDropSetMng = it->second;
	kDropSetMng.DropItem( kMonster, kUser );
	kDropSetMng.DropGold( kMonster, kUser );
}

