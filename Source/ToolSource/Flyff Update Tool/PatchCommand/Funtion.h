// Funtion.h: interface for the CFuntion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FUNTION_H__6745B350_7088_489C_A5BD_6722CEE65D2C__INCLUDED_)
#define AFX_FUNTION_H__6745B350_7088_489C_A5BD_6722CEE65D2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFuntion  
{
public:
	CFuntion();
	virtual ~CFuntion();

	void	Bagic_Message();
	BOOL	IsDirectory( const char * pszFlag, int nYear, int nMonth, int nDay );
	void	PatchexeFun( const char * pszServer, const char * pszFlag, int nYear, int nMonth, int nDay );
	void	GZip( const char * pszServer );
	void	CopyAndMoveFile( const char * pszServer, int nYear, int nMonth, int nDay );
	void	RemoveReleFile( const char * pszPath );
	void	OneCopyMove( const char * pszPath, const char * pszDirectory, short i = 0 );
};

#endif // !defined(AFX_FUNTION_H__6745B350_7088_489C_A5BD_6722CEE65D2C__INCLUDED_)
