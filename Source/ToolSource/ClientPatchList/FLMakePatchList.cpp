#include "stdafx.h"
#include "FLMakePatchList.h"


FLMakePatchList*	FLMakePatchList::GetInstance()
{
	static FLMakePatchList xMakePatchList;
	return & xMakePatchList;
}


FLMakePatchList::FLMakePatchList()
{
	Init();
}

FLMakePatchList::~FLMakePatchList()
{

}

const TCHAR*	FLMakePatchList::GetResDir()
{
	return m_szResClientPath;
}

bool	FLMakePatchList::LoadScript()
{
	FILE* pFile = _tfopen( "ClientPatchList.ini", "rt" );
	
	if( pFile == NULL )
	{
		return false;
	}

	TCHAR szBuffer[_MAX_PATH], szToken[2][_MAX_PATH];

	while( true )
	{
		::memset( szBuffer, 0, sizeof( szBuffer ) );
		if( _fgetts( szBuffer, _MAX_PATH - 1, pFile ) == NULL)
		{
			break;
		}
		::memset( szToken, 0, sizeof( szToken ) );
		_stscanf( szBuffer, "%s %s", szToken[0], szToken[1] );

		if( _tcscmp( szToken[0], "CurrentDirectory" ) == 0 )
		{
			_tcscpy( m_szResClientPath, szToken[1] );
		}

		else if( _tcscmp( szToken[0], "FlyffVersion" ) == 0 )
		{
			m_nFlyffVersion = _tstoi( szToken[1] );
			FILE* pList = _tfopen( "list.txt", "w" );
			if( pList != NULL )
			{
				TCHAR szVersion[10];
				::memset( szVersion, 0, sizeof( szVersion ) );
				_stprintf( szVersion, "%s: %d", "ver", m_nFlyffVersion );
				_ftprintf( pList, "%s\n", szVersion );
			}
			else
			{
				return false;
			}
			fclose( pList );
		}
	}
	
	fclose( pFile );

	if( _tcslen( m_szResClientPath ) == 0 || m_nFlyffVersion == 0 )
	{
		return false;
	}

	return true;
}

bool	FLMakePatchList::FindPatchFile( const TCHAR* szFindPath )
{
	TCHAR szCurrPath[_MAX_PATH];
	::memset( szCurrPath, 0, sizeof( szCurrPath ) );
	_stprintf( szCurrPath, "%s\\*.*", szFindPath );

	WIN32_FIND_DATA tFindData;
	::memset( &tFindData, 0, sizeof( tFindData ) );
	HANDLE hFileHandle = ::FindFirstFile( szCurrPath, &tFindData );
	if( hFileHandle == INVALID_HANDLE_VALUE )
	{
		::MessageBox( NULL, "Make Patch List Error. File Handle Invalid.", NULL, MB_OK );
		return false;
	}

	BOOL bResult = TRUE;
	while( bResult == TRUE )
	{
		if( tFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			if( tFindData.dwFileAttributes & ( FILE_ATTRIBUTE_HIDDEN ) )
			{
			}
			else
			{
				if( _tcscmp( tFindData.cFileName, "." ) != 0 && _tcscmp( tFindData.cFileName, ".." ) != 0 )
				{
					TCHAR szNewPath[_MAX_PATH];
					::memset( szNewPath, 0, sizeof( szNewPath ) );
					_stprintf( szNewPath, "%s\\%s", szFindPath, tFindData.cFileName );
					MakePatchList( szNewPath );
					if( FindPatchFile( szNewPath ) == false )
					{
						return false;
					}
				}
			}
		}
		else
		{
			if( tFindData.dwFileAttributes & ( FILE_ATTRIBUTE_HIDDEN ) )
			{
			}
			else
			{
				TCHAR szFileName[_MAX_PATH];
				::memset( szFileName, 0, sizeof( szFileName ) );
				_stprintf( szFileName, "%s\\%s", szFindPath, tFindData.cFileName );

				if( MakePatchList( szFileName ) == false )
				{
					return false;
				}
			}
		}
		bResult = ::FindNextFile( hFileHandle, &tFindData );
	}

	::FindClose( hFileHandle );
	return true;
}

void	FLMakePatchList::Init()
{
	m_nFlyffVersion = 0;
	::memset( m_szResClientPath, 0, sizeof( m_szResClientPath ) );
	InitKey();
}

