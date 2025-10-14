#pragma once

class FLCooperativeContributions_Common
{
public:
	static void				SerializeInfo_GlobalItemEndCondition( CAr & ar, DWORD & dwItemID, DWORD & dwItemMax, size_t & nContributionUnit, DWORD & dwItemContributed );
	static void				SerializeInfo_GlobalGoldEndCondition( CAr & ar, int & nGoldMax, size_t & nContributionUnit, int & nContributedGold );

	static void				SerializeInfo_GlobalItemReward( CAr & ar, DWORD & dwItemID, short & shItemCount );

	static void				SerializeInfo_GlobalExpFactorBuffReward( CAr & ar, DWORD & dwTextID, float & fRate );
	static void				SerializeInfo_GlobalDropRateBuffReward( CAr & ar, DWORD & dwTextID, float & fRate );
	static void				SerializeInfo_GlobalShopBuyFactorBuffReward( CAr & ar, DWORD & dwTextID, float & fFactor );
	static void				SerializeInfo_GlobalShopSellFactorBuffReward( CAr & ar, DWORD & dwTextID, float & fFactor );
	static void				SerializeInfo_GlobalDefencePowerBuffReward( CAr & ar, DWORD & dwTextID, int & nValue );
	static void				SerializeInfo_GlobalAttackPowerBuffReward( CAr & ar, DWORD & dwTextID, int & nValue );

	static void				SerializeValue_GlobalExpFactorBuffReward( CAr & ar, float & fRate );
	static void				SerializeValue_GlobalDropRateBuffReward( CAr & ar, float & fRate );
	static void				SerializeValue_GlobalShopBuyFactorBuffReward( CAr & ar, float & fFactor );
	static void				SerializeValue_GlobalShopSellFactorBuffReward( CAr & ar, float & fFactor );
	static void				SerializeValue_GlobalDefencePowerBuffReward( CAr & ar, int & nValue );
	static void				SerializeValue_GlobalAttackPowerBuffReward( CAr & ar, int & nValue );
};


