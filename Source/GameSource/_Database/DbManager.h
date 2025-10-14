/********************************************************************
	created:	2003/02/26
	created:	26:2:2003   16:49
	filename: 	c:\neurospace\program\_database\dbmanager.h
	file path:	c:\neurospace\program\_database
	file base:	dbmanager
	file ext:	h
	author:		Kim, pan-young
	
	purpose:	데이타 베이스 처리 클래스
*********************************************************************/

#ifndef _DB_MANAGER_H_
#define _DB_MANAGER_H_

#include "Query.h"
#include <network/FLAr.h>
#include <network/FLDPMng.h>
#include <memory/FLMemPooler.h>
#include <misc.h>
#include "..\_Network\Objects\Obj.h"
#include "AccountCacheMgr.h"
#include "../_CommonDefine/FLTm.h"
#include <post.h>


#include "../_CommonDefine/FLItemContainer.h"
#include "../_CommonDefine/Packet/FLPacket_ConsignmentSale.h"

	//	mulcom	BEGIN100415
	//const int	MAX_GETTHREAD_SIZE		= 8;
	//const int	MAX_PUTTHREAD_SIZE		= 16;
	const int	MAX_GETTHREAD_SIZE		= 7;		
	const int	MAX_PUTTHREAD_SIZE		= 14;
	//	mulcom	END100415

#ifdef MODIFY_INVEN_ITEM_DISSOLUTION_BUG
	const int	MAX_UPDATETHREAD_SIZE	= 0;
#else
	const int	MAX_UPDATETHREAD_SIZE	= 8;
#endif
	

const int	MAX_JOIN_SIZE			= 16;
const int	MAX_TASKBAR				= MAX_SHORTCUT_STRING * 2 + 44; 
const int	MAX_APPLETTASKBAR		= MAX_TASKBAR * MAX_SLOT_APPLET; 
const int	MAX_ITEMTASKBAR			= MAX_TASKBAR * 8 * 9;
const char	NullStr[2]				= "$";


#include "FLDBConsignmentSale.h"


enum QUERYMODE
{ 
	DB_CONNECT
	, DB_THREAD_END
	, CREATEACCOUNT
	, CREATEPLAYER
	, LOGIN
	, REMOVEPLAYER
	, SEL_ECT
	, SENDPLAYERLIST
	, JOIN
	, SAVEPLAYER
	, SAVECONCURRENTUSERNUMBER
	, SAVE_PLAY_TIME
	, LOG_PLAY_CONNECT
	, LOG_TRADE_ITEM
	, LOG_ITEM
	, LOG_PLAY_DEATH
	, LOG_LEVELUP
	, LOG_SERVER_DEATH
	, LOG_UNIQUEITEM
	, LOG_QUEST
	, CREATE_GUILD
	, DESTROY_GUILD
	, ADD_GUILD_MEMBER
	, REMOVE_GUILD_MEMBER
	, UPDATE_GUILD_LOGO
	, UPDATE_GUILD_CONTRIBUTION
	, UPDATE_GUILD_NOTICE
	, UPDATE_GUILD_MEMBER_LEVEL
	, UPDATE_GUILD_AUTHORITY
	, UPDATE_GUILD_PENYA
	, GUILD_SETNAME
	, UPDATE_GUILD_REALPAY
	, GUILD_BANK_UPDAATE
	, GUILD_BANK_QUERY
	, GUILD_RANKING_UPDATE
	, GUILD_RANKING_UPDATE_DB
	, ADD_GUILD_VOTE
	, REMOVE_GUILD_VOTE
	, CLOSE_GUILD_VOTE
	, CAST_GUILD_VOTE
	, LOG_GUILD_CREATE
	, LOG_GUILD_REMOVE
	, ACPT_GUILD_WAR
	, WAR_END
	, SURRENDER
	, WAR_DEAD
	, WAR_MASTER_ABSENT
	, REMOVE_FRIEND
	, ADD_PARTYNAME
	, CHANGE_BANKPASS
	, GAMEMA_CHAT
	, INSERT_FREQUENCY
	, LOG_PK_PVP
	, INSERT_TAG
	, SCHOOL_REPORT
	, UPDATE_GUILD_CLASS
	, UPDATE_GUILD_NICKNAME
	, LOG_SCHOOL
	, QM_QUERYGUILDQUEST
	, QM_INSERTGUILDQUEST
	, QM_UPDATEGUILDQUEST
	, BS_TEST
	, QM_DELETEGUILDQUEST
	, WANTED_OP
	, QM_SET_PLAYER_NAME
	, QM_SNOOPGUILD
	, START_GUILDCOMBAT
	, IN_GUILDCOMBAT
	, OUT_GUILDCOMBAT
	, RESULT_GUILDCOMBAT
	, GETPENYAGUILD_GUILDCOMBAT
	, GETPENYAPLAYER_GUILDCOMBAT
	, CONTINUE_GUILDCOMBAT
	, ADD_MAIL
	, REMOVE_MAIL
	, REMOVE_MAIL_ITEM
	, REMOVE_MAIL_GOLD
	, READ_MAIL
	, QM_UPDATEGUILDMASTER
	, QM_DELETE_REMOVE_GUILD_BANK_TBL
	, QM_LOG_EXPBOX
	, QM_CALL_USPLOGGINGQUEST
	, QM_CALL_XXX_MULTI_SERVER
	, LOG_SKILLPOINT
	, QM_CALL_USP_PET_LOG
	, RECOMMEND_EVE
	, QM_GETMAIL_REALTIME

	, QM_ADD_MESSENGER
	, QM_DELETE_MESSENGER
	, QM_UPDATE_MESSENGER

	, QM_GUILDBANK_LOG_VIEW
	, LOG_GETHONORTIME
	, QM_SEALCHAR
	, QM_SEALCHARCONM
	, QM_SEALCHARGET
	, QM_SEALCHARSET
	, GC1TO1_STATE
	, GC1TO1_LOAD
	, GC1TO1_TENDER
	, GC1TO1_LINEUP
	, GC1TO1_WARPERSON
	, GC1TO1_WARGUILD

	, QM_QUERY_MAIL_BOX
	, QM_ALL_PLAYER_DATA

	, LOG_INSTANCEDUNGEON

	, LOG_ERROR

	, QM_LOGIN_PROTECT
	, LOG_GUILDFURNITURE
	, QM_QUERY_COUPON_NUMBER


	, QM_QUERY_SERIALNUMBER		// 시리얼넘버 얻기

	// 협동 기부
	//, QM_COOPERATIVE_CONTRIBUTIONS_GET_CONTRIBUTION_INFO_AND_CLEAR_EXPIRED
	, QM_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE
	, QM_COOPERATIVE_CONTRIBUTIONS_GET_CONTRIBUTED
	, QM_COOPERATIVE_CONTRIBUTIONS_GET_RANKING
//	, QM_COOPERATIVE_CONTRIBUTIONS_TRY_REWARD_ITEM
	, QM_COOPERATIVE_CONTRIBUTIONS_TRY_WRITE_ITEM_REWARD_INFO

	// 협동 기부
	//, QM_REWARD_ITEM
	, QM_RECORD_DUNGEON_BEST_COMPLETE_TIME
	, QM_RECORD_GET_DUNGEON_RANKING

	, QM_TELEPORT_MAP_ADD_POINT			// 텔레포트맵 등록 지점 추가
	, QM_TELEPORT_MAP_CHG_POINT_NAME	// 텔레포트맵 등록 지점 이름 변경
	, QM_TELEPORT_MAP_DEL_POINT			// 텔레포트맵 등록 지점 삭제


	//, QM_CONSIGNMENT_SALE_GET_REGISTED_COUNT
	//, QM_CONSIGNMENT_SALE_REGIST
	//, QM_CONSIGNMENT_SALE_CANCEL_REGISTED
	//, QM_CONSIGNMENT_SALE_REGISTED_INFO_LIST
	//, QM_CONSIGNMENT_SALE_GET_PRICE
	//, QM_CONSIGNMENT_SALE_CLEAR_RESERVED_INFO
	//, QM_CONSIGNMENT_SALE_BUY_ITEM
	//, QM_CONSIGNMENT_SALE_COLLECT_SALE_GOLD
	//, QM_CONSIGNMENT_SALE_SEARCH
	//, QM_CONSIGNMENT_SALE_SALE_GOLD_INFO_LIST

	, QM_CHARACTER_SERVER_TRANSFORM

	, GUILD_BANK_INPUT_ITEM
	, GUILD_BANK_OUTPUT_ITEM
//	, GUILD_BANK_INPUT_GOLD
	, GUILD_BANK_OUTPUT_GOLD


};

typedef struct tagACCOUNT_INFO
{
	int		nPlayerSize;
	BYTE	byPlayerSelectPage;
	char	szAccount[MAX_ACCOUNT];
	char	szPassword[MAX_ENCRYPT_PWD];
	char	szPlayer[MAX_PLAYER];

	tagACCOUNT_INFO() : nPlayerSize( 0 ), byPlayerSelectPage( 0 )
	{
		memset( szAccount, 0, sizeof( char ) * MAX_ACCOUNT );
		memset( szPassword, 0, sizeof( char ) * MAX_ENCRYPT_PWD );
		memset( szPlayer, 0, sizeof( char ) * MAX_PLAYER );
	}

}	ACCOUNT_INFO, *LPACCOUNT_INFO;

typedef struct tagDB_OVERLAPPED_PLUS
{
	OVERLAPPED	Overlapped;
	int		nQueryMode;
	//	mulcom	BEGIN100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.
	BYTE*	lpBuf;
	//BYTE	lpBuf[CAr::nGrowSize * 2];
	//	mulcom	END100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.
	u_long	uBufSize;
	ACCOUNT_INFO	AccountInfo;
	DPID	dpid;
	DPID	dpidCache;
	DPID	dpidUser;
	DWORD	dwAuthKey;
	tagDB_OVERLAPPED_PLUS()
	{
		//	mulcom	BEGIN100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.
		lpBuf	= NULL;
		//mem_set( lpBuf, 0, sizeof(lpBuf) );
		//	mulcom	END100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.
		
		uBufSize = 0;
	}
}	DB_OVERLAPPED_PLUS,	*LPDB_OVERLAPPED_PLUS;

// 길드쿼리문장을 만든다.
struct GUILD_QUERYINFO
{
	LPCTSTR	pszType;
	u_long	idPlayer;
	u_long	idGuild;
	LPCTSTR	pszGuild;
	DWORD   dwLv1;
	DWORD   dwLv2;
	DWORD   dwLv3;
	DWORD   dwLv4;
	WORD	nLevel;
	int     nGuildGold;
	int     nGuildPxp;
	int     nWin;
	int		nLose;
	int     nSurrender;
	DWORD   dwLogo;
	int		nClass;
	LPCTSTR pszNotice;

	GUILD_QUERYINFO(LPCTSTR pszQueryType)
	{
		pszType = pszQueryType;
		idPlayer = 1;
		idGuild = 1;
		pszGuild = "";
		dwLv1 = dwLv2 = dwLv3 = dwLv4 = 0;
		nLevel = 0;
		nGuildGold = 0;
		nGuildPxp = 0;
		nWin = 0;
		nLose = 0;
		nSurrender = 0;
		dwLogo = 0;
		nClass = 0;
		pszNotice = "";
	};
};

// 길드 투표 쿼리문장을 만든다.
struct VOTE_QUERYINFO
{
	LPCTSTR		pszType;
	u_long		idVote;
	u_long		idGuild;
	LPCTSTR		pszTitle;
	LPCTSTR		pszQuestion;
	LPCTSTR		pszSelections[4];
	BYTE		cbSelection;

	VOTE_QUERYINFO(LPCTSTR pszQueryType)
	{
		pszType = pszQueryType;
		idVote = 0;
		idGuild = 0;
		pszTitle = "";
		pszQuestion = "";
		for( int i=0; i<4; ++i )
			pszSelections[i] = "";

		cbSelection = 0;
	};
}; 

