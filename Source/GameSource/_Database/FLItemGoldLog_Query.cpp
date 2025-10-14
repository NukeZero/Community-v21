#include "StdAfx.h"

#include "FLItemGoldLog_Query.h"


extern	APP_INFO			g_appInfo;

FLItemGold_LogQuery::FLItemGold_LogQuery( const LogItemInfo & info ) : m_kInfo( info )
{

}

bool FLItemGold_LogQuery::Execute( CQuery & query )
{
// 	char szQuery[ MAX_QUERY_SIZE ] = { 0 };
// 	CDbManager::DBQryNewItemLog( szQuery, _countof( szQuery ), m_kInfo );
// 	if( query.Exec( szQuery ) == FALSE )
// 	{
// 		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAIL - ITEM LOG(%s) ]" ), szQuery );
// 		return false;
// 	}
// 	return true;

	return CDbManager::DBQryNewItemLog( query, m_kInfo ) != FALSE;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------//

FLItem_LogQuery::FLItem_LogQuery( const char * szQueryState, const LogItemInfo & info, const u_long uPlayerID, const int nNo, const char * szItemName ) 
: m_kItemInfo( info )
, m_uPlayerID( uPlayerID )
, m_nNo( nNo )
{
	if( szQueryState == NULL )	m_szQueryState[ 0 ] = '\0';		else FLStrcpy( m_szQueryState, _countof( m_szQueryState ), szQueryState );
	if( szItemName == NULL )	m_szItemName[ 0 ] = '\0';		else FLStrcpy( m_szItemName, _countof( m_szItemName ), szItemName );
}

bool FLItem_LogQuery::Execute( CQuery & query )
{
	char szQuery[ MAX_QUERY_SIZE ] = { 0 };

	CDbManager::DBQryLog( szQuery, _countof( szQuery ), m_szQueryState, m_uPlayerID , g_appInfo.dwSys, m_nNo, 0, 0, 0, 0
		, m_kItemInfo.kLogItem.nItemResist, m_kItemInfo.kLogItem.nResistAbilityOption, 0, 0, 0, 0, 'A', 0, '\0'
		, 0.0f, 0.0f, 0.0f, 0, 0, "\0", 0, 0, 0, m_szItemName, 0, m_kItemInfo.kLogItem.nQuantity );

	if( FALSE == query.Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "LC = %s" ), szQuery );
		return false;
	}

// 	CDbManager::DBQryNewItemLog( szQuery, _countof( szQuery ), m_kItemInfo );
// 
// 	if( FALSE == query.Exec( szQuery ) )
// 	{
// 		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
// 		return FALSE;
// 	}
// 	return TRUE;

	return CDbManager::DBQryNewItemLog( query, m_kItemInfo ) != FALSE;
}


