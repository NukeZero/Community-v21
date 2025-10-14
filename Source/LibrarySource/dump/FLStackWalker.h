
#ifndef	__FLSTACKWALKER_H__
#define	__FLSTACKWALKER_H__


#include "FLDumpCommon.h"

#include <dbghelp.h>


enum
{
	FLSTACKWALKER_MAX_WALK_COUNT	= 1000,
	FLSTACKWALKER_MAX_NAME_LEN		= 512,
};

struct	T_CALL_STACK_ENTRY
{
	TCHAR	szModule[MAX_PATH];

	DWORD64	u64ExceptionAddress;
	WORD	wExceptionSegment;

	TCHAR	szFunctionName[FLSTACKWALKER_MAX_NAME_LEN];
	DWORD64	u64FunctionOffset;

	TCHAR	szSourceFile[MAX_PATH];
	DWORD	dwLineNumber;
	DWORD	dwLineOffset;
};


//////////////////////////////////////////////////////////////////////////
class	FLStackWalkReporter
{
public:

	void	Report( const T_CALL_STACK_ENTRY* pEntry );

private:

	virtual void	DoReport( const T_CALL_STACK_ENTRY* pEntry )	= 0;

};
//////////////////////////////////////////////////////////////////////////
class	FLFileStackWalkReporter : public FLStackWalkReporter
{
public:

	explicit FLFileStackWalkReporter( FLTextFile* pFile )
	{
		m_pFile = pFile;
	}

private:

	void	DoReport( const T_CALL_STACK_ENTRY* pEntry )
	{
		if( m_pFile != NULL && pEntry != NULL )
		{
#if defined _WIN64
			FLDumpPrint( m_pFile, _T( "%04lX:%016lX" ), pEntry->wExceptionSegment, pEntry->u64ExceptionAddress );
#elif defined _WIN32
			FLDumpPrint( m_pFile, _T( "%04lX:%08lX" ), pEntry->wExceptionSegment, pEntry->u64ExceptionAddress );
#endif

			if( pEntry->u64FunctionOffset != 0 )
			{
				FLDumpPrint( m_pFile, _T( ", %s()+%I64u byte(s)" ), pEntry->szFunctionName, pEntry->u64FunctionOffset );
			}
			else
			{
				FLDumpPrint( m_pFile, _T( ", %s()" ), pEntry->szFunctionName );
			}

			FLDumpPrint( m_pFile, _T( ", %s" ), pEntry->szModule );

			if( pEntry->szSourceFile[0] != _T( '\0' ) )
			{
				FLDumpPrint( m_pFile, _T( " in \"%s\"" ), pEntry->szSourceFile );
			}

			if( pEntry->dwLineNumber != 0 )
			{
				if( pEntry->dwLineOffset != 0 )
				{
					FLDumpPrint( m_pFile, _T( ", line %d+%d byte(s)" ), pEntry->dwLineNumber, pEntry->dwLineOffset );
				}
				else
				{
					FLDumpPrint( m_pFile, _T( ", line %d" ), pEntry->dwLineNumber );
				}
			}

			FLDumpPrint( m_pFile, _T( "\r\n" ) );
		}
	}

private:

	FLTextFile*	m_pFile;

};
//////////////////////////////////////////////////////////////////////////


class	FLStackWalker
{
public:

	FLStackWalker();
	~FLStackWalker();

	bool	Walk( EXCEPTION_POINTERS* pExceptionPtr, FLStackWalkReporter* pReporter, HANDLE hProcess, HANDLE hThread );
	T_CALL_STACK_ENTRY	SeekTop( EXCEPTION_POINTERS* pExceptionPtr, HANDLE hProcess, HANDLE hThread );

private:

	//////////////////////////////////////////////////////////////////////////
	class	FLStackWalkContext
	{
	public:

		DWORD					dwMachineType;
		STACKFRAME64			tStackFrame;
		CONTEXT					tContext;

		FLStackWalkReporter*	pReporter;
		HANDLE					hProcess;
		HANDLE					hThread;
		DWORD					dwWalkCount;

		bool	Initialize( EXCEPTION_POINTERS* pExceptionPtr, FLStackWalkReporter* pReporter, HANDLE hProcess, HANDLE hThread );
	};
	//////////////////////////////////////////////////////////////////////////

	bool	LoadFunctions( HANDLE hProcess );
	void	UnloadFunction( HANDLE hProcess );

	bool	DoStep( FLStackWalkContext* pContext );
	bool	GetStackEntry( FLStackWalkContext* pContext, T_CALL_STACK_ENTRY* pEntry );

private:

	typedef BOOL	(WINAPI *PSYM_INITIALIZE_F)( HANDLE hProcess, PSTR UserSearchPath, BOOL fInvadeProcess );
	typedef BOOL	(WINAPI *PSYM_CLEANUP_F)( HANDLE hProcess );

	typedef DWORD	(WINAPI *PSYM_GET_OPTIONS_F)( VOID );
	typedef DWORD	(WINAPI *PSYM_SET_OPTIONS_F)( DWORD SymOptions );

	typedef BOOL	(WINAPI *PSTACK_WALK64_F)( DWORD dwMachineType, HANDLE hProcess, HANDLE hThread, LPSTACKFRAME64 StackFrame, PVOID ContextRecord,
											PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine,
											PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine,
											PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine,
											PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress );
	typedef PVOID	(WINAPI *PSYM_FUNCTION_TABLE_ACCESS64_F)( HANDLE hProcess, DWORD64 AddrBase );
	typedef DWORD64	(WINAPI *PSYM_GET_MODULE_BASE64_F)( HANDLE hProcess, DWORD64 dwAddr );

	typedef BOOL	(WINAPI *PSYM_GET_FROM_ADDR64_F)( HANDLE hProcess, DWORD64 dwAddr, PDWORD64 pdwDisplacement, PIMAGEHLP_SYMBOL64 Symbol );
	typedef BOOL	(WINAPI *PSYM_GET_LINE_FROM_ADDR64_F)( HANDLE hProcess, DWORD64 dwAddr, PDWORD pdwDisplacement, PIMAGEHLP_LINE64 Line );

	HMODULE							m_hDbgHelpDll;

	PSYM_INITIALIZE_F				m_pSymInitialize;
	PSYM_CLEANUP_F					m_pSymCleanup;

	PSYM_GET_OPTIONS_F				m_pSymGetOptions;
	PSYM_SET_OPTIONS_F				m_pSymSetOptions;

	PSTACK_WALK64_F					m_pStackWalk64;
	PSYM_FUNCTION_TABLE_ACCESS64_F	m_pSymFunctionTableAccess64;
	PSYM_GET_MODULE_BASE64_F		m_pSymGetModuleBase64;

	PSYM_GET_FROM_ADDR64_F			m_pSymGetFromAddr64;
	PSYM_GET_LINE_FROM_ADDR64_F		m_pSymGetLineFromAddr64;

};


#endif