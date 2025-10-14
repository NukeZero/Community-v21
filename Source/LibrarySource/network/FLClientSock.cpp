
#include "FLClientSock.h"
#include "FLConnecter.h"
#include "../thread/FLAutoLock.h"


MEMPOOLER_IMPLEMENT( CClientSock, 128 );


CClientSock::CClientSock( FLStreamProtocol* pProtocol )
{
	m_pRecvBuffer	= CBufferFactory::GetInstance().CreateBuffer( MAX_BUFFER );
	m_pProtocol = pProtocol->Clone();

	m_bDoNotWrite	= FALSE;
	m_nReference	= 0;
}

CClientSock::~CClientSock()
{
	Close();

	if( m_pProtocol != NULL )
	{
		m_pProtocol->Release();
		m_pProtocol = NULL;
	}
}

bool	CClientSock::Connect( const char* lpAddr, WORD wPort )
{
	FLConnecter cConnecter;
	if( cConnecter.Connect( this, lpAddr, wPort ) == false )
	{
		return false;
	}

	return true;
}

bool	CClientSock::Recv()
{
	FLASSERT( m_hSocket != INVALID_SOCKET );
	DWORD dwBytesRecvd	= 0, dwFlags	= 0;

	WSABUF buffer;
	buffer.buf	= (char*)m_pRecvBuffer->GetWritableBuffer( &buffer.len );

	memset( &m_ovRecv, 0, sizeof(m_ovRecv) );

	if( WSARecv( GetHandle(), &buffer, 1, &dwBytesRecvd, &dwFlags, &m_ovRecv, NULL ) != 0 )
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

bool	CClientSock::Fetch( DWORD dwBytes, CBufferQueue* pQueue )
{
	if( m_hSocket == INVALID_SOCKET )
		return false;

	m_pRecvBuffer->UseTail( dwBytes );
	return m_pProtocol->FetchStream( &m_pRecvBuffer, pQueue );
}

bool CClientSock::PostSend()
{
	DWORD dwBufferCount, dwBytes;

	WSABUF wsabuf[MAX_WSABUF];
	m_lspSendBuffer.GetData( wsabuf, &dwBufferCount );
	memset( &m_ovSend, 0, sizeof(m_ovSend) );

	if (WSASend(GetHandle(), wsabuf, dwBufferCount, &dwBytes, 0, &m_ovSend, nullptr) != 0)
	{
		const int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			SetDebugInfo(err);
			FLERROR_LOG(GetNetworkLogName(), _T("Send 실패 Socket: %d, err: %d"), GetHandle(), err);
			return false;
		}
	}
	return true;
}

bool CClientSock::Send( const char* lpData, u_long dwDataSize, DPID )
{
	if( m_hSocket == INVALID_SOCKET )
		return false;

	CMclAutoLock Lock1( m_AccessLock );
	if( m_bDoNotWrite )
		return false;

	CMclAutoLock Lock(m_csSendBuffer);
	if (m_lspSendBuffer.IsEmpty())
	{
		if (m_pProtocol->SendStream(&m_lspSendBuffer, LPBYTE(lpData), dwDataSize) == true)
		{
			if (m_lspSendBuffer.IsEmpty())
				return true;

			if( PostSend() == true )
			{
				++m_nReference;
				return true;
			}
		}
		return false;
	}
	return m_pProtocol->SendStream( &m_lspSendBuffer, LPBYTE(lpData), dwDataSize );
}

bool CClientSock::SendRemnant(DWORD dwBytes)
{
	FLASSERT( m_hSocket != INVALID_SOCKET );
	CMclAutoLock	Lock( m_csSendBuffer );

	m_lspSendBuffer.RemoveData( dwBytes );

	if( m_lspSendBuffer.IsEmpty() )
	{
		if( !m_bDoNotWrite )
			m_nReference--;

		return (m_bDoNotWrite == FALSE);
	}

	return PostSend();
}

void	CClientSock::Close()
{
	CLOSE_SOCKET( m_hSocket );
	SAFE_DELETE( m_pRecvBuffer );
}

bool	CClientSock::CloseConnection( SOCKET hSocket )
{
	(hSocket);
	FLASSERT( hSocket == m_hSocket );
	Close();
	return true;
}

bool	CClientSock::StartProtocol()
{
	if( m_hSocket == INVALID_SOCKET )
		return false;

	CMclAutoLock	Lock1( m_AccessLock );
	if( m_bDoNotWrite )
		return false;

	CMclAutoLock	Lock( m_csSendBuffer );
	if( m_lspSendBuffer.IsEmpty() )
	{
		if( m_pProtocol->StartProtocol( &m_lspSendBuffer ) == true )
		{
			if( m_lspSendBuffer.IsEmpty() )
			{
				return true;
			}

			if( PostSend() == true )
			{
				++m_nReference;
				return true;
			}
		}

		return false;
	}

	return m_pProtocol->StartProtocol( &m_lspSendBuffer );
}

bool	CClientSock::ReadySyncProtocol()
{
	return m_pProtocol->ReadySyncProtocol();
}

bool	CClientSock::WaitSyncProtocol( DWORD dwWaitTime )
{
	if( m_pProtocol != NULL )
	{
		return m_pProtocol->WaitSyncProtocol( dwWaitTime );
	}

	return false;
}
