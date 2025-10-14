
#ifndef	__FLMINIDUMP_H__
#define	__FLMINIDUMP_H__


#include "FLDumpCommon.h"

#include <dbghelp.h>


class	FLMiniDump
{
public:

	bool	Write( HANDLE hDumpFile,
				MINIDUMP_TYPE nType,
				HANDLE hProcess,
				DWORD dwProcessID,
				DWORD dwThreadID,
				EXCEPTION_POINTERS* pException,
				bool bOutOfProcess );

private:

	bool	CallMiniDump( HANDLE hDumpFile,
						MINIDUMP_TYPE nType,
						HANDLE hProcess,
						DWORD dwProcessID,
						DWORD dwThreadID,
						EXCEPTION_POINTERS* pException,
						bool bOutOfProcess,
						DWORD* pError );

};






#endif