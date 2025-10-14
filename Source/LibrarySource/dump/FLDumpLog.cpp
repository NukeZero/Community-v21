
#include "FLDumpLog.h"
#include "FLStackWalker.h"

#include "../common/FLStringFunction.h"
#include "../common/FLSystemUtil.h"
#include "../common/FLEnumProcess.h"
#include "../common/FLEnumThread.h"
#include "../common/FLEnumModule.h"
#include "../common/FLModuleInfo.h"
#include "../bugtrap/FLBugTrapper.h"

#include <psapi.h>


//////////////////////////////////////////////////////////////////////////
namespace
{

	void	FLFormatingTime( TCHAR* szOutput, size_t cchOutput, FILETIME* pTime )
	{
		if( szOutput != NULL && cchOutput > 0 && pTime != NULL )
		{
			szOutput[0] = _T( '\0' );
			WORD wFATDate = 0, wFATTime = 0;
			if( ::FileTimeToLocalFileTime( pTime, pTime ) && ::FileTimeToDosDateTime( pTime, &wFATDate, &wFATTime ) )
			{
				FLSPrintf( szOutput, cchOutput, _T( "%d/%d/%d %02d:%02d:%02d" ),
					(wFATDate / 512) + 1980, (wFATDate / 32) & 15, wFATDate & 31,
					(wFATTime >> 11), (wFATTime >> 5) & 0x3F, (wFATTime & 0x1F) * 2 );
			}
		}
	}

