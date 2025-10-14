#if !defined(AFX_HELPCONVERT_H__D0AA9A1C_9009_4714_B78C_7C1C940D6D7D__INCLUDED_)
#define AFX_HELPCONVERT_H__D0AA9A1C_9009_4714_B78C_7C1C940D6D7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HelpConvert.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHelpConvert dialog
#include "CClass.h"

class CHelpConvert : public CDialog
{
// Construction
public:
	CHelpConvert(CWnd* pParent = NULL);   // standard constructor

	VEC_HELP_STR      m_vecSrc;
	VEC_HELP_STR	   m_vecDest;
	
	BOOL Save( LPCTSTR lpszFileName );
	BOOL Load(LPCTSTR lpszFileName, VEC_HELP_STR& vec_help);
	BOOL Merge(VEC_HELP_STR& vecSrc, const VEC_HELP_STR& vecDest);
	
// Dialog Data
	//{{AFX_DATA(CHelpConvert)
	enum { IDD = IDD_HELP_DIALOG };
	CComboBox	m_Lang;
	CComboBox	m_LangKor;
	CString	m_SrcPath;
	CString	m_DestPath;
	BOOL	m_bSkip;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHelpConvert)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHelpConvert)
	afx_msg void OnHelpButton1();
	afx_msg void OnHelpButton2();
	afx_msg void OnMerge();
	afx_msg void OnCheck1();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HELPCONVERT_H__D0AA9A1C_9009_4714_B78C_7C1C940D6D7D__INCLUDED_)
