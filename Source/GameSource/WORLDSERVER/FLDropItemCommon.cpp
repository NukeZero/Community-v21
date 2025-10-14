#include "StdAfx.h"

#include "FLDropItemCommon.h"
#include "FLMonsterDropItem.h"

#include "FLDropPenalty_ByLevelDist.h"

#include <spevent.h>
#include "FLCooperativeContributions.h"
#include "FLCC_Reward.h"
#include "slord.h"

#include "../_Common/party.h"
extern	CPartyMng	g_PartyMng;

extern	BOOL CanAdd( DWORD dwGold, int nPlus );



FLDropItem_Data::FLDropItem_Data() : m_dwProb( 0 ), m_dwItemID( 0 ), m_nItemCountMin( 0 ), m_nItemCountMax( 0 ), m_nQuestID( 0 )
{

}

FLDropItem_Data::~FLDropItem_Data()
{

}

//-------------------------------------------------------------------------------------------------------------//

FLDropSet::FLDropSet() : m_dwMainProb( 0 ), m_eSetKind( KIND_NONE )
{

}

FLDropSet::~FLDropSet()
{

}

void FLDropSet::DropItem( CMover & kMonster, FLWSUser & kUser )
{
	if( kMonster.GetProp() == NULL )
		return;

	//1. 외부 드랍률 계산
	const DWORD dwMainProb	= static_cast< DWORD >( m_dwMainProb * FLDoDrop::GetItemDropRateFactor( &kMonster, kUser, m_eSetKind ) );
	const DWORD dwMainRandom= ::xRandom( DROP_PROB_MAX );
	if( dwMainRandom >= dwMainProb )
		return;

	//2. 드랍
	DWORD dwAccumulatedProb	= 0;
	const DWORD dwRandom	= ::xRandom( DROP_PROB_MAX );
	for( DropItemVec::iterator it = m_vecDropItem.begin(); it != m_vecDropItem.end(); ++it )
	{
		FLDropItem_Data & kDropItem = *it;

		dwAccumulatedProb		+= kDropItem.m_dwProb;
		if( dwRandom >= dwAccumulatedProb )
			continue;

		if( kDropItem.m_nItemCountMax <= 0 )
			continue;

		int nItemCount	= static_cast< int >( ::xRandom( kDropItem.m_nItemCountMax - kDropItem.m_nItemCountMin ) + kDropItem.m_nItemCountMin );
		if( nItemCount <= 0 )
			nItemCount		= 1;

		const DWORD dwOwnerID	= ( kMonster.GetProp()->m_kDropItemAttr.bDropOwnerShipOff ) ? NULL_ID : kUser.GetId();
		
		if( kDropItem.m_nQuestID == 0 )
		{
			FLDoDrop::DropItem( kMonster, kUser, dwOwnerID, kDropItem.m_dwItemID, nItemCount );
			return;
		}

		QUEST * pQuest			= kUser.GetQuest( kDropItem.m_nQuestID );
		if( pQuest != NULL && pQuest->m_nState < QS_END )
		{
			FLDoDrop::DropItem( kMonster, kUser, dwOwnerID, kDropItem.m_dwItemID, nItemCount );
			return;
		}
	}
}


FLDropSet::E_DROP_SET_KIND FLDropSet::StringToKindEnum( std::string & strSetKind )
{
	std::transform( strSetKind.begin(), strSetKind.end(), strSetKind.begin(), toupper );
	if( strSetKind == "WEAPON" )
		return FLDropSet::KIND_WEAPON;
	else if( strSetKind == "ARMOR")
		return FLDropSet::KIND_ARMOR;
	else if( strSetKind == "ACCESSORY")
		return FLDropSet::KIND_ACCESSORY;
	else if( strSetKind == "MATERIAL")
		return FLDropSet::KIND_MATERIAL;
	else if( strSetKind == "ETC")
		return FLDropSet::KIND_ETC;
	else if( strSetKind == "QUEST")
		return FLDropSet::KIND_QUEST;
	else if( strSetKind == "EVENT")
		return FLDropSet::KIND_EVENT;
	else if( strSetKind == "RARE")
		return FLDropSet::KIND_RARE;

	return FLDropSet::KIND_NONE;
}

BOOL FLDropSet::Check_Item( const DWORD dwItemID, const int nQuestID )
{
	PT_ITEM_SPEC pItemSpec	= g_xSpecManager->GetSpecItem( dwItemID );
	if( pItemSpec == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ITEM ID (%u) ]" ), dwItemID );
		return FALSE;
	}

	//예외
	switch( m_eSetKind )
	{
	case FLDropSet::KIND_NONE:
		{
			if( nQuestID != 0 )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ SET QUEST (%d) ]" ), nQuestID );
				return FALSE;
			}
		}
		break;

	case FLDropSet::KIND_WEAPON:
		{
			if( pItemSpec->dwItemKind1 != IK1_WEAPON )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ NOT WEAPON SET (%u) ]" ), dwItemID );
				return FALSE;
			}
		}
		break;
	case FLDropSet::KIND_ARMOR:
		{
			if( pItemSpec->dwItemKind1 != IK1_ARMOR )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ NOT ARMOR SET (%u) ]" ), dwItemID );
				return FALSE;
			}
		}
		break;

	case FLDropSet::KIND_ACCESSORY:
		{
			if( pItemSpec->dwItemKind1 != IK1_GENERAL || pItemSpec->dwItemKind2 != IK2_JEWELRY )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ NOT ACCESSORY SET (%u) ]" ), dwItemID );
				return FALSE;
			}
		}
		break;

	case FLDropSet::KIND_MATERIAL:
		{
			if( pItemSpec->dwItemKind1 != IK1_GENERAL || pItemSpec->dwItemKind2 != IK2_MATERIAL )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ NOT MATERIAL SET (%u) ]" ), dwItemID );
				return FALSE;
			}
		}
		break;

	case FLDropSet::KIND_ETC:
		{
			// ETC는 기획팀에서 체크 안하기로함
			__noop;
		}
		break;

	case FLDropSet::KIND_QUEST:
		{
			if( ( pItemSpec->dwItemKind1 != IK1_SYSTEM || pItemSpec->dwItemKind2 != IK2_SYSTEM || pItemSpec->dwItemKind3 != IK3_QUEST ) 
				&& ( pItemSpec->dwItemKind3 != IK3_EVENTSUB ) 
				&& ( pItemSpec->dwItemKind3 != IK3_EVENTMAIN ) )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ NOT QUEST SET (%u) ]" ), dwItemID );
				return FALSE;
			}

			QuestProp * pQuestProp	= prj.m_aPropQuest.GetAt( nQuestID );
			if( pQuestProp == NULL )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID QUEST ID (%d) ]" ), nQuestID );
				return FALSE;
			}
		}
		break;
	case FLDropSet::KIND_EVENT:
		{
			__noop;
		}
		break;
	case FLDropSet::KIND_RARE:
		{
			__noop;
		}	
		break;

	default:
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID DROP SET KIND(%d) ]" ), m_eSetKind );
		break;
	}

	return TRUE;
}

