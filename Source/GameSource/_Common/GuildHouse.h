// GuildHouse.h: interface for the CGuildHouse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUILDHOUSE_H__7A401B90_A88E_4AB9_A5A3_CBFE71AB03F6__INCLUDED_)
#define AFX_GUILDHOUSE_H__7A401B90_A88E_4AB9_A5A3_CBFE71AB03F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum
{
	GUILDHOUSE_PCKTTYPE_LISTUP = 0,
	GUILDHOUSE_PCKTTYPE_LISTDROP,
	GUILDHOUSE_PCKTTYPE_SETUP,
	GUILDHOUSE_PCKTTYPE_REMOVE,
	GUILDHOUSE_PCKTTYPE_RESET,
	GUILDHOUSE_PCKTTYPE_REFRESH,
	GUILDHOUSE_PCKTTYPE_UPKEEP,
	GUILDHOUSE_PCKTTYPE_EXPIRED,
};

enum FURNITURE_LEVEL
{
	GH_FURNITURE_LEVEL_SMALL = 1
	, GH_FURNITURE_LEVEL_MIDDLE
	, GH_FURNITURE_LEVEL_LARGE
};

static const int	BUY_MIN_MEMBERSIZE		= 12;			// 길드하우스 구입 최소 길드원 수 
static const int	BUY_MIN_LEVEL			= 3;			// 길드하우스 구입 최소 길드 레벨
static const int	BUY_PENYA				= 100000000;	// 길드하우스 구입 페냐
static const int	MAX_LISTUP_NUM			= 200;			// 최대 ListUp 개수

static const int	UPKEEP_DAY_TIME			= 60 * 60 * 24;	// 1일 유지 시간
static const int	MAX_UPKEEP_DAY			= 35;			// 최대 유지 기간
static const int	UPKEEP_DAY_PENYA_SMALL	= 2400000;		// 소형길드하우스 1일 유지비
static const int	UPKEEP_DAY_PENYA_MIDDLE	= 4800000;		// 중형길드하우스 1일 유지비

static const int	MAX_REST_POINT			= 1000000;		// 휴식게이지의 최대값
static const int	REST_POINT_INC			= 10000;		// 휴식게이지의 Tick 당 증가값(로그인 상태)
static const int	REST_POINT_LOGOUT_INC	= 2500;			// 휴식게이지의 Tick 당 증가값(로그아웃 상태)
static const int	REST_POINT_TICK			= 60 * 4;		// 휴식게이지 증가 Tick(sec)


const DWORD			BUBBLEPOINT_BUFF_ID		= ITEM_INDEX( 24301, II_SYS_SYS_BUBBLEPOINT );


#ifdef __GUILD_HOUSE_MIDDLE

#define 	MAX_GH_COMMENT		64

enum { GH_STATE_NONE = 0, GH_STATE_USING, GH_STATE_TENDERABLE };
enum { GH_AREA_NONE = 0, GH_AREA_FLARIS, GH_AREA_SAINTMORNING, GH_AREA_ELIUN, GH_AREA_MAX };
enum { GH_TENDER_CLOSE = 0, GH_TENDER_CANCEL, GH_TENDER_CHECK_OPEN, GH_TENDER_OPEN, GH_TENDER_TENDER, GH_TENDER_TENDER_RESULT, GH_TENDER_MAX };

#endif // __GUILD_HOUSE_MIDDLE

//////////////////////////////////////////////////////////////////////
// CGuildHouseBase
//////////////////////////////////////////////////////////////////////

#ifdef __CLIENT
#define GuildHouse CGuildHouseBase::GetInstance()
#endif // __CLIENT
#define GH_Fntr_Info CGuildHouseBase::GuildHouse_Furniture_Info 

struct HOUSING_ITEM;

class CGuildHouseBase
{
public:
	struct GuildHouse_Furniture_Info
	{
		DWORD		dwItemId;		// 아이템 ID
		BOOL		bSetup;			// 설치여부
		D3DXVECTOR3	vPos;			// 설치위치
		float		fAngle;			// 설치각도
		OBJID		objId;			// 설치된 가구의 컨트롤ID(제거시 필요)
		time_t		tKeepTime;		// 기간(지속시간)
		int			nSeqNum;

