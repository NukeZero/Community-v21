#ifndef __FLCOOPERATIVE_CONTRIBUTIONS_REWARD_H__
#define __FLCOOPERATIVE_CONTRIBUTIONS_REWARD_H__

class FLWSUser;


#include "FLQuestInterface.h"
#include "../_Common/post.h"

#include "FLCooperativeContributions.h"

class FLWSUser;




namespace nsCooperativeContributions	{

template < class _Ty >
class IGlobalRewardAlarm
{
public:
	virtual void			SerializeOut_Info( CAr & ar, DWORD & dwTextID, _Ty & t ) = 0;		// 패킷 정보 
	virtual void			SerializeOut_Rewarded( CAr & ar ) = 0;
	
	virtual void			SetRewardValue( const _Ty t )	= 0;
	virtual _Ty				GetRewardValue() const			= 0;
	virtual _Ty				GetBaseRewardValue() const		= 0;

public:
	virtual void			onExpired() = 0;
	//virtual void			onReward() = 0;
	//virtual void			onRewardNotify( FLWSUser & User ) = 0;
};


// 하나의 보상
template < class _ValueType >			//TTP가 안됨.
class FLReward_Global : public IGlobalReward
{
public:
	FLReward_Global( IGlobalRewardAlarm< _ValueType > & globalData, const _ValueType t, const DWORD dwRewardTextID );
	virtual ~FLReward_Global();


	virtual void			SerializeOut_Info( CAr & ar );
	
	virtual BOOL			TryReward();
	virtual void			Clean_ExpiredReward();
	
	//virtual void			RewardNotify( FLWSUser & User );
	
private:
	/*const*/ _ValueType					m_fRewardConst;
	/*const*/ DWORD							m_dwRewardTextID;

	BOOL								m_bRewarded;
	IGlobalRewardAlarm< _ValueType > *	m_pGlobalRewardAlarm;
};


//하나의 보상 DATA
template < class _Ty >
class FLGlobalRewardAlarm : public IGlobalRewardAlarm< _Ty >
{
public:
	typedef					_Ty		value_type;

	//FLGlobalRewardAlarm() : m_value( 0 ){}
	FLGlobalRewardAlarm( const _Ty valueBase ) : m_valueBase( valueBase ), m_value( valueBase ){}
	~FLGlobalRewardAlarm() {}

	//virtual void			SeralizeInfo( CAr & ar, DWORD dwTextID, _Ty t ) = 0;
	virtual void			SetRewardValue( const _Ty value );
	virtual _Ty				GetRewardValue() const;
	virtual _Ty				GetBaseRewardValue() const;
	//virtual void			onRewardNotify( FLWSUser & User ) = 0;

protected:
	const _Ty				m_valueBase;
	_Ty						m_value;
};


template < class _Ty >
void FLGlobalRewardAlarm<_Ty>::SetRewardValue( const _Ty value )
{
	m_value		= value;
}

template < class _Ty >
_Ty FLGlobalRewardAlarm<_Ty>::GetRewardValue() const
{
	return m_value;
}

template < class _Ty >
_Ty FLGlobalRewardAlarm<_Ty>::GetBaseRewardValue() const
{
	return m_valueBase;
}

//-----------------------------------------------------------------------------------------------------------------------------//



//enum 
//{
#define	REWARD_PRECISION		100
//};


//글로벌 보상 data class 들
//class FLGlobalRewardAlarm_ExpFactor : public FLGlobalRewardAlarm< float >
class FLGlobalRewardAlarm_ExpFactor : public FLGlobalRewardAlarm< DWORD >
{
public:
	static FLGlobalRewardAlarm_ExpFactor & GetInstance();

	FLGlobalRewardAlarm_ExpFactor() : FLGlobalRewardAlarm< DWORD >( REWARD_PRECISION ) {}

	virtual void			SerializeOut_Info( CAr & ar, DWORD & dwTextID, DWORD & t );
	virtual void			SerializeOut_Rewarded( CAr & ar );
	//virtual void			SerializeType( CAr & ar );

private:
	//virtual void			onReward();
	virtual void			onExpired();
	//virtual void			onRewardNotify( FLWSUser & User );
};

inline static FLGlobalRewardAlarm_ExpFactor &		GLOBAL_REWARD_ALARM_EXP_FACTOR()			{	return FLGlobalRewardAlarm_ExpFactor::GetInstance();	}

//class FLGlobalRewardAlarm_DropRate : public FLGlobalRewardAlarm< float >
class FLGlobalRewardAlarm_DropRate : public FLGlobalRewardAlarm< DWORD >
{
public:
	static FLGlobalRewardAlarm_DropRate & GetInstance();

