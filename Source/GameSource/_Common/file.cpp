#include "stdafx.h"
#include <string.h>
#include <stdio.h>
#include "file.h"
#include "xutil.h"
#include "tools.h"

#include <common/FLStdFile.h>


/////////////////////////////////////////////////////////////////////////////
// CFileIO

BOOL CFileIO::Close(void)
{
	if( fp ) 
	{ 
		fclose( fp ); 
		fp = 0; 
		return TRUE; 
	}
	return FALSE;
}

BOOL CFileIO::Open( LPCTSTR fileName, TCHAR* mode )
{
	fp = FLStdFileOpen( fileName, mode );
	if( fp )
		return TRUE;
	return FALSE;
}

LPVOID CFileIO::Read(void)
{
	long size = GetLength();
	LPVOID ptr = new BYTE[ size ];
	if( !ptr ) 
		return NULL;

	if( fseek( fp, 0, SEEK_SET ) != 0 )
	{
		SAFE_DELETE_ARRAY( ptr ); 
		return NULL;
	}

	if( fread( ptr, size, 1, fp ) ) 
	{ 
		return ptr;
	}
	SAFE_DELETE_ARRAY( ptr ); 
	return NULL;
}

size_t CFileIO::Read( void *ptr, size_t size, size_t n /*= 1 */ )
{
	return fread( ptr, size, n, fp );
}
int CFileIO::PutWideString( LPCTSTR lpszString )
{
	wchar_t wszBuff[ 1024 ];
	int nLen = strlen( lpszString );

	ATL::_mbstowcsz(wszBuff, lpszString,nLen + 1 );

	return Write( wszBuff, wcslen( wszBuff ) * 2 );
}

#if defined( __CLIENT )
/////////////////////////////////////////////////////////////////////////////
// CResFile
CMapStringToPtr CResFile::m_mapResource;


CResFile::CResFile( LPCTSTR lpszFileName, TCHAR *mode ) 
{ 
	m_bResouceInFile = FALSE;
	m_nFileSize = 0;
	m_nFileBeginPosition = 0;
	m_nFileCurrentPosition = 0;
	m_nFileEndPosition = 0;
	m_bEncryption = FALSE;  
	ZeroMemory( m_szFileName, sizeof( m_szFileName ) );
	Open( lpszFileName, mode ); 
}
CResFile::CResFile() 
{ 
	m_bResouceInFile = FALSE;
	m_nFileSize = 0;
	m_nFileBeginPosition = 0;
	m_nFileCurrentPosition = 0;
	m_nFileEndPosition = 0;
	m_bEncryption = FALSE;  
	ZeroMemory( m_szFileName, sizeof( m_szFileName ) );
}
 
BOOL CResFile::Close( void )
{
	if( !m_bResouceInFile )
		return CFileIO::Close();
	else
	{
		if( INVALID_HANDLE_VALUE != ( HANDLE )m_File.m_hFile )
			m_File.Close();
		return TRUE;
	}
}



