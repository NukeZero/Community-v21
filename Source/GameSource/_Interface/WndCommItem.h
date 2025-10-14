#ifndef __WNDCOMMITEM__H
#define __WNDCOMMITEM__H

class C2DRender;
class FLItemElem;

#define ELLDINSJAR_COUNT 1

class CWndCommItemCtrl : public CWndBase
{
	void InterpriteScript(CScanner& scanner,CPtrArray& ptrArray); 
	//void PaintListBox(C2DRender* p2DRender,CPoint& pt,CPtrArray& ptrArray);
	int           m_nFontHeight ;
	CWndScrollBar m_wndScrollBar;
	
	CWndWorld* pWndWorld;
	int			m_dwDraw[SM_MAX+MAX_SKILLINFLUENCE + ELLDINSJAR_COUNT];
	int			m_nMaxDraw;

	CTexture m_texGauEmptyNormal ;
	CTexture m_texGauFillNormal  ;
	
	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge;


// Constructors
public:
	CRect         m_rect;
	DWORD         m_dwListCtrlStyle;


	void	DrawSM( C2DRender* p2DRender, CPoint* pPoint, int x, int &nScroll );
	void	DrawSkill( C2DRender* p2DRender, CPoint* pPoint, int x, int &nScroll );
	void	DrawTicket( C2DRender* p2DRender, CPoint* pPoint, int x, int &nScroll );
	void	DrawElldinsJar( C2DRender* p2DRender, CPoint* pPoint, int x, int &nScroll );

	CWndCommItemCtrl();
	~CWndCommItemCtrl();

	void GetFriendName();
	void Create( DWORD m_dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID );
	void LoadListBoxScript(LPCTSTR lpFileName); 
	LPSKILL GetSkill( int i );

	// Attributes
	COLORREF GetBkColor() const;
	BOOL SetBkColor(COLORREF cr);
	BOOL GetItem(LVITEM* pItem) const;
	BOOL SetItem(int nItem, int nSubItem, UINT nMask, LPCTSTR lpszItem,
		int nImage, UINT nState, UINT nStateMask, LPARAM lParam);

	virtual void OnMouseWndSurface( CPoint point );
	virtual	void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE );
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void PaintFrame( C2DRender* p2DRender );

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

// Implementation
public:
	int InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState,
		UINT nStateMask, int nImage, LPARAM lParam);
protected:
	void RemoveImageList(int nImageList);

private:
	//////////////////////////////////////////////////////////////////////////

	WORD	m_wRenderBuffList[MAX_RENDER_BUFF_COUNT];
	void	InitRenderBuffList();
	void	InsertRenderBuffList( WORD wRendBuffID );
	bool	IsRenderBuff( WORD wRendBuffID );
	//////////////////////////////////////////////////////////////////////////
};

class CWndCommItem : public CWndNeuz 
{ 
public:
	CWndCommItemCtrl	m_wndCommItemCtrl;
public: 
	CWndCommItem(); 
	~CWndCommItem(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndCommercialElem : public CWndNeuz 
{ 
public: 
	FLItemElem* m_pItemElem[2];
	CRect      m_Rect[2];

	void InitSetting( void );
	BOOL IsRestrictionItem( FLItemElem* pItemElem, BOOL bMessage = FALSE );
	BOOL IsUpgradeItem( FLItemElem* pItemElem, BOOL bMessage = FALSE );
	BOOL IsSMItem( FLItemElem* pItemElem, BOOL bMessage = FALSE );
	
	
	CWndCommercialElem(); 
	~CWndCommercialElem(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
}; 

class CWndRemoveElem : public CWndNeuz 
{ 
public: 
	BYTE		m_nType;
	int			m_nParts;
	DWORD		m_dwItemId;
	OBJID		m_objid;
	FLItemElem*	m_pItemElem;
	BOOL		m_bSetting;

	CWndRemoveElem(); 
	~CWndRemoveElem(); 

	void OnSetItem( BYTE nType, DWORD dwItemId, OBJID objid, int nParts, FLItemElem* pItemElem );
	
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
