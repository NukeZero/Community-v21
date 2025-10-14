
#include "StdAfx.h"
#include "FLItemExtract.h"


FLItemExtract::FLItemExtract()
{
	Clear();
}

FLItemExtract::~FLItemExtract()
{
	Clear();
}

FLItemExtract*	FLItemExtract::GetInstance()
{
	static FLItemExtract xItemExtract;

	return & xItemExtract;
}

void	FLItemExtract::Clear()
{
	m_mapOperExtractProb.clear();
}

bool	FLItemExtract::LoadScript()
{
	CLuaBase kLua;

	const TCHAR szFileName[]	= _T( "ItemExtract.lua" );

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), szFileName );

	if( kLua.RunScript( szFullPath ) != 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// 오퍼 추출 확률
	kLua.GetGloabal( "tOperExtract" );
	kLua.PushNil();
	while( kLua.TableLoop( -2 ) )
	{
		T_OPER_EXTRACT_PROB tOperExtractProb;

		int	nItemKind	= static_cast<int>( CScript::GetDefineNum( kLua.GetFieldToString( -1, "strItemKind" ) ) );

		kLua.GetField( -1, "tOperProb" );
		kLua.PushNil();
		while( kLua.TableLoop( -2 ) )
		{
			tOperExtractProb.nItemMinLevel			= static_cast<int>( kLua.GetFieldToNumber( -1, "dwItemMinLevel" ) );
			tOperExtractProb.nItemMaxLevel			= static_cast<int>( kLua.GetFieldToNumber( -1, "dwItemMaxLevel" ) );
			tOperExtractProb.nOperExtractProb		= static_cast<int>( kLua.GetFieldToNumber( -1, "nOperProb" ) );
			tOperExtractProb.nOperMinNum			= static_cast<int>( kLua.GetFieldToNumber( -1, "nOperMinNum" ) );
			tOperExtractProb.nOperMaxNum			= static_cast<int>( kLua.GetFieldToNumber( -1, "nOperMaxNum" ) );
			tOperExtractProb.nOperPieceExtractProb	= static_cast<int>( kLua.GetFieldToNumber( -1, "nOperPieceProb" ) );
			tOperExtractProb.nOperPieceMinNum		= static_cast<int>( kLua.GetFieldToNumber( -1, "nOperPieceMinNum" ) );
			tOperExtractProb.nOperPieceMaxNum		= static_cast<int>( kLua.GetFieldToNumber( -1, "nOperPieceMaxNum" ) );

			MAP_OPER_EXTRACT_PROB::iterator it = m_mapOperExtractProb.find( nItemKind );
			if( it != m_mapOperExtractProb.end() )
			{
				it->second.push_back( tOperExtractProb );
			}
			else
			{
				std::vector<T_OPER_EXTRACT_PROB>		vectOperExtractProb;
				vectOperExtractProb.push_back( tOperExtractProb );
				m_mapOperExtractProb.insert( MAP_OPER_EXTRACT_PROB::value_type( nItemKind, vectOperExtractProb ) );
			}

			kLua.Pop( 1 );
		}
		kLua.Pop( 1 );

		kLua.Pop( 1 );
	}
	kLua.Pop( 0 );
	//////////////////////////////////////////////////////////////////////////

	return true;
}

