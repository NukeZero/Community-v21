
#include "FLDPSock.h"
#include "../common/FLStringFunction.h"
#include "../common/FLSystemUtil.h"

#include <list>


unsigned int	__stdcall	IoWorkerThread( LPVOID lpvThreadParameter );
unsigned int	__stdcall	EventWorkerThread( LPVOID lpvThreadParameter );

CDPSock::CDPSock()
{
	m_fServer	= FALSE;
	m_pSock		= NULL;
	m_phCompletionPort	= NULL;
	m_lActiveIoWorker	= 0;
	m_hRecv		= CreateEvent( NULL, FALSE, FALSE, NULL );
	FLASSERT( m_hRecv );

	m_hClose	= WSACreateEvent();
	FLASSERT( m_hClose );

	m_uIoWorker		= 0;
	memset( m_lpAddr, 0, sizeof( m_lpAddr ) );
	m_pSockThreaded	= NULL;

	m_pProtocol = NULL;
}

CDPSock::~CDPSock()
{
	CLOSE_HANDLE( m_hRecv );
	CLOSE_WSAEVENT( m_hClose );

	Close();

	if( m_pProtocol != NULL )
	{
		m_pProtocol->Release();
		m_pProtocol = NULL;
	}
}

void	CDPSock::Close()
{
	CloseIoWorker();

	if( m_pSock )
	{
		m_pSock->Close();
		SAFE_DELETE( m_pSock );
	}

	CMclAutoLock	Lock( m_csRecvBuffer );
	m_lspRecvBuffer.Clear( TRUE );
}

void	CDPSock::SetConnectionInfo( const char* szIP, WORD wPort )
{
	if( szIP != NULL )
	{
		FLStrcpy( m_lpAddr, _countof( m_lpAddr ), szIP );
	}
	else
	{
		memset( m_lpAddr, 0, sizeof( m_lpAddr ) );
	}

	m_wPort	= wPort;
}

BOOL	CDPSock::CreateIoWorker( u_long uIoWorker )
{
	HANDLE hThread;

	m_uIoWorker	= uIoWorker;
	m_phCompletionPort	= new HANDLE[uIoWorker];
	for( DWORD iIoWorker = 0; iIoWorker < uIoWorker; iIoWorker++ )
	{
		if( (m_phCompletionPort[iIoWorker] = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 )) == NULL )
			return FALSE;
	}

	DWORD dwThreadId;
	for( DWORD i = 0; i < m_uIoWorker; i++ )
	{
		hThread		= chBEGINTHREADEX( NULL, 0, IoWorkerThread, this, 0, &dwThreadId );
		FLASSERT( hThread );
		SetThreadPriority( hThread, THREAD_PRIORITY_ABOVE_NORMAL );
		m_listthread.push_back( hThread );
	}
	return TRUE;
}

BOOL CDPSock::CloseIoWorker()
{
	DWORD cbThread;
	LPHANDLE lphThread;
	
	m_locklistthread.Enter();
	cbThread	= static_cast< DWORD >( m_listthread.size() );
	if( cbThread > 0 )
	{
		lphThread	= new HANDLE[cbThread];
		for( DWORD i = 0; i < cbThread; i++ )
		{
			lphThread[i]	= m_listthread.front();
			m_listthread.pop_front();
			if( !FLIsWindows9x() )
			{
				PostQueuedCompletionStatus( m_phCompletionPort[i], CLOSEIOWORKERMSG, TRUE, NULL );
			}
		}

		if( FLIsWindows9x() )
			WSASetEvent( m_hClose );

		WaitForMultipleObjects( cbThread, lphThread, TRUE, INFINITE );

		for( DWORD i = 0; i < cbThread; i++ )
		{
			CloseHandle( (HANDLE)lphThread[i] );
		}
		SAFE_DELETE_ARRAY( lphThread );
	}
	if( m_phCompletionPort )
	{
		for( DWORD iIoWorker = 0; iIoWorker < m_uIoWorker; iIoWorker++ )
			CLOSE_HANDLE( m_phCompletionPort[iIoWorker] );
		SAFE_DELETE_ARRAY( m_phCompletionPort );
	}
	m_uIoWorker	= 0;
	m_locklistthread.Leave();
	return TRUE;
}

