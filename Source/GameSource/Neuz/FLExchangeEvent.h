#ifndef __FLEXCHANGEEVENT_H__
#define __FLEXCHANGEEVENT_H__


#include "../_Common/FLExchange.h"

class FLExchangeEvent
{
	public:

									FLExchangeEvent( void );
									~FLExchangeEvent( void );

		static	FLExchangeEvent*	GetInstance( void );

		bool						LoadScript( void );

		bool						GetSetTextID( DWORD dwMenuID, TextIDVec* pTextID );
		bool						GetDescriptionTextID( DWORD dwMenuID, TextIDVec* pTextID );
		bool						GetResultMsgTextID( DWORD dwMenuID, DWORD dwListNth, TextIDVec* pTextID );
		bool						GetExchangeItemList( const DWORD dwMenuID, OUT ExchangeItemListMap& mapExchangeItemList );


	private:


		FLExchange					m_kExchange;
};

#define		g_pExchangeEvent	FLExchangeEvent::GetInstance()

#endif // __FLEXCHANGEEVENT_H__
