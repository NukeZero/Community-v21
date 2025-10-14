#if !defined(AFX_FILECOUNTERMNG_H__C723D602_4BDF_4712_B9A2_578F9F35DE48__INCLUDED_)
#define AFX_FILECOUNTERMNG_H__C723D602_4BDF_4712_B9A2_578F9F35DE48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileCounterMng.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileCounterMng dialog

class CFileCounterMng : public CDialog
{
// Construction
public:
	CFileCounterMng(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFileCounterMng)
	enum { IDD = IDD_FILE_COUNTER_DIALOG };
	CListBox	m_OF_XLS;
	CListBox	m_OF_TXT;
	CListBox	m_OF_Script;
	CListBox	m_OK_TXT;
	CListBox	m_OK_XLS;
	CListBox	m_OK_Script;
	CString	m_OK_FullPath;
	CString	m_11;
	CString	m_12;
	CString	m_13;
	CString	m_OF_FullPath;
	CString	m_21;
	CString	m_22;
	CString	m_23;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileCounterMng)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFileCounterMng)
	afx_msg void OnOldKorFileOpen();
	virtual BOOL OnInitDialog();
	afx_msg void OnOKXLS_DifferentButton();
	afx_msg void OnOKTXT_DifferentButton();
	afx_msg void OnDblclkList1();
	afx_msg void OnDblclkList2();
	afx_msg void OnDblclkList3();
	afx_msg void OnOldFFileOpen();
	afx_msg void OnButton6();
	afx_msg void OnButton7();
	afx_msg void OnDblclkList6();
	afx_msg void OnDblclkList5();
	afx_msg void OnDblclkList4();
	afx_msg void OnButton8();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILECOUNTERMNG_H__C723D602_4BDF_4712_B9A2_578F9F35DE48__INCLUDED_)
