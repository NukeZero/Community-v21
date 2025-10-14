#ifndef __SHOP_H
#define __SHOP_H

#define MAX_CART_SLOT 12			//카트 슬롯 최대 갯수 
class CWndItemCtrlVendor : public CWndItemCtrl
{
public:
	CWndItemCtrlVendor();
	~CWndItemCtrlVendor();
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );

};
class CWndConfirmBuy : public CWndNeuz 
{ 
public: 
	CWndConfirmBuy(); 
	~CWndConfirmBuy(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 

private:
	void		OnOK();
	void		OnChangeBuyCount( DWORD dwBuy );

public: 
	CMover*			m_pMover;
	FLItemElem*		m_pItemElem;
	DWORD			m_dwItemId;
	CWndEdit*		m_pEdit;
	CWndStatic*		m_pStatic;
	CWndStatic*		m_pStaticGold;

	int				m_nBuyType;		//sun: 11, 칩으로 상점 이용 하기 (길드 대전 관련)
};

class CWndWarning : public CWndNeuz 
{ 
public: 
	CMover*			m_pMover;
	FLItemElem*		m_pItemElem;
	CWndWarning(); 
	~CWndWarning(); 
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

// _JIN_SHOP_CART
class FLWndCart : public CWndNeuz 
{
public:
	FLWndCart(); 
	~FLWndCart(); 

public: 
	CMover*			m_pMover;
	CWndItemCtrl*	m_pItemCtrl;
	FLItemElem*		m_pItemElem;
	SHORTCUT		m_Shortcut;
	UINT			m_nIdWndTo;
	int				m_nCartCnt;
	int				m_nSlotTemp[MAX_CART_SLOT];
	ULONGLONG		m_ulCostSum;
	std::vector<int>		m_vecItemCheck;
public:
    virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 

private:
	void InsertNumber( int nNumber );
	void DeleteNumber_Init( void );
	void DeleteNumber_Back( void );
	void ViewItemCount( const char* strString,int ItemnCost);
	void OnBTOK();

	UINT GetCostofOneItem( void );

private:
	CWndEdit* m_pWndEdit;
	CWndStatic* m_pWndItemCnt;
	CWndStatic* m_pWndCostText;
	BOOL		m_bSameID;
	LPCHARACTER m_lpCharacter;
	BOOL		m_bBlockState;
}; 

class CWndConfirmSell : public CWndNeuz 
{ 
public: 
	CMover* m_pMover;
	FLItemElem* m_pItemElem;
	CWndEdit* m_pEdit;
	CWndStatic* m_pStatic;
	CWndStatic* m_pStaticGold;
	CWndConfirmSell(); 
	~CWndConfirmSell(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual BOOL Process( void );
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 
class CWndShop : public CWndNeuz 
{ 
public:
	CWndConfirmSell* m_pWndConfirmSell;
	CWndWarning*	m_pWndWarning;
public: 
	BOOL m_bSexSort;
	BOOL m_bLevelSort;
	int		m_nCartCnt;
	
	CMover* m_pMover;
	CWndItemCtrlVendor m_wndItemCtrl[ MAX_VENDOR_INVENTORY_TAB ];
	CWndStatic m_static;
	CWndShop(); 
	~CWndShop(); 
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );
	virtual	void OnDestroy( void );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 ){return FALSE;}
}; 

// _JIN_SHOP_CART
typedef struct _T_SHOPCART
{
	int			m_nCartCnt;
	FLItemElem*	m_pItemElem;
	LPSHORTCUT	m_pShortcut;
	int			m_nShoptab;
	int			m_nItemCnt;
	
}_T_SHOPCART,*PT_T_SHOPCART;


class CFLWndShop_Cart : public CWndNeuz 
{ 
public:
	CFLWndShop_Cart(); 
	~CFLWndShop_Cart(); 

public:
	void SetVecCart(std::vector<PT_T_SHOPCART> vecCart){m_vecCart = vecCart;/*.push_back(ptCart);*/}
	std::vector<PT_T_SHOPCART> GetVecCart(){return m_vecCart;}

	//FLWndCart에 필요한 함수들 ------------------------------------
	void			SetCost(ULONGLONG ulCost){m_ulTempCost = ulCost;}
	ULONGLONG		GetCost(){return m_ulTempCost;}
	void			SetCartCnt(int nCartCnt){m_nCartCnt = nCartCnt;}
	int				GetCartCnt(){return m_nCartCnt;}
	void			SetDropItem(int nDropItemIdx);
	int				GetDropItemIndex(){return m_nDropItem;}
	//------------------------------------------------------------
	int				HitTest(CPoint point);
	BOOL			IsSelectedItem( int iItem );
	BOOL			OnOK();
	CWndTabCtrl*	GetWndShopList(){return m_pWndShopList;}
	CWndStatic*		GetWndCartList(){return m_pWndStatic_Cart;}
	CWndStatic*		GetStaticCost(){return m_pCost;}
	void			ReceivedPacket( PT_PACKET_SHOPPING_BASKET  ptShop );

	virtual void OnMouseWndSurface( CPoint point );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );
	virtual	void OnDestroy( void );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
	virtual void OnMouseMove(UINT nFlags, CPoint point);
public:
	CMover*				m_pMover;
	CWndConfirmSell*	m_pWndConfirmSell;
	CWndWarning*		m_pWndWarning;

	BOOL	m_bSexSort;
	BOOL	m_bLevelSort;

