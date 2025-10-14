
#ifndef __FLFLYFFPIECEDAILYPAYMENT_H__
#define __FLFLYFFPIECEDAILYPAYMENT_H__

#include "../_CommonDefine/FLTm.h"

namespace nsFlyffPieceDailyPayment
{

enum E_BURNING_MODE
{
	E_BURNING_MODE_NONE = 0
	, E_ITEM_QUANTITY_MAX = 0x01
	, E_CONNECTION_TICK_DECREASE = 0x02
	, E_MODE_ALL = ( E_ITEM_QUANTITY_MAX | E_CONNECTION_TICK_DECREASE )
};

enum E_DAY
{
	DAY_NONE = 0, DAY_SUN, DAY_MON, DAY_TUE, DAY_WED, DAY_THU, DAY_FRI, DAY_SAT, DAY_MAX
};


struct T_PERIOD_DATA
{
	FLTm	tmStartDate;
	FLTm	tmEndDate;


	T_PERIOD_DATA() : tmStartDate( INVALID_DATETIME ), tmEndDate( INVALID_DATETIME )
	{
	}

	T_PERIOD_DATA( const FLTm StartDate, const FLTm EndDate) : tmStartDate( StartDate ), tmEndDate( EndDate )
	{
	}
};

struct T_CONNECTION_DATA
{
	int		nDay;
	DWORD	dwConnectionTick;


	T_CONNECTION_DATA() : nDay( 0 ), dwConnectionTick( 0 )
	{
	}

	T_CONNECTION_DATA( const int Day, const DWORD Tick ) : nDay( Day ), dwConnectionTick( Tick )
	{
	}
};

struct T_USER_DATA
{
	int		nMinLevel;
	int		nMaxLevel;
	DWORD	dwMinClass;
	DWORD	dwMaxClass;


	T_USER_DATA() : nMinLevel( 0 ), nMaxLevel( 0 ), dwMinClass( 0 ), dwMaxClass( 0 )
	{
	}

	T_USER_DATA( const int MinLevel, const int MaxLevel, const DWORD MinClass, const DWORD MaxClass )
		: nMinLevel( MinLevel ), nMaxLevel( MaxLevel ), dwMinClass( MinClass ), dwMaxClass( MaxClass )
	{
	}
};

struct T_ITEM_DATA
{
	DWORD	dwItemID;
	int		nMinQuantity;
	int		nMaxQuantity;
	bool	bBind;


	T_ITEM_DATA() : dwItemID( 0 ), nMinQuantity( 0 ), nMaxQuantity( 0 ), bBind( false )
	{
	}

	T_ITEM_DATA( const DWORD ItemID, const int Min, const int Max, const bool Bind )
		: dwItemID( ItemID ), nMinQuantity( Min ), nMaxQuantity( Max ), bBind( Bind )
	{
	}
};

typedef std::vector< DWORD /*dwIndex*/ >					IndexVec;
typedef std::map< int /*nDay*/, IndexVec >					IndexMap;
typedef std::map< DWORD /*dwIndex*/, T_PERIOD_DATA >		PeriodDataMap;
typedef std::map< DWORD /*dwIndex*/, T_CONNECTION_DATA >	ConnectionDataMap;
typedef std::map< DWORD /*dwIndex*/, T_USER_DATA >			UserDataMap;
typedef std::map< DWORD /*dwIndex*/, T_ITEM_DATA >			ItemDataMap;
typedef std::map< E_BURNING_MODE, time_t >					BurningModeDataMap;


struct T_PLAYER_INDEX_KEY
{
	DWORD	dwPlayerID;
	DWORD	dwIndex;

	T_PLAYER_INDEX_KEY() : dwPlayerID( NULL_PLAYER_ID ), dwIndex( 0 )
	{
	}

	T_PLAYER_INDEX_KEY( const DWORD PlayerID, const DWORD Index ) : dwPlayerID( PlayerID ), dwIndex( Index )
	{
	}

	bool	operator == ( const T_PLAYER_INDEX_KEY & rhs ) const
	{
		return ( dwPlayerID == rhs.dwPlayerID && dwIndex == rhs.dwIndex );
	}

