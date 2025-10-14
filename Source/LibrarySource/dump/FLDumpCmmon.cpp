
#include "FLDumpCommon.h"
#include "FLStackWalker.h"

#include <cstdlib>
#include <shlwapi.h>
#include <psapi.h>

#include "../common/FLStringFunction.h"

#pragma comment( lib, "shlwapi.lib" )
#pragma comment( lib, "psapi.lib" )


HMODULE	FLLoadDll( HANDLE hProcess, const TCHAR* szDll )
{
	HMODULE hDll = NULL;
	TCHAR szPath[MAX_PATH] = { 0, };

	// 실행파일이 있는 곳의 dbghelp.dll을 먼저 로드한다.
	FLGetProcessFolder( hProcess, szPath, _countof( szPath ) );
	::PathAppend( szPath, szDll );
	hDll = ::LoadLibrary( szPath );

	// 실패하면 기본 dll을 로드한다.
	if( hDll == NULL )
	{
		hDll = ::LoadLibrary( szDll );
	}

	return hDll;
}

void	FLGetProcessFolder( HANDLE hProcess, TCHAR* szOutput, size_t cchOutput )
{
	if( szOutput != NULL && cchOutput > 0 )
	{
		TCHAR szPath[MAX_PATH] = { 0, };
		if( ::GetModuleFileNameEx( hProcess, NULL, szPath, _countof( szPath ) ) == 0 )
		{
			FLStrcpy( szOutput, cchOutput, _T( "./" ) );
			return;
		}

		::PathRemoveFileSpec( szPath );
		FLStrcpy( szOutput, cchOutput, szPath );
	}
}

void	FLGetProcessName( HANDLE hProcess, TCHAR* szOutput, size_t cchOutput )
{
	if( szOutput != NULL && cchOutput > 0 )
	{
		TCHAR szPath[MAX_PATH] = { 0, };
		if( ::GetModuleFileNameEx( hProcess, NULL, szPath, _countof( szPath ) ) == 0 )
		{
			FLStrcpy( szOutput, cchOutput, _T( "Unknown" ) );
			return;
		}

#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		::_tsplitpath_s( szPath, NULL, 0, NULL, 0, szOutput, cchOutput, NULL, 0 );
#else
		::_tsplitpath( szPath, NULL, NULL, szOutput, NULL );
#endif
	}
}

void	FLGetProcessFileName( HANDLE hProcess, TCHAR* szOutput, size_t cchOutput )
{
	if( szOutput != NULL && cchOutput > 0 )
	{
		TCHAR szPath[MAX_PATH] = { 0, };
		if( ::GetModuleFileNameEx( hProcess, NULL, szPath, _countof( szPath ) ) == 0 )
		{
			FLStrcpy( szOutput, cchOutput, _T( "Unknown.exe" ) );
			return;
		}

		TCHAR szFile[_MAX_FNAME] = { 0, };
		TCHAR szExt[_MAX_EXT] = { 0, };

#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		::_tsplitpath_s( szPath, NULL, 0, NULL, 0, szFile, _countof( szFile ), szExt, _countof( szExt ) );
#else
		::_tsplitpath( szPath, NULL, NULL, szFile, szExt );
#endif

#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		::_tmakepath_s( szOutput, cchOutput, NULL, NULL, szFile, szExt );
#else
		::_tmakepath( szOutput, NULL, NULL, szFile, szExt );
#endif
	}
}

void	FLMakeFileName( TCHAR* szOutput, size_t cchOutput, const TCHAR* szName, const TCHAR* szDestFolder, const TCHAR* szPrefix, const TCHAR* szExt )
{
	if( szOutput != NULL && cchOutput > 0 )
	{
		SYSTEMTIME tSystemTime;
		::memset( &tSystemTime, 0, sizeof( tSystemTime ) );
		::GetLocalTime( &tSystemTime );

		const TCHAR szFileNameFormat[] = _T( "%s\\%s%s_%04d-%02d-%02d_%02d-%02d-%02d.%s" );

		FLSPrintf( szOutput, cchOutput, szFileNameFormat,
			szDestFolder,
			szPrefix != NULL ? szPrefix : _T( "" ),
			szName,
			tSystemTime.wYear, tSystemTime.wMonth, tSystemTime.wDay,
			tSystemTime.wHour, tSystemTime.wMinute, tSystemTime.wSecond,
			szExt != NULL ? szExt : _T( "txt" ) );

		int nCount = 0;
		DWORD dwAttr = ::GetFileAttributes( szOutput );
		while( dwAttr != INVALID_FILE_ATTRIBUTES )
		{
			const TCHAR szFileNameFormatNext[] = _T( "%s\\%s%s_%04d-%02d-%02d_%02d-%02d-%02d(%d).%s" );

			++nCount;

			FLSPrintf( szOutput, cchOutput, szFileNameFormatNext,
				szDestFolder,
				szPrefix != NULL ? szPrefix : _T( "" ),
				szName,
				tSystemTime.wYear, tSystemTime.wMonth, tSystemTime.wDay,
				tSystemTime.wHour, tSystemTime.wMinute, tSystemTime.wSecond,
				nCount,
				szExt != NULL ? szExt : _T( "txt" ) );

			dwAttr = ::GetFileAttributes( szOutput );
		}
	}
}

