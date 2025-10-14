
#ifndef	__FLEVENTARENASPECLOADER_H__
#define	__FLEVENTARENASPECLOADER_H__


#include "FLEventArenaSpec.h"
#include "FLEventArenaTeamManager.h"


class	FLEventArenaSpecLoader
{
public:

	FLEventArenaSpecLoader();
	~FLEventArenaSpecLoader();

	bool	Load( const TCHAR* pszConfigFile, FLEventArenaSpec* pSpec, FLEventArenaTeamManager* pTeamManager );

private:

	bool	LoadTeamList( const TCHAR* pszTeamList, FLEventArenaTeamManager* pTeamManager );
	bool	LoadTonementList( const TCHAR* pszTonementList, FLEventArenaSpec* pSpec );
	bool	LoadGiveItemList( const TCHAR* pszGiveItemList, FLEventArenaSpec* pSpec );
	bool	LoadTimeConfig( const TCHAR* pszTimeConfig, FLEventArenaSpec* pSpec );
	bool	LoadStartPositionList( const TCHAR* pszStartPositionList, FLEventArenaSpec* pSpec );
	bool	LoadSummonList( const TCHAR* pszSummonList, FLEventArenaSpec* pSpec );

};





#endif