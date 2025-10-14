#ifndef __WNDCHANGEFACE__H
#define __WNDCHANGEFACE__H

class CWndChangeFace : public CWndNeuz 
{ 
public: 
	int m_nSelectFace;
	CWndChangeFace(); 
	~CWndChangeFace(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndChangeSex : public CWndNeuz 
{ 
private:
	DWORD	m_dwItemId;
	DWORD	m_dwObjId;
public:
	CModelObject* m_pModel;
	void	SetData( DWORD dwItemId, DWORD dwObjId )
	{	m_dwItemId	= dwItemId;	m_dwObjId	= dwObjId;	}
	
public: 
	int m_nSelectFace;
	CWndChangeSex(); 
	~CWndChangeSex(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndItemTransy : public CWndNeuz 
{ 
public: 
	CRect		m_Rect[2];
	FLItemElem*	m_pItemElem[2];
	FLItemElem   m_pItemElemChange;
	BOOL		m_bMenu;
	
	CWndItemTransy(); 
	~CWndItemTransy(); 
	
	void Init( FLItemElem* pItemElem, BOOL bMenu = FALSE );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnRButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnDestroy( void );
	virtual	BOOL OnDropIcon( LPSHORTCUT	pShortcut, CPoint point );
}; 
#endif	// __WNDCHANGEFACE__H
