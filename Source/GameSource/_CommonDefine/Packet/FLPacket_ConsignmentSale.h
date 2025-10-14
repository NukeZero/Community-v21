#pragma once


#include <MsgHdr.h>


#include "../FLCommonContentDefine.h"


#pragma pack(push, 1)

namespace nsConsignmentSale {

struct FLOneSaleInfo
{
	FLOneSaleInfo() : u64SaleSRL( 0 ), nItemCostTot( 0 ), dwRemainSec( 0 ), tmExpiredDate( INVALID_DATETIME ) {	szItemName[ 0 ] = '\0';		szPlayerName[ 0 ] = '\0';		}
	FLOneSaleInfo( const FLOneSaleInfo & r );
	bool				Serialize( CAr & o_kAr );
	virtual	FLOneSaleInfo &  operator = ( const FLOneSaleInfo & r );

	UINT64				u64SaleSRL;
	int					nItemCostTot;
	TCHAR				szItemName[ ITEM_NAME_BUFFER_LEN ];
    TCHAR				szPlayerName[ MAX_NAME ];
	FLTm				tmExpiredDate;

	mutable FLItemElem	kItemElem;

	DWORD				dwRemainSec;
};



struct FLPacket_UserInfoReq : public FLPacket
{
public:
	FLPacket_UserInfoReq();

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );
};

struct FLPacket_UserInfoAck : public FLPacket
{
public:
	FLPacket_UserInfoAck( const int nPacketID = PACKETTYPE_CONSIGNMENT_SALE_USER_INFO_ACK );

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	//DWORD					m_dwRemainRegistCount;
	DWORD					m_dwDefaultRegistCount;
	DWORD					m_dwExtendRegistCount;
	DWORD					m_dwDefaultRegistCountMax;
	DWORD					m_dwExtendRegistCountMax;
};

struct FLSnapshot_UserInfoAck : public FLPacket_UserInfoAck
{
public:
	FLSnapshot_UserInfoAck() : FLPacket_UserInfoAck( SNAPSHOTTYPE_CONSIGNMENT_SALE_USER_INFO_ACK ) {}
};


struct FLPacket_RegistReq : public FLPacket
{
public:
	FLPacket_RegistReq();

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	//DWORD					m_dwInvenIndex;
	DWORD					m_dwItemObjID;
	int						m_nItemCost;
	int						m_nItemCount;
};

struct FLPacket_RegistAck : public FLPacket
{
public:
	enum				E_RESULT
	{
		E_FAILURE				= 0,
		E_SUCCESS,
		E_CRITICAL,

		E_OVERFLOW,
		E_NO_TRADABLE_ITEM,
		E_INVALID_GOLD, 
	};

public:
	FLPacket_RegistAck( const int nPacketID = PACKETTYPE_CONSIGNMENT_SALE_REGIST_ACK );

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	E_RESULT				m_eResult;

	//DWORD					m_dwDefaultRegistCount;
	//DWORD					m_dwExtendRegistCount;
	//DWORD					m_dwDefaultRegistCountMax;
	//DWORD					m_dwExtendRegistCountMax;
};

struct FLSnapshot_RegistAck : public FLPacket_RegistAck
{
public:
	FLSnapshot_RegistAck() : FLPacket_RegistAck( SNAPSHOTTYPE_CONSIGNMENT_SALE_REGIST_ACK ) {}
};



struct FLPacket_RegistedInfoListReq : public FLPacket
{
public:
	FLPacket_RegistedInfoListReq();

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );
};

struct FLPacket_RegistedInfoListAck : public FLPacket
{
public:
	FLPacket_RegistedInfoListAck( const int nPacketID = PACKETTYPE_CONSIGNMENT_SALE_REGISTED_INFO_LIST_ACK );
	virtual ~FLPacket_RegistedInfoListAck();

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	typedef					std::vector< FLOneSaleInfo >		SaleInfoVec;

public:
	mutable DWORD			m_dwCount;
	mutable SaleInfoVec		m_vecSaleInfoList;
};



struct FLSnapshot_RegistedInfoListAck : public FLPacket_RegistedInfoListAck
{
public:
	FLSnapshot_RegistedInfoListAck() : FLPacket_RegistedInfoListAck( SNAPSHOTTYPE_CONSIGNMENT_SALE_REGISTED_INFO_LIST_ACK ) {}
};



