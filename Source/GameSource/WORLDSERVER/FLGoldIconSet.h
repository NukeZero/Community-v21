#pragma once

class FLGoldIconSet
{
private:
	struct FLIconSet
	{
		FLIconSet( const int goldMax, const DWORD itemID );

		int					nGoldMax;		
		DWORD				dwItemID;
	};

public:
	static FLGoldIconSet &	GetInstance();

	void					Load();
	DWORD					GetIconItemID( const int nGold );


private:
	typedef					std::vector< FLIconSet >		IconSetVec;

	IconSetVec				m_vecIconSet;
};

inline static		FLGoldIconSet & GOLD_ICONSET() {		return FLGoldIconSet::GetInstance();	}