// ProcessLauncher.cpp: implementation of the CProcessLauncher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProcessLauncher.h"
#include "Peer.h"

const UINT ABORT_CODE = 100;

VOID CALLBACK APCProc( ULONG_PTR dwParam )
{
	// nothing
}

void __cdecl  _MonitorThread( void * pParam )
{ 
	CProcessLauncher* p	= (CProcessLauncher*)pParam;
	p->MonitorThread();
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcessLauncher::CProcessLauncher()
{
	m_lpAddr[0]	= '\0';
	m_lpApplicationName[0]	= '\0';
	m_lpCurrentDirectory[0]	= '\0';
	m_dwId					= 0;
	m_dwMilliseconds		= INFINITE;
	m_bStartWait			= false;
	m_hEvent				= NULL;
	m_hMonitor				= NULL;
	m_lProcessCount			= 0;

	m_pPeer					= NULL;	
	memset( &m_pi, 0x00, sizeof(m_pi) );
}

CProcessLauncher::~CProcessLauncher()
{
	if( m_hMonitor )
	{
		::QueueUserAPC( APCProc, m_hMonitor, 0 );
		WaitForSingleObject( m_hMonitor, INFINITE );
	}

	if( m_hEvent )
		CloseHandle( m_hEvent );
}

void CProcessLauncher::BeginMonitor()
{
	m_hMonitor = (HANDLE) _beginthread( _MonitorThread, 0, this );
}

BOOL CProcessLauncher::Init()
{
	bool bStartWait, bHeartbeat;
	bStartWait = bHeartbeat = false;

	CScanner s;
	if( s.Load( "launcher.ini" ) == FALSE)
		return FALSE;

	s.GetToken();
	while( s.tok != FINISHED )
	{
		if( s.Token == "lpAddr" )
		{
			s.GetToken();	// =
			s.GetToken();
			strcpy( m_lpAddr, s.Token );
		}
		else if( s.Token == "lpApplicationName" )
		{
			s.GetToken();	// =
			s.GetToken();
			strcpy( m_lpApplicationName, s.Token );
			SetConsoleTitle( m_lpApplicationName );
		}
		else if( s.Token == "lpCurrentDirectory" )
		{
			s.GetToken();	// =
			s.GetToken();
			strcpy( m_lpCurrentDirectory, s.Token );
		}
		else if( s.Token == "dwId" )
		{
			s.GetToken();	// =
			m_dwId	= (DWORD)s.GetNumber();
		}
		else if( s.Token == "DeadlockCheck" )
		{
			s.GetToken();	// =
			m_dwMilliseconds	= (DWORD)s.GetNumber();
			bHeartbeat = true;
		}
		else if( s.Token == "bWait" )
		{
			s.GetToken();	// =
			bStartWait = (bool)(s.GetNumber() != 0);
		}
		else if( s.Token == "lpDownDirectory" )
		{
			s.GetToken();	// =
			s.GetToken();
			//m_uploader.SetDownDirectory( s.Token );
			strcpy( m_lpDownDirectory, s.Token );			
		}
		s.GetToken();
	}

	if( bStartWait && bHeartbeat )
	{
		AfxMessageBox( "INVALID CONDITION - bWait AND bHeartbeat must be exclusive" );
		return FALSE;
	}
	m_bStartWait = bStartWait;
	
	if( bStartWait || bHeartbeat )
	{
		char lpName[32];
		sprintf( lpName, "%d", GetAppID() );

		m_hEvent = CreateEvent( NULL, FALSE, FALSE, lpName ) ;
		if( m_hEvent == FALSE )
		{
			char szBuffer[128];
			sprintf( szBuffer, "CreateEvent ERROR:%d", GetLastError() );
			AfxMessageBox( szBuffer );
			return FALSE;
		}
	}

	return TRUE;
}

void CProcessLauncher::MonitorThread()
{
	DWORD nCount = 1;
	HANDLE lpHandles[2] = {m_pi.hProcess, NULL};

	if( m_dwMilliseconds != INFINITE )	
	{
		// 데드락을 체크한다.
		lpHandles[1]	= m_hEvent;
		nCount = 2;
	}

	while( 1 )
	{
		DWORD dw = WaitForMultipleObjectsEx( nCount, lpHandles, FALSE, m_dwMilliseconds, TRUE );
		switch( dw )
		{
			case WAIT_IO_COMPLETION:
				puts("WAIT_IO_COMPLETION MonitorThread end.");
				return;

			case WAIT_OBJECT_0:			// 프로세스가 끝나는 경우 쓰레드종료 				
				OnTerminated();
				puts("WAIT_OBJECT_0 MonitorThread end.");
				return;

			case (WAIT_OBJECT_0 + 1):	// 하트비트의 경우 
				TRACE( "( WAIT_OBJECT_0 + 1 )\n" );
				break;

			case WAIT_TIMEOUT:			// 타임오버 - 데드락으로 인식  
				CTime time	= CTime::GetCurrentTime();
				printf( "%s\tdeadlock\n", time.Format( "%Y/%m/%d %H:%M:%S" ) );			

				TerminateProcess( m_pi.hProcess, 0 );	
				break;
		}
	}
}

bool CProcessLauncher::CreateMyProcess()
{
	if( m_lProcessCount > 0 )
	{
		puts("Process is running." );
		return false;
	}

	bool bOK = false;
	if( m_pi.hProcess == NULL )
	{
		if( ::RunProcess( &m_pi, (char *)GetAppName(), GetAppDirectory(), false, NORMAL_PRIORITY_CLASS ) )
		{
			printf( "Process[%s - %d] created.\n", GetAppName(), m_pi.hProcess );

			DWORD dwResult = WAIT_OBJECT_0;
			if( GetStartWait() )
				dwResult = WaitForSingleObject( GetEvent(), 1000 * 120 );	// 실행 후의 데이타 로딩을 최대 2분 기다린다.
			else
				WaitForInputIdle( m_pi.hProcess, 3000 );		

			bOK = ( dwResult == WAIT_OBJECT_0 );

			printf( "Process[%s - %d] wait result:%d\n", GetAppName(), m_pi.hProcess, dwResult );
		}
	}

	if( bOK )
	{
		InterlockedIncrement( &m_lProcessCount );
		BeginMonitor();
	}
	
	if( m_pPeer )
	{
		BEFORESEND( ar, PACKETTYPE_PROCESS_CREATED2 );
		ar << (BYTE)bOK;
		ar << (DWORD)( bOK ? 0 : GetLastError() );
		SEND( ar, m_pPeer, DPID_SERVERPLAYER );
	}

	return bOK;
}

void CProcessLauncher::TerminateMyProcess()
{
	if( m_lProcessCount == 0 )
	{
		puts("Process is not running.");
		return;
	}

	assert( m_pi.hProcess != NULL );

	printf("TerminateProcess( %d, %u )\n", m_pi.hProcess, ABORT_CODE );
	TerminateProcess( m_pi.hProcess, ABORT_CODE );	

	do 
	{
		Sleep( 1000 );
	} 
	while( m_lProcessCount > 0 );

	puts("TerminateMyProcess() end.");
}

void CProcessLauncher::OnTerminated()
{
	DWORD dwExitCode = 0;
	GetExitCodeProcess( m_pi.hProcess, &dwExitCode );
	CloseHandle( m_pi.hProcess );
	CloseHandle( m_pi.hThread );
	memset( &m_pi, 0x00, sizeof(m_pi) );
	InterlockedDecrement( &m_lProcessCount );

	printf( "GetExitCodeProcess: %d \n", dwExitCode );

	if( m_pPeer )
	{
		BYTE cbAbort = (dwExitCode == ABORT_CODE ? 1 : 0);

		BEFORESEND( ar, PACKETTYPE_PROCESS_TERMINATED2 );
		ar << cbAbort;
		SEND( ar, m_pPeer, DPID_SERVERPLAYER );
	}

//	Sleep( 3000 );
}

BYTE CProcessLauncher::IsProcessRunning()
{
	if( m_lProcessCount > 0 )
		return 1;
	else
		return 0;
}

void CProcessLauncher::Run()
{
	m_pPeer = new CPeer;
	m_pPeer->Init( m_lpDownDirectory, this );

	m_pPeer->Run();
	m_pPeer->DeleteDPObject();
	safe_delete( m_pPeer );
	m_pPeer = NULL;
}

