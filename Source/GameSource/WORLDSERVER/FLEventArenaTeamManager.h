
#ifndef	__FLEVENTARENATEAMMANAGER_H__
#define	__FLEVENTARENATEAMMANAGER_H__


#include "FLEventArenaSpecStruct.h"
#include "FLEventArenaTeam.h"


class	FLEventArenaTeamManager
{
public:

	FLEventArenaTeamManager();
	~FLEventArenaTeamManager();

	bool	AddTeamMember( const TCHAR* pszName, const T_EVENT_ARENA_TEAM_MEMBER_SPEC* pMember );
	bool	Check();

	void	Run();

	void	GetAllTeamInfo( std::vector< T_EVENT_ARENA_TEAM_INFO >& vecTeams )	const;
	void	Clear();

	FLEventArenaTeam*	FindTeam( const TCHAR* pszName )		const;
	FLEventArenaTeam*	FindTeam( DWORD dwTeamID )				const;
	FLEventArenaTeam*	FindMyTeam( const TCHAR* pszCharName )	const;
	FLEventArenaTeam*	FindMyTeam( FLWSUser* pUser )			const;

private:

	FLEventArenaTeam*	NewTeam( const TCHAR* pszName );

private:

	// ÆÀ
	typedef	std::vector< FLEventArenaTeam* >	TEAM_LIST;
	typedef	TEAM_LIST::iterator					TEAM_ITER;
	typedef	TEAM_LIST::const_iterator			TEAM_CITER;

	TEAM_LIST	m_vecTeams;

};







#endif