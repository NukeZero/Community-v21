// WndBase.h: interface for the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDCONTROL_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_)
#define AFX_WNDCONTROL_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndControl_Button.h"
#include "WndControl_Static.h"
#include "WndControl_Custom.h"
#include "WndControl_Tab.h"

class C2DRender;

// Notification code
#define WNM_ERRSPACE            (-2)
#define WNM_SELCHANGE           1
#define WNM_DBLCLK              2
#define WNM_SELCANCEL           3
#define WNM_SETFOCUS            4 
#define WNM_KILLFOCUS           5
#define WNM_CLICKED             6   
///////
#define WNM_ITEMCHANGING        10
#define WNM_ITEMCHANGED         11
#define WNM_INSERTITEM          12
#define WNM_DELETEITEM          13
#define WNM_DELETEALLITEMS      14
#define WNM_BEGINLABELEDITA     15
#define WNM_BEGINLABELEDITW     16
#define WNM_ENDLABELEDITA       17
#define WNM_ENDLABELEDITW       18
#define WNM_COLUMNCLICK         19
#define WNM_BEGINDRAG           20
#define WNM_BEGINRDRAG          21

#define WNM_ITEMCLICK           32
#define WNM_ITEMDBLCLICK        33
#define WNM_DIVIDERDBLCLICK     34
#define WNM_BEGINTRACK          35
#define WNM_ENDTRACK            36
#define WNM_TRACK               37

//  WndItemCtrl Norification Message
#define WIN_DBLCLK              40
#define WIN_ITEMDROP            41   

// 이것은 추가된 것
#define MB_CANCEL           0x00000006L


//////////////////////////////////////////////////////////////////////////////
// CWndScrollBar
//////////////////////////////////////////////////////////////////////////////

class CWndScrollBar : public CWndBase
{
protected:
	int        m_nScrollPos;
	int        m_nScrollMinPos;
	int        m_nScrollMaxPos;
	int        m_nScrollBarIdx;
	int        m_nScrollPage;
	int        m_nPos;
	BOOL       m_bPushPad; 
	CRect      m_rectStick;
	CWndButton m_wndArrow1;
	CWndButton m_wndArrow2;

public:
	CTexture* m_pTexButtVScrUp   ;
	CTexture* m_pTexButtVScrDown ;
	CTexture* m_pTexButtVScrBar  ;
	CTexture* m_pTexButtVScrPUp  ;
	CTexture* m_pTexButtVScrPDown;
	CTexture* m_pTexButtVScrPBar ;

//static CTexturePack m_texturePack;
	
	CWndScrollBar();
	~CWndScrollBar();

	BOOL Create(DWORD dwScrollStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID);//,CSprPack* pSprPack,int nSprIdx);

	int  GetMinScrollPos();
	int  GetMaxScrollPos();

