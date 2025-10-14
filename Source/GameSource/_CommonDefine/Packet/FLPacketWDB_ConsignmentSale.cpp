
#include "StdAfx.h"
#include "FLPacketWDB_ConsignmentSale.h"

namespace nsConsignmentSale {


FLPacketWDB_CheckRegistedCountReq::FLPacketWDB_CheckRegistedCountReq( const int nPacketID /*= PACKETTYPE_CONSIGNMENT_SALE_CHECK_REGISTED_COUNT_REQ */ ) 
:  FLPacket( nPacketID )
, m_uPlayerID( NULL_PLAYER_ID )
//, m_dwDefaultRegistCountMax( 0 )
//, m_dwItemRegistCountMax( 0 )

, m_dwItemObjID( NULL_ID )
, m_nItemCost( 0 )
, m_nItemCount( 0 )
{

}

bool FLPacketWDB_CheckRegistedCountReq::BodySerialize( CAr & kAr ) const
{
	kAr << m_uPlayerID << m_dwItemObjID << m_nItemCost << m_nItemCount;
	return true;
}

bool FLPacketWDB_CheckRegistedCountReq::BodyDeserialize( CAr & kAr )
{
	kAr >> m_uPlayerID >> m_dwItemObjID >> m_nItemCost >> m_nItemCount;
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------//

FLPacketWDB_CheckRegistedCountAck::FLPacketWDB_CheckRegistedCountAck( const int nPacketID /*= PACKETTYPE_CONSIGNMENT_SALE_CHECK_REGISTED_COUNT_ACK */ ) 
:  FLPacket( nPacketID )
, m_uPlayerID( NULL_PLAYER_ID )
, m_dwDefaultRegistCount( 0 )
, m_dwItemRegistCount( 0 )
, m_dwItemObjID( NULL_ID )
, m_nItemCost( 0 )
, m_nItemCount( 0 )
{

}

bool FLPacketWDB_CheckRegistedCountAck::BodySerialize( CAr & kAr ) const
{
	kAr << m_uPlayerID << m_dwDefaultRegistCount << m_dwItemRegistCount << m_dwItemObjID << m_nItemCost << m_nItemCount;
	return true;
}

bool FLPacketWDB_CheckRegistedCountAck::BodyDeserialize( CAr & kAr )
{
	kAr >> m_uPlayerID >> m_dwDefaultRegistCount >> m_dwItemRegistCount >> m_dwItemObjID >> m_nItemCost >> m_nItemCount;
	return true;
}


//----------------------------------------------------------------------------------------------------------------------------------------//

FLPacketWDB_GetRegistedCountReq::FLPacketWDB_GetRegistedCountReq( const int nPacketID /*= PACKETTYPE_CONSIGNMENT_SALE_GET_REGISTED_COUNT_REQ */ ) 
: FLPacket( nPacketID )
, m_uPlayerID( NULL_PLAYER_ID )
{

}

bool FLPacketWDB_GetRegistedCountReq::BodySerialize( CAr & kAr ) const
{
	kAr << m_uPlayerID;
	return true;
}

bool FLPacketWDB_GetRegistedCountReq::BodyDeserialize( CAr & kAr )
{
	kAr >> m_uPlayerID;
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------//

FLPacketWDB_GetRegistedCountAck::FLPacketWDB_GetRegistedCountAck( const int nPacketID /*= PACKETTYPE_CONSIGNMENT_SALE_GET_REGISTED_COUNT_ACK */ ) 
: FLPacket( nPacketID )
, m_uPlayerID( NULL_PLAYER_ID )
, m_dwDefaultRegistModeCount( 0 )
, m_dwItemRegistModeCount( 0 )
//, m_dwRegistedCount( 0 )
{

}

bool FLPacketWDB_GetRegistedCountAck::BodySerialize( CAr & kAr ) const
{
//	kAr << m_uPlayerID << m_dwRegistedCount;
	kAr << m_uPlayerID << m_dwDefaultRegistModeCount << m_dwItemRegistModeCount;
	return true;
}

bool FLPacketWDB_GetRegistedCountAck::BodyDeserialize( CAr & kAr )
{
//	kAr >> m_uPlayerID >> m_dwRegistedCount;
	kAr >> m_uPlayerID >> m_dwDefaultRegistModeCount >> m_dwItemRegistModeCount;
	return true;
}


//--------------------------------------------------------------------------------------------------------------------------------------------//


FLPacketWDB_GetPriceReq::FLPacketWDB_GetPriceReq( const int nPacketID /*= PACKETTYPE_CONSIGNMENT_SALE_GET_PRICE_REQ */ ) 
: FLPacket( nPacketID )
, m_uPlayerID( NULL_PLAYER_ID )
, m_u64SaleSRL( NULL_CONSIGNMENT_SALE_SRL )
{

}

bool FLPacketWDB_GetPriceReq::BodySerialize( CAr & kAr ) const
{
	kAr << m_uPlayerID << m_u64SaleSRL;
	return true;
}

bool FLPacketWDB_GetPriceReq::BodyDeserialize( CAr & kAr )
{
	kAr >> m_uPlayerID >> m_u64SaleSRL;
	return true;
}

//--------------------------------------------------------------------------------------------------------//


FLPacketWDB_ClearReservedInfoNoti::FLPacketWDB_ClearReservedInfoNoti( const int nPacketID /*= PACKETTYPE_CONSIGNMENT_SALE_CLEAR_RESERVE_INFO */ ) 
: FLPacket( nPacketID )
, m_u64SaleSRL( NULL_CONSIGNMENT_SALE_SRL )
{
}

bool FLPacketWDB_ClearReservedInfoNoti::BodySerialize( CAr & kAr ) const
{
	kAr << m_u64SaleSRL;
	return true;
}

bool FLPacketWDB_ClearReservedInfoNoti::BodyDeserialize( CAr & kAr )
{
	kAr >> m_u64SaleSRL;
	return true;
}

//--------------------------------------------------------------------------------------------------------//

FLPacketWDB_GetPriceAck::FLPacketWDB_GetPriceAck( const int nPacketID /*= PACKETTYPE_CONSIGNMENT_SALE_GET_PRICE_ACK */ ) 
: FLPacket( nPacketID )
, m_eResult( E_FAILURE )
, m_uPlayerID( NULL_PLAYER_ID )
, m_u64SaleSRL( NULL_CONSIGNMENT_SALE_SRL )
, m_uSalerID( NULL_PLAYER_ID )
, m_nPrice( 0 )

//, m_uSellerPlayerID( NULL_PLAYER_ID )
{

}

bool FLPacketWDB_GetPriceAck::BodySerialize( CAr & kAr ) const
{
	kAr << m_uPlayerID << m_u64SaleSRL << m_uSalerID << m_nPrice << m_eResult; // << m_uSellerPlayerID ;
	return true;
}

bool FLPacketWDB_GetPriceAck::BodyDeserialize( CAr & kAr )
{
	kAr >> m_uPlayerID >> m_u64SaleSRL >> m_uSalerID >> m_nPrice >> m_eResult; // >> m_uSellerPlayerID;
	return true;
}
//--------------------------------------------------------------------------------------------------------//


FLPacketWDB_BuyItemReq::FLPacketWDB_BuyItemReq()
: FLPacket( PACKETTYPE_CONSIGNMENT_SALE_BUY_ITEM_REQ )
, m_uPlayerID( NULL_PLAYER_ID )
, m_u64SaleSRL( NULL_CONSIGNMENT_SALE_SRL )
, m_uSalerID( NULL_PLAYER_ID )
, m_nPrice( 0 )

, m_dwWorldID( WI_WORLD_NONE )
{
	m_szPlayerName[ 0 ]		= '\0';
}

bool FLPacketWDB_BuyItemReq::BodySerialize( CAr & kAr ) const
{
	kAr << m_uPlayerID << m_u64SaleSRL << m_uSalerID << m_nPrice;
	kAr.WriteString( m_szPlayerName );
	kAr << m_dwWorldID;
	return true;
}

bool FLPacketWDB_BuyItemReq::BodyDeserialize( CAr & kAr )
{
	kAr >> m_uPlayerID >> m_u64SaleSRL >> m_uSalerID >> m_nPrice;
	kAr.ReadString( m_szPlayerName, _countof( m_szPlayerName ) );
	kAr >> m_dwWorldID;
	return true;
}

//---------------------------------------------------------------------------------------------------------

FLPacketWDB_RegistReq::FLPacketWDB_RegistReq( const int nPacketID /*= PACKETTYPE_CONSIGNMENT_SALE_REGIST_REQ */ ) 
: FLPacket( nPacketID )
, m_uPlayerID( NULL_PLAYER_ID )
, m_nPrice( 0 )
, m_nOnePrice( 0 )
, m_nPay( 0 )
, m_dwItemType1( TYPE1_NONE )
, m_dwItemType2( TYPE2_NONE )
, m_dwGrade( ITEM_GRADE_NONE )
, m_nLimitLevel( 0 )
, m_uRemainHour( 0 )
, m_eRegistMode( DEFAULT_REGIST_MODE )

, m_dwWorldID( WI_WORLD_NONE )
//, m_nGold( 0 )
{
	m_szItemName[ 0 ]		= '\0';
	m_szPlayerName[ 0 ]		= '\0';
}

bool FLPacketWDB_RegistReq::BodySerialize( CAr & kAr ) const
{
	kAr << m_uPlayerID << m_nPrice << m_nOnePrice << m_nPay;
	kAr.WriteString( m_szItemName );
	kAr.WriteString( m_szPlayerName );
	kAr << m_dwItemType1 << m_dwItemType2 << m_dwGrade << m_nLimitLevel << m_uRemainHour;
	m_kItemElem.Serialize( kAr );
	kAr << m_eRegistMode;

	kAr << m_dwWorldID;// << m_nGold;
	return true;
}

bool FLPacketWDB_RegistReq::BodyDeserialize( CAr & kAr )
{
	kAr >> m_uPlayerID >> m_nPrice >> m_nOnePrice >> m_nPay;
	kAr.ReadString( m_szItemName, _countof( m_szItemName ) );
	kAr.ReadString( m_szPlayerName, _countof( m_szPlayerName ) );
	kAr >> m_dwItemType1 >> m_dwItemType2 >> m_dwGrade >> m_nLimitLevel >> m_uRemainHour;
	m_kItemElem.Deserialize( kAr );
	kAr >> m_eRegistMode;

	kAr >> m_dwWorldID;// >> m_nGold;
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------//

FLPacketWDB_RegistAck::FLPacketWDB_RegistAck( const int nPacketID /*= PACKETTYPE_CONSIGNMENT_SALE_REGIST_ACK */ ) 
: FLPacket( nPacketID )
, m_uPlayerID( NULL_PLAYER_ID )
, m_eResult( E_FAILURE )
{

}

bool FLPacketWDB_RegistAck::BodySerialize( CAr & kAr ) const
{
	kAr << m_uPlayerID << m_eResult;
	return true;
}

bool FLPacketWDB_RegistAck::BodyDeserialize( CAr & kAr )
{
	kAr >> m_uPlayerID >> m_eResult;
	return true;
}

//------------------------------------------------------------------------------//

FLPacketWDB_CancelRegistedReq::FLPacketWDB_CancelRegistedReq()
: FLPacket( PACKETTYPE_CONSIGNMENT_SALE_CANCEL_REGISTED_REQ )
, m_uPlayerID( NULL_PLAYER_ID )
, m_u64SaleSRL( NULL_CONSIGNMENT_SALE_SRL )
, m_dwWorldID( WI_WORLD_NONE )
{
	m_szPlayerName[ 0 ]	= '\0';
}

bool FLPacketWDB_CancelRegistedReq::BodySerialize( CAr & kAr ) const
{
	kAr << m_uPlayerID << m_u64SaleSRL;
	kAr.WriteString( m_szPlayerName );
	kAr << m_dwWorldID;
	return true;
}

bool FLPacketWDB_CancelRegistedReq::BodyDeserialize( CAr & kAr )
{
	kAr >> m_uPlayerID >> m_u64SaleSRL;
	kAr.ReadString( m_szPlayerName, _countof( m_szPlayerName ) );
	kAr >> m_dwWorldID;
	return true;
}




} //nsConsignmentSale