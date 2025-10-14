#include "stdafx.h"

#include "FLCooperativeContributions_Common.h"

void FLCooperativeContributions_Common::SerializeInfo_GlobalItemEndCondition( CAr & ar, DWORD & dwItemID, DWORD & dwItemMax, size_t & nContributionUnit, DWORD & dwItemContributed )
{
	if( ar.IsStoring() == TRUE )
	{
		ar << dwItemID << dwItemMax << nContributionUnit << dwItemContributed;
	}
	else
	{
		ar >> dwItemID >> dwItemMax >> nContributionUnit >> dwItemContributed;
	}
}

void FLCooperativeContributions_Common::SerializeInfo_GlobalGoldEndCondition( CAr & ar, int & nGoldMax, size_t & nContributionUnit, int & nContributedGold )
{
	if( ar.IsStoring() == TRUE )
	{
		ar << nGoldMax << nContributionUnit << nContributedGold;
	}
	else
	{
		ar >> nGoldMax >> nContributionUnit >> nContributedGold;
	}
}

void FLCooperativeContributions_Common::SerializeInfo_GlobalItemReward( CAr & ar, DWORD & dwItemID, short & shItemCount )
{
	if( ar.IsStoring() == TRUE )
	{
		ar << dwItemID << shItemCount;
	}
	else
	{
		ar >> dwItemID >> shItemCount; 
	}
}

void FLCooperativeContributions_Common::SerializeInfo_GlobalExpFactorBuffReward( CAr & ar, DWORD & dwTextID, float & fRate )
{
	if( ar.IsStoring() == TRUE )
	{
		ar << dwTextID << fRate;
	}
	else
	{
		ar >> dwTextID >> fRate;
	}
}

void FLCooperativeContributions_Common::SerializeInfo_GlobalDropRateBuffReward( CAr & ar, DWORD & dwTextID, float & fRate )
{
	if( ar.IsStoring() == TRUE )
	{
		ar << dwTextID << fRate;
	}
	else
	{
		ar >> dwTextID >> fRate;
	}
}

void FLCooperativeContributions_Common::SerializeInfo_GlobalShopBuyFactorBuffReward( CAr & ar, DWORD & dwTextID, float & fFactor )
{
	if( ar.IsStoring() == TRUE )
	{
		ar << dwTextID << fFactor;
	}
	else
	{
		ar >> dwTextID >> fFactor;
	}
}

void FLCooperativeContributions_Common::SerializeInfo_GlobalShopSellFactorBuffReward( CAr & ar, DWORD & dwTextID, float & fFactor )
{
	if( ar.IsStoring() == TRUE )
	{
		ar << dwTextID << fFactor;
	}
	else
	{
		ar >> dwTextID >> fFactor;
	}
}

void FLCooperativeContributions_Common::SerializeInfo_GlobalDefencePowerBuffReward( CAr & ar, DWORD & dwTextID, int & nValue )
{
	if( ar.IsStoring() == TRUE )
	{
		ar << dwTextID << nValue;
	}
	else
	{
		ar >> dwTextID >> nValue;
	}
}

void FLCooperativeContributions_Common::SerializeInfo_GlobalAttackPowerBuffReward( CAr & ar, DWORD & dwTextID, int & nValue )
{
	if( ar.IsStoring() == TRUE )
	{
		ar << dwTextID << nValue;
	}
	else
	{
		ar >> dwTextID >> nValue;
	}
}


void FLCooperativeContributions_Common::SerializeValue_GlobalExpFactorBuffReward( CAr & ar, float & fRate )
{
	if( ar.IsStoring() == TRUE )
	{
		ar << fRate;
	}
	else
	{
		ar >> fRate;
	}
}

void FLCooperativeContributions_Common::SerializeValue_GlobalDropRateBuffReward( CAr & ar, float & fRate )
{
	if( ar.IsStoring() == TRUE )
	{
		ar << fRate;
	}
	else
	{
		ar >> fRate;
	}
}

void FLCooperativeContributions_Common::SerializeValue_GlobalShopBuyFactorBuffReward( CAr & ar, float & fFactor )
{
	if( ar.IsStoring() == TRUE )
	{
		ar << fFactor;
	}
	else
	{
		ar >> fFactor;
	}
}

void FLCooperativeContributions_Common::SerializeValue_GlobalShopSellFactorBuffReward( CAr & ar, float & fFactor )
{
	if( ar.IsStoring() == TRUE )
	{
		ar << fFactor;
	}
	else
	{
		ar >> fFactor;
	}
}

void FLCooperativeContributions_Common::SerializeValue_GlobalDefencePowerBuffReward( CAr & ar, int & nValue )
{
	if( ar.IsStoring() == TRUE )
	{
		ar << nValue;
	}
	else
	{
		ar >> nValue;
	}
}

void FLCooperativeContributions_Common::SerializeValue_GlobalAttackPowerBuffReward( CAr & ar, int & nValue )
{
	if( ar.IsStoring() == TRUE )
	{
		ar << nValue;
	}
	else
	{
		ar >> nValue;
	}
}
