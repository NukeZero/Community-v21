#include "stdafx.h"
#include "PatchManager.h"
#include "BetaPatchClientDlg.h"

#include <common/FLStdFile.h>

CPatchManager					g_PatchManager;

///////////////////////////////////////////////////////////////////////////////////////////
// extern 
///////////////////////////////////////////////////////////////////////////////////////////

extern CBetaPatchClientDlg*		g_pDlg;

///////////////////////////////////////////////////////////////////////////////////////////
// CPatchManager
///////////////////////////////////////////////////////////////////////////////////////////

CPatchManager::CPatchManager()
{
	ZeroMemory( m_szCurrentDirectory, sizeof( m_szCurrentDirectory ) );
	InitKey();
}

CPatchManager::~CPatchManager()
{
}

void CPatchManager::SetCurrentLocalDirectory()
{
	ZeroMemory( m_szCurrentDirectory, sizeof( m_szCurrentDirectory ) );
	GetCurrentDirectory( sizeof( m_szCurrentDirectory ), m_szCurrentDirectory );// 현재 디렉토리 얻음
}

void CPatchManager::CheckFiles()
{
	DWORD						dwTotal = m_files.size();
	DWORD						i = 0;
	T_FILE_INFO					tFileInfo;
	FileContainerIterator		it;

	for( it = m_files.begin(); it != m_files.end();  )
	{
		tFileInfo = ( *it );

		if( tFileInfo.bDir == true )				// 디렉토리이면
		{	
			CreateDirectory( tFileInfo.szPath, NULL );
			it = m_files.erase( it );
		}
		else						// 파일이면 
		{
			if( IsSameFile( tFileInfo ) )	
				it = m_files.erase( it );
			else
			{
				FLTRACE_LOG( PROGRAM_NAME, _T( "IsSameFile: %s" ), tFileInfo.szPath );
				++it;
			}
		}

		if( g_pDlg->IsClose() )
			return;

		i++;
		g_pDlg->m_File_Progress.SetPos( i * 100 / dwTotal );
	}
}

bool CPatchManager::IsSameFile( T_FILE_INFO& tFileInfo )
{
	WIN32_FILE_ATTRIBUTE_DATA	attribute;
	BOOL bOK = ::GetFileAttributesEx( tFileInfo.szPath, GetFileExInfoStandard, &attribute );
	if( bOK == TRUE )
	{							
		if( tFileInfo.dwFileSize == attribute.nFileSizeLow )	//  파일 크기 같고
		{
			__int64 nLocalFileTime = MAKEINT64( attribute.ftLastWriteTime.dwLowDateTime, attribute.ftLastWriteTime.dwHighDateTime );
			__int64 nFileTimeGap = tFileInfo.n64FileTime - nLocalFileTime;

			if( _abs64( nFileTimeGap ) <= 10000000 * 60 * 3 )	// 10000000 * 60 == 60초
			{
				return true;
			}
		}
	}
	return false;
}



MAKEPATCHLIST_RESULT CPatchManager::MakePatchList( int nVersion )
{
	m_files.clear();

	TCHAR szPath[MAX_PATH];
	::memset( szPath, 0, sizeof( szPath ) );
	FLSPrintf( szPath, _countof( szPath ), "%s\\%s", m_szCurrentDirectory, "list.txt" ); 

	FILE* fp = FLStdFileOpen( szPath, "rt" );
	if( fp == NULL )
	{
		return MAKEPATCHLIST_FILE_NOTFOUND;
	}

	TCHAR	szTokens[3][_MAX_PATH];
	TCHAR	szBuffer[512];


	while( true )
	{
		::memset( szBuffer, 0, sizeof( szBuffer ) );
		if( _fgetts( szBuffer, 256, fp ) == NULL)
		{
			break;
		}
		
		if( szBuffer[0] == 'v' )
		{
			if( memcmp( szBuffer, "ver:", 4 ) == 0 )
			{
				if( _tstoi( szBuffer+4 ) == nVersion )
				{
					continue;
				}
				else
				{
					fclose( fp );
					m_files.clear();
					return MAKEPATCHLIST_VERSION_MISMATCH;		
				}
			}
		}
		else
		{
			TCHAR szDecrypt[512];
			::memset( szDecrypt, 0, sizeof( szDecrypt ) );
            
			Decrypt( szBuffer, szDecrypt );

			DWORD dwFileSize = 0;
			__int64 n64FileTime = 0;

			::memset( szTokens, 0, sizeof( szTokens ) );
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
			_stscanf_s( szDecrypt, "%s %s %s", szTokens[0], _countof( szTokens[0] ), szTokens[1], _countof( szTokens[1] ), szTokens[2], _countof( szTokens[2] ) );
#else
			_stscanf( szDecrypt, "%s %s %s", szTokens[0], szTokens[1], szTokens[2] );
#endif

			if( _tcslen( szTokens[2] ) == 0 )
			{
				fclose( fp );
				return MAKEPATCHLIST_FILE_NOTFOUND;
			}

 			dwFileSize = _tstol( szTokens[0] );
 			n64FileTime = _tstoi64( szTokens[1] );

			TCHAR szFileName[_MAX_PATH];
			::memset( szFileName, 0, sizeof( szFileName ) );
			FLSPrintf( szFileName, _countof( szFileName ), "%s", szDecrypt + 30 );

			bool bDir = false;

			if( dwFileSize == 0 && n64FileTime == 0 )
			{
				bDir = true;
			}
			EnqueueFILE( szFileName, bDir, dwFileSize, n64FileTime );
		}
	}
	fclose( fp );

	CheckFiles();

	return MAKEPATCHLIST_OK;
}

void CPatchManager::EnqueueFILE( const TCHAR* szFile, bool bDir, DWORD dwSize, __int64 n64FileTime )
{
	static TCHAR s_cdn[] = SERVER_ROOT;

	T_FILE_INFO tFileInfo;
	::memset( &tFileInfo, 0, sizeof( tFileInfo ) );

	tFileInfo.bDir = bDir;
	tFileInfo.dwFileSize = dwSize;
	tFileInfo.n64FileTime = n64FileTime;
	FLSPrintf( tFileInfo.szPath, _countof( tFileInfo.szPath ), "%s\\%s", m_szCurrentDirectory, szFile ); 
	
	TCHAR szServerPath[MAX_PATH];
	::memset( szServerPath, 0, sizeof( szServerPath ) );
	FLSPrintf( szServerPath, _countof( szServerPath ), "%s\\%s.gz", s_cdn, szFile );
	
	CString str = szServerPath;
	str.Replace( '\\', '/' );
	
	FLStrcpy( tFileInfo.szServerPath, _countof( tFileInfo.szServerPath ), str );

	m_files.push_back( tFileInfo );
}

void	CPatchManager::InitKey()
{
	static unsigned char szKeys[3][8] =
	{
		{ 0x32, 0x89, 0x45, 0x9A, 0x89, 0xAB, 0xCD, 0x11 },
		{ 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01 },
		{ 0x1F, 0x67, 0x89, 0x54, 0xCD, 0xEF, 0x00, 0x23 }
	};

	des3_set_3keys( &m_tCtx3, szKeys[0], szKeys[1], szKeys[2] );
}

void	CPatchManager::Decrypt( const TCHAR* szIn, TCHAR* szOut )
{
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
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
			_stscanf_s( szEnc, "%02x", ( szEncrypt + i ) );
#else
			_stscanf( szEnc, "%02x", ( szEncrypt + i ) );
#endif
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