	void SetMinScrollPos();
	void SetMaxScrollPos();
	int  GetScrollPos() const; // Retrieves the current position of a scroll box. 
	int  SetScrollPos(int nPos, BOOL bRedraw = TRUE); // Sets the current position of a scroll box. 
	void GetScrollRange(LPINT lpMinPos, LPINT lpMaxPos) const; // Retrieves the current minimum and maximum scroll-bar positions for the given scroll bar. 
	int  GetScrollRange() const;
	void SetScrollRange(int nMinPos, int nMaxPos, BOOL bRedraw = TRUE); // Sets minimum and maximum position values for the given scroll bar. 
	void SetScrollPage(int nPage);
	int  GetScrollPage() const;
	int  GetScrollAbiliableRange();
	int  GetScrollAbiliablePos();
	int  GetScrollAbiliablePage(); 
	void ShowScrollBar(BOOL bShow = TRUE); // Shows or hides a scroll bar. 
	BOOL EnableScrollBar(UINT nArrowFlags = ESB_ENABLE_BOTH); // Enables or disables one or both arrows of a scroll bar. 
	BOOL SetScrollInfo(LPSCROLLINFO lpScrollInfo, BOOL bRedraw = TRUE); // Sets information about the scroll bar. 
	BOOL GetScrollInfo(LPSCROLLINFO lpScrollInfo, UINT nMask = SIF_ALL); // Retrieves information about the scroll bar. 
	int  GetScrollLimit(); // Retrieves the limit of the scroll bar
	void SetScrollDown();
/*
	CSprite* GetSprVertArrow1();
	CSprite* GetSprVertArrow2();
	CSprite* GetSprVertPad();
	CSprite* GetSprVertBar();
	CSprite* GetSprHoriArrow1();
	CSprite* GetSprHoriArrow2();
	CSprite* GetSprHoriPad();
	CSprite* GetSprHoriBar();
*/
	virtual void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
	virtual	void PaintFrame(C2DRender* p2DRender);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
};
//////////////////////////////////////////////////////////////////////////////
// CWndText
//////////////////////////////////////////////////////////////////////////////
#define EBS_LEFT             0x00000000L
#define EBS_CENTER           0x00000001L
#define EBS_RIGHT            0x00000002L
#define EBS_MULTILINE        0x00000004L
#define EBS_UPPERCASE        0x00000008L
#define EBS_LOWERCASE        0x00000010L
#define EBS_PASSWORD         0x00000020L
#define EBS_AUTOVSCROLL      0x00000040L
#define EBS_AUTOHSCROLL      0x00000080L
#define EBS_NOHIDESEL        0x00000100L
#define EBS_OEMCONVERT       0x00000400L
#define EBS_READONLY         0x00000800L
#define EBS_WANTRETURN       0x00001000L
#define EBS_NUMBER           0x00002000L

class CWndText : public CWndBase
{
protected:
	CTimer m_timerCaret;
	BOOL   m_bCaretVisible;
	CPoint m_ptCaret;
	BOOL   m_bLButtonDown;

	DWORD m_dwBlockBegin;
	DWORD m_dwBlockEnd;
	DWORD m_dwOffset;
	
	BOOL	m_bScr;
	int		m_nLineRefresh;

public:
	TCHAR m_TextComp[3];
	TCHAR m_szCaret[ 3 ];
	BOOL m_bEnableClipboard;
	//CTexture* m_apTexture[ 9 ];
	int m_nLineSpace; 
	CWndScrollBar m_wndScrollBar;
	CEditString m_string;

	CWndText();
	~CWndText();
	BOOL Create( DWORD dwTextStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID, int nFontColorR = 0, int nFontColorG = 0, int nFontColorB = 0 );
#ifdef __V050322_CACHEQUEST
	CPoint m_ptDeflateRect;
#else // __V050322_CACHEQUEST
//{{AFX
	void AdditionalSkinTexture( LPWORD pDest, CSize size );
//}}AFX
#endif	
	virtual	void OnInitialUpdate();
	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual	void SetFontColor(int nR,int nG,int nB);
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnMouseMove( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual void OnSetFocus( CWndBase* pOldWnd );
	virtual void OnKillFocus( CWndBase* pNewWnd );
	virtual void OnSize( UINT nType, int cx, int cy );
	virtual void OnChar( UINT nChar );
//	virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
//	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
#ifdef __Y0602
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
#endif
		
	int GetFontHeight() { return m_pFont->GetMaxHeight() + m_nLineSpace; }

	LONG GetOffset( CPoint point );
	void UpdateScrollBar();
	BOOL IsEmptyBlock() { return m_dwBlockBegin == m_dwBlockEnd; }
	CPoint OffsetToPoint( DWORD dwOffset, TCHAR* pszStr );
	CPoint GetCaretPos() { return m_ptCaret; }
virtual void DrawCaret( C2DRender* p2DRender );
	void SetCaretPos( CPoint ptCaret ) { m_ptCaret = ptCaret; m_timerCaret.Reset(); }
	void HideCaret() { m_bCaretVisible = FALSE; }
	void ShowCaret() { m_bCaretVisible = TRUE; }

	void SetString( LPCTSTR pszString, DWORD dwColor = 0xff000000 );
	void AddString( LPCTSTR pszString, DWORD dwColor = 0xff000000 );
	void ResetString();
};


//////////////////////////////////////////////////////////////////////////////
// CWndViewCtrl
//////////////////////////////////////////////////////////////////////////////

typedef struct tagScriptElem
{
#ifdef __V050322_CACHEQUEST
	tagScriptElem* m_lpParent;
	DWORD m_dwColor;
#endif
	CString   m_strKeyword;
	DWORD     m_dwData;
	CPtrArray m_ptrArray;
	BOOL      m_bOpen;
} 
TREEELEM,* LPTREEELEM;

class CWndTreeCtrl : public CWndBase
{
	typedef struct tagITEM
	{
		CRect      m_rect;
		BOOL       m_bButton;
		LPTREEELEM m_lpTreeElem;
	} TREEITEM,* LPTREEITEM;