struct FLPacket_CancelRegistedReq : public FLPacket
{
public:
	FLPacket_CancelRegistedReq();

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	UINT64					m_u64SaleSRL;
};

struct FLPacket_CancelRegistedAck : public FLPacket
{
public:
	enum				E_RESULT
	{
		E_FAILURE				= 0, 
		E_SUCCESS,

		E_AREADY_CANCELED,
		E_BUSY,
	};

public:
	FLPacket_CancelRegistedAck( const int nPacketID = PACKETTYPE_CONSIGNMENT_SALE_CANCEL_REGISTED_ACK );

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	E_RESULT				m_eResult;
	UINT64					m_u64SaleSRL;
};

struct FLSnapshot_CancelRegistedAck : public FLPacket_CancelRegistedAck
{
public:
	FLSnapshot_CancelRegistedAck() : FLPacket_CancelRegistedAck( SNAPSHOTTYPE_CONSIGNMENT_SALE_CANCEL_REGISTED_ACK ) {}
};



struct FLPacket_BuyItemReq : public FLPacket
{
public:
	FLPacket_BuyItemReq();

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	UINT64					m_u64SaleSRL;
};

struct FLPacket_BuyItemAck : public FLPacket
{
public:
	enum E_RESULT
	{
		E_FAILURE				= 0,
		E_SUCCESS,
		E_ALREADY_DELETE,
		E_BUSY,
	};


public:
	FLPacket_BuyItemAck( const int nPacketID = PACKETTYPE_CONSIGNMENT_SALE_BUY_ITEM_ACK );

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	E_RESULT				m_eResult;
	UINT64					m_u64SaleSRL;
	DWORD					m_dwItemID;
};

struct FLSnapshot_BuyItemAck : public FLPacket_BuyItemAck
{
public:
	FLSnapshot_BuyItemAck() : FLPacket_BuyItemAck( SNAPSHOTTYPE_CONSIGNMENT_SALE_BUY_ITEM_ACK ) {}
};




struct FLPacket_SellItemNoti : public FLPacket
{
public:
	FLPacket_SellItemNoti( const int nPacketID = PACKETTYPE_CONSIGNMENT_SALE_SELL_ITEM_NOTI );

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	char					m_szPlayerName[ MAX_NAME ];
	DWORD					m_dwItemID;
	int						m_nGold;
	E_REGIST_MODE			m_eRegistMode;				//기본 등록 아이템인지. 아이템 사용에 의한 버프 등록인지 구별
};

struct FLSnapshot_SellItemNoti : public FLPacket_SellItemNoti
{
public:
	FLSnapshot_SellItemNoti() : FLPacket_SellItemNoti( SNAPSHOTTYPE_CONSIGNMENT_SALE_SELL_ITEM_NOTI ) {}
};



struct FLPacket_CollectSaleGoldReq : public FLPacket
{
public:
	FLPacket_CollectSaleGoldReq();

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	UINT64					m_u64SaleGoldSRL;
};



struct FLPacket_CollectSaleGoldAck : public FLPacket
{
public:
	enum					E_RESULT
	{
		E_FAILURE			= 0,
		E_SUCCESS,

		E_AREADY_COLLECT,
	};

public:
	FLPacket_CollectSaleGoldAck( const int nPacketID = PACKETTYPE_CONSIGNMENT_SALE_COLLECT_SALE_GOLD_ACK );

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	E_RESULT				m_eResult;

	UINT64					m_u64SaleGoldSRL;
	int						m_nGold;
};

struct FLSnapshot_CollectSaleGoldAck : public FLPacket_CollectSaleGoldAck
{
public:
	FLSnapshot_CollectSaleGoldAck() : FLPacket_CollectSaleGoldAck( SNAPSHOTTYPE_CONSIGNMENT_SALE_COLLECT_GOLD_ACK ) {}
};





struct FLPacket_SearchReq : public FLPacket
{
public:
	enum
	{
		GRADE_MULTI_CHECK_MAX = 4,
		ODERING_COLUMN_STRING_BUFFER_MAX = 64,
	};

