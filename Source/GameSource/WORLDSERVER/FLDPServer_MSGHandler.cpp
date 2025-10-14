
#include "StdAfx.h"
#include "FLTeleportMapUserPointManager.h"
#include "FLEventArenaGlobal.h"
#include "FLChargeZoneTicket.h"
#include "FLTreasureChest.h"
#include "FLCharacterServerTransform.h"
#include "FLItemUsing.h"
#include "FLItemUpgradeManager.h"
#include "FLItemMergeManager.h"
#include "FLMadrigalGift.h"

#include "../_CommonDefine/Packet/FLPacketItemUpgrade.h"
#include "../_CommonDefine/Packet/FLPacketItemOption.h"

#include "FLFlyffPieceEvent.h"
#include "../_CommonDefine/Packet/FLPacketFlyffPieceEvent.h"

#include "FLExchangeEvent.h"
#include "../_CommonDefine/Packet/FLPacketItemExchange.h"


//////////////////////////////////////////////////////////////////////////
// shopping basket
void	CDPSrvr::OnReqBuyItemSoppingBasket( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE && pUser->IsDie() == FALSE )
	{
		T_PACKET_SHOPPING_BASKET tBasket = { 0 };
		//mem_set( &tBasket, 0, sizeof( tBasket ) );
		ar.Read( (void*)&tBasket, sizeof( T_PACKET_SHOPPING_BASKET ) );

		int nResult = g_xItemShopping->BuyItemByBasket( pUser, &tBasket );

		switch( nResult )
		{
		case FLItemShopping::ERR_SUCCESS:
			break;

		case FLItemShopping::ERR_BUY_ONLY_OCCUPATION:
			pUser->AddDefinedText( TID_GAME_SECRETROOM_STORE_BUY );
			break;

		case FLItemShopping::ERR_LACK_MONEY:
		case FLItemShopping::ERR_SUMCOST_OVERFLOW:
			pUser->AddDefinedText( TID_GAME_LACKMONEY );
			break;

		case FLItemShopping::ERR_LACK_CHIP:
		case FLItemShopping::ERR_SUMCHIP_OVERFLOW:
			pUser->AddDefinedText( TID_GAME_LACKCHIP );
			break;

		case FLItemShopping::ERR_LACK_SPACE:
			pUser->AddDefinedText( TID_GAME_LACKSPACE );
			break;

		default:
			break;
		}

		tBasket.bResult	= ( nResult == FLItemShopping::ERR_SUCCESS ) ? true : false;
		pUser->AddResultBuyItem( &tBasket );
	}
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// reset bind
void	CDPSrvr::OnReqResetBind( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE && pUser->IsDie() == FALSE )
	{
		T_PACKET_RESET_BIND tResetBind = { 0 };
		//mem_set( &tResetBind, 0, sizeof( tResetBind ) );
		ar.Read( (void*)&tResetBind, sizeof( T_PACKET_RESET_BIND ) );

		tResetBind.bResult = false;

		FLItemElem* pResetBindScroll = pUser->m_Inventory.GetAtId( tResetBind.dwSrcObjid );
		FLItemElem* pTargetBindItem = pUser->m_Inventory.GetAtId( tResetBind.dwDestObjid );

		int nResult = g_xUseActiveItem->OnDoUseActiveItem( pUser, pResetBindScroll, pTargetBindItem );

		if( nResult == FLUseActiveItem::ERR_SUCCESS_RESET_BIND )
		{
			tResetBind.bResult = true;

			g_DPSrvr.PutItemLog( pUser, "C", "RESET_BIND", pTargetBindItem );
			g_DPSrvr.PutItemLog( pUser, "N", "ResetBind_Use", pResetBindScroll );
			g_pItemUsing->CompleteUseItem( pUser, pResetBindScroll->m_dwObjId );
		}

		pUser->AddResultResetBind( &tResetBind );
	}
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// item merge
void	CDPSrvr::OnReqItemMerge( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE && pUser->IsDie() == FALSE )
	{
		T_PACKET_ITEM_MERGE tItemMerge = { 0 };
		//mem_set( &tItemMerge, 0, sizeof( tItemMerge ) );
		ar.Read( (void*)&tItemMerge, sizeof( T_PACKET_ITEM_MERGE ) );

		if( _GetContentState( CT_COSTUME_MIX ) == CS_VER1 )
		{
			g_xItemMergeManager->OnItemMerge( pUser, &tItemMerge );
		}
	}
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// item upgrade
void	CDPSrvr::OnReqUpgradeItemGeneralEnchant( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE && pUser->IsDie() == FALSE )
	{
		FLPacketItemUpgradeGeneralEnchantReq kPacket;

		kPacket.Deserialize( ar );

		g_pItemUpgradeManager->OnDoUpgradeItemGeneralEnchant( pUser, &kPacket );
	}
}

