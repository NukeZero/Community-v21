#ifndef __CFileIO_H
#define __CFileIO_H

#include <TCHAR.H>
#include <stdio.h>
#include <io.h>

void SetNdf(void);
void ClearNdf(void);
void SetNowDir(char *s);
void MakeNowDir(char *s);

class CFileIO
{
	protected:
	//	FILE* fp;

	public:
		FILE* fp;

		CFileIO( LPCTSTR lpszFileName, TCHAR* mode, int f = 1) { Open( lpszFileName, mode, f ); }
		CFileIO() { fp = 0; } virtual ~CFileIO() { Close(); }
		virtual BOOL  Open( LPCTSTR lpszFileName, TCHAR* mode, int f = 1 );
		virtual BOOL Close(void);
		virtual int	 Flush(void)  { return fflush( fp ); }
		int  Handle(void) { return fileno( fp ); }
		int  Error(void)  { return ferror( fp ); }
		virtual char  GetC(void)  { return (char)getc( fp ); }
		virtual WORD  GetW(void)  { return getwc( fp ); } 
		virtual DWORD GetDW(void) { DWORD dw; fread( &dw, sizeof( dw ), 1, fp ); return dw; }
		virtual long Tell(void)   { return ftell(fp); }
		//char *GetS( TCHAR* s, int n )  { return fgets( s, n, fp ); }
		virtual int  Seek( long offset, int whence ) { return fseek( fp, offset, whence ); }
		int  Eof(void) { return feof( fp ); }
		virtual long GetLength(void) { return _filelength( Handle() ); }
		virtual size_t Read( void *ptr, size_t size, size_t n = 1 )	{ return fread( ptr, size, n, fp ); }
		virtual LPVOID Read(void);
		int  PutC( char c ) { return putc( c, fp ); }
		int  PutW( WORD w ) { return putw( w, fp ); }
		int  PutDW( DWORD dw ) { return fwrite( &dw, sizeof( dw ), 1, fp ); }
		size_t Write( LPVOID ptr, size_t size, size_t n = 1 ) { return fwrite( ptr, size, n, fp ); }
		int PutString( LPCTSTR lpszString ) { return _ftprintf( fp, lpszString ); }
		int PutWideString( LPCTSTR lpszString );
};
//#ifdef __CLIENT
#if defined( __CLIENT ) || defined( __MERGEMASTER )  || defined( __PATCHCLIENT ) 
struct RESOURCE
{
	TCHAR szResourceFile[ 128 ];
	DWORD dwOffset;
	DWORD dwFileSize;
	BYTE  byEncryptionKey;
	bool  bEncryption;
}; 
class CResFile : public CFileIO
{
private:
	CFile m_File;
	bool m_bResouceInFile;
	char m_szFileName[_MAX_FNAME];
	DWORD m_nFileSize;
	DWORD m_nFileBeginPosition; // 시작 위치
	DWORD m_nFileCurrentPosition; // 현재 위치
	DWORD m_nFileEndPosition; // 끝 위치
	bool m_bEncryption;
	BYTE m_byEncryptionKey;

static void AddResource( TCHAR* lpResName );

public:
static CMapStringToPtr m_mapResource;

static BYTE Encryption( BYTE byEncryptionKey, BYTE byData )
	{
		byData = ( byData << 4 ) | ( byData >> 4 );
		return ( ~byData ) ^ byEncryptionKey;
	}
static BYTE Decryption( BYTE byEncryptionKey, BYTE byData )
	{
		byData = ~byData ^ byEncryptionKey;
		return ( byData << 4 ) | ( byData >> 4 );
	}
	CResFile( LPCTSTR lpszFileName, TCHAR *mode, int f = 1 );
	CResFile();

	BOOL IsEncryption() { return m_bEncryption; }
	virtual ~CResFile() { Close(); }
	virtual BOOL Open( LPCTSTR lpszFileName, TCHAR *mode, int f = 1 );
	virtual BOOL Close( void );
	
	virtual LPVOID Read(void);
	virtual size_t Read( void *ptr, size_t size, size_t n = 1 );
	virtual long GetLength(void);
	virtual int  Seek( long offset, int whence );
	virtual long Tell(void);
	
	virtual char  GetC(void);
	virtual WORD  GetW(void);
	virtual DWORD GetDW(void);
	
	virtual int	 Flush(void);
	
	BOOL FindFile(char *szSerchPath, LPCTSTR lpszFileName, TCHAR *mode, int f );
	BOOL FindFileFromResource( char *filepath, LPCTSTR lpszFileName );

static void ScanResource( LPCTSTR lpszRootPath );
static void FreeResource();
};
#else
#define CResFile CFileIO
#endif
class CFileFinder
{
	POSITION m_pos;
	long m_lHandle;
	CHAR m_szFilespec[ 256 ];
	BOOL m_bResFile;
public:
	CFileFinder();
	~CFileFinder();
	BOOL WildCmp( LPCTSTR lpszWild, LPCTSTR lpszString );

	BOOL FindFirst( LPCTSTR lpFilespec, struct _finddata_t *fileinfo );
	BOOL FindNext( struct _finddata_t *fileinfo );
	void FindClose();
};
#endif
