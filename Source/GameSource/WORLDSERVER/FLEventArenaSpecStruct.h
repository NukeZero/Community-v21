
#ifndef	__FLEVENTARENASPECSTRUCT_H__
#define	__FLEVENTARENASPECSTRUCT_H__


#include <defineEventArena.h>
#include "../_CommonDefine/Spec/FLSpecStruct.h"
#include "../_CommonDefine/Packet/FLPacketEventArena.h"


struct	T_EVENT_ARENA_TEAM_MEMBER_SPEC
{
	BYTE	byPartyNumber;			// 극단 번호
	TCHAR	szCharName[MAX_NAME];	// 캐릭터 이름
};

struct	T_EVENT_ARENA_TONEMENT_SPEC
{
	TCHAR	szName[MAX_EVENT_ARENA_TONEMENT_NAME];	// 토너먼트 이름
	int		nNeedWinCount;							// 필요 승수
	TCHAR	szDesc[MAX_EVENT_ARENA_TONEMENT_DESC];	// 설명
};

struct	T_EVENT_ARENA_GIVE_ITEM_SPEC
{
	DWORD	dwItemIndex;			// 아이템 인덱스
	BYTE	byCount;				// 지급 갯수
};


struct	T_EVENT_ARENA_TIME_CONFIG_SPEC
{
	DWORD	dwBattleTime;			// 전투 시간(단위 초)
	DWORD	dwCallWaitTime;			// 소환대기 시간(단위 초)
	DWORD	dwBattleStandbyTime;	// 전투대기 시간(단위 초)
	DWORD	dwWinnerDisplayTime;	// 승자표시 시간(단위 초)
};

struct	T_EVENT_ARENA_START_POSITION_SPEC
{
	int					nStartKind;	// 시작 위치 종류
	int					nTeam;		// 팀 종류

	T_WORLD_POSITION	tPosition;	// 위치

	DWORD				dwRadius;	// 반경
};




#endif