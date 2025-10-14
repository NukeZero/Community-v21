#include "stdafx.h"
#include "peer.h"

#include <dump/FLDumpRunner.h>
#include <common/FLFileUtil.h>

DWORD		g_dwCurrentFullDumpCount		= 0;
DWORD		g_dwFullDumpCount				= 3;
BOOL		g_bExternDumpEnable				= FALSE;
TCHAR		ADPLUS_FILE_NAME[MAX_PATH]		= _T( "./adplus/adplus.exe" );
TCHAR		PROCDUMP_FILE_NAME[MAX_PATH]	= _T( "./procdump/procdump.exe" );

SYSTEMTIME	g_tLastPingTime;

class	FLExternDumper : public FLDumper
{
public:

	static FLExternDumper*	GetInstance()
	{
		static FLExternDumper kExternDumper;
		return &kExternDumper;
	}

public:

	FLExternDumper()
	{
	}

	void	SetPrefix( const TCHAR* szPrefix )
	{
		m_kGenericDumper.SetPrefix( szPrefix );
	}

private:

	bool	DoDump( T_DUMP_PARAM* pParam )
	{
		if( g_bExternDumpEnable != FALSE )
		{
			DumpbyProcDump( pParam );
			DumpbyAdPlus( pParam );
		}

		return m_kGenericDumper.Dump( pParam );
	}

	void	DumpbyProcDump( T_DUMP_PARAM* pParam )
	{
		if( FLStrlen( PROCDUMP_FILE_NAME, _countof( PROCDUMP_FILE_NAME ) ) > 0 )
		{
			TCHAR szFileName[MAX_PATH] = { 0, };
			FLGetProcessFileName( pParam->hProcess, szFileName, _countof( szFileName ) );

			TCHAR szDumpFile[MAX_PATH] = { 0, };
			FLMakeFileNameByProcess( szDumpFile, _countof( szDumpFile ), pParam->hProcess, NULL, _T( "ProcDump_" ), _T( "dmp" ) );

			TCHAR szCommandLine[512] = { 0, };
			FLSPrintf( szCommandLine, _countof( szCommandLine ), _T( "\"%s\" %s %s" ), PROCDUMP_FILE_NAME, szFileName, szDumpFile );

			TCHAR szRunFolder[MAX_PATH] = { 0, };
			FLSplitPath( PROCDUMP_FILE_NAME, szRunFolder, _countof( szRunFolder ) );

			RunProcessAndWaitTerminate( szCommandLine, szRunFolder );
		}
	}

	void	DumpbyAdPlus( T_DUMP_PARAM* pParam )
	{
		if( FLStrlen( ADPLUS_FILE_NAME, _countof( ADPLUS_FILE_NAME ) ) > 0 )
		{
			TCHAR szDumpFolder[MAX_PATH] = { 0, };
			FLGetProcessFolder( pParam->hProcess, szDumpFolder, _countof( szDumpFolder ) );

			TCHAR szCommandLine[512] = { 0, };
			FLSPrintf( szCommandLine, _countof( szCommandLine ), _T( "\"%s\" -Hang -p %u -o %s" ), ADPLUS_FILE_NAME, pParam->dwProcessID, szDumpFolder );

			TCHAR szRunFolder[MAX_PATH] = { 0, };
			FLSplitPath( ADPLUS_FILE_NAME, szRunFolder, _countof( szRunFolder ) );

			RunProcessAndWaitTerminate( szCommandLine, szRunFolder );
		}
	}

	void	RunProcessAndWaitTerminate( TCHAR* szCommandLine, const TCHAR* szCurrentFolder )
	{
		STARTUPINFO tStartupInfo;
		::memset( &tStartupInfo, 0, sizeof( tStartupInfo ) );
		tStartupInfo.cb	= sizeof( tStartupInfo );

		PROCESS_INFORMATION tProcess;
		::memset( &tProcess, 0, sizeof( tProcess ) );

		if( ::CreateProcess( NULL, szCommandLine, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE, NULL, szCurrentFolder, &tStartupInfo, &tProcess ) != FALSE )
		{
			::WaitForSingleObject( tProcess.hProcess, INFINITE );

			::CloseHandle( tProcess.hProcess );
			::CloseHandle( tProcess.hThread );

			::memset( &tProcess, 0, sizeof( tProcess ) );
		}
	}

private:

