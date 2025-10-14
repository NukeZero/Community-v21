
#ifndef	__FLDUMPRUNNER_H__
#define	__FLDUMPRUNNER_H__


#include "FLDumpCommon.h"
#include "FLDumper.h"
#include "FLGenericDumper.h"


class	FLDumpRunner
{
public:

	static bool	DumpNow( FLDumper* pDumper,
						int nLevel,
						DWORD dwExceptionCode,
						const TCHAR* szExpression,
						const TCHAR* szFunction,
						const TCHAR* szFile,
						unsigned int nLine );


	static bool	DumpProcess( FLDumper* pDumper, int nLevel, const TCHAR* szProcessName );
	static bool	DumpProcess( FLDumper* pDumper, int nLevel, DWORD dwProcessID );
	static bool	DumpProcess( FLDumper* pDumper, int nLevel, DWORD dwExceptionCode, HANDLE hProcess, DWORD dwProcessID, HANDLE hThread, DWORD dwThreadID );

	static bool	Execute( FLDumper* pDumper,
						int nLevel,
						HANDLE hProcess,
						DWORD dwProcessID,
						HANDLE hThread,
						DWORD dwThreadID,
						EXCEPTION_POINTERS* pException,
						const TCHAR* szExpression,
						const TCHAR* szFunction,
						const TCHAR* szFile,
						unsigned int nLine );

private:

	static bool	DoExecute( FLDumper* pDumper,
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

public:

	static FLDumpRunner*	GetInstance();

private:

	FLDumpRunner();
	~FLDumpRunner();

public:

	bool	Start();
	void	Stop();

	bool	DoExecuteThread( FLDumper* pDumper,
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
							unsigned int nLine,
							bool* pStopSearch );

private:

	bool	DoStart();
	void	DoStop();

	bool	IsDumpable()	const;

	void	Run();

	void	SetData( FLDumper* pDumper,
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

	void	ClearData();

	static unsigned int __stdcall	DumpThreadFunction( void* pArg );

private:

	HANDLE				m_hThread;

	volatile bool		m_bShutdown;

	HANDLE				m_hStart;
	HANDLE				m_hFinish;

	CRITICAL_SECTION	m_tLock;

	FLDumper*			m_pDumper;

	bool				m_bStopSearch;
	T_DUMP_PARAM		m_tParam;
	T_DUMP_ASSERT_DATA	m_tAssert;

};




#define	FLDUMP_ASSERT_IF( Condition )	\
	if( !(Condition) )	\
	{	\
		FLDumpRunner::DumpNow( FLGenericDumper::GetInstance(),	\
							FLDUMP_LEVEL_LIGHT,	\
							EXCEPTION_NONCONTINUABLE_EXCEPTION,	\
							_T( #Condition ),	\
							_T( __FUNCTION__ ),	\
							_T( __FILE__ ),	\
							__LINE__ );	\
	}









#endif