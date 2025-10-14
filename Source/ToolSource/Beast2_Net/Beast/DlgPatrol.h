#if !defined(AFX_DLGPATROL_H__BE42686D_6339_4D63_81FE_89BFD9E25D1A__INCLUDED_)
#define AFX_DLGPATROL_H__BE42686D_6339_4D63_81FE_89BFD9E25D1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPatrol.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgPatrol dialog

class CDlgPatrol : public CDialog
{
// Construction
public:
	void LoadPatrol();
	void RenderPath( CObj* pObj );
	CString GetPathStringName( DWORD dwIndex );
	void UnSelectPathList();
	void SetSelectPathList(DWORD dwIndex, BOOL bFlag );
	void UpdateList();
	void AddPath( CPathObj* pPathobj );
	DWORD   m_dwOldIndx;
	DWORD   m_dwIndex;
	CDlgPatrol(CWnd* pParent = NULL);   // standard constructor
	BOOL	IsEdit();
	void RenderPath( DWORD dwIndex );
	void RenderAllPath();
		

// Dialog Data
	//{{AFX_DATA(CDlgPatrol)
	enum { IDD = IDD_PATROL };
	CButton	m_Edit;
	CListBox	m_PathList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPatrol)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPatrol)
	afx_msg void OnPathNew();
	afx_msg void OnPathDelete();
	afx_msg void OnPathEdit();
	afx_msg void OnSelchangePathList();
	afx_msg void OnPatrolSave();
	afx_msg void OnPatrolLoad();
	afx_msg void OnApplyObject();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPATROL_H__BE42686D_6339_4D63_81FE_89BFD9E25D1A__INCLUDED_)
