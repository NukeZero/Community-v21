
#pragma once 

namespace WndFlyffPieceSystem
{
	//---------------------------------------------------------------------------------------------
	// 트리 아이템 데이터
	//---------------------------------------------------------------------------------------------
	typedef struct stTREEDATA
	{
		DWORD		_dwKey;
		DWORD		_dwParentKey;

		CString		_strTitle;
		DWORD		_dwType[2];

		stTREEDATA()
		:	_dwKey( NULL_ID )
		,	_dwParentKey( NULL_ID )
		,	_strTitle( _T("") )
		{  
			ZeroMemory( _dwType, sizeof( DWORD ) * 2 );
		}
	}TREEDATA, *LPTREEDATA;
}

using namespace WndFlyffPieceSystem;

//---------------------------------------------------------------------------------------------
// 프리프 조각 시스템
//---------------------------------------------------------------------------------------------
class CWndFlyffPieceSystem		:	public CWndNeuz
{

private:

	
	CWndTreeCtrl*								m_pExchangeItemKindList;
	CWndListBox*								m_pExchangeItemList;
	CWndButton*									m_pExchangeButton;

	LPWNDCTRL									m_pExchangeItemKindCtrl;
	LPWNDCTRL									m_pExchangeItemCtrl;
	LPWNDCTRL									m_pButtonItem;
	LPWNDCTRL									m_pButtonLevel;
	LPWNDCTRL									m_pButtonCoupon;

	CRect										m_rtExchangeItemKindList;
	CRect										m_rtExchangeItemList;
	CRect										m_rtButtonItem;
	CRect										m_rtButtonLevel;
	CRect										m_rtButtonCoupon;

	CTexture*									m_pTexGuildImage;						// 안내 배경 이미지		
	CTexture*									m_pTexLoadingImage;						// 로딩 배경 이미지	

	CWndListBox::LPLISTITEM						m_pFocusItem;							// 현재 선택 된 아이템

	bool										m_bClickedExchangeItemKind_Type1;		// 아이템 종류 트리 리스트 중에서 대분류를 클릭하였는가 ?

	bool										m_bSendItemListPacket;					// 패킷 Send 했으면 true, Ack 받았다면 false ( 아이템 리스트 요청 )
	bool										m_bSendItemExchangePacket;				// 패킷 Send 했으면 true, Ack 받았다면 false ( 아이템 교환 요청 )

	BOOL										m_bAscendingOrderByName;				// 오름차순 정렬인가 (아이템 이름)
	BOOL										m_bAscendingOrderByLevel;				// 오름차순 정렬인가 (필요 레벨)
	BOOL										m_bAscendingOrderByNeedCouponCound;		// 오름차순 정렬인가 (필요 쿠폰 개수)

	typedef std::map< DWORD, TREEDATA* >				mapTreeDataContainer;
	typedef mapTreeDataContainer::iterator		mapTreeDataIter;						

	mapTreeDataContainer						m_mapTreeData;							// 스크립트에서 읽어드린 아이템 종류 리스트

	typedef std::vector< T_EXCHANGE_ITEM_DATA >		vecExchangeItem;					
	vecExchangeItem								m_vecExchangeItem;						// 교환 아이템 리스트

	typedef std::vector< CRect >						vecIconRect;	
	vecIconRect									m_vecIconRect;							// 아이콘 영역 리스트

public:


												CWndFlyffPieceSystem( void );
	virtual										~CWndFlyffPieceSystem( void );

	virtual BOOL 								Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);

	virtual BOOL								Process( void );
	virtual	void 								OnInitialUpdate( void );
	virtual void 								OnDraw( C2DRender* p2DRender );
	virtual BOOL								OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual BOOL								OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 

	bool										SendPacket_ItemListReq( DWORD dwTreeKey );
	void										SetListData( const vecExchangeItem vecItemData );

	void										SetSendItemExchangePacket( bool bSend )		{	m_bSendItemExchangePacket = bSend;		}			


private:


	bool										ParsingScript(  const char* szFileName );

	bool										CreateExchangeItemKindList( void );
	bool										CreateExchangeItemList( void );

	void										DrawGuideImage( C2DRender* p2DRender );
	void										DrawGuideText( C2DRender* p2DRender );

	void										DrawLoadingImage( C2DRender* p2DRender );
	void										DrawLoadingText( C2DRender* p2DRender );

	void										DrawExchangeItem( C2DRender* p2DRender );
	void										DrawItemListEmptyText( C2DRender* p2DRender );
	void										DrawTooltip( void );

	void										SetViewPort( C2DRender* p2DRender );
	void										DrawExchangeItem_Icon( C2DRender* p2DRender, const PT_ITEM_SPEC pProp, CPoint ptItem, const CRect rtItem );
	void										DrawExchangeItem_Name( C2DRender* p2DRender, const PT_ITEM_SPEC pProp, CPoint ptItem, const CRect rtItem );
	void										DrawExchangeItem_Level( C2DRender* p2DRender, const PT_ITEM_SPEC pProp, CPoint ptItem, const CRect rtItem );
	void										DrawExchangeItem_CouponIcon( C2DRender* p2DRender, const PT_ITEM_SPEC pProp, CPoint ptItem, const CRect rtItem );
	void										DrawExchangeItem_CouponCount( C2DRender* p2DRender, const UINT& nCount, CPoint ptItem, const CRect rtItem );
	void										DrawExchangeItem_SelectBox( C2DRender* p2DRender, DWORD dwExchangeItemDataIndex, CPoint ptItem, const CRect rtItem );

	void										DrawExchangeItem_WidthLine( C2DRender* p2DRender, const UINT& nCount, const CRect& rtItem );
	void										DrawExchangeItem_HeightLine( C2DRender* p2DRender );

	void										SendExchangeItemPacket( DWORD dwItemIndex );

	CString										GetExchangeItemMessageBoxText( void );

	BOOL										Process_ExchangeButtonClicked( UINT nID );
	BOOL										Process_CancelButtonClicked( UINT nID );
	BOOL										Process_MessageBoxButtonClicked( UINT nID, CWndBase* pWndBase );
	BOOL										Process_ItemButtonClicked( UINT nID );
	BOOL										Process_LevelButtonClicked( UINT nID );
	BOOL										Process_NeedCouponCountButtonClicked( UINT nID );

	static bool									SortItemListByName_Ascending( const T_EXCHANGE_ITEM_DATA& rLeft, const T_EXCHANGE_ITEM_DATA& rRight );									
	static bool									SortItemListByName_Descending( const T_EXCHANGE_ITEM_DATA& rLeft, const T_EXCHANGE_ITEM_DATA& rRight );										

	static bool									SortItemListByLevel_Ascending( const T_EXCHANGE_ITEM_DATA& rLeft, const T_EXCHANGE_ITEM_DATA&  rRight );										
	static bool									SortItemListByLevel_Descending( const T_EXCHANGE_ITEM_DATA& rLeft, const T_EXCHANGE_ITEM_DATA&  rRight );										
	
	static bool									SortItemListByNeedCouponCount_Ascending( const T_EXCHANGE_ITEM_DATA& rLeft, const T_EXCHANGE_ITEM_DATA&  rRight );										
	static bool									SortItemListByNeedCouponCount_Descending( const T_EXCHANGE_ITEM_DATA& rLeft, const T_EXCHANGE_ITEM_DATA&  rRight );										

};