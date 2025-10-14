#include "StdAfx.h"

#if defined __WORLDSERVER && defined __ENCHANT_BARUNA16

#include "NPCSummoner.h"

#include "../WorldServer/User.h"
#include "../WorldServer/ItemUpgrade.h"



FLNPCSummoner::FLNPCSummoner() 
: m_dwNPCItemOBJID( NULL_ID )
,m_CalledNPCOBJID( NULL_ID )
{
}


FLNPCSummoner::~FLNPCSummoner()
{
}


FLNPCSummoner::E_OPERATION::E
FLNPCSummoner::ExcuteCallNPCByItem( FLWSUser & Owner, FLItemElem & ItemElem/*,*/ /*const DWORD dwMoverIndex,*/ /*const TCHAR * szCharacterName*/ )
{	
	if( Owner.IsPlayer() == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ Owner is not player : name(%s) ]"), Owner.GetName() );
		return E_OPERATION::NO_OPERATION;
	}

	if( m_CalledNPCOBJID == NULL_ID )
	{
		return activateNPC( Owner, ItemElem/*,*/ /*dwMoverIndex,*//* szCharacterName*/ );
	}
	else
	{
		return InactivateNPC( Owner );
	}
}

OBJID
FLNPCSummoner::GetNPCOBJID() const
{
	return m_CalledNPCOBJID;
}

DWORD							
FLNPCSummoner::GetNPCItemOBJID() const
{
	return m_dwNPCItemOBJID;
}


FLNPCSummoner::E_OPERATION::E
FLNPCSummoner::TryInactivateNPCByRange( FLWSUser & Owner, const float fRange )
{
	if( Owner.IsPlayer() == FALSE )
	{
		//FLERROR_LOG( PROGRAM_NAME, _T( "[ Owner is not player : name(%s) ]" ), Owner.GetName() );
		return E_OPERATION::NO_OPERATION;
	}

	if( m_CalledNPCOBJID == NULL_ID )
	{
		return E_OPERATION::NO_OPERATION;
	}

	CMover * pNPC				= prj.GetMover( m_CalledNPCOBJID );
	if( IsValidObj( &Owner ) == FALSE || IsValidObj( pNPC ) == FALSE  )
	{
		Owner.AddDefinedText( TID_GAME_CALLED_NPC_NOW_USE );
		return E_OPERATION::NO_OPERATION;
	}

	const D3DXVECTOR3 vDistance	= Owner.GetPos() - pNPC->GetPos();
	if( ::D3DXVec3LengthSq( &vDistance ) > fRange 
		|| Owner.GetWorld() == NULL 
		|| pNPC->GetWorld() == NULL 
		|| ( Owner.GetWorld()->GetID() != pNPC->GetWorld()->GetID() ) 
		)
	{
		return InactivateNPC( Owner );
//		return E_OPERATION::INACTIVE;
	}

	return E_OPERATION::NO_OPERATION;
}

FLNPCSummoner::E_OPERATION::E
FLNPCSummoner::activateNPC( FLWSUser & Owner, FLItemElem & ItemElem/*,*/ /*const DWORD dwMoverIndex,*/ /*const TCHAR * szCharacterName*/ )
{
	if( Owner.IsPlayer() == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ Owner is not player : name(%s) ]"), Owner.GetName() );
		return E_OPERATION::NO_OPERATION;
	}

	PT_ITEM_SPEC pProp	= ItemElem.GetProp();
	if( pProp == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ PropItem is NULL :  User(%u) ItemID(%d) ItemIndex(%d) ]"), Owner.GetId(), ItemElem.m_dwItemId, ItemElem.m_dwObjIndex );
		return E_OPERATION::NO_OPERATION;
	}

	if( m_CalledNPCOBJID != NULL_ID )
	{
		return E_OPERATION::NO_OPERATION;
	}

	if( Owner.GetWorld() == NULL )
	{
		return InactivateNPC( Owner );
	}

	if( Owner.IsFly() == TRUE )
	{
		Owner.AddDefinedText( TID_GAME_CANNOT_SUMMON_NPC_ON_FLYING );
		return E_OPERATION::NO_OPERATION;
	}

	CMover  * pNPC			= CreateNPC( Owner.GetId(), *Owner.GetWorld(), pProp->szTextFileName, /*szCharacterName,*/ Owner.GetPos(), ItemElem.GetProp()->dwLinkKind, 1		//1 == 호전성
	//CMover  * pNPC			= CreateNPC( Owner.GetId(), *Owner.GetWorld(), "MaFl_Snowman01", Owner.GetPos(), MI_NPC_SNOWMAN01, 1		//1 == 호전성
#ifdef __LAYER_1015
		, Owner.GetLayer() 
#endif	// __LAYER_1015
		);

	if( pNPC == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ CAN'T CREATE NPC : World(%u), szCharacter(%s), Pos(x:%d,y:%d,z:%d ) Layer(%d) ]" ), Owner.GetWorld()->GetID(), pProp->szTextFileName, Owner.GetPos().x, Owner.GetPos().y, Owner.GetPos().z
#ifdef __LAYER_1015
			, Owner.GetLayer() 
#else
			, 0
#endif	// __LAYER_1015
			);
		return E_OPERATION::NO_OPERATION;
	}

	if( ItemElem.m_pPet == NULL )				//처음 호출에 만들어줘야함.
	{
		ItemElem.m_pPet		= new CPet;
		ItemElem.m_pPet->InitNPC();
	}

	m_dwNPCItemOBJID		= ItemElem.m_dwObjId;
	m_CalledNPCOBJID		= pNPC->GetId();

	g_dpDBClient.CalluspPetLog( Owner.m_idPlayer, ItemElem.GetSerialNumber(), 0, PETLOGTYPE_CALL, ItemElem.m_pPet );

	return E_OPERATION::ACTIVE;
}

