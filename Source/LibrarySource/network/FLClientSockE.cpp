
#include "FLClientSockE.h"
#include "FLConnecter.h"
#include "../thread/FLAutoLock.h"


MEMPOOLER_IMPLEMENT( CClientSockE, 128 );


CClientSockE::CClientSockE( FLStreamProtocol* pProtocol )
{
	m_hRecv		= m_hSend	= WSA_INVALID_EVENT;
	m_hWorker	= NULL;

	m_pRecvBuffer	= CBufferFactory::GetInstance().CreateBuffer( MAX_BUFFER );

	m_pProtocol = pProtocol->Clone();
}

CClientSockE::~CClientSockE()
{
	Close();

	if( m_pProtocol != NULL )
	{
		m_pProtocol->Release();
	}
}

bool	CClientSockE::Connect( const char* lpAddr, WORD wPort )
{
	FLConnecter cConnecter;
	if( cConnecter.Connect( this, lpAddr, wPort ) == false )
	{
		return false;
	}

	return true;
}

void	CClientSockE::Attach( SOCKET hSocket )
{
	Clear();

	if( m_hRecv == WSA_INVALID_EVENT )
		m_hRecv		= WSACreateEvent();
	if( m_hSend == WSA_INVALID_EVENT )
		m_hSend		= WSACreateEvent();

	CSock::Attach( hSocket );
}

bool	CClientSockE::Recv()
{
	FLASSERT( m_hSocket != INVALID_SOCKET );
	DWORD dwBytesRecvd, dwFlags	= 0;

	WSABUF buffer;
	buffer.buf	= (char*)m_pRecvBuffer->GetWritableBuffer( &buffer.len );

	memset( &m_ovRecv, 0, sizeof(m_ovRecv) );
	m_ovRecv.hEvent		= m_hRecv;

	if( WSARecv( GetHandle(), &buffer, (DWORD)1, &dwBytesRecvd, &dwFlags, &m_ovRecv, NULL ) != 0 )
	{
		int err = WSAGetLastError();
		if( err != WSA_IO_PENDING )
		{
			SetDebugInfo( err );

			FLERROR_LOG( GetNetworkLogName(), _T( "Recv 실패 Socket: %d, err: %d" ), GetHandle(), err );
			return false;
		}
	}
	return true;
}

bool	CClientSockE::Fetch( DWORD dwBytes, CBufferQueue* pQueue )
{
	if( m_hSocket == INVALID_SOCKET )
	{
		return false;
	}

	m_pRecvBuffer->UseTail( dwBytes );

	return m_pProtocol->FetchStream( &m_pRecvBuffer, pQueue );
}

bool	CClientSockE::PostSend()
{
	if( m_lspSendBuffer.IsEmpty() )
	{
		return true;
	}

	DWORD dwBufferCount, dwBytes;

	WSABUF	wsabuf[MAX_WSABUF];
	m_lspSendBuffer.GetData( wsabuf, &dwBufferCount );

	memset( &m_ovSend, 0, sizeof(m_ovSend) );
	m_ovSend.hEvent		= m_hSend;

	if( WSASend( GetHandle(), wsabuf, dwBufferCount, &dwBytes, 0, &m_ovSend, NULL ) != 0 )
	{
		int err = WSAGetLastError();
		if( err != WSA_IO_PENDING )
		{
			SetDebugInfo( err );

			FLERROR_LOG( GetNetworkLogName(), _T( "Send 실패 Socket: %d, err: %d" ), GetHandle(), err );
			return false;
		}
	}

	return true;
}

bool	CClientSockE::Send( const char* lpData, u_long dwDataSize, DPID /*dpidTo*/ )
{
	FLASSERT( m_hSocket != INVALID_SOCKET );

	CMclAutoLock	Lock( m_csSendBuffer );

	if( m_lspSendBuffer.IsEmpty() )
	{
		if( m_pProtocol->SendStream( &m_lspSendBuffer, (LPBYTE)lpData, dwDataSize ) == true )
		{
			return PostSend();
		}

		return false;
	}

	return m_pProtocol->SendStream( &m_lspSendBuffer, (LPBYTE)lpData, dwDataSize );
}

bool	CClientSockE::SendRemnant( DWORD dwBytes )
{
	FLASSERT( m_hSocket != INVALID_SOCKET );
	CMclAutoLock	Lock( m_csSendBuffer );

	m_lspSendBuffer.RemoveData( dwBytes );
	if( m_lspSendBuffer.IsEmpty() )
	{
		return true;
	}

	return PostSend();
}

void	CClientSockE::Close()
{
	CLOSE_SOCKET( m_hSocket );
	CLOSE_WSAEVENT( m_hRecv );
	CLOSE_WSAEVENT( m_hSend );

	SAFE_DELETE( m_pRecvBuffer );
}

bool	CClientSockE::CloseConnection( SOCKET hSocket )
{
	(hSocket);
	FLASSERT( hSocket == m_hSocket );
	Close();
	return true;
}

WSAEVENT	CClientSockE::GetRecvEvent()
{
	return m_hRecv;
}

WSAEVENT	CClientSockE::GetSendEvent()
{
	return m_hSend;
}

bool	CClientSockE::StartProtocol()
{
	FLASSERT( m_hSocket != INVALID_SOCKET );

	CMclAutoLock	Lock( m_csSendBuffer );
	if( m_lspSendBuffer.IsEmpty() )
	{
		if( m_pProtocol->StartProtocol( &m_lspSendBuffer ) == true )
		{
			return PostSend();
		}

		return false;
	}

	return m_pProtocol->StartProtocol( &m_lspSendBuffer );
}

bool	CClientSockE::ReadySyncProtocol()
{
	return m_pProtocol->ReadySyncProtocol();
}

bool	CClientSockE::WaitSyncProtocol( DWORD dwWaitTime )
{
	if( m_pProtocol != NULL )
	{
		return m_pProtocol->WaitSyncProtocol( dwWaitTime );
	}

	return false;
}