void CResFile::AddResource( TCHAR* lpszResName )
{
	//m_mapResource.Add( )
	int nFileHeaderSize = 0;
	int nFileNumber = 0;
	short	nFileNameLength = 0;
	char szFileName[_MAX_FNAME] = "";
	int nFileSize = 0;
	int nFilePosition = 0;
	BYTE byEncryptionKey;
	bool bEncryption;
	TCHAR szFullFileName[ MAX_PATH ];
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], name[ _MAX_FNAME ], ext[_MAX_EXT];
	FLSplitPath( lpszResName, drive, _countof( drive ), dir, _countof( dir ), name, _countof( name ), ext, _countof( ext ) );
	
	CFile file;
	if( file.Open( lpszResName, CFile::modeRead ) == FALSE )
		return;
	file.Read( &byEncryptionKey, sizeof( byEncryptionKey ) );
	file.Read( &bEncryption, sizeof( bEncryption ) );
	file.Read( &nFileHeaderSize, sizeof( int ) );
	
	char *pHeader = new char[ nFileHeaderSize ];
	// 해더는 암호 기본 
	//if( m_bEncryption ) // 암호화 되었다면
	{
		char *pCryptHeader = new char[ nFileHeaderSize ];
		file.Read( pCryptHeader, nFileHeaderSize );
		
		for( int i = 0; i < nFileHeaderSize; i++ )
		{
			pHeader[i] = Decryption( byEncryptionKey, pCryptHeader[ i ] );
		}
		
		SAFE_DELETE_ARRAY( pCryptHeader );
	}
	//else // 암호화 안되었다면
	//{
	//	m_File.Read( pHeader, nFileHeaderSize );
	//}
	int nHeaderPosition = 0;
	char strVersion[8] ="";
	memcpy( strVersion, &pHeader[ nHeaderPosition ], 7 ); nHeaderPosition += 7;
	
	memcpy( &nFileNumber, &pHeader[ nHeaderPosition ], sizeof( short ) ); nHeaderPosition += sizeof( short );
	time_t time_;
	BOOL bFind = FALSE;
	for( int i = 0; i < nFileNumber; i++ )
	{
		memcpy( &nFileNameLength, &pHeader[ nHeaderPosition ], sizeof( short ) ); nHeaderPosition += sizeof( short );
		memcpy( szFileName, &pHeader[ nHeaderPosition ], nFileNameLength ); nHeaderPosition += nFileNameLength;
		memcpy( &nFileSize, &pHeader[ nHeaderPosition ], sizeof( int ) ); nHeaderPosition += sizeof( int );
		memcpy( &time_, &pHeader[ nHeaderPosition ], sizeof( time_t ) ); nHeaderPosition += sizeof( time_t );
		memcpy( &nFilePosition, &pHeader[ nHeaderPosition ], sizeof( int ) ); nHeaderPosition += sizeof( int );
		RESOURCE* lpRes = new RESOURCE;
		ZeroMemory( lpRes, sizeof( RESOURCE ) );
		lpRes->dwOffset = nFilePosition;
		lpRes->dwFileSize = nFileSize;
		lpRes->byEncryptionKey = byEncryptionKey;
		lpRes->bEncryption = bEncryption;
		FLStrcpy( lpRes->szResourceFile, _countof( lpRes->szResourceFile ), lpszResName );
		FLStrLowercase( lpRes->szResourceFile, _countof( lpRes->szResourceFile ) );

		FLStrcpy( szFullFileName, _countof( szFullFileName ), dir );
		FLStrcat( szFullFileName, _countof( szFullFileName ), szFileName );
		FLStrLowercase( szFullFileName, _countof( szFullFileName ) );

		m_mapResource.SetAt( szFullFileName, lpRes );

		/*
		// 찾았다면
		if( 0 == strcmpi( lpszFileName, szFileName ) )
		{
			bFind = TRUE;
			//strcpy( m_szFileName, szFileName );
			//m_nFileSize = nFileSize;
			//m_nFileBeginPosition = nFilePosition;
			//m_nFileCurrentPosition = nFilePosition;
			//m_nFileEndPosition = nFilePosition + nFileSize;
			//m_File.Seek( m_nFileCurrentPosition, CFile::begin );
			break;
		}
		*/
		ZeroMemory( szFileName, sizeof( szFileName ) );
	}
	SAFE_DELETE_ARRAY( pHeader );
}

bool CompareExt( LPCTSTR szName, LPCTSTR szExt )
{
	int nExt = strlen( szExt );
	int n = strlen( szName );
	if( n < nExt )
		return false;
	return ( memcmp( szName + n - nExt, szExt, nExt ) == 0 );
}

