#pragma once


#if defined __WORLDSERVER && defined __ENCHANT_BARUNA16

class FLNPCSummoner
{
public:
	struct E_OPERATION {	enum E {
		NO_OPERATION,
		INACTIVE,
		ACTIVE,
	};						};

public:
	FLNPCSummoner();
	virtual ~FLNPCSummoner();

	//한번 실행하면 소환 다시실행하면 소환 해제 
	E_OPERATION::E			ExcuteCallNPCByItem( FLWSUser & Owner, FLItemElem & ItemElem/*, const DWORD dwMoverIndex*//*, const TCHAR * szCharacterName*/ );
	OBJID					GetNPCOBJID() const;
	DWORD					GetNPCItemOBJID() const;

	E_OPERATION::E			TryInactivateNPCByRange( FLWSUser & Owner, const float fRange );				// ( return TRUE면 소환 : FALSE면 해제 )

	E_OPERATION::E			InactivateNPC( FLWSUser & Owner );

protected:
	E_OPERATION::E			activateNPC( FLWSUser & Owner, FLItemElem & ItemElem/*, const DWORD dwMoverIndex*//*, const TCHAR * szCharacterName*/ );
	
private:
	DWORD					m_dwNPCItemOBJID;					//@ 좀더 제너릭하게 ItemSpec에 단일 실행 && 기존 실행 무시   옵션을 넣는다.
	OBJID					m_CalledNPCOBJID;
};


//#include <boost/bimap.hpp>

// ItemObjID, 
// NPCObjID -> ItemObjID
// ItemID,	( 아이템 실행 횟수 & 쿨타임 관리자 )

//class FLCalledNPCMng
//{
//public:
//	struct E_OPERATION {	enum E {
//		NO_OPERATION,
//		INACTIVE,
//		ACTIVE,
//	};						};
//
//	enum					{	ITEM_CALL_NPC_MAX	= 3 };
//
//public:
//	FLCalledNPCMng();
//	virtual ~FLCalledNPCMng();
//	
//	BOOL					OnUseSkill( FLWSUser & Owner );
//	BOOL					OnUseItem( FLWSUser & Owner, FLItemElem & ItemElem );
//
//	void					TryInactivateByRangeInfo( FLWSUser & Owner );
//	BOOL					InactivateNPC( FLWSUser & Owner, FLItemElem & ItemElem );
//
//private:
//	BOOL					activateNPC( FLWSUser & Owner, FLItemElem & ItemElem );
//
//private:
//	//typedef					boost::bimap< DWORD/*ItemID */, OBJID /*NPC ObjID*/ >	ItemID_NPCBimap;
//	//typedef					boost::bimap< DWORD/*ItemObjID */, DWORD /*ItemID*/ >	ItemObjID_ItemIDBimap;
//	//typedef					std::map< OBJID/*NPC ObjID*/, u_long/*Range*/ >				NPC_RangeMap;
//
//	//ItemID_NPCBimap			bimapCalledNPC;
//	//ItemObjID_ItemIDBimap	bimapUsedItem;
//	//NPC_RangeMap			mapRangeInfo;
//};
//
//
//



#endif //__WORLDSERVER && defined __ENCHANT_BARUNA16