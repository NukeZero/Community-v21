#ifndef __FLYFFCOMMONDEFINE_H__
#define __FLYFFCOMMONDEFINE_H__


//////////////////////////////////////////////////////////////////////////
//	BEGIN100616	WARNINGCODE
enum	_CONNECT_LEVEL
{
	CL_NORMAL			= 0
	, CL_DEVELOPMENT

	, CL_MAXVALUE
};

#define	MAX_USERIP	16
//	END100616	WARNINGCODE
//////////////////////////////////////////////////////////////////////////

//_SUN_CHECKDATA_
enum _CLIENT_ERRORS
{
	CLERR_PENYA_OVERLIMIT_P	=	100,		// 개인 페냐 한계초과
	CLERR_PENYA_UNDERZERO,					// 페냐( < 0 )
	CLERR_OVERNUMBER,						// 갯수, 수량이 잘못된 경우
};

const DWORD  MAX_PEYNA_PERSON = 0x7fffffff;		
const short  MAX_0X7FFFF = 0x7fff;


namespace std 
{ 
	typedef					basic_istringstream<TCHAR, char_traits<TCHAR>, allocator<TCHAR> > itstringstream;
	typedef					basic_ostringstream<TCHAR, char_traits<TCHAR>, allocator<TCHAR> > otstringstream;
	typedef					basic_stringstream<TCHAR, char_traits<TCHAR>, allocator<TCHAR> > tstringstream;
	
}  

#ifdef _DEBUG			//디버그 모드에서만 작동하는 코드 
#define DEBUG_EXCUTE( exc )			do{		exc;		}while(0)
#define DEBUG_ASSERT( x )		do{ if( x == false ) RELEASE_BREAK; }while(0)
#define DEBUG_BREAK				RELEASE_BREAK
#else
#define DEBUG_EXCUTE( exc )			__noop
#define DEBUG_ASSERT( x )			__noop
#define DEBUG_BREAK					__noop
#endif

//#define RELEASE_BREAK			do{	__asm{ INT 3 };  }while(0)
#define RELEASE_BREAK			do{	*( ( int * )NULL ) = 0;  }while(0)
#define RELEASE_ASSERT( x )		do{ if( x == false ) RELEASE_BREAK; }while(0)

#define		_MAX_COUPON_NUMBER_LEN		14


#define		_MAX_ES_SCROLL_NUM			5	// 최대 사용 개수( 위 스크롤 사용 최대값이 증가할 시 같이 증가시켜 주세요..)



#define		_MAX_ENCHANT_LEVEL_COLLECTOR		5
#define		_MAX_ENCHANT_LEVEL_GENERAL			10
#define		_MAX_ENCHANT_LEVEL_PIERCING			10
#define		_MAX_ENCHANT_LEVEL_ACCESSORY		20
#define		_MAX_ENCHANT_LEVEL_ATTRUBUTE		20
#define		_MAX_ENCHANT_LEVEL_BARUNA			20

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
#define		_MAX_ENCHANT_LEVEL_COSTUME			10
#endif

#define		_MAX_ENCHANT_LEVEL					20	// 제련 맥스치가 수정될 경우 같이 수정해주세요..



#define		_MAX_SHOPPING_BASKET		12

#define		VENDOR_TYPE_PENYA			0
#define		VENDOR_TYPE_CHIP			1

#define		VENDOR_KIND_PC				0
#define		VENDOR_KIND_NPC				1



enum
{
	CONTRIBUTE_KIND_LEN = 20,
	MAX_NAME			= 64
};



//////////////////////////////////////////////////////////////////////////
#define MAX_RENDER_BUFF_COUNT		30
#define DEFAULT_RENDER_BUFF_VALUE	0
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#define SKILLPOINT_DIVIDE					20
#define SKILLPOINT_BASE						2
#define SKILLPOINT_LEGENDHERO_BASE			4
#define SKILLPOINT_LEGENDHERO_EXCEPTION		6
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#define BIND_CONDITION_NOTBIND				0	// 귀속 안됨
#define BIND_CONDITION_GET					1	// 획득 시 귀속
#define BIND_CONDITION_USE					2	// 사용(착용) 시 귀속
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
enum E_CONTRIBUTION_KIND
{
	CONTRIBUTION_ITEM			= 0,
	CONTRIBUTION_GOLD,
};

enum E_REWARD_TYPE
{
	REWARD_ITEM					= 0,
	//REWARD_GOLD,

	REWARD_EXP_RATE,
	REWARD_DROP_RATE,
	REWARD_SHOP_BUY_FACTOR,
	REWARD_SHOP_SELL_FACTOR,
	REWARD_DEFENCE_POWER,
	REWARD_ATTACK_POWER,
};

