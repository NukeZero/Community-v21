
#include "FLModuleInfo.h"
#include "FLStringFunction.h"

#include <cstdlib>


FLModuleInfo::FLModuleInfo()
{
	::memset( m_wFileVersion, 0, sizeof( m_wFileVersion ) );
	::memset( m_wProductVersion, 0, sizeof( m_wProductVersion ) );

	::memset( m_szCompanyName, 0, sizeof( m_szCompanyName ) );
	::memset( m_szProductName, 0, sizeof( m_szProductName ) );
	::memset( m_szFileDescription, 0, sizeof( m_szFileDescription ) );
}

bool	FLModuleInfo::Load( const TCHAR* szPath )
{
	if( szPath == NULL )
	{
		return	false;
	}

	DWORD dwHandle = 0;
	DWORD dwInfoSize = 0;
	dwInfoSize = ::GetFileVersionInfoSize( szPath, &dwHandle );
	if( dwInfoSize == 0 )
	{
		return	false;
	}

	unsigned char* pData = reinterpret_cast< unsigned char * >( ::malloc( dwInfoSize+1 ) );
	if( pData == NULL )
	{
		return false;
	}

	::memset( pData, 0, dwInfoSize+1 );
	if( ::GetFileVersionInfo( szPath, dwHandle, dwInfoSize, pData ) != FALSE )
	{
		if( Query( pData ) == true )
		{
			::free( pData );
			pData = NULL;
			return true;
		}
	}

	::free( pData );
	pData = NULL;
	return false;
}

const TCHAR*	FLModuleInfo::CompanyName()		const
{
	return	m_szCompanyName;
}

const TCHAR*	FLModuleInfo::ProductName()		const
{
	return	m_szProductName;
}

const TCHAR*	FLModuleInfo::FileDescription()	const
{
	return	m_szFileDescription;
}

const WORD*	FLModuleInfo::FileVersion()		const
{
	return	m_wFileVersion;
}

const WORD*	FLModuleInfo::ProductVersion()	const
{
	return	m_wProductVersion;
}

bool	FLModuleInfo::Query( void* pData )
{
	VS_FIXEDFILEINFO tFixedInfo;
	if( QueryFixedInfo( pData, &tFixedInfo ) == false )
	{
		return	false;
	}

	m_wFileVersion[0]	= HIWORD( tFixedInfo.dwFileVersionMS );
	m_wFileVersion[1]	= LOWORD( tFixedInfo.dwFileVersionMS );
	m_wFileVersion[2]	= HIWORD( tFixedInfo.dwFileVersionLS );
	m_wFileVersion[3]	= LOWORD( tFixedInfo.dwFileVersionLS );

	m_wProductVersion[0]	= HIWORD( tFixedInfo.dwProductVersionMS );
	m_wProductVersion[1]	= LOWORD( tFixedInfo.dwProductVersionMS );
	m_wProductVersion[2]	= HIWORD( tFixedInfo.dwProductVersionLS );
	m_wProductVersion[3]	= LOWORD( tFixedInfo.dwProductVersionLS );

	// get string info
	if( QueryStringInfo( pData, _T( "CompanyName" ), m_szCompanyName, _countof( m_szCompanyName ) ) == false )
	{
		return false;
	}

	if( QueryStringInfo( pData, _T( "FileDescription" ), m_szProductName, _countof( m_szProductName ) ) == false )
	{
		return false;
	}

	if( QueryStringInfo( pData, _T( "ProductName" ), m_szFileDescription, _countof( m_szFileDescription ) ) == false )
	{
		return false;
	}

	return true;
}

bool	FLModuleInfo::QueryFixedInfo( void* pData, VS_FIXEDFILEINFO* pFixedInfo )
{
	if( pData == NULL || pFixedInfo == NULL )
	{
		return false;
	}

	UINT nLen = 0;
	VS_FIXEDFILEINFO* pTempInfo = NULL;

	if( ::VerQueryValue( pData, _T( "\\" ), reinterpret_cast< void** >( &pTempInfo ), &nLen ) == FALSE )
	{
		return false;
	}

	if( nLen == sizeof( VS_FIXEDFILEINFO ) )
	{
		::memcpy( pFixedInfo, pTempInfo, nLen );
	}

	return true;
}

bool	FLModuleInfo::QueryStringInfo( void* pData, const TCHAR* szKey, TCHAR* szValue, size_t cchCount )
{
	if( pData == NULL || szKey == NULL || szValue == NULL || cchCount <= 0 )
	{
		return false;
	}

	DWORD* pTranslation = NULL;
	UINT nLen = 0;

	if( ::VerQueryValue( pData, _T( "\\VarFileInfo\\Translation" ), reinterpret_cast< void** >( &pTranslation ), &nLen ) == FALSE )
	{
		return false;
	}

	TCHAR* szTempValue = NULL;
	TCHAR szFullKey[1024] = { 0, };
	FLSPrintf( szFullKey, _countof( szFullKey ), _T( "\\StringFileInfo\\%04x%04x\\%s" ), LOWORD( *pTranslation ), HIWORD( *pTranslation ), szKey );
	if( ::VerQueryValue( pData, szFullKey, reinterpret_cast< void** >( &szTempValue ), &nLen ) == FALSE )
	{
		return false;
	}

	if( cchCount >= nLen )
	{
		if( nLen != 0 )
		{
			FLStrcpy( szValue, cchCount, szTempValue );
		}
		else
		{
			szValue[0] = _T( '\0' );
		}
	}
	else
	{
		return false;
	}

	return true;
}