	FLGenericDumper	m_kGenericDumper;


};


CPeer::CPeer()
{
	*m_lpAddr	= 0;
	::memset( &m_tProcess, 0, sizeof( m_tProcess ) );
	*m_lpApplicationName	= '\0';
	*m_lpCurrentDirectory	= '\0';
	m_dwId	= 0;
	m_dwMilliseconds	= INFINITE;

	m_dwStartServerWaitTimeout = (20*60*1000);	// 20분

	m_hWorkerThread		= (HANDLE)0;
	m_hCloseWorker	= (HANDLE)0;

	m_hReadInheritable	= (HANDLE)0;

	m_hHeartbeat = NULL;

	m_hWait		= CreateEvent( NULL, FALSE, FALSE, NULL );
	m_bWait		= FALSE;

	ON_MSG( PACKETTYPE_CREATE_PROCESS, OnCreateProcess );
	ON_MSG( PACKETTYPE_TERMINATE_PROCESS, OnTerminateProcess );
	ON_MSG( PACKETTYPE_DEBUG_EXTERN_DUMP_OPTION_NOTI, OnExternDumpOptionNoti );
	ON_MSG( PACKETTYPE_DEBUG_DUMP_PROCESS_NOTI, OnDumpProcessNoti )
}

CPeer::~CPeer()
{
	if( m_hHeartbeat != NULL )
	{
		CloseHandle( m_hHeartbeat );
		m_hHeartbeat = NULL;
	}

	CloseHandle( m_hWait );
}

void CPeer::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			break;
		case DPSYS_DESTROYPLAYERORGROUP:
			break;
	}
}

void CPeer::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );
	GETTYPE( ar );
	PACKET_HANDLER_FUNC pfn	=	GetHandler( dw );
	
	if( pfn ) {
		( this->*( pfn ) )( ar );
	}
	else {
//		FLASSERT( 0 );
	}
}

BOOL CPeer::InitializeMyProcess( void )
{
	CScanner s;
	if( s.Load( "launcher.ini" ) )
	{
		s.GetToken();
		while( s.tok != FINISHED )
		{
			if( s.Token == "lpAddr" )
			{
				s.GetToken();	// =
				s.GetToken();
				FLStrcpy( m_lpAddr, _countof( m_lpAddr ), s.Token );
			}
			else if( s.Token == "lpApplicationName" )
			{
				s.GetToken();	// =
				s.GetToken();
				FLStrcpy( m_lpApplicationName, _countof( m_lpApplicationName ), s.Token );
			}
			else if( s.Token == "lpCurrentDirectory" )
			{
				s.GetToken();	// =
				s.GetToken();
				FLStrcpy( m_lpCurrentDirectory, _countof( m_lpCurrentDirectory ), s.Token );
			}
			else if( s.Token == "dwId" )
			{
				s.GetToken();	// =
				m_dwId	= (DWORD)s.GetNumber();
			}
			else if( s.Token == "dwMiliseconds" )
			{
				s.GetToken();	// =
				m_dwMilliseconds	= (DWORD)s.GetNumber();
			}
			else if( s.Token == "bWait" )
			{
				s.GetToken();	// =
				m_bWait	= (BOOL)s.GetNumber();
			}
			else if( s.Token == "StartServerWaitTime" )
			{
				s.GetToken();	// =
				m_dwStartServerWaitTimeout = (DWORD)s.GetNumber();
			}
			else if( s.Token == "FullDumpCount" )
			{
				s.GetToken();	// =
				g_dwFullDumpCount = s.GetNumber();
			}
			else if( s.Token == "ExternDump" )
			{
				s.GetToken();	// =
				g_bExternDumpEnable = (s.GetNumber() == 0) ? FALSE : TRUE;
			}
			else if( s.Token == "ADPlus" )
			{
				s.GetToken();	// =
				s.GetToken();
				FLStrcpy( ADPLUS_FILE_NAME, _countof( ADPLUS_FILE_NAME ), s.Token );
			}
			else if( s.Token == "ProcDump" )
			{
				s.GetToken();	// =
				s.GetToken();
				FLStrcpy( PROCDUMP_FILE_NAME, _countof( PROCDUMP_FILE_NAME ), s.Token );
			}

			s.GetToken();
		}
		return TRUE;
	}
	return FALSE;
}

