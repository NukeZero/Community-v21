
#include "FLConnecter.h"


FLConnecter::FLConnecter()
{
}

FLConnecter::~FLConnecter()
{
}

bool	FLConnecter::Connect( CSock* pSock, const char* lpAddr, WORD wPort )
{
	SOCKET hSocket = CSock::CreateSocket();
	if( hSocket == INVALID_SOCKET )
	{
		int err = WSAGetLastError();
		pSock->SetDebugInfo( err );

		FLERROR_LOG( GetNetworkLogName(), _T( "CSock::CreateSocket 실패 IP: %s, Port: %d, Err: %d" ), lpAddr, wPort, WSAGetLastError() );
		return false;
	}

	SOCKADDR_IN sin;
	memset( &sin, 0, sizeof( sin ) );

	sin.sin_family		= AF_INET;
	sin.sin_addr.s_addr	= inet_addr( lpAddr );
	sin.sin_port = htons( wPort );

	if( sin.sin_addr.s_addr == INADDR_NONE )
	{
		LPHOSTENT lphost;
		lphost	= gethostbyname( lpAddr );
		if( lphost )
		{
			sin.sin_addr.s_addr	= ((LPIN_ADDR)lphost->h_addr)->s_addr;
		}
		else
		{
			pSock->SetDebugInfo( WSAEINVAL );
			WSASetLastError( WSAEINVAL );
			CLOSE_SOCKET( hSocket );

			FLERROR_LOG( GetNetworkLogName(), _T( "gethostbyname 실패 IP: %s, Port: %d, Err: %d" ), lpAddr, wPort, WSAGetLastError() );
			return false;
		}
	}

	if( connect( hSocket, reinterpret_cast< LPSOCKADDR >( &sin ), sizeof( sin ) ) == SOCKET_ERROR )
	{
		int	err = WSAGetLastError();
		if( err == WSAEWOULDBLOCK )
		{
			return TRUE;
		}

		pSock->SetDebugInfo( err );
		FLERROR_LOG( GetNetworkLogName(), _T( "connect 실패 IP: %s, Port: %d, Err: %d" ), lpAddr, wPort, err );

		CLOSE_SOCKET( hSocket );
		return false;
	}

	pSock->Attach( hSocket );
	return true;
}
