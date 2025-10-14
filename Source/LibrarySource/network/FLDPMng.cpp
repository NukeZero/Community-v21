
#include "FLDPMng.h"
#include "../common/FLSystemUtil.h"


unsigned int	__stdcall ReceiveThread( LPVOID lpvThreadParameter );

CDPMng::CDPMng()
{
	m_pDPSock	= NULL;
	m_hRecvThread	= m_hClose	= (HANDLE)NULL;
	m_pProtocol = NULL;
}

CDPMng::~CDPMng()
{
}

BOOL	CDPMng::CreateDPObject( FLStreamProtocol* pProtocol, BOOL bSingleThreaded )
{
	DeleteDPObject();

	m_pProtocol = pProtocol->Clone();

	if( !m_pDPSock )
		m_pDPSock	= new CDPSock;

	if( !bSingleThreaded )
	{
		// Create event used to signal that the receive thread should exit
		if( !m_hClose )
		{
			m_hClose	= CreateEvent( NULL, FALSE, FALSE, NULL );
			FLASSERT( m_hClose );			
		}
		// Create thread to receive player messages
		if( !m_hRecvThread )
		{
			DWORD dwThreadId;
			m_hRecvThread	= chBEGINTHREADEX( NULL, 0, ReceiveThread, this, 0, &dwThreadId );
			FLASSERT( m_hRecvThread );
			SetThreadPriority( m_hRecvThread, THREAD_PRIORITY_ABOVE_NORMAL );
		}
	}

	return TRUE;
}

BOOL	CDPMng::DeleteDPObject()
{
	// Wake up receive thread and wait for it to quit
	CLOSE_THREAD( m_hRecvThread, m_hClose );
	if( m_pDPSock )
	{
		m_pDPSock->Close();
		SAFE_DELETE( m_pDPSock );
	}

	if( m_pProtocol != NULL )
	{
		m_pProtocol->Release();
		m_pProtocol = NULL;
	}

	return TRUE;
}

void	CDPMng::SetConnectionInfo( const char* szIP, WORD wPort )
{
	m_pDPSock->SetConnectionInfo( szIP, wPort );
}

BOOL	CDPMng::JoinSession( FLStreamProtocol* pProtocol, u_long uWaitingTime )
{
	if( FLIsWindows9x() == TRUE )
	{
		if( m_pDPSock->JoinToServerE( pProtocol, uWaitingTime ) == FALSE )
		{
			return FALSE;
		}
	}
	else
	{
		if( m_pDPSock->JoinToServer( pProtocol, uWaitingTime ) == FALSE )
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL	CDPMng::CreateSession( FLStreamProtocol* pProtocol )
{
	if( FLIsWindows9x() == TRUE )
	{
		if( m_pDPSock->CreateServerE( pProtocol ) == FALSE )
		{
			return FALSE;
		}
	}
	else
	{
		if( m_pDPSock->CreateServer( pProtocol ) == FALSE )
		{
			return FALSE;
		}
	}

	return TRUE;
}

void CDPMng::GetHostAddr( LPSTR lpAddr, size_t cchAddr )
{
	if( m_pDPSock )
	{
		m_pDPSock->GetHostAddr( lpAddr, cchAddr );
	}
}

void CDPMng::GetPlayerAddr( DPID dpid, char* lpAddr, size_t cchAddr )
{
	if( m_pDPSock )
	{
		m_pDPSock->GetPlayerAddr( dpid, lpAddr, cchAddr );
	}
}

DWORD CDPMng::GetPlayerAddr( DPID dpid )
{
	if( m_pDPSock )
	{
		return m_pDPSock->GetPlayerAddr( dpid );
	}
	return 0;
}

BOOL CDPMng::ReceiveMessage()
{
	if( !m_pDPSock )
		return FALSE;

#pragma	warning( push )
#pragma warning( disable : 4127 )	// 조건식이 상수입니다.
	while( 1 )
#pragma	warning( pop )
	{
		CBufferQueue cQueue;
		m_pDPSock->Receive( &cQueue );
		if( cQueue.IsEmpty() == TRUE )
		{
			break;
		}

		CBuffer* pBuffer = NULL;
		while( (pBuffer = cQueue.PopHead()) != NULL )
		{
			u_long nBuffSize = 0;
			LPBYTE ptr = pBuffer->GetReadableBuffer( &nBuffSize );
			u_long nBlockCount = pBuffer->GetBlockCount();
			while( nBlockCount-- > 0 )
			{
				void* pData = NULL;
				u_long nDataSize = NULL;
				m_pProtocol->GetData( ptr, &pData, &nDataSize );

				if( pBuffer->GetDPID() == DPID_SYSMSG )
				{
					SysMessageHandler( (LPDPMSG_GENERIC)pData, nDataSize, pBuffer->GetDPID() );
				}
				else
				{
					UserMessageHandler( (LPDPMSG_GENERIC)pData, nDataSize, pBuffer->GetDPID() );
				}

				ptr = reinterpret_cast< LPBYTE >( m_pProtocol->GetNextStream( ptr ) );
			}

			SAFE_DELETE( pBuffer );
		}

		FLASSERT( cQueue.IsEmpty() );
	}

	return TRUE;
}

BOOL CDPMng::DestroyPlayer( DPID dpid )
{
	if( m_pDPSock )
	{
		return m_pDPSock->Shutdown( dpid );
	}
	return FALSE;
}

DWORD CDPMng::GetDebugInfo( DPID dpid )
{
	if( m_pDPSock )
		return m_pDPSock->GetDebugInfo( dpid );
	return 0;
}

unsigned int __stdcall ReceiveThread( LPVOID lpvThreadParameter )
{
	CDPMng* pThis	= (CDPMng*)lpvThreadParameter;
	HANDLE lpHandle[2];
	lpHandle[0]		= pThis->GetCloseHandle();
	lpHandle[1]		= pThis->GetRecvHandle();
	// loop waiting for player events. If the kill event is signaled, the thread will exit

	while( WaitForMultipleObjects( 2, lpHandle, FALSE, INFINITE ) != WAIT_OBJECT_0 )
	{
		// receive any messages in the queue
		pThis->ReceiveMessage();
	}
	return( 0 );
}


BOOL InitializeNetLib()
{
	WSADATA wsaData;
	int err	= WSAStartup( 0x0202, &wsaData );
	if( err == SOCKET_ERROR )
	{
		return FALSE;
	}

	return TRUE;
}

void UninitializeNetLib()
{
	MEMPOOLER_DESTROY( CClientSock );
	MEMPOOLER_DESTROY( CClientSockE );

	MEMPOOLER_DESTROY( CBuffer );

	MEMPOOLER_DESTROY( FLXORProtocol );

	WSACleanup();
}
