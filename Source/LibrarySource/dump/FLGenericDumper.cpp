
#include "FLGenericDumper.h"
#include "FLDumpMini.h"
#include "FLDumpLog.h"
#include "../bugtrap/FLBugTrapper.h"

#include "../common/FLStringFunction.h"

#include <shlwapi.h>
#pragma comment( lib, "shlwapi.lib" )


FLGenericDumper*	FLGenericDumper::GetInstance()
{
	static FLGenericDumper cGenericDumper;
	return &cGenericDumper;
}

FLGenericDumper::FLGenericDumper()
{
	::memset( m_szDumpFolder, 0, sizeof( m_szDumpFolder ) );
	::memset( m_szPrefix, 0, sizeof( m_szPrefix ) );
}

void	FLGenericDumper::SetDumpFolder( const TCHAR* szDumpFolder )
{
	if( szDumpFolder != NULL )
	{
		FLStrcpy( m_szDumpFolder, _countof( m_szDumpFolder ), szDumpFolder );
	}
}

const TCHAR*	FLGenericDumper::GetDumpFolder()	const
{
	if( FLStrlen( m_szDumpFolder, _countof( m_szDumpFolder ) ) > 0 )
	{
		return m_szDumpFolder;
	}

	return NULL;
}

void	FLGenericDumper::SetPrefix( const TCHAR* szPrefix )
{
	if( szPrefix != NULL )
	{
		FLStrcpy( m_szPrefix, _countof( m_szPrefix ), szPrefix );
	}
}

const TCHAR*	FLGenericDumper::GetPrefix()	const
{
	if( FLStrlen( m_szPrefix, _countof( m_szPrefix ) ) > 0 )
	{
		return m_szPrefix;
	}

	return NULL;
}

bool	FLGenericDumper::DoDump( T_DUMP_PARAM* pParam )
{
	bool bStopSearch = true;

	FLDumpMini cMinidump( GetDumpFolder(), GetPrefix() );
	if( cMinidump.Dump( pParam ) == false )
	{
		bStopSearch = false;
	}

	FLDumpLog cDumpLog( GetDumpFolder(), GetPrefix() );
	if( cDumpLog.Dump( pParam ) == false )
	{
		bStopSearch = false;
	}

	//////////////////////////////////////////////////////////////////////////
	TCHAR szLogFolder[MAX_PATH] = { 0, };
	::GetCurrentDirectory( _countof( szLogFolder ), szLogFolder );
	::PathCombine( szLogFolder, szLogFolder, _T( "Log" ) );
	FLBugTrapper::AddFile( szLogFolder, 7 );

	FLBugTrapper::SendToBugTrapServer();
	//////////////////////////////////////////////////////////////////////////

	return bStopSearch;
}