BOOL CDPSock::CreateEventWorker( CSock* pSock )
{
	HANDLE hThread;
	m_uIoWorker++;
	m_pSockThreaded	= pSock;
	AddCreatePlayerOrGroupMsg( (DPID)pSock->GetHandle() );

	DWORD dwThreadId;
	hThread		= chBEGINTHREADEX( NULL, 0, EventWorkerThread, this, 0, &dwThreadId );
	FLASSERT( hThread );
	SetThreadPriority( hThread, THREAD_PRIORITY_NORMAL );

	( (CClientSockE*)pSock )->m_hWorker	= hThread;
	m_locklistthread.Enter();
	m_listthread.push_back( hThread );
	m_locklistthread.Leave();
	return TRUE;
}

void CDPSock::RemoveThread( HANDLE hThread )
{
	HANDLE h;
	m_locklistthread.Enter();

	if( m_listthread.size() > 0 )
	{
		for( std::list<HANDLE>::iterator i = m_listthread.begin(); i != m_listthread.end(); ++i )
		{
			h	= *i;
			if( h == hThread )
			{
				m_listthread.erase( i );
				m_uIoWorker--;
				break;
			}
		}
	}
	m_locklistthread.Leave();
}

BOOL	CDPSock::CreateServer( FLStreamProtocol* pProtocol )
{
	Close();

	m_pProtocol = pProtocol->Clone();

	FLSystemInfo si;
	CreateIoWorker( si.dwNumberOfProcessors*2 );

	CServerSock* pSock	= new CServerSock( pProtocol );

	m_fServer	= TRUE;
	m_pSock		= pSock;
	pSock->SetID( DPID_SERVERPLAYER );

	if( pSock->StartServer( m_lpAddr, m_wPort, m_phCompletionPort, m_uIoWorker ) == false )
	{
		SAFE_DELETE( pSock );
		m_pSock = NULL;
		return FALSE;
	}

	return TRUE;
}

BOOL	CDPSock::CreateServerE( FLStreamProtocol* pProtocol )
{
	Close();

	m_pProtocol = pProtocol->Clone();

	CServerSockE* pSock		= new CServerSockE( pProtocol );

	m_fServer	= TRUE;
	m_pSock		= pSock;
	pSock->SetID( DPID_SERVERPLAYER );

	if( pSock->StartServer( m_lpAddr, NULL, this ) == false )
	{
		SAFE_DELETE( pSock );
		m_pSock = NULL;
		return FALSE;
	}

	return TRUE;
}

BOOL	CDPSock::JoinToServer( FLStreamProtocol* pProtocol, u_long uWaitingTime )
{
	Close();

	m_pProtocol = pProtocol->Clone();

	m_fServer	= FALSE;

	CClientSock* pSock = new CClientSock( pProtocol );
	if( pSock->Connect( m_lpAddr, m_wPort ) == false )
	{
		SAFE_DELETE( pSock );
		return FALSE;
	}

	pSock->SetID( pSock->GetHandle() );
	pSock->SetPeerID( DPID_SERVERPLAYER );
	m_pSock		= pSock;

	if( pSock->ReadySyncProtocol() == false )
	{
		SAFE_DELETE( pSock );
		m_pSock = NULL;
		return FALSE;
	}

	CreateIoWorker( (u_long)0x01 );

	int iIoWorker	= ( pSock->GetHandle() / 4 ) % GetIoWorkerCount();
	if( !CreateIoCompletionPort( (HANDLE)pSock->GetHandle(), m_phCompletionPort[iIoWorker], pSock->GetHandle(),  0 ) )
	{
		SAFE_DELETE( pSock );
		m_pSock = NULL;
		return FALSE;
	}

	PostQueuedCompletionStatus( m_phCompletionPort[iIoWorker], NEWSOCKETMSG, pSock->GetHandle(), NULL );

	if( pSock->WaitSyncProtocol( uWaitingTime ) == false )
	{
		PostQueuedCompletionStatus( m_phCompletionPort[iIoWorker], (DWORD)-4, pSock->GetHandle(), NULL );
		return FALSE;
	}

	return TRUE;
}

