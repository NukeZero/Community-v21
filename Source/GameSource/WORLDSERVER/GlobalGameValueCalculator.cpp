#include "StdAfx.h"

#include "GlobalGameValueCalculator.h"

#include "FLCC_Reward.h"

const int PRECISION		= 10000;


BOOL 						FLGetGlobalBuyFactor( __int64 & io_n64Gold )
{
	using namespace nsCooperativeContributions;

	// 100% - ( ÇÒÀÎ·ü1 + ÇÒÀÎ·ü2 )
	const int nTotRate	= REWARD_PRECISION 
		- ( ( REWARD_PRECISION - ( int )( prj.m_EventLua.GetShopBuyFactor() * REWARD_PRECISION ) )
		+ ( REWARD_PRECISION - GLOBAL_REWARD_ALARM_SHOP_BUY_FACTOR().GetRewardValue() ) );

	io_n64Gold			= ( __int64 )( ( io_n64Gold * ( __int64 )nTotRate ) / REWARD_PRECISION );
	io_n64Gold			= ( __int64 )( io_n64Gold * prj.m_fShopCost );

	if( io_n64Gold < 0 || io_n64Gold > INT_MAX )
		return FALSE;

	return TRUE;
}

BOOL 						FLGetGlobalSellFactor( int & io_nGold )
{
	using namespace nsCooperativeContributions;

	// 100% - ( ÇÒÀÎ·ü1 + ÇÒÀÎ·ü2 )
	const int nTotRate	= REWARD_PRECISION 
		- ( ( REWARD_PRECISION - ( int )( prj.m_EventLua.GetShopSellFactor() * REWARD_PRECISION ) )
		+ ( REWARD_PRECISION - GLOBAL_REWARD_ALARM_SHOP_SELL_FACTOR().GetRewardValue() ) );

	const __int64 n64Gold		= ( __int64 )( ( ( __int64 )io_nGold * ( __int64 )nTotRate ) / REWARD_PRECISION );
	if( n64Gold < 0 || n64Gold > INT_MAX )
		return FALSE;

	io_nGold			= static_cast< int >( n64Gold );
	return TRUE;
}

