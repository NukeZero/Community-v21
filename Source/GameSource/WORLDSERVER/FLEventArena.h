
#ifndef	__FLEVENTARENA_H__
#define	__FLEVENTARENA_H__


#include "FLEventArenaSpec.h"
#include "FLEventArenaGame.h"
#include "FLEventArenaTeamManager.h"


class	FLEventArena
{
public:

	FLEventArena( const FLEventArenaSpec* pSpec, FLEventArenaTeamManager* pTeamManager );
	~FLEventArena();

	// 시스템 동작
	void	OnJoinHook( FLWSUser* pUser, DWORD* pWorldID, int* pLayer )	const;

	void	OnLogin( FLWSUser* pUser );
	void	OnLogout( FLWSUser* pUser );

	void	OnPlayerDied( CMover* pMover );
	void	OnPlayerResurrection( CMover* pMover );

	void	Run();

	bool	IsEnemy( CMover* pSrc, CMover* pTarget )	const;
	bool	IsPlaying()	const;

	// 패킷 핸들링
	void	SendTeamChat( FLWSUser* pPlayer, const TCHAR* pszChat );
	void	AcceptCallPlayer( FLWSUser* pPlayer );	// 선수의 대기지역 진입

	void	StartManage( FLWSUser* pManager );
	void	StopManage( FLWSUser* pManager, const TCHAR* szWhere );
	void	StopGame( FLWSUser* pManager, const TCHAR* szWhere );

	void	SetTonement( FLWSUser* pManager, const FLPacketEventArenaManageSetTonementNoti* pPacket );
	void	SetRedTeam( FLWSUser* pManager, const FLPacketEventArenaManageSetRedTeamNoti* pPacket );
	void	SetBlueTeam( FLWSUser* pManager, const FLPacketEventArenaManageSetBlueTeamNoti* pPacket );
	void	CallPlayers( FLWSUser* pManager, const FLPacketEventArenaManageCallPlayersNoti* pPacket );
	void	JoinPlayers( FLWSUser* pManager, const FLPacketEventArenaManageJoinPlayersNoti* pPacket );
	void	StartBattle( FLWSUser* pManager, const FLPacketEventArenaManageBattleStartNoti* pPacket );
	void	EndGame( FLWSUser* pManager, const FLPacketEventArenaManageGameEndNoti* pPacket );
	void	StopGame( FLWSUser* pManager, const FLPacketEventArenaManageGameStopNoti* pPacket );

private:

	const FLEventArenaSpec*		m_pSpec;
	FLEventArenaTeamManager*	m_pTeamManager;
	FLEventArenaGame			m_kGame;

};





#endif