
#include "StdAfx.h"
#include "FLDBConsignmentSale.h"
#include "../_CommonDefine/FLCommonCount.h"
#include "../DatabaseServer/dptrans.h"
#include <algorithm>


FLDBConsignmentSale::FLDBConsignmentSale()
{
}

FLDBConsignmentSale::~FLDBConsignmentSale()
{
}

FLDBConsignmentSale & FLDBConsignmentSale::GetInstance()
{
	static FLDBConsignmentSale inst; return inst;
}


bool FLDBConsignmentSale::_AddItem_AndSendRegistedCount( const DPID dpid, const FLSaleInfo & kSaleInfo )
{
	CMclAutoLock Lock( m_kGlobalCS );
	if( addItem( kSaleInfo ) == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T("[ DUPLICATE ID (%u64) ]"), kSaleInfo.u64ItemInstanceSRL );
		return false;
	}

	sumUserRegistCount( kSaleInfo.uPlayerID, kSaleInfo.eRegistMode, +1 );
	tryGetRegistedCount_AndSendRegistedCount( dpid, kSaleInfo.uPlayerID );
	return true;
}

bool FLDBConsignmentSale::_PopItem_AndSendRegistedCount( FLSaleInfo & o_kSaleInfo, const DPID dpid, const UINT64 u64SaleInfoSRL )
{
	CMclAutoLock Lock( m_kGlobalCS );
	if( popItem( u64SaleInfoSRL, o_kSaleInfo ) == false )
	{
		if( popExpiredData( u64SaleInfoSRL, o_kSaleInfo ) == false )
			return false;
	}

	sumUserRegistCount( o_kSaleInfo.uPlayerID, o_kSaleInfo.eRegistMode, -1 );
	tryGetRegistedCount_AndSendRegistedCount( dpid, o_kSaleInfo.uPlayerID );
	return true;
}


bool		FLDBConsignmentSale::checkCondition( const nsConsignmentSale::FLPacketWDB_SearchReq & req, const FLDBConsignmentSale::FLSaleInfo & kSaleInfo )
{
	using namespace nsConsignmentSale;

	//등급 체크
	size_t i = 0;
	for( ; i < FLPacketWDB_SearchReq::GRADE_MULTI_CHECK_MAX; ++i ) 
	{
		if( req.m_arrGrade[ i ]	== ITEM_GRADE_NONE )
			continue;

		if( req.m_arrGrade[ i ] == kSaleInfo.dwGrade )
			break;
	}

	if( i == FLPacketWDB_SearchReq::GRADE_MULTI_CHECK_MAX )
		return false;

	//제한 등급 체크
	if( req.m_nLimitLevelMin > kSaleInfo.nLimitLevel || kSaleInfo.nLimitLevel > req.m_nLimitLevelMax )
		return false;

	//제련 등급 체크
	if( req.m_nAbilityOptionMin > kSaleInfo.nAbilityOption || kSaleInfo.nAbilityOption > req.m_nAbilityOptionMax )
		return false;

	//아이템 카테고리 체크
	if( req.m_dwItemType1 != TYPE1_NONE && req.m_dwItemType1 != kSaleInfo.dwItemType1 )
		return false;

	if( req.m_dwItemType2 != TYPE2_NONE && req.m_dwItemType2 != kSaleInfo.dwItemType2 )
		return false;

	//이름 체크
	if( req.m_bSearchBySameName == TRUE && ::strcmp( kSaleInfo.szItemName, req.m_szSearchName ) != 0 )
		return false;

	if( req.m_bSearchBySameName == FALSE && ::strstr( kSaleInfo.szItemName, req.m_szSearchName ) == NULL )
		return false;

	return true;
}


