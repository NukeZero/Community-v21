#pragma once

class FLEventDropItem_Mng
{
public:
// 	struct FLPeriod
// 	{
// 		FLTm				kStartDate;
// 		FLTm				kEndDate;
// 	};
// 
// 	struct DropItem_Data
// 	{
// 		DropItem_Data() : lastTick( 0 ), skipCount( 0 ), itemID( 0 ), limitQuantityPerOneDay( 0 ), randomQuantityPerOneMonster( 0 ) {}
// 
// 		DWORD				lastTick;
// 		int					skipCount;
// 
// 		/************************************************************************/
// 		DWORD				itemID;
// 		int					limitQuantityPerOneDay;
// 		int					randomQuantityPerOneMonster;
// 		std::vector<DWORD>	intervalTable;
// 		/************************************************************************/
// 	};
// 
// 	class FLEventSet
// 	{
// 	public:
// 		FLEventSet() : m_nLevelMin( 0 ), m_nLevelMax( 0 ) {}
// 
// 		BOOL				CheckEvent( CMover & kMonster );
// 		void				DropItem( CMover & kMonster, FLWSUser & kUser );
//      
// 	public:
// 		typedef				std::vector< int/*Lang*/>		LangVec;
// 		typedef				std::vector< FLPeriod >			PeriodVec;
// 		typedef				std::vector< DropItem_Data >		DropItemVec;
// 
// 		int					m_nLevelMin;
// 		int					m_nLevelMax;
// 
// 		LangVec				m_vecLang;
// 		PeriodVec			m_vecPeriod;		
// 		DropItemVec			m_vecDropItem;		
// 
// 		std::string			m_strDesc;
// 	};

public:
	FLEventDropItem_Mng();
	virtual ~FLEventDropItem_Mng();

//	void					Load();

	void					DropItem( CMover & kMonster, FLWSUser & kUser );
	//void					DropItem( CMover & kMonster, CMover & kUser );

	static FLEventDropItem_Mng &	GetInstance();

private:
// 	typedef					std::map< DWORD/* dwEventID */, FLEventSet >		EventSetMap;
// 
//     EventSetMap				m_mapEventSet;			
};


inline static  FLEventDropItem_Mng & EVENTDROPITEM_MNG()		{	return FLEventDropItem_Mng::GetInstance();	}