void	CDPSrvr::OnReqUpgradeItemAttributeEnchant( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE && pUser->IsDie() == FALSE )
	{
		FLPacketItemUpgradeAttributeEnchantReq kPacket;

		kPacket.Deserialize( ar );

		g_pItemUpgradeManager->OnDoUpgradeItemAttributeEnchant( pUser, &kPacket );
	}
}

void	CDPSrvr::OnReqUpgradeItemPiercingInsertItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE && pUser->IsDie() == FALSE )
	{
		FLPacketItemUpgradePiercingItemInsertReq kPacket;

		kPacket.Deserialize( ar );

		g_pItemUpgradeManager->OnDoUpgradeItemPiercingItemInsert( pUser, &kPacket );
	}
}

void	CDPSrvr::OnReqUpgradeItemRandomOptionGenerate( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE && pUser->IsDie() == FALSE )
	{
		FLPacketItemUpgradeRandomOptionGenerateReq kPacket;

		kPacket.Deserialize( ar );

		g_pItemUpgradeManager->OnDoUpgradeItemRandomOptionGenerate( pUser, &kPacket );
	}
}

void	CDPSrvr::OnReqUpgradeItemRandomOptionGenerateRetry( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE && pUser->IsDie() == FALSE )
	{
		FLPacketItemUpgradeRandomOptionGenerateRetryReq kPacket;

		kPacket.Deserialize( ar );

		g_pItemUpgradeManager->OnDoUpgradeItemRandomOptionGenerateRetry( pUser, &kPacket );
	}
}

void	CDPSrvr::OnReqUpgradeItemRandomOptionInitialize( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE && pUser->IsDie() == FALSE )
	{
		FLPacketItemUpgradeRandomOptionInitializeReq kPacket;

		kPacket.Deserialize( ar );

		g_pItemUpgradeManager->OnDoUpgradeItemRandomOptionInitialize( pUser, &kPacket );
	}
}

void	CDPSrvr::OnReqUpgradeItemEquipLevelDecrease( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE && pUser->IsDie() == FALSE )
	{
		FLPacketItemUpgradeEquipLevelDecreaseReq kPacket;

		kPacket.Deserialize( ar );

		g_pItemUpgradeManager->OnDoUpgradeItemEquipLevelDecrease( pUser, &kPacket );
	}
}

void	CDPSrvr::OnReqUpgradeItemEquipLevelInitialize( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE && pUser->IsDie() == FALSE )
	{
		FLPacketItemUpgradeEquipLevelInitializeReq kPacket;

		kPacket.Deserialize( ar );

		g_pItemUpgradeManager->OnDoUpgradeItemEquipLevelInitialize( pUser, &kPacket );
	}
}

void	CDPSrvr::OnReqUpgradeItemCombine( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE && pUser->IsDie() == FALSE )
	{
		FLPacketItemUpgradeCombineReq kPacket;

		kPacket.Deserialize( ar );

		g_pItemUpgradeManager->OnDoUpgradeItemCombine( pUser, &kPacket );
	}
}

void	CDPSrvr::OnReqUpgradeItemCombineInitializeData( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE && pUser->IsDie() == FALSE )
	{
		FLPacketItemUpgradeCombineInitializeDataReq kPacket;

		kPacket.Deserialize( ar );

		FLSnapshotItemUpgradeCombineInitializeDataAck toClient;

		FLItemElem* pItemElem	= pUser->m_Inventory.GetAtId( kPacket.dwMainItemObjID );
		if( IsUsableItem( pItemElem ) == FALSE )
		{
			toClient.bCanInitialize		= false;
			pUser->AddPacket( &toClient );
			return;
		}

		T_COMBINE_DATA kCombineData;

		const bool bResult	= g_pItemUpgradeManager->GetCombineInitializeData( *pItemElem, kCombineData );
		
		toClient.bCanInitialize		= bResult;
		toClient.nInitializePenya	= bResult == true ? kCombineData.nInitializePenya : 0;

		pUser->AddPacket( &toClient );
	}
}

void	CDPSrvr::OnReqUpgradeItemCombineInitialize( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE && pUser->IsDie() == FALSE )
	{
		FLPacketItemUpgradeCombineInitializeReq kPacket;

		kPacket.Deserialize( ar );

		g_pItemUpgradeManager->OnDoUpgradeItemCombineInitialize( pUser, &kPacket );
	}
}