void CResFile::ScanResource( LPCTSTR lpszRootPath )
{
	TCHAR szPath[ MAX_PATH ];
	TCHAR szPathTemp[ MAX_PATH ];
	struct _finddata_t c_file;
	long hFile;

	FLStrcpy( szPathTemp, _countof( szPathTemp ), lpszRootPath );
	FLStrcat( szPathTemp, _countof( szPathTemp ), "*.*" );
	if( ( hFile = _findfirst( szPathTemp, &c_file ) ) != -1L )
	{
		do
		{
			FLStrLowercase( c_file.name, _countof( c_file.name ) );
			FLStrcpy( szPath, _countof( szPath ), lpszRootPath );
			if( c_file.attrib & _A_SUBDIR )//if( CFile::GetStatus( name, fileStatus) == TRUE )
			{
				if( strcmp( c_file.name, "." ) && strcmp( c_file.name, ".." ) 
#ifdef __INTERNALSERVER
					&& strcmp( c_file.name, ".svn" )
#endif // __INTERNALSERVER
					)
				{
					FLStrcat( szPath, _countof( szPath ), c_file.name );
					FLStrcat( szPath, _countof( szPath ), "\\" );
					ScanResource( szPath );
				}
			}
			else
			//if( strstr( c_file.name, ".res" ) != 0 )
			if( CompareExt( c_file.name, ".res" ) )
			{
				FLStrcat( szPath, _countof( szPath ), c_file.name );
				AddResource( szPath  );
			}
		}
		while( _findnext( hFile, &c_file ) == 0 );
			
		_findclose( hFile );
	}	
}


