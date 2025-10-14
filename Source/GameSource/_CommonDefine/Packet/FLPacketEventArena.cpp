
#include "StdAfx.h"
#include "FLPacketEventArena.h"

#include "../../_Network/MsgHdr.h"


FLEventArenaTeamStream::FLEventArenaTeamStream()
{
	//::mem_set( &tTeamInfo, 0, sizeof( tTeamInfo ) );
}

bool	FLEventArenaTeamStream::Serialize( CAr& kAr )	const
{
	kAr << tTeamInfo;

	BYTE byCount = 0;
	ptrdiff_t nCountPos = kAr.ReserveSpace( sizeof( byCount ) );

	std::vector< T_EVENT_ARENA_TEAM_MEMBER_INFO >::const_iterator pos = vecMembers.begin();
	std::vector< T_EVENT_ARENA_TEAM_MEMBER_INFO >::const_iterator end = vecMembers.end();

	for( ; pos != end; ++pos )
	{
		kAr << *pos;
		++byCount;
	}

	kAr.WriteReservedSpace( nCountPos, &byCount, sizeof( byCount ) );
	return true;
}

bool	FLEventArenaTeamStream::Deserialize( CAr& kAr )
{
	kAr >> tTeamInfo;

	BYTE byCount = 0;
	kAr >> byCount;
	for( int i = 0; i < byCount; ++i )
	{
		T_EVENT_ARENA_TEAM_MEMBER_INFO tMember;
		kAr >> tMember;
		vecMembers.push_back( tMember );
	}

	return true;
}

FLEventArenaBattleResultInfo::FLEventArenaBattleResultInfo()
{
	::memset( &tTonement, 0, sizeof( tTonement ) );
	::memset( &tRedTeam, 0, sizeof( tRedTeam ) );
	::memset( &tBlueTeam, 0, sizeof( tBlueTeam ) );

	::memset( &tBattleStart, 0, sizeof( tBattleStart ) );
	::memset( &tBattleEnd, 0, sizeof( tBattleEnd ) );

	u64BattleTime = 0;

	nWinTeam = 0;

	nRedWinCount = 0;
	nBlueWinCount = 0;
}

bool	FLEventArenaBattleResultInfo::Serialize( CAr& kAr )	const
{
	kAr << tTonement;
	kAr << tRedTeam;
	kAr << tBlueTeam;

	kAr << tBattleStart;
	kAr << tBattleEnd;

	kAr << u64BattleTime;

	kAr << nWinTeam;
	kAr.WriteString( strWinTeam.c_str() );

	kAr << nRedWinCount;
	kAr << nBlueWinCount;

	{
		BYTE byCount = 0;
		ptrdiff_t nCountPos = kAr.ReserveSpace( sizeof( byCount ) );

		std::vector< tstring >::const_iterator pos = vecRedSurvivalMembers.begin();
		std::vector< tstring >::const_iterator end = vecRedSurvivalMembers.end();

		for( ; pos != end; ++pos )
		{
			const tstring& strName = (*pos);
			kAr.WriteString( strName.c_str() );
			++byCount;
		}

		kAr.WriteReservedSpace( nCountPos, &byCount, sizeof( byCount ) );
	}

	{
		BYTE byCount = 0;
		ptrdiff_t nCountPos = kAr.ReserveSpace( sizeof( byCount ) );

		std::vector< tstring >::const_iterator pos = vecBlueSurvivalMembers.begin();
		std::vector< tstring >::const_iterator end = vecBlueSurvivalMembers.end();

		for( ; pos != end; ++pos )
		{
			const tstring& strName = (*pos);
			kAr.WriteString( strName.c_str() );
			++byCount;
		}

		kAr.WriteReservedSpace( nCountPos, &byCount, sizeof( byCount ) );
	}

	return true;
}

