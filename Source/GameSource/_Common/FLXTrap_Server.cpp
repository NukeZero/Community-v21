#include "StdAfx.h"

#ifdef _xtrap
#include "FLXTrap_Server.h"

#include <network/FLDPMng.h>
#include <common/FLStdFile.h>
#include <memory.h>

#include "MsgHdr.h"

//#include ""

#ifdef	_DLL
	#ifdef	_DEBUG
		#pragma comment( lib, "X-TrapDllD.lib" )
	#else
		#pragma comment( lib, "X-TrapDll.lib" )
	#endif
#else
	#ifdef	_DEBUG
		#pragma comment( lib, "X-TrapD.lib" )
	#else
		#pragma comment( lib, "X-Trap.lib" )
	#endif
#endif


FLXTrap_Server::FLXTrap_Server() : m_bActive( FALSE ), m_nMapLoadedCount( 0 ), m_dwLastUpdate( 0 )
{
#if CSFILE_NUM > 0
	memset( m_Map, 0, sizeof( m_Map ) );
#else
#endif
	

	//this->Start();
}

FLXTrap_Server::~FLXTrap_Server()
{
	//XTrap_S_FinalLoadModule();
}


FLXTrap_Server & FLXTrap_Server::GetInstance()
{
	static FLXTrap_Server s_inst;
	return s_inst;
}


BOOL	FLXTrap_Server::Start()
{
	if( this->loadMapFile( "map.cs3" ) == false )
	{
		return FALSE;
	}

	if( this->start() == FALSE )
	{
		return FALSE;
	}

	if( this->SetActiveCode( XTRAP_ACTIVE_CODE_LEVEL2 ) == FALSE )
	{
		return FALSE;
	}

	return TRUE;
}

void FLXTrap_Server::SetActive( const BOOL bActive )
{
	::InterlockedExchange( ( long * )&m_bActive, bActive );
	//m_bActive				= bActive;
}


BOOL FLXTrap_Server::IsActive() const
{
	return m_bActive;
}

BOOL FLXTrap_Server::IsUpdateTime( const DWORD dwCurrTick )
{
	if( m_dwLastUpdate + XTRAP_UPDATE_INTERVAL > dwCurrTick ) 
		return FALSE;

	m_dwLastUpdate			= dwCurrTick;
	return TRUE;
}


BOOL FLXTrap_Server::AllocSessionBuffer( /*CDPMng & DPMng, const DPID dpid,*/ FLSession & o_Session )			//소켓 close : User를 받을까?
{
#if CSFILE_NUM > 0 
	unsigned int uRet		= XTrap_S_SessionInit( XTRAP_DEFAULT_TIMEOUT, CSFILE_NUM, m_Map, o_Session.buffer );
#else
	unsigned int uRet		= XTrap_S_SessionInit( XTRAP_DEFAULT_TIMEOUT, CSFILE_NUM, NULL, o_Session.buffer );
#endif

	if( uRet != XTRAP_API_RETURN_OK )
	{
		FLERROR_LOG( PROGRAM_NAME, "[ ERROR(%u) ]", uRet );
		//DPMng.DestroyPlayer( dpid );
		return FALSE;
	}

	o_Session.dwLastUpdateTick		= ::GetTickCount();			//0으로 넣으면 바로 업데이트 된다.

	return TRUE;
}

void FLXTrap_Server::UpdateCheckSession( CDPMng & DPMng, const DPID dpid, FLSession & o_Session/*, char * o_pPacketBuffer*/ )	//소켓 번호, 소켓 close		//User를 받을까?
{
	const DWORD dwCurrTick	= ::GetTickCount();
	if( o_Session.dwLastUpdateTick + XTRAP_CHECK_EVENT_INTERVAL_MS > dwCurrTick )
		return;

	o_Session.dwLastUpdateTick		= dwCurrTick;

	char arrPacketData[ XTRAP_CS4_COMSIZE_PACKDATA ] = { 0 };
	const unsigned int uRet		= XTrap_CS_Step1( o_Session.buffer, arrPacketData );
//	BEFORESENDSOLE( ar, PACKETTYPE_XTRAP_COMMAND_REQUEST, dpid );
//	ar.Write( arrPacketData, sizeof( arrPacketData ) );
//	SEND( ar, &DPMng, DPID_SERVERPLAYER );

	BEFORESEND( ar, PACKETTYPE_XTRAP_COMMAND_REQUEST );
	ar.Write( arrPacketData, sizeof( arrPacketData ) );
	SEND( ar, &DPMng, dpid );

	if( uRet != XTRAP_API_RETURN_OK )
	{
		FLERROR_LOG( PROGRAM_NAME, "[ ERROR(%u) ]", uRet );
		DPMng.DestroyPlayer( dpid );
		return;
	}
}