FLNPCSummoner::E_OPERATION::E
FLNPCSummoner::InactivateNPC( FLWSUser & Owner )
{
	if( Owner.IsPlayer() == FALSE )
	{
		return E_OPERATION::NO_OPERATION;
	}

	const DWORD dwNPCItemOBJID		= Owner.GetBarunaNPCSummoner().GetNPCItemOBJID();
	if( dwNPCItemOBJID == NULL_ID )
	{
		return E_OPERATION::NO_OPERATION;
	}

	FLItemElem * pNPFLItemElem		= Owner.m_Inventory.GetAtId( dwNPCItemOBJID );
	if( pNPFLItemElem == NULL || pNPFLItemElem->m_pPet == NULL )
	{
		return E_OPERATION::NO_OPERATION;
	}

	if( m_CalledNPCOBJID == NULL_ID )
	{
		return E_OPERATION::NO_OPERATION;
	}

	CMover * pNPC				= prj.GetMover( m_CalledNPCOBJID );
	if( IsValidObj( pNPC ) == FALSE  )
	{
		static_cast<FLWSUser*>( &Owner )->AddDefinedText( TID_GAME_CALLED_NPC_NOW_USE );
		return E_OPERATION::NO_OPERATION;
	}

	if( pNPC->IsNPC() == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ ReturnCalledNPC : IS NOT NPC(%u) summoner(%s) ]" ), m_CalledNPCOBJID, Owner.GetName() );
		pNPC->Delete();
		return E_OPERATION::INACTIVE;
	}

	pNPC->Delete();

	g_dpDBClient.CalluspPetLog( Owner.m_idPlayer, pNPFLItemElem->GetSerialNumber(), 0, PETLOGTYPE_RELEASE, pNPFLItemElem->m_pPet );

	m_dwNPCItemOBJID			= NULL_ID;
	m_CalledNPCOBJID			= NULL_ID;
	return E_OPERATION::INACTIVE;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------//