// 길드 Log 쿼리문장을 만든다.
struct GUILDLOG_QUERYINFO
{
	LPCTSTR		pszType;			// 로그 타입
	u_long		idGuild;			// 길드 아이디
	u_long		idPlayer;			// 당한자
	u_long		idDoPlayer;			// 시킨자
	int			nGuildGold;			// 길드 골드
	int			nGuildPxp;			// 길드 Pxp
	int			nGuildLv;			// 길드 레벨
	int			nLevel;				// 당한자 레벨
	int 		nItem;				// 아이템 아이디 / 거래 페냐량
	int			nAbilityOption;		// 아이템 옵션
	int			nItem_count;		// 거래 갯수
	SERIALNUMBER	iUniqueNo;			// 유니크 넘버
	LPCTSTR		pszTime;			// 시각
	LPCTSTR		pszGuildBank;		// 길드 뱅크
	LPCTSTR		pszState;
	
	GUILDLOG_QUERYINFO(LPCTSTR pszQueryType)
	{
		pszType = pszQueryType;
		idGuild = 0;	
		idPlayer = 0;	
		idDoPlayer = 0;	
		nGuildGold = 0;	
		nGuildPxp = 0;	
		nGuildLv = 0;	
		nLevel = 0;		
		nItem = 0;
		nAbilityOption = 0;
		nItem_count = 0 ;
		iUniqueNo = 0;
		pszTime = "";
		pszGuildBank = "";
		pszState = "";
	};
}; 

struct ADDBANK_QUERYINFO
{
	LPCTSTR		pszType;
	u_long		idPlayer;
	LPCTSTR		pszBank;
	LPCTSTR		pszBankIndex;
	LPCTSTR		pszObjIndexBank;
	LPCTSTR		pszExtBank;
	LPCTSTR		pszPirecingBank;
	DWORD		dwGoldBank;
	LPCTSTR		pszBankPet;

	ADDBANK_QUERYINFO( LPCTSTR pszQueryType )
	{
		pszType = pszQueryType;
		idPlayer = 0;
		pszBank = "";
		pszBankIndex = "";
		pszObjIndexBank = "";
		pszExtBank = "";
		pszPirecingBank = "";
		dwGoldBank = 0;
		pszBankPet	= "";
	};
};

typedef	struct	_PocketParam
{
	u_long		idPlayer;
	int		nPocket;
	const char*	pszItem;
	const char* pszIndex;
	const char* pszObjIndex;
	const char* pszExt;
	const char* pszPiercing;
	const char* pszPet;
	BOOL	bExpired;
	time_t	tExpirationDate;

	_PocketParam()
	{
		idPlayer	= 0;
		nPocket		= 0;
		pszItem		= "";
		pszIndex	= "";
		pszObjIndex		= "";
		pszExt	= "";
		pszPiercing		= "";
		pszPet	= "";
		bExpired	= TRUE;
		tExpirationDate		= 0;
	}
}	PocketParam;

struct WAR_QUERYINFO
{
	LPCTSTR	pszType;
	u_long	idWar;
	u_long	idGuild;
	u_long	idPlayer;
	u_long	f_idGuild;
	int		nWinPoint;
	int		f_nWinPoint;
	char	iState;

	WAR_QUERYINFO( LPCTSTR pszQueryType )
	{
		pszType = pszQueryType;
		idWar	= 0;
		idGuild		= 0;
		iState	= '0';
		f_idGuild	= 0;
		idPlayer	= 0;
		nWinPoint	= 0;
		f_nWinPoint	= 0;
	};
};

///////////////////////////////////////////////////////////////////////
// WANTED_QUERYINFO
///////////////////////////////////////////////////////////////////////

// 길드 투표 쿼리문장을 만든다.
struct WANTED_QUERYINFO
{
	LPCTSTR		pszType;
	u_long		idPlayer;
	int			nGold;
	long		nEnd;
	char		szMsg[WANTED_MSG_MAX + 1];

	WANTED_QUERYINFO(LPCTSTR pszQueryType)
	{
		pszType = pszQueryType;
		idPlayer = 0;
		nGold = 0;
		nEnd = 0;
		szMsg[0] = '\0';
	};
}; 




///////////////////////////////////////////////////////////////////////
// ACCOUNT_CACHE
///////////////////////////////////////////////////////////////////////
struct ACCOUNT_CACHE
{
	time_t							m_tmPrevent;
	CMover*							pMover[MAX_CHARACTER_SLOT];
	u_long							m_idPlayerBank[MAX_CHARACTER_SLOT];
	CItemContainer< FLItemElem  >	m_Bank[MAX_CHARACTER_SLOT];
	DWORD							m_dwGoldBank[MAX_CHARACTER_SLOT];
	time_t							m_tmLastAccess;
	std::string							m_strAccount;

	// BEGIN110520
	BYTE							m_byDBDataSlot;
	// END110520

	void	Init();
	void	Clear();
	void	ClearMover( BYTE nSlot, u_long idPlayer );
	BOOL	IsReleaseable();
private:
	u_long	m_uMultiServer;
public:
	u_long	GetMultiServer( void )	{	return m_uMultiServer;	}
	void	ChangeMultiServer( u_long uMultiServer )	{ m_uMultiServer	= uMultiServer;	}
	BOOL	IsPlaying( void )	{ return m_uMultiServer != 0;		}
};

inline void ACCOUNT_CACHE::Init()
{
	m_tmPrevent = 0;
	m_tmLastAccess = ::time( NULL );

	for( int i = 0 ; i < MAX_CHARACTER_SLOT ; ++i )
	{
		pMover[i] = NULL;
		m_Bank[i].SetItemContainer( ITYPE_ITEM, CONTAINER_TYPE_USER_BANK, MAX_BANK ) ;
		m_dwGoldBank[i] = 0;
		m_idPlayerBank[i] = 0;
	}
	m_uMultiServer	= 0;
}


inline void ACCOUNT_CACHE::Clear()
{
	for( int i = 0 ; i < MAX_CHARACTER_SLOT ; ++i )
	{
		SAFE_DELETE( pMover[i] );
	}
}

inline void ACCOUNT_CACHE::ClearMover( BYTE nSlot, u_long idPlayer )
{
	m_Bank[nSlot].Clear();
	m_dwGoldBank[nSlot]	= 0;
	m_idPlayerBank[nSlot] = idPlayer;
	SAFE_DELETE( pMover[nSlot] );
}


inline BOOL ACCOUNT_CACHE::IsReleaseable()
{
	time_t t	= m_tmLastAccess + TIMEWAIT_RELEASE;
	if( ::time( NULL ) > t )
		return TRUE;
	return FALSE;
}


#define	MAX_QUERY_RESERVED	2

//typedef CMyMap2<CMover*>	C2Mover;
//typedef CMyBucket2<CMover*> CMoverBucket;
typedef	std::set<std::string>			SET_STRING;
typedef std::map<u_long, std::string>	ULONG2STRING;
typedef std::map<int, std::string>	INT2STRING;

typedef	struct	_ItemStruct
{
	char	szItem[512];
	char	szExt[64];
	char	szPiercing[256];

	char	szPet[100];

	_ItemStruct()
		{
			szItem[0]	= '\0';
			szExt[0]	= '\0';
			szPiercing[0]	= '\0';
			szPet[0]	= '\0';
		}
}	ItemStruct, *PItemStruct;

// typedef	struct	_ItemContainerStruct
// {
// 	char	szItem[6144];
// 	char	szIndex[512];
// 	char	szObjIndex[512];
// 	char	szExt[2048];
// 	char	szPiercing[8000];
// 	char	szPet[4200];	// 42 * 100
// 
// 	_ItemContainerStruct()
// 		{
// 			*szItem	= '\0';
// 			*szIndex	= '\0';
// 			*szObjIndex	= '\0';
// 			*szExt	= '\0';
// 			*szPiercing	= '\0';
// 			*szPet	= '\0';
// 		}
// }	ItemContainerStruct, *PItemContainerStruct;

// typedef	struct	_PocketStruct : public ItemContainerStruct
// {
// 	BOOL	bExpired;
// 	time_t	tExpirationDate;
// 	_PocketStruct() : ItemContainerStruct()
// 	{
// 		bExpired	= TRUE;
// 		tExpirationDate		= 0;
// 	}
// }	PocketStruct,	*PPocketStruct;


typedef	struct	_CONV_RESULT_ITEM
{
	DWORD	dwItemId;
	int		nAbilityOption;
	_CONV_RESULT_ITEM()
	{
		dwItemId	= 0;
		nAbilityOption	= 0;
	}
}	CONV_RESULT_ITEM;

class CGuildMng;
class CMail;

typedef	std::map<DWORD, CONV_RESULT_ITEM>	MDC;

class CDbManager
{
public:
	//////////////////////////////////////////////////////////////////////////
	// BEGIN110119