BOOL	CDPSock::JoinToServerE( FLStreamProtocol* pProtocol, u_long uWaitingTime )
{
	Close();

	m_pProtocol = pProtocol->Clone();

	m_fServer	= FALSE;

	CClientSockE* pSock	= new CClientSockE( pProtocol );
	if( pSock->Connect( m_lpAddr, m_wPort ) == false )
	{
		SAFE_DELETE( pSock );
		return FALSE;
	}

	m_pSock		= pSock;
	pSock->SetID( pSock->GetHandle() );
	pSock->SetPeerID( DPID_SERVERPLAYER );

	if( pSock->ReadySyncProtocol() == false )
	{
		SAFE_DELETE( pSock );
		m_pSock = NULL;
		return FALSE;
	}

	CreateEventWorker( (CClientSockE*)pSock );

	if( pSock->Recv() == false )
	{
		RemoveThread( pSock->m_hWorker );
		CloseConnection( pSock->GetID() );
		return FALSE;
	}

	if( pSock->WaitSyncProtocol( uWaitingTime ) == false )
	{
		RemoveThread( pSock->m_hWorker );
		CloseConnection( pSock->GetID() );
		return FALSE;
	}

	return TRUE;
}

BOOL CDPSock::Send( char* lpData, DWORD dwDataSize, DPID dpidTo )
{
	if( m_pSock )
	{
		if( m_pSock->Send( lpData, dwDataSize, dpidTo ) == true )
		{
			return TRUE;
		}
	}
	return FALSE;
}

void	CDPSock::Receive( CBufferQueue* pQueue )
{
	CMclAutoLock	Lock( m_csRecvBuffer );

	pQueue->AddTail( &m_lspRecvBuffer );
}

void CDPSock::CloseConnection( DPID dpid )
{
	if( m_pSock )
	{
		if( m_pSock->CloseConnection( dpid ) )
		{
			AddDestroyPlayerOrGroupMsg( dpid );
		}
		if( !m_fServer )
			SAFE_DELETE( m_pSock );
	}
}

BOOL CDPSock::Shutdown( DPID dpid )
{
	return ( m_pSock? m_pSock->Shutdown( dpid ): FALSE );
}

DWORD CDPSock::GetDebugInfo( DPID dpid )
{
	DWORD dwValue = 0;
	if( m_pSock )
	{
		if( dpid == DPID_UNKNOWN )
			dpid = m_pSock->GetID();
		dwValue = m_pSock->GetDebugInfo( dpid );
	}
	return dwValue;
}

void CDPSock::AddCreatePlayerOrGroupMsg( DPID dpid )
{
	DPMSG_CREATEPLAYERORGROUP createPlayer;
	
	ZeroMemory( (void*)&createPlayer, sizeof(DPMSG_CREATEPLAYERORGROUP) );
	createPlayer.dwType	= DPSYS_CREATEPLAYERORGROUP;	
	createPlayer.dwPlayerType	= DPPLAYERTYPE_PLAYER;	
	createPlayer.dpId	= dpid;	
	createPlayer.dpIdParent	= DPID_SERVERPLAYER;

	CBuffer* pBuffer = CBufferFactory::GetInstance().CreateBuffer( MAX_BUFFER );
	if( pBuffer != NULL && m_pProtocol->PushSystemStream( pBuffer, (LPBYTE)&createPlayer, sizeof( createPlayer ) ) == true )
	{
		pBuffer->SetDPID( DPID_SYSMSG );
		pBuffer->IncBlockCount();

		{
			CMclAutoLock	Lock( m_csRecvBuffer );
			m_lspRecvBuffer.AddTail( pBuffer );
		}

		SetEvent( m_hRecv );
	}
	else
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "push system stream failed. pBuffer: %p, dpid: %d" ), pBuffer, dpid );
	}
}