void FLDBConsignmentSale::_SearchItemList( nsConsignmentSale::FLPacketWDB_SearchAck & o_ack, const nsConsignmentSale::FLPacketWDB_SearchReq & req )
{
	using namespace nsConsignmentSale;

	const size_t ONE_PAGE_MAX = 10;
	const size_t OPTIMIZATION_COUNT = 200;
	const size_t COUNT_MAX	= OPTIMIZATION_COUNT/*page*/ * 10;

	o_ack.m_vecSaleInfoList.reserve( ONE_PAGE_MAX );

	
	std::vector< UINT64 > vec;
	vec.reserve( COUNT_MAX );

	//sort
	CMclAutoLock Lock( m_kGlobalCS );

	switch( req.m_eOrderingOption )
	{
	case FLPacketWDB_SearchReq::E_NONE:
		//__noop;		//break;
	case FLPacketWDB_SearchReq::E_ITEM_NAME:
		getList_FromMap( vec, req, m_mapSorted_ItemName, COUNT_MAX );
		break;

	case FLPacketWDB_SearchReq::E_ITEM_LEVEL:
		getList_FromMap( vec, req, m_mapSorted_LimitLevel, COUNT_MAX );
		break;

	case FLPacketWDB_SearchReq::E_PRICE:
		getList_FromMap( vec, req, m_mapSorted_Price, COUNT_MAX );
		break;

	case FLPacketWDB_SearchReq::E_ONE_PRICE:
		getList_FromMap( vec, req, m_mapSorted_OnePrice, COUNT_MAX );
		break;

	case FLPacketWDB_SearchReq::E_ABILITY_OPTION:
		getList_FromMap( vec, req, m_mapSorted_AbilityOption, COUNT_MAX );
		break;

	default:
		__noop;
		break;
	}

	//result
	const DWORD dwTotCount			= vec.size();
	const DWORD dwTailStartIndex	= dwTotCount - ( dwTotCount % CONSIGNMENT_SALE_ITEM_LIST_MAX );
	const DWORD dwReqeustStartIndex	= req.m_dwPage * CONSIGNMENT_SALE_ITEM_LIST_MAX;
	const DWORD dwRealStartIndex	= ( dwReqeustStartIndex > dwTailStartIndex ) ? dwTailStartIndex : dwReqeustStartIndex;


	for( DWORD i = dwRealStartIndex
		; i < dwTotCount && i < ( dwRealStartIndex + CONSIGNMENT_SALE_ITEM_LIST_MAX )
		; ++i )
	{
	//	const DWORD real_i	= ( req.m_bASC == TRUE ) ? i : dwTotCount - ( i + 1 );

		const UINT64 u64SRL	= vec.at( i );
		std::map< UINT64, FLSaleInfo >::const_iterator it = m_mapSaleInfo.find( u64SRL );
		if( it == m_mapSaleInfo.end() )
		{
			o_ack.m_vecSaleInfoList.push_back( FLOneSaleInfo() );
			continue;
		}

		const FLSaleInfo & kSaleInfo = it->second;

		//convert
		FLOneSaleInfo kOneSaleInfo;
		ToOneSaleInfo( kOneSaleInfo, kSaleInfo );

		o_ack.m_vecSaleInfoList.push_back( kOneSaleInfo );
	}


	o_ack.m_dwCountMax		= dwTotCount;
	o_ack.m_dwPage			= min( req.m_dwPage, o_ack.m_dwCountMax / CONSIGNMENT_SALE_ITEM_LIST_MAX );
}


void FLDBConsignmentSale::_GetRegistedCount_AndSendRegistedCount( const DPID dpid, const u_long uPlayerID )
{
	using namespace nsConsignmentSale;

	CMclAutoLock Lock( m_kGlobalCS );		//send 순서를 동기화 해줘야 하므로
	tryGetRegistedCount_AndSendRegistedCount( dpid, uPlayerID );
}


void FLDBConsignmentSale::tryGetRegistedCount_AndSendRegistedCount( const DPID dpid, const u_long uPlayerID )
{
	using namespace nsConsignmentSale;

	FLPacketWDB_GetRegistedCountAck ack;
	ack.m_uPlayerID			= uPlayerID;

	std::map< u_long, FLRegistCount >::const_iterator it = m_mapUserRegistCount.find( uPlayerID );
	if( it == m_mapUserRegistCount.end() )
	{
		CDPTrans::GetInstance()->SendPacket( dpid, &ack );
		return;
	}

	const FLRegistCount & kRegistCount = it->second;
	ack.m_dwDefaultRegistModeCount	= kRegistCount.arrCount[ DEFAULT_REGIST_MODE ];
	ack.m_dwItemRegistModeCount		= kRegistCount.arrCount[ ITEM_REGIST_MODE ];
	CDPTrans::GetInstance()->SendPacket( dpid, &ack );	
	return;
}

