#pragma once


#include "../_CommonDefine/Packet/FLPacketWDB_ConsignmentSale.h"



struct FLStringKey
{
	FLStringKey( const UINT64 u64srl, const TCHAR * sz ) : u64SRL( u64srl )		{	FLStrcpy( szItem, _countof( szItem ), sz );				}
	bool operator < ( const FLStringKey & r ) const								{		const int ret = ::strcmp( szItem, r.szItem );	return ( ret == 0 ) ? u64SRL < r.u64SRL : ( ret < 0 );	}

	const UINT64			u64SRL;
	TCHAR					szItem[ ITEM_NAME_BUFFER_LEN ];
};

struct FLIntKey
{
	FLIntKey( const UINT64 u64srl, const int i ) : u64SRL( u64srl ), n( i ) {}
	bool operator < ( const FLIntKey & r ) const		{			return ( n == r.n ) ? u64SRL < r.u64SRL : n < r.n;				}

	const UINT64			u64SRL;
	const int				n;
};

struct FLDwordKey
{
	FLDwordKey( const UINT64 u64srl, const DWORD i ) : u64SRL( u64srl ), n( i ) {}
	bool operator < ( const FLDwordKey & r ) const		{			return ( n == r.n ) ? u64SRL < r.u64SRL : n < r.n;				}

	const UINT64			u64SRL;
	const DWORD				n;
};

struct FLOnePriceKey
{
	FLOnePriceKey( const UINT64 u64srl, const DWORD price, const int num ) : u64SRL( u64srl ), dwTotPrice( price ), nNum( num ) {}
	bool operator < ( const FLOnePriceKey & r ) const		
	{	
		const DWORD PRECISION		= 10000;
		const UINT64 u64OnePrice	= ( ( UINT64 )dwTotPrice * PRECISION ) / nNum;
		const UINT64 u64rOnePrice	= ( ( UINT64 )r.dwTotPrice * PRECISION ) / r.nNum;

		return ( u64OnePrice == u64rOnePrice ) ? u64SRL < r.u64SRL : ( u64OnePrice < u64rOnePrice );				
	}

	const UINT64			u64SRL;
	const DWORD				dwTotPrice;
	const int				nNum;
};

struct FLLimitLevelKey
{
	FLLimitLevelKey( const UINT64 u64srl, const int limitLevel, const TCHAR * szItem ) : u64SRL( u64srl ), nLimitLevel( limitLevel ) 
	{
		FLStrcpy( szItemName, _countof( szItemName ), szItem );
	}
	bool operator < ( const FLLimitLevelKey & r ) const		
	{	
		const int ret		= ::strcmp( szItemName, r.szItemName );
		return ( nLimitLevel == r.nLimitLevel )
			? ( ( ret == 0 ) ?		u64SRL < r.u64SRL	:	ret < 0 )
			: nLimitLevel < r.nLimitLevel;
	}

	const UINT64			u64SRL;
	const int				nLimitLevel;
	TCHAR					szItemName[ ITEM_NAME_BUFFER_LEN ];
};


class FLDBConsignmentSale
{
public:
	struct FLRegistCount// : public CMclCritSec
	{
		FLRegistCount() {		::memset( arrCount, 0, sizeof( arrCount ) ); }

		DWORD				arrCount[ __REGIST_MODE_MAX__ ];
	};


	struct FLSaleInfo
	{
		FLSaleInfo();
		FLSaleInfo( const FLSaleInfo & r ); 
		~FLSaleInfo();

		virtual FLSaleInfo & operator = ( const FLSaleInfo & r );

		UINT64				u64ItemInstanceSRL;
		u_long				uPlayerID;
		//serverindex
		int					nPrice;
		int					nOnePrice;
		TCHAR				szItemName[ ITEM_NAME_BUFFER_LEN ];
		TCHAR				szPlayerName[ MAX_NAME ];
		DWORD				dwItemType1;
		DWORD				dwItemType2;
		DWORD				dwGrade;
		int					nLimitLevel;
		int					nAbilityOption;
		E_REGIST_MODE		eRegistMode;
		FLTm				tmExpiredDate;

		mutable FLItemElem	kItemElem;
	};

public:
	FLDBConsignmentSale();
	virtual ~FLDBConsignmentSale();

	static FLDBConsignmentSale & GetInstance();

	bool					_AddItem_AndSendRegistedCount( const DPID dpid, const FLSaleInfo & kSaleInfo );				//Send와 같이 묶은건 순서 동기화 때문
	bool					_PopItem_AndSendRegistedCount( FLSaleInfo & o_kSaleInfo, const DPID dpid, const UINT64 u64SaleInfoSRL );

private:
	bool					addItem( const FLSaleInfo & kSaleInfo );
	bool					popItem( const UINT64 u64SaleInfoSRL, FLSaleInfo & o_kSaleInfo );

public:
	void					_SearchItemList( nsConsignmentSale::FLPacketWDB_SearchAck & o_ack, const nsConsignmentSale::FLPacketWDB_SearchReq & req );
	void					_GetRegistedCount_AndSendRegistedCount( const DPID dpid, const u_long uPlayerID );
	FLRegistCount 			_SendGetRegistedCount( const DPID dpid, const u_long uPlayerID, const nsConsignmentSale::FLPacketWDB_CheckRegistedCountReq & req );

	void					_GetRegistedInfoList( nsConsignmentSale::FLPacketWDB_RegistedInfoListAck & o_ack, const u_long uPlayerID );
	bool 					_GetPriceInfo( u_long & o_uPlayerID, int & o_nPrice, const UINT64 u64SaleInfoSRL );