	CPtrArray m_treeItemArray;
	void InterpriteScript( CScript& script, CPtrArray& ptrArray ); 
	void PaintTree( C2DRender* p2DRender, CPoint& pt, CPtrArray& ptrArray );
	LPTREEELEM m_pFocusElem;
	int  m_nFontHeight ;
	DWORD m_nWndColor   ;
	TREEELEM m_treeElem;
	CWndScrollBar m_wndScrollBar;
	void FreeTree( CPtrArray& ptrArray );
public:
#ifdef __V050322_CACHEQUEST
	int   m_nLineSpace  ;
	CTexture* m_pTexButtOpen;
	CTexture* m_pTexButtClose;
#endif
	DWORD m_nFontColor  ; 
	DWORD m_nSelectColor;

	CWndTreeCtrl();
	~CWndTreeCtrl();
#ifdef __V050322_CACHEQUEST
	int GetFontHeight() { return m_pFont->GetMaxHeight() + m_nLineSpace; }
	BOOL DeleteAllItems();
	LPTREEELEM GetCurSel();
	LPTREEELEM GetRootElem();
	LPTREEELEM GetNextElem( LPTREEELEM pElem, int& nPos );
	LPTREEELEM FindTreeElem( DWORD dwData );
	LPTREEELEM FindTreeElem( CPtrArray& ptrArray, DWORD dwData );
	LPTREEELEM SetCurSel( DWORD dwData );
	LPTREEELEM SetCurSel( LPCTSTR lpszKeyword );
#endif
	LPTREEELEM FindTreeElem( LPCTSTR lpszKeyword );
	LPTREEELEM FindTreeElem( CPtrArray& ptrArray, LPCTSTR lpszKeyword );
	LPTREEELEM InsertItem( LPTREEELEM lpParent, LPCTSTR lpString, DWORD dwData );
	void LoadTreeScript( LPCTSTR lpFileName ); 
	BOOL Create(DWORD dwTextStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID,int nFontColorR, int nFontColorG, int nFontColorB);
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual void OnInitialUpdate();
	virtual void OnDraw( C2DRender* p2DRender );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual void OnRButtonDblClk( UINT nFlags, CPoint point );
	virtual void OnSize( UINT nType, int cx, int cy );
	virtual BOOL OnEraseBkgnd( C2DRender* p2DRender );
#ifndef __V050322_CACHEQUEST
//{{AFX
	virtual void AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface );
//}}AFX
#endif
#ifdef __Y0602
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
#endif		

};
//////////////////////////////////////////////////////////////////////////////
// CWndSliderCtrl
//////////////////////////////////////////////////////////////////////////////

class CWndSliderCtrl : public CWndBase
{
public:
};
//////////////////////////////////////////////////////////////////////////////
// CWndListBox
//////////////////////////////////////////////////////////////////////////////

/*
 * Listbox Styles
 */
#define WLBS_NOTIFY            0x0001L
#define WLBS_SORT              0x0002L
#define WLBS_NOREDRAW          0x0004L
#define WLBS_MULTIPLESEL       0x0008L
#define WLBS_OWNERDRAWFIXED    0x0010L
#define WLBS_OWNERDRAWVARIABLE 0x0020L
#define WLBS_HASSTRINGS        0x0040L
#define WLBS_USETABSTOPS       0x0080L
#define WLBS_NOINTEGRALHEIGHT  0x0100L
#define WLBS_MULTICOLUMN       0x0200L
#define WLBS_WANTKEYBOARDINPUT 0x0400L
#define WLBS_EXTENDEDSEL       0x0800L
#define WLBS_DISABLENOSCROLL   0x1000L
#define WLBS_NODATA            0x2000L
#define WLBS_NOSEL             0x4000L
#define WLBS_STANDARD          (LBS_NOTIFY | LBS_SORT | WS_VSCROLL | WS_BORDER)