void	FLMakePatchList::InitKey()
{
	static unsigned char szKeys[3][8] =
	{
		{ 0x32, 0x89, 0x45, 0x9A, 0x89, 0xAB, 0xCD, 0x11 },
		{ 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01 },
		{ 0x1F, 0x67, 0x89, 0x54, 0xCD, 0xEF, 0x00, 0x23 }
	};

	des3_set_3keys( &m_tCtx3, szKeys[0], szKeys[1], szKeys[2] );
}

bool	FLMakePatchList::MakePatchList( const TCHAR* szFullFileName )
{
	DWORD dwFileSize = 0;
	__int64 n64FileTime = 0;
	WIN32_FILE_ATTRIBUTE_DATA	tAttribute;
	::memset( &tAttribute, 0, sizeof( tAttribute ) );

	BOOL bOK = ::GetFileAttributesEx( szFullFileName, GetFileExInfoStandard, &tAttribute );
	if( bOK == TRUE )
	{
		if( !( tAttribute.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			dwFileSize = tAttribute.nFileSizeLow;
			n64FileTime = MAKEINT64( tAttribute.ftLastWriteTime.dwLowDateTime, tAttribute.ftLastWriteTime.dwHighDateTime );
		}

		FILE* pFile = _tfopen( "list.txt", "a" );
		if( pFile != NULL )
		{
			TCHAR szFileName[_MAX_PATH];
			::memset( szFileName, 0, sizeof( szFileName ) );
			::memcpy( szFileName, szFullFileName + ( _tcslen( GetResDir() ) + 1 ), ( _tcslen( szFullFileName ) - _tcslen( GetResDir() ) - 1 ) );

			TCHAR szOutput[512];
			::memset( szOutput, 0, sizeof( szOutput ) );
			_stprintf( szOutput, "%10d %18I64d %s", dwFileSize, n64FileTime, szFileName );

			TCHAR szEncrypt[512];
			::memset( szEncrypt, 0, sizeof( szEncrypt ) );

			Encrypt( szOutput, szEncrypt );
			_ftprintf( pFile, "%s\n", szEncrypt );
		}
		else
		{
			::MessageBox( NULL, "Make Patch List Error. list.txt open failed.", NULL, MB_OK );
			return false;
		}
		fclose( pFile );
	}
	else
	{
		::MessageBox( NULL, "Get File Attributes Error.", NULL, MB_OK );
		return false;
	}

	return true;
}

void	FLMakePatchList::Encrypt( const TCHAR* szIn, TCHAR* szOut )
{
	unsigned char szEncrypt[512];
	::memset( szEncrypt, 0, sizeof( szEncrypt ) );
	DWORD dwCount = 0;

	while( true )
	{
		unsigned char szBuf[8];
		::memset( szBuf, 0, sizeof( szBuf ) );
		::memcpy( szBuf, szIn + ( dwCount * 8 ), 8 );

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
		des3_encrypt( &m_tCtx3, szBuf, szEncrypt + ( dwCount * 8 ) );
		
		++dwCount;
	}

	TCHAR szBuffer[512];
	::memset( szBuffer, 0, sizeof( szBuffer ) );

	for( DWORD i = 0; i < dwCount * 8; ++i )
	{
		_stprintf( szBuffer + ( i * 2 ), "%02X", szEncrypt[i] );
	}

	::memcpy( szOut, szBuffer, sizeof( szBuffer ) );
}

void	FLMakePatchList::Decrypt( const TCHAR* szIn, TCHAR* szOut )
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

bool	FLMakePatchList::MakePatchListDecrypt()
{
	FILE* pFile = _tfopen( "list.txt", "rt" );
	FILE* pFile2 = _tfopen( "list_Dec.txt", "w" );
	if( pFile == NULL || pFile2 == NULL )
	{
		return false;
	}

	TCHAR szBuffer[512], szToken[3][512];
	::memset( szBuffer, 0, sizeof( szBuffer ) );
	::memset( szToken, 0, sizeof( szToken ) );

	while( true )
	{
		::memset( szBuffer, 0, sizeof( szBuffer ) );
		if( _fgetts( szBuffer, 256, pFile ) == NULL )
		{
			break;
		}

		TCHAR szOut[512];
		::memset( szOut, 0, sizeof( szOut ) );

		Decrypt( szBuffer, szOut );
		_ftprintf( pFile2, "%s\n", szOut );
	}
	fclose( pFile );
	fclose( pFile2 );

	return true;
}