	//Using SRL
	bool					_AddReserve( const UINT64 u64SaleInfoSRL );
	void					_RemoveReserve( const UINT64 u64SaleInfoSRL );
	
	//expired data
	bool					_AddExpiredData( const FLSaleInfo & kSaleInfo );

	//data manager
	bool					addExpiredData( const FLSaleInfo & kSaleInfo );
	bool					popExpiredData( const UINT64 u64SaleInfoSRL, FLSaleInfo & o_kSaleInfo );

	//Regist Count
	void					sumUserRegistCount( const u_long uPlayerID, const E_REGIST_MODE eMode, const int nSum );
	void					tryGetRegistedCount_AndSendRegistedCount( const DPID dpid, const u_long uPlayerID );

	//util
	static void				ToOneSaleInfo( nsConsignmentSale::FLOneSaleInfo & o_kOneSaleInfo, const FLSaleInfo & kSaleInfo );
	static bool				checkCondition( const nsConsignmentSale::FLPacketWDB_SearchReq & req, const FLDBConsignmentSale::FLSaleInfo & kSaleInfo );

private:
	// ConsignmentSale Data Manager
	CMclCritSec												m_kGlobalCS;
	std::map< UINT64/*SaleSRL*/, FLSaleInfo >					m_mapSaleInfo;	

	// logic lock
	std::map< UINT64/*SaleSRL*/, UINT64/*SaleSRL*/ >				m_mapUsginID;

	// sorted list
	std::map< FLStringKey/*itemName*/, UINT64/*SaleSRL*/ >		m_mapSorted_ItemName;
	std::map< FLLimitLevelKey , UINT64/*SaleSRL*/ >				m_mapSorted_LimitLevel;
	std::map< FLIntKey/*nPrice*/ , UINT64/*SaleSRL*/ >			m_mapSorted_Price;
	std::map< FLOnePriceKey , UINT64/*SaleSRL*/ >				m_mapSorted_OnePrice;
	std::map< FLIntKey/*nAbilityOption*/ , UINT64/*SaleSRL*/ >	m_mapSorted_AbilityOption;

	// 만료된 등록 아이템
	std::map< UINT64/*SaleSRL*/, FLSaleInfo >					m_mapExpiredSaleInfo;	

	// 유저 등록 카운트
	std::map< u_long/*PlayerID*/, FLRegistCount >				m_mapUserRegistCount;

protected:		//no lock
	template < class _Map >
	static void					getList_FromMap( std::vector< UINT64 > & o_vec
									, const nsConsignmentSale::FLPacketWDB_SearchReq & req
									, const _Map & Map, const size_t countMax );
};

inline FLDBConsignmentSale & DB_CONSIGNMENT_SALE()	{	return FLDBConsignmentSale::GetInstance();		}



//----------------------------------------------------- template 펑션 구현 시작 ---------------------------------------------------------//


template < class _Map >
void FLDBConsignmentSale::getList_FromMap( std::vector< UINT64 > & o_vec
										  , const nsConsignmentSale::FLPacketWDB_SearchReq & req, const _Map & Map, const size_t countMax )
{
	size_t c			= 0;
	const FLTm tmCurr	= DATE_TIMER().GetCurrentTime();

	std::vector< UINT64 > vecExpired;
	
	if( req.m_bASC == TRUE )
	{
		for( _Map::const_iterator it = Map.begin(); it != Map.end(); ++it )
		{
			//1. find data
			const UINT64 u64SRL	= it->second;
			std::map< UINT64, FLDBConsignmentSale::FLSaleInfo >::iterator itData = DB_CONSIGNMENT_SALE().m_mapSaleInfo.find( u64SRL );
			if( itData == DB_CONSIGNMENT_SALE().m_mapSaleInfo.end() )
			{
				continue;
			}

			//2. check expired
			const FLDBConsignmentSale::FLSaleInfo & kSaleInfo = itData->second;
			if( tmCurr > kSaleInfo.tmExpiredDate )
			{
				vecExpired.push_back( u64SRL );
				continue;
			}

			//3. check condition
			if( FLDBConsignmentSale::checkCondition( req, kSaleInfo ) == false )
				continue;
			
			++c;
			if( c >= countMax )
				break;

			o_vec.push_back( u64SRL );
		}
	}
	else if( req.m_bASC == FALSE )
	{
		for( _Map::const_reverse_iterator it = Map.rbegin(); it != Map.rend(); ++it )
		{
			//1. find data
			const UINT64 u64SRL	= it->second;
			std::map< UINT64, FLDBConsignmentSale::FLSaleInfo >::iterator itData = DB_CONSIGNMENT_SALE().m_mapSaleInfo.find( u64SRL );
			if( itData == DB_CONSIGNMENT_SALE().m_mapSaleInfo.end() )
			{
				continue;
			}

			//2. check expired
			const FLDBConsignmentSale::FLSaleInfo & kSaleInfo = itData->second;
			if( tmCurr > kSaleInfo.tmExpiredDate )
			{
				vecExpired.push_back( u64SRL );
				continue;
			}

			//3. check condition
			if( FLDBConsignmentSale::checkCondition( req, kSaleInfo ) == false )
				continue;

			++c;
			if( c >= countMax )
				break;

			o_vec.push_back( u64SRL );
		}
	}

	// 시간 만료 아이템 정리
	for( std::vector< UINT64 >::iterator it = vecExpired.begin(); it != vecExpired.end(); ++it )
	{
		const UINT64 u64SRL	= *it;
		FLSaleInfo kExpiredSaleInfo;
		if( DB_CONSIGNMENT_SALE().popItem( u64SRL, kExpiredSaleInfo ) == true )
		{
			DB_CONSIGNMENT_SALE().addExpiredData( kExpiredSaleInfo );
		}
	}

}





