
#ifndef __FLMADRIGALGIFT_H__
#define __FLMADRIGALGIFT_H__


namespace nsMadrigalGift
{

struct T_GIFT_PLAYER_DATA
{
	DWORD	dwGrade;
	int		nMinLevel;
	int		nMaxLevel;

	T_GIFT_PLAYER_DATA()	: dwGrade( NULL_ID ), nMinLevel( 0 ), nMaxLevel( 0 )
	{
	}

	T_GIFT_PLAYER_DATA( const DWORD Grade, const int MinLevel, const int MaxLevel)
	: dwGrade( Grade ), nMinLevel( MinLevel ), nMaxLevel( MaxLevel )
	{
	}

	bool	operator == ( const T_GIFT_PLAYER_DATA & rhs ) const
	{
		return ( dwGrade == rhs.dwGrade && nMinLevel <= rhs.nMinLevel && nMaxLevel >= rhs.nMaxLevel );
	}

	bool	operator < ( const T_GIFT_PLAYER_DATA & rhs ) const
	{	
		return ( dwGrade == rhs.dwGrade )
			? ( nMinLevel < rhs.nMinLevel && nMaxLevel < rhs.nMaxLevel )
			: ( dwGrade < rhs.dwGrade );

	}
};

struct T_GIFT_ITEM_DATA
{
	DWORD	dwItemID;
	DWORD	dwRewardProb;
	DWORD	dwMinQuantity;
	DWORD	dwMaxQuantity;

	T_GIFT_ITEM_DATA() : dwItemID( 0 ), dwRewardProb( 0 ), dwMinQuantity( 0 ), dwMaxQuantity( 0 )
	{
	}
};

typedef std::vector< T_GIFT_ITEM_DATA >						RewardItemVec;
typedef std::map< T_GIFT_PLAYER_DATA, RewardItemVec >		RewardItem_PlayerMap;


struct T_GIFT_POINT
{
	int		nMinPoint;
	int		nMaxPoint;

	T_GIFT_POINT() : nMinPoint( 0 ), nMaxPoint( 0 )
	{
	}

	T_GIFT_POINT( const int MinPoint, const int MaxPoint) : nMinPoint( MinPoint ), nMaxPoint( MaxPoint )
	{
	}

	bool	operator == ( const T_GIFT_POINT & rhs ) const
	{
		return ( nMinPoint <= rhs.nMinPoint && nMaxPoint >= rhs.nMaxPoint );
	}

	bool	operator < ( const T_GIFT_POINT & rhs ) const
	{	
		return ( nMinPoint < rhs.nMinPoint && nMaxPoint < rhs.nMaxPoint );
	}
};

struct T_GIFT_STEP
{
	BYTE	byStep;
	int		nAddLevel;

	T_GIFT_STEP() : byStep( 0 ), nAddLevel( 0 )
	{
	}
};

typedef std::map< T_GIFT_POINT, T_GIFT_STEP >		GiftStepDataMap;
typedef std::map< BYTE /* Step */, T_GIFT_POINT >	GiftPointDataMap;


const DWORD		MAX_REWARD_PROB				= 10000;

const DWORD		POINT_REWARD_TIME_GAP_SEC	= 60;

const float		DIVIDE_POINT_REWARD_RATE	= 10000.0f;

const int		MAX_LEVEL_GAP				= 3;

}	// namespace nsMadrigalGift


class FLMadrigalGift
{
	public:
		FLMadrigalGift();
		~FLMadrigalGift();

		static FLMadrigalGift* GetInstance();


		bool	IsActivate() const;
		void	SetActivate( const bool bActivate );

		bool	LoadScript();

		void	OnLogin( FLWSUser* pUser, const time_t tLogOut ) const;
		void	OnKeepConnection( FLWSUser* pUser, const DWORD dwTick ) const;
		void	OnMonsterKill( FLWSUser* pUser, CMover* pMover ) const;
		void	OnLevel( FLWSUser* pUser ) const;

		bool	OnReqReceiveGiftItem( FLWSUser* pUser ) const;


	private:
		void	Init();
		
		bool	LoadRewardItemScript( const TCHAR* pszFileName );
		bool	LoadStepPointScript( const TCHAR* pszFileName );
		bool	LoadAddPointScript( const TCHAR* pszFileName );
		
		DWORD	PlayerGradeStringToDefine( std::string & strPlayerGrade ) const;

		bool	CalcLoginPoint( FLWSUser* pUser, const time_t tLogOut ) const;
		bool	CalcKeepConnectionPoint( FLWSUser* pUser, const DWORD dwTick ) const;
		bool	CalcMonsterKillPoint( FLWSUser* pUser, CMover* pMover ) const;

		bool	DoReceiveGiftItem( FLWSUser* pUser ) const;

		void	SendSystemActiveNoti( FLWSUser* pUser ) const;
		void	SendGiftPointNoti( FLWSUser* pUser ) const;
		void	SendReceiveGiftItemAck( FLWSUser* pUser ) const;

		float	GetGiftPointRate( FLWSUser* pUser ) const;
		int		GetAddRewardLevel( FLWSUser* pUser ) const;

		bool	GetGiftStep( const int nHavePoint, nsMadrigalGift::T_GIFT_STEP & kGiftStep ) const;
		bool	GetGiftPoint( const BYTE byStep, nsMadrigalGift::T_GIFT_POINT & kGiftPoint ) const;
		bool	GetRewardItemList( const DWORD dwGrade, const int nLevel, nsMadrigalGift::RewardItemVec & vecRewardItem ) const;
		bool	GetRewardItem( const nsMadrigalGift::RewardItemVec & vecRewardItem, nsMadrigalGift::T_GIFT_ITEM_DATA & kRewardItem ) const;



	private:
		nsMadrigalGift::RewardItem_PlayerMap	m_mapRewardItemData;
		nsMadrigalGift::GiftStepDataMap			m_mapGiftStepData;
		nsMadrigalGift::GiftPointDataMap		m_mapGiftPointData;

		bool	m_bActivate;

		int		m_nLogOutPoint;
		int		m_nKeepConnectionPoint;
		int		m_nMaxGiftPoint;
};

#define		g_pMadrigalGift		FLMadrigalGift::GetInstance()

#endif // __FLMADRIGALGIFT_H__
