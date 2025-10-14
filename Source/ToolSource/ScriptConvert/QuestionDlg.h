#if !defined(AFX_QUESTIONDLG_H__A86DD99F_FF1A_4B21_B2E6_99FD0D2A2D62__INCLUDED_)
#define AFX_QUESTIONDLG_H__A86DD99F_FF1A_4B21_B2E6_99FD0D2A2D62__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QuestionDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CQuestionDlg dialog

class CQuestionDlg : public CDialog
{
// Construction
public:
	CQuestionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CQuestionDlg)
	enum { IDD = IDD_DIALOG1 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuestionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CQuestionDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUESTIONDLG_H__A86DD99F_FF1A_4B21_B2E6_99FD0D2A2D62__INCLUDED_)