		GuildHouse_Furniture_Info( DWORD dwII = NULL_ID, BOOL bS = FALSE, D3DXVECTOR3 v = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), float fA = 0.0f, time_t tK = 0 );
		void		Serialize( CAr & ar );
	};
	typedef std::vector<GuildHouse_Furniture_Info>	VECFurnituretInfo;
	typedef VECFurnituretInfo::iterator			VECFurnitureIter;
	
protected:
	DWORD	m_dwGuildId;
	DWORD	m_dwWorldId;
	VECFurnituretInfo	m_vecFntInfo;
#ifdef __GUILD_HOUSE_MIDDLE
	DWORD	m_dwGHIndex;
	DWORD	m_dwNpcIndex;
	int		m_nAreaIndex;
	int		m_nGHLevel;
	TCHAR	m_szGHComment[MAX_GH_COMMENT];
#endif // __GUILD_HOUSE_MIDDLE

private:
	time_t	m_tUpkeepTime;

public:
	CGuildHouseBase( DWORD dwGuildId, DWORD dwWorldId );
	virtual ~CGuildHouseBase();

	void	Clear();
	DWORD	GetType()	{ return m_dwWorldId; }
	BOOL	OnGuildHousePacket( int nPacketType, GuildHouse_Furniture_Info & gfi, int nIndex );
	void	SerializeAllInfo( CAr & ar );
	GuildHouse_Furniture_Info GetFurnitureInfo( int nIndex ) { return nIndex < (int)( m_vecFntInfo.size() ) ? m_vecFntInfo.at( nIndex ) : GuildHouse_Furniture_Info(); }
	time_t	GetUpkeepTime()	{ return m_tUpkeepTime; }
	void	SetUpkeeptime( time_t tUpkeepTime ) { m_tUpkeepTime = tUpkeepTime; }
	int		GetFurnitureListSize()	{ return m_vecFntInfo.size(); }
	BOOL	ExpiredGuildHouse();
#ifdef __GUILD_HOUSE_MIDDLE
	DWORD	GetGuildId()		{	return m_dwGuildId;	}
	DWORD	GetGuildHouseId()	{	return m_dwGHIndex;	}
	DWORD	GetGuildHouseNpcId()	{	return m_dwNpcIndex;	}
	int		GetAreaIndex()			{	return m_nAreaIndex;	}
	int		GetGuildHouseLevel()	{	return m_nGHLevel;	}
	void	GetGuildHouseComment( TCHAR* pszComment, size_t cchComment )	{	FLStrcpy( pszComment, cchComment, m_szGHComment );	}
#ifndef __CLIENT
	void	SetGuildHouseLevel( int nLevel )			{	m_nGHLevel = nLevel;	}
	void	SetGuildHouseData( DWORD dwGHIndex, DWORD dwNpcIndex, int nAreaIndex, int nGHLevel, time_t tUpkeepTime, const TCHAR* szGHComment );
	void	SetGuildHouseComment( const TCHAR* pszComment );
#endif // __nCLIENT
#endif // __GUILD_HOUSE_MIDDLE
	
#ifdef __CLIENT
	static CGuildHouseBase* GetInstance();
	void	SendClientToWorld( int nPacketType, GuildHouse_Furniture_Info & gfi, int nIndex );
	GuildHouse_Furniture_Info* GetFurnitureInfoPtr( int nIndex ) { return nIndex < (int)( m_vecFntInfo.size() ) ? &m_vecFntInfo.at( nIndex ) : NULL; }
	BOOL	IsMyGuildHouse( DWORD dwWorldId ) { return m_dwWorldId == dwWorldId; }
	GuildHouse_Furniture_Info* Find( OBJID objID );
	int		FindIndex( OBJID objID_ );
	void	ApplyEFTexture( );

	//gmpbigsun: 아래함수들은 콘트롤을 선택함에 있어서 팝업메뉴를 띄울수 없는 ( 자동으로 서버에서 타겟을 클리어시킴 ) 이유로 존재함 
	void	Setup( const HOUSING_ITEM& kInfo );
	void	Reset( const HOUSING_ITEM& kInfo );
	void	Remove( );
	void	Reset( );	// 편집기 요청 -> 모드저장 -> 실제 패킷전송 Reset호출 
	int		GetMode( )	{ return m_iMode; }				// 선택된 Obj에게 예약된 행동 ( 설치, 재설치, 해제 )

	DWORD	m_dwSelectedObjID;		//선택된 CCtrl ObjID
	int		m_iMode;
	int		m_nExtraExp;			//휴식경험치 ( 클라 보관용 )
	int		m_nWndDeployingIndex;	//클라 현재 설치, 재설치중인 녀석의 Index

