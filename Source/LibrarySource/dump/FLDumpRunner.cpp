
#include "FLDumpRunner.h"

#include "../common/FLStringFunction.h"
#include "../common/FLEnumThread.h"
#include "../common/FLEnumProcess.h"

#include <process.h>


namespace
{

	bool	FLCaptureContext( PCONTEXT pContext )
	{
		if( pContext != NULL )
		{
			typedef	VOID	(WINAPI * PRTL_CAPTURE_CONTEXT_F)( PCONTEXT );
			PRTL_CAPTURE_CONTEXT_F pRtlCaptureContext
				= reinterpret_cast< PRTL_CAPTURE_CONTEXT_F >( ::GetProcAddress( ::GetModuleHandle( _T( "kernel32.dll" ) ), "RtlCaptureContext" ) );
			if( pRtlCaptureContext != NULL )
			{
				pRtlCaptureContext( pContext );
				return true;
			}
		}

		return false;

	}

}

bool	FLDumpRunner::DumpNow( FLDumper* pDumper,
							int nLevel,
							DWORD dwExceptionCode,
							const TCHAR* szExpression,
							const TCHAR* szFunction,
							const TCHAR* szFile,
							unsigned int nLine )
{
	EXCEPTION_RECORD tExceptionRecord = {};
	CONTEXT tContext = {};
	EXCEPTION_POINTERS tExceptionPtrs = { &tExceptionRecord, &tContext };

	tContext.ContextFlags = CONTEXT_FULL;
	if( FLCaptureContext( &tContext ) == true )
	{
		tExceptionRecord.ExceptionCode = dwExceptionCode;
		tExceptionRecord.NumberParameters = 0;

		return FLDumpRunner::Execute( pDumper, nLevel, ::GetCurrentProcess(), ::GetCurrentProcessId(), ::GetCurrentThread(), ::GetCurrentThreadId(), &tExceptionPtrs, szExpression, szFunction, szFile, nLine );
	}

	return false;
}

bool	FLDumpRunner::DumpProcess( FLDumper* pDumper, int nLevel, const TCHAR* szProcessName )
{
	bool bResult = false;

	FLEnumProcess kEnumProcess;
	if( kEnumProcess.Open() == true )
	{
		DWORD dwProcessID = 0;
		const TCHAR* szEnumProcess = kEnumProcess.GetFirst();
		while( szEnumProcess != NULL )
		{
			if( ::_tcsicmp( szProcessName, szEnumProcess ) == 0 )
			{
				dwProcessID = kEnumProcess.GetProcessID();
				break;
			}

			szEnumProcess = kEnumProcess.GetNext();
		}

		kEnumProcess.Close();

		if( dwProcessID != 0 )
		{
			bResult = DumpProcess( pDumper, nLevel, dwProcessID );
		}
	}

	return bResult;
}

bool	FLDumpRunner::DumpProcess( FLDumper* pDumper, int nLevel, DWORD dwProcessID )
{
	bool bResult = false;

	HANDLE hProcess = ::OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessID );
	if( hProcess != NULL )
	{
		FLEnumThread kEnumThread;
		if( kEnumThread.Open() == true )
		{
			DWORD dwThreadID = kEnumThread.GetFirst( dwProcessID );
			kEnumThread.Close();

			if( dwThreadID != 0 )
			{
				HANDLE hThread = ::OpenThread( THREAD_ALL_ACCESS, FALSE, dwThreadID );
				if( hThread != NULL )
				{
					bResult = DumpProcess( pDumper, nLevel, EXCEPTION_NONCONTINUABLE_EXCEPTION, hProcess, dwProcessID, hThread, dwThreadID );

					::CloseHandle( hThread );
					hThread = NULL;
				}
			}
		}

		::CloseHandle( hProcess );
		hProcess = NULL;
	}

	return bResult;
}

bool	FLDumpRunner::DumpProcess( FLDumper* pDumper,
								int nLevel,
								DWORD dwExceptionCode,
								HANDLE hProcess,
								DWORD dwProcessID,
								HANDLE hThread,
								DWORD dwThreadID )
{
	bool bResult = false;
	DWORD dwCurrentThreadID = ::GetCurrentThreadId();
	if( dwCurrentThreadID != dwThreadID )
	{
		::SuspendThread( hThread );
	}

	EXCEPTION_RECORD tExceptionRecord = {};
	CONTEXT tContext = {};
	EXCEPTION_POINTERS tExceptionPtrs = { &tExceptionRecord, &tContext };

	tContext.ContextFlags = CONTEXT_FULL;
	if( ::GetThreadContext( hThread, &tContext ) != FALSE )
	{
		tExceptionRecord.ExceptionCode = dwExceptionCode;
		tExceptionRecord.NumberParameters = 0;

		bResult = FLDumpRunner::Execute( pDumper, nLevel, hProcess, dwProcessID, hThread, dwThreadID, &tExceptionPtrs, NULL, NULL, NULL, 0 );
	}

	if( dwCurrentThreadID != dwThreadID )
	{
		::ResumeThread( hThread );
	}

	return bResult;
}

