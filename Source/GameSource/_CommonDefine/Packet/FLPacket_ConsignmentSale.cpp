
#include "StdAfx.h"
#include "FLPacket_ConsignmentSale.h"

namespace nsConsignmentSale {

//------------------------------------------------------------------------------------------------------------------//

FLPacket_UserInfoReq::FLPacket_UserInfoReq() : FLPacket( PACKETTYPE_CONSIGNMENT_SALE_USER_INFO_REQ )
{

}

bool FLPacket_UserInfoReq::BodySerialize( CAr & /*kAr*/ ) const
{
	__noop;
	return true;
}

bool FLPacket_UserInfoReq::BodyDeserialize( CAr & /*kAr*/ )
{
	__noop;
	return true;
}

//------------------------------------------------------------------------------------------------------------------//

FLPacket_UserInfoAck::FLPacket_UserInfoAck( const int nPacketID ) 
: FLPacket( nPacketID )
//, m_dwRemainRegistCount( 0 )
,m_dwDefaultRegistCount( 0 )
,m_dwExtendRegistCount( 0 )
,m_dwDefaultRegistCountMax( 0 )
,m_dwExtendRegistCountMax( 0 )
{

}

bool FLPacket_UserInfoAck::BodySerialize( CAr & kAr ) const
{
	//kAr << m_dwRemainRegistCount;
	kAr << m_dwDefaultRegistCount << m_dwExtendRegistCount << m_dwDefaultRegistCountMax << m_dwExtendRegistCountMax;
	return true;
}

bool FLPacket_UserInfoAck::BodyDeserialize( CAr & kAr )
{
	//kAr >> m_dwRemainRegistCount;
	kAr >> m_dwDefaultRegistCount >> m_dwExtendRegistCount >> m_dwDefaultRegistCountMax >> m_dwExtendRegistCountMax;
	return true;
}	


//------------------------------------------------------------------------------------------------------------------//

FLPacket_RegistReq::FLPacket_RegistReq()
: FLPacket( PACKETTYPE_CONSIGNMENT_SALE_REGIST_REQ )
, m_dwItemObjID( NULL_ID )//NULL_INVEN_INDEX )
, m_nItemCost( 0 )
, m_nItemCount( 0 )
{
}

bool FLPacket_RegistReq::BodySerialize( CAr & kAr ) const
{
	kAr << m_dwItemObjID << m_nItemCost << m_nItemCount;
	return true;
}

bool FLPacket_RegistReq::BodyDeserialize( CAr & kAr )
{
	kAr >> m_dwItemObjID >> m_nItemCost >> m_nItemCount;
	return true;
}


//------------------------------------------------------------------------------------------------------------------//

FLPacket_RegistAck::FLPacket_RegistAck( const int nPacketID )
: FLPacket( nPacketID )
, m_eResult( E_FAILURE )
//, m_dwRemainRegistCount( 0 )
//,m_dwDefaultRegistCount( 0 )
//,m_dwExtendRegistCount( 0 )
//,m_dwDefaultRegistCountMax( 0 )
//,m_dwExtendRegistCountMax( 0 )
{

}

bool FLPacket_RegistAck::BodySerialize( CAr & kAr ) const
{
	//kAr << m_eResult << m_dwRemainRegistCount;

	kAr << m_eResult; // <<	m_dwDefaultRegistCount << m_dwExtendRegistCount << m_dwDefaultRegistCountMax << m_dwExtendRegistCountMax;
	return true;
}

bool FLPacket_RegistAck::BodyDeserialize( CAr & kAr )
{
	//kAr >> m_eResult >> m_dwRemainRegistCount;
	kAr >> m_eResult; // >>	m_dwDefaultRegistCount >> m_dwExtendRegistCount >> m_dwDefaultRegistCountMax >> m_dwExtendRegistCountMax;
	return true;
}

//------------------------------------------------------------------------------------------------------------------//

FLPacket_RegistedInfoListReq::FLPacket_RegistedInfoListReq() 
: FLPacket( PACKETTYPE_CONSIGNMENT_SALE_REGISTED_INFO_LIST_REQ )
{

}

bool FLPacket_RegistedInfoListReq::BodySerialize( CAr & /*kAr*/ ) const
{
	__noop;
	return true;
}

bool FLPacket_RegistedInfoListReq::BodyDeserialize( CAr & /*kAr*/ )
{
	__noop;
	return true;
}



//------------------------------------------------------------------------------------------------------------------//

FLPacket_RegistedInfoListAck::FLPacket_RegistedInfoListAck( const int nPacketID /*= PACKETTYPE_CONSIGNMENT_SALE_REGISTED_INFO_LIST_ACK */ ) 
: FLPacket( nPacketID ), m_dwCount( 0 )
{

}


FLPacket_RegistedInfoListAck::~FLPacket_RegistedInfoListAck()
{
	//for( SaleInfoVec::iterator it = m_vecSaleInfoList.begin(); it != m_vecSaleInfoList.end(); ++it )
	//{
	//	FLOneSaleInfo * pOneRegistedInfo	= *it;
	//	SAFE_DELETE( pOneRegistedInfo );
	//}
}

bool FLPacket_RegistedInfoListAck::BodySerialize( CAr & kAr ) const
{
	m_dwCount				= m_vecSaleInfoList.size();
	kAr << m_dwCount;

	for( SaleInfoVec::iterator it = m_vecSaleInfoList.begin(); it != m_vecSaleInfoList.end(); ++it )
	{
		FLOneSaleInfo & kOneRegistedInfo	= *it;
		kOneRegistedInfo.Serialize( kAr );
	}
	return true;
}

bool FLPacket_RegistedInfoListAck::BodyDeserialize( CAr & kAr )
{
	kAr >> m_dwCount;

	//for( SaleInfoVec::iterator it = m_vecSaleInfoList.begin(); it != m_vecSaleInfoList.end(); ++it )
	for( DWORD i = 0; i < m_dwCount; ++i )
	{
		FLOneSaleInfo kOneRegistedInfo;
		kOneRegistedInfo.Serialize( kAr );

		m_vecSaleInfoList.push_back( kOneRegistedInfo );
	}
	return true;
}



//------------------------------------------------------------------------------------------------------------------//


FLPacket_CancelRegistedReq::FLPacket_CancelRegistedReq()
: FLPacket( PACKETTYPE_CONSIGNMENT_SALE_CANCEL_REGISTED_REQ )
, m_u64SaleSRL( NULL_CONSIGNMENT_SALE_SRL )
{

}

bool FLPacket_CancelRegistedReq::BodySerialize( CAr & kAr ) const
{
	kAr << m_u64SaleSRL;
	return true;
}

bool FLPacket_CancelRegistedReq::BodyDeserialize( CAr & kAr )
{
	kAr >> m_u64SaleSRL;
	return true;
}

//------------------------------------------------------------------------------------------------------------------//

FLPacket_CancelRegistedAck::FLPacket_CancelRegistedAck( const int nPacketID )
: FLPacket( nPacketID )
, m_eResult( E_FAILURE )
//, m_eRegistMode( DEFAULT_REGIST_MODE )
, m_u64SaleSRL( NULL_CONSIGNMENT_SALE_SRL )
{

}

bool FLPacket_CancelRegistedAck::BodySerialize( CAr & kAr ) const
{
	kAr << m_eResult /*<< m_eRegistMode*/ << m_u64SaleSRL;
	return true;
}

bool FLPacket_CancelRegistedAck::BodyDeserialize( CAr & kAr )
{
	kAr >> m_eResult /*>> m_eRegistMode*/ >> m_u64SaleSRL;
	return true;
}

//------------------------------------------------------------------------------------------------------------------//

FLPacket_BuyItemReq::FLPacket_BuyItemReq()
: FLPacket( PACKETTYPE_CONSIGNMENT_SALE_BUY_ITEM_REQ )
,m_u64SaleSRL( NULL_CONSIGNMENT_SALE_SRL )
{

}

bool FLPacket_BuyItemReq::BodySerialize( CAr & kAr ) const
{
	kAr << m_u64SaleSRL;
	return true;
}

bool FLPacket_BuyItemReq::BodyDeserialize( CAr & kAr )
{
	kAr >> m_u64SaleSRL;
	return true;
}

//------------------------------------------------------------------------------------------------------------------//

FLPacket_BuyItemAck::FLPacket_BuyItemAck( const int nPacketID )
: FLPacket( nPacketID )
, m_eResult( E_FAILURE )
, m_u64SaleSRL( NULL_CONSIGNMENT_SALE_SRL )
//, m_nRollbackGold( 0 )
, m_dwItemID( NULL_ITEM_ID )
{

}

bool FLPacket_BuyItemAck::BodySerialize( CAr & kAr ) const
{
	kAr << m_eResult << m_u64SaleSRL << m_dwItemID/* << m_nRollbackGold*/;
	return true;
}

bool FLPacket_BuyItemAck::BodyDeserialize( CAr & kAr )
{
	kAr >> m_eResult >> m_u64SaleSRL >> m_dwItemID/* >> m_nRollbackGold*/;
	return true;
}

//------------------------------------------------------------------------------------------------------------------//

FLPacket_SellItemNoti::FLPacket_SellItemNoti( const int nPacketID )
: FLPacket( nPacketID )
//, m_uBuyerID( NULL_PLAYER_ID )
, m_dwItemID( NULL_ITEM_ID )
, m_nGold( 0 )
, m_eRegistMode( DEFAULT_REGIST_MODE )
{
	m_szPlayerName[ 0 ]		= '\0';
}

bool FLPacket_SellItemNoti::BodySerialize( CAr & kAr ) const
{
	kAr.WriteString( m_szPlayerName );
	kAr << m_dwItemID << m_nGold << m_eRegistMode;
	return true;
}

bool FLPacket_SellItemNoti::BodyDeserialize( CAr & kAr )
{
	kAr.ReadString( m_szPlayerName, _countof( m_szPlayerName ) );
	kAr >> m_dwItemID >> m_nGold >> m_eRegistMode;
	return true;
}


//------------------------------------------------------------------------------------------------------------------//

FLPacket_CollectSaleGoldReq::FLPacket_CollectSaleGoldReq()
: FLPacket( PACKETTYPE_CONSIGNMENT_SALE_COLLECT_SALE_GOLD_REQ )
, m_u64SaleGoldSRL( NULL_SALE_GOLD_SRL )
{

}

bool FLPacket_CollectSaleGoldReq::BodySerialize( CAr & kAr ) const
{
	kAr << m_u64SaleGoldSRL;
	return true;
}

bool FLPacket_CollectSaleGoldReq::BodyDeserialize( CAr & kAr )
{
	kAr >> m_u64SaleGoldSRL;
	return true;
}

//------------------------------------------------------------------------------------------------------------------//

FLPacket_CollectSaleGoldAck::FLPacket_CollectSaleGoldAck( const int nPacketID )
: FLPacket( nPacketID )
, m_eResult( E_FAILURE )
, m_u64SaleGoldSRL( NULL_SALE_GOLD_SRL )
, m_nGold( 0 )
{

}

bool FLPacket_CollectSaleGoldAck::BodySerialize( CAr & kAr ) const
{
	kAr << m_eResult << m_u64SaleGoldSRL << m_nGold;
	return true;
}

bool FLPacket_CollectSaleGoldAck::BodyDeserialize( CAr & kAr )
{
	kAr >> m_eResult >> m_u64SaleGoldSRL >> m_nGold;
	return true;
}

//------------------------------------------------------------------------------------------------------------------//

FLPacket_SearchReq::FLPacket_SearchReq()
: FLPacket( PACKETTYPE_CONSIGNMENT_SALE_SEARCH_REQ )
, m_dwPage( 0 )
, m_eOrderingOption( E_NONE )
, m_bASC( FALSE )

, m_nLimitLevelMin( 0 )
, m_nLimitLevelMax( 0 )

, m_nAbilityOptionMin( 0 )
, m_nAbilityOptionMax( 0 )
, m_dwItemType1( TYPE1_NONE ) 
, m_dwItemType2( TYPE2_NONE )

//, m_szSearchName[ _MAX_LOGSTR_ITEMNAME_LEN ];
, m_bSearchBySameName( FALSE )
{
	for( size_t i = 0; i < GRADE_MULTI_CHECK_MAX; ++i )
		m_arrGrade[ i ]		= ITEM_GRADE_NONE;

	m_szSearchName[ 0 ]		= '\0';
}

bool FLPacket_SearchReq::BodySerialize( CAr & kAr ) const
{
	kAr << m_eOrderingOption << m_bASC << m_dwPage;
	for( size_t i = 0; i < GRADE_MULTI_CHECK_MAX; ++i )
		kAr << m_arrGrade[ i ];
		//<< m_dwGrade1 << m_dwGrade2 << m_dwGrade3 << m_dwGrade4

	kAr	<< m_nLimitLevelMin << m_nLimitLevelMax << m_nAbilityOptionMin << m_nAbilityOptionMax << m_dwItemType1 << m_dwItemType2;
	kAr.WriteString( m_szSearchName );
	kAr << m_bSearchBySameName;
	return true;
}

bool FLPacket_SearchReq::BodyDeserialize( CAr & kAr )
{
	kAr >> m_eOrderingOption >> m_bASC >> m_dwPage; 
		//>> m_dwGrade1 >> m_dwGrade2 >> m_dwGrade3 >> m_dwGrade4
	for( size_t i = 0; i < GRADE_MULTI_CHECK_MAX; ++i )
		kAr >> m_arrGrade[ i ];
	kAr >> m_nLimitLevelMin >> m_nLimitLevelMax >> m_nAbilityOptionMin >> m_nAbilityOptionMax >> m_dwItemType1 >> m_dwItemType2;
	kAr.ReadString( m_szSearchName, _countof( m_szSearchName ) );
	kAr >> m_bSearchBySameName;
	return true;
}

const char * FLPacket_SearchReq::EnumToString( const E_OEDERING_OPTION eOption )
{
	switch ( eOption )
	{
	case E_ITEM_NAME:			return "szItemName";
	case E_ITEM_LEVEL:			return "dwLimitLevel";
	case E_PRICE:				return "nPrice";
	case E_ONE_PRICE:			return "nOnePrice";
	case E_ABILITY_OPTION:		return "nAbilityOption";
	}

	return "";
}

//------------------------------------------------------------------------------------------------------------------//

FLPacket_SearchAck::FLPacket_SearchAck( const int nPacketID )
: FLPacket( nPacketID )
, m_dwCount( 0 )
, m_dwCountMax( 0 )
, m_dwPage( 0 )
{
	
}

FLPacket_SearchAck::~FLPacket_SearchAck()
{
}


bool FLPacket_SearchAck::BodySerialize( CAr & kAr ) const
{
	m_dwCount				= m_vecSaleInfoList.size();
	kAr << m_dwCount << m_dwCountMax << m_dwPage /*<< m_dwPageMax*/;
	
	for( SaleInfoVec::iterator it = m_vecSaleInfoList.begin(); it != m_vecSaleInfoList.end(); ++it )
	{
		FLOneSaleInfo & kOneSaleInfo	= *it;
		kOneSaleInfo.Serialize( kAr );
	}
	return true;
}

bool FLPacket_SearchAck::BodyDeserialize( CAr & kAr )
{
	kAr >> m_dwCount >> m_dwCountMax >> m_dwPage /*>> m_dwPageMax*/;

	//for( SaleInfoVec::iterator it = m_vecSaleInfoList.begin(); it != m_vecSaleInfoList.end(); ++it )
	for( DWORD i = 0; i < m_dwCount; ++i )
	{
		FLOneSaleInfo kOneSaleInfo;
		kOneSaleInfo.Serialize( kAr );

		m_vecSaleInfoList.push_back( kOneSaleInfo );
	}
	return true;
}


//------------------------------------------------------------------------------------------------------------------//

FLPacket_SaleGoldInfoListReq::FLPacket_SaleGoldInfoListReq()
: FLPacket( PACKETTYPE_CONSIGNMENT_SALE_COLLECT_SALE_GOLD_INFO_LIST_REQ )
{

}

bool FLPacket_SaleGoldInfoListReq::BodySerialize( CAr & /*kAr*/ ) const
{
	__noop;
	return true;
}

bool FLPacket_SaleGoldInfoListReq::BodyDeserialize( CAr & /*kAr*/ )
{
	__noop;
	return true;
}

//------------------------------------------------------------------------------------------------------------------//

FLPacket_SaleGoldInfoListAck::FLPacket_SaleGoldInfoListAck( const int nPacketID )
: FLPacket( nPacketID )
, m_dwCount( 0 )
{

}


FLPacket_SaleGoldInfoListAck::FLSaleGoldInfo::FLSaleGoldInfo() 
: u64SaleGoldSRL( NULL_SALE_GOLD_SRL )//, dwItemID( NULL_ITEM_ID))
, nGold( 0 )
{
	szItemName[ 0 ]			= '\0';
}

FLPacket_SaleGoldInfoListAck::FLSaleGoldInfo::FLSaleGoldInfo( const FLSaleGoldInfo & r )
{
	*this = r;
}

void FLPacket_SaleGoldInfoListAck::FLSaleGoldInfo::Serialize( CAr & kAr )
{
	if( kAr.IsStoring() == TRUE )
	{
		kAr << u64SaleGoldSRL << nGold << kSaleDate;
		kItemElem.Serialize( kAr );
	}
	else
	{
		kAr >> u64SaleGoldSRL >> nGold >> kSaleDate;
		kItemElem.Deserialize( kAr );
	}
}

FLPacket_SaleGoldInfoListAck::FLSaleGoldInfo & 
FLPacket_SaleGoldInfoListAck::FLSaleGoldInfo::operator=( const FLSaleGoldInfo & r )
{
	FLStrcpy( szItemName, _countof( szItemName ), r.szItemName );
	u64SaleGoldSRL	= r.u64SaleGoldSRL;
	nGold			= r.nGold;
	kSaleDate		= r.kSaleDate;
	kItemElem		= r.kItemElem;

	return *this;
}


bool FLPacket_SaleGoldInfoListAck::BodySerialize( CAr & kAr ) const
{
    m_dwCount				= m_vecSaleGoldInfo.size();
	kAr << m_dwCount;//m_vecSaleGoldInfo.size();

	for( SaleGoldInfoVec::iterator it = m_vecSaleGoldInfo.begin()
		; it != m_vecSaleGoldInfo.end()
		; ++it
		)
	{
		FLSaleGoldInfo & kSaleGold = *it;
		kSaleGold.Serialize( kAr );
	}

	return true;
}

bool FLPacket_SaleGoldInfoListAck::BodyDeserialize( CAr & kAr )
{
	kAr >> m_dwCount;
	for( size_t i = 0; i < m_dwCount; ++i )
	{
		FLSaleGoldInfo kSaleGold;
		kSaleGold.Serialize( kAr );
		m_vecSaleGoldInfo.push_back( kSaleGold );
	}

	return true;
}


FLPacket_CalcTaxReq::FLPacket_CalcTaxReq( const int nPacketID /*= PACKETTYPE_CONSIGNMENT_SALE_CALC_TAX_REQ */ ) 
: FLPacket( nPacketID )
, m_nPrice( 0 )
{
}

bool FLPacket_CalcTaxReq::BodySerialize( CAr & kAr ) const
{
	kAr << m_nPrice;
	return true;
}

bool FLPacket_CalcTaxReq::BodyDeserialize( CAr & kAr )
{
	kAr >> m_nPrice;
	return true;
}

FLSnapshot_CalcTaxAck::FLSnapshot_CalcTaxAck( const int nPacketID /*= SNAPSHOTTYPE_CONSIGNMENT_SALE_CALC_TAX_ACK */ ) 
: FLPacket( nPacketID )
, m_nTax( 0 )
{
}

bool FLSnapshot_CalcTaxAck::BodySerialize( CAr & kAr ) const
{
	kAr << m_nTax;
	return true;
}

bool FLSnapshot_CalcTaxAck::BodyDeserialize( CAr & kAr )
{
	kAr >> m_nTax;
	return true;
}






bool FLOneSaleInfo::Serialize( CAr & o_kAr )
{
	if( o_kAr.IsStoring() == TRUE )
	{
		o_kAr << u64SaleSRL << nItemCostTot;
		kItemElem.Serialize( o_kAr );
		o_kAr.WriteString( szPlayerName );
		o_kAr << tmExpiredDate;
		o_kAr << dwRemainSec;
	}
	else
	{
		o_kAr >> u64SaleSRL >> nItemCostTot;
		kItemElem.Deserialize( o_kAr );
		o_kAr.ReadString( szPlayerName, _countof( szPlayerName ) );
		o_kAr >> tmExpiredDate;
		o_kAr >> dwRemainSec;
	}
	return true;
}

//----------------------------------------------------------------------------------------//

FLOneSaleInfo::FLOneSaleInfo( const FLOneSaleInfo & r )
{
	*this	= r;
}

FLOneSaleInfo & FLOneSaleInfo::operator=( const FLOneSaleInfo & r )
{
	u64SaleSRL			= r.u64SaleSRL;
	nItemCostTot		= r.nItemCostTot;
	FLStrcpy( szItemName, _countof( szItemName ), r.szItemName );
	FLStrcpy( szPlayerName, _countof( szPlayerName ), r.szPlayerName );
	tmExpiredDate		= r.tmExpiredDate;

	kItemElem			= r.kItemElem;
	dwRemainSec			= r.dwRemainSec;
	return *this;
}


//
//bool FLOneSaleInfo::Serialize( CAr & o_kAr )
//{
//	if( o_kAr.IsStoring() == TRUE )
//	{
//		kOneSaleInfo.Serialize( o_kAr );
//		o_kAr << dwRemainSec;
//	}
//	else
//	{
//		kOneSaleInfo.Serialize( o_kAr );
//		o_kAr >> dwRemainSec;
//	}
//	return true;
//}
//
//FLOneSaleInfo & FLOneSaleInfo::operator=( const FLOneSaleInfo & r )
//{
//	kOneSaleInfo		= r.kOneSaleInfo;
//	
//
//	return *this;
//}
//
//




} /* nsConsignmentSale */