public :
	void	SetFurnitureChannel( BOOL bSetFurnitureChannel )	{ m_bSetFurnitureChannel = bSetFurnitureChannel; }
	BOOL	IsSetFurnitureChannel()								{ return m_bSetFurnitureChannel; }
	void	ResetValues( );

private:
	BOOL	m_bSetFurnitureChannel;
#endif // __CLIENT

#ifdef __WORLDSERVER
public:
	void	AfterFailedGuildHousePacket( int nPacketType, GH_Fntr_Info& gfi, int nIndex );
	void	SetWaitDBAckPlayerId( DWORD dwWaitDBAckPlayerId )	{ m_dwWaitDBAckPlayerId = dwWaitDBAckPlayerId; }
	DWORD	GetWaitDBAckPlayerId()	{ return m_dwWaitDBAckPlayerId; }
	BOOL	IsWaitDBAck()	{ return m_dwWaitDBAckPlayerId != NULL_ID; }
	BOOL	SendWorldToDatabase( FLWSUser* pUser, int nPacketType, GH_Fntr_Info& gfi, int nIndex );
	void	SendWorldToClient( int nPacketType, GH_Fntr_Info& gfi, int nIndex );

	BOOL	IsEnteranceAble( FLWSUser* pUser );
	void	SetApplyDST( FLWSUser* pUser );
	void	ResetApplyDST( FLWSUser* pUser );
	BOOL	EnteranceGuildHouseRoom( FLWSUser* pUser );
	void	GoOutGuildHouseRoom( FLWSUser* pUser );
	void	CheckDestroyRoom( FLWSUser* pUser = NULL );
	
	virtual float		GetRestPointFactor() = 0;
#endif // __WORLDSERVER

private:
	BOOL	SetupFurnitureCtrl( int nIndex, GuildHouse_Furniture_Info & gfi );
	BOOL	RemoveFurnitureCtrl( int nIndex, GuildHouse_Furniture_Info & gfi );
	BOOL	ResetFurnitureCtrl( int nIndex, GuildHouse_Furniture_Info & gfi );
	BOOL	ListUpFurniture( GuildHouse_Furniture_Info & gfi );
	BOOL	ListDropFurniture( int nIndex );
#ifdef __WORLDSERVER
	BOOL	IsAuthority( FLWSUser* pUser, int nPacketType );
	BOOL	PreCheckPacket( FLWSUser* pUser, int nPacketType, GH_Fntr_Info& gfi, int nIndex );
	void	SetDSTFunriture( PT_ITEM_SPEC pItemProp );
	void	ResetDSTFunriture( PT_ITEM_SPEC pItemProp );

	BOOL	CreateGuildHouseRoom();
	BOOL	DestroyGuildHouseRoom();
	BOOL	IsListUpAble( DWORD dwItemId );
	virtual int			GetMaxListUpNum( PT_ITEM_SPEC pItemProp ) = 0;
	virtual int			GetUpkeepPenya() = 0;
	
	virtual D3DXVECTOR3	GetEnterPos() = 0;
	virtual BOOL		IsSetupAble( FLWSUser* pUser, int nIndex ) = 0;
	
	DWORD	m_dwWaitDBAckPlayerId;
#endif // __WORLDSERVER
#ifndef __CLIENT
public:
	virtual D3DXVECTOR3 GetTeleporterPos() = 0;
#endif // n__CLIENT