BOOL FLDropSet::Check_Prob() const
{
	DWORD dwTotProb		= 0;

	for( FLDropSet::DropItemVec::const_iterator itItem = m_vecDropItem.begin(); itItem != m_vecDropItem.end(); ++itItem )
	{
		const FLDropItem_Data & kItemData = * itItem;
		dwTotProb			+= kItemData.m_dwProb;
	}

	if( dwTotProb != DROP_PROB_MAX )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID PROB : Name(%s) TotalProb(%u) ]"), m_strMainProbName.c_str(), dwTotProb );
		return FALSE;
	}

	return TRUE;
}




//----------------------------------------------------------------------------------------------------------------------------------------//

namespace {
	bool	addItemToFlyingAttacker( CMover& kMonster, FLWSUser& kAttacker, const DWORD dwItemID, const int nItemCount )
	{
		if( kMonster.GetProp() == NULL || !kMonster.GetProp()->dwFlying ) {
			return false;
		}

		const PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( dwItemID );
		if( pItemProp == NULL ) {
			return false;
		}

		if( kAttacker.m_Inventory.GetEquip( PARTS_RIDE ) == NULL
			|| ( kAttacker.m_Inventory.GetEquip( PARTS_RIDE )->GetProp()->GetInventoryType() == pItemProp->GetInventoryType()
			&& kAttacker.m_Inventory.GetEmptyCountByInventoryType( pItemProp->GetInventoryType() ) <= 1 ) ) {
				return false;
		}

		FLItemElem itemElem;
		itemElem.m_dwItemId	= dwItemID;
		itemElem.m_nItemNum	= nItemCount;
		itemElem.SetSerialNumber();
		if( !kAttacker.CreateItem( &itemElem ) ) {
			return false;
		}

		kAttacker.AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", pItemProp->szName );
		if( pItemProp->dwItemKind1 == IK1_WEAPON || pItemProp->dwItemKind1 == IK1_ARMOR || ( pItemProp->dwItemKind1 == IK1_GENERAL && pItemProp->dwItemKind2 == IK2_JEWELRY ) )
		{
			switch( pItemProp->nLog )
			{
			case 1:		g_dpDBClient.SendLogUniqueItem2( &kAttacker, itemElem.GetProp(), itemElem.GetAbilityOption() );
				break;
			case 2:		g_dpDBClient.SendLogUniqueItem2( &kAttacker, itemElem.GetProp(), 200 );
				break;
			case 3:		g_dpDBClient.SendLogUniqueItem2( &kAttacker, itemElem.GetProp(), 100 );
				break;
			default:	__noop;
				break;
			}
		}

		return true;
	}

	bool	addGoldToFlyingAttacker( CMover& kMonster, FLWSUser & kAttacker, const int nGold )
	{
		if( kMonster.GetProp() == NULL || !kMonster.GetProp()->dwFlying ) {
			return false;
		}

		if( !CanAdd( kAttacker.GetGold(), nGold ) ) {
			return false;
		}

		kAttacker.AddGold( nGold );
		kAttacker.AddGoldText( nGold );

		return true;
	}
}

// nProbability * GetItemDropRateFactor( pAttacker );/* 이벤트 */

//void FLDoDrop::Drop( FLWUser & kMonster, FLWSUser & kAttacker )
//{
//	
//}

float FLDoDrop::GetItemDropRateFactor( CMover * pMonster, FLWSUser & kAttacker, const FLDropSet::E_DROP_SET_KIND eSetKind )
{
	float fFactor			= 1.0f;
	fFactor					+= prj.m_fItemDropRate - 1.0f;
	fFactor					+= CEventGeneric::GetInstance()->GetItemDropRateFactor() - 1.0f;
	fFactor					+= prj.m_EventLua.GetItemDropRate() - 1.0f;
	fFactor					+= ( nsCooperativeContributions::GLOBAL_REWARD_ALARM_DROP_RATE().GetRewardValue() / ( float )REWARD_PRECISION ) - 1.0f;
	fFactor					+= CPCBang::GetInstance()->GetPieceItemDropFactor( static_cast< FLWSUser * >( &kAttacker ) ) - 1.0f;
	//const float fEventLuaPieceItemRate	= prj.m_EventLua.GetPieceItemDropRate() - 1.0f;
	
	//sun: 12, 군주
	ILordEvent* pEvent		= CSLord::Instance()->GetEvent();
	if( pEvent != NULL )
	{
		fFactor				+= pEvent->GetIFactor() - 1.0f;
		fFactor				+= kAttacker.HasBuff( BUFF_ITEM, ITEM_INDEX( 20019, II_SYS_SYS_LS_CHEERING ) ) ? 1.1f - 1.0f : 0.0f;
		fFactor				+= kAttacker.HasBuff( BUFF_ITEM, ITEM_INDEX( 20036, II_SYS_SYS_VOTE_THANKS ) ) ? 1.05f - 1.0f : 0.0f;
		fFactor				+= kAttacker.HasBuff( BUFF_ITEM, ITEM_INDEX( 26740, II_COUPLE_BUFF_MIRACLE_01 ) ) ? 1.05f - 1.0f : 0.0f;
	}

	if( pMonster != NULL && pMonster->GetProp() != NULL )
	{
#ifdef __S1108_BACK_END_SYSTEM
		fFactor				+= pMonster->GetProp()->m_fItemDrop_Rate - 1.0f;
#endif // __S1108_BACK_END_SYSTEM
		if( eSetKind != FLDropSet::KIND_QUEST && pMonster->GetProp()->m_kDropItemAttr.bLevelDropPanaltyOff == FALSE )
		{
			fFactor			*= FLDropPenalty_ByLevelDist::GetInstance().GetDeclineProb_Item( kAttacker.GetLevel(), pMonster->GetLevel() );
		}
	}

	//극단 스킬
	//const DWORD SKILL_PROB_MAX			= 10000;
	CParty* pParty = g_PartyMng.GetParty( kAttacker.m_idparty );
	if( pParty && eSetKind == FLDropSet::KIND_RARE )
	{
		// 순회극단이며 링크어택중이며 단장만 데미지를 높여준다
		if( pParty->GetKind() == PARTY_KIND_EXTEND && pParty->m_dwModeTime[ PARTY_FORTUNECIRCLE_MODE ] )
		{
			SkillProp* pSkillProp	= prj.GetPartySkill( SKILL_INDEX( 6, ST_FORTUNECIRCLE )  );
			if( pSkillProp != NULL )
			{
				//sun: 12, 파스킬 아이템 수정
//				if( pParty->m_nModeTime[ PARTY_PARSKILL_MODE ] )
				if( pParty->IsActivatedEffect( PARTY_EFFECT_PARSKILLFULL ) == true )
				{
					//fFactor			+= pSkillProp->nProbability / 10000;
					fFactor				= fFactor + ( ( static_cast< float >( pSkillProp->nProbability) / 10000 ) - 1.0f );
				}
				else
				{
					FLWSUser* pLeader = g_xWSUserManager->GetUserByPlayerID( pParty->m_aMember[0].m_uPlayerId );
					if( IsValidObj( (CObj*)pLeader )/* && pLeader->IsNearPC( (FLWSUser*)pAttacker ) && IsOrigin()*/ )
					{
						if( pLeader->IsValidArea( & kAttacker, 255.0f ) )
						{
							//fFactor			+= pSkillProp->nProbability / 10000;
							fFactor				= fFactor + ( ( static_cast< float >( pSkillProp->nProbability) / 10000 ) - 1.0f );
						}
					}
				}
			}
		}
	}

	const int nParam	= kAttacker.GetParam( DST_DROP_ITEM_ALLGRADE_RATE );

	fFactor	+= static_cast<float>( nParam )	/ 100.0f;

	return fFactor > 0.0f ? fFactor : 0.0f;
}

