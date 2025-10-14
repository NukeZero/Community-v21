
#ifndef	__FLDUMPPARAM_H__
#define	__FLDUMPPARAM_H__


#include "FLDumpCommon.h"


struct	T_DUMP_ASSERT_DATA
{
	TCHAR	szExpression[256];
	TCHAR	szFunction[128];
	TCHAR	szFile[MAX_PATH];
	int		nLine;
};

struct	T_DUMP_PARAM
{
	int							nLevel;

	HANDLE						hProcess;
	DWORD						dwProcessID;
	HANDLE						hThread;
	DWORD						dwThreadID;

	bool						bOutOfProcess;
	EXCEPTION_POINTERS*			pException;
	const T_DUMP_ASSERT_DATA*	pAssertData;

};


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
						unsigned int nLine );



#endif