#ifdef __DBSERVER
public:
	int		GetFirstExpiredFurnitureIndex();
private:
#endif // __DBSERVER
};

#ifndef __CLIENT
//////////////////////////////////////////////////////////////////////
// CGuildHouseSmall : CGuildBase
//////////////////////////////////////////////////////////////////////
class CGuildHouseSmall : public CGuildHouseBase
{
public:
	CGuildHouseSmall( DWORD dwGuildId );
	virtual ~CGuildHouseSmall();

public:
#ifndef __CLIENT
	virtual D3DXVECTOR3 GetTeleporterPos()	{ return D3DXVECTOR3( 80.0f, 100.0f, 75.0f ); }
#endif // n__CLIENT

#ifdef __WORLDSERVER
public:
	virtual float		GetRestPointFactor() { return 1.0f; }
private:
	virtual D3DXVECTOR3 GetEnterPos() { return D3DXVECTOR3( (float)( xRandom( 78, 83 ) ), 100.0f, (float)( xRandom( 61, 66 ) ) ); }
	virtual int			GetMaxListUpNum( PT_ITEM_SPEC pItemProp );
	virtual int			GetUpkeepPenya() { return UPKEEP_DAY_PENYA_SMALL; }
	virtual BOOL		IsSetupAble( FLWSUser* pUser, int nIndex );
#endif // __WORLDSERVER


};


//////////////////////////////////////////////////////////////////////
// CGuildHouseMiddle : CGuildBase
//////////////////////////////////////////////////////////////////////
#ifdef __GUILD_HOUSE_MIDDLE

class CGuildHouseMiddle : public CGuildHouseBase
{
public:
	CGuildHouseMiddle( DWORD dwGuildId );
	virtual ~CGuildHouseMiddle();

public:
#ifndef __CLIENT
	virtual D3DXVECTOR3 GetTeleporterPos()	{ return D3DXVECTOR3( 298.0f, 100.0f, 194.0f ); }
#endif // n__CLIENT

#ifdef __WORLDSERVER
public:
	virtual float		GetRestPointFactor() { return 0.75f; }
private:
	virtual D3DXVECTOR3 GetEnterPos() { return D3DXVECTOR3( (float)( xRandom( 290, 296 ) ), 100.0f, (float)( xRandom( 188, 194 ) ) ); }
	virtual int			GetMaxListUpNum( PT_ITEM_SPEC /*pItemProp*/ )	{ return MAX_LISTUP_NUM; }
	virtual int			GetUpkeepPenya() { return UPKEEP_DAY_PENYA_MIDDLE; }
	virtual BOOL		IsSetupAble( FLWSUser* pUser, int nIndex );
#endif // __WORLDSERVER
};

#endif // __GUILD_HOUSE_MIDDLE



/*	15차 현재 - 기획이 전혀 없어서 사전 작업도 못한다.
				이후 작업자는 CGuildHouseMiddle을 참고하기 바란다. 
				CGuildHouseMng::MakeGuildHouse() 함수도 변경 바람.
				- chipi -
//////////////////////////////////////////////////////////////////////
// CGuildHouseBig : CGuildBase
//////////////////////////////////////////////////////////////////////
class CGuildHouseLarge : public CGuildHouseBase
{
public:
	CGuildHouseLarge( DWORD dwGuildId );
	virtual ~CGuildHouseLarge();

 public:
#ifndef __CLIENT
	virtual D3DXVECTOR3 GetTeleporterPos()	{ return D3DXVECTOR3( 270.0f, 0.0f, 300.0f ); }
#endif // n__CLIENT

#ifdef __WORLDSERVER
public:
	virtual float		GetRestPointFactor() { return 0.5f; }
private:
	virtual D3DXVECTOR3 GetEnterPos() { return D3DXVECTOR3( xRandom( 268, 273 ), 0.0f, xRandom( 298, 303 ) ); }
	virtual int			GetMaxListUpNum( PT_ITEM_SPEC pItemProp )	{ return MAX_LISTUP_NUM; }
	virtual int			GetUpkeepPenya() { return 24000000; }
	virtual BOOL		IsSetupAble( FLWSUser* pUser, int nIndex )	{ return TRUE; }
#endif // __WORLDSERVER
};
*/



