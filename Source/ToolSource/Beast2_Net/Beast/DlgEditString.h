#if !defined(AFX_DLGEDITSTRING_H__9CA02BF0_A37C_473B_A91F_8E18D799E5AA__INCLUDED_)
#define AFX_DLGEDITSTRING_H__9CA02BF0_A37C_473B_A91F_8E18D799E5AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEditString.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgEditString dialog

class CDlgEditString : public CDialog
{
// Construction
public:
	CDlgEditString(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgEditString)
	enum { IDD = IDD_EDIT_STRING };
	CString	m_string;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEditString)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgEditString)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITSTRING_H__9CA02BF0_A37C_473B_A91F_8E18D799E5AA__INCLUDED_)