FLDBConsignmentSale::FLRegistCount FLDBConsignmentSale::_SendGetRegistedCount( const DPID dpid, const u_long uPlayerID, const nsConsignmentSale::FLPacketWDB_CheckRegistedCountReq & req )
{
	using namespace nsConsignmentSale;

	CMclAutoLock Lock( m_kGlobalCS );

	FLRegistCount kRegistCount;
	std::map< u_long, FLRegistCount >::const_iterator it = m_mapUserRegistCount.find( uPlayerID );
	if( it != m_mapUserRegistCount.end() )
		kRegistCount		= it->second;

	FLPacketWDB_CheckRegistedCountAck ack;
	ack.m_uPlayerID		= req.m_uPlayerID;
	ack.m_dwDefaultRegistCount	= kRegistCount.arrCount[ DEFAULT_REGIST_MODE ];
	ack.m_dwItemRegistCount		= kRegistCount.arrCount[ ITEM_REGIST_MODE ];

	ack.m_dwItemObjID			= req.m_dwItemObjID;
	ack.m_nItemCost				= req.m_nItemCost;
	ack.m_nItemCount			= req.m_nItemCount;

	CDPTrans::GetInstance()->SendPacket( dpid, &ack );

	return kRegistCount;
}

void FLDBConsignmentSale::_GetRegistedInfoList( nsConsignmentSale::FLPacketWDB_RegistedInfoListAck & o_ack, const u_long uPlayerID )
{
	using namespace nsConsignmentSale;
	//수집
	CMclAutoLock Lock( m_kGlobalCS );

	for( std::map< UINT64, FLSaleInfo >::const_iterator it = m_mapExpiredSaleInfo.begin(); it != m_mapExpiredSaleInfo.end(); ++it )
	{
		const FLSaleInfo & kSaleInfo	= it->second;
		if( kSaleInfo.uPlayerID != uPlayerID )
			continue;

		FLOneSaleInfo kOneSaleInfo;
		ToOneSaleInfo( kOneSaleInfo, kSaleInfo );

		o_ack.m_vecSaleInfoList.push_back( kOneSaleInfo );
	}

	for( std::map< UINT64, FLSaleInfo >::const_iterator it = m_mapSaleInfo.begin(); it != m_mapSaleInfo.end(); ++it )
	{
		const FLSaleInfo & kSaleInfo	= it->second;
		if( kSaleInfo.uPlayerID != uPlayerID )
			continue;

		FLOneSaleInfo kOneSaleInfo;
		ToOneSaleInfo( kOneSaleInfo, kSaleInfo );

		o_ack.m_vecSaleInfoList.push_back( kOneSaleInfo );
	}
}

bool FLDBConsignmentSale::_GetPriceInfo( u_long & o_uPlayerID, int & o_nPrice, const UINT64 u64SaleInfoSRL )
{
	CMclAutoLock Lock( m_kGlobalCS );
	std::map< UINT64, FLSaleInfo >::const_iterator it = m_mapSaleInfo.find( u64SaleInfoSRL );
	if( it == m_mapSaleInfo.end() )
		return false;

	const FLSaleInfo & kSaleInfo = it->second;
	o_uPlayerID				= kSaleInfo.uPlayerID;
	o_nPrice				= kSaleInfo.nPrice;
	return true;
}


void FLDBConsignmentSale::ToOneSaleInfo( nsConsignmentSale::FLOneSaleInfo & o_kOneSaleInfo, const FLSaleInfo & kSaleInfo )
{
	using namespace nsConsignmentSale;

	o_kOneSaleInfo.u64SaleSRL		= kSaleInfo.u64ItemInstanceSRL;
	o_kOneSaleInfo.nItemCostTot		= kSaleInfo.nPrice;
	FLStrcpy( o_kOneSaleInfo.szItemName, _countof( o_kOneSaleInfo.szItemName ), kSaleInfo.szItemName );
	FLStrcpy( o_kOneSaleInfo.szPlayerName, _countof( o_kOneSaleInfo.szPlayerName ), kSaleInfo.szPlayerName );
	o_kOneSaleInfo.tmExpiredDate	= kSaleInfo.tmExpiredDate;
	o_kOneSaleInfo.kItemElem		= kSaleInfo.kItemElem;

	__time64_t tExpired				= 0;
	if( FLDateTimeToTimeT( tExpired, o_kOneSaleInfo.tmExpiredDate ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID TM(%s) ]" ), FLTmToString( o_kOneSaleInfo.tmExpiredDate, FLTm::E_FORMAT::YYYYMMDD_HHMMSS ) );
	}

	__time64_t tCurr				= 0;
	const FLTm tmCurr				= DATE_TIMER().GetCurrentTime();
	if( FLDateTimeToTimeT( tCurr, tmCurr ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID TM(%s) ]" ), FLTmToString( tmCurr, FLTm::E_FORMAT::YYYYMMDD_HHMMSS ) );
	}

	//const FLTm tmCurr				= DATE_TIMER().GetCurrentTime();
	//const UINT64 u64ExpiredSec		= FLTimeToSecond( o_kOneSaleInfo.tmExpiredDate.year, o_kOneSaleInfo.tmExpiredDate.hour, o_kOneSaleInfo.tmExpiredDate.minute, o_kOneSaleInfo.tmExpiredDate.second );
	//const UINT64 u64CurrSec			= FLTimeToSecond( tmCurr.year, tmCurr.hour, tmCurr.minute, tmCurr.second );
	o_kOneSaleInfo.dwRemainSec		= ( tExpired > tCurr ) ? ( DWORD )( tExpired - tCurr ) : 0;
	FLStrcpy( o_kOneSaleInfo.szItemName, _countof( o_kOneSaleInfo.szItemName ), kSaleInfo.szItemName );
}