//////////////////////////////////////////////////////////////////////
// CGuildHouseMng : CGuildBase
//////////////////////////////////////////////////////////////////////
#define	GuildHouseMng	CGuildHouseMng::GetInstance()
class CGuildHouseMng
{
public:
	CGuildHouseMng();
	~CGuildHouseMng();
	static CGuildHouseMng* GetInstance();

	CGuildHouseBase*	MakeGuildHouse( DWORD dwGuildId, DWORD dwWorldId );
	BOOL				AddGuildHouse( DWORD dwGuildId, CGuildHouseBase* pGuildHouse )	{ return m_mapGuildHouse.insert(std::make_pair( dwGuildId, pGuildHouse ) ).second; }
	BOOL				RemoveGuildHouse( DWORD dwGuildId );
	CGuildHouseBase* GetGuildHouse( DWORD dwGuildId );
	BOOL	IsEmpty()	{ return m_mapGuildHouse.empty(); }
	void	Serialize( CAr & ar );

#ifdef __GUILD_HOUSE_MIDDLE

	CGuildHouseBase* GetGuildHouseByIndex( DWORD dwIndex );

#endif // __GUILD_HOUSE_MIDDLE

#ifdef __WORLDSERVER
public:
	BOOL	IsGuildHouse( DWORD dwWorldId );
	BOOL	IsLoginAble( FLWSUser* pUser, DWORD dwWorldId, int nLayer );
	BOOL	EnteranceGuildHouse( FLWSUser* pUser, DWORD dwComebackItemId = NULL_ID, DWORD dwGHIndex = NULL_ID );
	void	GoOutGuildHouse( FLWSUser* pUser );
	void	CheckDestroyGuildHouse( FLWSUser* pUser );

	void	ReqBuyGuildHouse( FLWSUser* pUser );
	void	OnBuyGuildHouse( CAr & ar );
	void	OnRemoveGuildHouse( CAr & ar );
	BOOL	SendWorldToDatabase( FLWSUser* pUser, int nPacketType, GH_Fntr_Info& gfi, int nIndex = NULL_ID );

	void	SetApplyDST( FLWSUser* pUser );
	void	ResetApplyDST( FLWSUser* pUser );

	float	GetRestPointFactor( FLWSUser* pUser );

	void	AddRestExpFactorTable( int nMinLevel, int nMaxLevel, float fFactor );
	float	GetRestExpFactor( int nLevel );
	
private:
	BOOL	IsBuyAble( FLWSUser* pUser );
	
	typedef std::map<int, float> MapRestExpFactor;
	MapRestExpFactor	m_mapRestExpFactorTable;
#endif // __WORLDSERVER

#ifdef __DBSERVER
public:
	void	ProcessExpired();
	BOOL	DBLoadGuildHouse( CQuery* pQuery );
	int		AddSeqNum()	{ return ++m_nSeqNum; }
	int		GetSeqNum() { return m_nSeqNum; }

private:
	int		m_nSeqNum;
#endif // __DBSERVER

private:
	typedef std::map<DWORD, CGuildHouseBase*> MapGuildHouse;
	MapGuildHouse m_mapGuildHouse;


#ifdef __GUILD_HOUSE_MIDDLE
public:
	typedef struct _T_TENDER_DATA
	{
		int		nTenderId;
		int		nTenderArea;
		int		nTenderState;
		DWORD	dwNoticeTick;
		int		nMaxHouse;
		int		nMinPenya;
		int		nChargeRate;
		DWORD	dwPrevNoticeTick;
		DWORD	dwTenderNoticeTick;
		std::map<DWORD, int>	mapNpc;
		
		_T_TENDER_DATA() : nTenderId( 0 ), nTenderArea( 0 ), nTenderState( 0 ), dwNoticeTick( 0 ),
		nMaxHouse( 0 ), nMinPenya( 0 ), nChargeRate( 0 ), dwPrevNoticeTick( 0 ), dwTenderNoticeTick( 0 )	{}

	} T_TENDER_DATA, *PT_TENDER_DATA;
	typedef std::map<DWORD, T_TENDER_DATA>		MAP_T_TENDER_DATA;

