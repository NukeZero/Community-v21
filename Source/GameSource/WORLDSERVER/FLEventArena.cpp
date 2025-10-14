
#include "StdAfx.h"
#include "FLEventArena.h"



FLEventArena::FLEventArena( const FLEventArenaSpec* pSpec, FLEventArenaTeamManager* pTeamManager )
	: m_pSpec( pSpec )
	, m_pTeamManager( pTeamManager )
	, m_kGame( pSpec )
{
}

FLEventArena::~FLEventArena()
{
	m_pSpec = NULL;
	m_pTeamManager = NULL;
}

void	FLEventArena::OnJoinHook( FLWSUser* pUser, DWORD* pWorldID, int* pLayer )	const
{
	const T_WORLD_POSITION* pStartPos = m_pSpec->GetStartPos();
	if( pStartPos != NULL )
	{
		*pWorldID = pStartPos->dwWorldID;
#ifdef __LAYER_1015
		*pLayer = nDefaultLayer;
#endif // __LAYER_1015

		pUser->SetPos( pStartPos->tPos );
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "이벤트 아레나 시작 지점이 없습니다." ) );
	}
}

void	FLEventArena::OnLogin( FLWSUser* pUser )
{
	if( IsValidObj( pUser ) )
	{
		FLEventArenaTeam* pTeam = m_pTeamManager->FindMyTeam( pUser->GetName() );
		if( pTeam != NULL )
		{
			pTeam->OnLogin( pUser );
		}

		m_kGame.OnLogin( pUser );
	}
}

void	FLEventArena::OnLogout( FLWSUser* pUser )
{
	if( IsValidObj( pUser ) )
	{
		bool bJoined = m_kGame.IsJoined( pUser );
		FLEventArenaTeam* pTeam = m_pTeamManager->FindMyTeam( pUser );
		if( pTeam != NULL )
		{
			pTeam->OnLogout( pUser );
		}

		m_kGame.OnLogout( pUser, bJoined );
	}
}

void	FLEventArena::OnPlayerDied( CMover* pMover )
{
	if( IsValidObj( pMover ) )
	{
		m_kGame.OnPlayerDied( pMover );
	}
}

void	FLEventArena::OnPlayerResurrection( CMover* pMover )
{
	if( IsValidObj( pMover ) )
	{
		m_kGame.OnPlayerResurrection( pMover );
	}
}

void	FLEventArena::Run()
{
	m_pTeamManager->Run();
	m_kGame.Run();
}

bool	FLEventArena::IsEnemy( CMover* pSrc, CMover* pTarget )	const
{
	return m_kGame.IsEnemy( pSrc, pTarget );
}

bool	FLEventArena::IsPlaying()	const
{
	return m_kGame.IsPlaying();
}

void	FLEventArena::SendTeamChat( FLWSUser* pPlayer, const TCHAR* pszChat )
{
	FLEventArenaTeam* pTeam = m_pTeamManager->FindMyTeam( pPlayer );
	if( pTeam != NULL )
	{
		pTeam->SendTeamChat( pPlayer, pszChat );
	}
}

void	FLEventArena::AcceptCallPlayer( FLWSUser* pPlayer )
{
	if( IsValidObj( pPlayer ) )
	{
		m_kGame.AcceptCallPlayer( pPlayer );
	}
}

void	FLEventArena::StartManage( FLWSUser* pManager )
{
	m_kGame.StartManage( pManager, m_pTeamManager );
}

void	FLEventArena::StopManage( FLWSUser* pManager, const TCHAR* szWhere )
{
	m_kGame.StopManage( pManager, szWhere );
}

void	FLEventArena::StopGame( FLWSUser* pManager, const TCHAR* szWhere )
{
	m_kGame.StopGame( pManager, szWhere );
}

void	FLEventArena::SetTonement( FLWSUser* pManager, const FLPacketEventArenaManageSetTonementNoti* pPacket )
{
	const T_EVENT_ARENA_TONEMENT* pTonement = m_pSpec->GetTonement( pPacket->dwTonementID );
	if( pTonement != NULL )
	{
		m_kGame.SetTonement( pManager, pTonement );
	}
}

void	FLEventArena::SetRedTeam( FLWSUser* pManager, const FLPacketEventArenaManageSetRedTeamNoti* pPacket )
{
	FLEventArenaTeam* pTeam = NULL;
	if( pPacket->dwRedTeamID != 0 )
	{
		pTeam = m_pTeamManager->FindTeam( pPacket->dwRedTeamID );
	}

	m_kGame.SetRedTeam( pManager, pTeam );
}

void	FLEventArena::SetBlueTeam( FLWSUser* pManager, const FLPacketEventArenaManageSetBlueTeamNoti* pPacket )
{
	FLEventArenaTeam* pTeam = NULL;
	if( pPacket->dwBlueTeamID != 0 )
	{
		pTeam = m_pTeamManager->FindTeam( pPacket->dwBlueTeamID );
	}

	m_kGame.SetBlueTeam( pManager, pTeam );
}

void	FLEventArena::CallPlayers( FLWSUser* pManager, const FLPacketEventArenaManageCallPlayersNoti* /*pPacket*/ )
{
	m_kGame.CallPlayers( pManager );
}

void	FLEventArena::JoinPlayers( FLWSUser* pManager, const FLPacketEventArenaManageJoinPlayersNoti* /*pPacket*/ )
{
	m_kGame.JoinPlayers( pManager );
}

void	FLEventArena::StartBattle( FLWSUser* pManager, const FLPacketEventArenaManageBattleStartNoti* /*pPacket*/ )
{
	m_kGame.StartBattle( pManager );
}

void	FLEventArena::EndGame( FLWSUser* pManager, const FLPacketEventArenaManageGameEndNoti* /*pPacket*/ )
{
	m_kGame.EndGame( pManager );
}

void	FLEventArena::StopGame( FLWSUser* pManager, const FLPacketEventArenaManageGameStopNoti* /*pPacket*/ )
{
	m_kGame.StopGame( pManager, _T( "UICommand" ) );
}
