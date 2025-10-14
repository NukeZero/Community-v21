#include "StdAfx.h"

#include "FLCC_Reward.h"

#include "FLCooperativeContributions.h"
#include "../_CommonDefine/FLPacketStruct.h"
#include "../_Common/FLCooperativeContributions_Common.h"

namespace nsCooperativeContributions {



//---------------------------------------------------------------------------------------------------------------------------------------//

FLItemReward_User::FLItemReward_User( const DWORD dwItemID, const short shItemCount, const DWORD dwRewardTextID )
: m_dwItemID( dwItemID )
, m_shItemCount( shItemCount )
, m_dwRewardTextID( dwRewardTextID )
{
}

FLItemReward_User::~FLItemReward_User()
{

}



void FLItemReward_User::SerializeOut_Info( CAr & ar )
{
    ar << REWARD_ITEM;// << m_dwItemID;// << m_nItemCount;
	FLCooperativeContributions_Common::SerializeInfo_GlobalItemReward( ar, m_dwItemID, m_shItemCount );
}




BOOL FLItemReward_User::TryReward( const DWORD /*dwContributionID*/, FLWSUser & /*User*/ )
{
	//const FLContribution * pProp		= COOPERATIVE_CONTRIBUTIONS().FindContribution( dwContributionID );
	//if( pProp == NULL )
	//{
	//	Error( "[ INVALID dwContributionID(%u) ]", dwContributionID );
	//	return FALSE;
	//}


	//// header
	//BEFORESENDDUAL( ar, PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_REWARD_ITEM_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
	//T_W2DB_PACKET_COOPERATIVE_CONTRIBUTIONS_REWARD_ITEM_REQ reward;
	//reward.idPlayer			= User.m_idPlayer;
	//reward.dwContributionID	= dwContributionID;
	//reward.tmExpire			= pProp->m_tmEndDate;
	//FLStrcpy( reward.szRewardKind, _countof( reward.szRewardKind ), "ITEM" ); 
	//FLStrcpy( reward.szMailTitle, _countof( reward.szMailTitle ), pProp->m_szMailTitle ); 
	//FLStrcpy( reward.szMailText, _countof( reward.szMailText ), pProp->m_szMailText ); 
	////reward.dwRewardCount	= m_mapRewardItemProp.size();
	//reward.dwItemID			= m_dwItemID;
	//reward.nItemCount		= m_nItemCount;

	//ar.Write( & reward, sizeof( reward ) );

	//SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );	
	return TRUE;
}

void FLItemReward_User::Query_TryWriteRewardInfo( const DWORD dwContributionID, const FLTm & tmStart, const FLTm & tmEnd )
{
	T_W2DB_PACKET_TRY_WRITE_ITEM_REWARD_INFO_REQ req;
	
	req.dwContributionID		= dwContributionID;
	req.tmStart					= tmStart;
	req.tmEnd					= tmEnd;
	req.dwItemID				= m_dwItemID;
	req.shItemCount				= m_shItemCount;

	BEFORESENDDUAL( ar, PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_TRY_WRITE_ITEM_REWARD_INFO_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.Write( &req, sizeof( req ) );
	SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );	
	//return TRUE;
}
//
//FLItemRewards_User::FLItemRewards_User()
//{
//}
//	
//FLItemRewards_User::~FLItemRewards_User()
//{
//}
//
//void FLItemRewards_User::PushRewardItem( const DWORD dwItemID, const DWORD dwItemCount, const DWORD dwTextID )
//{
//	if( m_mapRewardItemProp.size() >= MAX_MAIL_ITEM )
//	{
//		FLERROR_LOG( PROGRAM_NAME, _T( "[ OVERFLOW REWARD ITEM (%u) ]" ), dwItemID );
//		return;
//	}
//
//	//typedef					FLCooperativeContributions::ItemCountMap	Map;
//	if( m_mapRewardItemProp.insert( ItemCountMap::value_type( dwItemID, dwItemCount ) ).second == false )
//	{
//		FLERROR_LOG( PROGRAM_NAME, _T( "[ DUPLICATE ITEM INDEX(%u) ]" ), dwItemID );
//		return;
//	}
//
//	pushTextID( dwTextID );
//}
//
//BOOL FLItemRewards_User::TryReward( const DWORD dwContributionID, FLWSUser & User )
//{
//	const FLContribution * pProp		= COOPERATIVE_CONTRIBUTIONS().FindContribution( dwContributionID );
//	if( pProp == NULL )
//	{
//		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID dwContributionID(%u) ]" ), dwContributionID );
//		return FALSE;
//	}
//
//	if( m_mapRewardItemProp.size() == 0 )
//	{
//		return FALSE;
//	}
//
//	// header
//	BEFORESENDDUAL( ar, PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_REWARD_ITEM_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
//	T_W2DB_PACKET_COOPERATIVE_CONTRIBUTIONS_REWARD_ITEM_REQ reward;
//	reward.idPlayer			= User.m_idPlayer;
//	reward.dwContributionID	= dwContributionID;
//	reward.tmExpire			= pProp->tmEndDate;
//	FLStrcpy( reward.szRewardKind, _countof( reward.szRewardKind ), "ITEM" ); 
//	FLStrcpy( reward.szMailTitle, _countof( reward.szMailTitle ), pProp->szMailTitle ); 
//	FLStrcpy( reward.szMailText, _countof( reward.szMailText ), pProp->szMailText ); 
//	reward.dwRewardCount	= m_mapRewardItemProp.size();
//
//	ar.Write( & reward, sizeof( reward ) );
//
//	// 시퀀스 data
//	//typedef					FLCooperativeContributions::ItemCountMap	Map;
//	for( ItemCountMap::iterator it = m_mapRewardItemProp.begin(); it != m_mapRewardItemProp.end(); ++it )
//	{
//		const DWORD dwItemID	= it->first;
//		const DWORD dwItemCount = it->second;
//
//		ar << dwItemID << dwItemCount;
//	}
//
//	SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );	
//
//	return TRUE;
//}
//	



//--------------------------------------------------------------------------------------------------------------------------------//

//void FLGlobalRewardAlarm_ExpFactor::onReward()
//{
//	for( std::map<u_long, CMover*>::const_iterator itUser = prj.m_idPlayerToUserPtr.begin(); itUser != prj.m_idPlayerToUserPtr.end(); ++itUser )
//	{
//		FLWSUser * pUser	= static_cast< FLWSUser * >( itUser->second );
//		if( pUser == NULL )
//			continue;
//
//		onRewardNotify( *pUser );
//	}
//}

void FLGlobalRewardAlarm_ExpFactor::onExpired()
{
	////////////////////////////////////////////////////////////////////////////////
	//neodeath : 기획팀에서 버프 아이콘이 없어도 된다고 하여 뺌 (2011-02-17)
	//for( std::map<u_long, CMover*>::const_iterator itUser = prj.m_idPlayerToUserPtr.begin(); itUser != prj.m_idPlayerToUserPtr.end(); ++itUser )
	//{
	//	FLWSUser * pUser	= static_cast< FLWSUser * >( itUser->second );
	//	if( pUser == NULL )
	//		continue;

	//	pUser->AddCooperativeContributions_SetGlobalBuffReward( REWARD_EXP_RATE );
	//}
	////////////////////////////////////////////////////////////////////////////////
}

//void FLGlobalRewardAlarm_ExpFactor::onRewardNotify( FLWSUser & User )
//{
//	////////////////////////////////////////////////////////////////////////////////
//	//neodeath : 기획팀에서 버프 아이콘이 없어도 된다고 하여 뺌 (2011-02-17)
//	//User.AddCooperativeContributions_SetGlobalBuffReward( REWARD_EXP_RATE );
//	////////////////////////////////////////////////////////////////////////////////
//}

void FLGlobalRewardAlarm_ExpFactor::SerializeOut_Info( CAr & ar, DWORD & dwTextID, DWORD & t )
{
	ar << REWARD_EXP_RATE;
	float fValue			= ( float )( t / REWARD_PRECISION );
	FLCooperativeContributions_Common::SerializeInfo_GlobalExpFactorBuffReward( ar, dwTextID, fValue );
}

void FLGlobalRewardAlarm_ExpFactor::SerializeOut_Rewarded( CAr & ar )
{
	float fValue			= ( float )( m_value / REWARD_PRECISION );
	FLCooperativeContributions_Common::SerializeValue_GlobalExpFactorBuffReward( ar, fValue );
}

FLGlobalRewardAlarm_ExpFactor & FLGlobalRewardAlarm_ExpFactor::GetInstance()
{
	static FLGlobalRewardAlarm_ExpFactor inst;
	return inst;
}
//void FLGlobalRewardAlarm_ExpFactor::SerializeType( CAr & ar )
//{
//	ar << REWARD_EXP_RATE;
//}
//--------------------------------------------------------------------------------------------------------------------------------//


//void FLGlobalRewardAlarm_DropRate::onReward()
//{
//	for( std::map<u_long, CMover*>::const_iterator itUser = prj.m_idPlayerToUserPtr.begin(); itUser != prj.m_idPlayerToUserPtr.end(); ++itUser )
//	{
//		FLWSUser * pUser	= static_cast< FLWSUser * >( itUser->second );
//		if( pUser == NULL )
//			continue;
//
//		onRewardNotify( *pUser );
//	}
//}

void FLGlobalRewardAlarm_DropRate::onExpired()
{
	////////////////////////////////////////////////////////////////////////////////
	////neodeath : 기획팀에서 버프 아이콘이 없어도 된다고 하여 뺌 (2011-02-17)
	//for( std::map<u_long, CMover*>::const_iterator itUser = prj.m_idPlayerToUserPtr.begin(); itUser != prj.m_idPlayerToUserPtr.end(); ++itUser )
	//{
	//	FLWSUser * pUser	= static_cast< FLWSUser * >( itUser->second );
	//	if( pUser == NULL )
	//		continue;

	//	pUser->AddCooperativeContributions_SetGlobalBuffReward( REWARD_DROP_RATE );
	//}
	////////////////////////////////////////////////////////////////////////////////
}

//void FLGlobalRewardAlarm_DropRate::onRewardNotify( FLWSUser & User )
//{
//	////////////////////////////////////////////////////////////////////////////////
//	////neodeath : 기획팀에서 버프 아이콘이 없어도 된다고 하여 뺌 (2011-02-17)
//	//User.AddCooperativeContributions_SetGlobalBuffReward( REWARD_DROP_RATE );
//	////////////////////////////////////////////////////////////////////////////////
//}

//void FLGlobalRewardAlarm_DropRate::SerializeType( CAr & ar )
//{
//	ar << REWARD_DROP_RATE;
//}

void FLGlobalRewardAlarm_DropRate::SerializeOut_Info( CAr & ar, DWORD & dwTextID, DWORD & t )
{
	ar << REWARD_DROP_RATE;// << t;
	float fValue			= ( float )( t / REWARD_PRECISION );
	FLCooperativeContributions_Common::SerializeInfo_GlobalDropRateBuffReward( ar, dwTextID, fValue );
}

void FLGlobalRewardAlarm_DropRate::SerializeOut_Rewarded( CAr & ar )
{
	float fValue			= ( float )( m_value / REWARD_PRECISION );
	FLCooperativeContributions_Common::SerializeValue_GlobalDropRateBuffReward( ar, fValue );
}

FLGlobalRewardAlarm_DropRate & FLGlobalRewardAlarm_DropRate::GetInstance()
{
	static FLGlobalRewardAlarm_DropRate inst;
	return inst;
}
//--------------------------------------------------------------------------------------------------------------------------------//


//void FLGlobalRewardAlarm_ShopBuyFactor::onReward()
//{
//	for( std::map<u_long, CMover*>::const_iterator itUser = prj.m_idPlayerToUserPtr.begin(); itUser != prj.m_idPlayerToUserPtr.end(); ++itUser )
//	{
//		FLWSUser * pUser	= static_cast< FLWSUser * >( itUser->second );
//		if( pUser == NULL )
//			continue;
//
//	//	onRewardNotify( *pUser );
//	}
//}

void FLGlobalRewardAlarm_ShopBuyFactor::onExpired()
{
	////////////////////////////////////////////////////////////////////////////////
	////neodeath : 기획팀에서 버프 아이콘이 없어도 된다고 하여 뺌 (2011-02-17)
	//for( std::map<u_long, CMover*>::const_iterator itUser = prj.m_idPlayerToUserPtr.begin(); itUser != prj.m_idPlayerToUserPtr.end(); ++itUser )
	//{
	//	FLWSUser * pUser	= static_cast< FLWSUser * >( itUser->second );
	//	if( pUser == NULL )
	//		continue;

	//	pUser->AddCooperativeContributions_SetGlobalBuffReward( REWARD_SHOP_BUY_FACTOR );
	//}
	////////////////////////////////////////////////////////////////////////////////
}

//void FLGlobalRewardAlarm_ShopBuyFactor::onRewardNotify( FLWSUser & User )
//{
//	////////////////////////////////////////////////////////////////////////////////
//	////neodeath : 기획팀에서 버프 아이콘이 없어도 된다고 하여 뺌 (2011-02-17)
//	//User.AddCooperativeContributions_SetGlobalBuffReward( REWARD_SHOP_BUY_FACTOR );
//	////////////////////////////////////////////////////////////////////////////////	
//}

//void FLGlobalRewardAlarm_ShopBuyFactor::SerializeType( CAr & ar )
//{
//	ar << REWARD_SHOP_BUY_FACTOR;
//}

void FLGlobalRewardAlarm_ShopBuyFactor::SerializeOut_Info( CAr & ar, DWORD & dwTextID, DWORD & t )
{
	ar << REWARD_SHOP_BUY_FACTOR;// << t;
	float fValue			= ( float )( t / REWARD_PRECISION );
	FLCooperativeContributions_Common::SerializeInfo_GlobalShopBuyFactorBuffReward( ar, dwTextID, fValue );
}

void FLGlobalRewardAlarm_ShopBuyFactor::SerializeOut_Rewarded( CAr & ar )
{
	float fValue			= ( float )( m_value / REWARD_PRECISION );
	FLCooperativeContributions_Common::SerializeValue_GlobalShopBuyFactorBuffReward( ar, fValue );
}

FLGlobalRewardAlarm_ShopBuyFactor & FLGlobalRewardAlarm_ShopBuyFactor::GetInstance()
{
	static FLGlobalRewardAlarm_ShopBuyFactor inst;
	return inst;
}
//--------------------------------------------------------------------------------------------------------------------------------//


//void FLGlobalRewardAlarm_ShopSellFactor::onReward()
//{
//	for( std::map<u_long, CMover*>::const_iterator itUser = prj.m_idPlayerToUserPtr.begin(); itUser != prj.m_idPlayerToUserPtr.end(); ++itUser )
//	{
//		FLWSUser * pUser	= static_cast< FLWSUser * >( itUser->second );
//		if( pUser == NULL )
//			continue;
//
//	//	onRewardNotify( *pUser );
//	}
//}

void FLGlobalRewardAlarm_ShopSellFactor::onExpired()
{
	////////////////////////////////////////////////////////////////////////////////
	////neodeath : 기획팀에서 버프 아이콘이 없어도 된다고 하여 뺌 (2011-02-17)
	//for( std::map<u_long, CMover*>::const_iterator itUser = prj.m_idPlayerToUserPtr.begin(); itUser != prj.m_idPlayerToUserPtr.end(); ++itUser )
	//{
	//	FLWSUser * pUser	= static_cast< FLWSUser * >( itUser->second );
	//	if( pUser == NULL )
	//		continue;

	//	pUser->AddCooperativeContributions_SetGlobalBuffReward( REWARD_SHOP_SELL_FACTOR );
	//}
	////////////////////////////////////////////////////////////////////////////////
}

//void FLGlobalRewardAlarm_ShopSellFactor::onRewardNotify( FLWSUser & User )
//{
//	////////////////////////////////////////////////////////////////////////////////
//	////neodeath : 기획팀에서 버프 아이콘이 없어도 된다고 하여 뺌 (2011-02-17)
//	//User.AddCooperativeContributions_SetGlobalBuffReward( REWARD_SHOP_SELL_FACTOR );
//	////////////////////////////////////////////////////////////////////////////////	
//}

//void FLGlobalRewardAlarm_ShopSellFactor::SerializeType( CAr & ar )
//{
//	ar << REWARD_SHOP_SELL_FACTOR;
//}

void FLGlobalRewardAlarm_ShopSellFactor::SerializeOut_Info( CAr & ar, DWORD & dwTextID, DWORD & t )
{
	ar << REWARD_SHOP_SELL_FACTOR;// << t;
	float fValue			= ( float )( t / REWARD_PRECISION );
	FLCooperativeContributions_Common::SerializeInfo_GlobalShopSellFactorBuffReward( ar, dwTextID, fValue );
}

void FLGlobalRewardAlarm_ShopSellFactor::SerializeOut_Rewarded( CAr & ar )
{
	float fValue			= ( float )( m_value / REWARD_PRECISION );
	FLCooperativeContributions_Common::SerializeValue_GlobalShopSellFactorBuffReward( ar, fValue );
}

FLGlobalRewardAlarm_ShopSellFactor & FLGlobalRewardAlarm_ShopSellFactor::GetInstance()
{
	static FLGlobalRewardAlarm_ShopSellFactor inst;
	return inst;
}
//--------------------------------------------------------------------------------------------------------------------------------//


//void FLGlobalRewardAlarm_DefencePower::onReward()
//{
//	for( std::map<u_long, CMover*>::const_iterator itUser = prj.m_idPlayerToUserPtr.begin(); itUser != prj.m_idPlayerToUserPtr.end(); ++itUser )
//	{
//		FLWSUser * pUser	= static_cast< FLWSUser * >( itUser->second );
//		if( pUser == NULL )
//			continue;
//
//	//	onRewardNotify( *pUser );
//	}
//}

void FLGlobalRewardAlarm_DefencePower::onExpired()
{
	////////////////////////////////////////////////////////////////////////////////
	////neodeath : 기획팀에서 버프 아이콘이 없어도 된다고 하여 뺌 (2011-02-17)
	//for( std::map<u_long, CMover*>::const_iterator itUser = prj.m_idPlayerToUserPtr.begin(); itUser != prj.m_idPlayerToUserPtr.end(); ++itUser )
	//{
	//	FLWSUser * pUser	= static_cast< FLWSUser * >( itUser->second );
	//	if( pUser == NULL )
	//		continue;

	//	pUser->AddCooperativeContributions_SetGlobalBuffReward( REWARD_DEFENCE_POWER );
	//}
	////////////////////////////////////////////////////////////////////////////////	
}

//void FLGlobalRewardAlarm_DefencePower::onRewardNotify( FLWSUser & User )
//{
//	////////////////////////////////////////////////////////////////////////////////
//	////neodeath : 기획팀에서 버프 아이콘이 없어도 된다고 하여 뺌 (2011-02-17)
//	//User.AddCooperativeContributions_SetGlobalBuffReward( REWARD_DEFENCE_POWER );
//	////////////////////////////////////////////////////////////////////////////////
//}

//void FLGlobalRewardAlarm_DefencePower::SerializeType( CAr & ar )
//{
//	ar << REWARD_DEFENCE_POWER;
//}

void FLGlobalRewardAlarm_DefencePower::SerializeOut_Info( CAr & ar, DWORD & dwTextID, int & t )
{
	ar << REWARD_DEFENCE_POWER;// << t;
	FLCooperativeContributions_Common::SerializeInfo_GlobalDefencePowerBuffReward( ar, dwTextID, t );
}

void FLGlobalRewardAlarm_DefencePower::SerializeOut_Rewarded( CAr & ar )
{
	FLCooperativeContributions_Common::SerializeValue_GlobalDefencePowerBuffReward( ar, m_value );
}

FLGlobalRewardAlarm_DefencePower & FLGlobalRewardAlarm_DefencePower::GetInstance()
{
	static FLGlobalRewardAlarm_DefencePower inst;
	return inst;
}
//--------------------------------------------------------------------------------------------------------------------------------//


//void FLGlobalRewardAlarm_AttackPower::onReward()
//{
//	for( std::map<u_long, CMover*>::const_iterator itUser = prj.m_idPlayerToUserPtr.begin(); itUser != prj.m_idPlayerToUserPtr.end(); ++itUser )
//	{
//		FLWSUser * pUser	= static_cast< FLWSUser * >( itUser->second );
//		if( pUser == NULL )
//			continue;
//
//	//	onRewardNotify( *pUser );
//	}
//}

void FLGlobalRewardAlarm_AttackPower::onExpired()
{
	////////////////////////////////////////////////////////////////////////////////
	////neodeath : 기획팀에서 버프 아이콘이 없어도 된다고 하여 뺌 (2011-02-17)
	//for( std::map<u_long, CMover*>::const_iterator itUser = prj.m_idPlayerToUserPtr.begin(); itUser != prj.m_idPlayerToUserPtr.end(); ++itUser )
	//{
	//	FLWSUser * pUser	= static_cast< FLWSUser * >( itUser->second );
	//	if( pUser == NULL )
	//		continue;

	//	pUser->AddCooperativeContributions_SetGlobalBuffReward( REWARD_ATTACK_POWER );
	//}
	////////////////////////////////////////////////////////////////////////////////
}

//void FLGlobalRewardAlarm_AttackPower::onRewardNotify( FLWSUser & User )
//{
//	////////////////////////////////////////////////////////////////////////////////
//	////neodeath : 기획팀에서 버프 아이콘이 없어도 된다고 하여 뺌 (2011-02-17)
//	//User.AddCooperativeContributions_SetGlobalBuffReward( REWARD_ATTACK_POWER );
//	////////////////////////////////////////////////////////////////////////////////
//}

//void FLGlobalRewardAlarm_AttackPower::SerializeType( CAr & ar )
//{
//	ar << REWARD_ATTACK_POWER;
//}

void FLGlobalRewardAlarm_AttackPower::SerializeOut_Info( CAr & ar, DWORD & dwTextID, int & t )
{
	ar << REWARD_ATTACK_POWER;// << t;
	FLCooperativeContributions_Common::SerializeInfo_GlobalAttackPowerBuffReward( ar, dwTextID, t );
}

void FLGlobalRewardAlarm_AttackPower::SerializeOut_Rewarded( CAr & ar )
{
	FLCooperativeContributions_Common::SerializeValue_GlobalAttackPowerBuffReward( ar, m_value );
}

FLGlobalRewardAlarm_AttackPower & FLGlobalRewardAlarm_AttackPower::GetInstance()
{
	static FLGlobalRewardAlarm_AttackPower inst;
	return inst;
}

} //nsCooperativeContributions