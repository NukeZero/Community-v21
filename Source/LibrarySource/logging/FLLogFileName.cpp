
#include "FLLogFileName.h"


//////////////////////////////////////////////////////////////////////////

FLLogFileName::~FLLogFileName()
{
}

bool	FLLogFileName::Generate( TCHAR* szFileName, size_t cchFileName, DWORD dwTryCount )
{
	return DoGenerate( szFileName, cchFileName, dwTryCount );
}

//////////////////////////////////////////////////////////////////////////
#include "../common/FLFileUtil.h"
#include "../common/FLStringFunction.h"

FLLogFixedFileName::FLLogFixedFileName()
{
	::memset( m_szFolder, 0, sizeof( m_szFolder ) );
	::memset( m_szFileName, 0, sizeof( m_szFileName ) );
}

FLLogFixedFileName::~FLLogFixedFileName()
{
	::memset( m_szFolder, 0, sizeof( m_szFolder ) );
	::memset( m_szFileName, 0, sizeof( m_szFileName ) );
}

bool	FLLogFixedFileName::SetInfo( const TCHAR* szFolder, const TCHAR* szFixedName )
{
	if( szFixedName == NULL )
	{
		return false;
	}

	if( szFolder != NULL )
	{
		FLStrcpy( m_szFolder, _countof( m_szFolder ), szFolder );
	}

	FLStrcpy( m_szFileName, _countof( m_szFileName ), szFixedName );

	if( FLCreateFolder( m_szFolder ) == false )
	{
		return false;
	}

	return true;
}

bool	FLLogFixedFileName::DoGenerate( TCHAR* szFileName, size_t cchFileName, DWORD dwTryCount )
{
	if( szFileName != NULL && cchFileName > 0 )
	{
		if( dwTryCount == 0 )
		{
			FLSPrintf( szFileName, cchFileName, _T( "%s/%s" ), m_szFolder, m_szFileName );
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

FLLogDateFileName::FLLogDateFileName()
{
	::memset( m_szFolder, 0, sizeof( m_szFolder ) );
	::memset( m_szPrefix, 0, sizeof( m_szPrefix ) );
}

FLLogDateFileName::~FLLogDateFileName()
{
	::memset( m_szFolder, 0, sizeof( m_szFolder ) );
	::memset( m_szPrefix, 0, sizeof( m_szPrefix ) );
}

bool	FLLogDateFileName::SetInfo( const TCHAR* szFolder, const TCHAR* szPrefix )
{
	if( szFolder != NULL )
	{
		FLStrcpy( m_szFolder, _countof( m_szFolder ), szFolder );
	}

	if( szPrefix != NULL )
	{
		FLStrcpy( m_szPrefix, _countof( m_szPrefix ), szPrefix );
	}

	if( FLCreateFolder( m_szFolder ) == false )
	{
		return false;
	}

	return true;
}

bool	FLLogDateFileName::DoGenerate( TCHAR* szFileName, size_t cchFileName, DWORD dwTryCount )
{
	if( szFileName != NULL && cchFileName > 0 )
	{
		SYSTEMTIME tNow;
		::memset( &tNow, 0, sizeof( tNow ) );
		::GetLocalTime( &tNow );

		if( dwTryCount == 0 )
		{
			FLSPrintf( szFileName, cchFileName, _T( "%s/%s_%04d-%02d-%02d.log" ),
				(::_tcslen( m_szFolder ) > 0 ? m_szFolder : _T( "." )),
				(::_tcslen( m_szPrefix ) > 0 ? m_szPrefix : _T( "log" )),
				tNow.wYear, tNow.wMonth, tNow.wDay );
		}
		else
		{
			FLSPrintf( szFileName, cchFileName, _T( "%s/%s_%04d-%02d-%02d(%u).log" ),
				(::_tcslen( m_szFolder ) > 0 ? m_szFolder : _T( "." )),
				(::_tcslen( m_szPrefix ) > 0 ? m_szPrefix : _T( "log" )),
				tNow.wYear, tNow.wMonth, tNow.wDay,
				dwTryCount );
		}

		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////


FLLogDateTimeFileName::FLLogDateTimeFileName()
{
	::memset( m_szFolder, 0, sizeof( m_szFolder ) );
	::memset( m_szPrefix, 0, sizeof( m_szPrefix ) );
}

FLLogDateTimeFileName::~FLLogDateTimeFileName()
{
	::memset( m_szFolder, 0, sizeof( m_szFolder ) );
	::memset( m_szPrefix, 0, sizeof( m_szPrefix ) );
}

bool	FLLogDateTimeFileName::SetInfo( const TCHAR* szFolder, const TCHAR* szPrefix )
{
	if( szFolder != NULL )
	{
		FLStrcpy( m_szFolder, _countof( m_szFolder ), szFolder );
	}

	if( szPrefix != NULL )
	{
		FLStrcpy( m_szPrefix, _countof( m_szPrefix ), szPrefix );
	}

	if( FLCreateFolder( m_szFolder ) == false )
	{
		return false;
	}

	return true;
}

bool	FLLogDateTimeFileName::DoGenerate( TCHAR* szFileName, size_t cchFileName, DWORD dwTryCount )
{
	if( szFileName != NULL && cchFileName > 0 )
	{
		SYSTEMTIME tNow;
		::memset( &tNow, 0, sizeof( tNow ) );
		::GetLocalTime( &tNow );

		if( dwTryCount == 0 )
		{
			FLSPrintf( szFileName, cchFileName, _T( "%s/%s_%04d-%02d-%02d_%02d-%02d-%02d.log" ),
				(::_tcslen( m_szFolder ) > 0 ? m_szFolder : _T( "." )),
				(::_tcslen( m_szPrefix ) > 0 ? m_szPrefix : _T( "log" )),
				tNow.wYear, tNow.wMonth, tNow.wDay,
				tNow.wHour, tNow.wMinute, tNow.wSecond );
		}
		else
		{
			FLSPrintf( szFileName, cchFileName, _T( "%s/%s_%04d-%02d-%02d_%02d-%02d-%02d(%u).log" ),
				(::_tcslen( m_szFolder ) > 0 ? m_szFolder : _T( "." )),
				(::_tcslen( m_szPrefix ) > 0 ? m_szPrefix : _T( "log" )),
				tNow.wYear, tNow.wMonth, tNow.wDay,
				tNow.wHour, tNow.wMinute, tNow.wSecond,
				dwTryCount );
		}

		return true;
	}

	return false;
}
