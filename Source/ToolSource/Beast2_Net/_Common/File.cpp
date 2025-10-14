#include "stdafx.h"

#include <string.h>
#include <stdio.h>
#include "file.h"
#include "xutil.h"
//#include <shlwapi.h>

TCHAR NOW_DIR[ 256 ];

void Exit(void);

void SetNowDir( TCHAR* s )
{
	_tcscpy( NOW_DIR, s );
}

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

BOOL CFileIO::Open( LPCTSTR fileName, TCHAR* mode, int f )
{
	TCHAR name[ 256 ];
	_tcscpy( name, fileName );
	if( ( fp = _tfopen( name, mode ) ) )
		return TRUE;
	return FALSE;
}

/*
LPVOID CFileIO::Read(void)
{
	long size = GetLength();
	LPVOID ptr = new BYTE[ size ];
	if( !ptr ) 
		return NULL;
	if( !fread( ptr, size, 1, fp ) ) 
	{ 
		delete ptr; 
		return NULL;
	}
	return ptr;
}
*/

LPVOID CFileIO::Read(void)
{
	long size = GetLength();
	LPVOID ptr = new BYTE[ size ];
	if( !ptr ) 
		return NULL;

	fseek( fp, 0, SEEK_SET );
	if( fread( ptr, size, 1, fp ) ) 
	{ 
		return ptr;
	}
	delete ptr; 
	return NULL;
	//}
	//return ptr;
}
int CFileIO::PutWideString( LPCTSTR lpszString )
{
	wchar_t wszBuff[ 1024 ];
	int nLen = strlen( lpszString );
	_mbstowcsz( wszBuff, lpszString, nLen + 1 );
	return Write( wszBuff, wcslen( wszBuff ) * 2 );
}
/////////////////////////////////////////////////////////////////////////////