bool	FLItemExtract::OnExtractOperByWeapon( FLWSUser* pUser, const PT_PACKET_OPER_EXTRACT ptOperExtract )
{
	if( IsValidObj( pUser ) == TRUE && ptOperExtract != NULL )
	{
		FLItemElem* pMaterial	= pUser->m_Inventory.GetAtId( ptOperExtract->dwMaterialObjid );

		if( IsUsableItem( pMaterial ) == TRUE )
		{
			if( pUser->m_Inventory.IsEquip( ptOperExtract->dwMaterialObjid ) == TRUE )
			{
				pUser->AddDefinedText( TID_GAME_EQUIPPUT );
				return false;
			}

			if( IsExtractAble( pUser, EXTRACT_WEAPON, pMaterial ) == false )
			{
				pUser->AddDefinedText( TID_MMI_NEWSMELT_OPEREXTRACT01 );
				return false;
			}

			int nProb = (int)( xRandom( 10000 ) );
			FLItemElem itemElem;

			if( GetOperOrOperPiece( pMaterial, EXTRACT_WEAPON, nProb, itemElem ) == true )
			{
				// success
				PT_ITEM_SPEC pOperProp = g_xSpecManager->GetSpecItem( itemElem.m_dwItemId );

				if( pOperProp != NULL && pUser->CreateItem( &itemElem ) == TRUE )
				{
					if( pOperProp->dwItemKind3 == IK3_OPER )
					{
						pUser->AddDefinedText( TID_MMI_NEWSMELT_OPEREXTRACT05, "%d", itemElem.m_nItemNum );
					}
					else if( pOperProp->dwItemKind3 == IK3_OPERCRYSTAL )
					{
						pUser->AddDefinedText( TID_MMI_NEWSMELT_OPEREXTRACT04, "%d", itemElem.m_nItemNum );
					}

					// log...
					g_DPSrvr.PutItemLog( pUser, "C", "SUCCESS_EXTRACT_OPER", &itemElem, itemElem.m_nItemNum );
					g_DPSrvr.PutItemLog( pUser, "R", "SUCCESS_EXTRACT_OPER", pMaterial, 1 );

					pUser->RemoveItem( ptOperExtract->dwMaterialObjid, 1 );
					return true;
				}
				else
				{
					pUser->AddDefinedText( TID_GAME_LACKSPACE );
					return false;
				}
			}
			else
			{
				// fail..
				pUser->AddDefinedText( TID_MMI_NEWSMELT_OPEREXTRACT03 );

				g_DPSrvr.PutItemLog( pUser, "R", "FAIL_EXTRACT_OPER", pMaterial, 1 );
				pUser->RemoveItem( ptOperExtract->dwMaterialObjid, 1 );
				return false;
			}
		}
		else
		{
			// 거래중인 아이템인가..
			pUser->AddDefinedText( TID_MMI_NEWSMELT_CID01 );
			return false;
		}
	}
	return false;
}

bool	FLItemExtract::OnExtractOperByArmor( FLWSUser* pUser, const PT_PACKET_OPER_EXTRACT ptOperExtract )
{
	if( IsValidObj( pUser ) == TRUE && ptOperExtract != NULL )
	{
		FLItemElem* pMaterial	= pUser->m_Inventory.GetAtId( ptOperExtract->dwMaterialObjid );

		if( IsUsableItem( pMaterial ) == TRUE )
		{
			if( pUser->m_Inventory.IsEquip( ptOperExtract->dwMaterialObjid ) == TRUE )
			{
				pUser->AddDefinedText( TID_GAME_EQUIPPUT );
				return false;
			}

			if( IsExtractAble( pUser, EXTRACT_ARMOR, pMaterial ) == false )
			{
				pUser->AddDefinedText( TID_MMI_NEWSMELT_OPEREXTRACT01 );
				return false;
			}

			int nProb = (int)( xRandom( 10000 ) );
			FLItemElem itemElem;
			if( GetOperOrOperPiece( pMaterial, EXTRACT_ARMOR, nProb, itemElem ) == true )
			{
				// success
				PT_ITEM_SPEC pOperProp = g_xSpecManager->GetSpecItem( itemElem.m_dwItemId );
				if( pOperProp != NULL && pUser->CreateItem( &itemElem ) == TRUE )
				{
					if( pOperProp->dwItemKind3 == IK3_OPER )
					{
						pUser->AddDefinedText( TID_MMI_NEWSMELT_OPEREXTRACT05, "%d", itemElem.m_nItemNum );
					}
					else if( pOperProp->dwItemKind3 == IK3_OPERCRYSTAL )
					{
						pUser->AddDefinedText( TID_MMI_NEWSMELT_OPEREXTRACT04, "%d", itemElem.m_nItemNum );
					}

					// log...
					g_DPSrvr.PutItemLog( pUser, "C", "SUCCESS_EXTRACT_OPER", &itemElem, itemElem.m_nItemNum );
					g_DPSrvr.PutItemLog( pUser, "R", "SUCCESS_EXTRACT_OPER", pMaterial, 1 );

					pUser->RemoveItem( ptOperExtract->dwMaterialObjid, 1 );
					return true;
				}
				else
				{
					pUser->AddDefinedText( TID_GAME_LACKSPACE );
					return false;
				}
			}
			else
			{
				// fail..
				pUser->AddDefinedText( TID_MMI_NEWSMELT_OPEREXTRACT03 );
				g_DPSrvr.PutItemLog( pUser, "R", "FAIL_EXTRACT_OPER", pMaterial, 1 );
				pUser->RemoveItem( ptOperExtract->dwMaterialObjid, 1 );
				return false;
			}
		}
		else
		{
			// 거래중인 아이템인가..
			pUser->AddDefinedText( TID_MMI_NEWSMELT_CID01 );
			return false;
		}
	}
	return false;
}

