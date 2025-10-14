#if !defined(AFX_VALUECHANGE_H__9DAC2BFE_E6E9_4CA4_A05E_AC62610E7758__INCLUDED_)
#define AFX_VALUECHANGE_H__9DAC2BFE_E6E9_4CA4_A05E_AC62610E7758__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ValueChange.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CValueChange dialog

class CValueChange : public CDialog
{
// Construction
public:
	void EnableControl( BOOL bFlag );
	CValueChange(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CValueChange)
	enum { IDD = IDD_EDIT_DIALOG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CValueChange)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CValueChange)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheck1();
	afx_msg void OnCheck2();
	afx_msg void OnCheck3();
	afx_msg void OnCheck4();
	afx_msg void OnCheck5();
	afx_msg void OnCheck6();
	afx_msg void OnCheck7();
	afx_msg void OnCheck8();
	afx_msg void OnCheck9();
	afx_msg void OnCheck10();
	afx_msg void OnCheck11();
	afx_msg void OnCheck12();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VALUECHANGE_H__9DAC2BFE_E6E9_4CA4_A05E_AC62610E7758__INCLUDED_)