//float FLDoDrop::GetItemDropRateFactor( CMover & kMonster, FLWSUser & kAttacker, const BOOL bQuest )
//{
//	float fFactor			= 1.0f;
//
//	fFactor					*= prj.m_fItemDropRate;
//#ifdef __S1108_BACK_END_SYSTEM
//	fFactor					*= kMonster.GetProp()->m_fItemDrop_Rate;
//#endif // __S1108_BACK_END_SYSTEM
//
//	fFactor					*= CEventGeneric::GetInstance()->GetItemDropRateFactor();
//
//	const float fMulRate	= ( prj.m_EventLua.GetItemDropRate() - 1.0f ) + ( nsCooperativeContributions::GLOBAL_REWARD_ALARM_DROP_RATE().GetRewardValue() - 1.0f ) + 1.0f;
//	fFactor					*= fMulRate;
//
//	fFactor					*= prj.m_EventLua.GetPieceItemDropRate();		//sun: 9, 이벤트 (루아 스크립트 적용)
//
//	//sun: 12, 군주
//	{
//		ILordEvent* pEvent		= CSLord::Instance()->GetEvent();
//		fFactor					*= pEvent->GetIFactor();
//		if( kAttacker.HasBuff( BUFF_ITEM, ITEM_INDEX( 20019, II_SYS_SYS_LS_CHEERING ) ) )
//			fFactor				*=	1.1F;
//		if( kAttacker.HasBuff( BUFF_ITEM, ITEM_INDEX( 20036, II_SYS_SYS_VOTE_THANKS ) ) )
//			fFactor				*= 1.05F;
//
//		//sun: 13, 커플 보상
//		if( kAttacker.HasBuff( BUFF_ITEM, ITEM_INDEX( 26740, II_COUPLE_BUFF_MIRACLE_01 ) ) )
//			fFactor				*= 1.05F;
//	}
//
//	fFactor					*= CPCBang::GetInstance()->GetPieceItemDropFactor( static_cast<FLWSUser*>( &kAttacker ) );
//
//	if( bQuest == TRUE || ( kMonster.GetProp() != NULL && kMonster.GetProp()->m_kDropItemAttr.bLevelDropPanaltyOff == FALSE ) )
//		fFactor					*= FLDropPenalty_ByLevelDist::GetInstance().GetDeclineProb_Item( kAttacker.GetLevel(), kMonster.GetLevel() );
//
//	return fFactor;
//}
//
//
//float FLDoDrop::GetItemDropRateFactor( CMover & kMonster, FLWSUser & kAttacker, const BOOL bQuest )
//{
//	float fFactor			= 1.0f;
//
//	fFactor					*= prj.m_fItemDropRate;
//#ifdef __S1108_BACK_END_SYSTEM
//	fFactor					*= kMonster.GetProp()->m_fItemDrop_Rate;
//#endif // __S1108_BACK_END_SYSTEM
//
//	fFactor					*= CEventGeneric::GetInstance()->GetItemDropRateFactor();
//
//	const float fMulRate	= ( prj.m_EventLua.GetItemDropRate() - 1.0f ) + ( nsCooperativeContributions::GLOBAL_REWARD_ALARM_DROP_RATE().GetRewardValue() - 1.0f ) + 1.0f;
//	fFactor					*= fMulRate;
//
//	fFactor					*= prj.m_EventLua.GetPieceItemDropRate();		//sun: 9, 이벤트 (루아 스크립트 적용)
//
//	//sun: 12, 군주
//	{
//		ILordEvent* pEvent		= CSLord::Instance()->GetEvent();
//		fFactor					*= pEvent->GetIFactor();
//		if( kAttacker.HasBuff( BUFF_ITEM, ITEM_INDEX( 20019, II_SYS_SYS_LS_CHEERING ) ) )
//			fFactor				*=	1.1F;
//		if( kAttacker.HasBuff( BUFF_ITEM, ITEM_INDEX( 20036, II_SYS_SYS_VOTE_THANKS ) ) )
//			fFactor				*= 1.05F;
//
//		//sun: 13, 커플 보상
//		if( kAttacker.HasBuff( BUFF_ITEM, ITEM_INDEX( 26740, II_COUPLE_BUFF_MIRACLE_01 ) ) )
//			fFactor				*= 1.05F;
//	}
//
//	fFactor					*= CPCBang::GetInstance()->GetPieceItemDropFactor( static_cast<FLWSUser*>( &kAttacker ) );
//
//	if( bQuest == TRUE || ( kMonster.GetProp() != NULL && kMonster.GetProp()->m_kDropItemAttr.bLevelDropPanaltyOff == FALSE ) )
//		fFactor					*= FLDropPenalty_ByLevelDist::GetInstance().GetDeclineProb_Item( kAttacker.GetLevel(), kMonster.GetLevel() );
//
//	return fFactor;
//}
//



