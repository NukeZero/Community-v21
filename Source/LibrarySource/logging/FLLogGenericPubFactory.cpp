
#include "FLLogGenericPubFactory.h"
#include "FLLogFormatEtc.h"

#include <shlwapi.h>

#include "../common/FLStringFunction.h"

#pragma comment( lib, "shlwapi.lib" )


FLLogPubGeneric::FLLogPubGeneric()
{
	m_pFormater = NULL;
}

FLLogPubGeneric::~FLLogPubGeneric()
{
	Destroy();
}

bool	FLLogPubGeneric::Create( const TCHAR* szName, const TCHAR* szFolder, const TCHAR* szFormat, size_t nMaxLogFileSize )
{
	if( szName == NULL || szFormat == NULL )
	{
		return false;
	}

	if( ::_tcsicmp( szFormat, _T( "Generic" ) ) == 0 )
	{
		m_pFormater = new FLLogGenericFormat();
	}
	else if( ::_tcsicmp( szFormat, _T( "CSV" ) ) == 0 )
	{
		m_pFormater = new FLLogCSVFormat();
	}

	if( m_pFormater == NULL )
	{
		return false;
	}

	if( m_cLogFileName.SetInfo( szFolder, szName ) == false )
	{
		return false;
	}

	if( m_cPubFile.Create( &m_cLogFileName, m_pFormater, nMaxLogFileSize ) == false )
	{
		return false;
	}

	return true;
}

void	FLLogPubGeneric::Destroy()
{
	m_cPubFile.Destroy();

	SAFE_DELETE( m_pFormater );
}

void	FLLogPubGeneric::DoPublish( const FLLogEvent* pEvent )
{
#ifdef	_DEBUG
	m_cPubDebugString.Publish( pEvent );
#endif
	m_cPubFile.Publish( pEvent );
}

//////////////////////////////////////////////////////////////////////////

FLLogGenericPubFactory::FLLogGenericPubFactory()
{
	FLStrcpy( m_szFormat, _countof( m_szFormat ), _T( "Generic" ) );

	TCHAR szModuleFullPath[MAX_PATH] = { 0, };
	::GetModuleFileName( NULL, szModuleFullPath, _countof( szModuleFullPath ) );
	::PathRemoveFileSpec( szModuleFullPath );
	::PathAppend( szModuleFullPath, _T( "\\Log" ) );
	FLStrcpy( m_szFolder, _countof( m_szFolder ), szModuleFullPath );

	m_nMaxLogFileSize = MAX_LOGFILE_SIZE;
}

FLLogGenericPubFactory::~FLLogGenericPubFactory()
{
}

FLLogPublisher*	FLLogGenericPubFactory::DoCreate( const TCHAR* szName )
{
	FLLogPubGeneric* pPublisher = new FLLogPubGeneric();
	if( pPublisher != NULL )
	{
		if( pPublisher->Create( szName, m_szFolder, m_szFormat, m_nMaxLogFileSize ) == true )
		{
			return pPublisher;
		}

		SAFE_DELETE( pPublisher );
	}

	return NULL;
}

void	FLLogGenericPubFactory::DoDestroy( FLLogPublisher* pPublisher )
{
	SAFE_DELETE( pPublisher );
}

void	FLLogGenericPubFactory::SetFormater( const TCHAR* szFormat )
{
	FLStrcpy( m_szFormat, _countof( m_szFormat ), szFormat );
}

void	FLLogGenericPubFactory::SetFolder( const TCHAR* szFolder )
{
	FLVERIFY( ::GetFullPathName( szFolder, _countof( m_szFolder ), m_szFolder, NULL ) != 0 );
}

void	FLLogGenericPubFactory::SetMaxLogFileSize( size_t nMaxLogFileSize )
{
	m_nMaxLogFileSize = nMaxLogFileSize;
}

