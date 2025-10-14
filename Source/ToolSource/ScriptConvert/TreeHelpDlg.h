#if !defined(AFX_TREEHELPDLG_H__356FA03F_2FF2_4B2D_90FC_40EBE2DCE264__INCLUDED_)
#define AFX_TREEHELPDLG_H__356FA03F_2FF2_4B2D_90FC_40EBE2DCE264__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeHelpDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTreeHelpDlg dialog
#include "CClass.h"

class CTreeHelpDlg : public CDialog
{
// Construction
public:
	VEC_LANG_STR_EX		m_vecSrc;
	VEC_LANG_STR_EX		m_vecDest;
	VEC_BRACES			m_vecBraces;
	
	CTreeHelpDlg(CWnd* pParent = NULL);   // standard constructor
	
	BOOL Save( LPCTSTR lpszFileName );
	BOOL Load(LPCTSTR lpszFileName, VEC_LANG_STR_EX& vec_help);
	BOOL Merge(VEC_LANG_STR_EX& vecSrc, const VEC_LANG_STR_EX& vecDest);
	
// Dialog Data
	//{{AFX_DATA(CTreeHelpDlg)
	enum { IDD = IDD_TREEHELP_DIALOG };
	CComboBox	m_LangKor;
	CComboBox	m_Lang;
	BOOL	m_bSkip;
	CString	m_SrcPath;
	CString	m_DestPath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeHelpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTreeHelpDlg)
	afx_msg void OnMerge();
	afx_msg void OnTreehelpLoad1();
	afx_msg void OnTreehelpLoad2();
	afx_msg void OnCheck1();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREEHELPDLG_H__356FA03F_2FF2_4B2D_90FC_40EBE2DCE264__INCLUDED_)
