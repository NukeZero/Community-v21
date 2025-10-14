#ifndef __FLEXCHANGE_H__
#define __FLEXCHANGE_H__


enum { EXCHANGE_TYPE_NONE, EXCHANGE_TYPE_NORMAL_ITEM, EXCHANGE_TYPE_ENCHANT_ITEM, EXCHANGE_TYPE_ENCHANT_MOVE, EXCHANGE_TYPE_MAX };

const DWORD		MAX_EXCHANGE_ENCHANT_MOVE	= 1;


typedef struct _T_EXCHANGE_ITEM
{
	DWORD	dwItemID;
	int		nItemQuantity;
	DWORD	dwMinGeneralEnchant;
	DWORD	dwMaxGeneralEnchant;
	DWORD	dwMinAttributeEnchant;
	DWORD	dwMaxAttributeEnchant;
	BYTE	byAttributeKind;
	bool	bCheckScriptAttribute;
	DWORD	dwPaymentProb;
	BYTE	byItemFlag;

	_T_EXCHANGE_ITEM()
	{
		dwItemID				= 0;
		nItemQuantity			= 0;
		dwMinGeneralEnchant		= 0;
		dwMaxGeneralEnchant		= 0;
		dwMinAttributeEnchant	= 0;
		dwMaxAttributeEnchant	= 0;
		byAttributeKind			= SAI79::NO_PROP;
		bCheckScriptAttribute	= false;
		dwPaymentProb			= 0;
		byItemFlag				= FLItemElem::normal;
	};

} T_EXCHANGE_ITEM, *PT_EXCHANGE_ITEM;

typedef std::map< DWORD /* ITEM ID*/, T_EXCHANGE_ITEM >		ExchangeItemMap;
typedef ExchangeItemMap::iterator							ExchangeItemItr;
typedef ExchangeItemMap::const_iterator						ExchangeItemCItr;


typedef std::vector< DWORD /*TEXT ID*/ >	TextIDVec;
typedef TextIDVec::iterator					TextIDItr;
typedef TextIDVec::const_iterator			TextIDCItr;

typedef struct _T_EXCHANGE_ITEM_LIST
{
	BYTE				byExchangeType;
	DWORD				dwSetTextID;
	TextIDVec			vecResultMsgTextID;
	ExchangeItemMap		mapConditionItem;
	ExchangeItemMap		mapPaymentItem;
	DWORD				dwPaymentLimitCount;

	_T_EXCHANGE_ITEM_LIST()
	{
		byExchangeType		= EXCHANGE_TYPE_NONE;
		dwSetTextID			= NULL_ID;
		dwPaymentLimitCount = 0;
	};

} T_EXCHANGE_ITEM_LIST, *PT_EXCHANGE_ITEM_LIST;

typedef std::map< DWORD /* Nth */, T_EXCHANGE_ITEM_LIST >	ExchangeItemListMap;
typedef ExchangeItemListMap::iterator						ExchangeItemListItr;
typedef ExchangeItemListMap::const_iterator					ExchangeItemListCItr;

typedef struct _T_EXCHANGE_DATA
{
	TextIDVec				vecDesciprtionTextID;
	ExchangeItemListMap		mapExchangeItemList;

} T_EXCHANGE_DATA, *PT_EXCHANGE_DATA;

typedef std::map< DWORD /* MMI ID */, T_EXCHANGE_DATA >	ExchangeDataMap;
typedef ExchangeDataMap::iterator						ExchangeDataItr;
typedef ExchangeDataMap::const_iterator					ExchangeDataCItr;


const DWORD MAX_PAYMENT_PROB	= 1000000;


class FLExchange
{
	public:
		FLExchange();
		~FLExchange();

		bool	LoadScript();
		bool	CheckExchangeData();

		const T_EXCHANGE_DATA*	Find( DWORD dwMenuID ) const;


	private:
		void	Clear();
		bool	IsPenyaItem( const DWORD dwItemID ) const;


	private:
		ExchangeDataMap		m_mapExchange;
};


#endif // __FLEXCHANGE_H__