	typedef struct _T_GUILDHOUSE_DATA
	{
		DWORD	dwGHIndex;
		DWORD	dwNpcIndex;
		DWORD	dwGHType;
		DWORD	dwGHTitleIndex;
		int		nTenderArea;
		int		nState;
		_T_GUILDHOUSE_DATA()
		: dwGHIndex( 0 ), dwNpcIndex( 0 ), dwGHType( 0 ), dwGHTitleIndex( 0 ), nTenderArea( 0 ), nState( 0 )	{}
	
	} T_GUILDHOUSE_DATA, *PT_GUILDHOUSE_DATA;
	typedef std::map<DWORD, T_GUILDHOUSE_DATA>		MAP_T_GUILDHOUSE_DATA;	// <길드하우스 인덱스, >

	typedef struct _T_GUILDHOUSE_QUEST
	{
		DWORD	dwGHType;
		int		nLevel;
		DWORD	dwDoorIndex;
		_T_GUILDHOUSE_QUEST() : dwGHType( 0 ), nLevel( 0 ), dwDoorIndex( 0 )	{}
	
	} T_GUILDHOUSE_QUEST, *PT_GUILDHOUSE_QUEST;
	typedef std::map<int, T_GUILDHOUSE_QUEST>		MAP_T_GUILDHOUSE_QUEST;

	typedef struct _T_GUILDHOUSE_TENDER
	{
		DWORD		dwGHIndex;
		DWORD		dwGuildId;
		DWORD		dwPlayerId;
		int			nTenderPerin;
		int			nTenderPenya;
		DWORD		dwGHType;
		int			nAreaIndex;
		time_t		tTenderTime;
		char		chState;

		_T_GUILDHOUSE_TENDER()
		: dwGHIndex( 0 ), dwGuildId( 0 ), dwPlayerId( 0 ), nTenderPerin( 0 ), nTenderPenya( 0 ), dwGHType( 0 ), nAreaIndex( 0 ), tTenderTime( NULL ), chState( 'T' )	{}
		_T_GUILDHOUSE_TENDER( DWORD _dwGHIndex, DWORD _dwGuildId, DWORD _dwPlayerId, int _nTenderPerin, int _nTenderPenya )
		: dwGHIndex( _dwGHIndex ), dwGuildId( _dwGuildId ), dwPlayerId( _dwPlayerId ), nTenderPerin( _nTenderPerin ), nTenderPenya( _nTenderPenya ),
		dwGHType( 0 ), nAreaIndex( 0 ), tTenderTime( NULL ), chState( 'T' )	{}
	
		void Serialize( CAr & ar )
		{
			if( ar.IsStoring() == TRUE )
			{
				ar << dwGHIndex << dwGuildId << dwPlayerId << nTenderPerin << nTenderPenya << dwGHType << nAreaIndex << tTenderTime << chState;
			}
			else
			{
				ar >> dwGHIndex >> dwGuildId >> dwPlayerId >> nTenderPerin >> nTenderPenya >> dwGHType >> nAreaIndex >> tTenderTime >> chState;
			}
		}

	} T_GUILDHOUSE_TENDER, *PT_GUILDHOUSE_TENDER;
	typedef std::map< DWORD, std::vector<T_GUILDHOUSE_TENDER> >		MAP_T_GUILDHOUSE_TENDER;


	int		GetTenderState( DWORD dwGHType );
	void	SetTenderState( DWORD dwGHType, int nState );
	void	LoadScript();
	
	int		GetTenderArea( DWORD dwGHType );
	int		GetTenderId( DWORD dwGHType );


	PT_TENDER_DATA			GetTenderData( DWORD dwGHType );
	PT_GUILDHOUSE_DATA		GetGuildHouseData( DWORD dwIndex );

	bool	GetTenderAbleGuildHouse( DWORD dwGHType, std::map<DWORD, DWORD> & mapGHTenderList );

