/////////////////////////////////////////////////////////////////////////////
// Check List Box for 32 and 16 bit
/////////////////////////////////////////////////////////////////////////////

#ifndef __CHKLSTBX_H
#define __CHKLSTBX_H

// Include for resources
#include "resource.h"

#define PADDING			5

#define HSCROLLAMOUNT	10

#define UNCHECKED		0
#define CHECKED			1
#define INDETERMINATE	2

// Structure to hold each item
struct LISTITEM
{
	LISTITEM*	pParentItem;

	int			nCheckedState;
	int			nTotalWidth;
	int			nTextLength;
	int			nLevel;
	bool		bSelected;
	DWORD		dwID;
	CString		csText;

	// Appearence
	COLORREF	crTextColor;
	COLORREF	crTextHighColor;
	COLORREF	crBgHighlightColor;
	
	// List to other child list items
	CPtrList	m_ItemList;
};

/////////////////////////////////////////////////////////////////////////////
// CCheckControl window
class CCheckList;
class CCheckControl : public CWnd
{
	int			m_nCurrentRow,
				m_nScreenRow,
				m_nPrevSelRow,
				m_nViewableRows,
				m_nTop,
				m_nLeft;
	CRect		m_WindowRect;
	LISTITEM*	m_pPrevSelItem;

	// Parents!
	CPtrList*		m_pRootPtrList;
	CCheckList*		m_pParent;
	CWnd*			m_pSuperParent;

	bool		DrawItems(CDC* pDC, CPtrList* pItemList, bool bChangeCheckedState = false, int nCheckedState = CHECKED);
	void		DrawItem(CDC* pDC, LISTITEM* pListItem);
	void		DrawBitmap( CDC* pdc, CSize ImagePos, int nCheckedState );
	LISTITEM*	GetItemClickedOn( CPtrList* pItemList, int nItem );
	void		ChangeParentItems(CDC* pDC, LISTITEM* pItem);

// Construction
public:
	CCheckControl();
	BOOL Create( CRect Rect, CCheckList* pParent, UINT nID, CWnd* pSuperParent, CPtrList* pRootPtrList, COLORREF crBkColor );
	void		UpdateScrollBar();
	void		SetTopIndex(int nTop);
	inline int	GetTopIndex()	{ return m_nTop; };
	int			SetCurSel(int nSelection);
	inline int  GetCurSel()		{ return m_nPrevSelRow; };
	int			SetCheck(int nItem, int nCheckState);
	inline int  GetViewableRows()	{ return m_nViewableRows; };

// Operations
public:
	
// Attributes
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckControl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCheckControl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCheckControl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CCheckList window
class CCheckList : public CWnd
{
	int			m_nLineHeight,
				m_nWidestItem,
				m_nTotalRowCount,
				m_nCurrentRow;

	bool		m_bDeleteFont;
	CRect		m_WindowRect;
	CBitmap*	m_pCheck;
	CBitmap*	m_pUnCheck;
	CBitmap*	m_pMiddleCheck;
	CSize		m_cBitmapDimen;
	
	// Check control 
	CCheckControl*	m_pCheckControl;

	// GDI Items
	CFont*		m_pTextFont;
	CPen*		m_pBkPen;
	CBrush*		m_pBkBrush;
	COLORREF	m_crTextColor;
	COLORREF	m_crTextHighColor;

	// Main Item list
	CPtrList		m_RootItemList;

	void		DeleteItems(CPtrList* pItemList);
	bool		LoadBitmaps(UINT nCheckBitmap, UINT nUnCheckBitmap, UINT nMiddleCheckBitmap);
	LISTITEM*	GetItem( CPtrList* pItemList, int nItem );
	LISTITEM*	GetItem( CPtrList* pItemList, DWORD dwID );
	void		GetWidestItem(CPtrList* pItemList);


// INTERNAL Functions
public:
	inline int			GetWidestItem()		{ return m_nWidestItem; };
	inline int			GetLineHeight()		{ return m_nLineHeight; };
	inline CPen*		GetBkPen()			{ return m_pBkPen; };
	inline CBrush*		GetBkBrush()		{ return m_pBkBrush; };
	inline CFont*		GetTextFont()		{ return m_pTextFont; };
	inline int			GetImageWidth()		{ return m_cBitmapDimen.cx; };
	inline int			GetImageHeight()	{ return m_cBitmapDimen.cy; };
	inline CBitmap*		GetCheckImage()		{ return m_pCheck; };
	inline CBitmap*		GetUnCheckImage()	{ return m_pUnCheck; };
	inline CBitmap*		GetMiddleImage()	{ return m_pMiddleCheck; };

// USER Functions
public:
	CCheckList();
	BOOL Create( CRect Rect, CWnd* pParent, UINT uID, UINT nCheckBitmap, UINT nUnCheckBitmap, UINT nMiddleCheckBitmap,
							COLORREF crBkColor   			= GetSysColor(COLOR_WINDOW),
							CFont* pCustomFont				= NULL);

	LISTITEM*	AddString(	CString		csText,
							LISTITEM*	pParentItem			= NULL,
							int			nCheckState			= UNCHECKED,
							DWORD		dwID				= -1,		
							COLORREF	crTextColor			= GetSysColor(COLOR_INFOTEXT),
							COLORREF	crTextHighColor	 	= GetSysColor(COLOR_HIGHLIGHTTEXT),
							COLORREF	crBgHighlightColor 	= GetSysColor(COLOR_HIGHLIGHT));
	
	int			DeleteString(int nItem);
	int			GetCount(LISTITEM*	pParentItem	= NULL);
	int			GetTopIndex();
	int			SetTopIndex(int nTop);
	DWORD		GetItemData(int nItem);
	int			SetItemData(int nItem, DWORD dwID);
	LISTITEM*	GetItem(int nItem);
	LISTITEM*	GetItem(DWORD dwID);
	int			GetText(int nItem, CString* pString );		
	int			GetTextLen(int nItem );		
	int			GetCurSel();
	int			SetCurSel(int nItem);
	int			SetCheck(int nItem, int nCheckState );
	int			GetCheck(int nItem);
	void		ResetContent();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckList)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCheckList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCheckList)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif


