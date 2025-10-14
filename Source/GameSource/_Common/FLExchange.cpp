#include "StdAfx.h"
#include "FLExchange.h"
#include <defineNeuz.h>

#ifdef __WORLDSERVER
#include "../worldserver/FLItemUpgradeManager.h"
#endif // __WORLDSERVER

#define MAX_ITEMDRAWCOUNT		15
#define MAX_ITEMQUANTITYCOUNT	1000000

FLExchange::FLExchange()
{

}

FLExchange::~FLExchange()
{
	Clear();	
}

void	FLExchange::Clear()
{
	for( ExchangeDataItr kItr = m_mapExchange.begin(); kItr != m_mapExchange.end(); ++kItr )
	{
		for( ExchangeItemListItr kItr2 = kItr->second.mapExchangeItemList.begin(); kItr2 != kItr->second.mapExchangeItemList.end(); ++kItr2 )
		{
			kItr2->second.vecResultMsgTextID.clear();
			kItr2->second.mapConditionItem.clear();
			kItr2->second.mapPaymentItem.clear();
		}

		kItr->second.vecDesciprtionTextID.clear();
		kItr->second.mapExchangeItemList.clear();
	}

	m_mapExchange.clear();
}

bool	FLExchange::CheckExchangeData()
{
	//	BEGIN120201	
	for( ExchangeDataCItr posExchangeData = m_mapExchange.begin(); posExchangeData != m_mapExchange.end(); ++posExchangeData )
	{
		const DWORD	dwMenuID								= posExchangeData->first;
		const T_EXCHANGE_DATA kExchangeData					= posExchangeData->second;
		
		for( ExchangeItemListCItr posExchangeItemList = kExchangeData.mapExchangeItemList.begin(); posExchangeItemList != kExchangeData.mapExchangeItemList.end(); ++posExchangeItemList )
		{
			const T_EXCHANGE_ITEM_LIST kExchangeItemList	= posExchangeItemList->second;

			if( kExchangeItemList.byExchangeType <= EXCHANGE_TYPE_NONE || kExchangeItemList.byExchangeType >= EXCHANGE_TYPE_MAX )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID EXCHANGE TYPE. MMI_ID(%u), TYPE(%d) ]" ), dwMenuID, kExchangeItemList.byExchangeType );
				FLASSERT( 0 );
				return false;
			}

			//////////////////////////////////////////////////////////////////////////
			// 제련 가능 여부 체크
#ifdef __WORLDSERVER
			if( kExchangeItemList.byExchangeType == EXCHANGE_TYPE_ENCHANT_ITEM )
			{
				for( ExchangeItemCItr pos = kExchangeItemList.mapConditionItem.begin(); pos != kExchangeItemList.mapConditionItem.end(); ++pos )
				{
					const T_EXCHANGE_ITEM kExchangeItemData		= pos->second;

					const DWORD dwMaxGeneralEnchantLevel		= g_pItemUpgradeManager->GetMaxSizeGeneralEnchant( kExchangeItemData.dwItemID );
					if( dwMaxGeneralEnchantLevel < kExchangeItemData.dwMaxGeneralEnchant )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID GENERAL ENCHANT LEVEL. MMI_ID(%u), COND_ITEM_ID(%u) ]" ), dwMenuID, kExchangeItemData.dwItemID );
						FLASSERT( 0 );
						return false;
					}

					const DWORD dwMaxAttributeEnchantLevel		= g_pItemUpgradeManager->GetMaxSizeAttributeEnchant( kExchangeItemData.dwItemID );
					if( dwMaxAttributeEnchantLevel < kExchangeItemData.dwMaxAttributeEnchant )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ATTRIBUTE ENCHANT LEVEL. MMI_ID(%u), COND_ITEM_ID(%u) ]" ), dwMenuID, kExchangeItemData.dwItemID );
						FLASSERT( 0 );
						return false;
					}
				}

				for( ExchangeItemCItr pos = kExchangeItemList.mapPaymentItem.begin(); pos != kExchangeItemList.mapPaymentItem.end(); ++pos )
				{
					const T_EXCHANGE_ITEM kExchangeItemData		= pos->second;
					
					const DWORD dwMaxGeneralEnchantLevel		= g_pItemUpgradeManager->GetMaxSizeGeneralEnchant( kExchangeItemData.dwItemID );
					if( dwMaxGeneralEnchantLevel < kExchangeItemData.dwMaxGeneralEnchant )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID GENERAL ENCHANT LEVEL. MMI_ID(%u), PAY_ITEM_ID(%u) ]" ), dwMenuID, kExchangeItemData.dwItemID );
						FLASSERT( 0 );
						return false;
					}

					const DWORD dwMaxAttributeEnchantLevel		= g_pItemUpgradeManager->GetMaxSizeAttributeEnchant( kExchangeItemData.dwItemID );
					if( dwMaxAttributeEnchantLevel < kExchangeItemData.dwMaxAttributeEnchant )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ATTRIBUTE ENCHANT LEVEL. MMI_ID(%u), PAY_ITEM_ID(%u) ]" ), dwMenuID, kExchangeItemData.dwItemID );
						FLASSERT( 0 );
						return false;
					}
				}
			}