void CResFile::FreeResource()
{
	CString strNameName;
	RESOURCE* lpRes;
	POSITION pos;
	pos = m_mapResource.GetStartPosition();
	while( pos )
	{
		m_mapResource.GetNextAssoc( pos, strNameName, (void*&) lpRes );
		SAFE_DELETE( lpRes );
	}
}
BOOL CResFile::Open( LPCTSTR lpszFileName, TCHAR *mode )
{
//	FLTRACE_LOG( PROGRAM_NAME, _T( "%s" ), lpszFileName );

	if( CS_VER1 == _GetContentState( CT_LOADSEQ_IMAGEDATA ) )
	{
		if( CFileIO::Open( lpszFileName, mode ) == TRUE )
			return TRUE;
	}

	char szSerchPath[MAX_PATH];
	GetCurrentDirectory( sizeof( szSerchPath ), szSerchPath );
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], name[ _MAX_FNAME ], ext[_MAX_EXT];
	FLSplitPath( lpszFileName, drive, _countof( drive ), dir, _countof( dir ), name, _countof( name ), ext, _countof( ext ) );
	
	TCHAR szFileName[ MAX_PATH ];
	RESOURCE* lpRes;
	FLStrcpy( szFileName, _countof( szFileName ), lpszFileName );
	FLStrLowercase( szFileName, _countof( szFileName ) );
	m_bResouceInFile = FALSE;
	if( m_mapResource.Lookup( szFileName, (void*&) lpRes ) )
	{
		CFileException fileExc;
		if( m_File.Open( lpRes->szResourceFile, CFile::modeRead | CFile::shareDenyNone, &fileExc ) )
		{
			m_nFileSize = lpRes->dwFileSize;
			m_nFileBeginPosition = lpRes->dwOffset;
			m_nFileCurrentPosition = lpRes->dwOffset;
			m_nFileEndPosition = lpRes->dwOffset + m_nFileSize;
			m_byEncryptionKey = lpRes->byEncryptionKey;
			m_bEncryption = lpRes->bEncryption;
			m_File.Seek( m_nFileCurrentPosition, CFile::begin );
			m_bResouceInFile = TRUE;
			//m_File.Close();
			return TRUE;
		}

		TCHAR szCause[255];
		fileExc.GetErrorMessage(szCause, 255);

		FLERROR_LOG( PROGRAM_NAME, _T( "CResFile Open Error %s FileName = %s, Resource = %s, CurrentDir = %s" ), 
			szCause,
			szFileName, 
			lpRes->szResourceFile, 
			szSerchPath );

		return FALSE;
	}
	else
	{
		if( CS_VER2 == _GetContentState( CT_LOADSEQ_IMAGEDATA ) )
		{
			if( CFileIO::Open( lpszFileName, mode ) == TRUE )
				return TRUE;
		}
	}
	return FALSE;
}
/*
BOOL CResFile::FindFile(char *szSerchPath, LPCTSTR lpszFileName, TCHAR *mode, int f )
{
	HANDLE hSrch;
	WIN32_FIND_DATA wfd;
	BOOL bResult=TRUE;
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char dir1[_MAX_DIR];
	char dir2[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char filepath[MAX_PATH];
	TCHAR szFileName[_MAX_FNAME];	

	_splitpath( lpszFileName, drive, dir1, szFileName, ext );
	wsprintf( szFileName, "%s%s", szFileName, ext );

	_splitpath( szSerchPath, drive, dir2, fname, ext );
	if( dir1[ 0 ] == '\0' )
		wsprintf( szSerchPath, "*.res" );
	else
		wsprintf( szSerchPath, "%s\\*.res", dir1 );

	_splitpath( szSerchPath, drive, dir, fname, ext );

	if( ( hSrch = FindFirstFile( szSerchPath, &wfd ) ) != INVALID_HANDLE_VALUE )
	{
		while ( bResult ) 
		{
			_tcslwr( wfd.cFileName );
			_splitpath( wfd.cFileName, NULL, NULL, NULL, ext );
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
			{
			} 
			else 
			{
				// .Res 파일을 찾았다면
				if( 0 == strcmpi( ext, ".res" ) )
				{
					wsprintf( filepath, "%s%s%s", drive, dir, wfd.cFileName);
					// .Res 파일 안에서 원하는 파일을 찾았다면
					if( TRUE == FindFileFromResource( filepath, szFileName ) )
					{
						return TRUE;
					}
				}
			}
			bResult = FindNextFile(hSrch,&wfd);
		}
		FindClose( hSrch );
	}
	// 찾는 파일이 외부에 있다면
	//if( 0 == strcmpi( szFileName, wfd.cFileName ) )
	//{
	m_bResouceInFile = FALSE;
	return CFileIO::Open( lpszFileName, mode );
}


BOOL CResFile::FindFileFromResource( char *filepath, LPCTSTR lpszFileName )
{
	int nFileHeaderSize = 0;
	int nFileNumber = 0;
	int nFileNameLength = 0;
	char szFileName[_MAX_FNAME] = "";
	int nFileSize = 0;
	int nFilePosition = 0;
	if( m_File.Open( filepath, CFile::modeRead ) == FALSE )
		return FALSE;
	m_File.Read( &m_byEncryptionKey, sizeof( m_byEncryptionKey ) );
	m_File.Read( &m_bEncryption, sizeof( m_bEncryption ) );
	m_File.Read( &nFileHeaderSize, sizeof( int ) );

	char *pHeader = new char[ nFileHeaderSize ];
	// 해더는 암호 기본 
	//if( m_bEncryption ) // 암호화 되었다면
	{
		char *pCryptHeader = new char[ nFileHeaderSize ];
		m_File.Read( pCryptHeader, nFileHeaderSize );

		for( int i = 0; i < nFileHeaderSize; i++ )
		{
			pHeader[i] = Decryption( m_byEncryptionKey, pCryptHeader[ i ] );
		}
 
		delete [] pCryptHeader;
	}
	//else // 암호화 안되었다면
	//{
	//	m_File.Read( pHeader, nFileHeaderSize );
	//}
	int nHeaderPosition = 0;
	char strVersion[8] ="";
	memcpy( strVersion, &pHeader[ nHeaderPosition ], 7 ); nHeaderPosition += 7;
	//FLTRACE_LOG( PROGRAM_NAME, _T( "1 %s %s %d" ), lpszFileName, strVersion, nHeaderPosition );

	memcpy( &nFileNumber, &pHeader[ nHeaderPosition ], sizeof( short ) ); nHeaderPosition += sizeof( short );
	//	FLTRACE_LOG( PROGRAM_NAME, _T( "1 -1 %s %s %d" ), lpszFileName, strVersion, nFileNumber );
	time_t time_;
	for( int i = 0; i < nFileNumber; i++ )
	{
		//FLTRACE_LOG( PROGRAM_NAME, _T( "2 %s %s %d" ), lpszFileName, szFileName, nFileNumber );

		memcpy( &nFileNameLength, &pHeader[ nHeaderPosition ], sizeof( short ) ); nHeaderPosition += sizeof( short );
		//FLTRACE_LOG( PROGRAM_NAME, _T( "3 %s %s %d" ), lpszFileName, szFileName, nFileNameLength );
		memcpy( szFileName, &pHeader[ nHeaderPosition ], nFileNameLength ); nHeaderPosition += nFileNameLength;
		//FLTRACE_LOG( PROGRAM_NAME, _T( "4 %s %s" ), lpszFileName, szFileName );
		memcpy( &nFileSize, &pHeader[ nHeaderPosition ], sizeof( int ) ); nHeaderPosition += sizeof( int );
		memcpy( &time_, &pHeader[ nHeaderPosition ], sizeof( time_t ) ); nHeaderPosition += sizeof( time_t );
		//FLTRACE_LOG( PROGRAM_NAME, _T( "5 %s %s" ), lpszFileName, szFileName );
		memcpy( &nFilePosition, &pHeader[ nHeaderPosition ], sizeof( int ) ); nHeaderPosition += sizeof( int );
		//FLTRACE_LOG( PROGRAM_NAME, _T( "6 %s %s" ), lpszFileName, szFileName );
		// 찾았다면
		if( 0 == strcmpi( lpszFileName, szFileName ) )
		{
			m_bResouceInFile = TRUE;
			strcpy( m_szFileName, szFileName );
			m_nFileSize = nFileSize;
			m_nFileBeginPosition = nFilePosition;
			m_nFileCurrentPosition = nFilePosition;
			m_nFileEndPosition = nFilePosition + nFileSize;
			m_File.Seek( m_nFileCurrentPosition, CFile::begin );
			break;
		}
		ZeroMemory( szFileName, sizeof( szFileName ) );
	}
	delete [] pHeader;

	if( m_bResouceInFile == TRUE )
		return TRUE;
	else
	{
		m_File.Close();
		return FALSE;
	}
}
*/
LPVOID CResFile::Read( void )
{
	if( !m_bResouceInFile )
		return CFileIO::Read();
	else
	{
		LONG size = m_nFileSize;
		LPVOID ptr = new BYTE[ size ];
		if( !ptr ) 
			return NULL;
		//m_File.Seek( m_nFileCurrentPosition, CFile::begin );
		//m_nFileCurrentPosition 
		m_File.Read( ptr, size );
		m_nFileCurrentPosition += size;
		// 암호화일 경우 암호를 푼다.
		if( IsEncryption() )
		{
			for( int i = 0; i < size;  i++ )
				((BYTE*)ptr)[ i ] = Decryption( m_byEncryptionKey, ((BYTE*)ptr)[ i ] );
		}
		return ptr;
	}
}