	bool	operator < ( const T_PLAYER_INDEX_KEY & rhs ) const
	{	
		return ( dwPlayerID == rhs.dwPlayerID ) ? ( dwIndex < rhs.dwIndex ) : ( dwPlayerID < rhs.dwPlayerID );
	}
};

typedef std::map< DWORD /*dwPlayerID*/, IndexVec >			PlayerIndexMap;
typedef std::map< T_PLAYER_INDEX_KEY, DWORD /*Tick*/ >		ConnectionPlayerMap;


//////////////////////////////////////////////////////////////////////////
// 아래는 기획팀의 요청으로 설정한 제한 수치.
const DWORD ONE_HOUR					= 60;
const DWORD DECREASE_CONNECTION_TICK	= MIN( 10 );					// 버닝 모드일때 줄어드는 접속 유지 시간

const DWORD MAX_BURNING_MINUTE			= ONE_HOUR * 6;					// 최대 버닝 타임 유지 시간

const DWORD MIN_KEEP_CONNECTION_TICK	= MIN( 30 );					// 접속 유지 시간 최소치
const DWORD MAX_KEEP_CONNECTION_TICK	= MIN( ONE_HOUR * 24 );			// 접속 유지 시간 최대치

const DWORD LIMIT_MIN_QUANTITY			= 2;							// 최소 지급 개수 제한 수치
const DWORD LIMIT_MAX_QUANTITY			= 3;							// 최대 지급 개수 제한 수치
//////////////////////////////////////////////////////////////////////////


class FLFlyffPieceDailyPayment
{
	public:
		FLFlyffPieceDailyPayment();
		~FLFlyffPieceDailyPayment();


		static	FLFlyffPieceDailyPayment*	GetInstance();

		void	Init();

		void	Load();

		bool	IsActivate() const;
		
		bool	SetBurningMode( const T_BURNING_MODE & kMode );
		bool	InitializeBurningMode();
		bool	IsActivateBurningMode( const E_BURNING_MODE eBurningMode ) const;

		void	SetDay( const int nDay );
		int		GetDay() const;
		int		GetCurrentDay() const;

		void	Process( const time_t time );

		void	OnDailyKeepConnection( FLWSUser* pUser, const DWORD dwCurrTick );
		void	OnLogOut( const DWORD dwPlayerID );


	private:
		bool	LoadScript( const TCHAR* pszFileName );

		bool	InsertDailyIndex( const int nDay, const DWORD dwIndex );
		bool	InsertDailyPeriodData( const DWORD dwIndex, const T_PERIOD_DATA & kData );
		bool	InsertDailyConnectionData( const DWORD dwIndex, const T_CONNECTION_DATA & kData );
		bool	InsertDailyUserData( const DWORD dwIndex, const T_USER_DATA & kData );
		bool	InsertDailyItemData( const DWORD dwIndex, const T_ITEM_DATA & kData );

		bool	CheckDailySettingData() const;

		void	SetActivate( const bool bActivate );

		bool	GetIndexByDay( const int nDay, IndexVec & vecIndex ) const;
		bool	GetPeriodDataByIndex( const DWORD dwIndex, T_PERIOD_DATA & kData ) const;
		bool	GetConnectionDataByIndex( const DWORD dwIndex, T_CONNECTION_DATA & kData ) const;
		bool	GetUserDataByIndex( const DWORD dwIndex, T_USER_DATA & kData ) const;
		bool	GetItemDataByIndex( const DWORD dwIndex, T_ITEM_DATA & kData ) const;
		bool	GetIndexByPlayerID( const DWORD dwPlayerID, IndexVec & vecIndex ) const;
		DWORD	GetLastTick( const T_PLAYER_INDEX_KEY & kKey ) const;

		DWORD	DayStringToDefine( std::string & strDay ) const;
		DWORD	PlayerGradeStringToDefine( std::string & strPlayerGrade ) const;

		bool	IsDailyPaymentPeriod( const T_PERIOD_DATA & kData ) const;
		bool	IsDailyPaymentUser( FLWSUser* pUser, const T_USER_DATA & kData ) const;

		bool	CanDailyPayment( FLWSUser* pUser, const DWORD dwIndex, const DWORD dwCurrTick );
		bool	DoDailyPayment( FLWSUser* pUser, const DWORD dwIndex ) const;
		
		bool	IsExistPlayerTick( const T_PLAYER_INDEX_KEY & kKey ) const;
		bool	InsertPlayerTick( const T_PLAYER_INDEX_KEY & kKey, const DWORD dwCurrTick );
		bool	UpdatePlayerTick( const T_PLAYER_INDEX_KEY & kKey, const DWORD dwCurrTick );
		void	InitializeAllPlayerTick();

		DWORD	CheckBurningMode( const time_t time );


	private:
		bool					m_bActivate;
		int						m_nDay;

		IndexMap				m_mapIndex;
		PeriodDataMap			m_mapPeriodData;
		ConnectionDataMap		m_mapConnectionData;
		UserDataMap				m_mapUserData;
		ItemDataMap				m_mapItemData;
		BurningModeDataMap		m_mapBurningModeData;

		PlayerIndexMap			m_mapPlayerIndex;
		ConnectionPlayerMap		m_mapConnectionPlayer;
};

}	// namespace nsFlyffPieceDailyPayment

#define		g_pFlyffPieceDailyPayment		FLFlyffPieceDailyPayment::GetInstance()

#endif // __FLFLYFFPIECEDAILYPAYMENT_H__