void CPeer::CreateMyProcess( void )
{
	FLASSERT( *m_lpApplicationName != '\0' );

//	KillProcess( m_lpApplicationName );
	TerminateMyProcess();

//	HANDLE hWriteUninheritable;
	HANDLE hWriteInheritable;
	HANDLE hSaveStdOut;
	
//	char lpCurrentDirectory[MAX_PATH], lpCommandLine[MAX_PATH];
//	GetCurrentDirectory( MAX_PATH, lpCurrentDirectory );
//	sprintf( lpCommandLine, "%s\\%s", lpCurrentDirectory, m_lpApplicationName );

	SECURITY_ATTRIBUTES	sa;
	sa.nLength	= sizeof(sa);
	sa.lpSecurityDescriptor	= NULL;
	sa.bInheritHandle	= TRUE;
	if( !CreatePipe( &m_hReadInheritable, &hWriteInheritable, &sa, 0 ) )
		AfxMessageBox( "Unable to create pipe." );
//	if( !DuplicateHandle( GetCurrentProcess(), hWriteInheritable, GetCurrentProcess(), &hWriteUninheritable, 0, TRUE, DUPLICATE_SAME_ACCESS ) )
//		AfxMessageBox( "Unable to duplicate handle." );
//	CloseHandle( hWriteInheritable );
	hSaveStdOut		= GetStdHandle( STD_OUTPUT_HANDLE );
	SetStdHandle( STD_OUTPUT_HANDLE, hWriteInheritable );

	STARTUPINFO si;
	memset( &si, 0, sizeof(STARTUPINFO) );
	si.cb	= sizeof(STARTUPINFO);

	if( FALSE == ::CreateProcess( NULL, m_lpApplicationName, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, m_lpCurrentDirectory, &si, &m_tProcess ) )	{
		FLASSERT( 0 );
	}

	SetStdHandle( STD_OUTPUT_HANDLE, hSaveStdOut );
	CloseHandle(  hWriteInheritable );

	CreateWorker();

	if( m_bWait )
		WaitForSingleObject( m_hWait, INFINITE );

	SendHdr( PACKETTYPE_PROCESS_CREATED );
}

void CPeer::CreateWorker( void )
{
	CloseWorker();

	DWORD dwThreadId;
	m_hCloseWorker	= CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hWorkerThread		= chBEGINTHREADEX( NULL, 0, _WorkerThread, this,  0, &dwThreadId );
	FLASSERT( m_hWorkerThread );

	HANDLE hThread	= (HANDLE)_beginthread( _pipe_read, 0, this );
	FLASSERT( hThread );
}

void CPeer::CloseWorker( void )
{
	CLOSE_THREAD( m_hWorkerThread, m_hCloseWorker );
}

void CPeer::TerminateMyProcess( void )
{
	CloseWorker();

	if( m_tProcess.hProcess )
	{
		DWORD dwExitCode;
		GetExitCodeProcess( m_tProcess.hProcess, &dwExitCode );
		TerminateProcess( m_tProcess.hProcess, dwExitCode );
		WaitForSingleObject( m_tProcess.hProcess, INFINITE );
		CloseHandle( m_tProcess.hProcess );
		CloseHandle( m_tProcess.hThread );
		::memset( &m_tProcess, 0, sizeof( m_tProcess ) );
	}
}

void	CPeer::DumpMyProcess( int nLevel )
{
	DWORD dwProcessID = m_tProcess.dwProcessId;
	if( dwProcessID != 0 )
	{
		if( nLevel == FLDUMP_LEVEL_LIGHT )
		{
			FLExternDumper::GetInstance()->SetPrefix( _T( "Hang_" ) );
			FLDumpRunner::DumpProcess( FLExternDumper::GetInstance(), FLDUMP_LEVEL_LIGHT, dwProcessID );
		}
		else if( nLevel == FLDUMP_LEVEL_MEDIUM )
		{
			FLExternDumper::GetInstance()->SetPrefix( _T( "Hang_" ) );
			FLDumpRunner::DumpProcess( FLExternDumper::GetInstance(), FLDUMP_LEVEL_MEDIUM, dwProcessID );
		}
		else if( nLevel == FLDUMP_LEVEL_HEAVY )
		{
			FLExternDumper::GetInstance()->SetPrefix( _T( "Hang_Full_" ) );
			FLDumpRunner::DumpProcess( FLExternDumper::GetInstance(), FLDUMP_LEVEL_HEAVY, dwProcessID );
		}
	}
}

