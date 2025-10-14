
#include "StdAfx.h"
#include "FLItemMergeRandomOption.h"
#include "../_CommonDefine/Packet/FLPacketItemOption.h"


FLItemMergeRandomOption::FLItemMergeRandomOption()
{

}

FLItemMergeRandomOption::~FLItemMergeRandomOption()
{
	Clear();
}

bool	FLItemMergeRandomOption::LoadScript()
{
	CScript s;

	const TCHAR szFileName[]	= _T( "ItemMergeRandomOption.txt" );

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), szFileName );

	if( s.Load( szFullPath ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return false;
	}

	s.GetToken();	// subject or FINISHED

	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "MERGE_RANDOMOPTION" ) )
		{
			T_ITEM_MERGE_DATA tItemMergeData;

			tItemMergeData.dwMergeType	= MERGE_RANDOMOPTION;
			tItemMergeData.dwMergeCount	= s.GetNumber();	// load count
			if( tItemMergeData.dwMergeCount > MAX_ITEM_MERGE_COUNT )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Merge Count Overflow. Fix Size:[%d]" ), tItemMergeData.dwMergeCount );
				return false;
			}
			tItemMergeData.dwMergeProb	= s.GetNumber();	// load prob

			s.GetToken();	// {

			DWORD dwParts = s.GetNumber();		// load parts

			while( *s.token != '}' )
			{
				if( dwParts != 0 && dwParts <= MAX_HUMAN_PARTS )
				{
					s.GetToken();	// {

					T_ITEM_DATA	tItemData = { 0 };
					//mem_set( &tItemData, 0, sizeof( tItemData ) );

					tItemData.dwParts		= dwParts;
					tItemData.dwItemKind1	= s.GetNumber();	// load itemkind1
					tItemData.dwItemKind2	= s.GetNumber();	// load itemkind2
					tItemData.dwItemKind3	= s.GetNumber();	// load itemkind3

					tItemMergeData.vectorItemData.push_back( tItemData );
					
					s.GetToken();	// }

					dwParts = s.GetNumber();		// load parts
				}
				else
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "Item Parts Invalid. dwParts:[%d]" ), dwParts );
					return false;
				}
			}

			if( tItemMergeData.vectorItemData.size() != tItemMergeData.dwMergeCount )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Merge Count Different. Fix Size:[%d], Load Count:[%d]" ),
					tItemMergeData.dwMergeCount, tItemMergeData.vectorItemData.size() );
				return false;
			}
			m_listItemMergeData.push_back( tItemMergeData );
		}
		s.GetToken();
	}

	return true;
}

void	FLItemMergeRandomOption::OnItemMerge( FLWSUser* pUser, PT_PACKET_ITEM_MERGE ptItemMerge )
{
	int nResult = ERR_GENERAL_ERROR;

	nResult = DoItemMerge( pUser, ptItemMerge );

	switch( nResult )
	{
	case ERR_SUCCESS:
		pUser->AddDefinedText( TID_COSTUME_COSTUMEMIXCOMPLETE );
		break;

	case ERR_FAIL_PROTECT:
		pUser->AddDefinedText( TID_MMI_COSTUME_SAFEMIXITEM03 );
		break;

	case ERR_FAIL_DESTROY:
		pUser->AddDefinedText( TID_MMI_COSTUME_COSTUMEMIXFAILED );
		break;

	case ERR_DONOT_VENDOR:
		pUser->AddDefinedText( TID_GAME_TRADELIMITUSING );
		break;

	case ERR_DONOT_EQUIP:
		pUser->AddDefinedText( TID_GAME_EQUIPPUT );
		break;

	case ERR_ITEM_MISMATCH:
		pUser->AddDefinedText( TID_COSTUME_NOTSAMEITEM );
		break;

	case ERR_NOT_RANDOMOPTION:
		pUser->AddDefinedText( TID_MMI_COSTUME_UNCONDITIONALINCHANT );
		break;

	case ERR_ITEM_DUPLICATION:
		pUser->AddDefinedText( TID_COSTUME_ITEMOVERLAP );
		break;

	case ERR_SIZE_MISMATCH:
		pUser->AddDefinedText( TID_MMI_COSTUME_MIXLEVELDIFFERENT );
		break;

	case ERR_OVER_RANDOMOPTION:
		pUser->AddDefinedText( TID_MMI_COSTUME_MIXLEVELOVER );
		break;

	case ERR_NOT_EXIST_MERGE_DATA:
	default:
		pUser->AddDefinedText( TID_COSTUME_REGISTERNOTITEM );
		break;
	}

	pUser->AddResultItemMerge( ptItemMerge );
}