	FLGlobalRewardAlarm_DropRate() : FLGlobalRewardAlarm< DWORD >( REWARD_PRECISION ) {}

	virtual void			SerializeOut_Info( CAr & ar, DWORD & dwTextID, DWORD & t );
	virtual void			SerializeOut_Rewarded( CAr & ar );
	//virtual void			SerializeType( CAr & ar );

private:
	//virtual void			onReward();
	virtual void			onExpired();
	//virtual void			onRewardNotify( FLWSUser & User );
};
inline static FLGlobalRewardAlarm_DropRate &			GLOBAL_REWARD_ALARM_DROP_RATE()			{	return FLGlobalRewardAlarm_DropRate::GetInstance();	}


//class FLGlobalRewardAlarm_ShopBuyFactor : public FLGlobalRewardAlarm< float >
class FLGlobalRewardAlarm_ShopBuyFactor : public FLGlobalRewardAlarm< DWORD >
{
public:
	static FLGlobalRewardAlarm_ShopBuyFactor & GetInstance();

	FLGlobalRewardAlarm_ShopBuyFactor() : FLGlobalRewardAlarm< DWORD >( REWARD_PRECISION ) {}

	virtual void			SerializeOut_Info( CAr & ar, DWORD & dwTextID, DWORD & t );
	virtual void			SerializeOut_Rewarded( CAr & ar );
	//virtual void			SerializeType( CAr & ar );

private:
	//virtual void			onReward();
	virtual void			onExpired();
	//virtual void			onRewardNotify( FLWSUser & User );
};
inline static FLGlobalRewardAlarm_ShopBuyFactor &	GLOBAL_REWARD_ALARM_SHOP_BUY_FACTOR()	{	return FLGlobalRewardAlarm_ShopBuyFactor::GetInstance();	}

//class FLGlobalRewardAlarm_ShopSellFactor : public FLGlobalRewardAlarm< float >
class FLGlobalRewardAlarm_ShopSellFactor : public FLGlobalRewardAlarm< DWORD >
{
public:
	static FLGlobalRewardAlarm_ShopSellFactor & GetInstance();

	FLGlobalRewardAlarm_ShopSellFactor() : FLGlobalRewardAlarm< DWORD >( REWARD_PRECISION ) {}

	virtual void			SerializeOut_Info( CAr & ar, DWORD & dwTextID, DWORD & t );
	virtual void			SerializeOut_Rewarded( CAr & ar );
	//virtual void			SerializeType( CAr & ar );

private:
	//virtual void			onReward();
	virtual void			onExpired();
	//virtual void			onRewardNotify( FLWSUser & User );
};
inline static FLGlobalRewardAlarm_ShopSellFactor &	GLOBAL_REWARD_ALARM_SHOP_SELL_FACTOR()	{	return FLGlobalRewardAlarm_ShopSellFactor::GetInstance();	}

class FLGlobalRewardAlarm_DefencePower : public FLGlobalRewardAlarm< int >
{
public:
	static FLGlobalRewardAlarm_DefencePower & GetInstance();

	FLGlobalRewardAlarm_DefencePower() : FLGlobalRewardAlarm< int >( 0 )	{}

	virtual void			SerializeOut_Info( CAr & ar, DWORD & dwTextID, int & t );
	virtual void			SerializeOut_Rewarded( CAr & ar );
	//virtual void			SerializeType( CAr & ar );

private:
	//virtual void			onReward();
	virtual void			onExpired();
	//virtual void			onRewardNotify( FLWSUser & User );
};
inline static FLGlobalRewardAlarm_DefencePower &		GLOBAL_REWARD_ALARM_DEFENCE_POWER()		{	return FLGlobalRewardAlarm_DefencePower::GetInstance();	}

class FLGlobalRewardAlarm_AttackPower : public FLGlobalRewardAlarm< int >
{
public:
	static FLGlobalRewardAlarm_AttackPower & GetInstance();

	FLGlobalRewardAlarm_AttackPower() : FLGlobalRewardAlarm< int >( 0 )	{}

	virtual void			SerializeOut_Info( CAr & ar, DWORD & dwTextID, int & t );
	virtual void			SerializeOut_Rewarded( CAr & ar );
	//virtual void			SerializeType( CAr & ar );

private:
	//virtual void			onReward();
	virtual void			onExpired();
	//virtual void			onRewardNotify( FLWSUser & User );
};
inline static FLGlobalRewardAlarm_AttackPower &		GLOBAL_REWARD_ALARM_ATTACK_POWER()		{	return FLGlobalRewardAlarm_AttackPower::GetInstance();	}








//-------------------------------------------------------------------------------------------------------------------------------//

//typedef						std::map< DWORD, DWORD >					ItemCountMap;

class FLItemReward_User : public IUserReward
{
public:
	FLItemReward_User( const DWORD dwItemID, const short shItemCount, const DWORD dwRewardTextID ); 
	virtual ~FLItemReward_User();

