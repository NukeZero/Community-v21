#pragma once 

//베트남

//길드 퀘스트
//파티 퀘스트

//이벤트 제너릭 (레벨 10이하 차이)
	//루아

//퀘스트 드랍

//파티 참여 여부(스킬)

//버프

//이벤트 아이템 한번 떨구기

//NPC가 주은 아이템 떨구기


enum {	DROP_PROB_MAX = 10000, };


// DROP ITEM COMMON
class FLDropItem_Data
{
public:
	FLDropItem_Data();
	virtual ~FLDropItem_Data();

public:
	DWORD					m_dwProb;
	DWORD					m_dwItemID;
	int						m_nItemCountMin;
	int						m_nItemCountMax;
	int						m_nQuestID;
};

class FLDropSet
{
public:
	enum E_DROP_SET_KIND			{	
		KIND_NONE	= 0,
		KIND_WEAPON,
		KIND_ARMOR,
		KIND_ACCESSORY,
		KIND_MATERIAL,
		KIND_ETC, 
		KIND_QUEST, 
		KIND_EVENT,
		KIND_RARE,
//		KIND_UNIQUE, 
	};

public:
	FLDropSet();
	virtual ~FLDropSet();

	void					DropItem( CMover & kMonster, FLWSUser & kUser );
	static FLDropSet::E_DROP_SET_KIND	StringToKindEnum( std::string & strSetKind );
	BOOL					Check_Item( const DWORD dwItemID, const int nQuestID );
	BOOL					Check_Prob() const;

public:
	typedef					std::vector< FLDropItem_Data >	DropItemVec;

	DropItemVec				m_vecDropItem;

	DWORD					m_dwMainProb;
	//BOOL					m_bQuestSet;
	E_DROP_SET_KIND			m_eSetKind;
	std::string				m_strMainProbName;
};



class FLDoDrop
{
//public:
//	static void				Drop( CMover & kMonster, FLWSUser & kAttacker );

//private:
public:
	//static float			GetItemDropRateFactor( CMover & kMonster, FLWSUser & kAttacker, const BOOL bQuest );
	static float			GetItemDropRateFactor( CMover * pMonster, FLWSUser & kAttacker, const FLDropSet::E_DROP_SET_KIND eSetKind );

	static int				GetAdditinalLoopCount( FLWSUser & kAttacker );
	//static float			GetItemDropRateFactor( FLWSUser & kAttacker, const BOOL bQuest );

	static float			GetGoldDropRateFactor( CMover & kMonster, FLWSUser & kAttacker );
	static void				DropItem( CMover & kMonster, FLWSUser & kAttacker, const DWORD dwOwnerPlayerID, const DWORD dwItemID, const int nItemCount );
	static void				DropGold( CMover & kMonster, FLWSUser & kAttacker, const DWORD dwOwnerPlayerID, const int nGold );
};