//////////////////////////////////////////////////////////////////////////
void	FLItemMergeRandomOption::Clear()
{
	for( ItemMergeDataItr it = m_listItemMergeData.begin(); it != m_listItemMergeData.end(); ++it )
	{
		it->vectorItemData.clear();
	}

	m_listItemMergeData.clear();
}

int		FLItemMergeRandomOption::IsItemMergeAbleState( FLWSUser* pUser, const PT_PACKET_ITEM_MERGE ptItemMerge )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return ERR_USER_INVALID;
	}

	if( pUser->IsDie() == TRUE )
	{
		return ERR_USER_INVALID;
	}

	if( pUser->m_vtInfo.GetOther() != NULL || pUser->m_vtInfo.VendorIsVendor() == TRUE )
	{
		return ERR_DONOT_VENDOR;
	}

	if( ptItemMerge == NULL || ptItemMerge->dwMergeCount > MAX_ITEM_MERGE_COUNT )
	{
		return ERR_DATA_INVALID;
	}

	DWORD dwSrcObjId	= NULL_ID;
	DWORD dwDestObjId	= NULL_ID;

	DWORD dwExistRandomOptionSize = NULL_ID;

	for( DWORD i = 0; i < ptItemMerge->dwMergeCount; ++i )
	{
		if( ptItemMerge->tItemMergeInfo[ i ].dwObjIdSrc == ptItemMerge->tItemMergeInfo[ i ].dwObjIdDest )
		{
			return ERR_ITEM_DUPLICATION;
		}

		if( dwSrcObjId != NULL_ID && dwSrcObjId == ptItemMerge->tItemMergeInfo[ i ].dwObjIdSrc )
		{
			return ERR_ITEM_DUPLICATION;
		}

		if( dwDestObjId != NULL_ID && dwDestObjId == ptItemMerge->tItemMergeInfo[ i ].dwObjIdDest )
		{
			return ERR_ITEM_DUPLICATION;
		}

		dwSrcObjId	= ptItemMerge->tItemMergeInfo[ i ].dwObjIdSrc;
		dwDestObjId	= ptItemMerge->tItemMergeInfo[ i ].dwObjIdDest;

		FLItemElem* pItemElemSrc		= pUser->m_Inventory.GetAtId( ptItemMerge->tItemMergeInfo[ i ].dwObjIdSrc );
		FLItemElem* pItemElemDest	= pUser->m_Inventory.GetAtId( ptItemMerge->tItemMergeInfo[ i ].dwObjIdDest );

		if( IsUsableItem( pItemElemSrc ) == FALSE || IsUsableItem( pItemElemDest ) == FALSE )
		{
			return ERR_ITEM_INVALID;
		}

		PT_ITEM_SPEC pItemPropSrc	= pItemElemSrc->GetProp();
		PT_ITEM_SPEC pItemPropDest	= pItemElemDest->GetProp();

		if( pItemPropSrc == NULL || pItemPropDest == NULL )
		{
			return ERR_ITEM_INVALID;
		}

		if( pUser->m_Inventory.IsEquip( ptItemMerge->tItemMergeInfo[ i ].dwObjIdSrc ) == TRUE
			|| pUser->m_Inventory.IsEquip( ptItemMerge->tItemMergeInfo[ i ].dwObjIdDest ) == TRUE )
		{
			return ERR_DONOT_EQUIP;
		}

		if( pItemPropSrc->dwParts != pItemPropDest->dwParts
			|| pItemPropSrc->dwItemKind1 != pItemPropDest->dwItemKind1
			|| pItemPropSrc->dwItemKind2 != pItemPropDest->dwItemKind2
			|| pItemPropSrc->dwItemKind3 != pItemPropDest->dwItemKind3
			)
		{
			return ERR_ITEM_MISMATCH;
		}

		if( pItemElemSrc->IsSetRandomOptionExtension() == false || pItemElemDest->IsSetRandomOptionExtension() == false )
		{
			return ERR_NOT_RANDOMOPTION;
		}

		DWORD dwSrcRandomOptSize	= pItemElemSrc->GetRandomOptionExtensionSize();
		DWORD dwDestRandomOptSize	= pItemElemDest->GetRandomOptionExtensionSize();

 		if( dwSrcRandomOptSize != dwDestRandomOptSize )
 		{
 			return ERR_SIZE_MISMATCH;
 		}

		if( dwExistRandomOptionSize != NULL_ID )
		{
			if( dwExistRandomOptionSize != dwSrcRandomOptSize )
			{
				return ERR_SIZE_MISMATCH;
			}
		}
		dwExistRandomOptionSize = dwSrcRandomOptSize;

		if( ( dwSrcRandomOptSize + dwDestRandomOptSize ) > MAX_RANDOMOPTION_MERGE_SIZE )
		{
			return ERR_OVER_RANDOMOPTION;
		}

		if( pItemElemSrc->IsSetSafeGuardRandomOptionExtensionFlag() == true
			|| pItemElemDest->IsSetSafeGuardRandomOptionExtensionFlag() == true 
			|| pItemElemSrc->GetRandomOptionExtensionFlag() != pItemElemDest->GetRandomOptionExtensionFlag() )
		{
			return ERR_ITEM_MISMATCH;
		}
	}

	return ERR_VALID_STATE;
}

