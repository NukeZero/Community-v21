#pragma once

#include "FLPacket_ConsignmentSale.h"
#include "FLPacketCommon.h"


#pragma pack(push, 1)
namespace nsConsignmentSale {

typedef	FLPacketPrefix_PlayerID< FLPacket_RegistedInfoListReq >	FLPacketWDB_RegistedInfoListReq;
typedef	FLPacketPrefix_PlayerID< FLPacket_RegistedInfoListAck >	FLPacketWDB_RegistedInfoListAck;

typedef	FLPacketPrefix_PlayerID< FLPacket_CancelRegistedAck >	FLPacketWDB_CancelRegistedAck;

typedef	FLPacketPrefix_PlayerID< FLPacket_BuyItemAck >			FLPacketWDB_BuyItemAck;
typedef	FLPacketPrefix_PlayerID< FLPacket_SellItemNoti >		FLPacketWDB_SellItemNoti;
//typedef	FLPacketPrefix_PlayerID< FLPacket_CollectSaleGoldReq >	FLPacketWDB_CollectSaleGoldReq;		//szPlayerName
typedef	FLPacketPrefix_PlayerIDAndName< FLPacket_CollectSaleGoldReq >	FLPacketWDB_CollectSaleGoldReq;
typedef	FLPacketPrefix_PlayerID< FLPacket_CollectSaleGoldAck >	FLPacketWDB_CollectSaleGoldAck;

typedef	FLPacketPrefix_PlayerID< FLPacket_SearchReq >			FLPacketWDB_SearchReq;
typedef	FLPacketPrefix_PlayerID< FLPacket_SearchAck >			FLPacketWDB_SearchAck;
//typedef	FLPacketPrefix_PlayerID< FLPacket_SaleGoldInfoListReq >	FLPacketWDB_SaleGoldInfoListReq;	//szPlayerName
typedef	FLPacketPrefix_PlayerIDAndName< FLPacket_SaleGoldInfoListReq >	FLPacketWDB_SaleGoldInfoListReq;
typedef	FLPacketPrefix_PlayerID< FLPacket_SaleGoldInfoListAck >	FLPacketWDB_SaleGoldInfoListAck;

struct FLPacketWDB_CheckRegistedCountReq : public FLPacket 
{
public:
	FLPacketWDB_CheckRegistedCountReq( const int nPacketID = PACKETTYPE_CONSIGNMENT_SALE_CHECK_REGISTED_COUNT_REQ );

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	u_long					m_uPlayerID;

	DWORD					m_dwItemObjID;
	int						m_nItemCost;
	int						m_nItemCount;
};

struct FLPacketWDB_CheckRegistedCountAck : public FLPacket 
{
public:
	FLPacketWDB_CheckRegistedCountAck( const int nPacketID = PACKETTYPE_CONSIGNMENT_SALE_CHECK_REGISTED_COUNT_ACK );

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	u_long					m_uPlayerID;
	DWORD					m_dwDefaultRegistCount;
	DWORD					m_dwItemRegistCount;

	DWORD					m_dwItemObjID;
	int						m_nItemCost;
	int						m_nItemCount;
};




struct FLPacketWDB_GetRegistedCountReq : public FLPacket 
{
public:
	FLPacketWDB_GetRegistedCountReq( const int nPacketID = PACKETTYPE_CONSIGNMENT_SALE_GET_REGISTED_COUNT_REQ );

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	u_long					m_uPlayerID;
};

struct FLPacketWDB_GetRegistedCountAck : public FLPacket 
{
public:
	FLPacketWDB_GetRegistedCountAck( const int nPacketID = PACKETTYPE_CONSIGNMENT_SALE_GET_REGISTED_COUNT_ACK );

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	u_long					m_uPlayerID;
	DWORD					m_dwDefaultRegistModeCount;
	DWORD					m_dwItemRegistModeCount;
};

struct FLPacketWDB_GetPriceReq : public FLPacket
{
public:
	FLPacketWDB_GetPriceReq( const int nPacketID = PACKETTYPE_CONSIGNMENT_SALE_GET_PRICE_REQ );

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	u_long					m_uPlayerID;
	UINT64					m_u64SaleSRL;
};


struct FLPacketWDB_CancelRegistedReq : public FLPacket
{
public:
	FLPacketWDB_CancelRegistedReq();

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	u_long					m_uPlayerID;
	UINT64					m_u64SaleSRL;

	TCHAR					m_szPlayerName[ MAX_NAME ];	
	DWORD					m_dwWorldID;
};


struct FLPacketWDB_BuyItemReq : public FLPacket
{
public:
	FLPacketWDB_BuyItemReq();

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	u_long					m_uPlayerID;
	UINT64					m_u64SaleSRL;
	u_long					m_uSalerID;
	int						m_nPrice;

	TCHAR					m_szPlayerName[ MAX_NAME ];	
	DWORD					m_dwWorldID;
};

struct FLPacketWDB_GetPriceAck : public FLPacket
{
public:
	enum E_RESULT
	{
		E_FAILURE			= 0,
		E_SUCCESS,
		E_ALREADY_DELETE,
		E_CANNOT_BUY_SAME_OWNER_ITEM, 
		E_BUSY,
	};

public:
	FLPacketWDB_GetPriceAck( const int nPacketID = PACKETTYPE_CONSIGNMENT_SALE_GET_PRICE_ACK );

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	E_RESULT				m_eResult;
	u_long					m_uPlayerID;
	UINT64					m_u64SaleSRL;
	u_long					m_uSalerID;
	int						m_nPrice;
};

struct FLPacketWDB_ClearReservedInfoNoti : public FLPacket
{
public:
	FLPacketWDB_ClearReservedInfoNoti( const int nPacketID = PACKETTYPE_CONSIGNMENT_SALE_CLEAR_RESERVE_INFO_NOTI );

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	UINT64					m_u64SaleSRL;
};



struct FLPacketWDB_RegistReq : public FLPacket
{
public:
	FLPacketWDB_RegistReq( const int nPacketID = PACKETTYPE_CONSIGNMENT_SALE_REGIST_REQ );

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	u_long					m_uPlayerID;

	int						m_nPrice;
	int						m_nOnePrice;
	int						m_nPay;
	TCHAR					m_szItemName[ ITEM_NAME_BUFFER_LEN ];
	TCHAR					m_szPlayerName[ MAX_NAME ];					
	DWORD					m_dwItemType1;
	DWORD					m_dwItemType2;
	DWORD					m_dwGrade;	
	int						m_nLimitLevel;	
	size_t					m_uRemainHour;
	//sizt_t	,@strExpiredDate			char(20)
	mutable FLItemElem		m_kItemElem;

	E_REGIST_MODE			m_eRegistMode;

	//Itemlog Á¤º¸
	DWORD					m_dwWorldID;
//	int						m_nGold;
};

struct FLPacketWDB_RegistAck : public FLPacket
{
public:
	enum				E_RESULT
	{
		E_FAILURE				= 0,
		E_SUCCESS		
	};

public:
	FLPacketWDB_RegistAck( const int nPacketID = PACKETTYPE_CONSIGNMENT_SALE_REGIST_ACK );

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	u_long					m_uPlayerID;
	E_RESULT				m_eResult;
};


} /* nsConsignmentSale */
#pragma pack(pop)