int	FLDoDrop::GetAdditinalLoopCount( FLWSUser & kAttacker )
{
	int nLoop				= 0;

	nLoop	+= kAttacker.GetAdjValueDuplicationBuff( IK3_MOREGETITEMSCROLL );

// 	if( kAttacker.HasBuff( BUFF_ITEM, ITEM_INDEX( 30115, II_SYS_SYS_SCR_GET01 ) ) )		//@@@@@@@@@@@@@@
// 		nLoop				+= 1;
// 	if( kAttacker.HasBuff( BUFF_ITEM, ITEM_INDEX( 30116, II_SYS_SYS_SCR_GET02 ) ) )		//@@@@@@@@@@@@@@
// 		nLoop				+= 2;

	nLoop	+= kAttacker.GetAdjParam( DST_GIFTBOX );
	
	CParty* pParty			= g_PartyMng.GetParty( kAttacker.m_idparty );
	if( pParty != NULL && pParty->GetKind() == PARTY_KIND_EXTEND )
	{
		if( pParty->m_dwModeTime[ PARTY_GIFTBOX_MODE ] )
		{
//			if( pParty->m_nModeTime[ PARTY_PARSKILL_MODE ] )
			if( pParty->IsActivatedEffect( PARTY_EFFECT_PARSKILLFULL ) == true )
			{
				nLoop	+= 1;
			}
			else
			{
				FLWSUser* pLeader = g_xWSUserManager->GetUserByPlayerID( pParty->m_aMember[0].m_uPlayerId );
				if( IsValidObj( (CObj*)pLeader ) && pLeader->IsValidArea( & kAttacker, 255.0f ) )
				{
					nLoop	+= 1;
				}
			}
		}
	}

	if( ::xRandom( DROP_PROB_MAX ) < static_cast<DWORD>( kAttacker.GetParam( DST_DROP_ITEM_ONEMORE_CHANCE ) ) )
	{
		nLoop	+= 1;
	}

	return nLoop > 0 ? nLoop : 0;
}




float FLDoDrop::GetGoldDropRateFactor( CMover & kMonster, FLWSUser & kAttacker )
{
	/*
	//@@@@@@@@ 보정치
	//	int	nSeedID = 0;
	//	int nNumGold = lpDropItem->dwNumber + xRandom( lpDropItem->dwNumber2 - lpDropItem->dwNumber );	// Number ~ Number2 사이의 랜덤값.
	//nNumGold	= nNumGold * nPenyaRate / 100;		//보정치	

	#ifdef __S1108_BACK_END_SYSTEM
	nNumGold	= (int)( nNumGold * prj.m_fGoldDropRate * lpMoverProp->m_fPenya_Rate );
	if( nNumGold == 0 )
	continue;
	#else // __S1108_BACK_END_SYSTEM
	nNumGold	*= prj.m_fGoldDropRate;
	#endif // __S1108_BACK_END_SYSTEM

	nNumGold	= (int)( nNumGold * prj.m_EventLua.GetGoldDropFactor() );	//sun: 9, 이벤트 (루아 스크립트 적용)
	*/

	//@@@@@@@@ 보정치
	float fFactor			= 1.0f;
	fFactor					+= prj.m_fGoldDropRate - 1.0f;
	fFactor					+= prj.m_EventLua.GetGoldDropFactor() - 1.0f;

	if( kMonster.GetProp() != NULL && kMonster.GetProp()->m_kDropItemAttr.bLevelDropPanaltyOff == FALSE )
		fFactor				*= FLDropPenalty_ByLevelDist::GetInstance().GetDeclineProb_Gold( kAttacker.GetLevel(), kMonster.GetLevel() );

	return fFactor;
}




void FLDoDrop::DropItem( CMover & kMonster, FLWSUser & kAttacker, const DWORD dwOwnerPlayerID, const DWORD dwItemID, const int nItemCount )
{
	if( nItemCount <= 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ITEM QUANTITY : nItemCount(%d) ]" ), nItemCount );
		return;
	}

	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( dwItemID );
	if( pItemProp == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ITEM ID : dwItemID(%u) ]" ), dwItemID );
		return;
	}

	CWorld* pWorld = kAttacker.GetWorld();
	if( pWorld == NULL )
	{
		return;
	}

	if( !addItemToFlyingAttacker( kMonster, kAttacker, dwItemID, nItemCount ) ) {
		FLItemElem * pItemElem	= new FLItemElem;
		pItemElem->m_dwItemId	= dwItemID;
		pItemElem->m_nItemNum	= nItemCount;
		pItemElem->m_nHitPoint	= ( pItemProp->dwEndurance == -1 ) ? 0 : pItemProp->dwEndurance;//pItemProp->dwEndurance;

		
		// Random option start
		MoverProp* lpMoverProp	= kMonster.GetProp();
		const WORD wOriginID = static_cast< WORD >( CRandomOptItemGen::GetInstance()->GenRandomOptItem( lpMoverProp->dwLevel, (FLOAT)100 / 100.0f, pItemProp, lpMoverProp->dwClass ) );
		//const WORD wOriginID = static_cast< WORD >( CRandomOptItemGen::GetInstance()->GenRandomOptItem( 10, (FLOAT)100 / 100.0f, pItemProp, 1 ) );
		pItemElem->SetRandomOptionOriginID( wOriginID );
		int nSize = lpMoverProp->m_DropItemGenerator.GetSize();
		
		DWORD dwEnhanceProb = xRandom( 3000000000 );
		DWORD dwEnhanceRandom	= xRandom( 3 );
		if( dwEnhanceProb > 2100000000 && ( pItemProp->dwItemKind1 == IK1_WEAPON || pItemProp->dwItemKind1 == IK1_ARMOR ) )
		{

			
			if ( !pItemProp->IsBaruna() && !pItemProp->IsUltimate() )	pItemElem->SetAbilityOption( dwEnhanceRandom+1 );		// 추가 능력치 +1, +2 같은거.
			else if ( dwEnhanceProb > 2700000000)						pItemElem->SetAbilityOption( 1 );		// 추가 능력치 +1, +2 같은거.
			
		}

		
		
		DROPITEM* lpDropItem;
		for ( int i = 0; i < nSize; i++ )
		{
			if( ( lpDropItem = lpMoverProp->m_DropItemGenerator.GetAt( i, FALSE, 100.0f ) ) != NULL )
			{
				if ( lpDropItem->dwIndex == dwItemID )
					pItemElem->SetAbilityOption( lpDropItem->dwLevel );		// 추가 능력치 +1, +2 같은거.
			}
		}
		// end

		pItemElem->SetSerialNumber();

		CItem* pItem			= new CItem;
		pItem->m_pItemBase		= pItemElem;
		pItem->m_idOwn			= dwOwnerPlayerID;		// 이 아이템의 소유가 pAttacker(어태커)꺼란걸 표시.
		pItem->m_dwDropTime		= ::GetTickCount();		// 드랍 했을당시의 시간을 기록함.
		pItem->m_bDropMob		= TRUE;		// 몹이 죽어서 떨군 돈은 표시를 해둠.
		pItem->SetIndex( D3DDEVICE, pItem->m_pItemBase->m_dwItemId );

		D3DVECTOR vPos			= kMonster.GetPos();
		vPos.x					+= ( xRandomF( 2.0f ) - 1.0f );
		vPos.z					+= ( xRandomF( 2.0f ) - 1.0f );
// 		vPos.y					+= 0.1f;									//몬스터 y부터 체크하면 바닥으로 떨어진다.vPos.y
// 		vPos.y					= pWorld->GetUnderHeight( vPos );
		pItem->SetPos( vPos );

		if( pWorld->ADDOBJ( pItem, TRUE, kAttacker.GetLayer() ) == FALSE )
		{
			SAFE_DELETE( pItem );
			return;
		}

		if( pItemProp->dwItemKind1 == IK1_WEAPON || pItemProp->dwItemKind1 == IK1_ARMOR || ( pItemProp->dwItemKind1 == IK1_GENERAL && pItemProp->dwItemKind2 == IK2_JEWELRY ) )
		{
			switch( pItemProp->nLog )
			{
			case 1:		g_dpDBClient.SendLogUniqueItem( &kAttacker, pItem, pItemElem->GetAbilityOption() );	// 일반 아이템 +아이템
				break;
			case 2:		g_dpDBClient.SendLogUniqueItem( &kAttacker, pItem, 200 );	// 이펙트 아이템
				break;
			case 3:		g_dpDBClient.SendLogUniqueItem( &kAttacker, pItem, 100 );	// 유니크 아이템
				break;
			}
		}
	}
}