UINT CPeer::_WorkerThread( LPVOID pParam )
{
	CPeer* pPeer	= (CPeer*)pParam;
//	FLASSERT( pPeer );
	return pPeer->WorkerThread();
}

BOOL	CPeer::WaitForHeartbeat( DWORD dwTimeout )
{
	//////////////////////////////////////////////////////////////////////////
	TCHAR szEventName[32] = _T( "" );
	FLSPrintf( szEventName, _countof( szEventName ), _T( "%d" ), m_dwId );

	HANDLE hHeartbeatWait[2] = { NULL, };
	hHeartbeatWait[0]	= m_hCloseWorker;
	hHeartbeatWait[1]	= m_tProcess.hProcess;

	DWORD dwWaitStartTick = ::GetTickCount();

	while( true )
	{
		DWORD dwResult = WaitForMultipleObjects( 2, hHeartbeatWait, FALSE, 500 );
		switch( dwResult )
		{
		case WAIT_OBJECT_0:
			{
				return FALSE;
			}
		case WAIT_OBJECT_0 + 1:
			{
				// ...
				CloseHandle( m_tProcess.hProcess );
				CloseHandle( m_tProcess.hThread );
				CloseHandle( m_hCloseWorker );
				CloseHandle( m_hWorkerThread );

				::memset( &m_tProcess, 0, sizeof( m_tProcess ) );
				m_hCloseWorker	= NULL;
				m_hWorkerThread		= NULL;

				SendHdr( PACKETTYPE_PROCESS_TERMINATED );
				Sleep( 3000 );
				return FALSE;
			}
		}

		m_hHeartbeat = OpenEvent( EVENT_ALL_ACCESS, FALSE, szEventName );
		if( m_hHeartbeat != NULL )
		{
			return TRUE;
		}

		if( (::GetTickCount() - dwWaitStartTick) >= dwTimeout )
		{
			break;
		}
	}

	if( m_hHeartbeat == NULL )
	{
		m_hHeartbeat = CreateEvent( NULL, FALSE, FALSE, szEventName );
		if( m_hHeartbeat == NULL )
		{
			m_hHeartbeat = OpenEvent( EVENT_ALL_ACCESS, FALSE, szEventName );
		}
	}

	if( m_hHeartbeat == NULL )
	{
		return FALSE;
	}

	return TRUE;
	//////////////////////////////////////////////////////////////////////////
}

void	CPeer::HangDumpScenario( DWORD dwProcessID )
{
	if( g_dwCurrentFullDumpCount < g_dwFullDumpCount )
	{
		++g_dwCurrentFullDumpCount;

		//////////////////////////////////////////////////////////////////////////
		FLExternDumper::GetInstance()->SetPrefix( _T( "Hang_" ) );

		// 1초 간격으로 3번 덤프를 남긴다.
		for( int i = 0; i < 3; ++i )
		{
			FLDumpRunner::DumpProcess( FLExternDumper::GetInstance(), FLDUMP_LEVEL_MEDIUM, dwProcessID );
			::Sleep( 1000 );
		}

		// 풀 덤프를 남긴다.
		FLExternDumper::GetInstance()->SetPrefix( _T( "Hang_Full_" ) );
		FLDumpRunner::DumpProcess( FLExternDumper::GetInstance(), FLDUMP_LEVEL_HEAVY, dwProcessID );
		//////////////////////////////////////////////////////////////////////////
	}
}

