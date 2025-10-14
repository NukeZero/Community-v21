#include "stdafx.h"
#include "dpmng.h"
#include <process.h>

#ifdef	__VERIFY_LOOP041010
	#undef INIT_LOOP
	#undef VERIFY_LOOP

	#define	INIT_LOOP					int __loop = 0;
	#define VERIFY_LOOP(file, line)		if( ++__loop == 1000 ) {	Error( "%s %d", file, line ); }
#else
	#undef INIT_LOOP
	#undef VERIFY_LOOP

	#define	INIT_LOOP					((void)0);
	#define VERIFY_LOOP(file, line)		((void)0);
#endif	


u_int	__stdcall ReceiveThread( LPVOID lpvThreadParameter );

CDPMng::CDPMng()
{
	m_pDPSock	= NULL;
	m_hRecvThread	= m_hClose	= (HANDLE)NULL;
}

CDPMng::~CDPMng()
{

}

BOOL CDPMng::CreateDPObject( BOOL bSingleThreaded )
{
	DeleteDPObject();
	if( !m_pDPSock )
		m_pDPSock	= new CDPSock;
	if( !bSingleThreaded )
	{
		// Create event used to signal that the receive thread should exit
		if( !m_hClose )
		{
			m_hClose	= CreateEvent( NULL, FALSE, FALSE, NULL );
			ASSERT( m_hClose );			
		}
		// Create thread to receive player messages
		if( !m_hRecvThread )
		{
			DWORD dwThreadId;
			m_hRecvThread	= chBEGINTHREADEX( NULL, 0, ReceiveThread, this, 0, &dwThreadId );
			ASSERT( m_hRecvThread );
			SetThreadPriority( m_hRecvThread, THREAD_PRIORITY_ABOVE_NORMAL );
		}
	}
	return TRUE;
}

BOOL CDPMng::DeleteDPObject( void )
{
	// Wake up receive thread and wait for it to quit
	CLOSE_THREAD( m_hRecvThread, m_hClose );
	if( m_pDPSock ) {
		m_pDPSock->Close();
		SAFE_DELETE( m_pDPSock );
	}
	return TRUE;
}

BOOL CDPMng::InitConnection( LPVOID lpConnection, u_short uPort )
{
	return	m_pDPSock->InitializeConnection( lpConnection, (DWORD)uPort );
}

