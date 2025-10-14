#pragma once

#include "FLDropItemCommon.h"
#include "FLGoldIconSet.h"


class FLMonsterDropSet_Mng
{
public:
	FLMonsterDropSet_Mng();
	virtual ~FLMonsterDropSet_Mng();

	void					DropItem( CMover & kMonster, FLWSUser & kAttacker );
	void					DropGold( CMover & kMonster, FLWSUser & kAttacker );
	
public:
	typedef					std::vector< FLDropSet > DropSetVec;

	DropSetVec				m_vecDropSet;

	DWORD					m_dwIndex;

	DWORD					m_dwGoldMainProb;
	int						m_nGoldMin;
	int						m_nGoldMax;

};


//-----------------------------------------------------------------------------------------------//

class FLMonsterDropItem_Mng
{
public:
	FLMonsterDropItem_Mng();
	virtual ~FLMonsterDropItem_Mng();

	static FLMonsterDropItem_Mng & GetInstance();

	void					Load();

	void					DropItemNGold( CMover & kMonster, FLWSUser & kUser );
	//void					DropItemNGold( CMover & kMonster, CMover & kUser );

private:
	typedef					std::map< DWORD/*MonsterID*/, FLMonsterDropSet_Mng >	DropSetMngMap;

	DropSetMngMap			m_mapDropSetMng;

	FLGoldIconSet			m_kGoldIconSet;
};

inline static  FLMonsterDropItem_Mng & MONSTERDROPITEM_MNG()		{	return FLMonsterDropItem_Mng::GetInstance();	}