
#ifndef __FLEXCHANGEEVENT_H__
#define __FLEXCHANGEEVENT_H__

#include "../_Common/FLExchange.h"
#include "../_CommonDefine/Packet/FLPacketItemExchange.h"

class FLExchangeEvent
{
	public:
		FLExchangeEvent();
		~FLExchangeEvent();

		static	FLExchangeEvent*	GetInstance();

		bool	LoadScript();

		void	OnDoExchange( FLWSUser* pUser, const FLPacketItemExchangeReq* pMsg ) const;

	private:
		bool	CanExchange( FLWSUser* pUser, const FLPacketItemExchangeReq* pMsg ) const;
		
		bool	DoExchange( FLWSUser* pUser, const FLPacketItemExchangeReq* pMsg ) const;

		bool	IsValidStateUser( FLWSUser* pUser ) const;
		bool	IsValidStateMsg( const FLPacketItemExchangeReq* pMsg ) const;
		
		bool	CheckConditionItem( FLWSUser* pUser, const ExchangeItemMap & mapExchangeItem, const BYTE byExchangeType, const int nExchangeCount ) const;
		bool	CheckPaymentItem( FLWSUser* pUser, const ExchangeItemMap & mapExchangeItem, const int nExchangeCount ) const;

		bool	RemoveExchangeItem( FLWSUser* pUser, const ExchangeItemMap & mapExchangeItem, const BYTE byExchangeType, const int nExchangeCount, FLItemElem & kRemoveItemElem ) const;
		void	PayExchangeItem( FLWSUser* pUser, const ExchangeItemMap & mapExchangeItem, const BYTE byExchangeType, const FLItemElem & kRemoveItemElem ) const;

		bool	IsHasConditionItem( FLWSUser* pUser, const T_EXCHANGE_ITEM & kExchangeItem, const BYTE byExchangeType, const int nExchangeCount ) const;
		DWORD	GetRemoveItemObjID( FLWSUser* pUser, const T_EXCHANGE_ITEM & kExchangeItem, const BYTE byExchangeType ) const;
		bool	GetPaymentItemList( const ExchangeItemMap & mapExchangeItem, const DWORD dwPaymentLimitCount, const int nExchangeCount, ExchangeItemMap & mapPaymentItem ) const;


	private:
		FLExchange		m_kExchange;
};

#define		g_pExchangeEvent	FLExchangeEvent::GetInstance()

#endif // __FLEXCHANGEEVENT_H__