class CWndListBox : public CWndBase
{
	void InterpriteScript(CScanner& scanner,CPtrArray& ptrArray); 
	void PaintListBox(C2DRender* p2DRender,CPoint& pt,CPtrArray& ptrArray);

public:
	typedef struct tagITEM
	{
		CRect      m_rect;
		BOOL       m_bButton;
		CString    m_strWord;
		DWORD      m_dwData;
	} LISTITEM,* LPLISTITEM;

protected:
	CPtrArray m_listItemArray;
	LPLISTITEM m_pFocusItem;
	int           m_nCurSelect  ;
	int           m_nFontHeight ;
	int           m_nLineSpace  ;
	DWORD         m_nWndColor   ;
	LISTITEM      m_listItem    ;
	CWndScrollBar m_wndScrollBar;
public:
	DWORD         m_nFontColor  ; 
	DWORD         m_nSelectColor;

	CWndListBox();
	~CWndListBox();

	void Create( DWORD dwListBoxStyle, RECT& rect, CWndBase* pParentWnd, UINT nID, int nR = 0, int nG = 0, int nB = 0 );
	int GetFontHeight() { return m_pFont->GetMaxHeight() + m_nLineSpace; }
	int   GetCount() const;
	DWORD GetItemData(int nIndex) const;
	void* GetItemDataPtr(int nIndex) const;
	int   SetItemData(int nIndex,DWORD dwItemData);
	int   SetItemDataPtr(int nIndex,void* pData);
	int   GetItemRect(int nIndex,LPRECT lpRect) const;
	int   GetSel(int nIndex) const;
	int   GetText(int nIndex,LPSTR lpszBuffer) const;
	void  GetText(int nIndex,CString& rString) const;
	int   GetTextLen(int nIndex) const;

	int   GetCurSel() const;
	int   SetCurSel(int nSelect);
	int   SetSel(int nIndex,BOOL bSelect = TRUE);
	int   GetSelCount() const;
	int   GetSelItems(int nMaxItems,LPINT rgIndex) const;

	int   AddString(LPCTSTR lpszItem);
	int   DeleteString(UINT nIndex);
	int   InsertString(int nIndex,LPCTSTR lpszItem);
	void	SetString( int nIndex, LPCTSTR lpszItem );
	void  ResetContent();
	int   FindString(int nStartAfter,LPCTSTR lpszItem) const;
	int   FindStringExact(int nIndexStart,LPCTSTR lpszItem) const;
	int   SelectString(int nStartAfter,LPCTSTR lpszItem);
	void  SortListBox();

	void LoadListBoxScript(LPCTSTR lpFileName); 

	virtual	void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnRButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnEraseBkgnd( C2DRender* p2DRender );
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual void OnSetFocus( CWndBase* pOldWnd );
	//virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
#ifndef __V050322_CACHEQUEST
//{{AFX
	virtual void AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface );
//}}AFX
#endif
#ifdef __Y0602
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
#endif
	
	friend int QSortListBox( const VOID* arg1, const VOID* arg2 );
};
// class CListCtrl  CButton

/////////////////////////////////////////////////////////////////////////////
// CListCtrl
// begin_r_commctrl
// List view Control

#define WLVS_ICON                0x0000
#define WLVS_REPORT              0x0001
#define WLVS_SMALLICON           0x0002
#define WLVS_LIST                0x0003
#define WLVS_TYPEMASK            0x0003
#define WLVS_SINGLESEL           0x0004
#define WLVS_SHOWSELALWAYS       0x0008
#define WLVS_SORTASCENDING       0x0010
#define WLVS_SORTDESCENDING      0x0020
#define WLVS_SHAREIMAGELISTS     0x0040
#define WLVS_NOLABELWRAP         0x0080
#define WLVS_AUTOARRANGE         0x0100
#define WLVS_EDITLABELS          0x0200
#define WLVS_NOSCROLL            0x2000
#define WLVS_TYPESTYLEMASK       0xfc00
#define WLVS_ALIGNTOP            0x0000
#define WLVS_ALIGNLEFT           0x0800
#define WLVS_ALIGNMASK           0x0c00
#define WLVS_OWNERDRAWFIXED      0x0400
#define WLVS_NOCOLUMNHEADER      0x4000
#define WLVS_NOSORTHEADER        0x8000