void FLDoDrop::DropGold( CMover & kMonster, FLWSUser & kAttacker, const DWORD dwOwnerPlayerID, const int nGold )
{
	if( nGold <= 0 )
		return;

	CWorld* pWorld = kAttacker.GetWorld();
	if( pWorld == NULL )
	{
		return;
	}

	//1. 골드 아이템 선정
	const DWORD dwItemID	= FLGoldIconSet::GetInstance().GetIconItemID( nGold );
	const PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( dwItemID );
	if( pItemProp == NULL ) {
		return;
	}

	if( !addGoldToFlyingAttacker( kMonster, kAttacker, nGold ) ) {
		FLItemElem* pItemElem	= new FLItemElem;
		pItemElem->m_dwItemId	= dwItemID;
		pItemElem->m_nItemNum	= nGold;	// 돈액수
		pItemElem->m_nHitPoint	= nGold;

		CItem* pItem			= new CItem;
		pItem->m_pItemBase		= pItemElem;
		pItem->m_idOwn			= dwOwnerPlayerID;	// 이 아이템의 소유가 pAttacker(어태커)꺼란걸 표시.
		pItem->m_dwDropTime		= ::GetTickCount();	// 드랍 했을당시의 시간을 기록함.
		pItem->m_bDropMob = TRUE;		// 몹이 죽어서 떨군 돈은 표시를 해둠.
		pItem->SetIndex( D3DDEVICE, pItem->m_pItemBase->m_dwItemId );

		D3DVECTOR vPos			= kMonster.GetPos();
		vPos.x					+= ( xRandomF( 2.0f ) - 1.0f );
		vPos.z					+= ( xRandomF( 2.0f ) - 1.0f );
// 		vPos.y					+= 0.1f;									//몬스터 y부터 체크하면 바닥으로 떨어진다.vPos.y
// 		vPos.y					= pWorld->GetUnderHeight( vPos );
		pItem->SetPos( vPos );
		if( pWorld->ADDOBJ( pItem, TRUE, kAttacker.GetLayer() ) == FALSE )
		{
			SAFE_DELETE( pItem );
		}
	}
}