	void	FLGetExceptionString( DWORD dwCode, TCHAR* szException, size_t cchException )
	{
		if( cchException > UINT_MAX )
		{
			return;
		}

#define	FLEXCEPTION_STRING( dwCode )	\
	case EXCEPTION_##dwCode:	\
		{	\
			FLStrcpy( szException, cchException, _T( #dwCode ) );	\
			return;	\
		}	\
		break;

		switch( dwCode )
		{
			FLEXCEPTION_STRING( ACCESS_VIOLATION )
			FLEXCEPTION_STRING( DATATYPE_MISALIGNMENT )
			FLEXCEPTION_STRING( BREAKPOINT )
			FLEXCEPTION_STRING( SINGLE_STEP )
			FLEXCEPTION_STRING( ARRAY_BOUNDS_EXCEEDED )
			FLEXCEPTION_STRING( FLT_DENORMAL_OPERAND )
			FLEXCEPTION_STRING( FLT_DIVIDE_BY_ZERO )
			FLEXCEPTION_STRING( FLT_INEXACT_RESULT )
			FLEXCEPTION_STRING( FLT_INVALID_OPERATION )
			FLEXCEPTION_STRING( FLT_OVERFLOW )
			FLEXCEPTION_STRING( FLT_STACK_CHECK )
			FLEXCEPTION_STRING( FLT_UNDERFLOW )
			FLEXCEPTION_STRING( INT_DIVIDE_BY_ZERO )
			FLEXCEPTION_STRING( INT_OVERFLOW )
			FLEXCEPTION_STRING( PRIV_INSTRUCTION )
			FLEXCEPTION_STRING( IN_PAGE_ERROR )
			FLEXCEPTION_STRING( ILLEGAL_INSTRUCTION )
			FLEXCEPTION_STRING( NONCONTINUABLE_EXCEPTION )
			FLEXCEPTION_STRING( STACK_OVERFLOW )
			FLEXCEPTION_STRING( INVALID_DISPOSITION )
			FLEXCEPTION_STRING( GUARD_PAGE )
			FLEXCEPTION_STRING( INVALID_HANDLE )
		}

#undef FLEXCEPTION_STRING

		// If not one of the "known" exceptions, try to get the string
		// from NTDLL.DLL's message table.

		::FormatMessage( FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE,
			::GetModuleHandle( _T( "ntdll.dll" ) ),
			dwCode, 0, szException, static_cast< DWORD >( cchException ), NULL );
	}

}
//////////////////////////////////////////////////////////////////////////


#define ONEKILO		1024
#define ONEMEGA		( ONEKILO * ONEKILO )


FLDumpLog::FLDumpLog( const TCHAR* szDumpFolder, const TCHAR* szPrefix )
	: m_szDumpFolder( szDumpFolder )
	, m_szPrefix( szPrefix )
{
}

bool	FLDumpLog::DoDump( T_DUMP_PARAM* pParam )
{
	if( pParam != NULL )
	{
		TCHAR szDumpFileName[MAX_PATH] = { 0, };
		FLMakeFileNameByProcess( szDumpFileName, _countof( szDumpFileName ), pParam->hProcess, m_szDumpFolder, m_szPrefix, _T( "rpt" ) );

		FLTextFile cTextFile;
		if( cTextFile.Create( szDumpFileName, GENERIC_WRITE, FILE_SHARE_READ ) == true )
		{
			Log( &cTextFile, pParam );

			//////////////////////////////////////////////////////////////////////////
			FLBugTrapper::AddFile( szDumpFileName );
			//////////////////////////////////////////////////////////////////////////

			return true;
		}
	}

	return false;
}

void	FLDumpLog::Log( FLTextFile* pFile, T_DUMP_PARAM* pParam )
{
	if( pFile != NULL && pParam != NULL )
	{
		HeaderLog( pFile );

		//////////////////////////////////////////////////////////////////////////
		AssertionFailedLog( pFile, pParam );
		ExceptionInformationLog( pFile, pParam );
		//////////////////////////////////////////////////////////////////////////
		ProgramInformationLog( pFile, pParam );
		SystemInformationLog( pFile, pParam );
		//////////////////////////////////////////////////////////////////////////
		RegisterLog( pFile, pParam );
		ThreadsCallStackLog( pFile, pParam );
		//////////////////////////////////////////////////////////////////////////
		ModuleListLog( pFile, pParam );
		ProcessesInformationLog( pFile, pParam );
		//////////////////////////////////////////////////////////////////////////

		FooterLog( pFile );
	}
}

void	FLDumpLog::HeaderLog( FLTextFile* pFile )
{
	FILETIME tCurrentTime;
	::GetSystemTimeAsFileTime( &tCurrentTime );

	TCHAR szTimeBuffer[100] = { 0, };
	FLFormatingTime( szTimeBuffer, _countof( szTimeBuffer ), &tCurrentTime );

	FLDumpPrint( pFile, _T( "\r\n===== [logged at %s] =====\r\n\r\n" ), szTimeBuffer );
}

void	FLDumpLog::FooterLog( FLTextFile* pFile )
{
	FLDumpPrint( pFile, _T( "===== [end of file] =====\r\n" ) );
}

void	FLDumpLog::AssertionFailedLog( FLTextFile* pFile, T_DUMP_PARAM* pParam )
{
	const T_DUMP_ASSERT_DATA* pAssertData = pParam->pAssertData;
	if( pAssertData != NULL )
	{
		size_t nExpressionLen = FLStrlen( pAssertData->szExpression, _countof( pAssertData->szExpression ) );
		size_t nFunctionLen = FLStrlen( pAssertData->szFunction, _countof( pAssertData->szFunction ) );
		size_t nFileLen = FLStrlen( pAssertData->szFile, _countof( pAssertData->szFile ) );

		if( nExpressionLen > 0 || nFunctionLen > 0 || nFileLen > 0 )
		{
			FLDumpPrint( pFile, _T( "[Assertion Failed]\r\n" ) );

			if( nExpressionLen > 0 )
			{
				FLDumpPrint( pFile, _T( "Expression: %s\r\n" ), pAssertData->szExpression );
			}

			if( nFunctionLen > 0 )
			{
				FLDumpPrint( pFile, _T( "Function: %s\r\n" ), pAssertData->szFunction );
			}

			if( nFileLen > 0 )
			{
				FLDumpPrint( pFile, _T( "File: %s(%d)\r\n" ), pAssertData->szFile, pAssertData->nLine );
			}

			FLDumpPrint( pFile, _T( "\r\n" ) );
		}
	}
}

void	FLDumpLog::ExceptionInformationLog( FLTextFile* pFile, T_DUMP_PARAM* pParam )
{
	PEXCEPTION_RECORD pException = pParam->pException->ExceptionRecord;

	TCHAR szProcessName[MAX_PATH] = { 0, };
	if( ::GetModuleFileNameEx( pParam->hProcess, NULL, szProcessName, _countof( szProcessName ) ) == 0 )
	{
		FLStrcpy( szProcessName, _countof( szProcessName ), _T( "Unknown" ) );
	}

	FLStackWalker cStackWalker;
	T_CALL_STACK_ENTRY tEntry = cStackWalker.SeekTop( pParam->pException, pParam->hProcess, pParam->hThread );

	TCHAR szException[1024] = { 0, };
	FLGetExceptionString( pException->ExceptionCode, szException, _countof( szException ) );

	FLDumpPrint( pFile, _T( "[Exception Information]\r\n%s (0x%08X) %s(%u)\r\nin module %s at " ),
		szException,
		pException->ExceptionCode,
		szProcessName,
		pParam->dwProcessID,
		tEntry.szModule );

#if defined _WIN64
	FLDumpPrint( pFile, _T( "%04lX:%016lX\r\n" ), tEntry.wExceptionSegment, tEntry.u64ExceptionAddress );
#elif defined _WIN32
	FLDumpPrint( pFile, _T( "%04lX:%08lX\r\n" ), tEntry.wExceptionSegment, tEntry.u64ExceptionAddress );
#endif

	if( pException->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && pException->NumberParameters >= 2 )
	{
		const TCHAR* szReadFrom = _T( "Read from" );
		if( pException->ExceptionInformation[0] )
		{
			szReadFrom = _T( "Write to" );
		}

		FLDumpPrint( pFile, _T( "%s location %08x caused an access violation.\r\n" ),
			szReadFrom, pException->ExceptionInformation[1] );
	}

	FLDumpPrint( pFile, _T( "\r\n" ) );
}

void	FLDumpLog::ProgramInformationLog( FLTextFile* pFile, T_DUMP_PARAM* pParam )
{
	FLDumpPrint( pFile, _T( "[Program Information]\r\n" ) );

	//////////////////////////////////////////////////////////////////////////
	TCHAR szModuleName[MAX_PATH] = { 0, };
	if( ::GetModuleFileNameEx( pParam->hProcess, NULL, szModuleName, _countof( szModuleName ) ) == 0 )
	{
		FLStrcpy( szModuleName, _countof( szModuleName ), _T( "UnknownProgram" ) );
	}

	unsigned __int64 u64WorkingSet = 0;
	PROCESS_MEMORY_COUNTERS tMemoryCounters;
	if( ::GetProcessMemoryInfo( pParam->hProcess, &tMemoryCounters, sizeof( tMemoryCounters ) ) != FALSE )
	{
		u64WorkingSet = (tMemoryCounters.WorkingSetSize + ONEMEGA - 1) / ONEMEGA;
	}

	FLDumpPrint( pFile, _T( "%s(%u)\r\n" ), szModuleName, pParam->dwProcessID );
	FLDumpPrint( pFile, _T( "%I64u MBytes memory in use.\r\n" ), u64WorkingSet );
	//////////////////////////////////////////////////////////////////////////

	FLDumpPrint( pFile, _T( "\r\n" ) );
}

void	FLDumpLog::SystemInformationLog( FLTextFile* pFile, T_DUMP_PARAM* /*pParam*/ )
{
	FLDumpPrint( pFile, _T( "[System Information]\r\n" ) );

	//////////////////////////////////////////////////////////////////////////
	// windows 버전 정보
	TCHAR szWinVersion[50] = { 0, };
	TCHAR szBuild[50] = { 0, };
	FLGetWinVersion( szWinVersion, _countof( szWinVersion ), szBuild, _countof( szBuild ) );
	FLDumpPrint( pFile, _T( "Operation system : %s (%s).\r\n" ), szWinVersion, szBuild );


	//////////////////////////////////////////////////////////////////////////
	// cpu 정보
	TCHAR szCpuString[MAX_PATH] = _T( "" );
	HKEY hKey = NULL;
	if( ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T( "Hardware\\Description\\System\\CentralProcessor\\0" ), 0, KEY_READ, &hKey ) == ERROR_SUCCESS )
	{
		DWORD dwCount = sizeof( szCpuString );
		if ( ::RegQueryValueEx( hKey, _T( "ProcessorNameString" ), NULL, NULL, reinterpret_cast< LPBYTE >( szCpuString ), &dwCount ) != ERROR_SUCCESS )
		{
			dwCount = sizeof( szCpuString );
			::RegQueryValueEx( hKey, _T( "Identifier" ), NULL, NULL, reinterpret_cast< LPBYTE >( szCpuString ), &dwCount );
		}
	}

	if( FLStrlen( szCpuString, _countof( szCpuString ) ) <= 0 )
	{
		FLStrcpy( szCpuString, _countof( szCpuString ), _T( "Unknown" ) );
	}

	FLSystemInfo cSysInfo;
	FLDumpPrint( pFile, _T( "%d processor(s), type %d(%s).\r\n" ), cSysInfo.dwNumberOfProcessors, cSysInfo.dwProcessorType, szCpuString );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// 메모리 정보
	MEMORYSTATUSEX tMemoryInfo;
	::memset( &tMemoryInfo, 0, sizeof( tMemoryInfo ) );
	tMemoryInfo.dwLength = sizeof( tMemoryInfo );
	::GlobalMemoryStatusEx( &tMemoryInfo );

	FLDumpPrint( pFile, _T( "%d%% memory in use.\r\n" ), tMemoryInfo.dwMemoryLoad );
	FLDumpPrint( pFile, _T( "%I64u MBytes physical memory.\r\n" ), (tMemoryInfo.ullTotalPhys + ONEMEGA - 1) / ONEMEGA );
	FLDumpPrint( pFile, _T( "%I64u MBytes physical memory free.\r\n" ), (tMemoryInfo.ullAvailPhys + ONEMEGA - 1) / ONEMEGA );
	FLDumpPrint( pFile, _T( "%I64u MBytes paging file.\r\n" ), (tMemoryInfo.ullTotalPageFile + ONEMEGA - 1) / ONEMEGA );
	FLDumpPrint( pFile, _T( "%I64u MBytes paging file free.\r\n" ), (tMemoryInfo.ullAvailPageFile + ONEMEGA - 1) / ONEMEGA );
	FLDumpPrint( pFile, _T( "%I64u MBytes user address space.\r\n" ), (tMemoryInfo.ullTotalVirtual + ONEMEGA - 1) / ONEMEGA );
	FLDumpPrint( pFile, _T( "%I64u MBytes user address space free.\r\n" ), (tMemoryInfo.ullAvailVirtual + ONEMEGA - 1) / ONEMEGA );
	//////////////////////////////////////////////////////////////////////////

	FLDumpPrint( pFile, _T( "\r\n" ) );
}

void	FLDumpLog::RegisterLog( FLTextFile* pFile, T_DUMP_PARAM* pParam )
{
	PCONTEXT pContext = pParam->pException->ContextRecord;

	FLDumpPrint( pFile, _T( "[Registers]\r\n" ) );

#if defined _M_IX86
	FLDumpPrint( pFile, _T( "EAX: %08X  EBX: %08X  ECX: %08X  EDX: %08X\r\n" ), pContext->Eax, pContext->Ebx, pContext->Ecx, pContext->Edx );
	FLDumpPrint( pFile, _T( "ESI: %08X  EDI: %08X  FLG: %08X\r\n" ), pContext->Esi, pContext->Edi, pContext->EFlags );
	FLDumpPrint( pFile, _T( "EBP: %08X  ESP: %08X  EIP: %08X\r\n" ), pContext->Ebp, pContext->Esp, pContext->Eip );
	FLDumpPrint( pFile, _T( "CS: %04X  DS: %04X  SS: %04X  ES: %04X  FS: %04X  GS: %04X\r\n" ), pContext->SegCs, pContext->SegDs, pContext->SegSs, pContext->SegEs, pContext->SegFs, pContext->SegGs );
#elif defined _M_X64
	FLDumpPrint( pFile, _T( "RAX: %016X  RBX: %016X\r\n" ), pContext->Rax, pContext->Rbx );
	FLDumpPrint( pFile, _T( "RCX: %016X  RDX: %016X\r\n" ), pContext->Rcx, pContext->Rdx );
	FLDumpPrint( pFile, _T( "RSI: %016X  RDI: %016X\r\n" ), pContext->Rsi, pContext->Rdi );
	FLDumpPrint( pFile, _T( "FLG: %08X           RBP: %016X\r\n" ), pContext->EFlags, pContext->Rbp );
	FLDumpPrint( pFile, _T( "RSP: %016X  RIP: %016X\r\n" ), pContext->Rsp, pContext->Rip );
	FLDumpPrint( pFile, _T( "CS: %04X  DS: %04X  SS: %04X  ES: %04X  FS: %04X  GS: %04X\r\n" ), pContext->SegCs, pContext->SegDs, pContext->SegSs, pContext->SegEs, pContext->SegFs, pContext->SegGs );
#else
	FLDumpPrint( pFile, _T( "CPU architecture is not supported.\r\n" ) );
#endif

	FLDumpPrint( pFile, _T( "\r\n" ) );
}

void	FLDumpLog::ThreadsCallStackLog( FLTextFile* pFile, T_DUMP_PARAM* pParam )
{
	FLDumpPrint( pFile, _T( "[Threads Call Stack]\r\n" ) );

	// Crash thread는 반드시 출력한다.
	FLDumpPrint( pFile, _T( "(Crash Thread(%u))\r\n" ), pParam->dwThreadID );
	FLPrintThreadCallStack( pFile, pParam->pException, pParam->hProcess, pParam->hThread );
	FLDumpPrint( pFile, _T( "\r\n" ) );


	//////////////////////////////////////////////////////////////////////////
	FLEnumThread cEnumThread;
	if( cEnumThread.Open() == true )
	{
		DWORD dwCurrentThreadID = ::GetCurrentThreadId();
		DWORD dwThreadID = cEnumThread.GetFirst( pParam->dwProcessID );
		while( dwThreadID != 0 )
		{
			if( dwThreadID != pParam->dwThreadID )
			{
				HANDLE hOtherThread = ::OpenThread( THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION | THREAD_SUSPEND_RESUME, FALSE, dwThreadID );
				if( hOtherThread != NULL )
				{
					if( dwCurrentThreadID != dwThreadID )
					{
						if( ::SuspendThread( hOtherThread ) == (DWORD)-1)
						{
							FLDumpPrint( pFile, _T( "(Running Thread(%u))\r\n" ), dwThreadID );
						}
						else
						{
							FLDumpPrint( pFile, _T( "(Suspended Thread(%u))\r\n" ), dwThreadID );
						}
					}
					else
					{
						FLDumpPrint( pFile, _T( "(Dumping Thread(%u))\r\n" ), dwThreadID );
					}

					EXCEPTION_RECORD tExceptionRecord = {};
					CONTEXT tContext = {};
					EXCEPTION_POINTERS tExceptionPtrs = { &tExceptionRecord, &tContext };

					tContext.ContextFlags = CONTEXT_FULL;
					if( ::GetThreadContext( hOtherThread, &tContext ) != FALSE )
					{
						FLPrintThreadCallStack( pFile, &tExceptionPtrs, pParam->hProcess, hOtherThread );
						FLDumpPrint( pFile, _T( "\r\n" ) );
					}

					if( dwCurrentThreadID != dwThreadID )
					{
						::ResumeThread( hOtherThread );
					}

					::CloseHandle( hOtherThread );
					hOtherThread = NULL;
				}
			}

			dwThreadID = cEnumThread.GetNext( pParam->dwProcessID );
		}

		cEnumThread.Close();
	}
	//////////////////////////////////////////////////////////////////////////


	FLDumpPrint( pFile, _T( "\r\n" ) );
}

void	FLDumpLog::ModuleListLog( FLTextFile* pFile, T_DUMP_PARAM* pParam )
{
	FLEnumModule kEnumModule;
	if( kEnumModule.Open( pParam->dwProcessID ) == true )
	{
		int nModuleNum = 0;

		const TCHAR* szModule = kEnumModule.GetFirst();
		while( szModule != NULL )
		{
			++nModuleNum;

			FLDumpPrint( pFile, _T( "[Module %d]\r\n" ), nModuleNum );
			FLDumpPrint( pFile, _T( "%s\r\n" ), szModule );

			PrintModuleInfo( pFile, szModule );

			FLDumpPrint( pFile, _T( "\r\n" ) );

			szModule = kEnumModule.GetNext();
		}

		kEnumModule.Close();
	}

	FLDumpPrint( pFile, _T( "\r\n" ) );
}

void	FLDumpLog::ProcessesInformationLog( FLTextFile* pFile, T_DUMP_PARAM* /*pParam*/ )
{
	FLDumpPrint( pFile, _T( "[Process Information]\r\n" ) );

	FLEnumProcess kEnumProcess;
	if( kEnumProcess.Open() == true )
	{
		int nProcessNum = 0;
		const TCHAR* szEnumProcess = kEnumProcess.GetFirst();
		while( szEnumProcess != NULL )
		{
			++nProcessNum;
			FLDumpPrint( pFile, _T( "[Process %d]\r\n" ), nProcessNum );

			DWORD dwProcessID = kEnumProcess.GetProcessID();

			TCHAR szProcessFullPath[MAX_PATH] = { 0, };
			//////////////////////////////////////////////////////////////////////////
			HANDLE hProcess = ::OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessID );
			if( hProcess != NULL )
			{
				if( ::GetModuleFileNameEx( hProcess, NULL, szProcessFullPath, _countof( szProcessFullPath ) ) != 0 )
				{
					szEnumProcess = szProcessFullPath;
				}

				::CloseHandle( hProcess );
				hProcess = NULL;
			}
			//////////////////////////////////////////////////////////////////////////

			FLDumpPrint( pFile, _T( "%s(%u)\r\n" ), szEnumProcess, dwProcessID );
			PrintModuleInfo( pFile, szEnumProcess );

			FLDumpPrint( pFile, _T( "\r\n" ) );

			szEnumProcess = kEnumProcess.GetNext();
		}

		kEnumProcess.Close();
	}

