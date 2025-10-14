
#include "FLAccepter.h"



unsigned int __stdcall AcceptThread( LPVOID lpParam )
{
	FLAccepter* pAccepter	= reinterpret_cast< FLAccepter* >( lpParam );
	if( pAccepter == NULL )
	{
		return 0;
	}

	pAccepter->DoAccept();
	return 0;
}


FLAccepter::FLAccepter()
{
	m_hListenThread = NULL;
	m_hClose = NULL;
	m_hListen = WSA_INVALID_EVENT;

	m_hListenSocket = INVALID_SOCKET;
	m_pHandler = NULL;
}

FLAccepter::~FLAccepter()
{
	Stop();
}

bool	FLAccepter::Create( CSock* pSock, const char* lpAddr, WORD wPort )
{
	SOCKET hSocket = CSock::CreateSocket();
	if( hSocket == INVALID_SOCKET )
	{
		int err = WSAGetLastError();
		pSock->SetDebugInfo( err );

		FLERROR_LOG( GetNetworkLogName(), _T( "CSock::CreateSocket 실패 IP: %s, Port: %d, Err: %d" ), lpAddr, wPort, WSAGetLastError() );
		return false;
	}

	int reuse	= 1;
	setsockopt( hSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast< const char* >( &reuse ), sizeof( reuse ) );

	SOCKADDR_IN sin;
	sin.sin_family		= AF_INET;
	if( lpAddr == NULL || strlen( lpAddr ) == 0 )
	{
		sin.sin_addr.s_addr	= htonl( INADDR_ANY );
	}
	else
	{
		sin.sin_addr.s_addr	= inet_addr( lpAddr );
	}

	sin.sin_port		= htons( wPort );
	if( bind( hSocket, reinterpret_cast< const struct sockaddr* >( &sin ), sizeof( sin ) ) == SOCKET_ERROR )
	{
		int err = WSAGetLastError();
		pSock->SetDebugInfo( err );

		CLOSE_SOCKET( hSocket );

		FLERROR_LOG( GetNetworkLogName(), _T( "bind 실패 IP: %s, Port: %d, Err: %d" ), lpAddr, wPort, err );
		return false;
	}

	if( listen( hSocket, SOMAXCONN ) == SOCKET_ERROR )
	{
		int err = WSAGetLastError();
		pSock->SetDebugInfo( err );

		CLOSE_SOCKET( hSocket );

		FLERROR_LOG( GetNetworkLogName(), _T( "listen 실패 IP: %s, Port: %d, Err: %d" ), lpAddr, wPort, err );
		return false;
	}

	pSock->Attach( hSocket );
	m_hListenSocket = hSocket;
	return true;
}

void	FLAccepter::Destroy()
{
	Stop();
	m_hListenSocket = INVALID_SOCKET;
}

bool	FLAccepter::Start( FLAcceptHandler* pHandler )
{
	m_hListen	= WSACreateEvent();
	FLASSERT( m_hListen != WSA_INVALID_EVENT );
	if( m_hListen == WSA_INVALID_EVENT )
	{
		return false;
	}

	m_hClose	= CreateEvent( NULL, FALSE, FALSE, NULL );
	FLASSERT( m_hClose );
	if( m_hClose == NULL )
	{
		return false;
	}

	DWORD dwThreadId;
	m_hListenThread		= chBEGINTHREADEX( NULL, 0, AcceptThread, this, NULL, &dwThreadId );
	FLASSERT( m_hListenThread );
	if( m_hListenThread == NULL )
	{
		return false;
	}

	if( WSAEventSelect( m_hListenSocket, m_hListen, FD_ACCEPT | FD_CONNECT ) == SOCKET_ERROR )
	{
		return false;
	}

	m_pHandler = pHandler;
	return true;
}

void	FLAccepter::Stop()
{
	CLOSE_WSAEVENT( m_hListen );
	CLOSE_THREAD( m_hListenThread, m_hClose );

	m_pHandler = NULL;
}

void	FLAccepter::DoAccept()
{
	HANDLE handle[2] = { NULL, };
	handle[0]	= m_hClose;
	handle[1]	= m_hListen;

#pragma	warning( push )
#pragma warning( disable : 4127 )	// 조건식이 상수입니다.
	while( 1 )
#pragma	warning( pop )
	{
		DWORD dw	= WaitForMultipleObjects( _countof( handle ), handle, FALSE, INFINITE );
		switch( dw )
		{
		case ( WAIT_OBJECT_0 ):	// close event
			return;

		case ( WAIT_OBJECT_0+1 ):	// accept event
			{
				WSANETWORKEVENTS ne;
				if( WSAEnumNetworkEvents( m_hListenSocket, m_hListen, &ne ) == SOCKET_ERROR )
				{
					FLERROR_LOG( GetNetworkLogName(), _T( "WSAEnumNetworkEvents 실패 ListenSocket: %d" ), m_hListenSocket );
					break;
				}

				if( ne.lNetworkEvents != FD_ACCEPT )
				{
					FLERROR_LOG( GetNetworkLogName(), _T( "ne.lNetworkEvents != FD_ACCEPT" ) );
					break;
				}

				SOCKADDR_IN addr;
				memset( &addr, 0, sizeof( addr ) );
				int nSize = sizeof( addr );

				SOCKET hSocket = WSAAccept( m_hListenSocket, reinterpret_cast< SOCKADDR * >( &addr ), &nSize, nullptr, 0 ); 
				if( hSocket  == INVALID_SOCKET )
				{
					FLERROR_LOG( GetNetworkLogName(), _T( "WSAAccept 실패 Err: %d" ), WSAGetLastError() );
					break;
				}

				DWORD dwAddr = addr.sin_addr.S_un.S_addr;
				OnAccept( hSocket, dwAddr );
				break;
			}

		case WAIT_TIMEOUT:
			break;
		}
	}
}

void	FLAccepter::OnAccept( SOCKET hSocket, DWORD dwAddr )
{
	if( m_pHandler != NULL )
	{
		m_pHandler->OnAccept( hSocket, dwAddr );
	}
}
