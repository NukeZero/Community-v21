
#include "FLFindFile.h"
#include "FLStringFunction.h"


//////////////////////////////////////////////////////////////////////////
FLFindFileInfo::FLFindFileInfo( const WIN32_FIND_DATA* pFindFileData, const TCHAR* szFolder )
{
	m_tFindFileData = *pFindFileData;
	FLStrcpy( m_szFullFileName, _countof( m_szFullFileName ), szFolder );
	FLStrcat( m_szFullFileName, _countof( m_szFullFileName ), m_tFindFileData.cFileName );
}

FLFindFileInfo::~FLFindFileInfo()
{
}

bool	FLFindFileInfo::IsFolder()	const
{
	if( m_tFindFileData.dwFileAttributes != INVALID_FILE_ATTRIBUTES )
	{
		if( m_tFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			return true;
		}
	}

	return false;
}

bool	FLFindFileInfo::IsDots()	const
{
	if( IsFolder() == true )
	{
		if( ::_tcscmp( m_tFindFileData.cFileName, _T( "." ) ) == 0 || ::_tcscmp( m_tFindFileData.cFileName, _T( ".." ) ) == 0 )
		{
			return true;
		}
	}

	return false;
}

bool	FLFindFileInfo::IsReadOnly()	const
{
	if( m_tFindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY )
	{
		return true;
	}

	return false;
}

const TCHAR*	FLFindFileInfo::GetName()	const
{
	return m_szFullFileName;
}

//////////////////////////////////////////////////////////////////////////

FLFindFile::FLFindFile()
{
	m_hFind = INVALID_HANDLE_VALUE;
	::memset( &m_tFindFileData, 0, sizeof( m_tFindFileData ) );
	::memset( m_szDestFolder, 0, sizeof( m_szDestFolder ) );
}

FLFindFile::~FLFindFile()
{
	Close();
}

bool	FLFindFile::FindFirst( const TCHAR* szFolder, const TCHAR* szFindFilter )
{
	if( szFolder == NULL || szFindFilter == NULL )
	{
		return false;
	}

	if( m_hFind != INVALID_HANDLE_VALUE )
	{
		Close();
	}

	DWORD dwPos = ::GetFullPathName( szFolder, _countof( m_szDestFolder ), m_szDestFolder, NULL );
	if( 0 < dwPos && dwPos < _countof( m_szDestFolder ) )
	{
		if( m_szDestFolder[dwPos-1] != _T( '\\' ) && m_szDestFolder[dwPos-1] != _T( '/' ) )
		{
			FLStrcat( m_szDestFolder, _countof( m_szDestFolder ), _T( "\\" ) );
		}
	}

	TCHAR szFullPathFilter[MAX_PATH] = { 0, };
	FLStrcpy( szFullPathFilter, _countof( szFullPathFilter ), m_szDestFolder );
	FLStrcat( szFullPathFilter, _countof( szFullPathFilter ), szFindFilter );

	m_hFind = ::FindFirstFile( szFullPathFilter, &m_tFindFileData );
	if( m_hFind == INVALID_HANDLE_VALUE )
	{
		return false;
	}

	return true;
}

bool	FLFindFile::FindFirst( const TCHAR* szFindFile )
{
	if( szFindFile == NULL )
	{
		return false;
	}

	if( m_hFind != INVALID_HANDLE_VALUE )
	{
		Close();
	}

	TCHAR* pFileName = NULL;
	DWORD dwPos = ::GetFullPathName( szFindFile, _countof( m_szDestFolder ), m_szDestFolder, &pFileName );
	if( 0 < dwPos && dwPos < _countof( m_szDestFolder ) )
	{
		if( pFileName != NULL )
		{
			*pFileName = _T( '\0' );
		}
	}

	m_hFind = ::FindFirstFile( szFindFile, &m_tFindFileData );
	if( m_hFind == INVALID_HANDLE_VALUE )
	{
		return false;
	}

	return true;
}

void	FLFindFile::Close()
{
	if( m_hFind != INVALID_HANDLE_VALUE )
	{
		FLVERIFY( ::FindClose( m_hFind ) );
		m_hFind = INVALID_HANDLE_VALUE;
	}

	::memset( &m_tFindFileData, 0, sizeof( m_tFindFileData ) );
	::memset( m_szDestFolder, 0, sizeof( m_szDestFolder ) );
}

FLFindFileInfo	FLFindFile::GetFindFileInfo()	const
{
	return FLFindFileInfo( &m_tFindFileData, m_szDestFolder );
}

bool	FLFindFile::FindNext()
{
	if( m_hFind != INVALID_HANDLE_VALUE )
	{
		if( ::FindNextFile( m_hFind, &m_tFindFileData ) != FALSE )
		{
			return true;
		}
	}

	return false;
}
