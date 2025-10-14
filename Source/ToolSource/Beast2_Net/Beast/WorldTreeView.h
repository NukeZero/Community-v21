#if !defined(AFX_WORLDTREEVIEW_H__49ED2F11_C2EA_41DB_AD3E_684E9D05C59E__INCLUDED_)
#define AFX_WORLDTREEVIEW_H__49ED2F11_C2EA_41DB_AD3E_684E9D05C59E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogView.h : header file
//
#ifdef __CSC_WORLDOBJ_LIST
#define IMAGE_ROOT    0
#define IMAGE_GROUP   1
#define IMAGE_FOLDER  2
#define IMAGE_MODEL   3
#define IMAGE_MOTION  4 
#define IMAGE_TEXTURE 5

typedef struct __WORLD_ELEMENT
{
	DWORD m_dwType;
	DWORD m_dwIndex;

	vector<void*> m_veclist;
} __WORLD_ELEMENT;
#endif //__CSC_WORLDOBJ_LIST
/////////////////////////////////////////////////////////////////////////////
// CWorldTreeView view

class CWorldTreeView : public CTreeView
{
	HTREEITEM m_hApplet;
protected:
	CWorldTreeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CWorldTreeView)
	CImageList m_images;
// Attributes
public:
#ifdef __CSC_WORLDOBJ_LIST
	map<DWORD, __WORLD_ELEMENT> m_MapList[MAX_OBJTYPE];
#endif //__CSC_WORLDOBJ_LIST

// Operations
public:
#ifdef __CSC_WORLDOBJ_LIST
	void ResetObjList();
	void MakeObjList(CLandscape* pLand);
	void ResetTree();
#endif //__CSC_WORLDOBJ_LIST
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorldTreeView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CWorldTreeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CWorldTreeView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);
	//afx_msg BOOL OnPopupCommand(UINT nID);
		//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORLDTREEVIEW_H__49ED2F11_C2EA_41DB_AD3E_684E9D05C59E__INCLUDED_)