#endif // __WORLDSERVER
			//////////////////////////////////////////////////////////////////////////

			if( kExchangeItemList.byExchangeType == EXCHANGE_TYPE_ENCHANT_MOVE )
			{
				const DWORD dwCondItemCount	= kExchangeItemList.mapConditionItem.size();
				if( dwCondItemCount > MAX_EXCHANGE_ENCHANT_MOVE )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "[ COND ITEM COUNT INVALID. COUNT(%d), MAX(%d) ]" )
						, dwCondItemCount, MAX_EXCHANGE_ENCHANT_MOVE );
					FLASSERT( 0 );
					return	false;
				}

				const DWORD dwPayItemCount		= kExchangeItemList.mapConditionItem.size();
				if( dwPayItemCount > MAX_EXCHANGE_ENCHANT_MOVE )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "[ PAY ITEM COUNT INVALID. COUNT(%d), MAX(%d) ]" )
						, dwPayItemCount, MAX_EXCHANGE_ENCHANT_MOVE );
					FLASSERT( 0 );
					return	false;
				}

				if( kExchangeItemList.dwPaymentLimitCount > MAX_EXCHANGE_ENCHANT_MOVE )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "[ PAY ITEM LIMIT COUNT INVALID. COUNT(%d), MAX(%d) ]" )
						, kExchangeItemList.dwPaymentLimitCount, MAX_EXCHANGE_ENCHANT_MOVE );
					FLASSERT( 0 );
					return	false;
				}

//				const DWORD	dwTotalSize = kExchangeItemList.dwPaymentLimitCount;

//				for( DWORD Nth = 0; Nth < dwTotalSize; ++Nth )
				{
					const DWORD dwCondItemID				= kExchangeItemList.mapConditionItem.begin()->first;
					const DWORD dwPayItemID					= kExchangeItemList.mapPaymentItem.begin()->first;

					const T_ITEM_SPEC* pConditionItemSpec	= g_xSpecManager->GetSpecItem( dwCondItemID );
					const T_ITEM_SPEC* pPaymentItemSpec		= g_xSpecManager->GetSpecItem( dwPayItemID );

					if( pConditionItemSpec == NULL || pPaymentItemSpec == NULL )
					{
						FLASSERT( 0 );
						return	false;
					}

					if( pConditionItemSpec->dwItemKind1 != pPaymentItemSpec->dwItemKind1
						|| pConditionItemSpec->dwItemKind2 != pPaymentItemSpec->dwItemKind2
						|| pConditionItemSpec->dwItemKind3 != pPaymentItemSpec->dwItemKind3 )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "[ COND ITEM and PAY ITEM MISMATCH. CHECK ITEM PROPERTY. MMI_ID(%d), COND_ITEM(%d), PAY_ITEM(%d) ]" )
							, dwMenuID, pConditionItemSpec->dwID, pPaymentItemSpec->dwID );
						FLASSERT( 0 );
						return	false;
					}
				}
			}
		}
	}

	return	true;
}


bool	FLExchange::IsPenyaItem(  const DWORD dwItemID  ) const
{
	if( dwItemID == ITEM_INDEX( 12, II_GOLD_SEED1 ) ||
		dwItemID == ITEM_INDEX( 13, II_GOLD_SEED2 ) ||
		dwItemID == ITEM_INDEX( 14, II_GOLD_SEED3 ) ||
		dwItemID == ITEM_INDEX( 15, II_GOLD_SEED4 ) )
		return true;

	return false;
}