size_t CResFile::Read( void *ptr, size_t size, size_t n /* = 1  */ )
{
	if( !m_bResouceInFile )
		return CFileIO::Read( ptr, size, n );
	else
	{
		//m_File.Seek( m_nFileCurrentPosition, CFile::begin );
		size_t size_  = m_File.Read( ptr, size * n );
		m_nFileCurrentPosition += size_;
		// 암호화일 경우 암호를 푼다.
		if( IsEncryption() )
		{
			for( int i = 0; i < (int)( size_ );  i++ )
				((BYTE*)ptr)[ i ] = Decryption( m_byEncryptionKey, ((BYTE*)ptr)[ i ] );
		}

		return size_;
	}
}

long CResFile::GetLength( void )
{
	if( !m_bResouceInFile )
		return CFileIO::GetLength();
	else
		return m_nFileSize;
}

int CResFile::Seek( long offset, int whence )
{
	if( !m_bResouceInFile )
		return CFileIO::Seek( offset, whence );
	else
	{
		if( whence == SEEK_SET )
		{
			m_nFileCurrentPosition = m_nFileBeginPosition + offset;
			m_File.Seek( m_nFileCurrentPosition, CFile::begin );
		}
		else 
		if( whence == SEEK_END )
		{
			m_nFileCurrentPosition = m_nFileEndPosition - offset;
			m_File.Seek( m_nFileCurrentPosition, CFile::end );//CFile::current );
		}
		else 
		if( whence == SEEK_CUR )
		{
			m_nFileCurrentPosition += offset;
			//m_File.GetPosition() + offset
			int n = (int)( m_File.GetPosition() );
			m_File.Seek( offset, CFile::current );
		}
		else
			return -1;
		return m_nFileCurrentPosition;
	}
}

