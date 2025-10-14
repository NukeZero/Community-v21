
#include "FLServerSock.h"
#include "FLDPSock.h"


CServerSock::CServerSock( FLStreamProtocol* pProtocol )
	: FLServerSockBase< CClientSock >( pProtocol )
{
	m_phCompletionPort	= NULL;
	m_uIoWorker		= 0;
}

CServerSock::~CServerSock()
{
	Close();
}

bool	CServerSock::StartServer( const char* lpAddr, WORD wPort, HANDLE* phCompletionPort, u_long uIoWorker )
{
	// check
	if( phCompletionPort == NULL )
		return false;

	for( u_long i = 0; i < uIoWorker; i++ )
	{
		if( phCompletionPort[i] == NULL )
			return false;
	}

	m_phCompletionPort	= phCompletionPort;
	m_uIoWorker	= uIoWorker;

	if( Accept( lpAddr, wPort ) == false )
	{
		return false;
	}

	return true;
}

void	CServerSock::OnAccept( SOCKET hSocket, DWORD dwAddr )
{
	if( CheckDOS( hSocket, dwAddr ) == false )
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "CheckDOS Socket: %d, dwAddr: %d" ), hSocket, dwAddr );
		return;
	}

	int iIoWorker	= ( hSocket / 4 ) % GetIoWorkerCount();
	PostQueuedCompletionStatus( GetCompletionPort( iIoWorker ), NEWSOCKETMSG, hSocket, (LPOVERLAPPED)dwAddr );
}

bool	CServerSock::OnNewChild( SOCKET hSocket, DWORD dwAddr )
{
	CClientSock* pChild	= AddChild( hSocket );
	if( pChild == NULL )
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "AddChild ½ÇÆÐ Socket: %d, dwAddr: %d" ), hSocket, dwAddr );
		return false;
	}

	int iIoWorker	= ( pChild->GetHandle() / 4 ) % GetIoWorkerCount();
	if( !CreateIoCompletionPort( (HANDLE) pChild->GetHandle(), GetCompletionPort( iIoWorker ), hSocket, GetIoWorkerCount() ) )
	{
		CloseChild( hSocket );

		FLERROR_LOG( GetNetworkLogName(), _T( "CreateIoCompletionPort Failure with error =%d\n" ), WSAGetLastError() );
		return false;
	}

	return true;
}

HANDLE	CServerSock::GetCompletionPort( u_long iIoWorker )	const
{
	FLASSERT( m_phCompletionPort );
	if( 0 <= iIoWorker && iIoWorker < m_uIoWorker )
	{
		return m_phCompletionPort[iIoWorker];
	}

	return NULL;
}

int		CServerSock::GetIoWorkerCount()	const
{
	return m_uIoWorker;
}
