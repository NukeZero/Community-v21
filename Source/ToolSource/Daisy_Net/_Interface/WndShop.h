#ifndef __SHOP_H
#define __SHOP_H

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
	CItemElem*		m_pItemElem;
	DWORD			m_dwItemId;
	CWndEdit*		m_pEdit;
	CWndStatic*		m_pStatic;
	CWndStatic*		m_pStaticGold;
};

class CWndWarning : public CWndNeuz 
{ 
public: 
	CMover* m_pMover;
	CItemElem* m_pItemElem;
	CWndWarning(); 
	~CWndWarning(); 
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

class CWndConfirmSell : public CWndNeuz 
{ 
public: 
	CMover* m_pMover;
	CItemElem* m_pItemElem;
	CWndEdit* m_pEdit;
	CWndStatic* m_pStatic;
	CWndStatic* m_pStaticGold;
	CWndConfirmSell(); 
	~CWndConfirmSell(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
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
#ifdef __SELLITEM1013
	CWndWarning*	m_pWndWarning;
#endif	// __SELLITEM1013
public: 
#ifdef __Y0402
	BOOL m_bSexSort;
	BOOL m_bLevelSort;
#endif

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
}; 

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
	int              m_nHairColorCostR;
	int              m_nHairColorCostG;
	int              m_nHairColorCostB;	
	CPoint			 m_ColorScrollBar[3];
	CPoint			 m_OriginalColorScrollBar[3];
	
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
}; 

#endif
