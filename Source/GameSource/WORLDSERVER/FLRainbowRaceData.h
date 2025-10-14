
#ifndef __FLRAINBOWRACEDATA_H__
#define __FLRAINBOWRACEDATA_H__


namespace nsRainbowRaceData
{

const int	LANG_COMMON		= -1;


struct T_RACE_PERIOD_DATA
{
	FLTm	StartTime;
	FLTm	EndTime;
	FLTm	OpenTime;

	T_RACE_PERIOD_DATA() : StartTime( INVALID_DATETIME ), EndTime( INVALID_DATETIME ), OpenTime( INVALID_DATETIME )
	{
	}
};

struct T_RACE_PROGRESS_DATA
{
	DWORD	WaitingTick;
	DWORD	ReadyTick;
	DWORD	ProgressTick;

	T_RACE_PROGRESS_DATA() : WaitingTick( 0 ), ReadyTick( 0 ), ProgressTick( 0 )
	{
	}
};

struct T_RACE_APPLICATION_DATA
{
	int						ApplicationPenya;
	T_RACE_PERIOD_DATA		PeriodData;
	T_RACE_PROGRESS_DATA	ProgressData;

	T_RACE_APPLICATION_DATA() : ApplicationPenya( 0 )
	{
	}
};

struct T_RACE_NPC_DATA
{
	DWORD			NPCID;
	DWORD			WorldID;
	D3DXVECTOR3		Pos;
	TCHAR			NPCKey[NPC_CHARACTER_KEY_LEN];

	T_RACE_NPC_DATA() : NPCID( 0 ), WorldID( WI_WORLD_NONE ), Pos( 0.0F, 0.0F, 0.0F )
	{
		NPCKey[0]	= '\0';
	}
};

struct T_RACE_PRIZE_ITEM_DATA
{
	DWORD	ItemID;
	int		Quantity;
	bool	Bind;

	T_RACE_PRIZE_ITEM_DATA() : ItemID( 0 ), Quantity( 0 ), Bind( false )
	{
	}
};

struct T_RACE_PRIZE_DATA
{
	DWORD		Ranking;
	float		ReturnRate;
	T_RACE_PRIZE_ITEM_DATA ItemData;

	T_RACE_PRIZE_DATA() : Ranking( 0 ), ReturnRate( 0.0f )
	{
	}
};

struct T_RACE_MINIGAME_PRIZE_DATA
{
	DWORD		CompleteCount;
	T_RACE_PRIZE_ITEM_DATA ItemData;

	T_RACE_MINIGAME_PRIZE_DATA() : CompleteCount( 0 )
	{
	}
};

typedef		std::map< int /* DAY */, T_RACE_APPLICATION_DATA >	ApplicationDataMap;

typedef		std::vector< T_RACE_NPC_DATA >						NPCDataVec;

typedef		std::vector< T_RACE_PRIZE_DATA >					PrizeDataVec;
typedef		std::map< int /* DAY */, PrizeDataVec >				PrizeDataMap;

typedef		std::vector< T_RACE_MINIGAME_PRIZE_DATA >			MiniGamePrizeDataVec;
typedef		std::map< int /* DAY */, MiniGamePrizeDataVec >		MiniGamePrizeDataMap;

}	// namespace nsRainbowRaceData

#endif // __FLRAINBOWRACEDATA_H__