	bool	AddGuildHouseTender( const PT_GUILDHOUSE_TENDER ptGHTender );

	void	CloseTender( DWORD dwGHType );
	
	CLuaBase	m_Lua;

private:
	void	SetTenderInfo( DWORD dwGHType, int nTenderId, int nTenderArea );

	MAP_T_TENDER_DATA			m_mapTenderData;
	MAP_T_GUILDHOUSE_DATA		m_mapGuildHouseData;
	MAP_T_GUILDHOUSE_TENDER		m_mapGuildHouseTender;
	


#ifdef __DBSERVER
public:
	BOOL	IsNoticeTime( DWORD dwGHType );
	BOOL	IsTenderTime( DWORD dwGHType );

	void	UpdateGuildHouseTender( DWORD dwGHType, CQuery* pQuery );
	void	ReturnPenyaTenderGuild( DWORD dwGHType, CQuery* pQuery );
	void	SendGuildHouseTenderInfo( DPID dpId );

	bool	IsExistTenderGuild( DWORD dwGHType );
	bool	CheckTenderOpenAble( DWORD dwGHType );

	void	ProcessTender( DWORD dwGHType );

	void	GetHighestTenderGuild( DWORD dwGHType );

	void	SetGMTenderMode( bool bGMMode )	{	m_bGMTenderMode = bGMMode;	}
	bool	IsGMTenderMode()				{	return m_bGMTenderMode;		}


private:
	const TCHAR* GetNoticeMessage( DWORD dwGHType, int nState );

	TCHAR		m_szNoticeMessage[256];

	bool		m_bGMTenderMode;

#endif // __DBSERVER
	
#ifdef __WORLDSERVER
public:
	bool	ReqTenderGuildHouseList( FLWSUser* pUser, DWORD dwGHType, DWORD dwNpcIndex );
	void	ReqTenderGuildHouseInfo( FLWSUser* pUser, DWORD dwGHIndex );
	void	ReqGuildHouseTenderJoin( FLWSUser* pUser, DWORD dwGHIndex, int nTenderPerin, int nTenderPenya );
	void	ReqGuildHouseInfo( FLWSUser* pUser, PT_PACKET_GUILDHOUSE_INFO ptGHInfo );
	void	ReqGuildHouseCommentChange( FLWSUser* pUser, const PT_PACKET_GUILDHOUSE_INFO ptGHInfo );
	bool	IsTenderAbleGuildHouse( DWORD dwGHType, DWORD dwGHIndex );
	bool	IsTenderAble( FLWSUser* pUser, DWORD dwGHIndex, int nTenderPerin, int nTenderPenya );



	PT_GUILDHOUSE_TENDER	GetGuildHouseTender( DWORD dwGuildId );
	std::vector<DWORD>			GetTenderView( DWORD dwGHIndex );

	
	void	OnGuildHouseTenderInfo( CAr & ar );
	void	OnGuildHouseTenderJoin( CAr & ar );
	void	OnGuildHouseTenderState( CAr & ar );
	void	OnGuildHouseGradeUpdate( CAr & ar );
	void	OnGuildHouseLevelUpdate( CAr & ar );
	void	OnGuildHouseExpired( CAr & ar );

	void	RemoveGuildHouseDoor( CObj* pObj, DWORD dwWorldId, int nLayer );
	void	RemoveGuildHouseDoor( CGuildHouseBase* pGuildHouse );

	CGuildHouseBase*	ReMakeGuildHouse( DWORD dwGuildId, DWORD dwWorldId );

	PT_GUILDHOUSE_QUEST	GetGuildHouseQuestData( int nQuestId );
	void	CheckGuildHouseQuest( FLWSUser* pUser, int nQuestId );

	void	CheckGuildHouseQuest( FLWSUser* pUser );

private:
	MAP_T_GUILDHOUSE_QUEST	m_mapGHQuest;
#endif // __WORLDSERVER

#endif // __GUILD_HOUSE_MIDDLE
};
#endif // __CLIENT 


#endif // !defined(AFX_GUILDHOUSE_H__7A401B90_A88E_4AB9_A5A3_CBFE71AB03F6__INCLUDED_)
