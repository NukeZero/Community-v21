#include "stdafx.h"
#include "FLListFileEncryption.h"

//#include "..\..\program\_CommonDefine\FlyffCommonDefine.h"

FLListFileEncryption::FLListFileEncryption()
{
	static unsigned char szKeys[3][8] =
	{
		{ 0x32, 0x89, 0x45, 0x9A, 0x89, 0xAB, 0xCD, 0x11 },
		{ 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01 },
		{ 0x1F, 0x67, 0x89, 0x54, 0xCD, 0xEF, 0x00, 0x23 }
	};

	des3_set_3keys( &m_tCtx3, szKeys[0], szKeys[1], szKeys[2] );

	::memset( m_szSourceFileName, 0, sizeof(m_szSourceFileName) );
	::memset( m_szDecFileName, 0, sizeof(m_szDecFileName) );
}

FLListFileEncryption::~FLListFileEncryption()
{

}

void	FLListFileEncryption::SetSourceFile( const TCHAR* pszFileName )
{
	::memset( m_szSourceFileName, 0, sizeof(m_szSourceFileName) );

	if( pszFileName != NULL )
	{
		::FLStrcpy( m_szSourceFileName, sizeof(m_szSourceFileName), pszFileName );

		TCHAR	szDrive[MAX_PATH];
		::memset( szDrive, 0, sizeof(szDrive) );
		TCHAR	szDirectory[MAX_PATH];
		::memset( szDirectory, 0, sizeof(szDirectory) );
		TCHAR	szFileName[MAX_PATH];
		::memset( szFileName, 0, sizeof(szFileName) );
		TCHAR	szFileExtension[MAX_PATH];
		::memset( szFileExtension, 0, sizeof(szFileExtension) );

		::_tsplitpath( m_szSourceFileName, szDrive, szDirectory, szFileName, szFileExtension );

		::FLSPrintf( m_szDecFileName, _countof(m_szDecFileName), "%s%s%s%s.dec", szDrive, szDirectory, szFileName, szFileExtension );
	}

	return;
}

bool	FLListFileEncryption::IsSetSourceFile()
{
	size_t	nFileNameLen	= 0;
	nFileNameLen	= _tcslen( m_szSourceFileName );

	if( nFileNameLen <= 0 )
	{
		return	false;
	}

	return	true;
}

TCHAR*	FLListFileEncryption::GetSourceFileName()
{
	return	( m_szSourceFileName );
}

TCHAR*	FLListFileEncryption::GetDecryptionFileName()
{
	return	( m_szDecFileName );
}

DWORD	FLListFileEncryption::DecryptionFile()
{
	if( IsSetSourceFile() == true )
	{
			return	( DecryptionFile( m_szSourceFileName ) );
	}
	else
	{
		return	( FILE_ENCRYPTION_NOT_INITIALIZE );
	}
}

DWORD	FLListFileEncryption::DecryptionFile( const TCHAR* pszFileName )
{
	if( IsSetSourceFile() == false )
	{
		return	( FILE_ENCRYPTION_NOT_INITIALIZE );
	}

	FILE* pFile = _tfopen( pszFileName, "rt" );
	if( pFile == NULL )
	{
		return	( DECRYPTION_SOURCE_FILE_OPEN_ERROR );
	}

	FILE* pFileDec = _tfopen( m_szDecFileName, "w" );
	if( pFileDec == NULL )
	{
		return	( DECRYPTION_RESULT_FILE_OPEN_ERROR );
	}

	TCHAR szBuffer[512];
	::memset( szBuffer, 0, sizeof( szBuffer ) );

	int	nLine = 0;

	while( true )
	{
		::memset( szBuffer, 0, sizeof( szBuffer ) );
		if( _fgetts( szBuffer, 512, pFile ) == NULL )
		{
			break;
		}

		TCHAR szOut[512];
		::memset( szOut, 0, sizeof( szOut ) );

		Decrypt( szBuffer, szOut );
		_ftprintf( pFileDec, "%s\n", szOut );
	}
	fclose( pFile );
	fclose( pFileDec );

	return true;
}

void	FLListFileEncryption::Decrypt( const TCHAR* szIn, TCHAR* szOut )
{
	if( _tcsicmp( szIn, "ver" ) > 0 )
	{
		::memcpy( szOut, szIn, _tcslen( szIn ) - 1 );
		return;
	}


	size_t nCount = _tcslen( szIn ) / 2;
	if( nCount > 0 )
	{
		unsigned char szEncrypt[512];
		::memset( szEncrypt, 0, sizeof( szEncrypt ) );
		TCHAR szEnc[3];

		for( DWORD i = 0; i < nCount; ++i )
		{
			::memset( szEnc, 0, sizeof( szEnc ) );
			::memcpy( szEnc, szIn + ( i * 2 ), 2 );						
			_stscanf( szEnc, "%02x", ( szEncrypt + i ) );
		}

		unsigned char szDecrypt[512];
		::memset( szDecrypt, 0, sizeof( szDecrypt ) );

		DWORD dwCount = 0;
		unsigned char szBuf[8];

		while( true )
		{
			::memset( szBuf, 0, sizeof( szBuf ) );
			::memcpy( szBuf, szEncrypt + ( dwCount * 8 ), 8 );

			BYTE byCount = 0;
			for( ; byCount < 8; ++byCount )
			{
				if( szBuf[byCount] != '\0' )
				{
					break;
				}
			}
			if( byCount == 8 )
			{
				break;
			}

			des3_decrypt( &m_tCtx3, szBuf, szDecrypt + ( dwCount * 8 ) );

			++dwCount;
		}

		::memcpy( szOut, szDecrypt, sizeof( szDecrypt ) );
	}
}