	FLDumpPrint( pFile, _T( "\r\n" ) );
}

void	FLDumpLog::PrintModuleInfo( FLTextFile* pFile, const TCHAR* szModulePath )
{
	FLModuleInfo cModuleInfo;
	if( cModuleInfo.Load( szModulePath ) == true )
	{
		FLDumpPrint( pFile, _T( "Version Information:\r\n" ) );

		FLDumpPrint( pFile, _T( "   Company:    %s\r\n" ), cModuleInfo.CompanyName() );
		FLDumpPrint( pFile, _T( "   Product:    %s\r\n" ), cModuleInfo.ProductName() );
		FLDumpPrint( pFile, _T( "   FileDesc:   %s\r\n" ), cModuleInfo.FileDescription() );

		const WORD* pFileVerion = cModuleInfo.FileVersion();
		FLDumpPrint( pFile, _T( "   FileVer:    %d.%d.%d.%d\r\n" ), 
			pFileVerion[0], pFileVerion[1], pFileVerion[2], pFileVerion[3] );

		const WORD* pProdVerion = cModuleInfo.ProductVersion();
		FLDumpPrint( pFile, _T( "   ProdVer:    %d.%d.%d.%d\r\n" ),
			pProdVerion[0], pProdVerion[1], pProdVerion[2], pProdVerion[3] );
	}
}
