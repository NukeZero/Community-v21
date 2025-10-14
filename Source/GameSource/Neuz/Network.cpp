#include "stdafx.h"
#include "Network.h"

#include "dpcertified.h"
#include "dploginclient.h"
#include "dpclient.h"

extern	CDPCertified	g_dpCertified;
extern	CDPLoginClient	g_dpLoginClient;
extern	CDPClient		g_DPlay;

static char* g_szText[] = 
{ 
	"CERT_CONNECTED",		
	"CERT_CONNECT_FAIL",	
	"CERT_SRVR_LIST",		
	"CERT_ERROR",		
	"LOGIN_CONNECTED",		
	"LOGIN_CONNECT_FAIL",	
	"LOGIN_PLAYER_LIST",		
	"LOGIN_AUTHKEY_ERROR",
	"LOGIN_CACHE_ADDR",		
	"CACHE_CONNECTED",		
	"CACHE_CONNECT_FAIL",	
	"LOGIN_REQ_PREJOIN",			
	"LOGIN_ACK_PREJOIN",			
	"CACHE_ACK_JOIN",		
	"LOGIN_ERROR",
	"CERT_DISCONNECT",		
	"LOGIN_DISCONNECT",		
	"CACHE_DISCONNECT",
	"CACHE_ERROR",
	"LOGIN_CONNECT_STEP_ERROR",
	"CACHE_CONNECT_STEP_ERROR",
};

////////////////////////////////////////////////////////////////////
// CNetwork
////////////////////////////////////////////////////////////////////

CNetwork& CNetwork::GetInstance()
{
	static CNetwork g_network;
	return g_network;
}


CNetwork::CNetwork()
{
}


CNetwork::~CNetwork()
{
}

void CNetwork::OnEvent( NETWORK_EVENT event )
{
	time_t	tmCur = time_null();
	TCHAR szTimeString[64] = { 0, };
	FLTimeToString( szTimeString, _countof( szTimeString ), &tmCur );
	FLERROR_LOG( NETWORK_LOG_NAME, _T( "%s%d - %s" ), szTimeString, event, g_szText[event] );

	LONG lError = 0;
	switch( event )
	{
	case CERT_ERROR:
		lError = g_dpCertified.GetErrorCode();
		FLERROR_LOG( NETWORK_LOG_NAME, _T( "protocol error:%d" ), lError );
		break;
	case LOGIN_ERROR:
		lError = g_dpLoginClient.GetErrorCode();
		FLERROR_LOG( NETWORK_LOG_NAME, _T( "protocol error:%d" ), lError );
		break;
	case CACHE_ERROR:
		lError = g_DPlay.GetErrorCode();
		FLERROR_LOG( NETWORK_LOG_NAME, _T( "protocol error:%d" ), lError );
		break;
	case CERT_CONNECT_FAIL:
		lError = g_dpCertified.GetDebugInfo( DPID_UNKNOWN );
		FLERROR_LOG( NETWORK_LOG_NAME, _T( "network error:%d" ), lError );
		break;
	case CERT_DISCONNECT:
		lError = g_dpCertified.GetNetError();
		FLERROR_LOG( NETWORK_LOG_NAME, _T( "network error:%d" ), lError );
		break;
	case LOGIN_CONNECT_FAIL:
		lError = g_dpLoginClient.GetDebugInfo( DPID_UNKNOWN );
		FLERROR_LOG( NETWORK_LOG_NAME, _T( "network error:%d" ), lError );
		break;
	case LOGIN_DISCONNECT:
		lError = g_dpLoginClient.GetNetError();
		FLERROR_LOG( NETWORK_LOG_NAME, _T( "network error:%d" ), lError );
		break;
	case CACHE_CONNECT_FAIL:
		lError = g_DPlay.GetDebugInfo( DPID_UNKNOWN );
		FLERROR_LOG( NETWORK_LOG_NAME, _T( "network error:%d" ), lError );
		break;
	case CACHE_DISCONNECT:
		lError = g_DPlay.GetNetError();
		FLERROR_LOG( NETWORK_LOG_NAME, _T( "network error:%d" ), lError );
		break;
	}
}