bool	FLItemExtract::OnCreateOper( FLWSUser* pUser, const PT_PACKET_OPER_CREATE ptOperCreate )
{
	if( IsValidObj( pUser ) == TRUE && ptOperCreate != NULL )
	{
		FLItemElem* pOperPiece = pUser->m_Inventory.GetAtId( ptOperCreate->dwOperPieceObjid );
		FLItemElem* pOperPieceCombine = pUser->m_Inventory.GetAtId( ptOperCreate->dwOperPieceCombineObjid );

		if( IsUsableItem( pOperPiece ) == TRUE && IsUsableItem( pOperPieceCombine ) == TRUE )
		{
			if( IsMixAble( pUser, MIX_CREATE_OPER, pOperPiece, pOperPieceCombine ) == true )
			{
				FLItemElem itemElem;
				itemElem.m_dwItemId = ITEM_INDEX( 7040, II_GEN_MAT_OPER );
				itemElem.m_nItemNum = 1;
				itemElem.SetSerialNumber();

				if( pUser->CreateItem( &itemElem ) == TRUE )
				{
					pUser->AddDefinedText( TID_MMI_NEWSMELT_OPER04 );
					// log...
					g_DPSrvr.PutItemLog( pUser, "C", "CREATE_OPER", &itemElem, itemElem.m_nItemNum );
					g_DPSrvr.PutItemLog( pUser, "R", "CREATE_OPER", pOperPiece, 1 );
					g_DPSrvr.PutItemLog( pUser, "R", "CREATE_OPER", pOperPieceCombine, 1 );

					pUser->RemoveItem( ptOperCreate->dwOperPieceObjid, 1 );
					pUser->RemoveItem( ptOperCreate->dwOperPieceCombineObjid, 1 );
					return true;
				}
				else
				{
					pUser->AddDefinedText( TID_GAME_LACKSPACE );
					return false;
				}
			}
			else
			{
				// 조합 불가능..
				return false;
			}
		}
		else
		{
			// 거래중인 아이템인가..
			pUser->AddDefinedText( TID_MMI_NEWSMELT_CID01 );
			return false;
		}
	}
	return false;
}

