#include "StdAfx.h"

#include "FLIniFile.h"

FLIniFile::FLIniFile()
{
	::memset( m_szFilePath, 0, sizeof(m_szFilePath) );
}

FLIniFile::~FLIniFile()
{
}

FLIniFile::E_RESULT	FLIniFile::InitializeINIFile( const TCHAR * pPathString, const E_BASE_PATH::E eBasePath /* = E_BASE_PATH::CURRENT_PATH */ )
{
	switch( eBasePath )
	{
		case E_BASE_PATH::ABSOLUTE_PATH:
			{
				FLStrcpy( m_szFilePath, _countof( m_szFilePath ), pPathString );
			}
			break;
		case E_BASE_PATH::CURRENT_PATH:
			{
				::GetCurrentDirectory( sizeof(m_szFilePath), m_szFilePath );
				FLSPrintf( m_szFilePath, _countof(m_szFilePath), "%s\\%s", m_szFilePath, pPathString );
			}
			break;
		case E_BASE_PATH::WINDOWS_PATH:
			{
				::GetWindowsDirectory( m_szFilePath, sizeof(m_szFilePath) );
				FLSPrintf( m_szFilePath, _countof(m_szFilePath), "%s\\%s", m_szFilePath, pPathString );
			}
			break;
		default:
			{
				return E_BASEPATH_TYPE_ERROR;
			}
			break;
	}

	if( _taccess( pPathString, 0 ) != 0 )
	{
		return E_NOT_EXIST;
	}

	return E_OK;
}

bool	FLIniFile::IniReadString( const TCHAR * pApp, const TCHAR * pKey, TCHAR * o_rOut, const int CCh )
{
	const DWORD Ret = ::GetPrivateProfileString( pApp, pKey, NULL, o_rOut, CCh, m_szFilePath );
	return ( Ret != 0 ) ? true : false;
}


bool	FLIniFile::IniWriteString( const TCHAR * pApp, const TCHAR * pKey, const TCHAR * pIn )
{
	if( pIn == NULL )
	{
		return false;
	}

	const BOOL Ret = ::WritePrivateProfileString( pApp, pKey, pIn, m_szFilePath );
	return ( Ret != FALSE ) ? true : false;
}

bool	FLIniFile::IniReadInt( const TCHAR * pApp, const TCHAR *pKey, int & o_rOut )
{
	TCHAR valueString[ MAX_PATH ] = { 0 };

	const DWORD Ret = GetPrivateProfileString( pApp, pKey, NULL, valueString, _countof( valueString ), m_szFilePath );
	
	return	( Ret != 0 ) ? o_rOut = ::_ttoi( valueString ), true : false;
}

bool	FLIniFile::IniWriteInt( const TCHAR * pApp, const TCHAR * pKey, const int & rIn )
{
	TCHAR valueString[ MAX_PATH ] = { 0 };
	FLIntToString( rIn, valueString, _countof( valueString ), 10 );

	const BOOL ret			= ::WritePrivateProfileString( pApp, pKey, valueString, m_szFilePath );
	return ( ret == FALSE ) ? false : true;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