	enum		E_OEDERING_OPTION
	{
		E_NONE				= 0,
		E_ITEM_NAME,
		E_ITEM_LEVEL,
		E_PRICE,
		E_ONE_PRICE,
		E_ABILITY_OPTION,
	};

	static const char *		EnumToString( const E_OEDERING_OPTION eOption );

public:
	FLPacket_SearchReq();

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	E_OEDERING_OPTION		m_eOrderingOption;
	BOOL					m_bASC;					//TRUE : 순차

	DWORD					m_dwPage;

	DWORD					m_arrGrade[ GRADE_MULTI_CHECK_MAX ];

	int						m_nLimitLevelMin;
	int						m_nLimitLevelMax;

	int						m_nAbilityOptionMin;
	int						m_nAbilityOptionMax;

	DWORD					m_dwItemType1;
	DWORD					m_dwItemType2;

	TCHAR					m_szSearchName[ ITEM_NAME_BUFFER_LEN ];
	BOOL					m_bSearchBySameName;
};


struct FLPacket_SearchAck : public FLPacket
{
public:
	FLPacket_SearchAck( const int nPacketID = PACKETTYPE_CONSIGNMENT_SALE_SEARCH_ACK );
	virtual ~FLPacket_SearchAck();

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	typedef					std::vector< FLOneSaleInfo >		SaleInfoVec;

public:
	DWORD					m_dwCountMax;
	DWORD					m_dwPage;
	//DWORD					m_dwPageMax;

	mutable DWORD			m_dwCount;
	mutable SaleInfoVec		m_vecSaleInfoList;
};

struct FLSnapshot_SearchAck : public FLPacket_SearchAck
{
public:
	FLSnapshot_SearchAck() : FLPacket_SearchAck( SNAPSHOTTYPE_CONSIGNMENT_SALE_SEARCH_ACK )		{}
};


struct FLPacket_SaleGoldInfoListReq : public FLPacket
{
public:
	FLPacket_SaleGoldInfoListReq();

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );
};

struct FLPacket_SaleGoldInfoListAck : public FLPacket
{
public: 
	struct FLSaleGoldInfo 
	{
		FLSaleGoldInfo();
		FLSaleGoldInfo( const FLSaleGoldInfo & r );

		void				Serialize( CAr & kAr );
		virtual	FLSaleGoldInfo &  operator = ( const FLSaleGoldInfo & r );

		TCHAR				szItemName[ ITEM_NAME_BUFFER_LEN ];
		UINT64				u64SaleGoldSRL;
		int					nGold;
		FLTm				kSaleDate;
		mutable FLItemElem	kItemElem;
	};


public:
	FLPacket_SaleGoldInfoListAck( const int nPacketID = PACKETTYPE_CONSIGNMENT_SALE_COLLECT_SALE_GOLD_INFO_LIST_ACK );

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	typedef					std::vector< FLSaleGoldInfo >	SaleGoldInfoVec;

	mutable DWORD			m_dwCount;
	mutable SaleGoldInfoVec	m_vecSaleGoldInfo;
};

struct FLSnapshot_SaleGoldInfoListAck : FLPacket_SaleGoldInfoListAck
{
public:
	FLSnapshot_SaleGoldInfoListAck() : FLPacket_SaleGoldInfoListAck( SNAPSHOTTYPE_CONSIGNMENT_SALE_SALE_GOLD_INFO_LIST_ACK ) {}
};


struct FLPacket_CalcTaxReq : public FLPacket
{
public:
	FLPacket_CalcTaxReq( const int nPacketID = PACKETTYPE_CONSIGNMENT_SALE_CALC_TAX_REQ );

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	int						m_nPrice;
};

struct FLSnapshot_CalcTaxAck : public FLPacket
{
public:
	FLSnapshot_CalcTaxAck( const int nPacketID = SNAPSHOTTYPE_CONSIGNMENT_SALE_CALC_TAX_ACK );

protected:
	virtual bool			BodySerialize( CAr & kAr ) const;
	virtual bool			BodyDeserialize( CAr & kAr );

public:
	int						m_nTax;
};



} /* nsConsignmentSale */
#pragma pack(pop)