bool	FLItemExtract::OnCreateCid( FLWSUser* pUser, const PT_PACKET_CID_CREATE ptCidCreate )
{
	if( IsValidObj( pUser ) == TRUE && ptCidCreate != NULL )
	{
		FLItemElem* pCid = pUser->m_Inventory.GetAtId( ptCidCreate->dwCidObjid );
		FLItemElem* pCidCombine = pUser->m_Inventory.GetAtId( ptCidCreate->dwCidCombineObjid );

		if( IsUsableItem( pCid ) == TRUE && IsUsableItem( pCidCombine ) == TRUE )
		{
			if( IsMixAble( pUser, MIX_CREATE_CID, pCid, pCidCombine ) == true )
			{
				FLItemElem itemElem;
				itemElem.m_dwItemId = ITEM_INDEX( 7037, II_GEN_MAT_CID ) ;
				itemElem.m_nItemNum = 1;
				itemElem.SetSerialNumber();

				if( pUser->CreateItem( &itemElem ) == TRUE )
				{
					pUser->AddDefinedText( TID_MMI_NEWSMELT_CID07 );
					// log...
					g_DPSrvr.PutItemLog( pUser, "C", "CREATE_CID", &itemElem, itemElem.m_nItemNum );
					g_DPSrvr.PutItemLog( pUser, "R", "CREATE_CID", pCid, 1 );
					g_DPSrvr.PutItemLog( pUser, "R", "CREATE_CID", pCidCombine, 1 );

					pUser->RemoveItem( ptCidCreate->dwCidObjid, 1 );
					pUser->RemoveItem( ptCidCreate->dwCidCombineObjid, 1 );
					return true;
				}
				else
				{
					pUser->AddDefinedText( TID_GAME_LACKSPACE );
					return false;
				}
			}
			else
			{
				// 조합 불가능..
				return false;
			}
		}
		else
		{
			// 거래중인 아이템인가..
			pUser->AddDefinedText( TID_MMI_NEWSMELT_CID01 );
			return false;
		}
	}
	return false;
}

bool	FLItemExtract::OnUpgradeCidPiece( FLWSUser* pUser, const PT_PACKET_CIDPIECE_UPGRADE ptCidPieceUpgrade )
{
	if( IsValidObj( pUser ) == TRUE && ptCidPieceUpgrade != NULL )
	{
		FLItemElem* pCidPiece = pUser->m_Inventory.GetAtId( ptCidPieceUpgrade->dwCidPieceObjid );
		FLItemElem* pCidPieceCombine = pUser->m_Inventory.GetAtId( ptCidPieceUpgrade->dwCidPieceCombineObjid );

		if( IsUsableItem( pCidPiece ) == TRUE && IsUsableItem( pCidPieceCombine ) == TRUE )
		{
			if( IsMixAble( pUser, MIX_UPGRADE_CIDPIECE, pCidPiece, pCidPieceCombine ) == true )
			{
				FLItemElem itemElem;

				switch( pCidPiece->m_dwItemId )
				{
				case ITEM_INDEX( 7045, II_GEN_MAT_CIDPIECE02 ):
					itemElem.m_dwItemId = ITEM_INDEX( 7038, II_GEN_MAT_CIDPIEC01 );
					break;

				case ITEM_INDEX( 7046, II_GEN_MAT_CIDPIECE03 ):
					itemElem.m_dwItemId = ITEM_INDEX( 7045, II_GEN_MAT_CIDPIECE02 );
					break;

				case ITEM_INDEX( 7047, II_GEN_MAT_CIDPIECE04 ):
					itemElem.m_dwItemId = ITEM_INDEX( 7046, II_GEN_MAT_CIDPIECE03 );
					break;

				case ITEM_INDEX( 7048, II_GEN_MAT_CIDPIECE05 ):
					itemElem.m_dwItemId = ITEM_INDEX( 7047, II_GEN_MAT_CIDPIECE04 );
					break;

				default:
					itemElem.m_dwItemId = NULL_ID;
					break;
				}

				if( itemElem.m_dwItemId != NULL_ID )
				{
					itemElem.m_nItemNum = 1;
					itemElem.SetSerialNumber();

					PT_ITEM_SPEC pUpgradeCidPieceProp = g_xSpecManager->GetSpecItem( itemElem.m_dwItemId );

					if( pUpgradeCidPieceProp != NULL && pUser->CreateItem( &itemElem ) == TRUE )
					{
						pUser->AddDefinedText( TID_MMI_NEWSMELT_CID08, "%d", pUpgradeCidPieceProp->dwItemLV );
						// log...
						g_DPSrvr.PutItemLog( pUser, "C", "UPGRADE_CID_PIECE", &itemElem, itemElem.m_nItemNum );
						g_DPSrvr.PutItemLog( pUser, "R", "UPGRADE_CID_PIECE", pCidPiece, 10 );
						g_DPSrvr.PutItemLog( pUser, "R", "UPGRADE_CID_PIECE", pCidPieceCombine, 1 );

						pUser->RemoveItem( ptCidPieceUpgrade->dwCidPieceObjid, 10 );
						pUser->RemoveItem( ptCidPieceUpgrade->dwCidPieceCombineObjid, 1 );
						return true;
					}
					else
					{
						pUser->AddDefinedText( TID_GAME_LACKSPACE );
						return false;
					}
				}
				else
				{
					// 왜 부적합 아이템이 여기까지 온걸까..
					pUser->AddDefinedText( TID_MMI_NEWSMELT_CID01 );
					return false;
				}
			}
			else
			{
				// 조합 불가능..
				return false;
			}
		}
		else
		{
			// 거래중인 아이템인가..
			pUser->AddDefinedText( TID_MMI_NEWSMELT_CID01 );
			return false;
		}
	}
	return false;
}