void CDPSock::AddDestroyPlayerOrGroupMsg( DPID dpid )
{
	DPMSG_DESTROYPLAYERORGROUP destroyPlayer;

	ZeroMemory( (void*)&destroyPlayer, sizeof(DPMSG_DESTROYPLAYERORGROUP) );
	destroyPlayer.dwType	= DPSYS_DESTROYPLAYERORGROUP;
	destroyPlayer.dwPlayerType	= DPPLAYERTYPE_PLAYER;	
	destroyPlayer.dpId	= dpid;		
	destroyPlayer.dpIdParent	= DPID_SERVERPLAYER;	
	destroyPlayer.dwFlags = GetDebugInfo( dpid );

	CBuffer* pBuffer = CBufferFactory::GetInstance().CreateBuffer( MAX_BUFFER );
	if( pBuffer != NULL && m_pProtocol->PushSystemStream( pBuffer, (LPBYTE)&destroyPlayer, sizeof( destroyPlayer ) ) == true )
	{
		pBuffer->SetDPID( DPID_SYSMSG );
		pBuffer->IncBlockCount();

		{
			CMclAutoLock	Lock( m_csRecvBuffer );
			m_lspRecvBuffer.AddTail( pBuffer );
		}

		SetEvent( m_hRecv );
	}
	else
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "push system stream failed. dpid: %d" ), dpid );
	}
}

