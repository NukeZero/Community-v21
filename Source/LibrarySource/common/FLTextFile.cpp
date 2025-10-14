
#include "FLTextFile.h"


#if defined(_UNICODE) || defined(UNICODE)
static const unsigned char FLUTF16_LITTLE_ENDIAN_BOM[]	= { 0xFF, 0xFE };
#endif

FLTextFile::FLTextFile()
{
}

FLTextFile::~FLTextFile()
{
	Close();
}

bool	FLTextFile::Create( const TCHAR* szName, DWORD dwAccessMode, DWORD dwShareMode, DWORD dwFlagsAndAttributes )
{
	FLASSERT( szName != NULL );

	if( szName != NULL )
	{
		if( m_cFile.Create( szName, dwAccessMode, dwShareMode, dwFlagsAndAttributes ) == false )
		{
			return false;
		}

#if defined(_UNICODE) || defined(UNICODE)
		FLVERIFY( m_cFile.Write( FLUTF16_LITTLE_ENDIAN_BOM, sizeof( FLUTF16_LITTLE_ENDIAN_BOM ) ) == sizeof( FLUTF16_LITTLE_ENDIAN_BOM ) );
#endif

		return true;
	}

	return false;
}

bool	FLTextFile::Open( const TCHAR* szName, DWORD dwAccessMode, DWORD dwShareMode, DWORD dwFlagsAndAttributes )
{
	FLASSERT( szName != NULL );

	if( szName != NULL )
	{
		if( m_cFile.Open( szName, dwAccessMode, dwShareMode, dwFlagsAndAttributes ) == false )
		{
			return false;
		}

#if defined(_UNICODE) || defined(UNICODE)
		unsigned char byBOM[sizeof( FLUTF16_LITTLE_ENDIAN_BOM )] = { 0, };
		if( m_cFile.Read( byBOM, sizeof( byBOM ) ) != 0 )
		{
			if( ::memcmp( byBOM, FLUTF16_LITTLE_ENDIAN_BOM, sizeof( FLUTF16_LITTLE_ENDIAN_BOM ) ) != 0 )
			{
				m_cFile.Seek( 0, FILE_BEGIN );
			}
		}
#endif

		return true;
	}

	return false;
}

void	FLTextFile::Close()
{
	m_cFile.Close();
}

bool	FLTextFile::Opened()	const
{
	return m_cFile.Opened();
}

size_t	FLTextFile::WriteText( const TCHAR* szText, size_t cchCount )
{
	FLASSERT( szText != NULL );
	FLASSERT( cchCount > 0 );
	FLASSERT( Opened() == true );

	size_t nWrittenSize = 0;
	if( szText != NULL && cchCount > 0 && Opened() == true )
	{
		nWrittenSize = m_cFile.Write( szText, (cchCount * sizeof( TCHAR )) );
	}

	return (nWrittenSize / sizeof( TCHAR ));
}

size_t	FLTextFile::ReadText( TCHAR* szText, size_t cchCount )
{
	FLASSERT( szText != NULL );
	FLASSERT( cchCount > 0 );
	FLASSERT( Opened() == true );

	size_t nReadSize = 0;
	if( szText != NULL && cchCount > 0 && Opened() == true )
	{
		nReadSize = m_cFile.Read( szText, ((cchCount-1) * sizeof( TCHAR )) );
		szText[nReadSize] = _T( '\0' );
	}

	return (nReadSize / sizeof( TCHAR ));
}

size_t	FLTextFile::ReadLine( TCHAR* szText, size_t cchCount )
{
	FLASSERT( szText != NULL );
	FLASSERT( cchCount > 0 );
	FLASSERT( Opened() == true );

	size_t nReadCount = 0;
	if( szText != NULL && cchCount > 0 && Opened() == true )
	{
		size_t nMaxReadCount = cchCount-1;
		while( nMaxReadCount > nReadCount )
		{
			TCHAR szOneChar = _T( '\0' );
			if( m_cFile.Read( &szOneChar, sizeof( szOneChar ) ) == 0 )
			{
				// end of file
				break;
			}

			if( szOneChar != _T( '\r' ) && szOneChar != _T( '\n' ) )
			{
				// not new line
				szText[nReadCount] = szOneChar;
				++nReadCount;
			}
			else
			{
				// move next line
				if( m_cFile.Read( &szOneChar, sizeof( szOneChar ) ) != 0 )
				{
					if( szOneChar != _T( '\r' ) && szOneChar != _T( '\n' ) )
					{
						m_cFile.Seek( -1i64 * static_cast< __int64 >( sizeof( szOneChar ) ), FILE_CURRENT );
						break;
					}
				}

				break;
			}
		}

		szText[nReadCount] = _T( '\0' );
	}

	return nReadCount;
}

__int64	FLTextFile::Seek( __int64 n64Offset, int nOrigin )
{
	return m_cFile.Seek( n64Offset, nOrigin );
}

bool	FLTextFile::Flush()
{
	return m_cFile.Flush();
}

unsigned __int64	FLTextFile::Size()	const
{
	return m_cFile.Size();
}


