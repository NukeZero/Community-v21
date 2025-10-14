#include "StdAfx.h"
#include "FLExpLog_Query.h"

extern	APP_INFO			g_appInfo;

FLExpLog_Query::FLExpLog_Query( CMover & kMover, const char command )//, const BOOL bLogout, const char * szAccount )
:// m_kMover( kMover )
//, 
m_command( command )
{
	m_kMover.Copy( &kMover, TRUE );
	//m_kMover.m_dwGold			= kMover.m_dwGold;
	//m_kMover.m_bLogout		= kMover.m_bLogout;
	//FLStrcpy( m_kMover.m_szAccount, _countof( m_kMover.m_szAccount ), kMover.m_szAccount );
}

FLExpLog_Query::~FLExpLog_Query()
{
	//SAFE_DELETE( m_kMover );
}


bool FLExpLog_Query::Execute( CQuery & query )
{
	BOOL bPut	= ( m_command != 'P' );	
	const DWORD dwTick	= GetTickCount();
	if( !bPut )
	{
		if( m_kMover.m_dwPeriodicTick < dwTick )
			bPut	= TRUE;
	}
	if( bPut )
	{
		m_kMover.m_dwPeriodicTick	= dwTick + MIN( 120 );

		char szSql[1024]	= { 0, };
		CDbManager::DBQryLog( szSql, _countof( szSql ), "L1", m_kMover.m_idPlayer, g_appInfo.dwSys,
			m_kMover.m_nExp1, m_kMover.m_nLevel,
			m_kMover.m_nJob, 0, m_kMover.GetFlightLv(),
			m_kMover.m_Stat.GetOriginStr(), m_kMover.m_Stat.GetOriginDex(), m_kMover.m_Stat.GetOriginInt(), m_kMover.m_Stat.GetOriginSta(),
			m_kMover.m_Stat.GetRemainStatPoint(), 0,
			m_command, 0, '\0', 0, 0, 0, m_kMover.GetGold(), 0 );

		if( FALSE == query.Exec( szSql ) )
			return false;
	}

	return true;
}

