
#include "FLDumpParam.h"
#include "../common/FLStringFunction.h"


void	FLMakeDumpParam( T_DUMP_PARAM* pParam,
						T_DUMP_ASSERT_DATA* pAssertData,
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
	pParam->nLevel = nLevel;
	pParam->hProcess = hProcess;
	pParam->dwProcessID = dwProcessID;
	pParam->hThread = hThread;
	pParam->dwThreadID = dwThreadID;

	pParam->bOutOfProcess = bOutOfProcess;
	pParam->pException = pException;
	pParam->pAssertData = NULL;

	if( szExpression != NULL || szFunction != NULL || szFile != NULL || nLine >= 0 )
	{
		if( szExpression != NULL )
		{
			FLStrcpy( pAssertData->szExpression, _countof( pAssertData->szExpression ), szExpression );
		}
		if( szFunction != NULL )
		{
			FLStrcpy( pAssertData->szFunction, _countof( pAssertData->szFunction ), szFunction );
		}
		if( szFile != NULL )
		{
			FLStrcpy( pAssertData->szFile, _countof( pAssertData->szFile ), szFile );
		}

		pAssertData->nLine = nLine;
		pParam->pAssertData = pAssertData;
	}
}
