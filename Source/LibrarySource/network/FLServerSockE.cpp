
#include "FLServerSockE.h"
#include "FLDPSock.h"


CServerSockE::CServerSockE( FLStreamProtocol* pProtocol )
	: FLServerSockBase< CClientSockE >( pProtocol )
{
	m_pDPSock	= NULL;
}

CServerSockE::~CServerSockE()
{
	m_pDPSock	= NULL;
}

bool	CServerSockE::StartServer( const char* lpAddr, WORD wPort, CDPSock* pDPSock )
{
	Close();

	m_pDPSock	= pDPSock;
	if( Accept( lpAddr, wPort ) == false )
	{
		return false;
	}

	return true;
}

void	CServerSockE::OnAccept( SOCKET hSocket, DWORD dwAddr )
{
	if( CheckDOS( hSocket, dwAddr ) == false )
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "CheckDOS Socket: %d, dwAddr: %d" ), hSocket, dwAddr );
		return;
	}

	CClientSockE* pChild	= AddChild( hSocket );
	if( pChild == NULL )
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "AddChild 실패 Socket: %d, dwAddr: %d" ), hSocket, dwAddr );
		return;
	}

	FLASSERT( m_pDPSock );
	m_pDPSock->CreateEventWorker( pChild );

	if( SendProtocolSync( pChild ) == false )
	{
		CloseChild( hSocket );

		FLERROR_LOG( GetNetworkLogName(), _T( "StartProtocol 실패 Socket: %d, dwAddr: %d" ), hSocket, dwAddr );
		return;
	}

	if( pChild->Recv() == false )
	{
		CloseChild( hSocket );

		FLERROR_LOG( GetNetworkLogName(), _T( "Recv 실패 Socket: %d, dwAddr: %d, Err: %d" ), hSocket, dwAddr, WSAGetLastError() );
		return;
	}
}