void	CDPSrvr::OnReqUpgradeItemLooksChange( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE && pUser->IsDie() == FALSE )
	{
		FLPacketItemUpgradeLooksChangeReq kPacket;

		kPacket.Deserialize( ar );

		g_pItemUpgradeManager->OnDoUpgradeItemLooksChange( pUser, &kPacket );
	}
}

void	CDPSrvr::OnReqUpgradeItemLooksInitialize( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE && pUser->IsDie() == FALSE )
	{
		FLPacketItemUpgradeLooksInitializeReq kPacket;

		kPacket.Deserialize( ar );

		g_pItemUpgradeManager->OnDoUpgradeItemLooksInitialize( pUser, &kPacket );
	}
}
#ifdef BARUNA_ULTIMATE_UPDATE
#include "FLItemUpgradeBaruna.h"
void	CDPSrvr::OnReqUpgradeItemUltimateTrans( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE && pUser->IsDie() == FALSE )
	{
		FLPacketItemUpgradeUltimateTransReq kPacket;

		kPacket.Deserialize( ar );

		//g_pItemUpgradeManager->OnDoUpgradeItemGeneralEnchant( pUser, &kPacket );
		FLSnapshotItemUpgradeUltimateTransResultAck kAckPacket;

		FLItemElem* pMainItem		= pUser->m_Inventory.GetAtId( kPacket.dwMainItemObjID );
		if( IsUsableItem( pMainItem ) == FALSE )
		{
			kAckPacket.dwResult		= FSC_ITEMUPGRADE_ITEM_DATA_INVALID;
			pUser->AddPacket( &kPacket );
			return;
		}

		const FLItemUpgradeBaruna* pItemUpgradeBase = FLItemUpgradeBaruna::GetInstance();;
		if( pItemUpgradeBase == NULL )
		{
			kAckPacket.dwResult		= FSC_ITEMUPGRADE_NOT_SUPPORT;
			pUser->AddPacket( &kPacket );
			return;
		}

		const DWORD dwResult		= pItemUpgradeBase->OnDoUpgradeItemUltimateTrans( pUser, &kPacket );
		if( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS || dwResult == FSC_ITEMUPGRADE_UPGRADE_FAIL || dwResult == FSC_ITEMUPGRADE_UPGRADE_MINUS )
		{
			const DWORD dwSoundID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? SND_INF_UPGRADESUCCESS : SND_INF_UPGRADEFAIL;
			const DWORD dwSfxObjID	= ( dwResult == FSC_ITEMUPGRADE_UPGRADE_SUCCESS ) ? XI_INDEX( 1714, XI_INT_SUCCESS ) : XI_INDEX( 1715, XI_INT_FAIL );

			pUser->AddPlaySound( dwSoundID );
			if( pUser->IsMode( TRANSPARENT_MODE ) == FALSE )
			{
				g_xWSUserManager->AddCreateSfxObj( pUser, dwSfxObjID, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
			}
		}
		
		kAckPacket.dwResult			= dwResult;

		pUser->AddPacket( &kAckPacket );

	}
}
#endif

#ifdef CARD_UPGRADE_SYSTEM
void	CDPSrvr::OnReqUpgradeItemCard( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE && pUser->IsDie() == FALSE )
	{
		FLPacketItemUpgradeCardReq kPacket;

		kPacket.Deserialize( ar );

		g_pItemUpgradeManager->OnDoUpgradeItemCard( pUser, &kPacket );
	}
}
#endif	// CARD_UPGRADE_SYSTEM

#ifdef	KEYBOARD_SET
void	CDPSrvr::OnKeyBoardModeSetting( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{

	int bKeyboardMode;
	ar >> bKeyboardMode;

	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( bKeyboardMode )
			pUser->SetMode( OLD_KEYBOARD_MODE );
		else
			pUser->SetNotMode( OLD_KEYBOARD_MODE );
		g_xWSUserManager->AddModifyMode( pUser );
	}
}
#endif	// KEYBOARD_SET 

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
void	CDPSrvr::OnReqUpgradeItemSetGem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE && pUser->IsDie() == FALSE )
	{
		FLPacketItemUpgradeSetGemReq kPacket;

		kPacket.Deserialize( ar );

		g_pItemUpgradeManager->OnDoUpgradeItemSetGem( pUser, &kPacket );
	}
}

