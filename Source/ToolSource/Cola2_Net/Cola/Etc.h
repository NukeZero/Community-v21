#ifndef		__ETC_H__
#define		__ETC_H__

int		xRand( void );
int		xRandom( int num );
void	xSRand( unsigned int seed );

void Message(char *str, ...);
void Error(char *str, ...);

LPCTSTR		GetFileName( LPCTSTR szSrc );
LPCTSTR		GetFileTitle( LPCTSTR szSrc );

inline BOOL IsEmpty( LPCTSTR str )
{
	return (str[0] == '\0') ? TRUE : FALSE;
}

#define		FRAME_PER_SEC		60

BOOL	GetFrameSkip( void );
void	SetFrameSkip( BOOL bFlag );
void	InitFST( void );
BOOL	IsDrawTiming( void );
void	SetFST( void );

//#define		FULLDIR_MODEL	MakePath( DIR_MODEL, "" )
//#define		FULLDIR_ASE		MakePath( DIR_ASE, "" )

void		InitLIB( void );
LPCTSTR		MakePath( LPCTSTR szPath, LPCTSTR szFileName );
LPCTSTR		MakeExePath( LPCTSTR szPath, LPCTSTR szFileName );

#endif