bool	FLExchange::LoadScript()
{
	CScanner kScanner;

	if( kScanner.Load( "Exchange_Script.txt" ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ Exchange_Script.txt LOAD FAILED. ]" ) );
		return false;
	}

	kScanner.GetToken();		// MMI ID

	while( kScanner.tok != FINISHED )
	{
		DWORD dwMenuID = static_cast<DWORD>( CScript::GetDefineNum( kScanner.Token ) );
		if( dwMenuID == NULL_ID || dwMenuID >= MAX_MOVER_MENU )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ MMI ID INVALID. MMI_ID:(%s) ]" ), kScanner.Token );
			return false;
		}

		TCHAR szMMIID[128] = { 0, };
		FLStrcpy( szMMIID, _countof( szMMIID ), kScanner.Token );

		kScanner.GetToken();	// {
		FLASSERT( kScanner.Token == "{" );

		T_EXCHANGE_DATA	kSetList;
		DWORD dwNth = 0;

		kScanner.GetToken();
		while( kScanner.Token != "}" )
		{
			if( kScanner.Token == "DESCRIPTION" )
			{
				kScanner.GetToken();	// {
				kScanner.GetToken();	// Text ID
				while( kScanner.Token != "}" )
				{
					DWORD dwTextID = static_cast<DWORD>( CScript::GetDefineNum( kScanner.Token ) );
					if( dwTextID == NULL_ID )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "[ DESCRIPTION TEXT ID INVALID. MMI_ID:(%s), TEXT_ID:(%s) ]" )
							, szMMIID, kScanner.Token );
						return false;
					}

					kSetList.vecDesciprtionTextID.push_back( dwTextID );
					
					kScanner.GetToken();
				}
			}
			else if( kScanner.Token == "SET" )
			{
				T_EXCHANGE_ITEM_LIST kSet;
				kSet.byExchangeType = EXCHANGE_TYPE_NORMAL_ITEM;

				kScanner.GetToken();	// Text ID
				kSet.dwSetTextID = static_cast<DWORD>( CScript::GetDefineNum( kScanner.Token ) );

				kScanner.GetToken();	// {
				FLASSERT( kScanner.Token == "{" );

				kScanner.GetToken();
				while( kScanner.Token != "}" )
				{
					if( kScanner.Token == "RESULTMSG" )
					{
						kScanner.GetToken();	// {
						FLASSERT( kScanner.Token == "{" );

						kScanner.GetToken();	// Text ID
						while( kScanner.Token != "}" )
						{
							DWORD dwTextID = static_cast<DWORD>( CScript::GetDefineNum( kScanner.Token ) );
							if( dwTextID == NULL_ID )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[ RESULTMSG TEXT ID INVALID. MMI_ID:(%s), TEXT_ID:(%s) ]" )
									, szMMIID, kScanner.Token );
								return false;
							}

							kSet.vecResultMsgTextID.push_back( dwTextID );

							kScanner.GetToken();
						}
					}
					else if( kScanner.Token == "CONDITION" )
					{
						kScanner.GetToken();	// {
						FLASSERT( kScanner.Token == "{" );

						kScanner.GetToken();
						while( kScanner.Token != "}" )
						{
							T_EXCHANGE_ITEM kExchangeItem;

							kExchangeItem.dwItemID = static_cast<DWORD>( CScript::GetDefineNum( kScanner.Token ) );
							if( IsPenyaItem( kExchangeItem.dwItemID ) )
								FLERROR_ASSERT_LOG_RETURN( NULL, false, _T("[EXCHANGE SCRIPT] Not Support PENYA Please Fix Script!!") );

							PT_ITEM_SPEC pItemSpec = g_xSpecManager->GetSpecItem( kExchangeItem.dwItemID );
							if( pItemSpec == NULL )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[ CONDITION ITEM ID INVALID. MMI_ID:(%s), ITEM_ID:(%d) ]" )
									, szMMIID, kExchangeItem.dwItemID );
								return false;
							}

							kExchangeItem.nItemQuantity	= kScanner.GetNumber();
							if( kExchangeItem.nItemQuantity <= 0 )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[ CONDITION ITEM NUM INVALID. MMI_ID:(%s), ITEM_ID:(%d), ITEM_NUM:(%d) ]" )
									, szMMIID, kExchangeItem.dwItemID, kExchangeItem.nItemQuantity );
								return false;
							}

							if( kExchangeItem.nItemQuantity > MAX_ITEMQUANTITYCOUNT )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[EXCHANGE SCRIPT] WARNING!! COND ITEM QUANTITY COUNT TOO MUCH - MAXIMUM [%d]. Maybe UserInterface Clip - MMI_ID:(%s) ]" ), MAX_ITEMQUANTITYCOUNT, szMMIID );
								FLASSERT( NULL );
							}

							const bool bRet = kSet.mapConditionItem.insert( ExchangeItemMap::value_type( kExchangeItem.dwItemID, kExchangeItem ) ).second;
							if( bRet == false )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[ CONDITION ITEM DUPLICATED. MMI_ID(%s), ITEM_ID(%d) ]" )
									, szMMIID, kExchangeItem.dwItemID );
								FLASSERT( 0 );
								return false;
							}

							kScanner.GetToken();
						}
					}
					else if( kScanner.Token == "REMOVE" )
					{
						kScanner.GetToken();	// {
						FLASSERT( kScanner.Token == "{" );

						kScanner.GetToken();
						while( kScanner.Token != "}" )
						{
							kScanner.GetToken();
						}
					}
					else if( kScanner.Token == "PAY" )
					{
						kSet.dwPaymentLimitCount	= static_cast<DWORD>( kScanner.GetNumber() );
						kScanner.GetToken();	// {
						FLASSERT( kScanner.Token == "{" );

						DWORD dwSumPaymentProb = 0;
						kScanner.GetToken();
						while( kScanner.Token != "}" )
						{
							T_EXCHANGE_ITEM kExchangeItem;

							kExchangeItem.dwItemID = static_cast<DWORD>( CScript::GetDefineNum( kScanner.Token ) );
							if( IsPenyaItem( kExchangeItem.dwItemID ) )
								FLERROR_ASSERT_LOG_RETURN( NULL, false, _T("[EXCHANGE SCRIPT] Not Support PENYA Please Fix Script!!") );

							PT_ITEM_SPEC pItemSpec = g_xSpecManager->GetSpecItem( kExchangeItem.dwItemID );
							if( pItemSpec == NULL )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[ PAY ITEM ID INVALID. MMI_ID:(%s), ITEM_ID:(%d) ]" )
									, szMMIID, kExchangeItem.dwItemID );
								return false;
							}

							kExchangeItem.nItemQuantity		= kScanner.GetNumber();
							if( kExchangeItem.nItemQuantity <= 0 )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[ PAY ITEM NUM INVALID. MMI_ID:(%s), ITEM_ID:(%d), ITEM_NUM:(%d) ]" )
									, szMMIID, kExchangeItem.dwItemID, kExchangeItem.nItemQuantity );
								return false;
							}

							if( kExchangeItem.nItemQuantity > MAX_ITEMQUANTITYCOUNT )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[EXCHANGE SCRIPT] WARNING!! PAY ITEM QUANTITY COUNT TOO MUCH - MAXIMUM [%d]. Maybe UserInterface Clip - MMI_ID:(%s) ]" ), MAX_ITEMQUANTITYCOUNT, szMMIID );
								FLASSERT( NULL );
							}

							kExchangeItem.dwPaymentProb		= static_cast<DWORD>( kScanner.GetNumber() );

							dwSumPaymentProb				+= kExchangeItem.dwPaymentProb;
							kExchangeItem.dwPaymentProb		= dwSumPaymentProb;
							if( dwSumPaymentProb > MAX_PAYMENT_PROB )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[ PAYMENT PROB SUM INVALID. MMI_ID:(%s), ITEM_ID:(%d), PROB:(%d) ]" )
									, szMMIID, kExchangeItem.dwItemID, dwSumPaymentProb );
								return false;
							}

							kScanner.SetMark();
							kScanner.GetToken();
							kScanner.GoMark();
							if( kScanner.tokenType == NUMBER )
							{
								kExchangeItem.byItemFlag = static_cast<BYTE>( kScanner.GetNumber() );
							}

							const bool bRet = kSet.mapPaymentItem.insert( ExchangeItemMap::value_type( kExchangeItem.dwItemID, kExchangeItem ) ).second;
							if( bRet == false )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[ PAY ITEM DUPLICATED. MMI_ID(%s), ITEM_ID(%d) ]" )
									, szMMIID, kExchangeItem.dwItemID );
								FLASSERT( 0 );
								return false;
							}

							kScanner.GetToken();
						}

						if( dwSumPaymentProb != MAX_PAYMENT_PROB )
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "[ PAYMENT PROB SUM INVALID. MMI_ID:(%s), PROB:(%d) ]" )
								, szMMIID, dwSumPaymentProb );
							return false;
						}
					}

					kScanner.GetToken();
				}

				if( kSet.mapConditionItem.empty() == true || kSet.mapPaymentItem.empty() == true )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "[ COND ITEM or PAY ITEM COUNT ZERO. MMI_ID:(%s) ]" ), szMMIID );
					return false;
				}

				int nItemTotalCount( kSet.mapConditionItem.size() + kSet.mapPaymentItem.size() );
				if( nItemTotalCount > MAX_ITEMDRAWCOUNT )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "[EXCHANGE SCRIPT] WARNING!!  COND and PAY ITEM TOTAL COUNT MORE THAN [%d]. Maybe UserInterface Clip - MMI_ID:(%s) ]" ), MAX_ITEMDRAWCOUNT, szMMIID );
					FLASSERT( NULL );
				}

				kSetList.mapExchangeItemList.insert( ExchangeItemListMap::value_type( dwNth, kSet ) );

				++dwNth;
			}
			else if( kScanner.Token == "SET_SMELT" )
			{
				T_EXCHANGE_ITEM_LIST kSet;
				kSet.byExchangeType = EXCHANGE_TYPE_ENCHANT_ITEM;

				kScanner.GetToken();	// Text ID
				kSet.dwSetTextID = static_cast<DWORD>( CScript::GetDefineNum( kScanner.Token ) );

				kScanner.GetToken();	// {
				FLASSERT( kScanner.Token == "{" );

				kScanner.GetToken();
				while( kScanner.Token != "}" )
				{
					if( kScanner.Token == "RESULTMSG" )
					{
						kScanner.GetToken();	// {
						FLASSERT( kScanner.Token == "{" );

						kScanner.GetToken();	// Text ID
						while( kScanner.Token != "}" )
						{
							DWORD dwTextID = static_cast<DWORD>( CScript::GetDefineNum( kScanner.Token ) );
							if( dwTextID == NULL_ID )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[ RESULTMSG TEXT ID INVALID. MMI_ID:(%s), TEXT_ID:(%s) ]" )
									, szMMIID, kScanner.Token );
								return false;
							}

							kSet.vecResultMsgTextID.push_back( dwTextID );

							kScanner.GetToken();
						}
					}
					else if( kScanner.Token == "CONDITION" )
					{
						kScanner.GetToken();	// {
						FLASSERT( kScanner.Token == "{" );

						kScanner.GetToken();
						while( kScanner.Token != "}" )
						{
							T_EXCHANGE_ITEM kExchangeItem;

							kExchangeItem.dwItemID = static_cast<DWORD>( CScript::GetDefineNum( kScanner.Token ) );
							if( IsPenyaItem( kExchangeItem.dwItemID ) )
								FLERROR_ASSERT_LOG_RETURN( NULL, false, _T("[EXCHANGE SCRIPT] Not Support PENYA Please Fix Script!!") );

							PT_ITEM_SPEC pItemSpec = g_xSpecManager->GetSpecItem( kExchangeItem.dwItemID );
							if( pItemSpec == NULL )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[ CONDITION ITEM ID INVALID. MMI_ID:(%s), ITEM_ID:(%d) ]" )
									, szMMIID, kExchangeItem.dwItemID );
								return false;
							}

							kExchangeItem.nItemQuantity	= kScanner.GetNumber();
							if( kExchangeItem.nItemQuantity <= 0 )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[ CONDITION ITEM NUM INVALID. MMI_ID:(%s), ITEM_ID:(%d), ITEM_NUM:(%d) ]" )
									, szMMIID, kExchangeItem.dwItemID, kExchangeItem.nItemQuantity );
								return false;
							}

							if( kExchangeItem.nItemQuantity > MAX_ITEMQUANTITYCOUNT )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[EXCHANGE SCRIPT] WARNING!! COND ITEM QUANTITY COUNT TOO MUCH - MAXIMUM [%d]. Maybe UserInterface Clip - MMI_ID:(%s) ]" ), MAX_ITEMQUANTITYCOUNT, szMMIID );
								FLASSERT( NULL );
							}

							kExchangeItem.dwMinGeneralEnchant = static_cast<DWORD>( kScanner.GetNumber() );
							kExchangeItem.dwMaxGeneralEnchant = static_cast<DWORD>( kScanner.GetNumber() );
							if( kExchangeItem.dwMinGeneralEnchant > kExchangeItem.dwMaxGeneralEnchant )
							{
								FLERROR_LOG( PROGDLG_NORMAL, _T( "[ CONDITION GENERAL ENCHANT LEVEL INVALID. MMI_ID:(%s), MIN:(%d), MAX:(%d) ]" )
									, szMMIID, kExchangeItem.dwMinGeneralEnchant, kExchangeItem.dwMaxGeneralEnchant );
								return false;
							}

							kExchangeItem.dwMinAttributeEnchant = static_cast<DWORD>( kScanner.GetNumber() );
							kExchangeItem.dwMaxAttributeEnchant = static_cast<DWORD>( kScanner.GetNumber() );
							if( kExchangeItem.dwMinAttributeEnchant > kExchangeItem.dwMaxAttributeEnchant )
							{
								FLERROR_LOG( PROGDLG_NORMAL, _T( "[ CONDITION ATTRIBUTE ENCHANT LEVEL INVALID. MMI_ID:(%s), MIN:(%d), MAX:(%d) ]" )
									, szMMIID, kExchangeItem.dwMinAttributeEnchant, kExchangeItem.dwMaxAttributeEnchant );
								return false;
							}

							kExchangeItem.byAttributeKind = static_cast<BYTE>( kScanner.GetNumber() );
							kExchangeItem.bCheckScriptAttribute = static_cast<bool>( kScanner.GetNumber() != 0 );
							if( kExchangeItem.bCheckScriptAttribute == true
								&& ( kExchangeItem.byAttributeKind <= SAI79::NO_PROP || kExchangeItem.byAttributeKind >= SAI79::END_PROP ) )
							{
								FLERROR_LOG( PROGDLG_NORMAL, _T( "[ CONDITION ATTRIBUTE PROPERTY INVALID. MMI_ID:(%s), PROP:(%d) ]" )
									, szMMIID, kExchangeItem.byAttributeKind );
								return false;
							}

							const bool bRet = kSet.mapConditionItem.insert( ExchangeItemMap::value_type( kExchangeItem.dwItemID, kExchangeItem ) ).second;
							if( bRet == false )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[ CONDITION ITEM DUPLICATED. MMI_ID(%s), ITEM_ID(%d) ]" )
									, szMMIID, kExchangeItem.dwItemID );
								FLASSERT( 0 );
								return false;
							}
							
							kScanner.GetToken();
						}
					}
					else if( kScanner.Token == "REMOVE" )
					{
						kScanner.GetToken();	// {
						FLASSERT( kScanner.Token == "{" );

						kScanner.GetToken();
						while( kScanner.Token != "}" )
						{
							kScanner.GetToken();
						}
					}
					else if( kScanner.Token == "PAY" )
					{
						kSet.dwPaymentLimitCount	= static_cast<DWORD>( kScanner.GetNumber() );
						kScanner.GetToken();	// {
						FLASSERT( kScanner.Token == "{" );

						DWORD dwSumPaymentProb = 0;
						kScanner.GetToken();
						while( kScanner.Token != "}" )
						{
							T_EXCHANGE_ITEM kExchangeItem;

							kExchangeItem.dwItemID = static_cast<DWORD>( CScript::GetDefineNum( kScanner.Token ) );
							if( IsPenyaItem( kExchangeItem.dwItemID ) )
								FLERROR_ASSERT_LOG_RETURN( NULL, false, _T("[EXCHANGE SCRIPT] Not Support PENYA Please Fix Script!!") );

							PT_ITEM_SPEC pItemSpec = g_xSpecManager->GetSpecItem( kExchangeItem.dwItemID );
							if( pItemSpec == NULL )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[ PAY ITEM ID INVALID. MMI_ID:(%s), ITEM_ID:(%d) ]" )
									, szMMIID, kExchangeItem.dwItemID );
								return false;
							}

							kExchangeItem.nItemQuantity		= kScanner.GetNumber();
							if( kExchangeItem.nItemQuantity <= 0 )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[ PAY ITEM NUM INVALID. MMI_ID:(%s), ITEM_ID:(%d), ITEM_NUM:(%d) ]" )
									, szMMIID, kExchangeItem.dwItemID, kExchangeItem.nItemQuantity );
								return false;
							}

							if( kExchangeItem.nItemQuantity > MAX_ITEMQUANTITYCOUNT )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[EXCHANGE SCRIPT] WARNING!! PAY ITEM QUANTITY COUNT TOO MUCH - MAXIMUM [%d]. Maybe UserInterface Clip - MMI_ID:(%s) ]" ), MAX_ITEMQUANTITYCOUNT, szMMIID );
								FLASSERT( NULL );
							}

							kExchangeItem.dwPaymentProb		= static_cast<DWORD>( kScanner.GetNumber() );

							dwSumPaymentProb				+= kExchangeItem.dwPaymentProb;
							kExchangeItem.dwPaymentProb		= dwSumPaymentProb;
							if( dwSumPaymentProb > MAX_PAYMENT_PROB )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[ PAYMENT PROB SUM INVALID. MMI_ID:(%s), ITEM_ID:(%d), PROB:(%d) ]" )
									, szMMIID, kExchangeItem.dwItemID, dwSumPaymentProb );
								return false;
							}

							kScanner.SetMark();
							kScanner.GetToken();
							kScanner.GoMark();
							if( kScanner.tokenType == NUMBER )
							{
								kExchangeItem.byItemFlag = static_cast<BYTE>( kScanner.GetNumber() );
							}

							kExchangeItem.dwMinGeneralEnchant = static_cast<DWORD>( kScanner.GetNumber() );
							kExchangeItem.dwMaxGeneralEnchant = static_cast<DWORD>( kScanner.GetNumber() );
							if( kExchangeItem.dwMinGeneralEnchant > kExchangeItem.dwMaxGeneralEnchant )
							{
								FLERROR_LOG( PROGDLG_NORMAL, _T( "[ PAY GENERAL ENCHANT LEVEL INVALID. MMI_ID:(%s), MIN:(%d), MAX:(%d) ]" )
									, szMMIID, kExchangeItem.dwMinGeneralEnchant, kExchangeItem.dwMaxGeneralEnchant );
								return false;
							}

							kExchangeItem.dwMinAttributeEnchant = static_cast<DWORD>( kScanner.GetNumber() );
							kExchangeItem.dwMaxAttributeEnchant = static_cast<DWORD>( kScanner.GetNumber() );
							if( kExchangeItem.dwMinAttributeEnchant > kExchangeItem.dwMaxAttributeEnchant )
							{
								FLERROR_LOG( PROGDLG_NORMAL, _T( "[ PAY ATTRIBUTE ENCHANT LEVEL INVALID. MMI_ID:(%s), MIN:(%d), MAX:(%d) ]" )
									, szMMIID, kExchangeItem.dwMinAttributeEnchant, kExchangeItem.dwMaxAttributeEnchant );
								return false;
							}

							kExchangeItem.byAttributeKind = static_cast<BYTE>( kScanner.GetNumber() );
							kExchangeItem.bCheckScriptAttribute = static_cast<bool>( kScanner.GetNumber() != 0 );
							if( kExchangeItem.bCheckScriptAttribute == true
								&& ( kExchangeItem.byAttributeKind <= SAI79::NO_PROP || kExchangeItem.byAttributeKind >= SAI79::END_PROP ) )
							{
								FLERROR_LOG( PROGDLG_NORMAL, _T( "[ PAY ATTRIBUTE PROPERTY INVALID. MMI_ID:(%s), PROP:(%d) ]" )
									, szMMIID, kExchangeItem.byAttributeKind );
								return false;
							}

							const bool bRet = kSet.mapPaymentItem.insert( ExchangeItemMap::value_type( kExchangeItem.dwItemID, kExchangeItem ) ).second;
							if( bRet == false )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[ PAY ITEM DUPLICATED. MMI_ID(%s), ITEM_ID(%d) ]" )
									, szMMIID, kExchangeItem.dwItemID );
								FLASSERT( 0 );
								return false;
							}

							kScanner.GetToken();
						}

						if( dwSumPaymentProb != MAX_PAYMENT_PROB )
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "[ PAYMENT PROB SUM INVALID. MMI_ID:(%s), PROB:(%d) ]" )
								, szMMIID, dwSumPaymentProb );
							return false;
						}
					}

					kScanner.GetToken();
				}

				if( kSet.mapConditionItem.empty() == true || kSet.mapPaymentItem.empty() == true )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "[ COND ITEM or PAY ITEM COUNT ZERO. MMI_ID:(%s) ]" ), szMMIID );
					return false;
				}

				int nItemTotalCount( kSet.mapConditionItem.size() + kSet.mapPaymentItem.size() );
				if( nItemTotalCount > MAX_ITEMDRAWCOUNT )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "[EXCHANGE SCRIPT] WARNING!!  COND and PAY ITEM TOTAL COUNT MORE THAN [%d]. Maybe UserInterface Clip - MMI_ID:(%s) ]" ), MAX_ITEMDRAWCOUNT, szMMIID );
					FLASSERT( NULL );
				}

				kSetList.mapExchangeItemList.insert( ExchangeItemListMap::value_type( dwNth, kSet ) );

				++dwNth;
			}
			else if( kScanner.Token == "SET_ENCHANT_MOVE" )
			{
				T_EXCHANGE_ITEM_LIST kSet;
				kSet.byExchangeType = EXCHANGE_TYPE_ENCHANT_MOVE;

				kScanner.GetToken();	// Text ID
				kSet.dwSetTextID = static_cast<DWORD>( CScript::GetDefineNum( kScanner.Token ) );

				kScanner.GetToken();	// {
				FLASSERT( kScanner.Token == "{" );

				kScanner.GetToken();
				while( kScanner.Token != "}" )
				{
					if( kScanner.Token == "RESULTMSG" )
					{
						kScanner.GetToken();	// {
						FLASSERT( kScanner.Token == "{" );

						kScanner.GetToken();	// Text ID
						while( kScanner.Token != "}" )
						{
							DWORD dwTextID = static_cast<DWORD>( CScript::GetDefineNum( kScanner.Token ) );
							if( dwTextID == NULL_ID )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[ RESULTMSG TEXT ID INVALID. MMI_ID:(%s), TEXT_ID:(%s) ]" )
									, szMMIID, kScanner.Token );
								return false;
							}

							kSet.vecResultMsgTextID.push_back( dwTextID );

							kScanner.GetToken();
						}
					}
					else if( kScanner.Token == "CONDITION" )
					{
						kScanner.GetToken();	// {
						FLASSERT( kScanner.Token == "{" );

						kScanner.GetToken();
						while( kScanner.Token != "}" )
						{
							T_EXCHANGE_ITEM kExchangeItem;

							kExchangeItem.dwItemID = static_cast<DWORD>( CScript::GetDefineNum( kScanner.Token ) );
							if( IsPenyaItem( kExchangeItem.dwItemID ) )
								FLERROR_ASSERT_LOG_RETURN( NULL, false, _T("[EXCHANGE SCRIPT] Not Support PENYA Please Fix Script!!") );

							PT_ITEM_SPEC pItemSpec = g_xSpecManager->GetSpecItem( kExchangeItem.dwItemID );
							if( pItemSpec == NULL )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[ CONDITION ITEM ID INVALID. MMI_ID:(%s), ITEM_ID:(%d) ]" )
									, szMMIID, kExchangeItem.dwItemID );
								return false;
							}

							kExchangeItem.nItemQuantity	= 1;

							const bool bRet = kSet.mapConditionItem.insert( ExchangeItemMap::value_type( kExchangeItem.dwItemID, kExchangeItem ) ).second;
							if( bRet == false )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[ CONDITION ITEM DUPLICATED. MMI_ID(%s), ITEM_ID(%d) ]" )
									, szMMIID, kExchangeItem.dwItemID );
								FLASSERT( 0 );
								return false;
							}

							kScanner.GetToken();
						}
					}
					else if( kScanner.Token == "REMOVE" )
					{
						kScanner.GetToken();	// {
						FLASSERT( kScanner.Token == "{" );

						kScanner.GetToken();
						while( kScanner.Token != "}" )
						{
							kScanner.GetToken();
						}
					}
					else if( kScanner.Token == "PAY" )
					{
						kSet.dwPaymentLimitCount	= static_cast<DWORD>( kScanner.GetNumber() );
						kScanner.GetToken();	// {
						FLASSERT( kScanner.Token == "{" );

						kScanner.GetToken();
						while( kScanner.Token != "}" )
						{
							T_EXCHANGE_ITEM kExchangeItem;

							kExchangeItem.dwItemID = static_cast<DWORD>( CScript::GetDefineNum( kScanner.Token ) );
							if( IsPenyaItem( kExchangeItem.dwItemID ) )
								FLERROR_ASSERT_LOG_RETURN( NULL, false, _T("[EXCHANGE SCRIPT] Not Support PENYA Please Fix Script!!") );

							PT_ITEM_SPEC pItemSpec = g_xSpecManager->GetSpecItem( kExchangeItem.dwItemID );
							if( pItemSpec == NULL )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[ PAY ITEM ID INVALID. MMI_ID:(%s), ITEM_ID:(%d) ]" )
									, szMMIID, kExchangeItem.dwItemID );
								return false;
							}
							kExchangeItem.nItemQuantity		= 1;
							kExchangeItem.dwPaymentProb		= MAX_PAYMENT_PROB;

							kScanner.SetMark();
							kScanner.GetToken();
							kScanner.GoMark();
							if( kScanner.tokenType == NUMBER )
							{
								kExchangeItem.byItemFlag = static_cast<BYTE>( kScanner.GetNumber() );
							}

							const bool bRet = kSet.mapPaymentItem.insert( ExchangeItemMap::value_type( kExchangeItem.dwItemID, kExchangeItem ) ).second;
							if( bRet == false )
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "[ PAY ITEM DUPLICATED. MMI_ID(%s), ITEM_ID(%d) ]" )
									, szMMIID, kExchangeItem.dwItemID );
								FLASSERT( 0 );
								return false;
							}

							kScanner.GetToken();
						}