bool	FLItemExtract::OnCreateOperCid( FLWSUser* pUser, const PT_PACKET_OPERCID_CREATE ptOperCidCreate )
{
	if( IsValidObj( pUser ) == TRUE && ptOperCidCreate != NULL )
	{
		FLItemElem* pOper = pUser->m_Inventory.GetAtId( ptOperCidCreate->dwOperObjid );
		FLItemElem* pCid = pUser->m_Inventory.GetAtId( ptOperCidCreate->dwCidObjid );
		FLItemElem* pBaryummeal = pUser->m_Inventory.GetAtId( ptOperCidCreate->dwBaryummealObjid );

		if( IsUsableItem( pOper ) == TRUE && IsUsableItem( pCid ) == TRUE && IsUsableItem( pBaryummeal ) == TRUE )
		{
			if( IsMixAble( pUser, MIX_CREATE_OPERCID, pOper, pCid, pBaryummeal ) == true )
			{
				FLItemElem itemElem;
				itemElem.m_dwItemId = ITEM_INDEX( 7049, II_GEN_MAT_OPERCID );
				itemElem.m_nItemNum = 1;
				itemElem.SetSerialNumber();

				if( pUser->CreateItem( &itemElem ) == TRUE )
				{
					pUser->AddDefinedText( TID_MMI_NEWSMELT_OPERCID04 );
					// log...
					g_DPSrvr.PutItemLog( pUser, "C", "CREATE_OPERCID", &itemElem, itemElem.m_nItemNum );
					g_DPSrvr.PutItemLog( pUser, "R", "CREATE_OPERCID", pOper, 1 );
					g_DPSrvr.PutItemLog( pUser, "R", "CREATE_OPERCID", pCid, 1 );
					g_DPSrvr.PutItemLog( pUser, "R", "CREATE_OPERCID", pBaryummeal, 1 );

					pUser->RemoveItem( ptOperCidCreate->dwOperObjid, 1 );
					pUser->RemoveItem( ptOperCidCreate->dwCidObjid, 1 );
					pUser->RemoveItem( ptOperCidCreate->dwBaryummealObjid, 1 );
					return true;
				}
				else
				{
					pUser->AddDefinedText( TID_GAME_LACKSPACE );
					return false;
				}
			}
			else
			{
				// 조합 불가능..
				return false;
			}
		}
		else
		{
			// 거래중인 아이템인가..
			pUser->AddDefinedText( TID_MMI_NEWSMELT_CID01 );
			return false;
		}
	}
	return false;
}

