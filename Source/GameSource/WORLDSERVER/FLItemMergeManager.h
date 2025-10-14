
#ifndef __FLITEMMERGEMANAGER_H__
#define __FLITEMMERGEMANAGER_H__

#include "FLItemMergeRandomOption.h"


class FLItemMergeBase;

class FLItemMergeManager
{
	public:
		static FLItemMergeManager* GetInstance();

		FLItemMergeManager();
		~FLItemMergeManager();


		bool	LoadScript();

		void	OnItemMerge( FLWSUser* pUser, const PT_PACKET_ITEM_MERGE ptItemMerge );

	private:
		FLItemMergeBase*	GetItemMergeType( PT_PACKET_ITEM_MERGE ptItemMerge );


	private:
		FLItemMergeRandomOption		m_kItemMergeRandomOption;

};


#define		g_xItemMergeManager		FLItemMergeManager::GetInstance()


#endif // __FLITEMMERGEMANAGER_H__

