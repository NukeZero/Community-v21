
#include "StdAfx.h"
#include "FLEventDropItem.h"
#include "FLDropItemCommon.h"


//static const int maxHour	= 24;
static const int	MAX_EVENT_ITEM_DROP_LEVEL_GAP	= 10;


FLEventDropItem_Mng::FLEventDropItem_Mng()
{

}

FLEventDropItem_Mng::~FLEventDropItem_Mng()
{

}

// void FLEventDropItem_Mng::Load()
// {
// 	const TCHAR szFileName[] = _T( "DropItem\\EventDrop.lua" );
// 
// 	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
// 	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), szFileName );
// 
// 	CLuaBase kLua;
// 	if( kLua.RunScript( szFullPath ) != 0 )
// 	{
// 		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
// 		return;
// 	}
// 
// 	std::vector<int> connectorTable;
// 	connectorTable.reserve( maxHour );
// 	int totalConnector = 0;
// 
// 	kLua.GetGloabal( "tHour" );
// 	kLua.PushNil();
// 	while( kLua.TableLoop( -2 ) )
// 	{
// 		const int connector	= static_cast<int>( kLua.ToNumber( -1 ) );
// 		if( connector <= 0 ) {
// 			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID CONNECTOR per HOUR : (%d) ]" ), connector );
// 			RELEASE_BREAK;
// 		}
// 		totalConnector	+=  connector;
// 		connectorTable.push_back( connector );
// 
// 		kLua.Pop( 1 );
// 	}
// 	kLua.Pop( 0 );
// 
// 	kLua.GetGloabal( "tEventDrop" );
// 	kLua.PushNil();
// 	//while( kLua.TableLoop( -2 ) )
// 	for( int i = 0; kLua.TableLoop( -2 ); ++i )
// 	{
// 		FLEventSet kEventSet;
// 		kEventSet.m_strDesc	= kLua.GetFieldToString( -1, "strDesc" );
// 
// 		kLua.GetField( -1, "tLang" );
// 		kLua.PushNil();
// 		while( kLua.TableLoop( -2 ) )
// 		{
// 			kEventSet.m_vecLang.push_back( CScript::GetDefineNum( kLua.GetFieldToString( -1, "nLang" ) ) );
// 
// 			kLua.Pop( 1 );
// 		}
// 		kLua.Pop( 1 );
// 
// 		kLua.GetField( -1, "tPeriod" );
// 		kLua.PushNil();
// 		while( kLua.TableLoop( -2 ) )
// 		{
// 			FLPeriod kPeriod;
// 
// 			tstring strStartDate = kLua.GetFieldToString( -1, "strStart" );
// 			tstring strEndDate	= kLua.GetFieldToString( -1, "strEnd" );
// 			strStartDate.append( _T( ":00" ) );
// 			strEndDate.append( _T( ":00" ) );
// 
// 			kPeriod.kStartDate	= FLStringToFLTm( strStartDate.c_str(), FLTm::E_FORMAT::YYYYMMDD_HHMMSS );
// 			if( kPeriod.kStartDate == INVALID_DATETIME )
// 			{
// 				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID START DATE(%s) ]" ), strStartDate.c_str() );
// 				RELEASE_BREAK;
// 			}
// 			kPeriod.kEndDate	= FLStringToFLTm( strEndDate.c_str(), FLTm::E_FORMAT::YYYYMMDD_HHMMSS );			
// 			if( kPeriod.kEndDate == INVALID_DATETIME )
// 			{
// 				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID END DATE(%s) ]" ), strEndDate.c_str() );
// 				RELEASE_BREAK;
// 			}
// 
// 			kEventSet.m_vecPeriod.push_back( kPeriod );
// 
// 			kLua.Pop( 1 );
// 		}
// 		kLua.Pop( 1 );
// 
// 		
// 		kLua.GetField( -1, "tItem" );
// 		kLua.PushNil();
// 		while( kLua.TableLoop( -2 ) )
// 		{
// 			DropItem_Data kDropItem;
// 			kDropItem.itemID						= CScript::GetDefineNum( kLua.GetFieldToString( -1, "strItemId" ) );
// 			kDropItem.limitQuantityPerOneDay		= static_cast< int >( kLua.GetFieldToNumber( -1, "limitQuantityPerOneDay" ) );
// 			kDropItem.randomQuantityPerOneMonster	= static_cast< int >( kLua.GetFieldToNumber( -1, "randomQuantityPerOneMonster" ) );
// 
// 			kDropItem.intervalTable.reserve( maxHour );
// 			for( size_t hour = 0; hour < maxHour; ++hour ) {
// 				const DWORD interval	= static_cast< DWORD >( FLTm::ONE_HOUR_SECOND * 1000 / static_cast<float>( kDropItem.limitQuantityPerOneDay * connectorTable.at( hour ) / static_cast<float>( totalConnector ) ) );
// 				kDropItem.intervalTable.push_back( interval );
// 			}
// 
// 			kEventSet.m_vecDropItem.push_back( kDropItem );
// 
// 			kLua.Pop( 1 );
// 		}
// 		kLua.Pop( 1 );
// 
// 		kEventSet.m_nLevelMin= static_cast< int >( kLua.GetFieldToNumber( -1, "dwLevelMin" ) );
// 		kEventSet.m_nLevelMax= static_cast< int >( kLua.GetFieldToNumber( -1, "dwLevelMax" ) );
// 
// 		/************************************************************************/
// 		// check
// 		if( kEventSet.m_vecLang.empty() || kEventSet.m_vecPeriod.empty() || kEventSet.m_vecDropItem.empty() ) {
// 			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID DROP ITEM SETTING. CHECK LANG or PERIOD or ITEM. FILENAME:(%s) ]" ), szFullPath );
// 			RELEASE_BREAK;
// 		}
// 
// 		if( kEventSet.m_nLevelMin <= 0 || kEventSet.m_nLevelMin > kEventSet.m_nLevelMax ) {
// 			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID DROP ITEM MONSTER LEVEL. MIN:(%d), MAX:(%d) ]" ), kEventSet.m_nLevelMin, kEventSet.m_nLevelMax );
// 			RELEASE_BREAK;
// 		}
// 		/************************************************************************/
// 
// 		if( m_mapEventSet.insert( EventSetMap::value_type( i, kEventSet ) ).second == false )
// 		{
// 			FLERROR_LOG( PROGRAM_NAME, _T( "[ CANNOT INSERT EVENTID(%s)(%d) ]" ), kEventSet.m_strDesc.c_str(), i );			
// 		}
// 
// 		kLua.Pop( 1 );
// 	}
// 
// 	kLua.Pop( 0 );
// }

