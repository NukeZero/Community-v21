#ifndef		__XUTIL_H__
#define		__XUTIL_H__

extern LARGE_INTEGER	g_llFreq;
extern int				g_nMaxTri;

/////////////////////////////////////////////////////////////////////////////
// random
/*
DWORD		xRand( void );
DWORD		xRandom( DWORD num );
*/
extern	DWORD	g_next;
inline DWORD	xRand( void )
{
	g_next = g_next * 1103515245 + 12345;// + nRandomSeed[ i++ ];
	return g_next;	// 억단위 확률이 필요해서바꿈.
}

inline DWORD xRandom( DWORD num )
{
	if( num == 0 )
	{
		return 0;
	}
	return  xRand() % num;
}

DWORD		xRandom( DWORD min, DWORD max );	// min에서 max까지  
float		xRandomF( float num );
void		xSRand( DWORD seed );
inline int	random( int nNum ) { return rand() % nNum; }

/////////////////////////////////////////////////////////////////////////////
// PATH
void	GetFileName( LPCTSTR szSrc, LPTSTR szFileName, size_t cchFileName );
void	GetFileTitle( LPCTSTR szSrc, LPTSTR szFileTitle, size_t cchFileTitle );
void	GetFileExt( LPCTSTR szSrc, LPTSTR szFileExt, size_t cchFileExt );
void	GetFilePath( LPCTSTR szSrc, LPTSTR szFilePath, size_t cchFilePath );

/////////////////////////////////////////////////////////////////////////////
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

void	MakeEven( long& x );
//int		StringFind( const char* string, int c );
void	StringTrimRight( char* szString );


/////////////////////////////////////////////////////////////////////////////
// MATH
inline	float xMathCCW( float x1, float y1, float x2, float y2 )
{
	return x1 * y2 - y1 * x2;
}



//int				VMessageBox( const UINT nType, const TCHAR * fmt, ... );



#endif		__XUTIL_H__
