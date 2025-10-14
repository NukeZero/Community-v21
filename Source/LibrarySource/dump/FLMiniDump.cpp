
#include "FLMiniDump.h"

#include <algorithm>

#include "../common/FLStringFunction.h"


namespace
{
	struct	T_DUMP_MINIDUMP_CALLBACK_CONTEXT
	{
		ULONG64	n64MemoryBase;
		ULONG	dwMemorySize;
		bool	bFinished;
	};

	BOOL CALLBACK	FLMinidumpWriteDumpCallback( PVOID pParam, const PMINIDUMP_CALLBACK_INPUT pCallbackInput, PMINIDUMP_CALLBACK_OUTPUT pCallbackOutput )
	{
		if( pCallbackInput == NULL || pCallbackOutput == NULL )
		{
			return FALSE;
		}

		switch( pCallbackInput->CallbackType )
		{
		case MemoryCallback:
			{
				T_DUMP_MINIDUMP_CALLBACK_CONTEXT* pCallbackContext = reinterpret_cast< T_DUMP_MINIDUMP_CALLBACK_CONTEXT* >( pParam );
				if( pCallbackContext == NULL || pCallbackContext->bFinished )
				{
					return FALSE;
				}

				// Include the specified memory region.
				pCallbackOutput->MemoryBase = pCallbackContext->n64MemoryBase;
				pCallbackOutput->MemorySize = pCallbackContext->dwMemorySize;

				pCallbackContext->bFinished = true;
				return TRUE;
			}
			break;

		// Include all modules.
		case IncludeModuleCallback:
		case ModuleCallback:
			{
				return TRUE;
			}
			break;

		// Include all threads.
		case IncludeThreadCallback:
		case ThreadCallback:
		case ThreadExCallback:
			{
				return TRUE;
			}
			break;

		// Stop receiving cancel callbacks.
		case CancelCallback:
			{
				pCallbackOutput->CheckCancel = FALSE;
				pCallbackOutput->Cancel = FALSE;
				return TRUE;
			}
			break;
		}

		// Ignore other callback types.
		return FALSE;
	}

}

bool	FLMiniDump::Write( HANDLE hDumpFile,
						  MINIDUMP_TYPE nType,
						  HANDLE hProcess,
						  DWORD dwProcessID,
						  DWORD dwThreadID,
						  EXCEPTION_POINTERS* pException,
						  bool bOutOfProcess )
{
	bool bResult = false;
	DWORD dwMiniDumpError = 0;
	bResult = CallMiniDump( hDumpFile,
							nType,
							hProcess,
							dwProcessID,
							dwThreadID,
							pException,
							bOutOfProcess,
							&dwMiniDumpError );

	if( bResult == false )
	{
		TCHAR szErrorMsg[256] = { 0, };
		FLSPrintf( szErrorMsg, _countof( szErrorMsg ), _T( "MiniDump failed(%u)." ), dwMiniDumpError );

		DWORD dwWrittenSize = 0;
		::WriteFile( hDumpFile, szErrorMsg, static_cast< DWORD >( FLStrlen( szErrorMsg, _countof( szErrorMsg ) ) * sizeof( TCHAR ) ), &dwWrittenSize, NULL );
	}

	return bResult;
}

bool	FLMiniDump::CallMiniDump( HANDLE hDumpFile,
								MINIDUMP_TYPE nType,
								HANDLE hProcess,
								DWORD dwProcessID,
								DWORD dwThreadID,
								EXCEPTION_POINTERS* pException,
								bool bOutOfProcess,
								DWORD* pError )
{
	typedef	BOOL (WINAPI * PMINI_DUMP_WRITE_DUMP_F)(
			HANDLE, DWORD, HANDLE, MINIDUMP_TYPE, 
			const PMINIDUMP_EXCEPTION_INFORMATION,
			const PMINIDUMP_USER_STREAM_INFORMATION,
			const PMINIDUMP_CALLBACK_INFORMATION
		);

	bool bResult = false;
	HMODULE hDbgHelp = FLLoadDll( hProcess, _T( "dbghelp.dll" ) );
	if( hDbgHelp != NULL )
	{
		PMINI_DUMP_WRITE_DUMP_F pMiniDumpWriteDump = reinterpret_cast< PMINI_DUMP_WRITE_DUMP_F >( ::GetProcAddress( hDbgHelp, "MiniDumpWriteDump" ) );
		if( pMiniDumpWriteDump != NULL )
		{
			MINIDUMP_EXCEPTION_INFORMATION tInfo;
			tInfo.ThreadId			= dwThreadID;
			tInfo.ExceptionPointers	= pException;
			tInfo.ClientPointers	= FALSE;

			MINIDUMP_CALLBACK_INFORMATION tCallback;
			T_DUMP_MINIDUMP_CALLBACK_CONTEXT tContext;
			MINIDUMP_CALLBACK_INFORMATION* pCallbackPointer = NULL;

			//from google-breakpad
			if( bOutOfProcess == false )
			{
				// Older versions of DbgHelp.dll don't correctly put the memory around
				// the faulting instruction pointer into the minidump. This
				// callback will ensure that it gets included.
				if( pException != NULL )
				{
					// Find a memory region of 256 bytes centered on the
					// faulting instruction pointer.
					const ULONG64 n64InstructionPointer = 
#if defined(_M_IX86)
					pException->ContextRecord->Eip;
#elif defined(_M_AMD64)
					pException->ContextRecord->Rip;
#else
#error				CPU architecture is not supported.
#endif

					MEMORY_BASIC_INFORMATION tMemoryBasicInfo;
					if( ::VirtualQuery( reinterpret_cast< LPCVOID >( n64InstructionPointer ), &tMemoryBasicInfo, sizeof( MEMORY_BASIC_INFORMATION )) != 0 &&
						tMemoryBasicInfo.State == MEM_COMMIT )
					{
						// Attempt to get 128 bytes before and after the instruction
						// pointer, but settle for whatever's available up to the
						// boundaries of the memory region.
						const ULONG64 n64IPMemorySize = 256;
						tContext.n64MemoryBase = (std::max)( reinterpret_cast< ULONG64 >( tMemoryBasicInfo.BaseAddress ), n64InstructionPointer - (n64IPMemorySize / 2) );
						ULONG64 n64EndOfRange = (std::min)( n64InstructionPointer + (n64IPMemorySize / 2), reinterpret_cast< ULONG64 >( tMemoryBasicInfo.BaseAddress ) + tMemoryBasicInfo.RegionSize );
						tContext.dwMemorySize = static_cast< ULONG >( n64EndOfRange - tContext.n64MemoryBase );

						tContext.bFinished = false;

						tCallback.CallbackRoutine = FLMinidumpWriteDumpCallback;
						tCallback.CallbackParam = reinterpret_cast< void* >( &tContext );
						pCallbackPointer = &tCallback;
					}
				}
			}

			bResult = (pMiniDumpWriteDump( hProcess,
										dwProcessID,
										hDumpFile,
										nType,
										pException ? &tInfo : NULL,
										NULL,
										pCallbackPointer ) != FALSE);

			if( bResult == false && pError != NULL )
			{
				*pError = ::GetLastError();
			}
		}

		::FreeLibrary( hDbgHelp );
		hDbgHelp = NULL;
	}

	return bResult;
}
