#ifndef __WNDBANK__H
#define __WNDBANK__H

class CWndBank : public CWndNeuz 
{ 
public: 
	CWndGold     m_wndGold[3];
	CWndItemCtrl m_wndItemCtrl[3];
	CRect RectItemCtrl[3];
	CRect RectGold[3];
	CWndStatic* pCost[3];
	BOOL  bUse[3];
#ifdef __YSBANK_IMAGE1014	
	CTexture*    m_pTexture;
#endif

	CWndBank(); 
	~CWndBank(); 
	void ReSetBank( void );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndConfirmBank : public CWndNeuz 
{ 
public:
#ifdef __INSTANT_BANK0715
	DWORD	m_dwId;
	DWORD	m_dwItemId;
	void	SetItem( DWORD dwId, DWORD dwItemId )	{	m_dwId	= dwId;	m_dwItemId	= dwItemId;		}
#endif	// __INSTANT_BANK0715
public: 
	CWndConfirmBank(); 
	~CWndConfirmBank(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndBankPassword : public CWndNeuz 
{ 
public:
	int m_nFlags;
#ifdef __INSTANT_BANK0715
	DWORD	m_dwId;
	DWORD	m_dwItemId;
	void	SetItem( DWORD dwId, DWORD dwItemId )	{	m_dwId	= dwId;		m_dwItemId	= dwItemId;		}
#endif	// __INSTANT_BANK0715
public: 
	CWndBankPassword(); 
	~CWndBankPassword(); 
	
	void SetBankPassword( int nFlags );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 


#endif