//////////////////////////////////////////////////////////////////////////
//윈도우메세지 박스에서 사용하는 버튼종류
#define MB_CANCEL           0x00000006L


enum INVENTORY_TYPE
{
	INVEN_TYPE_NONE = 0
	, INVEN_TYPE_GENERAL
	, INVEN_TYPE_PET
	, INVEN_TYPE_COSTUME
	, INVEN_TYPE_QUEST
	, INVEN_TYPE_MAX
};

const int MAX_G_NAME			= 48;

enum ITEM_CONTAINER_TYPE
{
	CONTAINER_TYPE_NONE = 0
	, CONTAINER_TYPE_INVENTORY
	, CONTAINER_TYPE_USER_BANK
	, CONTAINER_TYPE_POCKET
	, CONTAINER_TYPE_GUILD_BANK
	, CONTAINER_TYPE_SHOP
	, CONTAINER_TYPE_TRADE
};

enum ITEM_UPGRADE_TYPE
{
	IUTYPE_NONE							= 0x00
	, IUTYPE_ENCHANT_GENERAL			= 0x01
	, IUTYPE_ENCHANT_ATTRIBUTE			= 0x02
	, IUTYPE_ENCHANT_GEM				= 0x04
	, IUTYPE_PIERCING					= 0x08
	, IUTYPE_RANDOM_OPTION_ORIGIN		= 0x10
	, IUTYPE_RANDOM_OPTION_EXTENSION	= 0x20
	, IUTYPE_CHANGE_LOOKS				= 0x40
#ifdef BARUNA_ULTIMATE_UPDATE
	, IUTYPE_ULTIMATE_TRANS				= 0x80
#endif
};


const size_t	MAX_QUERY_SIZE		=	40960;

const DWORD		MAX_ITEM_PARAMETER	=	70;

const DWORD		MAX_ADD_ITEM_PARAMETER	= 26;


enum DB_CONNECT_TYPE
{
	LOGIN_DSN_NAME = 0
	, LOGIN_DB_ADMIN_ID
	, LOGIN_PASSWORD
	, CHARACTER_DSN_NAME
	, CHARACTER_DB_ADMIN_ID
	, CHARACTER_PASSWORD
	, LOG_DSN_NAME
	, LOG_DB_ADMIN_ID
	, LOG_PASSWORD
};


enum ITEM_MERGE_TYPE
{
	MERGE_NONE = 0
	, MERGE_RANDOMOPTION
	, MAX_MERGE_TYPE
};

const size_t	MAX_ITEM_MERGE_COUNT	=	5;

const int		MAX_CHARACTER_SLOT		=	3;
const int		MAX_CHARACTER_POCKET	=	3;


const DWORD		MAX_QUEST_KILL_NPC_COUNT	= 4;

const DWORD		MAX_TREASURE_ITEM_COUNT		= 3;

const DWORD		MAX_TREASURE_KEY_PROB		= 10000;

const size_t	MAX_SKILL_PARAM				= 2;
const size_t	MAX_ITEM_PARAM				= 6;

const size_t	MAX_LOG_SIZE				= 8192;

const size_t	MAX_MAGICALOPTION_SIZE		= 8;



const DWORD		ACTIONSLOT_DEFAULT_COOLTIME	= 20;

enum SKILL_QUEUE_STATE
{
	SKILL_QUEUE_NONE = -1
	, SKILL_QUEUE_USE = 0
};

enum BROADCAST_TYPE
{
	BROADCAST_TYPE_NONE = 0
	, BROADCAST_TYPE_NOTICE
	, BROADCAST_TYPE_PUTSTRING
};

enum GUILD_BANK_QUERY_ERROR
{
	E_NONE	= 0
	, E_EXIST_ALREADY_USE_WORLD
	, E_QUERY_FAIL
};

const DWORD		SELL_ITEM_TO_NPC_COST_DIVIDE		= 4;

// imParkth - Vis Content
const DWORD		MAX_VIS_CONDITION_COUNT				= 2;
const DWORD		MAX_VIS_GROUP_ITEM_COUNT			= 10;

const DWORD		MAX_SAFE_LEVEL_GENERAL_ENCHANT		= 2;
const DWORD		MIN_SIZE_RANDOMOPTION_RETRY			= 2;

const DWORD		MAX_MATERIAL_COMBINE_SIZE			= 2;

const float		decimalOneinHundred					= 0.01f;
const float		decimalOneinThousand				= 0.001f;
const float		decimalOneinTenThousand				= 0.0001f;

#endif	//__FLYFFCOMMONDEFINE_H__
