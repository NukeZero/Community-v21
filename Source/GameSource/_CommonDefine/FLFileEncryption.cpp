#include "stdafx.h"
#include "FLFileEncryption.h"

#include <common/FLStdFile.h>

//#include "..\..\program\_CommonDefine\FlyffCommonDefine.h"

FLFileEncryption::FLFileEncryption()
{
	Init();
}

FLFileEncryption::~FLFileEncryption()
{

}

FLFileEncryption* FLFileEncryption::GetInstance()
{
	static FLFileEncryption	xFileEncryption;

	return & xFileEncryption;
}

void	FLFileEncryption::SetSourceFile( const TCHAR* pszFileName )
{
	memset( m_szSourceFileName, 0, sizeof(m_szSourceFileName) );

	if( pszFileName != NULL )
	{
		::FLStrcpy( m_szSourceFileName, sizeof(m_szSourceFileName), pszFileName );

		TCHAR	szDrive[MAX_PATH] = { 0 };
		TCHAR	szDirectory[MAX_PATH] = { 0 };
		TCHAR	szFileName[MAX_PATH] = { 0 };
		TCHAR	szFileExtension[MAX_PATH] = { 0 };

		FLSplitPath( m_szSourceFileName, szDrive, _countof( szDrive ), szDirectory, _countof( szDirectory ), szFileName, _countof( szFileName ), szFileExtension, _countof( szFileExtension ) );

		FLSPrintf( m_szEncFileName, _countof(m_szEncFileName), "%s%s%s%s.enc", szDrive, szDirectory, szFileName, szFileExtension );
		FLSPrintf( m_szDecFileName, _countof(m_szDecFileName), "%s%s%s%s.dec", szDrive, szDirectory, szFileName, szFileExtension );
	}

	return;
}

bool	FLFileEncryption::IsSetSourceFile()
{
	size_t	nFileNameLen	= 0;
	nFileNameLen	= _tcslen( m_szSourceFileName );

	if( nFileNameLen <= 0 )
	{
		return	false;
	}

	return	true;
}

TCHAR*	FLFileEncryption::GetSourceFileName()
{
	return	( m_szSourceFileName );
}

TCHAR*	FLFileEncryption::GetEncryptionFileName()
{
	return	( m_szEncFileName );
}

TCHAR*	FLFileEncryption::GetDecryptionFileName()
{
	return	( m_szDecFileName );
}


DWORD	FLFileEncryption::EncryptionFile()
{
	if( IsSetSourceFile() == true )
	{
		return	( EncryptionFile( m_szSourceFileName ) );
	}
	else
	{
		return	( FILE_ENCRYPTION_NOT_INITIALIZE );
	}
}

DWORD	FLFileEncryption::EncryptionFile( const TCHAR* pszFileName )
{
	if( IsSetSourceFile() == false )
	{
		return	( FILE_ENCRYPTION_NOT_INITIALIZE );
	}

	FILE* pFile = FLStdFileOpen( pszFileName, "rt" );
	if( pFile == NULL )
	{
		return	( ENCRYPTION_SOURCE_FILE_OPEN_ERROR );
	}

	FILE* pFileEnc = FLStdFileOpen( m_szEncFileName, "w" );
	if( pFileEnc == NULL )
	{
		return	( ENCRYPTION_RESULT_FILE_OPEN_ERROR );
	}

	

	while( true )
	{
		TCHAR szBuffer[512] = { 0 };
		if( _fgetts( szBuffer, 256, pFile ) == NULL )
		{
			break;
		}

		TCHAR szOut[512] = { 0 };

		Encrypt( szBuffer, szOut );
		_ftprintf( pFileEnc, "%s\n", szOut );
	}
	fclose( pFile );
	fclose( pFileEnc );

	::FLStrcpy( m_szSourceFileName, sizeof(m_szSourceFileName), pszFileName );

	return	( ENCRYPTION_SUCCESS);
}

DWORD	FLFileEncryption::DecryptionFile()
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

DWORD	FLFileEncryption::DecryptionFile( const TCHAR* pszFileName )
{
	if( IsSetSourceFile() == false )
	{
		return	( FILE_ENCRYPTION_NOT_INITIALIZE );
	}

	FILE* pFile = FLStdFileOpen( pszFileName, "rt" );
	if( pFile == NULL )
	{
		return	( DECRYPTION_SOURCE_FILE_OPEN_ERROR );
	}

	FILE* pFileDec = FLStdFileOpen( m_szDecFileName, "w" );
	if( pFileDec == NULL )
	{
		return	( DECRYPTION_RESULT_FILE_OPEN_ERROR );
	}

	

	while( true )
	{
		TCHAR szBuffer[512] = { 0 };
		if( _fgetts( szBuffer, 512, pFile ) == NULL )
		{
			break;
		}

		TCHAR szOut[512] = { 0 };

		Decrypt( szBuffer, szOut );
		_ftprintf( pFileDec, "%s", szOut );
	}
	fclose( pFile );
	fclose( pFileDec );

	return true;
}

void	FLFileEncryption::Init()
{
	InitKey();

	memset( m_szSourceFileName, 0, sizeof(m_szSourceFileName) );
	memset( m_szEncFileName, 0, sizeof(m_szEncFileName) );
	memset( m_szDecFileName, 0, sizeof(m_szDecFileName) );
}

void	FLFileEncryption::InitKey()
{
	static unsigned char szKeys[3][8] =
	{
		{ 0x1F, 0x25, 0x38, 0x41, 0x7C, 0xBF, 0x1D, 0x1D },
		{ 0x2B, 0x4A, 0x6C, 0x49, 0xAF, 0xCD, 0xEA, 0x21 },
		{ 0x15, 0x37, 0x85, 0x51, 0xC6, 0xEC, 0x05, 0x21 }
	};

	des3_set_3keys( &m_tCtx3, szKeys[0], szKeys[1], szKeys[2] );
}

void	FLFileEncryption::Encrypt( const TCHAR* szIn, TCHAR* szOut )
{
	unsigned char szEncrypt[512] = { 0 };

	for( int nCount = 0; nCount < 16; ++nCount )
	{
		unsigned char szBuf[8] = { 0 };
		::memcpy( szBuf, szIn + ( nCount * 8 ), 8 );

		des3_encrypt( &m_tCtx3, szBuf, szEncrypt + ( nCount * 8 ) );
	}

	TCHAR szBuffer[512] = { 0 };

	for( DWORD i = 0; i < 16 * 8; ++i )
	{
		FLSPrintf( szBuffer + ( i * 2 ), _countof( szBuffer ), "%02X", szEncrypt[i] );
	}

	::memcpy( szOut, szBuffer, sizeof( szBuffer ) );
}

void	FLFileEncryption::Decrypt( const TCHAR* szIn, TCHAR* szOut )
{
	size_t nCount = _tcslen( szIn ) / 2;
	if( nCount > 0 )
	{
		unsigned char szEncrypt[512] = { 0 };
		
		for( DWORD i = 0; i < nCount; ++i )
		{
			TCHAR szEnc[3] = { 0 };
			::memcpy( szEnc, szIn + ( i * 2 ), 2 );
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
			_stscanf_s( szEnc, "%02x", ( szEncrypt + i ) );
#else
			_stscanf( szEnc, "%02x", ( szEncrypt + i ) );
#endif
		}

		unsigned char szDecrypt[512] = { 0 };

		DWORD dwCount = 0;
		

		while( true )
		{
			unsigned char szBuf[8] = { 0 };
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

