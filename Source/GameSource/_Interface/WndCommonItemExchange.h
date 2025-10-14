
#pragma once 

#include "WndCommonItemExchangeConfirm.h"

namespace WndCommonItemExchange
{
	typedef struct stIconDrawInfo
	{
		DWORD	m_dwItemID;
		CRect	m_rtItemIconDraw;		
		DWORD	m_dwMinGeneralEnchant;
		DWORD	m_dwMaxGeneralEnchant;
		BYTE	m_bind_flag;
	
		stIconDrawInfo()
		:	m_dwItemID( NULL_ID )
		,	m_rtItemIconDraw( 0, 0 ,0 ,0 )
		,	m_dwMinGeneralEnchant( NULL )
		,	m_dwMaxGeneralEnchant( NULL )
		,	m_bind_flag( 0 )
		{
		}
	}ICONDRAWINFO, *PICONDRAWINFO;
};

using namespace WndCommonItemExchange;

//---------------------------------------------------------------------------------------------
// 범용 아이템 교환 윈도우
//---------------------------------------------------------------------------------------------
class CWndCommonItemExchange	:	public CWndNeuz
{

private:

	CWndText*									m_pWndDescription;
	CWndListBox*								m_pWndExchangeList;

	CTexture*									m_pTexExchangeArrow;

	DWORD										m_dwMMIDefineIndex;
	DWORD										m_dwCurrentDescPageIndex;
	UINT										m_nTotalDescPageCount;

	ExchangeItemListMap							m_mapExchangeItem;

	typedef std::vector< ICONDRAWINFO >				vecIconDrawRect;	
	vecIconDrawRect								m_vecIconDrawRect;					// 아이콘 그리기 영역 리스트

	BYTE										m_byConditionItemMaxCount;			// 필요 아이템 중 가장 많은 개수를 저장한다. ( 줄 맞춰 그리기 위해 )
	BYTE										m_byPaymentItemMaxCount;			// 보상 아이템 중 가장 많은 개수를 저장한다. ( 줄 맞춰 그리기 위해 )

	bool										m_bSendPacket;		

	BYTE										m_byPaymentItemColorChangeCount;	// 보상 아이템 색상을 변경을 위한 카운트
	CTimer										m_PaymentItemColorChangeTimer;		// 보상 아이템 색상 변경용 타이머

	ICONDRAWINFO								m_stCurrentOnMouseItemDrawInfo;		// 현재 마우스가 올라가 있는 아이템의 그리기 정보

	CWndCommonItemExchangeConfirm*				m_pWndCommonItemExchangeConfirm;	// 교환 개수 입력 창
	DWORD										m_dwPossibleMaxExchangeCount;		// 교환 가능 한 최대 개수


public:

												CWndCommonItemExchange( void );
	virtual										~CWndCommonItemExchange( void );

	virtual BOOL 								Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);

	virtual	void 								OnInitialUpdate( void );
	virtual void 								OnDraw( C2DRender* p2DRender );
	virtual BOOL								OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 

	bool										SetItemExchangeInfo( const DWORD dwMMIDefineIndex );
	void										SetSendPacket( void );
	void										SetReceivedPacket( void );

	ICONDRAWINFO								GetCurrentOnMouseItemDrawInfo( void );


private:


	bool										SetDescriptionString( void );
	bool										CreateExchangeItemList( void );

	void										UpdatePaymentItemColorChange( void );

	void										SetViewPort( C2DRender* const p2DRender );	
	bool										CheckExchangeConditionItemHaveCount( const T_EXCHANGE_ITEM& stData );

	bool										DrawExchangeItemList( C2DRender* const p2DRender );
	bool										DrawExchangeItem_ConditionItemIcon( C2DRender* const p2DRender, const ExchangeItemMap& mapConditionItem, CPoint ptItem, const CRect rtItem );
	bool										DrawExchangeItem_ExchangeArrow( C2DRender* const p2DRender, CPoint ptItem, const CRect rtItem );
	bool										DrawExchangeItem_PaymentItemIcon( C2DRender* const p2DRender, const ExchangeItemMap& mapPaymentItem, CPoint ptItem, const CRect rtItem );
	bool										DrawExchangeItem_WidthLine( C2DRender* p2DRender, const UINT& nCount, const CRect& rtItem );
	bool										DrawExchangeItem_SelectBox( C2DRender* p2DRender, const UINT& nCount, CPoint ptItem, const CRect& rtItem );

	bool										DrawToolTip( void );


	bool										CheckPossibleMaxExchangeCount( void );



};