	virtual void			SerializeOut_Info( CAr & ar );

	virtual void			Query_TryWriteRewardInfo( const DWORD dwContributionID, const FLTm & tmStart, const FLTm & tmEnd );
	virtual BOOL			TryReward( const DWORD dwContributionID, FLWSUser & User );
	
private:
	/*const*/ DWORD				m_dwItemID;
	/*const*/ short				m_shItemCount;
	const DWORD				m_dwRewardTextID;
//	ItemCountMap			m_mapRewardItemProp;
};
//
//class FLRewardBuff_User : public IUserReward
//{
//public:
//	FLRewardBuff_User( const WORD wSkillID, const DWORD dwSkillLevel, const BYTE byBuffTickType, const DWORD dwRemainTick, const DWORD dwRewardTextID )
//L	: m_wSkillID( wSkillID )
//	, m_dwSkillLevel( dwSkillLevel )
//	, m_byBuffTickType( byBuffTickType )
//	, m_dwRemainTick( dwRemainTick )
//	, m_dwRewardTextID( dwRewardTextID )
//	{
//	}
//
//	virtual ~FLRewardBuff_User()
//	{
//
//	}
//
//	virtual BOOL			TryReward( const DWORD dwContiributionID, FLWSUser & User )
//	{
//		if( User.HasBuff( BUFF_SKILL, m_wSkillID ) == FALSE )
//			User.AddBuff( BUFF_SKILL, m_wSkillID, 0, m_dwRemainTick, 0, m_byBuffTickType, NULL_ID );
//	}
//
//	virtual DWORD			GetRewardTextID() const { return m_dwRewardTextID; }
//
//private:
//	const WORD				m_wSkillID;
//	const DWORD				m_dwSkillLevel;
//	const BYTE				m_byBuffTickType;
//	const DWORD				m_dwRemainTick;
//
//	const DWORD				m_dwRewardTextID;
//};

//typedef						std::map< DWORD/*ItemID*/, DWORD/*ItemCount*/ >					ItemCountMap;
//
//class FLItemRewards_User : public IUserRewards
//{
//public:
//	FLItemRewards_User(); 
//	virtual ~FLItemRewards_User();
//
//	virtual BOOL			TryReward( const DWORD dwContributionID, FLWSUser & User );
//	//virtual DWORD			GetRewardTextID() const	{ return m_dwRewardTextID; }
//
//	size_t					Size() const { return m_mapRewardItemProp.size(); }
//	void					PushRewardItem( const DWORD dwItemID, const DWORD dwItemCount, const DWORD dwTextID );
//
//private:
//	ItemCountMap			m_mapRewardItemProp;
////	const DWORD				m_dwItemID;
////	const DWORD				m_dwItemCount;
////	const DWORD				m_dwRewardTextID;
////	ItemCountMap			m_mapRewardItemProp;
//};
//





//----------------------------------------------------------------------------------------------------------//

template < class _ValueType >
FLReward_Global<_ValueType>::FLReward_Global( IGlobalRewardAlarm< _ValueType > & globalData, const _ValueType t, const DWORD dwRewardTextID ) 
: m_pGlobalRewardAlarm( &globalData ), m_fRewardConst( t ), m_dwRewardTextID( dwRewardTextID ), m_bRewarded( FALSE )
{

}

template < class _ValueType >
FLReward_Global<_ValueType>::~FLReward_Global()
{

}

template < class _ValueType >
void FLReward_Global<_ValueType>::SerializeOut_Info( CAr & ar )
{
	m_pGlobalRewardAlarm->SerializeOut_Info( ar, m_dwRewardTextID, m_fRewardConst );
}

template < class _ValueType >
BOOL FLReward_Global<_ValueType>::TryReward()
{
	if( m_bRewarded == TRUE )
		return FALSE;

	m_pGlobalRewardAlarm->SetRewardValue( m_fRewardConst );
	m_bRewarded				= TRUE;
	return TRUE;
}

template < class _ValueType >
void FLReward_Global<_ValueType>::Clean_ExpiredReward()
{
	m_pGlobalRewardAlarm->SetRewardValue( m_pGlobalRewardAlarm->GetBaseRewardValue() );
	m_pGlobalRewardAlarm->onExpired();
}


} //nsCooperativeContributions

#endif //__FLCOOPERATIVE_CONTRIBUTIONS_REWARD_H__