	FLWndCart*			m_pWndCart;
private:

	CWndTabCtrl*		m_pWndShopList;
	CWndStatic*			m_pWndStatic_Cart;
	CWndStatic*			m_pCost;
	CWndStatic*			m_Static_Cart;
	CWndItemCtrl*		m_pItemCtrl;
	CWndStatic*			m_pPlayerGold;
	FLItemElem*			m_pItemDrop;
	FLItemElem*			m_pFocusItem;

	BOOL	m_bWndState;
	ULONGLONG	m_ulTempCost;
	
	//Cart Icon Render position
	int			m_nCart_Width;
	int			m_nCart_nHeight;
	int			m_nCartIcon_PosX;
	int			m_nCartIcon_PosY;
    int			m_nSlotWidth;
	int			m_nCartCnt;
	int			m_nCurSelect;
	int			m_nDropItem;

	CRect		m_rectHittest;
	std::vector<PT_T_SHOPCART>	m_vecCart;
	CWndItemCtrlVendor m_wndItemCtrl[ MAX_VENDOR_INVENTORY_TAB ];

	CDWordArray		m_dwSelectAry;
	LPWNDCTRL		m_lpWndCtrl;
protected:
	static CFLWndShop_Cart* m_pShop_Cart;
public:
	static CFLWndShop_Cart* WndShop_Cart(){return m_pShop_Cart;}
	static void		CreateWndShop_Cart(){new CFLWndShop_Cart();}
	static void		DeleteWndShop_Cart(){SAFE_DELETE(m_pShop_Cart);}

private:
	void	Item_DBLCLK(LRESULT* pLResult);
	void	Item_DROP(UINT nID, LRESULT* pLResult,BOOL bWarning);
	void	Item_CLICKED();
	BOOL	IsFull(FLItemElem* pElem, PT_ITEM_SPEC pItemProp, int nNum );

	UINT GetCostofOneItem( FLItemElem* pItemElem );
	
	void	CalculationWindowPosition( void );
	
}; 


// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
class CWndBeautyShopConfirm;
class CWndUseCouponConfirm;

class CWndBeautyShop : public CWndNeuz 
{ 
public:
	CTexture         m_Texture;
	CWndConfirmSell* m_pWndConfirmSell;
	CModelObject*	 m_pModel;

	DWORD			 m_dwHairMesh;
	BOOL			 m_bLButtonClick;
	CRect			 m_ColorRect[3];
	FLOAT			 m_fColor[3];
	int              m_nHairCost;

	int				 m_nHairColorCost;		// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.

	int              m_nHairColorCostR;
	int              m_nHairColorCostG;
	int              m_nHairColorCostB;	

	CPoint			 m_ColorScrollBar[3];
	CPoint			 m_OriginalColorScrollBar[3];
	
// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
	CModelObject*	 m_pApplyModel;
	CWndEdit*		 m_pRGBEdit[3];
	int				 m_ChoiceBar;
	CModelObject*	 m_pHairModel;
	DWORD			 m_nHairNum[4];
	DWORD			 m_dwSelectHairMesh;
	CWndBeautyShopConfirm* m_pWndBeautyShopConfirm;
	
	BOOL m_bUseCoupon;
	CWndUseCouponConfirm* m_pWndUseCouponConfirm;
	
public: 
	CWndBeautyShop(); 
	~CWndBeautyShop(); 
	void    ReSetBar( FLOAT r, FLOAT g, FLOAT b );

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	
	virtual void OnMouseWndSurface( CPoint point );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );
	virtual	void OnDestroy( void );

// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
	void SetRGBToEdit(float color, int editnum);
	void SetRGBToBar(int editnum);
	void DrawHairKind(C2DRender* p2DRender, D3DXMATRIX matView);
	void UpdateModels();

	void UseHairCoupon(BOOL isUse);
}; 

// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
class CWndUseCouponConfirm : public CWndNeuz 
{	
public:
	BOOL  m_bUseCoupon;
	DWORD m_TargetWndId;
	BOOL  m_checkClose;
	int   m_MainFlag;
public: 
	CWndUseCouponConfirm(); 
	virtual ~CWndUseCouponConfirm(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 

	void SetInfo(DWORD targetWndId, int flag);
}; 

class CWndBeautyShopConfirm : public CWndNeuz 
{
public:
	int m_ParentId;

public: 
	CWndBeautyShopConfirm(); 
	virtual ~CWndBeautyShopConfirm(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndFaceShop : public CWndNeuz
{
public:
	CModelObject* m_pMainModel;
	CModelObject* m_pApplyModel;
	CModelObject* m_pFriendshipFace;
	CModelObject* m_pNewFace;

	DWORD m_dwFriendshipFace;
	DWORD m_dwNewFace;
	DWORD m_nSelectedFace;
	DWORD m_nFriendshipFaceNum[4];
	DWORD m_nNewFaceNum[4];
	int m_nCost;
	int m_ChoiceBar;
	
	CWndBeautyShopConfirm* m_pWndBeautyShopConfirm;

	BOOL m_bUseCoupon;
	CWndUseCouponConfirm* m_pWndUseCouponConfirm;	

public:
	CWndFaceShop(); 
	~CWndFaceShop();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual	void OnDestroy( void );
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );
		
	void DrawFaces(int ChoiceFlag, C2DRender* p2DRender, D3DXMATRIX matView);
	void UpdateModels();

	void UseFaceCoupon(BOOL isUse);
};

#endif