bool FLDBConsignmentSale::_AddReserve( const UINT64 u64SaleInfoSRL )
{
	CMclAutoLock Lock( m_kGlobalCS );
	return m_mapUsginID.insert( std::map< UINT64, UINT64 >::value_type( u64SaleInfoSRL, u64SaleInfoSRL ) ).second;
}

void FLDBConsignmentSale::_RemoveReserve( const UINT64 u64SaleInfoSRL )
{
	CMclAutoLock Lock( m_kGlobalCS );
	m_mapUsginID.erase( u64SaleInfoSRL );
}


void FLDBConsignmentSale::sumUserRegistCount( const u_long uPlayerID, const E_REGIST_MODE eMode, const int nSum )
{
	if( __REGIST_MODE_MAX__ <= eMode || eMode < 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, "[ INVALID REGIST MODE(%d) ]", eMode );
		return;
	}

	std::map< u_long, FLRegistCount >::iterator it = m_mapUserRegistCount.find( uPlayerID );
	if( it == m_mapUserRegistCount.end() )
	{
		std::pair< std::map< u_long, FLRegistCount >::iterator, bool> ret = m_mapUserRegistCount.insert(std::make_pair( uPlayerID, FLRegistCount() ) );
		if( ret.second == false )
			return;

		std::map< u_long, FLRegistCount >::iterator it_ret = ret.first;
		FLRegistCount & kRegistCount = it_ret->second;

		kRegistCount.arrCount[ eMode ] += nSum;
		return;
	}

	FLRegistCount & kRegistCount = it->second;

	kRegistCount.arrCount[ eMode ] += nSum;
}

bool FLDBConsignmentSale::addItem( const FLSaleInfo & kSaleInfo )
{
	if( m_mapSaleInfo.insert( /*std::map< UINT64, FLSaleInfo >::value_type*/
		std::make_pair( kSaleInfo.u64ItemInstanceSRL, kSaleInfo ) ).second == false )
		return false;

	m_mapSorted_ItemName.insert(std::make_pair( FLStringKey( kSaleInfo.u64ItemInstanceSRL, kSaleInfo.szItemName ), kSaleInfo.u64ItemInstanceSRL ) );
	m_mapSorted_LimitLevel.insert(std::make_pair( FLLimitLevelKey( kSaleInfo.u64ItemInstanceSRL, kSaleInfo.nLimitLevel, kSaleInfo.szItemName ), kSaleInfo.u64ItemInstanceSRL ) );
	m_mapSorted_Price.insert(std::make_pair( FLIntKey( kSaleInfo.u64ItemInstanceSRL, kSaleInfo.nPrice ), kSaleInfo.u64ItemInstanceSRL ) );
	m_mapSorted_OnePrice.insert(std::make_pair( FLOnePriceKey( kSaleInfo.u64ItemInstanceSRL, kSaleInfo.nPrice, kSaleInfo.kItemElem.m_nItemNum ), kSaleInfo.u64ItemInstanceSRL ) );
	m_mapSorted_AbilityOption.insert(std::make_pair( FLIntKey( kSaleInfo.u64ItemInstanceSRL, kSaleInfo.nAbilityOption ), kSaleInfo.u64ItemInstanceSRL ) );
	
	return true;
}

