#ifndef __FLDUMPLOG_H__
#define __FLDUMPLOG_H__


#include "FLDumpCommon.h"
#include "FLDumper.h"


class	FLDumpLog : public FLDumper
{
public:

	FLDumpLog( const TCHAR* szDumpFolder, const TCHAR* szPrefix );

private:

	bool	DoDump( T_DUMP_PARAM* pParam );

	void	Log( FLTextFile* pFile, T_DUMP_PARAM* pParam );

	void	HeaderLog( FLTextFile* pFile );

	void	AssertionFailedLog( FLTextFile* pFile, T_DUMP_PARAM* pParam );
	void	ExceptionInformationLog( FLTextFile* pFile, T_DUMP_PARAM* pParam );
	void	ProgramInformationLog( FLTextFile* pFile, T_DUMP_PARAM* pParam );
	void	SystemInformationLog( FLTextFile* pFile, T_DUMP_PARAM* pParam );
	void	RegisterLog( FLTextFile* pFile, T_DUMP_PARAM* pParam );
	void	ThreadsCallStackLog( FLTextFile* pFile, T_DUMP_PARAM* pParam );
	void	ModuleListLog( FLTextFile* pFile, T_DUMP_PARAM* pParam );
	void	ProcessesInformationLog( FLTextFile* pFile, T_DUMP_PARAM* pParam );
	void	PrintModuleInfo( FLTextFile* pFile, const TCHAR* szModulePath );

	void	FooterLog( FLTextFile* pFile );

private:

	const TCHAR*	m_szDumpFolder;
	const TCHAR*	m_szPrefix;

};


#endif