bool	FLDumpRunner::Execute( FLDumper* pDumper,
							int nLevel,
							HANDLE hProcess,
							DWORD dwProcessID,
							HANDLE hThread,
							DWORD dwThreadID,
							EXCEPTION_POINTERS* pException,
							const TCHAR* szExpression,
							const TCHAR* szFunction,
							const TCHAR* szFile,
							unsigned int nLine )
{
	if( pDumper == NULL || pException == NULL )
	{
		return false;
	}

	if( nLevel == FLDUMP_LEVEL_NONE )
	{
		return true;
	}

	bool bOutOfProcess = (::GetCurrentProcessId() == dwProcessID) ? false : true;
	if( bOutOfProcess == false )
	{
		FLDumpRunner* pRunner = FLDumpRunner::GetInstance();
		if( pRunner != NULL )
		{
			bool bStopSearch = false;
			 if( pRunner->DoExecuteThread( pDumper,
										nLevel,
										hProcess,
										dwProcessID,
										hThread,
										dwThreadID,
										bOutOfProcess,
										pException,
										szExpression,
										szFunction,
										szFile,
										nLine,
										&bStopSearch ) == true )
			 {
				 return bStopSearch;
			 }
		}
	}

	return DoExecute( pDumper,
					nLevel,
					hProcess,
					dwProcessID,
					hThread,
					dwThreadID,
					bOutOfProcess,
					pException,
					szExpression,
					szFunction,
					szFile,
					nLine );
}

bool	FLDumpRunner::DoExecute( FLDumper* pDumper,
								int nLevel,
								HANDLE hProcess,
								DWORD dwProcessID,
								HANDLE hThread,
								DWORD dwThreadID,
								bool bOutOfProcess,
								EXCEPTION_POINTERS* pException,
								const TCHAR* szExpression,
								const TCHAR* szFunction,
								const TCHAR* szFile,
								unsigned int nLine )
{
	if( pDumper != NULL )
	{
		T_DUMP_PARAM tParam;
		::memset( &tParam, 0, sizeof( tParam ) );
		T_DUMP_ASSERT_DATA tAssert;
		::memset( &tAssert, 0, sizeof( tAssert ) );

		FLMakeDumpParam( &tParam,
						&tAssert,
						nLevel,
						hProcess,
						dwProcessID,
						hThread,
						dwThreadID,
						bOutOfProcess,
						pException,
						szExpression,
						szFunction,
						szFile,
						nLine );

		return pDumper->Dump( &tParam );
	}

	return false;
}

FLDumpRunner*	FLDumpRunner::GetInstance()
{
	static FLDumpRunner kDumpRunner;
	return &kDumpRunner;
}

FLDumpRunner::FLDumpRunner()
{
	m_hThread = NULL;
	m_bShutdown = false;
	m_hStart = NULL;
	m_hFinish = NULL;

	ClearData();

	::InitializeCriticalSection( &m_tLock );
}

FLDumpRunner::~FLDumpRunner()
{
	Stop();

	::DeleteCriticalSection( &m_tLock );
}

bool	FLDumpRunner::Start()
{
	::EnterCriticalSection( &m_tLock );

	DoStop();

	if( DoStart() == false )
	{
		DoStop();

		::LeaveCriticalSection( &m_tLock );
		return false;
	}

	::LeaveCriticalSection( &m_tLock );
	return true;
}

void	FLDumpRunner::Stop()
{
	::EnterCriticalSection( &m_tLock );

	DoStop();

	::LeaveCriticalSection( &m_tLock );
}

