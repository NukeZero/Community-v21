#ifndef __FLITEMMERGEBASE_H__
#define __FLITEMMERGEBASE_H__


const DWORD		MAX_MERGE_PROB		= 10000;


class FLItemMergeBase
{
	public:
		FLItemMergeBase();
		virtual ~FLItemMergeBase();

		virtual void	OnItemMerge( FLWSUser* pUser, PT_PACKET_ITEM_MERGE ptItemMerge ) = 0;


	protected:
		typedef struct _T_ITEM_DATA
		{
			DWORD	dwParts;
			DWORD	dwItemKind1;
			DWORD	dwItemKind2;
			DWORD	dwItemKind3;

		} T_ITEM_DATA, *PT_ITEM_DATA;

		typedef		std::vector<T_ITEM_DATA>			VectorItemData;
		typedef		VectorItemData::iterator	ItemDataItr;


		typedef struct _T_ITEM_MERGE_DATA
		{
			DWORD				dwMergeType;
			DWORD				dwMergeCount;
			DWORD				dwMergeProb;
			VectorItemData		vectorItemData;

		} T_ITEM_MERGE_DATA, *PT_ITEM_MERGE_DATA;

		typedef		list<T_ITEM_MERGE_DATA>			ListItemMergeData;
		typedef		ListItemMergeData::iterator		ItemMergeDataItr;

};


#endif // __FLITEMMERGEBASE_H__

