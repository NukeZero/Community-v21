
#include "StdAfx.h"
#include "FLExchangeItem.h"


FLExchangeItem::FLExchangeItem()
{

}

FLExchangeItem::~FLExchangeItem()
{

}

bool	FLExchangeItem::LoadScript( const TCHAR* pszFileName, ExchangeItemDataMap & mapExchangeItemData ) const
{
	CScript kScanner;

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), pszFileName );

	if( kScanner.Load( szFullPath, FALSE ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return false;
	}

	DWORD dwLine = 0;

	DWORD dwIndex = static_cast<DWORD>( kScanner.GetNumber() );

	while( kScanner.tok != FINISHED )
	{
		++dwLine;

		T_EXCHANGE_ITEM_DATA kData;

		kData.dwIndex				= dwIndex;
		kData.kPayItem.dwItemID		= static_cast<DWORD>( kScanner.GetNumber() );
		kData.kPayItem.nQuantity	= kScanner.GetNumber();
		kData.kCondItem.dwItemID	= static_cast<DWORD>( kScanner.GetNumber() );
		kData.kCondItem.nQuantity	= kScanner.GetNumber();
		kData.bBinds				= ( kScanner.GetNumber() != 0 );

		const PT_ITEM_SPEC	pPayItemProp	= g_xSpecManager->GetSpecItem( kData.kPayItem.dwItemID );
		if( pPayItemProp == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ PAY ITEM SPEC NOT FOUND. Line:(%d), ItemID:(%d) ]" )
				, dwLine, kData.kPayItem.dwItemID );
			FLASSERT( 0 );
			return false;
		}

		if( kData.kPayItem.nQuantity <= 0 )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ PAY ITEM COUNT INVALID. Line:(%d), ItemID(%d), Count:(%d) ]" )
				, dwLine, kData.kPayItem.dwItemID, kData.kPayItem.nQuantity );
			FLASSERT( 0 );
			return false;
		}

		const PT_ITEM_SPEC	pCondItemProp	= g_xSpecManager->GetSpecItem( kData.kCondItem.dwItemID );
		if( pCondItemProp == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ COND ITEM SPEC NOT FOUND. Line:(%d), ItemID:(%d) ]" )
				, dwLine, kData.kCondItem.dwItemID );
			FLASSERT( 0 );
			return false;
		}

		if( kData.kCondItem.nQuantity <= 0 )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ COND ITEM COUNT INVALID. Line:(%d), ItemID(%d), Count:(%d) ]" )
				, dwLine, kData.kCondItem.dwItemID, kData.kCondItem.nQuantity );
			FLASSERT( 0 );
			return false;
		}

		if( mapExchangeItemData.insert( ExchangeItemDataMap::value_type( dwIndex, kData ) ).second == false )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INDEX DUPLRICATED. Line:(%d), Index:(%d) ]" )
				, dwLine, dwIndex );
			FLASSERT( 0 );
			return false;
		}

		dwIndex = static_cast<DWORD>( kScanner.GetNumber() );
	}

	return ( dwLine != 0 ) ? true : false;
}