// 						if( dwSumPaymentProb != MAX_PAYMENT_PROB )
// 						{
// 							FLERROR_LOG( PROGRAM_NAME, _T( "[ PAYMENT PROB SUM INVALID. MMI_ID:(%s), PROB:(%d) ]" )
// 								, szMMIID, dwSumPaymentProb );
// 							return false;
// 						}
					}

					kScanner.GetToken();
				}

				if( kSet.mapConditionItem.empty() == true || kSet.mapPaymentItem.empty() == true )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "[ COND ITEM or PAY ITEM COUNT ZERO. MMI_ID:(%s) ]" ), szMMIID );
					return false;
				}

				int nItemTotalCount( kSet.mapConditionItem.size() + kSet.mapPaymentItem.size() );
				if( nItemTotalCount > MAX_ITEMDRAWCOUNT )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "[EXCHANGE SCRIPT] WARNING!!  COND and PAY ITEM TOTAL COUNT MORE THAN [%d]. Maybe UserInterface Clip - MMI_ID:(%s) ]" ), MAX_ITEMDRAWCOUNT, szMMIID );
					FLASSERT( NULL );
				}

				kSetList.mapExchangeItemList.insert( ExchangeItemListMap::value_type( dwNth, kSet ) );

				++dwNth;
			}
			else
			{
				FLASSERT( 0 );
			}

			kScanner.GetToken();
		}	// end - while( kScanner.Token != "}" )

		const bool bRet = m_mapExchange.insert( ExchangeDataMap::value_type( dwMenuID, kSetList ) ).second;
		if( bRet == false )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ DUPLICATED ]" ) );
			return false;
		}

		kScanner.GetToken();

	}	// end - while( kScanner.tok != FINISHED )


	bool	bIsValidExchangeData	= false;

	bIsValidExchangeData = CheckExchangeData();

	return	bIsValidExchangeData;
}

const T_EXCHANGE_DATA*	FLExchange::Find( DWORD dwMenuID ) const
{
	ExchangeDataCItr kCItr = m_mapExchange.find( dwMenuID );
	if( kCItr != m_mapExchange.end() )
	{
		return &kCItr->second;
	}

	return NULL;
}
