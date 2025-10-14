#pragma once

//sun: 12, 세금
#include "Continent.h"

#define TAX_SALES		(BYTE)0	// 판매 세금
#define	TAX_PURCHASE	(BYTE)1 // 구매 세금
#define	TAX_ADMISSION	(BYTE)2 // 입장료(천상의 탑)

#ifdef __DBSERVER
#include "../DatabaseServer/dbcontroller.h"

enum {	QUERY_TAX_LOAD, 
		QUERY_TAX_PAY,
		QUERY_TAX_CHANGENEXT,
		QUERY_TAX_SETNEXT_SECRETROOMWINGUILD, 
		QUERY_TAX_SETNEXT_LORD,
		QUERY_TAX_SETNEXT_TAXRATE,
		QUERY_TAX_ADDTAX,
};

class CTaxDBController : public CDbController
{
public:
	CTaxDBController(void);
	~CTaxDBController(void);

	virtual	void	Handler( LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey );
	virtual void	OnTimer();

private:
	void PayTaxToPost();	// 당일 세금 수입을 우편으로 보낸다.
	void LoadTaxInfo();		// DB에 저장된 세금 정보를 로딩 후 월드 서버로 전송
	void InsertToDB();		// 변경된 정보를 DB에 추가
	void UpdateToDB( BYTE nContinent ); // 해당 대륙의 세금 정보를 저장
	void UpdateAllToDB();

	int m_nTimes;			// 회차
	BOOL m_bLoadTaxInfo;		// DB로 부터 로딩했는가?
};
#endif // __DBSERVER


struct __TAXDETAIL
{
	int nTaxRate;
#ifdef __DBSERVER
	int nTaxCount;
	int	nTaxGold;
	int nTaxPerin;
	int	nNextTaxRate;
#endif // __DBSERVER
	__TAXDETAIL()
	{
		nTaxRate = 0;
#ifdef __DBSERVER
		nTaxCount = 0;
		nTaxGold = 0;
		nTaxPerin = 0;
		nNextTaxRate = 0;
#endif // __DBSERVER
	}
};
typedef std::map<BYTE, __TAXDETAIL*> TAXDETAILMAP;

struct __TAXINFO
{
	DWORD			dwCurrPlayerID;
	BOOL			bSetTaxRate;
	TAXDETAILMAP	mapTaxDetail;
	DWORD			dwNextPlayerID;
	
	__TAXINFO()
	{
		dwCurrPlayerID	= NULL_ID;
		bSetTaxRate		= TRUE;
		mapTaxDetail.insert(std::make_pair( TAX_SALES, new __TAXDETAIL ) );			// 구매 세금은 기본
		mapTaxDetail.insert(std::make_pair( TAX_PURCHASE, new __TAXDETAIL ) );		// 판매 세금은 기본
		dwNextPlayerID	= NULL_ID;
	}
};
typedef std::map<BYTE, __TAXINFO*> TAXINFOMAP;


class CTax
{
public:
	CTax(void);
	~CTax(void);

	static CTax* GetInstance( void );
	void Serialize( CAr & ar );
	
	int m_nMinTaxRate;					// 최소 세율
	int m_nMaxTaxRate;					// 최대 세율

	TAXINFOMAP m_mapTaxInfo;	// 각 대륙별 점령길드 세율 정보 및 세금 수입

	__TAXINFO* GetTaxInfo( BYTE nContinent );
	void SetNextSecretRoomGuild( BYTE nCont, DWORD dwGuildId );
	void SetNextLord( DWORD dwIdPlayer );
	void SetNextTaxRate( BYTE nCont, int nSalesTaxRate, int nPurchaseTaxRate );
	void SetApplyTaxRateNow();	// GM명령으로 강제로 다음 세율을 적용한다.
	
	void	UpdateTaxRate( const int nTaxRate );

	BOOL AddTax( BYTE nCont, int nTax, BYTE nTaxKind );

#ifdef __WORLDSERVER
	void SendSetTaxRateOpenWnd( BYTE nCont, DWORD dwGuildId );
	void SendNoSetTaxRateOpenWnd( FLWSUser* pUser );
#endif // __WORLDSERVER

#ifdef __DBSERVER
	void LoadScript();
	BOOL CheckPayTime();		// 세금 수입 지급 시간 검사		
	void CheckChangeTime( BOOL bPay, BOOL bGameMaster = FALSE );		// 다음 세율 변경 시간 검사
	void SetChangeNextTax();	// 다음 세율 변경
	float GetEarningRate( BYTE nCont, BYTE nTaxKind ); // 수익률
	void LoadTaxInfo( DWORD dpId );

	void	SetUpdateTaxRate( const int nTaxRate );

	int m_nTaxSecretRoomRate;		// 비밀의 방 점령길드 수익률
	int m_nTaxLordRate;				// 군주 세금 수익률
	int m_nAdmissionSecretRoomRate;	// 동부 점령길드 입장료 수익률
	int m_nAdmissionLordRate;		// 군주 입장료 수익률
	int m_nDBSaveCount;				// DB에 저장 빈도(건수)
	
	CLuaBase m_Lua;
	CTaxDBController m_taxDBController;
	std::string	m_strChangedDate;		// 마지막으로 세율이 변경된 시간
#endif // __DBSERVER

#ifndef __DBSERVER
	// 아래 모든 pMover는 전부 판매자이다.( 판매자가 NPC인 경우 pMover는 NPC이다. )
	BYTE	GetContinent( CMover* pMover );
	float	GetSalesTaxRate( BYTE nContinent );
	float	GetSalesTaxRate( CMover* pMover );
	float	GetPurchaseTaxRate( BYTE nContinent );
	float	GetPurchaseTaxRate( CMover* pMover );
	bool	IsApplyPurchaseTaxRate( CMover* pMover, FLItemElem* pItemElem );
	bool	IsApplySalesTaxRate( CMover* pMover, FLItemElem* pItemElem );
private:
	bool	IsApplyTaxRate( CMover* pMover, FLItemElem* pItemElem );	// 세율적용이 가능한 아이템인가?
public:
#endif // __DBSERVER

	std::vector<DWORD> m_vecdwOccupationShopItem;

#ifdef __WORLDSERVER
	BOOL IsOccupationGuildMember( CMover* pMover );
	BOOL IsOccupationShopItem( DWORD dwItemId );
#endif // __WORLDSERVER
};
