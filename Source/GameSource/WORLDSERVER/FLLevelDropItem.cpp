#include "StdAfx.h"

#include "FLLevelDropItem.h"

#include <Project.h>


//--------------------------------------------------------------------------------------------------------------//

FLLevelDropSet_Mng::FLLevelDropSet_Mng() : m_dwIndex( 0 ), m_nLevelMin( 0 ), m_nLevelMax( 0 ), m_kStartDate( INVALID_DATETIME ), m_kEndDate( INVALID_DATETIME )
{

}

FLLevelDropSet_Mng::~FLLevelDropSet_Mng()
{

}

BOOL FLLevelDropSet_Mng::CheckEvent( CMover & kMonster )
{
	if( ( kMonster.GetLevel() < m_nLevelMin ) || ( m_nLevelMax < kMonster.GetLevel() ) )
		return FALSE;

	const FLTm kCurrDate	= DATE_TIMER().GetCurrentTime();
    if( ( kCurrDate	< m_kStartDate ) || ( m_kEndDate < kCurrDate ) )
		return FALSE;

	return TRUE;
}

void FLLevelDropSet_Mng::DropItem( CMover & kMonster, FLWSUser & kAttacker )
{
	for( DropSetVec::iterator it = m_vecDropSet.begin(); it != m_vecDropSet.end(); ++it )
	{
		FLDropSet & kDropSet	= *it;
		kDropSet.DropItem( kMonster, kAttacker );
	}
}



//------------------------------------------------------------------------------------------------------------//

FLLevelDropItem_Mng::FLLevelDropItem_Mng()
{

}

FLLevelDropItem_Mng::~FLLevelDropItem_Mng()
{

}

FLLevelDropItem_Mng & FLLevelDropItem_Mng::GetInstance()
{
	static FLLevelDropItem_Mng inst;
	return inst;
}

void FLLevelDropItem_Mng::Load()
{
	load_DropLevelSpec();
	load_WeekFlagsSpec();
}


void FLLevelDropItem_Mng::load_DropLevelSpec()
{
	//아이템이 로드된 후
	const TCHAR szFileName[] = _T( "DropItem/propDropMonsterLevel.csv" );

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), szFileName );

	CScript kScript;
	if( kScript.Load( szFullPath ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return;
	}

	//kScript.GetToken();

	if( kScript.tok == FINISHED )
		FLERROR_LOG( PROGRAM_NAME, _T( "ERROR FINISHIED" ) );


	DWORD dwEventID		= 0;
	int	nLastDropSet_Index	= -1;


	for( int line = 0; kScript.tok != FINISHED; ++line )
	{
		//1. preload
		const DWORD dwCurrEventID	= static_cast< DWORD >(	kScript.GetNumber( TRUE ) );				// ( kScript.GetNumber( TRUE ) );//CScript::GetDefineNum( kScript.Token ) );//( kScript.GetNumber( TRUE ) );//( CScript::GetDefineNum( kScript.Token ) );	//( kScript.GetNumber( TRUE ) );
		if( kScript.Token == "" )
			continue;

		const DWORD dwLevelMin	= static_cast< DWORD >(	kScript.GetNumber( TRUE ) );
		const DWORD dwLevelMax	= static_cast< DWORD >(	kScript.GetNumber( TRUE ) );

		kScript.GetToken( TRUE );
		std::string strStartDate = kScript.Token;
		strStartDate.append( _T( ":00" ) );
		const FLTm kStartDate	= FLStringToFLTm( strStartDate.c_str(), FLTm::E_FORMAT::YYYYMMDD_HHMMSS );

		kScript.GetToken( TRUE );
		std::string strEndDate	= kScript.Token;
		strEndDate.append( _T( ":00" ) );
		const FLTm kEndDate		= FLStringToFLTm( strEndDate.c_str(), FLTm::E_FORMAT::YYYYMMDD_HHMMSS );


		kScript.GetToken( TRUE );
		std::string strMainProbName = kScript.Token;
		const FLDropSet::E_DROP_SET_KIND eSetKind = FLDropSet::StringToKindEnum( strMainProbName );

		const DWORD dwMainProb	= static_cast< DWORD >( kScript.GetNumber( TRUE ) );

		kScript.GetToken( TRUE );
		std::string strItemID = kScript.Token;
		const DWORD dwItemID	= ::atoi( kScript.Token );	//const DWORD dwItemID	= CScript::GetDefineNum( kScript.Token );	//		const DWORD dwItemID	= static_cast< DWORD >( kScript.GetNumber( TRUE ) );	//CScript::GetDefineNum( kScript.Token ) )

		const DWORD dwProb		= static_cast< DWORD >( kScript.GetNumber( TRUE ) );
		const int nItemCountMin	= kScript.GetNumber( TRUE );
		const int nItemCountMax	= kScript.GetNumber( TRUE );
		const int nQuestID		= kScript.GetNumber( TRUE );

		//if( line == 1 )				//제목 스킵
		//	continue;

		//2. insert
		if( dwCurrEventID != NULL_ID && dwCurrEventID != 0 )
		{
			if( kStartDate == INVALID_DATETIME )
				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID DATA : startTime eventID(%u) ]" ), dwCurrEventID );

			if( kEndDate == INVALID_DATETIME )
				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID DATA : endTime eventID(%u) ]" ), dwCurrEventID );

			FLLevelDropSet_Mng kDropSetMng;
			kDropSetMng.m_dwIndex		= dwCurrEventID;
			kDropSetMng.m_nLevelMin		= dwLevelMin;
			kDropSetMng.m_nLevelMax		= dwLevelMax;
			kDropSetMng.m_kStartDate	= kStartDate;
			kDropSetMng.m_kEndDate		= kEndDate;

			dwEventID				= dwCurrEventID;

			const bool bInsert		= m_mapDropSetMng.insert( DropSetMngMap::value_type( dwCurrEventID, kDropSetMng ) ).second;
			if( bInsert == false )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ DUPLICATE EventID(%u) line(%d) ]" ), dwCurrEventID, line );
			}
			continue;
		}

		if( dwMainProb != 0 )
		{
			DropSetMngMap::iterator it	= m_mapDropSetMng.find( dwEventID );
			if( it == m_mapDropSetMng.end() )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ NOT EXIST EventID(%u) line(%d) ]" ), dwEventID, line );
				break;
			}

			FLLevelDropSet_Mng & kDropSetMng	= it->second;
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
			PT_ITEM_SPEC pItemSpec	= g_xSpecManager->GetSpecItem( dwItemID );
			if( pItemSpec == NULL )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID QUEST ID (%d) line(%d) ]" ), nQuestID, line );
				continue;
			}

			DropSetMngMap::iterator it	= m_mapDropSetMng.find( dwEventID );
			if( it == m_mapDropSetMng.end() )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ NOT EXIST EventID(%u) line(%d) ]" ), dwEventID, line );
				break;
			}

			FLLevelDropSet_Mng & kDropSetMng	= it->second;
			FLDropSet & kDropSet		= kDropSetMng.m_vecDropSet.at( nLastDropSet_Index );
			if( strItemID != "NONE" && kDropSet.Check_Item( dwItemID, nQuestID ) == FALSE )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ FAIL - CHECK ITEM : strItemID(%s) line(%d) ]" ), strItemID, line );
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
		const FLLevelDropSet_Mng & kDropSetMng	= it->second;
		for( FLLevelDropSet_Mng::DropSetVec::const_iterator itSet = kDropSetMng.m_vecDropSet.begin(); itSet != kDropSetMng.m_vecDropSet.end(); ++itSet )
		{
			const FLDropSet & kDropSet	= *itSet;
			if( kDropSet.Check_Prob() == FALSE )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ ERROR - INVALID LEVEL DROP PROB LEVEL ID(%d) ]" ), kDropSetMng.m_dwIndex );
			}
		}
	}

	FLINFO_LOG( PROGRAM_NAME, _T( "[ LOADED (%s) count(%u) ]" ), szFileName, m_mapDropSetMng.size() );
}

