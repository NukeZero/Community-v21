#pragma once


#include <network/FLNetworkCommon.h>
#include "DBManager.h"
	
class CQuery;
class FLDataBase;

class FLQueryCommand : public OVERLAPPED
{
	//enum	E_COMMAND_RESULT
	//{
	//	SUCCESS				= 0,
	//	DISCONNECT,
	//	QUERY_FAIL,
	//};  


public:
//	FLQueryCommand( const DPID dpid, const DPID dpidCache, const DPID dpidUser );
	FLQueryCommand(/* DB_OVERLAPPED_PLUS & kDbOverlappedPlus */);
	virtual ~FLQueryCommand();

	virtual bool 			Execute( CQuery & query ) = 0;
	bool					DirectExecute( FLDataBase & kDB, const unsigned long uKey );

//protected:
	//DB_OVERLAPPED_PLUS *	m_pDbOverlappedPlus;

//	DPID					m_dpid;
//	DPID					m_dpidCache;
//	DPID					m_dpidUser;
};


class FLQueryCommand_HasOverlappedPlus : public FLQueryCommand
{
public:
	FLQueryCommand_HasOverlappedPlus( const DPID dpid, const DPID dpidCache, const DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	virtual ~FLQueryCommand_HasOverlappedPlus();

	virtual bool 			Execute( CQuery & query ) = 0;

protected:
	DB_OVERLAPPED_PLUS		m_kDBOverlappedPlus;
};
