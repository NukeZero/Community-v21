
#ifndef __FLFLYFFPIECEEVENT_H__
#define __FLFLYFFPIECEEVENT_H__


#include "FLExchangeItem.h"


enum E_BASE_JOB { E_JOB_VAGRANT = 0, E_JOB_MERCENARY, E_JOB_ACROBAT, E_JOB_ASSIST, E_JOB_MAGICIAN };
enum E_SEX_TYPE { E_SEX_SEXLESS = 0, E_SEX_MALE, E_SEX_FEMALE };


const DWORD		MIN_MENU_INPUT_TICK		= 500;


struct T_CATEGORY_KEY
{
	DWORD	dwCategoey;
	DWORD	dwJob;
	DWORD	dwSex;

	T_CATEGORY_KEY() : dwCategoey( TYPE2_NONE ), dwJob( NULL_ID ), dwSex( NULL_ID )
	{
	}

	T_CATEGORY_KEY( const DWORD Categoey, const DWORD Job, const DWORD Sex ) : dwCategoey( Categoey ), dwJob( Job ), dwSex( Sex )
	{
	}

	bool	operator == ( const T_CATEGORY_KEY & rhs ) const
	{
		return ( dwCategoey == rhs.dwCategoey && dwJob == rhs.dwJob && ( dwSex == E_SEX_SEXLESS || dwSex == rhs.dwSex ) );
	}

	bool	operator < ( const T_CATEGORY_KEY & rhs ) const
	{
		return ( dwCategoey != rhs.dwCategoey )
			? ( dwCategoey < rhs.dwCategoey )
			: ( dwJob != rhs.dwJob )
			? ( dwJob < rhs.dwJob )
			: ( dwSex != E_SEX_SEXLESS && dwSex < rhs.dwSex );
	}
};


typedef std::vector< T_EXCHANGE_ITEM_DATA >					ExchangeItemDataVec;
typedef std::map< T_CATEGORY_KEY , ExchangeItemDataVec >	ExchangeCategoryMap;


class FLPacketFlyffPieceExchangeListReq;
class FLPacketExchangeFlyffPieceItemReq;
class FLPacketExchangeFlyffPieceItemReq;

class FLFlyffPieceEvent
{
	public:
		FLFlyffPieceEvent();
		~FLFlyffPieceEvent();

		static	FLFlyffPieceEvent*	GetInstance();

		bool	LoadScript();

		void	OnViewList( FLWSUser* pUser, const FLPacketFlyffPieceExchangeListReq* pMsg ) const;
		void	OnDoExchangeItem( FLWSUser* pUser, const FLPacketExchangeFlyffPieceItemReq* pMsg ) const;


	private:
		void	Init();
		bool	IsActivate() const;
		void	SetActivate( const bool bActivate );

		bool	ReprocessLoadData( const ExchangeItemDataMap & mapExchangeItemData );
		bool	InsertCatagoryData( const T_CATEGORY_KEY & kKey, const T_EXCHANGE_ITEM_DATA & kValue, ExchangeCategoryMap & _Map ) const;

		DWORD	GetBaseJob( const DWORD dwJob ) const;
		DWORD	GetSexType( const DWORD dwSex ) const;

		bool	GetExchangeItemData( const DWORD dwIndex, T_EXCHANGE_ITEM_DATA & kExchangeItemData ) const;
		bool	GetExchangeCategoryData( const T_CATEGORY_KEY kCategoryKey, ExchangeItemDataVec & vecExchangeItemData ) const;

		bool	DoExchangeItem( FLWSUser* pUser, const FLPacketExchangeFlyffPieceItemReq* pMsg ) const;

		bool	IsHasConditionItem( FLWSUser* pUser, const T_ITEM_BASE_INFO & kCondItemInfo ) const;
		bool	RemoveCondItem( FLWSUser* pUser, const T_ITEM_BASE_INFO & kCondItemInfo ) const;
		DWORD	GetRemoveItemObjID( FLWSUser* pUser, const T_ITEM_BASE_INFO & kCondItemInfo ) const;


	private:
		bool	m_bActivate;

		FLExchangeItem			m_kExchangeItem;

		ExchangeItemDataMap		m_mapExchangeItemData;
		ExchangeCategoryMap		m_mapCommonCategoryData;
		ExchangeCategoryMap		m_mapBaseJobCategoryData;
};

#define		g_pFlyffPieceEvent		FLFlyffPieceEvent::GetInstance()

#endif // __FLFLYFFPIECEEVENT_H__
