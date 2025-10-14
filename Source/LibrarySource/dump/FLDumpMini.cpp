
#include "FLDumpMini.h"
#include "FLMiniDump.h"
#include "../bugtrap/FLBugTrapper.h"


FLDumpMini::FLDumpMini( const TCHAR* szDumpFolder, const TCHAR* szPrefix )
	: m_szDumpFolder( szDumpFolder )
	, m_szPrefix( szPrefix )
{
}

bool	FLDumpMini::DoDump( T_DUMP_PARAM* pParam )
{
	bool bStopSearch = false;
	if( pParam != NULL )
	{
		TCHAR szDumpFileName[MAX_PATH] = { 0, };
		FLMakeFileNameByProcess( szDumpFileName, _countof( szDumpFileName ), pParam->hProcess, m_szDumpFolder, m_szPrefix, _T( "dmp" ) );

		int nDumpType = MiniDumpNormal;

		HANDLE hDumpFile = ::CreateFile( szDumpFileName,
										GENERIC_WRITE,
										0,
										NULL,
										CREATE_ALWAYS,
										FILE_ATTRIBUTE_NORMAL,
										NULL );
		if( hDumpFile != INVALID_HANDLE_VALUE )
		{
			if( pParam->nLevel == FLDUMP_LEVEL_LIGHT )
			{
				nDumpType |=	MiniDumpWithIndirectlyReferencedMemory |
								MiniDumpScanMemory;
			}
			else if( pParam->nLevel == FLDUMP_LEVEL_MEDIUM )
			{
				nDumpType |=	MiniDumpWithPrivateReadWriteMemory |
								MiniDumpWithDataSegs |
								MiniDumpWithFullMemoryInfo |
								MiniDumpWithHandleData |
								MiniDumpWithThreadInfo |
								MiniDumpWithUnloadedModules;
			}
			else if( pParam->nLevel == FLDUMP_LEVEL_HEAVY )
			{
				nDumpType |=	MiniDumpWithFullMemory |
								MiniDumpWithFullMemoryInfo |
								MiniDumpWithHandleData |
								MiniDumpWithThreadInfo |
								MiniDumpWithUnloadedModules;
			}

			FLMiniDump kMiniDump;
			bStopSearch = kMiniDump.Write( hDumpFile,
										static_cast< MINIDUMP_TYPE >( nDumpType ),
										pParam->hProcess,
										pParam->dwProcessID,
										pParam->dwThreadID,
										pParam->pException,
										pParam->bOutOfProcess );

			::CloseHandle( hDumpFile );
			hDumpFile = INVALID_HANDLE_VALUE;
		}

		//////////////////////////////////////////////////////////////////////////
		FLBugTrapper::AddFile( szDumpFileName );
		//////////////////////////////////////////////////////////////////////////
	}

	return bStopSearch;
}