#ifdef __CRC
BOOL CDPMng::CreateSession( LPCSTR lpSession, DWORD dwcrc )
{
	if( ( chWindows9x()? m_pDPSock->CreateServerE( dwcrc ): m_pDPSock->CreateServer( dwcrc ) ) )
#else	// __CRC
BOOL CDPMng::CreateSession( LPCSTR lpSession, BUFFER_TYPE nBufferType )
{
	if( ( chWindows9x()? m_pDPSock->CreateServerE( nBufferType ): m_pDPSock->CreateServer( nBufferType ) ) )
#endif	// __CRC
	{
		TRACE( "%s server launched.\n", lpSession );
		return TRUE;
	}
	TRACE( "%s server launching failed.\n", lpSession );
	return FALSE;
}

#ifdef __CRC
BOOL CDPMng::JoinSession( LPCSTR lpSession, DWORD dwcrc )
{
	if( ( chWindows9x()? m_pDPSock->JoinToServerE( dwcrc ): m_pDPSock->JoinToServer( dwcrc ) ) )
#else	// __CRC
BOOL CDPMng::JoinSession( LPCSTR lpSession, BUFFER_TYPE nBufferType )
{
	if( ( chWindows9x()? m_pDPSock->JoinToServerE( nBufferType ): m_pDPSock->JoinToServer( nBufferType ) ) )
#endif	// __CRC
	{
		TRACE( "%s server connected.\n", lpSession );
		return TRUE;
	}
	TRACE( "%s server connection failed.\n", lpSession );
	return FALSE;
}

void CDPMng::GetHostAddr( LPSTR lpAddr )
{
	if( m_pDPSock )
	{
		u_long uBufSize		= sizeof(lpAddr);
		m_pDPSock->GetHostAddr( (LPVOID)lpAddr, (LPDWORD)&uBufSize );
	}
}

void CDPMng::GetPlayerAddr( DPID dpid, LPSTR lpAddr )
{
	if( m_pDPSock )
	{
		u_long uBufSize		= sizeof(lpAddr);
		m_pDPSock->GetPlayerAddr( dpid, (LPVOID)lpAddr, (LPDWORD)&uBufSize );
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

BOOL CDPMng::ReceiveMessage( void )
{
	if( !m_pDPSock )
		return FALSE;

	CBuffer* pBuffer;
	LPBYTE ptr;
	u_long uDataSize;

	INIT_LOOP;
#ifdef __EH
	try
	{
#endif	// __EH
	while( 1 )	
	{
		VERIFY_LOOP( __FILE__, __LINE__ );
		pBuffer = m_pDPSock->Receive();     
		if( pBuffer == NULL )	
			break;

		ptr	= pBuffer->m_lpBufStart;
		int	__loop2 = 0;		// __VERIFY_LOOP041010
		while( pBuffer->cb-- > 0 )
		{
#ifdef __VERIFY_LOOP041010
			if( __loop2++ == 1000 )	{ Error( "CDPMng::ReceiveMessage %d", __loop2 ); }
#endif // __VERIFY_LOOP041010

#ifdef __CRC
			uDataSize	= *(UNALIGNED LPDWORD)( ptr + m_pDPSock->m_dwDataSizeOffset );
			if( pBuffer->dpid ==DPID_SYSMSG )
				SysMessageHandler( (LPDPMSG_GENERIC)&ptr[m_pDPSock->m_dwReadHeaderSize], uDataSize, pBuffer->dpid );
			else
				UserMessageHandler( (LPDPMSG_GENERIC)&ptr[m_pDPSock->m_dwReadHeaderSize], uDataSize, pBuffer->dpid );
			ptr	+=	m_pDPSock->m_dwReadHeaderSize + uDataSize;
#else	// __CRC			
			uDataSize = pBuffer->GetPacketSize( ptr );
			if( pBuffer->dpid == DPID_SYSMSG )
				SysMessageHandler( (LPDPMSG_GENERIC)( ptr + pBuffer->GetHeaderLength() ), uDataSize, pBuffer->dpid );
			else
				UserMessageHandler( (LPDPMSG_GENERIC)( ptr + pBuffer->GetHeaderLength() ), uDataSize, pBuffer->dpid );
			ptr += ( uDataSize + pBuffer->GetHeaderLength() );
#endif
		}
		SAFE_DELETE( pBuffer );
	}
#ifdef __EH
	}
	catch( ... )
	{
		WriteError( "%s %d", __FILE__, __LINE__ );
	}
#endif	// __EH

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

#ifdef __INFO_SOCKLIB0516
DWORD CDPMng::GetDebugInfo( DPID dpid )
{
	if( m_pDPSock )
		return m_pDPSock->GetDebugInfo( dpid );
	return 0;
}
#endif	// __INFO_SOCKLIB0516

u_int __stdcall ReceiveThread( LPVOID lpvThreadParameter )
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
	LOAD_WS2_32_DLL;
	return TRUE;
}

void UninitializeNetLib()
{
	SAFE_DELETE( CAr::m_pHeapMng );
	SAFE_DELETE( CClientSock::m_pPool );
	SAFE_DELETE( CClientSockE::m_pPool );
	SAFE_DELETE( CBuffer::m_pPool );
	SAFE_DELETE( CBuffer2::m_pPool2 );	
	SAFE_DELETE( CBuffer::m_pHeapMng );

	UNLOAD_WS2_32_DLL;
}
