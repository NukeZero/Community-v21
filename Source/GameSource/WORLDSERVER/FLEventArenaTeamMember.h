
#ifndef	__FLEVENTARENATEAMMEMBER_H__
#define	__FLEVENTARENATEAMMEMBER_H__


#include "FLEventArenaSpec.h"


class	FLEventArenaTeamMember
{
public:

	explicit FLEventArenaTeamMember( const T_EVENT_ARENA_TEAM_MEMBER_SPEC* pMember );
	~FLEventArenaTeamMember();

	void	Login( FLWSUser* pUser );
	void	Logout();

	void	Sync( std::vector< T_EVENT_ARENA_TEAM_MEMBER_INFO >& vecChangedMembers );

	void	SendChat( FLWSUser* pUser, const TCHAR* pszChat );

	void	Standby( const T_WORLD_POSITION* pPos );
	void	Join( const T_WORLD_POSITION* pPos, const std::vector< T_EVENT_ARENA_GIVE_ITEM_SPEC >& vecGiveItem );

	bool	CallDeathNPC( DWORD dwNPCID, const FLEventArenaSpec* pSpec, std::vector< CObj* >& vecDeathNPCs );

	void	NextBattle( const T_WORLD_POSITION* pPos );
	void	EndGame( const FLEventArenaSpec* pSpec, DWORD dwTransformMoverID );

	OBJID	GetObjID()	const;
	const T_EVENT_ARENA_TEAM_MEMBER_INFO&	GetInfo()	const;

	bool	IsEqualName( const TCHAR* pszName )	const;
	bool	IsEqualUser( FLWSUser* pUser )		const;

	bool	IsLogined()		const;
	bool	CanStandby()	const;
	bool	IsStandby()		const;
	bool	IsJoined()		const;
	bool	IsDied()		const;

	void	SendPacket( const FLPacket* pPacket );

private:

	void	Update();
	bool	IsChanged()	const;

	void	Clear();

private:

	T_EVENT_ARENA_TEAM_MEMBER_INFO	m_tInfo;
	T_EVENT_ARENA_TEAM_MEMBER_INFO	m_tOldInfo;

	FLWSUser*						m_pUser;					// 유저 포인터

};





#endif