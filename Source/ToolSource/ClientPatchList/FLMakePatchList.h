#ifndef __FLMAKEPATCHLIST_H__
#define __FLMAKEPATCHLIST_H__

#include "FLDes.h"

class FLMakePatchList
{
public:
	static FLMakePatchList* GetInstance();


	FLMakePatchList();
	~FLMakePatchList();

	bool	LoadScript();
	bool	FindPatchFile( const TCHAR* szFindPath );
	const TCHAR*	GetResDir();


	bool	MakePatchListDecrypt();

private:
	void	Init();
	void	InitKey();
	bool	MakePatchList( const TCHAR* szFullFileName );
	void	Encrypt( const char* szIn, char* szOut );
	void	Decrypt( const char* szIn, char* szOut );

	

private:
	TCHAR	m_szResClientPath[_MAX_PATH];
	int		m_nFlyffVersion;
	des3_context m_tCtx3;
};

#define g_xMakePatchList	FLMakePatchList::GetInstance()

#endif // __FLMAKEPATCHLIST_H__