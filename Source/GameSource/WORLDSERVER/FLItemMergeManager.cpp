
#include "StdAfx.h"
#include "FLItemMergeManager.h"


FLItemMergeManager::FLItemMergeManager()
{

}

FLItemMergeManager::~FLItemMergeManager()
{

}

FLItemMergeManager* FLItemMergeManager::GetInstance()
{
	static FLItemMergeManager xItemMergeManager;

	return & xItemMergeManager;
}

bool	FLItemMergeManager::LoadScript()
{
	return m_kItemMergeRandomOption.LoadScript();
}

void	FLItemMergeManager::OnItemMerge( FLWSUser* pUser, const PT_PACKET_ITEM_MERGE ptItemMerge )
{
	FLItemMergeBase* pItemMergeBase = GetItemMergeType( ptItemMerge );

	if( pItemMergeBase != NULL )
	{
		pItemMergeBase->OnItemMerge( pUser, ptItemMerge );
	}
}

FLItemMergeBase* FLItemMergeManager::GetItemMergeType( PT_PACKET_ITEM_MERGE ptItemMerge )
{
	if( ptItemMerge != NULL )
	{
		if( ptItemMerge->dwMergeType == MERGE_RANDOMOPTION )
		{
			return &m_kItemMergeRandomOption;
		}
	}

	return NULL;
}

