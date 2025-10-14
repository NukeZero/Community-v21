#if !defined(AFX_DLGSAVEOPTION_H__E37C79AF_EF11_4B99_A960_B0D9A13DE9F8__INCLUDED_)
#define AFX_DLGSAVEOPTION_H__E37C79AF_EF11_4B99_A960_B0D9A13DE9F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSaveOption.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSaveOption dialog

class CDlgSaveOption : public CDialog
{
// Construction
public:
	CDlgSaveOption(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSaveOption)
	enum { IDD = IDD_SAVE_OPTION };
	BOOL	m_bDDS;
	BOOL	m_bDYO;
	BOOL	m_bLND;
	BOOL	m_bRGN;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSaveOption)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSaveOption)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSAVEOPTION_H__E37C79AF_EF11_4B99_A960_B0D9A13DE9F8__INCLUDED_)
