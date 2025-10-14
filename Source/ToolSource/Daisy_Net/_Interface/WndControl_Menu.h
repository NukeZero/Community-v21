
#if _MSC_VER > 1000
#pragma once
#endif 

class C2DRender;

//////////////////////////////////////////////////////////////////////////////
// CWndMenu
class CWndMenu : public CWndBase
{
	BOOL IsOnMenu(CPoint pt);
	int m_nLargeWidth;
public:
	CPtrArray  m_awndMenuItem;
//	CWndButton m_pWndCommand[18];

// Constructors
	CWndMenu();
	~CWndMenu();

	CWndButton* GetMenuItem( int nPos );
	void SetVisibleSub( BOOL bVisible );
	void SetVisibleAllMenu( BOOL bVisible );

	BOOL CreateMenu(CWndBase* pWndParent);
	BOOL CreatePopupMenu();
	BOOL LoadMenu(LPCTSTR lpszResourceName);
	BOOL LoadMenu(UINT nIDResource);
	BOOL LoadMenuIndirect(const void* lpMenuTemplate);
	BOOL DestroyMenu();
	virtual void OnKillFocus(CWndBase* pNewWnd);
	virtual void PaintFrame(C2DRender* p2DRender);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
// Attributes
	//HMENU m_hMenu;          // must be first data member
	//HMENU GetSafeHmenu() const;
	//operator HMENU() const;

	//static CMenu* PASCAL FromHandle(HMENU hMenu);
	//static void PASCAL DeleteTempMap();
	//BOOL Attach(HMENU hMenu);
	//HMENU Detach();

// CMenu Operations
	void DeleteAllMenu();
	BOOL DeleteMenu(UINT nPosition, UINT nFlags);
	//BOOL TrackPopupMenu(UINT nFlags, int x, int y,CWnd* pWnd, LPCRECT lpRect = 0);
	//BOOL operator==(const CMenu& menu) const;
	//BOOL operator!=(const CMenu& menu) const;

// CMenuItem Operations
	virtual CWndButton* AppendMenu(UINT nFlags, UINT nIDNewItem = 0,	LPCTSTR lpszNewItem = NULL);
	//BOOL AppendMenu(UINT nFlags, UINT nIDNewItem, const CBitmap* pBmp);
	UINT CheckMenuItem(UINT nIDCheckItem, UINT nCheck);
	UINT EnableMenuItem(UINT nIDEnableItem, UINT nEnable);
	UINT GetMenuItemCount() const;
	UINT GetMenuItemID(int nPos) const;
	UINT GetMenuState(UINT nID, UINT nFlags) const;
	int  GetMenuString(UINT nIDItem, LPTSTR lpString, int nMaxCount,	UINT nFlags) const;
	int  GetMenuString(UINT nIDItem, CString& rString, UINT nFlags) const;
	//BOOL GetMenuItemInfo(UINT nIDItem, LPMENUITEMINFO lpMenuItemInfo,	BOOL fByPos = FALSE);
	CWndMenu* GetSubMenu(int nPos) const;
	BOOL InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem = 0,	LPCTSTR lpszNewItem = NULL);
	//BOOL InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem,	const CBitmap* pBmp);
	BOOL ModifyMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem = 0,	LPCTSTR lpszNewItem = NULL);
	//BOOL ModifyMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem,	const CBitmap* pBmp);
	BOOL RemoveMenu(UINT nPosition, UINT nFlags);
	//BOOL SetMenuItemBitmaps(UINT nPosition, UINT nFlags, const CBitmap* pBmpUnchecked, const CBitmap* pBmpChecked);
	BOOL CheckMenuRadioItem(UINT nIDFirst, UINT nIDLast, UINT nIDItem, UINT nFlags);
	BOOL SetDefaultItem(UINT uItem, BOOL fByPos = FALSE);
	UINT GetDefaultItem(UINT gmdiFlags, BOOL fByPos = FALSE);

// Context Help Functions
	//BOOL SetMenuContextHelpId(DWORD dwContextHelpId);
	//DWORD GetMenuContextHelpId() const;

// Overridables (must override draw and measure for owner-draw menu items)
	//virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
};
