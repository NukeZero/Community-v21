#include "stdafx.h"
#include "FLMakeFileData.h"
#include "FLFileCompare.h"

#include <common/FLStdFile.h>


FLMakeFileData::FLMakeFileData()
{
	Init();
}

FLMakeFileData::~FLMakeFileData()
{

}

FLMakeFileData*	FLMakeFileData::GetInstance()
{
	static FLMakeFileData xMakeFileData;

	return	&xMakeFileData;
}


void	FLMakeFileData::Init()
{
	memset( m_szCurrentDirectory, 0, sizeof( m_szCurrentDirectory ) );
	GetCurrentDirectory( sizeof( m_szCurrentDirectory ), m_szCurrentDirectory );

	static unsigned char szKeys[3][8] =
	{
		{ 0x32, 0x89, 0x45, 0x9A, 0x89, 0xAB, 0xCD, 0x11 },
		{ 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01 },
		{ 0x1F, 0x67, 0x89, 0x54, 0xCD, 0xEF, 0x00, 0x23 }
	};

	des3_set_3keys( &m_tCtx3, szKeys[0], szKeys[1], szKeys[2] );
}

void	FLMakeFileData::Decrypt( const TCHAR* szIn, TCHAR* szOut )
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

bool	FLMakeFileData::MakeFileDataFromList( const TCHAR* szFileName )
{
	TCHAR szFullFileName[_MAX_PATH] = { 0, };
	FLSPrintf( szFullFileName, _countof( szFullFileName ), "%s\\%s", m_szCurrentDirectory, szFileName );

	FILE* fp = FLStdFileOpen( szFullFileName, "rt" );
	if( fp == NULL )
	{
		return false;
	}

	
	
	

	while( true )
	{
		TCHAR	szBuffer[512] = { 0 };
		if( _fgetts( szBuffer, 256, fp ) == NULL)
		{
			break;
		}

		if( szBuffer[0] == 'v' )
		{
			if( memcmp( szBuffer, "ver:", 4 ) == 0 )
			{
				continue;
			}
		}
		else
		{
			TCHAR szDecrypt[512] = { 0 };
			memset( szDecrypt, 0, sizeof( szDecrypt ) );

			Decrypt( szBuffer, szDecrypt );

			//memset( szTokens, 0, sizeof( szTokens ) );
			TCHAR	szTokens[3][_MAX_PATH] = { 0 };

#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
			_stscanf_s( szDecrypt, "%s %s %s", szTokens[0], _countof( szTokens[0] ), szTokens[1], _countof( szTokens[1] ), szTokens[2], _countof( szTokens[2] ) );
#else
			_stscanf( szDecrypt, "%s %s %s", szTokens[0], szTokens[1], szTokens[2] );
#endif

			if( _tcslen( szTokens[2] ) == 0 )
			{
				fclose( fp );
				return true;	// oops..
			}

			DWORD dwFileSize = 0;
			__int64 n64FileTime = 0;

			dwFileSize = _tstol( szTokens[0] );
			n64FileTime = _tstoi64( szTokens[1] );

			TCHAR szFileName[_MAX_PATH] = { 0, };
			FLSPrintf( szFileName, _countof( szFileName ), "%s", szDecrypt + 30 );

			if( dwFileSize != 0 && n64FileTime != 0 )
			{
				T_FILE_INFO tFileInfo = { 0 };
				//mem_set( &tFileInfo, 0, sizeof( tFileInfo ) );
				tFileInfo.byCompare = FILECOMPARE_NOT_COMPARE;
				tFileInfo.dwFileSize = dwFileSize;
				tFileInfo.n64FileTime = n64FileTime;
				FLSPrintf( tFileInfo.szFullFileName, _countof( tFileInfo.szFullFileName ), "%s\\%s", m_szCurrentDirectory, szFileName );

				g_xFileCompare->InsertFileData( tFileInfo );
			}
		}
	}
	fclose( fp );

	return true;
}
