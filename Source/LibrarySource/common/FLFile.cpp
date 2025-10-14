
#include "FLFile.h"


FLFile::FLFile()
{
	m_hFile	= INVALID_HANDLE_VALUE;
}

FLFile::~FLFile()
{
	Close();
}

bool	FLFile::Create( const TCHAR* szName, DWORD dwAccessMode, DWORD dwShareMode, DWORD dwFlagsAndAttributes )
{
	FLASSERT( Opened() == false );

	m_hFile	= ::CreateFile( szName, dwAccessMode, dwShareMode, NULL, CREATE_ALWAYS, dwFlagsAndAttributes, NULL );
	if( m_hFile == INVALID_HANDLE_VALUE )
	{
		return false;
	}

	if( ::GetFileType( m_hFile ) != FILE_TYPE_DISK )
	{
		FLASSERT( _T( "GetFileType( m_hFile ) != FILE_TYPE_DISK" ) );

		Close();
		return false;
	}

	return true;
}

bool	FLFile::Open( const TCHAR* szName, DWORD dwAccessMode, DWORD dwShareMode, DWORD dwFlagsAndAttributes )
{
	FLASSERT( Opened() == false );

	m_hFile	= ::CreateFile( szName, dwAccessMode, dwShareMode, NULL, OPEN_EXISTING, dwFlagsAndAttributes, NULL );
	if( m_hFile == INVALID_HANDLE_VALUE )
	{
		return false;
	}

	if( ::GetFileType( m_hFile ) != FILE_TYPE_DISK )
	{
		FLASSERT( _T( "GetFileType( m_hFile ) != FILE_TYPE_DISK" ) );

		Close();
		return false;
	}

	return true;
}

void	FLFile::Close()
{
	if( m_hFile != INVALID_HANDLE_VALUE )
	{
		FLVERIFY( ::CloseHandle( m_hFile ) != FALSE );
		m_hFile	= INVALID_HANDLE_VALUE;
	}
}

bool	FLFile::Opened()	const
{
	return (m_hFile != INVALID_HANDLE_VALUE);
}

size_t	FLFile::Read( void* pBuffer, size_t nSize )
{
	FLASSERT( Opened() == true );
	FLASSERT( pBuffer != NULL );

	DWORD dwReadSize = 0;
	if( Opened() == true && pBuffer != NULL )
	{
		if( ::ReadFile( m_hFile, pBuffer, static_cast< DWORD >( nSize ), &dwReadSize, NULL ) == FALSE )
		{
			dwReadSize = 0;
		}
	}

	return dwReadSize;
}

size_t	FLFile::Write( const void* pBuffer, size_t nSize )
{
	FLASSERT( Opened() == true );
	FLASSERT( pBuffer != NULL );

	DWORD dwWrittenSize = 0;
	if( Opened() == true )
	{
		if( ::WriteFile( m_hFile, pBuffer, static_cast< DWORD >( nSize ), &dwWrittenSize, NULL ) == FALSE )
		{
			dwWrittenSize = 0;
		}
	}

	return dwWrittenSize;
}

__int64	FLFile::Seek( __int64 n64Offset, int nOrigin )
{
	FLASSERT( Opened() == true );

	LARGE_INTEGER tLarge;
	tLarge.QuadPart	= 0;

	if( Opened() == true )
	{
		tLarge.QuadPart	= n64Offset;
		tLarge.LowPart	= ::SetFilePointer( m_hFile, tLarge.LowPart, &tLarge.HighPart, nOrigin );
		if( tLarge.LowPart == INVALID_SET_FILE_POINTER && ::GetLastError() != NO_ERROR )
		{
			tLarge.QuadPart	= INVALID_SET_FILE_POINTER;
		}
	}

	return tLarge.QuadPart;
}

bool	FLFile::Flush()
{
	FLASSERT( Opened() == true );

	if( Opened() == true )
	{
		if( ::FlushFileBuffers( m_hFile ) == FALSE )
		{
			return false;
		}

		return true;
	}

	return false;
}

unsigned __int64	FLFile::Size()	const
{
	FLASSERT( Opened() == true );

	ULARGE_INTEGER	tLarge;
	tLarge.QuadPart	= 0;

	if( Opened() == true )
	{
		tLarge.LowPart	= ::GetFileSize( m_hFile, &tLarge.HighPart );
		if( tLarge.LowPart == INVALID_SET_FILE_POINTER && ::GetLastError() != NO_ERROR )
		{
			tLarge.QuadPart	= 0;
		}
	}

	return tLarge.QuadPart;
}