void FLLevelDropItem_Mng::DropItem( CMover & kMonster, FLWSUser & kAttacker )
{
	if( kMonster.GetProp() == NULL )
		return;


	const int nDayOfWeek		= CTime::GetCurrentTime().GetDayOfWeek() - 1 ;		//GetDayOfWeek는 1~7 일~토

	for( DropSetMngMap::iterator it	= m_mapDropSetMng.begin(); it != m_mapDropSetMng.end(); ++it )
	{
		const DWORD dwEvent				= it->first;
		FLLevelDropSet_Mng & kDropSetMng= it->second;

		const WeekFlagsMap::const_iterator itFlags = m_mapWeekFlags.find( dwEvent );
		if( itFlags != m_mapWeekFlags.end() )
		{
			if( 0 <= nDayOfWeek && nDayOfWeek < WEEK_MAX )
			{
				const FLWeekFlags & flags = itFlags->second;
				if( flags.arr[ nDayOfWeek ] == false )
					continue;
			}
		}
						
		if( kDropSetMng.CheckEvent( kMonster ) == FALSE )
			continue;

		kDropSetMng.DropItem( kMonster, kAttacker );
	}
}



void FLLevelDropItem_Mng::load_WeekFlagsSpec()
{
	//아이템이 로드된 후
	const TCHAR szFileName[] = _T( "DropItem/propDropMonsterLevelSetLang.csv" );

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), szFileName );

	CScript kScript;
	if( kScript.Load( szFullPath ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return;
	}

	if( kScript.tok == FINISHED )
		FLERROR_LOG( PROGRAM_NAME, _T( "ERROR FINISHIED" ) );

	CTime t = CTime::GetCurrentTime();
	for( int line = 0; kScript.tok != FINISHED; ++line )
	{
		//1. preload
		const DWORD dwEventID	= static_cast< DWORD >(	kScript.GetNumber( TRUE ) );	
		if( kScript.Token == "" )
			continue;

		const int nLang			= kScript.GetNumber( TRUE );

		FLWeekFlags flags;
		for( int i = 0; i < WEEK_MAX; ++i )
		{
			flags.arr[ i ]		= static_cast< bool >( kScript.GetNumber( TRUE ) != 0 );
		}

		if( g_xFlyffConfig->GetMainLanguage() != nLang )
			continue;

		if( m_mapWeekFlags.insert( make_pair( dwEventID, flags ) ).second == false )
		{
			FLERROR_LOG( PROGRAM_NAME, _T("[ DUPLICATE ID lang(%d) id(%u) ]"), nLang, dwEventID );
		}
	}

	FLINFO_LOG( PROGRAM_NAME, _T( "[ LOADED (%s) count(%u) ]" ), szFileName, m_mapWeekFlags.size() );
}