bool	FLItemExtract::IsExtractAble( FLWSUser* pUser, int nExtractType, FLItemElem* pMaterial )
{
	if( IsValidObj( pUser ) == FALSE || IsUsableItem( pMaterial ) == FALSE )
	{
		return false;
	}

	PT_ITEM_SPEC pMaterialProp = pMaterial->GetProp();
	if( pMaterialProp == NULL )
	{
		return false;
	}

	switch( nExtractType )
	{
	case EXTRACT_WEAPON:
		{
			if( pMaterialProp->dwItemKind1 == IK1_WEAPON )
			{
				if( pMaterialProp->dwReferStat1 != WEAPON_UNIQUE && pMaterialProp->IsUltimate() == FALSE && pMaterialProp->IsBaruna() == FALSE )
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		break;

	case EXTRACT_ARMOR:
		{
			if( pMaterialProp->dwItemKind1 == IK1_ARMOR )
			{
				if( pMaterialProp->IsBaruna() == FALSE )
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		break;

	default:
		break;
	}

	return false;
}

bool	FLItemExtract::IsMixAble( FLWSUser* pUser, int nMixType, FLItemElem* pMaterial, FLItemElem* pCombine, FLItemElem* pBaryummeal )
{
	if( IsValidObj( pUser ) == FALSE || IsUsableItem( pMaterial ) == FALSE || IsUsableItem( pCombine ) == FALSE )
	{
		return false;
	}

	PT_ITEM_SPEC pMaterialProp = pMaterial->GetProp();
	PT_ITEM_SPEC pCombineProp = pCombine->GetProp();
	if( pMaterialProp == NULL || pCombineProp == NULL )
	{
		return false;
	}

	switch( nMixType )
	{
	case MIX_CREATE_OPER:
		{
			if( pMaterialProp->dwItemKind3 == IK3_OPERCRYSTAL && pCombineProp->dwItemKind3 == IK3_OPERMIX )
			{
				return true;
			}
			else
			{
				pUser->AddDefinedText( TID_MMI_NEWSMELT_CID01 );
				return false;
			}
		}
		break;

	case MIX_CREATE_CID:
		{
			if( pMaterialProp->dwItemKind3 == IK3_CIDCRYSTAL && pCombineProp->dwItemKind3 == IK3_CIDMIX )
			{
				if( pMaterialProp->dwItemLV == 1 && pCombineProp->dwItemLV == 1 )
				{
					return true;
				}
				else
				{
					if( pMaterialProp->dwItemLV == 1 && pCombineProp->dwItemLV != 1 )
					{
						pUser->AddDefinedText( TID_MMI_NEWSMELT_CID06 );
					}
					else if( pMaterialProp->dwItemLV != 1 && pCombineProp->dwItemLV == 1 )
					{
						pUser->AddDefinedText( TID_MMI_NEWSMELT_CID04 );
					}
					else
					{
						pUser->AddDefinedText( TID_MMI_NEWSMELT_CID01 );
					}
					return false;
				}
			}
			else
			{
				pUser->AddDefinedText( TID_MMI_NEWSMELT_CID01 );
				return false;
			}
		}
		break;

	case MIX_UPGRADE_CIDPIECE:
		{
			if( pMaterialProp->dwItemKind3 == IK3_CIDCRYSTAL && pCombineProp->dwItemKind3 == IK3_CIDMIX )
			{
				if( pMaterialProp->dwItemLV > 1 && pMaterialProp->dwItemLV == pCombineProp->dwItemLV )
				{
					if( pMaterial->m_nItemNum >= 10 )
					{
						return true;
					}
					else
					{
						pUser->AddDefinedText( TID_MMI_NEWSMELT_CID05 );
						return false;
					}
				}
				else
				{
					pUser->AddDefinedText( TID_MMI_NEWSMELT_CID06 );
					return false;
				}
			}
			else
			{
				pUser->AddDefinedText( TID_MMI_NEWSMELT_CID01 );
				return false;
			}
		}
		break;

	case MIX_CREATE_OPERCID:
		{
			if( IsUsableItem( pBaryummeal ) == FALSE )
			{
				return false;
			}
			PT_ITEM_SPEC pBaryummealProp = pBaryummeal->GetProp();
			if( pBaryummealProp != NULL )
			{
				if( pMaterialProp->dwItemKind3 == IK3_OPER && pCombineProp->dwItemKind3 == IK3_CID && pBaryummealProp->dwItemKind3 == IK3_MEAL )
				{
					return true;
				}
				else
				{
					pUser->AddDefinedText( TID_MMI_NEWSMELT_CID01 );
					return false;
				}
			}
			else
			{
				pUser->AddDefinedText( TID_MMI_NEWSMELT_CID01 );
				return false;
			}
		}
		break;

	default:
		break;
	}

	return false;
}

bool	FLItemExtract::GetOperOrOperPiece( FLItemElem* pMaterial, int nMaterialType, int nProb, FLItemElem & itemElem )
{
	if( IsUsableItem( pMaterial ) == FALSE )
	{
		return false;
	}

	PT_ITEM_SPEC pMaterialProp = pMaterial->GetProp();
	if( pMaterialProp == NULL )
	{
		return false;
	}

	DWORD dwItemKind = 0;
	if( nMaterialType == EXTRACT_WEAPON )
	{
		dwItemKind = pMaterialProp->dwItemKind1;
	}
	else if( nMaterialType == EXTRACT_ARMOR )
	{
		dwItemKind = pMaterialProp->dwItemKind3;
	}

	MAP_OPER_EXTRACT_PROB::iterator itMap = m_mapOperExtractProb.find( dwItemKind );

	if( itMap != m_mapOperExtractProb.end() )
	{
		int nOperNumProb = (int)( xRandom( 10000 ) );
		std::vector<T_OPER_EXTRACT_PROB>	vectOper = itMap->second;

		for( std::vector<T_OPER_EXTRACT_PROB>::iterator itVec = vectOper.begin(); itVec != vectOper.end(); ++itVec )
		{
			if( itVec->nItemMinLevel <= pMaterialProp->nMinLimitLevel && pMaterialProp->nMinLimitLevel <= itVec->nItemMaxLevel )
			{
				if( nProb <= itVec->nOperExtractProb )
				{
					itemElem.m_dwItemId = ITEM_INDEX( 7040, II_GEN_MAT_OPER );

					int nDeviation = itVec->nOperMaxNum - itVec->nOperMinNum + 1;
					if( nDeviation > 1 )
					{
						while( nDeviation > 0 )
						{
							if( nOperNumProb <= ( 10000 / nDeviation ) )
							{
								if( itVec->nOperMinNum + nDeviation - 1 <= itVec->nOperMaxNum )
								{
									itemElem.m_nItemNum = itVec->nOperMinNum + nDeviation - 1;
								}
								else
								{
									itemElem.m_nItemNum = itVec->nOperMaxNum;
								}
								break;
							}
							--nDeviation;
						}
					}
					else
					{
						itemElem.m_nItemNum = itVec->nOperMinNum;
					}

					itemElem.SetSerialNumber();
					return true;
				}
				else if( nProb <= itVec->nOperPieceExtractProb )
				{
					itemElem.m_dwItemId = ITEM_INDEX( 7041, II_GEN_MAT_OPERPIECE );

					int nDeviation = itVec->nOperPieceMaxNum - itVec->nOperPieceMinNum + 1;
					if( nDeviation > 1 )
					{
						while( nDeviation > 0 )
						{
							if( nOperNumProb <= ( 10000 / nDeviation ) )
							{
								if( itVec->nOperPieceMinNum + nDeviation - 1 <= itVec->nOperPieceMaxNum )
								{
									itemElem.m_nItemNum = itVec->nOperPieceMinNum + nDeviation - 1;
								}
								else
								{
									itemElem.m_nItemNum = itVec->nOperPieceMaxNum;
								}
								break;
							}
							--nDeviation;
						}
					}
					else
					{
						itemElem.m_nItemNum = itVec->nOperPieceMinNum;
					}

					itemElem.SetSerialNumber();
					return true;
				}
				else
				{
					// fail..
					return false;
				}
			}
		}
	}
	else
	{
		return false;
	}
	return false;
}