//
//
//
//BOOL FLCalledNPCMng::OnUseSkill( FLWSUser & Owner )
//{
//	return FALSE;
//}
//
//
//
//
//
//
//
//BOOL FLCalledNPCMng::OnUseItem( FLWSUser & Owner, FLItemElem & ItemElem )
//{
//	PT_ITEM_SPEC pSpec	= ItemElem.GetProp();
//	if( pSpec == NULL )
//	{
//		FLERROR_LOG( PROGRAM_NAME, _T( "[ NOT EXIST ITEM SPEC - User(%u) ItemIndex(%u) ObjIndex(%u) ObjID(%u) ]" ), Owner.m_idPlayer, ItemElem.m_dwItemId, ItemElem.m_dwObjIndex, ItemElem.m_dwObjId );
//		return FALSE;
//	}
//
//	//if( ITEM_CALL_NPC_MAX >= bimapCalledNPC.size() )
//	//{
//	//	Owner.AddDefinedText( TID_GAME_SUMMON_NPC_FULL );
//	//	return FALSE;
//	//}
//
//	//ItemID_NPCBimap::left_const_iterator it = bimapCalledNPC.left.find( ItemElem.m_dwItemId );
//	//if( bimapCalledNPC.end() == it )
//	//{
//	//	return activateNPC( Owner, ItemElem );
//	//}
//
//	//ItemObjID_ItemIDBimap::left_const_iterator it = bimapUsedItem.left.find( ItemElem.m_dwObjId );
//	//if( bimapUsedItem.end() == it )
//	//{
//	//	//Owner.AddDefinedText( 중복 실행이 불가능 합니다. );
//	//	return FALSE;
//	//}
//
//	//return InactivateNPC( Owner, ItemElem );
//}
//
//BOOL FLCalledNPCMng::InactivateNPC( FLWSUser & Owner, FLItemElem & ItemElem )
//{
//	//PT_ITEM_SPEC pSpec	= ItemElem->GetProp();
//	//if( pSpec == NULL )
//	//{
//	//	FLERROR_LOG( PROGRAM_NAME, "[ NOT EXIST ITEM SPEC - User(%u) ItemIndex(%u) ObjIndex(%u) ObjID(%u) ]", Owner.m_idPlayer, ItemElem.m_dwItemId, ItemElem.m_dwObjIndex, ItemElem.m_dwObjId );
//	//	return FALSE;
//	//}
//
//	//ItemID_NPCBimap::left_const_iterator it = bimapCalledNPC.left.find( ItemElem.m_dwItemId );
//	//if( bimapCalledNPC.end() == it )
//	//{
//	//	return FALSE;
//	//}
//
//	//const OBJID dwObjID	= it->second;
//	//CMover * pNPC				= prj.GetMover( dwObjID );
//	//if( IsValidObj( pNPC ) == FALSE  )
//	//{
//	//	static_cast<FLWSUser*>( &Owner )->AddDefinedText( TID_GAME_CALLED_NPC_NOW_USE );
//	//	return E_OPERATION::NO_OPERATION;
//	//}
//
//
//	//pNPC->Delete();
//
//	//bimapCalledNPC.erase( ItemElem.m_dwItemId );
//	//bimapUsedItem.erase( ItemElem.m_dwObjId );
//
//	//g_dpDBClient.CalluspPetLog( Owner.m_idPlayer, ItemElem.GetSerialNumber(), 0, PETLOGTYPE_RELEASE, ItemElem.m_pPet );
//	return TRUE;
//}
//
//BOOL FLCalledNPCMng::activateNPC( FLWSUser & Owner, FLItemElem & ItemElem )
//{
//	PT_ITEM_SPEC pSpec	= ItemElem.GetProp();
//	if( pSpec == NULL )
//	{
//		FLERROR_LOG( PROGRAM_NAME, _T( "[ NOT EXIST ITEM SPEC - User(%u) ItemIndex(%u) ObjIndex(%u) ObjID(%u) ]" ), Owner.m_idPlayer, ItemElem.m_dwItemId, ItemElem.m_dwObjIndex, ItemElem.m_dwObjId );
//		return FALSE;
//	}
////
////	CMover  * pNPC			= CreateNPC( Owner.GetId(), *Owner.GetWorld(), pSpec->szTextFileName, Owner.GetPos(), pSpec->dwLinkKind, 1		//1 == 호전성
////#ifdef __LAYER_1015
////		, Owner.GetLayer() 
////#endif	// __LAYER_1015
////		);
////
////	if( pNPC == NULL )
////	{
////		FLERROR_LOG( PROGRAM_NAME, _T( "[ CAN'T CREATE NPC : World(%u), szCharacter(%s), Pos(x:%d,y:%d,z:%d ) Layer(%d) ]" )	, Owner.GetWorld()->GetID(), pSpec->szTextFileName, Owner.GetPos().x, Owner.GetPos().y, Owner.GetPos().z
////#ifdef __LAYER_1015
////			, Owner.GetLayer() 
////#else
////			, 0
////#endif	// __LAYER_1015
////			);
////		return E_OPERATION::NO_OPERATION;
////	}
////
////	if( ItemElem.m_pPet == NULL )				//처음 호출에 만들어줘야함.
////	{
////		ItemElem.m_pPet		= new CPet;
////		ItemElem.m_pPet->InitNPC();
////	}
////
////	if( bimapCalledNPC.insert( ItemID_NPCBimap::value_type( ItemElem.m_dwItemId, pNPC->GetId() ) ).second == false 
////		|| bimapUsedItem.insert( ItemObjID_ItemIDBimap::left_value_type( ItemElem.m_dwObjId, ItemElem.m_dwItemId ) ).second == false )
////	{
////		bimapCalledNPC.erase( ItemElem.m_dwItemId );
////		bimapUsedItem.erase( ItemElem.m_dwObjId );
////		pNPC->Delete();
////		return E_OPERATION::NO_OPERATION;
////	}
////	//m_dwNPCItemOBJID		= ItemElem.m_dwObjId;
////	//m_CalledNPCOBJID		= pNPC->GetId();
////
////	g_dpDBClient.CalluspPetLog( Owner.m_idPlayer, ItemElem.GetSerialNumber(), 0, PETLOGTYPE_CALL, ItemElem.m_pPet );
////
////	return E_OPERATION::ACTIVE;
//}
//





#endif // __WORLDSERVER && defined __ENCHANT_BARUNA16