void	FLEventDropItem_Mng::DropItem( CMover & kMonster, FLWSUser & kUser )
{
// 	if( kUser.IsPlayer() == FALSE )
// 		return;
// 
// 	for( EventSetMap::iterator it = m_mapEventSet.begin(); it != m_mapEventSet.end(); ++it )
// 	{
// 		FLEventSet & kEventSet	= it->second;
// 		if( kEventSet.CheckEvent( kMonster ) == FALSE )
// 			continue;
// 
// 		kEventSet.DropItem( kMonster, kUser );
// 	}

	/************************************************************************/
	if( kUser.IsPlayer() == FALSE || kMonster.IsPlayer() == TRUE ) {
		return;
	}

	if( kUser.GetLevel() - kMonster.GetLevel() >= MAX_EVENT_ITEM_DROP_LEVEL_GAP ) {
		return;
	}

	std::map< DWORD, int > eventItem( prj.m_EventLua.GetItem( kMonster.GetLevel() ) );
	if( eventItem.empty() ) {
		return;
	}

	for( std::map< DWORD, int >::const_iterator pos = eventItem.begin(); pos != eventItem.end(); ++pos ) {
		FLDoDrop::DropItem( kMonster, kUser, kUser.GetId(), pos->first, pos->second );
	}
	/************************************************************************/
}

FLEventDropItem_Mng & FLEventDropItem_Mng::GetInstance()
{
	static FLEventDropItem_Mng inst;
	return inst;
}

//---------------------------------------------------------------------------------------------------------------//

// BOOL FLEventDropItem_Mng::FLEventSet::CheckEvent( CMover & kMonster/*, FLWSUser & kUser*/ )
// {
// 	// 1. CHECK LANG
// 	BOOL bFindLang = FALSE;
// 	for( LangVec::const_iterator it = m_vecLang.begin(); it != m_vecLang.end(); ++it )
// 	{
// 		const int nLang		= *it;
// 		if( nLang == g_xFlyffConfig->GetMainLanguage() )
// 		{
// 			bFindLang		= TRUE;
// 			break;
// 		}
// 	}
// 	
// 	if( bFindLang == FALSE )
// 		return FALSE;
// 
// 	// 2. CHECK LEVEL
// 	if( m_nLevelMin > kMonster.GetLevel() || m_nLevelMax < kMonster.GetLevel() )
// 		return FALSE;
// 
// 	// 3. CHECK PERIOD
// 	const FLTm kCurrDate	= DATE_TIMER().GetCurrentTime();
// 	BOOL bValidPeriod = FALSE;
// 	for( PeriodVec::const_iterator it = m_vecPeriod.begin(); it != m_vecPeriod.end(); ++it )
// 	{
// 		const FLPeriod & kPeriod	= *it;
// 		if( kPeriod.kStartDate <= kCurrDate && kCurrDate <= kPeriod.kEndDate )
// 		{
// 			bValidPeriod	= TRUE;
// 			break;
// 		}
// 	}
// 	
// 	if( bValidPeriod == FALSE )
// 		return FALSE;
// 
// 	return TRUE;
// }
// 
// void FLEventDropItem_Mng::FLEventSet::DropItem( CMover & kMonster, FLWSUser & kUser )
// {
// 	const DWORD CurrTick	= ::GetTickCount();
// 	const int currentHour	= CTime::GetCurrentTime().GetHour();
// 	if( currentHour < 0 || currentHour >= maxHour ) {
// 		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID HOUR : (%d) ]" ), currentHour );
// 		FLASSERT( 0 );
// 		return;
// 	}
// 
// 	for( DropItemVec::iterator it = m_vecDropItem.begin(); it != m_vecDropItem.end(); ++it ) {
// 		const DWORD intervalTick		= it->intervalTable.at( currentHour );
// 		const int dropQuantity			= static_cast<int>( ::xRandom( it->randomQuantityPerOneMonster ) ) + 1;
// 
// 		if( ( it->lastTick + intervalTick ) < CurrTick ) {
// 			it->lastTick	= CurrTick;
// 			
// 			if( it->skipCount <= 0 ) {
// 				FLDoDrop::DropItem( kMonster, kUser, kUser.GetId(), it->itemID, dropQuantity );
// 				it->skipCount		= dropQuantity - 1;
// 			}
// 			else {
// 				--( it->skipCount );
// 			}
// 		}
// 	}
// }