bool	FLEventArenaBattleResultInfo::Deserialize( CAr& kAr )
{
	kAr >> tTonement;
	kAr >> tRedTeam;
	kAr >> tBlueTeam;

	kAr >> tBattleStart;
	kAr >> tBattleEnd;

	kAr >> u64BattleTime;

	kAr >> nWinTeam;
	TCHAR szWinTeamName[MAX_EVENT_ARENA_TEAM_NAME] = _T( "" );
	kAr.ReadString( szWinTeamName, _countof( szWinTeamName ) );
	strWinTeam = szWinTeamName;

	kAr >> nRedWinCount;
	kAr >> nBlueWinCount;

	{
		BYTE byCount = 0;
		kAr >> byCount;
		for( int i = 0; i < byCount; ++i )
		{
			TCHAR szName[MAX_NAME] = _T( "" );
			kAr.ReadString( szName, _countof( szName ) );
			vecRedSurvivalMembers.push_back( szName );
		}
	}

	{
		BYTE byCount = 0;
		kAr >> byCount;
		for( int i = 0; i < byCount; ++i )
		{
			TCHAR szName[MAX_NAME] = _T( "" );
			kAr.ReadString( szName, _countof( szName ) );
			vecBlueSurvivalMembers.push_back( szName );
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
FLSnapshotEventArenaTeamInfoNoti::FLSnapshotEventArenaTeamInfoNoti() :
	FLPacket( SNAPSHOTTYPE_EVENT_ARENA_TEAM_INFO_NOTI )
{
}

bool	FLSnapshotEventArenaTeamInfoNoti::BodySerialize( CAr& kAr )	const
{
	return kTeamInfo.Serialize( kAr );
}

bool	FLSnapshotEventArenaTeamInfoNoti::BodyDeserialize( CAr& kAr )
{
	return kTeamInfo.Deserialize( kAr );
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
FLSnapshotEventArenaTeamMemberUpdateNoti::FLSnapshotEventArenaTeamMemberUpdateNoti() :
	FLPacket( SNAPSHOTTYPE_EVENT_ARENA_TEAM_MEMBER_UPDATE_NOTI )
{
}

bool	FLSnapshotEventArenaTeamMemberUpdateNoti::BodySerialize( CAr& kAr )	const
{
	BYTE byCount = 0;
	ptrdiff_t nCountPos = kAr.ReserveSpace( sizeof( byCount ) );

	std::vector< T_EVENT_ARENA_TEAM_MEMBER_INFO >::const_iterator pos = vecChangedMembers.begin();
	std::vector< T_EVENT_ARENA_TEAM_MEMBER_INFO >::const_iterator end = vecChangedMembers.end();

	for( ; pos != end; ++pos )
	{
		kAr << *pos;
		++byCount;
	}

	kAr.WriteReservedSpace( nCountPos, &byCount, sizeof( byCount ) );
	return true;
}

bool	FLSnapshotEventArenaTeamMemberUpdateNoti::BodyDeserialize( CAr& kAr )
{
	BYTE byCount = 0;
	kAr >> byCount;
	for( int i = 0; i < byCount; ++i )
	{
		T_EVENT_ARENA_TEAM_MEMBER_INFO tMember;
		kAr >> tMember;
		vecChangedMembers.push_back( tMember );
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
FLSnapshotEventArenaCallPlayerNoti::FLSnapshotEventArenaCallPlayerNoti() :
	FLPacket( SNAPSHOTTYPE_EVENT_ARENA_CALL_PLAYER_NOTI )
{
	dwWaitTimeout = 0;
}

bool	FLSnapshotEventArenaCallPlayerNoti::BodySerialize( CAr& kAr )	const
{
	kAr << dwWaitTimeout;
	return true;
}

bool	FLSnapshotEventArenaCallPlayerNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> dwWaitTimeout;
	return true;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
FLSnapshotEventArenaGameJoinTeamInfoNoti::FLSnapshotEventArenaGameJoinTeamInfoNoti() :
	FLPacket( SNAPSHOTTYPE_EVENT_ARENA_GAME_JOIN_TEAM_INFO_NOTI )
{
}

bool	FLSnapshotEventArenaGameJoinTeamInfoNoti::BodySerialize( CAr& kAr )	const
{
	{
		BYTE byCount = 0;
		ptrdiff_t nCountPos = kAr.ReserveSpace( sizeof( byCount ) );

		std::vector< tstring >::const_iterator pos = vecRedTeamMembers.begin();
		std::vector< tstring >::const_iterator end = vecRedTeamMembers.end();

		for( ; pos != end; ++pos )
		{
			const tstring& strName = (*pos);
			kAr.WriteString( strName.c_str() );
			++byCount;
		}

		kAr.WriteReservedSpace( nCountPos, &byCount, sizeof( byCount ) );
	}

	{
		BYTE byCount = 0;
		ptrdiff_t nCountPos = kAr.ReserveSpace( sizeof( byCount ) );

		std::vector< tstring >::const_iterator pos = vecBlueTeamMembers.begin();
		std::vector< tstring >::const_iterator end = vecBlueTeamMembers.end();

		for( ; pos != end; ++pos )
		{
			const tstring& strName = (*pos);
			kAr.WriteString( strName.c_str() );
			++byCount;
		}

		kAr.WriteReservedSpace( nCountPos, &byCount, sizeof( byCount ) );
	}

	return true;
}

bool	FLSnapshotEventArenaGameJoinTeamInfoNoti::BodyDeserialize( CAr& kAr )
{
	{
		BYTE byCount = 0;
		kAr >> byCount;
		for( int i = 0; i < byCount; ++i )
		{
			TCHAR szName[MAX_NAME] = _T( "" );
			kAr.ReadString( szName, _countof( szName ) );
			vecRedTeamMembers.push_back( szName );
		}
	}

	{
		BYTE byCount = 0;
		kAr >> byCount;
		for( int i = 0; i < byCount; ++i )
		{
			TCHAR szName[MAX_NAME] = _T( "" );
			kAr.ReadString( szName, _countof( szName ) );
			vecBlueTeamMembers.push_back( szName );
		}
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
FLSnapshotEventArenaBattleResultNoti::FLSnapshotEventArenaBattleResultNoti() :
	FLPacket( SNAPSHOTTYPE_EVENT_ARENA_BATTLE_RESULT_NOTI )
{
	nBattleWinTeam = 0;

	::memset( szRedName, 0, sizeof( szRedName ) );
	nRedWinCount = 0;

	::memset( szBlueName, 0, sizeof( szBlueName ) );
	nBlueWinCount = 0;

	dwWinnerDisplayTime = 0;
}

bool	FLSnapshotEventArenaBattleResultNoti::BodySerialize( CAr& kAr )	const
{
	kAr << nBattleWinTeam;

	kAr.WriteString( szRedName );
	kAr << nRedWinCount;

	kAr.WriteString( szBlueName );
	kAr << nBlueWinCount;

	kAr << dwWinnerDisplayTime;
	return true;
}

bool	FLSnapshotEventArenaBattleResultNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> nBattleWinTeam;

	kAr.ReadString( szRedName, _countof( szRedName ) );
	kAr >> nRedWinCount;

	kAr.ReadString( szBlueName, _countof( szBlueName ) );
	kAr >> nBlueWinCount;

	kAr >> dwWinnerDisplayTime;
	return true;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
FLSnapshotEventArenaGameResultNoti::FLSnapshotEventArenaGameResultNoti() :
	FLPacket( SNAPSHOTTYPE_EVENT_ARENA_GAME_RESULT_NOTI )
{
	nGameWinTeam = 0;

	::memset( szRedName, 0, sizeof( szRedName ) );
	nRedWinCount = 0;

	::memset( szBlueName, 0, sizeof( szBlueName ) );
	nBlueWinCount = 0;

	dwWinnerDisplayTime = 0;
}

bool	FLSnapshotEventArenaGameResultNoti::BodySerialize( CAr& kAr )	const
{
	kAr << nGameWinTeam;

	kAr.WriteString( szRedName );
	kAr << nRedWinCount;

	kAr.WriteString( szBlueName );
	kAr << nBlueWinCount;

	kAr << dwWinnerDisplayTime;
	return true;
}

bool	FLSnapshotEventArenaGameResultNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> nGameWinTeam;

	kAr.ReadString( szRedName, _countof( szRedName ) );
	kAr >> nRedWinCount;

	kAr.ReadString( szBlueName, _countof( szBlueName ) );
	kAr >> nBlueWinCount;

	kAr >> dwWinnerDisplayTime;
	return true;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
FLSnapshotEventArenaGameScoreboardNoti::FLSnapshotEventArenaGameScoreboardNoti() :
	FLPacket( SNAPSHOTTYPE_EVENT_ARENA_GAME_SCOREBOARD_NOTI )
{
	::memset( szTonementName, 0, sizeof( szTonementName ) );
	::memset( szTonementDesc, 0, sizeof( szTonementDesc ) );

	byGameState = EVENT_ARENA_GAME_STATE_NONE;

	::memset( szRedTeamName, 0, sizeof( szRedTeamName ) );
	nRedWinCount = 0;
	nRedSurvivalMembers = 0;

	::memset( szBlueTeamName, 0, sizeof( szBlueTeamName ) );
	nBlueWinCount = 0;
	nBlueSurvivalMembers = 0;
}

bool	FLSnapshotEventArenaGameScoreboardNoti::BodySerialize( CAr& kAr )	const
{
	kAr.WriteString( szTonementName );
	kAr.WriteString( szTonementDesc );

	kAr << byGameState;

	kAr.WriteString( szRedTeamName );
	kAr << nRedWinCount;
	kAr << nRedSurvivalMembers;

	kAr.WriteString( szBlueTeamName );
	kAr << nBlueWinCount;
	kAr << nBlueSurvivalMembers;
	return true;
}

bool	FLSnapshotEventArenaGameScoreboardNoti::BodyDeserialize( CAr& kAr )
{
	kAr.ReadString( szTonementName, _countof( szTonementName ) );
	kAr.ReadString( szTonementDesc, _countof( szTonementDesc ) );

	kAr >> byGameState;

	kAr.ReadString( szRedTeamName, _countof( szRedTeamName ) );
	kAr >> nRedWinCount;
	kAr >> nRedSurvivalMembers;

	kAr.ReadString( szBlueTeamName, _countof( szBlueTeamName ) );
	kAr >> nBlueWinCount;
	kAr >> nBlueSurvivalMembers;
	return true;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
FLSnapshotEventArenaBattleResultLogNoti::FLSnapshotEventArenaBattleResultLogNoti() :
	FLPacket( SNAPSHOTTYPE_EVENT_ARENA_BATTLE_RESULT_LOG_NOTI )
{
}

bool	FLSnapshotEventArenaBattleResultLogNoti::BodySerialize( CAr& kAr )	const
{
	BYTE byCount = 0;
	ptrdiff_t nCountPos = kAr.ReserveSpace( sizeof( byCount ) );

	std::vector< T_BATTLE_RESULT_LOG >::const_iterator pos = vecBattleResultLog.begin();
	std::vector< T_BATTLE_RESULT_LOG >::const_iterator end = vecBattleResultLog.end();

	for( ; pos != end; ++pos )
	{
		kAr << *pos;
		++byCount;
	}

	kAr.WriteReservedSpace( nCountPos, &byCount, sizeof( byCount ) );
	return true;
}

bool	FLSnapshotEventArenaBattleResultLogNoti::BodyDeserialize( CAr& kAr )
{
	BYTE byCount = 0;
	kAr >> byCount;
	for( int i = 0; i < byCount; ++i )
	{
		T_BATTLE_RESULT_LOG tBattleResultLog;
		kAr >> tBattleResultLog;
		vecBattleResultLog.push_back( tBattleResultLog );
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
FLPacketEventArenaMoveBattleWaitNoti::FLPacketEventArenaMoveBattleWaitNoti() :
	FLPacket( PACKETTYPE_EVENT_ARENA_MOVE_BATTLE_WAIT_NOTI )
{
}

bool	FLPacketEventArenaMoveBattleWaitNoti::BodySerialize( CAr& /*kAr*/ )	const
{
	return true;
}

bool	FLPacketEventArenaMoveBattleWaitNoti::BodyDeserialize( CAr& /*kAr*/ )
{
	return true;
}
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
FLSnapshotEventArenaManageWindowNoti::FLSnapshotEventArenaManageWindowNoti() :
	FLPacket( SNAPSHOTTYPE_EVENT_ARENA_MANAGE_WINDOW_OPEN_NOTI )
{
	::memset( &tGameInfo, 0, sizeof( tGameInfo ) );
}

bool	FLSnapshotEventArenaManageWindowNoti::BodySerialize( CAr& kAr )	const
{
	// 설정된 데이터
	kAr << tGameInfo;		// 게임 설정 정보

	// 스펙
	{
		BYTE byCount = 0;
		ptrdiff_t nCountPos = kAr.ReserveSpace( sizeof( byCount ) );

		std::vector< T_EVENT_ARENA_TONEMENT >::const_iterator pos = vecTonements.begin();
		std::vector< T_EVENT_ARENA_TONEMENT >::const_iterator end = vecTonements.end();

		for( ; pos != end; ++pos )
		{
			kAr << *pos;
			++byCount;
		}

		kAr.WriteReservedSpace( nCountPos, &byCount, sizeof( byCount ) );
	}

	{
		BYTE byCount = 0;
		ptrdiff_t nCountPos = kAr.ReserveSpace( sizeof( byCount ) );

		std::vector< T_EVENT_ARENA_TEAM_INFO >::const_iterator pos = vecTeams.begin();
		std::vector< T_EVENT_ARENA_TEAM_INFO >::const_iterator end = vecTeams.end();

		for( ; pos != end; ++pos )
		{
			kAr << *pos;
			++byCount;
		}

		kAr.WriteReservedSpace( nCountPos, &byCount, sizeof( byCount ) );
	}

	return true;
}

bool	FLSnapshotEventArenaManageWindowNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> tGameInfo;

	BYTE byCount = 0;
	kAr >> byCount;
	for( int i = 0; i < byCount; ++i )
	{
		T_EVENT_ARENA_TONEMENT tTonement;
		kAr >> tTonement;
		vecTonements.push_back( tTonement );
	}

	byCount = 0;
	kAr >> byCount;
	for( int i = 0; i < byCount; ++i )
	{
		T_EVENT_ARENA_TEAM_INFO tTeam;
		kAr >> tTeam;
		vecTeams.push_back( tTeam );
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
FLSnapshotEventArenaManageWindowCloseNoti::FLSnapshotEventArenaManageWindowCloseNoti() :
	FLPacket( SNAPSHOTTYPE_EVENT_ARENA_MANAGE_WINDOW_CLOSE_NOTI )
{
}

bool	FLSnapshotEventArenaManageWindowCloseNoti::BodySerialize( CAr& /*kAr*/ )	const
{
	return true;
}

bool	FLSnapshotEventArenaManageWindowCloseNoti::BodyDeserialize( CAr& /*kAr*/ )
{
	return true;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
FLSnapshotEventArenaManageGameInfoSyncNoti::FLSnapshotEventArenaManageGameInfoSyncNoti() :
	FLPacket( SNAPSHOTTYPE_EVENT_ARENA_MANAGE_GAME_INFO_SYNC_NOTI )
{
	::memset( &tGameInfo, 0, sizeof( tGameInfo ) );
}

bool	FLSnapshotEventArenaManageGameInfoSyncNoti::BodySerialize( CAr& kAr )	const
{
	kAr << tGameInfo;		// 게임 설정 정보
	return true;
}

bool	FLSnapshotEventArenaManageGameInfoSyncNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> tGameInfo;
	return true;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
FLSnapshotEventArenaManageWaitCallPlayersNoti::FLSnapshotEventArenaManageWaitCallPlayersNoti() :
	FLPacket( SNAPSHOTTYPE_EVENT_ARENA_MANAGE_WAIT_CALL_PLAYERS_NOTI )
{
	dwWaitTimeout = 0;
}

bool	FLSnapshotEventArenaManageWaitCallPlayersNoti::BodySerialize( CAr& kAr )	const
{
	kAr << dwWaitTimeout;
	return true;
}

bool	FLSnapshotEventArenaManageWaitCallPlayersNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> dwWaitTimeout;
	return true;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
FLSnapshotEventArenaManageRedBlueTeamInfoNoti::FLSnapshotEventArenaManageRedBlueTeamInfoNoti() :
	FLPacket( SNAPSHOTTYPE_EVENT_ARENA_MANAGE_RED_BLUE_TEAM_INFO_NOTI )
{
}

bool	FLSnapshotEventArenaManageRedBlueTeamInfoNoti::BodySerialize( CAr& kAr )	const
{
	if( kRedTeamInfo.Serialize( kAr ) == false )
	{
		return false;
	}

	if( kBlueTeamInfo.Serialize( kAr ) == false )
	{
		return false;
	}

	return true;
}

bool	FLSnapshotEventArenaManageRedBlueTeamInfoNoti::BodyDeserialize( CAr& kAr )
{
	if( kRedTeamInfo.Deserialize( kAr ) == false )
	{
		return false;
	}

	if( kBlueTeamInfo.Deserialize( kAr ) == false )
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
FLSnapshotEventArenaManageRedBlueTeamInfoCloseNoti::FLSnapshotEventArenaManageRedBlueTeamInfoCloseNoti() :
	FLPacket( SNAPSHOTTYPE_EVENT_ARENA_MANAGE_RED_BLUE_TEAM_INFO_CLOSE_NOTI )
{
}

bool	FLSnapshotEventArenaManageRedBlueTeamInfoCloseNoti::BodySerialize( CAr& /*kAr*/ )	const
{
	return true;
}

bool	FLSnapshotEventArenaManageRedBlueTeamInfoCloseNoti::BodyDeserialize( CAr& /*kAr*/ )
{
	return true;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
FLSnapshotEventArenaManageWaitJoinPlayersNoti::FLSnapshotEventArenaManageWaitJoinPlayersNoti() :
	FLPacket( SNAPSHOTTYPE_EVENT_ARENA_MANAGE_WAIT_JOIN_PLAYERS_NOTI )
{
	dwWaitTimeout = 0;
}

bool	FLSnapshotEventArenaManageWaitJoinPlayersNoti::BodySerialize( CAr& kAr )	const
{
	kAr << dwWaitTimeout;
	return true;
}

bool	FLSnapshotEventArenaManageWaitJoinPlayersNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> dwWaitTimeout;
	return true;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
FLPacketEventArenaManageSetTonementNoti::FLPacketEventArenaManageSetTonementNoti() :
	FLPacket( PACKETTYPE_EVENT_ARENA_MANAGE_SET_TONEMENT_NOTI )
{
	dwTonementID = 0;
}

bool	FLPacketEventArenaManageSetTonementNoti::BodySerialize( CAr& kAr )	const
{
	kAr << dwTonementID;
	return true;
}

bool	FLPacketEventArenaManageSetTonementNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> dwTonementID;
	return true;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
FLPacketEventArenaManageSetRedTeamNoti::FLPacketEventArenaManageSetRedTeamNoti() :
	FLPacket( PACKETTYPE_EVENT_ARENA_MANAGE_SET_RED_TEAM_NOTI )
{
	dwRedTeamID = 0;
}

bool	FLPacketEventArenaManageSetRedTeamNoti::BodySerialize( CAr& kAr )	const
{
	kAr << dwRedTeamID;
	return true;
}

bool	FLPacketEventArenaManageSetRedTeamNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> dwRedTeamID;
	return true;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
FLPacketEventArenaManageSetBlueTeamNoti::FLPacketEventArenaManageSetBlueTeamNoti() :
	FLPacket( PACKETTYPE_EVENT_ARENA_MANAGE_SET_BLUE_TEAM_NOTI )
{
	dwBlueTeamID = 0;
}

bool	FLPacketEventArenaManageSetBlueTeamNoti::BodySerialize( CAr& kAr )	const
{
	kAr << dwBlueTeamID;
	return true;
}

bool	FLPacketEventArenaManageSetBlueTeamNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> dwBlueTeamID;
	return true;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
FLPacketEventArenaManageCallPlayersNoti::FLPacketEventArenaManageCallPlayersNoti() :
	FLPacket( PACKETTYPE_EVENT_ARENA_MANAGE_CALL_PLAYERS_NOTI )
{
}

bool	FLPacketEventArenaManageCallPlayersNoti::BodySerialize( CAr& /*kAr*/ )	const
{
	return true;
}

bool	FLPacketEventArenaManageCallPlayersNoti::BodyDeserialize( CAr& /*kAr*/ )
{
	return true;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
FLPacketEventArenaManageJoinPlayersNoti::FLPacketEventArenaManageJoinPlayersNoti() :
	FLPacket( PACKETTYPE_EVENT_ARENA_MANAGE_JOIN_PLAYERS_NOTI )
{
}

bool	FLPacketEventArenaManageJoinPlayersNoti::BodySerialize( CAr& /*kAr*/ )	const
{
	return true;
}

bool	FLPacketEventArenaManageJoinPlayersNoti::BodyDeserialize( CAr& /*kAr*/ )
{
	return true;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
FLPacketEventArenaManageBattleStartNoti::FLPacketEventArenaManageBattleStartNoti() :
	FLPacket( PACKETTYPE_EVENT_ARENA_MANAGE_BATTLE_START_NOTI )
{
}

bool	FLPacketEventArenaManageBattleStartNoti::BodySerialize( CAr& /*kAr*/ )	const
{
	return true;
}

bool	FLPacketEventArenaManageBattleStartNoti::BodyDeserialize( CAr& /*kAr*/ )
{
	return true;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
FLPacketEventArenaManageGameEndNoti::FLPacketEventArenaManageGameEndNoti() :
	FLPacket( PACKETTYPE_EVENT_ARENA_MANAGE_GAME_END_NOTI )
{
}

bool	FLPacketEventArenaManageGameEndNoti::BodySerialize( CAr& /*kAr*/ )	const
{
	return true;
}

bool	FLPacketEventArenaManageGameEndNoti::BodyDeserialize( CAr& /*kAr*/ )
{
	return true;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
FLPacketEventArenaManageGameStopNoti::FLPacketEventArenaManageGameStopNoti() :
	FLPacket( PACKETTYPE_EVENT_ARENA_MANAGE_GAME_STOP_NOTI )
{
}

bool	FLPacketEventArenaManageGameStopNoti::BodySerialize( CAr& /*kAr*/ )	const
{
	return true;
}

bool	FLPacketEventArenaManageGameStopNoti::BodyDeserialize( CAr& /*kAr*/ )
{
	return true;
}
//////////////////////////////////////////////////////////////////////////


