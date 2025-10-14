#ifndef __FLDUMPCOMMON_H__
#define __FLDUMPCOMMON_H__


#include "../common/FLCommon.h"
#include "../common/FLTextFile.h"

enum
{
	FLDUMP_LEVEL_NONE
	, FLDUMP_LEVEL_LIGHT
	, FLDUMP_LEVEL_MEDIUM
	, FLDUMP_LEVEL_HEAVY
};


HMODULE	FLLoadDll( HANDLE hProcess, const TCHAR* szDll );

void	FLGetProcessFolder( HANDLE hProcess, TCHAR* szOutput, size_t cchOutput );
void	FLGetProcessName( HANDLE hProcess, TCHAR* szOutput, size_t cchOutput );
void	FLGetProcessFileName( HANDLE hProcess, TCHAR* szOutput, size_t cchOutput );

void	FLMakeFileName( TCHAR* szOutput, size_t cchOutput, const TCHAR* szName, const TCHAR* szDestFolder, const TCHAR* szPrefix, const TCHAR* szExt );
void	FLMakeFileNameByProcess( TCHAR* szOutput, size_t cchOutput, HANDLE hProcess, const TCHAR* szDestFolder, const TCHAR* szPrefix, const TCHAR* szExt );


void	FLDumpPrint( FLTextFile* pFile, const TCHAR* szFormat, ... );
void	FLPrintThreadCallStack( FLTextFile* pFile, PEXCEPTION_POINTERS pException, HANDLE hProcess, HANDLE hThread );

void	FLPrintCurrentThreadCallStack( FLTextFile* pFile, const TCHAR* szComment );
void	FLPrintCallStackHelper( const TCHAR* szName, const TCHAR* szExpression, const TCHAR* szFunction, const TCHAR* szFile, int nLine );


#endif