long CResFile::Tell()
{
	if( !m_bResouceInFile )
		return CFileIO::Tell();
	else
		return m_nFileCurrentPosition - m_nFileBeginPosition;
}

char CResFile::GetC( void )
{
	if( !m_bResouceInFile )
		return CFileIO::GetC();
	else
	{
		char c;
		m_File.Read( &c, 1 );
		m_nFileCurrentPosition += sizeof( c );
		// 암호화일 경우 암호를 푼다.
		if( IsEncryption() )
			c = Decryption( m_byEncryptionKey, c );
		return c;
	}
}

WORD CResFile::GetW( void )
{
	if( !m_bResouceInFile )
		return CFileIO::GetW();
	else
	{
		WORD w;
		m_File.Read( &w, sizeof( w ) );
		m_nFileCurrentPosition += sizeof( w );
		if( IsEncryption() )
		{
			BYTE h, l;
			h = Decryption( m_byEncryptionKey, w >> 8 );
			l = Decryption( m_byEncryptionKey, w & 0x00ff );
			return h << 8 | l;
		}
		return w;
	}
}


DWORD CResFile::GetDW( void )
{
	if( !m_bResouceInFile )
		return CFileIO::GetDW();
	else
	{
		DWORD dw;
		m_File.Read( &dw, sizeof( dw ) );
		m_nFileCurrentPosition += sizeof( dw );
		if( IsEncryption() )
		{
			BYTE a,b,c,d;
			a = b = c = d = 0;
			a = Decryption( m_byEncryptionKey, ( a & 0xff000000 ) >> 24 );
			b = Decryption( m_byEncryptionKey, ( b & 0x00ff0000 ) >> 16 );
			c = Decryption( m_byEncryptionKey, ( c & 0x0000ff00 ) >> 8 );
			d = Decryption( m_byEncryptionKey, ( d & 0x000000ff ) );
			return a << 24 | b << 16 | c << 8 | d;
		}
		return dw;
	}
}


int	CResFile::Flush( void )
{
	if( !m_bResouceInFile )
		return CFileIO::Flush();
	else
	{
		m_File.Flush();
		return 0;
	}
}
#endif	// __CLIENT

CFileFinder::CFileFinder()
{
	m_pos = 0;
	m_lHandle = 0;
	m_bResFile = FALSE;
}
CFileFinder::~CFileFinder()
{
}

BOOL CFileFinder::WildCmp( LPCTSTR lpszWild, LPCTSTR lpszString ) 
{
	char* wild = (char*)lpszWild;
    char* string = (char*)lpszString;
	char* cp	= NULL;
	char* mp	= NULL;

	while ((*string) && (*wild != '*')) 
	{
		if ((*wild != *string) && (*wild != '?')) 
		{
			return 0;
		}
		wild++;
		string++;
	}
	while (*string) 
	{
		if (*wild == '*') 
		{
			if (!*++wild) 
			{
				return 1;
			}
			mp = wild;
			cp = string+1;
		} 
		else 
		if ((*wild == *string) || (*wild == '?')) 
		{
			wild++;
			string++;
		} 
		else 
		{
			wild = mp;
			string = cp++;
		}
	}
	while (*wild == '*') 
	{
		wild++;
	}
	return !*wild;
}