	typedef struct _T_CHARACTER_STR_INFO
	{
		// 1
		TCHAR			szGu[3];
		DWORD			dwPlayerId;
		TCHAR			szAccount[MAX_ACCOUNT];
		TCHAR			szPlayerName[MAX_PLAYER];
		int				nPlayerSlot;
		DWORD			dwWorldId;
		DWORD			dwIndex;
		float			fPos_x;
		float			fPos_y;

		// 11
		float			fPos_z;
		TCHAR			szCharacterKey[33];
		DWORD			dwSkinSet;
		DWORD			dwHairMesh;
		DWORD			dwHairColor;
		DWORD			dwHeadMesh;
		DWORD			dwSex;
		float			fScale_x;
		DWORD			dwMotion;
		float			fAngle;

		// 21
		int				nHitPoint;
		int				nManaPoint;
		int				nFatiguePoint;
		DWORD			dwRideItemId;
		DWORD			dwGold;
		int				nJob;
		TCHAR			szActMover[51];
		int				nStr;
		int				nSta;
		int				nDex;

		// 31
		int				nInt;
		int				nLevel;
		EXPINTEGER		n64Exp1;
		EXPINTEGER		n64Exp2;
		TCHAR			szJobSkill[501];
		TCHAR			szLicenseSkill[501];
		TCHAR			szJobLv[501];
		DWORD			dwExpertLv;
		DWORD			dwMarkingWorldId;
		float			fMarkingPos_x;

		// 41
		float			fMarkingPos_y;
		float			fMarkingPos_z;
		int				nRemainGP;
		int				nRemainLP;
		int				nTutorialState;	// @im_nFlightLv
		int				nFxp;
		int				nTxp;
		TCHAR			szQuestCntArray[3073];
		TCHAR			szAuthority[2];
		DWORD			dwMode;

		// 51
		int				nPartyId;
		DWORD			dwMuerdererId;
		int				nFame;
		EXPINTEGER		n64DeathExp;
		int				nDeathLevel;
		DWORD			dwFlyTime;
		DWORD			dwMessengerState;
		int				nTotalPlayTime;
		int				nPKValue;
		DWORD			dwPKPropensity;

		// 61
		DWORD			dwPKExp;
		TCHAR			szCard[1981];
		TCHAR			szIndex_Card[216];
		TCHAR			szObjIndex_Card[216];
		TCHAR 			szCube[1981];
		TCHAR 			szIndex_Cube[216];
		TCHAR 			szObjIndex_Cube[216];
		TCHAR 			szInventory[6941];
		TCHAR 			szIndex_Inventory[346];
		TCHAR 			szEquipment[136];

		// 71
		TCHAR 			szObjIndex_Inventory[346];
		TCHAR 			szSlotApplet[3101];
		TCHAR 			szSlotItem[6886];
		TCHAR 			szSlotQueue[226];
		int				nActionPoint;		// @im_SkillBar
		TCHAR 			szBank[4291];
		TCHAR 			szIndex_Bank[216];
		TCHAR 			szObjIndex_Bank[216];
		DWORD 			dwGoldBank;
		int				nFuel;

		// 81
		int				tmAccFuel;
		TCHAR 			szSMTime[2561];
		TCHAR 			szSkillInfluence[7501];
		DWORD 			dwSkillPoint;
		TCHAR 			szCompleteQuest[3073];
		TCHAR 			szExtInven[2001];
		TCHAR 			szExtBank[2001];
		TCHAR 			szPirecingInven[8001];
		TCHAR 			szPirecingBank[8001];
		DWORD 			dwReturnWorldId;

		// 91
		float 			fReturnPos_x;
		float 			fReturnPos_y;
		float 			fReturnPos_z;
		int 			nSkillPoint;
		int 			nSkillLevel;
		__int64 		n64SkillExp;
		__int64 		n64EventFlag;
		DWORD 			dwEventTime;
		DWORD 			dwEventElapsed;
		EXPINTEGER		n64AngelExp;

		// 101
		LONG			nAngelLevel;
		TCHAR 			szInventoryPet[4201];
		TCHAR 			szBankPet[4201];
		DWORD 			dwPetId;
		int 			nExpLog;
		int 			nAngelExpLog;
		int 			nCoupon;
		int 			nHonor;
		int 			nLayer;
		TCHAR			szCheckedQuest[101];

		// 111
		int				nCampusPoint;
		DWORD			dwCampusId;
		int				dcStr {0};
		int				dcSta { 0};
		int				dcDex { 0};
		int				dcInt { 0};

	private:
		_T_CHARACTER_STR_INFO();

	public:
		explicit _T_CHARACTER_STR_INFO( CMover & _Mover, const TCHAR* _pGu )
		{
			FLStrcpy( szGu, _countof( szGu ), _pGu );
			dwPlayerId					=	_Mover.m_idPlayer;
			FLStrcpy( szAccount, _countof( szAccount ), _Mover.m_szAccount );
			FLStrcpy( szPlayerName, _countof( szPlayerName ), _Mover.m_szName );
			nPlayerSlot					=	_Mover.m_nDBDataSlot;
			dwWorldId					=	_Mover.m_dwWorldID;
			dwIndex						=	_Mover.m_dwIndex;
			fPos_x						=	_Mover.m_vPos.x;
			fPos_y						=	_Mover.m_vPos.y;

			// 11
			fPos_z						=	_Mover.m_vPos.z;
			FLStrcpy( szCharacterKey, _countof( szCharacterKey ), _Mover.m_szCharacterKey );
			dwSkinSet					=	_Mover.m_dwSkinSet;
			dwHairMesh					=	_Mover.m_dwHairMesh;
			dwHairColor					=	_Mover.m_dwHairColor;
			dwHeadMesh					=	_Mover.m_dwHeadMesh;
			dwSex						=	_Mover.GetSex();
			fScale_x					=	_Mover.m_vScale.x;
			dwMotion					=	_Mover.m_dwMotion;
			fAngle						=	_Mover.m_fAngle;

			// 21
			nHitPoint					=	_Mover.m_nHitPoint;
			nManaPoint					=	_Mover.m_nManaPoint;
			nFatiguePoint				=	_Mover.m_nFatiguePoint;
			dwRideItemId				=	_Mover.m_dwRideItemIdx;
			dwGold						=	_Mover.GetGold();
			nJob						=	_Mover.m_nJob;

			TCHAR ActMover[50] = { 0, };
			FLSPrintf( ActMover, _countof( ActMover ), "%d,%d", _Mover.m_pActMover->m_dwState, _Mover.m_pActMover->m_dwStateFlag );

			FLStrcpy( szActMover, _countof( szActMover ), ActMover );
			nStr						=	_Mover.m_Stat.GetOriginStr();
			nSta						=	_Mover.m_Stat.GetOriginSta();
			nDex						=	_Mover.m_Stat.GetOriginDex();

			// 31
			nInt						=	_Mover.m_Stat.GetOriginInt();
			nLevel						=	_Mover.m_nLevel;
			n64Exp1						=	_Mover.m_nExp1;
			n64Exp2						=	0;
			szJobSkill[ 0 ]				=	'\0';
			szLicenseSkill[ 0 ]			=	'\0';

			TCHAR JobLv[500] = { 0, };
			CDbManager::SaveJobLv( &_Mover, JobLv, _countof( JobLv ) );
			FLStrcpy( szJobLv, _countof( szJobLv ), JobLv );
			dwExpertLv					=	0;
			dwMarkingWorldId			=	_Mover.m_idMarkingWorld;
			fMarkingPos_x				=	_Mover.m_vMarkingPos.x;

			// 41
			fMarkingPos_y				=	_Mover.m_vMarkingPos.y;
			fMarkingPos_z				=	_Mover.m_vMarkingPos.z;
			nRemainGP					=	_Mover.m_Stat.GetRemainStatPoint();
			nRemainLP					=	_Mover.m_kMadrigalGiftPoint.nPoint;
			nTutorialState				=	_Mover.GetTutorialState();		// @im_nFligh
			nFxp						=	_Mover.m_nFxp;
			nTxp						=	_Mover.m_nFxp;

			TCHAR QuestCnt[3072]	= { 0, };
			TCHAR CompleteQuest[3072] = { 0, };	// 1024 -> 3072
			TCHAR CheckedQuest[100] = { 0, };
			CDbManager::SaveQuest( &_Mover, QuestCnt, _countof( QuestCnt ), CompleteQuest, _countof( CompleteQuest ), CheckedQuest, _countof( CheckedQuest ) );
			FLStrcpy( szQuestCntArray, _countof( szQuestCntArray ), QuestCnt );
			FLStrcpy( szAuthority, _countof( szAuthority ), "F" );
			dwMode						=	_Mover.m_dwMode;

			// 51
			nPartyId					=	_Mover.m_idparty;
			dwMuerdererId				=	_Mover.m_idMurderer;
			nFame						=	_Mover.m_nFame;
			n64DeathExp					=	_Mover.m_nDeathExp;
			nDeathLevel					=	_Mover.m_nDeathLevel;
			dwFlyTime					=	_Mover.m_dwMute;
			dwMessengerState			=	_Mover.m_RTMessenger.GetState();
			nTotalPlayTime				=	0;
			nPKValue					=	_Mover.m_nPKValue;
			dwPKPropensity				=	_Mover.m_dwPKPropensity;

			// 61
			dwPKExp						=	_Mover.m_dwPKExp;
			FLStrcpy( szCard, _countof( szCard ), "$" );
			FLStrcpy( szIndex_Card, _countof( szIndex_Card ), "$" );
			FLStrcpy( szObjIndex_Card, _countof( szObjIndex_Card ), "$" );
			FLStrcpy( szCube, _countof( szCube ), "$" );
			FLStrcpy( szIndex_Cube, _countof( szIndex_Cube ), "$" );
			FLStrcpy( szObjIndex_Cube, _countof( szObjIndex_Cube ), "$" );
			FLStrcpy( szInventory, _countof( szInventory ), "$" );
			FLStrcpy( szIndex_Inventory, _countof( szIndex_Inventory ), "$" );

			TCHAR Equipment[500] = { 0, };
			CDbManager::SaveEquipment( &_Mover, Equipment, _countof( Equipment ) );
			FLStrcpy( szEquipment, _countof( szEquipment ), Equipment );

			// 71
			FLStrcpy( szObjIndex_Inventory, _countof( szObjIndex_Inventory ), "$" );

			TCHAR AppletTaskBar[MAX_APPLETTASKBAR] = { 0, }; 
			TCHAR ItemTaskBar[MAX_ITEMTASKBAR] = { 0, };  
			TCHAR SkillTaskBar[1024] = { 0, };
			CDbManager::SaveTaskBar( &_Mover, AppletTaskBar, _countof( AppletTaskBar ), ItemTaskBar, _countof( ItemTaskBar ), SkillTaskBar, _countof( SkillTaskBar ) );
			FLStrcpy( szSlotApplet, _countof( szSlotApplet ), AppletTaskBar );
			FLStrcpy( szSlotItem, _countof( szSlotItem ), ItemTaskBar );
			FLStrcpy( szSlotQueue, _countof( szSlotQueue ), SkillTaskBar );

			nActionPoint				=	_GetContentState( CT_NEWUI_19 ) == CS_VER1 ? _Mover.m_UserTaskBar.m_nActionPoint : 0;	// @im_SkillBar

			FLStrcpy( szBank, _countof( szBank ), "$" );
			FLStrcpy( szIndex_Bank, _countof( szIndex_Bank ), "$" );
			FLStrcpy( szObjIndex_Bank, _countof( szObjIndex_Bank ), "$" );

			const BYTE ProgramDataSlot	= GET_PLAYER_SLOT( _Mover.m_nDBDataSlot );
			dwGoldBank					=	_Mover.m_dwGoldBank[ProgramDataSlot];
			nFuel						=	_Mover.m_nFuel;

			// 81
			tmAccFuel					=	_Mover.m_tmAccFuel;

			TCHAR SMTime[3072] = { 0, };
			CDbManager::SaveSMCode( &_Mover, SMTime, _countof( SMTime ) );

			TCHAR SkillInfluence[7500] = { 0, };
			CDbManager::SaveSkillInfluence( &_Mover, SkillInfluence, _countof( SkillInfluence ) );
			FLStrcpy( szSMTime, _countof( szSMTime ), SMTime );
			FLStrcpy( szSkillInfluence, _countof( szSkillInfluence ), SkillInfluence );
			dwSkillPoint				=	0;
			FLStrcpy( szCompleteQuest, _countof( szCompleteQuest ), CompleteQuest );
			FLStrcpy( szExtInven, _countof( szExtInven ), "$" );
			FLStrcpy( szExtBank, _countof( szExtBank ), "$" );
			FLStrcpy( szPirecingInven, _countof( szPirecingInven ), "$" );
			FLStrcpy( szPirecingBank, _countof( szPirecingBank ), "$" );
			dwReturnWorldId				=	_Mover.m_dwReturnWorldID;

			// 91
			fReturnPos_x				=	_Mover.m_vReturnPos.x;
			fReturnPos_y				=	_Mover.m_vReturnPos.y;
			fReturnPos_z				=	_Mover.m_vReturnPos.z;
			nSkillPoint					=	_Mover.m_nSkillPoint;
			nSkillLevel					=	_Mover.m_nSkillLevel;
			n64SkillExp					=	0;
			n64EventFlag				=	_Mover.m_nEventFlag;
			dwEventTime					=	_Mover.m_dwEventTime;
			dwEventElapsed				=	_Mover.m_dwEventElapsed;
			n64AngelExp					=	_Mover.m_nAngelExp;

			// 101
			nAngelLevel					=	_Mover.m_nAngelLevel;
			FLStrcpy( szInventoryPet, _countof( szInventoryPet ), "$" );
			FLStrcpy( szBankPet, _countof( szBankPet ), "$" );
			dwPetId						=	_Mover.GetPetId();
			nExpLog						=	_Mover.m_nExpLog;
			nAngelExpLog				=	_Mover.m_nAngelExpLog;
			nCoupon						=	_Mover.m_nCoupon;
			nHonor						=	_Mover.m_nHonor;
			nLayer						=	_Mover.GetLayer();
			FLStrcpy( szCheckedQuest, _countof( szCheckedQuest ), CheckedQuest );

			// 111
			nCampusPoint				=	_Mover.GetCampusPoint();
			dwCampusId					=	_Mover.GetCampusId();
			dcStr						=	_Mover.m_Stat.GetDiceStr();
			dcSta						=	_Mover.m_Stat.GetDiceSta();
			dcDex						=	_Mover.m_Stat.GetDiceDex();
			dcInt						=	_Mover.m_Stat.GetDiceInt();
		}

	} T_CHARACTER_STR_INFO, *PT_CHARACTER_STR_INFO;