//
//-- drop item
//{
//	FLItemElem* pItemElem	= new FLItemElem;
//	pItemElem->m_dwItemId	= lpDropItem->dwIndex;
//	pItemElem->m_nItemNum	= (int)( xRandom( dwNum ) + 1 );
//	PT_ITEM_SPEC pItemProp		= pItemElem->GetProp();
//	if( pItemProp )
//	{
//		pItemElem->m_nHitPoint	= pItemProp->dwEndurance;
//		pItemElem->SetRandomOpt( CRandomOptItemGen::GetInstance()->GenRandomOptItem( lpMoverProp->dwLevel, (FLOAT)nProbability / 100.0f, pItemProp, lpMoverProp->dwClass ) );
//	}
//	pItemElem->SetAbilityOption( lpDropItem->dwLevel );		// 추가 능력치 +1, +2 같은거.
//	pItemElem->SetSerialNumber();
//	CItem* pItem	= new CItem;
//	pItem->m_pItemBase	= pItemElem;
//	BOOL bJJim = TRUE;		// 주인이 있다는걸 표시해야하는 템인가.
//	if( lpMoverProp->dwClass == RANK_SUPER )		// 보스몹이 드롭한 아이템은 아무나 먹을수 있다.
//		bJJim = FALSE;
//	if( GetIndex() == OBJECT_INDEX( 196, MI_DEMIAN5 ) || GetIndex() == OBJECT_INDEX( 197, MI_KEAKOON5 ) || GetIndex() == OBJECT_INDEX( 198, MI_MUFFRIN5 ) )
//		bJJim = FALSE;		// 얘들은 이벤트몹이므로 찜안해놔도 된다. 아무나 먹을수 있음
//	if( bJJim )
//	{
//		pItem->m_idOwn = pAttacker->GetId();	// 이 아이템의 소유가 pAttacker(어태커)꺼란걸 표시.
//		pItem->m_dwDropTime = timeGetTime();	// 드랍 했을당시의 시간을 기록함.
//	}
//	pItem->m_bDropMob = TRUE;		// 몹이 죽어서 떨군 돈은 표시를 해둠.
//	if( pItem->m_pItemBase->m_dwItemId == 0 ) FLERROR_LOG( PROGRAM_NAME, _T( "DropItem:2nd %s" ), GetName() );
//	pItem->SetIndex( D3DDEVICE, pItem->m_pItemBase->m_dwItemId );
//
//	vPos = GetPos();
//	vPos.x += ( xRandomF(2.0f) - 1.0f );
//	vPos.z += ( xRandomF(2.0f) - 1.0f );
//	vPos.y = GetPos().y;
//#ifdef __EVENT_MONSTER
//	// 이벤트 몬스터가 드랍한 아이템은 몬스터의 ID를 기억한다(펫이 못 줍게...)
//	if( CEventMonster::GetInstance()->SetEventMonster( lpMoverProp->dwID ) )
//	{
//		// 이벤트 몬스터는 무조건 선점권을 갖는다.
//		pItem->m_idOwn	= pAttacker->GetId();
//		pItem->m_dwDropTime		= timeGetTime();
//
//		pItem->m_IdEventMonster = lpMoverProp->dwID;
//		float fItemDropRange = CEventMonster::GetInstance()->GetItemDropRange(); 
//		vPos = GetPos();
//		vPos.x += ( xRandomF( fItemDropRange ) - (fItemDropRange / 2.0f) );
//		vPos.z += ( xRandomF( fItemDropRange ) - (fItemDropRange / 2.0f) );
//	}
//#endif // __EVENT_MONSTER
//	pItem->SetPos( vPos );
//
//	GetWorld()->ADDOBJ( pItem, TRUE, GetLayer() );
//
//	if( lpDropItem->dwNumber != (DWORD)-1 )
//		nNumber++;
//
//	if( pItemProp->dwItemKind1 == IK1_WEAPON || pItemProp->dwItemKind1 == IK1_ARMOR || ( pItemProp->dwItemKind1 == IK1_GENERAL && pItemProp->dwItemKind2 == IK2_JEWELRY ) )
//	{
//		switch( pItemProp->nLog )
//		{
//		case 1:		g_dpDBClient.SendLogUniqueItem( pAttacker, pItem, pItemElem->GetAbilityOption() );	// 일반 아이템 +아이템
//			break;
//		case 2:		g_dpDBClient.SendLogUniqueItem( pAttacker, pItem, 200 );	// 이펙트 아이템
//			break;
//		case 3:		g_dpDBClient.SendLogUniqueItem( pAttacker, pItem, 100 );	// 유니크 아이템
//			break;
//		}
//	}
//
//	if( nNumber == lpMoverProp->m_DropItemGenerator.m_dwMax )
//		break;
//};
//
//
//drop gold
//// 돈은 무조건떨어져야 한다.
//if( lpDropItem->dtType == DROPTYPE_SEED && k == 0 )
//{
//	int	nSeedID = 0;
//	int nNumGold = lpDropItem->dwNumber + xRandom( lpDropItem->dwNumber2 - lpDropItem->dwNumber );	// Number ~ Number2 사이의 랜덤값.
//	nNumGold	= nNumGold * nPenyaRate / 100;
//#ifdef __S1108_BACK_END_SYSTEM
//	nNumGold	= (int)( nNumGold * prj.m_fGoldDropRate * lpMoverProp->m_fPenya_Rate );
//	if( nNumGold == 0 )
//		continue;
//#else // __S1108_BACK_END_SYSTEM
//	nNumGold	*= prj.m_fGoldDropRate;
//#endif // __S1108_BACK_END_SYSTEM
//
//	nNumGold	= (int)( nNumGold * prj.m_EventLua.GetGoldDropFactor() );	//sun: 9, 이벤트 (루아 스크립트 적용)
//
//	if( lpMoverProp->dwFlying )
//	{
//		if( CanAdd( pAttacker->GetGold(), nNumGold ) )
//		{
//			pAttacker->AddGold( nNumGold );
//			( (FLWSUser*)pAttacker )->AddGoldText( nNumGold );
//		}
//	}
//	else
//	{
//		// 돈액수에 따라 어떤모양의 시드를 사용할지 결정한다.
//		if( nNumGold <= (int)( g_xSpecManager->GetSpecItem( ITEM_INDEX( 12, II_GOLD_SEED1 ) )->dwAbilityMax ) )
//			nSeedID = ITEM_INDEX( 12, II_GOLD_SEED1 );
//		else if( nNumGold <= (int)( g_xSpecManager->GetSpecItem( ITEM_INDEX( 13, II_GOLD_SEED2 ) )->dwAbilityMax ) )
//			nSeedID = ITEM_INDEX( 13, II_GOLD_SEED2 );
//		else if( nNumGold <= (int)( g_xSpecManager->GetSpecItem( ITEM_INDEX( 14, II_GOLD_SEED3 ) )->dwAbilityMax ) )
//			nSeedID = ITEM_INDEX( 14, II_GOLD_SEED3 );
//		else 
//			nSeedID = ITEM_INDEX( 15, II_GOLD_SEED4 );
//
//		FLItemElem* pItemElem	= new FLItemElem;
//		pItemElem->m_dwItemId	= nSeedID;
//		pItemElem->m_nItemNum	= nNumGold;	// 돈액수
//		pItemElem->m_nHitPoint	= nNumGold;
//		CItem* pItem	= new CItem;
//		pItem->m_pItemBase	= pItemElem;
//		BOOL bJJim = TRUE;
//		if( lpMoverProp->dwClass == RANK_SUPER )		// 보스몹이 드롭한 아이템은 아무나 먹을수 있다.
//			bJJim = FALSE;
//		if( GetIndex() == OBJECT_INDEX( 196, MI_DEMIAN5 ) || GetIndex() == OBJECT_INDEX( 197, MI_KEAKOON5 ) || GetIndex() == OBJECT_INDEX( 198, MI_MUFFRIN5 ) )
//			bJJim = FALSE;		// 얘들은 이벤트몹이므로 찜안해놔도 된다. 아무나 먹을수 있음
//		if( bJJim )
//		{
//			pItem->m_idOwn = pAttacker->GetId();	// 이 아이템의 소유가 pAttacker(어태커)꺼란걸 표시.
//			pItem->m_dwDropTime = timeGetTime();	// 드랍 했을당시의 시간을 기록함.
//		}
//		pItem->m_bDropMob = TRUE;		// 몹이 죽어서 떨군 돈은 표시를 해둠.
//		if( pItem->m_pItemBase->m_dwItemId == 0 ) FLERROR_LOG( PROGRAM_NAME, _T( "DropItem: 3rd %s" ), GetName() );
//		pItem->SetIndex( D3DDEVICE, pItem->m_pItemBase->m_dwItemId );
//
//		vPos = GetPos();
//		vPos.x += ( xRandomF(2.0f) - 1.0f );
//		vPos.z += ( xRandomF(2.0f) - 1.0f );
//#ifdef __EVENT_MONSTER
//		// 이벤트 몬스터가 드랍한 아이템은 몬스터의 ID를 기억한다(펫이 못 줍게...)
//		if( CEventMonster::GetInstance()->SetEventMonster( lpMoverProp->dwID ) )
//		{
//			// 이벤트 몬스터는 무조건 선점권을 갖는다.
//			pItem->m_idOwn	= pAttacker->GetId();
//			pItem->m_dwDropTime		= timeGetTime();
//
//			pItem->m_IdEventMonster = lpMoverProp->dwID;
//			float fItemDropRange = CEventMonster::GetInstance()->GetItemDropRange(); 
//			vPos = GetPos();
//			vPos.x += ( xRandomF( fItemDropRange ) - (fItemDropRange / 2.0f) );
//			vPos.z += ( xRandomF( fItemDropRange ) - (fItemDropRange / 2.0f));
//		}
//#endif // __EVENT_MONSTER
//		pItem->SetPos( vPos );
//		GetWorld()->ADDOBJ( pItem, TRUE, GetLayer() );
//	}
//} // DROPTYPE_SEED

