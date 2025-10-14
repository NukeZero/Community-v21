#include "StdAfx.h"


#include "FLQueryCommand.h"

#include "FLDataBase.h"

//FLQueryCommand::FLQueryCommand( const DPID dpid, const DPID dpidCache, const DPID dpidUser ) 
FLQueryCommand::FLQueryCommand(/* DB_OVERLAPPED_PLUS & kDbOverlappedPlus */)
//: m_pDbOverlappedPlus( &kDbOverlappedPlus ) //m_dpid( dpid ), m_dpidCache( dpidCache ), m_dpidUser( dpidUser )
{
//	m_szQueryString[ 0 ]	= '\0';
}

FLQueryCommand::~FLQueryCommand()
{
//	SAFE_DELETE_FREEREQUEST( m_pDbOverlappedPlus );
}

bool FLQueryCommand::DirectExecute( FLDataBase & kDB, const unsigned long uKey )
{
	CQuery * pQuery		= kDB.GetQuery( uKey );
	//if( pQuery == NULL )
	//{
	//	FLERROR_LOG( PROGRAM_NAME, _T( "[ DB NOT START ]" ) );
	//	
	//	return;
	//}

	return this->Execute( *pQuery );
}

//------------------------------------------------------------------------------------------------------------------------------//
FLQueryCommand_HasOverlappedPlus::FLQueryCommand_HasOverlappedPlus( const DPID dpid, const DPID dpidCache, const DPID dpidUser, LPBYTE lpBuf, u_long uBufSize ) 
{
	g_DbManager.MakeRequest( &m_kDBOverlappedPlus, lpBuf, uBufSize );
	m_kDBOverlappedPlus.dpid		= dpid;
	m_kDBOverlappedPlus.dpidCache	= dpidCache;
	m_kDBOverlappedPlus.dpidUser	= dpidUser;
	m_kDBOverlappedPlus.nQueryMode	= 0;
}


FLQueryCommand_HasOverlappedPlus::~FLQueryCommand_HasOverlappedPlus()
{
	SAFE_DELETE_ARRAY( m_kDBOverlappedPlus.lpBuf );
}