	typedef struct _T_ITEM_QUERY_INFO
	{
		DWORD			dwObjID;
		DWORD			dwObjIndex;
		DWORD			dwItemID;
		int				nItemNum;
		SERIALNUMBER	dwSerialNumber;
		int				nAbilityOption;
		int				nItemResist;
		int				nResistAbilityOption;
		int				nFlag;
		BOOL			bCharged;
		DWORD			dwKeepTime;
		__int64			n64RandomOptID;
		int				nHitPoint;
		int				nRepairNumber;
		int				nRepair;
		BOOL			bIsTransformVisPet;
		size_t			nUltimatePiercingSize;
		DWORD			arrUltimatePiercingItemID[ MAX_PIERCING_ULTIMATE ];
		size_t			nGeneralPiercingSize;
		DWORD			arrGeneralPiercingItemID[ MAX_PIERCING ];
		time_t			arrVisPetPiercingItemKeepTime[ MAX_PIERCING ];
		BOOL			bIsPet;
		int				nPetKind;
		int				nPetLevel;
		DWORD			dwPetExp;
		DWORD			dwPetEnergy;
		DWORD			dwPetLife;
		int				arrPetAvailLevel[ PL_MAX ];
		TCHAR			szPetName[ 65 ];
		int				nSerialNumberStatus;
		SERIALNUMBER	dwManagedSerialNumber;
		DWORD			dwCoupleID;
		int				nLevelDown;
		DWORD			dwRandomOptionOriginID;
		size_t			nRandomOptionExtensionSize;
		DWORD			dwRandomOptionExtensionFlag;
		DWORD			arrRandomOptionExtensionDstID[ MAX_RANDOMOPTION_DB_COLUMN ];
		int				arrRandomOptionExtensionAdjValue[ MAX_RANDOMOPTION_DB_COLUMN ];


		int				nGuildID;
		int				nResistSMItemID;


		DWORD			dwLooksChangeItemID;
		BOOL			bIsCombined;
		DWORD			dwCombinedAddDamage;
		DWORD			dwCombinedRandomOptionOriginID;
		size_t			nCombinedGeneralPiercingSize;
		DWORD			arrCombinedGeneralPiercingItemID[ MAX_PIERCING ];
		size_t			nCombinedRandomOptionExtensionSize;
		DWORD			arrCombinedRandomOptionExtensionDstID[ MAX_RANDOMOPTION_DB_COLUMN ];
		int				arrCombinedRandomOptionExtensionAdjValue[ MAX_RANDOMOPTION_DB_COLUMN ];


		TCHAR			szState[2];

		_T_ITEM_QUERY_INFO()
		{
			dwObjID								= NULL_ID;
			dwObjIndex							= NULL_ID;
			dwItemID							= 0;
			nItemNum							= 0;
			dwSerialNumber						= 0;
			nAbilityOption						= 0;
			nItemResist							= SAI79::NO_PROP;
			nResistAbilityOption				= 0;
			nFlag								= 0;
			bCharged							= FALSE;
			dwKeepTime							= 0;
			n64RandomOptID						= 0;
			nHitPoint							= -1;
			nRepairNumber						= 0;
			nRepair								= 0;
			bIsTransformVisPet					= FALSE;
			nUltimatePiercingSize				= 0;
			::memset( arrUltimatePiercingItemID, 0, sizeof( arrUltimatePiercingItemID ) );
			nGeneralPiercingSize				= 0;
			::memset( arrGeneralPiercingItemID, 0, sizeof( arrGeneralPiercingItemID ) );
			::memset( arrVisPetPiercingItemKeepTime, 0, sizeof( arrVisPetPiercingItemKeepTime ) );
			bIsPet								= FALSE;
			nPetKind							= 0;
			nPetLevel							= 0;
			dwPetExp							= 0;
			dwPetEnergy							= 0;
			dwPetLife							= 0;
			::memset( arrPetAvailLevel, 0, sizeof( arrPetAvailLevel ) );
			szPetName[0]						= _T( '\0' );
			nSerialNumberStatus					= 0;
			dwManagedSerialNumber				= 0;
			dwCoupleID							= NULL_PLAYER_ID;
			nLevelDown							= 0;
			dwRandomOptionOriginID				= 0;
			nRandomOptionExtensionSize			= 0;
			dwRandomOptionExtensionFlag			= 0;
			::memset( arrRandomOptionExtensionDstID, 0, sizeof( arrRandomOptionExtensionDstID ) );
			::memset( arrRandomOptionExtensionAdjValue, 0, sizeof( arrRandomOptionExtensionAdjValue ) );


			nGuildID							= 0;
			nResistSMItemID						= 0;


			dwLooksChangeItemID					= 0;
			bIsCombined							= FALSE;
			dwCombinedAddDamage					= 0;
			dwCombinedRandomOptionOriginID		= 0;
			nCombinedGeneralPiercingSize		= 0;
			::memset( arrCombinedGeneralPiercingItemID, 0, sizeof( arrCombinedGeneralPiercingItemID ) );
			nCombinedRandomOptionExtensionSize	= 0;
			::memset( arrCombinedRandomOptionExtensionDstID, 0, sizeof( arrCombinedRandomOptionExtensionDstID ) );
			::memset( arrCombinedRandomOptionExtensionAdjValue, 0, sizeof( arrCombinedRandomOptionExtensionAdjValue ) );

			//szState[0]							= _T( '\0' );
			FLStrcpy( szState, _countof( szState ), _T( "T" ) );
		}

		void	Copy( FLItemElem & kItemElem )
		{
			dwObjID							= kItemElem.m_dwObjId;
			dwObjIndex						= kItemElem.m_dwObjIndex;
			dwItemID						= kItemElem.m_dwItemId;
			nItemNum						= kItemElem.m_nItemNum;
			dwSerialNumber					= kItemElem.GetSerialNumber();
			nAbilityOption					= kItemElem.GetAbilityOption();
			nItemResist						= kItemElem.GetItemResist();
			nResistAbilityOption			= kItemElem.GetResistAbilityOption();
			nFlag							= kItemElem.m_byFlag;
			bCharged						= kItemElem.m_bCharged;
			dwKeepTime						= kItemElem.m_dwKeepTime;
			nHitPoint						= kItemElem.m_nHitPoint;
			nRepairNumber					= kItemElem.m_nRepairNumber;
			nRepair							= kItemElem.m_nRepair;
			bIsTransformVisPet				= kItemElem.IsTransformVisPet();
			
			if( kItemElem.IsSetUltimatePiercingOption() == true )
			{
				nUltimatePiercingSize			= kItemElem.GetUltimatePiercingSize();
				for( size_t Nth = 0; Nth < MAX_PIERCING_ULTIMATE; ++Nth )
				{
					arrUltimatePiercingItemID[ Nth ]		= kItemElem.GetUltimatePiercingItemID( Nth );
				}
			}

			if( kItemElem.IsSetGeneralPiercingOption() == true )
			{
				nGeneralPiercingSize			= kItemElem.GetGeneralPiercingSize();
				for( size_t Nth = 0; Nth < MAX_PIERCING; ++Nth )
				{
					arrGeneralPiercingItemID[ Nth ]			= kItemElem.GetGeneralPiercingItemID( Nth );
					arrVisPetPiercingItemKeepTime[ Nth ]	= kItemElem.GetVisPetPiercingItemKeepTime( Nth );
				}
			}

			if( kItemElem.m_pPet != NULL )
			{
				bIsPet						= TRUE;
				nPetKind					= kItemElem.m_pPet->GetKind();
				nPetLevel					= kItemElem.m_pPet->GetLevel();
				dwPetExp					= kItemElem.m_pPet->GetExp();
				dwPetEnergy					= kItemElem.m_pPet->GetEnergy();
				dwPetLife					= kItemElem.m_pPet->GetLife();
				
				for( BYTE byLevel = PL_D; byLevel < PL_MAX; ++byLevel )
				{
					arrPetAvailLevel[ byLevel ]	= kItemElem.m_pPet->GetAvailLevel( byLevel );
				}

				FLStrcpy( szPetName, _countof( szPetName ), kItemElem.m_pPet->GetName() );
			}
			
			dwCoupleID						= kItemElem.GetCoupleId();
			nLevelDown						= kItemElem.GetLevelDown();
			dwRandomOptionOriginID			= kItemElem.GetRandomOptionOriginID();

			if( kItemElem.IsSetRandomOptionExtension() == true )
			{
				nRandomOptionExtensionSize		= kItemElem.GetRandomOptionExtensionSize();
				dwRandomOptionExtensionFlag		= kItemElem.GetRandomOptionExtensionFlag();

				for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
				{
					arrRandomOptionExtensionDstID[ Nth ]	= kItemElem.GetRandomOptionExtensionDstID( Nth*2, Nth*2+1 );
					arrRandomOptionExtensionAdjValue[ Nth ]	= kItemElem.GetRandomOptionExtensionAdjValue( Nth*2, Nth*2+1 );
				}
			}


			nGuildID						= kItemElem.m_idGuild;
			nResistSMItemID					= kItemElem.m_nResistSMItemId;

			
			dwLooksChangeItemID				= kItemElem.GetLooksChangeItemID();
			bIsCombined						= kItemElem.IsSetCombinedOption();
			dwCombinedAddDamage				= kItemElem.GetCombinedAddDamage();
			dwCombinedRandomOptionOriginID	= kItemElem.GetCombinedRandomOptionOriginID();

			if( kItemElem.IsSetCombinedGeneralPiercingOption() == true )
			{
				nCombinedGeneralPiercingSize					= kItemElem.GetCombinedGeneralPiercingSize();

				for( size_t Nth = 0; Nth < MAX_PIERCING; ++Nth )
				{
					arrCombinedGeneralPiercingItemID[ Nth ]		= kItemElem.GetCombinedGeneralPiercingItemID( Nth );
				}
			}

			if( kItemElem.IsSetCombinedRandomOptionExtension() == true )
			{
				nCombinedRandomOptionExtensionSize					= kItemElem.GetCombinedRandomOptionExtensionSize();

				for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
				{
					arrCombinedRandomOptionExtensionDstID[ Nth ]	= kItemElem.GetCombinedRandomOptionExtensionDstID( Nth*2, Nth*2+1 );
					arrCombinedRandomOptionExtensionAdjValue[ Nth ]	= kItemElem.GetCombinedRandomOptionExtensionAdjValue( Nth*2, Nth*2+1 );
				}
			}
		}

	} T_ITEM_QUERY_INFO, *PT_ITEM_QUERY_INFO;


