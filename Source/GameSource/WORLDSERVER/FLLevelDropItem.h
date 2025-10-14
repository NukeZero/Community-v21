#pragma once

#include "FLDropItemCommon.h"
#include <../_CommonDefine/FLCommonCount.h>


class FLLevelDropSet_Mng
{
public:
	FLLevelDropSet_Mng();
	virtual ~FLLevelDropSet_Mng();

	BOOL					CheckEvent( CMover & kMonster );
	void					DropItem( CMover & kMonster, FLWSUser & kAttacker );

public:
	typedef					std::vector< FLDropSet > DropSetVec;
	
	DropSetVec				m_vecDropSet;

	DWORD					m_dwIndex;
	
	int						m_nLevelMin;
	int						m_nLevelMax;
	FLTm					m_kStartDate;
	FLTm					m_kEndDate;
};



//-----------------------------------------------------------------------------------------------//

class FLLevelDropItem_Mng
{
	struct FLWeekFlags
	{
		FLWeekFlags()	{		::memset( arr, 0, sizeof( arr ) ); }

		bool				arr[ WEEK_MAX ];
	};


public:
	FLLevelDropItem_Mng();
	virtual ~FLLevelDropItem_Mng();

	static FLLevelDropItem_Mng &	GetInstance();

	void					Load();

	void					DropItem( CMover & kMonster, FLWSUser & kAttacker );
	//void					DropItem( CMover & kMonster, CMover & kUser );

private:
	void					load_DropLevelSpec();
	void					load_WeekFlagsSpec();

private:
	typedef					std::map< DWORD/*Event*/, FLLevelDropSet_Mng >	DropSetMngMap;
	typedef					std::map< DWORD/*Event*/, FLWeekFlags  >		WeekFlagsMap;

	DropSetMngMap			m_mapDropSetMng;
	WeekFlagsMap			m_mapWeekFlags;
};


inline static  FLLevelDropItem_Mng & LEVELDROPITEM_MNG()		{	return FLLevelDropItem_Mng::GetInstance();	}