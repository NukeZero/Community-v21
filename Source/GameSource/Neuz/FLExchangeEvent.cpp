#include "StdAfx.h"
#include "FLExchangeEvent.h"


FLExchangeEvent::FLExchangeEvent()
{

}

FLExchangeEvent::~FLExchangeEvent()
{

}

FLExchangeEvent*	FLExchangeEvent::GetInstance()
{
	static FLExchangeEvent	kExchangeEvent;

	return & kExchangeEvent;
}

bool	FLExchangeEvent::LoadScript()
{
	return m_kExchange.LoadScript();
}

bool	FLExchangeEvent::GetSetTextID( DWORD dwMenuID, TextIDVec* pTextID )
{
 	const T_EXCHANGE_DATA* pSetList = m_kExchange.Find( dwMenuID );
 	if( pSetList != NULL )
 	{
		for( ExchangeItemListCItr kCItr = pSetList->mapExchangeItemList.begin(); kCItr != pSetList->mapExchangeItemList.end(); ++kCItr )
		{
			pTextID->push_back( kCItr->second.dwSetTextID );
		}
	}

	if( pTextID->empty() == false )
	{
		return true;
	}

	return false;
}

bool	FLExchangeEvent::GetDescriptionTextID( DWORD dwMenuID, TextIDVec* pTextID )
{
	const T_EXCHANGE_DATA* pSetList = m_kExchange.Find( dwMenuID );
	if( pSetList != NULL )
	{
		pTextID->assign( pSetList->vecDesciprtionTextID.begin(), pSetList->vecDesciprtionTextID.end() );
	}

	if( pTextID->empty() == false )
	{
		return true;
	}

	return false;
}

bool	FLExchangeEvent::GetResultMsgTextID( DWORD dwMenuID, DWORD dwListNth, TextIDVec* pTextID )
{
	const T_EXCHANGE_DATA* pSetList = m_kExchange.Find( dwMenuID );

	ExchangeItemListCItr kCItr = pSetList->mapExchangeItemList.find( dwListNth );
	if( kCItr == pSetList->mapExchangeItemList.end() )
	{
		pTextID->assign( kCItr->second.vecResultMsgTextID.begin(), kCItr->second.vecResultMsgTextID.begin() );
	}

	if( pTextID->empty() == false )
	{
		return true;
	}

	return false;
}


bool	FLExchangeEvent::GetExchangeItemList( const DWORD dwMenuID, OUT ExchangeItemListMap& mapExchangeItemList )
{
	const T_EXCHANGE_DATA* pSetList = m_kExchange.Find( dwMenuID );
	FLERROR_ASSERT_LOG_RETURN( pSetList, false, _T("[교환 이벤트] MMI에 해당하는 교환 정보를 찾을 수 없습니다. MMI ID - [%u]"), dwMenuID );

	mapExchangeItemList = pSetList->mapExchangeItemList;	

	if( mapExchangeItemList.empty() )
		return false;

	return true;
}
