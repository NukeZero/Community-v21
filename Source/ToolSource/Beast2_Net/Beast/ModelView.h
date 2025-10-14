#if !defined(AFX_DIALOGVIEW_H__49ED2F11_C2EA_41DB_AD3E_684E9D05C59E__INCLUDED_)
#define AFX_DIALOGVIEW_H__49ED2F11_C2EA_41DB_AD3E_684E9D05C59E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModelView view

class CModelView : public CTreeView
{
	//HTREEITEM m_hApplet;
	HTREEITEM m_hRootObject;
	HTREEITEM m_hRootTerrain;
	BOOL m_bModifiedMdlObj;
	BOOL m_bModifiedTerrain;
public:
	BOOL m_bModifiedMdlDyna;

#ifdef __N_WATERFRAME_050202
public:
	inline HTREEITEM GetHTreeTerrain() { return m_hRootTerrain; }
	inline HTREEITEM GetHTreeObject() { return m_hRootObject; }
#endif//__N_WATERFRAME_050202
 
protected:
	CModelView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CModelView)
	CImageList m_images;
// Attributes
public:
	CImageList*	m_pDragImage;
	BOOL		m_bLDragging;
	HTREEITEM	m_hitemDrag,m_hitemDrop;

	DWORD GetModelType( HTREEITEM hCurrent );
	HTREEITEM GetRootItem( HTREEITEM hCurrent );
	void GetItemImages(HTREEITEM hSrcTItem, int &nSelItemImage, int &nNonSelItemImage);
	int GetItemImage( HTREEITEM hSrcTItem );
	DWORD m_dwObjType;
	DWORD m_dwObjIndex;
	int m_nObjEditTool;

	BOOL IsModified() { return m_bModifiedMdlObj || m_bModifiedMdlDyna || m_bModifiedTerrain; }

	void RemoveTerrain( HTREEITEM hItem );
	void RemoveModelElem( HTREEITEM hTItem );
	void AddItem(HTREEITEM hSrcTItem,HTREEITEM hDestTItem );

	BOOL LoadTerrainScript( LPCTSTR lpszFileName );
	void ReadTerrainScript( CScript& script, HTREEITEM hTreeItem, DWORD dwId );
	BOOL LoadModelScript( HTREEITEM hRootItem,  LPCTSTR lpszFileName );
	void ReadModelScript( CScript& script, HTREEITEM hTreeItem, DWORD dwType );

	BOOL SaveTerrainScript( LPCTSTR lpszFileName );
	void WriteTerrainScript( CFileIO& file, HTREEITEM hTreeItem, int nInsSpace );
	BOOL SaveModelScript( LPCTSTR lpszFileName, DWORD dwObjFilter );
	void WriteModelScript( CFileIO& file, HTREEITEM hTreeItem, DWORD dwType, int nInsSpace );
#ifdef __N_WATERFRAME_050202
	//	IsFolder는 폴더만 카운트를 하느냐를 체크한다.
	int NodeCount( CTreeCtrl* pTree, HTREEITEM hTree, BOOL IsFolder = FALSE);
#endif//__N_WATERFRAME_050202

	BOOL SaveModelTerrain();

	void SetFocus();
	void SetObjEditTool( int nTool );
	void SetTransform();


// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CModelView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CModelView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnPopupCommand(UINT nID);
	afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
	//afx_msg BOOL OnPopupCommand(UINT nID);

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CDlgNewFolder dialog

class CDlgNewFolder : public CDialog
{
// Construction
public:
	CDlgNewFolder(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgNewFolder)
	enum { IDD = IDD_NEW_FOLDER };
	CString	m_strFolderName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNewFolder)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgNewFolder)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGVIEW_H__49ED2F11_C2EA_41DB_AD3E_684E9D05C59E__INCLUDED_)
