#include "StdAfx.h"
#include "FLCharacterServerTransform.h"
#include "../_Common/FLServerDescription.h"
#include "slord.h"
#include "couplehelper.h"
#include "FLItemUsing.h"


FLCharacterServerTransform::FLCharacterServerTransform()
{
}

FLCharacterServerTransform::~FLCharacterServerTransform()
{
}

FLCharacterServerTransform*		FLCharacterServerTransform::GetInstance()
{
	static FLCharacterServerTransform	kCharacterServerTransform;

	return & kCharacterServerTransform;
}

void	FLCharacterServerTransform::OnDoUseCharacterServerTransform( FLWSUser* pUser, DWORD dwItemObjID )
{
	if( CanUseCharacterServerTransform( pUser, dwItemObjID ) == true )
	{
		FLSnapshotCharacterServerTransformNoti kPacket;
		kPacket.dwItemObjID = dwItemObjID;
		pUser->AddPacket( &kPacket );
	}
}

void	FLCharacterServerTransform::OnDoTransformCharacterServer( FLWSUser* pUser, const FLPacketCharacterServerTransformReq* pMsg )
{
	if( pMsg != NULL )
	{
		if( CanUseCharacterServerTransform( pUser, pMsg->dwItemObjID ) == true && CanTransformCharacterServer( pUser, pMsg ) == true )
		{
			DoTransformCharacterServer( pUser, pMsg );
		}
	}
}

bool	FLCharacterServerTransform::CanUseCharacterServerTransform( FLWSUser* pUser, DWORD dwItemObjID )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return false;
	}

	CGuild* pGuild	= pUser->GetGuild();
	if( pGuild != NULL )
	{
		pUser->AddDefinedText( TID_MMI_WORLDCHANGEERROR_TEXT02 );
		return false;
	}

	if( CSLord::Instance()->GetElection() != NULL && CSLord::Instance()->GetElection()->GetCandidate( pUser->m_idPlayer ) != NULL )
	{
		pUser->AddDefinedText( TID_MMI_WORLDCHANGEERROR_TEXT03 );
		return false;
	}

	if( CSLord::Instance()->IsLord( pUser->m_idPlayer ) == TRUE )
	{
		pUser->AddDefinedText( TID_MMI_WORLDCHANGEERROR_TEXT03 );
		return false;
	}

	CCouple* pCouple	= CCoupleHelper::Instance()->GetCouple( pUser->m_idPlayer );
	if( pCouple != NULL )
	{
		pUser->AddDefinedText( TID_MMI_WORLDCHANGEERROR_TEXT02 );
		return false;
	}

	CCampus* pCampus	= CCampusHelper::GetInstance()->GetCampus( pUser->GetCampusId() );
	if( pCampus != NULL )
	{
		pUser->AddDefinedText( TID_MMI_WORLDCHANGEERROR_TEXT02 );
		return false;
	}

	FLItemElem* pItemElem	= pUser->m_Inventory.GetAtId( dwItemObjID );
	if( IsUsableItem( pItemElem ) == FALSE )
	{
		return false;
	}

	if( pItemElem->GetProp() == NULL || pItemElem->GetProp()->dwItemKind3 != IK3_SERVERMOVE )
	{
		return false;
	}

	if( pUser->m_Inventory.IsExistDoNotHaveItemServerTransform() == TRUE )
	{
		pUser->AddDefinedText( TID_MMI_WORLDCHANGEERROR_TEXT01 );
		return false;
	}

	const BYTE byProgramDataSlot	= GET_PLAYER_SLOT( pUser->m_nDBDataSlot );
	if( pUser->m_Bank[byProgramDataSlot].IsExistDoNotHaveItemServerTransform() == TRUE )
	{
		pUser->AddDefinedText( TID_MMI_WORLDCHANGEERROR_TEXT01 );
		return false;
	}

	if( pUser->m_Pocket.IsExistDoNotHaveItemServerTransform() == TRUE )
	{
		pUser->AddDefinedText( TID_MMI_WORLDCHANGEERROR_TEXT01 );
		return false;
	}

	__int64 n64PenyaSum = static_cast<__int64>( static_cast<__int64>( pUser->GetGold() ) + static_cast<__int64>( pUser->m_dwGoldBank[byProgramDataSlot] ) );
	if( n64PenyaSum > MAX_CAN_HAVE_PENYA )
	{
		pUser->AddDefinedText( TID_MMI_WORLDCHANGEERROR_TEXT07 );
		return false;
	}

	return true;
}

bool	FLCharacterServerTransform::CanTransformCharacterServer( FLWSUser* pUser, const FLPacketCharacterServerTransformReq* pMsg )
{
	const T_SERVER_DESC* pCurrentServerDesc		= g_pServerDescription->FindByID( NULL_ID, g_pServerDescription->GetServerIndex() );
	const T_SERVER_DESC* pTransformServerDesc	= g_pServerDescription->FindByID( NULL_ID, pMsg->dwTransformServerIndex );

	if( pCurrentServerDesc != NULL && pTransformServerDesc != NULL )
	{
		if( pCurrentServerDesc->dwMyID != pTransformServerDesc->dwMyID
			&& pTransformServerDesc->bTransformCharacter == true
			&& pTransformServerDesc->lCanConnection != 0 )
		{
			return true;
		}
	}

	pUser->AddDefinedText( TID_MMI_WORLDCHANGEERROR_TEXT06 );
	return false;
}

void	FLCharacterServerTransform::DoTransformCharacterServer( FLWSUser* pUser, const FLPacketCharacterServerTransformReq* pMsg )
{
	FLItemElem* pItemElem	= pUser->m_Inventory.GetAtId( pMsg->dwItemObjID );
	g_DPSrvr.PutItemLog( pUser, "U", "CHARACTER_SERVER_TRANSFORM", pItemElem );

	g_pItemUsing->CompleteUseItem( pUser, pMsg->dwItemObjID );

	pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3( 6971.984f, 99.8f, 3336.884f ), REPLACE_FORCE, nDefaultLayer );

	const T_SERVER_DESC* pCurrentServerDesc		= g_pServerDescription->FindByID( NULL_ID, g_pServerDescription->GetServerIndex() );
	const T_SERVER_DESC* pTransformServerDesc	= g_pServerDescription->FindByID( NULL_ID, pMsg->dwTransformServerIndex );

	FLPacketCharacterServerTransformAck kToClientPacket;
	FLSPrintf( kToClientPacket.szMessage, _countof( kToClientPacket.szMessage )
		, prj.GetText( TID_MMI_WORLDCHANGEERROR_TEXT08 ), pCurrentServerDesc->szName, pTransformServerDesc->szName );

	g_DPSrvr.SendPacket( pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, &kToClientPacket );

	FLPacketCharacterServerTransformNoti kToTransPacket;
	kToTransPacket.dwItemObjID				= pMsg->dwItemObjID;
	kToTransPacket.dwPlayerID				= pUser->m_idPlayer;
	kToTransPacket.dwTransformServerIndex	= pMsg->dwTransformServerIndex;
	FLStrcpy( kToTransPacket.szAccount, _countof( kToTransPacket.szAccount ), pUser->m_playAccount.lpszAccount );
	FLStrcpy( kToTransPacket.szBankPassword, _countof( kToTransPacket.szBankPassword ), pUser->m_szBankPass );

	g_dpDBClient.SendPacket( &kToTransPacket );

	g_DPSrvr.QueryDestroyPlayer( pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer );
}