BOOL CFileFinder::FindFirst( LPCTSTR lpFilespec, struct _finddata_t *fileinfo )
{
#if defined( __CLIENT )
	CHAR filespec[ MAX_PATH ];
	FLStrcpy( filespec, _countof( filespec ), lpFilespec );
	FLStrLowercase( filespec, _countof( filespec ) );

	CHAR szPath [ MAX_PATH ];
	CHAR szFile[ MAX_PATH ];

	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], name[ _MAX_FNAME ], ext[_MAX_EXT];
	FLSplitPath( lpFilespec, drive, _countof( drive ), dir, _countof( dir ), name, _countof( name ), ext, _countof( ext ) );

	// 드라이브명이 들어 있으면 완전한 path다. 그러므로 그냥 카피 
	if( drive[ 0 ] )
	{
		FLStrcpy( m_szFilespec, _countof( m_szFilespec ), lpFilespec );
	}
	// 드리이브명이 없으면 현재 path의 연장이다. 
	else
	{
		::GetCurrentDirectory( MAX_PATH, szPath );
		FLStrcpy( m_szFilespec, _countof( m_szFilespec ), szPath );
		FLStrcat( m_szFilespec, _countof( m_szFilespec ), "\\" );
		FLStrcat( m_szFilespec, _countof( m_szFilespec ), lpFilespec );
	}
	FLStrLowercase( m_szFilespec, _countof( m_szFilespec ) );

	CString strNameName;
	RESOURCE* lpRes;
	m_pos = CResFile::m_mapResource.GetStartPosition();
	while( m_pos )
	{
		CResFile::m_mapResource.GetNextAssoc( m_pos, strNameName, (void*&) lpRes );
		FLStrcpy( szFile, _countof( szFile ), szPath );
		FLStrcat( szFile, _countof( szFile ), "\\" );
		FLStrcat( szFile, _countof( szFile ), strNameName );
		FLStrLowercase( szFile, _countof( szFile ) );

		if( WildCmp( m_szFilespec, szFile ) )
		{
			FLSplitPath( strNameName, drive, _countof( drive ), dir, _countof( dir ), name, _countof( name ), ext, _countof( ext ) );
			FLStrcpy( fileinfo->name, _countof( fileinfo->name ), name );
			FLStrcat( fileinfo->name, _countof( fileinfo->name ), ext );
			fileinfo->size = lpRes->dwFileSize;
			fileinfo->attrib = _A_NORMAL;
			m_bResFile = TRUE;
			return TRUE;
		}
	}
#endif	// __CLIENT
	m_bResFile = 0;
	m_lHandle = _findfirst( lpFilespec, fileinfo );
	if( m_lHandle == -1 ) 
		return FALSE;
	return TRUE;
}
BOOL CFileFinder::FindNext( struct _finddata_t *fileinfo )
{
#if defined( __CLIENT )
	if( m_bResFile == 1 )
	{
		TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], name[ _MAX_FNAME ], ext[_MAX_EXT];
		CHAR szPath [MAX_PATH];
		CHAR szFile[MAX_PATH];
		CString strNameName;
		RESOURCE* lpRes;
		::GetCurrentDirectory( MAX_PATH, szPath );

		while( m_pos )
		{
			CResFile::m_mapResource.GetNextAssoc( m_pos, strNameName, (void*&) lpRes );
			FLStrcpy( szFile, _countof( szFile ), szPath );
			FLStrcat( szFile, _countof( szFile ), "\\" );
			FLStrcat( szFile, _countof( szFile ), strNameName );
			FLStrLowercase( szFile, _countof( szFile ) );

			if( WildCmp( m_szFilespec, szFile ) )
			{
				FLSplitPath( strNameName, drive, _countof( drive ), dir, _countof( dir ), name, _countof( name ), ext, _countof( ext ) );
				FLStrcpy( fileinfo->name, _countof( fileinfo->name ), name );
				FLStrcat( fileinfo->name, _countof( fileinfo->name ), ext );
				fileinfo->size = lpRes->dwFileSize;
				fileinfo->attrib = _A_NORMAL;
				return TRUE;
			}
		}
		return FALSE;
	}
#endif	// __CLIENT
	if( _findnext( m_lHandle, fileinfo ) == 0 )
		return TRUE;
	return FALSE;
}
void CFileFinder::FindClose()
{
	if( m_bResFile == 0 && m_lHandle != -1 )
		_findclose( m_lHandle );
}