void	CDPSrvr::OnReqUpgradeItemRemoveGem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE && pUser->IsDie() == FALSE )
	{
		FLPacketItemUpgradeRemoveGemReq kPacket;

		kPacket.Deserialize( ar );

		g_pItemUpgradeManager->OnDoUpgradeItemRemoveGem( pUser, &kPacket );
	}
}
#endif

#ifdef COSTUME_UPGRADE_MIX
void	CDPSrvr::OnReqUpgradeItemMix( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE && pUser->IsDie() == FALSE )
	{
		FLPacketItemUpgradeMixReq kPacket;

		kPacket.Deserialize( ar );

		g_pItemUpgradeManager->OnDoUpgradeItemMix( pUser, &kPacket );
	}
}
#endif
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// teleport map
void	CDPSrvr::OnReqTeleportMapToPoint( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		FLPacketTeleportMapToPointReq kFromClient;

		kFromClient.Deserialize( ar );

		g_pTeleportMapUserPointMng->HandleTeleportMapToPoint( pUser, &kFromClient );
	}
}

void	CDPSrvr::OnReqTeleportMapAddMyPoint( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		FLPacketTeleportMapAddMyPointReq kFromClient;

		kFromClient.Deserialize( ar );

		g_pTeleportMapUserPointMng->HandleTeleportMapAddMyPoint( pUser, &kFromClient );
	}
}

void	CDPSrvr::OnReqTeleportMapChgPointName( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		FLPacketTeleportMapChgPointNameReq kFromClient;

		kFromClient.Deserialize( ar );

		g_pTeleportMapUserPointMng->HandleTeleportMapChgPointName( pUser, &kFromClient );
	}
}

void	CDPSrvr::OnReqTeleportMapDelPoint( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		FLPacketTeleportMapDelPointReq kFromClient;

		kFromClient.Deserialize( ar );

		g_pTeleportMapUserPointMng->HandleTeleportMapDelPoint( pUser, &kFromClient );
	}
}

void	CDPSrvr::OnReqTeleportMapDelAllPoint( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		FLPacketTeleportMapDelAllPointReq kFromClient;

		kFromClient.Deserialize( ar );

		g_pTeleportMapUserPointMng->HandleTeleportMapDelAllPoint( pUser, &kFromClient );
	}
}
//////////////////////////////////////////////////////////////////////////

void	CDPSrvr::OnNotiEventArenaMoveBattleWait( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		if( g_pEventArenaGlobal->IsArenaChannel() )
		{
			FLPacketEventArenaMoveBattleWaitNoti kFromClient;
			kFromClient.Deserialize( ar );

			g_pEventArena->AcceptCallPlayer( pUser );
		}
	}
}


void	CDPSrvr::OnNotiEventArenaManageSetTonement( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		if( g_pEventArenaGlobal->IsArenaChannel() )
		{
			FLPacketEventArenaManageSetTonementNoti kFromClient;
			kFromClient.Deserialize( ar );

			g_pEventArena->SetTonement( pUser, &kFromClient );
		}
	}
}

void	CDPSrvr::OnNotiEventArenaManageSetRedTeam( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		if( g_pEventArenaGlobal->IsArenaChannel() )
		{
			FLPacketEventArenaManageSetRedTeamNoti kFromClient;
			kFromClient.Deserialize( ar );

			g_pEventArena->SetRedTeam( pUser, &kFromClient );
		}
	}
}

void	CDPSrvr::OnNotiEventArenaManageSetBlueTeam( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		if( g_pEventArenaGlobal->IsArenaChannel() )
		{
			FLPacketEventArenaManageSetBlueTeamNoti kFromClient;
			kFromClient.Deserialize( ar );

			g_pEventArena->SetBlueTeam( pUser, &kFromClient );
		}
	}
}

void	CDPSrvr::OnNotiEventArenaManageCallPlayers( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		if( g_pEventArenaGlobal->IsArenaChannel() )
		{
			FLPacketEventArenaManageCallPlayersNoti kFromClient;
			kFromClient.Deserialize( ar );

			g_pEventArena->CallPlayers( pUser, &kFromClient );
		}
	}
}

void	CDPSrvr::OnNotiEventArenaManageJoinPlayers( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		if( g_pEventArenaGlobal->IsArenaChannel() )
		{
			FLPacketEventArenaManageJoinPlayersNoti kFromClient;
			kFromClient.Deserialize( ar );

			g_pEventArena->JoinPlayers( pUser, &kFromClient );
		}
	}
}

