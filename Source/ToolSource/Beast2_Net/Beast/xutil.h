#ifndef		__XUTIL_H__
#define		__XUTIL_H__

extern LARGE_INTEGER	g_llFreq;
extern int				g_nMaxTri;

/////////////////////////////////////////////////////////////////////////////
// random
DWORD		xRand( void );
DWORD		xRandom( DWORD num );
DWORD		xRandom( DWORD min, DWORD max );	// min에서 max까지  
float		xRandomF( float num );
void		xSRand( DWORD seed );
inline int	random( int nNum ) { return rand() % nNum; }

/////////////////////////////////////////////////////////////////////////////
// PATH
LPCTSTR		MakeExePath( LPCTSTR szPath, LPCTSTR szFileName );
LPCTSTR		GetFileName( LPCTSTR szSrc );
LPCTSTR		GetFileTitle( LPCTSTR szSrc );
LPCTSTR		GetFileExt( LPCTSTR szSrc );
LPCTSTR		GetFilePath( LPCTSTR szSrc );

/////////////////////////////////////////////////////////////////////////////
// ERROR
LPCTSTR		Error( LPCTSTR str, ... );
extern		LPCTSTR Message( LPCTSTR str, ... );   // function body in debug.cpp
CString     GetNumberFormatEx( LPCTSTR szNumber );

/////////////////////////////////////////////////////////////////////////////
// ETC
enum  OSTYPE
{
	WINDOWS_UNKNOWN,
	WINDOWS_NT351,
	WINDOWS_95,
	WINDOWS_NT,
	WINDOWS_98,
	WINDOWS_ME,
	WINDOWS_2000,
	WINDOWS_XP,
	WINDOWS_SERVER_2003
};

extern OSTYPE g_osVersion;

LPCTSTR		GetCPUInfo( void ) ;
void		InitLIB( void );
#define		SWAP( A, B )	{ A ^= B; B ^= A; A ^= B; }
inline BOOL IsEmpty( LPCTSTR str )
{
	return (str[0] == '\0') ? TRUE : FALSE;
}

void MakeEven( long& x );

/////////////////////////////////////////////////////////////////////////////
// MATH
inline	float xMathCCW( float x1, float y1, float x2, float y2 )
{
	return x1 * y2 - y1 * x2;
}

/////////////////////////////////////////////////////////////////////////////

#define		CHECK1()	/##/
#define		CHECK2(A)	/##/

#endif
