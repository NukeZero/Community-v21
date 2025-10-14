
#ifndef	__FLPACKETEVENTARENA_H__
#define	__FLPACKETEVENTARENA_H__


#include "FLPacket.h"


enum
{
	EVENT_ARENA_STATE_CONFIG,		// 설정 단계
	EVENT_ARENA_STATE_CALL_WAIT,	// 선수 소환 대기
	EVENT_ARENA_STATE_CALL,			// 선수 소환
	EVENT_ARENA_STATE_CALLED,		// 선수 소환 끝
	EVENT_ARENA_STATE_JOIN,			// 선수 입장
	EVENT_ARENA_STATE_JOINED,		// 선수 입장 끝
	EVENT_ARENA_STATE_COUNTDOWN,	// 카운트 다운
	EVENT_ARENA_STATE_PLAYING,		// 전투 중
	EVENT_ARENA_STATE_RESULT,		// 전투 결과
	EVENT_ARENA_STATE_END,			// 종료
};


enum
{
	MAX_EVENT_ARENA_WIN_COUNT		= 7,
	MAX_EVENT_ARENA_TEAM_NAME		= 64,
	MAX_EVENT_ARENA_TONEMENT_NAME	= 128,
	MAX_EVENT_ARENA_TONEMENT_DESC	= 128,
	MAX_EVENT_ARENA_GAME_RESULT_CODE	= 16,
	MAX_EVENT_ARENA_GAME_RESULT_REASON	= 16,
};


enum
{
	EVENT_ARENA_TEAM_MEMBER_STATE_LOGOUT,		// 로그아웃 상태
	EVENT_ARENA_TEAM_MEMBER_STATE_LOGIN,		// 로그인 상태(관람지역)
	EVENT_ARENA_TEAM_MEMBER_STATE_BATTLE_WAIT,	// 전투 대기
	EVENT_ARENA_TEAM_MEMBER_STATE_BATTLE,		// 전투 참여
};

enum
{
	EVENT_ARENA_GAME_STATE_NONE,	// 
	EVENT_ARENA_GAME_STATE_CALL,	// 선수 소환 중
	EVENT_ARENA_GAME_STATE_JOIN,	// 선수 입장 중
	EVENT_ARENA_GAME_STATE_PLAYING,	// 경기 중
};

#pragma pack( 1 )

struct	T_EVENT_ARENA_TEAM_INFO
{
	T_EVENT_ARENA_TEAM_INFO() : dwID( 0 )	{ szName[ 0 ] = '\0'; }

	DWORD	dwID;
	TCHAR	szName[MAX_EVENT_ARENA_TEAM_NAME];
};


struct	T_EVENT_ARENA_TEAM_MEMBER_INFO
{
	OBJID	dwObjID;				// 캐릭터 오브젝트 아이디
	BYTE	byPartyNumber;			// 파티 넘버
	TCHAR	szCharName[MAX_NAME];	// 캐릭터 이름

	int		nState;					// 상태

	int		nCurrentHP;				// 현재 HP
	int		nMaxHP;					// 최대 HP
};

// 게임 정보
struct	T_EVENT_ARENA_GAME_INFO
{
	int		nState;				// 게임 상태

	DWORD	dwTonementID;		// 토너먼트 아이디
	DWORD	dwRedTeamID;		// 레드팀 아이디
	DWORD	dwBlueTeamID;		// 블루팀 아이디

	BYTE	byRedWinFlag[MAX_EVENT_ARENA_WIN_COUNT];	// 레드팀 승리 플래그
	BYTE	byBlueWinFlag[MAX_EVENT_ARENA_WIN_COUNT];	// 블루팀 승리 플래그

	DWORD	dwRemainBattleTime;	// 잔여 전투시간(1000이 1초)
};


struct	T_EVENT_ARENA_TONEMENT
{
	//T_EVENT_ARENA_TONEMENT() : dwID( 0 ), nNeedWinCount( 0 ) {		szName[ 0 ] = '\0';		szDesc[ 0 ] = '\0'; }

	DWORD	dwID;									// 토너먼트 아이디
	TCHAR	szName[MAX_EVENT_ARENA_TONEMENT_NAME];	// 토너먼트 이름
	int		nNeedWinCount;							// 필요 승수
	TCHAR	szDesc[MAX_EVENT_ARENA_TONEMENT_DESC];	// 설명
};

struct	T_BATTLE_RESULT_LOG
{
	int			nBattleNumber;								// 라운드 넘버
	int			nWinTeam;									// 승리팀 인덱스(Red, Blue 팀)
	TCHAR		szWinTeamName[MAX_EVENT_ARENA_TEAM_NAME];	// 승리팀 인덱스
	ULONGLONG	u64BattleTime;								// 전투시간
	int			nRedSurvivalMembers;						// 레드팀 생존자 수
	int			nBlueSurvivalMembers;						// 블루팀 생존자 수
};