	typedef struct _T_MAIL_STR_INFO
	{
		TCHAR			szGu[3];
		DWORD			dwMailId;
		DWORD			dwReceivePlayerId;
		DWORD			dwSendPlayerId;
		int				nGold;
		time_t			tmCreate;
		int				nReadDay;
		TCHAR			szMailTitle[129];
		TCHAR			szMailText[1025];
		DWORD			dwItemId;
		int				nItemNum;
		int				nRepairNumber;
		int				nHitPoint;
		int				nRepair;
		int				nMaterial;
		int				nFlag;
		SERIALNUMBER	dwSerialNumber;
		int				nOPtion;
		int				nItemResist;
		int				nResistAbilityOption;
		DWORD			dwGuildId;
		DWORD			dwResistSMItemId;
		BOOL			bCharged;
		DWORD			dwKeepTime;
		__int64			n64RandomOptItemId;
		int				nPiercingSize;
		DWORD			dwPiercedItemId1;
		DWORD			dwPiercedItemId2;
		DWORD			dwPiercedItemId3;
		DWORD			dwPiercedItemId4;
		BOOL			bIsPet;
		int				nPetKind;
		int				nPetLevel;
		DWORD			dwPetExp;
		DWORD			dwPetEnergy;
		DWORD			dwPetLife;
		int				nPetAvailLevel_D;
		int				nPetAvailLevel_C;
		int				nPetAvailLevel_B;
		int				nPetAvailLevel_A;
		int				nPetAvailLevel_S;
		DWORD			dwPiercedItemId5;
		DWORD			dwPiercedItemId6;
		DWORD			dwPiercedItemId7;
		DWORD			dwPiercedItemId8;
		DWORD			dwPiercedItemId9;
		DWORD			dwPiercedItemId10;
		DWORD			dwPiercedItemId11;
		DWORD			dwPiercedItemId12;
		DWORD			dwPiercedItemId13;
		DWORD			dwPiercedItemId14;
		DWORD			dwPiercedItemId15;
		int				nAddPiercingSize;
		TCHAR			szPetName[33];
		SERIALNUMBER	dwManagedSerialNumber;
		DWORD			dwCouplePlayerId;
		int				nLevelDown;
		DWORD			dwRandomOptionOriginId;
		DWORD			dwRandomOptionExtSize;
		DWORD			dwRandomOptionExtFlag;
		DWORD			arrRandomOptionExtensionDstID[ MAX_RANDOMOPTION_DB_COLUMN ];
		int				arrRandomOptionExtensionAdjValue[ MAX_RANDOMOPTION_DB_COLUMN ];

		DWORD			dwLooksChangeItemID;
		BOOL			bIsCombined;
		DWORD			dwCombinedAddDamage;
		DWORD			dwCombinedRandomOptionOriginID;
		size_t			nCombinedGeneralPiercingSize;
		DWORD			arrCombinedGeneralPiercingID[MAX_PIERCING];
		size_t			nCombinedRandomOptionExtensionSize;
		DWORD			arrCombinedRandomOptionExtensionDstID[ MAX_RANDOMOPTION_DB_COLUMN ];
		int				arrCombinedRandomOptionExtensionAdjValue[ MAX_RANDOMOPTION_DB_COLUMN ];


		_T_MAIL_STR_INFO()
		{
			szGu[0]					= '\0';
			dwMailId				= 0;
			dwReceivePlayerId		= NULL_PLAYER_ID;
			dwSendPlayerId			= NULL_PLAYER_ID;
			nGold					= 0;
			tmCreate				= 0;
			nReadDay				= 0;
			szMailTitle[0]			= '\0';
			szMailText[0]			= '\0';
			dwItemId				= 0;
			nItemNum				= 0;
			nRepairNumber			= 0;
			nHitPoint				= 0;
			nRepair					= 0;
			nMaterial				= 0;
			nFlag					= 0;
			dwSerialNumber			= 0;
			nOPtion					= 0;
			nItemResist				= 0;
			nResistAbilityOption	= 0;
			dwGuildId				= 0;
			dwResistSMItemId		= 0;
			bCharged				= FALSE;
			dwKeepTime				= 0;
			n64RandomOptItemId		= 0;
			nPiercingSize			= 0;
			dwPiercedItemId1		= 0;
			dwPiercedItemId2		= 0;
			dwPiercedItemId3		= 0;
			dwPiercedItemId4		= 0;
			bIsPet					= FALSE;
			nPetKind				= 0;
			nPetLevel				= 0;
			dwPetExp				= 0;
			dwPetEnergy				= 0;
			dwPetLife				= 0;
			nPetAvailLevel_D		= 0;
			nPetAvailLevel_C		= 0;
			nPetAvailLevel_B		= 0;
			nPetAvailLevel_A		= 0;
			nPetAvailLevel_S		= 0;
			dwPiercedItemId5		= 0;
			dwPiercedItemId6		= 0;
			dwPiercedItemId7		= 0;
			dwPiercedItemId8		= 0;
			dwPiercedItemId9		= 0;
			dwPiercedItemId10		= 0;
			dwPiercedItemId11		= 0;
			dwPiercedItemId12		= 0;
			dwPiercedItemId13		= 0;
			dwPiercedItemId14		= 0;
			dwPiercedItemId15		= 0;
			nAddPiercingSize		= 0;
			szPetName[0]			= '\0';
			dwManagedSerialNumber	= 0;
			dwCouplePlayerId		= 0;
			nLevelDown				= 0;
			dwRandomOptionOriginId	= 0;
			dwRandomOptionExtSize	= 0;
			dwRandomOptionExtFlag	= 0;
			::memset( arrRandomOptionExtensionDstID, 0, sizeof( arrRandomOptionExtensionDstID ) );
			::memset( arrRandomOptionExtensionAdjValue, 0, sizeof( arrRandomOptionExtensionAdjValue ) );

			dwLooksChangeItemID					= 0;
			bIsCombined							= FALSE;
			dwCombinedAddDamage					= 0;
			dwCombinedRandomOptionOriginID		= 0;
			nCombinedGeneralPiercingSize		= 0;
			memset( arrCombinedGeneralPiercingID, 0, sizeof( arrCombinedGeneralPiercingID ) );
			nCombinedRandomOptionExtensionSize	= 0;
			::memset( arrCombinedRandomOptionExtensionDstID, 0, sizeof( arrCombinedRandomOptionExtensionDstID ) );
			::memset( arrCombinedRandomOptionExtensionAdjValue, 0, sizeof( arrCombinedRandomOptionExtensionAdjValue ) );
		}


		void	CopyMailInfo( CMail* pMail )
		{
			if( pMail != NULL )
			{
				dwMailId			=	pMail->m_nMail;
				dwSendPlayerId		=	pMail->m_idSender;
				nGold				=	pMail->m_nGold;
				tmCreate			=	pMail->m_tmCreate;
				nReadDay			=	pMail->m_byRead;
				n64RandomOptItemId	=	static_cast<__int64>( 0 );

				if( pMail->m_pItemElem != NULL && pMail->m_pItemElem->GetProp() != NULL )
				{
					dwItemId				=	pMail->m_pItemElem->m_dwItemId;
					nItemNum				=	pMail->m_pItemElem->m_nItemNum;
					nRepairNumber			=	pMail->m_pItemElem->m_nRepairNumber;
					nHitPoint				=	pMail->m_pItemElem->m_nHitPoint;
					nRepair					=	pMail->m_pItemElem->m_nRepair;
					nFlag					=	pMail->m_pItemElem->m_byFlag;
					dwSerialNumber			=	pMail->m_pItemElem->GetSerialNumber();
					nOPtion					=	pMail->m_pItemElem->GetAbilityOption();
					nItemResist				=	pMail->m_pItemElem->m_byItemResist;
					nResistAbilityOption	=	pMail->m_pItemElem->m_nResistAbilityOption;
					dwGuildId				=	pMail->m_pItemElem->m_idGuild;
					dwResistSMItemId		=	pMail->m_pItemElem->m_nResistSMItemId;
					bCharged				=	pMail->m_pItemElem->m_bCharged;
					dwKeepTime				=	pMail->m_pItemElem->m_dwKeepTime;

					if( pMail->m_pItemElem->m_pPet != NULL )
					{
						bIsPet					=	TRUE;
						nPetKind				=	pMail->m_pItemElem->m_pPet->GetKind();
						nPetLevel				=	pMail->m_pItemElem->m_pPet->GetLevel();
						dwPetExp				=	pMail->m_pItemElem->m_pPet->GetExp();
						dwPetEnergy				=	pMail->m_pItemElem->m_pPet->GetEnergy();
						dwPetLife				=	pMail->m_pItemElem->m_pPet->GetLife();
						nPetAvailLevel_D		=	pMail->m_pItemElem->m_pPet->GetAvailLevel( PL_D );
						nPetAvailLevel_C		=	pMail->m_pItemElem->m_pPet->GetAvailLevel( PL_C );
						nPetAvailLevel_B		=	pMail->m_pItemElem->m_pPet->GetAvailLevel( PL_B );
						nPetAvailLevel_A		=	pMail->m_pItemElem->m_pPet->GetAvailLevel( PL_A );
						nPetAvailLevel_S		=	pMail->m_pItemElem->m_pPet->GetAvailLevel( PL_S );
						FLStrcpy( szPetName, _countof( szPetName ), pMail->m_pItemElem->m_pPet->GetName() );
					}

					if( pMail->m_pItemElem->GetProp()->IsUltimate() == TRUE 
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
						|| pMail->m_pItemElem->GetProp()->IsCostumeEnhanceParts() == TRUE 
#endif						
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
						|| pMail->m_pItemElem->GetProp()->IsBarunaWeapon() == TRUE
#endif
						)
					{
						nPiercingSize			=	pMail->m_pItemElem->GetUltimatePiercingSize();
						dwPiercedItemId1		=	pMail->m_pItemElem->GetUltimatePiercingItemID( 0 );
						dwPiercedItemId2		=	pMail->m_pItemElem->GetUltimatePiercingItemID( 1 );
						dwPiercedItemId3		=	pMail->m_pItemElem->GetUltimatePiercingItemID( 2 );
						dwPiercedItemId4		=	pMail->m_pItemElem->GetUltimatePiercingItemID( 3 );
						dwPiercedItemId5		=	pMail->m_pItemElem->GetUltimatePiercingItemID( 4 );
						dwPiercedItemId6		=	pMail->m_pItemElem->GetGeneralPiercingItemID( 0 );
						dwPiercedItemId7		=	pMail->m_pItemElem->GetGeneralPiercingItemID( 1 );
						dwPiercedItemId8		=	pMail->m_pItemElem->GetGeneralPiercingItemID( 2 );
						dwPiercedItemId9		=	pMail->m_pItemElem->GetGeneralPiercingItemID( 3 );
						dwPiercedItemId10		=	pMail->m_pItemElem->GetGeneralPiercingItemID( 4 );
						dwPiercedItemId11		=	pMail->m_pItemElem->GetGeneralPiercingItemID( 5 );
						dwPiercedItemId12		=	pMail->m_pItemElem->GetGeneralPiercingItemID( 6 );
						dwPiercedItemId13		=	pMail->m_pItemElem->GetGeneralPiercingItemID( 7 );
						dwPiercedItemId14		=	pMail->m_pItemElem->GetGeneralPiercingItemID( 8 );
						dwPiercedItemId15		=	pMail->m_pItemElem->GetGeneralPiercingItemID( 9 );
						nAddPiercingSize		=	pMail->m_pItemElem->GetGeneralPiercingSize();
					}
					else
					{
						nPiercingSize			=	pMail->m_pItemElem->GetGeneralPiercingSize();
						dwPiercedItemId1		=	pMail->m_pItemElem->GetGeneralPiercingItemID( 0 );
						dwPiercedItemId2		=	pMail->m_pItemElem->GetGeneralPiercingItemID( 1 );
						dwPiercedItemId3		=	pMail->m_pItemElem->GetGeneralPiercingItemID( 2 );
						dwPiercedItemId4		=	pMail->m_pItemElem->GetGeneralPiercingItemID( 3 );
						dwPiercedItemId5		=	pMail->m_pItemElem->GetGeneralPiercingItemID( 4 );
						dwPiercedItemId6		=	pMail->m_pItemElem->GetGeneralPiercingItemID( 5 );
						dwPiercedItemId7		=	pMail->m_pItemElem->GetGeneralPiercingItemID( 6 );
						dwPiercedItemId8		=	pMail->m_pItemElem->GetGeneralPiercingItemID( 7 );
						dwPiercedItemId9		=	pMail->m_pItemElem->GetGeneralPiercingItemID( 8 );
						dwPiercedItemId10		=	pMail->m_pItemElem->GetGeneralPiercingItemID( 9 );
					}

					dwManagedSerialNumber		= static_cast<SERIALNUMBER>( 0 );
					dwCouplePlayerId			= pMail->m_pItemElem->GetCoupleId();
					nLevelDown					= pMail->m_pItemElem->GetLevelDown();
					dwRandomOptionOriginId		= pMail->m_pItemElem->GetRandomOptionOriginID();
					dwRandomOptionExtSize		= pMail->m_pItemElem->GetRandomOptionExtensionSize();
					dwRandomOptionExtFlag		= pMail->m_pItemElem->GetRandomOptionExtensionFlag();
					for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
					{
						arrRandomOptionExtensionDstID[ Nth ]		= pMail->m_pItemElem->GetRandomOptionExtensionDstID( Nth*2, Nth*2+1 );
						arrRandomOptionExtensionAdjValue[ Nth ]		= pMail->m_pItemElem->GetRandomOptionExtensionAdjValue( Nth*2, Nth*2+1 );
					}

					dwLooksChangeItemID					= pMail->m_pItemElem->GetLooksChangeItemID();
					bIsCombined							= pMail->m_pItemElem->IsSetCombinedOption();
					dwCombinedAddDamage					= pMail->m_pItemElem->GetCombinedAddDamage();
					dwCombinedRandomOptionOriginID		= pMail->m_pItemElem->GetCombinedRandomOptionOriginID();
					nCombinedGeneralPiercingSize		= pMail->m_pItemElem->GetCombinedGeneralPiercingSize();
					for( size_t Nth = 0; Nth < MAX_PIERCING; ++Nth )
					{
						arrCombinedGeneralPiercingID[ Nth ]	= pMail->m_pItemElem->GetCombinedGeneralPiercingItemID( Nth );
					}
					nCombinedRandomOptionExtensionSize	= pMail->m_pItemElem->GetCombinedRandomOptionExtensionSize();
					for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
					{
						arrCombinedRandomOptionExtensionDstID[ Nth ]		= pMail->m_pItemElem->GetCombinedRandomOptionExtensionDstID( Nth*2, Nth*2+1 );
						arrCombinedRandomOptionExtensionAdjValue[ Nth ]		= pMail->m_pItemElem->GetCombinedRandomOptionExtensionAdjValue( Nth*2, Nth*2+1 );
					}
				}
			}
		}

	} T_MAIL_STR_INFO, *PT_MAIL_STR_INFO;