DWORD	FLItemMergeRandomOption::GetItemMergeProb( FLWSUser* pUser, const PT_PACKET_ITEM_MERGE ptItemMerge )
{
	bool bSame = false;
	DWORD dwSuccessProb = NULL_ID;

	for( ItemMergeDataItr itList = m_listItemMergeData.begin(); itList != m_listItemMergeData.end(); ++itList )
	{
		T_ITEM_MERGE_DATA tMergeData = *itList;

		if( tMergeData.dwMergeType == ptItemMerge->dwMergeType && tMergeData.vectorItemData.size() == ptItemMerge->dwMergeCount )
		{
			for( DWORD i = 0; i < tMergeData.vectorItemData.size(); ++i )
			{
				FLItemElem* pItemElemSrc		= pUser->m_Inventory.GetAtId( ptItemMerge->tItemMergeInfo[ i ].dwObjIdSrc );
				FLItemElem* pItemElemDest	= pUser->m_Inventory.GetAtId( ptItemMerge->tItemMergeInfo[ i ].dwObjIdDest );
				if( IsUsableItem( pItemElemSrc ) == FALSE || IsUsableItem( pItemElemDest ) == FALSE )
				{
					return dwSuccessProb;
				}

				PT_ITEM_SPEC pItemPropSrc	= pItemElemSrc->GetProp();
				PT_ITEM_SPEC pItemPropDest	= pItemElemDest->GetProp();
				if( pItemPropSrc == NULL || pItemPropDest == NULL )
				{
					return dwSuccessProb;
				}

				if( pItemPropSrc->dwParts != pItemPropDest->dwParts
					|| pItemPropSrc->dwItemKind1 != pItemPropDest->dwItemKind1
					|| pItemPropSrc->dwItemKind2 != pItemPropDest->dwItemKind2
					|| pItemPropSrc->dwItemKind3 != pItemPropDest->dwItemKind3
					)
				{
					return dwSuccessProb;
				}
				
				if( pItemPropSrc->dwParts == tMergeData.vectorItemData[ i ].dwParts
					&& pItemPropSrc->dwItemKind1 == tMergeData.vectorItemData[ i ].dwItemKind1
					&& ( pItemPropSrc->dwItemKind2 == tMergeData.vectorItemData[ i ].dwItemKind2 || tMergeData.vectorItemData[ i ].dwItemKind2 == NULL_ID )
					&& ( pItemPropSrc->dwItemKind3 == tMergeData.vectorItemData[ i ].dwItemKind3 || tMergeData.vectorItemData[ i ].dwItemKind3 == NULL_ID )
					)
				{
					bSame = true;
				}
				else
				{
					bSame = false;
					break;
				}
			}
		}

		if( bSame == true )
		{
			dwSuccessProb = tMergeData.dwMergeProb;
			break;
		}
	}

	return dwSuccessProb;
}