#if defined( __CLIENT ) || defined( __MERGEMASTER ) ||  defined( __PATCHCLIENT )
CResFile::CResFile( LPCTSTR lpszFileName, TCHAR *mode, int f ) 
{ 
	m_bResouceInFile = FALSE;
	m_nFileSize = 0;
	m_nFileBeginPosition = 0;
	m_nFileCurrentPosition = 0;
	m_nFileEndPosition = 0;
	m_bEncryption = FALSE;  
	ZeroMemory( m_szFileName, sizeof( m_szFileName ) );
	Open( lpszFileName, mode, f ); 
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
/*
void CResFile::ScanSubResource( LPCTSTR lpszRootPath )
{
	struct _finddata_t c_file;
	long hFile;
	int i;
	TCHAR path[ 128 ], drive[ 64 ], dir[ 64 ], name[ 64 ], ext[ 64 ];
	_splitpath( s.token, drive, dir, name, ext );
	if( (hFile = _findfirst( s.token, &c_file )) != -1L )
	{
		strcpy( name, dir );
		strcat( name, c_file.name );
		if( !(c_file.attrib & _A_SUBDIR ) )//CFile::GetStatus( name, fileStatus) == TRUE )
		{
			InsertFile.m_mtime = c_file.time_write;//fileStatus.m_mtime;
			InsertFile.FilePath = name;
			InsertFile.FileSize = c_file.size;//fileStatus.m_size;
			g_fv.push_back( InsertFile );
		}
		while( _findnext( hFile, &c_file ) == 0 )
		{
			strcpy( name, dir );
			strcat( name, c_file.name );
			if( !(c_file.attrib & _A_SUBDIR ) )//if( CFile::GetStatus( name, fileStatus) == TRUE )
			{
				InsertFile.m_mtime = c_file.time_write;//fileStatus.m_mtime;
				InsertFile.FilePath = name;
				InsertFile.FileSize = c_file.size;//fileStatus.m_size;
				g_fv.push_back( InsertFile );
			}
		}
		_findclose( hFile );
	}	
}
*/
CMapStringToPtr CResFile::m_mapResource;


void CResFile::AddResource( TCHAR* lpszResName )
{
	//m_mapResource.Add( )
	int nFileHeaderSize = 0;
	int nFileNumber = 0;
	int nFileNameLength = 0;
	char szFileName[_MAX_FNAME] = "";
	int nFileSize = 0;
	int nFilePosition = 0;
	BYTE byEncryptionKey;
	bool bEncryption;
	TCHAR szFullFileName[ _MAX_PATH ];
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], name[ _MAX_FNAME ], ext[_MAX_EXT];
	_splitpath( lpszResName, drive, dir, name, ext );
	
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
		
		delete [] pCryptHeader;
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
		strcpy( lpRes->szResourceFile, lpszResName );
		strlwr( lpRes->szResourceFile );

		strcpy( szFullFileName, dir );
		strcat( szFullFileName, szFileName );
		strlwr( szFullFileName );
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
	delete [] pHeader;
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
	TCHAR szPath[ 128 ];
	TCHAR szPathTemp[ 128 ];
	struct _finddata_t c_file;
	long hFile;

	strcpy( szPathTemp, lpszRootPath );
	strcat( szPathTemp, "*.*" );
	if( ( hFile = _findfirst( szPathTemp, &c_file ) ) != -1L )
	{
		do
		{
			strlwr( c_file.name );
			strcpy( szPath, lpszRootPath );
			if( c_file.attrib & _A_SUBDIR )//if( CFile::GetStatus( name, fileStatus) == TRUE )
			{
				if( strcmp( c_file.name, "." ) && strcmp( c_file.name, ".." ) )
				{
					strcat( szPath, c_file.name );
					strcat( szPath, "\\" );
					ScanResource( szPath );
				}
			}
			else
			//if( strstr( c_file.name, ".res" ) != 0 )
			if( CompareExt( c_file.name, ".res" ) )
			{
				strcat( szPath, c_file.name );
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
		delete lpRes;
	}
}
BOOL CResFile::Open( LPCTSTR lpszFileName, TCHAR *mode, int f /* = 1  */)
{
#ifdef __VFILEOPEN_20050215
	if( CFileIO::Open( lpszFileName, mode, f ) == TRUE )
		return TRUE;
#endif
	char szSerchPath[MAX_PATH];
	GetCurrentDirectory( sizeof( szSerchPath ), szSerchPath );
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], name[ _MAX_FNAME ], ext[_MAX_EXT];
	_splitpath( lpszFileName, drive, dir, name, ext );
	
	TCHAR szFileName[ _MAX_PATH ];
	RESOURCE* lpRes;
	strcpy( szFileName, lpszFileName );
	strlwr( szFileName );
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
#ifndef __MERGEMASTER
		switch( fileExc.m_cause )
		{
			case CFileException::none: ::Error( "FileError - No error occurred" ); break;
			case CFileException::generic: ::Error( "FileError - generic" ); break;	
			case CFileException::fileNotFound: ::Error( "FileError - fileNotFound" ); break;	
			case CFileException::badPath: ::Error( "FileError - badPath" ); break;	
			case CFileException::tooManyOpenFiles: ::Error( "FileError - tooManyOpenFiles" ); break;	
			case CFileException::accessDenied: ::Error( "FileError - accessDenied" ); break;	
			case CFileException::invalidFile: ::Error( "FileError - invalidFile" ); break;	
			case CFileException::removeCurrentDir: ::Error( "FileError - removeCurrentDir" ); break;	
			case CFileException::directoryFull: ::Error( "FileError - directoryFull" ); break;	
			case CFileException::badSeek: ::Error( "FileError - badSeek" ); break;	
			case CFileException::hardIO: ::Error( "FileError - hardIO" ); break;	
			case CFileException::sharingViolation: ::Error( "FileError - sharingViolation" ); break;	
			case CFileException::lockViolation: ::Error( "FileError - lockViolation" ); break;	
			case CFileException::diskFull: ::Error( "FileError - The disk is full." ); break;	
			case CFileException::endOfFile: ::Error( "FileError - The end of file was reached" ); break;	
		}
		::Error( "CResFile Open Error FileName = %s, Resource = %s\n", szFileName, lpRes->szResourceFile );
#endif
		/*
		if( g_Option.m_nToggleScreen )
		{
			FILEOUT( "ScanFile.txt", "CResFile Open Error FileName = %s, Resource = %s\n", szFileName, lpRes->szResourceFile );
			CString strNameName;
			RESOURCE* lpRes2;
			POSITION pos = m_mapResource.GetStartPosition();
			while( pos )
			{
				m_mapResource.GetNextAssoc( pos, strNameName, (void*&) lpRes2 );
				if( !strcmp( lpRes2->szResourceFile, lpRes->szResourceFile) )
					FILEOUT( "ScanFile.txt", "File = %s", strNameName );
			}
		}
		*/
		return FALSE;
	}
#ifdef __VFILEOPEN_20050215
	return FALSE;
#else
	return CFileIO::Open( lpszFileName, mode, f );
#endif
	//return FindFile( szSerchPath, lpszFileName, mode, f );
}

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
	char filepath[_MAX_PATH];
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
				// 디렉토리 생략
				/*
				if (wfd.cFileName[0]!='.') 
				{
					wsprintf(newpath,"%s%s%s\\*.*",drive,dir,wfd.cFileName);
					FindFileRecursive(newpath, fileName );
				}
				*/
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
	return CFileIO::Open( lpszFileName, mode, f );
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
	//FILEOUT( "c:\\debug.txt", "1 %s %s %d \n", lpszFileName, strVersion, nHeaderPosition );

	memcpy( &nFileNumber, &pHeader[ nHeaderPosition ], sizeof( short ) ); nHeaderPosition += sizeof( short );
	//	FILEOUT( "c:\\debug.txt", "1 -1 %s %s %d \n", lpszFileName, strVersion, nFileNumber );
	time_t time_;
	for( int i = 0; i < nFileNumber; i++ )
	{
		//FILEOUT( "c:\\debug.txt", "2 %s %s %d \n", lpszFileName, szFileName, nFileNumber );

		memcpy( &nFileNameLength, &pHeader[ nHeaderPosition ], sizeof( short ) ); nHeaderPosition += sizeof( short );
		//FILEOUT( "c:\\debug.txt", "3 %s %s %d \n", lpszFileName, szFileName, nFileNameLength );
		memcpy( szFileName, &pHeader[ nHeaderPosition ], nFileNameLength ); nHeaderPosition += nFileNameLength;
		//FILEOUT( "c:\\debug.txt", "4 %s %s \n", lpszFileName, szFileName );
		memcpy( &nFileSize, &pHeader[ nHeaderPosition ], sizeof( int ) ); nHeaderPosition += sizeof( int );
		memcpy( &time_, &pHeader[ nHeaderPosition ], sizeof( time_t ) ); nHeaderPosition += sizeof( time_t );
		//FILEOUT( "c:\\debug.txt", "5 %s %s \n", lpszFileName, szFileName );
		memcpy( &nFilePosition, &pHeader[ nHeaderPosition ], sizeof( int ) ); nHeaderPosition += sizeof( int );
		//FILEOUT( "c:\\debug.txt", "6 %s %s \n", lpszFileName, szFileName );
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
			for( int i = 0; i < size_;  i++ )
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
			int n = m_File.GetPosition();
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

#pragma warning( disable : 4700 ) // CResFile::GetDW 함수내의 초기화 warning을 무시합니다.

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
			a = Decryption( m_byEncryptionKey, ( a & 0xff000000 ) >> 24 );
			b = Decryption( m_byEncryptionKey, ( b & 0x00ff0000 ) >> 16 );
			c = Decryption( m_byEncryptionKey, ( c & 0x0000ff00 ) >> 8 );
			d = Decryption( m_byEncryptionKey, ( d & 0x000000ff ) );
			return a << 24 | b << 16 | c << 8 | d;
		}
		return dw;
	}
}
#pragma warning( default : 4700 ) // 다시 되돌립니다.


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
#endif

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
	char* cp,* mp;

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
#if defined( __CLIENT ) || defined( __MERGEMASTER )  || defined( __PATCHCLIENT ) 
	CHAR filespec[ _MAX_PATH ];
	strcpy( filespec, lpFilespec );
	strlwr( filespec );

	CHAR szPath [ _MAX_PATH ];
	CHAR szFile[ _MAX_PATH ];

	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], name[ _MAX_FNAME ], ext[_MAX_EXT];
	_splitpath( lpFilespec, drive, dir, name, ext );

	// 드라이브명이 들어 있으면 완전한 path다. 그러므로 그냥 카피 
	if( drive[ 0 ] )
	{
		_tcscpy( m_szFilespec, lpFilespec );
	}
	// 드리이브명이 없으면 현재 path의 연장이다. 
	else
	{
		::GetCurrentDirectory( _MAX_PATH, szPath );
		_tcscpy( m_szFilespec, szPath );
		_tcscat( m_szFilespec, "\\" );
		_tcscat( m_szFilespec, lpFilespec );
	}
	strlwr( m_szFilespec );

	CString strNameName;
	RESOURCE* lpRes;
	m_pos = CResFile::m_mapResource.GetStartPosition();
	while( m_pos )
	{
		CResFile::m_mapResource.GetNextAssoc( m_pos, strNameName, (void*&) lpRes );
		_tcscpy( szFile, szPath );
		_tcscat( szFile, "\\" );
		_tcscat( szFile, strNameName );
		strlwr( szFile );

		if( WildCmp( m_szFilespec, szFile ) )
		{
			_splitpath( strNameName, drive, dir, name, ext );
			strcpy( fileinfo->name, name );
			strcat( fileinfo->name, ext );
			fileinfo->size = lpRes->dwFileSize;
			fileinfo->attrib = _A_NORMAL;
			m_bResFile = TRUE;
			return TRUE;
		}
	}
