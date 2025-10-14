
#ifndef __FLEXCHANGEITEM_H__
#define __FLEXCHANGEITEM_H__


typedef std::map< DWORD /* dwIndex */, T_EXCHANGE_ITEM_DATA >		ExchangeItemDataMap;


class FLExchangeItem
{
	public:
		FLExchangeItem();
		~FLExchangeItem();


		bool	LoadScript( const TCHAR* pszFileName, ExchangeItemDataMap & mapExchangeItemData ) const;


	private:
};

#endif // __FLEXCHANGEITEM_H__
