
#include "FLStackWalker.h"
#include "../common/FLStringFunction.h"

#include <psapi.h>

//////////////////////////////////////////////////////////////////////////
namespace
{

	BOOL	CALLBACK	FLReadProcessMemoryProc64( HANDLE hProcess, DWORD64 pBaseAddress, PVOID pBuffer, DWORD dwSize, PDWORD pNumberOfBytesRead )
	{
		SIZE_T nNumberOfBytesRead = 0;
		BOOL bResult = ::ReadProcessMemory( hProcess, reinterpret_cast< LPCVOID >( pBaseAddress ), pBuffer, dwSize, &nNumberOfBytesRead );
		*pNumberOfBytesRead = static_cast< DWORD >( nNumberOfBytesRead );
		return bResult;
	}

}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
void	FLStackWalkReporter::Report( const T_CALL_STACK_ENTRY* pEntry )
{
	DoReport( pEntry );
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
bool	FLStackWalker::FLStackWalkContext::Initialize( EXCEPTION_POINTERS* pExceptionPtr, FLStackWalkReporter* pReporter, HANDLE hProcess, HANDLE hThread )
{
	if( pExceptionPtr == NULL || hProcess == NULL || hThread == NULL )
	{
		return false;
	}

	dwWalkCount = 0;
	this->pReporter = pReporter;
	this->hProcess = hProcess;
	this->hThread = hThread;

	tContext = *pExceptionPtr->ContextRecord;
	::memset( &tStackFrame, 0, sizeof( tStackFrame ) );

#if defined( _M_IX86 )
	dwMachineType					= IMAGE_FILE_MACHINE_I386;
	tStackFrame.AddrPC.Offset		= tContext.Eip;
	tStackFrame.AddrPC.Mode			= AddrModeFlat;
	tStackFrame.AddrFrame.Offset	= tContext.Ebp;
	tStackFrame.AddrFrame.Mode		= AddrModeFlat;
	tStackFrame.AddrStack.Offset	= tContext.Esp;
	tStackFrame.AddrStack.Mode		= AddrModeFlat;
#elif defined( _M_X64 )
	dwMachineType					= IMAGE_FILE_MACHINE_AMD64;
	tStackFrame.AddrPC.Offset		= tContext.Rip;
	tStackFrame.AddrPC.Mode			= AddrModeFlat;
	tStackFrame.AddrFrame.Offset	= tContext.Rsp;
	tStackFrame.AddrFrame.Mode		= AddrModeFlat;
	tStackFrame.AddrStack.Offset	= tContext.Rsp;
	tStackFrame.AddrStack.Mode		= AddrModeFlat;
#elif defined( _M_IA64 )
	dwMachineType					= IMAGE_FILE_MACHINE_IA64;
	tStackFrame.AddrPC.Offset		= tContext.StIIP;
	tStackFrame.AddrPC.Mode			= AddrModeFlat;
	tStackFrame.AddrFrame.Offset	= tContext.IntSp;
	tStackFrame.AddrFrame.Mode		= AddrModeFlat;
	tStackFrame.AddrBStore.Offset	= tContext.RsBSP;
	tStackFrame.AddrBStore.Mode		= AddrModeFlat;
	tStackFrame.AddrStack.Offset	= tContext.IntSp;
	tStackFrame.AddrStack.Mode		= AddrModeFlat;
#else
#error CPU architecture is not supported.
#endif

	return true;
}
//////////////////////////////////////////////////////////////////////////

FLStackWalker::FLStackWalker()
{
	m_hDbgHelpDll = NULL;

	m_pSymInitialize = NULL;
	m_pSymCleanup = NULL;

	m_pSymGetOptions = NULL;
	m_pSymSetOptions = NULL;

	m_pStackWalk64 = NULL;
	m_pSymFunctionTableAccess64 = NULL;
	m_pSymGetModuleBase64 = NULL;

	m_pSymGetFromAddr64 = NULL;
	m_pSymGetLineFromAddr64 = NULL;
}

FLStackWalker::~FLStackWalker()
{
	UnloadFunction( NULL );
}

bool	FLStackWalker::Walk( EXCEPTION_POINTERS* pExceptionPtr, FLStackWalkReporter* pReporter, HANDLE hProcess, HANDLE hThread )
{
	FLStackWalkContext cContext;
	if( cContext.Initialize( pExceptionPtr, pReporter, hProcess, hThread ) == false )
	{
		return false;
	}

	if( LoadFunctions( cContext.hProcess ) == false )
	{
		return false;
	}

	while( DoStep( &cContext ) == true )
	{
	}

	UnloadFunction( cContext.hProcess );
	return true;
}

T_CALL_STACK_ENTRY	FLStackWalker::SeekTop( EXCEPTION_POINTERS* pExceptionPtr, HANDLE hProcess, HANDLE hThread )
{
	T_CALL_STACK_ENTRY tEntry;
	::memset( &tEntry, 0, sizeof( tEntry ) );

	FLStackWalkContext cContext;
	if( cContext.Initialize( pExceptionPtr, NULL, hProcess, hThread ) == true )
	{
		if( LoadFunctions( cContext.hProcess ) == true )
		{
			GetStackEntry( &cContext, &tEntry );
			UnloadFunction( cContext.hProcess );
		}
	}

	return tEntry;
}

bool	FLStackWalker::LoadFunctions( HANDLE hProcess )
{
	m_hDbgHelpDll = FLLoadDll( hProcess, _T( "dbghelp.dll" ) );
	if( m_hDbgHelpDll != NULL )
	{
		m_pSymInitialize			= reinterpret_cast< PSYM_INITIALIZE_F >( ::GetProcAddress( m_hDbgHelpDll, "SymInitialize" ) );
		m_pSymCleanup				= reinterpret_cast< PSYM_CLEANUP_F >( ::GetProcAddress( m_hDbgHelpDll, "SymCleanup" ) );

		m_pSymGetOptions			= reinterpret_cast< PSYM_GET_OPTIONS_F >( ::GetProcAddress( m_hDbgHelpDll, "SymGetOptions" ) );
		m_pSymSetOptions			= reinterpret_cast< PSYM_SET_OPTIONS_F >( ::GetProcAddress( m_hDbgHelpDll, "SymSetOptions" ) );

		m_pStackWalk64				= reinterpret_cast< PSTACK_WALK64_F >( ::GetProcAddress( m_hDbgHelpDll, "StackWalk64" ) );
		m_pSymFunctionTableAccess64	= reinterpret_cast< PSYM_FUNCTION_TABLE_ACCESS64_F >( ::GetProcAddress( m_hDbgHelpDll, "SymFunctionTableAccess64" ) );
		m_pSymGetModuleBase64		= reinterpret_cast< PSYM_GET_MODULE_BASE64_F >( ::GetProcAddress( m_hDbgHelpDll, "SymGetModuleBase64" ) );

		m_pSymGetFromAddr64			= reinterpret_cast< PSYM_GET_FROM_ADDR64_F >( ::GetProcAddress( m_hDbgHelpDll, "SymGetSymFromAddr64" ) );
		m_pSymGetLineFromAddr64		= reinterpret_cast< PSYM_GET_LINE_FROM_ADDR64_F >( ::GetProcAddress( m_hDbgHelpDll, "SymGetLineFromAddr64" ) );

		if( m_pSymInitialize != NULL && m_pSymCleanup != NULL &&
			m_pSymGetOptions != NULL && m_pSymSetOptions != NULL &&
			m_pStackWalk64 != NULL && m_pSymFunctionTableAccess64 != NULL && m_pSymGetModuleBase64 != NULL &&
			m_pSymGetFromAddr64 != NULL && m_pSymGetLineFromAddr64 != NULL )
		{
			if( m_pSymInitialize( hProcess, NULL, TRUE ) != FALSE )
			{
				DWORD dwOptions = m_pSymGetOptions();
				m_pSymSetOptions( dwOptions | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME );
				//m_pSymSetOptions( dwOptions | SYMOPT_FAIL_CRITICAL_ERRORS ); //| SYMOPT_LOAD_LINES );
				return true;
			}
		}
	}

	return false;
}

void	FLStackWalker::UnloadFunction( HANDLE hProcess )
{
	if( m_pSymCleanup != NULL && hProcess != NULL )
	{
		m_pSymCleanup( hProcess );
	}

	m_pSymInitialize = NULL;
	m_pSymCleanup = NULL;

	m_pSymGetOptions = NULL;
	m_pSymSetOptions = NULL;

	m_pStackWalk64 = NULL;
	m_pSymFunctionTableAccess64 = NULL;
	m_pSymGetModuleBase64 = NULL;

	m_pSymGetFromAddr64 = NULL;
	m_pSymGetLineFromAddr64 = NULL;

	if( m_hDbgHelpDll != NULL )
	{
		::FreeLibrary( m_hDbgHelpDll );
		m_hDbgHelpDll = NULL;
	}
}

bool	FLStackWalker::DoStep( FLStackWalker::FLStackWalkContext* pContext )
{
	// 무한 재귀호출 방지
	if( ++pContext->dwWalkCount > FLSTACKWALKER_MAX_WALK_COUNT )
	{
		return false;
	}

	T_CALL_STACK_ENTRY tEntry;
	::memset( &tEntry, 0, sizeof( tEntry ) );

	if( GetStackEntry( pContext, &tEntry ) == false )
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	if( pContext->pReporter != NULL )
	{
		pContext->pReporter->Report( &tEntry );
	}
	//////////////////////////////////////////////////////////////////////////

	return true;
}

bool	FLStackWalker::GetStackEntry( FLStackWalkContext* pContext, T_CALL_STACK_ENTRY* pEntry )
{
	if( m_pStackWalk64( pContext->dwMachineType,
						pContext->hProcess,
						pContext->hThread,
						&pContext->tStackFrame,
						&pContext->tContext,
						FLReadProcessMemoryProc64,
						m_pSymFunctionTableAccess64,
						m_pSymGetModuleBase64,
						NULL ) == FALSE )
	{
		return false;
	}

	HINSTANCE hModule = reinterpret_cast< HINSTANCE >( m_pSymGetModuleBase64( pContext->hProcess, pContext->tStackFrame.AddrPC.Offset ) );
	if( hModule != NULL )
	{
		if( ::GetModuleFileNameEx( pContext->hProcess, hModule, pEntry->szModule, _countof( pEntry->szModule ) ) == 0 )
		{
			FLStrcpy( pEntry->szModule, _countof( pEntry->szModule ), _T( "Unknown" ) );
		}
	}

	pEntry->u64ExceptionAddress = pContext->tStackFrame.AddrPC.Offset;
	pEntry->wExceptionSegment = pContext->tStackFrame.AddrPC.Segment;

	BYTE arrSymBuffer[ sizeof( IMAGEHLP_SYMBOL64 ) + FLSTACKWALKER_MAX_NAME_LEN ] = { 0, };
	PIMAGEHLP_SYMBOL64 pSymbol = reinterpret_cast< PIMAGEHLP_SYMBOL64 >( arrSymBuffer );
	pSymbol->SizeOfStruct = sizeof( *pSymbol );
	pSymbol->MaxNameLength = FLSTACKWALKER_MAX_NAME_LEN;

	DWORD64 dwDisplacement64 = 0;
	if( m_pSymGetFromAddr64( pContext->hProcess, pEntry->u64ExceptionAddress, &dwDisplacement64, pSymbol ) != FALSE )
	{
#ifdef _UNICODE
		FLMultiByteToWideChar( CP_ACP, pSymbol->Name, -1, pEntry->szFunctionName, _countof( pEntry->szFunctionName ) );
#else
		FLStrcpy( pEntry->szFunctionName, _countof( pEntry->szFunctionName ), pSymbol->Name );
#endif
		pEntry->u64FunctionOffset = dwDisplacement64;
	}

	DWORD dwDisplacement32 = 0;
	IMAGEHLP_LINE64 tLine64;
	::memset( &tLine64, 0, sizeof( tLine64 ) );
	tLine64.SizeOfStruct = sizeof( tLine64 );

	if( m_pSymGetLineFromAddr64( pContext->hProcess, pEntry->u64ExceptionAddress, &dwDisplacement32, &tLine64 ) )
	{
#ifdef _UNICODE
		FLMultiByteToWideChar( CP_ACP, tLine64.FileName, -1, pEntry->szSourceFile, _countof( pEntry->szSourceFile ) );
#else
		FLStrcpy( pEntry->szSourceFile, _countof( pEntry->szSourceFile ), tLine64.FileName );
#endif
		pEntry->dwLineNumber = tLine64.LineNumber;
		pEntry->dwLineOffset = dwDisplacement32;
	}

	return true;
}
