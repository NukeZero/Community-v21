#include "StdAfx.h"

#include "FLDBCommonFuntion.h"

#include "playerdata.h"

void MakeItemLogInfo( LogItemInfo & o_info, const char * pszAction, const char * pszSender, const char * pszRecver
					 , const u_long uSenderID, const u_long uRecverID
					 , const DWORD dwWorldID, FLItemElem & kItemElem, const int nItemNum, const int nPayedGold, const int nPay )
{
	if( pszAction != NULL )
		FLStrcpy( o_info.Action, _countof( o_info.Action ), pszAction );
	if( pszSender != NULL )
		FLStrcpy( o_info.SendName, _countof( o_info.SendName ), pszSender );
	if( pszRecver != NULL )
		FLStrcpy( o_info.RecvName, _countof( o_info.RecvName ), pszRecver );

	o_info.WorldId			= dwWorldID;
	o_info.Gold				= nPayedGold + nPay;
	o_info.Gold2			= nPayedGold;

	o_info.idSendPlayer		= uSenderID;
	o_info.idRecvPlayer		= uRecverID;


	if( kItemElem.IsEmpty() == FALSE )
	{
		o_info.kLogItem.CopyItemInfo( kItemElem );

		o_info.kLogItem.nQuantity	= ( nItemNum <= 0 ) ? kItemElem.m_nItemNum : nItemNum;
		
// 		o_info.ItemNo			= kItemElem.GetSerialNumber();
// 		FLSPrintf( o_info.szItemName, _countof( o_info.szItemName ), "%d", kItemElem.GetProp()->dwID );
// 		o_info.itemNumber		= ( nItemNum == 0 ) ? kItemElem.m_nItemNum : nItemNum;
// 		o_info.nItemResist		= kItemElem.m_byItemResist;
// 		o_info.nResistAbilityOption = kItemElem.m_nResistAbilityOption;
// 		o_info.nAbilityOption	= kItemElem.GetAbilityOption();
// 		o_info.Negudo			= kItemElem.m_nHitPoint;
// 		o_info.MaxNegudo		= kItemElem.m_nRepair;
// 		o_info.m_bCharged		= kItemElem.m_bCharged;
// 		o_info.m_dwKeepTime		= kItemElem.m_dwKeepTime;
// 		o_info.nPiercedSize		= kItemElem.GetGeneralPiercingSize();
// 		for( size_t i=0; i<kItemElem.GetGeneralPiercingSize(); i++ )
// 			o_info.adwItemId[i] = kItemElem.GetGeneralPiercingItemID( i );
// 		o_info.nUMPiercedSize = kItemElem.GetUltimatePiercingSize();
// 		for( size_t i=0; i<kItemElem.GetUltimatePiercingSize(); i++ )
// 			o_info.adwUMItemId[i] = kItemElem.GetUltimatePiercingItemID( i );
// 
// 		if( kItemElem.m_pPet )
// 		{
// 			CPet* pPet = kItemElem.m_pPet;
// 
// 			o_info.nPetKind = pPet->GetKind();
// 			o_info.nPetLevel = pPet->GetLevel();
// 			o_info.dwPetExp = pPet->GetExp();
// 			o_info.wPetEnergy = pPet->GetEnergy();
// 			o_info.wPetLife = pPet->GetLife();
// 			o_info.nPetAL_D = pPet->GetAvailLevel( PL_D );
// 			o_info.nPetAL_C = pPet->GetAvailLevel( PL_C );
// 			o_info.nPetAL_B = pPet->GetAvailLevel( PL_B );
// 			o_info.nPetAL_A = pPet->GetAvailLevel( PL_A );
// 			o_info.nPetAL_S = pPet->GetAvailLevel( PL_S );
// 		}
// 		// mirchang_100514 TransformVisPet_Log
// 		else if( kItemElem.IsTransformVisPet() == TRUE )
// 		{
// 			o_info.nPetKind = (BYTE)100;
// 		}
// 
// 		o_info.dwCouplePlayerId				= kItemElem.GetCoupleId();
// 		o_info.nLevelDown					= kItemElem.GetLevelDown();
// 		o_info.wRandomOptionOriginId		= kItemElem.GetRandomOptionOriginID();
// 		o_info.nRandomOptionExtensionSize	= kItemElem.GetRandomOptionExtensionSize();
// 		o_info.wRandomOptionExtensionFlag	= kItemElem.GetRandomOptionExtensionFlag();
// 
// 		for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_SIZE; ++Nth )
// 		{
// 			o_info.awDstID[Nth]	= kItemElem.GetRandomOptionExtensionDstID( Nth );
// 			o_info.ashAdjValue[Nth]= kItemElem.GetRandomOptionExtensionAdjValue( Nth );
// 		}
	}


	// º¸Á¤
// 	if( _tcslen( o_info.szItemName ) == 0 )
// 	{
// 		FLSPrintf( o_info.szItemName, _countof( o_info.szItemName ), "%d", -1 );
// 	}
}