struct	T_GAME_RESULT_INFO
{
	T_EVENT_ARENA_TONEMENT	tTonement;				// 토너먼트 정보
	T_EVENT_ARENA_TEAM_INFO	tRedTeam;				// 레드팀 정보
	T_EVENT_ARENA_TEAM_INFO	tBlueTeam;				// 블루팀 정보

	TCHAR					szCode[MAX_EVENT_ARENA_GAME_RESULT_CODE];			// 로그 코드(경기 종료(GameEnd), 경기 중단(GameStop))
	TCHAR					szStopReason[MAX_EVENT_ARENA_GAME_RESULT_REASON];	// 경기 중단 추가 설명(명령어 중단(ChatCommand, UICommand), 운영자 로그아웃(Logout))

	SYSTEMTIME				tGameStart;				// 게임 시작 시간
	SYSTEMTIME				tGameEnd;				// 게임 종료 시간(로그 발생 시간)

	int						nWinTeam;				// 승리팀
	TCHAR					szWinTeam[MAX_EVENT_ARENA_TEAM_NAME];				// 승리팀 이름

	int						nRedWinCount;			// 레드팀 승수
	int						nBlueWinCount;			// 블루팀 승수
};

#pragma pack()


class	FLEventArenaTeamStream
{
public:

	FLEventArenaTeamStream();

	T_EVENT_ARENA_TEAM_INFO							tTeamInfo;		// 팀 정보
	std::vector< T_EVENT_ARENA_TEAM_MEMBER_INFO >	vecMembers;		// 맴버 리스트

	bool	Serialize( CAr& kAr )	const;
	bool	Deserialize( CAr& kAr );

};

class	FLEventArenaBattleResultInfo
{
public:

	FLEventArenaBattleResultInfo();

	T_EVENT_ARENA_TONEMENT	tTonement;				// 토너먼트 정보
	T_EVENT_ARENA_TEAM_INFO	tRedTeam;				// 레드팀 정보
	T_EVENT_ARENA_TEAM_INFO	tBlueTeam;				// 블루팀 정보

	SYSTEMTIME				tBattleStart;			// 전투 시작 시간
	SYSTEMTIME				tBattleEnd;				// 전투 종료 시간(로그 발생 시간)

	ULONGLONG				u64BattleTime;			// 전투 시간

	int						nWinTeam;				// 승리팀
	tstring					strWinTeam;				// 승리팀 이름

	int						nRedWinCount;			// 레드팀 승수
	int						nBlueWinCount;			// 블루팀 승수

	std::vector< tstring >	vecRedSurvivalMembers;	// 레드팀 생존자 명단
	std::vector< tstring >	vecBlueSurvivalMembers;	// 블루팀 생존자 명단

	bool	Serialize( CAr& kAr )	const;
	bool	Deserialize( CAr& kAr );

};


//////////////////////////////////////////////////////////////////////////
// Server -> Client
//////////////////////////////////////////////////////////////////////////

// =========================== 참가 선수 ============================ //

// SNAPSHOTTYPE_EVENT_ARENA_TEAM_INFO_NOTI			// 팀 정보 통보
class	FLSnapshotEventArenaTeamInfoNoti : public FLPacket
{
public:

	FLSnapshotEventArenaTeamInfoNoti();

	FLEventArenaTeamStream	kTeamInfo;	// 팀 정보

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};


// SNAPSHOTTYPE_EVENT_ARENA_TEAM_MEMBER_UPDATE_NOTI	// 팀원 정보 갱신(변경된 팀원만)
class	FLSnapshotEventArenaTeamMemberUpdateNoti : public FLPacket
{
public:

	FLSnapshotEventArenaTeamMemberUpdateNoti();

	std::vector< T_EVENT_ARENA_TEAM_MEMBER_INFO >	vecChangedMembers;		// 변경된 맴버들의 데이터

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};


// SNAPSHOTTYPE_EVENT_ARENA_CALL_PLAYER_NOTI		// 선수소환대기 통보
class	FLSnapshotEventArenaCallPlayerNoti : public FLPacket
{
public:

	FLSnapshotEventArenaCallPlayerNoti();

	DWORD	dwWaitTimeout;		// 대기 시간(단위 초)

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};


// =========================== 공용 ============================ //

// SNAPSHOTTYPE_EVENT_ARENA_GAME_JOIN_TEAM_INFO_NOTI	// 게임에 참여하는 레드, 블루팀 정보
class	FLSnapshotEventArenaGameJoinTeamInfoNoti : public FLPacket
{
public:

	FLSnapshotEventArenaGameJoinTeamInfoNoti();