	typedef struct _T_GUILD_BANK_STR_INFO
	{
		TCHAR	szGu[3];
		DWORD	dwGuildId;
		DWORD	dwServerIndex;
		DWORD	dwGuildGold;
		TCHAR	szIndex[216];
		TCHAR	szObjIndex[216];
		TCHAR	szGuildBank[8000];
		DWORD	dwPlayerId;
		TCHAR	szExtGuildBank[2000];
		TCHAR	szGuildBankPiercing[8000];
		TCHAR	szGuildBankPet[4200];

	} T_GUILD_BANK_STR_INFO, *PT_GUILD_BANK_STR_INFO;

	// END110119
	//////////////////////////////////////////////////////////////////////////


private:
	struct __GUILDCOMBATJOIN
	{
		DWORD dwPenya;
		BOOL  bRequest;
		u_long uGuildId;
	};
	struct __SendItemContents
	{
		char OneItem[MAX_PATH];
		int nNo, Item_Count;
		__int64 m_dwKeepTime;
		FLItemElem itemElem;
	};
	struct __GCRESULTVALUEGUILD
	{
		int	nCombatID;			// 길드대전 아이디
		u_long uidGuild;		// 길드 아이디
		__int64 nReturnCombatFee;	// 돌려받을 참여금
		__int64 nReward;				// 보상금
	};
	struct __GCRESULTVALUEPLAYER
	{
		int nCombatID;			// 길드대전 아이디
		u_long uidGuild;		// 길드 아이디
		u_long uidPlayer;		// 플레이어 아이디
		__int64 nReward;			// 보상금
	};
	struct __GCPLAYERPOINT
	{
		u_long	uidPlayer;
		int		nJob;
		int		nPoint;
		
		__GCPLAYERPOINT()
		{
			uidPlayer = nJob = nPoint = 0;
		}
	};
	
	struct __MAIL_REALTIME
	{
		int nMail_Before;
		int nMail_After;
		SERIALNUMBER m_iSerialNumber;
		int m_nHitPoint;
	};

public:
	SET_STRING	m_RemoveItem_List;
	INT2STRING		m_int2StrItemUpdate;

	CAccountCacheMgr m_AccountCacheMgr;
	CMclCritSec		m_AddRemoveLock;
	ULONG2STRING	m_2PartyNamePtr;

	//################# neodeath : 뭐에 쓰는 물건 ? 사용처가 없다.
	CMclCritSec		m_joinLock;
	char			m_aszJoin[MAX_JOIN_SIZE][MAX_PLAYER];
	int				m_nJoin;
	//################# neodeath : 뭐에 쓰는 물건 ? 사용처가 없다.


	CMclCritSec		m_csMapSavePlayer;
	std::map<u_long, int>	m_mapSavePlayer;		// m_idPlayer, nSlot
	HANDLE			m_hIOCPGet;
	HANDLE			m_hIOCPPut;

#ifdef MODIFY_INVEN_ITEM_DISSOLUTION_BUG
#else
	HANDLE			m_hIOCPUpdate;
#endif

	HANDLE			m_hIOCPGuild;			// 길드 업뎃용 
#ifdef __S1108_BACK_END_SYSTEM
	HANDLE			m_hWorker;
	HANDLE			m_hCloseWorker;
#endif // __S1108_BACK_END_SYSTEM
	HANDLE			m_hItemUpdateWorker;
	HANDLE			m_hItemUpdateCloseWorker;
	int				m_nItemUpdate;

	DB_OVERLAPPED_PLUS*		AllocRequest( void );
	void	FreeRequest( DB_OVERLAPPED_PLUS* pOverlappedPlus );
	void	MakeRequest( DB_OVERLAPPED_PLUS* pOverlappedPlus,  LPBYTE lpBuf, u_long uBufSize );

	FLMemPooler<DB_OVERLAPPED_PLUS>*	m_pDbIOData;

	std::vector<__GCRESULTVALUEGUILD>			m_GCResultValueGuild;		// 길드대전 결과값
	std::vector<__GCRESULTVALUEPLAYER>			m_GCResultValuePlayer;		// 길드대전 결과값
	std::vector<__GCPLAYERPOINT> m_vecGCPlayerPoint;

	std::vector<__GUILDCOMBATJOIN>	m_vecGuildCombat;	
	int		m_nGuildCombatIndex;
	u_long	m_uWinGuildId;
	u_long	m_uBestPlayer;
	int		m_nWinGuildCount;
//	CMclCritSec		m_csGuildCombat;

	char DB_ADMIN_PASS_LOG[256];
	char DB_ADMIN_PASS_CHARACTER01[256];
	char DB_ADMIN_PASS_BACKSYSTEM[256];
	char DB_ADMIN_PASS_ITEMUPDATE[256]; 

public:
	static CDbManager & GetInstance();
	~CDbManager();


	//////////////////////////////////////////////////////////////////////////
	// BEGIN110119

	static bool	DBQryCharacter( CQuery* pQuery, const PT_CHARACTER_STR_INFO ptCharacter );

	static bool	DbQryMail( CQuery* pQuery, const PT_MAIL_STR_INFO ptMail );

	static void	MakeQueryBindParameter( TCHAR* pSql, size_t cchSql, TCHAR* pProcedure, DWORD dwParameterNum );

	// END110119
	//////////////////////////////////////////////////////////////////////////

//	void	SavePlayer( CQuery *qry, CQuery* pQueryLog, CMover* pMover, char* szQuery, size_t cchQuery );
	void	SavePlayTime( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );	

	static void	SaveHonor( CQuery *qry, u_long uidPlayer, int * aHonor, char* szQuery, size_t cchQuery );

#ifdef __SKILL_0205
	static void	SaveSkill( CQuery *qry, u_long uidPlayer, LPSKILL aJobSkill, LPBYTE abUpdateSkill, char* szQuery, size_t cchQuery );
#else	// __SKILL_0205
	static void	SaveSkill( CQuery *qry, u_long uidPlayer, LPSKILL aJobSkill, char* szQuery, size_t cchQuery );
#endif	// __SKILL_0205

	static void	SaveJobLv( CMover* pMover, char* szJobLv, size_t cchJobLv );
	static void	SaveQuest( CMover* pMover, char* szQuestCnt, size_t cchQuestCnt, char* szm_aCompleteQuest, size_t cchCompleteQuest, char* szCheckedQuest, size_t cchCheckedQuest );

//	void	SaveInventory( CMover* pMover, PItemContainerStruct pItemContainerStruct );
//	void	SaveBank( CMover* pMover, CItemContainer<FLItemElem>* pPlayerBank, PItemContainerStruct pItemContainerStruct );
//	void	SaveGuildBank( CItemContainer<FLItemElem>*  pGuildBank, PItemContainerStruct pItemContainerStruct );

	static void	SaveEquipment( CMover* pMover, char* szEquipmen, size_t cchEquipmen );
	static void	SaveTaskBar( CMover* pMover, char* szAppletTaskBar, size_t cchAppletTaskBar, char* szItemTaskBar, size_t cchItemTaskBar, char* szSkillTaskBar, size_t cchSkillTaskBar );
	static void	SaveSMCode( CMover* pMover, char* szSMTime, size_t cchSMTime );
	static void	SaveSkillInfluence( CMover* pMover, char* szSkillInfluence, size_t cchSkillInfluence );

	static void	MakeQueryPocket( char* szQuery, size_t cchQuery, const PocketParam & p );