//-----------------------------------------------------------------------------------------------------------------------------------------------------//

//< CEventGeneric::GetInstance() >
//{
//
//	if( ( pAttacker->m_nLevel - (int)lpMoverProp->dwLevel ) < 10 )
//	{
//		int nNum	= 0;
//		CEventItem* pEventItem	= NULL;//	= CEventGeneric::GetInstance()->GetItem( &nNum );
//		while( pEventItem = CEventGeneric::GetInstance()->GetItem( &nNum ) )
//		{
//			FLItemElem* pItemElem	= new FLItemElem;
//			pItemElem->m_dwItemId	= pEventItem->m_dwItemId;
//			pItemElem->m_nItemNum	= nNum;
//			pItemElem->m_nHitPoint	= -1;
//			pItemElem->SetSerialNumber();
//			CItem* pItem	= new CItem;
//			pItem->m_pItemBase	= pItemElem;
//			pItem->m_idOwn	= pAttacker->GetId();
//			pItem->m_dwDropTime		= timeGetTime();
//			pItem->m_bDropMob	= TRUE;
//			pItem->SetIndex( D3DDEVICE, pItem->m_pItemBase->m_dwItemId );
//			D3DXVECTOR3 vPos = GetPos();
//			vPos.x += ( xRandomF(2.0f) - 1.0f );
//			vPos.z += ( xRandomF(2.0f) - 1.0f );
//			vPos.y = GetPos().y;
//			pItem->SetPos( vPos );
//			GetWorld()->ADDOBJ( pItem, TRUE, GetLayer() );
//		}
//		//sun: 9, 이벤트 (루아 스크립트 적용)
//		std::map<DWORD, int> mapItemList = prj.m_EventLua.GetItem( lpMoverProp->dwLevel );
//
//		for( std::map<DWORD, int>::iterator it=mapItemList.begin(); it!=mapItemList.end(); it++ )
//		{
//			FLItemElem* pItemElem	= new FLItemElem;
//			pItemElem->m_dwItemId	= it->first;
//			pItemElem->m_nItemNum	= it->second;
//			pItemElem->m_nHitPoint	= -1;
//			pItemElem->SetSerialNumber();
//			CItem* pItem	= new CItem;
//			pItem->m_pItemBase	= pItemElem;
//			pItem->m_idOwn	= pAttacker->GetId();
//			pItem->m_dwDropTime		= timeGetTime();
//			pItem->m_bDropMob	= TRUE;
//			pItem->SetIndex( D3DDEVICE, pItem->m_pItemBase->m_dwItemId );
//			D3DXVECTOR3 vPos = GetPos();
//			vPos.x += ( xRandomF(2.0f) - 1.0f );
//			vPos.z += ( xRandomF(2.0f) - 1.0f );
//			vPos.y = GetPos().y;
//			pItem->SetPos( vPos );
//			GetWorld()->ADDOBJ( pItem, TRUE, GetLayer() );
//		}
//	}
//}
//
//
//
//
//// 퀘스트 아이템 드랍
//QUESTITEM* pQuestItem;
//short nNum;
//u_long uSizeOfQuestItem	= lpMoverProp->m_QuestItemGenerator.GetSize();
//for( u_long i = 0; i < uSizeOfQuestItem; i++ ) 
//{
//	pQuestItem	= lpMoverProp->m_QuestItemGenerator.GetAt( i );
//	LPQUEST lpQuest = pAttacker->GetQuest( pQuestItem->dwQuest );
//	if( lpQuest && lpQuest->m_nState == pQuestItem->dwState )				//퀘스트가 있고 .  상태가 같다면.
//	{
//		DWORD dwProbability	= pQuestItem->dwProbability;
//
//		if( xRandom( 3000000000 ) <= dwProbability ) 
//		{
//			if( pQuestItem->dwNumber == 0 )
//				FLERROR_LOG( PROGRAM_NAME, _T( "%s의 quest item drop %d번째의 dwNumber가 0" ), GetName(), i );
//			nNum	= (short)( xRandom( pQuestItem->dwNumber ) + 1 );
//			if( pQuestItem->dwIndex == 0 )
//			{
//				FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), lpMoverProp->szName );
//				break;
//			}
//
//			// party 일때 
//			CParty* pParty	= g_PartyMng.GetParty( pAttacker->m_idparty );
//			if( pParty && pParty->IsMember( pAttacker->m_idPlayer ) )	// party
//			{
//				for( int j = 0; j < pParty->GetSizeofMember(); ++j )
//				{
//					PartyMember* pPartyMember	= &pParty->m_aMember[j];
//					CMover* pMember		= prj.GetUserByID( pPartyMember->m_uPlayerId );
//					if( !IsValidObj( pMember ) || !IsValidArea( pMember, 64.0f ) )
//						continue;
//
//					LPQUEST pMemberQuest	= pMember->GetQuest( pQuestItem->dwQuest );
//					if( pMemberQuest && pMemberQuest->m_nState == pQuestItem->dwState )
//					{
//						BYTE nId;
//						FLItemElem itemElem;
//						itemElem.m_dwItemId		= pQuestItem->dwIndex;
//						itemElem.m_nItemNum		= nNum;
//						itemElem.m_nHitPoint	= -1;
//						if( pMember->CreateItem( &itemElem, &nId ) == FALSE )
//						{
//							FLItemElem* pItemElem	= new FLItemElem;
//							pItemElem->m_dwItemId	= pQuestItem->dwIndex;
//							pItemElem->m_nItemNum	= nNum;
//							PT_ITEM_SPEC pItemProp		= pItemElem->GetProp();
//							if( pItemProp )
//								pItemElem->m_nHitPoint		= pItemProp->dwEndurance;
//							pItemElem->SetSerialNumber();
//							CItem* pItem	= new CItem;
//							pItem->m_pItemBase	= pItemElem;
//							if( pItemElem->m_dwItemId == 0 )
//								FLERROR_LOG( PROGRAM_NAME, _T( "DropItem:1st %s" ), GetName() );
//							pItem->SetIndex( D3DDEVICE, pItemElem->m_dwItemId );
//							pItem->SetPos( pMember->GetPos() );
//							pItem->SetAngle( (float)( xRandom( 360 ) ) );
//							pItem->m_idHolder	= pMember->m_idPlayer;
//							pItem->m_dwDropTime		= timeGetTime();
//							pMember->GetWorld()->ADDOBJ( pItem, TRUE, GetLayer() );
//						}
//						else
//						{
//							( (FLWSUser*)pMember )->AddDefinedText( TID_EVE_REAPITEM, "\"%s\"", g_xSpecManager->GetSpecItem( pQuestItem->dwIndex )->szName );
//							FLItemBase* pItemBase	= pMember->GetItemId( nId );
//							if( pItemBase )
//							{
//								FLItemElem* pItemElem	= (FLItemElem*)pItemBase;
//								LogItemInfo aLogItem;
//								aLogItem.Action = "Q";
//								aLogItem.SendName = pMember->m_szName;
//								aLogItem.RecvName = "QUEST";
//								aLogItem.WorldId = pMember->GetWorld()->GetID();
//								aLogItem.Gold = aLogItem.Gold2 = pMember->GetGold();
//								g_DPSrvr.OnLogItem( aLogItem, pItemElem, pItemElem->m_nItemNum );
//							}
//						}
//					}
//				}
//			}
//			else	// solo
//			{
//				BYTE nId;
//				FLItemElem itemElem;
//				itemElem.m_dwItemId		= pQuestItem->dwIndex;
//				itemElem.m_nItemNum		= nNum;
//				itemElem.m_nHitPoint	= -1;
//				if( pAttacker->CreateItem( &itemElem, &nId ) == FALSE )
//				{
//					FLItemElem* pItemElem	= new FLItemElem;
//					pItemElem->m_dwItemId	= pQuestItem->dwIndex;
//					pItemElem->m_nItemNum	= nNum;
//					PT_ITEM_SPEC pItemProp		= pItemElem->GetProp();
//					if( pItemProp )
//						pItemElem->m_nHitPoint		= pItemProp->dwEndurance;
//					pItemElem->SetSerialNumber();
//					CItem* pItem	= new CItem;
//					pItem->m_pItemBase	= pItemElem;
//					if( pItemElem->m_dwItemId == 0 )
//						FLERROR_LOG( PROGRAM_NAME, _T( "DropItem:1st %s" ), GetName() );
//					pItem->SetIndex( D3DDEVICE, pItemElem->m_dwItemId );
//					pItem->SetPos( pAttacker->GetPos() );
//					pItem->SetAngle( (float)( xRandom( 360 ) ) );
//					pItem->m_idHolder	= pAttacker->m_idPlayer;
//					pItem->m_dwDropTime		= timeGetTime();
//					pAttacker->GetWorld()->ADDOBJ( pItem, TRUE, GetLayer() );
//				}
//				else
//				{
//					( (FLWSUser*)pAttacker )->AddDefinedText( TID_EVE_REAPITEM, "\"%s\"", g_xSpecManager->GetSpecItem( pQuestItem->dwIndex )->szName );
//					FLItemBase* pItemBase	= pAttacker->GetItemId( nId );
//					if( pItemBase )
//					{
//						FLItemElem* pItemElem	= (FLItemElem*)pItemBase;
//						LogItemInfo aLogItem;
//						aLogItem.Action = "Q";
//						aLogItem.SendName = pAttacker->m_szName;
//						aLogItem.RecvName = "QUEST";
//						aLogItem.WorldId = pAttacker->GetWorld()->GetID();
//						aLogItem.Gold = aLogItem.Gold2 = pAttacker->GetGold();
//						g_DPSrvr.OnLogItem( aLogItem, pItemElem, pItemElem->m_nItemNum );
//					}
//				}
//			}
//		}
//	}
//}
//
//
//< 아이템 사용에 의한 횟수 증가 >
////sun: 9, // __II_SYS_SYS_SCR_GET
//if( pAttacker->HasBuff( BUFF_ITEM, ITEM_INDEX( 30115, II_SYS_SYS_SCR_GET01 ) ) )
//nloop += 1;
//if( pAttacker->HasBuff( BUFF_ITEM, ITEM_INDEX( 30116, II_SYS_SYS_SCR_GET02 ) ) )
//nloop += 2;
//
//nloop += pAttacker->GetAdjParam( DST_GIFTBOX );
//
//
//< 이벤트에 따른 횟수 감소 >
//#ifdef __EVENT_MONSTER
//// 이벤트 몬스터는 설정에 따라 한번만 드랍한다.
//if( CEventMonster::GetInstance()->SetEventMonster( lpMoverProp->dwID ) )
//if( !CEventMonster::GetInstance()->IsGiftBoxAble() )
//nloop = 1;
//#endif // __EVENT_MONSTER
//
//
//< 몹이 죽었을때 아이템 밷기 >
//if( IsNPC() )	// 몹이 죽었을때.. // 주은 아이템 밷기 인듯??????
//{
//	MoverProp* pMoverProp	= GetProp();
//	if( pMoverProp->m_nLoot == 2 )	// 2아 d옵션.  아이템 먹고 뱉기까지 하는 옵션.
//	{
//		DWORD dwStart = m_Inventory.GetMax() - m_Inventory.GetSize();
//		DWORD dwSize = m_Inventory.GetMax();
//		FLItemElem *pElem = NULL;
//		for( DWORD i = dwStart; i < dwSize; i ++ )
//		{
//			pElem = m_Inventory.GetAt( i );
//			if( pElem )
//			{
//				D3DXVECTOR3 vPos = GetPos();
//#ifdef __WORLDSERVER
//				vPos.x += ( xRandomF(2.0f) - 1.0f );
//				vPos.z += ( xRandomF(2.0f) - 1.0f );
//#endif
//				CItem *pItem = DropItem( i, pElem->m_nItemNum, vPos, FALSE );
//				pItem->m_idOwn	= pAttacker->GetId();
//				pItem->m_dwDropTime		= timeGetTime();
//			}
//		}
//	}
//}
//


