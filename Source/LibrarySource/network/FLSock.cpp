
#include "FLSock.h"
#include "../common/FLStringFunction.h"

//////////////////////////////////////////////////////////////////////////
bool	CSock::GetHostAddr( char* lpAddr, size_t cchAddr )
{
	FLASSERT( lpAddr );

	char name[255] = { 0, };
	if( gethostname( name, sizeof( name ) ) == SOCKET_ERROR )
	{
		return false;
	}

	LPHOSTENT lphost = NULL;
	SOCKADDR_IN sin;
	memset( &sin, 0, sizeof( sin ) );

	sin.sin_family	= AF_INET;
	lphost	= gethostbyname( name );
	if( lphost != NULL )
	{
		sin.sin_addr.s_addr		= ( (LPIN_ADDR)lphost->h_addr )->s_addr;
	}
	else
	{
		return false;
	}

	FLSPrintf( lpAddr, cchAddr, "%d.%d.%d.%d", sin.sin_addr.s_net
		, sin.sin_addr.s_host
		, sin.sin_addr.s_lh
		, sin.sin_addr.s_impno );

	return true;
}

SOCKET	CSock::CreateSocket( int type )
{
	int protocol = IPPROTO_TCP;
	if( type == SOCK_DGRAM )
	{
		protocol = IPPROTO_UDP;
	}

	SOCKET hSocket = WSASocket( AF_INET, type, protocol, NULL, 0, WSA_FLAG_OVERLAPPED );
	if( hSocket == INVALID_SOCKET )
	{
		return hSocket;
	}

	LINGER lLinger;
	lLinger.l_onoff		= 0;
	lLinger.l_linger	= 0;
	setsockopt( hSocket, SOL_SOCKET, SO_LINGER, reinterpret_cast< const char* >( &lLinger ), sizeof( lLinger ) );

	return hSocket;
}

void	CSock::CloseSocket( SOCKET hSocket )
{
	if( hSocket != INVALID_SOCKET )
	{
		shutdown( hSocket, SD_BOTH );
		closesocket( hSocket );
		hSocket	= INVALID_SOCKET;
	}
}
//////////////////////////////////////////////////////////////////////////


CSock::CSock()
{
	m_hSocket	= INVALID_SOCKET;
	m_dpid	= m_dpidpeer	= DPID_UNKNOWN;
	m_dwDbgInfo = 0;
}

CSock::~CSock()
{
	Clear();
}


SOCKET	CSock::GetHandle()
{
	return m_hSocket;
}

void	CSock::SetID( DPID dpid )
{
	m_dpid	= dpid;
}

DPID	CSock::GetID()
{
	return m_dpid;
}

void	CSock::SetPeerID( DPID dpid )
{
	m_dpidpeer	= dpid;
}

DPID	CSock::GetPeerID()
{
	return m_dpidpeer;
}

void	CSock::Clear()
{
	CLOSE_SOCKET( m_hSocket );
	m_dpid	= m_dpidpeer	= DPID_UNKNOWN;
}

void	CSock::Attach( SOCKET hSocket )
{
	Clear();
	m_hSocket	= hSocket;
}

void	CSock::Detach()
{
	m_hSocket	= INVALID_SOCKET;
	m_dpid	= m_dpidpeer	= DPID_UNKNOWN;
}

bool	CSock::GetPeerAddr( DPID /*dpidPlayer*/, char* lpAddr, size_t cchAddr )
{
	FLASSERT( lpAddr );

	SOCKADDR_IN sin;
	::memset( &sin, 0, sizeof( sin ) );

	int namelen	= sizeof( sin );
	if( getpeername( m_hSocket, reinterpret_cast< SOCKADDR* >( &sin ), &namelen ) == SOCKET_ERROR )
	{
		if( lpAddr != NULL && cchAddr > 0 )
		{
			*lpAddr = '\0';
		}

		return false;
	}

	FLSPrintf( lpAddr, cchAddr, "%d.%d.%d.%d", sin.sin_addr.s_net
		, sin.sin_addr.s_host
		, sin.sin_addr.s_lh
		, sin.sin_addr.s_impno );

	return true;
}

DWORD	CSock::GetPeerAddr( DPID /*dpid*/ )
{
	if( m_hSocket == INVALID_SOCKET )
	{
		return 0;
	}

	SOCKADDR_IN sin;
	::memset( &sin, 0, sizeof( sin ) );

	int namelen = sizeof( sin );
	if( getpeername( m_hSocket, reinterpret_cast< SOCKADDR* >( &sin ), &namelen ) == SOCKET_ERROR )
	{
		return 0;
	}

	return sin.sin_addr.S_un.S_addr;
}

CSock*	CSock::Get( SOCKET hSocket )
{
	return (m_hSocket == hSocket ? this : NULL);
}

void	CSock::SetDebugInfo( DWORD dwError )
{
	m_dwDbgInfo = dwError;
}

DWORD	CSock::GetDebugInfo( SOCKET /*hSocket*/ )
{
	return m_dwDbgInfo;
}
