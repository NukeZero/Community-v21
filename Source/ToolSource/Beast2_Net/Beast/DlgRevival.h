#if !defined(AFX_DLGREVIVAL_H__1CC7BA4D_4CB7_4C88_A102_6DA4C7AEF81B__INCLUDED_)
#define AFX_DLGREVIVAL_H__1CC7BA4D_4CB7_4C88_A102_6DA4C7AEF81B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRevival.h : header file
//

class CWorld;

/////////////////////////////////////////////////////////////////////////////
// CDlgRevival dialog

class CDlgRevival : public CDialog
{
// Construction
public:
	CDlgRevival(CWnd* pParent = NULL);   // standard constructor

	CWorld* m_pWorld;

// Dialog Data
	//{{AFX_DATA(CDlgRevival)
	enum { IDD = IDD_REVIVAL };
	CString	m_strWorld;
	CString	m_strKey;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRevival)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgRevival)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGREVIVAL_H__1CC7BA4D_4CB7_4C88_A102_6DA4C7AEF81B__INCLUDED_)
