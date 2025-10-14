#if !defined(AFX_DIALOGVIEW_H__49ED2F11_C2EA_41DB_AD3E_684E9D05C59E__INCLUDED_)
#define AFX_DIALOGVIEW_H__49ED2F11_C2EA_41DB_AD3E_684E9D05C59E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CResourceView view

class CResourceView : public CTreeView
{
	HTREEITEM m_hApplet;
protected:
	CResourceView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CResourceView)
	CImageList m_images;
// Attributes
public:

// Operations
public:
	void AddApplet( LPCTSTR lpszWIDC, LPWNDAPPLET lpWndBase );
	void ChangeApplet( LPCTSTR lpszOldWIDC, LPCTSTR lpszNewWIDC );
	void SortTreeApplet();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResourceView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CResourceView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CResourceView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnPopupCommand(UINT nID);
		//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void NewApplet();
	void AppClose(LPWNDAPPLET lpWndApplet);
	void AppRemove(LPWNDAPPLET lpWndApplet,CTreeCtrl* pTreeCtrl,HTREEITEM hItem);
	void AppOpen(LPWNDAPPLET lpWndApplet);

public:
	afx_msg void OnSearch();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGVIEW_H__49ED2F11_C2EA_41DB_AD3E_684E9D05C59E__INCLUDED_)