bool FLDBConsignmentSale::popItem( const UINT64 u64SaleInfoSRL, FLSaleInfo & o_kSaleInfo )
{
	std::map< UINT64, FLSaleInfo >::iterator it = m_mapSaleInfo.find( u64SaleInfoSRL );
	if( it == m_mapSaleInfo.end() )
		return false;

	o_kSaleInfo				= it->second;

	//m_mapSaleInfo.erase( u64SaleInfoSRL );
	m_mapSaleInfo.erase( it );

	m_mapSorted_ItemName.erase( FLStringKey( o_kSaleInfo.u64ItemInstanceSRL, o_kSaleInfo.szItemName ) );
	m_mapSorted_LimitLevel.erase( FLLimitLevelKey( o_kSaleInfo.u64ItemInstanceSRL, o_kSaleInfo.nLimitLevel, o_kSaleInfo.szItemName ) );
	m_mapSorted_Price.erase( FLIntKey( o_kSaleInfo.u64ItemInstanceSRL, o_kSaleInfo.nPrice ) );
	m_mapSorted_OnePrice.erase( FLOnePriceKey( o_kSaleInfo.u64ItemInstanceSRL, o_kSaleInfo.nPrice, o_kSaleInfo.kItemElem.m_nItemNum  ) );
	m_mapSorted_AbilityOption.erase( FLIntKey( o_kSaleInfo.u64ItemInstanceSRL, o_kSaleInfo.nAbilityOption ) );
	return true;
}

bool FLDBConsignmentSale::_AddExpiredData( const FLSaleInfo & kSaleInfo )
{
	CMclAutoLock Lock( m_kGlobalCS );
	if( m_mapExpiredSaleInfo.insert(std::make_pair( kSaleInfo.u64ItemInstanceSRL, kSaleInfo ) ).second == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T("[ DUPLICATE ID (%u64) ]"), kSaleInfo.u64ItemInstanceSRL );
		return false;
	}
	sumUserRegistCount( kSaleInfo.uPlayerID, kSaleInfo.eRegistMode, +1 );
	return true;
}


bool FLDBConsignmentSale::addExpiredData( const FLSaleInfo & kSaleInfo )
{
	return m_mapExpiredSaleInfo.insert(std::make_pair( kSaleInfo.u64ItemInstanceSRL, kSaleInfo ) ).second;
}

bool FLDBConsignmentSale::popExpiredData( const UINT64 u64SaleInfoSRL, FLSaleInfo & o_kSaleInfo )
{
	std::map< UINT64, FLSaleInfo >::iterator it = m_mapExpiredSaleInfo.find( u64SaleInfoSRL );
	if( it == m_mapExpiredSaleInfo.end() )
		return false;

	o_kSaleInfo				= it->second;

	m_mapExpiredSaleInfo.erase( it );
	return true;
}






//--------------------------------------------------------------------------------------------------------------------------------------------//

FLDBConsignmentSale::FLSaleInfo::FLSaleInfo()
: u64ItemInstanceSRL( NULL_CONSIGNMENT_SALE_SRL )
, uPlayerID( NULL_PLAYER_ID )
, nPrice( 0 )
, nOnePrice( 0 )
, dwItemType1( TYPE1_NONE )
, dwItemType2( TYPE2_NONE )
, dwGrade( ITEM_GRADE_NORMAL )
, nLimitLevel( 0 )
, nAbilityOption( 0 )
, eRegistMode( DEFAULT_REGIST_MODE )
{
	szItemName[ 0 ]			= '\0';
	szPlayerName[ 0 ]		= '\0';
}

FLDBConsignmentSale::FLSaleInfo::FLSaleInfo( const FLSaleInfo & r )
{
	*this = r;
}


FLDBConsignmentSale::FLSaleInfo::~FLSaleInfo()
{

}


FLDBConsignmentSale::FLSaleInfo & FLDBConsignmentSale::FLSaleInfo::operator=( const FLSaleInfo & r )
{
	u64ItemInstanceSRL		= r.u64ItemInstanceSRL;
	uPlayerID				= r.uPlayerID;

	nPrice					= r.nPrice;
	nOnePrice				= r.nOnePrice;
	FLStrcpy( szItemName, _countof( szItemName ), r.szItemName );
	FLStrcpy( szPlayerName, _countof( szPlayerName ), r.szPlayerName );
	dwItemType1				= r.dwItemType1;
	dwItemType2				= r.dwItemType2;
	dwGrade					= r.dwGrade;
	nLimitLevel				= r.nLimitLevel;
	nAbilityOption			= r.nAbilityOption;
	eRegistMode				= r.eRegistMode;
	tmExpiredDate			= r.tmExpiredDate;

	kItemElem				= r.kItemElem;
	return *this;
}