void	CDPSrvr::OnNotiEventArenaManageBattleStart( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		if( g_pEventArenaGlobal->IsArenaChannel() )
		{
			FLPacketEventArenaManageBattleStartNoti kFromClient;
			kFromClient.Deserialize( ar );

			g_pEventArena->StartBattle( pUser, &kFromClient );
		}
	}
}

void	CDPSrvr::OnNotiEventArenaManageGameEnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		if( g_pEventArenaGlobal->IsArenaChannel() )
		{
			FLPacketEventArenaManageGameEndNoti kFromClient;
			kFromClient.Deserialize( ar );

			g_pEventArena->EndGame( pUser, &kFromClient );
		}
	}
}

void	CDPSrvr::OnNotiEventArenaManageGameStop( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		if( g_pEventArenaGlobal->IsArenaChannel() )
		{
			FLPacketEventArenaManageGameStopNoti kFromClient;
			kFromClient.Deserialize( ar );

			g_pEventArena->StopGame( pUser, &kFromClient );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// charge zone ticket
void	CDPSrvr::OnReqChargeZoneEnterance( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		FLPacketChargeZoneTicketEnteranceReq kPacket;

		kPacket.Deserialize( ar );

		g_pChargeZoneTicket->EnterChargeZoneWorld( pUser, &kPacket );
	}
}

void	CDPSrvr::OnReqChargeZoneLeave( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		FLPacketChargeZoneTicketLeaveReq kPacket;

		kPacket.Deserialize( ar );

		g_pChargeZoneTicket->LeaveChargeZoneWorld( pUser, &kPacket );
	}
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// treasure chest
void	CDPSrvr::OnReqTreasureChestOpenByKey( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		FLPacketTreasureChestOpenByKeyReq kPacket;

		kPacket.Deserialize( ar );

		g_pTreasureChest->OnDoOpenTreasureChest( pUser, &kPacket );
	}
}
//////////////////////////////////////////////////////////////////////////

void	CDPSrvr::OnReqCharacterServerTransform( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == TRUE )
	{
		FLPacketCharacterServerTransformReq kPacket;

		kPacket.Deserialize( ar );

		g_pCharacterServerTransform->OnDoTransformCharacterServer( pUser, &kPacket );
	}
}

void	CDPSrvr::OnTeleportToNPC( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	DWORD dwNPC_ObjID = 0;
	int	nMMI_ID = 0;
	
	ar >> dwNPC_ObjID >> nMMI_ID;

	//pUser->AddGold(-10000);		// 텔레포트시 페냐 소비

	pUser->TeleportToNPC( dwNPC_ObjID, nMMI_ID );
}

void	CDPSrvr::OnItemUpgradeRandomOptionSyncReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	FLPacketItemOptionRandomOptionSyncReq fromClient;
	fromClient.Deserialize( ar );

	FLItemElem* pItemElem	= pUser->m_Inventory.GetAtId( fromClient.dwItemObjID );
	if( pItemElem != NULL )
	{
		FLSnapshotItemUpgradeRandomOptionAck toClient;

		toClient.dwItemObjID		= pItemElem->m_dwObjId;
		toClient.dwSerialNumber		= pItemElem->GetSerialNumber();
		pItemElem->GetRandomOption( toClient.kRandomOption );

		pUser->AddPacket( &toClient );
	}

	FLSnapshotItemUpgradeRandomOptionSyncAck toClient;
	toClient.dwItemObjID		= fromClient.dwItemObjID;
	toClient.bSuccess			= ( pItemElem != NULL ) ? true : false;

	pUser->AddPacket( &toClient );
}

void	CDPSrvr::OnMadrigalGiftRewardItemRecvReq( CAr & /*ar*/, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

// 	FLPacketMadrigalGiftItemReceiveReq fromClient;
// 	fromClient.Deserialize( ar );

	g_pMadrigalGift->OnReqReceiveGiftItem( pUser );
}

void	CDPSrvr::OnFlyffPieceExchangeListReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	FLPacketFlyffPieceExchangeListReq fromClient;
	fromClient.Deserialize( ar );

	g_pFlyffPieceEvent->OnViewList( pUser, &fromClient );
}

void	CDPSrvr::OnExchangeFlyffPieceItemReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	FLPacketExchangeFlyffPieceItemReq fromClient;
	fromClient.Deserialize( ar );

	g_pFlyffPieceEvent->OnDoExchangeItem( pUser, &fromClient );
}

void	CDPSrvr::OnItemExchangeReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long )
{
	FLWSUser* pUser = g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	FLPacketItemExchangeReq fromClient;
	fromClient.Deserialize( ar );

	g_pExchangeEvent->OnDoExchange( pUser, &fromClient );
}
