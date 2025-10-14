#ifndef __FLCOMMONSTRUCT_H__
#define __FLCOMMONSTRUCT_H__

#pragma pack( 1 )


//////////////////////////////////////////////////////////////////////////
//	LOG_STR Procedure Data Struct.
//////////////////////////////////////////////////////////////////////////
#define _MAX_LOGSTR_TYPE_LEN	3
#define _MAX_LOGSTR_STATE_LEN	2
#define _MAX_LOGSTR_PLAYERNAME_LEN	33
#define _MAX_LOGSTR_ITEMNAME_LEN	33
#define _MAX_LOGSTR_STARTTIME_LEN	15
#define _MAX_LOGSTR_IP_LEN			17

typedef	struct  _T_LOGSTR_DATA
{
	TCHAR	szType[ _MAX_LOGSTR_TYPE_LEN ];
	TCHAR	szState[ _MAX_LOGSTR_STATE_LEN ];
	DWORD	dwPlayerID;
	DWORD	dwServerIndex;
	__int64	n64Exp1;
	int		nLevel;
	int		nJob;
	int		nJobLv;
	int		nFlightLv;
	int		nStr;
	int		nDex;
	int		nInt;
	int		nSta;
	int		nRemainGP;
	int		nRemainLP;
	int		nWorldID;
	TCHAR	szKilledPlayerName[ _MAX_LOGSTR_PLAYERNAME_LEN ];
	float	fPosX;
	float	fPosY;
	float	fPosZ;
	int		nAttackPower;
	int		nMaxHP;
	TCHAR	szGetPlayerName[ _MAX_LOGSTR_PLAYERNAME_LEN ];
	DWORD	dwGold;
	int		nRemainGold;
	SERIALNUMBER nItemUniqueNo;
	TCHAR	szItemName[ _MAX_LOGSTR_ITEMNAME_LEN ];
	int		nItemDurability;
	int		nItemCount;
	int		nItemAddLv;
	TCHAR	szStartTime[ _MAX_LOGSTR_STARTTIME_LEN ];
	int		nTotalPlayerTime;
	TCHAR	szRemoteIP[ _MAX_LOGSTR_IP_LEN ];
	int		nQuestIndex;
	DWORD	dwKillPlayerID;

	_T_LOGSTR_DATA( TCHAR* pszQueryType )
	{
		FLStrcpy( szType, _countof(szType), pszQueryType );
		dwPlayerID			= 0;
		dwServerIndex		= 0;
		n64Exp1				= 0;
		nLevel				= 0;
		nJob				= 0;
		nJobLv				= 0;
		nFlightLv			= 0;
		nStr				= 0;
		nDex				= 0;
		nInt				= 0;
		nSta				= 0;
		nRemainGP			= 0;
		nRemainLP			= 0;
		nWorldID			= 0;
		fPosX				= 0.0f;
		fPosY				= 0.0f;
		fPosZ				= 0.0f;
		nAttackPower		= 0;
		nMaxHP				= 0;
		dwGold				= 0;
		nRemainGold			= 0;
		nItemUniqueNo		= 0;
		nItemDurability		= 0;
		nItemCount			= 0;
		nItemAddLv			= 0;
		nTotalPlayerTime	= 0;
		nQuestIndex			= 0;
		dwKillPlayerID		= 0;

		memset( szState,				0,	sizeof(szState) );
		memset( szKilledPlayerName,	0,	sizeof(szKilledPlayerName) );
		memset( szGetPlayerName,		0,	sizeof(szGetPlayerName) );
		memset( szItemName,			0,	sizeof(szItemName) );
		memset( szStartTime,			0,	sizeof(szStartTime) );
		memset( szRemoteIP,			0,	sizeof(szRemoteIP) );
	};

} T_LOGSTR_DATA, *PT_LOGSTR_DATA;


//////////////////////////////////////////////////////////////////////////
// server description
typedef struct	_T_SERVER_DESC
{
	DWORD	dwParentID;
	DWORD	dwMyID;
	TCHAR	szName[36];
	TCHAR	szAddress[16];
	bool	bAdultOnly;
	long	lCanConnection;
	long	lMaxConnection;
	long	lConnectionCount;
	bool	bTransformCharacter;

	_T_SERVER_DESC()
	{
		dwParentID				= NULL_ID;
		dwMyID					= NULL_ID;
		memset( szName, 0, sizeof( szName ) );
		memset( szAddress, 0, sizeof( szAddress ) );
		bAdultOnly				= false;
		lCanConnection			= 0;
		lMaxConnection			= 0;
		lConnectionCount		= 0;
		bTransformCharacter		= false;
	}

} T_SERVER_DESC, *PT_SERVER_DESC;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
struct T_CONNECTION_POINT
{
	int		nPoint;
	DWORD	dwGetPointTick;

	T_CONNECTION_POINT() : nPoint( 0 ), dwGetPointTick( 0 )
	{
	}
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
struct T_ITEM_BASE_INFO
{
	DWORD	dwItemID;
	int		nQuantity;

	T_ITEM_BASE_INFO() : dwItemID( 0 ), nQuantity( 0 )
	{
	}
};

struct T_EXCHANGE_ITEM_DATA
{
	DWORD	dwIndex;
	bool	bBinds;

	T_ITEM_BASE_INFO	kCondItem;
	T_ITEM_BASE_INFO	kPayItem;

	T_EXCHANGE_ITEM_DATA() : dwIndex( 0 ), bBinds( false )
	{
	}
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
struct T_BURNING_MODE
{
	DWORD	dwBurningMode;
	DWORD	dwMinute;

	T_BURNING_MODE() : dwBurningMode( 0 ), dwMinute( 0 )
	{
	}
};
//////////////////////////////////////////////////////////////////////////

struct T_GUILD_BANK_QUERY_FAIL_ACK
{
	DWORD	dwErrorCode;
	DWORD	dwGuildBankUseWorldID;

	T_GUILD_BANK_QUERY_FAIL_ACK() : dwErrorCode( 0 ), dwGuildBankUseWorldID( 0 )
	{
	}
};

struct T_SELL_ITEM_TO_NPC_ACK
{
	DWORD	dwItemObjID;
	bool	bSuccess;

	T_SELL_ITEM_TO_NPC_ACK() : dwItemObjID( NULL_ID ), bSuccess( false )
	{
	}
};

#pragma pack()

#endif	//__FLCOMMONSTRUCT_H__