// end_r_commctrl

class CWndEdit;

class CWndListCtrl : public CWndBase
{
	void InterpriteScript(CScanner& scanner,CPtrArray& ptrArray); 
	//void PaintListBox(C2DRender* p2DRender,CPoint& pt,CPtrArray& ptrArray);
	LVITEM*       m_pFocusItem;
	int           m_nCurSelect  ;
	int           m_nFontHeight ;
	DWORD         m_nWndColor   ;
//	DWORD         m_nFontColor  ; 
//	DWORD         m_nSelectColor;
	CWndScrollBar m_wndScrollBar;
	CPtrArray     m_aItems;
	CPtrArray     m_aColumns;

// Constructors
public:
#ifdef __V050322_CACHEQUEST
	int           m_nLineSpace  ;
#endif
	DWORD         m_nFontColor  ; 
	DWORD         m_nSelectColor;
	//	DWORD         m_dwListCtrlStyle;

	//CWndListCtrl();
	//BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	CWndListCtrl();
	~CWndListCtrl();

	void Create( DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID );
	void LoadListBoxScript(LPCTSTR lpFileName); 
#ifdef __V050322_CACHEQUEST
	int GetFontHeight() { return m_pFont->GetMaxHeight() + m_nLineSpace; }
#endif
// Attributes
	int   GetCurSel() const;
	int   SetCurSel(int nSelect);
	COLORREF GetBkColor() const;
	BOOL SetBkColor(COLORREF cr);
	//CImageList* GetImageList(int nImageList) const;
	//CImageList* SetImageList(CImageList* pImageList, int nImageListType);
	int GetItemCount() const;
	BOOL GetItem(LVITEM* pItem) const;
	BOOL SetItem(const LVITEM* pItem);
	BOOL SetItem(int nItem, int nSubItem, UINT nMask, LPCTSTR lpszItem,
		int nImage, UINT nState, UINT nStateMask, LPARAM lParam);
	UINT GetCallbackMask() const;
	BOOL SetCallbackMask(UINT nMask);
	int GetNextItem(int nItem, int nFlags) const;
	POSITION GetFirstSelectedItemPosition() const;
	int GetNextSelectedItem(POSITION& pos) const;
	BOOL GetItemRect(int nItem, LPRECT lpRect, UINT nCode) const;
	BOOL SetItemPosition(int nItem, POINT pt);
	BOOL GetItemPosition(int nItem, LPPOINT lpPoint) const;
	int GetStringWidth(LPCTSTR lpsz) const;
	CWndEdit* GetEditControl() const;
	BOOL GetColumn(int nCol, LVCOLUMN* pColumn) const;
	BOOL SetColumn(int nCol, const LVCOLUMN* pColumn);
	int GetColumnWidth(int nCol) const;
	BOOL SetColumnWidth(int nCol, int cx);
	BOOL GetViewRect(LPRECT lpRect) const;
	COLORREF GetTextColor() const;
	BOOL SetTextColor(COLORREF cr);
	COLORREF GetTextBkColor() const;
	BOOL SetTextBkColor(COLORREF cr);
	int GetTopIndex() const;
	int GetCountPerPage() const;
	BOOL GetOrigin(LPPOINT lpPoint) const;
	BOOL SetItemState(int nItem, LVITEM* pItem);
	BOOL SetItemState(int nItem, UINT nState, UINT nMask);
	UINT GetItemState(int nItem, UINT nMask) const;
	CString GetItemText(int nItem, int nSubItem) const;
	int GetItemText(int nItem, int nSubItem, LPTSTR lpszText, int nLen) const;
	BOOL SetItemText(int nItem, int nSubItem, LPCTSTR lpszText);
	void SetItemCount(int nItems);
	BOOL SetItemData(int nItem, DWORD dwData);
	DWORD GetItemData(int nItem) const;
	UINT GetSelectedCount() const;
	BOOL SetColumnOrderArray(int iCount, LPINT piArray);
	BOOL GetColumnOrderArray(LPINT piArray, int iCount = -1);
	CSize SetIconSpacing(CSize size);
	CSize SetIconSpacing(int cx, int cy);
	//CHeaderCtrl* GetHeaderCtrl();
	//HCURSOR GetHotCursor();
	//HCURSOR SetHotCursor(HCURSOR hc);
	BOOL GetSubItemRect(int iItem, int iSubItem, int nArea, CRect& ref);
	int GetHotItem();
	int SetHotItem(int iIndex);
	int GetSelectionMark();
	int SetSelectionMark(int iIndex);
	DWORD GetExtendedStyle();
	DWORD SetExtendedStyle(DWORD dwNewStyle);
	int SubItemHitTest(LPLVHITTESTINFO pInfo);
	void SetWorkAreas(int nWorkAreas, LPRECT lpRect);
	BOOL SetItemCountEx(int iCount, DWORD dwFlags = LVSICF_NOINVALIDATEALL);
	CSize ApproximateViewRect(CSize sz = CSize(-1, -1), int iCount = -1) const;
	BOOL GetBkImage(LVBKIMAGE* plvbkImage) const;
	DWORD GetHoverTime() const;
	void GetWorkAreas(int nWorkAreas, LPRECT prc) const;
	BOOL SetBkImage(HBITMAP hbm, BOOL fTile = TRUE, int xOffsetPercent = 0, int yOffsetPercent = 0);
	BOOL SetBkImage(LPTSTR pszUrl, BOOL fTile = TRUE, int xOffsetPercent = 0, int yOffsetPercent = 0);
	BOOL SetBkImage(LVBKIMAGE* plvbkImage);
	DWORD SetHoverTime(DWORD dwHoverTime = (DWORD)-1);
	UINT GetNumberOfWorkAreas() const;
	BOOL GetCheck(int nItem) const;
	BOOL SetCheck(int nItem, BOOL fCheck = TRUE);

// Operations
	int InsertItem(const LVITEM* pItem);
	int InsertItem(int nItem, LPCTSTR lpszItem);
	int InsertItem(int nItem, LPCTSTR lpszItem, int nImage);
	BOOL DeleteItem(int nItem);
	BOOL DeleteAllItems();
	//int FindItem(LVFINDINFO* pFindInfo, int nStart = -1) const;
	//int HitTest(LVHITTESTINFO* pHitTestInfo) const;
	int HitTest(CPoint pt, UINT* pFlags = NULL) const;
	BOOL EnsureVisible(int nItem, BOOL bPartialOK);
	BOOL Scroll(CSize size);
	BOOL RedrawItems(int nFirst, int nLast);
	BOOL Arrange(UINT nCode);
	CEdit* EditLabel(int nItem);
	int InsertColumn(int nCol, const LVCOLUMN* pColumn);
	int InsertColumn(int nCol, LPCTSTR lpszColumnHeading,
		int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);
	BOOL DeleteColumn(int nCol);
//	CImageList* CreateDragImage(int nItem, LPPOINT lpPoint);
	BOOL Update(int nItem);
	//BOOL SortItems(PFNLVCOMPARE pfnCompare, DWORD dwData);

// Overridables
	//virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual	void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnRButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual	void PaintFrame(C2DRender* p2DRender);
#ifndef __V050322_CACHEQUEST
//{{AFX
	virtual void AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface );
//}}AFX
#endif
// Implementation
public:
	int InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState,
		UINT nStateMask, int nImage, LPARAM lParam);
	//virtual ~CWndListCtrl();
protected:
	void RemoveImageList(int nImageList);
//	virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);
protected:
	////{{AFX_MSG(CListCtrl)
	//afx_msg void OnNcDestroy();
	////}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};


//////////////////////////////////////////////////////////////////////////////
// CWndGroupBox
//////////////////////////////////////////////////////////////////////////////

class CWndGroupBox : public CWndBase
{
protected:

public:
	CWndGroupBox();
	~CWndGroupBox();
	BOOL Create( DWORD dwTextStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID );

	virtual	void OnInitialUpdate();
	//virtual void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
	virtual	void PaintFrame(C2DRender* p2DRender);
	virtual void OnDraw(C2DRender* p2DRender);
	//virtual void OnLButtonDown(UINT nFlags, CPoint point);
	//virtual void OnLButtonUp(UINT nFlags, CPoint point);
	//virtual void OnMouseMove(UINT nFlags, CPoint point);
	//virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	//virtual void OnSetFocus(CWndBase* pOldWnd);
	//virtual void OnKillFocus(CWndBase* pNewWnd);aa
	//virtual void OnSize(UINT nType, int cx, int cy);
};