unsigned int	__stdcall IoWorkerThread( LPVOID lpvThreadParameter )
{
	CDPSock* pDPSock	= (CDPSock*)lpvThreadParameter;
	SOCKET hSocket;
	DWORD dwBytes;
	CClientSock* pClientSock;
	LPOVERLAPPED lpov;
	BOOL fOk;
	long lIoWorker	= InterlockedIncrement( &pDPSock->m_lActiveIoWorker );

//#pragma	warning( push )
//#pragma warning( disable : 4127 )	// 조건식이 상수입니다.
	//while( 1 )
//#pragma	warning( pop )
	for( ;; )
	{
		fOk = ::GetQueuedCompletionStatus( pDPSock->GetCompletionPort( lIoWorker - 1 ), &dwBytes, (PULONG_PTR)&hSocket, &lpov, INFINITE );
		if( dwBytes == CLOSEIOWORKERMSG )
		{
			InterlockedDecrement( &pDPSock->m_lActiveIoWorker );
			return( 0 );
		}

		if( dwBytes == NEWSOCKETMSG )
		{
			DWORD dwAddr	= (DWORD)lpov;
			CServerSock* pServerSock	= (CServerSock*)pDPSock->GetServerSock();
			if( pServerSock )
				pServerSock->OnNewChild( hSocket, dwAddr );

			pClientSock		= pDPSock->Get( hSocket );
			if( pClientSock == NULL )
			{
				FLERROR_LOG( GetNetworkLogName(), _T( "NEWSOCKETMSG, ClientSock 찾기 실패 Socket: %d" ), hSocket );
				continue;
			}

			pDPSock->AddCreatePlayerOrGroupMsg( hSocket );

			pClientSock->m_AccessLock.Enter();	// lock
			pClientSock->m_nReference++;
			if( pClientSock->Recv() == false )
			{
				pClientSock->m_bDoNotWrite	= TRUE;
				int nReference	= --pClientSock->m_nReference;

				pClientSock->SetDebugInfo( WSAGetLastError() ); // * 1000 + 1 * 100 + nReference * 10;
				pClientSock->m_AccessLock.Leave();	// unlock
				if( nReference == 0 )
					pDPSock->CloseConnection( hSocket );
			}
			else
			{
				pClientSock->m_AccessLock.Leave();	// unlock
				if( pServerSock->SendProtocolSync( pClientSock ) == false )
				{
					FLERROR_LOG( GetNetworkLogName(), _T( "StartProtocol 실패 Socket: %d" ), pClientSock->GetHandle() );
				}
			}
			continue;
		}

		if( (pClientSock = pDPSock->Get( hSocket )) == NULL )
		{
			FLERROR_LOG( GetNetworkLogName(), _T( "pDPSock->Get() 실패 Socket: %d" ), hSocket );
			continue;
		}

		if( !fOk || dwBytes == 0 )	// When Connection Closed by peer gracefully or becoz of error
		{

			int err		=  GetLastError();

			switch( err )
			{
				case ERROR_NETNAME_DELETED:		//64
				case ERROR_OPERATION_ABORTED:
				case 0:
				case WSAENOTSOCK:	//10038
				default:
					{
						FLERROR_LOG( GetNetworkLogName(), _T( "OnError() Socket: %d, Error: %d, fOk: %d, dwBytes: %d" ), pClientSock->GetHandle(), err, fOk, dwBytes );

						pClientSock->m_AccessLock.Enter();
						pClientSock->m_bDoNotWrite	= TRUE;
						int nReference	= --pClientSock->m_nReference;
						pClientSock->SetDebugInfo( err );
						pClientSock->m_AccessLock.Leave();
						if( nReference == 0 )
							pDPSock->CloseConnection( hSocket );

						break;
					}
			}
			continue;
		}

		if( &pClientSock->m_ovRecv == lpov )	// receive i/o completed
		{
			CBufferQueue cQueue;
			if( pClientSock->Fetch( dwBytes, &cQueue ) == true )
			{
				if( cQueue.IsEmpty() == FALSE )
				{
					cQueue.SetDPID( hSocket );

					{
						CMclAutoLock	Lock( pDPSock->m_csRecvBuffer );
						pDPSock->m_lspRecvBuffer.AddTail( &cQueue );
					}

					SetEvent( pDPSock->GetRecvHandle() );
				}
			}
			else
			{
				pDPSock->CloseConnection( hSocket );
				FLERROR_LOG( GetNetworkLogName(), _T( "pClientSock->Fetch() 실패" ) );
				continue;
			}


			if( pClientSock->Recv() == false )	// i/o error
			{
				FLERROR_LOG( GetNetworkLogName(), _T( "pClientSock->Recv() Socket: %d, Error: %d" ), pClientSock->GetHandle(), WSAGetLastError() );

				pClientSock->m_AccessLock.Enter();
				pClientSock->m_bDoNotWrite	= TRUE;
				int nReference	= --pClientSock->m_nReference;
				pClientSock->SetDebugInfo( WSAGetLastError() );
				pClientSock->m_AccessLock.Leave();
				if( nReference == 0 )
					pDPSock->CloseConnection( hSocket );
			}
		}
		else if( &pClientSock->m_ovSend == lpov )	// send i/o completed
		{
			pClientSock->m_AccessLock.Enter();	// lock
			if( pClientSock->SendRemnant( dwBytes ) == false )
			{
				FLERROR_LOG( GetNetworkLogName(), _T( "pClientSock->SendRemnant() Socket: %d, Error: %d" ), pClientSock->GetHandle(), WSAGetLastError() );

				pClientSock->m_bDoNotWrite	= TRUE;
				int nReference	= --pClientSock->m_nReference;
				pClientSock->SetDebugInfo( WSAGetLastError() ); // * 1000 + 5 * 100 + nReference * 10;
				pClientSock->m_AccessLock.Leave();	// unlock
				if( nReference == 0 )
					pDPSock->CloseConnection( hSocket );
			}
			else
			{
				pClientSock->m_AccessLock.Leave();	// unlock
			}
		}
//		Sleep( 0 );
	}
	//return( 0 );
}