bool	FLDumpRunner::DoExecuteThread( FLDumper* pDumper,
										int nLevel,
										HANDLE hProcess,
										DWORD dwProcessID,
										HANDLE hThread,
										DWORD dwThreadID,
										bool bOutOfProcess,
										EXCEPTION_POINTERS* pException,
										const TCHAR* szExpression,
										const TCHAR* szFunction,
										const TCHAR* szFile,
										unsigned int nLine,
										bool* pStopSearch )
{
	bool bResult = false;

	::EnterCriticalSection( &m_tLock );

	if( IsDumpable() == true )
	{
		SetData( pDumper, nLevel, hProcess, dwProcessID, hThread, dwThreadID, bOutOfProcess, pException, szExpression, szFunction, szFile, nLine );

		::ReleaseSemaphore( m_hStart, 1, NULL );
		::WaitForSingleObject( m_hFinish, INFINITE );

		bResult = true;
		if( pStopSearch != NULL )
		{
			*pStopSearch = m_bStopSearch;
		}

		ClearData();
	}

	::LeaveCriticalSection( &m_tLock );

	return bResult;
}

bool	FLDumpRunner::DoStart()
{
	m_bShutdown = false;
	m_hStart = ::CreateSemaphore( NULL, 0, 1, NULL );
	if( m_hStart == NULL )
	{
		return false;
	}

	m_hFinish = ::CreateSemaphore( NULL, 0, 1, NULL );
	if( m_hFinish == NULL )
	{
		return false;
	}

	unsigned int dwID = 0;
	static const int FLDUMP_THREAD_INITIAL_STACK_SIZE = 64 * 1024;	// from google-breakpad
	m_hThread = reinterpret_cast< HANDLE >( ::_beginthreadex( NULL, FLDUMP_THREAD_INITIAL_STACK_SIZE, DumpThreadFunction, this, 0, &dwID ) );
	if( m_hThread == NULL )
	{
		return false;
	}

	return true;
}

void	FLDumpRunner::DoStop()
{
	m_bShutdown = true;
	if( m_hThread != NULL )
	{
		if( m_hStart != NULL )
		{
			::ReleaseSemaphore( m_hStart, 1, NULL );
			::WaitForSingleObject( m_hThread, INFINITE );
		}
		else
		{
			::TerminateThread( m_hThread, 1 );
		}

		::CloseHandle( m_hThread );
		m_hThread = NULL;
	}

	if( m_hStart != NULL )
	{
		::CloseHandle( m_hStart );
		m_hStart = NULL;
	}

	if( m_hFinish != NULL )
	{
		::CloseHandle( m_hFinish );
		m_hFinish = NULL;
	}
}

bool	FLDumpRunner::IsDumpable()	const
{
	if( m_hThread == NULL )
	{
		return false;
	}

	if( m_bShutdown == true )
	{
		return false;
	}

	if( m_hStart == NULL )
	{
		return false;
	}

	if( m_hFinish == NULL )
	{
		return false;
	}

	return true;
}

void	FLDumpRunner::SetData( FLDumper* pDumper,
							int nLevel,
							HANDLE hProcess,
							DWORD dwProcessID,
							HANDLE hThread,
							DWORD dwThreadID,
							bool bOutOfProcess,
							EXCEPTION_POINTERS* pException,
							const TCHAR* szExpression,
							const TCHAR* szFunction,
							const TCHAR* szFile,
							unsigned int nLine )
{
	m_pDumper = pDumper;
	m_bStopSearch = false;

	FLMakeDumpParam( &m_tParam,
					&m_tAssert,
					nLevel,
					hProcess,
					dwProcessID,
					hThread,
					dwThreadID,
					bOutOfProcess,
					pException,
					szExpression,
					szFunction,
					szFile,
					nLine );
}

void	FLDumpRunner::ClearData()
{
	m_pDumper = NULL;

	m_bStopSearch = false;

	::memset( &m_tParam, 0, sizeof( m_tParam ) );
	::memset( &m_tAssert, 0, sizeof( m_tAssert ) );
}

void	FLDumpRunner::Run()
{
	while( ::WaitForSingleObject( m_hStart, INFINITE ) == WAIT_OBJECT_0 )
	{
		if( m_bShutdown == true )
		{
			break;
		}
		else
		{
			m_bStopSearch = false;
			if( m_pDumper != NULL )
			{
				m_bStopSearch = m_pDumper->Dump( &m_tParam );
			}
		}

		::ReleaseSemaphore( m_hFinish, 1, NULL );
	}
}

unsigned int __stdcall	FLDumpRunner::DumpThreadFunction( void* pArg )
{
	FLDumpRunner* pDumpRunner = reinterpret_cast < FLDumpRunner* >( pArg );
	if( pDumpRunner != NULL )
	{
		pDumpRunner->Run();
	}

	return 0;
}