	static void	DBQryAddBankSave( char* szSql, size_t cchSql, const ADDBANK_QUERYINFO & info );
//	void	SaveOneItem( FLItemElem* pItemElem, PItemStruct pItemStruct );
	void	SendPlayerList( CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	static void	SendJoin( CMover* pMover, LPDB_OVERLAPPED_PLUS lpDBOP, DWORD dwAuthKey, 
						  DWORD dwBillingClass, LPCTSTR szPartyName, LPCTSTR szBankPass, 
						  DWORD dwPlayTime, DWORD dwFlyTime, int nMaximumLevel, int nTags, TAG_ENTRY* tags,
						  LPCTSTR szTimeGuild, const CTime & tmBirthDate );
	void	Join( CQuery* qry, CQuery* qry1, CQuery* qrylog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );		//@@@@@@@@@ 안쓸꺼여
	static void	SelectTeleportMapPointList( CQuery* pQuery, DWORD dwPlayerID, LPDB_OVERLAPPED_PLUS pDbOverlappedPlus );

	void	CreatePlayer( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	RemovePlayer( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	ChangeBankPass( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	Gamema_Chat( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

//	u_long	GetPlayerID( LPCSTR lpszPlayer );

	BOOL	LoadPlayerData( void );

	BOOL	GetPartyName( void );
	void	SerializeGuildCombat( CAr & ar );
	void	SerializeResultValue( CAr & ar );
	void	SerializePlayerPoint( CAr & ar );
	BOOL	OpenGuildCombat( void );
	BOOL	LoadPost( void );
	void	GetItemFromMail( CQuery* pQuery, FLItemElem* pItemElem );
	void	RemoveMail(std::list<CMail*> & lspMail );
	CQuery*	m_apQuery[MAX_QUERY_RESERVED];
	BOOL	QueryRemoveGuildBankTbl( void );
	void	DeleteRemoveGuildBankTbl( CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	call_uspLoggingQuest( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
	BOOL	QueryGetMailRealTime( CQuery* pQuery );

	void	LoadGC1to1TenderGuild( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, DPID dpid );
	void	GC1to1State( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GC1to1Tender( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GC1to1LineUp( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GC1to1WarPerson( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GC1to1WarGuild( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	void	GuildBankLogView( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	void	SealChar( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	SealCharConm( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	SealCharGet( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	SealCharSet( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	void	CalluspPetLog( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );

	static BOOL	GetMyPartyName( CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, u_long uidPlayer, char * szPartyName, size_t cchPartyName );

	CQuery*		CreateCharQuery( void );
	BOOL	ConvertPocket( MDC & mConv );

	void	CreateGuild( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	DestroyGuild( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	AddGuildMember( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	RemoveGuildMember( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	OpenGuild( void );
	void	UpdateGuildMemberLv( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildClass( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildNickName( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildMaster( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	void	UpdateGuildLogo( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildContribution( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildNotice( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildAuthority( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildPenya( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildRealPay( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildSetName( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildBankUpdate( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	OpenQueryGuildBank( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	OpenQueryGuildVote( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	
	void	AddGuildVote( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	RemoveGuildVote( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	CloseGuildVote( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	CastGuildVote( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	void	AcptWar( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	WarEnd( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	Surrender( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	WarDead( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	WarMasterAbsent( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );


	void	UpdateGuildRanking( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildRankingDB( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	void	AddPartyName( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

//	BOOL	call_uspLoggingTrade( CQuery* pQuery, const T_TRADE_ITEM_LOG & kTradeItemLog );

	void	call_uspLoggingTrade( CQuery & kQuery, const int nTradeID, const DWORD dwWorldID );

	void	call_uspLoggingTradePlayer( CQuery & kQuery, const int nTradeID, const T_LOG_TRADE & kLogTrade );

	void	call_uspLoggingTradeItem( CQuery & kQuery, const int nTradeID, const u_long idPlayerID, const T_LOG_ITEM & kLogItem );

	LONG	m_cbTrade;
	int		GetTradeNo( void );
	void	call_uspXXXMultiServer( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );

	BOOL	CreateDbWorkers( void );
	void	CloseDbWorkers( void );
	void	GetThread( void );
	void	PutThread( void );
	void	SPThread( void );
	void	UpdateThread( void );
	void	GuildThread( void );
#ifdef __S1108_BACK_END_SYSTEM
	void	BackSystem( void );
#endif // __S1108_BACK_END_SYSTEM
	void	ItemUpdateThread( void );
	void	ChangeItemUpdate( CQuery* pQuery );
	void	ChangeSkillUpdate( CQuery* pQuery );
	void	ChangeMonsterUpdate( CQuery* pQuery );
	void	ChangeQuestUpdate( CQuery* pQuery );
	void	ChangeJobNameUpdate( CQuery* pQuery );
	void	ChangeSexUpdate( CQuery* pQuery );
	//void	GetBank( BOOL bCache, ACCOUNT_CACHE* AccountCache, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, int nMySlot, char* pszBankPW );
	static BOOL	GetBank( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, int nSlot );
	static BOOL	GetInventory( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	static void	LoadPiercingInfo( FLItemElem & itemElem, char* szPirecingInven, int* pLocation );
	void	GetPiercingInfoFromMail( CQuery* pQuery, FLItemElem* pItemElem );

	static BOOL	GetPocket( CMover* pMover, CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
//	void	SavePocket( CMover* pMover, PPocketStruct pPocketStruct );

	static BOOL	GetEquipment( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	static BOOL	GetTaskBar( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	static void	GetBaseCharacter( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	static BOOL	GetSkill( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	static BOOL	GetQuest( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	static BOOL	GetSMMode( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	static BOOL	GetSKillInfluence( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	static void	GetHonor( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );


	static	UINT	_GetThread( LPVOID pParam );
	static	UINT	_PutThread( LPVOID pParam );
	static	UINT	_SPThread( LPVOID pParam );
	static	UINT	_UpdateThread( LPVOID pParam );
	static	UINT	_GuildThread( LPVOID pParam );
#ifdef __S1108_BACK_END_SYSTEM
	static  UINT	_BackSystem( LPVOID pParam );
#endif // __S1108_BACK_END_SYSTEM
	static	UINT	_ItemUpdateThread( LPVOID pParam );

	void	Clear( void );

	static void	DBQryLog( char* qryLog, size_t cchLog, char* Gu, u_long idPlayer, int nserverindex, EXPINTEGER nExp1 = 0, int nLevel = 0,
						  int nJob = 0, int JobLv = 0, int FlightLv = 0, int nStr = 0, int nDex = 0,
						  int nInt = 0, int nSta = 0, int nRemainGP = 0, int nRemainLP = 0, char szState = 'A',
						  int WorldID = 0, char* szkilled_szName = '\0', float vPos_x = 0.0f, float vPos_y = 0.0f, float vPos_z = 0.0f,
						  int nAttackPower = 0, int nMax_HP = 0, char *uGetidPlayer = '\0', DWORD dwGold = 0, int nRemainGold = 0,
						  int nItem_UniqueNo = 0, char *szItem_Name = '\0', int Item_durability = 0, int Item_count = 0, int ItemAddLv = 0,
						  char* szStart_Time = '\0', int TotalPlayerTime = 0, char *szRemoteIP = '\0', int nQuest_Index = 0, u_long uKill_idPlayer = 0 );	

	void	DBQryGuild( char* szSql, size_t cchSql, const char* szType, u_long idPlayer = 0, u_long idGuild = 0, const char* szGuild = "" );
	void	DBQryGuild( char* szSql, size_t cchSql, const GUILD_QUERYINFO & info);
	void	DBQryVote( char* szSql, size_t cchSql, const VOTE_QUERYINFO& info );
	void	DBQryGuildLog( char* szSql, size_t cchSql, const GUILDLOG_QUERYINFO& info );
	//////////////////////////////////////////////////////////////////////////
	void	MakeLogGuildItem( CItemContainer<FLItemElem>* pGuildBank, TCHAR* pszItemLog, size_t cchItemLog );
	//////////////////////////////////////////////////////////////////////////
	void	DBQryWar( char* szSql, size_t cchSql, const WAR_QUERYINFO & info );
//	static void	DBQryNewItemLog( char* qryLog, size_t cchLog, const LogItemInfo & info );
	static BOOL	DBQryNewItemLog( CQuery & kQuery, const LogItemInfo & info );
	static void	WriteLogItem( const LogItemInfo & kLogItemInfo );

	static int		GetOneItem( FLItemElem* pItemElem, char* pstrItem, int *pLocation );
	void	GetOneSkill( LPSKILL pSkill, char* pstrSkill, int *pLocation );
	static void	GetOneQuest( LPQUEST pQuest, char* pstrQuest, int *pLocation );

	static BOOL	GetSerialNumberStatus( char* pstrInven, int* pLocation );
	static void	SaveSerialNumberStatus( char* pstrInven, size_t cchInven, BOOL bNewSerialNumber );

	BOOL	ConvItem050615( void );
	void	ConvChangeItem050615( CMover* pMover );
	void	ConvChangeGuild050615( CMover* pMover );
	void	ConvChangeGuild050615( CItemContainer< FLItemElem  >*  GuildBank );
	BOOL	RemoveItemInvenBank( CMover* pMover, int* nCountItem0, int* nCountItem1 );
	BOOL	RemoveItemGuildBank( int nGuildId, CItemContainer< FLItemElem  >*  GuildBank, int* nCountItem0, int* nCountItem1 );
	DWORD	GetRemoveItemPanya( int nItem0, int nItem1 );

	void	InitConvItemDialog( void );
	void	UpdateConvItemDialog( char* pTitle, char* pString, char* pPosStirng, int nPos );
	BOOL	ConvItemConnectDB( CQuery* pQueryChar, CQuery* pQuerySave );
	DWORD	GetMaxInventory( char* pszSQL, size_t cchSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	DWORD	GetMaxGuildBank( char* pszSQL, size_t cchSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	CreateInvenBankView( char* pszSQL, size_t cchSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	CreateGuildBankView( char* pszSQL, size_t cchSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	DeleteInvenBankView( char* pszSQL, size_t cchSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	DeleteGuildBankView( char* pszSQL, size_t cchSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	GetInventoryBankView( char* pszSQL, size_t cchSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	GetGuildBankView( char* pszSQL, size_t cchSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	GetUserInventoryBank( CMover* pMover, CQuery* pQueryChar );	
	BOOL	InventoryBankConv( char* pszSQL, size_t cchSQL, CMover* pMover, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	GuildBankConv( char* pszSQL, size_t cchSQL, int nGuildId, CItemContainer< FLItemElem >* GuildBank, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	RemoveGuildBankList( char* pszSQL, int nGuildId, CItemContainer< FLItemElem  >*  GuildBank, int* nCountItem0, int* nCountItem1 );
	BOOL 	RemoveInventoryBankList( char* pszSQL, CMover* pMover, int* nCountItem0, int* nCountItem1 );

	BOOL	OpenWanted( CAr& ar );

	static void	MakeQuery_CooperativeContributionsRewardItemMail( const u_long idPlayer, const DWORD dwContributionID, const FLTm tExpire, char * o_szSql, const size_t cchSql, const CMail * pMail, const u_long idReceiver );

	//////////////////////////////////////////////////////////////////////////
	// BEGIN110119

// 	static void	MakeQueryAddMail( char * o_szSql, const size_t cchSql, const CMail * pMail, const u_long idReceiver );
// 	static void	MakeQueryMailParam( char* o_szSql, const size_t cchSql, const CMail * pMail, const u_long idReceiver );
	
	// END110119
	//////////////////////////////////////////////////////////////////////////

	static void	GetDBFormatStr( char* szDst, size_t cchDst, const char* szSrc );

	bool		GetMover_FromAccountCacheMgrA( CMover & io_mover, const BOOL brefresh, const char * szAccount, const BYTE byProgramDataSlot, const DWORD dwPlayTime );
	void		GetMover_FromAccountCacheMgrB( CMover & o_mover, const char * szAccount, const BYTE byProgramDataSlot );
	

private:
	CDbManager();


	//////////////////////////////////////////////////////////////////////////
	// BEGIN110119
	static BOOL				Exec_ItemLogQuery( CQuery & kLogQuery, const LogItemInfo & info );

	static BOOL	RemovePenya( CQuery *pQry, CMover* pMover, int nItem_nCount, int nNo, char chState );
	
	static SERIALNUMBER	RemoveItem( CQuery *pQry, int nNo, CMover* pMover, const char* szItemName
		, int nItemCount, int nAbilityOption, int nItemResist, int nResistAbilityOption, char chState );

	static SERIALNUMBER	SendItem( CQuery *pQry, CMover* pMover, __SendItemContents& SendItemContents );

	static BOOL	SendPenya( CQuery *pQry, CMover* pMover, int nPenya, int nNo );

	static BOOL	SendItemDeleteQuery( CQuery *pQry, int nNo );

	static BOOL	RemoveItemDeleteQuery( CQuery *pQry, int nNo );

	static void	WriteLogCharacter( const PT_CHARACTER_STR_INFO ptCharacter );

	static void	WriteLogMail( const PT_MAIL_STR_INFO ptMail );

	// END110119
	//////////////////////////////////////////////////////////////////////////

public:
	static void	WriteFileLogCharacter( const TCHAR* _pFileName, const TCHAR* _pContext, const PT_CHARACTER_STR_INFO pCharacterStrInfo );

	static void	LoadMessenger( CMover* pMover, CQuery* pQuery );

private:
	void	AddMessenger( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
	void	DeleteMessenger( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
	void	UpdateMessenger( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );

//	BOOL	IsAbnormalPlayerData( CMover* pMover );
//	BOOL	NormalizePlayerData( CMover* pMover );
public:
	static int		SelectTag( CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, u_long uidPlayer, TAG_ENTRY* tags );

private:
	static void	WriteTag( CAr &ar, int count, const TAG_ENTRY* tags );
	void	InsertTag( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GetStrTime( CTime *time, const char *strbuf );
	static BOOL	VerifyString( const char* lpString, const TCHAR* lpFileName, int nLine, const TCHAR* pFunctionName, const char* lpName, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = NULL );

	void	LogPlayConnect(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogPlayDeath(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogLevelUp(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogServerDeath(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogUniqueItem(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogQuest(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogPkPvp( CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	LogSchool( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	LogSkillPoint( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	LogTradeItem(CQuery *pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogItem(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogConcurrentUserNumber( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	void	LogInstanceDungeon( CQuery *pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	void	LogError( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	void	LogGuildFurniture( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	void	SetPlayerName( CQuery* pQueryChar, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	SnoopGuild( CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	void	PutExpLog( CQuery* pQueryLog, CMover* pMover, char ch );
	void	QueryGuildQuest( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	InsertGuildQuest( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus ); 
	void	UpdateGuildQuest( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

public:
	static BOOL	SendItemtoCharacter( CMover* pMover, CQuery *qry, CQuery *qry1, CQuery * qrylog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	static BOOL	RemoveItemtoCharacter( CMover* pMover, CQuery *qry, CQuery *qry1, CQuery *qrylog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

private:
	static SERIALNUMBER	RemoveItemBank( PT_ITEM_SPEC pItemProp, CMover* pMover, int nAbilityOption, int nItemResist, int nResistAbilityOption, int nItemCount );
	static SERIALNUMBER	RemoveItemPocket( PT_ITEM_SPEC pItemProp, CMover* pMover, int nAbilityOption, int nItemResist, int nResistAbilityOption, int nItemCount );

	void	SchoolReport( CQuery* pQuery, LPDB_OVERLAPPED_PLUS po );

	static BOOL	GetSendItem( CQuery *pQry, __SendItemContents * pSendItemContents );

	static SERIALNUMBER	RemoveItemInventory( PT_ITEM_SPEC pItemProp, CMover* pMover, int nAbilityOption, int nItemResist, int nResistAbilityOption, int nItemCount );
	static BOOL	GetRemoveItem( CQuery *pQry, int &nNo, char* pOneItem, size_t nOneItemCount, int &nItem_nCount, int &nAbilityOption, int &nItemResist, int &nResistAbilityOption, char &chState );	

	static BOOL	ItemLogQuery( CQuery *pQryLog, char *szQueryState, const LogItemInfo& info, u_long uIdPlayer, int nNo, char *szItemName );

	void	RecommendEve( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	void	LogGetHonorTime(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);

	CQuery	m_qryPostProc;
	void	AddMail( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
public:
	static void					PostMail( CQuery & kQuery, const u_long uReceiver, const u_long uSender, FLItemElem & kItemElem, const int nGold, const char * szTitle, const char * szText );
private:
	void	RemoveMail( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
	void	RemoveMailItem( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
	void	RemoveMailGold( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
	void	ReadMail( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );

	void	LogExpBox( CQuery *pQuery, LPDB_OVERLAPPED_PLUS pov );

#ifdef __S1108_BACK_END_SYSTEM
	void	GetGemeSettingtime( CQuery* pQuery, int nChat = 0 );
	BOOL	GetBaseGameSetting( CQuery* pQuery, char* lpstrTime = '\0' );
	void	GetMonsterRespawnSetting( CQuery* pQuery, int& nRemoveMaxRespawn, int aRemoveRespawn[] = NULL, char* lpstrTime = '\0' );
	void	GetMonsterPropSetting( CQuery* pQuery, char* lpstrTime = '\0' );
	BOOL	AddMonsterRespawn( MONSTER_RESPAWN BufMonsterRespawn );
	BOOL	RemoveMonsterRespawn( int nIndex, int &nMaxRemove, int aRemoveRespawn[] );
	BOOL	AddMonsterProp( MONSTER_PROP BufMonsterProp );
	BOOL	RemoveMonsterProp( MONSTER_PROP BufMonsterProp );
	BOOL	GetGMChat( CQuery* pQuery, char * lpstrTime, int& nChatCount );
#endif // __S1108_BACK_END_SYSTEM

	BOOL	OnWantedQuery( CQuery* pQuery, WANTED_QUERYINFO& info );
	void	SerializeWanted( CQuery* pQuery, CAr& ar );
	void	WantedOperation( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
public:
	static BOOL	CanJoin( LPCTSTR szAccount, ACCOUNT_CACHE* pAccountCache );

private:
	void	StartGuildCombat( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	InGuildCombat( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	OutGuildCombat( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	ResultGuildCombat( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GetPenyaGuildGC( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GetPenyaPlayerGC( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	ContinueGC( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

private:
	static void	GetStrFromDBFormat( char* szDst, const char* szSrc, int& n );
	static void	SetStrDBFormat( char* szDst, size_t cchDst, const char* szSrc );

	void	SetDBFormatStr( char* szDst, size_t cchDst, const char* szSrc );

	void	LoginProtectCert( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	//////////////////////////////////////////////////////////////////////////
	// mirchang_100723 give coupon item event

	void	GetCouponNumberItem( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	// mirchang_100723 give coupon item event
	//////////////////////////////////////////////////////////////////////////

	// 협동 기부
	void	ExecCooperativeContributions_Contribute( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	ExecCooperativeContributions_GetContributed( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	ExecCooperativeContributions_GetRanking( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	ExecCooperativeContributions_TryRewardItem( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	ExecCooperativeContributions_TryWriteItemRewardInfo( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	// 협동 기부
	void	ExecRecord_DungeonBestCompleteTime( CQuery* pQuery, CQuery * pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	ExecRecord_GetDungeonRanking( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	CMclCritSec	m_csSerialNumber;
	void	SelectSerialNumber( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	//////////////////////////////////////////////////////////////////////////
	void	TeleportMapAddPoint( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS pDbOverlappedPlus );
	void	TeleportMapChgPointName( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS pDbOverlappedPlus );
	void	TeleportMapDelPoint( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS pDbOverlappedPlus );
	void	LogTeleportMap( CQuery* pLogQuery, DWORD dwPlayerID, PT_TELEPORTMAP_POINT pUserPoint, char chOP );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// BEGIN Character Inventory
public:
	static bool	LoadInventory( CMover* pMover, CQuery* pQuery );
	static void	SaveInventory( CMover* pMover, CQuery* pQuery );

private:
	bool	LoadEquipInfo( CQuery* pQuery, DWORD dwPlayerId, DWORD dwMaxPartsSize, std::vector< EQUIP_INFO > & vecEquipInfo );
	static bool	UpdateInventory( CMover* pMover, CQuery* pQuery );
	static void	InsertInventory( CMover* pMover, CQuery* pQuery );
	static void	DeleteInventory( CMover* pMover, CQuery* pQuery );
	// END Character Inventory
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// BEGIN Character Bank
public:
	static bool	LoadCharacterBank( CMover* pMover, CQuery* pQuery, u_long uPlayerId, int nDBDataSlot );
	static void	SaveCharacterBank( CMover* pMover, CQuery* pQuery, u_long uPlayerId, int nDBDataSlot );
private:
	static bool	UpdateCharacterBank( u_long uPlayerId, CQuery* pQuery, int nDBDataSlot );
	static bool	InsertCharacterBank( CMover* pMover, CQuery* pQuery, u_long uPlayerId, int nDBDataSlot );
	static void	DeleteCharacterBank( u_long uPlayerId, CQuery* pQuery, int nDBDataSlot );
	// END Character Bank
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// BEGIN Character Pocket
public:
	static bool	LoadCharacterPocket( CMover* pMover, CQuery* pQuery, int nPocket );
public:
	static void	SaveCharacterPocket( CMover* pMover, CQuery* pQuery, int nPocket );
private:
	static bool	UpdateCharacterPocket( CMover* pMover, CQuery* pQuery, int nPocket );
	static void	InsertCharacterPocket( CMover* pMover, CQuery* pQuery, int nPocket );
	static void	DeleteCharacterPocket( CMover* pMover, CQuery* pQuery, int nPocket );
	// END Character Pocket
	//////////////////////////////////////////////////////////////////////////


	//// 거래 중개
	//
	//void	ConsignmentSale_GetRegistedCount( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	//void	ConsignmentSale_Regist( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus ); 
	//void	ConsignmentSale_CancelRegisted( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	//void	ConsignmentSale_RegistedInfoList( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	//void	ConsignmentSale_GetPrice( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	//void	ConsignmentSale_ClearReserveInfo( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	//void	ConsignmentSale_BuyItem( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	//void	ConsignmentSale_CollectSaleGold( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	//void	ConsignmentSale_Search( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	//void	ConsignmentSale_SaleGoldInfoList( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );


	//////////////////////////////////////////////////////////////////////////
	void	CharacterServerTransform( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pDbOverlappedPlus );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// BEGIN Guild Bank
public:
	bool	LoadGuildBank( DWORD dwGuildId, CItemContainer<FLItemElem>* pGuildBank, CQuery* pQuery );
	bool	SaveGuildBank( DWORD dwGuildId, CItemContainer<FLItemElem>* pGuildBank, CQuery* pQuery );

public:
	static void	LoadOneSaleInfoHeader( CQuery & kQuery/*, const char * pszTableName*/, nsConsignmentSale::FLOneSaleInfo & o_kOneSaleInfo );
	static void LoadOneSaleInfo_ForServer( CQuery & kQuery/*, const char * pszTableName*/, FLDBConsignmentSale::FLSaleInfo & o_kSaleInfo );
	static void	LoadOneItemCommon( CQuery & kQuery/*, const char * pszTableName*/, const DWORD dwObjID, const DWORD dwCurrObjIndex, FLItemElem & o_kItemElem, int & o_nSerialNumberStatus );
	static bool	ModifyItemElem( const CMover & kMover, FLItemElem & kItemElem );
	static void	SaveOneItemCommon( CQuery & kQuery, T_ITEM_QUERY_INFO & kItem );


	bool	UpdateGuildBank( DWORD dwGuildId, CQuery* pQuery );
	void	InsertGuildBank( DWORD dwGuildId, CItemContainer<FLItemElem>* pGuildBank, CQuery* pQuery );
	void	DeleteGuildBank( DWORD dwGuildId, CQuery* pQuery );
	// END Guild Bank
	//////////////////////////////////////////////////////////////////////////

	static BOOL	GetSerialNumberStatus( int nSerialNumberStatus );
	static int		SaveSerialNumberStatus( BOOL bNewSerialNumber );

	static void		RefreshTaskBar( CMover* pMover );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	void	GuildBankInputItem( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GuildBankOutputItem( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
//	void	GuildBankInputGold( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GuildBankOutputGold( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	//////////////////////////////////////////////////////////////////////////
#ifdef PASSWORD_RESET_2ND
	int	CheckResetPassword( CQuery* pQuery, DWORD dwPlayerId );
#endif // PASSWORD_RESET_2ND

public:
	DWORD			m_dwGuildBankUseWorldKey;
	
	u_long			m_idPlayer;
	HANDLE			m_hThreadGet[MAX_GETTHREAD_SIZE];
	HANDLE			m_hThreadPut[MAX_PUTTHREAD_SIZE];
	HANDLE			m_hSPThread;
	HANDLE			m_hCloseSPThread;
#ifdef MODIFY_INVEN_ITEM_DISSOLUTION_BUG
#else
	HANDLE			m_hThreadUpdate[MAX_UPDATETHREAD_SIZE];
#endif
	HANDLE			m_hThreadGuild;
	HANDLE			m_hWait;
};

inline BOOL CDbManager::VerifyString( const char* lpString, const TCHAR* lpFileName, int nLine, const TCHAR* pFunctionName, const char* lpName, LPDB_OVERLAPPED_PLUS /*lpOverlapped*/ )
{
	#define	FLDBVERIFYSTRING_LOG	FLLOG_IMPL( FLLOG_ERROR, pFunctionName, lpFileName, nLine )

	int len		= strlen( lpString );
	if( len > 0 && lpString[len-1] == '$' )
		return TRUE;

	FLDBVERIFYSTRING_LOG( PROGRAM_NAME, _T( "%s" ), lpName );
	return FALSE;
}

extern CDbManager & g_DbManager;




void		PostOverlappedPlus( const int nQM, const HANDLE hIOCP, const DWORD dwNumberOfBytesTransferred, const ULONG_PTR dwCompletionKey
								   , const DPID dpid, const DPID dpidCache, const DPID dpidUser, const LPBYTE lpBuf, const u_long uBufSize );




#define	VERIFYSTRING( lpString, lpszPlayer )	\
	if( FALSE == VerifyString( lpString, _T( __FILE__ ), __LINE__, _T( __FUNCTION__ ), lpszPlayer, lpDbOverlappedPlus ) )		return;

#define	VERIFY_GUILD_STRING( lpString, lpszGuild )	\
	if( FALSE == VerifyString( lpString, _T( __FILE__ ), __LINE__, _T( __FUNCTION__ ), lpszGuild ) )		return;

// mirchang_100416 VERIFYSTRING use return value
#define	VERIFYSTRING_RETURN( lpString, lpszPlayer )	\
	if( FALSE == VerifyString( lpString, _T( __FILE__ ), __LINE__, _T( __FUNCTION__ ), lpszPlayer, lpDbOverlappedPlus ) )		return FALSE;

#define	VERIFYSTRING_BANK( lpString, lpszPlayer )	\
	if( FALSE == VerifyString( lpString, _T( __FILE__ ), __LINE__, _T( __FUNCTION__ ), lpszPlayer ) )		return FALSE;
// mirchang_100416




#endif	// _DB_MANAGER_H_