	std::vector< tstring >	vecRedTeamMembers;
	std::vector< tstring >	vecBlueTeamMembers;

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// SNAPSHOTTYPE_EVENT_ARENA_BATTLE_RESULT_NOTI		// 전투 결과 통보
class	FLSnapshotEventArenaBattleResultNoti : public FLPacket
{
public:

	FLSnapshotEventArenaBattleResultNoti();

	int		nBattleWinTeam;							// 승리팀

	TCHAR	szRedName[MAX_EVENT_ARENA_TEAM_NAME];	// 레드팀 이름
	int		nRedWinCount;							// 레드팀 승수

	TCHAR	szBlueName[MAX_EVENT_ARENA_TEAM_NAME];	// 블루팀 이름
	int		nBlueWinCount;							// 블루팀 승수

	DWORD	dwWinnerDisplayTime;					// 승자표시 시간(단위 초)

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// SNAPSHOTTYPE_EVENT_ARENA_GAME_RESULT_NOTI			// 게임 결과(최종 승리) 통보
class	FLSnapshotEventArenaGameResultNoti : public FLPacket
{
public:

	FLSnapshotEventArenaGameResultNoti();

	int		nGameWinTeam;							// 최종 승리팀

	TCHAR	szRedName[MAX_EVENT_ARENA_TEAM_NAME];	// 레드팀 이름
	int		nRedWinCount;							// 레드팀 승수

	TCHAR	szBlueName[MAX_EVENT_ARENA_TEAM_NAME];	// 블루팀 이름
	int		nBlueWinCount;							// 블루팀 승수

	DWORD	dwWinnerDisplayTime;					// 승자표시 시간(단위 초)

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};


//SNAPSHOTTYPE_EVENT_ARENA_GAME_SCOREBOARD_NOTI		// 게임 전광판 통보
class	FLSnapshotEventArenaGameScoreboardNoti : public FLPacket
{
public:

	FLSnapshotEventArenaGameScoreboardNoti();

	TCHAR	szTonementName[MAX_EVENT_ARENA_TONEMENT_NAME];	// 토너먼트 이름
	TCHAR	szTonementDesc[MAX_EVENT_ARENA_TONEMENT_DESC];	// 설명

	BYTE	byGameState;									// 상태

	TCHAR	szRedTeamName[MAX_EVENT_ARENA_TEAM_NAME];		// 레드팀 이름
	int		nRedWinCount;									// 레드팀 승수
	int		nRedSurvivalMembers;							// 레드팀 생존자 수

	TCHAR	szBlueTeamName[MAX_EVENT_ARENA_TEAM_NAME];		// 블루팀 이름
	int		nBlueWinCount;									// 블루팀 승수
	int		nBlueSurvivalMembers;							// 블루팀 생존자 수

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};


// SNAPSHOTTYPE_EVENT_ARENA_BATTLE_RESULT_LOG_NOTI	// 전투 결과 로그 통보
class	FLSnapshotEventArenaBattleResultLogNoti : public FLPacket
{
public:

	FLSnapshotEventArenaBattleResultLogNoti();

	std::vector< T_BATTLE_RESULT_LOG >	vecBattleResultLog;

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};


//////////////////////////////////////////////////////////////////////////
// Client -> Server
//////////////////////////////////////////////////////////////////////////

// =========================== 참가 선수 ============================ //

// PACKETTYPE_EVENT_ARENA_MOVE_BATTLE_WAIT_NOTI	// NPC 이용 선수대기 지역 이동 요청
class	FLPacketEventArenaMoveBattleWaitNoti : public FLPacket
{
public:

	FLPacketEventArenaMoveBattleWaitNoti();

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};



// =========================== 관리자 ============================ //

//////////////////////////////////////////////////////////////////////////
// Server -> Client
//////////////////////////////////////////////////////////////////////////

// SNAPSHOTTYPE_EVENT_ARENA_MANAGE_WINDOW_OPEN_NOTI	// 관리창 오픈 통보
class	FLSnapshotEventArenaManageWindowNoti : public FLPacket
{
public:

	FLSnapshotEventArenaManageWindowNoti();

	// 설정된 데이터
	T_EVENT_ARENA_GAME_INFO					tGameInfo;		// 게임 설정 정보

	// 스펙
	std::vector< T_EVENT_ARENA_TONEMENT >	vecTonements;	// 토너먼트 리스트
	std::vector< T_EVENT_ARENA_TEAM_INFO >	vecTeams;		// 팀 리스트

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// SNAPSHOTTYPE_EVENT_ARENA_MANAGE_WINDOW_CLOSE_NOTI			// 관리창 닫기 통보
class	FLSnapshotEventArenaManageWindowCloseNoti : public FLPacket
{
public:

