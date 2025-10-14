
#ifndef	__FLEVENTARENATEAM_H__
#define	__FLEVENTARENATEAM_H__


#include "FLEventArenaSpecStruct.h"
#include "FLEventArenaTeamMember.h"


class	FLEventArenaTeam
{
private:

	enum
	{
		MIN_MEMBER	= 2,
		MAX_MEMBER	= 8,
		MAX_PARTY	= 4,
	};

public:

	explicit FLEventArenaTeam( const T_EVENT_ARENA_TEAM_INFO* pInfo );
	~FLEventArenaTeam();

	// build
	bool	AddMember( const T_EVENT_ARENA_TEAM_MEMBER_SPEC* pMember );
	bool	Check()	const;

	bool	OnLogin( FLWSUser* pUser );
	bool	OnLogout( FLWSUser* pUser );

	void	Sync();

	void	SendTeamChat( FLWSUser* pUser, const TCHAR* pszChat );

	void	StartStandby( DWORD dwCallWaitTime );
	void	Standby( const T_WORLD_POSITION* pPos );
	bool	Standby( FLWSUser* pUser, const T_WORLD_POSITION* pPos );

	void	Join( const T_WORLD_POSITION* pPos, const std::vector< T_EVENT_ARENA_GIVE_ITEM_SPEC >& vecGiveItem );

	bool	CallDeathNPC( DWORD dwNPCID, const FLEventArenaSpec* pSpec, std::vector< CObj* >& vecDeathNPCs );

	void	NextBattle( const T_WORLD_POSITION* pPos );
	void	EndGame( const FLEventArenaSpec* pSpec, DWORD dwTransformMoverID );

	DWORD							GetID()	const;
	const TCHAR*					GetName()	const;
	const T_EVENT_ARENA_TEAM_INFO*	GetInfo()	const;

	void	GetStream( FLEventArenaTeamStream* pStream )	const;
	void	GetAllMemberName( std::vector< tstring >& vecMembers )	const;
	void	GetSurvivalMembers( std::vector< tstring >& vecSurvivalMembers )	const;
	int		GetSurvivalMembersCount()	const;

	size_t	GetJoinedMemberCount()	const;

	bool	IsEqualName( const TCHAR* pszName )		const;

	bool	IsMember( FLWSUser* pUser )				const;
	bool	IsMember( const TCHAR* pszCharName )	const;

	bool	IsAllStandby()	const;
	bool	IsJoined( FLWSUser* pUser )	const;
	bool	IsAllDied()	const;

private:

	void	SendPacket( const FLPacket* pPacket );

	FLEventArenaTeamMember*	Find( const TCHAR* pszName )	const;
	FLEventArenaTeamMember*	Find( FLWSUser* pUser )			const;

private:

	typedef	std::vector< FLEventArenaTeamMember* >	MEMBER_LIST;
	typedef	MEMBER_LIST::iterator					MEMBER_ITER;
	typedef	MEMBER_LIST::const_iterator				MEMBER_CITER;

	T_EVENT_ARENA_TEAM_INFO	m_tInfo;		// ÆÀ Á¤º¸
	MEMBER_LIST				m_vecMembers;	// ÆÀ ¸É¹ö


};







#endif