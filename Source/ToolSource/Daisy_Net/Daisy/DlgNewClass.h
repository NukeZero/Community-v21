#if !defined(AFX_DLGNEWCLASS_H__5451B9A9_72BF_4078_8BA5_50556A84EF8B__INCLUDED_)
#define AFX_DLGNEWCLASS_H__5451B9A9_72BF_4078_8BA5_50556A84EF8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgNewClass.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgNewClass dialog

class CDlgNewClass : public CDialog
{
// Construction
public:
	CWndDialog* m_pWndDialog;
	CDlgNewClass(CWnd* pParent = NULL);   // standard constructor

	BOOL m_bEditFileName;
	CString m_strWndId;
	CString m_strClassName;
	CString m_strPath;
	CString m_strFileName;
	CString m_strHeaderName;

	void Init( CWndDialog* pWndDialog );
// Dialog Data
	//{{AFX_DATA(CDlgNewClass)
	enum { IDD = IDD_NEW_CLASS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNewClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgNewClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditName();
	virtual void OnOK();
	afx_msg void OnChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDlgChangeFiles dialog

class CDlgChangeFiles : public CDialog
{
// Construction
public:
	BOOL m_bEditFileName;
	CDlgChangeFiles(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgChangeFiles)
	enum { IDD = IDD_CHANGE_FILES };
	CString	m_strDirectory;
	CString	m_strHeader;
	CString	m_strImplementation;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgChangeFiles)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgChangeFiles)
	afx_msg void OnDirectory();
	afx_msg void OnHeader();
	afx_msg void OnImplementation();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditHeader();
	afx_msg void OnChangeEditImplementation();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNEWCLASS_H__5451B9A9_72BF_4078_8BA5_50556A84EF8B__INCLUDED_)