void	FLMakeFileNameByProcess( TCHAR* szOutput, size_t cchOutput, HANDLE hProcess, const TCHAR* szDestFolder, const TCHAR* szPrefix, const TCHAR* szExt )
{
	if( szOutput != NULL && cchOutput > 0 )
	{
		// 폴더 얻기
		TCHAR szTempDir[MAX_PATH] = { 0, };
		if( szDestFolder != NULL )
		{
			FLStrcpy( szTempDir, _countof( szTempDir ), szDestFolder );
		}
		else	// 폴더가 지정되지 않았다면 프로세스가 존재하는 폴더를 선택한다.
		{
			FLGetProcessFolder( hProcess, szTempDir, _countof( szTempDir ) );
		}

		// 프로세스 이름 얻기
		TCHAR szProcessName[_MAX_FNAME]	= { 0, };
		FLGetProcessName( hProcess, szProcessName, _countof( szProcessName ) );

		FLMakeFileName( szOutput, cchOutput, szProcessName, szTempDir, szPrefix, szExt );
	}
}

void	FLDumpPrint( FLTextFile* pFile, const TCHAR* szFormat, ... )
{
	if( pFile != NULL && pFile->Opened() == true && szFormat != NULL )
	{
		TCHAR szBuffer[8*1024] = { 0, };

		va_list arglist;
		va_start( arglist, szFormat );
		FLVSPrintf( szBuffer, _countof( szBuffer ), szFormat, arglist );
		va_end( arglist );

		FLVERIFY( pFile->WriteText( szBuffer, ::_tcslen( szBuffer ) ) == ::_tcslen( szBuffer ) );
	}
}

void	FLPrintThreadCallStack( FLTextFile* pFile, PEXCEPTION_POINTERS pException, HANDLE hProcess, HANDLE hThread )
{
	FLFileStackWalkReporter kReporter( pFile );
	FLStackWalker kStackWalker;
	kStackWalker.Walk( pException, &kReporter, hProcess, hThread );
}

void	FLPrintCurrentThreadCallStack( FLTextFile* pFile, const TCHAR* szComment )
{
	if( pFile != NULL )
	{
		DWORD dwProcessID = ::GetCurrentProcessId();
		HANDLE hProcess = ::OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessID );
		if( hProcess != NULL )
		{
			HANDLE hThread = ::GetCurrentThread();
			DWORD dwThreadID = ::GetCurrentThreadId();

			FLDumpPrint( pFile, _T( "%s(Thread(%u) Call Stack)\r\n" ), szComment != NULL ? szComment : _T( "" ), dwThreadID );

			EXCEPTION_RECORD tExceptionRecord = {};
			CONTEXT tContext = {};
			EXCEPTION_POINTERS tExceptionPtrs = { &tExceptionRecord, &tContext };

			tContext.ContextFlags = CONTEXT_FULL;
			if( ::GetThreadContext( hThread, &tContext ) != FALSE )
			{
				FLPrintThreadCallStack( pFile, &tExceptionPtrs, hProcess, hThread );

				FLDumpPrint( pFile, _T( "\r\n" ) );
			}

			::CloseHandle( hProcess );
			hProcess = NULL;
		}
	}
}

void	FLPrintCallStackHelper( const TCHAR* szName, const TCHAR* szExpression, const TCHAR* szFunction, const TCHAR* szFile, int nLine )
{
	TCHAR szFolder[MAX_PATH] = { 0, };
	FLGetProcessFolder( ::GetCurrentProcess(), szFolder, _countof( szFolder ) );

	TCHAR szCallStackFile[MAX_PATH] = { 0, };

	SYSTEMTIME tSystemTime;
	::memset( &tSystemTime, 0, sizeof( tSystemTime ) );
	::GetLocalTime( &tSystemTime );

	FLSPrintf( szCallStackFile, _countof( szCallStackFile ), _T( "%s\\%s_%04d-%02d-%02d.rpt" ),
		szFolder, szName, tSystemTime.wYear, tSystemTime.wMonth, tSystemTime.wDay );

	FLTextFile kFile;

	int nCount = 0;
	while( kFile.Open( szCallStackFile, GENERIC_WRITE, FILE_SHARE_READ ) == false &&
		kFile.Create( szCallStackFile, GENERIC_WRITE, FILE_SHARE_READ ) == false )
	{
		++nCount;

		FLSPrintf( szCallStackFile, _countof( szCallStackFile ), _T( "%s\\%s_%04d-%02d-%02d(%d).rpt" ),
			szFolder, szName, tSystemTime.wYear, tSystemTime.wMonth, tSystemTime.wDay, nCount );
	}

	kFile.Seek( 0, FILE_END );

	FLDumpPrint( &kFile, _T( "[%04d-%02d-%02d %02d:%02d:%02d] %s at %s\r\n%s(%d)\r\n" ),
		tSystemTime.wYear, tSystemTime.wMonth, tSystemTime.wDay,
		tSystemTime.wHour, tSystemTime.wMinute, tSystemTime.wSecond,
		szExpression != NULL ? szExpression : _T( "No Expression" ),
		szFunction != NULL ? szFunction : _T( "Unknown Function" ),
		szFile != NULL ? szFile : _T( "Unknown File" ),
		nLine );

	FLPrintCurrentThreadCallStack( &kFile, szFunction );

	FLDumpPrint( &kFile, _T( "\r\n" ) );
}