void FLXTrap_Server::Response( FLSession & o_Session, const char * pRecvData )
{
	if( pRecvData == NULL )
		return;// false;

	const unsigned int uRet = XTrap_CS_Step3( o_Session.buffer, pRecvData );
	if( uRet != XTRAP_API_RETURN_OK )
	{
		FLERROR_LOG( PROGRAM_NAME, "[ ERROR(%u) ]", uRet );
		return;
	}

#ifdef __INTERNALSERVER
	FLINFO_LOG( PROGRAM_NAME, "[ ret (%u) ]", uRet );
#endif //__INTERNALSERVER
}

BOOL FLXTrap_Server::SetActiveCode( const unsigned int ulActiveCode )
{
	unsigned int uRet		= XTrap_S_SetActiveCode( ulActiveCode );
	if( uRet != XTRAP_API_RETURN_OK )
	{
		FLERROR_LOG( PROGRAM_NAME, "[ ERROR(%u) ]", uRet );
		return FALSE;
	}

	return TRUE;
}

BOOL FLXTrap_Server::SetAllowDelayCycle( const unsigned int uAllowDelayCycle )
{
	unsigned int uRet		= XTrap_S_SetAllowDelay( uAllowDelayCycle );
	if( uRet != XTRAP_API_RETURN_OK )
	{
		FLERROR_LOG( PROGRAM_NAME, "[ ERROR(%u) ]", uRet );
		return FALSE;
	}
	return TRUE;
}


BOOL FLXTrap_Server::loadMapFile( const char * szMapFile )
{
	//const int nRet			= XTrap_S_InitLoadModule( ... );
	//if( nRet != XTRAP_API_RETURN_OK )
	//{
	//	FLERROR_LOG( PROGRAM_NAME, "[ ERROR(%d) ]", nRet );
	//	if( nRet & BW_XTRAP_NOTLOADED_DLL )
	//	{
	//		FLERROR_LOG( PROGRAM_NAME, "[ NOT LOADED XTrap4Server_LoadModule.dll ]" );
	//	}
	//	else if( nRet & BW_XTRAP_WISELOGIC_SUPPORT )
	//	{
	//		FLERROR_LOG( PROGRAM_NAME, "[ send supportteam@wiselogic.co.kr ]" );
	//	}
	//	return FALSE;
	//}

	//return true;

	if( CSFILE_NUM <= m_nMapLoadedCount )
	{
		FLERROR_LOG( PROGRAM_NAME, "[ (%s) INVALID MAP COUNT ]", szMapFile );
		return FALSE;
	}

	FILE * fp			= FLStdFileOpen( szMapFile, "rb" );
	if( fp == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, "[ (%s) NOT EXIST ]", szMapFile );
		return FALSE;
	}

	fread( m_Map[ m_nMapLoadedCount++ ], XTRAP_CS4_BUFSIZE_MAP, 1, fp );
	fclose( fp );

	return TRUE;
}

BOOL FLXTrap_Server::start()
{
//	if( loadMapFile( "adfsa.map" ) == false )
//		return false;

	unsigned int uRet		= XTrap_S_LoadDll();
	if( uRet & BW_XTRAP_SYSTEM_ERROR ) 
	{
		FLERROR_LOG( PROGRAM_NAME, "[ SYSTEM ERROR (%u) : send supportteam@wiselogic.co.kr or search error code ]", uRet );
		return FALSE;
	}

	if( uRet & BW_XTRAP_WISELOGIC_ERROR )
	{
		FLERROR_LOG( PROGRAM_NAME, "[ ERROR (%u) : send supportteam@wiselogic.co.kr ]", uRet );
		return FALSE;
	}

#if CSFILE_NUM > 0 
	uRet					= XTrap_S_Start( XTRAP_DEFAULT_TIMEOUT, CSFILE_NUM, m_Map, 0 );
#else
	uRet					= XTrap_S_Start( XTRAP_DEFAULT_TIMEOUT, CSFILE_NUM, NULL, 0 );
#endif
	if( uRet != XTRAP_API_RETURN_INVALID_FILEVERSION && uRet != XTRAP_API_RETURN_OK ) 
	{
		FLERROR_LOG( PROGRAM_NAME, "[ ERROR (%u) : send supportteam@wiselogic.co.kr ]", uRet );
		return FALSE;
	}

	return TRUE;
}
#endif