	FLSnapshotEventArenaManageWindowCloseNoti();

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};


// SNAPSHOTTYPE_EVENT_ARENA_MANAGE_GAME_INFO_SYNC_NOTI		// 게임 정보 동기 요청
class	FLSnapshotEventArenaManageGameInfoSyncNoti : public FLPacket
{
public:

	FLSnapshotEventArenaManageGameInfoSyncNoti();

	T_EVENT_ARENA_GAME_INFO	tGameInfo;		// 게임 설정 정보

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};


// SNAPSHOTTYPE_EVENT_ARENA_MANAGE_WAIT_CALL_PLAYERS_NOTI		// 선수소환 대기 통보
class	FLSnapshotEventArenaManageWaitCallPlayersNoti : public FLPacket
{
public:

	FLSnapshotEventArenaManageWaitCallPlayersNoti();

	DWORD	dwWaitTimeout;	// 대기 시간(단위 초)

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};


// SNAPSHOTTYPE_EVENT_ARENA_MANAGE_RED_BLUE_TEAM_INFO_NOTI	// 레드, 블루 팀 정보 통보(주기적으로 전송)
class	FLSnapshotEventArenaManageRedBlueTeamInfoNoti : public FLPacket
{
public:

	FLSnapshotEventArenaManageRedBlueTeamInfoNoti();

	FLEventArenaTeamStream	kRedTeamInfo;
	FLEventArenaTeamStream	kBlueTeamInfo;

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};


// SNAPSHOTTYPE_EVENT_ARENA_MANAGE_RED_BLUE_TEAM_INFO_CLOSE_NOTI	// 레드, 블루 팀 정보 창 닫기 통보
class	FLSnapshotEventArenaManageRedBlueTeamInfoCloseNoti : public FLPacket
{
public:

	FLSnapshotEventArenaManageRedBlueTeamInfoCloseNoti();

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};


// SNAPSHOTTYPE_EVENT_ARENA_MANAGE_WAIT_JOIN_PLAYERS_NOTI		// 선수입장 대기 통보
class	FLSnapshotEventArenaManageWaitJoinPlayersNoti : public FLPacket
{
public:

	FLSnapshotEventArenaManageWaitJoinPlayersNoti();

	DWORD	dwWaitTimeout;	// 대기 시간(단위 초)

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};


//////////////////////////////////////////////////////////////////////////
// Client -> Server
//////////////////////////////////////////////////////////////////////////

// PACKETTYPE_EVENT_ARENA_MANAGE_SET_TONEMENT_NOTI		// 토너먼트 설정 통지
class	FLPacketEventArenaManageSetTonementNoti : public FLPacket
{
public:

	FLPacketEventArenaManageSetTonementNoti();

	DWORD	dwTonementID;		// 토너먼트 아이디

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// PACKETTYPE_EVENT_ARENA_MANAGE_SET_RED_TEAM_NOTI		// 레드팀 설정 통지
class	FLPacketEventArenaManageSetRedTeamNoti : public FLPacket
{
public:

	FLPacketEventArenaManageSetRedTeamNoti();

	DWORD	dwRedTeamID;		// 레드팀 아이디

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// PACKETTYPE_EVENT_ARENA_MANAGE_SET_BLUE_TEAM_NOTI		// 블루팀 설정 통지
class	FLPacketEventArenaManageSetBlueTeamNoti : public FLPacket
{
public:

	FLPacketEventArenaManageSetBlueTeamNoti();

	DWORD	dwBlueTeamID;		// 블루팀 아이디

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// PACKETTYPE_EVENT_ARENA_MANAGE_CALL_PLAYERS_NOTI		// 선수소환 통지
class	FLPacketEventArenaManageCallPlayersNoti : public FLPacket
{
public:

	FLPacketEventArenaManageCallPlayersNoti();

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// PACKETTYPE_EVENT_ARENA_MANAGE_JOIN_PLAYERS_NOTI		// 선수입장 통지
class	FLPacketEventArenaManageJoinPlayersNoti : public FLPacket
{
public:

	FLPacketEventArenaManageJoinPlayersNoti();

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// PACKETTYPE_EVENT_ARENA_MANAGE_BATTLE_START_NOTI		// 전투 시작 통지
class	FLPacketEventArenaManageBattleStartNoti : public FLPacket
{
public:

	FLPacketEventArenaManageBattleStartNoti();

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// PACKETTYPE_EVENT_ARENA_MANAGE_GAME_END_NOTI			// 게임 종료 통지
class	FLPacketEventArenaManageGameEndNoti : public FLPacket
{
public:

	FLPacketEventArenaManageGameEndNoti();

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};


// PACKETTYPE_EVENT_ARENA_MANAGE_GAME_STOP_NOTI			// 게임 중지 통지
class	FLPacketEventArenaManageGameStopNoti : public FLPacket
{
public:

	FLPacketEventArenaManageGameStopNoti();

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};




#endif