BOOL CPeer::WorkerThread( void )
{
	::memset( &g_tLastPingTime, 0, sizeof( g_tLastPingTime ) );

	if( WaitForHeartbeat( m_dwStartServerWaitTimeout ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Heartbeat wait failed." ) );
		return FALSE;
	}

	DWORD dw;
	HANDLE lpHandles[3];

	lpHandles[0]	= m_hCloseWorker;
	lpHandles[1]	= m_tProcess.hProcess;
	lpHandles[2]	= m_hHeartbeat;

	while( 1 )
	{
		dw	= WaitForMultipleObjects( 3, lpHandles, FALSE, m_dwMilliseconds );
		switch( dw )
		{
			case ( WAIT_OBJECT_0 ):
				{
					CloseHandle( m_hHeartbeat );
					m_hHeartbeat = NULL;
					return( 0 );
				}
			case ( WAIT_OBJECT_0 + 1 ):
				{
					FLINFO_LOG( PROGRAM_NAME, _T( "%s Terminated. Last Ping Time: %04d-%02d-%02d %02d:%02d:%02d" ),
						m_lpApplicationName,
						g_tLastPingTime.wYear, g_tLastPingTime.wMonth, g_tLastPingTime.wDay,
						g_tLastPingTime.wHour, g_tLastPingTime.wMinute, g_tLastPingTime.wSecond );

					CloseHandle( m_tProcess.hProcess );
					CloseHandle( m_tProcess.hThread );
					CloseHandle( m_hCloseWorker );
					CloseHandle( m_hWorkerThread );
					CloseHandle( m_hHeartbeat );

					::memset( &m_tProcess, 0, sizeof( m_tProcess ) );
					m_hCloseWorker	= NULL;
					m_hWorkerThread		= NULL;
					m_hHeartbeat = NULL;

					SendHdr( PACKETTYPE_PROCESS_TERMINATED );
					Sleep( 3000 );
					return( 0 );
				}
			case ( WAIT_TIMEOUT ):
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "%s Heartbeat timeout Last Ping Time: %04d-%02d-%02d %02d:%02d:%02d" ),
						m_lpApplicationName,
						g_tLastPingTime.wYear, g_tLastPingTime.wMonth, g_tLastPingTime.wDay,
						g_tLastPingTime.wHour, g_tLastPingTime.wMinute, g_tLastPingTime.wSecond );

					CTime time	= CTime::GetCurrentTime();
					printf( "%s\tdeadlock\n", time.Format( "%Y/%m/%d %H:%M:%S" ) );

					//////////////////////////////////////////////////////////////////////////
					HangDumpScenario( m_tProcess.dwProcessId );
					//////////////////////////////////////////////////////////////////////////

					DWORD dwExitCode;
					GetExitCodeProcess( m_tProcess.hProcess, &dwExitCode );
					if( TerminateProcess( m_tProcess.hProcess, dwExitCode ) == FALSE )
					{
						DWORD dwLastError = ::GetLastError();
						FLERROR_LOG( PROGRAM_NAME, _T( "TerminateProcess Failed: ProcessID: %u, Handle: %u, Error: %u" ),
							m_tProcess.dwProcessId, m_tProcess.hProcess, dwLastError );
					}

					break;
				}
			case ( WAIT_OBJECT_0 + 2 ):
				{
					FLTRACE_LOG( PROGRAM_NAME, _T( "Heartbeat received" ) );
					::GetLocalTime( &g_tLastPingTime );
					//FLTRACE_LOG( PROGRAM_NAME, _T( "( WAIT_OBJECT_0 + 2 )" ) );
					break;
				}
		}
	}
	return( 0 );
}

void CPeer::SendMyProcess( void )
{
	BEFORESEND( ar, PACKETTYPE_MY_PROCESS );
	ar << m_dwId;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CPeer::OnCreateProcess( CAr & ar )
{
	CreateMyProcess();
}

void CPeer::OnTerminateProcess( CAr & ar )
{
	TerminateMyProcess();
}

void	CPeer::OnExternDumpOptionNoti( CAr& ar )
{
	BOOL bEnable = FALSE;
	ar >> bEnable;

	g_bExternDumpEnable = (bEnable == 0) ? FALSE : TRUE;
}

void	CPeer::OnDumpProcessNoti( CAr& ar )
{
	int nLevel = FLDUMP_LEVEL_MEDIUM;
	ar >> nLevel;

	DumpMyProcess( nLevel );
}

void _cdecl CPeer::_pipe_read( LPVOID pParam )
{
	CPeer* pPeer	= (CPeer*)pParam;
	pPeer->pipe_read();
}

void CPeer::pipe_read( void )
{
	BOOL bStatus;
	do {
		DWORD dwData	= 0;
		DWORD dwBytesRead;
		bStatus		= ReadFile( m_hReadInheritable, &dwData, sizeof(dwData), &dwBytesRead, NULL );
//		FLTRACE_LOG( PROGRAM_NAME, _T( "Child successfully read, %ld" ), dwData );
		switch( dwData )
		{
			case 0x00000001:	// is started
				{
					SetEvent( m_hWait );
					break;
				}
		}
	} while( bStatus );

	CloseHandle( m_hReadInheritable );
	m_hReadInheritable	= (HANDLE)0;

	return;
}
