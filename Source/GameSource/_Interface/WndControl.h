// WndBase.h: interface for the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDCONTROL_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_)
#define AFX_WNDCONTROL_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "WndControl_Button.h"
#include "WndControl_Text.h"
#include "WndControl_Tab.h"
#include "WndControl_ComboBox.h"

class C2DRender;

// Notification code
#define WNM_ERRSPACE            (-2)
#define WNM_SELCHANGE           1
#define WNM_DBLCLK              2
#define WNM_SELCANCEL           3
#define WNM_SETFOCUS            4 
#define WNM_KILLFOCUS           5
#define WNM_CLICKED             6   
#define WNM_ONITEM				7		//added by gmpbigsun( 20110907 ) : 커서가 아이템위에 위치함.
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
///////
//#define WNM_ITEMCHANGING        30
//#define WNM_ITEMCHANGED         31 
#define WNM_ITEMCLICK           32
#define WNM_ITEMDBLCLICK        33
#define WNM_DIVIDERDBLCLICK     34
#define WNM_BEGINTRACK          35
#define WNM_ENDTRACK            36
#define WNM_TRACK               37

//  WndItemCtrl Norification Message
#define WIN_DBLCLK              40
#define WIN_ITEMDROP            41   
#define WIN_RCLK				42


//NM_CLICK
//WNM_BEGINRDRAG
//HDN_TRACK

/**
 윈도 매시지 박스에서 사용하는 버튼 종류 
 MB_OK                 
 MB_OKCANCEL           
 MB_ABORTRETRYIGNORE   
 MB_YESNOCANCEL        
 MB_YESNO              
 MB_RETRYCANCEL        
**/
// 이것은 추가된 것
#define MB_CANCEL           0x00000006L

//////////////////////////////////////////////////////////////////////////////
// CWndButton 
//////////////////////////////////////////////////////////////////////////////

class CWndStatic : public CWndBase
{
public:
	CWndStatic();// { m_byWndType = WTYPE_STATIC; m_dwColor = 0xff646464; }
	~CWndStatic() { }
	virtual BOOL IsPickupSpace(CPoint point); 
	void OnDraw( C2DRender* p2DRender );
	void SetWndRect( CRect rectWnd, BOOL bOnSize );
	BOOL Create( LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID ,int nR,int nG,int nB);	
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual	void PaintFrame( C2DRender* p2DRender );
	int GetOffset() {return m_nOffsetX;}
	int GetOffsetMax() {return m_nMaxOffset;}
	void SetTextSpeed(int nSpeed){m_nTextSpeed = nSpeed;}
	void SetOffset(int nOffset) {m_nOffsetX = nOffset;}
	//by gmpbigsun(20110624): x축으로 이동하는 효과
private:
	int m_nOffsetX;
	int m_nMaxOffset;
	int m_nTextSpeed;

};

class CWndCustom : public CWndBase
{
public:
	CWndCustom() { m_byWndType = WTYPE_CUSTOM; }
	~CWndCustom() { }

	void OnDraw( C2DRender* p2DRender );
	BOOL Create( LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID )	{
		if( lpszCaption && strlen( lpszCaption ) ) m_strTitle = lpszCaption;
		return CWndBase::Create( dwStyle | WBS_CHILD | WBS_NOFRAME | WBS_NODRAWFRAME, rect,  pParentWnd, nID );
	}
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

//////////////////////////////////////////////////////////////////////////////
// CWndStatic
//////////////////////////////////////////////////////////////////////////////
/*
class CWndStatic : public CWndBase
{
public:

	CWndStatic();
	~CWndStatic();
	BOOL Create(DWORD dwTextStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID);

	virtual	void OnInitialUpdate();
	virtual void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
	virtual	void PaintFrame(C2DRender* p2DRender);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnSetFocus(CWndBase* pOldWnd);
	virtual void OnKillFocus(CWndBase* pNewWnd);
	virtual void OnSize(UINT nType, int cx, int cy);

	LONG GetOffset(CPoint point);
	void UpdateScrollBar();

	CPoint OffsetToPoint(DWORD dwOffset);
	CPoint GetCaretPos() { return m_ptCaret; }
	void DrawCaret(C2DRender* p2DRender);
	void SetCaretPos(CPoint ptCaret) { m_ptCaret = ptCaret; }
	void HideCaret() { m_bCaretVisible = FALSE; }
	void ShowCaret() { m_bCaretVisible = TRUE; }

	void SetString(LPCTSTR pszString);
	void AddString(LPCTSTR pszString);
	void ResetString();
};
*/
//////////////////////////////////////////////////////////////////////////////
// CWndViewCtrl
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// CWndSliderCtrl
//////////////////////////////////////////////////////////////////////////////

class CWndSliderCtrl : public CWndBase
{
public:
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
	int           m_nLineSpace  ;
	DWORD         m_nFontColor  ; 
	DWORD         m_nSelectColor;
	//	DWORD         m_dwListCtrlStyle;

	//CWndListCtrl();
	//BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	CWndListCtrl();
	~CWndListCtrl();

	void Create( DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID );
	void LoadListBoxScript(LPCTSTR lpFileName); 
	int GetFontHeight() { return m_pFont->GetMaxHeight() + m_nLineSpace; }
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

#endif // !defined(AFX_WNDCONTROL_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_)