//////////////////////////////////////////////////////////////////////////////
// CWndComboBox
//////////////////////////////////////////////////////////////////////////////

#define WCBS_SIMPLE            0x0001L
#define WCBS_DROPDOWN          0x0002L
#define WCBS_DROPDOWNLIST      0x0003L
#define WCBS_OWNERDRAWFIXED    0x0010L
#define WCBS_OWNERDRAWVARIABLE 0x0020L
#define WCBS_AUTOHSCROLL       0x0040L

#define WCBS_OEMCONVERT        0x0080L
#define WCBS_SORT              0x0100L
#define WCBS_HASSTRINGS        0x0200L
#define WCBS_NOINTEGRALHEIGHT  0x0400L
#define WCBS_DISABLENOSCROLL   0x0800L
#define WCBS_UPPERCASE         0x2000L
#define WCBS_LOWERCASE         0x4000L

#include "WndEditCtrl.h"

class CWndComboBox : public CWndEdit
{
public: 
//	DWORD m_dwComboBoxStyle; 
	CWndListBox m_wndListBox;
	CWndButton  m_wndButton;

	CWndComboBox();
	~CWndComboBox();

// Attributes
	// for entire combo box
	int GetCount() const;
	int GetCurSel() const;
	int SetCurSel(int nSelect);
	LCID GetLocale() const;
	LCID SetLocale(LCID nNewLocale);
// Win4
	int GetTopIndex() const;
	int SetTopIndex(int nIndex);
	int InitStorage(int nItems, UINT nBytes);
	void SetHorizontalExtent(UINT nExtent);
	UINT GetHorizontalExtent() const;
	int SetDroppedWidth(UINT nWidth);
	int GetDroppedWidth() const;

	// for edit control
	DWORD GetEditSel() const;
	BOOL LimitText(int nMaxChars);
	BOOL SetEditSel(int nStartChar, int nEndChar);

	// for combobox item
	DWORD GetItemData(int nIndex) const;
	int SetItemData(int nIndex, DWORD dwItemData);
	void* GetItemDataPtr(int nIndex) const;
	int SetItemDataPtr(int nIndex, void* pData);
	int GetLBText(int nIndex, LPTSTR lpszText) const;
	void GetLBText(int nIndex, CString& rString) const;
	int GetLBTextLen(int nIndex) const;

	int SetItemHeight(int nIndex, UINT cyItemHeight);
	int GetItemHeight(int nIndex) const;
	int FindStringExact(int nIndexStart, LPCTSTR lpszFind) const;
	int SetExtendedUI(BOOL bExtended = TRUE);
	BOOL GetExtendedUI() const;
	void GetDroppedControlRect(LPRECT lprect) const;
	BOOL GetDroppedState() const;

// Operations
	// for drop-down combo boxes
	void ShowDropDown(BOOL bShowIt = TRUE);

	// manipulating listbox items
	int AddString(LPCTSTR lpszString);
	int DeleteString(UINT nIndex);
	int InsertString(int nIndex, LPCTSTR lpszString);
	void ResetContent();
	int Dir(UINT attr, LPCTSTR lpszWildCard);

	// selection helpers
	int FindString(int nStartAfter, LPCTSTR lpszString) const;
	int SelectString(int nStartAfter, LPCTSTR lpszString);

	// Clipboard operations
	void Clear();
	void Copy();
	void Cut();
	void Paste();
/*
// Overridables (must override draw, measure and compare for owner draw)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
*/


	void Create( DWORD dwComboBoxStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID, int nR = 0, int nG = 0, int nB = 0 );
	virtual	void OnInitialUpdate();
	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
};

#endif // !defined(AFX_WNDCONTROL_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_)