int		FLItemMergeRandomOption::DoItemMerge( FLWSUser* pUser, PT_PACKET_ITEM_MERGE ptItemMerge )
{
	int nResult = ERR_GENERAL_ERROR;

	nResult = IsItemMergeAbleState( pUser, ptItemMerge );
	if( nResult != ERR_VALID_STATE )
	{
		return nResult;
	}

	const DWORD dwSuccessProb = GetItemMergeProb( pUser, ptItemMerge );
	if( dwSuccessProb == NULL_ID )
	{
		return ERR_NOT_EXIST_MERGE_DATA;
	}
	
	bool bSuccess = false;
	bool bProtect = false;

	const DWORD dwProb	= xRandom( MAX_MERGE_PROB );
	if( dwProb < dwSuccessProb )
	{
		bSuccess = true;
	}

	if( pUser->HasBuffByIk3( IK3_COSTUME_DEFENDER ) == TRUE )
	{
		pUser->RemoveIk3Buffs( IK3_COSTUME_DEFENDER );

		bProtect = true;
	}

	for( DWORD i = 0; i < ptItemMerge->dwMergeCount; ++i )
	{
		FLItemElem* pItemElemSrc		= pUser->m_Inventory.GetAtId( ptItemMerge->tItemMergeInfo[ i ].dwObjIdSrc );
		FLItemElem* pItemElemDest	= pUser->m_Inventory.GetAtId( ptItemMerge->tItemMergeInfo[ i ].dwObjIdDest );

		if( IsUsableItem( pItemElemSrc ) == FALSE || IsUsableItem( pItemElemDest ) == FALSE )
		{
			return ERR_ITEM_INVALID;
		}

		if( bSuccess == true )
		{
			if( pItemElemSrc->IsSetRandomOptionExtension() == true && pItemElemDest->IsSetRandomOptionExtension() == true )
			{
				for( size_t Nth = 0; Nth < pItemElemDest->GetRandomOptionExtensionSize(); ++Nth )
				{
					T_RANDOMOPTION_EXT kRandomOptionExtension;

					kRandomOptionExtension.wDstID		= pItemElemDest->GetRandomOptionExtensionDstID( Nth );
					kRandomOptionExtension.shAdjValue	= pItemElemDest->GetRandomOptionExtensionAdjValue( Nth );

					pItemElemSrc->SetRandomOptionExtension( kRandomOptionExtension );
				}
			}

			FLSnapshotItemUpgradeRandomOptionAck toClient;
			toClient.dwItemObjID		= pItemElemSrc->m_dwObjId;
			toClient.dwSerialNumber		= pItemElemSrc->GetSerialNumber();
			pItemElemSrc->GetRandomOption( toClient.kRandomOption );
			pUser->AddPacket( &toClient );

// 			int nDestRandomOptSize	= g_xRandomOptionProperty->GetRandomOptionSize( pItemElemDest->GetRandomOptItemId() );
// 
// 			for( int j = 0; j < nDestRandomOptSize; ++j )
// 			{
// 				int nDst = 0;
// 				int nAdj = 0;
// 				if( g_xRandomOptionProperty->GetParam( pItemElemDest->GetRandomOptItemId(), j, &nDst, &nAdj ) == TRUE )
// 				{
// 					g_xRandomOptionProperty->SetParam( pItemElemSrc->GetRandomOptItemIdPtr(), nDst, nAdj );
// 				}
// 			}

//			pUser->UpdateItemEx( (BYTE)( pItemElemSrc->m_dwObjId ), UI_RANDOMOPTITEMID, pItemElemSrc->GetRandomOptItemId() );

			// log
			LogItemInfo aLogItem;
			//aLogItem.Action = "M";
			//aLogItem.SendName = pUser->GetName();
			//aLogItem.RecvName = "MERGE_RANDOMOPTION_SUCCESS";
			FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "M" );
			FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
			FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "MERGE_RANDOMOPTION_SUCCESS" );

			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
					
			
			g_DPSrvr.OnLogItem( aLogItem, pItemElemSrc, pItemElemSrc->m_nItemNum );
			g_DPSrvr.OnLogItem( aLogItem, pItemElemDest, pItemElemDest->m_nItemNum );
			
			pUser->RemoveItem( pItemElemDest->m_dwObjId, pItemElemDest->m_nItemNum );

			ptItemMerge->bResult = true;

			nResult = ERR_SUCCESS;
		}
		else
		{
			if( bProtect == true )		// protect
			{
				// log
				LogItemInfo aLogItem;
				//aLogItem.Action = "M";
				//aLogItem.SendName = pUser->GetName();
				//aLogItem.RecvName = "MERGE_RANDOMOPTION_FAIL_PROTECT";
				FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "M" );
				FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
				FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "MERGE_RANDOMOPTION_FAIL_PROTECT" );

				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
								
				g_DPSrvr.OnLogItem( aLogItem, pItemElemSrc, pItemElemSrc->m_nItemNum );
				g_DPSrvr.OnLogItem( aLogItem, pItemElemDest, pItemElemDest->m_nItemNum );

				nResult = ERR_FAIL_PROTECT;
			}
			else		// destroy
			{
				// log
				LogItemInfo aLogItem;
				//aLogItem.Action = "M";
				//aLogItem.SendName = pUser->GetName();
				//aLogItem.RecvName = "MERGE_RANDOMOPTION_FAIL_DESTROY";
				FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "M" );
				FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
				FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "MERGE_RANDOMOPTION_FAIL_DESTROY" );

				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();

				g_DPSrvr.OnLogItem( aLogItem, pItemElemSrc, pItemElemSrc->m_nItemNum );
				g_DPSrvr.OnLogItem( aLogItem, pItemElemDest, pItemElemDest->m_nItemNum );

				pUser->RemoveItem( pItemElemSrc->m_dwObjId, pItemElemSrc->m_nItemNum );
				pUser->RemoveItem( pItemElemDest->m_dwObjId, pItemElemDest->m_nItemNum );

				nResult = ERR_FAIL_DESTROY;
			}

			ptItemMerge->bResult = false;
		}
	}

	return nResult;
}