unsigned int	__stdcall	EventWorkerThread( LPVOID lpvThreadParameter )
{
	CDPSock* pDPSock	= (CDPSock*)lpvThreadParameter;
	CClientSockE* pClientSock	= (CClientSockE*)pDPSock->GetSockThreaded();
	SOCKET hSocket	= pClientSock->GetHandle();

	WSAEVENT aEvent[4];
	aEvent[0]	= pClientSock->GetRecvEvent();
	aEvent[1]	= pClientSock->GetSendEvent();
	aEvent[2]	= pDPSock->GetCloseHandle();
	aEvent[3]	= NULL;

	DWORD dwBytes	= 0, dw;
	BOOL fOk;

#pragma	warning( push )
#pragma warning( disable : 4127 )	// 조건식이 상수입니다.
	while( 1 )
#pragma	warning( pop )
	{
		dw	= WSAWaitForMultipleEvents( 3, aEvent, FALSE, INFINITE, FALSE );
		switch( dw )
		{
			case WSA_WAIT_EVENT_0+2:	// close event
				return 0;
			case WSA_WAIT_FAILED:
				FLERROR_LOG( GetNetworkLogName(), _T( "WSA_WAIT_FAILED with error %d\n" ), WSAGetLastError() );
				continue;
				break;
			case WSA_WAIT_EVENT_0:	// recv event
				{
					fOk		= GetOverlappedResult( (HANDLE)hSocket, &pClientSock->m_ovRecv, &dwBytes, TRUE );
					if( !fOk || dwBytes == 0 )	// When Connection Closed by peer gracefully or because of error
					{
						int err		= WSAGetLastError();
						switch( err )
						{
							case ERROR_NETNAME_DELETED:
							case ERROR_OPERATION_ABORTED:
							case 0:
							case WSAENOTSOCK:
							default:
								{
									FLERROR_LOG( GetNetworkLogName(), _T( "OnRecvError() Socket: %d, Error: %d" ), pClientSock->GetHandle(), err );

									pDPSock->CloseConnection( hSocket );
									return 0;
									break;
								}
						}
					}

					CBufferQueue cQueue;
					if( pClientSock->Fetch( dwBytes, &cQueue ) == true )
					{
						if( cQueue.IsEmpty() == FALSE )
						{
							cQueue.SetDPID( hSocket );

							{
								CMclAutoLock	Lock( pDPSock->m_csRecvBuffer );
								pDPSock->m_lspRecvBuffer.AddTail( &cQueue );
							}

							SetEvent( pDPSock->GetRecvHandle() );
						}
					}
					else
					{
						FLERROR_LOG( GetNetworkLogName(), _T( "pClientSock->Fetch() Socket: %d" ), pClientSock->GetHandle() );

						pDPSock->CloseConnection( hSocket );
						return 0;
					}

					if( pClientSock->Recv() == false )	// i/o error
					{
						FLERROR_LOG( GetNetworkLogName(), _T( "pClientSock->Recv() Socket: %d, Err: %d" ), pClientSock->GetHandle(), WSAGetLastError() );

						pDPSock->RemoveThread( pClientSock->m_hWorker );
						pDPSock->CloseConnection( hSocket );
						return 0;
					}
					break;
				}
			case WSA_WAIT_EVENT_0+1:	// send event
				{
					fOk		= GetOverlappedResult( (HANDLE)hSocket, &pClientSock->m_ovSend, &dwBytes, TRUE );
					WSAResetEvent( aEvent[1] );
					if( !fOk || dwBytes == 0 )	// When Connection Closed by peer gracefully or because of error
					{
						int err		= WSAGetLastError();
						switch( err )
						{
							case ERROR_NETNAME_DELETED:
							case ERROR_OPERATION_ABORTED:
							case 0:
							case WSAENOTSOCK:
							default:
								{
									FLERROR_LOG( GetNetworkLogName(), _T( "OnSendError() Socket: %d, Error: %d" ), pClientSock->GetHandle(), err );

									pDPSock->RemoveThread( pClientSock->m_hWorker );
									pDPSock->CloseConnection( hSocket );
									return 0;
									break;
								}
						}
					}
				
					if( pClientSock->SendRemnant( dwBytes ) == false )
					{
						pDPSock->RemoveThread( pClientSock->m_hWorker );
						pDPSock->CloseConnection( hSocket );
						return 0;
					}
					break;
				}
		}
	}
	return 0;
}