#endif
	m_bResFile = 0;
	m_lHandle = _findfirst( lpFilespec, fileinfo );
	if( m_lHandle == -1 ) 
		return FALSE;
	return TRUE;
}
BOOL CFileFinder::FindNext( struct _finddata_t *fileinfo )
{
#if defined( __CLIENT ) || defined( __MERGEMASTER )  || defined( __PATCHCLIENT ) 
	if( m_bResFile == 1 )
	{
		TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], name[ _MAX_FNAME ], ext[_MAX_EXT];
		CHAR szPath [_MAX_PATH];
		CHAR szFile[_MAX_PATH];
		CString strNameName;
		RESOURCE* lpRes;
		::GetCurrentDirectory( _MAX_PATH, szPath );

		while( m_pos )
		{
			CResFile::m_mapResource.GetNextAssoc( m_pos, strNameName, (void*&) lpRes );
			_tcscpy( szFile, szPath );
			_tcscat( szFile, "\\" );
			_tcscat( szFile, strNameName );
			strlwr( szFile );

			if( WildCmp( m_szFilespec, szFile ) )
			{
				_splitpath( strNameName, drive, dir, name, ext );
				strcpy( fileinfo->name, name );
				strcat( fileinfo->name, ext );
				fileinfo->size = lpRes->dwFileSize;
				fileinfo->attrib = _A_NORMAL;
				return TRUE;
			}
		}
		return FALSE;
	}
#endif
	if( _findnext( m_lHandle, fileinfo ) == 0 )
		return TRUE;
	return FALSE;
}
void CFileFinder::FindClose()
{
	if( m_bResFile == 0 && m_lHandle != -1 )
		_findclose( m_lHandle );
}
